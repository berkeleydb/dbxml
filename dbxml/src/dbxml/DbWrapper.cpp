//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "DbWrapper.hpp"
#include "Cursor.hpp"
#include "Key.hpp"
#include "NameID.hpp"
#include "DocID.hpp"
#include "Log.hpp"
#include "OperationContext.hpp"
#include "IndexEntry.hpp"
#include "db_rdbt.h"
#include "nodeStore/NsUtil.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "ContainerConfig.hpp"

#include <cassert>
#include <cerrno>
#include <sstream>

using namespace std;
using namespace DbXml;

// DbWrapper
#define DBXML_DEFAULT_PAGESIZE (8 * 1024)

static bool checkCDB(DB_ENV *env)
{
	if (env) {
		u_int32_t envFlags;
		env->get_flags(env, &envFlags);
		if (envFlags & DB_CDB_ALLDB)
			return true;
	}
	return false;
}

DbWrapper::DbWrapper(DB_ENV *environment, const std::string &name,
		     const std::string &prefixName,
		     const std::string &databaseName,
		     const ContainerConfig &config)
	: flags_(0),
	  name_(name),
	  prefixName_(prefixName),
	  databaseName_(databaseName),
	  pageSize_(config.getPageSize()),
	  db_(0),
	  environment_(environment),
	  readCursor_(0),
	  writeCursor_(0)
{
	int ret = db_create(&db_, environment, 0);
	if (ret != 0)
		throw XmlException(ret);
	if (environment_)
		setFlags(environment_);
}

DbWrapper::~DbWrapper()
{
	cleanup();
}

void DbWrapper::cleanup()
{
	if (readCursor_)
		delete readCursor_;
	if (writeCursor_)
		delete writeCursor_;
	int err = close(0);
	if(err != 0) {
		ostringstream oss;
		oss << "DB error during database cleanup: " << err;
		Log::log(environment_, Log::C_CONTAINER,
			 Log::L_ERROR, oss.str().c_str());
	}
}

void DbWrapper::setFlags(DB_ENV *env)
{
	DBXML_ASSERT(env);
	u_int32_t oflags = 0;
	env->get_open_flags(env, &oflags);
	if (checkCDB(environment_))
		setTransacted();
	if (oflags & DB_INIT_CDB)
		setCDBEnv();
	if (oflags & DB_INIT_LOCK)
		setLocking();
}

int DbWrapper::open(Transaction *txn, DBTYPE type, const ContainerConfig &config)
{
	ContainerConfig config2(config);
	int err = 0;
	if (pageSize_ > 0)
		db_->set_pagesize(db_, pageSize_);
	
	err = db_->set_flags(db_, config2.getDbSetFlagsForSet_Flags());
	if (err)
		throw XmlException(err);

	string dbname(getDatabaseName());

	// The file is opened in the directory specified by the environment.
	const char *fileName= 0;
	const char *databaseName = 0;
	if(name_.length() > 0) {
#ifdef DBXML_USE_SEPARATE_DBS
		// this is for DEBUG use only because it only
		// allows one container for a given directory/environment
		fileName = dbname.c_str();
#else
		fileName= name_.c_str();
		databaseName = dbname.c_str();
#endif
	} else {
		// creating an in-memory container -- DB_CREATE keeps
		// DB quiet about this
		config2.setAllowCreate(true);
	}
	if (txn)
		setTransacted();
	err = db_->open(db_, Transaction::toDB_TXN(txn), fileName,
			databaseName, type, config2.getDbOpenFlags(), config2.getMode());
	if (err == DB_LOCK_DEADLOCK)
		throw XmlException(err);
	if (err == 0) {
		if (pageSize_ == 0) {
			// Find out the page size of the underlying database.
			// We don't use stat because it will fail with
			// 'Invalid argument' if the open is within a txn.
			//
			pageSize_ = db_->pgsize;
		}
	}
	return err;
}

int DbWrapper::close(u_int32_t flags)
{
	// We are forgiving and allow close to be called even if the db
	// isn't actually open. This is for the error handling code in
	// Container. It calls close on all the DbWrapper objects to make
	// sure they are all in the closed state before they are destroyed.
	//
	if (db_) {
		// db.close kills the db handle even if it fails.
		(void) db_->close(db_, flags);
		db_ = 0;
	}
	return 0;
}

/*
 * pr_callback - C++ callback function for using pr_* functions from C++.
 */
extern "C"
{
	int pr_callback(void *handle, const void *str_arg) {
		std::ostream &out = *(std::ostream *)handle;
		const char *str = (const char *)str_arg;

		out << str;
		return (0);
	}
} /* extern "C" */

int DbWrapper::dump(std::ostream *out)
{
	int ret, err = 0;

	if ((ret = open(NULL, DB_UNKNOWN, DB_RDONLY)) == 0) {
		err = db_->dump(db_, NULL, pr_callback, out, 0, 1);
		ret = close(0);
	}
	if (ret == 0)
		ret = err;
	return (ret);
}

extern "C"
{
	/*
	 * Implementation of READ_FN for reading from a C++ istream.
	 * Reads at most 'len' characters into 'buf' up to the first 'until' characater
	 * (if non-zero). The terminator (if found) is discarded, and the string is nul
	 * terminated if 'len' > 1.
	 * Returns: zero on success, DB_NOTFOUND if 'until' != 0 and not found, or
	 * EOF on EOF
	 */
	int
	read_callback(char *buf, size_t len, char until, void *handle) {
		std::istream &in = *(std::istream *)handle;
		char *p = buf;
		char c;
		size_t bytes_read;

		for (bytes_read = 0; bytes_read < len; bytes_read++) {
			if (!in.get(c) || (until != 0 && c == until))
				break;
			else
				*p++ = c;
		}

		if (bytes_read < len)
			*p = '\0';

		if (!in && bytes_read == 0)
			return (EOF);
		else if (until != 0 && c != until)
			return (DB_NOTFOUND);
		else
			return (0);
	}
} /* extern "C" */

int DbWrapper::load(std::istream *in, unsigned long *lineno)
{
	int version, ret, t_ret;
	DBTYPE dbtype;
	char *subdb = 0;
	u_int32_t read_flags, tflags;
	DBT key, data;
	db_recno_t recno, datarecno;
	DB_ENV *dbenv = environment_;
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	if ((ret = __db_rheader(dbenv, db_, &dbtype,
				&subdb, &version, &read_flags, read_callback, in, lineno)) != 0)
		goto err;

	/* We always print with keys */
	if (!(read_flags & DB_READ_HASKEYS)) {
		db_->errx(db_, "Invalid DbXml dump: keys missing");
		ret = EINVAL;
		goto err;
	}

	if ((ret = open(NULL, dbtype, DB_CREATE|DB_EXCL)) != 0)
		goto err;

	/* Initialize the key/data pair. */
	if (dbtype == DB_RECNO || dbtype == DB_QUEUE) {
		key.size = sizeof(recno);
		key.data = &datarecno;
	} else {
		key.ulen = 1024;
		key.data = (void *)malloc(key.ulen);
	}
	data.ulen = 1024;
	data.data = (void *)malloc(data.ulen);
	if (key.data == NULL || data.data == NULL) {
		db_->err(db_, ENOMEM, NULL);
		goto err;
	}
	
	// work around gcc optimizer issue that seems to modify
	// read_flags (4.1.1 on 64-bit linux)
	tflags = read_flags;
	/* Get each key/data pair and add them to the database. */
	for (recno = 1;; ++recno) {
		if ((ret = __db_rdbt(dbenv, &key, &data,
				     tflags, read_callback, in, lineno)) != 0) {
			if (ret == EOF)
				ret = 0;
			break;
		}

		switch (ret = db_->put(db_, NULL, &key, &data, 0)) {
		case 0:
			break;
		case DB_KEYEXIST:
			db_->errx(db_, "line %d: key already exists, not loaded:", *lineno);
			dbenv->prdbt(&key, tflags & DB_READ_PRINTABLE,
				     0, &std::cerr, pr_callback, 0);
			break;
		default:
			db_->err(db_, ret, NULL);
			goto err;
		}
	}

 err:	/* Close the database. */
	if ((t_ret = close(0)) != 0 && ret == 0)
		ret = t_ret;

	/* Free allocated memory. */
	if (subdb != NULL)
		free(subdb);
	if (key.data != NULL && dbtype != DB_RECNO && dbtype != DB_QUEUE)
		free(key.data);
	if (data.data != NULL)
		free(data.data);

	return (ret);
}

extern "C" int __db_verify_internal __P((DB *, const char *, const char *, void *, int (*)(void *, const void *), u_int32_t));

extern "C" int _verify_callback_dbxml(void *handle, const void *str_arg);

int DbWrapper::verify(std::ostream *out, u_int32_t flags)
{
	// TBD.  Note -- if done, need to clear db_ because the handle
	// is closed unconditionally in verify.
	int ret = 0;
	if (db_) {
		ret = __db_verify_internal(
			db_,
			(name_.length() == 0 ? 0 : name_.c_str()), //db
			getDatabaseName().c_str(),
			out, _verify_callback_dbxml, flags);
		db_ = 0;
	}
	return ret;
}

const char *DbWrapper::operationToString(DbWrapper::Operation operation)
{
	switch (operation) {
	case NONE:
		return "none";
	case ALL:
		return "all";
	case EQUALITY:
		return "=";
	case NEG_NOT_EQUALITY:
		return "inv(!=)";
	case LTX:
		return "<";
	case LTE:
		return "<=";
	case GTX:
		return ">";
	case GTE:
		return ">=";
	case RANGE:
		return "range";
	case PREFIX:
		return "prefix";
	case SUBSTRING:
		return "substring";
	}
	return "unknown";
}

const char *DbWrapper::operationToWord(DbWrapper::Operation operation)
{
	switch (operation) {
	case NONE:
		return "none";
	case ALL:
		return "all";
	case EQUALITY:
		return "eq";
	case NEG_NOT_EQUALITY:
		return "inv(ne)";
	case LTX:
		return "lt";
	case LTE:
		return "lte";
	case GTX:
		return "gt";
	case GTE:
		return "gte";
	case RANGE:
		return "range";
	case PREFIX:
		return "prefix";
	case SUBSTRING:
		return "substring";
	}
	return "unknown";
}

u_int32_t DbWrapper::getPageSize() const
{
	return pageSize_;
}

unsigned long DbWrapper::getNumberOfPages() const
{
	// assume that a sequential scan is very expensive.
	// TBD: heuristic to figure out size of database/real expense
	return 0xFFFF; 
}

// copy all records from "from" database to this one.
// assumes that any custom comparison functions have been set.
int DbWrapper::copy(DbWrapper &from, u_int32_t putFlags)
{
	Cursor dest(*this, (Transaction*)0, DbXml::CURSOR_WRITE);
	Cursor src(from, (Transaction*)0, DbXml::CURSOR_READ);
	DbXmlDbt key;
	DbXmlDbt data;
	int err = 0, ret = 0;
	while ((err == 0) &&
	       (ret = src.get(key, data, DB_NEXT)) == 0) {
		err = dest.put(key, data, putFlags);
	}

	if (err != 0)
		return err;
	if (ret != 0 && ret != DB_NOTFOUND)
		return ret;
	return 0;
}

// static
int DbWrapper::copySecondary(DB_ENV *env,
			     const std::string &oldcname,
			     const std::string &newcname,
			     const std::string &prefix,
			     const std::string &dbname,
			     bool duplicates)
{
	int err;
	DbWrapper olddb(env, oldcname, prefix, dbname, DEFAULT_CONFIG);
	ContainerConfig config;
	config.setReadOnly(true);
	err = olddb.open(0, DB_BTREE, config);
	if (err) {
		// ignore ENOENT -- it's OK sometimes
		if (err == ENOENT)
			err = 0;
		return err;
	}
	config.setReadOnly(false);
	config.setPageSize(olddb.getPageSize());
	DbWrapper newdb(env, newcname, prefix, dbname,
			config);
	if (duplicates) {
		err = newdb.getDb()->set_flags(newdb.getDb(), DB_DUP|DB_DUPSORT);
		if (err)
			throw XmlException(err);
	}
	err = newdb.open(0, DB_BTREE, DB_CREATE|DB_EXCL);
	if (err == 0)
		err = newdb.copy(olddb, DB_KEYFIRST);
	return err;
}

// PrimaryDatabase

PrimaryDatabase::PrimaryDatabase(DB_ENV *environment, const std::string &name,
				 const std::string& databaseName,
				 const ContainerConfig &config)
	: DbWrapper(environment, name, "primary_",
		    databaseName, config)
{}

PrimaryDatabase::~PrimaryDatabase()
{}

int PrimaryDatabase::open(Transaction *txn, const ContainerConfig &config)
{
	return DbWrapper::open(txn, DB_RECNO, config);
}

int PrimaryDatabase::getPrimary(OperationContext &context, const NameID &id,
				DbtOut *data, u_int32_t flags) const
{
	id.setDbtFromThis(context.key());
	return get(context.txn(), &context.key(), data, flags);
}

int PrimaryDatabase::putPrimary(OperationContext &context, const NameID &id,
				const DbXmlDbt &data, u_int32_t flags)
{
	id.setDbtFromThis(context.key());
	return put(context.txn(), &context.key(), &(const_cast<DbXmlDbt&>(data)), flags);
}

int PrimaryDatabase::appendPrimary(OperationContext &context, NameID &id,
				   const DbXmlDbt &data, u_int32_t flags)
{
	int err = put(context.txn(), &context.key(), &(const_cast<DbXmlDbt&>(data)),
		      flags | DB_APPEND);
	if (err == 0)
		id.setThisFromDbtAsId(context.key());
	return err;
}

int PrimaryDatabase::deletePrimary(OperationContext &context,
				   const NameID &id, u_int32_t flags)
{
	id.setDbtFromThis(context.key());
	return del(context.txn(), &context.key(), flags);
}

// SecondaryDatabase

SecondaryDatabase::SecondaryDatabase(DB_ENV *environment,
				     const std::string &name,
				     const std::string& databaseName,
				     const ContainerConfig &config)
	: DbWrapper(environment, name, "secondary_",
		    databaseName, config)
{
}

int SecondaryDatabase::open(Transaction *txn, bool duplicates,
			    const ContainerConfig &config)
{
	if (duplicates) {
		int err = db_->set_flags(db_, DB_DUP|DB_DUPSORT);
		if (err)
			throw XmlException(err);
	}
	return DbWrapper::open(txn, DB_BTREE, config);
}

// IndexDatabase

extern "C" {
int
index_duplicate_compare(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	const xmlbyte_t *p1 = (const xmlbyte_t *)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t *)dbt2->data;
	++p1; ++p2; // Get past format byte

	// Compare the DocID first
	int res = DocID::compareMarshaled(p1, p2);
	if(res != 0) return res;

	// Compare the node ID by using a lexicographical
	// comparison, stopping at the terminating null.
	//
	// (If no node ID is stored, a null is stored instead,
	//  so that we can simplify this sort algorithm.)

	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}
};

IndexDatabase::IndexDatabase(DB_ENV *environment, const std::string &name,
			     const std::string& databaseName,
			     const Syntax *syntax,
			     const ContainerConfig &config)
	: DbWrapper(environment, name, "secondary_",
		    databaseName, config),
	  syntax_(syntax)
{
	DBXML_ASSERT(syntax != 0);

	DbWrapper::bt_compare_fn fn = syntax->get_bt_compare();
	if(fn != 0) {
		db_->set_bt_compare(db_, fn);
	}
}

int IndexDatabase::open(Transaction *txn, bool duplicates, bool nodesIndexed, const ContainerConfig &config)
{
	if(duplicates) {
		int err = db_->set_flags(db_, DB_DUPSORT);
		if (err == 0) {
			// Don't need a custom comparator unless nodes are indexed
			db_->set_dup_compare(db_, index_duplicate_compare);
		}
		if (err)
			throw XmlException(err);
	}
	return DbWrapper::open(txn, DB_BTREE, config);
}

// We assume that key has the correct endianness.
int IndexDatabase::getIndexEntry(OperationContext &context, const DbXmlDbt &key, IndexEntry &ie) const
{
	u_int32_t flags = (context.txn()) ? DB_READ_COMMITTED : 0;
	int err = get(context.txn(), &(const_cast<DbXmlDbt&>(key)), &context.data(), flags);
	if(err == 0) {
		ie.setThisFromDbt(context.data());
	}
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::putIndexEntry(OperationContext &context, const DbXmlDbt &key, const DbXmlDbt &data)
{
	int err = put(context.txn(), &(const_cast<DbXmlDbt&>(key)),
		      &(const_cast<DbXmlDbt&>(data)), DB_NODUPDATA);
	if(err == DB_KEYEXIST) err= 0; // Suppress duplicate data error, it's OK really.
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::updateIndexEntry(OperationContext &context, const DbXmlDbt &key, const DbXmlDbt &data)
{
	Cursor cursor(*this, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if(err == 0) {
		// We want to do a partial retrieval of 0 bytes,
		// to get the cursor in the right position
		DbXmlDbt tmp;
		tmp.data = data.data;
		tmp.size = data.size;
		tmp.doff = 0; 
		tmp.dlen = 0;
		tmp.set_flags(DB_DBT_PARTIAL);

		// Find the existing entry
		err = cursor.get(const_cast<DbXmlDbt&>(key), tmp, DB_GET_BOTH);

		if(err == 0) {
			err = cursor.del( /*no flags*/0);
		} else if(err == DB_NOTFOUND) {
			// If we can't find the index entry already, just add it
			err = 0;
		}

		if(err == 0) {
			err = cursor.put(const_cast<DbXmlDbt&>(key), const_cast<DbXmlDbt&>(data), DB_NODUPDATA);
			if(err == DB_KEYEXIST) err = 0; // Suppress duplicate data error, it's OK really.
		}
	}
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::delIndexEntry(OperationContext &context, const DbXmlDbt &key, const DbXmlDbt &data)
{
	Cursor cursor(*this, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if (err == 0) {
		err = cursor.get(const_cast<DbXmlDbt&>(key), const_cast<DbXmlDbt&>(data), DB_GET_BOTH);
		if (err == 0) {
			err = cursor.del( /*no flags*/0);
		}
	}
	return err;
}

// We assume that key has the correct endianness.
bool IndexDatabase::exists(Transaction *txn, const DbXmlDbt &key) const
{
	// Check if the key already exists.
	// DB_DBT_PARTIAL and len=0 prevents retrieval of the data.
	DbtIn dbt;
	dbt.set_flags(DB_DBT_PARTIAL);
	int err = get(txn, &(const_cast<DbXmlDbt&>(key)), &dbt, /*no flags*/0);
	return (err != DB_NOTFOUND);
}

void IndexDatabase::getMinKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyMin(0);
	if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
		keyMin.set(key, 0, 0);
	}
	else {
		keyMin.set(key);
	}
	keyMin.setDbtFromThis(dbt);
}

void IndexDatabase::getMaxKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyMin(0);
	if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
		keyMin.set(key, 0, 0);
	}
	else {
		keyMin.set(key);
	}
	getNextKeyDbt(keyMin, dbt);
}

void IndexDatabase::getNextKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyNext(0);
	keyNext.set(key);
	keyNext.setDbtFromThis(dbt);

	// We want to increment the dbt value to the next key,
	// but if incrementing the last byte results in a carry
	// (value overflows and goes back to zero), then we have
	// to increment the last-but-one byte - and so on.
	unsigned char *maxPtr = (unsigned char *)dbt.data;
	u_int32_t maxSize = dbt.size;
	unsigned char *pos = maxPtr + maxSize;
	for(--pos; pos >= maxPtr; --pos) {
		++(*pos);
		if(*pos != 0) break;
	}
}

double IndexDatabase::percentage(OperationContext &context, Operation operation, Operation gto, Operation lto, const Key &key1, const Key &key2) const
{
	DbtOut &dbt1 = context.key();
	DbtOut &dbt2 = context.data();

	DB_KEY_RANGE krMin;
	getMinKeyDbt(key1, dbt1);
	key_range(context.txn(), &dbt1, &krMin, 0);

	DB_KEY_RANGE krMax;
	getMaxKeyDbt(key1, dbt1);
	key_range(context.txn(), &dbt1, &krMax, 0);

	// range is the % of the database keys that the keys for this index occupy.
	double range = krMax.less - krMin.less;
	double extent = 0.0;

	if (range > 0.0) {
		// extent is the % of the database keys that the keys for this index match this operation.
		DB_KEY_RANGE kr1;
		DB_KEY_RANGE kr2;

		switch(operation) {
		case DbWrapper::PREFIX: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			getNextKeyDbt(key1, dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - kr1.less;
			break;
		}
		case DbWrapper::LTX:
		case DbWrapper::LTE: {
			key1.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - krMin.less + (operation == DbWrapper::LTE ? kr2.equal : 0);
			break;
		}
		case DbWrapper::GTX:
		case DbWrapper::GTE: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			extent = krMax.less + krMax.equal - kr1.less + (operation == DbWrapper::GTX ? kr1.equal : 0);
			break;
		}
		case DbWrapper::RANGE: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			key2.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - kr1.less + (lto == DbWrapper::LTE ? kr2.equal : 0) + (gto == DbWrapper::GTX ? kr1.equal : 0);
			break;
		}
		case DbWrapper::EQUALITY: {
			key1.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.equal;
			break;
		}
		case DbWrapper::ALL: {
			extent = range;
			break;
		}
		default: {
			break;
		}
		}
	}

	// Return a small percentage in the case of a zero range or extent -
	// it's unlikely that zero is really the right answer
	if(range == 0 || extent == 0) return 0.001;

	// extent/range is the % of keys within this index that match this operation.
	return extent / range;
}

IndexCursor *IndexDatabase::createCursor(Transaction *txn,
					 DbWrapper::Operation operation,
					 const Key *key, bool reverse)
{
	switch(operation) {
	case DbWrapper::EQUALITY:
		// For Equals, reverse makes no sense
		return new EqualsIndexCursor(*this, txn, key);
		break;
	case DbWrapper::PREFIX:
		if (!reverse)
			return new PrefixIndexCursor(*this, txn, key);
		else
			return new ReversePrefixIndexCursor(*this, txn, key);
		break;
	case DbWrapper::LTX:
	case DbWrapper::LTE:
	case DbWrapper::GTX:
	case DbWrapper::GTE:
	case DbWrapper::ALL:
		if (!reverse)
			return new InequalityIndexCursor(
				*this, txn, operation,
				key, syntax_);
		else
			return new ReverseInequalityIndexCursor(
				*this, txn, operation,
				key, syntax_);
		break;
	case DbWrapper::NONE:
	case DbWrapper::RANGE:
	case DbWrapper::NEG_NOT_EQUALITY:
		DBXML_ASSERT(0);
		break;
	default: break;
	}
	return NULL;
}

IndexCursor *IndexDatabase::createCursor(
	Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto,
	const Key *ltk, bool reverse)
{
	if (!reverse)
		return new InequalityIndexCursor(*this, txn, gto, gtk,
						 lto, ltk, syntax_);
	else
		return new ReverseInequalityIndexCursor(
			*this, txn, gto, gtk, lto, ltk, syntax_);
}

NodeIterator *IndexDatabase::createNodeIterator(Transaction *txn, Operation operation,
	const Key *key, ContainerBase *container, bool documentIndex, const LocationInfo *l)
{
	if(operation == DbWrapper::EQUALITY) {
		// Equality gets it's own NodeIterator object, since
		// it doesn't need sorting
		if(documentIndex) {
			return new EqualsDocumentIndexIterator(container, *this, txn, key, l);
		} else {
			return new EqualsIndexIterator(container, *this, txn, key, l);
		}
	} else {
		if(documentIndex) {
			return new SortingDocumentIndexIterator(container,
				createCursor(txn, operation, key, /*reverse*/false), l);
		} else {
			return new SortingIndexIterator(container,
				createCursor(txn, operation, key, /*reverse*/false), l);
		}
	}
}

NodeIterator *IndexDatabase::createNodeIterator(Transaction *txn, Operation gto,
	const Key *gtk, Operation lto, const Key *ltk,
	ContainerBase *container, bool documentIndex, const LocationInfo *l)
{
	if(documentIndex) {
		return new SortingDocumentIndexIterator(container,
			createCursor(txn, gto, gtk, lto, ltk, /*reverse*/false), l);
	} else {
		return new SortingIndexIterator(container,
			createCursor(txn, gto, gtk, lto, ltk, /*reverse*/false), l);
	}
}
