//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "EventReaderToWriter.hpp"
#include <dbxml/XmlEventReader.hpp>
#include <dbxml/XmlEventWriter.hpp>
#include "EventReader.hpp"
#include "EventWriter.hpp"
#include "NsEvent.hpp"

using namespace DbXml;

namespace DbXml {

class EventReaderAttrList : public NsEventAttrList {
public:
	EventReaderAttrList(const XmlEventReader &reader) :
	reader_(reader) {}
	virtual ~EventReaderAttrList() {}
	virtual bool isEmpty(void) const {
		return (reader_.getAttributeCount() == 0);
	}
	virtual int numAttributes(void) const {
		return reader_.getAttributeCount();
	}
	virtual const xmlbyte_t *prefix(int index) const {
		return reader_.getAttributePrefix(index);
	}
	virtual const xmlbyte_t *localName(int index) const {
		return reader_.getAttributeLocalName(index);
	}
	virtual const xmlbyte_t *value(int index) const {
		return reader_.getAttributeValue(index);
	}
	virtual const xmlbyte_t *uri(int index) const {
		return reader_.getAttributeNamespaceURI(index);
	}
#ifdef NS_USE_SCHEMATYPES	
	virtual bool hasTypeInfo(int index) const {
		return false;
		// TBD
		//return reader_.getAttributeHasTypeInfo(index);
	}
	virtual const xmlbyte_t *typeName(int index) const {
		return 0;
		// TBD
		//return reader_.getAttributeTypename(index);
	}
	virtual const xmlbyte_t *typeUri(int index) const {
		return 0;
		// TBD
		//return reader_.getAttributeTypeURI(index);
	}
#endif
	virtual bool needsEscape(int index) const {
		return reader_.needsEntityEscape(index);
	}
	virtual bool isSpecified(int index) const {
		return reader_.isAttributeSpecified(index);
	}
private:
	const XmlEventReader &reader_;
};
}

EventReaderToWriter::EventReaderToWriter(XmlEventReader &reader,
					 XmlEventWriter &writer,
					 bool ownsReader,
					 bool ownsWriter)
	: reader_(reader), writer_(&writer), ownsReader_(ownsReader),
	  ownsWriter_(ownsWriter), isInternalReader_(false),
		      isInternalWriter_(false)
{
}

EventReaderToWriter::EventReaderToWriter(EventReader &reader,
					 EventWriter &writer,
					 bool ownsReader,
					 bool ownsWriter)
	: reader_(reader), writer_(&writer), ownsReader_(ownsReader),
	  ownsWriter_(ownsWriter), isInternalReader_(true),
	  isInternalWriter_(true)
{
}

EventReaderToWriter::EventReaderToWriter(XmlEventReader &reader,
					 EventWriter &writer,
					 bool ownsReader,
					 bool ownsWriter)
	: reader_(reader), writer_(&writer), ownsReader_(ownsReader),
	  ownsWriter_(ownsWriter), isInternalReader_(false),
	  isInternalWriter_(true)
{
}

EventReaderToWriter::EventReaderToWriter(EventReader &reader,
					 XmlEventWriter &writer,
					 bool ownsReader,
					 bool ownsWriter)
	: reader_(reader), writer_(&writer), ownsReader_(ownsReader),
	  ownsWriter_(ownsWriter), isInternalReader_(true),
	  isInternalWriter_(false)
{
}

EventReaderToWriter::EventReaderToWriter(XmlEventReader &reader,
					 bool ownsReader,
					 bool isInternal)
	: reader_(reader), writer_(0), ownsReader_(ownsReader),
	  ownsWriter_(false), isInternalReader_(isInternal),
	  isInternalWriter_(false)
{
}

EventReaderToWriter::~EventReaderToWriter()
{
	if (ownsReader_)
		reader_.close();
	if (ownsWriter_)
		writer_->close();
}

void EventReaderToWriter::reset()
{
	if (ownsReader_) {
		reader_.close();
		ownsReader_ = false;
	}
	if (ownsWriter_) {
		writer_->close();
		ownsWriter_ = false;
	}
}

void EventReaderToWriter::start()
{
	if (!writer_)
		throw XmlException(XmlException::EVENT_ERROR,
				   "EventReaderToWriter::start() called without writer");
	while (reader_.hasNext())
		doEvent(writer_, isInternalWriter_);
}

bool EventReaderToWriter::nextEvent(EventWriter *writer)
{
	if (reader_.hasNext()) {
		doEvent(writer, true);
		return true;
	}
	return false;
}

void EventReaderToWriter::doEvent(XmlEventWriter *writer, bool isInternal)
{
	XmlEventReader::XmlEventType etype = reader_.next();
	switch (etype) {
	case XmlEventReader::StartElement:
	{
		if (isInternal) {
			doStartElement((EventWriter*)writer);
			break;
		}
		int nattrs = reader_.getAttributeCount();
		writer->writeStartElement(reader_.getLocalName(),
					  reader_.getPrefix(),
					  reader_.getNamespaceURI(),
					  nattrs,
					  reader_.isEmptyElement());
		for (int i = 0; i < nattrs; i++) {
			writer->writeAttribute(reader_.getAttributeLocalName(i),
					       reader_.getAttributePrefix(i),
					       reader_.getAttributeNamespaceURI(i),
					       reader_.getAttributeValue(i),
					       reader_.isAttributeSpecified(i));
		}
		break;
	}
	case XmlEventReader::EndElement:
	{
		if (isInternal && isInternalReader_) {
			IndexNodeInfo *info = ((EventReader&)reader_).getIndexNodeInfo();
			((EventWriter*)writer)->writeEndElementWithNode(
				reader_.getLocalName(),
				reader_.getPrefix(),
				reader_.getNamespaceURI(),
				info);
		} else {
			writer->writeEndElement(reader_.getLocalName(),
						reader_.getPrefix(),
						reader_.getNamespaceURI());
		}
		break;
	}
	case XmlEventReader::CDATA:
	case XmlEventReader::Characters:
	case XmlEventReader::Comment:
	case XmlEventReader::Whitespace:
	{
		size_t len;
		const unsigned char *chars = reader_.getValue(len);
		if (isInternalReader_ && isInternal &&
		    etype == XmlEventReader::Characters) {
			bool needsEscape =
				((EventReader&)reader_).needsEntityEscape(0);
			((EventWriter*)writer)->writeTextWithEscape(
				etype, chars, len, needsEscape);
		} else
			writer->writeText(etype, chars, len);
		break;
	}
	case XmlEventReader::StartDocument:
	{
		const unsigned char *sa = 0;
		if (reader_.standaloneSet()) {
			if (reader_.isStandalone())
				sa = (const unsigned char *)"yes";
			else
				sa = (const unsigned char *)"no";
		}
		writer->writeStartDocument(reader_.getVersion(),
					   reader_.getEncoding(),
					   sa);
		break;
	}
	case XmlEventReader::EndDocument:
	{
		writer->writeEndDocument();
		break;
	}
	case XmlEventReader::DTD:
	{
		size_t len;
		const unsigned char *chars = reader_.getValue(len);
		writer->writeDTD(chars, len);
		break;
	}
	case XmlEventReader::ProcessingInstruction:
	{
		size_t len;
		const unsigned char *target = reader_.getLocalName();
		const unsigned char *data = reader_.getValue(len);
		writer->writeProcessingInstruction(target, data);
		break;
	}
	case XmlEventReader::StartEntityReference:
	{
		size_t len;
		const unsigned char *chars = reader_.getValue(len);
		writer->writeStartEntity(chars, reader_.getExpandEntities());
		break;
	}
	case XmlEventReader::EndEntityReference:
	{
		size_t len;
		const unsigned char *chars = reader_.getValue(len);
		writer->writeEndEntity(chars);
		break;
	}
	default:
		throw XmlException(XmlException::EVENT_ERROR,
				   "EventReaderToWriter::doEvent() unknown event type");
	}
}

void
EventReaderToWriter::doStartElement(EventWriter *writer)
{
	EventReaderAttrList  alist(reader_);
	IndexNodeInfo *info = (isInternalReader_ ?
			      ((EventReader&)reader_).getIndexNodeInfo() : 0);
	const xmlbyte_t *prefix = reader_.getPrefix();
	const xmlbyte_t *uri = reader_.getNamespaceURI();
	writer->writeStartElementWithAttrs(reader_.getLocalName(),
					   prefix,
					   uri,
					   alist.numAttributes(),
					   &alist,
					   info,
					   reader_.isEmptyElement());
}



