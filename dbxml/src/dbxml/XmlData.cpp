//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "dbxml/XmlData.hpp"
#include "Buffer.hpp"

using namespace DbXml;

XmlData::XmlData()
  : buffer_(new Buffer())
{
}

XmlData::XmlData(const XmlData &o)
  : buffer_(new Buffer(o.get_data(), o.get_size(), false))
{
}

XmlData::XmlData(void *data, size_t size)
  : buffer_(new Buffer(data, size, true))
{
}

XmlData &XmlData::operator=(const XmlData &o)
{
  if(&o != this) {
	delete buffer_;
    buffer_ = new Buffer(o.get_data(), o.get_size(), false);
  }
  return *this;
}

XmlData::~XmlData()
{
  delete buffer_;
}

void XmlData::set(const void *data, size_t size)
{
	buffer_->reset();
	buffer_->write(data, size);
	buffer_->resetCursorPosition();
}

void XmlData::append(const void *data, size_t size)
{
	buffer_->append(data, size);
}

void *XmlData::get_data() const
{
  return buffer_->getBuffer(0);
}

size_t XmlData::get_size() const
{
  return buffer_->getOccupancy();
}

void XmlData::set_size(size_t size)
{
	buffer_->setOccupancy(size);
}

void XmlData::reserve(size_t size)
{
	size_t offset(0);
	size_t occ = buffer_->getOccupancy();
	buffer_->reserve(offset, size);
	buffer_->resetCursorPosition();
	buffer_->setOccupancy(occ);
}

size_t XmlData::getReservedSize() const
{
	return buffer_->getSize();
}

void XmlData::adoptBuffer(XmlData &src)
{
	delete buffer_;
	Buffer **otherBuffer = src.getBuffer();
	buffer_ = *otherBuffer;
	*otherBuffer = new Buffer();
}

