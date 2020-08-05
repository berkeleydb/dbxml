//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DESCENDANTJOINQP_HPP
#define	__DESCENDANTJOINQP_HPP

#include "StructuralJoinQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class DescendantJoinQP : public StructuralJoinQP
{
public:
	DescendantJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class DescendantOrSelfJoinQP : public StructuralJoinQP
{
public:
	DescendantOrSelfJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class DescendantIterator : public ProxyIterator
{
public:
	DescendantIterator(bool orSelf, NodeIterator *ancestors, NodeIterator *descendants, const LocationInfo *o);
	~DescendantIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	bool orSelf_;

	NodeIterator *ancestors_;
	NodeIterator *descendants_;

	enum { INIT, RUNNING, DONE } state_;
};

}

#endif
