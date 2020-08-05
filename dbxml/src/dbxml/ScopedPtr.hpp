//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SCOPEDPTR_HPP
#define	__SCOPEDPTR_HPP

namespace DbXml
{

template<class T>
class DBXML_EXPORT ScopedPtr
{
public:
	ScopedPtr(T *p = 0)
		: p_(p) {}

	~ScopedPtr()
	{
		delete p_;
	}

	void reset(T *p = 0)
	{
		if(p_ != p) {
			delete p_;
			p_ = p;
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

private:
	ScopedPtr(const ScopedPtr<T> &o);
	ScopedPtr &operator=(const ScopedPtr<T> &o);

	T *p_;
};

}

#endif
