//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// Some of the transcoding code is from Xerces, and is
// under the Apache license:
//
// The Apache Software License, Version 1.1
//
// Copyright (c) 1999-2004 The Apache Software Foundation.  All rights
//  reserved.
//
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "Manager.hpp"
#include "Buffer.hpp"
#include <db.h>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

using namespace DbXml;

XERCES_CPP_NAMESPACE_USE

//static
void *NsUtil::allocate(size_t size, const char *loc)
{
	void *ret = ::malloc(size);
	if (!ret) {
		std::string msg = "allocation failed";
		if (loc) {
			msg += ": ";
			msg += loc;
		}
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 msg.c_str());
	}
	return ret;
}
//static
void NsUtil::deallocate(void *obj)
{
	::free(obj);
}


//
// Transcoding
//

/*
 * Transcoding globals (from Xerces)
 *
 *  gUTFBytes
 *      A list of the size of the UTF-8 character for each initial byte in the input.
 *
 *  gUTFByteIndicator
 *      For a UTF8 sequence of n bytes, n>=2, the first byte of the
 *      sequence must contain n 1's followed by precisely 1 0 with the
 *      rest of the byte containing arbitrary bits.  This array stores
 *      the required bit pattern for validity checking.
 *
 *  gUTFByteIndicatorTest
 *      When bitwise and'd with the observed value, if the observed
 *      value is correct then a result matching gUTFByteIndicator will
 *      be produced.
 *
 *  gUTFOffsets
 *      A list of values to offset each result char type, according to how
 *      many source bytes when into making it.
 *
 *  gFirstByteMark
 *      A list of values to mask onto the first byte of an encoded sequence,
 *      indexed by the number of bytes used to create the sequence.
 */

const xmlbyte_t NsUtil::gUTFBytes[256] =
{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
    ,   4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6
};

static const xmlbyte_t gUTFByteIndicator[7] =
{
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const xmlbyte_t gUTFByteIndicatorTest[7] =
{
    0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE
};

const uint32_t NsUtil::gUTFOffsets[7] =
{
    0, 0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080
};

const xmlbyte_t NsUtil::gFirstByteMark[7] =
{
	0x00, 0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
};

/*
 * transcode from UTF-16 to UTF-8
 *  o will (re)allocate destination buffer if not present.
 *  start out with 3x the number of characters.  This memory
 *  should not hang around long.
 *  o returns number of *bytes* in the transcoded buffer.
 *  o code is derived from the Xerces class, XMLUTF8Transcoder
 *  o returns true or false in hasEntity if a special entity is
 *  encountered: '&', '<', '>', '"', ''
 *  No error checking -- the UTF-16 input was created by Xerces, and
 *  is trusted.
 */

size_t
NsUtil::nsToUTF8(xmlbyte_t **dest, const xmlch_t *src,
		 size_t nchars, size_t maxbytes, bool *hasEntity,
		 enum checkType type)
{
	size_t bufsize;
	size_t nrb = 0;
	xmlbyte_t *outPtr = *dest;
	if (hasEntity)
		*hasEntity = false;
	if (!outPtr) {
		bufsize = nchars * 3;
		outPtr = (xmlbyte_t *)NsUtil::allocate(
			bufsize * sizeof(xmlbyte_t));
		*dest = outPtr;
	} else {
		bufsize = maxbytes;
	}
	const xmlch_t *srcEnd = src + nchars;
	const xmlbyte_t *outEnd = outPtr + bufsize;

	while (src < srcEnd) {
		uint32_t curVal = *src;
		// deal with surrogates
		if ((curVal >= 0xd800) && (curVal <= 0xdbff)) {
			curVal =  ((curVal - 0xd800) << 10)
				+ ((*(src + 1) - 0xdc00) + 0x10000);
			src++;
		}
		src++;
		CHECK_ENT(curVal);
		// how many bytes
		int nbytes = codepointToUTF8Size(curVal);

		if (outPtr + nbytes > outEnd) {
			// reallocate (double size), or fail
			size_t count = (outPtr - *dest);
			xmlbyte_t *newmem =
				(xmlbyte_t *) NsUtil::allocate(bufsize << 1);
			memcpy(newmem, *dest, bufsize);
			bufsize <<= 1;
			NsUtil::deallocate(*dest);
			*dest = newmem;
			outEnd = newmem + bufsize;
			outPtr = newmem + count;
		}
		/* optimize 1-byte case */
		if (nbytes == 1) {
			*outPtr++ = (xmlbyte_t) curVal;
		} else {
			codepointToUTF8(curVal, nbytes, outPtr);
			outPtr += nbytes; // add bytes
		}
	} // while
	return (outPtr - *dest);
}

static inline void checkTrailingBytes(const xmlbyte_t toCheck)
{
	if ((toCheck & 0xC0) != 0x80)
		NsUtil::nsThrowException(XmlException::INVALID_VALUE,
			"checkTrailingBytes: bad utf-8 encoding",
			__FILE__, __LINE__);
}

/*
 * _nsFromUTF8 -- internal method for transcoding from utf-8 to
 *  utf-16
 *
 * This method returns how much of the original
 * utf-8 string is consumed in the eaten paramter.
 * This allows long strings to be transcoded in chunks.
 *
 * The code is basically structured to allow multiple chunk calls.
 * This is derived from Xerces transcoding code.
 *
 * The string is not entirely trusted as a valid utf-8 encoding.
 */
static size_t
_nsFromUTF8(xmlch_t *dest, const unsigned char *src,
	    size_t nbytes, size_t maxchars, size_t &eaten)
{
	DBXML_ASSERT(nbytes);

	const xmlbyte_t*  srcP = (const xmlbyte_t *) src;
	const xmlbyte_t*  endP = srcP + nbytes;
	xmlch_t *outP = dest;
	xmlch_t *outEnd = outP + maxchars;

	// loop until done with input, or out of space
	while ((srcP < endP) && (outP < outEnd)) {
		// ascii subset is simple
		if (*srcP <= 127) {
			*outP++ = *srcP++;
			continue;
		}

		// how many trailing src bytes will this sequence require
		const int utf8size = (int)NsUtil::nsCharSizeUTF8(*srcP);

		// if not enough source to do an entire character, break out
		// before processing the current one
		if (srcP + utf8size > endP)
			break;

		// start constructing the value.
		// Don't entirely trust encoding -- do validation checks
		// test first byte
		if((gUTFByteIndicatorTest[utf8size] & *srcP) !=
		   gUTFByteIndicator[utf8size]) {
			NsUtil::nsThrowException(XmlException::INVALID_VALUE,
					 "nsFromUTF8: bad utf-8 encoding",
					 __FILE__, __LINE__);
		}

		/***
		 * See http://www.unicode.org/reports/tr27/
		 *
		 * Table 3.1B. lists all of the byte sequences that
		 * are legal in UTF-8. A range of byte values such as
		 * A0..BF indicates that any byte from A0 to BF (inclusive)
		 * is legal in that position.
		 * Any byte value outside of the ranges listed is illegal.
		 *
		 * For example,the byte sequence <C0 AF> is illegal
		 * since C0 is not legal in the 1st Byte column.
		 *
		 * The byte sequence <E0 9F 80> is illegal since in the row
		 * where E0 is legal as a first byte, 9F is not legal
		 * as a second byte.
		 *
		 * The byte sequence '<F4 80 83 92>' is legal,
		 * since every byte in that sequence matches
		 * a byte range in a row of the table (the last row).
		 *
		 * The table is reproduced here for reference
		 *
		 * Table 3.1B. Legal UTF-8 Byte Sequences
		 * Code Points       1st Byte 2nd Byte 3rd Byte 4th Byte
		 * ==========================================================
		 * U+0000..U+007F      00..7F
		 * ----------------------------------------------------------
		 * U+0080..U+07FF      C2..DF  80..BF
		 *
		 * ----------------------------------------------------------
		 * U+0800..U+0FFF      E0      A0..BF   80..BF
		 *                             --
		 *
		 * U+1000..U+FFFF      E1..EF  80..BF   80..BF
		 *
		 * ----------------------------------------------------------
		 * U+10000..U+3FFFF    F0      90..BF   80..BF   80..BF
		 *                             --
		 * U+40000..U+FFFFF    F1..F3  80..BF   80..BF   80..BF
		 * U+100000..U+10FFFF  F4      80..8F   80..BF   80..BF
		 *                                 --
		 * ==========================================================
		 *
		 * Cases where a trailing byte range is not 80..BF are
		 * underlined in the table to draw attention to them.
		 * These occur only in the second byte of a sequence.
		 */
		switch(utf8size) {
		case 2 :
			// UTF-8:   [110y yyyy] [10xx xxxx]
			// Unicode: [0000 0yyy] [yyxx xxxx]
			//
			// 0xC0, 0xC1 has been filtered out
			checkTrailingBytes(*(srcP+1));
			break;
		case 3 :
			// UTF-8:   [1110 zzzz] [10yy yyyy] [10xx xxxx]
			// Unicode: [zzzz yyyy] [yyxx xxxx]
			//
			if (( *srcP == 0xE0) && ( *(srcP+1) < 0xA0)) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}

			checkTrailingBytes(*(srcP+1));
			checkTrailingBytes(*(srcP+2));

			//
			// See D36 (a)-(c) of
			//  http://www.unicode.org/reports/tr27/
			//
			//irregular three bytes sequence
			// that is zzzzyy matches leading surrogate tag
			// 110110 or trailing surrogate tag 110111
			//
			// *srcP=1110 1101
			// *(srcP+1)=1010 yyyy or
			// *(srcP+1)=1011 yyyy
			//
			// 0xED 1110 1101
			// 0xA0 1010 0000

			if ((*srcP == 0xED) && (*(srcP+1) >= 0xA0)) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}
			break;
		case 4 :
			// UTF-8:[1111 0uuu] [10uu zzzz] [10yy yyyy] [10xx xxxx]*
			// Unicode: [1101 10ww] [wwzz zzyy] (high surrogate)
			//          [1101 11yy] [yyxx xxxx] (low surrogate)
			//          * uuuuu = wwww + 1
			//
			if (((*srcP == 0xF0) && (*(srcP+1) < 0x90)) ||
			    ((*srcP == 0xF4) && (*(srcP+1) > 0x8F))) {
				NsUtil::nsThrowException(
					XmlException::INVALID_VALUE,
					"nsFromUTF8: bad utf-8 encoding",
					__FILE__, __LINE__);
			}

			checkTrailingBytes(*(srcP+1));
			checkTrailingBytes(*(srcP+2));
			checkTrailingBytes(*(srcP+3));
			break;
		default: // trailingBytes > 3

			/***
			 * The definition of UTF-8 in Annex D of ISO/IEC
			 * 10646-1:2000 also allows for the use of five- and
			 * six-byte sequences to encode characters that are
			 * outside the range of the Unicode character set;
			 * those five- and six-byte sequences are illegal
			 * for the use of UTF-8 as a transformation of Unicode
			 * characters. ISO/IEC 10646 does not allow mapping
			 * of unpaired surrogates, nor U+FFFE and
			 * U+FFFF (but it does allow other noncharacters).
			 ***/
			NsUtil::nsThrowException(
				XmlException::INVALID_VALUE,
				"nsFromUTF8: bad utf-8 encoding",
				__FILE__, __LINE__);
			break;
		}

		uint32_t tmpVal;
		NsUtil::UTF8ToCodepoint(srcP, utf8size, tmpVal);
		srcP += utf8size;

		//
		//  If it will fit into a single char, then put it in. Otherwise
		//  encode it as a surrogate pair. If its not valid, use the
		//  replacement char.
		//
		if (!(tmpVal & 0xFFFF0000)) {
			*outP++ = (xmlch_t) tmpVal;
		} else if (tmpVal > 0x10FFFF) {
			NsUtil::nsThrowException(
				XmlException::INVALID_VALUE,
				"nsFromUTF8: bad utf-8 encoding",
				__FILE__, __LINE__);
		} else {
			// if there's not enough room for all chars,
			// break out.
			if (outP + 1 >= outEnd)
				break; // this char never happened

			// Store the leading surrogate char
			tmpVal -= 0x10000;
			*outP++ = (xmlch_t)((tmpVal >> 10) + 0xD800);

			// the trailing char
			*outP++ = (xmlch_t)((tmpVal & 0x3FF) + 0xDC00);
		}
	}

	// Update the bytes eaten
	eaten = srcP - src;

	// Return the characters read
	return outP - dest;
}

/*
 * transcode from UTF-8 to UTF-16.  Return number of UTF-16
 * characters that resulted, including NULLs, if any are included
 * in nbytes.
 */
size_t
NsUtil::nsFromUTF8(xmlch_t **dest, const xmlbyte_t *src,
		   size_t nbytes, size_t maxchars)
{
	size_t bufsize;
	xmlch_t *outPtr = *dest;
	size_t eaten;
	if (!outPtr) {
		bufsize = nbytes << 1; // multiply by 2
		// caller needs to know if using manager or new for allocation
		outPtr = (xmlch_t *) NsUtil::allocate(bufsize);
		*dest = outPtr;
	}
	if (!maxchars)
		maxchars = nbytes;

	return _nsFromUTF8(outPtr, src, nbytes, maxchars, eaten);
}

// String routines

xmlbyte_t *
NsUtil::nsStringDup(const xmlbyte_t *str, size_t *lenP)
{
	if(str == 0) return 0;
	size_t len = nsStringLen(str) + 1;
	xmlbyte_t *copy = (xmlbyte_t *) NsUtil::allocate(len);
	memcpy(copy, str, len);
	if (lenP)
		*lenP = len;
	return copy;
}

xmlch_t *
NsUtil::nsStringDup(const xmlch_t *str, size_t *lenP)
{
	if(str == 0) return 0;
	size_t len = (nsStringLen(str) + 1) << 1;
	xmlch_t *copy = (xmlch_t *) NsUtil::allocate(len);
	memcpy(copy, str, len);
	if (lenP)
		*lenP = (len >> 1); // divide bytes by 2
	return copy;
}

bool
NsUtil::nsStringEqual(const xmlch_t *str1, const xmlch_t *str2)
{
	if (str1 == 0 || str2 == 0)
		return (str1 == str2);

	while (*str1 == *str2) {
		if (*str1 == 0)
			return true;
		++str1;
		++str2;
	}
	return false;
}

int NsUtil::stringEqualsIgnoreCase(const char *s1, const char *s2)
{
	u_char s1ch, s2ch;

	for (;;) {
		s1ch = *s1++;
		s2ch = *s2++;
		if (s1ch >= 'A' && s1ch <= 'Z')		/* tolower() */
			s1ch += 32;
		if (s2ch >= 'A' && s2ch <= 'Z')		/* tolower() */
			s2ch += 32;
		if (s1ch != s2ch)
			return (s1ch - s2ch);
		if (s1ch == '\0')
			return (0);
	}
	/* NOTREACHED */
}

int NsUtil::stringNEqualsIgnoreCase(const char *s1, const char *s2, size_t n)
{
	u_char s1ch, s2ch;

	for (; n != 0; --n) {
		s1ch = *s1++;
		s2ch = *s2++;
		if (s1ch >= 'A' && s1ch <= 'Z')		/* tolower() */
			s1ch += 32;
		if (s2ch >= 'A' && s2ch <= 'Z')		/* tolower() */
			s2ch += 32;
		if (s1ch != s2ch)
			return (s1ch - s2ch);
		if (s1ch == '\0')
			return (0);
	}
	return (0);
}

/*
 * Escape < & " and sometimes >
 */
static uint32_t _amp = '&';
static uint32_t _lt = '<';
static uint32_t _gt = '>';
static uint32_t _dq = '"';
static uint32_t _sp = ' ';
static uint32_t _rb = ']';
static const char *_ampStr = "&amp;";
static const char *_ltStr = "&lt;";
static const char *_gtStr = "&gt;";
static const char *_dquotStr = "&quot;";
#define _isNum(ch) (((ch) >= '0') && ((ch) <= '9'))
//
// only escape '&', '<', and '"'
// and '>' if it follows 2 ']' characters (per XML spec)
//
size_t
NsUtil::nsEscape(char *dest, const xmlbyte_t *src, size_t len, bool isAttr)
{
	char *start = dest;
	uint32_t ch;
	size_t nrb = 0; // num right bracket
	while (--len != 0 && (ch = *src++) != 0) {
		// reduce odds on common characters.  adds instructions
		// for real escapes, and some other characters,
		// but they are presumed uncommon
		if (ch > _gt || ch == _sp || _isNum(ch)) {
			if (ch == _rb)
				++nrb;
			else
				nrb = 0;
			*dest++ = ch;
		} else {
			if (ch == _amp) {
				memcpy(dest, _ampStr, 5);
				dest += 5;
			} else if (ch == _lt) {
				memcpy(dest, _ltStr, 4);
				dest += 4;
			} else if ((ch == _dq) && isAttr) {
				memcpy(dest, _dquotStr, 6);
				dest += 6;
			} else if ((ch == _gt) && nrb >= 2) {
				memcpy(dest, _gtStr, 4);
				dest += 4;
			} else {
				*dest++ = ch;
			}
			nrb = 0;
		}
	}
	*dest = '\0';
	return (dest-start);
}

//
// error handling utilities
//

// TBD: change code to XmlException::ExceptionCode
// Implement XmlException calls.
void
NsUtil::nsThrowException(XmlException::ExceptionCode code, const char *desc,
			 const char *file, int line)
{
	throw XmlException(code, desc, file, line);
}

void
NsUtil::nsThrowParseException(const char *message)
{
	throw XmlException(XmlException::INDEXER_PARSER_ERROR,
			   message);
}

//
// Class used for transcode/donate for donation of transcode
// strings to other objects. This happens during node storage parsing.
//

NsDonator::NsDonator(const xmlch_t *src, size_t len,
		     enum checkType type)
	: _str(0), _str2(0), _len(0),
	  _hasEntity(false)
{
	// handle empty strings -- this can happen for comments
	if (len == 0)
		return;
	bool checkEntity = (type == ignore ? false : true);
	if (src && *src)
		_len = NsUtil::nsToUTF8(&_str, src,
					len+1, 0, (checkEntity ?
						   &_hasEntity : 0), type) - 1;
	// if no src, parts will be uninitialized, and _str is null
}

NsDonator::NsDonator(const xmlch_t *src1,
		     const xmlch_t *src2,
		     XPath2MemoryManager *mmgr,
		     enum checkType type)
	:_str(0), _str2(0), _len(0), _hasEntity(false)
{
	if(src1 != 0 || src2 != 0) {
		bool checkEntity = (type == ignore ? false : true);
		size_t nlen = src1 ? NsUtil::nsStringLen(src1) : 0;
		size_t vlen = src2 ? NsUtil::nsStringLen(src2) : 0;

		size_t sz = (nlen + vlen);
		// make plenty of space for transcoding (3x) and 2 nulls.
		sz = (sz * 3) + 4;
		xmlbyte_t *str = (xmlbyte_t *) (mmgr ? mmgr->allocate(sz) :
						NsUtil::allocate(sz));
		size_t tlen = 0;
		if(src1 != 0) {
			_str = str;
			// never check for entities on first string
			tlen = NsUtil::nsToUTF8(&str, src1, nlen + 1, sz, 0);
			str += tlen;
		}
		if(src2 != 0) {
			_str2 = str;
			tlen += NsUtil::nsToUTF8(&str, src2, vlen + 1, sz - tlen,
				(checkEntity ? &_hasEntity : 0), type);
		}
		_len = tlen;
	}
}

static const unsigned char *encode64 = (const unsigned char *)
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void NsUtil::encodeBase64Binary(Buffer *buffer, const char *p, size_t l)
{
	int chNum = 0;
	unsigned char encode = 0, result, ch;
	const char *end = p + l;
	for(; p < end; ++p) {
		ch = *p;
		switch(chNum) {
		case 0:
			result = ch >> 2;
			DBXML_ASSERT(result < 64);
			result = encode64[result];
			buffer->write(&result, 1);

			encode = (ch & 0x3) << 4;
			chNum = 1;
			break;
		case 1:
			result = encode | (ch >> 4);
			DBXML_ASSERT(result < 64);
			result = encode64[result];
			buffer->write(&result, 1);

			encode = (ch & 0xF) << 2;
			chNum = 2;
			break;
		case 2:
			result = encode | (ch >> 6);
			DBXML_ASSERT(result < 64);
			result = encode64[result];
			buffer->write(&result, 1);

			result = (ch & 0x3F);
			DBXML_ASSERT(result < 64);
			result = encode64[result];
			buffer->write(&result, 1);

			encode = 0;
			chNum = 0;
			break;
		}
	}

	// Make sure a multiple of four bytes has been written
	switch(chNum) {
	case 0:
		// No more bits to write
		break;
	case 1:
		// Two bits to write
		result = encode;
		DBXML_ASSERT(result < 64);
		result = encode64[result];
		buffer->write(&result, 1);

		result = '=';
		buffer->write(&result, 1);
		buffer->write(&result, 1);
		break;
	case 2:
		// 4 bits to write
		result = encode;
		DBXML_ASSERT(result < 64);
		result = encode64[result];
		buffer->write(&result, 1);

		result = '=';
		buffer->write(&result, 1);
		break;
	}
}

static const unsigned char decode64[] = {
	//0    1    2    3    4    5    6    7    8    9
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //   0
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  10
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  20
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  30
	  0,   0,   0,  62,   0,   0,   0,  63,  52,  53, //  40
	 54,  55,  56,  57,  58,  59,  60,  61,   0,   0, //  50
	  0,   0,   0,   0,   0,   0,   1,   2,   3,   4, //  60
	  5,   6,   7,   8,   9,  10,  11,  12,  13,  14, //  70
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24, //  80
	 25,   0,   0,   0,   0,   0,   0,  26,  27,  28, //  90
	 29,  30,  31,  32,  33,  34,  35,  36,  37,  38, // 100
	 39,  40,  41,  42,  43,  44,  45,  46,  47,  48, // 110
	 49,  50,  51,   0,   0,   0,   0,   0,   0,   0, // 120
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 130
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 140
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 150
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 160
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 170
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 180
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 190
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 200
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 210
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 220
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 230
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 240
	  0,   0,   0,   0,   0,   0                      // 250
};

void NsUtil::decodeBase64Binary(Buffer *buffer, const char *p, size_t l)
{
	int chNum = 0;
	unsigned char result = 0, decode;
	const char *end = p + l;
	for(; p < end && *p != '='; ++p) {
		if(isWhitespace(*p)) continue;

		decode = decode64[(unsigned char)*p];
		switch(chNum) {
		case 0:
			result = decode << 2;
			chNum = 1;
			break;
		case 1:
			result |= decode >> 4;
			buffer->write(&result, 1);
			result = decode << 4;
			chNum = 2;
			break;
		case 2:
			result |= decode >> 2;
			buffer->write(&result, 1);
			result = decode << 6;
			chNum = 3;
			break;
		case 3:
			result |= decode;
			buffer->write(&result, 1);
			chNum = 0;
			break;
		}
	}
}

static const unsigned char decodeHex[] = {
	//0    1    2    3    4    5    6    7    8    9
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //   0
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  10
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  20
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  30
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   1, //  40
	  2,   3,   4,   5,   6,   7,   8,   9,   0,   0, //  50
	  0,   0,   0,   0,   0,  10,  11,  12,  13,  14, //  60
	 15,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  70
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  80
	  0,   0,   0,   0,   0,   0,   0,  10,  11,  12, //  90
	 13,  14,  15,   0,   0,   0,   0,   0,   0,   0, // 100
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 110
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 120
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 130
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 140
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 150
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 160
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 170
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 180
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 190
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 200
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 210
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 220
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 230
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 240
	  0,   0,   0,   0,   0,   0                      // 250
};

void NsUtil::decodeHexBinary(Buffer *buffer, const char *p, size_t l)
{
	unsigned char result = 0;
	const char *end = p + l - 1;
	for(; p < end; ++p) {
		result = decodeHex[(unsigned char)*p] << 4;
		++p;
		result |= decodeHex[(unsigned char)*p];
		buffer->write(&result, 1);
	}
}

NsString::NsString(const xmlch_t *text)
{
	text_ = NsUtil::nsStringDup(text, 0);
	owned_ = true;
}

void NsString::set(const xmlch_t *text, bool owned)
{
	if (text_ && owned_)
		::free((void *)text_);
	text_ = text;
	owned_ = owned;
}

void NsString::set(const xmlch_t *text)
{
	clear();
	text_ = NsUtil::nsStringDup(text, 0);
	owned_ = true;
}
