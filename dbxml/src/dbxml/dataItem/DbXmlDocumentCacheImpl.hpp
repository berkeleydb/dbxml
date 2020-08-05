//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLDOCUMENTCACHEIMPL_HPP
#define	__DBXMLDOCUMENTCACHEIMPL_HPP

#include <xqilla/schema/DocumentCacheImpl.hpp>

namespace DbXml
{

class DbXmlDocumentCacheImpl : public DocumentCacheImpl
{
public:
	DbXmlDocumentCacheImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
		: DocumentCacheImpl(memMgr) {}
	DbXmlDocumentCacheImpl(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *gramPool, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
		: DocumentCacheImpl(memMgr, gramPool, /*makeScanner*/true) {}

	virtual DocumentCache *createDerivedCache(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const;
};

}

#endif
