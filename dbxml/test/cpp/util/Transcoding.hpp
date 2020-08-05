//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
// Copied from DB XML code (all of UTF8, subset of NsUtil)

#ifndef __TRANSCODING_HPP
#define __TRANSCODING_HPP

#include <xercesc/util/XMLUTF8Transcoder.hpp>

#include "dbxml/XmlException.hpp"
#include <db.h>

#include <string>

//////////////////////////////////////////////
// from NsTypes.hpp

#define XER_NS XERCES_CPP_NAMESPACE_QUALIFIER

#define uint8_t u_int8_t
#define uint16_t u_int16_t
#define uint32_t u_int32_t

typedef XMLCh xmlch_t;  /* UTF-16 character */
typedef unsigned char xmlbyte_t; /* UTF-8 character */

namespace DbXmlTest
{

class UTF8ToXMLCh
{
public:
	UTF8ToXMLCh(const std::string &s);
	UTF8ToXMLCh(const char *s, size_t l);
	~UTF8ToXMLCh();
	const XMLCh *str() const;
	int len() const;
	XMLCh *adopt();
private:
	XMLCh *xmlch_;
	int len_;
};

class XMLChToUTF8
{
public :
	XMLChToUTF8(const XMLCh* const toTranscode, int len = 0);
	~XMLChToUTF8();
	const char *str() const;
	int len() const;
	char *adopt();
private :
	xmlbyte_t *p_;
	int len_;
};

class NsUtil {
public:
	static int nsToUTF8(xmlbyte_t **dest,
			    const xmlch_t *src, uint32_t nchars,
			    uint32_t maxbytes, bool *hasEntity);
	static int nsFromUTF8(xmlch_t **dest,
			      const xmlbyte_t *src,
			      uint32_t nbytes, uint32_t maxchars);
	static int nsStringLen(const xmlch_t *str);

	static void nsThrowException(DbXml::XmlException::ExceptionCode,
				     const char *desc,
				     const char *file =0, int line = 0);
	static void *allocate(size_t size);
	static void deallocate(void *obj);
};

#define NS_ASSERT(x) { if (!(x)) NsUtil::nsThrowException(XmlException::INTERNAL_ERROR, "Assertion Failure", __FILE__, __LINE__); }

}

#endif
