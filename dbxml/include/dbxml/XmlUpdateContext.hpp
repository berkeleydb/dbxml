//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLUPDATECONTEXT_HPP
#define	__XMLUPDATECONTEXT_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class UpdateContext;

class DBXML_EXPORT XmlUpdateContext
{
public:
	/** @name For Reference Counting */
	XmlUpdateContext();
	XmlUpdateContext(const XmlUpdateContext&);
	XmlUpdateContext &operator=(const XmlUpdateContext &);
	virtual ~XmlUpdateContext();
	bool isNull() const { return updateContext_ == 0; }

	/** @name Private Methods (for internal use) */
	// @{

	XmlUpdateContext(UpdateContext *uc);
	operator UpdateContext&()
	{
		return *updateContext_;
	}
	operator UpdateContext*()
	{
		return updateContext_;
	}

	//@}

private:
	UpdateContext *updateContext_;
};

}
#endif
