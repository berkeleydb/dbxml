//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLINDEXLOOKUP_HPP
#define	__XMLINDEXLOOKUP_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include "XmlValue.hpp"
#include "db.h"

namespace DbXml
{

class XmlContainer;
class IndexLookup;
	
class DBXML_EXPORT XmlIndexLookup
{
public:
	// operation only applies when a non-null value is used
	enum Operation {
		NONE,
		EQ,  ///< equals
		GT,  ///< greater than
		GTE, ///< greater than or equal
		LT,  ///< less than
		LTE  ///< less than or equal
	};

	/** @name For Reference Counting */
	XmlIndexLookup();
	XmlIndexLookup(const XmlIndexLookup &o);
	XmlIndexLookup &operator=(const XmlIndexLookup &o);
	~XmlIndexLookup();
	bool isNull() const { return indexLookup_ == 0; }

	const std::string &getIndex() const;
	void setIndex(const std::string &index);

	const std::string &getNodeURI() const;
	const std::string &getNodeName() const;
	void setNode(const std::string &uri, const std::string &name);

	// Parent only applies to edge indexes
	const std::string &getParentURI() const;
	const std::string &getParentName() const;
	void setParent(const std::string &uri, const std::string &name);

	// low and high boundaries for range lookups
	const XmlValue &getLowBoundValue() const;
	Operation getLowBoundOperation() const;
	void setLowBound(const XmlValue &value, Operation op);

	const XmlValue &getHighBoundValue() const;
	Operation getHighBoundOperation() const;
	void setHighBound(const XmlValue &value, Operation op);

	const XmlContainer &getContainer() const;
	void setContainer(XmlContainer &container);
	
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 * DBXML_NO_INDEX_NODES, DBXML_CACHE_DOCUMENTS
	 */
	XmlResults execute(XmlQueryContext &context,
			   u_int32_t flags = 0) const;

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 * DBXML_NO_INDEX_NODES, DBXML_CACHE_DOCUMENTS
	 */
	XmlResults execute(XmlTransaction &txn, XmlQueryContext &context,
			   u_int32_t flags = 0) const;

	// internal use only
	XmlIndexLookup(IndexLookup *il);
	operator IndexLookup &() const
	{
		return *indexLookup_;
	}
private:
	class IndexLookup *indexLookup_;
};
	
}

#endif

