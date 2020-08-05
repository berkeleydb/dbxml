//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "AncestorJoinQP.hpp"
#include "QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

AncestorJoinQP::AncestorJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(ANCESTOR, l, r, flags, mm)
{
}

NodeIterator *AncestorJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new AncestorIterator(/*orSelf*/false, li.adopt(), ri.adopt(), this);
}

QueryPlan *AncestorJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) AncestorJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void AncestorJoinQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

string AncestorJoinQP::getLongName() const
{
	return "AncestorJoinQP";
}

string AncestorJoinQP::getShortName() const
{
	return "an";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AncestorOrSelfJoinQP::AncestorOrSelfJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(ANCESTOR_OR_SELF, l, r, flags, mm)
{
}

NodeIterator *AncestorOrSelfJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new AncestorIterator(/*orSelf*/true, li.adopt(), ri.adopt(), this);
}

QueryPlan *AncestorOrSelfJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) AncestorOrSelfJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void AncestorOrSelfJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string AncestorOrSelfJoinQP::getLongName() const
{
	return "AncestorOrSelfJoinQP";
}

string AncestorOrSelfJoinQP::getShortName() const
{
	return "ans";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AncestorIterator::AncestorIterator(bool orSelf, NodeIterator *descendants, NodeIterator *ancestors,
	const LocationInfo *o)
	: ProxyIterator(o),
	  orSelf_(orSelf),
	  descendants_(descendants),
	  ancestors_(ancestors),
	  state_(INIT)
{
}

AncestorIterator::~AncestorIterator()
{
	delete descendants_;
	delete ancestors_;
}

bool AncestorIterator::next(DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!ancestors_->next(context)) break;
		if(!descendants_->seek(ancestors_, context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		if(!ancestors_->next(context)) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool AncestorIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT: {
		state_ = RUNNING;
		if(!ancestors_->seek(container, did, nid, context)) break;
		if(!descendants_->seek(ancestors_, context)) break;
		return doJoin(context);
	}
	case RUNNING: {
		if(!ancestors_->seek(container, did, nid, context)) break;
		return doJoin(context);
	}
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool AncestorIterator::doJoin(DynamicContext *context)
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
			result_ = ancestors_;
			return true;
		}
	}

	state_ = DONE;
	return false;
}

