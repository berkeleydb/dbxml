//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NAME_HPP
#define	__NAME_HPP

#include <string>
#include <iostream>

// NOTE: the "canonical" string form of a Name includes
// both URI and localName, and is:
//  localName\0uri\0
// The localName is first to make lookups faster in the database,
// since it will be unique more often than uri.  A name with no
// uri is:
//  localName\0\0
//
// This format affects code in Name.cpp, as well as Indexer.cpp
// and DictionaryDatabase.cpp
//
namespace DbXml
{
class DbtOut;
class Buffer;

class  DBXML_EXPORT Name
{
public:
	static const Name dbxml_colon_name;
	static const Name dbxml_colon_root;

	Name();
	Name(const char *uri, const char *name);
	Name(const std::string &uri, const std::string &name);
	Name(const char *uriname);
	Name(const Name &n);
	~Name();
	Name &operator = (const Name &n);
	bool operator<(const Name &n) const;
	bool operator==(const Name &n) const;
	const char *getURI() const;
	const char *getName() const;
	const std::string getURIName() const;
	void setThisFromDbt(const DbtOut &dbt);
	void setDbtFromThis_PrimaryValue(DbtOut &dbt) const;
	void setDbtFromThis_SecondaryKey(DbtOut &dbt) const;
	void reset();
	bool hasURI() const;
	std::string asString() const;

	static void writeToBuffer(Buffer &b, const char *name, size_t nl,
				  const char *uri, size_t ul);
private:
	int compare(const Name &n) const;
	std::string &uri();
	void set(const Name &n);
	void set(const char *n, size_t nl, const char *u, size_t ul);

	const Name *known_;
	size_t size_;
	char *buffer_;
	char *uri_;
	char *name_;
};

std::ostream& operator<<(std::ostream& s, const Name &name);

}

#endif
