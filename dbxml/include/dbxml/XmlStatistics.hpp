//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLSTATISTICS_HPP
#define __XMLSTATISTICS_HPP

#include "XmlPortability.hpp"

namespace DbXml
{

class Statistics;

class DBXML_EXPORT XmlStatistics
{
public:
	/** @name For Reference Counting */
	XmlStatistics();
	XmlStatistics(const XmlStatistics&);
	XmlStatistics &operator=(const XmlStatistics &);
	virtual ~XmlStatistics();
	bool isNull() const { return statistics_ == 0; }

	/** @name Statistics Methods */
	double getNumberOfIndexedKeys() const;
	double getNumberOfUniqueKeys() const;
	double getSumKeyValueSize() const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlStatistics(Statistics *statistics);
	operator Statistics *()
	{
		return statistics_;
	}
	operator Statistics &()
	{
		return *statistics_;
	}

	//@}

private:
	Statistics *statistics_;
};

}

#endif

