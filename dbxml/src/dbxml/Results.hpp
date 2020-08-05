//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __RESULTS_HPP
#define	__RESULTS_HPP

#include <string>
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "dbxml/XmlQueryExpression.hpp"
#include "dbxml/XmlEventReader.hpp"
#include "ReferenceCounted.hpp"
#include "Value.hpp"
#include "HighResTimer.hpp"
#include "ReferenceMinder.hpp"
#include "IndexEntry.hpp"
#include "Name.hpp"
#include "Key.hpp"
#include "IndexSpecification.hpp"
#include "Cursor.hpp"
#include "query/QueryExecutionContext.hpp"
#include "dataItem/DbXmlConfiguration.hpp"
#include "CacheDatabaseMinder.hpp"

class DynamicContext;

namespace DbXml
{

class Container;
class ResultsEventWriter;
/**
 * Results super class
 */
class DBXML_EXPORT Results : public ReferenceCounted
{
public:
	Results(XmlManager &mgr, Transaction *t = 0) : mgr_(mgr), oc_(t), writer_(0) {}
	virtual ~Results();

	virtual int next(XmlValue &value) = 0;
	virtual bool hasNext() = 0;
	virtual int peek(XmlValue &value) = 0;
	virtual int previous(XmlValue &value) = 0;
	virtual bool hasPrevious() = 0;
	virtual int reset() = 0;
	virtual void add(const XmlValue &value) = 0;
	virtual size_t size() const = 0;
	virtual std::map<int, XmlContainer> getContainers() const { return containers_; }
	virtual bool isLazy() const { return false; }
	virtual ResultsEventWriter& asEventWriter();
	virtual Results* copyResults();
	virtual void concatResults(Results &from);

	CacheDatabaseMinder &getDbMinder() { return dbMinder_; }
	OperationContext &getOperationContext() { return oc_; }
	XmlManager &getManager() { return mgr_; }
protected:
	CacheDatabaseMinder dbMinder_;
	XmlManager mgr_;
	OperationContext oc_;
	std::map<int, XmlContainer> containers_;
        ResultsEventWriter* writer_;
	friend class ResultsEventWriter;
private:
	// no need for copy and assignment
	Results(const Results&);
	Results &operator=(const Results &);
	static XmlEventReader::XmlEventType mapToEtype(short nodeType);
};

/**
 * Lazily evaluated results
 */
class LazyResults : public Results
{
public:
	LazyResults(XmlManager &mgr, Transaction *txn) : Results(mgr, txn) {}
	virtual void add(const XmlValue &value);
	virtual void add(const XmlValueVector &vv);
	virtual size_t size() const;
	virtual bool isLazy() const { return true; }
};

/**
 * Results from a lazily evaluated expression
 */
class LazyDIResults : public LazyResults
{
public:
	LazyDIResults(QueryContext *context, Value *contextItem, QueryExpression &expr, Transaction *txn, u_int32_t flags);
	~LazyDIResults();

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();
	virtual std::map<int, XmlContainer> getContainers() const;

protected:
	XmlQueryContext context_;
	XmlQueryExpression expr_;
	XmlValue contextItem_;
	bool lazyDocs_;

	HighResTimer timer_;
	ReferenceMinder evaluationMinder_;
	QueryExecutionContext qec_;
	DbXmlConfiguration conf_;

	DynamicContext *xqc_;
	Result result_;
	Item::Ptr nextItem_;
};

/**
 * Results from a lazily manifested index data
 */
class LazyIndexResults : public LazyResults
{
public:
	LazyIndexResults(Container &container,
			 QueryContext *context,
			 Transaction *txn, const Index &index,
			 const IndexLookup &il,
			 u_int32_t flags);

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();
private:
	virtual int doNext(XmlValue &value, bool isPeek);

protected:
	XmlQueryContext context_;
	DbXmlConfiguration conf_;
	XmlContainer container_;
	
	DbWrapper::Operation lowOp_;
	Key lowKey_;
	DbWrapper::Operation highOp_;
	Key highKey_;
	bool reverse_;
	bool docOnly_;
	bool cacheDocuments_;

	ScopedPtr<IndexCursor> cursor_;
	IndexEntry ie_;
	ReferenceMinder evaluationMinder_;
};

/**
 * Results to hold an eagerly evaluated set of values
 */
class DBXML_EXPORT ValueResults : public Results
{
public:
	ValueResults(XmlManager &mgr, Transaction *t = 0);
	ValueResults(const ValueResults &o, XmlManager &mgr,
		     Transaction *t = 0);
	ValueResults(const XmlValue &value, XmlManager &mgr,
		     Transaction *t = 0);
	ValueResults(Results *resultsToAdopt, XmlManager &mgr,
		     Transaction *t = 0);
	virtual ~ValueResults();

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();
	virtual void add(const XmlValue &value);
	virtual void add(const XmlValueVector &vv);
	virtual size_t size() const;

private:
	virtual int doNext(XmlValue &value, bool isPeek);
	void init(XmlResults &results);
protected:
	// For Eager evaluation we copy the values into a vector.
	DbXml::XmlValueVector vv_;
	DbXml::XmlValueVector::iterator *vvi_;
};

}

#endif

