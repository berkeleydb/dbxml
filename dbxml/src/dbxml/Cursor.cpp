//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "Cursor.hpp"
#include "Key.hpp"
#include "Log.hpp"
#include "IndexEntry.hpp"
#include "Container.hpp"
#include "Manager.hpp"
#include "ScopedDbt.hpp"
#include <dbxml/XmlException.hpp>

#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <cassert>
#include <sstream>

using namespace DbXml;
using namespace std;

#define MINIMUM_BULK_GET_BUFFER 256 * 1024

// determine if two keys refer to the same index, based
// on initial bytes of the key (index type, etc).
static bool isSameIndex(const DbXml::DbXmlDbt &k1, const DbXml::DbXmlDbt &k2)
{
	const xmlbyte_t *p1 = (const xmlbyte_t*)k1.data;
	const xmlbyte_t *p2 = (const xmlbyte_t*)k2.data;
	return (Key::compareStructure(p1, p1 + k1.size, p2,
				      p2 + k2.size) == 0);
}

// Cursor
Cursor::Cursor()
	: dbc_(0),
	  error_(0),
	  cursorName_(0)
{
}

Cursor::Cursor(DbWrapper &db, Transaction *txn, CursorType type,
	       const char *name, u_int32_t flags)
	: dbc_(0),
	  error_(0),
	  cursorName_(name)
{
	if (!db.isTransacted())
		txn = 0;
	int err = open(db, txn, type, flags);
	if (err != 0)
		throw XmlException(err);
}

Cursor::~Cursor()
{
#if 0
	if (cursorName_) {
		cout << "Closing Cursor: " << cursorName_ << endl;
		cursorName_ = 0;
	}
#endif
	close();
}

int Cursor::open(DbWrapper &db, Transaction *txn, CursorType type, u_int32_t flags)
{
	close();

	DBXML_ASSERT(!(txn && !db.isTransacted()));
	if (db.getEnvironment() != 0 && type==CURSOR_WRITE) {
		// If we're in a CDS environment then the cursor used to
		// delete the keys must be created with the DB_WRITECURSOR flag.
		//
		if (db.isCDBEnv())
			flags |= DB_WRITECURSOR;
	}
	DB *dbp = db.getDb();
	error_ = dbp->cursor(dbp, Transaction::toDB_TXN(txn), &dbc_,
			     DB_CURSOR_CREATE_MASK(db,flags));
#if 0
	if (txn && cursorName_) {
		u_int32_t tid = 0;
		tid = txn->getDbTxn()->id();
		cout << "Created Cursor: " << cursorName_ <<
			", tid: " << tid << endl;
	}
#endif
	return error_;
}

// IndexCursor

IndexCursor::IndexCursor(IndexDatabase &db, Transaction *txn, bool initBulk)
	: cursor_(db, txn, CURSOR_READ, "IndexCursor",
		  (db.isLocking() ? DB_READ_COMMITTED : 0)),
	  done_(false)
{
	if (initBulk) {
		// The bulk get buffer should be a multiple of the pagesize
		u_int32_t pagesize = db.getPageSize();
		
		while(pagesize < MINIMUM_BULK_GET_BUFFER)
			pagesize <<= 1;

		bulk_.data = new char[pagesize / sizeof(char)];
                bulk_.ulen = pagesize / sizeof(char);
		bulk_.set_flags(DB_DBT_USERMEM);
	}
}

IndexCursor::~IndexCursor()
{
	if (bulk_.data)
                delete [] (char*)bulk_.data;
}

int IndexCursor::first(IndexEntry &ie)
{
	int err = first();
	if(err == 0 && !done_) {
		ie.setThisFromDbt(data_);
	} else {
		ie.reset();
	}
	return err;
}

int IndexCursor::next(IndexEntry &ie)
{
	int err = next();
	if(err == 0 && !done_) {
		ie.setThisFromDbt(data_);
	} else {
		ie.reset();
	}
	return err;
}

int IndexCursor::nextEntries(u_int32_t flags)
{
	tmpKey_.data = key_.data;
	tmpKey_.size = key_.size;
	int err = cursor_.get(tmpKey_, bulk_, flags);

	while(err == DB_BUFFER_SMALL) {
		// If the buffer is too small, retry with a bigger buffer
		// bulk_.size has the necessary number in it
		delete [] (char*)bulk_.data;
		u_int32_t newsize = bulk_.ulen;
		while (newsize < bulk_.size)
			newsize <<= 1;
		bulk_.data = new char[newsize / sizeof(char)];
		bulk_.ulen = newsize / sizeof(char);
		
		tmpKey_.data = key_.data;
		tmpKey_.size = key_.size;
		err = cursor_.get(tmpKey_, bulk_, flags);
	}
	if(err != 0) {
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
	}

	return err;
}

int IndexCursor::prevEntry()
{
	tmpKey_.data = key_.data;
	tmpKey_.size = key_.size;
	int err = cursor_.get(tmpKey_, data_, DB_PREV);
	if(err != 0) {
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
	}
	return err;
}

// find the last key value that matches the prefix
// Algorithm:
//  1.  first use DB_SET_RANGE to make sure there is
//  at least one match.
//  2.  Add 1 to the least significant digit
//  of the key, do DB_SET_RANGE, and DB_PREV until
//  a match is found.  This finds the last match.
//  NOTE: this algorithm ONLY works for prefix matches,
//  where the key is <index_type,id[,id]>.  It will not
//  work correctly if there is a value at the end of the
//  key, because adding one may generate a type format error.
//  Things to handle:
//   o what if lsb is 255? In that case, add a digit
//     rather than 1.  For now, I think that this is not
//     possible, as 255 isn't going to show up.
//   o no entries -- means initial get will fail
//   o no entries after the range
int IndexCursor::findLastPrefix(DbXmlDbt &key)
{
	int err;
	u_int32_t keySize = key.size;
	DbXmlDbt *lookupKey = &tmpKey_;
	tmpKey_.data = key.data;
	tmpKey_.size = keySize;
	err = cursor_.get(tmpKey_, data_, DB_SET_RANGE);
	if (err != 0) {
		done_ = true;
		return err;
	}
	// one or more matches in DB
	// reset tmpKey_ size, add one to key
	tmpKey_.size = keySize;
	unsigned char *keyArray = (unsigned char *)tmpKey_.data;
	unsigned char last = keyArray[keySize-1];
	DbtOut keyPlusOne;
	if ((unsigned int)last < 255)
		keyArray[keySize-1] += 1;
	else {
		// create a Dbt with an extra byte.  set() method
		// will copy/realloc
		keyPlusOne.set(keyArray, keySize + 1);
		keyArray = (unsigned char *)keyPlusOne.data;
		keyArray[keySize] = 1; //put something in
		lookupKey = &keyPlusOne;
	}

	err = cursor_.get(*lookupKey, data_, DB_SET_RANGE);
	// can use tmpKey_ from here down
	if (err == DB_NOTFOUND) {
		// handle case where there are no entries past
		// the prefix requested
		err = cursor_.get(tmpKey_, data_, DB_LAST);
		if ((err == 0) &&
		    (memcmp(key.data, tmpKey_.data,
			    keySize) == 0)) {
			return err;
		}
	}
	while ((err = cursor_.get(tmpKey_, data_, DB_PREV)) == 0) {
		DBXML_ASSERT(tmpKey_.size >= keySize);
		if (memcmp(key.data, tmpKey_.data,
			   keySize) == 0)
			break;
	}
	return err;
}

// InequalityIndexCursor

InequalityIndexCursor::InequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation operation,
	const Key *k1, const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(operation),
	  greaterThanOperation_(DbWrapper::NONE),
	  lessThanOperation_(DbWrapper::NONE)
{
	DBXML_ASSERT(operation_ != DbWrapper::EQUALITY);
	DBXML_ASSERT(operation_ != DbWrapper::PREFIX);
	DBXML_ASSERT(operation_ != DbWrapper::RANGE);
	DBXML_ASSERT(operation_ != DbWrapper::NEG_NOT_EQUALITY);
	DBXML_ASSERT(syntax != 0);
	DB_MULTIPLE_INIT(p_, &bulk_);
	k1->setDbtFromThis(key_);
}

InequalityIndexCursor::InequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto, const Key *ltk,
	const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(DbWrapper::RANGE),
	  greaterThanOperation_(gto),
	  lessThanOperation_(lto)
{
	DBXML_ASSERT(lto == DbWrapper::LTX || lto == DbWrapper::LTE);
	DBXML_ASSERT(gto == DbWrapper::GTX || gto == DbWrapper::GTE);
	DBXML_ASSERT(syntax != 0);
	DB_MULTIPLE_INIT(p_, &bulk_);
	gtk->setDbtFromThis(key_);
	ltk->setDbtFromThis(key2_);
}

int InequalityIndexCursor::first()
{
	int flags = 0;
	int err = 0;
	// For a RANGE we start with the greaterThanOperation_.
	switch (operation_ == DbWrapper::RANGE ? greaterThanOperation_ : operation_) {
	case DbWrapper::ALL:
	case DbWrapper::LTE:
	case DbWrapper::LTX: {
		// Unmarshal the index
		Index index;
		index.setFromPrefix(*(char*)key_.data);

		// Set the tmpKey_ to the structure prefix of the key
		tmpKey_.data = key_.data;
		tmpKey_.size = (u_int32_t)Key::structureKeyLength(index, key_);

		// Do a DB_SET_RANGE lookup on the prefix
		err = cursor_.get(tmpKey_, data_, DB_SET_RANGE);

		flags = DB_CURRENT;
		break;
	}
	case DbWrapper::GTX:
		// Skip the first matching duplicate keys.
		// no throw for NOTFOUND and KEYEMPTY
		err = cursor_.get(key_, data_, DB_SET);
		if(err == 0) {
			// save key structure info to avoid iterating
			// past end of entries for desired index
			DbtOut tmp(key_.data,key_.size);
			// Do the DB_NEXT_NODUP without the DB_MULTIPLE_KEY,
			// otherwise the multiple get will get all of it's keys
			// with the NODUP flag...
			err = cursor_.get(key_, data_, DB_NEXT_NODUP);
			if ((err == 0) && !isSameIndex(key_, tmp)) {
				done_ = true;
				return 0;
			}
			flags = DB_CURRENT;
		} else if(err == DB_NOTFOUND) {
			err = 0;
			flags = DB_SET_RANGE;
		}
		break;
	case DbWrapper::GTE:
		flags = DB_SET_RANGE;
		break;
	default:
		// This just won't happen.
		err = DB_NOTFOUND;
		DBXML_ASSERT(0);
		break;
	}
	if(err != 0) {
		if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
		return err;
	}

	err = nextEntries(flags | DB_MULTIPLE_KEY);
	if(err != 0 || done_) {
		return err;
	} else {
		DB_MULTIPLE_INIT(p_, &bulk_);
	}

	if(operation_ == DbWrapper::RANGE) {
		// From now on, behave like the less than operator
		operation_ = lessThanOperation_;
		key_.set(key2_.data, key2_.size);
	}

	return next();
}

int InequalityIndexCursor::next()
{
	if(done_) {
		return 0;
	}
	
	DB_MULTIPLE_KEY_NEXT(p_, &bulk_, tmpKey_.data, tmpKey_.size, data_.data, data_.size);
	while(p_ == 0) {
		int err = nextEntries(DB_NEXT | DB_MULTIPLE_KEY);
		if(err != 0 || done_) {
			return err;
		} else {
			DB_MULTIPLE_INIT(p_, &bulk_);			
		}
		DB_MULTIPLE_KEY_NEXT(p_, &bulk_, tmpKey_.data, tmpKey_.size, data_.data, data_.size);
	}

	switch (operation_) {
	case DbWrapper::ALL:
	case DbWrapper::GTX:
	case DbWrapper::GTE: {
		// Check the Prefix and VIDs are the same.
		if (!isSameIndex(key_, tmpKey_)) {
			done_ = true;
			return 0;
		}
		break;
	}
	case DbWrapper::LTX: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) >= 0) {
			done_ = true;
			return 0;			
		}
		break;
	}
	case DbWrapper::LTE: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) > 0) {
			done_ = true;
			return 0;			
		}
		break;
	}
	default:
		// This just won't happen.
		DBXML_ASSERT(0);
		break;
	}

	return 0;
}

// PrefixIndexCursor

PrefixIndexCursor::PrefixIndexCursor(IndexDatabase &db, Transaction *txn,
				     const Key *k1)
	: IndexCursor(db, txn)
{
	DB_MULTIPLE_INIT(p_, &bulk_);
	k1->setDbtFromThis(key_);
}

int PrefixIndexCursor::first()
{
	int err;
	err = nextEntries(DB_SET_RANGE | DB_MULTIPLE_KEY);
	
	if(err != 0 || done_) {
		return err;
	} else {
		DB_MULTIPLE_INIT(p_, &bulk_);
		err = next();
	}
	return err;
}

int PrefixIndexCursor::next()
{
	if(done_) {
		return 0;
	}
	
	DB_MULTIPLE_KEY_NEXT(p_, &bulk_, tmpKey_.data, tmpKey_.size, data_.data, data_.size);
	while(p_ == 0) {
		int err = nextEntries(DB_NEXT | DB_MULTIPLE_KEY);
		if(err != 0 || done_) {
			return err;
		} else {
			DB_MULTIPLE_INIT(p_, &bulk_);
		}
		DB_MULTIPLE_KEY_NEXT(p_, &bulk_, tmpKey_.data, tmpKey_.size, data_.data, data_.size);
	}

	if (tmpKey_.size < key_.size ||
	    memcmp(key_.data, tmpKey_.data, key_.size) != 0) {
		done_ = true;
	}

	return 0;
}

// ReversePrefixIndexCursor

ReversePrefixIndexCursor::ReversePrefixIndexCursor(IndexDatabase &db,
						   Transaction *txn,
						   const Key *k1)
	: IndexCursor(db, txn, false)
{
	k1->setDbtFromThis(key_);
}

int ReversePrefixIndexCursor::first()
{
	return findLastPrefix(key_);
}

int ReversePrefixIndexCursor::next()
{
	if(done_) {
		return 0;
	}

	int err;
	err = prevEntry();
	if(err != 0 || done_) {
		return err;
	}

	if (tmpKey_.size < key_.size ||
	    memcmp(key_.data, tmpKey_.data, key_.size) != 0) {
		done_ = true;
	}

	return 0;
}

// EqualsIndexCursor

EqualsIndexCursor::EqualsIndexCursor(IndexDatabase &db, Transaction *txn,
				     const Key *k1)
	: IndexCursor(db, txn)
{
	DB_MULTIPLE_INIT(p_, &bulk_);
	k1->setDbtFromThis(key_);
}

int EqualsIndexCursor::first()
{
	int err = nextEntries(DB_SET | DB_MULTIPLE);
	if(err != 0 || done_) {
		return err;
	} else {
		DB_MULTIPLE_INIT(p_, &bulk_);
	}
	return next();
}

int EqualsIndexCursor::next()
{
	if (done_) {
		return 0;
	}
	DB_MULTIPLE_NEXT(p_, &bulk_, data_.data, data_.size);
	while (p_ == 0) {
		int err = nextEntries(DB_NEXT_DUP | DB_MULTIPLE);
		if(err != 0 || done_) {
			return err;
		} else {
			DB_MULTIPLE_INIT(p_, &bulk_);
		}
		DB_MULTIPLE_NEXT(p_, &bulk_, data_.data, data_.size);
	}

	return 0;
}

// ReverseInequalityIndexCursor

ReverseInequalityIndexCursor::ReverseInequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation operation,
	const Key *k1, const Syntax *syntax)
	: IndexCursor(db, txn, false),
	  syntax_(syntax),
	  operation_(operation),
	  greaterThanOperation_(DbWrapper::NONE),
	  lessThanOperation_(DbWrapper::NONE)
{
	DBXML_ASSERT(operation_ != DbWrapper::EQUALITY);
	DBXML_ASSERT(operation_ != DbWrapper::PREFIX);
	DBXML_ASSERT(operation_ != DbWrapper::RANGE);
	DBXML_ASSERT(operation_ != DbWrapper::NEG_NOT_EQUALITY);
	DBXML_ASSERT(syntax != 0);
	k1->setDbtFromThis(key_);
}

ReverseInequalityIndexCursor::ReverseInequalityIndexCursor(
	IndexDatabase &db, Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto, const Key *ltk,
	const Syntax *syntax)
	: IndexCursor(db, txn),
	  syntax_(syntax),
	  operation_(DbWrapper::RANGE),
	  greaterThanOperation_(gto),
	  lessThanOperation_(lto)
{
	DBXML_ASSERT(lto == DbWrapper::LTX || lto == DbWrapper::LTE);
	DBXML_ASSERT(gto == DbWrapper::GTX || gto == DbWrapper::GTE);
	DBXML_ASSERT(syntax != 0);

	gtk->setDbtFromThis(key_);
	ltk->setDbtFromThis(key2_);
}

int ReverseInequalityIndexCursor::first()
{
	int err = 0;
	DbtOut *key = &key_;
	DbWrapper::Operation op = operation_;
	// For a RANGE we start with the lessThanOperation_,
	// and key2_, which is the opposite of the forward cursor
	if (op == DbWrapper::RANGE) {
		op = lessThanOperation_;
		DBXML_ASSERT((op == DbWrapper::LTE) ||
			     (op == DbWrapper::LTX));
		key = &key2_;
	}
	switch (op) {
	case DbWrapper::ALL:
	case DbWrapper::LTE:
	case DbWrapper::LTX: {
		// this case covers both single values and ranges
		// find last matching key
		err = findLastValue(*key, (op == DbWrapper::LTX));
		break;
	}
	case DbWrapper::GTE:
	case DbWrapper::GTX: {
		// valid for single values only.
		// Unmarshal the index
		Index index;
		index.setFromPrefix(*(char*)key_.data);
		DbXmlDbt tkey;
		// Set the tkey to the structure prefix of the key
		tkey.data = key_.data;
		tkey.size = (u_int32_t)Key::structureKeyLength(index, key_);

		err = findLastPrefix(tkey);
		break;
	}
	default:
		// This just won't happen.
		err = DB_NOTFOUND;
		DBXML_ASSERT(0);
		break;
	}
	if (err != 0 || done_ == true) {
		if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
			err = 0;
		}
		done_ = true;
		return err;
	}

	if(operation_ == DbWrapper::RANGE) {
		// From now on, behave like the greater than operator
		operation_ = greaterThanOperation_;
	}

	return compare();
}

int ReverseInequalityIndexCursor::next()
{
	if(done_) {
		return 0;
	}

	int err = prevEntry();
	if(err != 0 || done_) {
		return err;
	}
	return compare();
}

int ReverseInequalityIndexCursor::compare()
{
		
	switch (operation_) {
	case DbWrapper::ALL:
	case DbWrapper::LTX:
	case DbWrapper::LTE: {
		// Check the Prefix and VIDs are the same.
		if (!isSameIndex(key_, tmpKey_)) {
			done_ = true;
			return 0;
		}
		break;
	}
	case DbWrapper::GTX: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) <= 0) {
			done_ = true;
			return 0;			
		}
		break;
	}
	case DbWrapper::GTE: {
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key_) < 0) {
			done_ = true;
			return 0;			
		}
		break;
	}
	default:
		// This just won't happen.
		DBXML_ASSERT(0);
		break;
	}

	return 0;
}

// find the last value that matches the key
int ReverseInequalityIndexCursor::findLastValue(DbXmlDbt &key, bool excl)
{
	int err;
	u_int32_t keySize = key.size;
	tmpKey_.data = key.data;
	tmpKey_.size = keySize;
	err = cursor_.get(tmpKey_, data_, DB_SET_RANGE);
	if (err != 0) {
		// no exact matches; find last entry that matches prefix
		Index index;
		index.setFromPrefix(*(char*)key.data);
		DbXmlDbt tkey;
		tkey.data = key.data;
		tkey.size = (u_int32_t)Key::structureKeyLength(index, key);
		err = findLastPrefix(tkey);
	} else {
		// if exact match, get to end of duplicates
		if(syntax_->get_bt_compare()(0, &tmpKey_, &key) == 0) {
			while ((err = cursor_.get(tmpKey_, data_,
						  DB_NEXT_DUP)) == 0) {
				// just iterate
			}
			// iterate back over match if doing LTX
			if (excl)
				cursor_.get(tmpKey_, data_, DB_PREV_NODUP);
		} else {
			// not a match, so we've got an entry beyond the
			// range; move back one
			cursor_.get(tmpKey_, data_, DB_PREV);
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// An equality index lookup is already in document
// order, so we perform the lookup directly on the
// index.
//

#define THROW_XMLEXCEPTION(err) { \
  XmlException ex((err)); \
  ex.setLocationInfo(location_); \
  throw ex; \
}

EqualsIndexIterator::EqualsIndexIterator(ContainerBase *container, IndexDatabase &db, Transaction *txn,
	const Key *k1, const LocationInfo *location)
	: IndexEntryIterator(container, location),
	  cursor_(db, txn, CURSOR_READ, "EqualsIndexIterator",
		  (db.isLocking() ? DB_READ_COMMITTED : 0)),
	  toDo_(true)
{
	if(cursor_.error() != 0) throw XmlException(cursor_.error());
	k1->setDbtFromThis(key_);
}

bool EqualsIndexIterator::next(DynamicContext *context)
{
	int err = 0;

	if(toDo_) {
		toDo_ = false;
		// Use DB_SET to get to the first index entry with
		// the given key
		err = cursor_.get(key_, data_, DB_SET);
	}
	else {
		// Use DB_NEXT_DUP to get the next duplicate key
		err = cursor_.get(key_, data_, DB_NEXT_DUP);
	}

	if(err != 0) {
		if(err == DB_NOTFOUND) return false;
		else { THROW_XMLEXCEPTION(err); }
	}

	// Unmarshal the index entry
	ie_->setThisFromDbt(data_);

	return true;
}

bool EqualsIndexIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(cont_->getContainerID() < container) return false;

	int err = 0;

	if(toDo_) {
		toDo_ = false;

		if(cont_->getContainerID() > container) {
			// seek() must always move the cursor forwards.
			err = cursor_.get(key_, data_, DB_SET);
		} else {
			// Use DB_GET_BOTH_RANGE to do a >= match on the key and the data
			IndexEntry::marshalLookupFormat(did, nid, data_);
			err = cursor_.get(key_, data_, DB_GET_BOTH_RANGE);
		}

		if(err != 0) {
			if(err == DB_NOTFOUND) return false;
			else { THROW_XMLEXCEPTION(err); }
		}

		// Unmarshal the index entry
		ie_->setThisFromDbt(data_);

	} else {
		// seek() must always move the cursor forwards. Therefore
		// we check to see if the seek target is before or equal to
		// DB_NEXT_DUP, and if it is we return it.

		err = cursor_.get(key_, data_, DB_NEXT_DUP);

		if(err != 0) {
			if(err == DB_NOTFOUND) return false;
			else { THROW_XMLEXCEPTION(err); }
		}

		IndexEntry::marshalLookupFormat(did, nid, tmp_);

		if(cont_->getContainerID() == container &&
		   index_duplicate_compare(0, &data_, &tmp_) < 0) {
			err = cursor_.get(key_, tmp_, DB_GET_BOTH_RANGE);

			if(err != 0) {
				if(err == DB_NOTFOUND) return false;
				else { THROW_XMLEXCEPTION(err); }
			}

			// Unmarshal the index entry
			// memory needs to be owned by data_
			data_.set(tmp_.data,
				  tmp_.size);
			ie_->setThisFromDbt(data_);
		} else {
			// Unmarshal the index entry
			ie_->setThisFromDbt(data_);
		}
	}

	return true;
}

void EqualsIndexIterator::adoptMemory()
{
	ie_->setMemory(data_.adopt_data());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EqualsDocumentIndexIterator::next(DynamicContext *context)
{
	if(toDo_) {
		if(!EqualsIndexIterator::next(context))
			return false;
	} else {
		do {
			if(!EqualsIndexIterator::next(context))
				return false;
		} while(ie_->getDocID() == docid_);
	}

	ie_->setFormat(IndexEntry::D_FORMAT);
	docid_ = ie_->getDocID();
	return true;
}

bool EqualsDocumentIndexIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(toDo_) {
		if(!EqualsIndexIterator::seek(container, did, nid, context))
			return false;
	} else {
		if(!EqualsIndexIterator::seek(container, did, nid, context))
			return false;
		while(ie_->getDocID() == docid_) {
			if(!EqualsIndexIterator::next(context))
				return false;
		}
	}

	ie_->setFormat(IndexEntry::D_FORMAT);
	docid_ = ie_->getDocID();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SortingIndexIterator takes the data from the IndexCursor and
// puts it into an in memory BTree, which sorts it into document
// order. The results returned are then read from this in memory
// sorted BTree.
//
// The in-memory BTree has the index entry as it's key (the data
// from the actual indexes), and has blank data.
//
// TBD Maybe share the sorted index results if it might be useful to reuse them? - jpcs
// TBD What page size shoud we use? - jpcs
//

SortingIndexIterator::SortingIndexIterator(ContainerBase *container, IndexCursor *inputCursor, const LocationInfo *l)
	: IndexEntryIterator(container, l),
	  inputCursor_(inputCursor),
	  manager_(container->getManager()),
	  toDo_(true)
{
}

void SortingIndexIterator::sortIndex(DynamicContext *context)
{
	// Iterate over the IndexCursor, adding the data to the
	// in memory BTree
	if (sorted_.isNull())
		sorted_ = manager_.createDocDatabase(index_duplicate_compare);
	
	Cursor sortedCursor(*sorted_.getDb(), 0, CURSOR_WRITE, 0);
	int err = inputCursor_->first();
	while(err == 0 && !inputCursor_->isDone()) {
		context->testInterrupt();

		sortedCursor.put(const_cast<DbXmlDbt&>(inputCursor_->getData()), data_, DB_KEYLAST);
		err = inputCursor_->next();
	}
	if(err != 0) THROW_XMLEXCEPTION(err);

	inputCursor_.set(0);

	// Delayed initialisation of cursor_, due to sorted_ not
	// being open before.
	if(cursor_.open(*sorted_.getDb(), 0, CURSOR_READ, 0) != 0) THROW_XMLEXCEPTION(cursor_.error());
}

bool SortingIndexIterator::next(DynamicContext *context)
{
	int err = 0;

	if(toDo_) {
		toDo_ = false;

		sortIndex(context);

		// Use DB_FIRST to get to the first index entry
		err = cursor_.get(key_, data_, DB_FIRST);
	}
	else {
		// Use DB_NEXT to get the next index entry
		err = cursor_.get(key_, data_, DB_NEXT);
	}

	if(err == DB_NOTFOUND) return 0;
	if(err != 0) THROW_XMLEXCEPTION(err);

	// Unmarshal the index entry
	ie_->setThisFromDbt(key_);

	return true;
}

bool SortingIndexIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(cont_->getContainerID() < container) return false;

	int err = 0;

	if(toDo_) {
		toDo_ = false;

		sortIndex(context);

		if(cont_->getContainerID() > container) {
			// seek() must always move the cursor forwards.
			err = cursor_.get(key_, data_, DB_FIRST);
		} else {
			// Use DB_SET_RANGE to do a >= match on the key
			IndexEntry::marshalLookupFormat(did, nid, key_);
			err = cursor_.get(key_, data_, DB_SET_RANGE);
		}

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		// Unmarshal the index entry
		ie_->setThisFromDbt(key_);

	} else {
		// seek() must always move the cursor forwards. Therefore
		// we check to see if the seek target is before or equal to
		// DB_NEXT, and if it is we return it.

		err = cursor_.get(key_, data_, DB_NEXT);

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		IndexEntry::marshalLookupFormat(did, nid, tmp_);

		if(cont_->getContainerID() == container &&
		   index_duplicate_compare(0, &key_, &tmp_) < 0) {
			err = cursor_.get(tmp_, data_, DB_SET_RANGE);

			if(err == DB_NOTFOUND) return false;
			if(err != 0) THROW_XMLEXCEPTION(err);

			// Unmarshal the index entry (memory needs to be
			// owned by key_
			key_.set(tmp_.data,
				 tmp_.size);
			ie_->setThisFromDbt(key_);
		} else {
			// Unmarshal the index entry
			ie_->setThisFromDbt(key_);
		}
	}

	return true;
}

void SortingIndexIterator::adoptMemory()
{
	ie_->setMemory(key_.adopt_data());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SortingDocumentIndexIterator::sortIndex(DynamicContext *context)
{
	// Iterate over the IndexCursor, adding the data to the
	// in memory BTree
	if (sorted_.isNull())
		sorted_ = manager_.createDocDatabase(index_duplicate_compare);

	Cursor sortedCursor(*sorted_.getDb(), 0, CURSOR_WRITE, 0);
	IndexEntry ie;
	int err = inputCursor_->first(ie);
	while(err == 0 && !inputCursor_->isDone()) {
		context->testInterrupt();

		// Set the key to just the document ID
		ie.setFormat(IndexEntry::D_FORMAT);
		ie.setDbtFromThis(key_);

		// Store it in the sorted BTree (data_ is empty)
		sortedCursor.put(key_, data_, DB_KEYLAST);

		err = inputCursor_->next(ie);
	}
	if(err != 0) THROW_XMLEXCEPTION(err);

	inputCursor_.set(0);

	// Delayed initialisation of cursor_, due to sorted_ not
	// being open before.
	if(cursor_.open(*sorted_.getDb(), 0, CURSOR_READ, 0) != 0) THROW_XMLEXCEPTION(cursor_.error());
}

