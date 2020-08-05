//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __UTF8_HPP
#define	__UTF8_HPP

#include <xercesc/util/XMLUTF8Transcoder.hpp>

#include <string>
#include "nodeStore/NsTypes.hpp"

namespace DbXml
{
//
// UTF8ToXMLCh and XMLChToUTF8 always allocate at least
// one byte for even a null string, so that str() always
// returns a valid pointer (possibly to null).
// This semantic is required by some code that uses them.
//
// The *Null versions allow a null pointer, which is
// more efficient when a real pointer is not required.
//
class UTF8ToXMLCh
{
public:
	UTF8ToXMLCh() : xmlch_(0), len_(0) {}
	UTF8ToXMLCh(const std::string &s);
	UTF8ToXMLCh(const char *s);
	UTF8ToXMLCh(const char *s, size_t l);
	~UTF8ToXMLCh();
	void set(const char *s);
	const XMLCh *str() const;
	XMLCh *strToModify() { return xmlch_; }
	size_t len() const;
	XMLCh *adopt();
protected:
	void init(const char *s, size_t l);
	XMLCh *xmlch_;
	size_t len_;
};

class XMLChToUTF8
{
public :
	XMLChToUTF8() : p_(0), len_(0) {}
	XMLChToUTF8(const XMLCh* const toTranscode, size_t len = 0);
	~XMLChToUTF8();
	void set(const XMLCh* const toTranscode);
	const char *str() const {
		return (const char *)p_;
	}
	const unsigned char *ucstr() const {
		return (const unsigned char *)p_;
	}
	char *strToModify() { return (char*) p_; }
	size_t len() const { return len_; }
	char *adopt();
protected:
	void init(const XMLCh* const toTranscode, size_t len = 0);
	xmlbyte_t *p_;
	size_t len_;
};

class UTF8ToXMLChNull : public UTF8ToXMLCh
{
public:
	UTF8ToXMLChNull(const std::string &s) {
		if (s.length() != 0)
			init(s.c_str(), s.length());
	}
	UTF8ToXMLChNull(const char *s, size_t l) {
		if (s)
			init(s, l);
	}
	~UTF8ToXMLChNull() {
		if (xmlch_)
			delete [] xmlch_;
	}
};

class XMLChToUTF8Null : public XMLChToUTF8
{
public:
	XMLChToUTF8Null(const XMLCh* const toTranscode, size_t len = 0) {
		if (toTranscode)
			init(toTranscode, len);
	}
};
		
}

#endif
