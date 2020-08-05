//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlStatistics.hpp"
#include "Statistics.hpp"

using namespace DbXml;

static const char *className = "XmlStatistics";
#define CHECK_POINTER checkNullPointer(statistics_,className)

XmlStatistics::XmlStatistics()
  : statistics_(0)
{
}

XmlStatistics::XmlStatistics(Statistics *statistics)
  : statistics_(statistics)
{
  statistics_->acquire();
}

XmlStatistics::~XmlStatistics()
{
	statistics_->release();
}

XmlStatistics::XmlStatistics(const XmlStatistics &o)
	: statistics_(o.statistics_)
{
	if (statistics_ != 0)
		statistics_->acquire();
}

XmlStatistics &XmlStatistics::operator=(const XmlStatistics &o)
{
	if (this != &o && statistics_ != o.statistics_) {
		if (statistics_ != 0)
			statistics_->release();
		statistics_ = o.statistics_;
		if (statistics_ != 0)
			statistics_->acquire();
	}
	return *this;
}

double XmlStatistics::getNumberOfIndexedKeys() const
{
	CHECK_POINTER;
	return statistics_->getNumberOfIndexedKeys();
}

double XmlStatistics::getNumberOfUniqueKeys() const
{
	CHECK_POINTER;
	return statistics_->getNumberOfUniqueKeys();
}

double XmlStatistics::getSumKeyValueSize() const
{
	CHECK_POINTER;
	return statistics_->getSumKeyValueSize();
}

