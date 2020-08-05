//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "DbXmlInternal.hpp"
#include <sstream>

#include <dbxml/XmlException.hpp>
#include "DictionaryDatabase.hpp"
#include "Manager.hpp"
#include "Name.hpp"
#include "ScopedDbt.hpp"
#include "Container.hpp"
#include "OperationContext.hpp"
#include "MutexLock.hpp"
#include "db_utils.h"

using namespace DbXml;
using namespace std;

//
// The name dictionary maps between name ids and string names.
// It comprises 2 databases:
// 1.  Primary (recno), value is of the form:
//    name<null>{uri}<null>
// where the uri is optional
//
// 2.  Secondary, which is a no-duplicate btree, with keys:
//    name{:uri}
// where the value is a compressed form of the integer key to
// the Primary
//
//

// preload or otherwise reserve the first 50 ids
// IMPORTANT: only add to the end of this list, or the
// id to name mappings will be incompatible.  The order
// matters. Do not exceed DICTIONARY_RESERVE_SIZE, *ever*

#define DICTIONARY_RESERVE_SIZE 50
#define DICTIONARY_NUM_QNAMES 2
static const char *preloadNames[] = {
	// BDB XML names
	"name:http://www.sleepycat.com/2002/dbxml",
	"root:http://www.sleepycat.com/2002/dbxml",
	"dbxml",
	"http://www.sleepycat.com/2002/dbxml",

	// XML community names/uris
	"xml",
	"xmlns",
	"xsi",
	"xsd",
	"xs",
	"xlink",
	"xhtml",
	"http://www.w3.org/XML/1998/namespace/",
	"http://www.w3.org/XML/2000/xmlns/",
	"http://www.w3.org/2000/xmlns/",
	"http://www.w3.org/1999/xlink",
	"http://www.w3.org/2001/XMLSchema-instance",
	"http://www.w3.org/2001/XMLSchema",
	"http://www.w3.org/1999/xhtml",

	// some common names...
	"id",
	"name", //20 (1-based)
};

static const u_int32_t lastPreloadId = 20;
static const u_int32_t dictCacheBytes = 30 * 1024; // 30k
static const char *dictionary_name = "dictionary";

DictionaryDatabase::DictionaryDatabase(DB_ENV *env, Transaction *txn,
				       const std::string &name,
				       const ContainerConfig &config,
				       bool useMutex)
	: stringCache_(true),
	  environment_(env),
	  name_(name),
	  primary_(new PrimaryDatabase(env, name,
				       dictionary_name,
				       DEFAULT_CONFIG)),
	  secondary_(new SecondaryDatabase(env, name,
					   dictionary_name,
					   DEFAULT_CONFIG)),
	  nidName_(0),
	  nidRoot_(0),
	  usePreloads_(false),
	  isTransacted_(txn ? true : false),
	  mutex_(useMutex ? MutexLock::createMutex() : 0)
{
	cache_.setDictionaryDatabase(this);
	if (!isTransacted_ && env) {
		u_int32_t envFlags;
		env->get_flags(env, &envFlags);
		if (envFlags & DB_CDB_ALLDB)
			isTransacted_ = true;
	}
	int err = 0;
	try {
		// set cache sizes low if no DB_ENV -- this is the in-memory
		// dictionary for transient docs
		if (!env) {
			primary_->getDb()->set_cachesize(primary_->getDb(), 0, dictCacheBytes, 1);
			secondary_->getDb()->set_cachesize(secondary_->getDb(), 0, dictCacheBytes, 1);
		}

		// Open the Db objects
		err = primary_->open(txn, config);
		if (err == 0)
			err = secondary_->open(txn, /*duplicates*/true, config);

		if (err == 0) {
			// Lookup/Define the dbxml namespace names (but don't define
			// if this is a read-only container)
			bool rdonly = config.getReadOnly();
			preloadDictionary(txn, rdonly);
		}

	} catch (XmlException &) {
		secondary_->cleanup();
		primary_->cleanup();
		if (txn)
			txn->abort();
		throw;
	}
	if (err != 0) {
		secondary_->cleanup();
		primary_->cleanup();
		if (txn)
			txn->abort();
		string msg = name;
		if (err == EEXIST) {
			msg += ": container exists";
			throw XmlException(
				XmlException::CONTAINER_EXISTS, msg);
		} else if (err == ENOENT) {
			msg += ": container file not found, or not a container";
			throw XmlException(XmlException::CONTAINER_NOT_FOUND,
					   msg);
		}
		throw XmlException(err);
	}
}

DictionaryDatabase::~DictionaryDatabase()
{
	if (mutex_)
		MutexLock::destroyMutex(mutex_);
}

//
// NOTE: can only use the preload array as a short cut for lookup if
// the container was preloaded with that array.  This is not
// the case, for example, for upgraded containers.
// Check whether this is possible by doing a check of the last
// preloaded string -- if it matches, then preloads are safe
//
void DictionaryDatabase::preloadDictionary(Transaction *txn, bool rdonly)
{
	OperationContext oc(txn);
	int err = 0;
	if (!rdonly) {
		NameID dummy(0);
		std::string dummys;
		err = lookupIDFromName(oc, Name::dbxml_colon_name, dummy, false);
		if (err || (dummy ==0)) {
			// need to pre-load
			uint32_t i;
			for (i = 0;
			     i < (sizeof(preloadNames)/sizeof(const char *));
			     i++) {
				if (i < DICTIONARY_NUM_QNAMES)
					err = defineQName(oc, preloadNames[i], dummy);
				else
					err = defineStringName(
						oc,
						preloadNames[i],
						::strlen(preloadNames[i]),
						dummy);
				DBXML_ASSERT(err == 0);
			}
			while (i < DICTIONARY_RESERVE_SIZE) {
				// just use up space in recno, up to reserved
				// size, to reserve those IDs
				// no need to actually define a name,
				// just take up ID space in the primary
				const char *unused = "x";
				DbtIn primaryData((void*)unused, 2);
				err = primary_->appendPrimary(oc, dummy,
							      primaryData,
							      /*no flags*/0);
				i++;
			}
		}
	}
	
	err = lookupIDFromName(oc, Name::dbxml_colon_name, nidName_, !rdonly);
	if (err == 0)
		err = lookupIDFromName(oc, Name::dbxml_colon_root, nidRoot_, !rdonly);

	// check to see if it's OK to use the preload array as a shortcut
	// for lookup
	NameID tid(lastPreloadId);
	const char *tname;
	err = lookupStringNameFromID(oc, tid, &tname);
	if (err == 0 && (::strcmp(tname, preloadNames[lastPreloadId-1]) == 0)) {
		usePreloads_ = true;
	}
}

// mutex is locked if present and necessary
int DictionaryDatabase::defineStringName(OperationContext &context,
					 const char *name, size_t namelen,
					 NameID &id)
{
	// Primary { id -> name\0 }
	// Secondary { name -> id }
	int err = 0;
	id.reset();
	DbtIn primaryData((void*)name, namelen + 1); // add null
	DbtIn secondaryKey((void*)name, namelen); // don't add null
	err = primary_->appendPrimary(context, id,
				      primaryData, /*no flags*/0);
	if (err == 0) {
		/* Cache after insert.  This prevents the cache from returning bad answers
		 * on a transaction abort, and speeds up access to newly inserted element
		 * and attribute names that are needed for indexing after an insert.  
		 */
		cache_.insert(id.raw(), primaryData);
		id.setDbtFromThis(context.key());
		Transaction *txn = getTxn(context);
		err = secondary_->put(txn, &secondaryKey,
				      &context.key(),
				      /*no flags*/0);
		if (err == 0) {
			/* Add to the transaction's string cache, that way if an abort occurs
			 * the values can be removed from the cache. 
			 */
			if (txn) { 
				DictionaryStringCache *dsc = txn->getStringCache(this, true);
				dsc->insert(&secondaryKey, id.raw());
			}
			if (Log::isLogEnabled(Log::C_DICTIONARY, Log::L_INFO)) {
				ostringstream oss;
				oss << "Define new name " << id << " -> " << name;
				Log::log(environment_, Log::C_DICTIONARY, Log::L_INFO,
					name_.c_str(),
					oss.str().c_str());
			}
		}
	}
	return err;
}

// mutex is locked if present and necessary
int DictionaryDatabase::defineName(OperationContext &context,
				   const Name &name, NameID &id)
{
	// Primary { id -> name\0uri\0 }
	// Secondary { name:uri -> id }
	id.reset();
	name.setDbtFromThis_PrimaryValue(context.data());
	int err = primary_->appendPrimary(context, id, context.data(), /*no flags*/0);
	if (err == 0) {
		/*Cache after insert.  This prevents the cache from returning bad answers
		 * on a transaction abort, and speeds up access to newly inserted element
		 * and attribute names that are needed for indexing after an insert.  
		 */
		cache_.insert(id.raw(), context.data());
		id.setDbtFromThis(context.key());
		name.setDbtFromThis_SecondaryKey(context.data());
		Transaction *txn = getTxn(context);
		err = secondary_->put(txn, &context.data(), &context.key(), /*no flags*/0);
		if (err == 0) {
			/* Add to the transaction's string cache, that way if an abort occurs
			 * the values can be removed from the cache. 
			 */
			if (txn) { 
				DictionaryStringCache *dsc = txn->getStringCache(this, true);
				dsc->insert(&context.data(), id.raw());
			}
			if(Log::isLogEnabled(Log::C_DICTIONARY, Log::L_INFO)) {
				ostringstream oss;
				oss << "Define new name " << id << " -> " << name;
				Log::log(environment_, Log::C_DICTIONARY, Log::L_INFO,
					 name_.c_str(),
					oss.str().c_str());
			}
		}
	}	
	return err;
}

// mutex is locked if present and necessary
int DictionaryDatabase::defineQName(OperationContext &context,
				    const char *uriname, NameID &id)
{
	Name name(uriname);
	return defineName(context, name, id);
}

// puts result in dbt, which may be context.data(), or may be user-provided
int DictionaryDatabase::lookupFromID(
	OperationContext &context, DbtOut &dbt,
	const NameID &id) const
{
	int err = 0;
	if (cache_.lookup(context, id, dbt, false))
		return 0;
	u_int32_t flags = (getTxn(context)) ? DB_READ_COMMITTED : 0;
	// primary key is the integer value of the id (database is
	// recno)
	id.setDbtFromThisAsId(context.key());
	MutexLock ml(mutex_);
	err = primary_->get(getTxn(context), &context.key(),
			    &dbt, flags);
	if (err == 0)
		cache_.insert(id.raw(), dbt);
	return err;
}

void DictionaryDatabase::display(OperationContext &context, ostream &out) const
{
	{
	Cursor myCursor(const_cast<PrimaryDatabase&>(*primary_.get()), getTxn(context), CURSOR_READ, 0);
	if(myCursor.error() != 0) throw XmlException(myCursor.error());

	int err = 0;
	NameID id;
	while((err = myCursor.get(context.key(), context.data(), DB_NEXT)) == 0) {
		id.setThisFromDbtAsId(context.key());
		Buffer val(context.data().data, context.data().size, true);
		out << id << " -> " << val.asString(true) << endl;
	}
	}
	{
	Cursor myCursor(const_cast<SecondaryDatabase&>(*secondary_.get()), getTxn(context), CURSOR_READ, 0);
	if(myCursor.error() != 0) throw XmlException(myCursor.error());

	int err = 0;
	NameID id;
	while((err = myCursor.get(context.key(), context.data(), DB_NEXT)) == 0) {
		Buffer val(context.key().data, context.key().size, true);
		id.setThisFromDbt(context.data());
		out << val.asString(true) << " -> " << id << endl;
	}
	}
}

// lookup through cache

const char *DictionaryDatabase::lookupName(OperationContext &context,
					   const NameID &id) const
{
	nameId_t raw = id.raw();
	if ((raw < DICTIONARY_RESERVE_SIZE) && usePreloads_) {
		return preloadNames[raw-1]; // preload space is 1-based, not 0
	} else
		return cache_.lookup(context, raw, true);
}

int DictionaryDatabase::lookupNameFromID(OperationContext &context,
					 const NameID &id, Name &name) const
{
	int err = 0;
	nameId_t raw = id.raw() - 1; // id space is 1-based, not 0
	if (id == nidName_) {
		name = Name::dbxml_colon_name;
		return 0;
	} else if (id == nidRoot_) {
		name = Name::dbxml_colon_root;
		return 0;
	}
	
	if ((raw < DICTIONARY_RESERVE_SIZE) && usePreloads_) {
		name = preloadNames[raw];
	} else {
		err = lookupFromID(context, context.data(), id);
		if (err == 0) {
			name.setThisFromDbt(context.data());
		} else {
			name.reset();
		}
	}
	return err;
}

int DictionaryDatabase::lookupStringNameFromID(
	OperationContext &context,
	const NameID &id, const char **name) const
{
	int err = 0;
	nameId_t raw = id.raw() - 1; // id space is 1-based, not 0
	if ((raw < DICTIONARY_RESERVE_SIZE) && usePreloads_) {
		*name = preloadNames[raw];
	} else {
		err = lookupFromID(context, context.data(), id);
		if (err == 0)
			*name = (const char *)context.data().data;
	}

	return err;
}

int DictionaryDatabase::lookupStringNameFromID(
	OperationContext &context,
	const NameID &id, DbtOut &dbt) const
{
	int err = 0;
	nameId_t raw = id.raw() - 1; // id space is 1-based, not 0
	if ((raw < DICTIONARY_RESERVE_SIZE) && usePreloads_) {
		const char *name = preloadNames[raw];
		dbt.set((const void *)name, ::strlen(name) + 1);
	} else
		err = lookupFromID(context, dbt, id);

	return err;
}

bool DictionaryDatabase::stringCacheLookup(OperationContext &context,
					   DbXmlDbt &dbt, NameID &id) const
{
	nameId_t *nid = 0;
	Transaction *txn = getTxn(context);
	if (txn) {
		DictionaryStringCache *dsc =
			txn->getStringCache(const_cast<DictionaryDatabase*>(this), false);
		if (dsc) {
			nid = dsc->lookup(&dbt);
			if (nid) {
				DbXmlDbt dbt((void *)nid, sizeof(nameId_t));
				id.setThisFromDbtAsId(dbt);
				return true;
			}
		}
	}
	nid = stringCache_.lookup(&dbt);
	if (nid) {
		DbXmlDbt tdbt((void *)nid, sizeof(nameId_t));
		id.setThisFromDbtAsId(tdbt);
		return true;
	}
	return false;
}

int DictionaryDatabase::lookupIDFromName(OperationContext &context,
					 const Name &name,
					 NameID &id, bool define)
{
	int err = 0;
	if (name == Name::dbxml_colon_name) {
		id = nidName_;
	} else if (name == Name::dbxml_colon_root) {
		id = nidRoot_;
	}
	if (id == 0) {
		MutexLock ml(mutex_);
		u_int32_t flags = (getTxn(context) && !define) ? DB_READ_COMMITTED : 0;
		name.setDbtFromThis_SecondaryKey(context.key());
		if (stringCacheLookup(context, context.key(), id))
			return 0;
		err = secondary_->get(getTxn(context), &context.key(), &context.data(), flags);
		if (err == 0) {
			id.setThisFromDbt(context.data());
			stringCache_.insert(&context.key(), id.raw());
		} else if (err == DB_NOTFOUND && define) {
			err = defineName(context, name, id); // define from name:uri
		} else {
			id.reset();
		}
	}
	return err;
}

// qname is of format name:uri
int DictionaryDatabase::lookupIDFromQName(OperationContext &context,
					  DbXmlDbt &qname,
					  NameID &id, bool define)
{
	MutexLock ml(mutex_);
	int err = lookupIDFromStringNameInternal(context, qname,
						 id, define);
	if (err == DB_NOTFOUND && define)  // define from name:uri
		err = defineQName(context, (const char *)qname.data, id);
	return err;
}

int DictionaryDatabase::lookupIDFromStringName(OperationContext &context,
					       const char *name,
					       size_t namelen,
					       NameID &id, bool define)
{
	MutexLock ml(mutex_);
	DbtIn dbt((void *)name, namelen);
	int err = lookupIDFromStringNameInternal(context, dbt, id, define);
	if (err == DB_NOTFOUND && define)
		err = defineStringName(context, name, namelen, id); // define from name
	return err;
}

int DictionaryDatabase::lookupIDFromStringNameInternal(
	OperationContext &context,
	DbXmlDbt &dbt,
	NameID &id,
	bool define) const
{
	if (!dbt.size) {
		id.reset();
		return 0;
	}
	u_int32_t flags = (getTxn(context) && !define) ? DB_READ_COMMITTED : 0;
	if (stringCacheLookup(context, dbt, id))
			return 0;
	int err = secondary_->get(getTxn(context), &dbt,
				  &context.data(), flags);
	if (err == 0) {
		id.setThisFromDbt(context.data());
		stringCache_.insert(&dbt, id.raw());
	} else
		id.reset();
	return err;
}

int DictionaryDatabase::dump(DB_ENV *env, const std::string &name,
			     std::ostream *out)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));

	int err = Container::writeHeader(primary->getDatabaseName(), out);
	if(err == 0) err = primary->dump(out);

	if(err == 0) err = Container::writeHeader(secondary->getDatabaseName(), out);
	if(err == 0) err = secondary->dump(out);

	return err;
}

int DictionaryDatabase::load(DB_ENV *env, const std::string &name,
			     std::istream *in, unsigned long *lineno)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));

	// Load primary
	int err = Container::verifyHeader(primary->getDatabaseName(), in);
	if(err != 0) {
		ostringstream oss;
		oss << "DictionaryDatabase::load() invalid database dump file loading '" << name << "'";
		Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
	}
	else {
		err = primary->load(in, lineno);
	}

	// Load secondary
	if(err == 0) {
		err = Container::verifyHeader(secondary->getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DictionaryDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
		}
		else {
			err = secondary->load(in, lineno);
		}
	}

	return err;
}

int DictionaryDatabase::verify(DB_ENV *env, const std::string &name,
			       std::ostream *out, u_int32_t flags)
{
	PrimaryDatabase::Ptr primary(new PrimaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));
	SecondaryDatabase::Ptr secondary(new SecondaryDatabase(env, name, dictionary_name, DEFAULT_CONFIG));

	int err = 0;
	int terr = 0;
	// once created the databases need to be verified to destroy the handles
	if(flags & DB_SALVAGE)
		terr = Container::writeHeader(primary->getDatabaseName(), out);
	err = terr;
	terr = primary->verify(out, flags);
	if (terr)
		err = terr;

	if(flags & DB_SALVAGE)
		terr = Container::writeHeader(secondary->getDatabaseName(), out);
	if (terr)
		err = terr;
	terr = secondary->verify(out, flags);
	if (terr)
		err = terr;
	return err;
}

// upgrades required:
//  from version 3 (2.0) to 4 (2.1) or higher
//  3->4 (2.0->2.1)
//   o byte-swap ids
//  4->5 (2.1->2.2)
//   o marshal all ids in secondary.  Keys are unchanged.
//     old value format: nameID as 32-bit int
//     new value format: nameID as packed/marshaled int
//  5->6 (2.2->2.3)
//   o nothing

// determine if the name ID needs swapping.
// If the container was created big-endian, then yes.
// This is detected by finding *little-endian* values
// that are very large. Note that this could have issues
// if there is a *HUGE* number of dictionary names, which
// is very unlikely
static bool needsSwap(u_int32_t val)
{
	bool be = Manager::isBigendian();
	if (be) {
		if (val <= 0x00010000)
			return true;
	} else {
		if (val >= 0x00010000)
			return true;
	}
	return false;
	
}

//static
void DictionaryDatabase::upgrade(const std::string &name,
				 const std::string &tname, Manager &mgr,
				 int old_version, int current_version)
{
	DBXML_ASSERT(old_version < current_version);
	int err = 0;
	// could probably do 2-stage upgrade in one iteration, but
	// for now, it's simpler to do 2 separate stages
	if (old_version == VERSION_20) {
		SecondaryDatabase secondary(mgr.getDB_ENV(),
					    name, dictionary_name, DEFAULT_CONFIG);
		err = secondary.open(0, /*duplicates*/true, DEFAULT_CONFIG);
		// byte-swap dictionary ids
		// iterate through all records in secondary
		DbXmlDbt key;
		DbXmlDbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((ret = curs.get(key, data, DB_NEXT)) == 0) {
			u_int32_t *p = reinterpret_cast<u_int32_t*>(data.data);
			if (!needsSwap(*p))
				break;
			curs.del(0);
			M_32_SWAP(*p);
			int putRet = curs.put(key, data, DB_NODUPDATA);
			if (putRet != 0)
				err = 1;
		}
	}
	if (err == 0 && old_version < VERSION_22) {
		// need to do upgrade to 2.2
		SecondaryDatabase secondary(mgr.getDB_ENV(),
					    name, dictionary_name, DEFAULT_CONFIG);
		err = secondary.open(0, /*duplicates*/true, DEFAULT_CONFIG);
		DbXmlDbt key;
		DbXmlDbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((ret = curs.get(key, data, DB_NEXT)) == 0) {
			u_int32_t tid = *(reinterpret_cast<u_int32_t *>
					  (data.data));
			if (Manager::isBigendian())
				M_32_SWAP(tid);
			// create and marshal ID, then put
			DbtOut dbt;
			NameID id(tid);
			id.setDbtFromThis(dbt);
			curs.del(0);
			int putRet = curs.put(key, dbt, DB_NODUPDATA);
			if (putRet != 0)
				err = 1;
		}
	}
	// copy to new container
	if (err == 0)
		err = DbWrapper::copySecondary(
			mgr.getDB_ENV(),
			name, tname, "secondary_",
			dictionary_name, true);
	if (err == 0) {
		// copy dictionary primary
		PrimaryDatabase primary(mgr.getDB_ENV(), name,
					dictionary_name, DEFAULT_CONFIG);
		primary.open(0, DEFAULT_CONFIG);
		PrimaryDatabase newPrimary(mgr.getDB_ENV(), tname,
					   dictionary_name, DEFAULT_CONFIG);
		newPrimary.open(0, CREATE_CONFIG);

		Cursor src(primary, (Transaction*)0, DbXml::CURSOR_READ);
		DbXmlDbt key;
		DbXmlDbt data;
		int err = 0, ret = 0;
		while ((err == 0) &&
		       (ret = src.get(key, data, DB_NEXT)) == 0) {
			newPrimary.put(0, &key, &data, DB_APPEND);
		}
	}

	if (err != 0) {
		char buf[100];
		sprintf(buf, "Unexpected error upgrading Dictionary DB: %d",
			err);
		throw XmlException(XmlException::DATABASE_ERROR, buf);
	}
}
