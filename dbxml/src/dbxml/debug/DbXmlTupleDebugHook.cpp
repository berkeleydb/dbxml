//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlTupleDebugHook.hpp"
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

class DbXmlTupleStackFrame : public StackFrameImpl {
public:
	DbXmlTupleStackFrame(const TupleNode *ast, DynamicContext *context)
		: StackFrameImpl(ast, context) {}

	virtual std::string getQueryPlan() const
	{
		return DbXmlPrintAST::print((TupleNode*)location_, context_, 0);
	}
};

class DbXmlTupleDebugHookResult : public TupleResult
{
public:
	DbXmlTupleDebugHookResult(const TupleNode *expr, DynamicContext *context)
		: TupleResult(expr),
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

	~DbXmlTupleDebugHookResult()
	{
		parent_ = 0;

		XmlDebugListener *dl = GET_DEBUGLISTENER(frame_.getContext());
		AutoXmlStackFrameReset reset(GET_CONFIGURATION(frame_.getContext()), &frame_);
		if(dl) dl->end(&frame_);
	}
  
	virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const
	{
		return parent_ ? parent_->getVar(namespaceURI, name) : 0;
	}

	virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
	{
		if(parent_) parent_->getInScopeVariables(variables);
	}

	virtual bool next(DynamicContext *context)
	{
		XmlDebugListener *dl = GET_DEBUGLISTENER(frame_.getContext());

		AutoXmlStackFrameReset reset(GET_CONFIGURATION(context), &frame_);
		if(dl) dl->enter(&frame_);
		try {
			bool result = parent_ ? parent_->next(context) : false;
			if(dl) dl->exit(&frame_);
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
	DbXmlTupleStackFrame frame_;
	TupleResult::Ptr parent_;
};

TupleResult::Ptr DbXmlTupleDebugHook::createResult(DynamicContext* context) const
{
	return new DbXmlTupleDebugHookResult(parent_, context);
}
