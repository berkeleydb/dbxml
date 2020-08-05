//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ParentOfChildJoinQP.hpp"
#include "QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

ParentOfChildJoinQP::ParentOfChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(PARENT_OF_CHILD, l, r, flags, mm)
{
}

NodeIterator *ParentOfChildJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new ParentOfChildIterator(li.adopt(), ri.adopt(), this);
}

QueryPlan *ParentOfChildJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) ParentOfChildJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ParentOfChildJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ParentOfChildJoinQP::getLongName() const
{
	return "ParentOfChildJoinQP";
}

string ParentOfChildJoinQP::getShortName() const
{
	return "pc";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentOfChildIterator::ParentOfChildIterator(NodeIterator *children, NodeIterator *parents, const LocationInfo *o)
	: DbXmlNodeIterator(o),
	  children_(children),
	  parents_(parents),
	  state_(INIT)
{
}

ParentOfChildIterator::~ParentOfChildIterator()
{
	delete children_;
	delete parents_;
}

bool ParentOfChildIterator::next(DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!parents_->next(context)) break;
		if(!children_->seek(parents_, context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		if(it_ != results_.end()) {
			node_ = *it_;
			++it_;
			return true;
		}
		results_.clear();

		if(parents_ == 0) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool ParentOfChildIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!parents_->seek(container, did, nid, context)) break;
		if(!children_->seek(parents_, context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		while(it_ != results_.end()) {
			if(NodeInfo::isSameNID(*it_, container, did, nid) >= 0) {
				node_ = *it_;
				++it_;
				return true;
			}
			++it_;
		}
		results_.clear();

		if(parents_ == 0) break;
		if(NodeInfo::isSameNID(parents_, container, did, nid) < 0 &&
			!parents_->seek(container, did, nid, context)) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool ParentOfChildIterator::doJoin(DynamicContext *context)
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

		int cmp = parents_ == 0 ? -1 : isDescendantOf(children_, parents_, /*orSelf*/false);
		if(cmp < 0) {
			if(!ancestorStack_.empty()) {
				// We've found the closest ancestor - is it a parent?
				if(ancestorStack_.back()->getNodeLevel() == (children_->getNodeLevel() - 1)) {
					// Maintain invarient 3
					if(results_.empty() || NodeInfo::compare(results_.back(), ancestorStack_.back()) < 0)
						results_.push_back(ancestorStack_.back());
					else results_.insert(results_.begin(), ancestorStack_.back());
					ancestorStack_.pop_back();
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

