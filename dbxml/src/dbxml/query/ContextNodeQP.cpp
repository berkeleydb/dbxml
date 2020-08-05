//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ContextNodeQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/DynamicErrorException.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

NodeIterator *ContextNodeQP::createNodeIterator(DynamicContext *context) const
{
	return new ContextNodeIterator(this);
}

QueryPlan *ContextNodeQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ContextNodeQP *result = new (mm) ContextNodeQP(container_, nodeTest_, flags_, mm);
	result->_src.copy(_src);
	result->setLocationInfo(this);
	return result;
}

void ContextNodeQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *ContextNodeQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	_src.getStaticType() = context->getContextItemType();
	_src.contextItemUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE |
		StaticAnalysis::ONENODE);

	return this;
}

void ContextNodeQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	_src.getStaticType() = StaticType::NODE_TYPE;
	_src.contextItemUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE |
		StaticAnalysis::ONENODE);
}

QueryPlan *ContextNodeQP::optimize(OptimizationContext &opt)
{
	return this;
}

void ContextNodeQP::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost ContextNodeQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	return Cost(1, 1);
}

bool ContextNodeQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == CONTEXT_NODE) return true;
	return false;
}

string ContextNodeQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<ContextNodeQP/>" << endl;

	return s.str();
}

string ContextNodeQP::toString(bool brief) const
{
	ostringstream s;

	s << "CN";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

bool ContextNodeIterator::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;

		Item::Ptr item = context->getContextItem();
		if(item.isNull()) {
			XQThrow3(DynamicErrorException,X("ContextNodeIterator::next"),
				X("It is an error for the context item to be undefined when using it [err:XPDY0002]"), location_);
		}
		if(!item->isNode()) {
			XQThrow3(TypeErrorException,X("ContextNodeIterator::next"),
				X("An attempt was made to perform an axis step when the Context Item was not a node [err:XPTY0020]"), location_);
		}

		node_ = (DbXmlNodeImpl*)item.get();
		return true;
	}

	return false;
}

bool ContextNodeIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	return next(context);
}

