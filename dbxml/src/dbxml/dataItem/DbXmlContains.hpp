//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLCONTAINS_HPP
#define	__DBXMLCONTAINS_HPP

#include "DbXmlFunction.hpp"

namespace DbXml
{

class DbXmlContainsFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	DbXmlContainsFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Sequence createSequence(DynamicContext* context, int flags=0) const;
};

}

#endif
