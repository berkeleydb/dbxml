//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "UTF8.hpp"
#include "nodeStore/NsUtil.hpp"

using namespace DbXml;

UTF8ToXMLCh::UTF8ToXMLCh(const std::string &s)
{
	size_t l = s.length();
	xmlch_ = new XMLCh[l + 1];
	try {
		// len_ does not get trailing null
		len_ = NsUtil::nsFromUTF8(&xmlch_,
					  (const xmlbyte_t *)s.c_str(),
					  l + 1, l + 1) - 1;
	} catch (...) { delete [] xmlch_; xmlch_ = 0; throw; }
}

UTF8ToXMLCh::UTF8ToXMLCh(const char *s)
{
	init(s, s ? ::strlen(s) : 0);
}

// string may not be null terminated.
UTF8ToXMLCh::UTF8ToXMLCh(const char *s, size_t l)
{
	init(s, l);
}

void UTF8ToXMLCh::set(const char *s)
{
	init(s, s ? ::strlen(s) : 0);
}

void
UTF8ToXMLCh::init(const char *s, size_t l)
{
	xmlch_ = new XMLCh[l + 1];
	try {
		if(s == 0) {
			delete [] xmlch_;
			xmlch_ = 0;
			len_ = 0;
		}
		else if(l == 0) {
			len_ = 0;
			xmlch_[0] = 0; // Add null terminator
		}
		else {
			// len_ does not get trailing null, so l must not include it.
			len_ = NsUtil::nsFromUTF8(&xmlch_,
						  (const xmlbyte_t *)s, l,
						  l + 1);
			xmlch_[len_] = 0; // Add null terminator
		}
	} catch (...) { delete [] xmlch_; xmlch_ = 0; throw; }
}

UTF8ToXMLCh::~UTF8ToXMLCh()
{
	delete [] xmlch_;
}

const XMLCh *UTF8ToXMLCh::str() const
{
	return xmlch_;
}

size_t UTF8ToXMLCh::len() const
{
	return len_;
}

XMLCh *UTF8ToXMLCh::adopt()
{
	XMLCh *result = xmlch_;
	xmlch_ = 0;
	return result;
}

XMLChToUTF8::XMLChToUTF8(const XMLCh* const toTranscode, size_t len)
{
	init(toTranscode, len);
}

void XMLChToUTF8::set(const XMLCh* const toTranscode)
{
	init(toTranscode);
}

void
XMLChToUTF8::init(const XMLCh* const toTranscode, size_t len)
{
	if(toTranscode == 0) {
		p_ = (xmlbyte_t *)NsUtil::allocate(1);
		*p_ = 0;
		len_ = 0;
	} else {
		if (!len)
			len = NsUtil::nsStringLen(toTranscode);
		// 3 bytes per XMLCh is the worst case, + '\0'
		const size_t needed = len * 3 + 1;
		p_ = (xmlbyte_t *) NsUtil::allocate(needed);
		len_ = NsUtil::nsToUTF8(&p_, toTranscode, len,
					needed);
		// null terminate
		p_[len_] = 0;
	}
}

XMLChToUTF8::~XMLChToUTF8()
{
	if (p_)
		NsUtil::deallocate(p_);
}

char *XMLChToUTF8::adopt()
{
	char *result = (char *)p_;
	p_ = 0;
	return result;
}

