//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "VariableQP.hpp"
#include "ASTToQueryPlan.hpp"
#include "QueryExecutionContext.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/DynamicErrorException.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/VarHashEntry.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

VariableQP::VariableQP(const XMLCh *prefix, const XMLCh *uri, const XMLCh *name, ContainerBase *container,
	DbXmlNodeTest *nodeTest, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(VARIABLE, flags, mm),
	  prefix_(prefix),
	  uri_(uri),
	  name_(name),
	  container_(container),
	  nodeTest_(nodeTest)
{
}

NodeIterator *VariableQP::createNodeIterator(DynamicContext *context) const
{
	return new ASTToQueryPlanIterator(context->getVariableStore()->getVar(uri_, name_), this);
}

QueryPlan *VariableQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	VariableQP *result = new (mm) VariableQP(mm->getPooledString(prefix_), mm->getPooledString(uri_),
		mm->getPooledString(name_), container_, nodeTest_, flags_, mm);
	result->_src.copy(_src);
	result->setLocationInfo(this);
	return result;
}

void VariableQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *VariableQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	const StaticAnalysis *var_src = context->getVariableTypeStore()->getVar(uri_, name_);
	DBXML_ASSERT(var_src != 0);

	_src.setProperties(var_src->getProperties());
	_src.getStaticType() = var_src->getStaticType();
	_src.variableUsed(uri_, name_);

	return this;
}

void VariableQP::staticTypingLite(StaticContext *context)
{
}

QueryPlan *VariableQP::optimize(OptimizationContext &opt)
{
	return this;
}

void VariableQP::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost VariableQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(_src.getProperties() & StaticAnalysis::ONENODE) {
		return Cost(1, 1);
	}

	// TBD we need some way to estimate this - jpcs
	return Cost(5, 1);
}

bool VariableQP::isSubsetOf(const QueryPlan *o) const
{
	// TBD Can we do better here? - jpcs
	return false;
}

string VariableQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<VariableQP name=\"";
	if(prefix_ != 0)
		s << XMLChToUTF8(prefix_).str() << ":";
	s << XMLChToUTF8(name_).str();
	s << "\"/>" << endl;

	return s.str();
}

string VariableQP::toString(bool brief) const
{
	ostringstream s;

	s << "VAR(";
	if(prefix_ != 0)
		s << XMLChToUTF8(prefix_).str() << ":";
	s << XMLChToUTF8(name_).str() << ")";

	return s.str();
}
 
