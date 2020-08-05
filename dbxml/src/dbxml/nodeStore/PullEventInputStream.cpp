//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <dbxml/XmlException.hpp>

#include "PullEventInputStream.hpp"
#include "NsUtil.hpp"

using namespace DbXml;

PullEventInputStream::PullEventInputStream(NsPullEventSource *events)
	: events_(events),
	  cursorPos_(0),
	  bufferOffset_(0),
	  toFill_(0),
	  bytesLeft_(0)
{
	writer_.setStream(this);
}

PullEventInputStream::~PullEventInputStream()
{
	delete events_;
}

unsigned int PullEventInputStream::curPos() const
{
	return (unsigned int)cursorPos_;
}

unsigned int PullEventInputStream::readBytes(char *toFill, const unsigned int maxToRead)
{
	size_t originalCursorPos = cursorPos_;

	toFill_ = toFill;
	bytesLeft_ = maxToRead;

	if(buffer_.getOccupancy() > 0) {
		// There's data left in the buffer

		size_t bufferBytesLeft = buffer_.getOccupancy() - bufferOffset_;
		if(bufferBytesLeft > maxToRead) {
			// There's more data in the buffer than we need.
			// Read some of it, and increase the bufferOffset_.
			buffer_.read(bufferOffset_, toFill, maxToRead);

			toFill_ += maxToRead;
			bytesLeft_ -= maxToRead;

			bufferOffset_ += maxToRead;

			cursorPos_ += maxToRead;
		}
		else {
			// There's less data in the buffer than we need.
			// Read it all, and reset the buffer and bufferOffset_.
			buffer_.read(bufferOffset_, toFill, bufferBytesLeft);

			toFill_ += bufferBytesLeft;
			bytesLeft_ -= bufferBytesLeft;

			cursorPos_ += bufferBytesLeft;

			buffer_.reset();
			bufferOffset_ = 0;
		}
	}

	while(bytesLeft_ > 0 &&	events_->nextEvent(&writer_)) {}

	return (unsigned int)(cursorPos_ - originalCursorPos);
}

void PullEventInputStream::write(const xmlbyte_t *data)
{
	write(data, NsUtil::nsStringLen(data));
}

void PullEventInputStream::write(const xmlbyte_t *data, size_t len)
{
	if(len <= bytesLeft_) {
		// All the data fits in the destination,
		// so write it all there
		::memcpy(toFill_, data, len);
		toFill_ += len;
		bytesLeft_ -= len;

		cursorPos_ += len;
	}
	else {
		// The data is too big for the destination,
		// so write as much as will fit, and put
		// the rest in the buffer.
		::memcpy(toFill_, data, bytesLeft_);
		data += bytesLeft_;
		len -= bytesLeft_;

		cursorPos_ += bytesLeft_;

		toFill_ += bytesLeft_;
		bytesLeft_ = 0;

		buffer_.write(data, len);
	}
}
