//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlIndexSpecification.hpp"
#include "IndexSpecification.hpp"

using namespace DbXml;

XmlIndexSpecification::XmlIndexSpecification()
	: is_(new IndexSpecification),
	  isi_(new IndexSpecificationIterator(*is_))
{
	is_->acquire();
}

XmlIndexSpecification::~XmlIndexSpecification()
{
	is_->release();
	delete isi_;
}

XmlIndexSpecification::XmlIndexSpecification(const XmlIndexSpecification &o)
	: is_(o.is_),
	  isi_(new IndexSpecificationIterator(*is_))
{
	if (is_ != 0)
		is_->acquire();
}

XmlIndexSpecification &XmlIndexSpecification::operator=(const XmlIndexSpecification &o)
{
	if (this != &o && is_ != o.is_) {
		if (is_ != 0)
			is_->release();
		is_ = o.is_;
		if (is_ != 0)
			is_->acquire();
		delete isi_;
		isi_ = new IndexSpecificationIterator(*is_);
	}
	return *this;
}

void XmlIndexSpecification::addIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax)
{
	is_->addIndex(uri, name, Index(type, syntax));
}

void XmlIndexSpecification::addIndex(const std::string &uri, const std::string &name, const std::string &index)
{
	is_->addIndex(uri, name, index);
}

void XmlIndexSpecification::deleteIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax)
{
	is_->deleteIndex(uri, name, Index(type, syntax));
}

void XmlIndexSpecification::deleteIndex(const std::string &uri, const std::string &name, const std::string &index)
{
	is_->deleteIndex(uri, name, index);
}

void XmlIndexSpecification::replaceIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax)
{
	std::string oldindex;
	if(is_->find(uri, name, oldindex))
		is_->deleteIndex(uri, name, oldindex);
	is_->addIndex(uri, name, Index(type, syntax));
}

void XmlIndexSpecification::replaceIndex(const std::string &uri, const std::string &name, const std::string &index)
{
	std::string oldindex;
	if(is_->find(uri, name, oldindex))
		is_->deleteIndex(uri, name, oldindex);
	is_->addIndex(uri, name, index);
}

bool XmlIndexSpecification::getAutoIndexing() const
{
	return is_->getAutoIndexing();
}

void XmlIndexSpecification::setAutoIndexing(bool value)
{
	is_->setAutoIndexing(value);
}

bool XmlIndexSpecification::find(const std::string &uri, const std::string &name, std::string &index) const
{
	return is_->find(uri, name, index);
}

void XmlIndexSpecification::addDefaultIndex(Type type, XmlValue::Type syntax)
{
	is_->addDefaultIndex(Index(type, syntax));
}

void XmlIndexSpecification::addDefaultIndex(const std::string &index)
{
	is_->addDefaultIndex(index);
}

void XmlIndexSpecification::deleteDefaultIndex(Type type, XmlValue::Type syntax)
{
	is_->deleteDefaultIndex(Index(type, syntax));
}

void XmlIndexSpecification::deleteDefaultIndex(const std::string &index)
{
	is_->deleteDefaultIndex(index);
}

void XmlIndexSpecification::replaceDefaultIndex(Type type, XmlValue::Type syntax)
{
	std::string oldindex;
	is_->getDefaultIndex()->clear();
	is_->addDefaultIndex(Index(type, syntax));
}

void XmlIndexSpecification::replaceDefaultIndex(const std::string &index)
{
	std::string oldindex;
	is_->getDefaultIndex()->clear();
	is_->addDefaultIndex(index);
}

std::string XmlIndexSpecification::getDefaultIndex() const
{
	return is_->getDefaultIndex()->asString();
}

bool XmlIndexSpecification::next(std::string &uri, std::string &name, Type &type, XmlValue::Type &syntax)
{
	return isi_->next(uri, name, type, syntax);
}

bool XmlIndexSpecification::next(std::string &uri, std::string &name, std::string &index)
{
	return isi_->next(uri, name, index);
}

void XmlIndexSpecification::reset()
{
	isi_->reset();
}

static XmlValue::Type valueTypeFromSyntaxType(Syntax::Type stype)
{
	switch(stype) {
	case Syntax::STRING: return XmlValue::STRING;
	case Syntax::BASE_64_BINARY: return XmlValue::BASE_64_BINARY;
	case Syntax::BOOLEAN: return XmlValue::BOOLEAN;
	case Syntax::DATE: return XmlValue::DATE;
	case Syntax::DATE_TIME: return XmlValue::DATE_TIME;
	case Syntax::DAY: return XmlValue::G_DAY;
	case Syntax::DECIMAL: return XmlValue::DECIMAL;
	case Syntax::DOUBLE: return XmlValue::DOUBLE;
	case Syntax::DURATION: return XmlValue::DURATION;
	case Syntax::FLOAT: return XmlValue::FLOAT;
	case Syntax::HEX_BINARY: return XmlValue::HEX_BINARY;
	case Syntax::MONTH: return XmlValue::G_MONTH;
	case Syntax::MONTH_DAY: return XmlValue::G_MONTH_DAY;
	case Syntax::TIME: return XmlValue::TIME;
	case Syntax::YEAR: return XmlValue::G_YEAR;
	case Syntax::YEAR_MONTH: return XmlValue::G_YEAR_MONTH;
	case Syntax::NONE: default: return XmlValue::NONE;

	case Syntax::DEPRECATED_ANY_URI:
	case Syntax::DEPRECATED_NOTATION:
	case Syntax::DEPRECATED_QNAME:
		DBXML_ASSERT(false);
		return XmlValue::NONE;
	}
}

// return the XmlValue::Type of the string index
// static
XmlValue::Type XmlIndexSpecification::getValueType(
	const std::string &index)
{
	Index idx(index);
	return valueTypeFromSyntaxType(idx.getSyntax());
}
