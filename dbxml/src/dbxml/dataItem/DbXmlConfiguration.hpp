//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLCONFIGURATION_HPP
#define	__DBXMLCONFIGURATION_HPP

#include <xqilla/simple-api/XQillaConfiguration.hpp>
#include <xqilla/context/MessageListener.hpp>

#include "DbXmlURIResolver.hpp"
#include "../OperationContext.hpp"
#include "../MutexLock.hpp"
#include "../StructuralStatsDatabase.hpp"
#include "../CacheDatabaseMinder.hpp"

#include <map>

#define GET_CONFIGURATION(x) (static_cast<DbXmlConfiguration*>((x)->getConfiguration()))

class DatatypeLookup;

namespace DbXml
{

class QueryContext;
class DecisionPointQP;
class DecisionPointIterator;
class BufferSource;
class Transaction;
class QueryExecutionContext;
class CacheDatabaseMinder;
class ReferenceMinder;
class ImpliedSchemaNode;
class Document;
class XmlStackFrame;

typedef std::vector<const ImpliedSchemaNode*> ISNVector;
typedef std::map<std::string, ISNVector> ISNStringMap;

struct ISNMapValue
{
	ISNVector schemas;
	ISNStringMap docMap;
};

typedef std::map<int, ISNMapValue> ISNMap;

struct ProjectionInfo
{
	ProjectionInfo() : override(false) {}

	bool override;
	ISNMap schemas;
	ISNStringMap uriSchemas;
};

struct CompileInfo
{
	CompileInfo();
	~CompileInfo();

	dbxml_mutex_t mutex;
	unsigned int bufferId;
	unsigned int tmpVarCounter;
};

class DbXmlConfiguration : public XQillaConfiguration, public MessageListener
{
public:
	DbXmlConfiguration(QueryContext &qc, Transaction *txn, CompileInfo *ci, DatatypeLookup *dl = 0);
	virtual ~DbXmlConfiguration();

	virtual DocumentCache *createDocumentCache(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);
	virtual SequenceBuilder *createSequenceBuilder(const DynamicContext *context);
	virtual ItemFactory *createItemFactory(DocumentCache *cache,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);
	virtual UpdateFactory *createUpdateFactory(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);
	virtual URIResolver *createDefaultURIResolver(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);
	virtual void populateStaticContext(StaticContext *context);
	virtual void populateDynamicContext(DynamicContext *context);
	virtual void testInterrupt();

	virtual void warning(const XMLCh *message, const LocationInfo *location);
	virtual void trace(const XMLCh *label, const Sequence &sequence, const LocationInfo *location,
		const DynamicContext *context);

	dbxml_mutex_t getMutex() const { return compileInfo_->mutex; }

	QueryContext &getQueryContext() const { return qc_; }

	DecisionPointQP *getDecisionPoint() const { return dp_; }
	void setDecisionPoint(DecisionPointQP *dp) { dp_ = dp; }

	DecisionPointIterator *getDecisionPointIterator() const { return dpi_; }
	void setDecisionPointIterator(DecisionPointIterator *dpi) { dpi_ = dpi; }

	BufferSource *getBufferSource() const { return buffers_; }
	void setBufferSource(BufferSource *b) { buffers_ = b; }

	XmlStackFrame *getStackFrame() const { return frame_; }
	void setStackFrame(XmlStackFrame *f) { frame_ = f; }

	unsigned int allocateBufferID();
	const XMLCh *allocateTempVarName(XPath2MemoryManager *mm);

	StructuralStatsCache &getStatsCache() { return statsCache_; }

	void getImpliedSchemaNodes(const Document *document, ISNVector &result) const;
	void getImpliedSchemaNodes(const std::string &documentURI, ISNVector &result) const;
	void addImpliedSchemaNode(int containerId, const char *docName, const ImpliedSchemaNode *isn);
	void addImpliedSchemaNode(int containerId, const ImpliedSchemaNode *isn);
	void addImpliedSchemaNode(const std::string &documentURI, const ImpliedSchemaNode *isn);

	void overrideProjection() { pi_->override = true; }
	void setProjectionInfo(ProjectionInfo *pi) { pi_ = pi; }

	OperationContext &getOperationContext() { return oc_; }
	Transaction *getTransaction() { return oc_.txn(); }
	// gmf void setTransaction(Transaction *txn);
	QueryExecutionContext *getQueryExecutionContext() { return qec_; }
	void setQueryExecutionContext(QueryExecutionContext *qec) { qec_ = qec; }
	ReferenceMinder *getMinder() { return minder_; }
	void setMinder(ReferenceMinder *minder) { minder_ = minder; }
	void setDbMinder(CacheDatabaseMinder &dbMinder) {
		dbMinder_ = dbMinder;
	}
	CacheDatabaseMinder &getDbMinder() { return dbMinder_; }
	u_int32_t getFlags() const { return db_flags_; }
	void setFlags(u_int32_t flags) { db_flags_ = flags; }
	const std::string getDefaultCollection();
	XmlManager &getManager();
	
protected:
	DbXmlURIResolver resolver_;
	DatatypeLookup *datatypeLookup_;

	QueryContext &qc_;
	DecisionPointQP *dp_;
	DecisionPointIterator *dpi_;

	BufferSource *buffers_;

	XmlStackFrame *frame_;

	StructuralStatsCache statsCache_;

	// Evalution context -- these are owned by other objects
	OperationContext oc_;
	QueryExecutionContext *qec_;
	ReferenceMinder *minder_;
	CacheDatabaseMinder dbMinder_;
	u_int32_t db_flags_;

	ProjectionInfo *pi_;
	CompileInfo *compileInfo_;
};

class AutoDecisionPointReset
{
public:
	AutoDecisionPointReset(DbXmlConfiguration *conf, DecisionPointQP *dp)
	{
		conf_ = conf;
		oldDP_ = conf->getDecisionPoint();
		conf->setDecisionPoint(dp);
	}

	~AutoDecisionPointReset()
	{
		conf_->setDecisionPoint(oldDP_);
	}

protected:
	DbXmlConfiguration *conf_;
	DecisionPointQP *oldDP_;
};

class AutoDecisionPointIteratorReset
{
public:
	AutoDecisionPointIteratorReset(DbXmlConfiguration *conf, DecisionPointIterator *dp)
	{
		conf_ = conf;
		oldDP_ = conf->getDecisionPointIterator();
		conf->setDecisionPointIterator(dp);
	}

	~AutoDecisionPointIteratorReset()
	{
		conf_->setDecisionPointIterator(oldDP_);
	}

protected:
	DbXmlConfiguration *conf_;
	DecisionPointIterator *oldDP_;
};

class AutoBufferSourceReset
{
public:
	AutoBufferSourceReset(DbXmlConfiguration *conf, BufferSource *buf)
	{
		conf_ = conf;
		oldBuf_ = conf->getBufferSource();
		conf->setBufferSource(buf);
	}

	~AutoBufferSourceReset()
	{
		conf_->setBufferSource(oldBuf_);
	}

protected:
	DbXmlConfiguration *conf_;
	BufferSource *oldBuf_;
};

class AutoXmlStackFrameReset
{
public:
	AutoXmlStackFrameReset(DbXmlConfiguration *conf, XmlStackFrame *frame)
	{
		conf_ = conf;
		oldFrame_ = conf->getStackFrame();
		conf->setStackFrame(frame);
	}

	~AutoXmlStackFrameReset()
	{
		conf_->setStackFrame(oldFrame_);
	}

protected:
	DbXmlConfiguration *conf_;
	XmlStackFrame *oldFrame_;
};

}

#endif
