//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlDocumentCacheImpl.hpp"

#include <xercesc/internal/IGXMLScanner.hpp>

XERCES_CPP_NAMESPACE_USE;

using namespace DbXml;
using namespace std;

DocumentCache *DbXmlDocumentCacheImpl::createDerivedCache(MemoryManager *memMgr) const
{
	// lock the grammar pool, so we can share it accross threads
	grammarResolver_->getGrammarPool()->lockPool();

	// Construct a new DbXmlDocumentCacheImpl, based on this one
	return new (memMgr) DbXmlDocumentCacheImpl(grammarResolver_->getGrammarPool(), memMgr);
}
