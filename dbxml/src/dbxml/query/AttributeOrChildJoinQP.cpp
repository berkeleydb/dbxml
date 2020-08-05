//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "AttributeOrChildJoinQP.hpp"
#include "AttributeJoinQP.hpp"
#include "ChildJoinQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../QueryContext.hpp"
#include "../Manager.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

AttributeOrChildJoinQP::AttributeOrChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(ATTRIBUTE_OR_CHILD, l, r, flags, mm)
{
}

QueryPlan *AttributeOrChildJoinQP::optimize(OptimizationContext &opt)
{
	QueryPlan *result = StructuralJoinQP::optimize(opt);
	if(result != this) return result;

	XPath2MemoryManager *mm = opt.getMemoryManager();

	ImpliedSchemaNode::Type type = findType(right_);
	if(type == ImpliedSchemaNode::ATTRIBUTE) {
		// Convert to a AttributeJoinQP
		QueryPlan *result = new (mm) AttributeJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation(opt.getLog(), "More specific join", this, result);
		return result->optimize(opt);
	}

	if(type != -1) {
		// Convert to a ChildJoinQP
		QueryPlan *result = new (mm) ChildJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation(opt.getLog(), "More specific join", this, result);
		return result->optimize(opt);
	}

	return this;
}

QueryPlan *AttributeOrChildJoinQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	StructuralJoinQP::staticTyping(context, styper);

	XPath2MemoryManager *mm = context->getMemoryManager();

	if(right_->getStaticAnalysis().getStaticType().isType(StaticType::ATTRIBUTE_TYPE)) {
		// Convert to a AttributeJoinQP
		QueryPlan *result = new (mm) AttributeJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation((Manager&)GET_CONFIGURATION(context)->getManager(),
			"More specific join", this, result);
		return result->staticTyping(context, styper);
	}

	if(!right_->getStaticAnalysis().getStaticType().containsType(StaticType::ATTRIBUTE_TYPE)) {
		// Convert to a ChildJoinQP
		QueryPlan *result = new (mm) ChildJoinQP(left_, right_, flags_, mm);
		result->setLocationInfo(this);

		logTransformation((Manager&)GET_CONFIGURATION(context)->getManager(),
			"More specific join", this, result);
		return result->staticTyping(context, styper);
	}

	return this;
}

NodeIterator *AttributeOrChildJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new AttributeOrChildIterator(li.adopt(), ri.adopt(), this);
}

QueryPlan *AttributeOrChildJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) AttributeOrChildJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void AttributeOrChildJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string AttributeOrChildJoinQP::getLongName() const
{
	return "AttributeOrChildJoinQP";
}

string AttributeOrChildJoinQP::getShortName() const
{
	return "ca";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AttributeOrChildIterator::AttributeOrChildIterator(NodeIterator *ancestors, NodeIterator *descendants,
	const LocationInfo *o)
	: DescendantIterator(/*orSelf*/false, ancestors, descendants, o)
{
}

bool AttributeOrChildIterator::doJoin(DynamicContext *context)
{
	// Invarient 1: if ancestorStack_ is not empty, the current
	// descendants_ node is a descendant of every node on the stack

	// Invarient 2: The current ancestors_ node always follows every
	// node on the ancestorStack_ in document order

	while(true) {
		context->testInterrupt();

		// Maintain invarient 1
		while(!ancestorStack_.empty() &&
			isDescendantOf(descendants_, ancestorStack_.back().get(), /*orSelf*/false) > 0) {
			ancestorStack_.pop_back();
		}

		int cmp = ancestors_ == 0 ? -1 : isDescendantOf(descendants_, ancestors_, /*orSelf*/false);
		if(cmp < 0) {
			if(ancestorStack_.empty()) {
				if(ancestors_ == 0) break;
				if(!descendants_->seek(ancestors_, context)) break;
			} else {
				// We've found the closest ancestor - is it a parent?
				// TBD check the second on the stack as well - jpcs
				if(ancestorStack_.back()->getNodeLevel() ==
					(descendants_->getNodeLevel() - (descendants_->getType() == ATTRIBUTE ? 0 : 1))) {
					result_ = descendants_;
					return true;
				}

				if(!descendants_->next(context)) break;
			}
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
			ancestorStack_.push_back(ancestors_->asDbXmlNode(context));
			if(!ancestors_->next(context)) {
				delete ancestors_;
				ancestors_ = 0;
			}
		}
	}

	ancestorStack_.clear();
	state_ = DONE;
	return false;
}

