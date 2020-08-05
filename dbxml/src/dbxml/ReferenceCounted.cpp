//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "ReferenceCounted.hpp"
#include "Globals.hpp"

using namespace DbXml;

ReferenceCountedProtected::ReferenceCountedProtected()
	: count_(0),
	  mutex_(MutexLock::createMutex())
{}

ReferenceCountedProtected::~ReferenceCountedProtected()
{
	MutexLock::destroyMutex(mutex_);
}

void ReferenceCountedProtected::acquire()
{
	MutexLock lock(mutex_);
	++count_;
}

void ReferenceCountedProtected::lock()
{
	MutexLock::lockMutex(mutex_);
}

void ReferenceCountedProtected::unlock()
{
	MutexLock::unlockMutex(mutex_);
}

void ReferenceCountedProtected::release()
{
	int newValue;
	
	// Musn't hold the mutex lock when this class is deleted
	{
		MutexLock lock(mutex_);
		newValue = --count_;
	}
	
	if(newValue == 0) {
		delete this;
	}
}

