//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __INDEXLOOKUP_HPP
#define	__INDEXLOOKUP_HPP

#include <string>
#include "dbxml/XmlIndexLookup.hpp"
#include "dbxml/XmlContainer.hpp"
#include "ReferenceCounted.hpp"

// Implementation class for XmlIndexLookup

namespace DbXml
{

class Transaction;
class Results;

class IndexLookup: public ReferenceCounted
{
public:
	IndexLookup(XmlContainer &container,
		    const std::string &uri, const std::string &name,
		    const std::string &index, const XmlValue &value,
		    XmlIndexLookup::Operation op);
	~IndexLookup();
	
	const std::string &getIndex() const;
	void setIndex(const std::string &index);

	const std::string &getNodeURI() const;
	const std::string &getNodeName() const;
	void setNode(const std::string &uri, const std::string &name);

	// Parent only applies to edge indexes
	bool hasParent() const;
	const std::string &getParentURI() const;
	const std::string &getParentName() const;
	void setParent(const std::string &uri, const std::string &name);

	// low and high boundaries for range lookups
	const XmlValue &getLowBoundValue() const;
	XmlIndexLookup::Operation getLowBoundOperation() const;
	void setLowBound(XmlIndexLookup::Operation op, const XmlValue &value);

	const XmlValue &getHighBoundValue() const;
	XmlIndexLookup::Operation getHighBoundOperation() const;
	void setHighBound(XmlIndexLookup::Operation op, const XmlValue &value);

	const XmlContainer &getContainer() const;
	void setContainer(XmlContainer &container);
	
	Results *execute(Transaction *txn,
			 XmlQueryContext &context,
			 u_int32_t flags) const;

	void validate() const;
private:
	mutable XmlContainer container_;
	std::string uri_;
	std::string name_;
	std::string parentUri_;
	std::string parentName_;
	std::string index_;
	XmlValue lowValue_;
	XmlValue highValue_;
	XmlIndexLookup::Operation lowOp_;
	XmlIndexLookup::Operation highOp_;
};
	
}

#endif

