//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "LevelFilterQP.hpp"
#include "StructuralJoinQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

LevelFilterQP::LevelFilterQP(QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(LEVEL_FILTER, arg, flags, mm)
{
}

NodeIterator *LevelFilterQP::createNodeIterator(DynamicContext *context) const
{
	return new LevelFilter(arg_->createNodeIterator(context), this);
}

QueryPlan *LevelFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	LevelFilterQP *result = new (mm) LevelFilterQP(arg_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void LevelFilterQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *LevelFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	_src.setProperties(_src.getProperties() | StaticAnalysis::PEER);

	return this;
}

void LevelFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	_src.setProperties(_src.getProperties() | StaticAnalysis::PEER);
}

QueryPlan *LevelFilterQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

// 	QueryPlan *lkup = StructuralJoinQP::findLookup(arg_);
// 	if(lkup != 0) {
// 		PresenceQP *pqp = (PresenceQP*)lkup;
// 		if(pqp->isParentSet() && NsUtil::nsStringEqual((const xmlbyte_t*)pqp->getParentName(),
// 			   (const xmlbyte_t*)Name::dbxml_colon_root.getURIName().c_str())) {
// 			// The parent of the right argument is set to dbxml:root, so we don't
// 			// need a join against the document node sequential scan
// 			logTransformation(opt.getLog(), "Redundant level filter", this, arg_);
// 			return arg_;
// 		}
// 	}

	return this;
}

void LevelFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		LevelFilterQP *result = new (mm) LevelFilterQP(*it, flags_, mm);
		result->setLocationInfo(this);

		combinations.push_back(result);
	}
}

bool LevelFilterQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == LEVEL_FILTER) {
		LevelFilterQP *vf = (LevelFilterQP*)o;
		return arg_->isSubsetOf(vf->arg_);
	}

	return arg_->isSubsetOf(o);
}

string LevelFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<LevelFilterQP>" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << in << "</LevelFilterQP>" << endl;

	return s.str();
}

string LevelFilterQP::toString(bool brief) const
{
	ostringstream s;

	s << "LF(";
	s << arg_->toString(brief);
	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LevelFilter::LevelFilter(NodeIterator *parent, const LocationInfo *location)
	: ProxyIterator(location)
{
	result_ = parent;
}

LevelFilter::~LevelFilter()
{
	delete result_;
}

bool LevelFilter::next(DynamicContext *context)
{
	while(result_->next(context)) {
		if(result_->getNodeLevel() == 1) return true;
	}
	return false;
}

bool LevelFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!result_->seek(container, did, nid, context)) return false;
	while(result_->getNodeLevel() != 1) {
		if(!result_->next(context)) return false;
	}
	return true;
}

