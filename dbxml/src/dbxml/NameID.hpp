//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NAMEID_HPP
#define	__NAMEID_HPP

#include <dbxml/XmlPortability.hpp>
#include <iosfwd>
#include <db.h>

/** NameID
 * A class to encapsulate Name IDs, which are primary
 * keys into the DB RECNO dictionary database (32-bit)
 */

typedef u_int32_t nameId_t;

namespace DbXml
{

class DbXmlDbt;
class DbtOut;

class DBXML_EXPORT NameID
{
public:
	NameID() : id_(0)
	{}
	NameID(nameId_t id) : id_(id)
	{ }
	void reset()
	{
		id_ = 0;
	}
	nameId_t raw() const
	{
		return id_;
	}
	const void *rawPtr() const
	{
		return &id_;
	}
	void setThisFromDbt(const DbXmlDbt &dbt);
	void setDbtFromThis(DbtOut &dbt) const;
	void setThisFromDbtAsId(const DbXmlDbt &dbt);
	void setDbtFromThisAsId(DbtOut &dbt) const;
	u_int32_t unmarshal(const void *buf);
	u_int32_t marshal(void *buf) const;
	u_int32_t size() const { return sizeof(nameId_t); }
	u_int32_t marshalSize() const;
	bool operator==(const NameID &o) const
	{
		return id_ == o.id_;
	}
	bool operator==(nameId_t id) const
	{
		return id_ == id;
	}
	bool operator!=(const NameID &o) const
	{
		return id_ != o.id_;
	}
	bool operator!=(nameId_t id) const
	{
		return id_ != id;
	}
	bool operator<(const NameID &o) const
	{
		return id_ < o.id_;
	}
	bool operator>(const NameID &o) const
	{
		return id_ > o.id_;
	}

	static int compareMarshaled(const unsigned char *&p1,
				    const unsigned char *&p2);

	// default implementation
	// NameID(const NameID&);
	// void operator=(const NameID &);
private:
	nameId_t id_;
};

std::ostream& operator<<(std::ostream& s, NameID id);

}

#endif

