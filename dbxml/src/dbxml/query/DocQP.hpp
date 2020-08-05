//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DOCQP_HPP
#define	__DOCQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"
#include "QueryPlanHolder.hpp"
#include "FilterQP.hpp"

namespace DbXml
{

/**
 * Performs the function of the doc() function
 */
class DocQP : public QueryPlan, public QueryPlanRoot
{
public:
	DocQP(ASTNode *arg, ImpliedSchemaNode *isn, DynamicContext *context, u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *getArg() const { return arg_; }
	void setArg(ASTNode *arg) { arg_ = arg; }

	const XMLCh *getUriArg(DynamicContext *context) const;

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	DocQP(ASTNode *arg, ImpliedSchemaNode *isn, ContainerBase *cont, const char *docName, u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *arg_;
	ImpliedSchemaNode *isn_;
	const char *documentName_;
};

class DocIterator : public ProxyIterator
{
public:
	DocIterator(const DocQP *qp)
		: ProxyIterator(qp), qp_(qp), toDo_(true) {}
	~DocIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	void init(DynamicContext *context);

	const DocQP *qp_;
	bool toDo_;

	std::string documentName_;
	XmlContainer cont_;
};

/**
 * Raises an error if the argument does not return a result
 */
class DocExistsCheckQP : public FilterQP
{
public:
	DocExistsCheckQP(QueryPlan *arg, const char *documentName, u_int32_t flags, XPath2MemoryManager *mm);

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

private:
	const char *documentName_;
};

class DocExistsCheck : public ProxyIterator
{
public:
	DocExistsCheck(NodeIterator *parent, const char *documentName, const LocationInfo *location);
	~DocExistsCheck();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

private:
	const char *documentName_;
	bool seenOne_;
};

}

#endif
