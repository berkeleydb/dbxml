//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYEXECUTIONCONTEXT_HPP
#define	__QUERYEXECUTIONCONTEXT_HPP

#include <string>

#include "IndexEntry.hpp"

class DynamicContext;

namespace DbXml
{

class ContainerBase;
class QueryContext;
class Container;
	
/**
 * This function pointer declaration is used by the
 * QueryExecutionContext class to declare a callback
 * to a cost calculation function. There is a feedback
 * mechanism within the query plan execution engine
 * so that candidate set construction can be short
 * circuited once it becomes cheaper to filter.
 */
typedef double (*CostFunctionPointer)(long numDocuments);

/**
 * QueryExecutionContext
 *
 * Note that the costTofilter mechanism below is provided so that
 * during evaluation a query plan node can compare the cost of
 * refining the candidate set further, or giving up and allowing
 * the documents to be retrieved and filtered through a complete
 * XPath to DOM implementation.
 */
class QueryExecutionContext
{
public:
	QueryExecutionContext(QueryContext &context, bool debugging);
	~QueryExecutionContext();

	const ContainerBase *getContainerBase() const { return container_; }
	void setContainerBase(const ContainerBase *container) { container_ = container; }
	Container *getContainer();

	DynamicContext *getDynamicContext() { return dc_; }
	void setDynamicContext(DynamicContext *dc) { dc_ = dc; }

	QueryContext &getContext() const { return context; }

private:
	// no need for copy and assignment
	QueryExecutionContext(const QueryExecutionContext&);
	QueryExecutionContext &operator=(const QueryExecutionContext &);

	const ContainerBase *container_;

	QueryContext &context;
	DynamicContext *dc_;
};

}

#endif
