//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "Name.hpp"
#include "ScopedDbt.hpp"
#include "Buffer.hpp"

using namespace std;
using namespace DbXml;

//
// these globals are used to make detection of these particular Name objects
// efficient.  When used in construction or comparison of Name objects
// new objects will point to these via the known_ member rather than
// going to the expense of copying strings.
//
const Name Name::dbxml_colon_name(metaDataNamespace_uri, metaDataName_name);
const Name Name::dbxml_colon_root(metaDataNamespace_uri, metaDataName_root);

Name::Name()
	: known_(0),
	size_(0),
	buffer_(0),
	uri_(0),
	name_(0)
{}

Name::Name(const char *u, const char *n)
	: known_(0),
	size_(0),
	buffer_(0),
	uri_(0),
	name_(0)
{
	size_t ul = (u == 0 ? 0 : strlen(u));
	size_t nl = (n == 0 ? 0 : strlen(n));
	set(n, nl, u, ul);
}

Name::Name(const std::string &u, const std::string &n)
	: known_(0),
	size_(0),
	buffer_(0),
	uri_(0),
	name_(0)
{
	set(n.c_str(), n.length(), u.c_str(), u.length());
}

Name::Name(const char *uriname)
	: known_(0),
	size_(0),
	buffer_(0),
	uri_(0),
	name_(0)
{
	// uriname= { name:uri | name }
	const char *p = strchr(uriname, ':');
	if (p == 0) {
		set(uriname, strlen(uriname), 0, 0);
	} else {
		++p; // skip ':'
		set(uriname, p - uriname - 1, p, strlen(p));
	}
}

Name::Name(const Name &n)
	: known_(0),
	size_(0),
	buffer_(0),
	uri_(0),
	name_(0)
{
	set(n);
}

Name::~Name()
{
	delete [] buffer_;
}

Name &Name::operator = (const Name &n)
{
	if (this != &n) {
		set(n);
	}
	return *this;
}

bool Name::operator<(const Name &n) const
{
	return (compare(n) < 0);
}

bool Name::operator==(const Name &n) const
{
	// compare() is not all that efficient in
	// the face of "known" objects, so bypass some code
	if (known_ && (known_ == n.known_ || known_ == &n))
		return true;
	else if (n.known_ == this)
		return true;
	return (compare(n) == 0);
}

const char *Name::getURI() const
{
	return known_ ? known_->getURI() : uri_;
}

const char *Name::getName() const
{
	return known_ ? known_->getName() : name_;
}

void Name::setThisFromDbt(const DbtOut &dbt)
{
	// name \0 uri \0
	const char *p = (const char*)dbt.data;
	size_t nl = strlen(p);
	size_t ul = strlen(p + nl + 1);
	set(p, nl, p + nl + 1, ul);
}

void Name::setDbtFromThis_PrimaryValue(DbtOut &dbt) const
{
	if (known_) {
		known_->setDbtFromThis_PrimaryValue(dbt);
	} else {
		// name \0 uri \0
		dbt.set(buffer_, size_);
	}
}

void Name::setDbtFromThis_SecondaryKey(DbtOut &dbt) const
{
	if (known_) {
		known_->setDbtFromThis_SecondaryKey(dbt);
	} else {
		size_t ul = (uri_ == 0 ? 0 : strlen(uri_));
		size_t nl = (name_ == 0 ? 0 : strlen(name_));
		size_t l = ul + (ul > 0 ? 1 : 0) + nl;
		dbt.set(0, l);
		Buffer b(dbt.data, l, /*wrapper=*/true);
		writeToBuffer(b, name_, nl, uri_, ul);
	}
}

//static
void Name::writeToBuffer(Buffer &b, const char *name, size_t nl,
			 const char *uri, size_t ul)
{
	// string format is name { : uri }
	// name is first to make comparisons faster, as uri
	// is likely to be repeated
	b.write(name, nl);
	if (ul > 0) {
		static const char colon = ':';
		b.write(&colon, sizeof(colon));
		b.write(uri, ul);
	}
}

std::ostream& DbXml::operator<<(std::ostream& s, const Name &n)
{
	s << n.asString();
	return s;
}

std::string Name::asString() const
{
	if (known_) {
		return known_->asString();
	} else {
		if (hasURI()) {
			std::string s("{");
			s += name_;
			if (hasURI()) {
				s += ",";
				s += uri_;
			}
			s += "}";
			return s;
		} else
			return name_;
	}
}

const std::string Name::getURIName() const
{
	if (known_) {
		return known_->getURIName();
	} else {
		std::string r;
		r += name_;
		if (hasURI()) {
			r += ":";
			r += uri_;
		}
		return r;
	}
}

int Name::compare(const Name &n) const
{
	if (known_ && (known_ == n.known_))
		return 0;
	const char *uri = getURI();
	const char *nuri = n.getURI();
	int r = 0;
	if (uri == 0 && nuri == 0) {
		r = 0;
	} else if (uri == 0 && nuri != 0) {
		r = -1;
	} else if (uri != 0 && nuri == 0) {
		r = 1;
	} else {
		r = strcmp(uri, nuri);
	}
	if (r == 0) {
		r = strcmp(getName(), n.getName());
	}
	return r;
}

void Name::reset()
{
	known_ = 0;
	if (buffer_ != 0)
		delete [] buffer_;
	buffer_ = 0;
	size_ = 0;
	uri_ = 0;
	name_ = 0;
}

bool Name::hasURI() const
{
	return known_ ? known_->hasURI() : (uri_ != 0 && uri_[0] != 0);
}

void Name::set(const Name &n)
{
	if (&n == &dbxml_colon_name) {
		reset();
		known_ = &n;
	} else if (&n == &dbxml_colon_root) {
		reset();
		known_ = &n;
	} else if (n.known_ != 0) {
		reset();
		known_ = n.known_;
	} else {
		size_t ul = (n.uri_ == 0 ? 0 : strlen(n.uri_));
		size_t nl = (n.name_ == 0 ? 0 : strlen(n.name_));
		set(n.name_, nl, n.uri_, ul);
	}
}

void Name::set(const char *n, size_t nl, const char *u, size_t ul)
{
	known_ = 0;
	if (buffer_ != 0)
		delete [] buffer_;
	size_ = ul + nl + 2; // add 2 for trailing nulls
	buffer_ = new char[size_];
	name_ = buffer_;
	uri_ = buffer_ + nl + 1;
	if (ul == 0)
		uri_[0] = 0;
	else {
		strncpy(uri_, u, ul);
		uri_[ul] = 0;
	}
	if (nl == 0)
		name_[0] = 0;
	else {
		strncpy(name_, n, nl);
		name_[nl] = 0;
	}
}
