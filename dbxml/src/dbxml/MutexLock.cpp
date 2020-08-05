//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "MutexLock.hpp"
#include "Globals.hpp"
#include <xercesc/util/PlatformUtils.hpp>

#if _XERCES_VERSION >= 30000
#include <xercesc/util/XMLMutexMgr.hpp>
#endif

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

// it is safe to pass a NULL for mutex in which case
// this class is a big no-op.  This allows conditional
// locks
MutexLock::MutexLock(dbxml_mutex_t mutex)
	: toLock_(mutex)
{
	if (!MUTEX_IS_NULL(toLock_))
		lockMutex(toLock_);
}

MutexLock::~MutexLock()
{
	if (!MUTEX_IS_NULL(toLock_))
		unlockMutex(toLock_);
}

void MutexLock::unlock()
{
	if (!MUTEX_IS_NULL(toLock_))
		unlockMutex(toLock_);
}

void MutexLock::lock()
{
	if (!MUTEX_IS_NULL(toLock_))
		lockMutex(toLock_);
}

#if _XERCES_VERSION >= 30000
class ManageMutexes
{
public:
	ManageMutexes()
		: mm(),
		  manager(XMLPlatformUtils::makeMutexMgr(&mm))
	{
	}

	SimpleMemoryManager mm;
	XMLMutexMgr *manager;
};

static ManageMutexes gMutexMgr;

// static
dbxml_mutex_t MutexLock::createMutex()
{
	return (dbxml_mutex_t)gMutexMgr.manager->create(&gMutexMgr.mm);
}

// static
void MutexLock::destroyMutex(dbxml_mutex_t mutex)
{
	if (!MUTEX_IS_NULL(mutex))
		gMutexMgr.manager->destroy(mutex, &gMutexMgr.mm);
}

// static
void MutexLock::unlockMutex(dbxml_mutex_t mutex)
{
	gMutexMgr.manager->unlock(mutex);
}

// static
void MutexLock::lockMutex(dbxml_mutex_t mutex)
{
	gMutexMgr.manager->lock(mutex);
}

#else // _XERCES_VERSION < 30000

// static
dbxml_mutex_t MutexLock::createMutex()
{
        return (dbxml_mutex_t) XMLPlatformUtils::makeMutex(
                Globals::defaultMemoryManager);
}

// static
void MutexLock::destroyMutex(dbxml_mutex_t mutex)
{
        XMLPlatformUtils::closeMutex(mutex);
}

// static
void MutexLock::unlockMutex(dbxml_mutex_t mutex)
{
        XMLPlatformUtils::unlockMutex(mutex);
}

// static
void MutexLock::lockMutex(dbxml_mutex_t mutex)
{
        XMLPlatformUtils::lockMutex(mutex);
}

#endif
