//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CACHEDATABASEMINDER_HPP
#define	__CACHEDATABASEMINDER_HPP

#include "DbXmlInternal.hpp"
#include "CacheDatabase.hpp"
#include "DocID.hpp"
#include "ReferenceCounted.hpp"
#include "Manager.hpp"
#include <vector>

namespace DbXml
{

class CacheDBEntry
{
public:
	CacheDBEntry(): cid_(0) {}
	~CacheDBEntry() {}
	int getContainerID() const { return cid_; }
	CacheDatabase *getDB() { return cdb_; }

	void setContainerID(int cid) { cid_ = cid; }
	void setDB(CacheDatabase *cdb) { cdb_ = cdb; }
private:
	int cid_;
	CacheDatabaseHandle cdb_;
};
	
class CacheDatabaseMinderImpl : public ReferenceCounted
{
public:
	CacheDatabaseMinderImpl(Manager &mgr)
		: mgr_(mgr), entries_(3) {}
	~CacheDatabaseMinderImpl();

	// find or create a cache db for container id
	CacheDatabase *findOrAllocate(int cid, bool allocate);

	// notification that a document can be removed
	// TBD -- not yet used or implemented
	void releaseDoc(int cid, const DocID &did) {}
	
	// merge the Document's cache database into the minder.  Last
	// one wins, replacing earlier databases for the same
	// container
	void mergeCacheDatabase(Document *doc);
private:
	// ensure that an entry has a database
	CacheDatabase *verify(CacheDBEntry &entry, int cid);
	void copyDatabase(CacheDatabase *db, int cid);
protected:
	Manager &mgr_;
	typedef std::vector<CacheDBEntry*> CacheDBEntries;
	CacheDBEntries entries_; // for "real" containers only
	CacheDBEntry tempDb_; // 0 id, for constructed docs
};

///////////////////
// CacheDatabaseMinder -- a handle class for CacheDatabaseMinderImpl
//   Consumers should use this class.
//
class CacheDatabaseMinder
{
public:
	CacheDatabaseMinder() : impl_(0) {}
	CacheDatabaseMinder(CacheDatabaseMinderImpl *minder) : impl_(minder)
	{
		if (impl_)
			impl_->acquire();
	}

	~CacheDatabaseMinder()
	{
		if (impl_)
			impl_->release();
	}

	// allocate an implementation if not present
	void init(Manager &mgr);
	
	operator CacheDatabaseMinderImpl &() const {
		return *impl_;
	}
	operator CacheDatabaseMinderImpl *() const {
		return impl_;
	}

	CacheDatabaseMinder &operator=(const CacheDatabaseMinder &o)
	{
		if (this != &o && impl_ != o.impl_) {
			if (impl_ != 0)
				impl_->release();
			impl_ = o.impl_;
			if (impl_ != 0)
				impl_->acquire();
		}
		return *this;
	}
	bool isNull() const { return (impl_ == 0); }

	// find or create a cache db for container id
	CacheDatabase *findOrAllocate(Manager &mgr, int cid,
				      bool allocate = true);

	// allocate a new doc id (only works for the single
	// container with id 0)
	DocID allocateDocID(Manager &mgr) {
		return mgr.allocateTempDocID();
	}

	// notification that a document can be removed
	// TBD -- not yet used or implemented
	void releaseDoc(int cid, const DocID &did) {
		DBXML_ASSERT(impl_);
		impl_->releaseDoc(cid, did);
	}

	void mergeCacheDatabase(Document *doc);
private:
	CacheDatabaseMinderImpl *impl_;
};

}

#endif
