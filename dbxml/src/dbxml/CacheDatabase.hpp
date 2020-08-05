//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CACHEDATABASE_HPP
#define	__CACHEDATABASE_HPP

#include <string>
#include <db.h>
#include <errno.h>
#include "dbxml/XmlException.hpp"
#include "DbWrapper.hpp"
#include "ReferenceCounted.hpp"

namespace DbXml
{

class OperationContext;
class DocID;

// comparison function
typedef int (*CacheDatabaseCompareFcn)(DB *, const DBT *, const DBT *);
	
class CacheDatabase : public ReferenceCounted {
public:
	CacheDatabase(DB_ENV *env, DBTYPE type, CacheDatabaseCompareFcn compare);
	virtual ~CacheDatabase() {}
	DB_ENV *getDB_ENV();
	DbWrapper *getDb() { return &db_; }

protected:
	DbWrapper db_;
};

class RecordDatabase : public CacheDatabase {
public:
	RecordDatabase(DB_ENV *env);
};

class DocDatabase : public CacheDatabase {
public:
	DocDatabase(DB_ENV *env, CacheDatabaseCompareFcn compare = 0);
};


class CacheDatabaseHandle
{
public:
	CacheDatabaseHandle() : cdb_(0) {}
	CacheDatabaseHandle(CacheDatabase *cdb) : cdb_(cdb)
	{
		if (cdb_ != 0)
			cdb_->acquire();
	}
	~CacheDatabaseHandle()
	{
		if (cdb_ != 0)
			cdb_->release();
	}

	operator CacheDatabase &() const {
		return *cdb_;
	}
	operator CacheDatabase *() const {
		return cdb_;
	}

	DbWrapper *getDb() const {
		if (cdb_) return cdb_->getDb();
		return 0;
	}

	CacheDatabaseHandle &operator=(const CacheDatabaseHandle &o)
	{
		if (this != &o && cdb_ != o.cdb_) {
			if (cdb_ != 0)
				cdb_->release();
			cdb_ = o.cdb_;
			if (cdb_ != 0)
				cdb_->acquire();
		}
		return *this;
	}
	CacheDatabaseHandle &operator=(CacheDatabase *cdb)
	{
		if (cdb_ != cdb) {
			if (cdb_ != 0)
				cdb_->release();
			cdb_ = cdb;
			if (cdb_ != 0)
				cdb_->acquire();
		}
		return *this;
	}
	bool isNull() const { return (cdb_ == 0); }
	static bool docExists(DbWrapper *db, const DocID &did);
private:
	CacheDatabase *cdb_;
};
	
}

#endif

