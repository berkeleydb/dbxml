//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SEQUENTIALSCANQP_HPP
#define	__SEQUENTIALSCANQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"
#include "../Cursor.hpp"
#include "../CacheDatabase.hpp"
#include "../NameID.hpp"
#include "../nodeStore/NsNode.hpp" // for nsAttr_t

namespace DbXml
{

class DocumentDatabase;
class DocDatabase;
class DictionaryDatabase;
	
class SequentialScanQP : public QueryPlan
{
public:
	SequentialScanQP(ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm);
	SequentialScanQP(ImpliedSchemaNode::Type type, ImpliedSchemaNode *isn,
		ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm);

	const char *getChildName() const;
	ImpliedSchemaNode::Type getNodeType() const { return nodeType_; }
	ImpliedSchemaNode *getImpliedSchemaNode() const { return isn_; }
	ContainerBase *getContainerBase() const { return container_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;
	virtual StructuralStats getStructuralStats(OperationContext &oc, StructuralStatsCache &cache,
		const NameID &nameID, bool ancestor) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	static StructuralStats getStructuralStats(ContainerBase *container, const char *childUriName,
		OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor);

protected:
	ImpliedSchemaNode::Type nodeType_;
	ImpliedSchemaNode *isn_;
	NameID nameid_;

	ContainerBase *container_;

	mutable NameID nsUriID_; // uri ID for the "xmlns" prefix
	mutable Cost cost_;
};

class ElementSSIterator : public NsNodeIterator
{
public:
	ElementSSIterator(DbWrapper *docDb, ContainerBase *container,
			  DynamicContext *context, const LocationInfo *location);
	ElementSSIterator(ContainerBase *container,
			  const LocationInfo *location);
	int open(DbWrapper *db, Transaction *txn);
	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
	
	virtual Type getType() const;
	virtual int32_t getNodeURIIndex();
	virtual const xmlbyte_t *getNodeName();
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);
	
	static DbXmlNodeImpl::Ptr getDbXmlNodeImpl(
		XmlDocument &doc, DocID &did,
		DbtOut &key, DbtOut &data, DynamicContext *context);

protected:
	bool unmarshal(DbtOut &key, DbtOut &data);
	DbXmlNodeImpl::Ptr getDbXmlNodeImpl(XmlDocument &doc, DynamicContext *context);
	
	Cursor cursor_;
	DbtOut key_;
	DbtOut data_;
	DbtOut tmp_;
};

class AttributeSSIterator : public ElementSSIterator
{
public:
	AttributeSSIterator(DbWrapper *docDb, ContainerBase *container,
			    DynamicContext *context,
			    const LocationInfo *location,
			    const NameID &nsUriID);
	
	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
	
	virtual Type getType() const;
	virtual int32_t getNodeURIIndex();
	virtual const xmlbyte_t *getNodeName();
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);
	
protected:
	u_int32_t index_;
	NameID nsUriID_;
	nsAttr_t attr_;
	const unsigned char *current_;
};

class DLSElementSSIterator : public ElementSSIterator
{
public:
	DLSElementSSIterator(DocumentDatabase *ddb, DictionaryDatabase *dict,
			     ContainerBase *container,
			     DynamicContext *context,
			     const LocationInfo *location);
	~DLSElementSSIterator();
	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did,
			  const NsNid &nid, DynamicContext *context);
protected:
	bool nextDoc(DynamicContext *context);
	int materializeDoc(DynamicContext *context);
	void resetCursor(const NsNid &nid);
	u_int32_t getCursorFlag() {
		if (useSet_) {
			useSet_ = false; return DB_SET_RANGE;
		} else return DB_NEXT;
	}
protected:
	ScopedPtr<DocumentCursor> docCursor_;
	DocumentDatabase *ddb_;
	DbWrapper *content_;
	CacheDatabaseHandle docdb_;
	DictionaryDatabase *dict_;
	DocID curDid_;
	DbtOut docKey_;
	DbtOut docData_;
	bool useSet_;
};

class DLSAttributeSSIterator : public DLSElementSSIterator
{
public:
	DLSAttributeSSIterator(DocumentDatabase *ddb,
			       DictionaryDatabase *dict,
			       ContainerBase *container,
			       DynamicContext *context,
			       const LocationInfo *location,
			       const NameID &nsUriID);
	
	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did,
			  const NsNid &nid, DynamicContext *context);
	
	virtual Type getType() const;
	virtual int32_t getNodeURIIndex();
	virtual const xmlbyte_t *getNodeName();
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);
	
protected:
	u_int32_t index_;
	NameID nsUriID_;
	nsAttr_t attr_;
	const unsigned char *current_;
};
}
#endif
