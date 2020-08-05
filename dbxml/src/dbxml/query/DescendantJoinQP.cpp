//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DescendantJoinQP.hpp"
#include "QueryExecutionContext.hpp"
#include "SequentialScanQP.hpp"
#include "UnionQP.hpp"
#include "IntersectQP.hpp"
#include "StepQP.hpp"
#include "ExceptQP.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

DescendantJoinQP::DescendantJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(DESCENDANT, l, r, flags, mm)
{
}

NodeIterator *DescendantJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new DescendantIterator(/*orSelf*/false, li.adopt(), ri.adopt(), this);
}

void DescendantJoinQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	if(containsAllDocumentNodes(left_)) {
		ImpliedSchemaNode::Type rType = findType(right_);
		if(rType != (ImpliedSchemaNode::Type)-1 && rType != ImpliedSchemaNode::METADATA) {
			// Any node except a document node is a descendant of all document nodes
			logTransformation(opt.getLog(), "Redundant descendant", this, right_);
			alternatives.push_back(right_->copy(mm));
		}
	}

	StructuralJoinQP::applyConversionRules(maxAlternatives, opt, alternatives);
}

QueryPlan *DescendantJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) DescendantJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void DescendantJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string DescendantJoinQP::getLongName() const
{
	return "DescendantJoinQP";
}

string DescendantJoinQP::getShortName() const
{
	return "d";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DescendantOrSelfJoinQP::DescendantOrSelfJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(DESCENDANT_OR_SELF, l, r, flags, mm)
{
}

void DescendantOrSelfJoinQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	if(containsAllDocumentNodes(left_)) {
		// Any node is a descendant-or-self of all document nodes
		logTransformation(opt.getLog(), "Redundant descendant-or-self", this, right_);
		alternatives.push_back(right_->copy(mm));
	}

	StructuralJoinQP::applyConversionRules(maxAlternatives, opt, alternatives);
}

QueryPlan *DescendantOrSelfJoinQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	QueryPlan *qp = StructuralJoinQP::optimize(opt);
	if(qp != this) return qp;

	if(findType(left_) == ImpliedSchemaNode::METADATA) {
		switch(right_->getType()) {
		case DESCENDANT_OR_SELF: {
			DescendantOrSelfJoinQP *rsj = (DescendantOrSelfJoinQP*)right_;
			if(findType(rsj->left_) == ImpliedSchemaNode::METADATA) {
				string before = logBefore(this);

				left_ = new (mm) IntersectQP(left_, rsj->left_, 0, mm);
				left_->setLocationInfo(rsj);
				right_ = rsj->right_;
				flags_ |= rsj->flags_;

				logTransformation(opt.getLog(), "Combine document join", before, this);
				return optimize(opt);
			}
			break;
		}
		default: {
			if(findType(right_) == ImpliedSchemaNode::METADATA) {
				string before = logBefore(this);

				QueryPlan *result = new (mm) IntersectQP(left_, right_, flags_, mm);
				result->setLocationInfo(this);

				logTransformation(opt.getLog(), "Combine document join", this, result);
				return result->optimize(opt);
			}
			break;
		}
		}
	}

	if(opt.getPhase() < OptimizationContext::REMOVE_REDUNDENTS)
		return this;

	if(findType(left_) == ImpliedSchemaNode::METADATA) {
		switch(right_->getType()) {
		case STEP: {
			string before = logBefore(this);

			StepQP *step = (StepQP*)right_;
			right_ = step->getArg();
			step->setArg(this);

			logTransformation(opt.getLog(), "Push back document join", before, step);
			return step->optimize(opt);
		}
		case DESCENDANT:
		case DESCENDANT_OR_SELF:
		case ATTRIBUTE:
		case CHILD:
		case ATTRIBUTE_OR_CHILD: {
			string before = logBefore(this);

			StructuralJoinQP *sj = (StructuralJoinQP*)right_;
			right_ = sj->getLeftArg();
			sj->setLeftArg(this);

			logTransformation(opt.getLog(), "Push back document join", before, sj);
			return sj->optimize(opt);
		}
		case ANCESTOR:
		case ANCESTOR_OR_SELF:
		case PARENT:
		case PARENT_OF_ATTRIBUTE:
		case PARENT_OF_CHILD: {
			string before = logBefore(this);

			StructuralJoinQP *sj = (StructuralJoinQP*)right_;
			right_ = sj->getRightArg();
			sj->setRightArg(this);

			logTransformation(opt.getLog(), "Push back document join", before, sj);
			return sj->optimize(opt);
		}
		case EXCEPT: {
			string before = logBefore(this);

			// Add to both arguments of ExceptQP

			ExceptQP *ex = (ExceptQP*)right_;
			right_ = ex->getLeftArg();
			ex->setLeftArg(this);

			QueryPlan *copy = new (mm) DescendantOrSelfJoinQP(left_->copy(mm), ex->getRightArg(), flags_, mm);
			copy->setLocationInfo(this);
			ex->setRightArg(copy);

			logTransformation(opt.getLog(), "Push back document join", before, ex);
			return ex->optimize(opt);
		}
		default: break;
		}
	}

	// TBD remove the need for QueryExecutionContext here - jpcs
	QueryExecutionContext qec(GET_CONFIGURATION(opt.getContext())->getQueryContext(),
		/*debugging*/false);
	qec.setContainerBase(opt.getContainerBase());
	qec.setDynamicContext(opt.getContext());

	// Remove this document index join if it's unlikely to be useful
	if(isDocumentIndex(left_, /*toBeRemoved*/true) && isSuitableForDocumentIndexComparison(right_)) {
		Cost rCost = right_->cost(opt.getOperationContext(), qec);
		Cost lCost = left_->cost(opt.getOperationContext(), qec);

		// TBD Calculate these constants? - jpcs
		static const double KEY_RATIO_THRESHOLD = 2.0;
		static const double KEYS_PER_PAGE_THRESHOLD = 2.0;

		if((lCost.keys / rCost.keys) > KEY_RATIO_THRESHOLD ||
			(lCost.keys / lCost.totalPages()) > KEYS_PER_PAGE_THRESHOLD) {

			logTransformation(opt.getLog(), "Remove document join", this, right_);
			right_->logCost(qec, rCost, 0);
			left_->logCost(qec, lCost, 0);
			return right_;
		}
	}

	return this;
}

NodeIterator *DescendantOrSelfJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new DescendantIterator(/*orSelf*/true, li.adopt(), ri.adopt(), this);
}

QueryPlan *DescendantOrSelfJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;

	QueryPlan *result = new (mm) DescendantOrSelfJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void DescendantOrSelfJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string DescendantOrSelfJoinQP::getLongName() const
{
	return "DescendantOrSelfJoinQP";
}

string DescendantOrSelfJoinQP::getShortName() const
{
	return "ds";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DescendantIterator::DescendantIterator(bool orSelf, NodeIterator *ancestors, NodeIterator *descendants,
	const LocationInfo *o)
	: ProxyIterator(o),
	  orSelf_(orSelf),
	  ancestors_(ancestors),
	  descendants_(descendants),
	  state_(INIT)
{
}

DescendantIterator::~DescendantIterator()
{
	delete ancestors_;
	delete descendants_;
}

bool DescendantIterator::next(DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!ancestors_->next(context)) break;
		if(!descendants_->seek(ancestors_, context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		if(!descendants_->next(context)) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool DescendantIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!descendants_->seek(container, did, nid, context)) break;
		if(!ancestors_->next(context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		if(!descendants_->seek(container, did, nid, context)) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool DescendantIterator::doJoin(DynamicContext *context)
{
	while(true) {
		context->testInterrupt();

		int cmp = isDescendantOf(descendants_, ancestors_, orSelf_);
		if(cmp < 0) {
			if(!descendants_->seek(ancestors_, context)) break;
		} else if(cmp > 0) {
			if(NodeInfo::isSameDocument(descendants_, ancestors_) > 0) {
				if(!ancestors_->seek(descendants_->getContainerID(), descendants_->getDocID(),
					   *NsNid::getRootNid(), context)) break;
			} else {
				NsNidWrap nr(ancestors_->getLastDescendantID());
				if(!ancestors_->seek(ancestors_->getContainerID(), ancestors_->getDocID(),
					   nr, context)) break;
			}
		} else {
			result_ = descendants_;
			return true;
		}
	}

	state_ = DONE;
	return false;
}

