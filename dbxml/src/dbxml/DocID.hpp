//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DOCID_HPP
#define	__DOCID_HPP

#include <iosfwd>
#include <db.h>
/** DocID
 * A class to encapsulate Document IDs, which are
 * 64-bits, as of release 2.3.
 */


typedef u_int64_t docId_t;

namespace DbXml
{
	
class DbXmlDbt;
class DbtOut;
class ContainerBase;
class XmlDocument;
class ReferenceMinder;
class OperationContext;
class DbXmlConfiguration;
	
class DocID
{
public:
	DocID() : id_(0)
	{}
	DocID(docId_t id) : id_(id)
	{ }
	DocID &operator=(const DocID &o) {
		id_ = o.id_;
		return *this;
	}
	void reset()
	{
		id_ = 0;
	}
	const void *rawPtr() const
	{
		return &id_;
	}
	void setThisFromDbt(const DbXmlDbt &dbt);
	void setDbtFromThis(DbtOut &dbt) const;
	u_int32_t unmarshal(const void *buf);
	u_int32_t marshal(void *buf) const;
	u_int32_t size() const { return sizeof(docId_t); }
	u_int32_t marshalSize() const;
	bool operator==(const DocID &o) const
	{
		return id_ == o.id_;
	}
	bool operator==(docId_t id) const
	{
		return id_ == id;
	}
	bool operator!=(const DocID &o) const
	{
		return id_ != o.id_;
	}
	bool operator!=(docId_t id) const
	{
		return id_ != id;
	}
	bool operator<(const DocID &o) const
	{
		return id_ < o.id_;
	}
	bool operator>(const DocID &o) const
	{
		return id_ > o.id_;
	}
	bool operator<=(const DocID &o) const
	{
		return id_ <= o.id_;
	}
	bool operator>=(const DocID &o) const
	{
		return id_ >= o.id_;
	}
	DocID &operator++()
	{
		++id_;
		return *this;
	}
	std::string asString() const;
	void asChars(char *buf, int buflen) const;
	void fetchDocument(const ContainerBase *container, DbXmlConfiguration &conf,
		XmlDocument &doc, ReferenceMinder *minder) const;
	void fetchDocument(const ContainerBase *container, OperationContext &oc,
		u_int32_t flags, XmlDocument &doc, ReferenceMinder *minder) const;

	static int compareMarshaled(const unsigned char *&p1,
				    const unsigned char *&p2);

private:
	docId_t id_;
};

}

#endif

