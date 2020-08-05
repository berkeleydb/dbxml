//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlSequenceBuilder.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlConfiguration.hpp"
#include "../Document.hpp"
#include "../QueryContext.hpp"
#include "../UTF8.hpp"
#include "../ReferenceMinder.hpp"
#include "../nodeStore/NsDocument.hpp"
#include "../nodeStore/NsDom.hpp"
#include "../nodeStore/NsEventWriter.hpp"
#include "../DictionaryDatabase.hpp"
#include "../Manager.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlSequenceBuilder::DbXmlSequenceBuilder(const DynamicContext *context)
	: context_(context),
	  xmlDoc_(0),
	  doingSequence_(0),
	  writer_(0),
	  seq_(context->getMemoryManager()),
	  nextIsRoot_(false)
{
	rootNid_.setLen(0, false);
}

DbXmlSequenceBuilder::~DbXmlSequenceBuilder()
{
	delete writer_;
}

static inline NsEventWriter *createWriter(XmlDocument &xdoc,
					  const DynamicContext *context,
					  NsFullNid *rootNid)
{
	XmlManager &mgr = GET_CONFIGURATION(context)->getManager();
	xdoc = mgr.createDocument();
	
	// prepare to create NsEventWriter
	DictionaryDatabase * dict = ((Manager&)mgr).getDictionary();
	CacheDatabaseMinder &dbMinder = GET_CONFIGURATION(context)->
		getDbMinder();
	// get the temp DB for construction (container id 0)
	CacheDatabase *cdb = dbMinder.findOrAllocate((Manager&)mgr, 0);
	DocID did = dbMinder.allocateDocID((Manager&)mgr);

	// Tell the document to use this database, and that it's
	// content is "NsDom"
	// TBD GMF: Eventually it may be that Document and NsDocument objects 
	// will not be required by DbXmlNodeImpl, so creation/init of Document
	// objects will not be required.
	((Document&)xdoc).setContentAsNsDom(did, cdb);

	NsEventWriter *writer = new NsEventWriter(cdb->getDb(), dict, did);
	writer->writeStartDocumentInternal(0, 0, 0, rootNid);
	return writer;
}

void DbXmlSequenceBuilder::addNodeToSequence()
{
	DBXML_ASSERT(writer_);
	DBXML_ASSERT(doingSequence_ == 0);
	writer_->writeEndDocumentInternal();
	// get the "root" of the sequence and create a node
	NsDomElement *root = ((Document&)xmlDoc_).getElement(NsNid(&rootNid_), 0);
	DBXML_ASSERT(root);
	
	seq_.addItem(((DbXmlFactoryImpl*)context_->getItemFactory())->
		     createNode(root, xmlDoc_, context_));
	// reset state
	xmlDoc_ = 0;
	writer_->closeInternal();
	writer_ = 0;
	rootNid_.freeNid();
}

void DbXmlSequenceBuilder::startDocumentEvent(const XMLCh *documentURI,
					      const XMLCh *encoding)
{
	DBXML_ASSERT(!writer_);
	DBXML_ASSERT(!doingSequence_);
	DBXML_ASSERT(!nextIsRoot_);
	writer_ = createWriter(xmlDoc_, context_, &rootNid_);
	nextIsRoot_ = true;
	doingSequence_++;
	((Document*)xmlDoc_)->setDocumentURI(documentURI);
	((Document*)xmlDoc_)->getNsDocument()->
		setEncodingStr((const xmlbyte_t*)XMLChToUTF8(encoding).str());
}

void DbXmlSequenceBuilder::endDocumentEvent()
{
	DBXML_ASSERT(writer_);
	if (--doingSequence_ == 0)
		addNodeToSequence();
	DBXML_ASSERT(doingSequence_ == 0); // better be true
}

void DbXmlSequenceBuilder::endEvent()
{
}

void DbXmlSequenceBuilder::startElementEvent(const XMLCh *prefix,
					     const XMLCh *uri,
					     const XMLCh *localname)
{
	bool initRootNid = false;
	if (writer_ == 0) {
		DBXML_ASSERT(!doingSequence_);
		writer_ = createWriter(xmlDoc_, context_, 0);
		initRootNid = true;
		nextIsRoot_ = true;
	}
	doingSequence_++;
	XMLChToUTF8Null pfx(prefix);
	XMLChToUTF8Null turi(uri);
	XMLChToUTF8Null lname(localname);
	writer_->writeStartElementInternal(lname.ucstr(), pfx.ucstr(),
					   turi.ucstr(),
					   (initRootNid ? &rootNid_ : 0),
					   nextIsRoot_);
	nextIsRoot_ = false; // unconditionally clear
}

void DbXmlSequenceBuilder::endElementEvent(const XMLCh *prefix,
					   const XMLCh *uri,
					   const XMLCh *localname,
					   const XMLCh *typeURI,
					   const XMLCh *typeName)
{
	DBXML_ASSERT(writer_);
	writer_->writeEndElementInternal();
	if (--doingSequence_ == 0)
		addNodeToSequence();
}

void DbXmlSequenceBuilder::piEvent(const XMLCh *target, const XMLCh *value)
{
	if (doingSequence_) {
		DBXML_ASSERT(writer_ != 0);
		XMLChToUTF8Null targ(target);
		XMLChToUTF8Null val(value);
		writer_->writeProcessingInstruction(targ.ucstr(), val.ucstr());
	} else {
		seq_.addItem(((DbXmlFactoryImpl*)context_->getItemFactory())->
			     createPINode(target, value, context_));
		xmlDoc_ = 0;
	}
}

void DbXmlSequenceBuilder::textEvent(const XMLCh *value,
				     unsigned int length)
{
	// for now, ignore length;
	textEvent(value);
}

void DbXmlSequenceBuilder::textEvent(const XMLCh *chars)
{
	if (doingSequence_) {
		// empty text is not valid in element context
		if (chars && *chars) {
			DBXML_ASSERT(writer_ != 0);
			XMLChToUTF8Null val(chars);
			writer_->writeText(XmlEventReader::Characters,
					   val.ucstr(),
					   val.len());
		}
	} else {
		seq_.addItem(((DbXmlFactoryImpl*)context_->getItemFactory())->
			     createTextNode(nsNodeText, chars, context_));
		xmlDoc_ = 0;
	}
}

void DbXmlSequenceBuilder::commentEvent(const XMLCh *value)
{

	if (doingSequence_) {
		// empty value is OK for comments
		DBXML_ASSERT(writer_ != 0);
		XMLChToUTF8Null val(value);
		writer_->writeText(XmlEventReader::Comment,
				   val.ucstr(),
				   val.len());
	} else {
		seq_.addItem(((DbXmlFactoryImpl*)context_->getItemFactory())->
			     createTextNode(nsNodeComment, value, context_));

		xmlDoc_ = 0;
	}
}

void DbXmlSequenceBuilder::attributeEvent(const XMLCh *prefix,
					  const XMLCh *uri,
					  const XMLCh *localname,
					  const XMLCh *value,
					  const XMLCh *typeURI,
					  const XMLCh *typeName)
{

	if (doingSequence_) {
		// type is ignored for now
		DBXML_ASSERT(writer_ != 0);
		XMLChToUTF8Null pfx(prefix);
		XMLChToUTF8Null turi(uri);
		XMLChToUTF8Null lname(localname);
		XMLChToUTF8Null val(value);
		writer_->writeAttributeInternal(lname.ucstr(), pfx.ucstr(),
						turi.ucstr(),
						val.ucstr(), true);
	} else {
		seq_.addItem(((DbXmlFactoryImpl*)context_->getItemFactory())->
			     createAttrNode(prefix, uri, localname, value,
					    typeURI, typeName,
					    0, /* ContainerBase */
					    0, /* Document */
					    NsNid(), 0,
					    context_));
		xmlDoc_ = 0;
	}
}

void DbXmlSequenceBuilder::namespaceEvent(const XMLCh *prefix,
					  const XMLCh *uri)
{
	DBXML_ASSERT(doingSequence_ && (writer_ != 0));
	XMLChToUTF8Null pfx((prefix ? prefix : XMLUni::fgXMLNSString));
	XMLChToUTF8Null nsuri(XMLUni::fgXMLNSURIName);
	XMLChToUTF8Null nspfx(XMLUni::fgXMLNSString);
	XMLChToUTF8Null turi(uri);
	writer_->writeAttributeInternal(pfx.ucstr(),
					(prefix != 0) ? nspfx.ucstr() : 0,
					nsuri.ucstr(),
					turi.ucstr(),
					true);
}

void DbXmlSequenceBuilder::atomicItemEvent(const XMLCh *value,
					   const XMLCh *typeURI,
					   const XMLCh *typeName)
{
	DBXML_ASSERT(doingSequence_ == 0);

	seq_.addItem(context_->getItemFactory()->
		     createDerivedFromAtomicType(typeURI, typeName,
						 value, context_));
}

