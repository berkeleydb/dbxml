//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsNode.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "UTF8.hpp"

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

#define nsNodeAllocate NsUtil::allocate
#define nsNodeDeallocate NsUtil::deallocate

void NsNode::acquire()
{
	DBXML_ASSERT(nd_count>=0);
	++nd_count;
}

void NsNode::release()
{
	if (--nd_count == 0) {
#ifdef NS_DEBUG		
		nd_count = -1;
#endif
		freeNode(this);
	}
}

// static
nsNav_t *
NsNode::allocNav()
{
	nsNav_t *nav = (nsNav_t *)nsNodeAllocate(sizeof(nsNav_t));
	memset(nav, 0, sizeof(nsNav_t));
	return nav;
}

// static
void
NsNode::freeNav(nsNav_t *nav,
		bool freePointer)
{
	if (!nav->nn_prev.isNull())
		nav->nn_prev.freeNid();
	if (!nav->nn_lastChild.isNull())
		nav->nn_lastChild.freeNid();
	if (freePointer)
		nsNodeDeallocate(nav);
}

//static
nsAttrList_t *
NsNode::allocAttrList(uint32_t attrCount)
{
	if (attrCount != 0) {
		size_t msize = sizeof(nsAttrList_t) +
			((attrCount - 1) * sizeof(nsAttr_t));
		nsAttrList_t *attrlist =
			(nsAttrList_t *)nsNodeAllocate(msize);
		memset(attrlist, 0, msize);
		attrlist->al_max = attrCount;
		return attrlist;
	}
	return 0;
}

/*
 * reallocate list, copying array of nsAttr_t structs
 */
static nsAttrList_t *
_reallocAttrList(nsAttrList_t *list)
{
	if (list == 0)
		return NsNode::allocAttrList(NS_NATTR_DEFAULT);

	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsAttrList_t) +
		((list->al_max - 1) * sizeof(nsAttr_t));

	list->al_max <<= 1;
	nsAttrList_t *result = NsNode::allocAttrList(list->al_max);

	memcpy(result, list, oldsize);
	nsNodeDeallocate(list);
	return result;
}


/*
 * allocate a node.  Since attribute count is always known,
 * allocate space for it, as well.
 * Alignment node: since NsNode is all pointers, assume
 * that memory after it is sufficiently aligned to allow
 * placing nsAttrList_t there.
 */
// static
NsNode *
NsNode::allocNode(uint32_t attrCount,
		  uint32_t flags)
{
	/* node + [attrList + array of text for attrs] */
	size_t msize = sizeof(NsNode);
	NsNode *node = (NsNode *)nsNodeAllocate(msize);
	memset(node, 0, msize);
	// NS_ALLOCATED must be set, or the node won't be deallocated properly
	node->setFlag(NS_ALLOCATED|flags);
	if (attrCount != 0) {
		node->setAttrList(allocAttrList(attrCount));
		node->setFlag(NS_HASATTR);
	}
	node->setNamePrefix(NS_NOPREFIX);
	node->setFormat(&(NsFormat::getFormat(NS_PROTOCOL_VERSION)));
	return node;
}

/*
 * Free all memory associated with node.
 */
// static
void
NsNode::freeNode(NsNode *node)
{
	DBXML_ASSERT(node->count() == 0 || node->count() == -1);
	if(node->isAllocated()) {
		// Each part is allocated separatly, including all lists and strings
		uint32_t flags = node->getFlags();
		node->nd_header.nh_id.freeNid();
		node->nd_header.nh_parent.freeNid();
		node->nd_header.nh_lastDescendant.freeNid();
		nsNodeDeallocate(node->getNameChars());

		if (node->getNav()) {
			freeNav(node->getNav());
		}
		if (flags & NS_HASATTR) {
			freeAttrList(node->nd_attrs);
		}
		if (flags & NS_HASTEXT) {
			freeTextList(node->nd_text);
		}
	} else {
		// Even if the node is one chunk, large node ids
		// could have resulted in extra allocations, so it's necessary
		// to free ids separately
		node->nd_header.nh_id.freeNid();
		node->nd_header.nh_parent.freeNid();
		node->nd_header.nh_lastDescendant.freeNid();
		// freeNav calls freeNid on all navigational NsNids,
		// pass false to *not* free the pointer, too
		if (node->getNav())
			freeNav(node->getNav(),
				(node->checkFlag(NS_DEALLOCNAV) ? true :
				 false));
		// if node was renamed, the new name may have
		// been dynamically allocated
		if (node->checkFlag(NS_DEALLOCNAME))
			nsNodeDeallocate(node->getNameChars());
		
		// Free any extra memory attached to the NsNode
		if(node->nd_memory) {
			// We call free, because this memory comes
			// from a Dbt with DB_DBT_MALLOC set
			::free(node->nd_memory);
		}
	}

	// All NsNode memory allocated in one large chunk
	nsNodeDeallocate(node);
}

//static
void
NsNode::freeAttrList(nsAttrList_t *attrs)
{
	int32_t nattrs = attrs->al_nattrs;
	for (int i = 0; i < nattrs; i++) {
		if ((attrs->al_attrs[i].a_flags & NS_ATTR_DONTDELETE) == 0)
			nsNodeDeallocate(
				attrs->al_attrs[i].a_name.n_text.t_chars);
	}
	nsNodeDeallocate(attrs);
}

//static
void
NsNode::freeTextList(nsTextList_t *text)
{
	int32_t ntext = text->tl_ntext;
	for (int i = 0; i < ntext; i++) {
		if ((text->tl_text[i].te_type & NS_DONTDELETE) == 0)
			nsNodeDeallocate(text->tl_text[i].te_text.t_chars);
	}
	nsNodeDeallocate(text);
}

//
// non-static methods
//

static void _throwLengthException(size_t expected, size_t actual)
{
	std::ostringstream oss;
	oss << "Invalid string length: " << (unsigned int)expected;
	if (actual < expected)
		oss << ", actual length is " << (unsigned int)actual;
	else
		oss << ", actual length is larger";
	oss << ".";
	throw XmlException(XmlException::EVENT_ERROR,
			   oss.str());
}
	
// memcpy that copies utf-8 chars, checking length,
// and throws an exception if they don't match.  This is
// because the length args may have come from the application,
// via XmlEventWriter.  The length should include the trailing null.
static void _memcpyCheckLen(void *dest, const xmlbyte_t *src, size_t len)
{
	xmlbyte_t *tdest = (xmlbyte_t *)dest;
	size_t tlen = 0;
	while ((*tdest++ = *src++) != '\0') {
		++tlen;
		if (tlen == len)
			break;
	}
	if (tlen != (len - 1))
		_throwLengthException(len-1, tlen);
}

static bool _memcpyCheck8(xmlbyte_t *dest, const xmlbyte_t *src,
			  size_t len, enum checkType type,
			  size_t &nrb, bool isMulti)
{
	bool hasEnt = false;
	bool *hasEntity = &hasEnt;
	xmlbyte_t c;
	size_t tlen = 0;
	do {
		c = *src++;
		CHECK_ENT(c);
		++tlen;
		*dest++ = c;
		if (!c && isMulti) {
			isMulti = false;
			continue;
		}
	} while (c);
	if (len != tlen)
		_throwLengthException(len-1, tlen-1);
	return hasEnt;
}

// Input is one of:
//   utf-8 -- donated
//   utf-8 -- must be copied.
// In the last instance, the string is scanned during the copy
// to check for entities, and the result returned in hasEntity
//
// static
size_t
NsNode::createText(nsText_t *dest,
		   const xmlbyte_t*text,
		   size_t length, bool donate, bool doubleString,
		   bool *hasEntity, enum checkType type)
{
	size_t allocLen;
	if (length == 0 && text == 0) {
		// handle empty text
		static const char emptyText[4] = { 0, 0, 0, 0};
		if (!donate)
			text = (const xmlbyte_t *) emptyText;
		else {
			text = (const xmlbyte_t *) nsNodeAllocate(4);
			*((char*)text) = 0;
		}
	}
	length += 1; // trailing null
	if (!donate) {
		allocLen = length;
		xmlbyte_t *newchars =
			(xmlbyte_t*) nsNodeAllocate(allocLen);
		try {
			if (hasEntity) {
				size_t nrb = 0;
				*hasEntity = _memcpyCheck8(
					newchars,
					(const xmlbyte_t *)text,
					allocLen, type, nrb, false);
			} else {
				// No need to check the length if this is
				// a double string (str1\0str2\0), since this
				// won't have come from the user
				if(doubleString)
					memcpy(newchars, text, allocLen);
				else _memcpyCheckLen(newchars, text, allocLen);
			}
		} catch (...) {
			nsNodeDeallocate(newchars);
			throw;
		}
		dest->t_chars = newchars;
	} else
		dest->t_chars = (xmlbyte_t *) text; // cast is safe on donate
	dest->t_len = length - 1; /* don't include trailing null */
	return length;
}

/*
 * Allocate default size for text list
 */
//static
nsTextList_t *NsNode::createTextList(uint32_t ntext)
{
	// subtract 1 from ntext because there's a built-in nsTextEntry_t
	// already
	size_t msize = sizeof(nsTextList_t) +
		((ntext - 1) * sizeof(nsTextEntry_t));
	nsTextList_t *tlist = (nsTextList_t *) nsNodeAllocate(msize);
	if (tlist != 0) {
		memset(tlist, 0, msize);
		tlist->tl_max = ntext;
	}
	return tlist;
}

/*
 * reallocate list, copying array of nsTextEntry_t structs
 */
static nsTextList_t *
_reallocTextList(nsTextList_t *list)
{
	if (list == 0)
		return NsNode::createTextList();
	/* realloc -- double size */
	uint32_t oldsize = sizeof(nsTextList_t) +
		((list->tl_max - 1) * sizeof(nsTextEntry_t));
	list->tl_max <<= 1;
	size_t msize = sizeof(nsTextList_t) +
		((list->tl_max-1) * sizeof(nsTextEntry_t));

	// reallocate
	void *newlist = nsNodeAllocate(msize);
	memcpy(newlist, list, oldsize);
	nsNodeDeallocate(list);
	return (nsTextList_t *) newlist;
}

/*
 * Add a text node to the list.  Length is
 * available in this case.  Return index used.
 * Create text list if necessary.
 */
// static
nsTextList_t *
NsNode::addText(nsTextList_t *list,
		const xmlbyte_t *text,
		size_t length, uint32_t type, bool donate)
{
	bool reallocd = false;
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		list = _reallocTextList(list);
		reallocd = true;
	}
	/* add to list */
	int index = list->tl_ntext++;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = type;
	if ((type & NS_DONTDELETE) != 0) {
		// reference existing string -- no copy
		entry->te_text.t_chars = (xmlbyte_t *)text;
		entry->te_text.t_len = length;
		list->tl_len += length + 1;
	} else {
		try {
			bool hasEntity = false;
			// set isCharacters -- it's ignored if the
			// hasEntity arg is 0
			list->tl_len +=
				createText(&entry->te_text,
					   text, length, donate,
					   nsTextType(type) == NS_PINST,
					   (nsTextType(type) == NS_TEXT ? &hasEntity : 0),
					   isCharacters);
			if (hasEntity)
				entry->te_type |= NS_ENTITY_CHK;
		} catch (...) {
			if (reallocd)
				NsUtil::deallocate(list);
			throw;
		}
	}
	return list;
}

// static
size_t
NsNode::coalesceText(nsTextList_t *list, 
		     const xmlbyte_t *text, size_t len, bool checkEnt)
{
	size_t nrb = 0;
	// get last entry for coalesce
	nsTextEntry_t *entry = &list->tl_text[list->tl_ntext-1];
	size_t allocLen, copyLen, newLen, oldLen = entry->te_text.t_len;
	newLen = oldLen + len + 1;

	// do copy operations in bytes
	allocLen = newLen;
	copyLen = len + 1; // add trailing null

	xmlbyte_t *newText = (xmlbyte_t *)nsNodeAllocate(allocLen);
	// copy original string, but don't copy null
	// only do entity check copy if 1) not already set, and
	// 2) checkEnt is true
	// this function is only called for character text
	if (checkEnt && !(entry->te_type & NS_ENTITY_CHK)) {
		xmlbyte_t *src = entry->te_text.t_chars;
		(void) _memcpyCheck8(
			(xmlbyte_t*)newText, src, oldLen + 1, //call includes null
			isCharacters, nrb, false);
	} else
		memcpy(newText, entry->te_text.t_chars, oldLen);
	if (checkEnt && !(entry->te_type & NS_ENTITY_CHK)) {
		checkEnt = _memcpyCheck8(
			(xmlbyte_t*)(newText + oldLen),
			(xmlbyte_t*) text, copyLen,
			isCharacters, nrb, false);
		if (checkEnt)
			entry->te_type |= NS_ENTITY_CHK;
	} else {
		// add trailing null for this copy
		_memcpyCheckLen(newText + oldLen, text, copyLen);
	}
	if ((entry->te_type & NS_DONTDELETE) == 0)
		nsNodeDeallocate(entry->te_text.t_chars);
	else
		entry->te_type &= ~NS_DONTDELETE;
	entry->te_text.t_chars = newText;
	entry->te_text.t_len = newLen - 1; // subtract null
	// NOTE: tl_len is ONLY used for marshaling, which means
	// it's a *byte* character count.
	list->tl_len += len; // trailing null already accounted for
	return allocLen;
}

/*
 * Add text of the form, name0value0 to a text node.
 * Return total number of characters used, including all
 * nulls.  Value can be null, in which case, use a null string
 */
static const xmlbyte_t nullUTF8[1] = { 0 };

static size_t
_addMultiText(const xmlbyte_t *name,
	      const xmlbyte_t *value,
	      nsText_t *text, bool *hasEntity)
{
	size_t nlen, vlen, ncopyLen, vcopyLen, allocLen;
	// operate in byte lengths
	if (value == 0)
		value = nullUTF8;
	nlen = NsUtil::nsStringLen(name);
	vlen = NsUtil::nsStringLen(value);
	allocLen = nlen + vlen + 2;
	ncopyLen = nlen + 1;
	vcopyLen = vlen + 1;

	text->t_chars =	(xmlbyte_t *) nsNodeAllocate(allocLen);
	text->t_len = nlen + vlen + 1; // don't count final NULL

	// copy to format, name0value0, only do entity check on value
	// hasEntity is ONLY non-zero for attributes...
	memcpy(text->t_chars, name, ncopyLen);
	if (hasEntity) {
		size_t nrb = 0;
		*hasEntity = _memcpyCheck8(
			((xmlbyte_t*)text->t_chars) +
			ncopyLen, (const xmlbyte_t*)value,
			vcopyLen, isAttrVal, nrb, true);
	} else {
		// Don't use memcpyCheckLen() in this path -- all
		// lengths come via strlen, not directly from user.
		memcpy(((xmlbyte_t*)text->t_chars) + ncopyLen,
		       value, vcopyLen);
	}
	return (nlen + vlen + 2);
}

nsTextList_t *
NsNode::startElem(NsNode *child,
		  nsTextList_t *textList)
{
	DBXML_ASSERT(!child->getTextList());
	child->setTextList(textList);
	nd_header.nh_flags |= NS_HASCHILD;
	if (textList)
		child->nd_header.nh_flags |= NS_HASTEXT;
	if (!noNav()) {
		if (!nd_nav) {
			nd_nav = allocNav();
			setFlag(NS_DEALLOCNAV);
		}
        }
        // handle first child and child's parent
        // last child is handled in endElem
        child->getParentNid()->copyNid(getFullNid());
	return 0; // text list has been consumed
}

nsTextList_t *
NsNode::endElem(NsNode *parent, NsNode *previous,
		nsTextList_t *textList)
{
	if (textList) {
		nd_header.nh_flags |= (NS_HASTEXTCHILD|NS_HASTEXT);
		nsTextList_t *oldlist = getTextList();
		if (oldlist) {
			// append new list to old, and make
			// new entries child entries
			DBXML_ASSERT(oldlist->tl_nchild == 0);
			for (unsigned int i = 0; i < textList->tl_ntext; i++) {
				nsTextEntry_t *entry = &textList->tl_text[i];
				oldlist = addText(oldlist,
						  entry->te_text.t_chars,
						  entry->te_text.t_len,
						  entry->te_type,
						  false);
				oldlist->tl_nchild++;
			}
			setTextList(oldlist); // in case of reallocation
		} else {
			// all nodes are child nodes
			textList->tl_nchild = textList->tl_ntext;
			setTextList(textList); // eat the list
			textList = 0;
		}
	}
	DBXML_ASSERT(!hasTextChild() || getTextList()->tl_nchild != 0);
	// if not donated, free it
	if (textList)
		freeTextList(textList);

	//
	// Done with text, handle navigational structure, if not
	// otherwise indicated (e.g. by transient dom, which
	// maintains its own)
	//
	if (!noNav()) {
		if (previous) {
			DBXML_ASSERT(nd_nav);
			nd_nav->nn_lastChild.copyNid(
						    previous->getFullNid());
		}
	}

	//
	// Set parent's last descendant.  It's either
	// this node's last descendant, or this node itself
	// Parent is null for document element
	//
	if (parent && !parent->noNav()) {
		if (getLastDescendantNid()) {
			parent->getLastDescendantNidPtr()->copyNid(
				getLastDescendantNid());
		} else {
			parent->getLastDescendantNidPtr()->copyNid(
				getFullNid());
		}
	}
	return 0;
}

void
NsNode::clearNext()
{
	nd_header.nh_flags &= ~NS_HASNEXT;
}

void
NsNode::clearPrev()
{
	nd_header.nh_flags &= ~NS_HASPREV;
	if (!noNav()) {
		DBXML_ASSERT(nd_nav);
		nd_nav->nn_prev.freeNid();
		memset(&nd_nav->nn_prev, 0, sizeof(NsNid));
	} else
		DBXML_ASSERT(false); // find this path, if it exists -- GMF

}

void
NsNode::setNextPrev(NsNode *previous)
{
	// Need to set next/prev flags even if not copying
	// nids, so NsDom will function correctly -- materializing
	// text on demand.
	previous->setFlag(NS_HASNEXT);
	setFlag(NS_HASPREV);
	if (!noNav()) {
		if (!nd_nav) {
			nd_nav = allocNav();
			setFlag(NS_DEALLOCNAV);
		}
		nd_nav->nn_prev.copyNid(previous->getFullNid());
	} else
		DBXML_ASSERT(false); // find this path, if it exists -- GMF

}

// use child == 0 to clear
void
NsNode::setLastChild(NsNode *child)
{
	if (!noNav()) {
		if (!nd_nav) {
			nd_nav = allocNav();
			setFlag(NS_DEALLOCNAV);
		}
		if (child) {
			DBXML_ASSERT(hasChildElem());
			nd_nav->nn_lastChild.copyNid(child->getFullNid());
		} else {
			nd_nav->nn_lastChild.freeNid();
			memset(&nd_nav->nn_lastChild, 0, sizeof(NsNid));
		}
	}

	if (!child) // no more children
		nd_header.nh_flags &= ~NS_HASCHILD;
}

void
NsNode::setLastDescendantNid(const NsFullNid *nid)
{
	if (hasChildElem())
		getLastDescendantNidPtr()->copyNid(nid);
	else {
		getLastDescendantNidPtr()->freeNid();
		memset(getLastDescendantNidPtr(), 0, sizeof(NsNid));
	}
}

/*
 * Set attribute in the list.
 * If donate is true:
 *   o the string, starting at name, must be of the form, "name0value0"
 *   o value must point to the correct string
 *   o len must be set to indicate the length of the string, including both nulls
 */
static nsAttr_t *
_setAttr(NsNode *node, uint32_t index,
	 const xmlbyte_t *name, const xmlbyte_t *value,
	 bool donate, size_t len, bool specified)
{
	nsAttrList_t *attrList = node->getAttrList();
	nsAttr_t *attr =  &(attrList->al_attrs[index]);

	if(attr->a_name.n_text.t_chars != 0)
		nsNodeDeallocate(attr->a_name.n_text.t_chars);

	// add both name/value to a_name.n_text
	attr->a_name.n_prefix = NS_NOPREFIX;
#ifdef NS_USE_SCHEMATYPES	
	attr->a_name.n_type = NS_NOTYPE;
#endif
	attr->a_uri = NS_NOURI;
	attr->a_flags = 0;
	if (donate) {
		DBXML_ASSERT(len);
		attr->a_name.n_text.t_chars = (xmlbyte_t *) name;
		attr->a_name.n_text.t_len = len - 1; // len includes trailing null
		attr->a_value = (xmlbyte_t *)value;
	} else {
		// do real work
		bool hasEntity = false;
		len = _addMultiText(name, value,
				    &(attr->a_name.n_text),
				    &hasEntity);
		xmlbyte_t *tptr8 =
			(xmlbyte_t *) attr->a_name.n_text.t_chars;
		while (*tptr8++); // get past name
		attr->a_value = tptr8;
		if (hasEntity)
			attr->a_flags |= NS_ATTR_ENT;
	}
	if (!specified)
		attr->a_flags |= NS_ATTR_NOT_SPECIFIED;
	attrList->al_len += len;
	return attr;
}

int
NsNode::addAttr(NsDocument *doc, const xmlbyte_t *prefix,
		const xmlbyte_t *uri, const xmlbyte_t *localname,
		const xmlbyte_t *value, bool specified)
{
	nsAttrList_t *attrList = nd_attrs;

	nd_header.nh_flags |= NS_HASATTR;
	if ((attrList == 0) || (attrList->al_nattrs == attrList->al_max)) {
		attrList = _reallocAttrList(attrList);
		DBXML_ASSERT(attrList);
		nd_attrs = attrList;
	}

	uint32_t index = attrList->al_nattrs++;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);
	memset(attr, 0, sizeof(attr));
	
	attr = _setAttr(this, index,
			localname, value, false,
			0, specified);
	int32_t uriIndex = NS_NOURI;
	int32_t prefixIndex = NS_NOPREFIX;
	if (prefix == 0 &&
	    NsUtil::nsStringEqual(localname,
				  (const xmlbyte_t *)_xmlnsPrefix8)) {
		/* default ns -- xmlns="uri" */
		setFlag(NS_HASNSINFO);
	}
	if (NsUtil::nsStringEqual(prefix,
				  (const xmlbyte_t *)_xmlnsPrefix8)) {
		/* prefix mapping -- xmlns:xxx */
		setFlag(NS_HASNSINFO);
	}
	if (uri) {
		uriIndex = doc->addIDForString((const char *)uri,
					       ::strlen((const char *)uri));
		// xmlns="" has uri, but no prefix
		if (prefix) {
			prefixIndex = doc->addIDForString(
				(const char *)prefix,
				::strlen((const char *)prefix));
		}
	}
	setAttrPrefix(attr, prefixIndex);
	setAttrUri(attr, uriIndex);
#ifdef NS_USE_SCHEMATYPES		
	// TBD.  If the attribute has type info,
	// add it to namespace prefix,
	// and put prefix in a_name.n_type.
#endif
	return (int) index;
}

nsAttr_t *
NsNode::setAttr(NsDocument *doc, uint32_t index,
		const xmlch_t *prefix, const xmlch_t *uri,
		const xmlch_t *localname, const xmlch_t *value,
		bool specified)
{
	int32_t uriIndex = NS_NOURI;
	int32_t prefixIndex = NS_NOPREFIX;
	nsAttr_t *attr = 0;
	NsDonator tattr(localname, value, NULL, isAttrVal);
	attr = _setAttr(this, index,
			tattr.getStr(),
			tattr.getStr2(),
			true,
			tattr.getLen(), specified);
	if (tattr.getHasEntity())
		attr->a_flags |= NS_ATTR_ENT;

	if (prefix == 0 &&
	    NsUtil::nsStringEqual(localname,
				  _xmlnsPrefix16)) {
		/* default ns -- xmlns="uri" */
		setFlag(NS_HASNSINFO);
		attr->a_flags |= NS_ATTR_IS_DECL;
	}
	if (NsUtil::nsStringEqual(prefix,
				  _xmlnsPrefix16)) {
		/* prefix mapping -- xmlns:xxx */
		setFlag(NS_HASNSINFO);
		attr->a_flags |= NS_ATTR_IS_DECL;
	}
	if (uri) {
		XMLChToUTF8Null xuri(uri);
		uriIndex = doc->addIDForString(xuri.str(), xuri.len());
		// xmlns="" has uri, but no prefix
		if (prefix) {
			XMLChToUTF8Null xprefix(prefix);
			prefixIndex = doc->addIDForString(xprefix.str(),
							  xprefix.len());
		}
	}
	setAttrPrefix(attr, prefixIndex);
	setAttrUri(attr, uriIndex);
#ifdef NS_USE_SCHEMATYPES		
	// TBD.  If the attribute has type info,
	// add it to namespace prefix,
	// and put prefix in a_name.n_type.
#endif
	return attr;
}

int
NsNode::addAttr(NsDocument *doc, const xmlch_t *prefix,
		const xmlch_t *uri, const xmlch_t *localname,
		const xmlch_t *value, bool specified)
{
	nsAttrList_t *attrList = nd_attrs;

	nd_header.nh_flags |= NS_HASATTR;
	if ((attrList == 0) || (attrList->al_nattrs == attrList->al_max)) {
		attrList = _reallocAttrList(attrList);
		DBXML_ASSERT(attrList);
		nd_attrs = attrList;
	}

	uint32_t index = attrList->al_nattrs++;
	nsAttr_t *attr =  &(attrList->al_attrs[index]);
	memset(attr, 0, sizeof(*attr));

	setAttr(doc, index, prefix, uri, localname, value, specified);
	return (int) index;
}

size_t
NsNode::createPI(nsText_t *dest,
		 const xmlbyte_t *target, const xmlbyte_t *data,
		 bool donate, size_t len)
{
	if (donate) {
		DBXML_ASSERT(len);
		dest->t_chars = (xmlbyte_t *) target;
		dest->t_len = len - 1; // subtract trailing null
		return len;
	} else {
		return _addMultiText(target, data,
				     dest, 0);
	}
}

//
// if donate is true:
//  o string, starting at target, is of the form, target0data0
//  o len is the byte length of the entire string, including both nulls
//  o data is ignored, and can be null
//
//static
nsTextList_t *
NsNode::addPI(nsTextList_t *list,
	      const xmlbyte_t *target,
	      const xmlbyte_t *data, bool donate, size_t len)
{
	/* all this is duplicated from addText() */
	if ((list == 0) || (list->tl_ntext == list->tl_max)) {
		list = _reallocTextList(list);
		DBXML_ASSERT(list);
	}
        /* add to list */
	int index = list->tl_ntext++;
	nsTextEntry_t *entry = &list->tl_text[index];
	entry->te_type = NS_PINST;

	len = createPI(&entry->te_text, target,
		       data, donate, len);
	DBXML_ASSERT(len > 0);

	list->tl_len += len;
	return list;
}

//
// Update methods
//

// remove attribute at the specified index
// note that this leaves the NsNode in an odd state, with a "hole"
// in the attr list.  This is OK because the only thing done with
// this node is marshaling for re-write
void NsNode::removeAttr(int index)
{
	size_t attrSize;
	nsAttrList_t *attrList = nd_attrs;
	DBXML_ASSERT(attrList && (int) attrList->al_nattrs > index &&
		     (int) attrList->al_max > index);
	nsAttr_t *attr = &(attrList->al_attrs[index]);
	// tell marshal code to ignore this attribute.
	// Null value is the trigger (could add flag if necessary)
	attr->a_value = 0;
	attrList->al_nattrs--;
	if (attrList->al_nattrs == 0)
		nd_header.nh_flags &= ~NS_HASATTR;
	// Calculate the length of name, value, plus nulls
	attrSize = attr->a_name.n_text.t_len + 1;
	attrList->al_len -= attrSize;
}

// remove text at the specified index
// note that this leaves the NsNode in an odd state, with a "hole"
// in the text list.  This is OK because the only thing done with
// this node is marshaling for re-write
nsTextList_t * NsNode::removeText(int startIndex, int endIndex)
{
	// just remove the range, leaving a hole
	nsTextList_t *textList = nd_text;
	DBXML_ASSERT(endIndex >= startIndex);
	DBXML_ASSERT(textList && (int) textList->tl_ntext > endIndex &&
		     (int) textList->tl_max > endIndex);

	// mark entries deleted
	int firstChild = getFirstTextChildIndex();
	for (int i = startIndex; i <= endIndex; i++) {
		nsTextEntry_t *text = &(textList->tl_text[i]);
		// t_len doesn't include last trailing null
		textList->tl_len -= (text->te_text.t_len + 1);
		// tell marshal code to ignore this text entry;
		text->te_type = NS_DELETEDTEXT;
		if (firstChild >= 0 && i >= firstChild)
			textList->tl_nchild--;
		textList->tl_ntext--;
	}
	
	// adjust flags
	if (textList->tl_nchild == 0)
		nd_header.nh_flags &= ~NS_HASTEXTCHILD;
	if (textList->tl_ntext == 0)
		nd_header.nh_flags &= ~NS_HASTEXT;
	return textList;
}

// are there adjacent, compatible text nodes that can be coalesced?
bool NsNode::canCoalesceText() const
{
	int32_t numLeading = getNumLeadingText();
	int32_t numChild = getNumChildText();
	if (numLeading < 2 && numChild < 2)
		return false;

	nsTextEntry_t *entries = nd_text->tl_text;
	int i, lastType = -1;
	for (i = 0; i < numLeading; i++) {
		if (nsTextType(lastType) == NS_TEXT &&
		    nsTextType(entries[i].te_type) == NS_TEXT)
			return true;
		lastType = entries[i].te_type;
	}
	if (numChild) {
		lastType = -1;
		for (i = getFirstTextChildIndex(); i < getNumText(); i++) {
			if (nsTextType(lastType) == NS_TEXT &&
			    nsTextType(entries[i].te_type) == NS_TEXT)
				return true;
			lastType = entries[i].te_type;
		}
	}
	return false;
}

// copy attribute by reference
static void copyAttr(nsAttr_t *dest,
		     nsAttr_t *src)
{
	memcpy(dest, src, sizeof(nsAttr_t));
	dest->a_flags |= NS_ATTR_DONTDELETE;
}

nsAttrList_t *NsNode::copyAttrList(int numNewAttrs)
{
	nsAttrList_t *attrList = nd_attrs;
	nsAttrList_t *newList = allocAttrList(
		numNewAttrs + (attrList ? attrList->al_nattrs : 0));
	if (attrList) {
		for (uint32_t i = 0; i < attrList->al_nattrs; i++) {
			copyAttr(&newList->al_attrs[i],
				 &attrList->al_attrs[i]);
			newList->al_nattrs++;
		}
		newList->al_len = attrList->al_len;
	}
	nd_attrs = newList;
	return attrList;
}

// handle NULL oldlist -- the initial state for the node
// may have been "no attributes" and the temporary list still
// needs to be freed.
nsAttrList_t *NsNode::replaceAttrList(nsAttrList_t *oldlist,
				      bool freeOldList)
{
	if (oldlist != nd_attrs) {
		nsAttrList_t *toFree = nd_attrs;
		nd_attrs = oldlist;
		if (freeOldList) {
			DBXML_ASSERT(toFree);
			freeAttrList(toFree);
			toFree = 0;
		}
		return toFree;
	}
	return 0;
}

// handle NULL oldlist -- the initial state for the node
// may have been "no text list" and the temporary list still
// needs to be freed.
nsTextList_t *NsNode::replaceTextList(nsTextList_t *oldlist,
				      bool freeOldList)
{
	if (oldlist != nd_text) {
		nsTextList_t *toFree = nd_text;
		nd_text = oldlist;
		if (freeOldList) {
			DBXML_ASSERT(toFree);
			freeTextList(toFree);
			toFree = 0;
		}
		return toFree;
	}
	return 0;
}

void NsNode::renameElement(const char *name,
			   size_t nameLen,
			   int uri, int prefix)
{
	// replace URI index and prefix
	nd_header.nh_uriIndex = uri;
	nd_header.nh_name.n_prefix = prefix;
	// see if there's enough space; if not, allocate
	if (nameLen > nd_header.nh_name.n_text.t_len) {
		nd_header.nh_name.n_text.t_chars =
			(xmlbyte_t *)nsNodeAllocate(nameLen+1);
		setFlag(NS_DEALLOCNAME);
	}
	memcpy(nd_header.nh_name.n_text.t_chars,
	       name, nameLen + 1);
	nd_header.nh_name.n_text.t_len = nameLen;
	if (uri != NS_NOURI)
		setFlag(NS_HASURI);
	if (prefix != NS_NOPREFIX)
		setFlag(NS_NAMEPREFIX);
}

// Display routine used for logging.  It could be enhanced
// to show attributes and text if necessary.
void NsNode::displayNode(std::ostream &out) const
{
	NsNode *n = const_cast<NsNode*>(this);
	if (isDoc()) {
		out << "#doc";
	} else {
		if (hasNamePrefix())
			out << namePrefix() << ":";
		out << (const char *) n->getNameChars();
		if (n->getParentNid()) {
			out << ", pnid: ";
			n->getParentNid()->displayNid(out);
		}
		if (numAttrs())
			out << ", nattr: " << numAttrs();
	}
	out << ", flags 0x" << std::hex << getFlags() << std::dec;
	if (hasText()) {
		int32_t nleading = getNumLeadingText();
		int32_t nchild= getNumChildText();
		out << ", txt (lead,child): (" << nleading << ","<< nchild << ")";
	}
	if (hasPrev()) {
		out << ", prev: "; n->getPrevNid()->displayNid(out);
	} if (n->getLastChildNid() && !n->getLastChildNid()->isNull()) {
		out << ", lastChild: "; n->getLastChildNid()->displayNid(out);
	} if (n->getLastDescendantNid() && !n->getLastDescendantNid()->isNull()) {
		out << ", lastDesc: "; n->getLastDescendantNid()->displayNid(out);
	}
}

//
// NsNodeRef -- ref-counting wrapper for NsNode
//
NsNodeRef::NsNodeRef(const NsNode *node) : node_((NsNode*)node) {
	if (node_) node_->acquire();
}
NsNodeRef::NsNodeRef(const NsNodeRef &other) : node_(other.get()) {
	if (node_) node_->acquire();
}
NsNodeRef::~NsNodeRef() { if (node_) node_->release(); }
NsNodeRef &NsNodeRef::operator=(const NsNode *n) {
	if (node_ != n) {
		if (node_) node_->release();
		node_ = const_cast<NsNode*>(n);
		if (node_) node_->acquire();
	}
	return *this;
}
NsNodeRef &NsNodeRef::operator=(const NsNodeRef &other) {
	*this = other.get();
	return *this;
}
NsNode *NsNodeRef::get() const {
	return node_;
}
NsNode *NsNodeRef::operator->() const {
	return node_;
}
NsNode *NsNodeRef::operator*() const {
	return node_;
}

// the only not-inline method for NsNodeIndexNodeInfo
size_t NsNodeIndexNodeInfo::getNodeDataSize()
{
	return NsFormat::getNodeDataSize(node_);
}

//
// utilities and debug routines
//
#if defined(NS_DEBUG)

#define NS_CH_CAST xmlbyte_t
static const char crText[2] = {'\\','r'};
static const char lfText[2] = {'\\','n'};
static const char tabText[2] = {'\\','t'};
static const char spText[2] = {'s','p'};
static const unsigned char ch_CR = '\015';
static const unsigned char ch_LF = '\012';
static const unsigned char ch_Tab = '\027';
static const unsigned char ch_Space = ' ';
static const char *

_asWhiteSpace(const char *chars, char *buf)
{
	const char* ptr = chars;
	char *toptr = buf;
	bool isAllWS = true;
	while (*ptr) {
		if (*ptr == ch_CR) {
			memcpy(toptr, crText, 2);
			toptr += 2;
		} else if (*ptr == ch_LF) {
			memcpy(toptr, lfText, 2);
			toptr += 2;
		} else if (*ptr == ch_Tab) {
			memcpy(toptr, tabText, 2);
			toptr += 2;
		} else if (*ptr == ch_Space) {
			memcpy(toptr, spText, 2);
			toptr += 2;
		} else {
			*toptr++ = *ptr;
			isAllWS = false;
		}
		ptr++;
	}
	*toptr = 0;
	return (isAllWS ? buf : chars);
}

static const char *
_printFlags(uint32_t flags, char *buf)
{
	*buf = 0;
	if (flags  & NS_ALLOCATED)
		strcat(buf, "ALLOCATED|");
	if (flags & NS_HASCHILD)
		strcat(buf, "CHILD|");
	if (flags & NS_HASTEXTCHILD)
		strcat(buf, "TEXTCHILD|");
	if (flags & NS_HASNEXT)
		strcat(buf, "NEXT|");
	if (flags & NS_HASPREV)
		strcat(buf, "PREV|");
	if (flags & NS_HASATTR)
		strcat(buf, "ATTR|");
	if (flags & NS_HASTEXT)
		strcat(buf, "TEXT|");
	if (flags & NS_HASURI)
		strcat(buf, "HASURI|");
	if (flags & NS_LAST_IS_LAST_DESC)
		strcat(buf, "LAST_IS_LAST_DESC|");
	if (flags & NS_ISDOCUMENT)
		strcat(buf, "ISDOCUMENT|");
	if (flags & NS_HASNSINFO)
		strcat(buf, "NSINFO|");
	if (flags & NS_STANDALONE)
		strcat(buf, "STANDALONE|");
	size_t l = strlen(buf);
	if (buf[l-1] == '|')
		buf[l-1] = '\0';
	return buf;
}

static void
_printNav(nsNav_t *nav)
{
	std::ostringstream oss;
	oss << "    Nav:\n";
	if (!nav->nn_prev.isNull()) {
		oss << "\tPrev: ";
		nav->nn_prev.displayNid(oss);
		oss << "\n";
	}
	if (!nav->nn_lastChild.isNull()) {
		oss << "\tlastChild: ";
		nav->nn_lastChild.displayNid(oss);
		oss << "\n";
	}
	printf("%s", oss.str().c_str());
}

static void
_printText(nsTextEntry_t *entry, int index)
{
	const char *type = "text";
	const char *text = (char *)entry->te_text.t_chars;
	char *buf = (char *)malloc((entry->te_text.t_len + 1) << 1);
	if (!buf)
		return;

	printf("\t%d, len %zu ", index, entry->te_text.t_len);
	if (nsTextType(entry->te_type) == NS_COMMENT)
		type = "comment";
	else if (nsTextType(entry->te_type) == NS_CDATA)
		type = "cdata";
	else  if (nsTextType(entry->te_type) == NS_PINST) {
		/* PI is stored as target0data0 */
		const char *datap = text + NsUtil::nsStringLen((const unsigned char *)text) + 1;
		type = "pinst";
		printf("(%s): ", type);
		printf("%s=", _asWhiteSpace(text, buf));
		printf("%s\n", _asWhiteSpace(datap, buf));
		free(buf);
		return;
	}
	printf("(%s): %s\n", type, _asWhiteSpace((const char *)text, buf));
	free(buf);
}

void
NsNode::printNode(NsDocument *doc)
{
	char fbuf[512];
	const char *name = "#DOCUMENT";
	int32_t prefix = NS_NOPREFIX;
	int32_t typeIndex = NS_NOTYPE;
	printf("Node: ");
	if (!(nd_header.nh_flags & NS_ISDOCUMENT)) {
		name = (const char *)
			nd_header.nh_name.n_text.t_chars;
		prefix = nd_header.nh_name.n_prefix;
#ifdef NS_USE_SCHEMATYPES			
		typeIndex = nd_header.nh_name.n_type;
#endif
	}

	if (prefix != NS_NOPREFIX) {
		if(doc == 0)
			printf(" %d:", prefix);
		else
			printf(" %s:", doc->getStringForID(prefix));
	}
	printf("%s, level %d, %s, NS prefix %d",
	       name,
	       nd_level,
	       _printFlags(nd_header.nh_flags, fbuf),
	       prefix);
	if (typeIndex != NS_NOTYPE) {
		if(doc == 0)
			printf(", type: %d", typeIndex);
		else
			printf(", type: %s", doc->getStringForID(typeIndex));
	}
	printf("\n");
	std::ostringstream oss;
	oss << "    ";
	getNid().displayNid(oss);
	printf("%s\n", oss.str().c_str());
	if (nd_attrs) {
		nsAttrList_t *attrs = nd_attrs;
		printf("    Attrs (num, total len, max):  %d, %zu, %d\n",
		       attrs->al_nattrs, attrs->al_len, attrs->al_max);
		for (uint32_t i = 0; i < attrs->al_nattrs; i++) {
			nsText_t *text = &attrs->al_attrs[i].a_name.n_text;
			printf("\t");
			if (attrs->al_attrs[i].a_name.n_prefix != NS_NOPREFIX) {
				if(doc == 0)
					printf("%d:", attrs->al_attrs[i].
						a_name.n_prefix);
				else
					printf("%s:", doc->getStringForID(
						       attrs->al_attrs[i].
						       a_name.n_prefix));
			}
			const xmlbyte_t *value = (const xmlbyte_t *)attrs->
				al_attrs[i].a_value;
			printf("%s = %s", (const xmlbyte_t *)
			       text->t_chars, value);
#ifdef NS_USE_SCHEMATYPES				
			if (attrs->al_attrs[i].a_name.n_type != NS_NOTYPE) {
				if(doc == 0)
					printf(", attrtype: %d", attrs->al_attrs[i].
						a_name.n_type);
				else
					printf(", attrtype: %s",
						doc->getStringForID(
							attrs->al_attrs[i].
							a_name.n_type));
			}
#endif
			printf("\n");
		}
	}
	if (nd_text) {
		nsTextEntry_t *entries = nd_text->tl_text;
		printf("    Text nodes (num, total len, max):  %d, %zu, %d\n",
		       nd_text->tl_ntext,
		       nd_text->tl_len, nd_text->tl_max);
		for (uint32_t i = 0; i < nd_text->tl_ntext; i++) {
			_printText(&entries[i], i);
		}
	}
	if (nd_nav)
		_printNav(nd_nav);
}

#endif // NS_DEBUG

