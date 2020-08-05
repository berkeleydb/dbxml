//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __METADATUM_HPP
#define	__METADATUM_HPP

#include "Name.hpp"
#include "dbxml/XmlValue.hpp"

namespace DbXml
{

class Container;
class DbtIn;
class DbtOut;
class NameID;
class DocID;
class OperationContext;

class MetaDatum
{
public:
	MetaDatum(const Name &name, XmlValue::Type type);
	MetaDatum(const Name &name, XmlValue::Type type,
		  DbtOut **dbt, bool modified); // Note: Consumes dbt
	virtual ~MetaDatum();
	const DbtOut *getDbt() const;
	void setDbt(DbtOut **dbt); // Note: Consumes dbt
	void setDbt(MetaDatum *md); // Note: Consumes md
	const char *getValue() const;
	XmlValue::Type getType() const;
	const Name &getName() const;
	static void decodeKeyDbt(const DbtOut &key, DocID &did, NameID &nid,
				 XmlValue::Type &type);
	static void setKeyDbt(const DocID &did, const NameID &nid,
			      XmlValue::Type type, DbtOut &key);
	void setValueDbtFromThis(DbtIn &value);
	bool canBeIndexed() const;
	void asValue(XmlValue &value) const;
	bool isModified() const { return modified_; }
	void setModified(bool m) { modified_ = m; }
	bool isRemoved() const { return removed_; }
	void setRemoved()  { removed_ = true; modified_ = true; }
private:
	MetaDatum(const MetaDatum &md);
	MetaDatum &operator=(const MetaDatum &o);

	DbXml::Name name_;
	XmlValue::Type type_;
	DbtOut *dbt_;
	bool modified_;
	bool removed_;
};

}

#endif
