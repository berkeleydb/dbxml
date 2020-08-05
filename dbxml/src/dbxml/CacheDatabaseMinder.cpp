//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#include "DbXmlInternal.hpp"
#include "CacheDatabaseMinder.hpp"
#include "Manager.hpp"
#include "Document.hpp"

using namespace DbXml;

CacheDatabase *CacheDatabaseMinder::findOrAllocate(Manager &mgr, int cid,
						   bool allocate)
{
	if (allocate)
		init(mgr);
	if (impl_)
		return impl_->findOrAllocate(cid, allocate);
	return 0;
}

 // allocate an implementation if not present
void CacheDatabaseMinder::init(Manager &mgr)
{
	if (!impl_) {
		impl_ = new CacheDatabaseMinderImpl(mgr);
		if (!impl_)
			throw XmlException(XmlException::NO_MEMORY_ERROR,
					   "Unable to allocate memory for object");
		impl_->acquire();
	}
}

void CacheDatabaseMinder::mergeCacheDatabase(Document *doc)
{
	if (!impl_)
		init(doc->getManager());
	impl_->mergeCacheDatabase(doc);
}

CacheDatabaseMinderImpl::~CacheDatabaseMinderImpl()
{
	size_t size = entries_.size();
	for (size_t i = 0; i < size; i++) {
		if (entries_[i]) {
			delete entries_[i];
			entries_[i] = 0; // not necessary, but..
		}
	}
}
	
// find or create a cache db for container id
CacheDatabase *CacheDatabaseMinderImpl::findOrAllocate(int cid,
						       bool allocate)
{
	if (cid == 0)
		return verify(tempDb_, cid);
	size_t size = entries_.size();
	size_t i;
	for (i = 0; i < size; i++) {
		if (entries_[i]) {
		    if (entries_[i]->getContainerID() == cid) {
			    DBXML_ASSERT(entries_[i]->getDB());
			    return entries_[i]->getDB();
		    }
		} else
			break;
	}
	if (!allocate)
		return 0;
	if (i == size)
		entries_.resize(size << 1);
	CacheDBEntry *newentry = new CacheDBEntry();
	entries_[i] = newentry;
	return verify(*newentry, cid);
}

CacheDatabase *CacheDatabaseMinderImpl::verify(CacheDBEntry &entry, int cid)
{
	if (entry.getDB() == 0) {
		entry.setDB(mgr_.createDocDatabase());
		entry.setContainerID(cid);
	}
	DBXML_ASSERT(entry.getDB() != 0);
	return entry.getDB();
}

// merge the CacheDatabase for the Document into this
// minder, replacing any existing databases with the new
// one (last in wins).  This ensures only one CacheDatabase
// for a given container in flight at the same time.
void CacheDatabaseMinderImpl::mergeCacheDatabase(Document *doc)
{
	CacheDatabase *cdb = doc->getCacheDatabase();
	int cid = doc->getContainerID();
	DBXML_ASSERT(cdb && (cid != 0));
	for (size_t i = 0; i < entries_.size(); i++) {
		CacheDBEntry *entry = entries_[i];
		if (entry && (entry->getContainerID() == cid)) {
			entry->setDB(cdb);
			return;
		}
	}
	copyDatabase(cdb, cid);
}

void CacheDatabaseMinderImpl::copyDatabase(CacheDatabase *db, int cid)
{
	size_t size = entries_.size();
	size_t i;
	for (i = 0; i < size; i++) {
		if (!entries_[i])
			break;
	}
	if (i == size)
		entries_.resize(size << 1);
	
	CacheDBEntry *newentry = new CacheDBEntry();
	if (!newentry)
		throw XmlException(XmlException::NO_MEMORY_ERROR,
				   "Unable to allocate memory for object");
	newentry->setDB(db);
	newentry->setContainerID(cid);
	entries_[i] = newentry;
}
