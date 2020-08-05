//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ChildJoinQP.hpp"
#include "LevelFilterQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../nodeStore/NsUtil.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

ChildJoinQP::ChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(CHILD, l, r, flags, mm)
{
}

NodeIterator *ChildJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new ChildIterator(li.adopt(), ri.adopt(), this);
}

void ChildJoinQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	QueryPlan *result;

	if(containsAllDocumentNodes(left_)) {
		// Add a check for level == 1
		result = new (mm) LevelFilterQP(right_->copy(mm), 0, mm);
		result->setLocationInfo(this);
		logTransformation(opt.getLog(), "Redundant child", this, result);
		alternatives.push_back(result);
	}

	StructuralJoinQP::applyConversionRules(maxAlternatives, opt, alternatives);
}

QueryPlan *ChildJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) ChildJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ChildJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ChildJoinQP::getLongName() const
{
	return "ChildJoinQP";
}

string ChildJoinQP::getShortName() const
{
	return "c";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ChildIterator::ChildIterator(NodeIterator *ancestors, NodeIterator *descendants, const LocationInfo *o)
	: DescendantIterator(/*orSelf*/false, ancestors, descendants, o)
{
}

bool ChildIterator::doJoin(DynamicContext *context)
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
				if(ancestorStack_.back()->getNodeLevel() == (descendants_->getNodeLevel() - 1)) {
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

