//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <dbxml/XmlPortability.hpp>

#ifndef __SHAREDPTR_HPP
#define	__SHAREDPTR_HPP

namespace DbXml
{

template<class T>
class DBXML_EXPORT SharedPtr
{
public:
	SharedPtr(T *p = 0)
		: p_(p), count_(new unsigned int(1)) {}

	SharedPtr(const SharedPtr<T> &o)
		: p_(o.p_), count_(o.count_)
	{
		++*count_;
	}

	SharedPtr &operator=(const SharedPtr<T> &o)
	{
		if(p_ != o.p_) {
			if(--*count_ == 0) {
				delete p_;
				delete count_;
			}
			p_ = o.p_;
			count_ = o.count_;
			++*count_;
		}
		return *this;
	}

	~SharedPtr()
	{
		if(--*count_ == 0) {
			delete p_;
			delete count_;
		}
	}

	void reset(T *p = 0)
	{
		if(p_ != p) {
			if(--*count_ == 0) {
				delete p_;
				delete count_;
			}
			p_ = p;
			count_ = new unsigned int(1);
		}
	}

	T &operator*() const
	{
		return *p_;
	}

	T *operator->() const
	{
		return p_;
	}

	T *get() const
	{
		return p_;
	}

	operator bool() const
	{
		return p_ != 0;
	}

private:
	T *p_;
	unsigned int *count_;
};

}

#endif
