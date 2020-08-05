//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "CacheDatabase.hpp"
#include "Cursor.hpp"
#include "nodeStore/NsFormat.hpp"
#include "ContainerConfig.hpp"

using namespace std;
using namespace DbXml;

#define DEFAULT_CACHE_PAGESIZE (8 * 1024)

static ContainerConfig createCacheConfig() {
  ContainerConfig config;
  config.setPageSize(DEFAULT_CACHE_PAGESIZE);
  return config;
}

// TBD Should we specify a page size here, or just leave it at 0 for the default - jpcs
CacheDatabase::CacheDatabase(DB_ENV *env, DBTYPE type,
			     CacheDatabaseCompareFcn compare)
	: db_(env, "", "", "", createCacheConfig())
{
	if (compare) {
		DBXML_ASSERT(type == DB_BTREE);
		db_.getDb()->set_bt_compare(db_.getDb(), compare);
	}
	ContainerConfig config;
	config.setAllowCreate(true);
	config.setDbOpenFlags(config.getDbOpenFlags()|DB_NO_AUTO_COMMIT);
	config.setDbSetFlags(config.getDbSetFlags());
	int err = db_.open(0, type, config);
	if (err == 0) {
		try {
			db_.setReadCursor(new Cursor(db_, NULL, CURSOR_READ));
#if DBVER >= 48
			db_.setWriteCursor(new Cursor(db_, NULL, CURSOR_WRITE, "cachedb", DB_CURSOR_BULK));
#else
			db_.setWriteCursor(new Cursor(db_, NULL, CURSOR_WRITE));
#endif
		} catch (...) {
			db_.cleanup();
			throw;
		}
	} else {
		db_.cleanup();
		throw XmlException(err);
	}
}

DB_ENV *CacheDatabase::getDB_ENV()
{
	return db_.getEnvironment();
}

RecordDatabase::RecordDatabase(DB_ENV *env) : CacheDatabase(env, DB_RECNO, 0)
{
}

DocDatabase::DocDatabase(DB_ENV *env,
			 CacheDatabaseCompareFcn compare)
	: CacheDatabase(env, DB_BTREE, compare)
{
}

//static
bool CacheDatabaseHandle::docExists(DbWrapper *db, const DocID &id)
{
	DbtOut key;
	DbtIn none(0, 0);
	none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.

	// Use the doc metadata node to test for existence.
	NsFormat::marshalNodeKey(id, *NsNid::getMetaDataNid(), key);
	int err = db->get(0, &key, &none, 0);
	return (err == 0);
}

