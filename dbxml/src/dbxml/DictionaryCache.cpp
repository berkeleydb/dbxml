//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DictionaryCache.hpp"
#include "DictionaryDatabase.hpp"
#include "ScopedDbt.hpp"
#include "nodeStore/NsUtil.hpp"

using namespace DbXml;

namespace DbXml
{

//
// This implementation relies on the fact that the values never
// disappear -- the pointers must remain valid
//	
class DictionaryCacheEntry {
public:
	DictionaryCacheEntry(nameId_t nid, void *value,
			     int valueLen) : nid_(nid), next_(0), len_(valueLen)
	{
		// allocator created enough space for copy
		memcpy(((char*)this) + sizeof(*this), value, valueLen);
	}
	DictionaryCacheEntry *getNext() const {
		return next_;
	}
	void setNext(DictionaryCacheEntry *next) {
		next_ = next;
	}
	nameId_t getNid() const {
		return nid_;
	}
	nameId_t *getNidPtr() {
		return &nid_;
	}
	size_t getLen() const {
		return len_;
	}
	// value starts just after nid and next
	const char *getValue() const {
		return ((char *)this) + sizeof(*this);
	}
private:
	nameId_t nid_;
	DictionaryCacheEntry *next_;
	int len_;
	// value follows
};
}

const int DictionaryCacheBuffer::dcacheBufferSize_ = 4096;

DictionaryCacheBuffer::DictionaryCacheBuffer(bool isFirst) : current_(0), next_(0),
	buffer_(0), capacity_(dcacheBufferSize_), used_(0)
{
	if (isFirst)
		current_ = this;
	buffer_ = (char *) ::malloc(dcacheBufferSize_);
	if (!buffer_)
		throw XmlException(
			XmlException::NO_MEMORY_ERROR,
			"Failed to allocate memory for DictionaryCache");
}
	
DictionaryCacheBuffer::~DictionaryCacheBuffer() {
	// semi-recursive.  If this is ever a problem, change it
	if (next_) {
		delete next_;
		next_ = 0;
	}
	::free(buffer_);
}

DictionaryCache::DictionaryCache()
	: ddb_(0), mem_(true), mutex_(MutexLock::createMutex())
{
	memset(htable_, 0, DC_HASHSIZE * sizeof(DictionaryCacheEntry*));
}

DictionaryCache::~DictionaryCache()
{
	MutexLock::destroyMutex(mutex_);
}

// Basic lookup, returning a pointer that will be valid
// until this object is deleted.  Lookup does not require locking
// because entries are never removed, and they are only added
// at the beginning of the hash chain with a simple pointer update.
// A race condition could theoretically happen if a compiler reorders
// some of the code in insert
const char *
DictionaryCache::lookup(OperationContext &context, const NameID &id,
			bool useDictionary)
{
	while (true) { // will only ever loop once
		nameId_t nid = id.raw();
		int bucket = hash(nid);
		DictionaryCacheEntry *current = htable_[bucket];
		while (current && (current->getNid() != nid))
			current = current->getNext();
		if (current)
			return current->getValue();
		if (!useDictionary)
			return 0;
		// read through the dictionary
		const char *name = 0;
		int ret = ddb_->lookupStringNameFromID(context, id,
						       &name);
		if (ret == 0) {
			DBXML_ASSERT(name);
			insert(nid, name);
		} else {
			// this should never happen, but if it does,
			// be silent (for now)
			return 0;
		}
	}
}

// Basic lookup, returning a pointer that will be valid
// until this object is deleted.  Lookup does not require locking
// because entries are never removed, and they are only added
// at the beginning of the hash chain with a simple pointer update.
// A race condition could theoretically happen if a compiler reorders
// some of the code in insert
bool
DictionaryCache::lookup(OperationContext &context, const NameID &id,
			DbtOut &dbt, bool useDictionary)
{
	while (true) { // will only ever loop once
		nameId_t nid = id.raw();
		int bucket = hash(nid);
		DictionaryCacheEntry *current = htable_[bucket];
		while (current && (current->getNid() != nid))
			current = current->getNext();
		if (current) {
			dbt.set(current->getValue(), current->getLen());
			return true;
		}
		if (!useDictionary)
			return false;
		// read through the dictionary
		int ret = ddb_->lookupStringNameFromID(context, id, dbt);
		if (ret == 0) {
			DBXML_ASSERT(dbt.size);
			insert(nid, dbt);
		} else {
			// this should never happen, but if it does,
			// be silent (for now)
			return false;
		}
	}
}

// inserting at the beginning of the chain
void
DictionaryCache::insert(const nameId_t nid, const char *value)
{
	DbtIn val((void *)value, ::strlen(value) + 1);
	insert(nid, val);
}

// inserting at the beginning of a chain
void
DictionaryCache::insert(const nameId_t nid, const DbXmlDbt &value)
{
	// will throw if no memory
	DictionaryCacheEntry *newEntry = mem_.allocateEntry(
		value.size, mutex_);
	// placement new
	(void) new (newEntry) DictionaryCacheEntry(nid, value.data,
						   value.size);
	int bucket = hash(nid);
	MutexLock ml(mutex_);
	DictionaryCacheEntry *current = htable_[bucket];
	if (!current) {
		htable_[bucket] = newEntry;
	} else {
		//Theoretical race condition if a compiler
 		//switches these two operations
		newEntry->setNext(current);
		htable_[bucket] = newEntry;
	}
}

DictionaryCacheEntry *
DictionaryCacheBuffer::allocateEntry(int valueLen, dbxml_mutex_t mutex) {
	int needed = valueLen + sizeof(DictionaryCacheEntry);
	needed = align(needed); // round up to 4-byte boundary

	MutexLock ml(mutex);
	if ((current_->used_ + needed) > current_->capacity_) {
		ml.unlock(); // unlock for allocation; note: this can race
		DictionaryCacheBuffer *newbuf =
			new DictionaryCacheBuffer();
		if (!newbuf)
			throw XmlException(
				XmlException::NO_MEMORY_ERROR,
				"Failed to allocate memory for DictionaryCache");
		ml.lock(); // re-lock, and check state again
		if ((current_->used_ + needed) > current_->capacity_) {
			current_->next_ = newbuf;
			current_ = newbuf;
		} else
			delete newbuf; // lost the race
	}
	DBXML_ASSERT((current_->used_ % 4) == 0);
	DictionaryCacheEntry *retVal = (DictionaryCacheEntry *)
		(current_->buffer_+current_->used_);
	current_->used_ += needed;
	return retVal;
}

DictionaryStringCache::DictionaryStringCache(bool useMutex)
: mem_(true), mutex_(useMutex ? MutexLock::createMutex() : 0)
{
	memset(htable_, 0, DC_HASHSIZE * sizeof(DictionaryCacheEntry*));
}

DictionaryStringCache::~DictionaryStringCache()
{
	MutexLock::destroyMutex(mutex_);
}

/* The value in string cache does not terminate with a null, so have
 * to compare up to its length.
 * I found that this function (and NsUtil::nsStringEqual) were much
 * faster than the str(n)cmp function on Linux, which is why I use this
 * function.  May have just been an issue with that specific library
 * implementation.
*/
static bool strcmplen(const char *str1, const char *value, size_t valueLen) {
	if(str1 == value) return true;

	if (str1 == 0 || value == 0)
		return false;

	for (size_t i = 0; i < valueLen; i++) {
		if(*str1 != *value) return false;
		++str1;
		++value;
	}
	return true;
}

nameId_t *DictionaryStringCache::lookup(DbXmlDbt *str) const
{
	if (!str->size) return 0;
	u_int32_t bucket = hash((const char *)str->data, str->size);
	DictionaryCacheEntry *current = htable_[bucket];
	while (current && !(str->size == current->getLen() &&
		strcmplen((const char *)str->data, current->getValue(), current->getLen())))
		current = current->getNext();
	if (current)
		return current->getNidPtr();
	return 0;
}
void DictionaryStringCache::insert(DbXmlDbt *str, const nameId_t nid)
{
	if (!str->size) return;
	// will throw if no memory
	DictionaryCacheEntry *newEntry = mem_.allocateEntry(
		str->size, mutex_);
	// placement new
	(void) new (newEntry) DictionaryCacheEntry(nid, str->data, str->size);
	int bucket = hash((const char *)str->data, str->size);
	MutexLock ml(mutex_);
	DictionaryCacheEntry *current = htable_[bucket];
	if (!current) {
		htable_[bucket] = newEntry;
	} else {
		//Theoretical race condition if a compiler
 		//switches these two operations
		newEntry->setNext(current);
		htable_[bucket] = newEntry;
	}
}
