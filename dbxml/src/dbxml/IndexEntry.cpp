//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "nodeStore/NsNode.hpp"
#include "IndexEntry.hpp"
#include "ScopedDbt.hpp"
#include "SyntaxDatabase.hpp"
#include "Container.hpp"
#include "Globals.hpp"
#include "Document.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"
#include "nodeStore/NsFormat.hpp"
#include "nodeStore/NsDom.hpp"

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE

IndexEntry::IndexEntry()
{
	memset(this, 0, sizeof(IndexEntry));
}

IndexEntry::~IndexEntry()
{
	reset();
}

void IndexEntry::reset()
{
	// use free because this memory came from a Dbt
	// using DB_DBT_MALLOC
	if (memory_)
		::free(memory_);
	last_descendant_internal_.freeNid();
	memset(this, 0, sizeof(IndexEntry));
}

static void logIndexError(const Document &document, const char *msg)
{
	ScopedContainer sc(const_cast<Document&>(document).getManager(),
			   document.getContainerID(), true);
	sc.get()->log(Log::C_QUERY, Log::L_ERROR,
		      msg);
	throw XmlException(XmlException::INTERNAL_ERROR, msg);
}

NsDomNode *IndexEntry::fetchNode(const Document *document,
				 Transaction *txn,
				 DbXmlConfiguration *conf) const
{
	// Find the ImpliedSchemaNodes for the document, so we can
	// perform document projection
	ISNVector isns;
	if(conf != 0)
		conf->getImpliedSchemaNodes(document, isns);

	if(!isSpecified(NODE_ID)) {
		// Return the document
		return document->getContentAsNsDom(
			&isns, txn);
	}

	NsDomElement *element = document->getElement(getNodeID(), &isns);
	if(element == 0) {
		ostringstream s;
		s << "Invalid node index values found during query (element 0x";
		getNodeID().displayNid(s);
		s << " not found)";
		logIndexError(*document, s.str().c_str());
	}

	if(isSpecified(ATTRIBUTE_INDEX)) {
		NsDomAttr *attr = element->getNsAttr(index_);
		if(attr == 0) {
			logIndexError(*document,
				      "Invalid index values found during query (attribute not found)");
		}
		delete element;
		return attr;
	}
	if(isSpecified(TEXT_INDEX) || isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX)) {
		NsDomText *text = element->getNsTextNode(index_);
		if(text == 0) {
			logIndexError(*document,
				      "Invalid index values found during query (text not found)");
		}
		delete element;
		return text;
	}

	return element;
}

const NsNid IndexEntry::getNodeID() const
{
	return node_id_;
}

void IndexEntry::setNodeID(const NsNid &nid)
{
	node_id_ = nid;
}

u_int32_t IndexEntry::getNodeLevel() const
{
	return node_level_;
}

void IndexEntry::setNodeLevel(u_int32_t level)
{
	node_level_ = level;
}

void IndexEntry::setIndexNodeInfo(IndexNodeInfo *ninfo)
{
	if (!ninfo)
		hasNinfo_ = false;
	else {
		hasNinfo_ = true;
		node_level_ = ninfo->getNodeLevel();
		node_id_ = ninfo->getNodeID();
		NsNode *node = const_cast<NsNode*>(ninfo->getNode());
		if (node && node->getLastDescendantNid())
			setLastDescendant(NsNid(node->getLastDescendantNidPtr()));
	}
	
}

void IndexEntry::setLastDescendant(const NsNid &last_descendant)
{
	// in this path, copy the memory because during indexing,
	// the last descendant will be free'd before the "current" node
	if (last_descendant.isNull()) {
		last_descendant_internal_.freeNid();
		last_descendant_ = 0;
	} else {
		last_descendant_internal_.copyNid(last_descendant.getBytes(),
						  last_descendant.getLen());
		last_descendant_ = last_descendant_internal_.getBytes();
	}
}

string IndexEntry::getNodeHandle() const
{
	int size = marshal(0, /*count*/true);
	size += 1; // The checksum

	Buffer rawBuf(0, size);
	size_t offset;
	rawBuf.reserve(offset, size);

	xmlbyte_t *ptr = (xmlbyte_t*)rawBuf.getBuffer(offset);
	ptr += marshal(ptr, /*count*/false);

	// Calculate the checksum
	xmlbyte_t chk = 0;
	for(xmlbyte_t *cp = (xmlbyte_t*)rawBuf.getBuffer(offset);
	    cp < ptr; ++cp) chk += *cp;
	*ptr++ = (chk & 0xFF); // Checksum byte

	Buffer base64Buf(0, size * 2);
	NsUtil::encodeBase64Binary(&base64Buf, (char*)rawBuf.getBuffer(offset), size);

	return string((char*)base64Buf.getBuffer(), base64Buf.getOccupancy());
}

void IndexEntry::setFromNodeHandle(Buffer &buffer, const std::string &str)
{
	NsUtil::decodeBase64Binary(&buffer, str.c_str(), str.length());

	const xmlbyte_t *ptr = (const xmlbyte_t*)buffer.getBuffer();

	// Calculate the checksum
	unsigned int chk = 0;
	const xmlbyte_t *end = ptr + buffer.getOccupancy() - 1;
	for(const xmlbyte_t *cp = ptr; cp < end; ++cp) chk += *cp;

	if(*end != (chk & 0xFF)) {
		throw XmlException(XmlException::INVALID_VALUE,
			"The node handle has a bad checksum");
	}

	unmarshal(ptr);
}

void IndexEntry::setDbtFromThis(DbtOut &dbt) const
{
	int count = marshal(0, /*count*/true);
	dbt.set(0, count);
	marshal((xmlbyte_t*)dbt.data, /*count*/false);
}

void IndexEntry::setThisFromDbt(const DbXmlDbt &dbt)
{
	unmarshal((const xmlbyte_t *)dbt.data);
}

void IndexEntry::marshalLookupFormat(const DocID &did, const NsNid &nid, DbtOut &dbt)
{
	if(nid.isDocRootNid()) {
		int size = 1; // For the prefix byte
		size += did.marshalSize();
		size += 1; // For the NODE_ID null

		dbt.set(0, size);

		xmlbyte_t *ptr = (xmlbyte_t*)dbt.data;

		*ptr++ = (xmlbyte_t)D_FORMAT;
		ptr += did.marshal(ptr);
		*ptr++ = 0;
	} else {
		int size = 1; // For the prefix byte
		size += did.marshalSize();
		size += NsFormat::countId(nid);

		dbt.set(0, size);

		xmlbyte_t *ptr = (xmlbyte_t*)dbt.data;

		*ptr++ = (xmlbyte_t)LOOKUP_FORMAT;
		ptr += did.marshal(ptr);
		ptr += NsFormat::marshalId(ptr, nid);
	}
}

int IndexEntry::marshal(xmlbyte_t *ptr, bool count) const
{
	int size = 0;

	if(count) {
		size += 1; // For the format_
		size += docid_.marshalSize();

		if(isSpecified(NODE_ID)) {
			size += NsFormat::countId(getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			size += 1;
		}
#if 0
		// gmf -- these are not used at this time
		if(isSpecified(LAST_CHILD_ID)) {
			const NsFullNid *lastchild = node_->getLastChildNid();
			if(lastchild == 0) size += 1; // For a single null
			else size += NsFormat::countId(lastchild);
		}
		if(isSpecified(PARENT_ID))
			size += NsFormat::countId(node_->getParentNid());
#endif
		if(isSpecified(NODE_LEVEL))
			size += NsFormat::countInt(getNodeLevel());
		if(isSpecified(LAST_DESCENDANT_ID))
			size += NsFormat::countId(last_descendant_);
		if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
			isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX))
			size += NsFormat::countInt(index_);
	} else {
		xmlbyte_t *orig = ptr;

		*ptr++ = (xmlbyte_t)format_;
		ptr += docid_.marshal(ptr);

		if(isSpecified(NODE_ID)) {
			ptr += NsFormat::marshalId(ptr, getNodeID());
		} else {
			// We'll store a null instead, for ease of sorting
			*ptr++ = 0;
		}
#if 0
		// gmf -- these are not used at this time
		if(isSpecified(LAST_CHILD_ID)) {
			const NsFullNid *lastchild = node_->getLastChildNid();
			if(lastchild == 0) *ptr++ = 0; // Store a single null
			else ptr += NsFormat::marshalId(ptr, lastchild);
		}
		if(isSpecified(PARENT_ID))
			ptr += NsFormat::marshalId(ptr, node_->getParentNid());
#endif
		if(isSpecified(NODE_LEVEL))
			ptr += NsFormat::marshalInt(ptr, getNodeLevel());
		if(isSpecified(LAST_DESCENDANT_ID)) {
			if(last_descendant_.isNull()) *ptr++ = 0; // Store a single null
			else ptr += NsFormat::marshalId(ptr, last_descendant_);
		}
		if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
			isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX))
			ptr += NsFormat::marshalInt(ptr, index_);

		size = (int)(ptr - orig);
	}

	return size;
}

int IndexEntry::unmarshal(const xmlbyte_t *ptr)
{
	const xmlbyte_t *orig = ptr;

	format_ = (Format)*ptr++;
	if(format_ >= KNOWN_FORMATS)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unknown format in index entry",
					 __FILE__, __LINE__);
	ptr += docid_.unmarshal(ptr);

	if(isSpecified(NODE_ID)) {
		node_id_ = ptr;
		ptr = NsFormat::skipId(ptr);
	} else {
		// Skip the null that was marshalled
		++ptr;
	}
	if(isSpecified(LAST_CHILD_ID)) {
		// We don't keep the last child id around
		// as it's not used
		ptr = NsFormat::skipId(ptr);
	}
	if(isSpecified(NODE_LEVEL)) {
		ptr += NsFormat::unmarshalInt(ptr, &node_level_);
	}
	if(isSpecified(PARENT_ID)) {
		// We don't keep the parent id around
		// as it's not used
		ptr = NsFormat::skipId(ptr);
	}
	if(isSpecified(LAST_DESCENDANT_ID)) {
		last_descendant_ = ptr;
		ptr = NsFormat::skipId(ptr);
	}
	if(isSpecified(ATTRIBUTE_INDEX) || isSpecified(TEXT_INDEX) ||
		isSpecified(COMMENT_INDEX) || isSpecified(PI_INDEX)) {
		ptr += NsFormat::unmarshalInt(ptr, &index_);
	}

	return (int)(ptr - orig);
}

bool IndexEntry::operator<(const IndexEntry &o) const
{
	if(docid_ != o.docid_)
		return docid_ < o.docid_;

	if(!isSpecified(NODE_ID)) {
		return o.isSpecified(NODE_ID);
	}
	if(!o.isSpecified(NODE_ID)) {
		return false;
	}

	return getNodeID().compareNids(o.getNodeID()) < 0;
}

bool IndexEntry::indexFormats_[IndexEntry::KNOWN_FORMATS][IndexEntry::INFO_MAX] = {
	/*                        NODE_ID,     NODE_LEVEL,   L_DESC_ID     TEXT_INDEX     PI_INDEX */
	/*                             L_CHILD_ID,    PARENT_ID     ATTR_INDEX    CMNT_INDEX       */
	/* D_FORMAT            */ {false, false, false, false, false, false, false, false, false},
	/* DSEL_OLD_FORMAT     */ {true,  true,  true,  false, false, false, false, false, false},
	/* DSEL_FORMAT         */ {true,  false, true,  false, true,  false, false, false, false},
	/* NH_ATTRIBUTE_FORMAT */ {true,  false, false, false, false, true,  false, false, false},
	/* ATTR_OLD_FORMAT2    */ {true,  false, true,  false, false, true,  false, false, false},
	/* ATTRIBUTE_FORMAT    */ {true,  false, true,  false, true,  true,  false, false, false},
	/* NH_ELEMENT_FORMAT   */ {true,  false, false, false, false, false, false, false, false},
	/* NH_TEXT_FORMAT      */ {true,  false, false, false, false, false, true,  false, false},
	/* NH_COMMENT_FORMAT   */ {true,  false, false, false, false, false, false, true,  false},
	/* NH_PI_FORMAT        */ {true,  false, false, false, false, false, false, false, true }
};

std::ostream& DbXml::operator<<(std::ostream& s, const IndexEntry &ie)
{
	s << "did: " << ie.getDocID().asString();
	if(ie.isSpecified(IndexEntry::NODE_ID)) {
		NsNid nid = ie.getNodeID();
		s << ", nid: ";
		nid.displayNid(s);
		if(ie.isSpecified(IndexEntry::NODE_LEVEL)) {
			s << ", level " << ie.getNodeLevel();
		}
		if(ie.isSpecified(IndexEntry::LAST_DESCENDANT_ID)) {
			s << ", lastDesc: ";
			ie.getLastDescendant().displayNid(s);
		}
	}
	return s;
}

