//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LOOKUPINDEXFUNCTION_HPP
#define	__LOOKUPINDEXFUNCTION_HPP

#include "DbXmlFunction.hpp"
#include "../query/QueryPlanHolder.hpp"
#include "DbXmlASTNode.hpp"

#include <xqilla/framework/XPath2MemoryManager.hpp>

namespace DbXml
{

class LookupIndexFunction : public DbXmlFunction, public QueryPlanRoot
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	ContainerBase *getContainerArg(DynamicContext *context, bool lookup) const;
	const char *getURINameArg(unsigned int argNum, DynamicContext *context, bool lookup) const;
	virtual QueryPlan *createQueryPlan(DynamicContext *context, bool lookup) const;

	const char *getChildURIName() const { return childURIName_; }
	const char *getParentURIName() const { return parentURIName_; }
	bool isParentSet() const { return _args.size() == 3; }

protected:
	LookupIndexFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo,
		const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	class LookupIndexFunctionResult : public ResultImpl
	{
	public:
		LookupIndexFunctionResult(const LookupIndexFunction *func)
			: ResultImpl(func), func_(func), qp_(0), result_(0) {}
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const
		{
			return "lookupindexfunctionresult";
		}
	private:
		const LookupIndexFunction *func_;
		AutoRelease<QueryPlan> qp_;
		Result result_;
	};

	const char *childURIName_, *parentURIName_;
};

class LookupAttributeIndexFunction : public LookupIndexFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupAttributeIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual QueryPlan *createQueryPlan(DynamicContext *context, bool lookup) const;
};

class LookupMetaDataIndexFunction : public LookupIndexFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupMetaDataIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual QueryPlan *createQueryPlan(DynamicContext *context, bool lookup) const;
};

}

#endif
