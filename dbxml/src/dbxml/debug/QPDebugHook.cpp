//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "QPDebugHook.hpp"
#include "../query/ASTToQueryPlan.hpp"
#include "../query/QueryPlanToAST.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../Results.hpp"
#include "../QueryContext.hpp"
#include "../QueryExpression.hpp"
#include <dbxml/XmlResults.hpp>

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

QPDebugHook::QPDebugHook(QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(DEBUG_HOOK, arg, flags, mm)
{
	_src.copy(arg_->getStaticAnalysis());
	setLocationInfo(arg_);
}

class QPStackFrame : public StackFrameImpl {
public:
	QPStackFrame(const QueryPlan *qp, DynamicContext *context)
		: StackFrameImpl(qp, context) {}

	virtual std::string getQueryPlan() const
	{
		return ((QueryPlan*)location_)->printQueryPlan(context_, 0);
	}
};

class QPDebugIterator : public ProxyIterator
{
public:
	QPDebugIterator(const QueryPlan *arg, DynamicContext *context, bool lazy)
		: ProxyIterator(arg),
		  frame_(arg, context),
		  lazy_(lazy)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->start(&frame_);
		if(dl && !lazy_) dl->enter(&frame_);
		try {
			result_ = arg->createNodeIterator(context);
		}
		catch(QueryInterruptedException &ex) {
			XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(QueryTimeoutException &ex) {
			XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XQException &ex) {
			XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XmlException &ex) {
			if(dl) dl->error(ex, &frame_);
		}
	}

	~QPDebugIterator()
	{
		delete result_;

		XmlDebugListener *dl = GET_DEBUGLISTENER(frame_.getContext());

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(frame_.getContext()), &frame_);
		if(dl && !lazy_) dl->exit(&frame_);
		if(dl) dl->end(&frame_);
	}

	virtual bool next(DynamicContext *context)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		frame_.setPreviousFrame(context);
		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl && lazy_) dl->enter(&frame_);
		try {
			bool result = result_ ? result_->next(context) : false;
			if(dl && lazy_) dl->exit(&frame_);
			return result;
		}
		catch(QueryInterruptedException &ex) {
			XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(QueryTimeoutException &ex) {
			XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XQException &ex) {
			XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XmlException &ex) {
			if(dl) dl->error(ex, &frame_);
		}
		return false;
	}

	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		frame_.setPreviousFrame(context);
		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl && lazy_) dl->enter(&frame_);
		try {
			bool result = result_ ? result_->seek(container, did, nid, context) : false;
			if(dl && lazy_) dl->exit(&frame_);
			return result;
		}
		catch(QueryInterruptedException &ex) {
			XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(QueryTimeoutException &ex) {
			XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XQException &ex) {
			XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
			if(dl) dl->error(xmlEx, &frame_);
		}
		catch(XmlException &ex) {
			if(dl) dl->error(ex, &frame_);
		}
		return false;
	}

private:
	QPStackFrame frame_;
	bool lazy_;
};

NodeIterator *QPDebugHook::createNodeIterator(DynamicContext *context) const
{
	XmlDebugListener *dl = GET_DEBUGLISTENER(context);

	if(!dl) return arg_->createNodeIterator(context);

	bool lazy = GET_CONFIGURATION(context)->getQueryContext().getEvaluationType() == XmlQueryContext::Lazy;
	return new QPDebugIterator(arg_, context, lazy);
}

QueryPlan *QPDebugHook::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	QPDebugHook *result = new (mm) QPDebugHook(arg_->copy(mm), flags_, mm);
	result->_src.copy(_src);
	result->setLocationInfo(this);
	return result;
}

void QPDebugHook::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *QPDebugHook::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());

	return this;
}

void QPDebugHook::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
}

QueryPlan *QPDebugHook::optimize(OptimizationContext &opt)
{
	arg_ = arg_->optimize(opt);
	return this;
}

string QPDebugHook::printQueryPlan(const DynamicContext *context, int indent) const
{
	return arg_->printQueryPlan(context, indent);

// 	ostringstream s;

// 	string in(PrintAST::getIndent(indent));

// 	s << in << "<QPDebugHook";
// 	s << " location=\"" << XMLChToUTF8(getFile()).str() << ":" << getLine() << ":" << getColumn() << "\"";
// 	s <<">" << endl;
// 	s << arg_->printQueryPlan(context, indent + INDENT);
// 	s << in << "</QPDebugHook>" << endl;

// 	return s.str();
}

string QPDebugHook::toString(bool brief) const
{
	return arg_->toString(brief);
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

StackFrameImpl::StackFrameImpl(const LocationInfo *location, DynamicContext *context)
	: location_(location),
	  context_(context),
	  contextItem_(context->getContextItem()),
	  contextPosition_(context->getContextPosition()),
	  contextSize_(context->getContextSize()),
	  variables_(context->getVariableStore()),
	  nsResolver_(context->getNSResolver()),
	  defaultElementNS_(context->getDefaultElementAndTypeNS()),
	  prev_(GET_CONFIGURATION(context)->getStackFrame())
{
}

const char *StackFrameImpl::getQueryFile() const
{
	if(file_ == "")
		file_ = XMLChToUTF8(location_->getFile()).str();
	return file_.c_str();
}

int StackFrameImpl::getQueryLine() const
{
	return location_->getLine();
}

int StackFrameImpl::getQueryColumn() const
{
	return location_->getColumn();
}

XmlResults StackFrameImpl::query(const std::string &queryString) const
{
	XmlResults xmlresult = new ValueResults(GET_CONFIGURATION(context_)->getManager());

	AutoDelete<DynamicContext> context(0);
	AutoDelete<XQQuery> query(0);
	try {
		context.set(context_->createDebugQueryContext(contextItem_, contextPosition_,
				    contextSize_, variables_,
				    nsResolver_, defaultElementNS_));
		query.set(XQilla::parse(UTF8ToXMLCh(queryString).str(), context.get(), 0, XQilla::NO_ADOPT_CONTEXT));

		ScopedPtr<Optimizer> optimizer(QueryExpression::createOptimizer(context.get(),
						       *GET_CONFIGURATION(context_)->getMinder()));
		optimizer->startOptimize(query.get());
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::QUERY_PARSER_ERROR, e);
	}
	try {
		Result res = query->execute(context);
		Item::Ptr item;
		while((item = res->next(context)).notNull()) {
			if(item->isNode()) {
				xmlresult.add(Value::create((Node*)item.get(), /*lazyDocs*/true));
				/* The XmlResults needs access to the dbMinder so that
				   it can access temporary databases if the nodes are passed
				   to Java then need to be recreated in C++ */
				if((*xmlresult).getDbMinder().isNull())
					(*xmlresult).getDbMinder() = GET_CONFIGURATION(context_)->getDbMinder();
			}
			else {
				xmlresult.add(Value::create(item, context_));
			}
		}
	}
	catch(QueryInterruptedException &e) {
		throw XmlException(XmlException::OPERATION_INTERRUPTED, e);
	}
	catch(QueryTimeoutException &e) {
		throw XmlException(XmlException::OPERATION_TIMEOUT, e);
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::QUERY_EVALUATION_ERROR, e);
	}

	return xmlresult;
}

void StackFrameImpl::setPreviousFrame(DynamicContext *context)
{
	prev_ = GET_CONFIGURATION(context)->getStackFrame();
}
