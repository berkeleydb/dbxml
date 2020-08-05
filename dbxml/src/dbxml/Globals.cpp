//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "Globals.hpp"
#include "Value.hpp"
#include "Log.hpp"
#include "IndexSpecification.hpp"
#include "SyntaxManager.hpp"
#include "QueryContext.hpp"
#include "nodeStore/NsNid.hpp"
#include <sstream>

#include <xercesc/util/PlatformUtils.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xercesc/internal/MemoryManagerImpl.hpp>
#include <xqilla/schema/AnyAtomicTypeDatatypeValidator.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/items/DatatypeLookup.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <math.h>
#define srandom srand
#endif

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

Globals::NameToNumber *Globals::indexMap = 0;

bool Globals::isBigendian_;
int Globals::refCount_ = 0;
dbxml_mutex_t Globals::mutex_ = 0;
DocumentCache *Globals::documentCache_ = 0;
DatatypeLookup *Globals::datatypeLookup_ = 0;

static SimpleMemoryManager staticMemoryManager;
MemoryManager *Globals::defaultMemoryManager = &staticMemoryManager;

static Counters staticCounters;
Counters *Globals::counters_ = &staticCounters;

static int bdb_major = DB_VERSION_MAJOR;
static int bdb_minor = DB_VERSION_MINOR;
static int bdb_patch = DB_VERSION_PATCH;

// make sure that we built against a version of DB that is
// compatible with the one being used.  Major+minor need to match.
static void checkDbVersion()
{
	int major, minor, patch;
	(void) db_version(&major, &minor, &patch);
	if (major != bdb_major || minor != bdb_minor) {
		char buf[512];
		sprintf(buf, "Mismatch of Berkeley DB libraries; BDB XML was compiled with %d.%d.%d and is running against %d.%d.%d, which is not allowed", bdb_major, bdb_minor, bdb_patch, major, minor, patch);
		throw XmlException(XmlException::INTERNAL_ERROR, buf);
	}
}

void *SimpleMemoryManager::allocate(XercesSize size)
{
	return malloc(size);
}

void SimpleMemoryManager::deallocate(void *p)
{
	free(p);
}

MemoryManager *SimpleMemoryManager::getExceptionMemoryManager()
{
  return XMLPlatformUtils::fgMemoryManager;
}


void Globals::initialize(DB_ENV *env)
{
	if (mutex_ == 0) {
		mutex_ = MutexLock::createMutex();
	}
	MutexLock lock(mutex_);

	if (refCount_++ == 0) {

		checkDbVersion();
		
		SyntaxManager::initSyntaxManager();

		// seed the random number function (used by name generator)
		srandom((unsigned int)time(0));

		// init Xerces and XQilla, and related structures
		initializeXmlPlatform();
		
		// initialise the map of index names to values
		indexMap = new NameToNumber;
		(*indexMap)["none"] = Index::NONE;
		(*indexMap)["unique"] = Index::UNIQUE_ON;
		(*indexMap)["node"] = Index::PATH_NODE;
		(*indexMap)["edge"] = Index::PATH_EDGE;
		(*indexMap)["element"] = Index::NODE_ELEMENT;
		(*indexMap)["attribute"] = Index::NODE_ATTRIBUTE;
		(*indexMap)["metadata"] = Index::NODE_METADATA;
		(*indexMap)["equality"] = Index::KEY_EQUALITY;
		(*indexMap)["presence"] = Index::KEY_PRESENCE;
		(*indexMap)["substring"] = Index::KEY_SUBSTRING;
		isBigendian_ = (env->is_bigendian()) ? true : false;
		// initialise the document root NID
		NsFullNid::initDocRootNid();
	}
}

void Globals::initializeXmlPlatform()
{
	try {
		// initialise XQilla and Xerces
		XQillaPlatformUtils::initialize();

		documentCache_ = new DocumentCacheImpl(defaultMemoryManager);
		datatypeLookup_ = new DatatypeLookup(documentCache_, defaultMemoryManager);
	}
	catch (const XMLException &e) {
		char *desc = XMLString::transcode(e.getMessage());
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			"Error during Xerces-C initialization: " +
			std::string(desc), __FILE__, __LINE__);
		XMLString::release(&desc);
	}
	catch (const XQillaException &e) {
		char *desc = XMLString::transcode(e.getString());
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			"Error during XQilla initialization: " +
			std::string(desc), __FILE__, __LINE__);
		XMLString::release(&desc);
	}
}

void Globals::terminate()
{
	{
		MutexLock lock(mutex_);
		if (--refCount_ == 0) {
			// delete the map of index names to values
			delete indexMap;
			indexMap = 0;

			delete datatypeLookup_;
			datatypeLookup_ = 0;
			delete documentCache_;
			documentCache_ = 0;

			// terminate XQilla and Xerces
			XQillaPlatformUtils::terminate();
			SyntaxManager::uninitSyntaxManager();
			// log counters
			if (Log::isLogEnabled(Log::C_MANAGER, Log::L_INFO)) {
				ostringstream oss;
				dumpCountersToStream(oss);
				Log::log(0, //env *
					 Log::C_MANAGER, Log::L_INFO,
					 oss.str().c_str());
			}
		}
	}
	// Don't close the mutex, as it can't be done in a thread safe fashion
}

const XMLCh Globals::XMLCh_UTF8[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_U, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chDash,
	XERCES_CPP_NAMESPACE_QUALIFIER chDigit_8, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh Globals::XMLCh_dbxml_scheme[] = {
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull
};
