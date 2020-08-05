//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "../DocID.hpp"
#include "../ScopedDbt.hpp"
#include "NsRawNode.hpp"

using namespace DbXml;

NsRawNode::NsRawNode() : data_(0), cid_(0), unmarshaled_(false)
{}

void NsRawNode::clear()
{
	int tcid = cid_; // save this
	// clear everything (uninitialized state is all 0)
	memset(this, 0, sizeof(NsRawNode));
	cid_ = tcid; // restore container ID
}

void NsRawNode::setNode(const DbXmlDbt &key, const DbXmlDbt &data)
{
	if (data_) clear();
	data_ = (const unsigned char *)data.data;
	unsigned char *ptr = (unsigned char *)key.data;
	ptr += did_.unmarshal(ptr);
	nid_ = ptr; // raw pointer to NID
	dataSize_ = data.size;
}

const NsNid NsRawNode::getLastDescendantID()
{
	initialize();
	return lastDescendant_;
}

#if 0
const unsigned char *NsRawNode::getNodeValue()
{
	initialize();
	return 0;
	// TBD
}

// valid for text types (text, comment, PI)
bool NsRawNode::isLeadingText()
{
	return false; // TBD GMF
}
#endif

// initialize element state from raw data, including:
//    nattrs, lastDescendant, level, namespace uri, node name
//
// Nid is set in setNode().
// Attribute and text info is unmarshaled on demand elsewhere.
// TBD: think about making this a function in NsFormat.
void NsRawNode::initialize_internal()
{
	DBXML_ASSERT(!unmarshaled_);
	// NOTE: this is cribbed from NsFormat, and
	// dependent on NsFormat2::unmarshalNodeData
	unsigned char *ptr = (unsigned char *)data_;
	DBXML_ASSERT(*ptr == NS_PROTOCOL_VERSION);
	++ptr; // get past protocol version
	ptr += NsFormat::unmarshalInt(ptr, &flags_);
	if (flags_ & NS_HASATTR)
		ptr += NsFormat::unmarshalInt(ptr, &nattrs_);
	else
		nattrs_ = 0;
	attrIndex_ = -1; // not initialized
	textIndex_ = -1;
	if (flags_ & NS_HASTEXT)
		ptr += NsFormat::unmarshalInt(ptr, &ntext_);
	flags_ &= ~(NS_LAST_IS_LAST_DESC);
	if (flags_ & NS_HASCHILD) {
		lastDescendant_ = ptr;
		while (*ptr++); // get past null
	} else {
		// last descendant is self
		lastDescendant_ = nid_;
	}
	if (flags_ & NS_ISDOCUMENT) {
		level_ = 0;
		name_ = ptr; // hack for seeking to text
	} else {
		ptr += NsFormat::unmarshalInt(ptr, &level_);
		// skip parent NID
		while (*ptr++);
		// ignore prefix, URI is the important value
		if (flags_ & NS_NAMEPREFIX)
			ptr += NsFormat::unmarshalInt(ptr,
						      &pfx_);
		else
			pfx_ = NS_NOPREFIX;
		if (flags_ & NS_HASURI)
			ptr += NsFormat::unmarshalInt(ptr,
						      &uri_);
		else
			uri_ = NS_NOURI;
#ifdef NS_USE_SCHEMATYPES
		// skip type
		if (flags_ & NS_NAMETYPE)
			ptr += NsFormat::unmarshalInt(ptr,
						      &dummy);
		if (flags_ & NS_UNIONTYPE)
			ptr += NsFormat::unmarshalInt(
				ptr, &dummy);
#endif
		// find name string
		name_ = ptr;
	}
	unmarshaled_ = true;
}

// Attribute iteration...
//
// Format:
//   name text
//   if (text nodes)
//     int skip size, and plus 1 if has text children
//   if (attributes)
//      skip size
//      For each attr:
//        flags, prefix?, uri?, type?, name0value0

// seek from name pointer to start of attributes.

static const unsigned char *seekToAttrs(const unsigned char *current,
					u_int32_t flags)
{
	DBXML_ASSERT(current);
	current += NsUtil::nsStringLen(current) + 1;
	int32_t skip;
	if (flags & NS_HASTEXT) {
		current += NsFormat::unmarshalInt(current, &skip);
		current += skip;
		if (flags & NS_HASTEXTCHILD) {
			// skip #text children
			current += NsFormat::unmarshalInt(current, &skip);
		}
	}
	// unmarshal skip size for attrs -- not used
	current += NsFormat::unmarshalInt(current, &skip);
	return current;
}

static const unsigned char *seekToAttrName(
	const unsigned char *current, nsAttr_t *attr)
{
	u_int32_t aflags;
	current += NsFormat::unmarshalInt(current, &aflags);
	attr->a_flags = aflags;
	
	if (aflags & NS_ATTR_PREFIX)
		current += NsFormat::unmarshalInt(current, &attr->a_name.n_prefix);
	else
		attr->a_name.n_prefix = NS_NOPREFIX;
	if (aflags & NS_ATTR_URI) {
		current += NsFormat::unmarshalInt(current, &attr->a_uri);
	} else
		attr->a_uri = NS_NOURI;
#ifdef NS_USE_SCHEMATYPES		
	if (aflags & NS_ATTR_TYPE) {
		current += NsFormat::unmarshalInt(
			current, &attr->a_name.n_type);
		if (aflags & NS_ATTR_UNION_TYPE)
			current += NsFormat::unmarshalInt(
				current, &attr->a_name.n_utype);
	}
#endif
	attr->a_name.n_text.t_chars = (xmlbyte_t *)current;
	return current;
}

// this function relies on the caller for maintaining state,
// and not going off the end of the attribute list.  That is,
// the caller MUST NOT call this function if there are no attributes
// left on the list.  If called with a null "current" parameter,
// assume that unmarshaling starts at the node name (name_)
const unsigned char *NsRawNode::getNextAttr(
	const unsigned char *current, nsAttr_t *attr,
	int index)
{
	initialize();
	DBXML_ASSERT(nattrs_ != 0 && (index < (int)nattrs_));
	if (index == -1) index = attrIndex_ + 1;
	if (index != (attrIndex_+1)) {
		current = 0; // reset to start
		attrIndex_ = -1;
	}
	if (!current) {
		++attrIndex_;
		current = seekToAttrs(name_, flags_);
		// current is pointing to first attribute
		current = seekToAttrName(current, attr);
	}
	while (attrIndex_ < index) {
		++attrIndex_;
		// start with current and move to next attribute.
		size_t len = NsUtil::nsStringLen(current) + 1;
		current += (len + NsUtil::nsStringLen(current + len) + 1);
		current = seekToAttrName(current, attr);
	}
	if (current)
		attr->a_value =
			(current + NsUtil::nsStringLen(current) + 1);
	return current;
}

// Text methods
// Format:
//   name text
//   if (text nodes)
//     int skip size, [num child text], (type, text, 0)+
//
static const unsigned char *seekToText(const unsigned char *current,
				       u_int32_t flags,
				       u_int32_t &nchildtext)
{
	DBXML_ASSERT(current);
	DBXML_ASSERT(flags & NS_HASTEXT);
	if (!(flags & NS_ISDOCUMENT))
		current += NsUtil::nsStringLen(current) + 1;
	int32_t skip;
	current += NsFormat::unmarshalInt(current, &skip);
	if (flags & NS_HASTEXTCHILD)
		current += NsFormat::unmarshalInt(current, &nchildtext);
	else
		nchildtext = 0;
	// current is pointing to first text node
	return current;
}

u_int32_t NsRawNode::getNumChildText()
{
	initialize();
	seekToText(name_, flags_, nchildtext_);
	return nchildtext_;
}

nsTextEntry_t *NsRawNode::getTextEntry(
	nsTextEntry_t *entry, int index)
{
	// assume entry is pointing to last text, if it's valid
	const unsigned char *current =
		(const unsigned char *)entry->te_text.t_chars;
	if (!current || index <= 0 || index < textIndex_) {
		initialize();
		current = seekToText(name_, flags_, nchildtext_);
		textIndex_ = -1;
		if (index < 0)
			index = 0;
	} else {
		current += entry->te_text.t_len + 1;
	}
	DBXML_ASSERT(current);
	while (textIndex_ < index) {
			entry->te_type = *current++; // type
			size_t len = NsUtil::nsStringLen(current); // don't add null
			if (nsTextType(entry->te_type) == NS_PINST)
				len += NsUtil::nsStringLen(current + len + 1) + 1;
			// includes only embedded trailing nulls (e.g. PI)
			entry->te_text.t_len = len;
			entry->te_text.t_chars = (xmlbyte_t *)current;
			current += len + 1;
			textIndex_++;
	}
	return entry;
}


// convenience method to check for root or metadata nodes
// without doing the full assignment.  The down side is that
// DocID gets unmarshaled twice for useful nodes if this is used.
//static
bool NsRawNode::isRootOrMetaData(DbXmlDbt &key)
{
	DocID tdid;
	unsigned char *ptr = (unsigned char *)key.data;
	ptr += tdid.unmarshal(ptr);
	NsNid tnid(ptr);
	if (tnid.isDocRootNid() || tnid.isMetaDataNid())
		return true;
	return false;
}
