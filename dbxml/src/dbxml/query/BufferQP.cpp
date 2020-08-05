//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "BufferQP.hpp"
#include "QueryPlanToAST.hpp"
#include "ASTToQueryPlan.hpp"
#include "ExceptQP.hpp"
#include "FilterQP.hpp"
#include "StructuralJoinQP.hpp"
#include "StepQP.hpp"
#include "DecisionPointQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../optimizer/NodeVisitingOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

BufferQP::BufferQP(QueryPlan *parent, QueryPlan *arg, unsigned int bufferId, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(BUFFER, flags, mm),
	  parent_(parent),
	  arg_(arg),
	  bufferId_(bufferId),
	  parentCost_(),
	  parentCostSet_(false)
{
}

/** Sets the BufferQP object on all it's BufferReferenceQP objects */
class BufferReferenceSetter : public NodeVisitingOptimizer
{
public:
	virtual void run(BufferQP *b)
	{
		buf = b;
		NodeVisitingOptimizer::optimizeQP(buf->getArg());
	}

private:
	virtual QueryPlan *optimizeBufferReference(BufferReferenceQP *item)
	{
		if(item->getBufferID() == buf->getBufferID()) {
			item->setBuffer(buf);
		}
		return item;
	}

	BufferQP *buf;
};

QueryPlan *BufferQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	parent_ = parent_->staticTyping(context, styper);
	parentCostSet_ = false;
	// Do not add the StaticAnalysis, as
	// that is handled by the BufferEndQP objects

	BufferReferenceSetter().run(this);

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());

	return this;
}

void BufferQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	parent_->staticTypingLite(context);
	parentCostSet_ = false;
	// Do not add the StaticAnalysis, as
	// that is handled by the BufferEndQP objects

	BufferReferenceSetter().run(this);

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
}

QueryPlan *BufferQP::optimize(OptimizationContext &opt)
{
	// Optimise the parent QueryPlan
	parent_ = parent_->optimize(opt);
	parentCostSet_ = false;

	BufferReferenceSetter().run(this);

	arg_ = arg_->optimize(opt);

	return this;
}

#define INLINE_BYTES_PER_KEY_THRESHOLD 1024
#define INLINE_SIZE_THRESHOLD 20

class QueryPlanSize : public NodeVisitingOptimizer
{
public:
	virtual unsigned int size(QueryPlan *item)
	{
		count_ = 0;
		NodeVisitingOptimizer::optimizeQP(item);
		return count_;
	}

private:
	virtual ASTNode *optimize(ASTNode *item)
	{
		count_ += INLINE_SIZE_THRESHOLD;
		return item;
	}

	virtual QueryPlan *optimizeQP(QueryPlan *item)
	{
		++count_;
		return NodeVisitingOptimizer::optimizeQP(item);
	}

	virtual QueryPlan *optimizeDecisionPoint(DecisionPointQP *item)
	{
		count_ += INLINE_SIZE_THRESHOLD;
		return item;
	}

	virtual QueryPlan *optimizeDecisionPointEnd(DecisionPointEndQP *item)
	{
		count_ += INLINE_SIZE_THRESHOLD;
		return item;
	}

	virtual QueryPlan *optimizeBuffer(BufferQP *item)
	{
		count_ += INLINE_SIZE_THRESHOLD;
		return item;
	}

	virtual QueryPlan *optimizeBufferReference(BufferReferenceQP *item)
	{
		count_ += INLINE_SIZE_THRESHOLD;
		return item;
	}

	unsigned int count_;
};

class BufferUseCount : public NodeVisitingOptimizer
{
public:
	virtual int count(unsigned int id, QueryPlan *item)
	{
		id_ = id;
		count_ = 0;
		NodeVisitingOptimizer::optimizeQP(item);
		return count_;
	}

private:
	virtual QueryPlan *optimizeBufferReference(BufferReferenceQP *item)
	{
		if(item->getBufferID() == id_) ++count_;
		return item;
	}

	unsigned int id_;
	int count_;
};

class BufferRemover : public NodeVisitingOptimizer
{
public:
	virtual QueryPlan *run(const BufferQP *b, XPath2MemoryManager *m)
	{
		buf = b;
		mm = m;
		return NodeVisitingOptimizer::optimizeQP(buf->getArg()->copy(mm));
	}

private:
	virtual QueryPlan *optimizeBufferReference(BufferReferenceQP *item)
	{
		if(item->getBufferID() == buf->getBufferID()) {
			item->release();
			return buf->getParent()->copy(mm);
		}
		return item;
	}

	const BufferQP *buf;
	XPath2MemoryManager *mm;
};

void BufferQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// This is deliberately performed on the original BufferQP object
	int useCount = BufferUseCount().count(bufferId_, arg_);
	if(useCount <= 1) {
		AutoRelease<QueryPlan> qp(BufferRemover().run(this, mm));
		qp->createCombinations(maxAlternatives, opt, combinations);
	} else {
		BufferQP *result = new (mm) BufferQP(parent_->chooseAlternative(opt, "buffer"), arg_->chooseAlternative(opt, "buffer"), bufferId_, flags_, mm);
		result->setLocationInfo(this);
		BufferReferenceSetter().run(result);
		combinations.push_back(result);
	}
}

void BufferQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	alternatives.push_back(this);

	int useCount = BufferUseCount().count(bufferId_, arg_);
	if(useCount <= 1 || QueryPlanSize().size(parent_) < INLINE_SIZE_THRESHOLD) {
		AutoRelease<QueryPlan> qp(BufferRemover().run(this, mm));
		qp->createAlternatives(maxAlternatives, opt, alternatives);
	}
}

NodeIterator *BufferQP::createNodeIterator(DynamicContext *context) const
{
	return new BufferIterator(this, context);
}

QueryPlan *BufferQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	// We deliberately don't copy the parentCost_ - it's only a cache
	BufferQP *result = new (mm) BufferQP(parent_->copy(mm), arg_->copy(mm), bufferId_, flags_, mm);
	result->setLocationInfo(this);
	BufferReferenceSetter().run(result);

	return result;
}

void BufferQP::release()
{
	parent_->release();
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

void BufferQP::findQueryPlanRoots(QPRSet &qprset) const
{
	parent_->findQueryPlanRoots(qprset);
	arg_->findQueryPlanRoots(qprset);
}

Cost BufferQP::getParentCost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(!parentCostSet_) {
		parentCost_ = parent_->cost(context, qec);
		parentCostSet_ = true;
	}
	return parentCost_;
}

Cost BufferQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	getParentCost(context, qec);

	Cost cost = arg_->cost(context, qec);
	cost.pagesOverhead += parentCost_.totalPages();

	// Add the cost of storing the buffered data
	cost.pagesOverhead += parentCost_.keys * INLINE_BYTES_PER_KEY_THRESHOLD;

	return cost;
}

bool BufferQP::isSubsetOf(const QueryPlan *o) const
{
	return arg_->isSubsetOf(o);
}

string BufferQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<BufferQP id=\"" << bufferId_ << "\">" << endl;
	s << parent_->printQueryPlan(context, indent + INDENT);
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << in << "</BufferQP>" << endl;

	return s.str();
}

string BufferQP::toString(bool brief) const
{
	ostringstream s;

	s << "BUF(";
	s << bufferId_ << ",";
	s << parent_->toString(brief) << ",";
	s << arg_->toString(brief);
	s << ")";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

NodeIterator *BufferReferenceQP::createNodeIterator(DynamicContext *context) const
{
	BufferSource *source = GET_CONFIGURATION(context)->getBufferSource();
	DBXML_ASSERT(source);
	return source->getBuffer(bufferId_);
}

QueryPlan *BufferReferenceQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	BufferReferenceQP *result = new (mm) BufferReferenceQP(bufferId_, bqp_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void BufferReferenceQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *BufferReferenceQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	_src.copy(bqp_->getParent()->getStaticAnalysis());

	return this;
}

void BufferReferenceQP::staticTypingLite(StaticContext *context)
{
	_src.clear();
	_src.copy(bqp_->getParent()->getStaticAnalysis());
}

QueryPlan *BufferReferenceQP::optimize(OptimizationContext &opt)
{
	return this;
}

void BufferReferenceQP::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost BufferReferenceQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost;
	cost.keys = bqp_->getParentCost(context, qec).keys;

	// Add the cost of retriving the buffered data
	cost.pagesForKeys = cost.keys * INLINE_BYTES_PER_KEY_THRESHOLD;

	return cost;
}

bool BufferReferenceQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == BUFFER_REF) {
		return bufferId_ == ((BufferReferenceQP*)o)->bufferId_;
	}

	return bqp_->getParent()->isSubsetOf(o);
}

string BufferReferenceQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<BufferReferenceQP id=\"" << bufferId_ << "\"/>" << endl;

	return s.str();
}

string BufferReferenceQP::toString(bool brief) const
{
	ostringstream s;

	s << "BR(" << bufferId_ << ")";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

BufferIterator::BufferIterator(const BufferQP *qp, DynamicContext *context)
	: ProxyIterator(qp),
	  qp_(qp),
	  // TBD Make the use count actually be the number of times that the buffer will be accessed - jpcs
// 	  buf_(new QueryPlanToASTResult(qp->getParent()->createNodeIterator(context), qp), qp->getUseCount()),
	  buf_(new QueryPlanToASTResult(qp->getParent()->createNodeIterator(context), qp)),
	  parentSource_(GET_CONFIGURATION(context)->getBufferSource())
{
	// TBD implement this using a temporary BTree, rather than ResultBuffer - jpcs

	AutoBufferSourceReset reset(GET_CONFIGURATION(context), this);
	result_ = qp->getArg()->createNodeIterator(context);
}

BufferIterator::~BufferIterator()
{
	if (result_)
		delete result_;
}

NodeIterator *BufferIterator::getBuffer(unsigned id)
{
	if(id == qp_->getBufferID())
		return new ASTToQueryPlanIterator(buf_.createResult(), location_);

	DBXML_ASSERT(parentSource_);
	return parentSource_->getBuffer(id);
}

bool BufferIterator::next(DynamicContext *context)
{
	AutoBufferSourceReset reset(GET_CONFIGURATION(context), this);
	return result_->next(context);
}

bool BufferIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	AutoBufferSourceReset reset(GET_CONFIGURATION(context), this);
	return result_->seek(container, did, nid, context);
}
