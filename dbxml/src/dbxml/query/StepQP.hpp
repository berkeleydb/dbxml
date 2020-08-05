//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __STEPQP_HPP
#define	__STEPQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"
#include "../dataItem/Join.hpp"

namespace DbXml
{

/**
 * Performs conventional navigation
 */
class StepQP : public QueryPlan
{
public:
	StepQP(QueryPlan *arg, Join::Type join, DbXmlNodeTest *nodeTest, ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm);

	void addPaths(const ImpliedSchemaNode::MVector &paths);
	void addPaths(const ImpliedSchemaNode::Vector &paths);

	QueryPlan *getArg() const { return arg_; }
	void setArg(QueryPlan *arg) { arg_ = arg; costSet_ = false; }

	Join::Type getJoinType() const { return joinType_; }
	void setJoinType(Join::Type j) { joinType_ = j; }
	DbXmlNodeTest *getNodeTest() const { return nodeTest_; }

	ContainerBase *getContainerBase() const { return container_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;
	virtual StructuralStats getStructuralStats(OperationContext &oc, StructuralStatsCache &cache,
		const NameID &nameID, bool ancestor) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	static Cost cost(ContainerBase *container, const QueryPlan *arg, const Cost &argCost, Join::Type join,
		const DbXmlNodeTest *nodeTest, OperationContext &oc, StructuralStatsCache &cache);
	static StructuralStats getStructuralStats(ContainerBase *container, const DbXmlNodeTest *nodeTest,
		OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor);
	static std::string toString(Join::Type joinType, const DbXmlNodeTest *nodeTest, const QueryPlan *arg,
		bool brief = true);
	static ContainerBase *findContainer(const QueryPlan *qp);
	static DbXmlNodeTest *findNodeTest(const QueryPlan *lookup);
	static DbXmlNodeTest *findNodeTest(const ImpliedSchemaNode *isn);
	static QueryPlan *createStep(QueryPlan *arg, Join::Type joinType, QueryPlan *lookup,
		u_int32_t flags, const LocationInfo *location, XPath2MemoryManager *mm);
	static QueryPlan *createStep(QueryPlan *arg, Join::Type joinType, ImpliedSchemaNode *isn,
		ImpliedSchemaNode *isn2, ContainerBase *cont, u_int32_t flags, const LocationInfo *location,
		XPath2MemoryManager *mm);

protected:
	void staticTypingImpl(StaticContext *context);
	static Cost getStepCost(ContainerBase *container, const QueryPlan *arg, Join::Type join, OperationContext &oc,
		StructuralStatsCache &cache, const NameID &descendantID);

	ImpliedSchemaNode::MVector paths_;

	ContainerBase *container_;
	QueryPlan *arg_;
	Join::Type joinType_;
	DbXmlNodeTest *nodeTest_;
	bool needsSort_;

	mutable Cost cost_;
	mutable bool costSet_;
};

class StepIterator : public DbXmlNodeIterator
{
public:
	StepIterator(NodeIterator *parent, const StepQP *qp);
	~StepIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	NodeIterator *parent_;
	const StepQP *qp_;
        Result result_;
};

class SortingStepIterator : public DbXmlNodeIterator
{
public:
	SortingStepIterator(NodeIterator *parent, const StepQP *qp);
	~SortingStepIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	NodeIterator *parent_;
	bool toDo_;
	const StepQP *qp_;
        Result sorted_;
};

class ElementChildAxisIterator : public NsNodeIterator
{
public:
	ElementChildAxisIterator(NodeIterator *parent, const DbXmlNodeTest *nodeTest, const LocationInfo *location);
	~ElementChildAxisIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

	virtual Type getType() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;
	virtual int32_t getNodeURIIndex();
	virtual const xmlbyte_t *getNodeName();
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);

protected:
	bool nextChild(DynamicContext *context);

	NodeIterator *parent_;
	const DbXmlNodeTest *nodeTest_;
	int32_t uriIndex_;

	DbXmlNodeImpl::Ptr nodeObj_;
	XmlDocument doc_;
	Cursor *cursor_;
	DbtOut key_;
	DbtOut data_;
	NsFullNid nextNid_;
};

}

#endif
