//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

/**
 * NsFullNid.cpp
 *
 * Node ID and accessors and nid generation
 *
 * This includes both transient and persistent formats of
 * this object, encapsulating its format, allowing for
 * possible changes
 */

#include "NsNid.hpp"
#include "NsConstants.hpp"
#include "NsUtil.hpp"
#include "NsDom.hpp" // for NsDomInsertType enum
#include <sstream>

XERCES_CPP_NAMESPACE_USE
using namespace DbXml;

/**
 * Node Id Format
 *
 * Node Ids are stored as null-terminated arrays of bytes,
 * with valid values as determined by the NS_ID_* constants
 * defined below.  Collation order is simple string collation.
 *
 * Each ID is of the form:
 *  byte 0:  number of bytes before the "decimal" point (n)
 *  bytes 1-(n-1): the primary node id
 *  bytes n-end_of_string: bytes after the "decimal" point,
 *    allocated/used when inserting new node ids between other
 *    node ids.  An unmodified document will not have any of
 *    these
 *
 * For example, let's say that the valid characters are only a-z.
 * In this case, a newly-created document will have ids like this:
 * 1 a 0
 * 1 b 0
 * ...
 * 1 z 0
 * 2 aa 0
 * 2 aa 0
 * ...
 *
 * This format is designed for these requirements:
 *  1.  Be compact when there are no node insertions after creation
 *  2.  Allow efficient comparison
 *  3.  Allow efficient insertion of new node ids between existing nodes
 *
 * Here's an example or two of inserting new nodes, using the
 * same a-z range as above:
 * 1.  Between "1a0" and "1b0" ==> 1am
 * 2.  Between "1a0" and "1am" ==> 1ag
 * 3.  Between "1a0" and "1ab" ==> 1aam (cannot use "1aa")
 */

NsFullNid NsFullNid::docRootFullNid;
NsNid NsNid::docRootNid;
NsNid NsNid::docMetaDataNid;

#define NID_BETW_INITIAL_SIZE 4 // must be two or more
#define NID_INITIAL_DIGIT 0

#ifdef DEBUG
// Add conditional that can be turned on to test behavior
// of large node IDs.  This can be hard to trigger with content
// alone.  NEVER use this in production or even normal debug
// builds...
//#define USE_LARGE_NIDS 1
#endif

//
// Node Id Comparison function
//
// A simple lexicographical comparison, stopping at the first null.
//
extern "C" {
int
nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	// Assume null termination of node ids
	int res;
	const xmlbyte_t *p1 = (const xmlbyte_t *)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t *)dbt2->data;
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}
};

// NsFullNid methods

void
NsFullNid::setDocRootNid()
{
	freeNid();
	nidStore.nidBytes[0] = NID_ROOT_SIZE;
	nidStore.nidBytes[1] = NS_ROOT_ID;
	nidStore.nidBytes[2] = 0;
	nidLen = (uint32_t)3;
}

void
NsFullNid::setSpecialNid(xmlbyte_t id)
{
	freeNid();
	nidStore.nidBytes[0] = NID_ROOT_SIZE;
	nidStore.nidBytes[1] = id;
	nidStore.nidBytes[2] = 0;
	nidLen = 3;
}

bool
NsFullNid::isDocRootNid() const
{
	return ((getBytes()[1] == NS_ROOT_ID) &&
		(getLen() == 3));
}

bool
NsFullNid::isMetaDataNid() const
{
	return ((getBytes()[1] == NS_METADATA_ID) &&
		(getLen() == 3));
}

//static -- used only for one-byte ids -- metadata, etc
void
NsFullNid::initNid(xmlbyte_t *buf, xmlbyte_t id)
{
	buf[0] = 1;
	buf[1] = id;
	buf[2] = 0;
}

void
NsFullNid::copyNid(const NsFullNid *from)
{
	copyNid(from->getBytes(), from->getLen());
}

void
NsFullNid::copyNid(const xmlbyte_t *fromPtr, uint32_t newLen)
{
	// reuse existing memory if possible
	if (!newLen) {
		freeNid();
		return;
	}
	if (newLen > NID_BYTES_SIZE) {
		if (getLen() < newLen) {
			// cannot reuse
			freeNid();
			nidStore.nidPtr = (xmlbyte_t *)::malloc(newLen);
			if (!nidStore.nidPtr)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "copyNid failed to allocate memory",
							 __FILE__, __LINE__);
			setLenAlloc(newLen);
		} else {
			setLen(newLen); // maintains alloc'd status
		}
		memcpy(nidStore.nidPtr, fromPtr, newLen);
	} else {
		freeNid();
		memcpy(nidStore.nidBytes, fromPtr, newLen);
		setLen(newLen);
	}
	DBXML_ASSERT(getLen() == newLen);
}

// static
void NsNid::displayNid(std::ostream &out, const char *buf, uint32_t len) {
	// find out where the decimal point is
	uint32_t n = (uint32_t)buf[0];

	++buf;
	--len;
	out << (char)(n+0x30) << ".";
	for(uint32_t i = 0; i < len; ++i, ++buf) {
		if(i == n) out << ".";

		unsigned char b = *buf;
		int hn = (b & 0xF0) >> 4;
		int ln = b & 0x0F;
		out << (char)(hn < 10 ? '0' + hn : 'A' + hn - 10);
		out << (char)(ln < 10 ? '0' + ln : 'A' + ln - 10);
	}
}

// NsNidGen methods

NsNidGen::NsNidGen()
	: _idDigit(NID_INITIAL_DIGIT)
{
	memset(_idBuf, 0, NIDBUF_SIZE);
	_idBuf[1] = NS_ROOT_ID;
	_idBuf[0] = NID_ROOT_SIZE;
}

// static
void NsNidGen::_nextId(unsigned char *id, uint32_t &digit)
{
	unsigned char *dest = id + 1;
	int32_t i = digit;
	while (i >= 0 && (dest[i] == NS_ID_LAST)) {
		dest[i--] = NS_ID_FIRST;
	}
	if (i < 0) {
		++digit;
		dest[digit] = NS_ID_FIRST;
	} else
		dest[i] += 1;

	DBXML_ASSERT(dest[digit+1] == 0);
	// assign number of digits to leading byte
	*id = (unsigned char)(digit + 1);
}

void
NsNidGen::nextId(NsFullNid *id)
{
	if(id != 0) {
#ifdef USE_LARGE_NIDS
		// debugging code to artificially bump Nid size
		// for testing.  Turn it on at top of this file...
		if (_idDigit == 0 && _idBuf[0] == 1 &&
		    _idBuf[1] == 5) {
			// force a large size -- enough to allocate
			_idDigit = 4;
			_idBuf[1] = NS_ID_LAST;
			_idBuf[2] = NS_ID_LAST;
			_idBuf[3] = NS_ID_LAST;
			_idBuf[4] = NS_ID_LAST;
			_idBuf[5] = NS_ID_LAST;
			_idBuf[0] = 5;
		}
#endif
		xmlbyte_t *dest;
		uint32_t len = _idDigit + 3; // incl leading byte and trailing null
		// ensure enough space for the id
		if (len > NID_BYTES_SIZE) {
			dest = id->nidStore.nidPtr =
				(xmlbyte_t *) ::malloc(len);
			if (!dest)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					"nextId failed to allocate memory",
					__FILE__, __LINE__);
			id->setLenAlloc(len);
		} else {
			dest = id->nidStore.nidBytes;
			id->setLen(len);
		}
		// this should not be possible but just in case
		if (len >= NIDBUF_SIZE)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "nextId node id size too large");
		// copy the id
		memcpy(dest, _idBuf, len);
	}
	// Increment the id, for next time
	_nextId(_idBuf, _idDigit); 
}

NsNid::NsNid(const NsFullNid *fullnid) : nid_(0)
{
	if (fullnid)
		nid_ = fullnid->getBytes();
}

// returns:
// 0 if equal
// < 0 if "this" is less than other
// > 0 if "this" is greater than other
// static
int
NsNid::compare(const unsigned char *p1,
	       const unsigned char *p2)
{
	// Assume null termination of node ids
	int res = 0;
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}

bool
NsNid::isDocRootNid() const
{
	return (nid_ && (nid_[1] == NS_ROOT_ID) &&
		(nid_[2] == 0));
}

bool
NsNid::isMetaDataNid() const
{
	return (nid_ && (nid_[1] == NS_METADATA_ID) &&
		(nid_[2] == 0));
}


uint32_t NsNidWrap::set(const unsigned char *nid)
{
	clear();
	uint32_t len = 0;
	if ((const unsigned char *)nid) {
		len = (uint32_t)NsUtil::nsStringLen(nid) + 1;
		nid_ = (const unsigned char*) NsUtil::allocate(len);
		memcpy((void*)nid_, nid, len);
	}
	return len;
}

void NsNidWrap::clear()
{
	if (nid_) NsUtil::deallocate((void*)nid_);
}
