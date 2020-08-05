//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __BUFFERQP_HPP
#define	__BUFFERQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

#include <xqilla/runtime/ResultBuffer.hpp>

namespace DbXml
{

class BufferSource
{
public:
	virtual ~BufferSource() {}
	virtual NodeIterator *getBuffer(unsigned id) = 0;
};

class BufferQP : public QueryPlan
{
public:
	BufferQP(QueryPlan *parent, QueryPlan *arg, unsigned int bufferId, u_int32_t flags, XPath2MemoryManager *mm);

	QueryPlan *getParent() const { return parent_; }
	void setParent(QueryPlan *p) { parent_ = p; }

	QueryPlan *getArg() const { return arg_; }
	void setArg(QueryPlan *a) { arg_ = a; }

	unsigned int getBufferID() const { return bufferId_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;
	Cost getParentCost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	QueryPlan *parent_;
	QueryPlan *arg_;
	unsigned int bufferId_;

	mutable Cost parentCost_;
	mutable bool parentCostSet_;
};

class BufferReferenceQP : public QueryPlan
{
public:
	BufferReferenceQP(BufferQP *bqp, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(BUFFER_REF, flags, mm), bufferId_(bqp->getBufferID()), bqp_(bqp) {}

	unsigned int getBufferID() const { return bufferId_; }
	BufferQP *getBuffer() const { return bqp_; }
	void setBuffer(BufferQP *buf) { bqp_ = buf; }

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
	BufferReferenceQP(unsigned int bufferId, BufferQP *bqp, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(BUFFER_REF, flags, mm), bufferId_(bufferId), bqp_(bqp) {}

	unsigned int bufferId_;
	BufferQP *bqp_;
};

class BufferIterator : public ProxyIterator, public BufferSource
{
public:
	BufferIterator(const BufferQP *qp, DynamicContext *context);
	~BufferIterator();
	virtual NodeIterator *getBuffer(unsigned id);

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

private:
	const BufferQP *qp_;
	ResultBuffer buf_;
	BufferSource *parentSource_;
};

}

#endif
