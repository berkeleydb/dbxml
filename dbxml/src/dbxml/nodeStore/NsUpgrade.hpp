
//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSUPGRADE_HPP
#define __DBXMLNSUPGRADE_HPP
	
#include <db.h>
#include "EventReader.hpp"
#include "NsNid.hpp"
#include "NsNode.hpp"
#include "NsFormat.hpp"
#include "NsEvent.hpp"
#include "../ScopedDbt.hpp"
#include "../Cursor.hpp"
#include "../DocID.hpp"

namespace DbXml
{

// forwards
class NsUpgradeReaderBuf;
class NsUpgradeReaderNodeList;
class NsNamespaceInfo;
class OldNsNode;

/**
 * NsUpgradeReader
 *
 * Implements XmlEventReader using the node storage format
 * pre-2.3 (protocol version 1).  It is stripped down, with
 * little/no error handling.  It is only used for upgrading
 * older containers.
 *
 * Usage:
 *   NsUpgradeReader(DbWrapper &database,
 *                   const DocID &docId);
 *
 * The DbWrapper needs to be for the nodestorage database
 * itself.  Avoid dependencies on active objects that
 * may change with versions (e.g. NsDocumentDatabase), and
 * stick with generic objects (DbWrapper, Cursor).
 *
 * Note: DocID is 64-bit as of 2.3, but pre-2.3 values will
 * never be that large, and the format is the same, on-disk
 *
 */
class NsUpgradeReader : public EventReader {
public:
	NsUpgradeReader(DbWrapper &db, const DocID &docId);
	virtual ~NsUpgradeReader();
	virtual void close();

	// the public XmlEventReader interface

	// iterators
	virtual XmlEventType next();

	// naming
	virtual const unsigned char *getNamespaceURI() const;
	// getlocalName returns processing instruction target
	virtual const unsigned char *getLocalName() const;
	virtual const unsigned char *getPrefix() const;
	
	// value -- text values and processing instruction data
	virtual const unsigned char *getValue(size_t &len) const;

	// attribute access (includes namespace attributes)
	virtual int getAttributeCount() const;
	virtual bool isAttributeSpecified(int index) const;
	virtual const unsigned char *getAttributeLocalName(int index) const;
	virtual const unsigned char *getAttributeNamespaceURI(int index) const;
	virtual const unsigned char *getAttributePrefix(int index) const;
	virtual const unsigned char *getAttributeValue(int index) const;

	// start_document only -- see EventReader

	// BDB XML "extensions" in XmlEventReader
	virtual bool needsEntityEscape(int index = 0) const;
	virtual bool isEmptyElement() const;

	// required by EventReader
	IndexNodeInfo *getIndexNodeInfo() const;
	const unsigned char *getVersion() const;
	const unsigned char *getEncoding() const;
	bool standaloneSet() const;
	bool encodingSet() const;
	bool isStandalone() const;

private:
	bool doText();
	bool doElement(bool start);
	void popElement();
	void nextNode(NsUpgradeReaderBuf **bufp, DbXmlDbt &data,
		      xmlbyte_t *startId = 0);
	void releaseNode(NsUpgradeReaderBuf *buf);
	void initDocInfo();

	// name info
	const unsigned char * localName_;
	const unsigned char * prefix_;
	const unsigned char * uri_;
	
	// text event info
	uint32_t textType_;

	// element/attribute info
	OldNsNode *node_;
	bool emptyElement_;
	int nattrs_;

	// data
	DbWrapper &db_;
	const xmlbyte_t *encStr_;
	const xmlbyte_t *standStr_;
	const xmlbyte_t *sniffStr_;
	int32_t xmlDecl_;
	NsNamespaceInfo *nsInfo_;
	DocID id_;
	bool doInit_;
	bool popElement_;
	int entityCount_;

	mutable NsUpgradeReaderNodeList *current_;
	NsUpgradeReaderBuf *currentBuffer_;

	DbtOut docKey_;
        Cursor cursor_;
	u_int32_t cursorFlags_;
	xmlbyte_t startBuf_[5];
	// memory mgmt
	NsUpgradeReaderBuf *reuseList_;
	NsUpgradeReaderBuf *freeList_;
};

//
// Old NsNode format.  It didn't work out to
// lump this in with NsNode.hpp, so keep it entirely
// separate, for easy removal, later
//
enum OLD_NS_FLAGS {
	OLD_NS_HASCHILD =   0x00000001,
	OLD_NS_HASATTR =    0x00000002,
	OLD_NS_HASTEXT =    0x00000004,
	OLD_NS_NAMEPREFIX = 0x00000008,
	OLD_NS_HASURI =     0x00000010,
	OLD_NS_HASNSINFO =  0x00000020,
	OLD_NS_NAMETYPE =   0x00000040,
	OLD_NS_UNIONTYPE =  0x00000080,
	OLD_NS_ISDOCUMENT = 0x00000100
};

typedef struct {
	uint32_t ce_textIndex;
	NsFullNid    ce_id;
} nsChildEntryCompat_t;

typedef struct {
	uint32_t cl_numChild;
	uint32_t cl_maxChild;
	nsChildEntryCompat_t  cl_child[1];
} nsChildListCompat_t;

typedef NsFullNid OldNsNid;

class OldNsNode {
public:
	uint32_t getFlags() const {
		return ond_header.nh_flags;
	}
	
	bool hasChildElem() const {
		return ((ond_header.nh_flags & OLD_NS_HASCHILD) != 0);
	}
	
	bool hasText() const {
		return ((ond_header.nh_flags & OLD_NS_HASTEXT) != 0);
	}
	
	bool hasUri() const {
		return ((ond_header.nh_flags & OLD_NS_HASURI) != 0);
	}

	bool hasAttr() const {
		return ((ond_header.nh_flags & OLD_NS_HASATTR) != 0);
	}

	bool checkFlag(uint32_t flag) const {
		return ((ond_header.nh_flags & flag) != 0);
	}

	uint32_t numAttrs() const {
		if (hasAttr())
			return (ond_attrs->al_nattrs);
		return 0;
	}
	
	void setChildListCompat(nsChildListCompat_t *list) {
		ond_child = list;
	}

	uint32_t *getFlagsPtr() {
		return &(ond_header.nh_flags);
	}

	int32_t *uriIndexPtr() {
		return &(ond_header.nh_uriIndex);
	}
	
	uint32_t *getLevelPtr() {
		return &(ond_level);
	}

	nsName_t *getName() {
		return &(ond_header.nh_name);
	}
	
	OldNsNid *getNid() {
		return &(ond_header.nh_id);
	}
	
	OldNsNid *getParentNid() {
		return &(ond_header.nh_parent);
	}
	
	OldNsNid *getChildNid(uint32_t index) {
		if (hasChildElem()) {
			if (index < ond_child->cl_numChild)
				return &(getChild(index)->ce_id);
		}
		return 0;
	}

 	OldNsNid *getLastChildNid() {
		if (hasChildElem()) {
			return &(getChild(ond_child->cl_numChild - 1)->ce_id);
		}
		return 0;
	}

	nsChildListCompat_t *getChildListCompat() {
		return ond_child;
	}
	
	nsChildEntryCompat_t *getChild(uint32_t index) {
		if (hasChildElem())
			return &ond_child->cl_child[index];
		return 0;
	}

	nsTextEntry_t *getTextEntry(uint32_t index) const {
		return &(ond_text->tl_text[index]);
	}

	nsAttr_t *getAttr(uint32_t index) const {
		return &(ond_attrs->al_attrs[index]);
	}

	int32_t namePrefix() const {
		return ond_header.nh_name.n_prefix;
	}

	int32_t uriIndex() const {
		return ond_header.nh_uriIndex;
	}
	
	uint32_t childTextIndex(uint32_t index) {
		if (hasChildElem()) {
			if (index < ond_child->cl_numChild)
				return getChild(index)->ce_textIndex;
			else
				return numText();
		}
		return 0;
	}

	int32_t numText() const {
		if (hasText()) return ond_text->tl_ntext;
		return 0;
	}
	
	// child list compat
	uint32_t numChildElem() const {
		if (hasChildElem())
			return ond_child->cl_numChild;
		return 0;
	}
	// all children, incl text
	uint32_t numChild() const {
		uint32_t ret = numChildElem();
		ret += numText();
		return ret;
	}

	static nsChildListCompat_t *allocChildListCompat(
		uint32_t nchild);
	static nsChildListCompat_t *copyChildListCompat(
		const nsChildListCompat_t *clist);
	static void freeChildListCompat(nsChildListCompat_t *clist);
	int addChild(uint32_t textIndex);
	void insertChild(uint32_t index,
			 uint32_t textIndex);
	void removeChild(uint32_t index);

	// unmarshaling routines
	static OldNsNode *unmarshalOldNode(unsigned char *buf,
					   uint32_t bufsize,
					   bool adoptBuffer);
	static unsigned char *unmarshalOldHeader(OldNsNode *node,
						 unsigned char **endPP,
						 unsigned char *ptr,
						 bool copyStrings);
public:
	nsHeader_t     ond_header;
	nsChildListCompat_t *ond_child;
	nsAttrList_t  *ond_attrs;
	nsTextList_t  *ond_text;
	uint32_t       ond_level;
	/// Kept for memory management puposes
	unsigned char *ond_memory;
	/// Plumbing required for parsing
	OldNsNode     *ond_parent;
	const NsFormat *ond_format;
};

}

#endif
