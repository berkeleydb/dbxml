//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "DbXmlInternal.hpp"
#include "BulkPut.hpp"
#include "ScopedDbt.hpp"
#include "OperationContext.hpp"
#include "DbWrapper.hpp"

using namespace DbXml;

#define BULK_SIZE 1024*64 // TBD: make tunable/flexible

BulkPut *OperationContext::getBulkPut(bool allocate)
{
#ifdef DBXML_BULK
	if (!bp_ && allocate)
		bp_ = new BulkPut(BULK_SIZE);
	if (!bp_ && allocate)
		throw XmlException(XmlException::NO_MEMORY_ERROR,
				   "Could not allocate BulkPut object");
	return bp_;
#else
	return NULL;
#endif
}

BulkPut::BulkPut(size_t bufSize)
	: ptr_(0), db_(0)
{
#ifdef DBXML_BULK
	multiple_.flags = DB_DBT_USERMEM;
	multiple_.size = multiple_.ulen = (u_int32_t) bufSize;
	DBXML_ASSERT(multiple_.size % sizeof(u_int32_t) == 0);
	multiple_.data = new u_int32_t[multiple_.size / sizeof(u_int32_t)]; 
	if (!multiple_.data)
		throw XmlException(XmlException::NO_MEMORY_ERROR,
				   "malloc failed in BulkPut constructor");
	DB_MULTIPLE_WRITE_INIT(ptr_, &multiple_);
#endif
}

BulkPut::~BulkPut()
{
#ifdef DBXML_BULK
	if (multiple_.data)
		delete []((u_int32_t *)multiple_.data);
#endif
}

void BulkPut::flush(Transaction *txn)
{
#ifdef DBXML_BULK
	if (ptr_) {
		DBXML_ASSERT(db_);
		// sort...  If ever possible make sorting optional
		DB *db = db_->getDb();
		int err = db->sort_multiple(db, &multiple_, NULL, DB_MULTIPLE_KEY);
		// put
		if (err == 0)
			err = db_->put(txn, &multiple_, NULL, DB_MULTIPLE_KEY);
		if (err != 0)
			throw XmlException(err);
		DB_MULTIPLE_WRITE_INIT(ptr_, &multiple_);
	}
#endif
}

bool BulkPut::store(Transaction *txn, DbXmlDbt *key, DbXmlDbt *data)
{
#ifdef DBXML_BULK
	void *ptr;
	void *dptr;
	DB_MULTIPLE_KEY_RESERVE_NEXT(ptr_, &multiple_, ptr, key->size, dptr, data->size);
	if (ptr == 0) {
		flush (txn);
		DB_MULTIPLE_KEY_RESERVE_NEXT(ptr_, &multiple_, ptr, key->size, dptr, data->size);
	}
	if (!ptr && !dptr)
		return false; // record is too large for buffer
	memcpy(ptr, key->data, key->size);
	memcpy(dptr, data->data, data->size);
	return true;
#else
	return false;
#endif
}

