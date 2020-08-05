//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __MUTEXLOCK_HPP
#define	__MUTEXLOCK_HPP

// internal type for mutex.  Note: this must be
// an atomic type (NULL is legal) for this code to work
// properly as-is
typedef void * dbxml_mutex_t;
#define MUTEX_IS_NULL(m) ((m) == (void*)0)

namespace DbXml
{

class MutexLock
{
public:
	MutexLock(dbxml_mutex_t mutex);
	~MutexLock();
	// allow unlock/relock, at cost of extra boolean
	void unlock();
	void lock();

	static dbxml_mutex_t createMutex();
	static void destroyMutex(dbxml_mutex_t mutex);
	static void lockMutex(dbxml_mutex_t mutex);
	static void unlockMutex(dbxml_mutex_t mutex);
private:
	MutexLock(const MutexLock&);
	MutexLock &operator=(const MutexLock&);
	
	dbxml_mutex_t toLock_;
};

}

#endif

