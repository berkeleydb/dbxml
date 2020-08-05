//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

/**
 * NsNid and NsNidGen
 *
 * Node ID and accessors and node id generation.
 *
 * This includes both transient and persistent formats of
 * this object, encapsulating its format, allowing for
 * possible changes
 *
 * See NsNid.cpp for a description of the format
 */
#ifndef __DBXMLNSNID_HPP
#define __DBXMLNSNID_HPP

#include "NsTypes.hpp"
#include <dbxml/XmlPortability.hpp>
#include <string.h>
#include <iostream>

#define NIDBUF_SIZE 50
#define NID_BYTES_SIZE 5
#define NID_ROOT_SIZE 1
#define NS_ID_ZERO 0x01
#define NS_ID_FIRST 0x02
#define NS_ID_LAST 0xFF
#define NS_METADATA_ID (NS_ID_FIRST + 0) // document metadata
#define NS_ROOT_ID (NS_ID_FIRST + 2) // the document node

/*
 * Invariant:
 *  If length is > NID_BYTES_SIZE, nid is in nidStore.nidPtr,
 *  and if it is <= NID_BYTES_SIZE, it's in nidStore.nidBytes.
 *  Allocation (isAlloced) is distinct from this, because unmarshaled
 *  node ids may be > NID_BYTES_SIZE, but not require deallocation,
 *  because the storage is from the node buffer.
 */
extern "C" {
int nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2);
};

namespace DbXml
{
static const unsigned char rootNid[3] = {1, NS_ROOT_ID, 0};
static const unsigned char metadataNid[3] = {1, NS_METADATA_ID, 0};	

class NsNidGen;
class NsFullNid;
	
#define NID_ALLOC_BIT 0x10000000
#define NID_ALLOC_MASK ~(0x10000000)

class NsNid {
public:
	NsNid() : nid_(0) {}
	NsNid(const unsigned char *nid) : nid_(nid) {}
	NsNid &operator=(const unsigned char *nid) {
		nid_ = nid;
		return *this;
	}

	NsNid &operator=(const NsNid &other) {
		nid_ = other.nid_;
		return *this;
	}

	NsNid(const NsFullNid *fullnid);
	
	bool isNull() const { return (nid_ == 0); }
	operator const unsigned char *() const { return nid_; }
	operator const char *() const { return (const char *)nid_; }
	operator xmlbyte_t *() const { return (xmlbyte_t*)nid_; }
	const xmlbyte_t *getBytes() const { return nid_; }
	int getBytesBeforeDecimal() const {
		if (nid_) return nid_[0]; else return -1; }
	uint32_t getLen() const {
		DBXML_ASSERT(nid_);
		// start with ptr at the "decimal point", which will
		// be the end of most nids.
		const unsigned char *ptr = &nid_[((int)*nid_) + 1];
		while (*ptr++);
		return (uint32_t)(ptr - nid_);
	}
	int compareNids(const NsNid &other) const {
		return compare(nid_, other.nid_);
	}
	bool operator==(const NsNid &other) const {
		return (compareNids(other) == 0);
	}
	bool operator!=(const NsNid &other) const {
		return (compareNids(other) != 0);
	}
	operator bool() const {
		return (nid_ != 0);
	}

	void displayNid(std::ostream &out) const {
		displayNid(out, (const char *)nid_,
			   (uint32_t)::strlen((const char *)nid_));
	}

	bool isDocRootNid() const;
	bool isMetaDataNid() const;

	// static methods
	static void initDocRoot() {
		docRootNid = rootNid;
		docMetaDataNid = metadataNid;
	}
	static void displayNid(std::ostream &out, const char *buf, uint32_t len);
	static const NsNid *getRootNid() {
		return const_cast<NsNid*>(&docRootNid);
	}
	static const NsNid *getMetaDataNid() {
		return const_cast<NsNid*>(&docMetaDataNid);
	}
	static int compare(const unsigned char *n1,
			   const unsigned char *n2);
	
protected:
	const unsigned char *nid_;
	static NsNid docRootNid;
	static NsNid docMetaDataNid;
};
	
class NsFullNid {
private:
	// the state of a node id
	union nidStore_u {
		xmlbyte_t *nidPtr;
		xmlbyte_t nidBytes[NID_BYTES_SIZE];
	} nidStore;
	uint32_t nidLen;
	
	friend class NsNidGen;

public:
	// basic accessors
	bool isNull() const { return (getLen() == 0); }
	uint32_t getLen() const { return (nidLen & NID_ALLOC_MASK); }
	const xmlbyte_t *getBytes() const {
		if (getLen() > NID_BYTES_SIZE)
			return nidStore.nidPtr;
		return nidStore.nidBytes;
	}

	// next few are necessary for marshaling/unmarshaling
	xmlbyte_t *getBytesForCopy() {
		if (getLen() > NID_BYTES_SIZE)
			return nidStore.nidPtr;
		return nidStore.nidBytes;
	}
	
	int getBytesBeforeDecimal() const {
		const xmlbyte_t *bytes = getBytes();
		if (bytes) return bytes[0]; else return -1;
	}
	
	void setLen(uint32_t len, bool alloced) {
		nidLen = len;
		if(alloced) nidLen |= NID_ALLOC_BIT;
	}
	void setLen(uint32_t len) {
		if (!isAlloced()) nidLen = len;
		else setLenAlloc(len);
	}
	void setLenAlloc(uint32_t len) { nidLen = (len | NID_ALLOC_BIT); }
	void setPtr(xmlbyte_t *ptr) { nidStore.nidPtr = ptr; }
	bool isAlloced() const { return ((nidLen&NID_ALLOC_BIT) != 0); }

	// instance methods
	int compareNids(const NsFullNid *other) const {
		return NsNid::compare(getBytes(), other->getBytes());
	}
	void freeNid() {
		if (isAlloced())
			::free((void*)nidStore.nidPtr);
		nidLen = 0;
	}
	void copyNid(const NsFullNid *from);
	void copyNid(const xmlbyte_t *ptr, uint32_t len);
	void displayNid(std::ostream &out) const {
		NsNid::displayNid(out, (const char *)getBytes(), getLen() - 1);
	}

	void setDocRootNid();
	void setSpecialNid(xmlbyte_t id);

	bool isDocRootNid() const;
	bool isMetaDataNid() const;
	
	bool operator==(const NsFullNid &other) const {
		return (compareNids(&other) == 0);
	}
	bool operator!=(const NsFullNid &other) const {
		return (compareNids(&other) != 0);
	}
	void clear() {
		memset(this, 0, sizeof(NsFullNid));
	}
	operator bool() const {
		return !isNull();
	}
public:
	// static class methods
	static uint32_t nidStaticSize() { return NID_BYTES_SIZE; }
	static int compare(const unsigned char *n1, const unsigned char *n2) {
		return ::strcmp((const char *)n1, (const char *)n2);
	}
	static int getNidOverhead() {
		// protocol byte, 2 id bytes, null byte
		return 4;
	}
	static const NsFullNid *getRootNid() {
		return const_cast<const NsFullNid*>(&docRootFullNid);
	}
	static void initDocRootNid() {
		docRootFullNid.setDocRootNid();
		NsNid::initDocRoot();
	}
	static void initNid(xmlbyte_t *buf, xmlbyte_t id);
private:
	static NsFullNid docRootFullNid;
};

class DBXML_EXPORT NsNidGen {
public:
	NsNidGen();
	virtual ~NsNidGen() {}
	virtual void nextId(NsFullNid *id);
protected:
	static void _nextId(unsigned char *id, uint32_t &digit);
private:
	// id generation
	uint32_t _idDigit;
	uint8_t _idBuf[NIDBUF_SIZE];
};

// wrapper class for NsNid that extends it and
// copies the bytes for safe keeping
class NsNidWrap : public NsNid {
public:
	NsNidWrap() {}
	NsNidWrap(const NsNid &nid) {
		set((const unsigned char *)nid);
	}
	NsNidWrap &operator=(const NsNidWrap &nid) {
		set(nid);
		return *this;
	}
	~NsNidWrap() {
		clear();
	}
	void copy(const NsNid &nid) {
		set((const unsigned char *)nid);
	}
	uint32_t set(const unsigned char *nid);
	void clear();
};

// a class to add full-blown class semantics to NsFullNid
class NsFullNidWrap {
public:
	NsFullNidWrap() {
		nid_.clear();
	}
	NsFullNidWrap(const NsFullNid *fullNid) {
		nid_.clear();
		*this = fullNid;
	}
	NsFullNidWrap(const NsNid &nid) {
		nid_.clear();
		*this = nid;
	}
	~NsFullNidWrap() {
		nid_.freeNid();
	}
	NsFullNidWrap &operator=(const NsNid &nid) {
		if (!nid.isNull())
			nid_.copyNid(nid.getBytes(), nid.getLen());
		else
			nid_.clear();
		return *this;
	}
	NsFullNidWrap &operator=(const NsFullNid *fullNid) {
		nid_.copyNid(fullNid);
		return *this;
	}
	NsFullNid *operator->() {
		return &nid_;
	}
	NsFullNid *get() {
		return &nid_;
	}
	NsFullNid &operator*() {
		return nid_;
	}
	NsNid nid() {
		return NsNid(&nid_);
	}
	operator bool() const {
		return nid_;
	}
private:
	NsFullNid nid_;
};

}

#endif
