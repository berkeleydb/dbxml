//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlDocument.hpp>
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlQueryContext.hpp>
#include <dbxml/XmlQueryExpression.hpp>
#include <dbxml/XmlMetaDataIterator.hpp>
#include <dbxml/XmlData.hpp>
#include "Document.hpp"
#include "QueryContext.hpp"
#include "UTF8.hpp"
#include "QueryExpression.hpp"
#include "Value.hpp"
#include "Results.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;

static const char *className = "XmlDocument";
#define CHECK_POINTER checkNullPointer(document_,className)

XmlDocument::XmlDocument()
	: document_(0)
{
}

XmlDocument::XmlDocument(Document *document)
	: document_(document)
{
	if (document_ != 0)
		document_->acquire();
}

XmlDocument::~XmlDocument()
{
	if (document_ != 0)
		document_->release();
}

XmlDocument::XmlDocument(const XmlDocument &o)
	: document_(o.document_)
{
	if (document_ != 0)
		document_->acquire();
}

XmlDocument &XmlDocument::operator=(const XmlDocument &o)
{
	if (this != &o && document_ != o.document_) {
		if (document_ != 0)
			document_->release();
		document_ = o.document_;
		if (document_ != 0)
			document_->acquire();
	}
	return *this;
}

void XmlDocument::setName(const std::string &name)
{
	CHECK_POINTER;
	document_->setName(name, /*modified*/true);
}

std::string XmlDocument::getName() const
{
	CHECK_POINTER;
	return document_->getName();
}

static Document *copyDoc(Document *doc)
{
	if (!doc->isEmpty()) {
		Document *newdoc = doc->createCopy();
		doc->release();
		newdoc->acquire();
		return newdoc;
	}
	return doc;
}

void XmlDocument::setContent(const std::string &content)
{
	CHECK_POINTER;
	DbtOut *dbtContent = new DbtOut((void*)content.c_str(),
					content.length());
	document_ = copyDoc(document_);
	document_->setContentAsDbt(&dbtContent);
}

void XmlDocument::setContent(const XmlData &content)
{
	CHECK_POINTER;
	DbtOut *dbtContent = new DbtOut(content.get_data(), content.get_size());
	document_ = copyDoc(document_);
	document_->setContentAsDbt(&dbtContent);
}

void XmlDocument::setContentAsXmlInputStream(XmlInputStream *adopted_str)
{
	CHECK_POINTER;
	document_ = copyDoc(document_);
	document_->setContentAsInputStream(&adopted_str);
}

void XmlDocument::setContentAsEventReader(XmlEventReader &reader)
{
	CHECK_POINTER;
	document_ = copyDoc(document_);
	document_->setContentAsEventReader(reader);
}

void XmlDocument::getContentAsEventWriter(XmlEventWriter &writer)
{
	CHECK_POINTER;
	document_->getContentAsEventWriter(writer);
}

XmlInputStream *XmlDocument::getContentAsXmlInputStream() const
{
	CHECK_POINTER;
	return document_->getContentAsInputStream();
}

XmlData XmlDocument::getContent() const
{
	CHECK_POINTER;
	const DbXmlDbt *dbt = document_->getContentAsDbt();
	if (dbt != 0 && dbt->size > 0) {
		return XmlData(dbt->data, dbt->size);
	}
	return XmlData();
}

std::string &XmlDocument::getContent(std::string &s) const
{
	CHECK_POINTER;
	const DbXmlDbt *dbt = document_->getContentAsDbt();
	if (dbt != 0 && dbt->size > 0) {
		s.assign((const char *)dbt->data, dbt->size);
	} else {
		s.erase();
	}
	return s;
}

XmlEventReader &XmlDocument::getContentAsEventReader() const
{
	CHECK_POINTER;
	return document_->getContentAsEventReader();
}


void XmlDocument::fetchAllData()
{
	CHECK_POINTER;
	document_->setEagerContentAndMetaData();
}

void XmlDocument::setMetaData(const std::string &uri,
			      const std::string &name, const XmlValue &value)
{
	CHECK_POINTER;
	Name n(uri.c_str(), name.c_str());
	document_->setMetaData(n, value, /*modified*/true);
}

bool XmlDocument::getMetaData(const std::string &uri,
			      const std::string &name, XmlValue &value) const
{
	CHECK_POINTER;
	Name n(uri.c_str(), name.c_str());
	return document_->getMetaData(n, value);
}

void XmlDocument::setMetaData(const std::string &uri,
			      const std::string &name, const XmlData &value)
{
	CHECK_POINTER;
	Name n(uri.c_str(), name.c_str());
	document_->setMetaData(n,
			       XmlValue::BINARY,
			       value, /*modified*/true);
}

bool XmlDocument::getMetaData(const std::string &uri,
			      const std::string &name, XmlData &value) const
{
	CHECK_POINTER;
	Name n(uri.c_str(), name.c_str());
	return document_->getMetaData(n, value);
}

void XmlDocument::removeMetaData(const std::string &uri,
				 const std::string &name)
{
	CHECK_POINTER;
	Name n(uri.c_str(), name.c_str());
	document_->removeMetaData(n);
}

XmlMetaDataIterator XmlDocument::getMetaDataIterator() const
{
	CHECK_POINTER;
	return new MetaDataIterator(document_);
}

bool XmlDocument::operator == (const XmlDocument &other) const {
	return (*document_ == *(other.document_));
}

bool XmlDocument::operator != (const XmlDocument &other) const {
	return !(*document_ == *(other.document_));
}
