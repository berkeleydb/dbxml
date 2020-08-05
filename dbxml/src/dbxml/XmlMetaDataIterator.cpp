//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlMetaDataIterator.hpp>
#include "Document.hpp"

using namespace DbXml;

static const char *className = "XmlMetaDataIterator";
#define CHECK_POINTER checkNullPointer(i_,className)

XmlMetaDataIterator::XmlMetaDataIterator()
	: i_(0)
{
}

XmlMetaDataIterator::XmlMetaDataIterator(MetaDataIterator *i)
	: i_(i)
{
	i_->acquire();
}

XmlMetaDataIterator::~XmlMetaDataIterator()
{
	i_->release();
}

XmlMetaDataIterator::XmlMetaDataIterator(const XmlMetaDataIterator &o)
	: i_(o.i_)
{
	i_->acquire();
}

XmlMetaDataIterator &XmlMetaDataIterator::operator=(const XmlMetaDataIterator &o)
{
	if (this != &o && i_ != o.i_) {
		if(i_) i_->release();
		i_ = o.i_;
		if(i_) i_->acquire();
	}
	return *this;
}

bool XmlMetaDataIterator::next(std::string &uri, std::string &name, XmlValue &value)
{
	CHECK_POINTER;
	return i_->next(uri, name, value);
}

void XmlMetaDataIterator::reset()
{
	CHECK_POINTER;
	i_->reset();
}
