//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DECISIONPOINTQP_HPP
#define	__DECISIONPOINTQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"
#include "../MutexLock.hpp"

namespace DbXml
{

class ContainerIterator;
class DecisionPointIterator;
class ReferenceMinder;
class ContainerBase;

class DecisionPointSource
{
public:
	enum Type {
		QUERY_PLAN
	};

	DecisionPointSource(Type type, XPath2MemoryManager *mm) : type_(type), mm_(mm) {}
	virtual ~DecisionPointSource() {}

	Type getType() const { return type_; }

	virtual DecisionPointSource *copy(XPath2MemoryManager *mm) const = 0;
	virtual void release() = 0;

	virtual void staticTypingLite(StaticContext *context, StaticAnalysis &src) = 0;
	virtual void staticTyping(StaticContext *context, StaticTyper *styper, StaticAnalysis &src) = 0;
	virtual void optimize(OptimizationContext &opt) = 0;
	virtual DecisionPointSource *chooseAlternative(OptimizationContext &opt) const = 0;
	virtual ContainerIterator *createContainerIterator(DynamicContext *context) const = 0;

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const = 0;
	virtual std::string toString(bool brief = true) const = 0;

protected:
	Type type_;
	XPath2MemoryManager *mm_;
};

class DecisionPointQP : public QueryPlan
{
public:
	class ListItem
	{
	public:
		ListItem(ContainerBase *c, ListItem *n) : container(c), qp(0), next(n) {}

		ContainerBase *container;
		QueryPlan *qp;

		ListItem *next;
	};

	DecisionPointQP(QueryPlan *arg, DecisionPointSource *dps, u_int32_t flags,
		XPath2MemoryManager *mm);

	DecisionPointSource *getSource() const { return dps_; }
	void setSource(DecisionPointSource *s) { dps_ = s; }

	QueryPlan *getArg() const { return arg_; }
	void setArg(QueryPlan *a) { arg_ = a; }

	ListItem *getList() const { return qpList_; }

	bool isRemoved() const { return removed_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	static void justInTimeOptimize(QueryPlan *&qp, ContainerBase *container, DynamicContext *context);

private:
	DecisionPointQP(const DecisionPointQP *o, XPath2MemoryManager *mm);
	DecisionPointQP(const DecisionPointQP *o, OptimizationContext &opt, XPath2MemoryManager *mm);
	ListItem *justInTimeOptimize(int contID, DynamicContext *context);

	DecisionPointSource *dps_;
	QueryPlan *arg_;
	bool removed_;

	ListItem *qpList_;
	bool qpListDone_;
	ReferenceMinder *compileTimeMinder_;
	DynamicContext *compileTimeContext_;

	friend class DecisionPointIterator;
};

class QueryPlanDPSource : public DecisionPointSource
{
public:
	QueryPlanDPSource(QueryPlan *parent, XPath2MemoryManager *mm)
		: DecisionPointSource(QUERY_PLAN, mm), parent_(parent) {}

	QueryPlan *getParent() const { return parent_; }
	void setParent(QueryPlan *p) { parent_ = p; }

	virtual DecisionPointSource *copy(XPath2MemoryManager *mm) const;
	virtual void release();

	virtual void staticTypingLite(StaticContext *context, StaticAnalysis &src);
	virtual void staticTyping(StaticContext *context, StaticTyper *styper, StaticAnalysis &src);
	virtual void optimize(OptimizationContext &opt);
	virtual DecisionPointSource *chooseAlternative(OptimizationContext &opt) const;
	virtual ContainerIterator *createContainerIterator(DynamicContext *context) const;

	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	QueryPlan *parent_;
};

class DecisionPointEndQP : public QueryPlan
{
public:
	DecisionPointEndQP(QueryPlanDPSource *dps, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(DECISION_POINT_END, flags, mm), dps_(dps) {}

	void setDPSource(QueryPlanDPSource *dps) { dps_ = dps; }

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
	QueryPlanDPSource *dps_;
};

class ContainerIterator
{
public:
	virtual ~ContainerIterator() {}

	virtual bool nextContainerID(int &contID, DynamicContext *context) = 0;
	virtual bool seekContainerID(int &contID, int toSeek, DynamicContext *context) = 0;
};

class QueryPlanContainerIterator : public ContainerIterator
{
public:
	QueryPlanContainerIterator(NodeIterator *parent)
		: parent_(parent), parentMoved_(false), peekDone_(false), prevCont_(-1) {}
	~QueryPlanContainerIterator();

	NodeIterator *parentPeekNext(DynamicContext *context);
	NodeIterator *parentPeekSeek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
	void parentTakePeek();

	int getLastContainerID() const { return prevCont_; }

	virtual bool nextContainerID(int &contID, DynamicContext *context);
	virtual bool seekContainerID(int &contID, int toSeek, DynamicContext *context);

protected:
	NodeIterator *parent_;
	bool parentMoved_;
	bool peekDone_;
	int prevCont_;
};

class DecisionPointIterator : public ProxyIterator
{
public:
	DecisionPointIterator(ContainerIterator *contIt, const DecisionPointQP *dp)
		: ProxyIterator(dp), contIt_(contIt), dp_(dp), list_(dp->qpList_) {}
	~DecisionPointIterator();

	ContainerIterator *getContainerIterator() const { return contIt_; }

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	bool nextContainer(int contID, DynamicContext *context);

	ContainerIterator *contIt_;

	const DecisionPointQP *dp_;
	const DecisionPointQP::ListItem *list_;
};

class DecisionPointEndIterator : public ProxyIterator
{
public:
	DecisionPointEndIterator(QueryPlanContainerIterator *ci, const LocationInfo *location)
		: ProxyIterator(location), ci_(ci) {}

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

private:
	// We don't own this, and should not delete it
	QueryPlanContainerIterator *ci_;
};

}

#endif
