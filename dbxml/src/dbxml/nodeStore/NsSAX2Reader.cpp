//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
// The code in this file is derived from Apache's NsSAX2Reader code
/*
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999-2003 The Apache Software Foundation.  All rights
 * reserved.
 *
 */
/*
 * This is a stripped-down version of the Xerces SAX2Reader.
 * It does not generate any SAX2 events, and therefore has no
 * support for set/get of SAX2 handlers.  Additional pieces may
 * yet be removed.  See comments in NsSAX2Reader.hpp
 */

#include <xercesc/util/IOException.hpp>
#include <xercesc/util/XMLChTranscoder.hpp>
#include <xercesc/util/RefStackOf.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax2/DeclHandler.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/internal/XMLScannerResolver.hpp>
#include <xercesc/validators/common/GrammarResolver.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/framework/XMLSchemaDescription.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <string.h>

#include <xqilla/utils/XPath2Utils.hpp>

#include "NsSAX2Reader.hpp"
#include "NsUtil.hpp"
#include "../Log.hpp"
#include "../XmlInputStreamWrapper.hpp"
#include "../Manager.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include <sstream>

XERCES_CPP_NAMESPACE_USE

#define XercesStringLen(s) (unsigned int) XMLString::stringLen(s)
//
// DB XML additions
//
#define DBXML_MIN_URI 3 // 3 is xml, 4 is xmlns
const XMLCh gEndIntSubset[] = { chCloseSquare, chCloseAngle, chNull };
const XMLCh gStartIntSubset[] = { chOpenSquare, chNull };
const XMLCh gEndExtSubset[] = {  chCloseAngle, chNull };

using namespace DbXml;

class NsEventAttrListXimpl : public NsEventAttrList16 {
public:
	NsEventAttrListXimpl(const RefVectorOf<XMLAttr> &list,
			     const SAX2XMLReader &reader,
			     int count);
	virtual ~NsEventAttrListXimpl();
	virtual bool isEmpty(void) const;
	virtual int numAttributes(void) const { return _count; }
	virtual const xmlch_t *prefix(int index) const {
		const xmlch_t *p = (_list.elementAt(index))->getPrefix();
		return ((p && *p) ? p : 0);
	}
	virtual const xmlch_t *localName(int index) const {
		return (_list.elementAt(index))->getName();
	}
	virtual const xmlch_t *value(int index) const {
		return (_list.elementAt(index))->getValue();
	}
	virtual const xmlch_t *uri(int index) const {
		const XMLAttr *attr = _list.elementAt(index);
		int uriId = attr->getURIId();
		if (XPath2Utils::equals(attr->getQName(),
					XMLUni::fgXMLNSString))
			return XMLUni::fgXMLNSURIName;
		else if (uriId >= DBXML_MIN_URI)
			return (_reader.getURIText(uriId));
		return NULL;
	}
#ifdef NS_USE_SCHEMATYPES	
	virtual bool hasTypeInfo(int index) const {
		// TBD
		return false;
	}
	virtual const xmlch_t *typeName(int index) const {
		// TBD:  see AbstractDOMParser
		return 0;
	}
	virtual const xmlch_t *typeUri(int index) const {
		// TBD:  see AbstractDOMParser
		return 0;
	}
#endif
	virtual bool needsEscape(int index) const {
		return true;
	}
	virtual bool isSpecified(int index) const {
		return (_list.elementAt(index))->getSpecified();
	}
private:
	const RefVectorOf<XMLAttr> & _list;
	const SAX2XMLReader &_reader;
	int _count;
};

NsEventAttrListXimpl::~NsEventAttrListXimpl() {}
NsEventAttrListXimpl::NsEventAttrListXimpl(const RefVectorOf<XMLAttr> &list,
					   const SAX2XMLReader &reader,
					   int count) :
	_list(list), _reader(reader), _count(count) {}

bool NsEventAttrListXimpl::isEmpty(void) const { return (_count == 0); }
// end DBXML additions

XERCES_CPP_NAMESPACE_USE

static void
_nsSAXunimplemented(int line)
{
	NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				 "nsSAX2Reader: operation not supported",
				 __FILE__, line);
}

const XMLCh gDTDEntityStr[] =
{
	chOpenSquare, chLatin_d, chLatin_t, chLatin_d, chCloseSquare, chNull
};

NsSAX2Reader::NsSAX2Reader(Manager &mgr, Transaction *txn,
			   unsigned int flags,
			   MemoryManager* const  manager,
			   XMLGrammarPool* const gramPool)
	:
	fParseInProgress(false)
	, fElemDepth(0)
	, fPSVIHandler(0)
	, fScanner(0)
	, fGrammarResolver(0)
	, fURIStringPool(0)
	, fValidator(0)
	, fMemoryManager(manager)
	, fGrammarPool(gramPool)
	 // DBXML
	, fBufMgr(manager)
	, fSubset(fBufMgr.bidOnBuffer())
	, fReadingIntSubset(false)
	, fHasExtSubset(false)
	, fHasIntSubset(false)
	, fHandler(0)
	, fMgr(mgr)
	, fResolver(mgr, txn)
	 // end DBXML
{
    try {
        initialize(flags);
    }
    catch(const OutOfMemoryException&) {
	    NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				     "Cannot initialize NsSAX2Reader parser",
				     __FILE__, __LINE__);
    }
    catch(...) {
        cleanUp();
	NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				 "Exception caught in NsSAX2Reader init",
				 __FILE__, __LINE__);
    }
}

NsSAX2Reader::~NsSAX2Reader()
{
    cleanUp();
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Initialize/Cleanup methods
// ---------------------------------------------------------------------------
void NsSAX2Reader::initialize(unsigned int flags)
{
	bool validate =
		((flags & NsParserEventSource::NS_PARSER_WELL_FORMED) == 0);
	// Create grammar resolver and string pool that we pass to the scanner
	fGrammarResolver = new (fMemoryManager)
		GrammarResolver(fGrammarPool, fMemoryManager);
	fURIStringPool = fGrammarResolver->getStringPool();

	//  Create a scanner and tell it what validator to use. Then set us
	//  as the document event handler.
#if 1
	// for now (2.1) always use a scanner that can read
	// a DTD and the internal subset.  This simplifies
	// life, at the expense of performance.
	// TBD: think about more flexible API to control
	// this behavior
	if (flags & NsParserEventSource::NS_PARSER_WF_ONLY)
		fScanner = XMLScannerResolver::resolveScanner(
			XMLUni::fgWFXMLScanner, 0, fGrammarResolver,
			fMemoryManager);
	else
		fScanner = XMLScannerResolver::resolveScanner(
			XMLUni::fgIGXMLScanner, 0, fGrammarResolver,
			fMemoryManager);
#else
	if (validate) {
		fScanner = XMLScannerResolver::resolveScanner(
			XMLUni::fgIGXMLScanner, 0, fGrammarResolver,
			fMemoryManager);
	} else {
		fScanner = XMLScannerResolver::resolveScanner(
			XMLUni::fgWFXMLScanner, 0, fGrammarResolver,
			fMemoryManager);
	}
#endif
	fScanner->setURIStringPool(fURIStringPool);

	// TBD: would error handler be useful?  Scanner uses it.
	fScanner->setErrorReporter(this);
	fScanner->setDocHandler(this);
	fScanner->setDocTypeHandler(this);
	fScanner->setEntityHandler(this);

	// Different default settings, depending on when used:
	// 1.  Initial parse:
	//      o need validation, schema, namespaces
	// 2.  Parse for query (implies initial parse already done):
	//      o don't need validation, but should load DTDs
	//      o want schema for querying and type info
	//
	// NOTE: Indexer can optimize, and not create handler if neither
	// attributes nor elements are indexed.
	//
	// Default settings for DB XML
	setDoNamespaces(true) ;
	if(validate) {
		// don't validate unless really needed (initial insertion)
		bool fullValidate = ((flags & NsParserEventSource::NS_PARSER_MUST_VALIDATE) != 0);
		// allow load, even if not validating, for entities...
		fScanner->setLoadExternalDTD(true);
		fScanner->setValidationConstraintFatal(false);
		// need schema for default attribute values (too bad...),
		// even if not doing full validation
		setDoSchema(true);
		if (fullValidate) {
			setValidationScheme(Val_Auto); // auto
			fScanner->setValidationSchemaFullChecking(true);
		} else {
			fScanner->setIdentityConstraintChecking(false);
			fScanner->setValidationSchemaFullChecking(false);
			setValidationScheme(Val_Never);
		}
	}
}

void NsSAX2Reader::cleanUp()
{
	delete fScanner;
	delete fGrammarResolver;
	// grammar pool must do this
	//delete fURIStringPool;
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader Validator functions
// ---------------------------------------------------------------------------
void NsSAX2Reader::setValidator(XMLValidator* valueToAdopt)
{
	fValidator = valueToAdopt;
	fScanner->setValidator(valueToAdopt);
}

XMLValidator* NsSAX2Reader::getValidator() const
{
	return fScanner->getValidator();
}

// ---------------------------------------------------------------------------
//  SAX2XMLReader Interface
// ---------------------------------------------------------------------------
#if _XERCES_VERSION >= 30000
XMLSize_t NsSAX2Reader::getErrorCount() const
#else
int NsSAX2Reader::getErrorCount() const
#endif
{
	return fScanner->getErrorCount();
}

void NsSAX2Reader::setPSVIHandler(PSVIHandler* const handler)
{
	fPSVIHandler = handler;
	if (fPSVIHandler) {
		fScanner->setPSVIHandler(fPSVIHandler);
	}
	else {
		fScanner->setPSVIHandler(0);
	}
}

void NsSAX2Reader::setExitOnFirstFatalError(const_bool newState)
{
	fScanner->setExitOnFirstFatal(newState);
}

void NsSAX2Reader::setValidationConstraintFatal(const_bool newState)
{
	fScanner->setValidationConstraintFatal(newState);
}

void NsSAX2Reader::parse(XmlInputStream **is) // Consumes XmlInputStream
{
	XmlInputStreamWrapper isw(is);
	parse(isw);
}

void NsSAX2Reader::parse (const InputSource& source)
{
	if (!fHandler) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "NsSAX2Reader -- no handler",
					 __FILE__, __LINE__);
		return;
	}
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INDEXER_PARSER_ERROR,
					 "Multiple entry into NsSAX2Reader",
					 __FILE__, __LINE__);
	try {
		fParseInProgress = true;
		fScanner->scanDocument(source);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
	catch(const XmlException &) {
		fParseInProgress = false;
		throw;
	}
	catch(const XMLException &e) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 (std::string("XMLException exception during NsSAX2Reader parse: ")
					 + XMLChToUTF8(e.getMessage()).str()).c_str(),
					 __FILE__, __LINE__);
	}
	catch (...) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception during NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
}

void NsSAX2Reader::parse (const XMLCh* const systemId)
{
	if (!fHandler) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "NsSAX2Reader -- no handler",
					 __FILE__, __LINE__);
		return;
	}
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INDEXER_PARSER_ERROR,
					 "Multiple entry into NsSAX2Reader",
					 __FILE__, __LINE__);
	try {
		fParseInProgress = true;
		fScanner->scanDocument(systemId);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
	catch(XmlException &) {
		fParseInProgress = false;
		throw;
	}
	catch (...) {
		fParseInProgress = false;
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception during NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
}

void NsSAX2Reader::parse (const char* const systemId)
{
	if (!fHandler) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "NsSAX2Reader -- no handler",
					 __FILE__, __LINE__);
		return;
	}
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INDEXER_PARSER_ERROR,
					 "Multiple entry into NsSAX2Reader",
					 __FILE__, __LINE__);
	try {
		fParseInProgress = true;
		fScanner->scanDocument(systemId);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
	catch(XmlException &) {
		fParseInProgress = false;
		throw;
	}
	catch (...) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception during NsSAX2Reader parse",
					 __FILE__, __LINE__);
	}
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Progressive parse methods.  Not supported (DB XML)
// ---------------------------------------------------------------------------
bool NsSAX2Reader::parseFirst(const XMLCh* const systemId,
			       XMLPScanToken& toFill)
{
	_nsSAXunimplemented(__LINE__); return false;
}

bool NsSAX2Reader::parseFirst(const char* const systemId,
			       XMLPScanToken& toFill)
{
	_nsSAXunimplemented(__LINE__); return false;
}

bool NsSAX2Reader::parseFirst(const InputSource& source,
			       XMLPScanToken& toFill)
{
	_nsSAXunimplemented(__LINE__); return false;
}

bool NsSAX2Reader::parseNext(XMLPScanToken& token)
{
	_nsSAXunimplemented(__LINE__); return false;
}

void NsSAX2Reader::parseReset(XMLPScanToken& token)
{
	// Reset the scanner
	fScanner->scanReset(token);
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Overrides of the XMLDocumentHandler interface
// ---------------------------------------------------------------------------
void NsSAX2Reader::docCharacters(const XMLCh* const chars,
				  const XercesSizeUint length,
				  const_bool cdataSection)
{
	// Suppress the chars before the root element.
	if (!fElemDepth)
		return;
	fHandler->characters(chars, length, cdataSection, true);
}

void NsSAX2Reader::docComment(const XMLCh* const commentText)
{
	uint32_t length = XercesStringLen(commentText);
	fHandler->comment(commentText, length);
}

void NsSAX2Reader::elementTypeInfo(const XMLCh* const typeName,
				    const XMLCh* const typeURI)
{
	// this method is called after startElement, but
	// before endElement, so it's necessary to rely on
	// the fact that the "current" element is correct.
#ifdef NS_USE_SCHEMATYPES	
	if (typeName && *typeName) {
		fHandler->setTypeInfo(typeName, typeURI);
	}
#endif
}

void NsSAX2Reader::XMLDecl(const XMLCh* const versionStr,
			    const XMLCh* const encodingStr,
			    const XMLCh* const standaloneStr,
			    const XMLCh* const autoEncodingStr)
{
	fHandler->xmlDecl(versionStr, encodingStr, standaloneStr);
}

void NsSAX2Reader::docPI(const XMLCh* const target,
			  const XMLCh* const data)
{
	fHandler->processingInstruction(target, data);
}

void NsSAX2Reader::endDocument()
{
	fHandler->endDocument();
}

void NsSAX2Reader::endEntityReference(const XMLEntityDecl& entityDecl)
{
	const xmlch_t *name = entityDecl.getName();
	uint32_t length = XercesStringLen(name);
	fHandler->endEntity(name, length);
}

void NsSAX2Reader::ignorableWhitespace(const XMLCh* const chars,
					const XercesSizeUint length,
					const_bool cdataSection)
{
	fHandler->ignorableWhitespace(chars, length, cdataSection);
}

void NsSAX2Reader::resetDocument()
{
	// Make sure our element depth flag gets set back to zero
	fElemDepth = 0;
}

void NsSAX2Reader::startDocument()
{
	fHandler->startDocument(fScanner->getReaderMgr()->getCurrentEncodingStr());
}

void NsSAX2Reader::startElement(
	const XMLElementDecl& elemDecl,
	const unsigned int elemURLId,
	const XMLCh* const elemPrefix,
	const RefVectorOf<XMLAttr>& attrList,
	const XercesSizeUint attrCount,
	const_bool isEmpty,
	const_bool isRoot)
{
	NsEventAttrListXimpl alist(attrList, *this, (unsigned int)attrCount);
	const XMLCh *uri = 0;
	unsigned int uriId = elemURLId;
	if (uriId >= DBXML_MIN_URI && getDoNamespaces())
		uri = getURIText(elemURLId);
	fHandler->startElement(elemDecl.getBaseName(),
			       (elemPrefix && *elemPrefix) ? elemPrefix : 0,
			       uri, &alist, (unsigned int)attrCount,
			       isEmpty);

	// Bump the element depth counter if not empty
	if (!isEmpty)
		fElemDepth++;
}

void NsSAX2Reader::endElement(const XMLElementDecl& elemDecl,
			       const unsigned int uriId,
			       const_bool isRoot,
			       const XMLCh* const elemPrefix)
{
	const XMLCh *uri = 0;
	if (uriId >= DBXML_MIN_URI && getDoNamespaces())
		uri = getURIText(uriId);
	fHandler->endElement(elemDecl.getBaseName(),
			     (elemPrefix && *elemPrefix) ? elemPrefix : 0,
			     uri);
	//
	//  Dump the element depth down again. Don't let it underflow in case
	//  of malformed XML.
	//
	if (fElemDepth)
		fElemDepth--;
}

void NsSAX2Reader::startEntityReference(const XMLEntityDecl& entityDecl)
{
	const xmlch_t *name = entityDecl.getName();
	uint32_t length = XercesStringLen(name);
	fHandler->startEntity(name, length);
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Overrides of the DocTypeHandler interface
// ---------------------------------------------------------------------------
void NsSAX2Reader::attDef(const DTDElementDecl& elemDecl,
			   const DTDAttDef& attDef,
			   const_bool ignoring)
{
	if (fReadingIntSubset) {
		if (elemDecl.hasAttDefs()) {
			fSubset.append(attDef.getFullName());

			// Get the type and display it
			const XMLAttDef::AttTypes type = attDef.getType();
			switch(type)
			{
			case XMLAttDef::CData :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgCDATAString);
				break;
			case XMLAttDef::ID :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgIDString);
				break;
			case XMLAttDef::IDRef :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgIDRefString);
				break;
			case XMLAttDef::IDRefs :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgIDRefsString);
				break;
			case XMLAttDef::Entity :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgEntityString);
				break;
			case XMLAttDef::Entities :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgEntitiesString);
				break;
			case XMLAttDef::NmToken :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgNmTokenString);
				break;
			case XMLAttDef::NmTokens :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgNmTokensString);
				break;

			case XMLAttDef::Notation :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgNotationString);
				break;

			case XMLAttDef::Enumeration :
				{
				fSubset.append(chSpace);
				const XMLCh* enumString = attDef.getEnumeration();
				int length = (int) XercesStringLen(enumString);
				if (length > 0) {

					fSubset.append(chOpenParen );
					for(int i=0; i<length; i++) {
						if (enumString[i] == chSpace)
							fSubset.append(chPipe);
						else
							fSubset.append(enumString[i]);
					}
					fSubset.append(chCloseParen);
				}
				}
				break;
			default :
				break;
			}
			//get te default types of the attlist
			const XMLAttDef::DefAttTypes def = attDef.getDefaultType();
			switch(def)
			{
			case XMLAttDef::Required :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgRequiredString);
				break;
			case XMLAttDef::Implied :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgImpliedString);
				break;
			case XMLAttDef::Fixed :
				fSubset.append(chSpace);
				fSubset.append(XMLUni::fgFixedString);
				break;
			default :
				break;
			}

			const XMLCh* defaultValue = attDef.getValue();
			if (defaultValue != 0) {
				fSubset.append(chSpace);
				fSubset.append(chDoubleQuote);
				fSubset.append(defaultValue);
				fSubset.append(chDoubleQuote);
			}
		}
	}
}

void NsSAX2Reader::doctypeComment(const XMLCh* const commentText)
{
	if (fReadingIntSubset) {
		if (commentText != 0) {
			fSubset.append(XMLUni::fgCommentString);
			fSubset.append(chSpace);
			fSubset.append(commentText);
			fSubset.append(chSpace);
			fSubset.append(chDash);
			fSubset.append(chDash);
			fSubset.append(chCloseAngle);
		}
	}
}

void NsSAX2Reader::doctypeDecl(const DTDElementDecl& elemDecl,
				const XMLCh* const publicId,
				const XMLCh* const systemId,
				const_bool hasIntSubset,
				const_bool hasExtSubset)
{
	fSubset.append(XMLUni::fgDocTypeString);
	fSubset.append(chSpace);
	fSubset.append(elemDecl.getFullName());
	fSubset.append(chSpace);

	if (publicId && *publicId) {
		fSubset.append(XMLUni::fgPubIDString);
		fSubset.append(chSpace);
		fSubset.append(chDoubleQuote);
		fSubset.append(publicId);
		fSubset.append(chDoubleQuote);
		fSubset.append(chSpace);
	}
	if (systemId && *systemId) {
		if (!(publicId && *publicId)) {
			fSubset.append(XMLUni::fgSysIDString);
			fSubset.append(chSpace);
		}
		fSubset.append(chDoubleQuote);
		fSubset.append(systemId);
		fSubset.append(chDoubleQuote);
	}
	if (hasIntSubset) {
		fSubset.append(gStartIntSubset);
		fHasIntSubset = true;
	} else if (hasExtSubset) {
		fHasExtSubset = true;
		fSubset.append(gEndExtSubset);
	}
}

void NsSAX2Reader::doctypePI(const XMLCh* const target,
			      const XMLCh* const data)
{
	if (fReadingIntSubset) {
		//add these chars to internalSubset variable
		fSubset.append(chOpenAngle);
		fSubset.append(chQuestion);
		fSubset.append(target);
		fSubset.append(chSpace);
		fSubset.append(data);
		fSubset.append(chQuestion);
		fSubset.append(chCloseAngle);
	}
}

void NsSAX2Reader::doctypeWhitespace(const XMLCh* const chars,
				     const XercesSizeUint length)
{
	if (fReadingIntSubset)
		fSubset.append(chars);
}

void NsSAX2Reader::elementDecl(const DTDElementDecl& elemDecl,
				const_bool isIgnored)
{
	if (fReadingIntSubset) {
		fSubset.append(chOpenAngle);
		fSubset.append(chBang);
		fSubset.append(XMLUni::fgElemString);
		fSubset.append(chSpace);
		fSubset.append(elemDecl.getFullName());

		//get the ContentSpec information
		const XMLCh* contentModel =
			elemDecl.getFormattedContentModel();
		if (contentModel != 0) {
			fSubset.append(chSpace);
			fSubset.append(contentModel);
		}
		fSubset.append(chCloseAngle);
	}
}

void NsSAX2Reader::endAttList(const DTDElementDecl&)
{
	if (fReadingIntSubset) {
		//print the closing angle
		fSubset.append(chCloseAngle);
	}
}

void NsSAX2Reader::endIntSubset()
{
	fReadingIntSubset = false;
	fSubset.append(gEndIntSubset);
	const XMLCh *subset = fSubset.getRawBuffer();
	uint32_t len = XercesStringLen(subset);
	fHandler->docTypeDecl(subset, len);
}

void NsSAX2Reader::endExtSubset()
{
	// If there was an external DTD reference,
	// put its content in the document
	if (fHasExtSubset) {
		const XMLCh *subset = fSubset.getRawBuffer();
		uint32_t len = XercesStringLen(subset);
		fHandler->docTypeDecl(subset, len);
	}
}

void NsSAX2Reader::entityDecl(const DTDEntityDecl& entityDecl,
			       const_bool isPEDecl,
			       const_bool isIgnored)
{
	if (fReadingIntSubset) {
		//add these chars to internalSubset variable
		fSubset.append(chOpenAngle);
		fSubset.append(chBang);
		fSubset.append(XMLUni::fgEntityString);
		fSubset.append(chSpace);

		fSubset.append(entityDecl.getName());

		const XMLCh* id = entityDecl.getPublicId();
		if (id != 0) {
			fSubset.append(chSpace);
			fSubset.append(XMLUni::fgPubIDString);
			fSubset.append(chSpace);
			fSubset.append(chDoubleQuote);
			fSubset.append(id);
			fSubset.append(chDoubleQuote);
		}
		id = entityDecl.getSystemId();
		if (id != 0) {
			fSubset.append(chSpace);
			fSubset.append(XMLUni::fgSysIDString);
			fSubset.append(chSpace);
			fSubset.append(chDoubleQuote);
			fSubset.append(id);
			fSubset.append(chDoubleQuote);

		}
		id = entityDecl.getNotationName();
		if (id != 0) {
			fSubset.append(chSpace);
			fSubset.append(XMLUni::fgNDATAString);
			fSubset.append(chSpace);
			fSubset.append(id);
		}
		id = entityDecl.getValue();
		if (id !=0) {
			fSubset.append(chSpace);
			fSubset.append(chDoubleQuote);
			fSubset.append(id);
			fSubset.append(chDoubleQuote);
		}

		fSubset.append(chCloseAngle);
	}
}

void NsSAX2Reader::resetDocType()
{

}

void NsSAX2Reader::notationDecl(const XMLNotationDecl& notDecl,
				 const_bool isIgnored)
{
	if (fReadingIntSubset) {
		//add thes chars to internalSubset variable
		fSubset.append(chOpenAngle);
		fSubset.append(chBang);
		fSubset.append(XMLUni::fgNotationString);
		fSubset.append(chSpace);

		fSubset.append(notDecl.getName());

		const XMLCh* id = notDecl.getPublicId();
		if (id != 0) {
			fSubset.append(chSpace);
			fSubset.append(XMLUni::fgPubIDString);
			fSubset.append(chSpace);
			fSubset.append(chDoubleQuote);
			fSubset.append(id);
			fSubset.append(chDoubleQuote);
		}
		id = notDecl.getSystemId();
		if (id != 0) {
			fSubset.append(chSpace);
			fSubset.append(XMLUni::fgSysIDString);
			fSubset.append(chSpace);
			fSubset.append(chDoubleQuote);
			fSubset.append(id);
			fSubset.append(chDoubleQuote);
		}
		fSubset.append(chCloseAngle);
	}
}

void NsSAX2Reader::startAttList(const DTDElementDecl &elemDecl)
{
	if (fReadingIntSubset) {
		fSubset.append(chOpenAngle);
		fSubset.append(chBang);
		fSubset.append(XMLUni::fgAttListString);
		fSubset.append(chSpace);
		fSubset.append(elemDecl.getFullName());
	}
}

void NsSAX2Reader::startIntSubset()
{
	fReadingIntSubset = true;
}

void NsSAX2Reader::startExtSubset()
{
	// nothing to do -- post an event when done reading
}

void NsSAX2Reader::TextDecl(const XMLCh* const versionStr,
			     const XMLCh* const encodingStr)
{

}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Handlers for the XMLEntityHandler interface
// ---------------------------------------------------------------------------
void NsSAX2Reader::endInputSource(const InputSource&)
{

}

bool NsSAX2Reader::expandSystemId(const XMLCh* const, XMLBuffer&)
{
	return false;
}

void NsSAX2Reader::resetEntities()
{
	// Nothing to do for this one
}

InputSource* NsSAX2Reader::resolveEntity(const XMLCh* const publicId,
					  const XMLCh* const systemId,
					  const XMLCh* const baseURI)
{
	// Just map to the SAX entity resolver handler
	return 0;
}

InputSource* NsSAX2Reader::resolveEntity(
	XMLResourceIdentifier* resourceIdentifier)
{
	//
	//  Just map it to the SAX entity resolver. If there is not one
	// installed, return a null pointer to cause the default resolution.
	//
	return fResolver.resolveEntity(resourceIdentifier);
}

void NsSAX2Reader::startInputSource(const InputSource&)
{
	// Nothing to do for this one
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Overrides of the XMLErrorReporter interface
// ---------------------------------------------------------------------------
void NsSAX2Reader::resetErrors()
{

}

void NsSAX2Reader::error(const unsigned int code,
			 const XMLCh* const msgDomain,
#if _XERCES_VERSION >= 30000
			 const XER_NS XMLErrorReporter::ErrTypes errType,
#else			 
			 const XMLErrorReporter::ErrTypes errType,
#endif
			   const XMLCh* const errorText,
			   const XMLCh* const systemId,
			   const XMLCh* const publicId,
			   const XercesFileLoc lineNum,
			   const XercesFileLoc colNum)
{
	size_t len = NsUtil::nsStringLen(errorText);
	xmlbyte_t *bufp = (xmlbyte_t *)NsUtil::allocate(500);
	try {
		len = NsUtil::nsToUTF8(&bufp, errorText, len+1, 500);
	} catch (...) {
		NsUtil::deallocate(bufp);
		throw;
	}
	NsUtil::AutoDealloc ad(bufp);
	std::ostringstream s;
	s << "XML Indexer: ";
	if (errType == XMLErrorReporter::ErrType_Warning)
		s << "Warning";
	else if (errType == XMLErrorReporter::ErrType_Fatal)
		s << "Fatal";
	s << " Parse error in document ";
	s << "at line " << lineNum;
	s << ", char " << colNum;
	s << ". Parser message: " << bufp;
	// log warnings as info, and errors as warning.
	// Neither is fatal to the program, and may be
	// what is expected.
	if (errType == XMLErrorReporter::ErrType_Warning) {
		((Manager&)fMgr).log(Log::C_INDEXER, Log::L_INFO, s.str().c_str());
	} else {
		((Manager&)fMgr).log(Log::C_INDEXER, Log::L_WARNING, s.str().c_str());
		NsUtil::nsThrowParseException(s.str().c_str());
	}
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Features and Properties
// ---------------------------------------------------------------------------

void NsSAX2Reader::setFeature(const XMLCh* const name, const_bool value)
{
	NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				 "setFeature is not supported",
				 __FILE__, __LINE__);
}

bool NsSAX2Reader::getFeature(const XMLCh* const name) const
{
    if (XMLString::compareIString(name, XMLUni::fgSAX2CoreNameSpaces) == 0)
        return getDoNamespaces();
    else if (XMLString::compareIString(name, XMLUni::fgXercesSchema) == 0)
        return getDoSchema();
    else if (XMLString::compareIString(name, XMLUni::fgXercesSchemaFullChecking) == 0)
        return fScanner->getValidationSchemaFullChecking();
    else if (XMLString::compareIString(name, XMLUni::fgXercesLoadExternalDTD) == 0)
        return fScanner->getLoadExternalDTD();
    else if (XMLString::compareIString(name, XMLUni::fgXercesContinueAfterFatalError) == 0)
        return !fScanner->getExitOnFirstFatal();
    else if (XMLString::compareIString(name, XMLUni::fgXercesValidationErrorAsFatal) == 0)
        return fScanner->getValidationConstraintFatal();
    else if (XMLString::compareIString(name, XMLUni::fgXercesCacheGrammarFromParse) == 0)
        return fScanner->isCachingGrammarFromParse();
    else if (XMLString::compareIString(name, XMLUni::fgXercesUseCachedGrammarInParse) == 0)
        return fScanner->isUsingCachedGrammarInParse();
    else if (XMLString::compareIString(name, XMLUni::fgXercesCalculateSrcOfs) == 0)
        return fScanner->getCalculateSrcOfs();
    else if (XMLString::compareIString(name, XMLUni::fgXercesStandardUriConformant) == 0)
        return fScanner->getStandardUriConformant();
    else
	    NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				     "Unknown parser feature get called",
				     __FILE__, __LINE__);
    return false;
}

void NsSAX2Reader::setProperty(const XMLCh* const name, void* value)
{
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Cannot setProperty during parse",
					 __FILE__, __LINE__);

	if (XMLString::compareIString(name, XMLUni::fgXercesSchemaExternalSchemaLocation) == 0)
	{
		fScanner->setExternalSchemaLocation((XMLCh*)value);
	}
	else if (XMLString::compareIString(name, XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation) == 0)
	{
		fScanner->setExternalNoNamespaceSchemaLocation((XMLCh*)value);
	}
	else if (XMLString::compareIString(name, XMLUni::fgXercesSecurityManager) == 0)
	{
		fScanner->setSecurityManager((SecurityManager*)value);
	}
    else if (XMLString::equals(name, XMLUni::fgXercesScannerName))
    {
        XMLScanner* tempScanner = XMLScannerResolver::resolveScanner
        (
            (const XMLCh*) value
            , fValidator
            , fGrammarResolver
            , fMemoryManager
        );

        if (tempScanner) {

            tempScanner->setParseSettings(fScanner);
            tempScanner->setURIStringPool(fURIStringPool);
            delete fScanner;
            fScanner = tempScanner;
        }
    }
    else
	    NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				     "Unknown property in setProperty",
				     __FILE__, __LINE__);
}

void* NsSAX2Reader::getProperty(const XMLCh* const name) const
{
	if (XMLString::compareIString(name,
		      XMLUni::fgXercesSchemaExternalSchemaLocation) == 0)
		return (void*)fScanner->getExternalSchemaLocation();
	else if (XMLString::compareIString(name,
		      XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation) == 0)
		return (void*)fScanner->getExternalNoNamespaceSchemaLocation();
	else if (XMLString::compareIString(name, XMLUni::fgXercesSecurityManager) == 0)
		return (void*)fScanner->getSecurityManager();
	else if (XMLString::equals(name, XMLUni::fgXercesScannerName))
		return (void*)fScanner->getName();
	else
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown property in getProperty",
					 __FILE__, __LINE__);

	return 0;
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Private getters and setters for conveniences
// ---------------------------------------------------------------------------

void NsSAX2Reader::setValidationScheme(const ValSchemes newScheme)
{
	if (newScheme == Val_Never)
		fScanner->setValidationScheme(XMLScanner::Val_Never);
	else if (newScheme == Val_Always)
		fScanner->setValidationScheme(XMLScanner::Val_Always);
	else
		fScanner->setValidationScheme(XMLScanner::Val_Auto);
}

void NsSAX2Reader::setDoNamespaces(const_bool newState)
{
	fScanner->setDoNamespaces(newState);
}

bool NsSAX2Reader::getDoNamespaces() const
{
	return fScanner->getDoNamespaces();
}

void NsSAX2Reader::setDoSchema(const_bool newState)
{
	fScanner->setDoSchema(newState);
}

bool NsSAX2Reader::getDoSchema() const
{
	return fScanner->getDoSchema();
}

// ---------------------------------------------------------------------------
//  NsSAX2Reader: Grammar preparsing
// ---------------------------------------------------------------------------
#if _XERCES_VERSION >= 30000
Grammar* NsSAX2Reader::loadGrammar(const char* const systemId,
				   const Grammar::GrammarType grammarType,
				   const_bool toCache)
#else
Grammar* NsSAX2Reader::loadGrammar(const char* const systemId,
				   const short grammarType,
				   const_bool toCache)
#endif
{
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Cannot loadGrammar during parse",
					 __FILE__, __LINE__);

	Grammar* grammar = 0;
	try {
		fParseInProgress = true;
		grammar = fScanner->loadGrammar(systemId,
						grammarType, toCache);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in loadGrammar",
					 __FILE__, __LINE__);
	}
	catch(...) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception in loadGrammar",
					 __FILE__, __LINE__);
	}

	return grammar;
}

#if _XERCES_VERSION >= 30000
Grammar* NsSAX2Reader::loadGrammar(const XMLCh* const systemId,
				   const Grammar::GrammarType grammarType,
				   const_bool toCache)
#else
Grammar* NsSAX2Reader::loadGrammar(const XMLCh* const systemId,
				   const short grammarType,
				   const_bool toCache)
#endif
{
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Cannot loadGrammar during parse",
					 __FILE__, __LINE__);

	Grammar* grammar = 0;
	try {
		fParseInProgress = true;
		grammar = fScanner->loadGrammar(systemId,
						grammarType, toCache);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in loadGrammar",
					 __FILE__, __LINE__);
	}
	catch(...) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception in loadGrammar",
					 __FILE__, __LINE__);
	}

	return grammar;
}

#if _XERCES_VERSION >= 30000
Grammar* NsSAX2Reader::loadGrammar(const InputSource& source,
				   const Grammar::GrammarType grammarType,
				   const_bool toCache)
#else
Grammar* NsSAX2Reader::loadGrammar(const InputSource& source,
				   const short grammarType,
				   const_bool toCache)
#endif
{
	// Avoid multiple entrance
	if (fParseInProgress)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Cannot loadGrammar during parse",
					 __FILE__, __LINE__);

	Grammar* grammar = 0;
	try {
		fParseInProgress = true;
		grammar = fScanner->loadGrammar(source, grammarType, toCache);
		fParseInProgress = false;
	}
	catch(const OutOfMemoryException&) {
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "Out of memory in loadGrammar",
					 __FILE__, __LINE__);
	}
	catch(...) {
		fParseInProgress = false;
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Unknown exception in loadGrammar",
					 __FILE__, __LINE__);
	}

	return grammar;
}

void NsSAX2Reader::resetCachedGrammarPool()
{
	fGrammarResolver->resetCachedGrammar();
}

Grammar* NsSAX2Reader::getGrammar(const XMLCh* const nameSpaceKey)
{
	return fGrammarResolver->getGrammar(nameSpaceKey);
}

