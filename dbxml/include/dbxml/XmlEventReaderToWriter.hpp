//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLEVENTREADERTOWRITER_HPP
#define __XMLEVENTREADERTOWRITER_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"

/*
 * XmlEventReaderToWriter
 *
 * Write to XmlEventWriter from XmlEventReader events
 */
namespace DbXml
{

class XmlEventReader;
class XmlEventWriter;
class EventReaderToWriter;
	
class DBXML_EXPORT XmlEventReaderToWriter
{
public:
	XmlEventReaderToWriter();
	XmlEventReaderToWriter(const XmlEventReaderToWriter&);
	XmlEventReaderToWriter &operator=(const XmlEventReaderToWriter &);

	XmlEventReaderToWriter(XmlEventReader &reader,
			       XmlEventWriter &writer,
			       bool ownsReader = true);
	XmlEventReaderToWriter(XmlEventReader &reader,
			       XmlEventWriter &writer,
			       bool ownsReader, bool ownsWriter);
	~XmlEventReaderToWriter();
	void start();

	bool isNull() const { return impl_ == 0; }
private:
	EventReaderToWriter *impl_;
};
}
	
#endif

