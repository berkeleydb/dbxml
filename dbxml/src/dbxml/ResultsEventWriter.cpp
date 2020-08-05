//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include <dbxml/XmlResults.hpp>
#include "ResultsEventWriter.hpp"
#include "Results.hpp"
#include "UTF8.hpp"
#include "nodeStore/NsDom.hpp"

using namespace DbXml;

ResultsEventWriter::ResultsEventWriter(Results *res):
	res_(res),
	xmlDoc_(0),
	hasStartDocument_(false),
	depth_(0),
	writer_(0),
	version_(0),
	encoding_ (0),
	standalone_(0)
{
}

ResultsEventWriter::~ResultsEventWriter()
{
	res_->writer_ = 0;
	if(writer_ != NULL)
		delete writer_;
}

void ResultsEventWriter::close()
{
	int depth = depth_;
	
	// delete object unconditionally
	delete this;
	if(depth != 0)
		throwBadWrite("cannot call close before document/element is complete");
}

DocID ResultsEventWriter::allocateDocID(void)
{
	DBXML_ASSERT(depth_ == 0);

	XmlManager& mgr = res_->getManager();
	CacheDatabaseMinder& dbMinder = res_->getDbMinder();
	return dbMinder.allocateDocID((Manager&)mgr);
}

void ResultsEventWriter::createEventWriter(void)
{
	DBXML_ASSERT(depth_ == 0);
	DBXML_ASSERT(!writer_);
	DBXML_ASSERT(!xmlDoc_);

	// prepare to create NsEventWriter
	XmlManager& mgr = res_->getManager();
	CacheDatabaseMinder& dbMinder = res_->getDbMinder();
	xmlDoc_ = mgr.createDocument();

	// allocate a DocID
	DocID did = dbMinder.allocateDocID((Manager&)mgr);

	// get the temp DB for construction (container id 0)
	CacheDatabase *cdb = dbMinder.findOrAllocate((Manager&)mgr, 0);

	// Tell the document to use this database, and that it's
	// content is "NsDom"
	// TBD GMF: Eventually it may be that Document and NsDocument objects
	// will not be required by DbXmlNodeImpl, so creation/init of Document
	// objects will not be required.
	DictionaryDatabase *dict = ((Manager&)mgr).getDictionary();
	((Document&)xmlDoc_).setContentAsNsDom(did, cdb);

	writer_ = new NsEventWriter(cdb->getDb(), dict, did);
	writer_->writeStartDocument(version_, encoding_, standalone_);

	DBXML_ASSERT(writer_);
	DBXML_ASSERT(xmlDoc_);
}

void ResultsEventWriter::addNode(void)
{
	DBXML_ASSERT(writer_);
	DBXML_ASSERT(xmlDoc_);
	DBXML_ASSERT(depth_ == 0);

	writer_->writeEndDocument();
	writer_->close();

	XmlValue value(xmlDoc_);
	res_->add(value);

	// reset state
	xmlDoc_ = 0;
	writer_ = 0;
	version_ = NULL;
	encoding_ = NULL;
	standalone_ = NULL;
}

void ResultsEventWriter::throwBadWrite(const char *msg)
{
	throw XmlException(XmlException::EVENT_ERROR,
			   std::string("XmlResult::asEventWriter(): ") + msg);
}

void ResultsEventWriter::writeAttribute(const unsigned char *localName,
					const unsigned char *prefix,
					const unsigned char *uri,
					const unsigned char *value,
					bool isSpecified)
{
	if (depth_!=0) {
		DBXML_ASSERT(writer_);
		writer_->writeAttribute(localName, prefix, uri, value, isSpecified);
	} else {
		// top-level attribute
		UTF8ToXMLCh lname((const char *)localName);
		UTF8ToXMLCh pfx((const char *)prefix);
		UTF8ToXMLCh u((const char *)uri);
		UTF8ToXMLCh val((const char *)value);

		DbXmlNodeImpl *node = new DbXmlAttributeNode(pfx.str(), u.str(),
						lname.str(), val.str(),
						NULL, NULL, NULL,
						allocateDocID(), NsNid(), 0);

		// assuming Value::create() has changed to just take "bool lazyDocs"
		res_->add(XmlValue(Value::create((const Node::Ptr)node, false)));
	}
}

void ResultsEventWriter::writeText(XmlEventReader::XmlEventType type,
				   const unsigned char *text,
				   size_t length)
{
	if (depth_!=0) {
		DBXML_ASSERT(writer_);
		writer_->writeText(type, text, length);
	} else {
		// top-level text
		short nsNodeType = 0;
		switch (type) {
		case XmlEventReader::Characters:
			nsNodeType = nsNodeText;
			break;
		case XmlEventReader::Comment:
			nsNodeType = nsNodeComment;
			break;
		case XmlEventReader::CDATA:
			nsNodeType = nsNodeCDATA;
			break;
		default:
			throwBadWrite("writeText called with bad type.");
			break;
		}

		UTF8ToXMLCh value((const char *)text);
		DbXmlNodeImpl *node = new DbXmlTextNode(nsNodeType,
							value.str(),
							allocateDocID());

		// assuming Value::create() has changed to just take "bool lazyDocs"
		res_->add(XmlValue(Value::create((const Node::Ptr)node, false)));
	}
}

void ResultsEventWriter::writeStartDocument(const unsigned char *version,
					    const unsigned char *encoding,
					    const unsigned char *standalone)
{
	if (hasStartDocument_)
		throwBadWrite("writeStartDocument: attempt to create nested documents");

	version_ = version;
	encoding_ = encoding;
	standalone_ = standalone;

	if (writer_ == 0) {
		DBXML_ASSERT(depth_ == 0);
		createEventWriter();
	}
	DBXML_ASSERT(writer_);
	hasStartDocument_ = true;
	depth_++;
}

void ResultsEventWriter::writeEndDocument()
{
	if (depth_ == 0)
		throwBadWrite("writeEndDocument called for empty Document");
	if (--depth_ == 0)
		addNode();
	hasStartDocument_ = false;
}

void ResultsEventWriter::writeStartElement(const unsigned char *localName,
					   const unsigned char *prefix,
					   const unsigned char *uri,
					   int numAttributes,
					   bool isEmpty)
{
	if (writer_ == 0) {
		DBXML_ASSERT(depth_ == 0);
		createEventWriter();
	}
	DBXML_ASSERT(writer_);
	writer_->writeStartElement(localName, prefix, uri, numAttributes, isEmpty);

	if (!isEmpty)
		depth_++;
	else if (depth_ == 0)
		addNode();
}

void ResultsEventWriter::writeEndElement(const unsigned char *localName,
					 const unsigned char *prefix,
					 const unsigned char *uri)
{
	if (depth_ == 0)
		throwBadWrite("writeEndElement called for empty Element");

	DBXML_ASSERT(writer_);
	writer_->writeEndElement(localName, prefix, uri);

	if (--depth_ == 0)
		addNode();
}

void ResultsEventWriter::writeProcessingInstruction(const unsigned char *target,
						    const unsigned char *data)
{
	if (depth_ != 0) {
		DBXML_ASSERT(writer_);
		writer_->writeProcessingInstruction(target, data);
	} else {
		UTF8ToXMLCh targetXmlCh((const char *)target);
		UTF8ToXMLCh dataXmlCh((const char *)data);
		DbXmlNodeImpl *node = new DbXmlTextNode(targetXmlCh.str(),
							dataXmlCh.str(),
							allocateDocID());

		// assuming Value::create() has changed to just take "bool lazyDocs"
		res_->add(XmlValue(Value::create((const Node::Ptr)node, false)));
	}
}

void ResultsEventWriter::writeStartEntity(const unsigned char *name,
					  bool expandedInfoFollows)
{
	throwBadWrite("does not support writeStartEntity.");
}

void ResultsEventWriter::writeEndEntity(const unsigned char *name)
{
	throwBadWrite("does not support writeEndEntity.");
}

void ResultsEventWriter::writeDTD(const unsigned char *dtd, size_t length)
{
	throwBadWrite("does not support writeDTD.");
}
