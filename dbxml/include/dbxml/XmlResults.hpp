//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLRESULTS_HPP
#define __XMLRESULTS_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"
#include "XmlQueryContext.hpp"
#include <string>

namespace DbXml
{

class Results;

class DBXML_EXPORT XmlResults
{
public:
	/** @name For Reference Counting */
	XmlResults();
	XmlResults(const XmlResults&);
	XmlResults &operator=(const XmlResults &);
	virtual ~XmlResults();
	bool isNull() const { return results_ == 0; }

	/** @name Iteration Methods */
	bool next(XmlValue &value);
	bool next(XmlDocument &document);

	bool previous(XmlValue &value);
	bool previous(XmlDocument &document);

	bool peek(XmlValue &value);
	bool peek(XmlDocument &document);

	bool hasNext();
	bool hasPrevious();
	void reset();

	XmlQueryContext::EvaluationType getEvaluationType() const;

	/** @name Eager Results Methods */
	size_t size() const;
	void add(const XmlValue &value);

	/** @name Private Methods (for internal use) */
	// @{

	XmlResults(Results *results);
	operator Results *()
	{
		return results_;
	}
	operator Results &()
	{
		return *results_;
	}

	//@}

	XmlEventWriter& asEventWriter();
	XmlResults copyResults();
	void concatResults(XmlResults &from);
private:
	Results *results_;
};

}

#endif

