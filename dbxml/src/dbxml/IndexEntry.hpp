//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __INDEXENTRY_HPP
#define	__INDEXENTRY_HPP

#include <iosfwd>

#include "nodeStore/NsTypes.hpp"
#include "nodeStore/NsNid.hpp"
#include "SharedPtr.hpp"
#include "DocID.hpp"

class DynamicContext;

namespace DbXml
{

class DbXmlDbt;
class DbtOut;
class DbXmlNodeImpl;
class Document;
class IndexNodeInfo;
class NsDomNode;
class Buffer;
class Transaction;
	
class IndexEntry
{
public:
	typedef SharedPtr<IndexEntry> Ptr;

	/// Keep less than 1 byte
	enum Format {
		/// Has DOC_ID
		D_FORMAT = 0,
		NH_DOCUMENT_FORMAT = 0,
		/**
		 * Has DOC_ID, NODE_ID, LAST_CHILD_ID, and NODE_LEVEL.
		 * This is now DEPRECATED as it was meant to be like DSEL_FORMAT.
		 */
		DSEL_OLD_FORMAT = 1,
		/// Has DOC_ID, NODE_ID, LAST_DESCENDANT_ID, and NODE_LEVEL
		DSEL_FORMAT = 2,
		/// Used by node handle. Has DOC_ID, NODE_ID, and ATTRIBUTE_INDEX
		NH_ATTRIBUTE_FORMAT = 3,
		ATTR_OLD_FORMAT = 3,
		/**
		 * Has DOC_ID, NODE_ID, NODE_LEVEL and ATTRIBUTE_INDEX
		 * This is now DEPRECATED as it was meant to be like ATTRIBUTE_FORMAT.
		 */
		ATTR_OLD_FORMAT2 = 4,
		/// Has DOC_ID, NODE_ID, LAST_DESCENDANT_ID, NODE_LEVEL and ATTRIBUTE_INDEX
		ATTRIBUTE_FORMAT = 5,

		/// Used by node handle and for marshaling a key to look up. Has DOC_ID and NODE_ID
		NH_ELEMENT_FORMAT = 6,
		LOOKUP_FORMAT = 6,

		/// Used by node handle. Has DOC_ID, NODE_ID and TEXT_INDEX
		NH_TEXT_FORMAT = 7,
		/// Used by node handle. Has DOC_ID, NODE_ID and COMMENT_INDEX
		NH_COMMENT_FORMAT = 8,
		/// Used by node handle. Has DOC_ID, NODE_ID and PI_INDEX
		NH_PI_FORMAT = 9,

		KNOWN_FORMATS = 10
	};

	enum Info {
		NODE_ID = 0,
		LAST_CHILD_ID = 1,
		NODE_LEVEL = 2,
		PARENT_ID = 3,
		LAST_DESCENDANT_ID = 4,
		ATTRIBUTE_INDEX = 5,
		TEXT_INDEX = 6,
		COMMENT_INDEX = 7,
		PI_INDEX = 8,

		INFO_MAX = 9
	};

	IndexEntry();
	~IndexEntry();

	void reset();

	Format getFormat() const { return format_; }
	const DocID &getDocID() const { return docid_; }
	const NsNid getNodeID() const;
	u_int32_t getNodeLevel() const;
	const NsNid getLastDescendant() const { return last_descendant_; }
	uint32_t getIndex() const { return index_; }

	bool hasIndexNodeInfo() { return hasNinfo_; }

	void setFormat(Format format) { format_ = format; }
	void setDocID(const DocID &docid) { docid_ = docid; }
	void setNodeID(const NsNid &nid);
	void setIndexNodeInfo(IndexNodeInfo *ninfo);
	void setLastDescendant(const NsNid &last_descendant);
	void setNodeLevel(u_int32_t level);
	void setIndex(uint32_t index) { index_ = index; }
	
	NsDomNode *fetchNode(const Document *document,
			     Transaction *txn, DbXmlConfiguration *conf = 0) const;

	std::string getNodeHandle() const;
	void setFromNodeHandle(Buffer &buffer, const std::string &str);

	void setDbtFromThis(DbtOut &dbt) const;
	void setThisFromDbt(const DbXmlDbt &dbt);

	/// Returns the marshal size, if count is true
	int marshal(xmlbyte_t *buffer, bool count) const;
	int unmarshal(const xmlbyte_t *ptr);

	bool isSpecified(Info info) const {
		return indexFormats_[format_][info];
	}
	static bool isSpecified(Format format, Info info) {
		return indexFormats_[format][info];
	}

	/// for sorting
	bool operator<(const IndexEntry &o) const;

	/// to store Dbt memory referenced by node ids
	void setMemory(void *mem) {
		DBXML_ASSERT(!memory_);
		memory_ = mem;
	}
	static void marshalLookupFormat(const DocID &did, const NsNid &nid, DbtOut &dbt);

private:
	IndexEntry(const IndexEntry &);
	IndexEntry &operator=(const IndexEntry &);

protected:
	Format format_;
	DocID docid_;

	NsNid node_id_;
	u_int32_t node_level_;
	NsNid last_descendant_;
	NsFullNid last_descendant_internal_; // need to hold onto storage
	uint32_t index_;
	bool hasNinfo_;
	void *memory_;

	/// A truth table for index entry formats
	static bool indexFormats_[KNOWN_FORMATS][INFO_MAX];
};

std::ostream& operator<<(std::ostream& s, const IndexEntry &ie);

}

#endif
