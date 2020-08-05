//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBWRAPPER_HPP
#define	__DBWRAPPER_HPP

#include <dbxml/XmlPortability.hpp>
#include <string>
#include <db.h>
#include <errno.h>
#include "ScopedPtr.hpp"
#include "SharedPtr.hpp"
#include "Transaction.hpp"
#include "Globals.hpp"
#include "OperationContext.hpp"
#include "dbxml/XmlException.hpp"

extern "C" {
int index_duplicate_compare(DB *db, const DBT *dbt1, const DBT *dbt2);
};

// provide consistent handling of deadlock and DB errors
#define HANDLE_DB_ERRORS \
	if (dberr == DB_LOCK_DEADLOCK) \
		throw XmlException(dberr); \
return dberr;

// Counters
#define INCR_WRAPPER_STATS(ctr) Globals::incrementCounter(ctr)

namespace DbXml
{

class Key;
class PrimaryCursor;
class IndexCursor;
class NameID;
class Syntax;
class OperationContext;
class DbtOut;
class IndexEntry;
class NodeIterator;
class ContainerBase;
class Cursor;
	
/// Wraps a Db.
class DBXML_EXPORT DbWrapper
{
public:
	// this class is used to run a given function on *all* databases
	// in a container (e.g. truncate(), compact()).  Not used
	// for upgrade because upgrade is order-dependent.
	class DbFunctionRunner {
	public:
		virtual ~DbFunctionRunner() {}
		virtual void run(Transaction *txn, DbWrapper &database) = 0;
	};
	
	enum Operation { NONE, ALL, EQUALITY, NEG_NOT_EQUALITY, LTX, LTE, GTX, GTE,
			 RANGE, PREFIX, SUBSTRING }; // Cursor Operation
	typedef int(*bt_compare_fn)(DB *, const DBT *, const DBT *);

	DbWrapper(DB_ENV *environment, const std::string &name,
		  const std::string &prefixName,
		  const std::string &databaseName,
		  const ContainerConfig &config);
	virtual ~DbWrapper();

	int open(Transaction *txn, DBTYPE type, const ContainerConfig &config);
	int close(u_int32_t flags);
	void cleanup();
	int dump(std::ostream *out);
	int load(std::istream *in, unsigned long *lineno);
	int verify(std::ostream *out, u_int32_t flags);
	int sync()
	{
		int dberr = db_->sync(db_, 0);
		HANDLE_DB_ERRORS;
	}
	int put(Transaction *txn, DbXmlDbt *key, DbXmlDbt *data, u_int32_t flags)
	{
		int dberr = db_->put(db_, Transaction::toDB_TXN(getTxn(txn)), key,
				     data, flags);
		INCR_WRAPPER_STATS(Counters::num_dbput);
		HANDLE_DB_ERRORS;
	}
	int get(Transaction *txn, DbXmlDbt *key, DbXmlDbt *data, u_int32_t flags) const
	{
		int dberr = const_cast<DB*>(db_)->
			get(db_, Transaction::toDB_TXN(getTxn(txn)), key,
			    data, flags);
		INCR_WRAPPER_STATS(Counters::num_dbget);
		HANDLE_DB_ERRORS;
	}
	int del(Transaction *txn, DbXmlDbt *key, u_int32_t flags)
	{
		int dberr = db_->del(db_, Transaction::toDB_TXN(getTxn(txn)),
				     key, flags);
		INCR_WRAPPER_STATS(Counters::num_dbdel);
		HANDLE_DB_ERRORS;
	}
	int truncate(Transaction *txn, u_int32_t *countp, u_int32_t flags)
	{
		int dberr = db_->truncate(db_, Transaction::toDB_TXN(getTxn(txn)),
					  countp, flags);
		HANDLE_DB_ERRORS;
	}
	int key_range(Transaction *txn, DbXmlDbt *key, DB_KEY_RANGE *key_range,
		      u_int32_t flags) const
	{
		int dberr = const_cast<DB*>(db_)->
			key_range(db_, Transaction::toDB_TXN(getTxn(txn)),
				  key, key_range, flags);
		HANDLE_DB_ERRORS;
	}
	int stat(Transaction *txn, void *sp, u_int32_t flags) const
	{
		int dberr = const_cast<DB*>(db_)->
			stat(db_, Transaction::toDB_TXN(getTxn(txn)),
			     sp, flags);
		HANDLE_DB_ERRORS;
	}

	/// Returns the number of pages in the database.
	unsigned long getNumberOfPages() const;

	u_int32_t getPageSize() const;

	DB *getDb() {
		return db_;
	}

	DB_ENV *getEnvironment() const {
		return environment_;
	}

	std::string getDatabaseName() const {
		return prefixName_ + databaseName_;
	}
	
	// copy all records -- used for upgrade
	int copy(DbWrapper &from, u_int32_t putFlags);

	static int copySecondary(DB_ENV *env,
				 const std::string &oldcname,
				 const std::string &newcname,
				 const std::string &prefix,
				 const std::string &dbname,
				 bool duplicates = false);

	bool isTransacted() const { 
		return ((flags_ & DBW_TRANSACTED) != 0);
	}
	
	void setTransacted() { flags_ |= DBW_TRANSACTED; }
	
	bool isCDBEnv() const { 
		return ((flags_ & DBW_CDB_ENV) != 0);
	}
	
	void setCDBEnv() { flags_ |= DBW_CDB_ENV; }
	
	bool isLocking() const { 
		return ((flags_ & DBW_LOCKING) != 0);
	}
	
	void setLocking() { flags_ |= DBW_LOCKING; }
	
	Transaction *getTxn(OperationContext &oc) const {
		return (isTransacted() ? oc.txn() : 0);
	}
	Transaction *getTxn(Transaction *txn) const {
		return (isTransacted() ? txn : 0);
	}
	Cursor *getReadCursor() {
		return readCursor_;
	}
	Cursor *getWriteCursor() {
		return writeCursor_;
	}
	void setReadCursor(Cursor *cursor) {
		readCursor_ = cursor;
	}
	void setWriteCursor(Cursor *cursor) {
		writeCursor_ = cursor;
	}
	static const char *operationToString(Operation operation);
	static const char *operationToWord(Operation operation);

protected:
	u_int32_t flags_;
	std::string name_;
	std::string prefixName_;
	std::string databaseName_;
	u_int32_t pageSize_;
	
	DB *db_;
	DB_ENV *environment_;
	Cursor *readCursor_;
	Cursor *writeCursor_;

private:
	// no need for copy and assignment
	DbWrapper(const DbWrapper&);
	DbWrapper & operator = (const DbWrapper &);
	enum flags {
		DBW_TRANSACTED = 1,
		DBW_LOCKING = 2,
		DBW_CDB_ENV = 4
	};
        void setFlags(DB_ENV *env);
};

/// Wraps a Primary Db.
class PrimaryDatabase : public DbWrapper
{
public:
	typedef ScopedPtr<PrimaryDatabase> Ptr;

	PrimaryDatabase(DB_ENV *environment, const std::string &name,
			const std::string &databaseName,
			const ContainerConfig &config);
	virtual ~PrimaryDatabase();

	int open(Transaction *txn, const ContainerConfig &config);

	int getPrimary(OperationContext &context, const NameID &id,
		       DbtOut *data, u_int32_t flags) const;
	/// put some data using the next available id as the key.
	int putPrimary(OperationContext &context, const NameID &id,
		       const DbXmlDbt &data, u_int32_t flags);
	int appendPrimary(OperationContext &context, NameID &id,
			  const DbXmlDbt &data, u_int32_t flags);
	int deletePrimary(OperationContext &context, const NameID &id,
			  u_int32_t flags);

private:
	// no need for copy and assignment
	PrimaryDatabase(const PrimaryDatabase&);
	PrimaryDatabase& operator = (const PrimaryDatabase&);
};

/// Wraps a Secondary Db.
class SecondaryDatabase : public DbWrapper
{
public:
	typedef SharedPtr<SecondaryDatabase> Ptr;

	SecondaryDatabase(DB_ENV *environment, const std::string &name,
			  const std::string &databaseName,
			  const ContainerConfig &config);

	int open(Transaction *txn, bool duplicates, const ContainerConfig &config);

private:
	// no need for copy and assignment
	SecondaryDatabase(const SecondaryDatabase&);
	SecondaryDatabase &operator = (const SecondaryDatabase&);
};

/// Wraps an Index Db.
class IndexDatabase : public DbWrapper
{
public:
	typedef SharedPtr<IndexDatabase> Ptr;

	IndexDatabase(DB_ENV *environment, const std::string &name,
		      const std::string &databaseName,
		      const Syntax *syntax,
		      const ContainerConfig &config);

	int open(Transaction *txn, bool duplicates, bool nodesIndexed,
		 const ContainerConfig &config);

	// We assume that key has the correct endianness.
	int getIndexEntry(OperationContext &context, const DbXmlDbt &key,
			  IndexEntry &ie) const;

	// We assume that key has the correct endianness.
	int putIndexEntry(OperationContext &context, const DbXmlDbt &key,
			  const DbXmlDbt &data);

	// We assume that key has the correct endianness.
	int updateIndexEntry(OperationContext &context, const DbXmlDbt &key,
			     const DbXmlDbt &data);

	// We assume that key has the correct endianness.
	int delIndexEntry(OperationContext &context, const DbXmlDbt &key,
			  const DbXmlDbt &data);

	// We assume that key has the correct endianness.
	bool exists(Transaction *txn, const DbXmlDbt &key) const;

	// Returns the % of keys in the index that will be returned
	// by the operation.
	double percentage(OperationContext &context, Operation operation,
			  Operation gto, Operation lto, const Key &key1,
			  const Key &key2) const;

	// Create cursors for index databases
	IndexCursor *createCursor(Transaction *txn, Operation operation,
		const Key *key, bool reverse);
	IndexCursor *createCursor(Transaction *txn, Operation gto,
		const Key *gtk, Operation lto, const Key *ltk, bool reverse);

	// Create cursors for an index as NodeIterator objects
	NodeIterator *createNodeIterator(Transaction *txn, Operation operation,
		const Key *key, ContainerBase *container, bool documentIndex,
		const LocationInfo *l);
	NodeIterator *createNodeIterator(Transaction *txn, Operation gto,
		const Key *gtk, Operation lto, const Key *ltk,
		ContainerBase *container, bool documentIndex, const LocationInfo *l);

private:
	// no need for copy and assignment
	IndexDatabase(const IndexDatabase&);
	IndexDatabase &operator = (const IndexDatabase&);

	void getMinKeyDbt(const Key &key, DbtOut &dbt) const;
	void getMaxKeyDbt(const Key &key, DbtOut &dbt) const;
	void getNextKeyDbt(const Key &key, DbtOut &dbt) const;
	// The syntax for the database. Describes how keys are compared.
	const Syntax *syntax_;
};

}

#endif

