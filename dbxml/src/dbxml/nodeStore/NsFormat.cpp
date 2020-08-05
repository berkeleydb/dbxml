//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "NsNode.hpp"
#include "NsFormat.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"
#include "../DocID.hpp"
#include "../ScopedDbt.hpp"
// next 3 are for the Db operations put/get/del
#include "../DbWrapper.hpp"
#include "../OperationContext.hpp"
#include "../Cursor.hpp"
#include "../Log.hpp"
#include "../Buffer.hpp"

#define FORMAT_DEBUG 1

#ifdef FORMAT_DEBUG
#include <ostream>
#include <sstream>
using namespace std;
#endif	

/*
 * NsFormat and subclasses
 *
 * The persistent node format is ultimately determined by the
 * marshaling functions.
 *
 * This class is partly static, and partly virtual, allowing
 * for multiple protocol versions/formats using the same basic
 * record layout.  Persistence models that don't fit a basic
 * node record style will probably not share this code.
 * 
 * NOTES: Namespace prefixes, uri indexes, and
 * type information are all integer indexes into
 * the maps that map the integer to a specific string.
 *
 * All string values are encoded
 * in UTF-8, and length fields are in bytes.
 * Integers are encoded in a variable-length format (see
 * comments in NsFormat.cpp on the format)
 *
 * All node storage documents are kept in a single DB btree
 * database.  The database allows duplicates, and keeps them
 * sorted.  Each individual document is a set of duplicate
 * keys, where the key is the integer document id.  The
 * duplicates are sorted by their values.  The first several
 * bytes of each node are used for sorting.  They are:
 *   protocol version byte, node id string (null-terminated).
 * There is a custom comparison function registered that knows
 * how to compare node ids.  The protocol version is ignored
 * for sorting purposes, as it's the same for each node.
 *
 *
 * Format Descriptions
 *
 * Protocol Version 1
 *
 * NOTE: implementation of unmarshal for version 1 is in
 * NsUpgrade.cpp.  Marshaling is no longer required.
 *
 * In protocol version 1, each document has 1-2 metadata
 * nodes (ids '1' and '2') that keep document metadata and
 * (optional) namespace/type information.
 * The first "regular" node of a document (id 'A') is the
 * "document" node, but looks like any regular node.
 *
 * This is the format of nodes in protocol version 1:
 *
 * <header information>
 *    Protocol version -- one byte (NS_PROTOCOL_VERSION)
 *    Node id -- variable length, null-terminated id
 *    Unmarshal Size -- integer size indicating how much space
 *         is needed to unmarshal this node
 *    Flags -- integer flags (NS_*, from NsNode.hpp)
 *    Level -- integer tree level of node (doc node is 0)
 *    Parent id -- node id of parent (except for document node)
 *    Node name (except for document node)
 *      optional integer prefix
 *      optional integer URI index
 *      optional integer type
 *      text name
 * <end header information>
 * <optional information>
 *    If has child nodes (elements),
 *      Integer number of child nodes
 *      For each child:
 *        If there are text children, an integer for each of the element
 *          children, representing the child's text index.
 *        A node id for the child
 *    If has attributes:
 *      Integer number of attributes
 *      For each attribute:
 *        optional integer prefix
 *        optional integer type
 *        name/value byte array (name0value0)
 *
 *    If has text nodes:
 *      Integer number of text nodes
 *      For each text node:
 *        integer type of text node (enum NS_TEXTTYPES)
 *        text bytes (PIs may have embedded nulls)
 *
 * This is the format of node id '1' -- document metadata:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '1', 0
 *  Flags -- integer
 *  XML decl -- integer indicating xml 1.0 or 1.1
 *  Optional encoding string (if specified)
 *
 * The optional namespace/type info, id '2', is of the format:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '2', 0
 *  Num Uris -- integer number of URIs
 *  For each URI, null-terminated uri string
 *  Num prefixes -- integer number of prefixes in map
 *  For each prefix:
 *    integer index into uri array
 *    null-terminated prefix string
 *
 */
/*
 * This is the format of nodes in protocol version 2:
 *
 * NOTE:
 *    o uri and type indexes reference into per-container lists
 *    o prefix map is kept with the document, and maps from an integer
 *    to a string, plus uri index.
 *    o Node ID now goes in the database key
 *
 * <header information> (same as version 1)
 *    Protocol version -- one byte (NS_PROTOCOL_VERSION)
 *    Flags -- integer flags (NS_*, from NsNode.hpp)
 *    If has attrs, integer number of attrs
 *    If has text, integer number of text nodes
 *    Last descendant nid
 *    Level -- integer tree level of node (doc node is 0)
 *    Parent id -- node id of parent (except for document node)
 *    Node name (except for document node)
 *      optional integer prefix
 *      optional integer URI index
 *      optional integer type
 *      text name
 * <end header information>
 * <optional information>
 *   If has sibling element(s)
 *     nextElem nid
 *   If has text:
 *     Integer skip size
 *     Integer num child text nodes (last)
 *     For each text node:
 *       type, bytes
 *       If type is NS_EXTERNAL, bytes is elsewhere (TBD)
 *     leading (sibling) text
 *     child text
 *   If has attrs:
 *     Integer skip size for attr list
 *     For each attribute:
 *        optional integer prefix
 *        optional integer type
 *        name/value byte array (name0value0)
 *   If has sibling element(s)
 *     prevElem nid
 *   If has child elements
 *     last child nid
 */
using namespace DbXml;

namespace DbXml {
	
int _dumpFormatNodes = 0;
#ifdef FORMAT_DEBUG
void dumpNode(NsNode *node);
#endif	

class NsFormat2: public NsFormat {
public:
	virtual size_t marshalNodeKey(
		const DocID &did, const NsNid &nid,
		unsigned char *buf, bool count) const;
	virtual void unmarshalNodeKey(
		DocID &did, NsFullNid *nid,
		unsigned char *buf, bool copyStrings) const;

	virtual size_t marshalNodeData(
		const NsNode *node,
		unsigned char *buf, bool count) const;
	virtual  NsNode *unmarshalNodeData(
		unsigned char *buf, bool adoptBuffer, nodeRecordSizes *sizes = 0) const;
};

// NOTE: format 1 is not implemented here.  See NsUpgrade.*	
NsFormat2 f2;	
NsFormat *NsFormat::formats[NS_NUM_PROTOCOLS] = {0, &f2};
};

XERCES_CPP_NAMESPACE_USE

/*
 * Marshaling/unmarshaling related functions
 */

// NS_UNMARSH_MIN* used on unmarshal side only

#define NS_UNMARSH_MIN_2 sizeof(NsNode) + \
	sizeof(nsTextList_t) +		  \
	sizeof(nsAttrList_t) +		  \
	sizeof(nsNav_t)

/* t_len is in UTF-8 bytes */
#define _nsTextSize(t) ((t).t_len + 1) /* include null */

/*
 * node put/get/delete
 */
#define STACK_BUF_SIZE 512

class NodeMarshalBuffer {
public:
	NodeMarshalBuffer(const NsNode *node, size_t nodeSize = 0)
	{
		const NsFormat &fmt = *node->getFormat();

		data.data = stackBuf;
		if (nodeSize)
			data.size = (uint32_t)nodeSize;
		else
			data.size = (uint32_t)
				fmt.marshalNodeData(node, NULL, true);
		
		if(data.size > STACK_BUF_SIZE) {
			data.data = malloc(data.size);
			if(!data.data)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					"NodeMarshalBuffer::NodeMarshalBuffer",
					__FILE__, __LINE__);
		}
		fmt.marshalNodeData(node, (unsigned char*)data.data, false);
	}

	~NodeMarshalBuffer()
	{
		if(data.size > STACK_BUF_SIZE)
			free(data.data);
	}

	DbXml::DbXmlDbt data;
	unsigned int stackBuf[STACK_BUF_SIZE];
};

void NsFormat::logNodeOperation(DbWrapper &db, const DocID &did,
				const NsNode *node, const NsNid &nid,
				const DbXmlDbt *data, const char *op, int err)
{
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		if (err != 0)
			oss << "Failed node op: " << op << ", err " << err;
		else
			oss << op;
		oss << ", did:nid: " << did.asString() << ":";
		nid.displayNid(oss);
		if (node) {
			oss << ", node: ";
			node->displayNode(oss);
		} else if (data) {
			oss << ", data: ";
			Buffer b((const xmlbyte_t*)data->data, data->size, true);
			oss << b.asString();
		}
		// TBD: get container name (from caller?)
		std::string dbname = db.getDatabaseName();
		if (!dbname.size())
			dbname = "temp";
		Log::log(db.getEnvironment(), Log::C_NODESTORE, Log::L_DEBUG,
			 dbname.c_str(),
			 oss.str().c_str());
	}
}

int NsFormat::putNodeRecord(DbWrapper &db, OperationContext &context,
			    const DocID &did, const NsNid &nid,
			    const DbXmlDbt *data)
{
	marshalNodeKey(did, nid, context.key());
	int ret = 0;
	try {
		// use bulk put in preference to a write cursor because it
		// appears to be faster (write cursor will only be set for
		// cache databases)
		
		// don't allocate if not already done
		BulkPut *bp = context.getBulkPut(false);
		if (bp) {
			bp->setDbWrapper(&db);
			if (bp->store(context.txn(), &context.key(),
				      const_cast<DbXmlDbt*>(data)))
				return 0;
			// above will fail if node is too large for bulk buffer
		}	
		
		Cursor *cursor = db.getWriteCursor();
		if (cursor)
			ret = cursor->put(context.key(),
					  const_cast<DbXmlDbt&>(*data), DB_KEYFIRST);
		else {
			ret = db.put(context.txn(), &context.key(),
				     const_cast<DbXmlDbt*>(data), 0);
		}
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR)
			ret = xe.getDbErrno();
		else
			throw; // re-throw
	}
	return ret;
}

int NsFormat::putNodeRecord(DbWrapper &db, OperationContext &context,
			    const DocID &id, const NsNode *node, bool add, size_t nodeSize)
{
	NodeMarshalBuffer nmb(node, nodeSize);
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
		logNodeOperation(db, id,
				 node,
				 const_cast<NsNode*>(node)->getNid(),
				 NULL, // don't pass data
				 (add ? "add" : "updating"));
	return putNodeRecord(db, context, id, const_cast<NsNode*>(node)->getNid(),
			     &nmb.data);
}

int NsFormat::delNodeRecord(DbWrapper &db, OperationContext &context,
			    const DocID &did, const NsNid &nid)
{
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
		logNodeOperation(db, did, 0, nid, 0, "deleting");

	marshalNodeKey(did, nid, context.key());
	int ret = 0;
	try {
		ret = db.del(context.txn(), &context.key(), 0);
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR)
			ret = xe.getDbErrno();
		else
			throw; // re-throw
	}
	return ret;
}

int NsFormat::getNodeRecord(DbWrapper &db, OperationContext &context,
			    const DocID &did, const NsNid &nid, DbXmlDbt *data,
			    u_int32_t flags)
{
	marshalNodeKey(did, nid, context.key());
	int ret = 0;
	try {
		Cursor *cursor = db.getReadCursor();
		if (cursor)
			ret = cursor->get(context.key(), *data, DB_SET);
		else
			ret = db.get(context.txn(), &context.key(), data, flags);
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR)
			ret = xe.getDbErrno();
		else
			throw; // re-throw
	}
	return ret;
}

int NsFormat::getNextNodeRecord(DbWrapper &db, OperationContext &context,
				const DocID &did, const NsNid &nid, DbXmlDbt *data,
				u_int32_t flags)
{
	Cursor cursor(db, context.txn(), CURSOR_READ, 0);
	if(cursor.error() != 0) return cursor.error();

	// Marshal a key that comes directly after
	// the given node ID
	marshalNextNodeKey(did, nid, context.key());

	// Find the next node record
	int ret = 0;
	try {
		ret = cursor.get(context.key(), *data, DB_SET_RANGE);
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR)
			ret = xe.getDbErrno();
		else
			throw; // re-throw
	}
	cursor.close(); // avoid close in dtor
	return ret;
}

int NsFormat::deleteAllNodes(DbWrapper &db, OperationContext &context,
			     const DocID &did)
{
#if DBVER >= 48
	Cursor cursor(db, context.txn(), CURSOR_WRITE, "bulkdel", DB_CURSOR_BULK);
#else
	Cursor cursor(db, context.txn(), CURSOR_WRITE, 0);
#endif
	if(cursor.error() != 0) return cursor.error();
	int err = 0;
	try {
		DbtOut data;
		data.set_flags(DB_DBT_PARTIAL); // only want keys
		
		marshalNodeKey(did, NsNid(), context.key());
		err = cursor.get(context.key(), data, DB_SET_RANGE);
		
		DocID tmp;
		while (err == 0) {
			tmp.setThisFromDbt(context.key());
			if(tmp != did) {
				break;
			}
			err = cursor.del(0);
			if(err == 0)
				err = cursor.get(context.key(), data, DB_NEXT);
		}
		
		if(err == DB_NOTFOUND)
			err = 0;
			
	} catch (...) {
		// deadlock probably
		cursor.close();
		throw;
	}
	cursor.close(); // avoid close in dtor
	return err;
}

// fetch an unmarshaled node
//static
NsNode *NsFormat::fetchNode(const NsNid &nid,
			    const DocID &did,
			    DbWrapper &db,
			    OperationContext &oc,
			    bool forWrite)
{
	DbXmlDbt data;
	data.set_flags(DB_DBT_MALLOC);
	u_int32_t flags = ((forWrite && oc.txn() && db.isTransacted()) ? DB_RMW : 0);
	int err = getNodeRecord(db, oc, did, nid,
				&data, flags);
	if (err)
		throw XmlException(err);
	unsigned char *buf = (unsigned char *)data.data;
	const NsFormat &fmt = NsFormat::getFormat((int)(*buf));
	NsNode *newNode = fmt.unmarshalNodeData(buf,
						true /* adoptBuffer */);
	newNode->getFullNid()->copyNid(nid.getBytes(), nid.getLen());
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
		logNodeOperation(db, did, newNode, nid,
				 NULL, // don't pass data
				 "fetchNode");
	return newNode;
}

/*
 * marshal UTF-8 text, including trailing null.
 * The text has already been transcoded to UTF-8.
 *
 * An advantage of using UTF-8 here is that there is no
 * chance of buffer overflow.
 *
 */
static inline size_t _nsMarshText(unsigned char *ptr, const nsText_t *text)
{
	memcpy(ptr, text->t_chars, text->t_len + 1);
	return (text->t_len + 1);
}

/*
 * marshal an attribute.  Format is:
 *  flags [prefixIndex] [uriIndex] [typeIndex] [unionTypeIndex] name0value0
 * where the various indexes (ints) are based on their related flags fields.
 * unionTypeIndex can only exist if typeIndex is set, also.
 */
static inline size_t _nsMarshAttr(unsigned char *ptr, const nsAttr_t *attr)
{
	// nsText_t field of the attribute name has entire
	// name/value string, contiguous.
	unsigned char *tptr = ptr;
	uint32_t aflags = attr->a_flags & NS_ATTR_MASK;
	tptr += NsFormat::marshalInt(tptr, aflags);
	if (aflags & NS_ATTR_PREFIX)
		tptr += NsFormat::marshalInt(tptr, attr->a_name.n_prefix);
	if (aflags & NS_ATTR_URI)
		tptr += NsFormat::marshalInt(tptr, attr->a_uri);
#ifdef NS_USE_SCHEMATYPES	
	if (aflags & NS_ATTR_TYPE) {
		tptr += NsFormat::marshalInt(tptr, attr->a_name.n_type);
		if (aflags & NS_ATTR_UNION_TYPE)
			tptr += NsFormat::marshalInt(tptr, attr->a_name.n_utype);
	}
#endif
	tptr += _nsMarshText(tptr, &attr->a_name.n_text);

	return (tptr-ptr);
}

/*
 * allocate "space" for xmlbyte_t from end of array.
 * endPP points to last *available* memory.
 */
static inline xmlbyte_t *_allocChars(xmlbyte_t **endPP, size_t len) {
	*endPP -= (len);
	xmlbyte_t *ret = *endPP + 1;
	return (xmlbyte_t *) ret;
}

#define _nsAllocChars(dest, endPP, len) (dest) = _allocChars((endPP), (len))

size_t
NsFormat::unmarshalId(const xmlbyte_t *ptr, NsFullNid *id)
{
	size_t len = NsUtil::nsStringLen(ptr) + 1;
	if(len == 1) {
		id->freeNid();
	} else {
		id->copyNid(ptr, (unsigned int)len);
	}
	return len;
}

/*
 * Unmarshal ID.
 * Eventually, this will use space off of endPP, which is
 * why it's passed.
 */
size_t
NsFormat::unmarshalId(const xmlbyte_t *ptr, NsFullNid *id,
		      xmlbyte_t ** endPP, bool copyStrings)
{
	size_t len = NsUtil::nsStringLen(ptr) + 1;
	id->setLen((unsigned int)len, /*alloced*/false);
	if (len > NsFullNid::nidStaticSize()) {
		xmlbyte_t *dest;
		if(copyStrings) {
			dest = (xmlbyte_t*)_allocChars(endPP, len);
			memcpy(dest, ptr, len);
		} else {
			dest = (xmlbyte_t*)ptr;
		}
		id->setPtr(dest);
	} else {
		// id's len must be set for this to work
		memcpy(id->getBytesForCopy(), ptr, len);
	}
	return len;
}

/*
 * Unmarshal text.  Text comes from a null-terminated, UTF-8 string,
 * pointed to by the ptr argument.  Return all characters moved, including
 * the NULL.
 */

//static
size_t
NsFormat::unmarshText(unsigned char *ptr, nsText_t *text,
		      xmlbyte_t **endPP, bool copyStrings)
{
	size_t len = NsUtil::nsStringLen(ptr) + 1;

	if(copyStrings) {
		/* allocate UTF-16 memory */
		_nsAllocChars(text->t_chars, endPP, len);
		memcpy(text->t_chars, ptr, len);
	}
	else {
		text->t_chars = (xmlbyte_t *)ptr;
	}
	text->t_len = len - 1;
	return len;
}

/*
 * unmarshal attribute list
 * ptr points to packed attribute list of the form:
 *   name10value10name20value20...
 *
 */
//static
unsigned char *
NsFormat::unmarshAttrs(unsigned char *ptr, nsAttrList_t *attrs,
		       xmlbyte_t **endPP, bool copyStrings)
{
	for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
		nsAttr_t *attr = &attrs->al_attrs[i];
		nsName_t *name = &attr->a_name;
		ptr += NsFormat::unmarshalInt(ptr,
					    (uint32_t*)&(attr->a_flags));
		if (attr->a_flags & NS_ATTR_PREFIX) {
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)&(name->n_prefix));
		} else {
			name->n_prefix = NS_NOPREFIX;
		}
		if (attr->a_flags & NS_ATTR_URI) {
			// TBD GMF think about using the xmlns uri index, if
			// reliable, to set NS_ATTR_IS_DECL...
			ptr += NsFormat::unmarshalInt(ptr,
					     (uint32_t*)&(attr->a_uri));
		} else
			attr->a_uri = NS_NOURI;
#ifdef NS_USE_SCHEMATYPES		
		if (attr->a_flags & NS_ATTR_TYPE) {
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)&(name->n_type));
			if (attr->a_flags & NS_ATTR_UNION_TYPE)
				ptr += NsFormat::unmarshalInt(
					ptr, (uint32_t*)&(name->n_utype));
		} else
			name->n_type = NS_NOTYPE;
#endif

		size_t name_len = NsUtil::nsStringLen(ptr) + 1; /* add null */
		size_t alen = name_len + NsUtil::nsStringLen(ptr + name_len) + 1;

		if(copyStrings) {
			_nsAllocChars(name->n_text.t_chars, endPP, alen);
			memcpy(name->n_text.t_chars, ptr, alen);
		}
		else {
			name->n_text.t_chars = (xmlbyte_t *)ptr;
		}
		name->n_text.t_len = alen - 1;
		// set value
		attr->a_value = ((xmlbyte_t*)name->n_text.t_chars) + name_len;

		/* al_len includes all nulls */
		attrs->al_len += alen;
		ptr += alen;
	}
	return ptr;
}

/*
 * unmarshal text list
 * ptr points to packed text list of the form:
 *   [type]text10[type]text20...
 * except for PI, which is [type]target0data0
 *
 */
//static
unsigned char *
NsFormat::unmarshTextList(unsigned char *ptr, nsTextList_t *text,
			  xmlbyte_t **endPP, bool copyStrings)
{
	for (uint32_t i = 0; i < text->tl_ntext; i++) {
		nsTextEntry_t *entry = &text->tl_text[i];
		entry->te_type = (uint32_t) *ptr++;
		size_t tlen = NsUtil::nsStringLen(ptr) + 1; /* add null */
		if (nsTextType(entry->te_type) == NS_PINST)
			tlen += NsUtil::nsStringLen(ptr + tlen) + 1;
		if(copyStrings) {
			_nsAllocChars(entry->te_text.t_chars, endPP, tlen);
			memcpy(entry->te_text.t_chars, ptr, tlen);
		}
		else {
			entry->te_text.t_chars = (xmlbyte_t *)ptr;
		}
		entry->te_text.t_len = tlen - 1;
		/* tl_len includes all nulls */
		text->tl_len += tlen;
		ptr += tlen;
	}
	return ptr;
}

//
// Format-dependent functions
//

// static
void
NsFormat::marshalNodeKey(const DocID &did, const NsNid &nid, DbtOut &dbt)
{
	const NsFormat &fmt = NsFormat::getFormat(NS_PROTOCOL_VERSION);
	
	size_t size = fmt.marshalNodeKey(did, nid, 0, /*count*/true);
	dbt.set(0, size);

	fmt.marshalNodeKey(did, nid, (unsigned char*)dbt.data,
		/*count*/false);
}

// static
void
NsFormat::marshalNextNodeKey(const DocID &did, const NsNid &nid, DbtOut &dbt)
{
	const NsFormat &fmt = NsFormat::getFormat(NS_PROTOCOL_VERSION);
	
	size_t size = fmt.marshalNodeKey(did, nid, 0, /*count*/true);
	dbt.set(0, size + 1);

	fmt.marshalNodeKey(did, nid, (unsigned char*)dbt.data,
		/*count*/false);

	// We add an extra 0 at the end of the key - this turns it into an
	// invalid node ID, but that doesn't matter as we just want to use
	// this key with DB_SET_RANGE to get the next entry in the BTree.
	((unsigned char*)dbt.data)[size] = 0;
}

size_t NsFormat2::marshalNodeKey(const DocID &did, const NsNid &nid,
	unsigned char *ptr, bool count) const
{
	size_t size = 0;

	if(count) {
		size = did.marshalSize();
		if(!nid.isNull())
			size += countId(nid);
	} else {
		ptr += did.marshal(ptr);
		if(!nid.isNull())
			ptr += NsFormat::marshalId(ptr, nid);
	}

	return size;
}

void NsFormat2::unmarshalNodeKey(DocID &did, NsFullNid *nid,
	unsigned char *ptr, bool copyStrings) const
{
	ptr += did.unmarshal(ptr);

	if(copyStrings) {
		ptr += unmarshalId(ptr, nid);
	} else {
		ptr += unmarshalId(ptr, nid, 0, copyStrings);
	}
}

//
// NsFormat2 implementation
//
// NB node ID now goes in the database key
// NB first child NID is implied, since it is the next
//    node in the database
// NB next sibling NID is implied by being the next node
//    after the last descendant
//
// NsFormat2 header: unmarshal
// protocol, flags, [nattrs], [ntext], last descendant nid,
//   level, parentNid, [name pfx],[name uri],[name type],
//   name text
//   if document node, no level, parent, name info
// Body:
// if has text:
//    skip size, num child text nodes,
//    leading (sibling) text nodes: <type, bytes>+
//    child text nodes: <type, bytes>+
// if has attr(s)
//    skip size, <[pfx],[type],name0value0>+
// if has previous sibling
//    prevElem nid
// if has child elems
//    last child nid
//
size_t
NsFormat2::marshalNodeData(const NsNode *nodep, unsigned char *buf, bool count) const
{
	// accessors to NsNode are non-const, but safe
	NsNode *node = (NsNode *)nodep;
	unsigned char *ptr = buf;
	size_t size = 0;
	// nattrs and ntext go in header to better enable
	// unmarshal to estimate memory requirements
	int32_t nattrs = 0;
	int32_t ntext = 0;
	if (node->hasAttributes())
		nattrs = node->numAttrs();
	if (node->hasText())
		ntext = node->getNumText();

	// set last_is_last_desc in flags
	// before counting flags.  These flags are *only* used during
	// marshal/unmarshal, and are not part of normal state
	uint32_t tflags = 0;
	if (node->hasChildElem()) {
		if (*node->getLastChildNid() == *node->getLastDescendantNid())
			tflags = NS_LAST_IS_LAST_DESC;
	}

	// add in "normal" flags
	tflags |= node->getFlags();
	if (count) {
		size += 1; // Protocol version
		size += countInt(tflags & NS_DBFLAGMASK);
		if (nattrs)
			size += countInt(nattrs);
		if (ntext)
			size += countInt(ntext);
		if (node->hasChildElem())
			size += countId(node->getLastDescendantNid());
		if (!(tflags & NS_ISDOCUMENT)) {
			// document node has no parent, and no name, and level is 0
			size += countInt(node->getLevel());
			size += countId(node->getParentNid());
			size += _nsTextSize(*node->getNameText());
			if (tflags & NS_NAMEPREFIX)
				size += countInt(node->namePrefix());
			if (tflags & NS_HASURI)
				size += countInt(node->uriIndex());
#ifdef NS_USE_SCHEMATYPES			
			if (tflags & NS_NAMETYPE) {
				size += countInt(node->typeIndex());
				if (tflags & NS_UNIONTYPE)
					size += countInt(node->utypeIndex());
			}
#endif
		}
	} else {
		*ptr++ = NS_PROTOCOL_VERSION;
		// Remove flags that shouldn't go into the db
		ptr += marshalInt(ptr, tflags & NS_DBFLAGMASK);
		// number of attribute and text nodes, up front, to
		// aid in unmarshal
		if (nattrs)
			ptr += marshalInt(ptr, nattrs);
		if (ntext)
			ptr += marshalInt(ptr, ntext);
		if (node->hasChildElem())
			ptr += marshalId(ptr, node->getLastDescendantNid());
		if (!(tflags & NS_ISDOCUMENT)) {
			ptr += marshalInt(ptr, node->getLevel());
			ptr += marshalId(ptr, node->getParentNid());
			if (tflags & NS_NAMEPREFIX)
				ptr += marshalInt(ptr, node->namePrefix());
			if (tflags & NS_HASURI)
				ptr += marshalInt(ptr, node->uriIndex());
#ifdef NS_USE_SCHEMATYPES			
			if (tflags & NS_NAMETYPE) {
				ptr += marshalInt(ptr, node->typeIndex());
				if (tflags & NS_UNIONTYPE)
					ptr += marshalInt(
						ptr, node->utypeIndex());
			}
#endif
			ptr += _nsMarshText(ptr, node->getNameText());
		}
	}
	//
	// optional information, in order of:
	//  text (leading and child)
	//  attributes
	//  previous sibling, last child
	//
	// text: ntext is in header, acquired above
	// <skipSize, [numChildText] (textType, text, 0)+>
	//
	if (ntext) {
		nsTextList_t *list = node->getTextList();
		size_t msize = list->tl_len +
			list->tl_ntext; // type bytes
		if (count) {
			// ntext already counted
			size += msize;
			size += countInt((uint32_t)msize);
			if (node->hasTextChild()) {
				DBXML_ASSERT(list->tl_nchild);
				size += countInt(list->tl_nchild);
			}
		} else {
			ptr += marshalInt(ptr, (uint32_t)msize);
			if (node->hasTextChild()) {
				DBXML_ASSERT(list->tl_nchild);
				ptr += marshalInt(ptr, list->tl_nchild);
			}
			// skip delete text nodes
			int index = 0;
			int i = 0;
			while (i < ntext) {
				if (list->tl_text[index].te_type == NS_DELETEDTEXT) {
					index++;
					continue;
				}
				*ptr++ = (unsigned char)list->tl_text[index].te_type;
				ptr += _nsMarshText(ptr,
						    &(list->tl_text[index].te_text));
				i++; index++;
			}
		}
	}

	// attributes: nattrs is in header
	// < skipSize, (nm0val0)+ >
	if (nattrs) {
		nsAttrList_t *attrs = node->getAttrList();
		if (count) {
			size_t msize = attrs->al_len;
			// num attrs already counted
			// need to count optional:
			//  prefixes, uris, and types
			for (int i = 0; i < nattrs; i++) {
				nsAttr_t *attr = &(attrs->al_attrs[i]);
				// if attr value is 0, it's been removed, continue
				if (!attr->a_value) { ++nattrs; continue;}
				msize += 1; // a_flags are ALWAYS 1 byte
				uint32_t aflags = attr->a_flags & NS_ATTR_MASK;
				if (aflags & NS_ATTR_PREFIX)
					msize += countInt(attr->a_name.n_prefix);
				if (aflags & NS_ATTR_URI)
					msize += countInt(attr->a_uri);
#ifdef NS_USE_SCHEMATYPES				
				if (aflags & NS_ATTR_TYPE) {
					msize += countInt(attr->a_name.n_type);
					if (aflags &
					    NS_ATTR_UNION_TYPE)
						msize += countInt(attr->a_name.n_utype);
				}
#endif
			}
			size += msize;
			size += countInt((uint32_t)msize);
			attrs->al_marshalLen = msize;
		} else {
			// TBD: either use skip size or nuke it...
			ptr += marshalInt(ptr, (uint32_t)attrs->al_marshalLen);
			for (int i = 0; i < nattrs; i++) {
				nsAttr_t *attr = &(attrs->al_attrs[i]);
				// if attr value is 0, it's been removed, continue
				if (!attr->a_value) { ++nattrs; continue;}
				ptr += _nsMarshAttr(ptr, attr);
			}
		}
	}

	// previous sibling, last child
	if (count) {
		if (node->hasPrev())
			size += countId(node->getPrevNid());
		if (node->hasChildElem() && !(tflags & NS_LAST_IS_LAST_DESC)) {

			size += countId(node->getLastChildNid());

			DBXML_ASSERT(*node->getLastChildNid() !=
				*node->getLastDescendantNid());
		}
	} else {
		if (node->hasPrev())
			ptr += marshalId(ptr, node->getPrevNid());
		if (node->hasChildElem() && !(tflags & NS_LAST_IS_LAST_DESC)){
			ptr += marshalId(ptr, node->getLastChildNid());
		}
	}

	return size;
}


NsNode *
NsFormat2::unmarshalNodeData(unsigned char *buf,
			     bool adoptBuffer, nodeRecordSizes *sizes) const
{
	size_t allocSize = NS_UNMARSH_MIN_2;
	uint32_t flags, dummy;
	uint32_t nattrs = 0;
	uint32_t ntext = 0;
	NsNode *node;
	char *curP;
	unsigned char *ptr = buf;
	unsigned char *prev = ptr;
	xmlbyte_t *endP;

	if (*ptr++ != NS_PROTOCOL_VERSION)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "protocal mismatch in node",
					 __FILE__, __LINE__);
	//
	// unmarshal size is determined by number of attributes and text nodes,
	// plus fixed overhead.
	// Header is: protocol, flags, [nattrs], [ntext], ...
	// 

	ptr += unmarshalInt(ptr, &flags);
	if (flags & NS_HASATTR) {
		ptr += unmarshalInt(ptr, &nattrs);
		allocSize += nattrs * sizeof(nsAttr_t);
	}
	if (flags & NS_HASTEXT) {
		ptr += unmarshalInt(ptr, &ntext);
		allocSize += ntext * sizeof(nsTextEntry_t);
	}

	// Allocate memory for the node
	curP = (char *)NsUtil::allocate(allocSize);
	memset(curP, 0, allocSize);

	node = (NsNode*)curP;
	node->setFormat(this);
	*node->getFlagsPtr() = flags & ~(NS_LAST_IS_LAST_DESC);

	if(adoptBuffer) node->setMemory(buf);

	endP = (xmlbyte_t *) (curP + allocSize - 1);
	curP = (char *) (node + 1);

	// unmarshal the rest of the header
	if (node->hasChildElem())
		ptr += unmarshalId(ptr, node->getLastDescendantNidPtr(),
			&endP, /*copyStrings*/false);

	if(!(node->checkFlag(NS_ISDOCUMENT))) {
		ptr += NsFormat::unmarshalInt(ptr, node->getLevelPtr());
		nsName_t *nm = node->getName();
		ptr += NsFormat::unmarshalId(ptr, node->getParentNid(),
					   0, /*copyStrings*/false);

		if (node->checkFlag(NS_NAMEPREFIX))
			ptr += NsFormat::unmarshalInt(ptr,
		            (uint32_t*)&(nm->n_prefix));
		else
			nm->n_prefix = NS_NOPREFIX;
		if (node->checkFlag(NS_HASURI))
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)node->uriIndexPtr());
		else
			*(node->uriIndexPtr()) = 0;
#ifdef NS_USE_SCHEMATYPES		
		if (node->checkFlag(NS_NAMETYPE)) {
			ptr += NsFormat::unmarshalInt(ptr,
		            (uint32_t*)&(nm->n_type));
			if (node->checkFlag(NS_UNIONTYPE))
				ptr += NsFormat::unmarshalInt(
					ptr, (uint32_t*)&(nm->n_utype));
		} else
			nm->n_type = NS_NOTYPE;
#endif
		prev = ptr;
		ptr += unmarshText(ptr, &(nm->n_text),
				   0, /*copyStrings*/false);
		if(sizes)
			sizes->name += ptr - prev;
	} else
		node->setLevel(0); // document node is level 0

	if(sizes) {
		sizes->header += ptr - buf;
		prev = ptr;
	}

	//
	// TBD: unmarshal on demand -- either that, or drop skip size
	// from text and attrs
	//
	// Body: text, attrs, prev sib, last child

	// text: ntext is in header
	// <skipSize, [numChildText] (textType, text, 0)+>
	if (node->hasText()) {
		DBXML_ASSERT(ntext);
		ptr += unmarshalInt(ptr, &dummy); // not using skip size for now
		nsTextList_t *text = (nsTextList_t *)curP;
		node->setTextList(text);
		text->tl_ntext = ntext; // from header
		if (node->hasTextChild())
			ptr += unmarshalInt(ptr, &(text->tl_nchild));
		text->tl_max = ntext;
		curP = (char *)(text + 1);
		curP += ((ntext - 1) * sizeof(nsTextEntry_t));
		// TBD: handle external text
		ptr = unmarshTextList(ptr, text, &endP, /*copyStrings*/false);
		if(sizes) {
			sizes->text += ptr - prev;
			prev = ptr;
		}
	}

	// attributes: nattrs is in header
	// < skipSize, (nm0val0)+ >
	if (node->hasAttributes()) {
		DBXML_ASSERT(nattrs);
		ptr += unmarshalInt(ptr, &dummy); // not using skip size for now
		nsAttrList_t *attrs = (nsAttrList_t *)curP;
		node->setAttrList(attrs);
		attrs->al_nattrs = nattrs;
		attrs->al_max = nattrs;
		curP = (char *)(attrs + 1);
		/* add nattrs-1 worth of nsText_t */
		curP += ((nattrs - 1) * sizeof(nsAttr_t));
		/* unmarshal the attributes */
		ptr = unmarshAttrs(ptr, attrs, &endP, /*copyStrings*/false);
		if(sizes) {
			sizes->attributes += ptr - prev;
			prev = ptr;
		}
	}

	if (node->needsNav()) {
		nsNav_t *nav = (nsNav_t *)curP;
		node->setNav(nav);
		curP += sizeof(nsNav_t);
	}

	// previous sibling
	if (node->hasPrev())
		ptr += unmarshalId(ptr, node->getPrevNid(),
				   &endP, /*copyStrings*/false);

	// last child
	if (node->hasChildElem()) {
		if (flags & NS_LAST_IS_LAST_DESC) {
			node->getLastChildNid()->copyNid(
				node->getLastDescendantNid());
		} else {
			ptr += unmarshalId(ptr, node->getLastChildNid(),
				&endP, /*copyStrings*/false);
		}
	}
	if(sizes) {
		sizes->navigation += ptr - prev;
		prev = ptr;
	}

	/* debug */
	if (curP >= (char *)endP) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unmarshalNode overlap",
					 __FILE__, __LINE__);
	}
#ifdef FORMAT_DEBUG
	if (_dumpFormatNodes && node)
		dumpNode(node);
#endif	
	return node;
}

/**
 * 64-bit integer marshaling
 */
int NsFormat::marshalInt64(xmlbyte_t *buf, uint64_t i)
{
	if (i <= NSINT_MAX1) {
		// no swapping for one byte value
		buf[0] = (unsigned char) i;
		return 1;
	} else {
		uint64_t tmp = i;
		if (!Globals::isBigendian_)
			M_64_SWAP(tmp);
		unsigned char *p = (unsigned char *) & tmp;
		if (i <= NSINT_MAX2) {
			buf[0] = (p[6] | NSINT_2BYTE_VAL);
			buf[1] = p[7];
			return 2;
		} else if (i <= NSINT_MAX3) {
			buf[0] = (p[5] | NSINT_3BYTE_VAL);
			buf[1] = p[6];
			buf[2] = p[7];
			return 3;
		} else  if (i <= NSINT_MAX4) {
			buf[0] = (p[4] | NSINT_4BYTE_VAL);
			buf[1] = p[5];
			buf[2] = p[6];
			buf[3] = p[7];
			return 4;
		} else if (i <= NSINT_MAX5) {
			buf[0] = (p[3] | NSINT_5BYTE_VAL);
			buf[1] = p[4];
			buf[2] = p[5];
			buf[3] = p[6];
			buf[4] = p[7];
			return 5;
		} else if (i <= NSINT_MAX6) {
			buf[0] = NSINT_6BYTE_VAL;
			buf[1] = p[3];
			buf[2] = p[4];
			buf[3] = p[5];
			buf[4] = p[6];
			buf[5] = p[7];
			return 6;
		} else {
			/* 64-bit */
			buf[0] = NSINT_9BYTE_VAL;
			buf[1] = p[0];
			buf[2] = p[1];
			buf[3] = p[2];
			buf[4] = p[3];
			buf[5] = p[4];
			buf[6] = p[5];
			buf[7] = p[6];
			buf[8] = p[7];
			return 9;
		}
	}
}

int NsFormat::unmarshalInt64(const xmlbyte_t *buf, uint64_t *i)
{
	int len = 0;
	uint64_t tmp = 0;
	unsigned char *p = (unsigned char *) &tmp;
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		// no swapping for one byte value
		*i = buf[0];
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		p[6] = (c & ~NSINT_2BIT_MASK);
		p[7] = buf[1];
		len = 2;
		goto out;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		p[5] = (c & ~NSINT_3BIT_MASK);
		p[6] = buf[1];
		p[7] = buf[2];
		len = 3;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_4BYTE_VAL) {
		p[4] = (c & ~NSINT_5BIT_MASK);
		p[5] = buf[1];
		p[6] = buf[2];
		p[7] = buf[3];
		len = 4;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == NSINT_5BYTE_VAL) {
		p[3] = (c & ~NSINT_5BIT_MASK);
		p[4] = buf[1];
		p[5] = buf[2];
		p[6] = buf[3];
		p[7] = buf[4];
		len = 5;
		goto out;
	} else if ((c & NSINT_8BIT_MASK) == NSINT_6BYTE_VAL) {
		p[3] = buf[1];
		p[4] = buf[2];
		p[5] = buf[3];
		p[6] = buf[4];
		p[7] = buf[5];
		len = 6;
		goto out;
	} else if ((c & NSINT_8BIT_MASK) == NSINT_9BYTE_VAL) {
		p[0] = buf[1];
		p[1] = buf[2];
		p[2] = buf[3];
		p[3] = buf[4];
		p[4] = buf[5];
		p[5] = buf[6];
		p[6] = buf[7];
		p[7] = buf[8];
		len = 9;
		goto out;
	}
 out:
	*i = tmp;
	if (!Globals::isBigendian_)
		M_64_SWAP(*i);
	return len;
}

// static
size_t NsFormat::getNodeDataSize(const NsNode *node)
{
	const NsFormat &fmt =
		NsFormat::getFormat(NS_PROTOCOL_VERSION);
	return fmt.marshalNodeData(node, 0, /*count*/true);
}

#ifdef FORMAT_DEBUG
namespace DbXml {

void dumpFlags(uint32_t flags, ostringstream &out)
{
	out << "\n\tFlags: (" << hex << flags << ")" <<
		(flags & NS_HASCHILD ? "NS_HASCHILD," : "") <<
		(flags & NS_HASTEXT ? "NS_HASTEXT," : "") <<
		(flags & NS_HASTEXTCHILD ? "NS_HASTEXTCHILD," : "") <<
		(flags & NS_HASNEXT ? "NS_HASNEXT," : "") <<
		(flags & NS_HASPREV ? "NS_HASPREV," : "");
	// NS_LAST_IS_LAST_DESC will have been cleared at this point
}
	
void dumpNode(NsNode *node)
{
	ostringstream out;
	if (node->isDoc()) {
		out << "Node: Document";
	} else {
		const char *name = (const char *)node->getNameChars();
		out << "Node:\n\tName, uri, level: " << (name ? name : "no name")
		    << "," << node->uriIndex() << ", " << node->getLevel();
		out << "\n\tparent nid: ";
		node->getParentNid()->displayNid(out);
	}
	/* Nid isn't set
	   node->getNid().displayNid(out);
	*/
	out << "\n\tlast child, last descendant: ";
	if (node->hasChildElem()) {
		node->getLastChildNid()->displayNid(out);
		out << ", ";
		node->getLastDescendantNid()->displayNid(out);
	} else
		out << "null";

	out << "\n\tprev: ";
	if (node->hasPrev())
		node->getPrevNid()->displayNid(out);
	else
		out << "null";
	if (node->hasAttributes())
		out << "\n\thas attributes -- " << node->numAttrs();
	else
		out << "\n\tno attributes";
	dumpFlags(node->getFlags(), out);
	out << "\n\t";

	std::cout << out.str() << endl;
}
}
#endif

