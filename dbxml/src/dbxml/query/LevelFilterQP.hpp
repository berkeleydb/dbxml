//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LEVELFILTERQP_HPP
#define	__LEVELFILTERQP_HPP

#include "FilterQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

/**
 * Filters based on the level of the node
 */
class LevelFilterQP : public FilterQP
{
public:
	LevelFilterQP(QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class LevelFilter : public ProxyIterator
{
public:
	LevelFilter(NodeIterator *parent, const LocationInfo *location);
	~LevelFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
};

}

#endif
