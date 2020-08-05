//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLDOCAVAILABLE_HPP
#define	__DBXMLDOCAVAILABLE_HPP

#include <xqilla/ast/XQFunction.hpp>

namespace DbXml
{

class ContainerBase;
class QueryPlan;
class ReferenceMinder;
class XmlManager;
class Transaction;

class DbXmlDocAvailable : public XQFunction
{
public:
	static const XMLCh name[];

	DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Sequence createSequence(DynamicContext* context, int flags=0) const;

private:
	const XMLCh *getUriArg(DynamicContext *context) const;
};

}

#endif
