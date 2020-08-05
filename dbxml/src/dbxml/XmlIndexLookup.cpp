//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "IndexLookup.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlTransaction.hpp"

using namespace DbXml;
using namespace std;

static const char *className = "XmlIndexLookup";
#define CHECK_POINTER checkNullPointer(indexLookup_,className)


XmlIndexLookup::XmlIndexLookup()
	: indexLookup_(0)
{
}

XmlIndexLookup::XmlIndexLookup(IndexLookup *il)
	: indexLookup_(il)
{
	if (indexLookup_ != 0)
		indexLookup_->acquire();
}

XmlIndexLookup::XmlIndexLookup(const XmlIndexLookup &o)
	: indexLookup_(o.indexLookup_)
{
	if (indexLookup_ != 0)
		indexLookup_->acquire();
}
	
XmlIndexLookup &XmlIndexLookup::operator=(const XmlIndexLookup &o)
{
	if (indexLookup_ != o.indexLookup_) {
		if (indexLookup_ != 0)
			indexLookup_->release();
		indexLookup_ = o.indexLookup_;
		if (indexLookup_ != 0)
			indexLookup_->acquire();
	}
	return *this;
}

XmlIndexLookup::~XmlIndexLookup()
{
	if (indexLookup_ != 0)
		indexLookup_->release();
}
	
const std::string &XmlIndexLookup::getIndex() const
{
	CHECK_POINTER;
	return indexLookup_->getIndex();
}

void XmlIndexLookup::setIndex(const std::string &index)
{
	CHECK_POINTER;
	indexLookup_->setIndex(index);
}

const std::string &XmlIndexLookup::getNodeURI() const
{
	CHECK_POINTER;
	return indexLookup_->getNodeURI();
}

const std::string &XmlIndexLookup::getNodeName() const
{
	CHECK_POINTER;
	return indexLookup_->getNodeName();
}

void XmlIndexLookup::setNode(const std::string &uri, const std::string &name)
{
	CHECK_POINTER;
	indexLookup_->setNode(uri, name);
}

const std::string &XmlIndexLookup::getParentURI() const
{
	CHECK_POINTER;
	return indexLookup_->getParentURI();
}

const std::string &XmlIndexLookup::getParentName() const
{
	CHECK_POINTER;
	return indexLookup_->getParentName();
}

void XmlIndexLookup::setParent(const std::string &uri, const std::string &name)
{
	CHECK_POINTER;
	indexLookup_->setParent(uri, name);
}

const XmlValue &XmlIndexLookup::getLowBoundValue() const
{
	CHECK_POINTER;
	return indexLookup_->getLowBoundValue();
}

XmlIndexLookup::Operation XmlIndexLookup::getLowBoundOperation() const
{
	CHECK_POINTER;
	return indexLookup_->getLowBoundOperation();
}

void XmlIndexLookup::setLowBound(const XmlValue &value, Operation op)
{
	CHECK_POINTER;
	indexLookup_->setLowBound(op, value);
}

const XmlValue &XmlIndexLookup::getHighBoundValue() const
{
	CHECK_POINTER;
	return indexLookup_->getHighBoundValue();
}

XmlIndexLookup::Operation XmlIndexLookup::getHighBoundOperation() const
{
	CHECK_POINTER;
	return indexLookup_->getHighBoundOperation();
}

void XmlIndexLookup::setHighBound(const XmlValue &value, Operation op)
{
	CHECK_POINTER;
	indexLookup_->setHighBound(op, value);
}

const XmlContainer &XmlIndexLookup::getContainer() const
{
	CHECK_POINTER;
	return indexLookup_->getContainer();
}
	
void XmlIndexLookup::setContainer(XmlContainer &container)
{
	CHECK_POINTER;
	indexLookup_->setContainer(container);
}

XmlResults XmlIndexLookup::execute(XmlQueryContext &context,
				   u_int32_t flags) const
{
	CHECK_POINTER;
	return indexLookup_->execute(NULL, context, flags);
}

XmlResults XmlIndexLookup::execute(XmlTransaction &txn,
				   XmlQueryContext &context,
				   u_int32_t flags) const
{
	CHECK_POINTER;
	return indexLookup_->execute(txn, context, flags);
}


