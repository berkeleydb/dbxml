//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// $Id: Document.hpp,v 86f215ae8bc4 2008/08/20 17:42:30 john $
//

#ifndef __DOCUMENT_HPP
#define	__DOCUMENT_HPP

#include <vector>
#include <string>
#include <map>
#include "dbxml/XmlValue.hpp"
#include "ReferenceCounted.hpp"
#include "DocID.hpp"
#include "ScopedDbt.hpp"
#include "SharedPtr.hpp"
#include "Value.hpp"
#include "MetaDatum.hpp"
#include "Container.hpp"
#include <dbxml/XmlInputStream.hpp>
#include "OperationContext.hpp"
#include "CacheDatabaseMinder.hpp"
#include "dataItem/DbXmlConfiguration.hpp"

#include <xercesc/parsers/XercesDOMParser.hpp>

namespace DbXml
{

class Container;
class DbtOut;
class OperationContext;
class QueryContext;
class NsDocument;
class NsDomElement;
class NsPushEventSource;
class EventReader;
class EventWriter;
class NsEventTranslator;
class ReferenceMinder;
class XmlEventReader;
class DocDatabase;
class DictionaryDatabase;
class DbWrapper;
class NsNode;
	
typedef std::vector<MetaDatum*> MetaData;

class DBXML_EXPORT MetaDataIterator : public ReferenceCounted
{
public:
	MetaDataIterator(Document *document);
	~MetaDataIterator();
	
	bool next(std::string &uri, std::string &name, XmlValue &value);
	void reset();
	
	bool previousIsModified() const;
	bool previousIsRemoved() const;
private:
	// no need for copy and assignment
	MetaDataIterator(const MetaDataIterator&);
	MetaDataIterator &operator=(const MetaDataIterator&);

	Document *document_;
	MetaData::const_iterator i_;
};
	
class DBXML_EXPORT Document : public ReferenceCounted
{
public:
 	enum Content {
		// treat these as flags
		NONE = 0,
		DBT = 1,
		INPUTSTREAM = 2,
		DOM = 4,
		READER = 8
	};
	enum Validation {
		VALIDATE = 1,
		DONT_VALIDATE = 2,
		WF_ONLY = 3
	};
public:
	Document(Manager &mgr);
	virtual ~Document();

	Document *createCopy() const;

	void setMetaDataFromContainer(Container *container,
				      const DocID &id, Transaction *txn,
				      u_int32_t flags);
	void setEagerMetaData();
	void setEagerContentAndMetaData();

	void setContainer(Container *);
	void setContainerID(int cid) { cid_ = cid; }
	int getContainerID() const { return cid_; }
	std::string getContainerName() const;
	bool isWholedoc() const;
        bool isDocumentIndex() const;

	DictionaryDatabase *getDictionaryDB() const;
	
	void setValidation(int validation) {
		validation_ = validation;
	}

	/**
	 * Set the ID of a document. The ID is assigned when the document
	 * is first written to a container.
	 */
	void setID(const DocID &id) {
		id_ = id;
	}
	const DocID &getID() const {
		return id_;
	}
	DocID &getIDToSet() {
		return id_;
	}

	Manager &getManager() const { return (Manager&)mgr_; }

	const XMLCh *getDocumentURI() const;
	void setDocumentURI(const XMLCh *uri);

	void addReferenceMinder(ReferenceMinder *r);
	void removeReferenceMinder(ReferenceMinder *r);

	// Name
	void setName(const std::string &name, bool modified);
	std::string getName() const;
	void clearModified(const Name &name);

	// Meta-Data
	bool containsMetaData(const Name &name) const;
	bool isMetaDataModified(const Name &name) const;

	// Meta-Data, as XmlValue
	void setMetaData(const Name &name, const XmlValue &value, bool modified);
	bool getMetaData(const Name &name, XmlValue &value);

	// Meta-Data, as Dbt
	void setMetaData(const Name &name, XmlValue::Type type,
			 const XmlData &value, bool modified);
	void setMetaData(const Name &name, XmlValue::Type type,
			 DbtOut **value, bool modified); // Note: Consumes value
	bool getMetaData(const Name &name, XmlData &value);

	// Remove Meta-Data item
	void removeMetaData(const Name &name);

	// Meta-Data, as MetaDatum
	const MetaDatum *getMetaDataPtr(const Name &name) const;
	void setMetaDataPtr(MetaDatum *mdp);
	MetaData::const_iterator metaDataBegin() const;
	MetaData::const_iterator metaDataEnd() const;

	// Consider removal; for now, these are needed/used
	void resetContentAsNsDom(bool resetDocDb) const;
	void resetContentAsDbt() const;
	
	// Content
	bool isContentModified() const;
	void setContentModified(bool modified) const
	{
		contentModified_ = modified;
	}
	Content getDefinitiveContent() const
	{
		return definitiveContent_;
	}

	// Content as Dbt
	void setContentAsDbt(DbtOut **value, bool setOnly = false); // Consumes value
	const DbXmlDbt *getContentAsDbt() const;

	// Content as InputStream
	void setContentAsInputStream(XmlInputStream **adopted_str);
	XmlInputStream *getContentAsInputStream() const; // donated to caller

	// Content as NsDom
	void changeContentToNsDom(const ISNVector *isns) const;
	NsDomElement *getContentAsNsDom(const ISNVector *isns,
					Transaction *txn) const;
	NsDomElement *getElement(const NsNid nid, const ISNVector *isns) const;
	NsDomElement *getElement(NsNode *node) const;

	// assign a given doc id and cache db toa document
	void setContentAsNsDom(const DocID &did, CacheDatabase *db);

	// Content as XmlEventReader
	void setContentAsEventReader(XmlEventReader &reader);
	XmlEventReader &getContentAsEventReader() const;
	
	// Content as XmlEventWriter
	void getContentAsEventWriter(XmlEventWriter &writer);


	bool isUninitialized() const; // mostly for debug

	bool isEmpty() const {
		return (definitiveContent_ == NONE && lazy_ == NEITHER);
	}
	
	void resetDocDb() const;


	DbWrapper *getDocDb();

	CacheDatabase *getCacheDatabase() { return (CacheDatabase *)docdb_; }

	NsDocument *getNsDocument() const {
		return nsDocument_;
	}

	// Content as events
	EventReader *getElementAsReader(NsDomElement *element) const;
	NsPushEventSource *getContentAsEventSource(
		Transaction *txn,
		bool needsValidation, // true on initial input (wholedoc only)
		bool nodeEvents, // Provide NsNode objects in the events
		bool useId = false // force use of ID, not definitiveContent_
		) const;

	void initNsObjects(DbWrapper *db, Transaction *txn) const;

	CacheDatabaseMinder &getDbMinder() {
		return dbMinder_;
	}

	// unconditionally set txn
	void setTransaction(Transaction *txn) const {
		oc_.set(txn);
	}
	
	void setDbMinder(CacheDatabaseMinder &minder) {
		dbMinder_ = minder;
	}
	void setDbMinderIfNull(CacheDatabaseMinder &minder) {
		if (dbMinder_.isNull()) dbMinder_ = minder;
	}
	OperationContext &getOperationContext() const {
		return oc_;
	}

	bool operator==(const Document &other) const;

	/* Used by RawNodeValue, where an empty document is created
		that has a document and container id, but is created
		as empty. LJF*/
	void setAsNotMaterialized();
private:
	// no need for copy and assignment
	Document(const Document&);
	Document &operator=(const Document &);

	void copyMetaData(Document &to) const;
	
	/** @name Content Conversion Methods */
	// @{

	void id2dbt() const;
	void id2dom(const ISNVector *isns) const;
	void id2stream() const;
	void id2reader() const;
	void dbt2stream(bool adopt=false) const;
	void dbt2dom() const;
	void dbt2reader() const;
	void dom2dbt() const;
	void dom2stream() const;
	void dom2reader() const;
	void stream2dom(const ISNVector *isns) const;
	void stream2dbt() const;
	void stream2reader() const;

	void reader2dbt() const;
	void reader2stream() const;

	bool hasNsDom() const;
	
	NsPushEventSource *id2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *dbt2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *dom2events(Transaction *txn, bool needsValidation,
		bool nodeEvents) const;
	NsPushEventSource *stream2events(Transaction *txn, bool needsValidation,
		bool nodeEvents, const ISNVector *isns) const;

	// @}

	void createNsObjects(bool resetDom = true) const;

private:
	/** @name Document ID attributes */

	mutable XmlManager mgr_;
	DocID id_;
	int cid_; // container id
	XMLCh *documentURI_;
	int validation_;
	mutable CacheDatabaseHandle docdb_;
	mutable CacheDatabaseMinder dbMinder_;

	typedef std::set<ReferenceMinder*> RefMinders;
	RefMinders refMinders_; // ReferenceMinder objects that point to us

	/** @name Metadata attributes */

	MetaData metaData_;

	/** @name Lazy document attributes */
	// The sense of this is reverse in that the flag says
	// what is "lazy" as in not yet materialized.  Also,
	// the NEITHER state is the initial state of the Document object,
	// where it has no definitiveContent_
	enum LazyMode {
		NEITHER, // means that content and m-d are materialized
		CONTENT, // means that content has not been materialized
		BOTH     // means that neither has been materialized
	};

	LazyMode lazy_;
	mutable OperationContext oc_;
	u_int32_t db_flags_;

	/** @name content management attributes */

	mutable Content definitiveContent_;
	mutable bool contentModified_;
	mutable bool consumed_;

	/** @name content as dbt attributes */

	mutable DbtOut *dbtContent_;

	/** @name content as input stream attributes */

	mutable XmlInputStream *inputStream_;

	/** @name content as input stream attributes */

	mutable XmlEventReader *reader_;

	/** @name content as NsDom attributes */

	mutable NsDocument *nsDocument_;

	// allow DocumentDatabase to access private methods
	friend class DocumentDatabase;
};

}

#endif

