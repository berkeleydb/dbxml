//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __VALUEFILTERQP_HPP
#define	__VALUEFILTERQP_HPP

#include "FilterQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class ImpliedSchemaNode;

/**
 * Filters based on the value of the node.
 */
class ValueFilterQP : public FilterQP
{
public:
	ValueFilterQP(QueryPlan *arg, ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm);

	ImpliedSchemaNode *getImpliedSchemaNode() const { return isn_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	ImpliedSchemaNode *isn_;
	Collation *collation_;
};

class ValueFilter : public DbXmlNodeIterator
{
public:
	ValueFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location);
	~ValueFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context) = 0;

	NodeIterator *parent_;
	ImpliedSchemaNode *isn_;
};

class ValueCompareFilter : public ValueFilter
{
public:
	ValueCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation *collation,
		const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);

	Collation *collation_;
};

class GeneralCompareFilter : public ValueFilter
{
public:
	GeneralCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation *collation,
		const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);

	Collation *collation_;
};

class MetaDataGeneralCompareFilter : public ValueFilter
{
public:
	MetaDataGeneralCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation *collation,
		const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);

	Collation *collation_;
};

class ContainsFilter : public ValueFilter
{
public:
	ContainsFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);
};

class ContainsCDFilter : public ValueFilter
{
public:
	ContainsCDFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);
};

class StartsWithFilter : public ValueFilter
{
public:
	StartsWithFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);
};

class EndsWithFilter : public ValueFilter
{
public:
	EndsWithFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location);

protected:
	virtual bool doNext(DynamicContext *context);
};

}

#endif
