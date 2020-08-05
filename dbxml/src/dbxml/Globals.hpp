//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __GLOBALS_HPP
#define	__GLOBALS_HPP

#include <string>
#include <map>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include "ReferenceCounted.hpp" // for MutexLock
#include "Counters.hpp"
#include "DbXmlXerces.hpp"

class DatatypeLookup;
class DocumentCache;

namespace DbXml
{

class SimpleMemoryManager : public XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager
{
public:
	SimpleMemoryManager() {}
	void *allocate(XercesSize size);
	void deallocate(void *p);
	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* getExceptionMemoryManager();
};

class Globals
{
public:
	/// Called by Manager::initialize()
	static void initialize(DB_ENV *env);
	/// Called by Manager::terminate()
	static void terminate();

	static const DatatypeLookup *getDatatypeLookup() {
		return datatypeLookup_;
	}

	static bool isInitialized() { return (refCount_ != 0); }
	//
	// Counters manipulation
	//
	static Counters *getCounters() {
		return counters_;
	}
	static void dumpCounters() {
		counters_->dump();
	}
	static void dumpCountersToStream(std::ostream &out) {
		counters_->dumpToStream(out);
	}
	static void incrementCounter(int i) {
		counters_->incr(i);
	}
	static void setCounter(int i, int value) {
		counters_->set(i, value);
	}
	static void resetCounters() {
		counters_->reset();
	}
	static int getCounter(int i) {
		return counters_->get(i);
	}

	/// The map of index names to values
	typedef std::map<std::string, unsigned long> NameToNumber;
	static NameToNumber *indexMap;

	static XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *defaultMemoryManager;

	/// Mutex for the globals
	static dbxml_mutex_t mutex_;
	static int refCount_; // how many Manager objects are alive

	static const XMLCh XMLCh_UTF8[]; // = "UTF-8"
	static const XMLCh XMLCh_dbxml_scheme[]; // = "dbxml"

	static bool isBigendian_;
private:
	static void initializeXmlPlatform();

	static DocumentCache *documentCache_;
	static DatatypeLookup *datatypeLookup_;
	static Counters *counters_;
};

}

#endif
