//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __EVENTREADERTOWRITER_HPP
#define __EVENTREADERTOWRITER_HPP

#include "NsEvent.hpp"
#include "EventWriter.hpp"
#include "../ReferenceCounted.hpp"
/*
 * EventReaderToWriter
 *
 * Internal implementation of XmlEventReaderToWriter that
 * knows about internal interfaces
 */
namespace DbXml
{

class XmlEventReader;
class XmlEventWriter;
class EventReader;
class EventWriter;
	
class EventReaderToWriter : public NsPullEventSource, public NsPushEventSource, public ReferenceCounted
{
public:
	EventReaderToWriter(XmlEventReader &reader,
			    XmlEventWriter &writer,
			    bool ownsReader, bool ownsWriter);
	EventReaderToWriter(EventReader &reader,
			    EventWriter &writer,
			    bool ownsReader, bool ownsWriter);
	EventReaderToWriter(XmlEventReader &reader,
			    EventWriter &writer,
			    bool ownsReader, bool ownsWriter);
	EventReaderToWriter(EventReader &reader,
			    XmlEventWriter &writer,
			    bool ownsReader, bool ownsWriter);
	EventReaderToWriter(XmlEventReader &reader,
			    bool ownsReader,
			    bool isInternal);
	~EventReaderToWriter();
	XmlEventWriter *getWriter() const {
		return writer_;
	}
	void reset();

	// NsPushEventSource
	virtual void start();
	virtual EventWriter *getEventWriter() const {
		DBXML_ASSERT(isInternalWriter_);
		return (EventWriter *)writer_;
	}
	virtual void setEventWriter(EventWriter *writer) {
		writer_ = writer;
		isInternalWriter_ = true;
		DBXML_ASSERT(!ownsWriter_); // must not be owned 
	}
	
	// NsPullEventSource
	virtual bool nextEvent(EventWriter *writer);
private:
	void doEvent(XmlEventWriter *writer, bool isInternal);
	void doStartElement(EventWriter *writer);
private:
	XmlEventReader &reader_;
	XmlEventWriter *writer_;
	bool ownsReader_;
	bool ownsWriter_;
	bool isInternalReader_;
	bool isInternalWriter_;
};
}
	
#endif

