//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlDocument.hpp"
#include "dbxml/XmlEventWriter.hpp"
#include "ResultsEventWriter.hpp"
#include "Results.hpp"

using namespace DbXml;

static const char *className = "XmlResults";
#define CHECK_POINTER checkNullPointer(results_,className)

XmlResults::XmlResults()
  : results_(0)
{
}

XmlResults::XmlResults(Results *results)
	: results_(results)
{
	if (results_ != 0)
		results_->acquire();
}

XmlResults::~XmlResults()
{
	if (results_ != 0)
		results_->release();
}

XmlResults::XmlResults(const XmlResults &o)
	: results_(o.results_)
{
	if (results_ != 0)
		results_->acquire();
}

XmlResults &XmlResults::operator=(const XmlResults &o)
{
	if (this != &o && results_ != o.results_) {
		if (results_ != 0)
			results_->release();
		results_ = o.results_;
		if (results_ != 0)
			results_->acquire();
	}
	return *this;
}

void XmlResults::reset()
{
	CHECK_POINTER;
	int err = results_->reset();
	if (err != 0)
		throw XmlException(err);
}

size_t XmlResults::size() const
{
	CHECK_POINTER;
	return results_->size();
}

bool XmlResults::hasNext()
{
	CHECK_POINTER;
	return results_->hasNext();
}

bool XmlResults::hasPrevious()
{
	CHECK_POINTER;
	return results_->hasPrevious();
}

bool XmlResults::next(XmlValue &value)
{
	CHECK_POINTER;
	int err = results_->next(value);
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::peek(XmlValue &value)
{
	CHECK_POINTER;
	int err = results_->peek(value);
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::previous(XmlValue &value)
{
	CHECK_POINTER;
	int err = results_->previous(value);
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::next(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(next(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

bool XmlResults::previous(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(previous(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

bool XmlResults::peek(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(peek(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

void XmlResults::add(const XmlValue &value)
{
	CHECK_POINTER;
	results_->add(value);
}

XmlQueryContext::EvaluationType XmlResults::getEvaluationType() const
{
	CHECK_POINTER;
	if (results_->isLazy())
		return XmlQueryContext::Lazy;
	else
		return XmlQueryContext::Eager;
}

XmlEventWriter& XmlResults::asEventWriter()
{
	CHECK_POINTER;
	return results_->asEventWriter();
}

XmlResults XmlResults::copyResults()
{
	CHECK_POINTER;
	return results_->copyResults();
}

void XmlResults::concatResults(XmlResults &from)
{
	CHECK_POINTER;
	results_->concatResults(*(Results *)from);
}

