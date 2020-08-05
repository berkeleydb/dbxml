//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PARENTJOINQP_HPP
#define	__PARENTJOINQP_HPP

#include "ParentOfChildJoinQP.hpp"

namespace DbXml
{

class ParentJoinQP : public StructuralJoinQP
{
public:
	ParentJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class ParentIterator : public ParentOfChildIterator
{
public:
	ParentIterator(NodeIterator *children, NodeIterator *parents, const LocationInfo *o);

protected:
	virtual bool doJoin(DynamicContext *context);
};

}

#endif
