//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
// Copied from DB XML code (all of UTF8, subset of NsUtil)

#include <xercesc/util/XMLUniDefs.hpp>
#include "dbxml/DbXml.hpp"
#include "Transcoding.hpp"

using namespace DbXmlTest;
using namespace DbXml;

XERCES_CPP_NAMESPACE_USE

UTF8ToXMLCh::UTF8ToXMLCh(const std::string &s)
{
	uint32_t l = (uint32_t)s.length();
	xmlch_ = (XMLCh *) NsUtil::allocate((l + 1) << 1);
	// len_ does not get trailing null
	len_ = NsUtil::nsFromUTF8(&xmlch_, (const xmlbyte_t *)s.c_str(),
				  l + 1, l + 1) - 1;
}

// string may not be null terminated.
UTF8ToXMLCh::UTF8ToXMLCh(const char *s, size_t l)
{
	xmlch_ = (XMLCh *) NsUtil::allocate((l + 1) << 1);
	// len_ does not get trailing null, so l must not include it.
	len_ = NsUtil::nsFromUTF8(&xmlch_, (const xmlbyte_t *)s,
				  (uint32_t)l, (uint32_t)l + 1);
	xmlch_[l] = 0; // Add null terminator
}

UTF8ToXMLCh::~UTF8ToXMLCh()
{
	delete [] xmlch_;
}

const XMLCh *UTF8ToXMLCh::str() const
{
	return xmlch_;
}

int UTF8ToXMLCh::len() const
{
	return len_;
}

XMLCh *UTF8ToXMLCh::adopt()
{
	XMLCh *result = xmlch_;
	xmlch_ = 0;
	return result;
}

XMLChToUTF8::XMLChToUTF8(const XMLCh* const toTranscode, int len)
{
	if(toTranscode == 0) {
		p_ = (xmlbyte_t *)NsUtil::allocate(1);
		*p_ = 0;
		len_ = 0;
	} else {
		if (!len)
			len = NsUtil::nsStringLen(toTranscode);
		// 3 bytes per XMLCh is the worst case, + '\0'
		const unsigned int needed = len * 3 + 1;
		p_ = (xmlbyte_t *) NsUtil::allocate(needed);
		// len_ does not get trailing null
		len_ = NsUtil::nsToUTF8(&p_, toTranscode, len + 1,
					needed, 0) - 1;
	}
}

XMLChToUTF8::~XMLChToUTF8()
{
	if (p_)
		delete [] p_;
}

const char *XMLChToUTF8::str() const
{
	return (const char *)p_;
}

int XMLChToUTF8::len() const
{
	return len_;
}

char *XMLChToUTF8::adopt()
{
	char *result = (char *)p_;
	p_ = 0;
	return result;
}

/*
 * Transcoding globals (from Xerces)
 *
 *  gUTFBytes
 *      A list of counts of trailing bytes for each initial byte in the input.
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

static const xmlbyte_t gUTFBytes[256] =
{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const xmlbyte_t gUTFByteIndicator[6] =
{
    0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const xmlbyte_t gUTFByteIndicatorTest[6] =
{
    0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE
};

static const uint32_t gUTFOffsets[6] =
{
    0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080
};

static const xmlbyte_t gFirstByteMark[7] =
{
	0x00, 0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
};

#define CHECK_ENT(c) \
if (hasEntity && ((c) < chLatin_A)) { \
  if (((c) == chAmpersand) || ((c) == chOpenAngle) || \
    ((c) == chCloseAngle) || ((c) == chDoubleQuote) || \
    ((c) == chSingleQuote)) *hasEntity = true; \
}

/*
 * transcode from UTF-16 to UTF-8
 *  o will (re)allocate destination buffer if not present.
 *  start out with 3x the number of characters.  This memory
 *  should not hang around long.
 *  o returns number of *bytes* in the transcoded buffer.
 *  o code is derived from the Xerces class, XMLUTF8Transcoder
 *  o returns true or false in hasEntity if a special entity is
 *  encountered: &, <, >, ", '
 *  No error checking -- the UTF-16 input was created by Xerces, and
 *  is trusted.
 */

int
NsUtil::nsToUTF8(xmlbyte_t **dest, const xmlch_t *src,
	 uint32_t nchars, uint32_t maxbytes, bool *hasEntity)
{
	uint32_t bufsize;
	xmlbyte_t *outPtr = *dest;
	if (hasEntity)
		*hasEntity = false;
	if (!outPtr) {
		bufsize = nchars * 3;
		outPtr = (xmlbyte_t *) NsUtil::allocate(bufsize);
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
		uint32_t nbytes;
		if (curVal < 0x80)
			nbytes = 1;
		else if (curVal < 0x800)
			nbytes = 2;
		else if (curVal < 0x10000)
			nbytes = 3;
		else if (curVal < 0x200000)
			nbytes = 4;
		else if (curVal < 0x4000000)
			nbytes = 5;
		else if (curVal <= 0x7FFFFFFF)
			nbytes = 6;

		if (outPtr + nbytes > outEnd) {
			// reallocate (double size), or fail
			uint32_t count = (uint32_t)(outPtr - *dest);
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
			outPtr += nbytes; // work backwards
			switch(nbytes) {
			case 6 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 5 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 4 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 3 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 2 : *--outPtr =
					 xmlbyte_t((curVal | 0x80UL) & 0xbfUL);
				curVal >>= 6;
			case 1 : *--outPtr = xmlbyte_t
					 (curVal | gFirstByteMark[nbytes]);
			}
			outPtr += nbytes; // add bytes back again
		}
	} // while
	return (int)(outPtr - *dest);
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
static int
_nsFromUTF8(xmlch_t *dest, const unsigned char *src,
	    uint32_t nbytes, uint32_t maxchars, uint32_t &eaten)
{
	NS_ASSERT(nbytes)

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
		const uint32_t trailingBytes = gUTFBytes[*srcP];

		// if not enough source to do an entire character, break out
		// before processing the current one
		if (srcP + trailingBytes >= endP)
			break;

		// start constructing the value.
		// Don't entirely trust encoding -- do validation checks
		// test first byte
		if((gUTFByteIndicatorTest[trailingBytes] & *srcP) !=
		   gUTFByteIndicator[trailingBytes]) {
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
		 * The byte sequence <F4 80 83 92> is legal,
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
		uint32_t tmpVal = 0;

		switch(trailingBytes) {
		case 1 :
			// UTF-8:   [110y yyyy] [10xx xxxx]
			// Unicode: [0000 0yyy] [yyxx xxxx]
			//
			// 0xC0, 0xC1 has been filtered out
			checkTrailingBytes(*(srcP+1));

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			break;
		case 2 :
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

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;

			break;
		case 3 :
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

			tmpVal = *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;
			tmpVal <<= 6;
			tmpVal += *srcP++;

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

		tmpVal -= gUTFOffsets[trailingBytes];

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
	eaten = (uint32_t)(srcP - src);

	// Return the characters read
	return (int)(outP - dest);
}

/*
 * transcode from UTF-8 to UTF-16.  Return number of UTF-16
 * characters that resulted, including NULLs, if any are included
 * in nbytes.
 */
int
NsUtil::nsFromUTF8(xmlch_t **dest, const xmlbyte_t *src,
		  uint32_t nbytes, uint32_t maxchars)
{
	uint32_t bufsize;
	xmlch_t *outPtr = *dest;
	uint32_t eaten;
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

int
NsUtil::nsStringLen(const xmlch_t *str)
{
	int ret = 0;
	while (*str++ != 0)
		ret++;
	return ret;
}

void
NsUtil::nsThrowException(XmlException::ExceptionCode code, const char *desc,
			 const char *file, int line)
{
	throw XmlException(code, desc, file, line);
}

void *NsUtil::allocate(size_t size)
{
	void *ret = ::malloc(size);
	if (!ret) {
		nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "allocation failed");
	}
	return ret;
}

void NsUtil::deallocate(void *obj)
{
	::free(obj);
}
