//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __COLLECTIONQP_HPP
#define	__COLLECTIONQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"
#include "QueryPlanHolder.hpp"

namespace DbXml
{

/**
 * Performs the function of the collection() function
 */
class CollectionQP : public QueryPlan, public QueryPlanRoot
{
public:
	CollectionQP(ASTNode *arg, ImpliedSchemaNode *isn, DynamicContext *context, u_int32_t flags, XPath2MemoryManager *mm);

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
	CollectionQP(ASTNode *arg, ImpliedSchemaNode *isn, ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *arg_;
	ImpliedSchemaNode *isn_;
};

class CollectionIterator : public ProxyIterator
{
public:
	CollectionIterator(const CollectionQP *qp)
		: ProxyIterator(qp), qp_(qp), toDo_(true) {}
	~CollectionIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	void init(DynamicContext *context);

	const CollectionQP *qp_;
	bool toDo_;

	XmlContainer cont_;
};

}

#endif
