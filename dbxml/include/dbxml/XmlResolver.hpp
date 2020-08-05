//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLRESOLVER_HPP
#define	__XMLRESOLVER_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

/**
 * Base class designed for the user to derive from, to implement
 * aspects of file resolution policy.
 */
class DBXML_EXPORT XmlResolver
{
public:
	/** @name Virtual Destructor */
	virtual ~XmlResolver() {};

	/** @name Document and Collection Resolution */

	/**
	 * Resolve the given uri to an XmlValue. If the uri cannot be resolved
	 * by this XmlResolver, then the method should return false, otherwise
	 * it should return true.
	 */
	virtual bool resolveDocument(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, XmlValue &result) const;
	/**
	 * Resolve the given uri to an XmlResults. If the uri cannot be resolved
	 * by this XmlResolver, then the method should return false, otherwise
	 * it should return true.
	 */
	virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, XmlResults &result) const;

	/** @name Schema and DTD Resolution */

	/**
	 * Resolve the given schema location and namespace to an XmlInputStream.If the uri
	 * cannot be resolved by this XmlResolver, then the method should return 0. The
	 * returned XmlInputStream is adopted by DbXml, and will be deleted by it.
	 */
	virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
		const std::string &schemaLocation, const std::string &nameSpace) const;
	/**
	 * Resolve the given system id and public id to an XmlInputStream.If the uri
	 * cannot be resolved by this XmlResolver, then the method should return 0. The
	 * returned XmlInputStream is adopted by DbXml, and will be deleted by it.
	 */
	virtual XmlInputStream *resolveEntity(XmlTransaction *txn, XmlManager &mgr,
		const std::string &systemId, const std::string &publicId) const;

	/** @name Module Resolution */

	/**
	 * Resolve the given module namespace to a list of strings that are the
	 * locations for the files that make up the module. If the module can be
	 * resolved by this XmlResolver, then the method should populate the
	 * XmlResults object with string type XmlValue objects (the module
	 * locations) and return true. If the module cannot be resolved by this
	 * XmlResolver, then the method should return false.
	 *
	 * This method is only called for module imports for which no locations
	 * are given in the query.
	 */
	virtual bool resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
		const std::string &nameSpace, XmlResults &result) const;

	/**
	 * Resolve the given module location and namespace into an XQuery module.
	 * If the location cannot be resolved by this XmlResolver, then the method should return 0. The
	 * returned XmlInputStream is adopted by DbXml, and will be deleted by it.
	 */
	virtual XmlInputStream *resolveModule(XmlTransaction *txn, XmlManager &mgr,
		const std::string &moduleLocation, const std::string &nameSpace) const;

	/** @name External Functions Resolution */
	// @{

	/**
	 * Resolve the URI, name and number of arguments to an XmlExternalFunction
	 * implementation. If the external function cannot be resolved by this
	 * XmlResolver, then the method should return 0. The returned XmlExternalFunction
	 * is adopted by DbXml, the close() method will be called on it when DB XML has
	 * finished with it.
	 */
	virtual XmlExternalFunction *resolveExternalFunction(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, const std::string &name, size_t numberOfArgs) const;

	// @}

protected:
	XmlResolver() {};
	XmlResolver(XmlResolver &) {};
};

}

#endif
