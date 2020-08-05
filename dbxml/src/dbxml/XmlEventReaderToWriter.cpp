//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlEventReaderToWriter.hpp>
#include <dbxml/XmlEventReader.hpp>
#include <dbxml/XmlEventWriter.hpp>
#include "nodeStore/EventReaderToWriter.hpp"

using namespace DbXml;

XmlEventReaderToWriter::XmlEventReaderToWriter()
	: impl_(0)
{
}

XmlEventReaderToWriter::XmlEventReaderToWriter(const XmlEventReaderToWriter &o)
	: impl_(o.impl_)
{
	if (impl_ != 0) {
		impl_->acquire();
	}
}

XmlEventReaderToWriter &
XmlEventReaderToWriter::operator=(const XmlEventReaderToWriter &o)
{
	if (this != &o && impl_ != o.impl_) {
		if (impl_ != 0)
			impl_->release();
		impl_ = o.impl_;
		if (impl_ != 0)
			impl_->acquire();
	}
	return *this;
}

XmlEventReaderToWriter::XmlEventReaderToWriter(XmlEventReader &reader,
					       XmlEventWriter &writer,
					       bool ownsReader)
{
	impl_ = new EventReaderToWriter(reader, writer, ownsReader,
					true /* owns writer */);
	impl_->acquire();
}

XmlEventReaderToWriter::XmlEventReaderToWriter(XmlEventReader &reader,
					       XmlEventWriter &writer,
					       bool ownsReader,
					       bool ownsWriter)
{
	impl_ = new EventReaderToWriter(reader, writer, ownsReader,
					ownsWriter);
	impl_->acquire();
}

XmlEventReaderToWriter::~XmlEventReaderToWriter()
{
	if (impl_) {
		impl_->release();
	}
}

void XmlEventReaderToWriter::start()
{
	DBXML_ASSERT(impl_);
	try {
		impl_->start();
	} catch (...) {
		try {
			// this is a one-use object
			impl_->reset();
		} catch (...) {}
		throw;
	}
	// this is a one-use object
	impl_->reset();
}

