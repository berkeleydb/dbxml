//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ParentOfAttributeJoinQP.hpp"
#include "QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

ParentOfAttributeJoinQP::ParentOfAttributeJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: StructuralJoinQP(PARENT_OF_ATTRIBUTE, l, r, flags, mm)
{
}

NodeIterator *ParentOfAttributeJoinQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new ParentOfAttributeIterator(li.adopt(), ri.adopt(), this);
}

QueryPlan *ParentOfAttributeJoinQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) mm = memMgr_;
	QueryPlan *result = new (mm) ParentOfAttributeJoinQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ParentOfAttributeJoinQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ParentOfAttributeJoinQP::getLongName() const
{
	return "ParentOfAttributeJoinQP";
}

string ParentOfAttributeJoinQP::getShortName() const
{
	return "pa";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParentOfAttributeIterator::ParentOfAttributeIterator(NodeIterator *parents, NodeIterator *attributes,
	const LocationInfo *o)
	: IntersectIterator(parents, attributes, o)
{
}

bool ParentOfAttributeIterator::doJoin(DynamicContext *context)
{
	while(true) {
		context->testInterrupt();

		int cmp = isSameNID(left_, right_);
		if(cmp < 0) {
			if(!left_->seek(right_, context)) break;
		} else if(cmp > 0) {
			if(!right_->seek(left_, context)) break;
		} else {
			if(right_->getType() == ELEMENT) {
				if(left_->getType() == ATTRIBUTE) {
					result_ = right_;
					return true;
				} else {
					if(!left_->next(context)) break;
				}
			} else {
				if(!right_->next(context)) break;
			}
		}
	}

	return false;
}

