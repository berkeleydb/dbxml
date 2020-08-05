//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLEXTERNALFUNCTION_HPP
#define __XMLEXTERNALFUNCTION_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"

namespace DbXml
{

class DBXML_EXPORT XmlArguments
{
public:
	virtual ~XmlArguments() {}

	/// Return the result for the argument at the index specified. Argument indexes are zero based.
	virtual XmlResults getArgument(size_t index) const = 0;

	/// Return the number of arguments specified
	virtual unsigned int getNumberOfArgs() const = 0;
};

class DBXML_EXPORT XmlExternalFunction
{
public:
	virtual ~XmlExternalFunction() {}

	/**
	 * Executes the external function with the given arguments, returning
	 * an XmlResults object as the result. The XmlTransaction object given should
	 * be used for any Berkeley DB or DB XML operations.
	 */
	virtual XmlResults execute(XmlTransaction &txn, XmlManager &mgr, const XmlArguments &args) const = 0;

	/**
	 * Called when DB XML has finished with this XmlExternalFunction object
	 */
	virtual void close() = 0;

protected:
	XmlExternalFunction() {}
private:
	XmlExternalFunction(const XmlExternalFunction &);
	XmlExternalFunction &operator=(const XmlExternalFunction &);
};

}

#endif

