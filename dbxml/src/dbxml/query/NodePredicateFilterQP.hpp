//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NODEPREDICATEFILTERQP_HPP
#define	__NODEPREDICATEFILTERQP_HPP

#include "FilterQP.hpp"
#include "NodeIterator.hpp"

#include <xqilla/context/VariableStore.hpp>

namespace DbXml
{

/**
 * Filters based on the truth value of the QueryPlan predicate
 */
class NodePredicateFilterQP : public FilterQP
{
public:
	NodePredicateFilterQP(QueryPlan *arg, QueryPlan *pred, const XMLCh *uri, const XMLCh *name,
		u_int32_t flags, XPath2MemoryManager *mm);

	QueryPlan *getPred() const { return pred_; }
	void setPred(QueryPlan *pred) { pred_ = pred; }

	const XMLCh *getURI() const { return uri_; }
	const XMLCh *getName() const { return name_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	QueryPlan *pred_;

	const XMLCh *uri_;
	const XMLCh *name_;
	StaticAnalysis varSrc_;
};

/**
 * Filters based on the truth value of the QueryPlan predicate
 */
class NegativeNodePredicateFilterQP : public FilterQP
{
public:
	NegativeNodePredicateFilterQP(QueryPlan *arg, QueryPlan *pred, const XMLCh *uri,
		const XMLCh *name, u_int32_t flags, XPath2MemoryManager *mm);

	QueryPlan *getPred() const { return pred_; }
	void setPred(QueryPlan *pred) { pred_ = pred; }

	const XMLCh *getURI() const { return uri_; }
	const XMLCh *getName() const { return name_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	QueryPlan *pred_;

	const XMLCh *uri_;
	const XMLCh *name_;
	StaticAnalysis varSrc_;
};

class NodePredicateFilter : public DbXmlNodeIterator
{
public:
	NodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const LocationInfo *location);
	~NodePredicateFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const QueryPlan *pred_;
	bool toDo_;
};

class VarNodePredicateFilter : public DbXmlNodeIterator, public VariableStore
{
public:
	VarNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const XMLCh *uri,
		const XMLCh *name, const LocationInfo *location);
	~VarNodePredicateFilter();

	virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const;
	virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const;

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const QueryPlan *pred_;
	const XMLCh *uri_;
	const XMLCh *name_;

	const VariableStore *scope_;
};

class NegativeNodePredicateFilter : public DbXmlNodeIterator
{
public:
	NegativeNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const LocationInfo *location);
	~NegativeNodePredicateFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const QueryPlan *pred_;
	bool toDo_;
};

class VarNegativeNodePredicateFilter : public DbXmlNodeIterator, public VariableStore
{
public:
	VarNegativeNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const XMLCh *uri,
		const XMLCh *name, const LocationInfo *location);
	~VarNegativeNodePredicateFilter();

	virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const;
	virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const;

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const QueryPlan *pred_;
	const XMLCh *uri_;
	const XMLCh *name_;

	const VariableStore *scope_;
};

}

#endif
