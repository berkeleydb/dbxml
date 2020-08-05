//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ATTRIBUTEORCHILDJOINQP_HPP
#define	__ATTRIBUTEORCHILDJOINQP_HPP

#include "DescendantJoinQP.hpp"

#include <vector>

namespace DbXml
{

class AttributeOrChildJoinQP : public StructuralJoinQP
{
public:
	AttributeOrChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class AttributeOrChildIterator : public DescendantIterator
{
public:
	AttributeOrChildIterator(NodeIterator *ancestors, NodeIterator *descendants, const LocationInfo *o);

protected:
	virtual bool doJoin(DynamicContext *context);

	std::vector<DbXmlNodeImpl::Ptr> ancestorStack_;
};

}

#endif
