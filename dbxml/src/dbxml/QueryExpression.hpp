//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __EXPRESSION_HPP
#define	__EXPRESSION_HPP

#include <string>
#include "dbxml/XmlQueryContext.hpp"
#include "ReferenceCounted.hpp"
#include "ReferenceMinder.hpp"
#include "query/QueryExecutionContext.hpp"
#include "dataItem/DbXmlConfiguration.hpp"

#include <xqilla/framework/XPath2MemoryManager.hpp>

class XPath2MemoryManager;
class ASTNode;

class DynamicContext;
class XQQuery;
class Optimizer;

namespace DbXml
{

class Results;
class Value;
class Transaction;

class QueryExpression : public ReferenceCountedProtected
{
public:
	QueryExpression(const std::string &query,
		XmlQueryContext &context, Transaction *txn, bool debug = false);
	virtual ~QueryExpression();
	
	const std::string &getQuery() const {
		return query_;
	}
	std::string getQueryPlan() const;
	
	XmlQueryContext &getContext() {
		return context_;
	}
	Results *execute(Transaction *txn, Value *contextItem,
			 XmlQueryContext &context, u_int32_t flags = 0);

	DynamicContext *getDynamicContext() const;
	XQQuery *getCompiledExpression() const;
	bool isUpdating() const;

	CompileInfo *getCompileInfo() { return &ci_; }
	ProjectionInfo *getProjectionInfo() { return &pi_; }

	std::map<int, XmlContainer> getContainers() const;

	static Optimizer *createOptimizer(DynamicContext *context,
		ReferenceMinder &minder, bool debug = false);
	
protected:
	ReferenceMinder minder_;

private:
	QueryExpression(const QueryExpression&);
	QueryExpression &operator=(const QueryExpression &);

	std::string query_;
	XmlQueryContext context_;
	QueryExecutionContext qec_;

	CompileInfo ci_;
	ProjectionInfo pi_;
	DbXmlConfiguration conf_;

	AutoDelete<DynamicContext> xqContext_;
	XQQuery *expr_;
};
	
DBXML_EXPORT XmlQueryExpression debugOptimization(XmlTransaction &txn, const std::string &query, XmlQueryContext &context);

}

#endif
