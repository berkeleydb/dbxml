//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PULLEVENTINPUTSTREAM_HPP
#define	__PULLEVENTINPUTSTREAM_HPP

#include <dbxml/XmlInputStream.hpp>
#include "NsEvent.hpp"
#include "NsWriter.hpp"

namespace DbXml
{

/**
 * Takes an NsPullEventSource, and turns it into an XmlInputStream,
 * using NsWriter to write out the XML.
 */
class PullEventInputStream : public XmlInputStream, private NsStream
{
public:
	PullEventInputStream(NsPullEventSource *events);
	virtual ~PullEventInputStream();

	virtual unsigned int curPos() const;
	virtual unsigned int readBytes(char *toFill, 
		const unsigned int maxToRead);

private:
	virtual void write(const xmlbyte_t *data);
	virtual void write(const xmlbyte_t *data, size_t len);

private:
	NsPullEventSource *events_;
	NsWriter writer_;
	size_t cursorPos_;

	Buffer buffer_;
	size_t bufferOffset_;

	char *toFill_;
	size_t bytesLeft_;
};

}

#endif
