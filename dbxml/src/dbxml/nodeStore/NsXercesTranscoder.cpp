//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsXercesTranscoder.hpp"
#include "NsDom.hpp"
#include "EventWriter.hpp"
#include "NsDocument.hpp"
#include "NsDocumentDatabase.hpp"
#include "NsConstants.hpp"
#include "NsNode.hpp"
#include "../UTF8.hpp"
#include <xercesc/framework/MemoryManager.hpp>

#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace std;
using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

NsXercesTranscoder::NsXercesTranscoder(Transaction *txn, DbWrapper *db,
				       DictionaryDatabase *ddb,
				       const DocID &docId, int cid,
				       u_int32_t flags)
	: NsHandlerBase(&_document, 0), _document(0),
	  _needsStart(false)
{
	// NOTE: it's ok for db to be null, which is the case for
	// node indexes on wholedoc containers
	_document.initDoc(txn, db, ddb, docId, cid, flags);
}

NsXercesTranscoder::~NsXercesTranscoder()
{
}

void
NsXercesTranscoder::startDocument(const xmlch_t *sniffedEncoding)
{
	// create the document node
	NsNode *node = NsNode::allocNode(0, NS_STANDALONE);
	node->setFlag(NS_ISDOCUMENT);
	_current = node;
	node->acquire();
	
	nextId(node->getFullNid());
	DBXML_ASSERT(node->getNid().isDocRootNid());

	if (sniffedEncoding && *sniffedEncoding) {
		_doc->setSniffedEncodingStr((const xmlbyte_t *)XMLChToUTF8Null(sniffedEncoding).str());
	} else {
		_doc->setSniffedEncodingStr(0);
	}

	_needsStart = true;
}

void
NsXercesTranscoder::xmlDecl(const xmlch_t *xmlDecl,
			    const xmlch_t *encodingStr,
			    const xmlch_t *standaloneStr)
{
	DBXML_ASSERT(_current && _current->isDoc());
	// initialize document element-specific state
	if (xmlDecl && *xmlDecl) {
		if (NsUtil::nsStringEqual(xmlDecl, _decl_1_0 ))
			_doc->setXmlDecl(NS_DECL_1_0);
		else if (NsUtil::nsStringEqual(xmlDecl, _decl_1_1 ))
			_doc->setXmlDecl(NS_DECL_1_1);
		else
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "startDocument: bad XML decl",
						 __FILE__, __LINE__);
	}
	if (encodingStr && *encodingStr) {
		_doc->setEncodingStr((const xmlbyte_t *)XMLChToUTF8Null(encodingStr).str());
	}
	if (standaloneStr && *standaloneStr) {
		if (*standaloneStr == (unsigned short)('y'))
			_doc->setStandalone(true);
		else
			_doc->setStandalone(false);
	}
	if (_needsStart)
		doStart();
}

void
NsXercesTranscoder::doStart()
{
	DBXML_ASSERT(_needsStart);
	_needsStart = false;
	if (_ewriter) {
		_ewriter->writeStartDocument(_doc->getXmlDecl(),
					    _doc->getEncodingStr(),
					    _doc->getStandaloneStr());
		
		_ewriter->writeSniffedEncoding(_doc->getSniffedEncodingStr());
	}
}

void
NsXercesTranscoder::endDocument()
{
	endDoc();
	// event writer
	if (_ewriter)
		_ewriter->writeEndDocument();
}

void
NsXercesTranscoder::startElement(const xmlch_t *localName,
				 const xmlch_t *prefix,
				 const xmlch_t *uri,
				 NsEventAttrList16 *attrs,
				 const uint32_t attrCount,
				 bool isEmpty)
{
	if (_needsStart)
		doStart();

	// create the node
	NsNode *node = NsNode::allocNode(attrCount, NS_STANDALONE);
	nextId(node->getFullNid());
	
	NsDonator lname8(localName,
			 NsUtil::nsStringLen(localName));

	XMLChToUTF8Null uri8(uri);
	XMLChToUTF8Null prefix8(prefix);

	node->acquire();
	// initialize the node plumbing
	try {
		startElem(node, lname8.getStr(),
			  uri8.str(), prefix8.str(), true);
	} catch (...) {
		node->release();
		throw;
	}

	if (attrCount != 0) {
		for (unsigned int i = 0; i < attrCount; i++) {
			node->addAttr(_doc, attrs->prefix(i),
				      attrs->uri(i), attrs->localName(i),
				      attrs->value(i),
				      attrs->isSpecified(i));
		}
	}

	// endElem() must not delete node
	if (isEmpty)
		endElem();

	// event writer
	if (_ewriter) {
		const xmlbyte_t *tname = (const xmlbyte_t *) node->getNameChars();
		// constructor is ok with empty attr list.
		NsEventNodeAttrList alist(node->getAttrList(), *_doc);
		NsNodeIndexNodeInfo ninfo(node);
		_ewriter->writeStartElementWithAttrs(
			tname, (const unsigned char *)prefix8.str(),
			(const unsigned char *)uri8.str(), attrCount,
			(attrCount ? &alist : 0),
			&ninfo, isEmpty);
	}
}

void
NsXercesTranscoder::endElement(const xmlch_t *localName,
			       const xmlch_t *prefix,
			       const xmlch_t *uri)
{
	// After endElem(), _current has become _previous
	NsNode *node = _current;

	// common node work (must not delete node)
	endElem();

	// This callback is done last because structural stats counts the size of the
	// node which is not accurate until endElem() has been called.
	if (_ewriter) {
		DBXML_ASSERT(node);
		XMLChToUTF8Null uri8(uri);
		XMLChToUTF8Null prefix8(prefix);
		NsNodeIndexNodeInfo ninfo(node);
		const xmlbyte_t *tname = (const xmlbyte_t *)node->getNameChars();
		_ewriter->writeEndElementWithNode(
			tname, (const unsigned char *)prefix8.str(),
			(const unsigned char *)uri8.str(), &ninfo);
	}

}

void
NsXercesTranscoder::doCharacters(const xmlch_t *characters,
				 size_t len, bool isCDATA,
				 bool ignorable)
{
	if (_needsStart)
		doStart();

	bool needsEscape = false;
	if (!len)
		len = NsUtil::nsStringLen(characters);
	enum checkType ttype = (isCDATA || ignorable) ? ignore : isCharacters;
	NsDonator chars(characters, len, ttype);
	uint32_t textType;
	if (isCDATA)
		textType = NS_CDATA;
	else {
		textType = NS_TEXT;
		if (chars.getHasEntity()) {
			textType |= NS_ENTITY_CHK;
			needsEscape = true;
		}
	}
	if (ignorable)
		textType |= NS_IGNORABLE;

	// do event before adding text.  If coalesced, the donated
	// string will be deleted by addText().
	if (_ewriter) {
		XmlEventReader::XmlEventType type;
		if (isCDATA)
			type = XmlEventReader::CDATA;
		else if (ignorable)
			type = XmlEventReader::Whitespace;
		else
			type = XmlEventReader::Characters;
		_ewriter->writeTextWithEscape(type, chars.getStr(),
					     chars.getLen(), needsEscape);
	}
	
	addText(chars.getStr(), chars.getLen(), textType, true);
}

void
NsXercesTranscoder::characters(const xmlch_t *characters,
			       size_t len, bool isCDATA,
			       bool needsEscape)
{
	doCharacters(characters, len, isCDATA, false);
}

void
NsXercesTranscoder::ignorableWhitespace(const xmlch_t *characters,
					size_t len, bool isCDATA)
{
	doCharacters(characters, len, isCDATA, true);
}

void
NsXercesTranscoder::comment(const xmlch_t *comment, size_t len)
{
	if (_needsStart)
		doStart();

	if (!len)
		len = NsUtil::nsStringLen(comment);
	NsDonator chars(comment, len);
	addText(chars.getStr(), chars.getLen(), NS_COMMENT, true);

	// event writer (comment text will never be deleted by addText)
	if (_ewriter)
		_ewriter->writeText(XmlEventReader::Comment,
				    chars.getStr(), chars.getLen());
}

void
NsXercesTranscoder::processingInstruction(const xmlch_t *target,
					  const xmlch_t *data)
{
	if (_needsStart)
		doStart();

	NsDonator chars(target, data);
	addPI(chars.getStr(), chars.getStr2(), true, chars.getLen());

	// event writer (PI text will never be deleted by addText)
	if (_ewriter)
		_ewriter->writeProcessingInstruction(chars.getStr(),
						    chars.getStr2());
}

#ifdef NS_USE_SCHEMATYPES
void
NsXercesTranscoder::setTypeInfo(const xmlch_t *name,
				const xmlch_t *uri)
{
	// TBD put type info into _current element -- add method
	// to NsHandlerBase() for this.
	// event handler (PI text will never be deleted by addText)
#if 0	
	NsDonator lname(name, NsUtil::nsStringLen(name));
	NsDonator luri(uri, (uri ? NsUtil::nsStringLen(uri) : 0));

	// TBD...
	
	if (_ewriter)
		_ewriter->setTypeInfo(lname.getStr(), luri.getStr());
#endif	
}
#endif

void
NsXercesTranscoder::docTypeDecl(const xmlch_t *data, size_t len)
{
	if (_needsStart)
		doStart();

	NsDonator chars(data, len);
	addText(chars.getStr(), chars.getLen(), NS_SUBSET, true);
	if (_ewriter)
		_ewriter->writeDTD(chars.getStr(), chars.getLen());
}

void
NsXercesTranscoder::startEntity(const xmlch_t *name, size_t len)
{
	if (_needsStart)
		doStart();
	NsDonator chars(name, len);
	addText(chars.getStr(), chars.getLen(), NS_ENTSTART, true);
	if (_ewriter)
		_ewriter->writeStartEntity(chars.getStr(), true);
}

void
NsXercesTranscoder::endEntity(const xmlch_t *name, size_t len)
{
	NsDonator chars(name, len);
	addText(chars.getStr(), chars.getLen(), NS_ENTEND, true);
	if (_ewriter)
		_ewriter->writeEndEntity(chars.getStr());
}

