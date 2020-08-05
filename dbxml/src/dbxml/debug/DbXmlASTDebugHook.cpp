//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlASTDebugHook.hpp"
#include "QPDebugHook.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../QueryContext.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>
#include <xqilla/update/PendingUpdateList.hpp>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

class DbXmlASTStackFrame : public StackFrameImpl {
public:
	DbXmlASTStackFrame(const ASTNode *ast, DynamicContext *context)
		: StackFrameImpl(ast, context) {}

	virtual std::string getQueryPlan() const
	{
		return DbXmlPrintAST::print((ASTNode*)location_, context_, 0);
	}
};

class DbXmlASTDebugHookResult : public ResultImpl
{
public:
	DbXmlASTDebugHookResult(const ASTNode *expr, DynamicContext *context)
		: ResultImpl(expr),
		  context_(context),
		  frame_(expr, context),
		  parent_(0)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->start(&frame_);
		try {
			parent_ = expr->createResult(context);
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
  
	DbXmlASTDebugHookResult(const Result &contextItems, const ASTNode *expr, DynamicContext *context)
		: ResultImpl(expr),
		  context_(context),
		  frame_(expr, context),
		  parent_(0)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->start(&frame_);
		try {
			parent_ = expr->iterateResult(contextItems, context);
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

	~DbXmlASTDebugHookResult()
	{
		parent_ = 0;

		XmlDebugListener *dl = GET_DEBUGLISTENER(frame_.getContext());

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(frame_.getContext()), &frame_);
		if(dl) dl->end(&frame_);
	}
  
	Item::Ptr next(DynamicContext *context)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		frame_.setPreviousFrame(context);
		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->enter(&frame_);
		try {
			Item::Ptr item = parent_->next(context);
			if(dl) dl->exit(&frame_);
			return item;
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
		return 0;
	}

	Item::Ptr nextOrTail(Result &tail, DynamicContext *context)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(context);

		frame_.setPreviousFrame(context);
		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->enter(&frame_);
		try {
			ResultImpl *oldparent = parent_.get();
			Item::Ptr item = parent_->nextOrTail(parent_, context);
			if(dl) dl->exit(&frame_);
			if(parent_.get() != oldparent) tail = parent_;
			return item;
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
		return 0;
	}

protected:
	DynamicContext *context_;
	DbXmlASTStackFrame frame_;
	Result parent_;
};

Result DbXmlASTDebugHook::createResult(DynamicContext *context, int flags) const
{
	XmlDebugListener *dl = GET_DEBUGLISTENER(context);

	if(!dl) return expr_->createResult(context);

	if(GET_CONFIGURATION(context)->getQueryContext().getEvaluationType() == XmlQueryContext::Lazy)
		return new DbXmlASTDebugHookResult(expr_, context);

	DbXmlASTStackFrame frame(expr_, context);
	AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame);

	dl->start(&frame);
	dl->enter(&frame);
	Sequence result(context->getMemoryManager());
	try {
		result = expr_->createResult(context)->toSequence(context);
		dl->exit(&frame);
		dl->end(&frame);
	}
	catch(QueryInterruptedException &ex) {
		XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
		dl->error(xmlEx, &frame);
	}
	catch(QueryTimeoutException &ex) {
		XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XQException &ex) {
		XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XmlException &ex) {
		dl->error(ex, &frame);
	}

	return result;
}

Result DbXmlASTDebugHook::iterateResult(const Result &contextItems, DynamicContext *context) const
{
	XmlDebugListener *dl = GET_DEBUGLISTENER(context);

	if(!dl) return expr_->iterateResult(contextItems, context);

	if(GET_CONFIGURATION(context)->getQueryContext().getEvaluationType() == XmlQueryContext::Lazy)
		return new DbXmlASTDebugHookResult(contextItems, expr_, context);

	return ASTNodeImpl::iterateResult(contextItems, context);
}

EventGenerator::Ptr DbXmlASTDebugHook::generateEvents(EventHandler *events, DynamicContext *context,
	bool preserveNS, bool preserveType) const
{
	XmlDebugListener *dl = GET_DEBUGLISTENER(context);

	if(!dl) return expr_->generateEvents(events, context, preserveNS, preserveType);

	DbXmlASTStackFrame frame(expr_, context);
	AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame);

	dl->start(&frame);
	dl->enter(&frame);
	try {
		EventGenerator::Ptr result = expr_->generateEvents(events, context, preserveNS, preserveType);
		dl->exit(&frame);
		dl->end(&frame);
		return result;
	}
	catch(QueryInterruptedException &ex) {
		XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
		dl->error(xmlEx, &frame);
	}
	catch(QueryTimeoutException &ex) {
		XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XQException &ex) {
		XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XmlException &ex) {
		dl->error(ex, &frame);
	}

	return 0;
}

PendingUpdateList DbXmlASTDebugHook::createUpdateList(DynamicContext *context) const
{
	XmlDebugListener *dl = GET_DEBUGLISTENER(context);

	if(!dl) return expr_->createUpdateList(context);

	DbXmlASTStackFrame frame(expr_, context);
	AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame);

	dl->start(&frame);
	dl->enter(&frame);
	PendingUpdateList pul;
	try {
		pul = expr_->createUpdateList(context);
		dl->exit(&frame);
		dl->end(&frame);
	}
	catch(QueryInterruptedException &ex) {
		XmlException xmlEx(XmlException::OPERATION_INTERRUPTED, ex);
		dl->error(xmlEx, &frame);
	}
	catch(QueryTimeoutException &ex) {
		XmlException xmlEx(XmlException::OPERATION_TIMEOUT, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XQException &ex) {
		XmlException xmlEx(XmlException::QUERY_EVALUATION_ERROR, ex);
		dl->error(xmlEx, &frame);
	}
	catch(XmlException &ex) {
		dl->error(ex, &frame);
	}

	return pul;
}

