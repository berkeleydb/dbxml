//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "../Container.hpp"
#include "../Document.hpp"
#include "../UpdateContext.hpp"
#include "NsEventWriter.hpp"
#include "NsNode.hpp"
#include "NsEvent.hpp"
#include "NsWriter.hpp"
#include "NsConstants.hpp"
#include "NsDocumentDatabase.hpp"

using namespace DbXml;
using namespace std;

#define CHECK_SUCCESS() \
if (!success_)throwBadWrite("XmlEventWriter: cannot write after an exception is thrown")
#define CHECK_NULL(p) if (p && (*p == '\0')) p = 0

#define CANT_BE_NULL(p, f, n) \
if (!p) {  \
	string msg = "XmlEventWriter::";	\
	msg += f;				\
	msg += ": argument cannot be null: ";	\
	msg += n;				\
	throwBadWrite(msg.c_str());		\
}


/*
 * NsEventWriter implementation
 */
NsEventWriter::NsEventWriter(Container *container, UpdateContext *uc,
			     Document *document, u_int32_t flags)
	: NsHandlerBase(&nsdoc_, 0), nsdoc_(0),
	  uc_(uc), cont_(container), doc_(document), db_(0),
	  docId_(document->getID()), writer_(0),
	  writerStream_(0), mustBeEnd_(false), success_(true),
	  needsStartElement_(false),
	  needsEndDocument_(false), isEmpty_(true), attrsToGo_(0), nidGen_(0)
{
	DBXML_ASSERT(uc_ && cont_ && !doc_.isNull());
	uc_->acquire();
	cont_->acquire();
	db_ = cont_->getDocumentDB()->getNodeDatabase();
	OperationContext &oc = uc_->getOperationContext();
	nsdoc_.initDoc(oc.txn(),
		       db_,
		       cont_->getDictionaryDatabase(),
		       docId_, cont_->getContainerID(), flags);
	// note NsDocument and Document objects are entirely separate
}

//
// This method is used for simple upgrade to copy documents
// as well as updates
// There may or may not be an actual container object involved.
//
NsEventWriter::NsEventWriter(DbWrapper *db,
			     DictionaryDatabase *dict,
			     const DocID &id,
			     Transaction *txn)
	: NsHandlerBase(&nsdoc_, 0), nsdoc_(0),
	  uc_(0), cont_(0), db_(db),
	  docId_(id), writer_(0),
	  writerStream_(0), mustBeEnd_(false), success_(true),
	  needsStartElement_(false),
	  needsEndDocument_(false), isEmpty_(true), attrsToGo_(0), nidGen_(0)
{
	// no flags allowed (easy to add if necessary)
	nsdoc_.initDoc(txn, db_, dict, docId_, 0, 0);
}

NsEventWriter::~NsEventWriter()
{
	if (uc_)
		uc_->release();
	if (cont_)
		cont_->release();
	if (writerStream_)
		delete writerStream_;
	if (writer_)
		delete writer_;
}

void NsEventWriter::close()
{
	if (needsEndDocument_)
		writeEndDocument();
	bool bad = (success_ && !mustBeEnd_);
	delete this; // object is deleted, throw or not
	if (bad)
		throwBadWrite("XmlEventWriter: cannot call close before document is complete");
}

// implement close.
void NsEventWriter::completeDoc()
{
	if (success_ && cont_) {
		DBXML_ASSERT(!doc_.isNull() && uc_);
		if (!db_) {
			DBXML_ASSERT(writerStream_);
			// put output of writer into doc_
			DbtOut *data = new DbtOut();
			data->setNoCopy(writerStream_->buffer.donateBuffer(),
					writerStream_->buffer.getOccupancy());
			(*doc_).setContentAsDbt(&data, true); // consumes data
		}
		int err = cont_->getDocumentDB()->addContent(*doc_, *uc_);
		if (err == 0) {
			(*doc_).setContentModified(false);
			cont_->completeAddDocument(*doc_, *uc_);
		}
	}
}

void NsEventWriter::createNsWriter()
{
	writerStream_ = new BufferNsStream();
	if (writerStream_) {
		writer_ = new NsWriter(writerStream_);
	}
	if (!writerStream_ || !writer_)
		throw XmlException(XmlException::NO_MEMORY_ERROR,
				   "Failed to allocate memory for XmlEventWriter");
}

void NsEventWriter::exceptionNotify(XmlException &xe)
{
	if (success_) {
		success_ = false;
		if (cont_) {
			bool isDbExc = false;
			if( xe.getExceptionCode() == XmlException::DATABASE_ERROR )
				isDbExc = true; 

			cont_->getDocumentDB()->addContentException(
				*doc_, *uc_, isDbExc);
        	}
	}
	throw xe;
}

void NsEventWriter::throwBadWrite(const char *msg)
{
	XmlException xe(XmlException::EVENT_ERROR, msg);
	exceptionNotify(xe);
}

void NsEventWriter::doStartElem(NsEventNodeAttrList *attrs)
{
	try {
		NsNode *node = _current;
		needsStartElement_ = false;
		// need to use std::string to store prefix and uri
		// since their storage may be reused
		std::string spref;
		std::string suri;
		const xmlbyte_t *tpref = 0, *turi = 0;
		if (node->hasUri()) {
			suri = _doc->getStringForID(node->uriIndex());
			turi = (const xmlbyte_t *) suri.c_str();
			if (node->hasNamePrefix()) {
				spref = _doc->getStringForID(node->namePrefix());
				tpref = (const xmlbyte_t *) spref.c_str();
			}
		}
		const xmlbyte_t *tname = (const xmlbyte_t *) node->getNameChars();
		// constructor is ok with empty attr list.
		NsEventNodeAttrList alist(node->getAttrList(), *_doc);
		NsNodeIndexNodeInfo ninfo(node);
		if (_ewriter)
			_ewriter->writeStartElementWithAttrs(
				tname, tpref, turi,
				alist.numAttributes(), &alist, &ninfo, isEmpty_);
		if (writer_)
			writer_->writeStartElementWithAttrs(
				tname, tpref, turi,
				alist.numAttributes(), &alist, &ninfo, isEmpty_);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeAttribute(const unsigned char *localName,
                                   const unsigned char *prefix,
                                   const unsigned char *uri,
                                   const unsigned char *value,
                                   bool isSpecified)
{
        CHECK_NULL(localName);
        CHECK_NULL(prefix);
        CHECK_NULL(uri);
        CHECK_NULL(value);
        CHECK_SUCCESS();
        CANT_BE_NULL(localName, "writeAttribute", "localName");
        if (!needsStartElement_ || !_current || !attrsToGo_)
                throwBadWrite("writeAttribute called out of order");
	writeAttributeInternal(localName, prefix, uri, value, isSpecified);
}

void NsEventWriter::writeText(XmlEventReader::XmlEventType type,
			      const unsigned char *text,
			      size_t length)
{
	CHECK_NULL(text);
	CHECK_SUCCESS();
	if (!length && (text != 0))
		length = ::strlen((const char *)text);
	if (!_current)
		throwBadWrite("writeText: requires writeStartDocument");
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeText called before all attributes written");
			doStartElem(NULL);
		}
		uint32_t textType;
		switch (type) {
		case XmlEventReader::Whitespace:
			textType = NS_TEXT|NS_IGNORABLE;
			break;
		case XmlEventReader::Characters:
			textType = NS_TEXT;
			break;
		case XmlEventReader::CDATA:
			textType = NS_CDATA;
			break;
		case XmlEventReader::Comment:
			textType = NS_COMMENT;
			break;
		default:
			throwBadWrite("writeText called with bad event");
			break;
		}
		addText(text, length, textType, false);
		bool needsEscape = true;
		if ((type != XmlEventReader::Whitespace) &&
		    (type != XmlEventReader::Comment)){
			DBXML_ASSERT(_textList);
			needsEscape =
				(_textList->tl_text[_textList->tl_ntext-1].te_type
				 & NS_ENTITY_CHK) != 0;
		}
		if (_ewriter)
			_ewriter->writeTextWithEscape(type, text,
						      length, needsEscape);
		if (writer_)
			writer_->writeTextWithEscape(type, text,
						     length, needsEscape);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data)
{
	CHECK_NULL(target);
	CHECK_NULL(data);
	CHECK_SUCCESS();
	CANT_BE_NULL(target, "writeProcessingInstruction", "target");
	if (!_current)
		throwBadWrite("writeProcessingInstructio: requires writeStartDocument");
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeProcessingInstruction called before all attributes written");
			doStartElem(NULL);
		}
		
		// last arg (len) is ignored if not donating
		addPI(target, data, false, 0); 
		if (_ewriter)
			_ewriter->writeProcessingInstruction(target, data);
		if (writer_)
			writer_->writeProcessingInstruction(target, data);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeStartElement(const unsigned char *localName,
				      const unsigned char *prefix,
				      const unsigned char *uri,
				      int numAttributes,
				      bool isEmpty)
{
	CHECK_NULL(localName);
	CHECK_NULL(prefix);
	CHECK_NULL(uri);
	CHECK_SUCCESS();
	CANT_BE_NULL(localName, "writeStartElement", "localName");
	if (!_current) {
		// in this case, there was no  startDocument event.
		// rather than throw, just write one
		writeStartDocument(0, 0, 0);
		DBXML_ASSERT(_current);
		needsEndDocument_ = true;
	}
	if (mustBeEnd_)
		throwBadWrite("writeStartElement: document can only have one root");
	try {
		NsNode *node = NsNode::allocNode(numAttributes,
						 NS_STANDALONE);
		getNextNid(node->getFullNid());
		node->acquire();
		startElem(node, localName, (const char *)uri,
			  (const char *)prefix, false);
		isEmpty_ = isEmpty;
		if (numAttributes == 0) {
			NsNodeIndexNodeInfo ninfo(node);
			needsStartElement_ = false;
			if (_ewriter)
				_ewriter->writeStartElementWithAttrs(
					localName, prefix, uri,
					numAttributes, NULL,
					&ninfo, isEmpty_);
			if (writer_)
				writer_->writeStartElementWithAttrs(
					localName, prefix, uri,
					numAttributes, NULL,
					&ninfo, isEmpty_);
			if (isEmpty_) {
				endElem();
				if (_current->isDoc())
					mustBeEnd_ = true;
				isEmpty_ = false;
			}
		} else {
			needsStartElement_ = true;
			attrsToGo_ = numAttributes;
		}
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri)
{
	CHECK_NULL(localName);
	CHECK_NULL(prefix);
	CHECK_NULL(uri);
	CHECK_SUCCESS();
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeEndElement called before all attributes written");
			doStartElem(NULL);
		}
		if (isEmpty_)
			throwBadWrite("writeEndElement called for empty element");
		if (!_current)
			throwBadWrite("writeEndElement called with no current element");
		
		// do event before formally ending the element, as state will change
		NsNodeIndexNodeInfo ninfo(_current);
		if (_ewriter)
			_ewriter->writeEndElementWithNode(localName, prefix,
							  uri, &ninfo);
		if (writer_)
			writer_->writeEndElementWithNode(localName, prefix,
							 uri, &ninfo);
		endElem();
		if (!_current || _current->isDoc())
			mustBeEnd_ = true;
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeDTD(const unsigned char *dtd, size_t length)
{
	CHECK_NULL(dtd);
	CHECK_SUCCESS();
	if (!length)
		length = ::strlen((const char *)dtd);

	if (_current) {
		if (!_current->isDoc())
			throwBadWrite("writeDTD: must occur before content");
	} else
		throwBadWrite("writeDTD: requires writeStartDocument");
	try {
		addText(dtd, length, NS_SUBSET, false);
		if (_ewriter)
			_ewriter->writeDTD(dtd, length);
		if (writer_)
			writer_->writeDTD(dtd, length);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeStartDocument(const unsigned char *version,
				       const unsigned char *encoding,
				       const unsigned char *standalone)
{
	CHECK_NULL(version);
	CHECK_NULL(encoding);
	CHECK_NULL(standalone);
	CHECK_SUCCESS();
	if (_current)
		throwBadWrite("writeStartDocument: must be the first event written");
	
	try {
		// use internal version
		writeStartDocumentInternal(version, encoding,
					   standalone, NULL);

		// event handler
		if (_ewriter)
			_ewriter->writeStartDocument(version, encoding,
						     standalone);
		if (writer_)
			writer_->writeStartDocument(version, encoding,
						    standalone);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeEndDocument()
{
	CHECK_SUCCESS();
	needsEndDocument_ = false;
	if (!_current)
		throwBadWrite("writeEndDocument: requires writeStartDocument");
	if (!mustBeEnd_)
		throwBadWrite("writeEndDocument: called before document is complete");
	try {
		if (_ewriter)
			_ewriter->writeEndDocument();
		if (writer_)
			writer_->writeEndDocument();
		endDoc();
		completeDoc();
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows)
{
	CHECK_NULL(name);
	CHECK_SUCCESS();
	CANT_BE_NULL(name, "writeStartEntity", "name");
	if (!_current)
		throwBadWrite("writeStartEntity: requires writeStartDocument");
	try {
		if (_ewriter)
			_ewriter->writeStartEntity(name, expandedInfoFollows);
		if (writer_)
			writer_->writeStartEntity(name, expandedInfoFollows);
		size_t len = NsUtil::nsStringLen(name);
		addText(name, len, NS_ENTSTART, false);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeEndEntity(const unsigned char *name)
{
	CHECK_NULL(name);
	CHECK_SUCCESS();
	if (!_current)
		throwBadWrite("writeEndEntity: requires writeStartDocument");
	try {
		size_t len = NsUtil::nsStringLen(name);
		if (_ewriter)
			_ewriter->writeEndEntity(name);
		if (writer_)
			writer_->writeEndEntity(name);
		addText(name, len, NS_ENTEND, false);
		
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

////////////////////////////////////////////
// "Internal" API -- no error/state checks

void NsEventWriter::writeAttributeInternal(const unsigned char *localName,
					   const unsigned char *prefix,
					   const unsigned char *uri,
					   const unsigned char *value,
					   bool isSpecified)
{
	try {
		_current->addAttr(_doc, prefix, uri, localName,
				  value, isSpecified);
		if (--attrsToGo_ == 0) {
			NsEventNodeAttrList alist(_current->getAttrList(),
						  *_doc);
			doStartElem(&alist);
			// if it's empty, do end as well
			if (isEmpty_) {
				endElem();
				if (_current->isDoc())
					mustBeEnd_ = true;
				isEmpty_ = false;
			}
		}
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

// this version of writeStartElement does not know if there are
// attributes or not.  It also does not call any event handlers,
// which should not be present anyway, so doStartElem() is not relevant
void NsEventWriter::writeStartElementInternal(const unsigned char *localName,
					      const unsigned char *prefix,
					      const unsigned char *uri,
					      NsFullNid *nid, bool setIsRoot)
{
	DBXML_ASSERT(!writer_);
	DBXML_ASSERT(!_ewriter);
	// trust caller
	try {
		// don't know number of attributes -- use 0, they will
		// be allocated on demand.
		NsNode *node = NsNode::allocNode(0, NS_STANDALONE);
		getNextNid(node->getFullNid());
		node->acquire();
		if (nid)
			nid->copyNid(node->getFullNid());
		if (setIsRoot)
			node->setIsRoot();
		startElem(node, localName, (const char *)uri,
			  (const char *)prefix, false);
		isEmpty_ = false; // don't know if empty or not
		needsStartElement_ = false;
		attrsToGo_ = -1;  // this ensures that adding attributes
		// never tries to call event handlers in doStartElem().
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeEndElementInternal()
{
	try {
		endElem();
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}
	
void NsEventWriter::writeStartDocumentInternal(const unsigned char *version,
					       const unsigned char *encoding,
					       const unsigned char *standalone,
					       NsFullNid *nid)
{
	// trust the caller
	try {
		// create the document node
		NsNode *node = NsNode::allocNode(0, NS_STANDALONE);
		node->setFlag(NS_ISDOCUMENT);
		_current = node;
		node->acquire();
		getNextNid(node->getFullNid());
		if (nid)
			nid->copyNid(node->getFullNid());
		    
		DBXML_ASSERT(node->getNid().isDocRootNid());
		
		if (version && *version) {
			if (NsUtil::nsStringEqual(version, _decl8_1_0 ))
				_doc->setXmlDecl(NS_DECL_1_0);
			else if (NsUtil::nsStringEqual(version, _decl8_1_1 ))
				_doc->setXmlDecl(NS_DECL_1_1);
			else
				throwBadWrite(
					"writeStartDocument: bad XML decl");
		}
		if (encoding && *encoding) {
			_doc->setEncodingStr((const xmlbyte_t *)encoding);
		}
		if (standalone) {
			if (NsUtil::nsStringEqual(standalone,
						  (const unsigned char *)"yes"))
				_doc->setStandalone(true);
			else
				_doc->setStandalone(false);
		}
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::writeEndDocumentInternal()
{
	try {
		endDoc();
		completeDoc();
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
}

void NsEventWriter::closeInternal()
{
	// if this assertion ever goes off, it
	// means a writeEndDocument() call should be added
	DBXML_ASSERT(!needsEndDocument_);
	delete this;
}

void NsEventWriter::getNextNid(NsFullNid *nid)
{
	if (nidGen_)
		nidGen_->nextId(nid);
	else
		nextId(nid);
}
