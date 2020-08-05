//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSUTIL_HPP
#define __DBXMLNSUTIL_HPP

/*
 * NsUtil.hpp
 *
 * Utility functions
 */

#include "NsNid.hpp"
#include "../DbXmlInternal.hpp"
#include "../Globals.hpp"
#include "../db_utils.h"
#include <string>
#include "dbxml/XmlException.hpp"


/*
 * Utility routines:
 *   o utf-8 and utf-16 string manipulation and transcoding
 * These are grouped as static functions in a single class,
 * NsUtil, for namespace and convenience.
 */

/*
 * The only issues in text and attribute values are
 * *mostly* for the characters: & and <.
 * >, ', and " can be parsed in most contexts in XML.
 * '>' following "]]" needs to be escaped, too.  That is
 * handled as a special case.
 *
 * The only issue is " vs ' in attribute values.  DB XML
 * uses the convention of attr="value" so it will escape
 * any double-quote (") in attribute values, on output.
 */
#define CHECK_ENT(c)							\
	if ((c) == ']')							\
		++nrb;							\
	else if (c) {							\
		if (hasEntity && ((c) < 'A')) {				\
			if (((c) == '&') || ((c) == '<'))		\
				*hasEntity = true;			\
			else if ((c) == '"' && type == isAttrVal)	\
				*hasEntity = true;			\
			else if ((c) == '>' && nrb >= 2)		\
				*hasEntity = true;			\
		}							\
		nrb = 0;						\
	}								

enum checkType {
	ignore = 0,
	isCharacters = 1,
	isAttrVal = 2
};

class XPath2MemoryManager;

namespace DbXml
{

class Buffer;

class NsUtil {
public:
	// transcoding
	static size_t nsToUTF8(xmlbyte_t **dest,
			       const xmlch_t *src, size_t nchars,
			       size_t maxbytes,
			       bool *hasEntity = 0, enum checkType type = ignore);
	static size_t nsFromUTF8(xmlch_t **dest,
				 const xmlbyte_t *src,
				 size_t nbytes, size_t maxchars);

	/// Returns the number of bytes in the UTF-8 character
	static int codepointToUTF8Size(uint32_t ch);
	/// Returns the number of bytes in the UTF-8 character
	static int codepointToUTF8(uint32_t ch, xmlbyte_t *dest);
	static void codepointToUTF8(uint32_t ch, int utf8size, xmlbyte_t *dest);

	/// Returns the number of bytes in the UTF-8 character with the given first byte
	static int nsCharSizeUTF8(xmlbyte_t firstByte);
	/// Returns the number of bytes in the UTF-8 character
	static int UTF8ToCodepoint(const xmlbyte_t *src, uint32_t &ch);
	// Entirely trusts the UTF-8 encoding
	static void UTF8ToCodepoint(const xmlbyte_t *src, int utf8size, uint32_t &ch);

	// string methods
	static bool nsStringEqual(const xmlch_t *str1, const xmlch_t *str2);
	static bool nsStringEqual(const xmlbyte_t *str1,
				  const xmlbyte_t *str2);
	static size_t nsStringLen(const xmlch_t *str);
	static size_t nsStringLen(const unsigned char *str);
	static xmlbyte_t *nsStringDup(const xmlbyte_t *str, size_t *lenP);
	static xmlch_t *nsStringDup(const xmlch_t *str,
				    size_t *lenP);
	static int stringEqualsIgnoreCase(const char *s1, const char *s2);
	static int stringNEqualsIgnoreCase(const char *s1, const char *s2, 
						size_t n);

	// Escape < & " (latter only if isAttr)
	static size_t nsEscape(char *dest, const xmlbyte_t *src, size_t len,
			       bool isAttr = false);

	// Whitespace munging
	static bool isWhitespace(char c);
	static bool isWhitespace(const char *c);
	static void trimWhitespace(const char *&p, size_t &l);
	/**
	 * Performs in place trim of leading and trailing
	 * whitespace, and collapse of concurrent internal
	 * whitespace to a single space
	 */
	static void collapseWhitespace(xmlch_t *val);

	// Base 64 and hex binary
	static void encodeBase64Binary(Buffer *buffer, const char *p, size_t l);
	static void decodeBase64Binary(Buffer *buffer, const char *p, size_t l);
	static void decodeHexBinary(Buffer *buffer, const char *p, size_t l);

	// Memory mgmt
	static void *allocate(size_t size, const char *loc = 0);
	static void deallocate(void *obj);
        class AutoDealloc {
	public:
		AutoDealloc(void*p) {p_ = p;}
		~AutoDealloc() {
			if (p_)
				deallocate(p_);
		}
		void dealloc() {
			if (p_) {
				deallocate(p_);
				p_ = 0;
			}
		}
	private:
		void *p_;
	};

	// error handling
	static void nsThrowException(XmlException::ExceptionCode,
				     const char *desc,
				     const char *file =0, int line = 0);
	static void nsThrowParseException(const char *message);

private:
	static const xmlbyte_t gUTFBytes[256];
	static const uint32_t gUTFOffsets[7];
	static const xmlbyte_t gFirstByteMark[7];
};

/**
 * NsDonator
 *
 * An internal class to transcode UTF-16 strings to UTF-8, for donation
 * to the node storage classes.
 *
 * For multi-strings (str10str20), getLen() returns the total length,
 * including both nulls.  For normal strings, it's the same as nsStringLen(),
 * which does NOT include the single, trailing NULL.
 *
 * len passed in does not include trailing NULL.  This must be added
 * for nsToUTF8.
 */
class NsDonator {
public:
	NsDonator(const xmlch_t *src, size_t len,
		  enum checkType type = ignore);
	// multi-string constructor (creates src10src20)
	NsDonator(const xmlch_t *src1,
		  const xmlch_t *src2,
		  XPath2MemoryManager *mmgr = 0,
		  enum checkType type = ignore);
	xmlbyte_t *getStr() const { return _str; }
	xmlbyte_t *getStr2() const { return _str2; }
	size_t getLen() const { return _len; }
	bool getHasEntity() const { return _hasEntity; }
private:
	xmlbyte_t *_str;
	xmlbyte_t *_str2;
	size_t _len;
	bool _hasEntity;
};

		
// utility class to cache UTF-16 strings
class NsString {
public:
	NsString(const xmlch_t *text);
	NsString() : text_(0), owned_(false) {}
	~NsString() {
		if (text_ && owned_)
			::free((void*)text_);
	}
	void set(const xmlch_t *text);
	void set(const xmlch_t *text, bool owned);
	void clear();
	const xmlch_t *get() const;
	operator const XMLCh *() const { return text_; }
	NsString &operator=(const XMLCh *text) { set(text); return *this; }
private:
	const xmlch_t *text_;
	bool owned_;
};

// inlines
inline size_t
NsUtil::nsStringLen(const xmlch_t *str)
{
	int ret = 0;
	while (*str++ != 0)
		ret++;
	return ret;
}

inline size_t
NsUtil::nsStringLen(const unsigned char *str)
{
	return str == 0 ? 0 : strlen((const char *)str);
}

inline bool
NsUtil::nsStringEqual(const xmlbyte_t *str1, const xmlbyte_t *str2)
{
	if(str1 == str2) return true;

	if (str1 == 0 || str2 == 0)
		return false;

	while (*str1 == *str2) {
		if (*str1 == 0)
			return true;
		++str1;
		++str2;
	}
	return false;
// 	return (::strcmp((const char *)str1, (const char *)str2) == 0);
}

inline bool NsUtil::isWhitespace(char c)
{
	switch(c) {
	case ' ':
	case '\t':
	case '\n':
	case '\r': return true;
	default: break;
	}
	return false;
}
	
inline bool NsUtil::isWhitespace(const char *c)
{
	if (c && *c) {
		while (*c != '\0') {
			if (!isWhitespace(*c++))
				return false;
		}
		return true;
	}
	return false;
}

inline void NsUtil::trimWhitespace(const char *&p, size_t &l)
{
	while(l && isWhitespace(*p)) {
		++p; --l;
	}

	while(l && isWhitespace(*(p + l - 1))) {
		--l;
	}
}

inline void NsUtil::collapseWhitespace(xmlch_t *val)
{
	if(val == 0) return;

	bool seenWS = true;
	xmlch_t *src = val;
	xmlch_t *dest = val;
	while(*src != 0) {
		switch(*src) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			if(!seenWS) {
				*dest = ' ';
				++dest;
			}
			++src;
			seenWS = true;
			break;
		default:
			*dest = *src;
			++dest;
			++src;
			seenWS = false;
			break;
		}
	}

	if(seenWS && dest > val) {
		--dest;
	}
	*dest = 0;
}

/// Returns the number of bytes in the UTF-8 character
inline int NsUtil::codepointToUTF8Size(uint32_t ch)
{
	if(ch < 0x80)
		return 1;
	else if(ch < 0x800)
		return 2;
	else if(ch < 0x10000)
		return 3;
	else if(ch < 0x200000)
		return 4;
	else if(ch < 0x4000000)
		return 5;

// 	else if(ch <= 0x7FFFFFFF)
	return 6;
}

inline void NsUtil::codepointToUTF8(uint32_t ch, int utf8size, xmlbyte_t *dest)
{
	dest += utf8size; // work backwards
	switch(utf8size) {
	case 6: *--dest = xmlbyte_t((ch | 0x80UL) & 0xbfUL);
		ch >>= 6;
	case 5: *--dest = xmlbyte_t((ch | 0x80UL) & 0xbfUL);
		ch >>= 6;
	case 4: *--dest = xmlbyte_t((ch | 0x80UL) & 0xbfUL);
		ch >>= 6;
	case 3: *--dest = xmlbyte_t((ch | 0x80UL) & 0xbfUL);
		ch >>= 6;
	case 2: *--dest = xmlbyte_t((ch | 0x80UL) & 0xbfUL);
		ch >>= 6;
	case 1: *--dest = xmlbyte_t(ch | gFirstByteMark[utf8size]);
	}
}

/// Returns the number of bytes in the UTF-8 character
inline int NsUtil::codepointToUTF8(uint32_t ch, xmlbyte_t *dest)
{
	int size = codepointToUTF8Size(ch);
	codepointToUTF8(ch, size, dest);
	return size;
}

inline int NsUtil::nsCharSizeUTF8(xmlbyte_t firstByte)
{
	return gUTFBytes[firstByte];
}

// Entirely trusts the UTF-8 encoding
inline void NsUtil::UTF8ToCodepoint(const xmlbyte_t *src, int utf8size, uint32_t &ch)
{
	ch = 0;
	switch(utf8size) {
	case 4: ch += *src++;
		ch <<= 6;
	case 3: ch += *src++;
		ch <<= 6;
	case 2: ch += *src++;
		ch <<= 6;
	case 1: ch += *src;
	}

	ch -= gUTFOffsets[utf8size];
}

inline int NsUtil::UTF8ToCodepoint(const xmlbyte_t *src, uint32_t &ch)
{
	int size = nsCharSizeUTF8(*src);
	UTF8ToCodepoint(src, size, ch);
	return size;
}

inline const xmlch_t *NsString::get() const
{
	return text_;
}

inline void NsString::clear()
{
	if (text_ && owned_)
		::free((void*)text_);
	text_ = 0;
	owned_ = false;
}

}
#endif
