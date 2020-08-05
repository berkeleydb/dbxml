//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "IndexLookup.hpp"
#include "Container.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlException.hpp"

using namespace DbXml;
using namespace std;

IndexLookup::IndexLookup(XmlContainer &container,
			 const std::string &uri, const std::string &name,
			 const std::string &index, const XmlValue &value,
			 XmlIndexLookup::Operation op)
	: container_(container),
	  uri_(uri),
	  name_(name),
	  index_(index),
	  lowValue_(value),
	  highValue_(XmlValue()),
	  lowOp_(op),
	  highOp_(XmlIndexLookup::NONE)
{
}

IndexLookup::~IndexLookup()
{
}
	
const std::string &IndexLookup::getIndex() const
{
	return index_;
}

void IndexLookup::setIndex(const std::string &index)
{
	index_ = index;
}

const std::string &IndexLookup::getNodeURI() const
{
	return uri_;
}

const std::string &IndexLookup::getNodeName() const
{
	return name_;
}

void IndexLookup::setNode(const std::string &uri, const std::string &name)
{
	uri_ = uri;
	name_ = name;
}

bool IndexLookup::hasParent() const
{
	return (parentName_.length() != 0);
}

const std::string &IndexLookup::getParentURI() const
{
	return parentUri_;
}

const std::string &IndexLookup::getParentName() const
{
	return parentName_;
}

void IndexLookup::setParent(const std::string &uri, const std::string &name)
{
	parentUri_ = uri;
	parentName_ = name;
}

const XmlValue &IndexLookup::getLowBoundValue() const
{
	return lowValue_;
}

XmlIndexLookup::Operation IndexLookup::getLowBoundOperation() const
{
	return lowOp_;
}

void IndexLookup::setLowBound(XmlIndexLookup::Operation op,
			      const XmlValue &value)
{
	lowOp_ = op;
	lowValue_ = value;
}

const XmlValue &IndexLookup::getHighBoundValue() const
{
	return highValue_;
}

XmlIndexLookup::Operation IndexLookup::getHighBoundOperation() const
{
	return highOp_;
}

void IndexLookup::setHighBound(XmlIndexLookup::Operation op,
			       const XmlValue &value)
{
	highOp_ = op;
	highValue_ = value;
}

const XmlContainer &IndexLookup::getContainer() const
{
	return container_;
}
	
void IndexLookup::setContainer(XmlContainer &container)
{
	container_ = container;
}

Results *IndexLookup::execute(Transaction *txn,
			      XmlQueryContext &context,
			      u_int32_t flags) const
{
	Container *cont = container_;
	if (!cont) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Container is not a valid object");
	}
	return cont->lookupIndex(txn, context, *this, flags);
}

// verify that the object is self-consistent.
// Rules:
//   o if no value set, operation is ignored (assumed to be an entire
//     index lookup).
//   o if both low and high bounds set:
//     1. low bound op must be GT or GTE and high bound op must be LT or LTE.
//     2. values must be the same type.
//
void IndexLookup::validate() const
{
	if (!lowValue_.isNull() && !highValue_.isNull()) {
		if (lowValue_.getType() != highValue_.getType())
			throw XmlException(
				XmlException::INVALID_VALUE,
				"XmlIndexLookup::execute: XmlValue types must be the same for range lookup");
			
		if (((lowOp_ != XmlIndexLookup::GT) &&
		     (lowOp_ != XmlIndexLookup::GTE)) ||
		    ((highOp_ != XmlIndexLookup::LT) &&
		     (highOp_ != XmlIndexLookup::LTE))) {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"XmlIndexLookup::execute: invalid operation combination for range lookup");
		}
	}
}
