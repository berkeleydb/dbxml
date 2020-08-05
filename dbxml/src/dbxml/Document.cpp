//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// $Id: Document.cpp,v 86f215ae8bc4 2008/08/20 17:42:30 john $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlData.hpp"
#include "Document.hpp"
#include "CacheDatabase.hpp"
#include "OperationContext.hpp"
#include "UTF8.hpp"
#include "Value.hpp"
#include "MetaDatum.hpp"
#include "Manager.hpp"
#include "ReferenceMinder.hpp"
#include "MemBufInputStream.hpp"
#include "ScopedDbt.hpp"
#include "nodeStore/NsEventReader.hpp"
#include "nodeStore/EventReaderToWriter.hpp"
#include "nodeStore/NsDocument.hpp"
#include "nodeStore/NsSAX2Reader.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsXercesIndexer.hpp"
#include "nodeStore/NsXercesTranscoder.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/NsDocumentDatabase.hpp"
#include "nodeStore/PullEventInputStream.hpp"
#include "nodeStore/NsImpliedSchemaFilter.hpp"
#include "dataItem/DbXmlUri.hpp"
#include <dbxml/XmlEventReaderToWriter.hpp>
#include "Globals.hpp"

#include "db_utils.h"

#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

/*********************************************************
 * Document
 *
 * This object is a bit more complex than desirable, but for
 * now, that's the way it is.  Here are some rules and
 * state transitions that occur.
 *
 * 
 *
 *********************************************************/

static inline void consumed(const string &name, bool consumed)
{
	if(consumed){
		throw XmlException(XmlException::INVALID_VALUE, 
			"The XmlEventReader or XmlInputStream that was used to set the content of document " + name + 
			" has been consumed, you must set the document content again before you can access it.");
	}
}

Document::Document(Manager &mgr)
	: mgr_(mgr),
	  id_(0),
	  cid_(0),
	  documentURI_(0),
	  validation_(DONT_VALIDATE),
	  lazy_(NEITHER),
	  db_flags_(0),
	  definitiveContent_(NONE),
	  contentModified_(false),
	  consumed_(false),
	  dbtContent_(0),
	  inputStream_(0),
	  reader_(0),
	  nsDocument_(0) 
{
}

Document::~Document()
{
	if(!refMinders_.empty()) {
		RefMinders::iterator i;
		for(i = refMinders_.begin(); i != refMinders_.end();++i)
			(*i)->removeDocument(this);
		refMinders_.clear();
	}
	setDocumentURI(0);
	oc_.set(0);
	MetaData::iterator i;
	for(i = metaData_.begin();i != metaData_.end();++i) {
		delete *i;
	}
	metaData_.clear();
	resetDocDb();
	if (dbtContent_)
		delete dbtContent_;
	if (inputStream_)
		delete inputStream_;
	if (nsDocument_)
		delete nsDocument_;
	if (reader_)
		reader_->close();
}

bool Document::operator==(const Document &other) const
{
	// if doc IDs are 0, this is a transient doc,
	// so compare pointers; otherwise, use IDs
	if (id_ == 0)
		return (this == &other);
	return ((id_ == other.id_) &&
		(cid_ == other.cid_));
}

Document *Document::createCopy() const
{
	Document *doc = new Document(mgr_);
	doc->setID(getID());
	doc->setContainerID(cid_);
	copyMetaData(*doc);
	return doc;
}

void Document::copyMetaData(Document &doc) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		const DbXmlDbt *dbt = (*i)->getDbt();
		DbtOut *ddbt = new DbtOut(*dbt);
		MetaDatum *newmd = new MetaDatum((*i)->getName(), (*i)->getType(),
						 &ddbt, (*i)->isModified());
		doc.setMetaDataPtr(newmd);
	}
}

const XMLCh *Document::getDocumentURI() const
{
	if(documentURI_ == 0) {
		string contName = getContainerName();
		string docName = getName();

		if(contName != "" && docName != "" &&
			docName.find('/') == string::npos) {
			XMLBuffer buf;
			buf.append(DbXmlUri::dbxmlScheme16);
			buf.append(':');
			buf.append('/');
			buf.append('/');
			buf.append('/');
			buf.append(UTF8ToXMLCh(contName).str());
			buf.append('/');
			buf.append(UTF8ToXMLCh(docName).str());

			if (XPath2Utils::isValidURI(buf.getRawBuffer(),
						   Globals::defaultMemoryManager)) {
				const_cast<XMLCh*&>(documentURI_) =
					NsUtil::nsStringDup(
						buf.getRawBuffer(), 0);
			}
		}
	}

	return documentURI_;
}

void Document::setDocumentURI(const XMLCh *uri)
{
	if(documentURI_)
		NsUtil::deallocate(documentURI_);
	documentURI_ = NsUtil::nsStringDup(uri, 0);
}

void Document::addReferenceMinder(ReferenceMinder *r)
{
	DBXML_ASSERT(r != 0);
	refMinders_.insert(r);
}

void Document::removeReferenceMinder(ReferenceMinder *r)
{
	DBXML_ASSERT(r != 0);
	refMinders_.erase(r);
}

void Document::setMetaDataFromContainer(Container *container,
					const DocID &id, Transaction *txn,
					u_int32_t flags)
{
	// Set up lazy meta data
	setContainer(container);
	setID(id);
	oc_.set(txn);
	db_flags_ = flags & ~DBXML_LAZY_DOCS;
	lazy_ = BOTH;

	if((flags & DBXML_LAZY_DOCS) == 0) {
		setEagerMetaData();
	}
}

void Document::setEagerMetaData()
{
	if(lazy_ == BOTH) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		if(definitiveContent_ == NONE) {
			if(container->isWholedocContainer())
				id2dbt();
			// This was "else id2dom()" for NLS; however,
			// that makes the definitiveContent_ DOM,
			// which bypasses the most efficient way to
			// materialize NLS -- via NsDocumentDatabase::getContent
		}
		int err = container->getDocumentDB()->
			getAllMetaData(oc_, container->getDictionaryDatabase(),
				       this, db_flags_);
		if(err != 0)
			throw XmlException(err);
		// NLS docs are still lazy at this point
		if(container->isWholedocContainer()) {
			lazy_ = NEITHER;
		} else
			lazy_ = CONTENT;
	}
}

void Document::setEagerContentAndMetaData()
{
	// force materialization (as Dbt) of Node Storage documents.
	// NOTE: this will change definitiveContent_ to Dbt (from DOM).
	// If definitiveContent_ is already DOM, it will use the DOM to
	// generate events, rather than the raw document.  This means
	// that modifications, if they exist, will be seen.
	//
	setEagerMetaData();
	getContentAsDbt();
	lazy_ = NEITHER;
	definitiveContent_ = DBT;
}

void Document::setContainer(Container *container)
{
	if (container) {
		validation_ = (container->getDoValidation()) ? VALIDATE :
			DONT_VALIDATE;
		cid_ = container->getContainerID();
	} else {
		validation_ = DONT_VALIDATE;
		cid_ = 0;
	}
}

void Document::setMetaData(const Name &name, const XmlValue &value, bool modified)
{
	switch (value.getType()) {
	case XmlValue::NODE:
		throw XmlException(XmlException::INVALID_VALUE,
				   "setMetaData expects a typed value, not a node");
		break;
	case XmlValue::NONE:
		throw XmlException(XmlException::INVALID_VALUE,
				   "setMetaData expects a typed value");
		break;
	case XmlValue::BINARY: {
		DbtOut *ddbt = new DbtOut(value.asBinary().get_data(), value.asBinary().get_size());
		setMetaDataPtr(new MetaDatum(name, value.getType(),
					     &ddbt, modified));
		return;
	}
	default:
		// one of the atomic types
		break;
	}
	string v(value.asString());
	DbtOut *dbt = new DbtOut((void*)v.c_str(), v.length() + 1); // Include the terminating 0
	setMetaDataPtr(new MetaDatum(name, value.getType(), &dbt, modified));
}

void Document::setMetaData(const Name &name, XmlValue::Type type, const XmlData &value, bool modified)
{
	DbtOut *dbt = new DbtOut(value.get_data(), value.get_size());
	setMetaData(name, type, &dbt, modified);
}

void Document::setMetaData(const Name &name, XmlValue::Type type,
			   DbtOut **value, bool modified) // Note: Consumes value
{
	setMetaDataPtr(new MetaDatum(name, type, value, modified)); // Note: Consumes Dbt.
}

void Document::setMetaDataPtr(MetaDatum *pmd)
{
	MetaData::iterator i;
	for (i = metaData_.begin();i != metaData_.end();++i) {
		if ((*i)->getName() == pmd->getName()) {
			(*i)->setDbt(pmd);
			return;
		}
	}
	metaData_.push_back(pmd);
}

bool Document::getMetaData(const Name &name, XmlValue &value)
{
	const MetaDatum *md = getMetaDataPtr(name);
	if (md != 0) {
		if(md->getDbt() == 0) return false;
		try {
			md->asValue(value);
		}
		catch(XmlException &e) {
			return false;  // cannot convert binary metadata to an XmlValue
		}
		return true;
	}

	return false;
}

bool Document::getMetaData(const Name &name, XmlData &value)
{
	const MetaDatum *md = getMetaDataPtr(name);
	if (md != 0) {
		const DbXmlDbt *dbt = md->getDbt();
		if(dbt == 0) return false;
		value.set(dbt->data, dbt->size);
		return true;
	}
	return false;
}

void Document::removeMetaData(const Name &name)
{
	MetaData::iterator i;
	bool removed = false;
	while (!removed) {
		for(i = metaData_.begin(); i != metaData_.end(); ++i) {
			if((*i)->getName() == name) {
				if (name == Name::dbxml_colon_name) {
					throw XmlException(
						XmlException::INVALID_VALUE,
						"You cannot remove the name metadata item");
				}
				(*i)->setRemoved();
				removed = true;
				break;
			}
		}
		if (!removed && (lazy_ == BOTH)) {
			// create a "removed" item for this name
			// type doesn't matter for removal
			XmlValue val("x");
			setMetaData(name, val, true);
		} else
			break;
	}
}

bool Document::containsMetaData(const Name &name) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			return (*i)->getDbt() != 0;
		}
	}
	return false;
}

bool Document::isMetaDataModified(const Name &name) const
{
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			return (*i)->isModified();
		}
	}
	return false;
}

const MetaDatum *Document::getMetaDataPtr(const Name &name) const
{
	const MetaDatum *r = 0;
	MetaData::const_iterator i;
	for(i = metaData_.begin(); i != metaData_.end(); ++i) {
		if((*i)->getName() == name) {
			if (!(*i)->isRemoved()) {
				r = *i;
			}
			// doc may be lazy_
			return r;
		}
	}
	if(r == 0 && lazy_ == BOTH) {
		// If we can't find it, then try looking it up in the database
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		int err = 0;
		DbtOut *data = new DbtOut();
		try {
			XmlValue::Type type;
			err = container->getDocumentDB()->
				getMetaData(oc_,
					    container->getDictionaryDatabase(),
					    name,
					    id_, type, data, db_flags_);
			if(err == 0) const_cast<Document*>(this)->
					     setMetaData(name, type, &data, /*modified*/false);
			delete data; // If not consumed by setThisFromDbt
		}
		catch(...) {
			delete data; // If not consumed by setThisFromDbt
			throw;
		}

		if(err == DB_NOTFOUND) {
			// Add a dummy MetaDatum, to avoid doing the database lookup again
			const_cast<Document*>(this)->setMetaDataPtr(new MetaDatum(name, XmlValue::NONE));
			err = 0;
		}
		else if(err != 0) {
			throw XmlException(err);
		}

		r = metaData_.back(); // The added MetaDatum will be last in the vector
	}
	return r;
}

string Document::getName() const
{
	const MetaDatum *name = getMetaDataPtr(Name::dbxml_colon_name);
	return name != 0 ? (name->getValue() != 0 ? name->getValue() : "") : "";
}

string Document::getContainerName() const
{
	ScopedContainer sc(getManager(), cid_, false);
	if (sc.getContainer())
		return sc.getContainer()->getName();
	return "";
}

bool Document::isWholedoc() const
{
	ScopedContainer sc(getManager(), cid_, false);
	if (sc.getContainer())
		return sc.getContainer()->isWholedocContainer();
	return false;
}

bool Document::isDocumentIndex() const
{
	ScopedContainer sc(getManager(), cid_, false);
	if (sc.getContainer())
		return !sc.getContainer()->nodesIndexed();
	return false;
}

DictionaryDatabase *Document::getDictionaryDB() const
{
	ScopedContainer sc(getManager(), cid_, false);
	if (sc.get())
		return sc.get()->getDictionaryDatabase();
	return getManager().getDictionary();
}

void Document::setName(const string &name, bool modified)
{
	DbtOut *dbt = new DbtOut(name.c_str(), name.length() + 1); // Store the trailing '\0'
	setMetaDataPtr(new MetaDatum(Name::dbxml_colon_name, XmlValue::STRING, &dbt, modified)); // Note: Consumes Dbt.
}

void Document::clearModified(const Name &name)
{
	MetaData::iterator i;
	for (i = metaData_.begin();i != metaData_.end();++i) {
		if ((*i)->getName() == name) {
			(*i)->setModified(false);
			return;
		}
	}
}

// Note: Consumes Dbt.
void Document::setContentAsDbt(DbtOut **value, bool setOnly)
{
	DBXML_ASSERT(isUninitialized() || setOnly);

	if (dbtContent_) 
		delete dbtContent_;

	dbtContent_ = *value;
	*value = 0;
	if (setOnly)
		return;
	definitiveContent_ = DBT;
	contentModified_ = true;
}

const DbXml::DbXmlDbt *Document::getContentAsDbt() const
{
	switch(definitiveContent_) {
	case DBT: break;
	case INPUTSTREAM:
	{
		if(!inputStream_) break;
		stream2dbt(); 
		break;
	}
	case DOM: dom2dbt(); break;
	case READER: 
	{
		if(!reader_) break;
		reader2dbt(); break;
	}
	case NONE: id2dbt(); break;
	}

	if(!dbtContent_ ) consumed(getName(), consumed_);
	
	// definitiveContent does not change, Dbt is a cache
		
	return dbtContent_;
}

void Document::setContentAsInputStream(XmlInputStream **adopted_str)
{
	DBXML_ASSERT(isUninitialized());
	inputStream_ = *adopted_str;
	*adopted_str = 0;
	definitiveContent_ = INPUTSTREAM;
	contentModified_ = true;
}

void Document::setContentAsEventReader(XmlEventReader &reader)
{
	DBXML_ASSERT(isUninitialized());
	reader_ = &reader;
	definitiveContent_ = READER;
	contentModified_ = true;
}

// the object is donated to caller.  This is because
// input stream objects cannot be reset, so they are
// "one-use only"
XmlInputStream *Document::getContentAsInputStream() const
{
	switch(definitiveContent_) {
	case DBT:
	{
		if(dbtContent_ && dbtContent_->size)
			dbt2stream(true); // hand off dbtContent_
		else
			inputStream_ = new MemBufInputStream(0, 0, getName().c_str(),false);
		break;
	}
	case INPUTSTREAM: break;
	case DOM: dom2stream(); break;
	case READER: reader2stream(); break;
	case NONE: id2stream(); break;
	}

	XmlInputStream *ret = inputStream_;
	inputStream_ = 0;
	if(definitiveContent_ == INPUTSTREAM){
		consumed_ = true;
		definitiveContent_ = NONE;
	}
	if(!ret){
		consumed(getName(), consumed_);
		ret = new MemBufInputStream(0, 0, getName().c_str(),false);
	}
	if(definitiveContent_ == DBT) {
		DBXML_ASSERT(dbtContent_ == 0);
		definitiveContent_ = NONE;
	}
	return ret;
}

void Document::resetContentAsNsDom(bool resetDb) const
{
	if (nsDocument_)
		delete nsDocument_;
	if (resetDb)
		resetDocDb();
	nsDocument_ = 0;
}

void Document::resetContentAsDbt() const
{
	if (dbtContent_) {
		delete dbtContent_;
		dbtContent_ = 0;
		if (definitiveContent_ == DBT)
			definitiveContent_ = NONE;
	}
}

void Document::resetDocDb() const {
	docdb_ = 0;
}

DbWrapper * Document::getDocDb() {
	if (!docdb_.isNull())
		return docdb_.getDb();
	if (nsDocument_)
		return nsDocument_->getDocDb();
	if (!dbMinder_.isNull()) {
		CacheDatabase *cdb =
			dbMinder_.findOrAllocate(
				(Manager&)mgr_, cid_);
		if (cdb) {
			docdb_ = cdb;
			return cdb->getDb();
		}
	}
	ScopedContainer sc(getManager(), cid_, true);
	return sc.get()->getDbWrapper();
}
	
void Document::createNsObjects(bool resetDom) const
{
	if (resetDom)
		resetContentAsNsDom(true);
	DictionaryDatabase *ddb;
	ScopedContainer sc(getManager(), cid_, true);
	ddb = sc.get()->getDictionaryDatabase();
	nsDocument_ = new NsDocument(this);
	nsDocument_->initNsDom(id_, ddb);
}

void Document::initNsObjects(DbWrapper *db, Transaction *txn) const
{
	DBXML_ASSERT(nsDocument_);
	ScopedContainer sc(getManager(), cid_, true);
	ContainerBase *container = sc.get();
	DictionaryDatabase *ddb = container->getDictionaryDatabase();
	nsDocument_->initDoc(txn, db, ddb, id_, cid_, db_flags_);
}

void Document::setAsNotMaterialized() { 
	lazy_ = BOTH; 
}

void Document::changeContentToNsDom(const ISNVector *isns) const
{
	switch(definitiveContent_) {
	case DBT: dbt2dom(); break;
	case INPUTSTREAM: stream2dom(isns); break;
	case DOM: break;
	case READER:
	{
		reader2stream();
		stream2dom(isns);
		break;
	}
	case NONE: id2dom(isns); break;
	}
}

// Transaction is passed in case the document does not yet have
// one set.
NsDomElement *Document::getContentAsNsDom(const ISNVector *isns,
					  Transaction *txn) const
{
	oc_.set(txn);
	changeContentToNsDom(isns);

	if(nsDocument_ == 0) {
		if (definitiveContent_ == NONE) // no-content doc
			return NULL;
		// empty/blank NsDom, need NsDocument and a document node
		createNsObjects();
		NsNode *node = NsNode::allocNode(0, NS_STANDALONE);
		node->setIsDocument();
		node->setNoNav();
		NsNidGen gen;
		gen.nextId(node->getFullNid());
		nsDocument_->setDocumentNode(node);
	}
	return nsDocument_->getDocumentNode();
}

NsDomElement *Document::getElement(const NsNid nid, const ISNVector *isns) const
{
	changeContentToNsDom(isns);

	if (!nsDocument_) return 0; //empty document
	if (nid.isDocRootNid())
		return nsDocument_->getDocumentNode();
	NsNode *nsNode = nsDocument_->getNode(nid, /*getNext*/false);
	if(nsNode)
		return new NsDomElement(nsNode, nsDocument_);
	throw XmlException(DB_NOTFOUND);
	return 0;
}

NsDomElement *Document::getElement(NsNode *node) const
{
	changeContentToNsDom(0); // No document projection
	return new NsDomElement(node, nsDocument_);
}

// this is *only* used by DbXmlSequenceBuilder to construct
// XML in a query -- it always uses a fresh Document object
void Document::setContentAsNsDom(const DocID &did, CacheDatabase *db)
{
	if (definitiveContent_ == DOM)
		return;
	DBXML_ASSERT(isUninitialized());
	docdb_ = db;
	id_ = did;
	createNsObjects(false); // don't reset DOM, it'll clear the DB
	initNsObjects(docdb_.getDb(), NULL); // no txn
	definitiveContent_ = DOM;
	contentModified_ = false;
}

EventReader *Document::getElementAsReader(NsDomElement *element) const
{
	ScopedContainer sc(getManager(), cid_, false);
	ContainerBase *container = sc.get();
	DBXML_ASSERT(element->getNsDoc()->getDocID() == id_);
	// Otherwise get the data directly from the database
	DbWrapper *db = container->getDbWrapper();
	if (!db) {
		db = docdb_.getDb();
		DBXML_ASSERT(db);
	}
	NsNid elnid = element->getNodeId();
	return new NsEventReader(
		oc_.txn(),
		db,
		container->getDictionaryDatabase(),
		id_, cid_, db_flags_, NS_EVENT_VALUE_BUFSIZE,
		&elnid, docdb_);
}

NsPushEventSource *Document::getContentAsEventSource(
	Transaction *txn,
	bool needsValidation,
	bool nodeEvents,
	bool useID) const
{
	Content src = (useID ? NONE : definitiveContent_);
	switch (src) {
	case DBT: return dbt2events(txn, needsValidation, nodeEvents);
	case INPUTSTREAM: return stream2events(txn, needsValidation,
					       nodeEvents, 0);
	case DOM: return dom2events(txn, needsValidation, nodeEvents);
	case READER:
	{
		DBXML_ASSERT(reader_);
		EventReaderToWriter *r2w = new EventReaderToWriter(
			*reader_,
			true, //owned
			false); //not internal
		reader_ = 0;
		definitiveContent_ = NONE;
		return r2w;
	}
	case NONE: return id2events(txn, needsValidation, nodeEvents);
	}
	return 0;
}

//
// Event translations...
//
XmlEventReader &Document::getContentAsEventReader() const
{
	switch(definitiveContent_) {
	case DBT: 
	{
		if(!dbtContent_ || !dbtContent_->size) break;
		dbt2reader(); 
		break;
	}
	case INPUTSTREAM: 
	{
		if(!inputStream_) break;
		stream2reader(); 
		break;
	}
	case DOM: dom2reader(); break;
	case READER: break;
	case NONE: id2reader(); break;
	}

	if(!reader_){
		consumed(getName(), consumed_);
		throw XmlException(XmlException::INVALID_VALUE,
			"Cannot create an XmlEventReader for the empty document "+ getName());
	}

	XmlEventReader *ret = reader_;
	reader_ = 0;
	if(definitiveContent_ == READER) {
		consumed_ = true;
		definitiveContent_ = NONE;
	}
	return *ret;
}

// write content to the writer, then close it
void Document::getContentAsEventWriter(XmlEventWriter &writer)
{
	// DBT or stream: parse directly to writer
	// DOM or READER: get XmlEventReader, use EventReaderToWriter.
	switch(definitiveContent_) {
	case DOM:
		dom2reader();
		break;
	case READER:
		break;
	case NONE:
	{
		id2reader();
		break;
	}
        // TBD: see below -- try to avoid intermediate reader
        // by writing parse events directly to the writer
	case DBT: dbt2reader(); break;
	case INPUTSTREAM: stream2reader(); break;
	default:
		break;
	}
	XmlEventReader *reader = reader_;
	if (reader)  {
		reader_ = 0;
		if(definitiveContent_ == READER)
			definitiveContent_ = NONE;
		XmlEventReaderToWriter r2w(*reader, writer, true);
		r2w.start();
	} else {
		// TBD: implement this -- it allows
		// the parse events to be written directly to the
		// writer, rather than using an intermediate reader
		// need to parse to events
		switch(definitiveContent_) {
		case DBT:
			dbt2stream();
			break;
		case INPUTSTREAM:
			break;
		case NONE:
			id2stream();
			break;
		default:
			DBXML_ASSERT(0);
			break;
		}
		// parse stream into writer
		DBXML_ASSERT(inputStream_ != 0);
		ScopedPtr<NsPushEventSource>
			pushSource(stream2events(
					   oc_.txn(),
					   false, // needs validation
					   false, 0)); // nodeEvents
	}
}

bool Document::isContentModified() const
{
	return contentModified_;
}

void Document::id2dbt() const
{
	if(lazy_ != NEITHER && dbtContent_ == 0) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		int err = container->getDocumentDB()->getContent(
			oc_, const_cast<Document*>(this), db_flags_);
		if(err != 0) {
			throw XmlException(err);
		}
		if (dbtContent_ && !(container->isNodeContainer())) {
			// no-content docs..
			definitiveContent_ = DBT;
		}
		// this method wipes out any possible
		// modifications that were present in memory
		contentModified_ = false;
	}
}

void Document::id2dom(const ISNVector *isns) const
{
	DbWrapper *db = 0;
	if(lazy_ != NEITHER && nsDocument_ == 0) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		if(container->isNodeContainer()) {
			db = container->getDbWrapper();
			createNsObjects();
			initNsObjects(db, oc_.txn());
			definitiveContent_ = DOM;
			contentModified_ = false;
		} else {
			id2dbt();
			dbt2stream(true);
			stream2dom(isns);
		}
	}
}

void Document::id2stream() const
{
	if(lazy_ != NEITHER && inputStream_ == 0) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		if(container->isNodeContainer()) {
			inputStream_ = new PullEventInputStream(
				new EventReaderToWriter(
					*(new NsEventReader(
					  oc_.txn(),
					  container->getDbWrapper(),
					  container->getDictionaryDatabase(),
					  id_, cid_, db_flags_, NS_EVENT_BULK_BUFSIZE, 0, docdb_)),
					true,
					true /* owns reader */)
				);
		} else {
			id2dbt();
			dbt2stream(true);
		}
	}
}

void Document::dbt2stream(bool adopt) const
{
	if(inputStream_ == 0) {
		if (!dbtContent_ || !dbtContent_->size)
			return; // no-content doc

		inputStream_ =
			new MemBufInputStream(
				dbtContent_, getName().c_str(),
				/* adoptDbt */ adopt);
		if (adopt)
			dbtContent_ = 0;
	}
}

void Document::dbt2dom() const
{
	resetDocDb();
	dbt2stream(true);
	stream2dom(0); // No document projection
}

void Document::dom2dbt() const
{
	if(dbtContent_ == 0) {
		DBXML_ASSERT(nsDocument_);
		// Make output writer
		BufferNsStream output;
		NsWriter writer(&output);
		
		// Make event generator
		NsEventReader reader(*nsDocument_, NS_EVENT_BULK_BUFSIZE, 0, docdb_);
		writer.writeFromReader(reader);
		
		// directly donate memory from Buffer to DbtOut
		dbtContent_ = new DbtOut();
		dbtContent_->setNoCopy(output.buffer.donateBuffer(),
				       output.buffer.getOccupancy());
	}
	// Do NOT change definitiveContent_, definitiveContent_ is still
	// DOM; dbtContent_ is a cache
}

void Document::dom2stream() const
{
	if(inputStream_ == 0) {
		DBXML_ASSERT(nsDocument_);

		if((lazy_ == NEITHER)|| isContentModified()) {
			inputStream_ = new PullEventInputStream(
				new EventReaderToWriter(
					*(new NsEventReader(*nsDocument_, NS_EVENT_BULK_BUFSIZE, 0, docdb_)),
					true, // owns reader
					true /* internal */));
		} else {
			id2stream();
		}
	}
}

void Document::stream2dom(const ISNVector *isns) const
{
	// Use temporary database for DLS documents
	if (nsDocument_ == 0) {
		if (!inputStream_) // no content doc
			return;
		if (docdb_.isNull()) {
			bool exists = false;
			CacheDatabase *cdb =
				dbMinder_.findOrAllocate(
					(Manager&)mgr_, cid_);
			if (id_ == 0) {
				DBXML_ASSERT(cid_ == 0);
				const_cast<Document*>(this)->setID(
					dbMinder_.allocateDocID(
						(Manager&)mgr_));
			}
			// see if doc exists in DB already...
			exists = CacheDatabaseHandle::docExists(
				cdb->getDb(), id_);
			docdb_ = cdb;
			if (!exists) {
				NsPushEventSource *events =
					stream2events(oc_.txn(), false, true, isns);
				try {
					events->start();
				}
				// GMF TBD -- add more error handling
				catch (XmlException &xe) {
					delete events;
					throw;
				}
				delete events;
			}
		}
		// document should be initialized by now
		createNsObjects(false); // don't reset DOM, it'll clear the DB
		/* Transaction may be needed by dictionary lookups in the container; 
		it is ignored by temporary databases */
		initNsObjects(docdb_.getDb(), oc_.txn()); 
		delete inputStream_;
		inputStream_ = 0;
		definitiveContent_ = DOM;
		contentModified_ = false;
	}
}

// TBD GMF -- is this really correct/necessary?  
bool Document::hasNsDom() const
{
	if (nsDocument_ || !docdb_.isNull()) {
		// TBD -- should this change definitiveContent_?
		// probably not without resetting something, and
		// understanding why it wasn't already DOM
		return true;
	}
	if (dbMinder_.isNull())
		return false;
	CacheDatabase *cdb =
		dbMinder_.findOrAllocate(
			(Manager&)mgr_, cid_);
	if (id_ == 0) {
		DBXML_ASSERT(cid_ == 0);
		const_cast<Document*>(this)->setID(
			dbMinder_.allocateDocID((Manager&)mgr_));
	}
	// see if doc exists in DB already...
	if (CacheDatabaseHandle::docExists(
		    cdb->getDb(), id_)) {
		docdb_ = cdb;
		createNsObjects(false); // don't reset DOM, it'll clear the DB
		initNsObjects(docdb_.getDb(), NULL); // no txn
		definitiveContent_ = DOM;
		// TBD -- should something else be reset, and why?
		return true;
	}
	return false;
}

#define TMP_BUF_SIZE (4 * 1024)
void Document::stream2dbt() const
{
	if(dbtContent_ == 0) {
		DBXML_ASSERT(inputStream_);

		char tmp[TMP_BUF_SIZE];
		Buffer buffer(0, 16 * 1024); // 16k buffer, to start

		unsigned int length;
		while((length = inputStream_->
		       readBytes(tmp, TMP_BUF_SIZE * sizeof(char))) != 0) {
			buffer.write((void*)tmp, length);
		}

		dbtContent_ =
			new DbtOut(buffer.getBuffer(), buffer.getOccupancy());

		// An inputstream can only be used once
		delete inputStream_;
		inputStream_ = 0;
		DBXML_ASSERT(definitiveContent_ == INPUTSTREAM);
		definitiveContent_ = DBT;
	}
}

NsPushEventSource *Document::id2events(Transaction *txn, bool needsValidation,
				       bool nodeEvents) const
{
	NsPushEventSource *result = 0;
	if(lazy_ != NEITHER) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		if(container->isNodeContainer() && !needsValidation) {
			DbWrapper *db = container->getDbWrapper();
			result = new EventReaderToWriter(
				*(new NsEventReader(
					  txn, db,
					  container->getDictionaryDatabase(),
					  id_, cid_, db_flags_, NS_EVENT_BULK_BUFSIZE, 0, docdb_)),
				true, // owns reader
				true /* internal */);
		} else {
			id2stream();
			return stream2events(txn, needsValidation,
				nodeEvents, 0); // No document projection
		}
	}
	return result;
}

NsPushEventSource *Document::dbt2events(Transaction *txn, bool needsValidation,
					bool nodeEvents) const 
{
	dbt2stream();
	return stream2events(txn, needsValidation, nodeEvents, 0); // No document projection
}

NsPushEventSource *Document::dom2events(Transaction *txn, bool needsValidation,
					bool nodeEvents) const 
{
	DBXML_ASSERT(nsDocument_);
	// may need to parse the document for these cases:
	// (1) validation
	// (2) reindexing of modified wholedoc content

	if (needsValidation || isWholedoc()) {
		// parse from scratch
		dom2stream();
		return stream2events(txn, needsValidation, nodeEvents, 0); // No document projection
	} else {	
		return 	new EventReaderToWriter(
			*(new NsEventReader(*nsDocument_, NS_EVENT_BULK_BUFSIZE, 0, docdb_)),
			true, // owns reader
			true /* internal */);
	}
}

NsPushEventSource *Document::stream2events(Transaction *txn,
	bool needsValidation, bool nodeEvents, const ISNVector *isns) const
{
	if (!inputStream_) return 0; // no-content doc

	ScopedContainer sc(getManager(), cid_, false);
	Container *container = sc.getContainer();
	DbWrapper *db = 0;
	DictionaryDatabase *ddb = 0;
	// don't set db if content has been modified -- that means we're
	// called via updateDocument or modifyDocument, which is reading
	// DOM events from the existing docdb_
	if (!docdb_.isNull() &&
	    !(contentModified_ &&
	      definitiveContent_ == DOM))
		db = docdb_.getDb();
	if (container) {
		if (!db)
			db = container->getDbWrapper();
		ddb = container->getDictionaryDatabase();
	}
	if (db && !ddb)
		ddb = ((Manager&)mgr_).getDictionary();
	NsEventTranslator *translator;
	if(nodeEvents || db != 0) {
		if(isns != 0 && !isns->empty()) {
			// Apply document projection
			translator = new NsImpliedSchemaFilter(*isns,
				new NsXercesTranscoder(txn, db, ddb, id_, cid_, 0));
		} else {
			translator = new NsXercesTranscoder(
				txn, db, ddb, id_, cid_, 0);
		}
	} else {
		translator = new NsXercesIndexer();
	}

	unsigned int pflags = 0;
	if (validation_ == WF_ONLY)
		pflags |= NsParserEventSource::NS_PARSER_WF_ONLY;
	else if (validation_ == DONT_VALIDATE)
		pflags |= NsParserEventSource::NS_PARSER_WELL_FORMED;
	else if (needsValidation)
		pflags |= NsParserEventSource::NS_PARSER_MUST_VALIDATE;
	NsParserEventSource *pes = new NsParserEventSource(mgr_, txn, pflags,
							   &inputStream_);

	if(definitiveContent_ == INPUTSTREAM) {
		definitiveContent_ = NONE;
	}

	return new NsPushEventSourceTranslator(pes, translator);
}

// XmlEventReader methods

//
// Generate an XmlEventReader from the id
//
void Document::id2reader() const
{
	if(lazy_ != NEITHER && reader_ == 0) {
		ScopedContainer sc(getManager(), cid_, true);
		Container *container = sc.getContainer();
		if(container->isNodeContainer()) {
			reader_ = new NsEventReader(
				oc_.txn(),
				container->getDbWrapper(),
				container->getDictionaryDatabase(),
				id_, cid_, db_flags_, NS_EVENT_BULK_BUFSIZE, 0, docdb_);
		} else {
			// wholedoc requires parsing into dom, which serves
			// as the source for the events to be read
			// The returned reader is not part of the document object
			id2dom(0); // No document projection
			dom2reader();
		}
	}
}

void Document::dbt2reader() const
{
	// no need to txn if the dbt is in memory already
	dbt2dom();
	dom2reader();
}

void Document::dom2reader() const
{
	DBXML_ASSERT(nsDocument_);
	reader_ = new NsEventReader(*nsDocument_, NS_EVENT_BULK_BUFSIZE, 0, docdb_);
}

void Document::stream2reader() const
{
	stream2dom(0); // No document projection
	dom2reader();
}

void Document::reader2stream() const
{
	DBXML_ASSERT(reader_);
	DBXML_ASSERT(!inputStream_);

	inputStream_ = new PullEventInputStream(
		new EventReaderToWriter(*reader_, true, true));
	DBXML_ASSERT(definitiveContent_ == READER);
	definitiveContent_ = INPUTSTREAM;
	reader_ = 0;
}

void Document::reader2dbt() const
{
	DBXML_ASSERT(reader_);
	// write from reader to NsWriter
	BufferNsStream output;
	NsWriter writer(&output);
	EventReaderToWriter r2e(*reader_, writer, true, true);
	reader_ = 0;
	DBXML_ASSERT(!dbtContent_);
	r2e.start();
	dbtContent_ = new DbtOut();
	dbtContent_->setNoCopy(output.buffer.donateBuffer(),
			       output.buffer.getOccupancy());
	DBXML_ASSERT(definitiveContent_ == READER);
	definitiveContent_ = DBT;
}

// debugging...
bool Document::isUninitialized() const
{
	// NOTE: used to include || (CacheDatabase*)docdb_
	// but there is one instance where that is allowed
	if (definitiveContent_ ||
	    dbtContent_ ||
	    inputStream_ ||
	    reader_ ||
	    nsDocument_)
		return false;
	return true;
}

///////////////////////////
// MetaData

MetaData::const_iterator Document::metaDataBegin() const {
	return metaData_.begin();
}

MetaData::const_iterator Document::metaDataEnd() const {
	return metaData_.end();
}

MetaDataIterator::MetaDataIterator(Document *document)
	: document_(document)
{
	document_->acquire();
	document_->setEagerMetaData();
	i_ = document_->metaDataBegin();
}

MetaDataIterator::~MetaDataIterator()
{
	document_->release();
}

bool MetaDataIterator::next(string &uri, string &name, XmlValue &value)
{
	if(i_ == document_->metaDataEnd()) {
		uri= "";
		name= "";
		value = XmlValue();
		return false;
	}
	else {
		const Name &mdName = (*i_)->getName();
		uri = mdName.getURI();
		name = mdName.getName();
		(*i_)->asValue(value);
		++i_;
		return true;
	}
}

void MetaDataIterator::reset()
{
	i_ = document_->metaDataBegin();
}

bool MetaDataIterator::previousIsModified() const 
{ 
	if(i_ == document_->metaDataBegin()) return false;
	return (*(i_-1))->isModified(); 
}
bool MetaDataIterator::previousIsRemoved() const 
{ 
	if(i_ == document_->metaDataBegin()) return false;
	return (*(i_-1))->isRemoved(); 
}
