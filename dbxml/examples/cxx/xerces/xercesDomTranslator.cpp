/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 */

/*
 * Implementation of sample code that uses Berkeley DB XML XmlEventReader
 * and XmlEventWriter interfaces to translate documents to and
 * from Xerces-C DOM documents.  It is provided as-is without
 * extensive testing.
 * See comments in accompanying header file for a high-level
 * description of the implementation.
 */

#include "xercesDomTranslator.hpp"
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <iostream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

#define localAssert(expr, msg) if (!(expr)) throw(msg)

//
// toXMLCh and fromXMLCh are utility classes to transcode
// to/from Xerces-C DOM (these could easily be replaced
// by better/more efficient code).  They are necessary because
// Xerces-C uses XMLCh (usually UTF-16) and BDB XML uses UTF-8
// in its event interfaces.
//
class toXMLCh
{
public:
	toXMLCh(const unsigned char *str) {
		if (str)
			str_ = XMLString::transcode((const char *)str);
		else
			str_ = 0;
	}
	toXMLCh(const char *str) {
		if (str)
			str_ = XMLString::transcode(str);
		else
			str_ = 0;
	}
	~toXMLCh() { XMLString::release(&str_); }
	const XMLCh *str() const { return str_; }
private:
	XMLCh *str_;
		
};

class fromXMLCh
{
public:
	fromXMLCh(const XMLCh *str) {
		if (str && *str)
			str_ = XMLString::transcode(str);
		else
			str_ = 0;
	}
	~fromXMLCh() { XMLString::release(&str_); }
	const unsigned char *str() const {
		return (const unsigned char *)str_;
	}
	operator const unsigned char *() {
		return str();
	}
private:
	char *str_;
		
};

//
// Public API implementation
//
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *
getContentAsDOM(const XmlValue &value)
{
	XmlEventReader &reader = value.asEventReader();
	DOMEventWriter domWriter;
	XmlEventReaderToWriter r2w(reader, domWriter,
				   false, false);
	r2w.start();
	reader.close();
	return domWriter.getDocument();
}

void putDocumentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc,
		      const std::string &name,
		      XmlManager &manager,
		      XmlContainer &container,
		      XmlTransaction &txn, u_int32_t flags)
{
	XmlUpdateContext uc = manager.createUpdateContext();
	XmlDocument doc = manager.createDocument();
	doc.setName(name);
	XmlEventWriter &writer = container.putDocumentAsEventWriter(
		txn, doc, uc, flags);
	DOMToEventWriter dom2writer(writer, domDoc);
	dom2writer.start();
	// note DOMToEventWriter destructor closes writer
}

void putDocumentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc,
		      const std::string &name,
		      XmlManager &manager,
		      XmlContainer &container,
		      u_int32_t flags)
{
	XmlUpdateContext uc = manager.createUpdateContext();
	XmlDocument doc = manager.createDocument();
	doc.setName(name);
	XmlEventWriter &writer = container.putDocumentAsEventWriter(
		doc, uc, flags);
	DOMToEventWriter dom2writer(writer, domDoc);
	dom2writer.start();
	// note DOMToEventWriter destructor closes writer
}

//
// Internals of implementation from here down
//

//
// EventReaderToDOM Implementation.  This class knows how to
// construct Xerces-C DOM objects from BDB XML events
//
DOMEventWriter::DOMEventWriter()
	: doc_(0), currentParent_(0), currentNode_(0)
{
	DOMImplementation *domImpl = DOMImplementation::getImplementation();
	doc_ = domImpl->createDocument();
	currentParent_ = doc_;
	currentNode_ = doc_;
}

void DOMEventWriter::close()
{
	delete this;
}

void DOMEventWriter::writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified)
{
	localAssert((doc_ && currentParent_), "state");
	DOMAttr *attr = doc_->createAttributeNS(
		toXMLCh(uri).str(), toXMLCh(localName).str());
	if (prefix)
		attr->setPrefix(toXMLCh(prefix).str());
	attr->setValue(toXMLCh(value).str());
	currentNode_ = attr;
	currentParent_->getAttributes()->setNamedItemNS(attr);
}

// text, comments, CDATA, ignorable whitespace
// length does not include trailing null
void DOMEventWriter::writeText(
	DbXml::XmlEventReader::XmlEventType type,
	const unsigned char *text,
	size_t length)
{
	// TBD -- maybe use length...
	toXMLCh x(text);
	if(currentNode_ && currentNode_->getNodeType() == DOMNode::TEXT_NODE
		&& type == XmlEventReader::Characters) {
		((DOMText *)currentNode_)->appendData(x.str()); // coalesce
	} else {
		DOMNode *node = 0;
		switch (type) {
		case XmlEventReader::Characters:
		case XmlEventReader::Whitespace:
			node = doc_->createTextNode(x.str());
			break;
		case XmlEventReader::CDATA:
			node = doc_->createCDATASection(x.str());
			break;
		case XmlEventReader::Comment:
			node = doc_->createComment(x.str());
			break;
		default:
			localAssert(false, "unknown text type");
		};
		localAssert(currentParent_, "state");
		currentParent_->appendChild(node);
		currentNode_ = node;
	}
}

// processing Instruction
void DOMEventWriter::writeProcessingInstruction(
	const unsigned char *target,
	const unsigned char *data)
{
	toXMLCh t(target);
	toXMLCh d(data);
	DOMProcessingInstruction *pi = doc_->createProcessingInstruction(
		t.str(), d.str());
	localAssert(currentParent_, "state");
	currentParent_->appendChild(pi);
	currentNode_ = pi;
}

// elements
void DOMEventWriter::writeStartElement(
	const unsigned char *localName,
	const unsigned char *prefix,
	const unsigned char *uri,
	int numAttributes,
	bool isEmpty)
{
	toXMLCh lname(localName);
	toXMLCh u(uri);
	
	DOMElement *elem = doc_->createElementNS(u.str(), lname.str());
	if(prefix != 0)
		elem->setPrefix(toXMLCh(prefix).str());
	if (currentParent_)
		currentParent_->appendChild(elem);
	currentParent_ = elem;
	currentNode_ = elem;
	// empty elements don't get end element events
	if (isEmpty)
		writeEndElement(localName, prefix, uri);
}

void DOMEventWriter::writeEndElement(
	const unsigned char *localName,
	const unsigned char *prefix,
	const unsigned char *uri)
{
	currentNode_ = currentParent_;
	currentParent_ = currentNode_->getParentNode();
}
	
// DTD
void DOMEventWriter::writeDTD(
	const unsigned char *dtd, size_t length)
{
	localAssert(false, "cannot write DTD");
}

// XML decl and start document
void DOMEventWriter::writeStartDocument(
	const unsigned char *version,
	const unsigned char *encoding,
	const unsigned char *standalone)
{
	// no-op -- allocate document node in ctor
	localAssert(doc_, "state");
}

void DOMEventWriter::writeEndDocument()
{
	currentNode_ = currentParent_;
	currentParent_ = currentParent_->getParentNode();
}

// note the start of entity expansion
void DOMEventWriter::writeStartEntity(
	const unsigned char *name,
	bool expandedInfoFollows)
{
}

// note the end of entity expansion
void DOMEventWriter::writeEndEntity(
	const unsigned char *name)
{
}

//
// DOMToEventWriter Implementation.  This class knows how
// to walk a Xerces-C DOM tree writing BDB XML events.
// It is a recursive algorithm.  For very deep, structured XML,
// one may want to rewrite this to remove recursion.
//
DOMToEventWriter::DOMToEventWriter(
	DbXml::XmlEventWriter &writer,
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc)
	: writer_(writer), doc_(domDoc)
{}

DOMToEventWriter::~DOMToEventWriter()
{
	writer_.close();
}

void DOMToEventWriter::start()
{
	toEvents(doc_);
}


// recursive algorithm for generating events
void DOMToEventWriter::toEvents(const DOMNode *node)
{
	switch(node->getNodeType()) {
	case DOMNode::DOCUMENT_NODE: {
		writer_.writeStartDocument(
			NULL,
			(const unsigned char *)"UTF-8",
			NULL);
		DOMNode *child = node->getFirstChild();
		while(child != 0) {
			toEvents(child);
			child = child->getNextSibling();
		}
		writer_.writeEndDocument();
		break;
	}
	case DOMNode::ELEMENT_NODE: {
		fromXMLCh lname(node->getLocalName());
		fromXMLCh pfx(node->getPrefix());
		fromXMLCh uri(node->getNamespaceURI());
		DOMNode *child = node->getFirstChild();
		bool isEmpty = (child ? false : true);
		DOMNamedNodeMap *attrs = node->getAttributes();
		writer_.writeStartElement(
			lname.str(), pfx.str(), uri.str(),
			attrs->getLength(), isEmpty);
		
		for(unsigned int i = 0; i < attrs->getLength(); ++i) {
			toEvents(attrs->item(i));
		}
		
		while(child != 0) {
			toEvents(child);
			child = child->getNextSibling();
		}
		if (!isEmpty)
			writer_.writeEndElement(
				lname.str(), pfx.str(), uri.str());
		break;
	}
	case DOMNode::TEXT_NODE:
		writer_.writeText(XmlEventReader::Characters,
				  fromXMLCh(node->getNodeValue()).str(),
				  0);
		break;
	case DOMNode::CDATA_SECTION_NODE:
		writer_.writeText(XmlEventReader::CDATA,
				  fromXMLCh(node->getNodeValue()).str(),
				  0);
		break;
	case DOMNode::COMMENT_NODE:
		writer_.writeText(XmlEventReader::Comment,
				  fromXMLCh(node->getNodeValue()).str(),
				  0);
		break;
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
		writer_.writeProcessingInstruction(
			fromXMLCh(node->getNodeName()).str(),
			fromXMLCh(node->getNodeValue()).str());
		break;
	case DOMNode::ATTRIBUTE_NODE:
		writer_.writeAttribute(
			fromXMLCh(node->getLocalName()).str(),
			fromXMLCh(node->getPrefix()).str(),
			fromXMLCh(node->getNamespaceURI()).str(),
			fromXMLCh(node->getNodeValue()).str(),
			true);
		break;
	default:
		localAssert(false, "state");
		break;
	}
}

//
// here down is test code to drive the samples
//
#if _XERCES_VERSION >= 30000
#include <xercesc/dom/DOMLSSerializer.hpp>
#else
#include <xercesc/dom/DOMWriter.hpp>
#endif
#include <xercesc/framework/StdOutFormatTarget.hpp>
static void serialize(DOMDocument *doc)
{
	DOMImplementation *domImpl = DOMImplementation::getImplementation();
	XMLFormatTarget *fmt = new StdOutFormatTarget();

#if _XERCES_VERSION >= 30000
	DOMLSSerializer *dwriter = domImpl->createLSSerializer();
	DOMLSOutput *output = domImpl->createLSOutput();
	output->setByteStream(fmt);
	dwriter->write(doc, output);
	delete output;
	delete dwriter;
#else
	DOMWriter *dwriter = domImpl->createDOMWriter();
	dwriter->writeNode(fmt, *doc);
	delete dwriter;
#endif

	delete fmt;
}

using namespace std;

int main(int argc, char **argv)
{
	XmlManager mgr;
	XmlDocument doc = mgr.createDocument();
	doc.setName("doc");
	doc.setContent("<root><a/><b attr='1' a1='2'>b text<!--comment--></b></root>");
	XmlValue value(doc);
	// Turn the document into DOM
	DOMDocument *ddoc = getContentAsDOM(value);

	// serialize the resulting DOM tree
	serialize(ddoc);

	// put the DOM in a container
	XmlContainer cont = mgr.createContainer("");
	putDocumentAsDOM(ddoc,
			 "doc", mgr, cont, 0);
	// done with DOMDocument *
	ddoc->release();

	// get document and look at content
	XmlDocument newdoc = cont.getDocument("doc");
	std::string content;
	content = newdoc.getContent(content);
	cout << "\n***************\n";
	cout << content << endl;
	return 0;
}
