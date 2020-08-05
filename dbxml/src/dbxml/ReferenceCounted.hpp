//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __REFERENCECOUNTED_HPP
#define	__REFERENCECOUNTED_HPP

#include "MutexLock.hpp"
#include <dbxml/XmlPortability.hpp>

namespace DbXml
{

class DBXML_EXPORT ReferenceCounted
{
public:
	ReferenceCounted() : count_(0) {}
	virtual ~ReferenceCounted() {}
	void acquire() { ++count_; }
	void release() { if (--count_ == 0) delete this; }
	int count() const { return count_; }
private:
	int count_;
};

template<class T> class DBXML_EXPORT RefCountJanitor
{
public:
	RefCountJanitor(T *p = 0) : p_(p) {
		if(p_) ((ReferenceCounted*)p_)->acquire();
	}
	
	RefCountJanitor(const RefCountJanitor<T> &o) : p_(o.p_) {
		if(p_) ((ReferenceCounted*)p_)->acquire();
	}

	RefCountJanitor &operator=(const RefCountJanitor<T> &o) {
		if(p_ != o.p_) {
			if(p_) ((ReferenceCounted*)p_)->release();
			p_ = o.p_;
			if(p_) ((ReferenceCounted*)p_)->acquire();
		}
		return *this;
	}

	~RefCountJanitor() {
		if(p_) ((ReferenceCounted*)p_)->release();
	}

	void reset(T *p = 0) {
		if(p_ != p) {
			if(p_) ((ReferenceCounted*)p_)->release();
			p_ = p;
			if(p_) ((ReferenceCounted*)p_)->acquire();
		}
	}

	T &operator*() const {
		return *p_;
	}

	T *operator->() const {
		return p_;
	}
	
	T *get() const {
		return p_;
	}

	operator bool() const {
		return p_ != 0;
	}

	bool operator==(const RefCountJanitor<T> &o) {
		return (p_ == o.p_);
	}

	bool operator!=(const RefCountJanitor<T> &o) {
		return (p_ != o.p_);
	}

private:
	T *p_;
};

// ReferenceCountedProtected	
// a lock-protected instance of ReferenceCounted, that
// protects the count for free-threaded objects
// Make release and acquire virtual to allow objects
// finer control
class DBXML_EXPORT ReferenceCountedProtected
{
public:
	ReferenceCountedProtected();
	virtual ~ReferenceCountedProtected();
	virtual void acquire();
	virtual void release();
	void lock();
	void unlock();
protected:
	int count_;
	dbxml_mutex_t mutex_;
};


}

#endif

