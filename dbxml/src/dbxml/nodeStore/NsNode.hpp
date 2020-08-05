//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

/*
 * NsNode.hpp
 *
 * Transient node storage object, NsNode and accessors.
 * On-disk nodes are marshaled from, and into this object.
 */

#ifndef __DBXMLNSNODE_HPP
#define __DBXMLNSNODE_HPP

#include "NsTypes.hpp"
#include "NsNid.hpp"
#include "NsUtil.hpp"

// Not currently supporting schema type info, so keep
// it out of objects. Leave code for the future.
// #define NS_USE_SCHEMATYPES 1

namespace DbXml
{
#ifdef DEBUG
#define NS_DEBUG 1
class NsDomNode;
#endif


// forward
class NsDocument;
class NsFormat;

/*
 * Properties:
 * o it is designed to minimize the need for excessive
 * memory allocation.  All memory can be allocated in one
 * chunk
 * o it does not know the on-disk record format, but is
 * obviously related
 * o it is agnostic to specific navigation
 * o any references to other element nodes is indirect, via node ids
 * o it can be used for purely transient nodes as well as persistent
 * o it is reference counted to allow multiple objects (e.g.
 *   elements, attributes, text nodes) to share a single NsNode
 * o it is *not* reentrant/thread safe -- no locking is performed
 *
 * The term "DOM" is used loosely to mean any object model that is similar
 * to the actual DOM specified by the W3C
 */
/*
 * Format and notes:
 *
 * NsNode is used to hold element nodes, plus
 * related attributes and (child) text node information.  It is used
 * at the time of document creation (parsing) and for materialization of
 * parts of documents stored as nodes.  In addition, it is used for
 * storage during transient DOM parsing.
 *
 * Character encoding issues:
 *  An instance of NsNode will store UTF-8 strings.
 *
 *  When nodes are materialized, text is still
 *  UTF-8, and is transcoded to UTF-16 on demand, and the new
 *  (transcoded) strings are stored in the containing objects,
 *  and not the NsNode.
 *
 * DOM note:
 *   A single NsNode object is shared among *all* of the "DOM"
 *   nodes that require its data (one element, N attributes, N text nodes).
 *   This significantly reduces the memory allocation needs and allows
 *   on-demand creation of DOM objects.  Reference counting NsNode
 *   allows such referencing nodes to be deleted safely
 *
 * The object is split into a  mandatory header, followed
 * by a number of optional members.  Which members are present/valid
 * is determined by the flags in the header.
 *
 * In this file, the internal structure objects are declared first, in
 * order of dependence:
 *  nsText_t -- holds text, such as names, attributes, text
 *  nsName_t -- name, which may be prefixed, and may be typed.
 *  nsHeader_t -- mandatory header
 *  nsAttr_t -- stores one attribute/value pair.
 *  nsAttrList_t -- list of attributes
 *  nsTextEntry_t -- a single, typed text entry
 *  nsTextList_t -- list of text entries
 */

/**
 * Note on namespace uris and prefixes, as well as type names and
 * uris.  These are kept as int32_t (signed) to allow the use of -1
 * to indicate "none."  They are, in reality, treated as NameID objects
 * (see ../NameID.hpp), as the integers map to dictionary entries for
 * the uri or prefix involved.
 *
 * This means that all documents in a container share URI and prefix
 * names.  The plus is that they are amortized across documents.
 * The minus is that using a shared dictionary for these can introduce
 * some concurrency issues during loading; however, given that most
 * applications will store similar documents in a container, once
 * the dictionary is "loaded" there will be no such problem
 */
 
/**
 * nsText_t -- holds string text, such as names, attributes, text
 *
 * In memory, all strings are 
 * UTF-8 (xmlbyte_t), and all are NULL terminated.
 *
 * Length (t_len) in this structure is a "character" count,
 * not including the trailing NULL (i.e. the
 * same as strlen() results).
 *
 * Characters that are encoded in more than one
 * xmlbyte_t are counted as more than one (e.g. 3-byte utf-8
 * character counts as 3).  So, in the case of UTF-8, t_len
 * is equal to byte count (vs character count).
 *
 * Rules:
 *   o The string and t_len are in UTF-8 bytes
 */
typedef struct {
	size_t t_len;      /**< num characters, not including NULL */
        xmlbyte_t *t_chars;      /**< pointer to string, NULL-terminated */
} nsText_t;

/**
 * nsName_t -- name, which may be prefixed, and may be typed.
 *
 * An element or attribute name, which may have
 * a namespace prefix.  n_prefix is NS_NOPREFIX (-1)
 * if no prefix present.  n_type is NS_NOTYPE (-1) if
 * no type information is available.
 * XML Schema type information (when implemented) is
 * type name and associated type URI.
 *
 * the n_prefix is really a NameID (see NameID.hpp), as it
 * maps to a dictionary entry for the prefix.  See note above.
 *
 * The localname is stored in n_text.
 */
typedef struct {
	int32_t n_prefix;
#ifdef NS_USE_SCHEMATYPES	
	int32_t n_type;   /**< type id */
	int32_t n_utype;  /**< union type id if union */
#endif
	nsText_t n_text;
} nsName_t;
	
/**
 * nsHeader_t -- mandatory header
 *  Includes Node id, flags, parent id, and naming information
 *  Last descendant is here because it's materialized for both
 *  NLS and DLS, while the nsNav_t is only used for NLS.
 *
 *  If we wanted the overhead of copying NIDs for DLS all the time,
 *  it'd go into nsNav_t, which would not be optional.
 */
typedef struct {
	NsFullNid       nh_id;       /**< node id */
	uint32_t    nh_flags;    /**< node state flags */
	NsFullNid       nh_parent;   /**< parent node id */
	NsFullNid       nh_lastDescendant;   /**< last descendant node id */
	int32_t     nh_uriIndex; /**< uri index, 0 if default */
	nsName_t    nh_name;     /**< node name */
} nsHeader_t;

/**
 * nsNav_t -- sibling/child navigational info
 */
typedef struct {
	NsFullNid nn_prev;
	NsFullNid nn_lastChild;
} nsNav_t;

/**
 * nsAttr_t -- stores one attribute/value pair.
 *
 * The nsAttr_t structure is used to store an attribute.
 * the a_name field is nsName_t, which stores *both*
 * the name and value in the form:
 *   attrName0attrValue0
 * The a_name.n_text.t_chars field references the name,
 * the a_value pointer points directly to the value.
 *
 * This scheme makes materialization faster, as well as simplifying memory
 * management.
 *
 * The a_len field of each nsAttr_t is the length
 * in characters of the entire name/value string,
 * minus the trailing NULL.
 * If the name has a namespace prefix, that information is
 * stored in the nsName_t n_prefix field.  If there is any
 * XML schema information available for this attribute, it is
 * kept in the nsName_t n_type field.
 */
typedef struct {
	nsName_t a_name;     /**< attribute name */
	const xmlbyte_t *a_value; /**< attribute value */
	uint32_t a_flags;    /**< attribute flags */
	int32_t a_uri;       /**< attribute uri */
} nsAttr_t;
	
// short cuts
#define a_len a_name.n_text.t_len
#define a_prefix a_name.n_prefix
#define a_name8 a_name.n_text.t_chars

#ifdef NS_USE_SCHEMATYPES
#define a_type a_name.n_type
#define a_utype a_name.n_utype
#endif
	
/**
 * nsAttrList_t -- present if a node has attributes.
 */
typedef struct {
	/**
	 * a_len is total length, in UTF-8 characters, of all
	 * name/value strings, including the NULLs.  Used to determine space
	 * needed for persistent storage.
	 */
	size_t  al_len;
	size_t  al_marshalLen; // al_len + type info for marshaling
	uint32_t  al_nattrs;   /**< number of attributes */
	uint32_t  al_max;   /**< num entries that can be put in attr list */
	nsAttr_t  al_attrs[1]; /**< the attributes */
} nsAttrList_t;

#define NS_NATTR_DEFAULT 4
	
/**
 * nsTextEntry_t -- a single, typed text entry
 * type is NS_TEXT, NS_COMMENT, NS_PINST, etc.
 */
typedef struct nsTextEntry {
	uint32_t  te_type;  /**< type of text */
	nsText_t  te_text;  /**< the text */
} nsTextEntry_t;
	
/**
 * nsTextList_t -- present if node has any text children
 *
 * When it is being created from a parse, the nsTextEntry_t
 * list may need to be reallocated, as new text nodes show up.
 * Start with room for 2.  Double on every realloc.
 */
#define NS_NTEXT_DEFAULT 2
	
typedef struct nsTextList {
	/**
	 * total length, in characters, of all text strings,
	 * used to determine persistent storage size.  Includes NULLs.
	 */
	size_t  tl_len;
	uint32_t  tl_ntext;   /**< number of text nodes */
	uint32_t  tl_nchild;  /**< number that are child text */
	uint32_t  tl_max;     /**< num entries that can be put in entry list */
	nsTextEntry_t tl_text[1];  /**< the nodes */
} nsTextList_t;
/**
 * state flags -
 *
 * Most used bits should be the lowest in value, to aid the
 * compression of the flags int (note: flags are renumbered
 * between 2.2 and 2.3; old flags values are in NsUpgrade.*).
 */
enum NS_FLAGS {
	NS_HASCHILD =      0x00000001, /**< has at least one child element */
	NS_HASATTR =       0x00000002, /**< has attributes */
	NS_HASTEXT =       0x00000004, /**< has text nodes */
	NS_HASTEXTCHILD =  0x00000008, /**< has a text child attached */
	NS_HASNEXT =       0x00000010, /**< has next sibling */
	NS_HASPREV =       0x00000020, /**< has prev sibling */
	NS_ISROOT =        0x00000040, /**< only set for constructed XML where node is the
					  root element */
	NS_LAST_IS_LAST_DESC = 0x00000080, /**< last child is a leaf */
	NS_NAMEPREFIX =    0x00000100, /**< name has a prefix */
	NS_HASURI =        0x00000200, /**< has a namespace URI */
	NS_HASNSINFO =     0x00000400, /**< has namespace decls in attrs */
	NS_ISDOCUMENT =    0x00000800, /**< is the document node */
	NS_NAMETYPE =      0x00001000, /**< element has a type */
	NS_UNIONTYPE =     0x00002000, /**< element has a type */

	// Flags that aren't put into the database
	NS_ALLOCATED =   0x20000000,  /**< each part is allocated separatly (rather than in one large chunk) */
	NS_STANDALONE =  0x40000000,  /**< is a DOM parse, or standalone node (not persistent at all) */
	NS_DEALLOCNAME = 0x80000000,  /**< Deallocate name all the time */
	NS_NONAV =       0x01000000,  /**< don't copy navigational info (transient dom) */
	NS_DEALLOCNAV =  0x02000000   /**< Deallocate nd_nav */
};
#define NS_DBFLAGMASK (~(NS_ALLOCATED|NS_STANDALONE|NS_DEALLOCNAME|NS_NONAV|NS_DEALLOCNAV)) ///< flags that aren't put into the database

/* attribute-specific flags; keep under 7 bits */
enum NS_ATTR_FLAGS {
	NS_ATTR_PREFIX =      0x0001, /**< has a namespace prefix */
	NS_ATTR_TYPE   =      0x0002, /**< has a type */
	NS_ATTR_UNION_TYPE  = 0x0004, /**< has a union type as well */
	NS_ATTR_ENT    =      0x0008, /**< has entities for escaping */
	NS_ATTR_URI    =      0x0010, /**< has a namespace uri */
	NS_ATTR_NOT_SPECIFIED=0x0020, /**< NOT specified;default is specified */
	NS_ATTR_IS_DECL=      0x0040  /**< is a namespace decl*/
};

#define NS_ATTR_MASK 0xff
#define NS_ATTR_DONTDELETE 0x1000	

/**
 * text type is 2 parts: a type (0-7), and flags (bit flags starting
 * at 0x8.  These values are persisted, and the format is intended
 * to keep the size to 7 bits.
 */
enum NS_TEXTTYPES {
	NS_TEXT =       0,
	NS_COMMENT =    1,  /**< text is comment */
	NS_CDATA =      2,  /**< text is CDATA */
	NS_PINST =      3,  /**< text is processing instruction */
	NS_SUBSET =     4,  /**< text is doc type decl */
	NS_ENTSTART =   5,  /**< text is an entity name -- start */
	NS_ENTEND =     6,  /**< text is an entity name -- end */
	NS_EXTERNAL =   7   /**< text is in an external node */
};

enum NS_TEXTFLAGS {
	NS_ENTITY_CHK = 0x08,   /**< look for entities in serialize */
	NS_IGNORABLE  = 0x10,   /**< ignorable whitespace */
	NS_DONTDELETE = 0x20,   /**< don't delete t_chars */
	NS_DELETEDTEXT= 0x0ff    /**< deleted text entry */
};

#define NS_TEXTMASK 0x7	
#define NS_NOPREFIX (-1) // no namespace prefix
#define NS_NOURI (-1) // no uri
#define NS_NOTYPE (-1)   // no type information available
#define NS_NOTEXT (-1)	


/**
 * NsNode -- the node itself
 *
 * Transient node.  Includes a header and pointers
 * to all optional members. Only valid members will be non-null.
 * This structure is used for both parsing and persistent
 * DOM materialization.
 *
 * During the parse, the optional structures are allocated
 * on demand.  When materialized from a DB record, the
 * presence and size of optional fields are known, and
 * can be allocated in a single, large chunk of memory,
 * carved up during initialization (unmarshaling).
 *
 * The structure includes a transient parent link that is
 * used only during initial parsing.
 */
class NsNode {
public:
	void acquire();
	void release();
	int count() const { return nd_count; }
	
	bool isAllocated() const {
		return ((nd_header.nh_flags & NS_ALLOCATED) != 0);
	}
	bool isStandalone() const {
		return ((nd_header.nh_flags & NS_STANDALONE) != 0);
	}
	bool isDoc() const {
		return ((nd_header.nh_flags & NS_ISDOCUMENT) != 0);
	}
	bool isRoot() const {
		return ((nd_header.nh_flags & NS_ISROOT) != 0);
	}
	bool hasChildElem() const {
		return ((nd_header.nh_flags & NS_HASCHILD) != 0);
	}
	bool hasText() const {
		return ((nd_header.nh_flags & NS_HASTEXT) != 0);
	}
	bool hasTextChild() const {
		return ((nd_header.nh_flags & NS_HASTEXTCHILD) != 0);
	}
	bool isTextChild(uint32_t index) const {
		if ((nd_header.nh_flags & NS_HASTEXTCHILD) &&
		    (index >= (nd_text->tl_ntext - nd_text->tl_nchild)))
			return true;
		return false;
	}
	bool hasLeadingText() const {
		return (getNumText() > getNumChildText());
	}
	bool hasNext() const {
		return ((nd_header.nh_flags & NS_HASNEXT) != 0);
	}
	bool hasPrev() const {
		return ((nd_header.nh_flags & NS_HASPREV) != 0);
	}
	bool needsNav() const {
		return (hasPrev() || hasChildElem());
	}
	bool noNav() const {
		return ((nd_header.nh_flags & NS_NONAV) != 0);
	}
	bool hasChildNode() const {
		return ((nd_header.nh_flags & (NS_HASCHILD|NS_HASTEXTCHILD)) != 0);
	}
	bool hasAttributes() const {
		return ((nd_header.nh_flags & NS_HASATTR) != 0);
	}
	bool hasNamePrefix() const {
		return ((nd_header.nh_flags & NS_NAMEPREFIX) != 0);
	}
	bool hasUri() const {
		return ((nd_header.nh_flags & NS_HASURI) != 0);
	}
	bool hasNsinfo() const {
		if ((nd_header.nh_flags & NS_HASNSINFO) != 0)
			return true;
		return false;
	}
	bool attrHasUri(uint32_t index) const {
		if (attrFlags(index) & NS_ATTR_URI)
			return true;
		return false;
	}

	bool attrIsSpecified(uint32_t index) const {
		if (attrFlags(index) & NS_ATTR_NOT_SPECIFIED)
			return false;
		return true;
	}

	bool checkFlag(uint32_t flag) const {
		return ((nd_header.nh_flags & flag) != 0);
	}

	uint32_t getFlags() const {
		return nd_header.nh_flags;
	}
		
	uint32_t *getFlagsPtr()  {
		return &nd_header.nh_flags;
	}
		
	// int/uint getters
	
	int32_t getNumText() const {
		if (hasText())
			return nd_text->tl_ntext;
		return 0;
	}

	int32_t getNumLeadingText() const {
		if (hasText())
			return (nd_text->tl_ntext - nd_text->tl_nchild);
		return 0;
	}

	int32_t getNumChildText() const {
		if (hasText())
			return nd_text->tl_nchild;
		return 0;
	}

	int32_t getFirstTextChildIndex() const {
		if (hasText())
			return (nd_text->tl_ntext - nd_text->tl_nchild);
		return -1;
	}

	uint32_t getLevel() const {
		return nd_level;
	}

	uint32_t *getLevelPtr()  {
		return &nd_level;
	}
		

	uint32_t numAttrs() const {
		if (hasAttributes())
			return nd_attrs->al_nattrs;
		return 0;
	}

	int32_t namePrefix() const {
		return nd_header.nh_name.n_prefix;
	}
#ifdef NS_USE_SCHEMATYPES
	int32_t typeIndex() const {
		return nd_header.nh_name.n_type;
	}

	int32_t utypeIndex() const {
		return nd_header.nh_name.n_utype;
	}
#endif
	int32_t uriIndex() const {
		return nd_header.nh_uriIndex;
	}

	int32_t *uriIndexPtr() {
		return &(nd_header.nh_uriIndex);
	}

	uint32_t textType(uint32_t index) const {
		return  nd_text->tl_text[index].te_type;
	}

	uint32_t attrFlags(uint32_t index) const {
		return  (nd_attrs->al_attrs[index].a_flags);
	}

	const xmlbyte_t *attrValue(uint32_t index) const {
		return  (nd_attrs->al_attrs[index].a_value);
	}

	bool isAttrNamespaceDecl(uint32_t index) const {
		return ((nd_attrs->al_attrs[index].a_flags & NS_ATTR_IS_DECL)
			!= 0);
	}

	int32_t attrNamePrefix(uint32_t index) const {
		return getAttrName(index)->n_prefix;
	}

	int32_t attrUri(uint32_t index) const {
		nsAttr_t *attr = getAttr(index);
		if (attr->a_flags & NS_ATTR_URI)
			return attr->a_uri;
		return NS_NOURI;
	}

	bool attrNeedsEscape(uint32_t index) const {
		nsAttr_t *attr = getAttr(index);
		if (attr->a_flags & NS_ATTR_ENT)
			return true;
		return false;
	}
		
	NsNode *getParent() {
		return nd_parent;
	}

 	const NsFormat *getFormat() const {
		return nd_format;
	}

	NsFullNid *getFullNid() {
		return &(nd_header.nh_id);
	}

	NsNid getNid() const {
		return NsNid(&nd_header.nh_id);
	}

	NsFullNid *getPrevNid() {
		if (nd_nav)
			return &nd_nav->nn_prev;
		return 0;
	}

	const NsFullNid *getLastDescendantNid() const {
		if (nd_header.nh_lastDescendant.isNull())
			return 0;
		return &nd_header.nh_lastDescendant;
	}

	const NsFullNid *getLastDescendantNidOrSelf() const {
		if (nd_header.nh_lastDescendant.isNull())
			return &nd_header.nh_id;
		return &nd_header.nh_lastDescendant;
	}

	NsFullNid *getLastDescendantNidPtr() {
		return &nd_header.nh_lastDescendant;
	}

	NsFullNid *getLastChildNid() {
		if (nd_nav)
			return &nd_nav->nn_lastChild;
		return 0;
	}
	
	NsFullNid *getParentNid() {
		DBXML_ASSERT(!isDoc());
		return &(nd_header.nh_parent);
	}

	nsNav_t *getNav() {
		return nd_nav;
	}

	nsName_t *getName() {
		return  &(nd_header.nh_name);
	}

	nsText_t *getNameText() {
		return  &(nd_header.nh_name.n_text);
	}

	xmlbyte_t *getNameChars() {
		return  nd_header.nh_name.n_text.t_chars;
	}

	size_t getNameLen() {
		return  nd_header.nh_name.n_text.t_len;
	}

	nsText_t *getText(uint32_t index) const {
		return  &(nd_text->tl_text[index].te_text);
	}

	nsTextList_t *getTextList() {
		return nd_text;
	}

	nsTextEntry_t *getTextEntry(uint32_t index) const {
		return  &(nd_text->tl_text[index]);
	}

	nsTextEntry_t *getLastTextEntry() const {
		return  &(nd_text->tl_text[nd_text->tl_ntext-1]);
	}

	nsAttr_t *getAttr(uint32_t index) const {
		return  &(nd_attrs->al_attrs[index]);
	}

	nsAttrList_t *getAttrList() {
		return nd_attrs;
	}

	nsName_t *getAttrName(uint32_t index) const {
		return  &(getAttr(index)->a_name);
	}

	// setters
	void setFlag(uint32_t flag)  {
		nd_header.nh_flags |= flag;
	}

	void clearFlag(uint32_t flag)  {
		nd_header.nh_flags &= ~flag;
	}

	void setNoNav() {
		setFlag(NS_NONAV);
	}

	void setIsDocument() {
		setFlag(NS_ISDOCUMENT);
	}

	void setIsRoot() {
		setFlag(NS_ISROOT);
	}
	
	void setHasChildElem() {
		setFlag(NS_HASCHILD);
	}
	
	void setNamePrefix(int32_t prefix)  {
		nd_header.nh_name.n_prefix = prefix;
	}

	void setUriIndex(int32_t index)  {
		nd_header.nh_uriIndex = index;
	}

	void setAttrPrefix(nsAttr_t *attr, int32_t prefix) {
		attr->a_name.n_prefix = prefix;
		if (prefix != NS_NOPREFIX)
			attr->a_flags |= NS_ATTR_PREFIX;
	}

	void setAttrNamePrefix(uint32_t index, int32_t prefix) {
		setAttrPrefix(getAttr(index), prefix);
	}

	void setAttrUri(nsAttr_t *attr, int32_t uri) {
		attr->a_uri = uri;
		if (uri != NS_NOURI)
			attr->a_flags |= NS_ATTR_URI;
	}

	void setAttrList(nsAttrList_t *list) {
		nd_attrs = list;
	}

	void setLevel(uint32_t level) {
		nd_level = level;
	}

	void setNav(nsNav_t *nav) {
		nd_nav = nav;
	}

	void setTextList(nsTextList_t *list) {
		nd_text = list;
	}

	void setParent(NsNode *parent) {
		nd_parent = parent;
	}

	void setNextPrev(NsNode *previous); // not inline

	void clearNext();

	void clearPrev();

	void setLastChild(NsNode *child); // not inline
	void setLastDescendantNid(const NsFullNid *nid); // not inline
	void setFormat(const NsFormat *fmt) {
		nd_format = fmt;
	}

	void setMemory(unsigned char *buf) {
		nd_memory = buf;
	}

	// tell node that a child is being added, passing current
	// accumulated text nodes (previous siblings)
	nsTextList_t *startElem(NsNode *child,
				nsTextList_t *textList);

	nsTextList_t *endElem(NsNode *parent, NsNode *previous,
			      nsTextList_t *textList);

public:
	nsAttr_t *setAttr(NsDocument *doc, uint32_t index,
			  const xmlch_t *prefix, const xmlch_t *uri,
			  const xmlch_t *localname, const xmlch_t *value,
			  bool specified);
	int addAttr(NsDocument *doc, const xmlch_t *prefix,
		    const xmlch_t *uri, const xmlch_t *localname,
		    const xmlch_t *value, bool specified);
	int addAttr(NsDocument *doc, const xmlbyte_t *prefix,
		    const xmlbyte_t *uri, const xmlbyte_t *localname,
		    const xmlbyte_t *value, bool specified);
	
	void displayNode(std::ostream &out) const;
	
	static NsNode *allocNode(uint32_t attrCount, uint32_t flags);
	static nsNav_t *allocNav();
	static void freeNav(nsNav_t *nav,
			    bool freePointer = true);
	static nsAttrList_t *allocAttrList(uint32_t attrCount);
	static void freeAttrList(nsAttrList_t *attrs);
	static nsTextList_t *addText(nsTextList_t *list,
				     const xmlbyte_t *text, size_t len,
				     uint32_t type, bool donate);
	static nsTextList_t * addPI(nsTextList_t *list,
				    const xmlbyte_t *target, const xmlbyte_t *data,
				    bool donate, size_t len);
	static nsTextList_t *createTextList(uint32_t ntext = NS_NTEXT_DEFAULT);
	static void freeTextList(nsTextList_t *text);
	static size_t createText(nsText_t *dest,
				 const xmlbyte_t *text, size_t length, bool donate,
				 bool doubleString = false, bool *hasEntity = 0,
				 enum checkType type = ignore);
	static size_t createPI(nsText_t *dest,
			       const xmlbyte_t *target, const xmlbyte_t *data,
			       bool donate, size_t len);
	static size_t coalesceText(nsTextList_t *list, const xmlbyte_t *text,
				   size_t len, bool checkEnt);
#if defined(NS_DEBUG)
	void printNode(NsDocument *doc);
#endif
public:
	// Methods for updating existing nodes
	void removeAttr(int index);
	nsTextList_t *removeText(int startIndex, int endIndex);
	bool canCoalesceText() const;
	void renameElement(const char *name,
			   size_t nameLen,
			   int uri, int prefix);
	nsAttrList_t *copyAttrList(int numNewAttrs);
	nsAttrList_t *replaceAttrList(nsAttrList_t *oldlist,
				      bool freeOldList = false);
	nsTextList_t *replaceTextList(nsTextList_t *oldlist,
				      bool freeOldList = false);
private:
	static void freeNode(NsNode *node);
private:
	nsHeader_t     nd_header;
	nsAttrList_t  *nd_attrs;
	nsTextList_t  *nd_text;
	nsNav_t       *nd_nav;
	uint32_t       nd_level;
	/// Kept for memory management puposes
	unsigned char *nd_memory;
	/// Plumbing required for parsing
	NsNode        *nd_parent;
	const NsFormat *nd_format;
	int            nd_count;
};


//
// ref-counting wrapper class
//

class DBXML_EXPORT NsNodeRef {
public:
	NsNodeRef(const NsNode *node = 0);
	NsNodeRef(const NsNodeRef &other);
	~NsNodeRef();
	NsNodeRef &operator=(const NsNode *n);
	NsNodeRef &operator=(const NsNodeRef &other);
	NsNode *get() const;
	NsNode *operator->() const;
	NsNode *operator*() const;
	operator bool() const {
		return (node_ != 0);
	}
private:
	NsNode *node_;
};
	
//
// Some helper functions for text types
//

inline uint32_t nsTextType(uint32_t type)
{
	return (type & NS_TEXTMASK);
}

inline bool nsTextEntityChk(uint32_t type)
{
	if (type & NS_ENTITY_CHK)
		return true;
	return false;
}
inline bool nsIsEntityText(uint32_t type)
{
	return  ((nsTextType(type) == NS_ENTSTART) ||
		 (nsTextType(type) == NS_ENTEND));
}

inline bool nsIsEntityType(uint32_t ttype)
{
	const uint32_t type = nsTextType(ttype);
	return ((type == NS_ENTSTART)
		|| (type == NS_ENTSTART)
		|| (type == NS_SUBSET));
}

inline bool nsIsText(uint32_t type)
{
	return  (nsTextType(type) == NS_TEXT);
}

inline bool nsIsCDATA(uint32_t type)
{
	return  (nsTextType(type) == NS_CDATA);
}

inline bool nsIsComment(uint32_t type)
{
	return  (nsTextType(type) == NS_COMMENT);
}

inline bool nsIsPI(uint32_t type)
{
	return  (nsTextType(type) == NS_PINST);
}

inline bool nsIsSubset(uint32_t type)
{
	return  (nsTextType(type) == NS_SUBSET);
}

inline bool nsIsEntityStart(uint32_t type)
{
	return  (nsTextType(type) == NS_ENTSTART);
}

inline bool nsIsEntityEnd(uint32_t type)
{
	return  (nsTextType(type) == NS_ENTEND);
}

class IndexNodeInfo {
public:
	virtual ~IndexNodeInfo() {}
	virtual const NsNid getNodeID() = 0;
	virtual u_int32_t getNodeLevel() = 0;
	virtual size_t getNodeDataSize() = 0;
	virtual const NsNode *getNode() const { return 0; }
};

class NsNodeIndexNodeInfo : public IndexNodeInfo {
public:
	NsNodeIndexNodeInfo() : node_(0) {}
	NsNodeIndexNodeInfo(const NsNode *node) : node_(node) {}
	const NsNid getNodeID() { return node_->getNid(); }
	u_int32_t getNodeLevel() { return node_->getLevel(); }
	size_t getNodeDataSize();
	const NsNode *getNode() const { return node_; }
	const NsNode *node_;
};

}
#endif
