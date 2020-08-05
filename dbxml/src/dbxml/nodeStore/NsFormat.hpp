//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSFORMAT_HPP
#define __DBXMLNSFORMAT_HPP

/*
 * NsFormat.hpp
 *
 * Class and utility functions that implement the node storage format
 * itself, including marshal/unmarshal of node storage nodes.
 * The class and abstraction can support multiple formats, if
 * necessary.  See NsFormat.cpp for descriptions and implementation
 */

#include "NsNid.hpp"
#include "NsUtil.hpp"
#include "NsNode.hpp"
#include "../Globals.hpp"

#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/Numeric.hpp>

// marshaling "protocol" version
#define NS_PROTOCOL_VERSION_COMPAT 1
#define NS_PROTOCOL_VERSION 2
#define NS_NUM_PROTOCOLS 2

namespace DbXml
{

class DocID;
class DbtOut;
class DbXmlDbt;
class OperationContext;
class DbWrapper;

class NsFormat {
public:
	struct nodeRecordSizes {
		size_t header;
		size_t text;
		size_t attributes;
		size_t navigation;
		size_t name;
	};
	virtual size_t marshalNodeKey(const DocID &did, const NsNid &nid,
		unsigned char *buf, bool count) const = 0;
	virtual void unmarshalNodeKey(DocID &did, NsFullNid *nid,
		unsigned char *buf, bool copyStrings) const = 0;

	virtual size_t marshalNodeData(const NsNode *node,
		unsigned char *buf, bool count) const = 0;
	virtual NsNode *unmarshalNodeData(
		unsigned char *buf, bool adoptBuffer, nodeRecordSizes *sizes = 0) const = 0;

	virtual ~NsFormat() {}
	// "factory" method to get the appropriate object
	static const NsFormat &getFormat(int protocol) {
		return *formats[protocol - 1];
	}

	// static utility methods
	
	// put/get/del node records
	static int putNodeRecord(DbWrapper &db, OperationContext &oc,
				 const DocID &did, const NsNid &nid,
				 const DbXmlDbt *data);
	static int putNodeRecord(DbWrapper &db, OperationContext &oc,
				 const DocID &id, const NsNode *node, bool add, size_t nodeSize = 0);
	static int delNodeRecord(DbWrapper &db, OperationContext &oc,
				 const DocID &id, const NsNid &nid);
	static int deleteAllNodes(DbWrapper &db, OperationContext &oc,
				  const DocID &id);
	static int getNodeRecord(DbWrapper &db, OperationContext &oc,
				 const DocID &did, const NsNid &nid,
				 DbXmlDbt *data, u_int32_t flags);
	static int getNextNodeRecord(DbWrapper &db, OperationContext &oc,
				     const DocID &did, const NsNid &nid,
				     DbXmlDbt *data, u_int32_t flags);
	// fetch an unmarshaled node
	static NsNode *fetchNode(const NsNid &nid,
				 const DocID &did,
				 DbWrapper &db,
				 OperationContext &oc,
				 bool forWrite);
	
	// marshaling
	static void marshalNodeKey(const DocID &did, const NsNid &nid, DbtOut &dbt);
	static void marshalNextNodeKey(const DocID &did, const NsNid &nid, DbtOut &dbt);

	static size_t unmarshalId(const xmlbyte_t *ptr, NsFullNid *id);
	static size_t unmarshalId(const xmlbyte_t *ptr, NsFullNid *id,
				  xmlbyte_t **endPP, bool copyStrings);
	static int countId(const NsFullNid *id);
	static int countId(const NsNid &id);
	static int marshalId(unsigned char *ptr, const NsFullNid *id);
	static int marshalId(unsigned char *ptr, const NsNid &id);
	static int marshalInt(xmlbyte_t *buf, uint32_t i);
	static int unmarshalInt(const xmlbyte_t *buf, uint32_t *i);
	static int unmarshalInt(const xmlbyte_t *buf, int32_t *i);
	static int countInt(uint32_t i);
	static int countMarshaledInt(const xmlbyte_t *buf); // knows 64-bit
	static int marshalInt64(xmlbyte_t *buf, uint64_t i);
	static int unmarshalInt64(const xmlbyte_t *buf, uint64_t *i);
	static int countInt64(uint64_t i);
	static const xmlbyte_t *skipId(const xmlbyte_t *ptr);
	static size_t unmarshText(unsigned char *ptr, nsText_t *text,
				    xmlbyte_t **endPP, bool copyStrings);
	static unsigned char *unmarshAttrs(unsigned char *ptr,
					   nsAttrList_t *attrs,
					   xmlbyte_t **endPP, bool copyStrings);
	static unsigned char *unmarshTextList(unsigned char *ptr,
					      nsTextList_t *text,
					      xmlbyte_t **endPP, bool copyStrings);

	static int countMAPM(const MAPM &value, Numeric::State state = Numeric::NUM);
	static void marshalMAPM(xmlbyte_t *buf, const MAPM &value, Numeric::State state = Numeric::NUM);
	static int unmarshalMAPMToTemporary(const xmlbyte_t *buf, M_APM_struct &tmp, Numeric::State *state = 0);
	static int unmarshalMAPM(const xmlbyte_t *buf, MAPM &value, Numeric::State *state = 0);
	static int compareMarshaledMAPM(const xmlbyte_t *&p1, const xmlbyte_t *&p2);
	static size_t getNodeDataSize(const NsNode *node);
	
	// logging
	static void logNodeOperation(DbWrapper &db, const DocID &did,
				     const NsNode *node, const NsNid &nid,
				     const DbXmlDbt *data, const char *op,
				     int err = 0);
private:
	// all supported formats
	static NsFormat *formats[NS_NUM_PROTOCOLS];
};

inline int NsFormat::unmarshalInt(const xmlbyte_t *buf, int32_t *i)
{
	return unmarshalInt(buf, (uint32_t*)i);
}

inline const xmlbyte_t *NsFormat::skipId(const xmlbyte_t *ptr)
{
	while (*ptr++);
	return ptr;
}

inline int NsFormat::countId(const NsFullNid *id)
{
	return id->getLen();
}

inline int NsFormat::marshalId(unsigned char *ptr, const NsFullNid *id)
{
	memcpy(ptr, id->getBytes(), id->getLen());
	return id->getLen();
}

inline int NsFormat::countId(const NsNid &id)
{
	return (int)id.getLen();
}

inline int NsFormat::marshalId(unsigned char *ptr, const NsNid &id)
{
	int ret = 1; // trailing null
	const unsigned char *n = (const unsigned char *)id;
	while ((*ptr++ = *n++) != 0) ++ret;
	return ret;
}

/*
 * Simple, variable-length encoding for integers.
 * The algorithm extends beyond 4-byte numbers, but
 * that's all that's necessary.  This is based on
 * an algorithm from:
 *   http://www.dlugosz.com/ZIP2/VLI.html
 * Used with permission.
 *
 * The high-order bits of the first byte indicate
 * number of bytes used for encoding:
 *   [0 xxxxxxx] 0-127
 *   [10 xxxxxx] [yyyyyyyy] 0-16383 [msb][lsb]
 *   [110 xxxxx] [yyyyyyyy] [zzzzzzzz] 0-2097151 [msb]...[lsb]
 *   [111 00 xxx] [3 bytes] 0-134217727 [msb]...[lsb]
 *   [111 01 xxx] [4 bytes] up to 35-bits (32G -- 0-34,359,738,367)
 *   [111 11 000] [5 bytes] up to 40-bits (1T -- 0-1,099,511,627,775)
 *   [111 11 001] [8 bytes] up to 64-bits (64-bit, plus 1 byte overhead)
 *
 * This results in using 1 byte for small numbers, which
 * is the common case in DB XML (nattrs, ntext, nchild, other
 * small sizes).
 */

#define NSINT_1BIT_MASK 0x80  /* one bit */
#define NSINT_2BIT_MASK 0xc0  /* two bits */
#define NSINT_3BIT_MASK 0xe0  /* three bits */
#define NSINT_5BIT_MASK 0xf8  /* five bits */
#define NSINT_8BIT_MASK 0xff  /* eight bits */

#define NSINT_2BYTE_VAL 0x80  /* 10 */
#define NSINT_3BYTE_VAL 0xc0  /* 110 */
#define NSINT_4BYTE_VAL 0xe0  /* 111 00 */
#define NSINT_5BYTE_VAL 0xe8  /* 111 01 */
#define NSINT_6BYTE_VAL 0xf8  /* 111 11 000 */
#define NSINT_9BYTE_VAL 0xf9  /* 111 11 001 */

#define NSINT_MAX1 0x7f
#define NSINT_MAX2 0x3fff
#define NSINT_MAX3 0x1fffff
#define NSINT_MAX4 0x7ffffff
#if defined(_MSC_VER) && _MSC_VER < 1300
#define NSINT_MAX5 0x7ffffffffi64
#define NSINT_MAX6 0xffffffffffi64
#else
#define NSINT_MAX5 0x7ffffffffLL
#define NSINT_MAX6 0xffffffffffLL
#endif
/* > MAX6 goes to 9-byte */

/*
 * NOTE: for everything else in DB XML, on-disk order is
 * little-endian, but this compression algorithm depends
 * on big-endian order, so swap if necessary.
 */
inline int
NsFormat::countInt(uint32_t i)
{
	if (i <= NSINT_MAX1)
		return 1;
	else if (i <= NSINT_MAX2)
		return 2;
	else if (i <= NSINT_MAX3)
		return 3;
	else  if (i <= NSINT_MAX4)
		return 4;
	else
		return 5;
}

inline int
NsFormat::countInt64(uint64_t i)
{
	if (i <= NSINT_MAX1)
		return 1;
	else if (i <= NSINT_MAX2)
		return 2;
	else if (i <= NSINT_MAX3)
		return 3;
	else  if (i <= NSINT_MAX4)
		return 4;
	else  if (i <= NSINT_MAX5)
		return 5;
	else  if (i <= NSINT_MAX6)
		return 6;
	else
		return 9;
}

/* encode: return nbytes */
inline int
NsFormat::marshalInt(xmlbyte_t *buf, uint32_t i)
{
	if (i <= NSINT_MAX1) {
		// no swapping for one byte value
		buf[0] = (unsigned char) i;
		return 1;
	} else {
		uint32_t tmp = i;
		if (!Globals::isBigendian_)
			M_32_SWAP(tmp);
		unsigned char *p = (unsigned char *) & tmp;
		if (i <= NSINT_MAX2) {
			buf[0] = (p[2] | NSINT_2BYTE_VAL);
			buf[1] = p[3];
			return 2;
		} else if (i <= NSINT_MAX3) {
			buf[0] = (p[1] | NSINT_3BYTE_VAL);
			buf[1] = p[2];
			buf[2] = p[3];
			return 3;
		} else  if (i <= NSINT_MAX4) {
			buf[0] = (p[0] | NSINT_4BYTE_VAL);
			buf[1] = p[1];
			buf[2] = p[2];
			buf[3] = p[3];
			return 4;
		} else {
			//32-bit marshaled format doesn't add
			// the mask bits, since the extra 3 bits
			// won't fit into uint32_t anyway.
			buf[0] = NSINT_5BYTE_VAL;
			buf[1] = p[0];
			buf[2] = p[1];
			buf[3] = p[2];
			buf[4] = p[3];
			return 5;
		}
	}
}

/** countMarshaledInt
 * Understands 32- or 64-bit encodings, as it only looks at first byte
 */
inline int
NsFormat::countMarshaledInt(const xmlbyte_t *buf)
{
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		return 2;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		return 3;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_4BYTE_VAL) {
		return 4;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_5BYTE_VAL) {
		return 5;
	} else if ((c & NSINT_8BIT_MASK) == NSINT_6BYTE_VAL) {
		return 6;
	} else {
		return 9;
	}
}

/* decode: return nbytes */
inline int
NsFormat::unmarshalInt(const xmlbyte_t *buf, uint32_t *i)
{
	int len;
	uint32_t tmp;
	unsigned char *p = (unsigned char *) &tmp;
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		// no swapping for one byte value
		*i = buf[0];
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		p[0] = p[1] = 0;
		p[2] = (c & ~NSINT_2BIT_MASK);
		p[3] = buf[1];
		len = 2;
		goto out;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		p[0] = 0;
		p[1] = (c & ~NSINT_3BIT_MASK);
		p[2] = buf[1];
		p[3] = buf[2];
		len = 3;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == 0xe0) {
		p[0] = (c & ~NSINT_5BIT_MASK);
		p[1] = buf[1];
		p[2] = buf[2];
		p[3] = buf[3];
		len = 4;
		goto out;
	} else {
		//32-bit marshaled format doesn't add
		// the mask bits, since the extra 3 bits
		// won't fit into uint32_t anyway.
		p[0] = buf[1];
		p[1] = buf[2];
		p[2] = buf[3];
		p[3] = buf[4];
		len = 5;
		goto out;
	}
 out:
	*i = tmp;
	if (!Globals::isBigendian_)
		M_32_SWAP(*i);
	return len;
}

/*
 * Format for arbitrary precision decimals:
 *
 * We use a format very close to the in-memory format for the MAPM
 * library which we use. This is extended to enable us to represent
 * (+/-) infinity and "not a number", which are needed for xs:float
 * and xs:double.
 *
 * First byte is an enumeration of the state of the number. This
 * includes it's sign and special status (infinity, not a number). The
 * enumeration is ordered specifically to allow simple comparison on the
 * state before needing to continue unmarshaling.
 *
 * The exponent (a signed integer) comes next, and specifies the
 * position of the decimal point in the numeric data. This is stored as
 * a packed integer.
 *
 * The datalength, a positive packed integer, is stored next. It
 * records the number of *digits* in the numeric data - where two digits
 * are stored per byte. The length of the numeric data in bytes is
 * therfore "(datalength + 1) >> 1".
 *
 * Next the raw numeric data is stored. Two (base 10) digits are stored
 * per byte, meaning that the byte is a number between 0 and 99. This is
 * *not* binary coded decimal - the digits relative values are added
 * togther.
 *
 * This format can unmarshaled into a M_APM_struct in a way that does not
 * need to copy memory - however, care should be taken with a MAPM
 * unmarshaled in this way, since MAPM objects created this way cannot be
 * freed or reallocated in the normal way!
 */

inline int NsFormat::countMAPM(const MAPM &value, Numeric::State state)
{
	const M_APM cval = value.c_struct();

	int result = 1; // State byte
	if((state == Numeric::NUM || state == Numeric::NEG_NUM) &&
		cval->m_apm_sign != 0) {
		result += NsFormat::countInt((uint32_t)cval->m_apm_exponent);
		result += NsFormat::countInt((uint32_t)cval->m_apm_datalength);
		result += (cval->m_apm_datalength + 1) >> 1; // Actual data
	}

	return result;
}

enum MAPMMarshalState {
	MARSHAL_NEGATIVE_INFINITY = 0,
	MARSHAL_NEGATIVE = 1,
	MARSHAL_ZERO = 2,
	MARSHAL_POSITIVE = 3,
	MARSHAL_POSITIVE_INFINITY = 4,
	MARSHAL_NOT_A_NUMBER = 5
};

inline void NsFormat::marshalMAPM(xmlbyte_t *buf, const MAPM &value, Numeric::State state)
{
	const M_APM cval = value.c_struct();

	switch(state) {
	case Numeric::NaN:
		*buf++ = MARSHAL_NOT_A_NUMBER;
		break;
	case Numeric::INF:
		*buf++ = MARSHAL_POSITIVE_INFINITY;
		break;
	case Numeric::NEG_INF:
		*buf++ = MARSHAL_NEGATIVE_INFINITY;
		break;
	case Numeric::NEG_NUM:
	case Numeric::NUM:
		if(cval->m_apm_sign == 0) {
			*buf++ = MARSHAL_ZERO;
		}
		else {
			if(cval->m_apm_sign < 0)
				*buf++ = MARSHAL_NEGATIVE;
			else *buf++ = MARSHAL_POSITIVE;

			buf += NsFormat::marshalInt(buf, (uint32_t)cval->m_apm_exponent);
			buf += NsFormat::marshalInt(buf, (uint32_t)cval->m_apm_datalength);

			memcpy(buf, cval->m_apm_data, (cval->m_apm_datalength + 1) >> 1);
		}
		break;
	}
}

inline int NsFormat::unmarshalMAPMToTemporary(const xmlbyte_t *buf, M_APM_struct &tmp, Numeric::State *state)
{
	tmp.m_apm_refcount = 1;
	tmp.m_apm_malloclength = 0;

	const xmlbyte_t *ptr = buf;

	switch((MAPMMarshalState)*ptr++) {
	case MARSHAL_NEGATIVE_INFINITY:
		if(state != 0) *state = Numeric::NEG_INF;
		tmp.m_apm_sign = 0;
		break;
	case MARSHAL_POSITIVE_INFINITY:
		if(state != 0) *state = Numeric::INF;
		tmp.m_apm_sign = 0;
		break;
	case MARSHAL_NOT_A_NUMBER:
		if(state != 0) *state = Numeric::NaN;
		tmp.m_apm_sign = 0;
		break;
	case MARSHAL_NEGATIVE:
		if(state != 0) *state = Numeric::NEG_NUM;
		tmp.m_apm_sign = -1;
		break;
	case MARSHAL_POSITIVE:
		if(state != 0) *state = Numeric::NUM;
		tmp.m_apm_sign = 1;
		break;
	case MARSHAL_ZERO:
		if(state != 0) *state = Numeric::NUM;
		tmp.m_apm_sign = 0;
		break;
	}

	if(tmp.m_apm_sign == 0) {
		tmp.m_apm_exponent = 0;
		tmp.m_apm_datalength = 0;
		tmp.m_apm_data = 0;
	}
	else {
		ptr += NsFormat::unmarshalInt(ptr, (int32_t*)&tmp.m_apm_exponent);
		ptr += NsFormat::unmarshalInt(ptr, (int32_t*)&tmp.m_apm_datalength);
		tmp.m_apm_data = (UCHAR*)ptr;
		ptr += (tmp.m_apm_datalength + 1) >> 1;
	}

	return (int)(ptr - buf);
}

inline int NsFormat::unmarshalMAPM(const xmlbyte_t *buf, MAPM &value, Numeric::State *state)
{
	M_APM_struct tmp;

	int length = unmarshalMAPMToTemporary(buf, tmp, state);

	MAPM copy;
	m_apm_copy(const_cast<M_APM>(copy.c_struct()), &tmp);

	value = copy;

	return length;
}

inline int NsFormat::compareMarshaledMAPM(const xmlbyte_t *&p1, const xmlbyte_t *&p2)
{
	MAPMMarshalState state1 = (MAPMMarshalState)*p1;
	MAPMMarshalState state2 = (MAPMMarshalState)*p2;

	int cmp = (int)state1 - (int)state2;
	if(cmp != 0) return cmp;
		
	switch(state1) {
	case MARSHAL_NEGATIVE_INFINITY:
	case MARSHAL_POSITIVE_INFINITY:
	case MARSHAL_NOT_A_NUMBER:
	case MARSHAL_ZERO:
		++p1; ++p2;
		return 0;
	case MARSHAL_NEGATIVE:
	case MARSHAL_POSITIVE:
		break;
	}

	M_APM_struct mapm1, mapm2;
	p1 += unmarshalMAPMToTemporary(p1, mapm1);
	p2 += unmarshalMAPMToTemporary(p2, mapm2);

	return m_apm_compare(&mapm1, &mapm2);
}

}

#endif
