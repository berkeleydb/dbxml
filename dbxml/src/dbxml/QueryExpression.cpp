//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <sstream>

#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>
#include <xqilla/optimizer/PartialEvaluator.hpp>

#include "dataItem/DbXmlPrintAST.hpp"
#include "QueryExpression.hpp"
#include "QueryContext.hpp"
#include "Results.hpp"
#include "optimizer/ASTReplaceOptimizer.hpp"
#include "optimizer/QueryPlanOptimizer.hpp"
#include "optimizer/ImpliedSchemaGenerator.hpp"
#include "optimizer/QueryPlanGenerator.hpp"
#include "optimizer/DbXmlStaticTyper.hpp"
#include "debug/DbXmlDebugHookDecorator.hpp"
#include "Manager.hpp"
#include "HighResTimer.hpp"

#include "UTF8.hpp"

using namespace DbXml;
using namespace std;

// Counters
#define INCR(ctr) Globals::incrementCounter(ctr)

// QueryExpression
QueryExpression::QueryExpression(const std::string &query, XmlQueryContext &context, Transaction *txn, bool debug)
	: query_(query),
	  context_(context),
	  qec_(context_, /*debugging*/false),
	  conf_(context, txn, &ci_),
	  xqContext_(XQilla::createContext(XQilla::XQUERY_UPDATE, &conf_, Globals::defaultMemoryManager)),
	  expr_(0)
{
	((Manager &)((QueryContext &)getContext()).getManager())
		.log(Log::C_OPTIMIZER, Log::L_INFO, "Started parse");

	HighResTimer t;
	t.start();

	conf_.setMinder(&minder_);
	conf_.setQueryExecutionContext(&qec_);
	conf_.setProjectionInfo(&pi_);

	try {
		((QueryContext&)context_).startQuery();
		expr_ = XQilla::parse(UTF8ToXMLCh(getQuery()).str(), xqContext_, 0,
			XQilla::NO_STATIC_RESOLUTION | XQilla::NO_ADOPT_CONTEXT);

		ScopedPtr<Optimizer> optimizer(createOptimizer(xqContext_, minder_, debug));
		optimizer->startOptimize(expr_);
	}
	catch(const QueryInterruptedException &) {
		delete expr_;
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(const QueryTimeoutException &) {
		delete expr_;
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		delete expr_;
		throw XmlException(XmlException::QUERY_PARSER_ERROR, e);
	}
	catch(...) {
		delete expr_;
		throw;
	}
	t.stop();

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		ostringstream s;
		s << "Finished parse, time taken = " << (t.durationInSeconds() * 1000) << "ms";
		((Manager &)((QueryContext &)getContext()).getManager())
			.log(Log::C_OPTIMIZER, Log::L_INFO, s);
	}
}

QueryExpression::~QueryExpression()
{
	delete expr_;
}

std::string QueryExpression::getQueryPlan() const
{
	return DbXmlPrintAST::print(getCompiledExpression(), xqContext_);
}

DynamicContext *QueryExpression::getDynamicContext() const
{
	return xqContext_;
}

XQQuery *QueryExpression::getCompiledExpression() const
{
	return expr_;
}

bool QueryExpression::isUpdating() const
{
	return expr_->getQueryBody()->getType() == ASTNode::UAPPLY_UPDATES;
}

Results *QueryExpression::execute(Transaction *txn, Value *contextItem, XmlQueryContext &context, u_int32_t flags)
{
	if (contextItem && (contextItem->getType() == XmlValue::BINARY))
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlQueryExpression::execute: context item cannot be a binary value");

	// Count query
	INCR(Counters::num_queryExec);

	XmlManager &mgr = ((QueryContext&)context).getManager();

	// NOTE: LazyDIResults::next() catches all query exceptions.
	// If Eager resulst are ever changed to not be implemented in terms
	// of Lazy results, that code needs to catch exceptions as well
	// The timer, however, is never used for purely lazy queries, as
	// there is no control over iteration.

	if(isUpdating()) {
		// Auto-transact and execute an updating query
		TransactionGuard txnGuard;
		if(txn != 0 || (flags & DBXML_NO_AUTO_COMMIT) == 0) {
			txn = Transaction::autoTransact(txn, mgr, txnGuard,
				((Manager&)mgr).isTransactedEnv(), ((Manager&)mgr).isCDBEnv());
		}
		flags &= ~DBXML_NO_AUTO_COMMIT;

		{
			XmlResults res(new LazyDIResults(context, contextItem, *this, txn, flags));
			res.hasNext(); // Executes the update
		}

		txnGuard.commit();

		return new ValueResults(mgr);
	}

	Results *ret = new LazyDIResults(context, contextItem, *this, txn, flags & ~DBXML_NO_AUTO_COMMIT);
	if(context.getEvaluationType() == XmlQueryContext::Eager) {
		ret = new ValueResults(ret, mgr, txn);
	}
	return ret;
}

std::map<int, XmlContainer> QueryExpression::getContainers() const
{
	return minder_.getContainers();
}

Optimizer *QueryExpression::createOptimizer(DynamicContext *context, ReferenceMinder &minder, bool debug)
{
	// This needs to be kept in sync with DecisionPointQP::justInTimeOptimize()

	Optimizer *optimizer = 0;
	if(debug) optimizer = new OutputTree("Initial tree", context, optimizer);
	optimizer = new StaticResolver(context, optimizer);
	optimizer = new DbXmlStaticTyper(context, optimizer);
	if(debug) optimizer = new OutputTree("After Static Resolution", context, optimizer);
	optimizer = new ASTReplaceOptimizer(minder, context, optimizer);
	optimizer = new PartialEvaluator(context, optimizer);
	if(debug) optimizer = new OutputTree("After Partial Evaluator", context, optimizer);
	optimizer = new DbXmlStaticTyper(context, optimizer);
	if(debug) optimizer = new OutputTree("After Static Typing", context, optimizer);
	optimizer = new ImpliedSchemaGenerator(context, optimizer);
	optimizer = new QueryPlanGenerator(context, optimizer);
	if(debug) optimizer = new OutputTree("After Query Plan Generator", context, optimizer);
	optimizer = new QueryPlanOptimizer(OptimizationContext::RESOLVE_INDEXES, context, optimizer);
	if(debug) optimizer = new OutputTree("After Query Plan Optimization Phase 1 (RESOLVE_INDEXES)", context, optimizer);
	optimizer = new DbXmlStaticTyper(context, optimizer);
	if(debug) optimizer = new OutputTree("After Static Typing (2)", context, optimizer);
	optimizer = new QueryPlanOptimizer(OptimizationContext::ALTERNATIVES, context, optimizer);
	if(debug) optimizer = new OutputTree("After Query Plan Optimization Phase 2 (ALTERNATIVES)", context, optimizer);
	optimizer = new QueryPlanOptimizer(OptimizationContext::REMOVE_REDUNDENTS, context, optimizer);
	if(debug) optimizer = new OutputTree("After Query Plan Optimization Phase 3 (REMOVE_REDUNDENTS)", context, optimizer);
	optimizer = new DbXmlStaticTyper(context, optimizer);
	if(debug) optimizer = new OutputTree("After Static Typing (3)", context, optimizer);
	optimizer = new ProjectionSchemaAdjuster(context, optimizer);

	if(GET_CONFIGURATION(context)->getQueryContext().getDebugListener()) {
		optimizer = new DbXmlDebugHookDecorator(context, optimizer);
	}
	return optimizer;
}

XmlQueryExpression DbXml::debugOptimization(XmlTransaction &txn, const std::string &query, XmlQueryContext &context)
{
	INCR(Counters::num_prepare);
	return new QueryExpression(query, context, txn, /*debug*/true);
}

