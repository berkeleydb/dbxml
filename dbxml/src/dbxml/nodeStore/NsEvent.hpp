//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSEVENT_HPP
#define __DBXMLNSEVENT_HPP

//
// This file declares some core interfaces used for input and output
// events for BDB XML.
//
// They handle translation of UTF-16 to UTF-8, as well as
// encapsulating the passing of attributes.  The *16 classes exist
// purely to handle the fact that the default parser, Xerces,
// transcodes to UTF-16 and generates its events in that encoding.
//
// BDB XML internals use UTF-8.
//
///

#include "NsTypes.hpp"
#include "NsNode.hpp"
#include "../ScopedDbt.hpp"

namespace DbXml {

class NsDocument;
class EventWriter;
	
//
// NsEventAttrList16
//
// Passes Xerces-C attributes during parse
//	
class NsEventAttrList16 {
public:
	virtual ~NsEventAttrList16();
	virtual bool isEmpty(void) const = 0;
	virtual int numAttributes(void) const = 0;
	virtual const xmlch_t *prefix(int index) const = 0;
	virtual const xmlch_t *localName(int index) const =0;
	virtual const xmlch_t *value(int index) const = 0;
	virtual const xmlch_t *uri(int index) const = 0;
#ifdef NS_USE_SCHEMATYPES
	virtual bool hasTypeInfo(int index) const = 0;
	virtual const xmlch_t *typeName(int index) const = 0;
	virtual const xmlch_t *typeUri(int index) const = 0;
#endif
	virtual bool needsEscape(int index) const = 0;
	virtual bool isSpecified(int index) const = 0;
};

//
// NsEventHandler16
//
// Interface implemented by receivers of parse events.
//	
class NsEventHandler16 {
public:
	virtual ~NsEventHandler16() {}
	virtual void startDocument(const xmlch_t *sniffedEncoding) {}
	virtual void xmlDecl(const xmlch_t *xmlDecl,
			     const xmlch_t *encodingStr,
			     const xmlch_t *standaloneStr) {}
	virtual void endDocument() {}
	virtual void startElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri,
				  NsEventAttrList16 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty) = 0;
	virtual void endElement(const xmlch_t *localName,
				const xmlch_t *prefix,
				const xmlch_t *uri) = 0;
	virtual void characters(const xmlch_t *characters,
				size_t len, bool isCDATA,
				bool needsEscape) = 0;
	virtual void ignorableWhitespace(const xmlch_t *characters,
					 size_t len, bool isCDATA) {}
	virtual void comment(const xmlch_t *comment, size_t len) = 0;
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data) = 0;
#ifdef NS_USE_SCHEMATYPES	
	virtual void setTypeInfo(const xmlch_t *typeName,
				 const xmlch_t *typeUri) {}
#endif
	// these next several have default, no-op implementations
	virtual void docTypeDecl(const xmlch_t *data, size_t len);
	virtual void startEntity(const xmlch_t *name, size_t len);
	virtual void endEntity(const xmlch_t *name, size_t len);
};

//
// NsEventTranslator
//
// Translates from NsEventHandler16 events into EventWriter
// events.
//
class NsEventTranslator : public NsEventHandler16 {
public:
	virtual ~NsEventTranslator() {}
	virtual EventWriter *getEventWriter(void) const = 0;
	virtual void setEventWriter(EventWriter *writer) = 0;
};

//
// NsPushEventSource16
//
// Interface that can be implemented by a parser or other
// event generator for NsEventHandler16.
//
class NsPushEventSource16 {
public:
	virtual ~NsPushEventSource16() {}
	virtual void start() = 0;
	virtual NsEventHandler16 *getNsEventHandler16(void) const = 0;
	virtual void setNsEventHandler16(NsEventHandler16 *handler) = 0;
};

//
// NsPushEventSource
//
// Interface that can be implemented by a parser or other
// event generator for EventWriter
//
class NsPushEventSource {
public:
	virtual ~NsPushEventSource() {}
	virtual void start() = 0;
	virtual EventWriter *getEventWriter(void) const = 0;
	virtual void setEventWriter(EventWriter *writer) = 0;
};

	
//
// NsPullEventSource
//
// Interface that can be implemented by a parser or other
// pull type event generator for EventWriter.
//
class NsPullEventSource {
public:
	virtual ~NsPullEventSource() {}
	virtual bool nextEvent(EventWriter *writer) = 0;
};

	
//
// NsPushEventSourceTranslator
//
// Converts an NsPushEventSource16 into an NsPushEventSource using an
// NsEventTranslator. Adopts and deletes both of it's
// constructor arguments.
//
class NsPushEventSourceTranslator : public NsPushEventSource
{
public:
	NsPushEventSourceTranslator(NsPushEventSource16 *source,
				    NsEventTranslator *translator);
	virtual ~NsPushEventSourceTranslator();

	virtual void start();
	virtual EventWriter *getEventWriter(void) const;
	virtual void setEventWriter(EventWriter *writer);
private:
	NsPushEventSource16 *source_;
	NsEventTranslator *translator_;
};

//
// NsEventAttrList
//
// Abstraction passed to EventWriter::writeStartElementWithAttrs
//
class NsEventAttrList {
public:
	virtual ~NsEventAttrList() {}
	virtual bool isEmpty(void) const = 0;
	virtual int numAttributes(void) const = 0;
	virtual const xmlbyte_t *prefix(int index) const = 0;
	virtual const xmlbyte_t *localName(int index) const =0;
	virtual const xmlbyte_t *value(int index) const = 0;
	virtual const xmlbyte_t *uri(int index) const = 0;
#ifdef NS_USE_SCHEMATYPES	
	virtual bool hasTypeInfo(int index) const = 0;
	virtual const xmlbyte_t *typeName(int index) const = 0;
	virtual const xmlbyte_t *typeUri(int index) const = 0;
#endif	
	virtual bool needsEscape(int index) const = 0;
	virtual bool isSpecified(int index) const = 0;
};
	
//
// NsEventNodeAttrList
//
// Instance of NsEventAttrList that is constructed using
// a NsNode.  NOTE: the NsNode MUST have utf-8 strings
// in it, which means it's either the result of a transcoded
// parse (See NsXercesTranscoder) or read from a database (see
// NsEventReader).
//
//
class NsEventNodeAttrList : public NsEventAttrList {
public:
	NsEventNodeAttrList(nsAttrList_t *list,
			    NsDocument &doc, bool isUTF16 = false);
	virtual ~NsEventNodeAttrList();
	
	virtual bool isEmpty(void) const { return (_list == 0); }
	virtual int numAttributes(void) const;
	virtual const xmlbyte_t *prefix(int index) const;
	virtual const xmlbyte_t *localName(int index) const;
	virtual const xmlbyte_t *value(int index) const;
	virtual const xmlbyte_t *uri(int index) const;
#ifdef NS_USE_SCHEMATYPES	
	virtual bool hasTypeInfo(int index) const {
		return (_list->al_attrs[index].a_type != NS_NOTYPE);
	}
	virtual const xmlbyte_t *typeName(int index) const;
	virtual const xmlbyte_t *typeUri(int index) const;
#endif
	virtual bool needsEscape(int index) const;
	virtual bool isSpecified(int index) const;
private:
	nsAttrList_t *_list;
	NsDocument &_doc;
	bool _isUTF16;
	mutable xmlbyte_t **_localnames;
	mutable xmlbyte_t **_values;
};

}

#endif
