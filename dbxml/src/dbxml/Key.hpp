//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __KEY_H
#define	__KEY_H

#include <string>
#include <vector>
#include "NameID.hpp"
#include "Syntax.hpp"
#include "IndexSpecification.hpp"
#include "nodeStore/NsTypes.hpp"

namespace DbXml
{

class XmlValue;
class KeyStash;
class Container;
class Buffer;
class IndexEntry;
class ContainerBase;

/** id1 is always the child, and id2 is always the parent.
    This means that id1 should always be set, and id2 is only
    set for an edge axis.*/
class Key
{
public:
	Key(int timezone);
	Key(const Key&);
	Key &operator=(const Key &);
	~Key();

	void set(const Index &i, const NameID &id1, const NameID &id2);
	void set(const Index &index, const NameID &id1,
		 const NameID &id2, const char *value);
	void set(const Index &index, const NameID &id1,
		 const NameID &id2, XmlValue &v);
	void set(const Key &key);
	void set(const Key &key, const char *p, size_t l);
	void set(const Key &key, const XmlValue &v);
	void reset();

	void setNodeLookup(bool nlup) { nodeLookup_ = nlup; }
	bool getNodeLookup() { return nodeLookup_; }

	/// Methods used by the QueryPlan
	void setDbtFromThis(DbtOut &dbt) const;
	void unmarshalStructure(const DbXmlDbt &dbt);

	void setValue(const char *p, size_t l);
	void setValue(const XmlValue &v);
	void addValue(const char *value, size_t len);
	const char *getValue() const;
	size_t getValueSize() const;
	void setIndex(const Index &index)
	{
		index_ = index;
	}
	const Index &getIndex() const
	{
		return index_;
	}
	Index &getIndex()
	{
		return index_;
	}
	Syntax::Type getSyntaxType() const
	{
		return (Syntax::Type)index_.getSyntax();
	} // jcm - cast
	const Syntax *getSyntax() const;
	NameID &getID1()
	{
		return id1_;
	}
	NameID &getID2()
	{
		return id2_;
	}
	void setID1(const NameID &id)
	{
		id1_ = id;
	}
	void setID2(const NameID &id)
	{
		id2_ = id;
	}
	void setIDsFromNames(OperationContext &context, const ContainerBase &container,
			     const char *parentUriName, const char *childUriName);

	std::string asString() const;
	std::string asString_XML(OperationContext &context,
				 const Container &container) const;

	bool operator<(const Key &o) const;

	/** Marshals the value before putting it in the buffer.
	    Returns the number of bytes written */
	size_t marshal(Buffer &buffer, const char *value, size_t length) const;
	static size_t structureKeyLength(const Index &index, const DbXmlDbt &dbt);
	static int compareStructure(const xmlbyte_t *&start1, const xmlbyte_t *end1, const xmlbyte_t *&start2, const xmlbyte_t *end2);
private:
	/// Unmarshals the value when taking it out of the buffer
	void unmarshalStructure(Buffer &buffer);

	bool nodeLookup_;
	Index index_;
	NameID id1_;
	NameID id2_;
	Buffer *value_;
	int timezone_;
};

inline int Key::compareStructure(const xmlbyte_t *&p1, const xmlbyte_t *e1, const xmlbyte_t *&p2, const xmlbyte_t *e2)
{
	// The first byte of the key is the prefix.
	int res = *p1 - *p2;
	if(res != 0) return res;

	// Unmarshal the index type
	Index::Type path = Index::getPathFromPrefix(*(char*)p1);
	++p1; ++p2;

	// Every key has at least one marshalled ID,
	// so now we check that.
	res = NameID::compareMarshaled(p1, p2);
	if(res != 0) return res;

	if(path == Index::PATH_EDGE) {
		// Check to see if we've come to the end of a key
		if(p1 >= e1) {
			if(p2 >= e2) return 0;
			return -1;
		}
		if(p2 >= e2) return +1;

		// The key has a second marshalled ID,
		// so lets check that.
		res = NameID::compareMarshaled(p1, p2);
		if(res != 0) return res;
	}

	return 0;
}

}

#endif

