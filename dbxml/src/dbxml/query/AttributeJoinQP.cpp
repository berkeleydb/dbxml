//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "AttributeJoinQP.hpp"
#include "QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

AttributeJoinQP::AttributeJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(ATTRIBUTE, l, r, flags, mm)
{
}

NodeIterator *AttributeJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new AttributeIterator(li.adopt(), ri.adopt(), this);
}

QueryPlan *AttributeJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) AttributeJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void AttributeJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string AttributeJoinQP::getLongName() const
{
	return "AttributeJoinQP";
}

string AttributeJoinQP::getShortName() const
{
	return "a";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AttributeIterator::AttributeIterator(NodeIterator *parents, NodeIterator *attributes, const LocationInfo *o)
	: ProxyIterator(o),
	  parents_(parents),
	  attributes_(attributes),
	  state_(INIT)
{
}

AttributeIterator::~AttributeIterator()
{
	delete parents_;
	delete attributes_;
}

bool AttributeIterator::next(DynamicContext *context)
{
	switch(state_) {
	case INIT:
		state_ = RUNNING;
		if(!parents_->next(context)) break;
		if(!attributes_->seek(parents_, context)) break;
		return doJoin(context);
	case RUNNING:
		if(!attributes_->next(context)) break;
		return doJoin(context);
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool AttributeIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	switch(state_) {
	case INIT:
	case RUNNING:
		state_ = RUNNING;
		if(!parents_->seek(container, did, nid, context)) break;
		if(!attributes_->seek(parents_, context)) break;
		return doJoin(context);
	case DONE: break;
	}

	state_ = DONE;
	return false;
}

bool AttributeIterator::doJoin(DynamicContext *context)
{
	while(true) {
		context->testInterrupt();

		int cmp = isSameNID(parents_, attributes_);
		if(cmp < 0) {
			if(!parents_->seek(attributes_, context)) break;
		} else if(cmp > 0) {
			if(!attributes_->seek(parents_, context)) break;
		} else {
			if(parents_->getType() == ELEMENT) {
				if(attributes_->getType() == ATTRIBUTE) {
					result_ = attributes_;
					return true;
				} else {
					if(!attributes_->next(context)) break;
				}
			} else {
				if(!parents_->next(context)) break;
			}
		}
	}

	return false;
}

