//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlConfiguration.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlSequenceBuilder.hpp"
#include "DbXmlDocumentCacheImpl.hpp"
#include "DbXmlUpdateFactory.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include "../QueryContext.hpp"
#include "../Document.hpp"
#include "../query/BufferQP.hpp"

#include <sstream>
#include <string.h>

#include <xqilla/context/DynamicContext.hpp>

XERCES_CPP_NAMESPACE_USE;

using namespace DbXml;
using namespace std;

DbXmlConfiguration::DbXmlConfiguration(QueryContext &qc, Transaction *txn, CompileInfo *ci, DatatypeLookup *dl)
	: resolver_(qc.getManager(), txn),
	  datatypeLookup_(dl),
	  qc_(qc),
	  dp_(0),
	  dpi_(0),
	  buffers_(0),
	  frame_(0),
	  oc_(txn),
	  qec_(0),
	  minder_(0),
	  db_flags_(0),
	  pi_(0),
	  compileInfo_(ci)
{
}

DbXmlConfiguration::~DbXmlConfiguration()
{
}

DocumentCache *DbXmlConfiguration::createDocumentCache(MemoryManager *memMgr)
{
	return new (memMgr) DbXmlDocumentCacheImpl(memMgr);
}

SequenceBuilder *DbXmlConfiguration::createSequenceBuilder(const DynamicContext *context)
{
	return new (context->getMemoryManager()) DbXmlSequenceBuilder(context);
}

ItemFactory *DbXmlConfiguration::createItemFactory(DocumentCache *cache, MemoryManager *memMgr)
{
	if(datatypeLookup_) {
		return new (memMgr) DbXmlFactoryImpl(datatypeLookup_);
	} else {
		return new (memMgr) DbXmlFactoryImpl(cache, memMgr);
	}
}

UpdateFactory *DbXmlConfiguration::createUpdateFactory(MemoryManager *memMgr)
{
	return new (memMgr) DbXmlUpdateFactory();
}

URIResolver *DbXmlConfiguration::createDefaultURIResolver(MemoryManager *memMgr)
{
	// We deal with this below instead - jpcs
	return 0;
}

void DbXmlConfiguration::populateStaticContext(StaticContext *context)
{
	qc_.populateStaticContext(context);

	context->setMessageListener(this);
	context->setModuleResolver(&resolver_);
	context->setExternalFunctionResolver(&resolver_);
	context->setXMLEntityResolver(&resolver_);

	// DB XML doesn't support schema typed documents
	context->getDocumentCache()->setDoPSVI(false);
}

void DbXmlConfiguration::populateDynamicContext(DynamicContext *context)
{
	qc_.populateDynamicContext(context);

	context->setMessageListener(this);
	context->setDefaultURIResolver(&resolver_, /*adopt*/false);
	context->setXMLEntityResolver(&resolver_);
}

void DbXmlConfiguration::testInterrupt()
{
	qc_.testInterrupt();
}

void DbXmlConfiguration::warning(const XMLCh *message, const LocationInfo *location)
{
	ostringstream oss;
	oss << XMLChToUTF8(location->getFile()).str() << ":" << location->getLine() << ":" << location->getColumn()
	    << ": " << XMLChToUTF8(message).str();
	
	((Manager &)qc_.getManager()).log(Log::C_QUERY, Log::L_WARNING, oss.str());
}

void DbXmlConfiguration::trace(const XMLCh *label, const Sequence &sequence, const LocationInfo *location,
		const DynamicContext *context)
{
	ostringstream oss;
	oss << XMLChToUTF8(location->getFile()).str() << ":" << location->getLine() << ":" << location->getColumn()
	    << ": trace: " << XMLChToUTF8(label).str() << " ";

	size_t len = sequence.getLength();
	if(len == 1) {
		oss << XMLChToUTF8(sequence.first()->asString(context)).str();
	}
	else if(len > 1) {
		oss << "(";
		Sequence::const_iterator i = sequence.begin();
		Sequence::const_iterator end = sequence.end();
		while(i != end) {
			oss << XMLChToUTF8((*i)->asString(context)).str();
			if(++i != end) oss << ",";
		}
		oss << ")";
	}

	((Manager &)qc_.getManager()).log(Log::C_QUERY, Log::L_INFO, oss.str());
}

unsigned int DbXmlConfiguration::allocateBufferID()
{
	// Hold the mutex while doing this, since it could happen at runtime
	MutexLock lock(compileInfo_->mutex);
	return compileInfo_->bufferId++;
}

static void numToBuf(unsigned int n, XMLBuffer &buf)
{
  if(n >= 10) numToBuf(n / 10, buf);
  buf.append('0' + (n % 10));
}

const XMLCh *DbXmlConfiguration::allocateTempVarName(XPath2MemoryManager *mm)
{
        static XMLCh prefix[] = { '#', 't', 'm', 'p', 0 };

	// Hold the mutex while doing this, since it could happen at runtime
	MutexLock lock(compileInfo_->mutex);

        XMLBuffer buf(20, mm);
	buf.set(prefix);
	numToBuf(compileInfo_->tmpVarCounter, buf);

	++(compileInfo_->tmpVarCounter);
        return mm->getPooledString(buf.getRawBuffer());
}

const std::string DbXmlConfiguration::getDefaultCollection()
{
	return qc_.getDefaultCollection();
}

XmlManager &DbXmlConfiguration::getManager()
{
	return qc_.getManager();
}

void DbXmlConfiguration::getImpliedSchemaNodes(const Document *doc, ISNVector &result) const
{
	if(pi_ == 0 || pi_->override) return;

	ISNMap::iterator i = pi_->schemas.find(doc->getContainerID());
	if(i != pi_->schemas.end()) {
		ISNMapValue &mapValue = i->second;

		ISNVector::iterator j = mapValue.schemas.begin();
		for(; j != mapValue.schemas.end(); ++j) {
			result.push_back(*j);
		}

		if(!mapValue.docMap.empty()) {
			ISNStringMap::iterator k = mapValue.docMap.find(doc->getName());
			if(k != mapValue.docMap.end()) {
				j = k->second.begin();
				for(; j != k->second.end(); ++j) {
					result.push_back(*j);
				}
			}
		}
	}
}

void DbXmlConfiguration::addImpliedSchemaNode(int containerId, const char *docName, const ImpliedSchemaNode *isn)
{
	pi_->schemas[containerId].docMap[docName].push_back(isn);
}

void DbXmlConfiguration::addImpliedSchemaNode(int containerId, const ImpliedSchemaNode *isn)
{
	pi_->schemas[containerId].schemas.push_back(isn);
}

void DbXmlConfiguration::getImpliedSchemaNodes(const string &documentURI, ISNVector &result) const
{
	if(pi_ == 0 || pi_->override) return;

	ISNStringMap::iterator i = pi_->uriSchemas.find(documentURI);
	if(i != pi_->uriSchemas.end()) {
		ISNVector::iterator j = i->second.begin();
		for(; j != i->second.end(); ++j) {
			result.push_back(*j);
		}
	}
}

void DbXmlConfiguration::addImpliedSchemaNode(const string &documentURI, const ImpliedSchemaNode *isn)
{
	pi_->uriSchemas[documentURI].push_back(isn);
}

CompileInfo::CompileInfo()
	: mutex(MutexLock::createMutex()),
	  bufferId(0),
	  tmpVarCounter(0)
{
}

CompileInfo::~CompileInfo()
{
	MutexLock::destroyMutex(mutex);
}
