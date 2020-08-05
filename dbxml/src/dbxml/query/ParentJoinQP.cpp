//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ParentJoinQP.hpp"
#include "ParentOfAttributeJoinQP.hpp"
#include "ParentOfChildJoinQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../QueryContext.hpp"
#include "../Manager.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

ParentJoinQP::ParentJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(PARENT, l, r, flags, mm)
{
}

QueryPlan *ParentJoinQP::optimize(OptimizationContext &opt)
{
	QueryPlan *result = StructuralJoinQP::optimize(opt);
	if(result != this) return result;

	XPath2MemoryManager *mm = opt.getMemoryManager();

	ImpliedSchemaNode::Type type = findType(left_);
	if(type == ImpliedSchemaNode::ATTRIBUTE) {
		// Convert to a ParentOfAttributeJoinQP
		QueryPlan *result = new (mm) ParentOfAttributeJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation(opt.getLog(), "More specific join", this, result);
		return result->optimize(opt);
	}

	if(type != -1) {
		// Convert to a ParentOfChildJoinQP
		QueryPlan *result = new (mm) ParentOfChildJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation(opt.getLog(), "More specific join", this, result);
		return result->optimize(opt);
	}

	return this;
}

QueryPlan *ParentJoinQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	StructuralJoinQP::staticTyping(context, styper);

	XPath2MemoryManager *mm = context->getMemoryManager();

	if(left_->getStaticAnalysis().getStaticType().isType(StaticType::ATTRIBUTE_TYPE)) {
		// Convert to a ParentOfAttributeJoinQP
		QueryPlan *result = new (mm) ParentOfAttributeJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation((Manager&)GET_CONFIGURATION(context)->getManager(),
			"More specific join", this, result);
		return result->staticTyping(context, styper);
	}

	if(!left_->getStaticAnalysis().getStaticType().containsType(StaticType::ATTRIBUTE_TYPE)) {
		// Convert to a ParentOfChildJoinQP
		QueryPlan *result = new (mm) ParentOfChildJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation((Manager&)GET_CONFIGURATION(context)->getManager(),
			"More specific join", this, result);
		return result->staticTyping(context, styper);
	}

	return this;
}

NodeIterator *ParentJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new ParentIterator(li.adopt(), ri.adopt(), this);
}

QueryPlan *ParentJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) ParentJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ParentJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ParentJoinQP::getLongName() const
{
	return "ParentJoinQP";
}

string ParentJoinQP::getShortName() const
{
	return "p";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentIterator::ParentIterator(NodeIterator *children, NodeIterator *parents, const LocationInfo *o)
	: ParentOfChildIterator(children, parents, o)
{
}

bool ParentIterator::doJoin(DynamicContext *context)
{
	// Invarient 1: If ancestorStack_ is not empty, the current
	// children_ node is a descendant of every node on the stack

	// Invarient 2: The current parents_ node always follows every
	// node on the ancestorStack_ in document order

	// Invarient 3: The results_ buffer is maintained in document order,
	// by either inserting at the begining or the end of it.

	// Invarient 4: When ancestorStack_ is empty we can output the
	// buffered results_, since any more results will come after them in
	// document order.

	while(true) {
		context->testInterrupt();

		int cmp = parents_ == 0 ? -1 : isDescendantOf(children_, parents_, /*orSelf*/true);
		if(cmp < 0) {
			if(!ancestorStack_.empty()) {
				// We've found the closest ancestor - is it a parent?
				std::vector<DbXmlNodeImpl::Ptr>::iterator i = ancestorStack_.end();
				--i;
				if((*i)->getNodeLevel() == children_->getNodeLevel()) {
					if(children_->getType() != ATTRIBUTE &&
						(i == ancestorStack_.begin() || (*(--i))->getNodeLevel() != (children_->getNodeLevel() - 1))) {
						i = ancestorStack_.end();
					}
				} else if((*i)->getNodeLevel() != (children_->getNodeLevel() - 1)) {
					i = ancestorStack_.end();
				}

				if(i != ancestorStack_.end()) {
					// Maintain invarient 3
					if(results_.empty() || NodeInfo::compare(results_.back(), *i) < 0)
						results_.push_back(*i);
					else results_.insert(results_.begin(), *i);
					ancestorStack_.erase(i);
				}
			}

			if(ancestorStack_.empty()) {
				if(!results_.empty()) {
					// Output the results we have collected so far
					it_ = results_.begin();
					node_ = *it_;
					++it_;
					return true;
				}

				if(parents_ == 0) break;
				if(!children_->seek(parents_, context)) break;
			} else {
				if(!children_->next(context)) break;
			}

			// Maintain invarient 1
			while(!ancestorStack_.empty() &&
				isDescendantOf(children_, ancestorStack_.back().get(), /*orSelf*/false) > 0) {
				ancestorStack_.pop_back();
			}

			if(ancestorStack_.empty() && !results_.empty()) {
				// Output the results we have collected so far
				it_ = results_.begin();
				node_ = *it_;
				++it_;
				return true;
			}
		} else if(cmp > 0) {
			if(NodeInfo::isSameDocument(children_, parents_) > 0) {
				if(!parents_->seek(children_->getContainerID(), children_->getDocID(),
					   *NsNid::getRootNid(), context)) break;
			} else {
				NsNidWrap nr(parents_->getLastDescendantID());
				if(!parents_->seek(parents_->getContainerID(), parents_->getDocID(),
					   nr, context)) break;
			}
		} else {
			ancestorStack_.push_back(parents_->asDbXmlNode(context));
			if(!parents_->next(context)) {
				delete parents_;
				parents_ = 0;
			}
		}
	}

	delete parents_;
	parents_ = 0;
	delete children_;
	children_ = 0;
	ancestorStack_.clear();

	if(!results_.empty()) {
		// Output the results we have collected so far
		it_ = results_.begin();
		node_ = *it_;
		++it_;
		return true;
	}

	state_ = DONE;
	return false;
}

