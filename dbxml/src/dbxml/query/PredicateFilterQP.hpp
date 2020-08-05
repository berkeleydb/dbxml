//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PREDICATEFILTERQP_HPP
#define	__PREDICATEFILTERQP_HPP

#include "FilterQP.hpp"
#include "NodeIterator.hpp"

#include <xqilla/context/VariableStore.hpp>

namespace DbXml
{

/**
 * Filters based on the truth value of the ASTNode predicate
 */
class PredicateFilterQP : public FilterQP
{
public:
	PredicateFilterQP(QueryPlan *arg, ASTNode *pred, const XMLCh *uri, const XMLCh *name, u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *getPred() const { return pred_; }
	void setPred(ASTNode *pred) { pred_ = pred; }

	const XMLCh *getURI() const { return uri_; }
	const XMLCh *getName() const { return name_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	ASTNode *pred_;

	const XMLCh *uri_;
	const XMLCh *name_;
	StaticAnalysis varSrc_;
};

/**
 * Filters based on the truth value or reverse numeric position of the ASTNode predicate
 */
class NumericPredicateFilterQP : public FilterQP
{
public:
	NumericPredicateFilterQP(QueryPlan *arg, ASTNode *pred, bool reverse,
		u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *getPred() const { return pred_; }
	void setPred(ASTNode *pred) { pred_ = pred; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	ASTNode *pred_;
	bool reverse_;
};

class PredicateFilter : public DbXmlNodeIterator
{
public:
	PredicateFilter(NodeIterator *parent, const ASTNode *pred, const LocationInfo *location);
	~PredicateFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const ASTNode *pred_;
	bool toDo_;
};

class VarPredicateFilter : public DbXmlNodeIterator, public VariableStore
{
public:
	VarPredicateFilter(NodeIterator *parent, const ASTNode *pred, const XMLCh *uri, const XMLCh *name,
		const LocationInfo *location);
	~VarPredicateFilter();

	virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const;
	virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const;

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const ASTNode *pred_;
	const XMLCh *uri_;
	const XMLCh *name_;

	const VariableStore *scope_;
};

class NumericPredicateFilter : public DbXmlNodeIterator
{
public:
	NumericPredicateFilter(NodeIterator *parent, const ASTNode *pred, bool reverse,
		const LocationInfo *location);
	~NumericPredicateFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doNext(DynamicContext *context);

	NodeIterator *parent_;
	const ASTNode *pred_;
	bool toDo_;

	size_t contextPos_;
	size_t contextSize_;
	bool reverse_;
	Item::Ptr first_;
	Item::Ptr second_;
};

}

#endif
