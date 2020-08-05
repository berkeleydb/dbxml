//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CHILDJOINQP_HPP
#define	__CHILDJOINQP_HPP

#include "DescendantJoinQP.hpp"

#include <vector>

namespace DbXml
{

class ChildJoinQP : public StructuralJoinQP
{
public:
	ChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class ChildIterator : public DescendantIterator
{
public:
	ChildIterator(NodeIterator *ancestors, NodeIterator *descendants, const LocationInfo *o);

protected:
	virtual bool doJoin(DynamicContext *context);

	std::vector<DbXmlNodeImpl::Ptr> ancestorStack_;
};

}

#endif
