//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlQueryExpression.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "QueryExpression.hpp"

using namespace DbXml;

static const char *className = "XmlQueryExpression";
#define CHECK_POINTER checkNullPointer(expression_,className)

static inline void checkFlags(u_int32_t flags) {
	if(flags & ~(DB_READ_COMMITTED|DB_RMW|DB_READ_UNCOMMITTED|DBXML_DOCUMENT_PROJECTION
		     |DBXML_LAZY_DOCS|DB_TXN_SNAPSHOT|DBXML_WELL_FORMED_ONLY|DBXML_NO_AUTO_COMMIT)) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Invalid flags to method XmlQueryExpression::execute");
	}
}

XmlQueryExpression::XmlQueryExpression()
	: expression_(0)
{
}

XmlQueryExpression::XmlQueryExpression(QueryExpression *expression)
	: expression_(expression)
{
	if (expression_ != 0)
		expression_->acquire();
}

XmlQueryExpression::~XmlQueryExpression()
{
	if (expression_ != 0)
		expression_->release();
}

XmlQueryExpression::XmlQueryExpression(const XmlQueryExpression& o)
	: expression_(o.expression_)
{
	if (expression_ != 0)
		expression_->acquire();
}

XmlQueryExpression &XmlQueryExpression::operator=(const XmlQueryExpression& o)
{
	if (this != &o && expression_ != o.expression_) {
		if (expression_ != 0)
			expression_->release();
		expression_ = o.expression_;
		if (expression_ != 0)
			expression_->acquire();
	}
	return *this;
}

const std::string &XmlQueryExpression::getQuery() const
{
	CHECK_POINTER;
	return expression_->getQuery();
}

std::string XmlQueryExpression::getQueryPlan() const
{
	CHECK_POINTER;
	return expression_->getQueryPlan();
}

bool XmlQueryExpression::isUpdateExpression() const
{
	CHECK_POINTER;
	return expression_->isUpdating();
}

XmlResults XmlQueryExpression::execute(XmlQueryContext &context,
				       u_int32_t flags) const
{
	CHECK_POINTER;
	checkFlags(flags);
	return expression_->execute(0, 0, context, flags);
}

XmlResults XmlQueryExpression::execute(XmlTransaction &txn,
				       XmlQueryContext &context,
				       u_int32_t flags) const
{
	CHECK_POINTER;
	checkFlags(flags);
	return expression_->execute(txn, 0, context, flags);
}

XmlResults XmlQueryExpression::execute(const XmlValue &contextItem,
				       XmlQueryContext &context,
				       u_int32_t flags) const
{
	CHECK_POINTER;
	checkFlags(flags);
	return expression_->execute(0, contextItem, context, flags);
}

XmlResults XmlQueryExpression::execute(
	XmlTransaction &txn, const XmlValue &contextItem,
	XmlQueryContext &context, u_int32_t flags) const
{
	CHECK_POINTER;
	checkFlags(flags);
	return expression_->execute(txn, contextItem, context, flags);
}
