//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NODEHANDLEFUNCTION_HPP
#define	__NODEHANDLEFUNCTION_HPP

#include "DbXmlFunction.hpp"

#include <xqilla/runtime/SingleResult.hpp>

namespace DbXml
{

class Container;

class NodeToHandleFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	NodeToHandleFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	class NodeToHandleResult : public SingleResult
	{
	public:
		NodeToHandleResult(const NodeToHandleFunction *func);
		Item::Ptr getSingleResult(DynamicContext *context) const;
	private:
		const NodeToHandleFunction *func_;
	};
};

class HandleToNodeFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	HandleToNodeFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	Container *getContainerArg(DynamicContext *context, bool lookup) const;

private:
	class HandleToNodeResult : public SingleResult
	{
	public:
		HandleToNodeResult(const HandleToNodeFunction *func);
		Item::Ptr getSingleResult(DynamicContext *context) const;
	private:
		const HandleToNodeFunction *func_;
	};

	Container *container_;
};

}

#endif
