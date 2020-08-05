//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DICTIONARYCACHE_HPP
#define	__DICTIONARYCACHE_HPP

#include <dbxml/XmlPortability.hpp>
#include "DbXmlInternal.hpp"
#include "NameID.hpp"
#include "ReferenceCounted.hpp"

namespace DbXml
{

#define ALIGN_OFFSET (ALLOC_ALIGN - 1)
#define DC_HASHSIZE 211
        
class DictionaryCacheEntry;
class DictionaryDatabase;
class OperationContext;

class DBXML_EXPORT DictionaryCacheBuffer {
public:
	DictionaryCacheBuffer(bool isFirst = false);
	~DictionaryCacheBuffer();
	DictionaryCacheEntry *allocateEntry(int len, dbxml_mutex_t mutex);

private:
	static int align(int sz) {
		return ((sz + ALIGN_OFFSET) & ~ALIGN_OFFSET);
	}
	DictionaryCacheBuffer *current_;
	DictionaryCacheBuffer *next_;
	char *buffer_;
	int capacity_;
	int used_;
	static const int dcacheBufferSize_;
};
	
/**
 * Implements a hash table cache of ID->name for a dictionary
 */
class DBXML_EXPORT DictionaryCache
{
public:
	DictionaryCache();
	~DictionaryCache();
	void setDictionaryDatabase(DictionaryDatabase *ddb) {
		ddb_ = ddb;
	}
	// Only one basic lookup, returning a pointer that will be valid
	// until this object is deleted
	const char *lookup(OperationContext &context, const NameID &id,
			   bool useDictionary);
	bool lookup(OperationContext &context, const NameID &id,
		    DbtOut &dbt, bool useDictionary);
	void insert(const nameId_t nid, const char *value);
	void insert(const nameId_t nid, const DbXmlDbt &value);

private:
	static int hash(const nameId_t nid) {
		return (nid % DC_HASHSIZE);
	}
private:
	// configuration
	DictionaryDatabase *ddb_;
	DictionaryCacheEntry *htable_[DC_HASHSIZE];
	DictionaryCacheBuffer mem_;
	dbxml_mutex_t mutex_;
};

//Ozan Yigit's original sdbm hash, implemented in db/hash/hash_func.c
extern "C" u_int32_t __ham_func3(DB *, const void *, u_int32_t);

/* The string cache is used to cache string/id pairs. It has a rather complex
 * structure in order to prevent string/id pairs that have been removed from
 * the dictionary database after an abort from remaining in the cache.  There exists
 * a per dictionary database cache that is multi-threaded and used to hold 
 * pairs after a get to the secondary dictionary database.  There also exist 
 * string caches in notification objects in each transaction that performs inserts 
 * on the dictionary database.  These caches are single threaded, and have entries
 * added after an insert into the database.  This means that the dictionary database
 * cache is only updated after lookups, and never after inserts.  After the transaction 
 * owning the string cache commits the cache is discarded.  Merging the local caches into the
 * main cache after a commit was tried, but created too much of a concurrency
 * bottleneck.
*/
class DBXML_EXPORT DictionaryStringCache
{
public:
	// Construction is a bit expensive.  Only create when absolutely needed.
	DictionaryStringCache(bool useMutex);
	~DictionaryStringCache();
	nameId_t *lookup(DbXmlDbt *str) const;
	void insert(DbXmlDbt *str, const nameId_t nid);

private:
	static u_int32_t hash(const char *str, u_int32_t len) {
		return __ham_func3(0, (const void *)str, len) % DC_HASHSIZE;
	}
private:
	// configuration
	DictionaryCacheEntry *htable_[DC_HASHSIZE];
	DictionaryCacheBuffer mem_;
	dbxml_mutex_t mutex_;
};
}

#endif

