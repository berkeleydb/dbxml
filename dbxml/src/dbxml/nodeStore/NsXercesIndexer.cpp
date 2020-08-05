//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsXercesIndexer.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "EventWriter.hpp"
#include "UTF8.hpp"
#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace DbXml;

//
// NsEventAttrList implementation for indexer.
//
// This class must transcode on demand, which means it's
// complexity is in tracking allocations for transcoded strings,
// for cleanup.
//
NsEventAttrListIndexer::~NsEventAttrListIndexer()
{
	struct attrString *list = _toFree;
	while (list) {
		struct attrString *next = list->next;
		::free(list);
		list = next;
	}
}

inline struct attrString *
NsEventAttrListIndexer::_allocString(const xmlch_t *str, size_t &nchars) const
{
	nchars = NsUtil::nsStringLen(str) + 1;
	size_t len = nchars * 3;
	struct attrString *ret = (struct attrString *)
		NsUtil::allocate(sizeof(struct attrString) + len);
	ret->next = _toFree;
	_toFree = ret;
	return ret;
}

const xmlbyte_t *
NsEventAttrListIndexer::prefix(int index) const
{
	if (_alist) {
		const xmlch_t *lprefix = _alist->prefix(index);
		if (lprefix) {
			size_t nchars;
			struct attrString *asPrefix = _allocString(lprefix, nchars);
			xmlbyte_t *dest = asPrefix->s;
			NsUtil::nsToUTF8(&dest, lprefix, nchars, nchars*3);
			return asPrefix->s;
		}
	}
	return 0;
}

const xmlbyte_t *
NsEventAttrListIndexer::localName(int index) const
{
	if (_alist) {
		const xmlch_t *lname = _alist->localName(index);
		if (lname) {
			size_t nchars;
			struct attrString *asName = _allocString(lname, nchars);
			xmlbyte_t *dest = asName->s;
			NsUtil::nsToUTF8(&dest, lname, nchars, nchars*3);
			return asName->s;
		}
	}
	return 0;
}

const xmlbyte_t *
NsEventAttrListIndexer::value(int index) const {
	if (_alist) {
		const xmlch_t *lvalue = _alist->value(index);
		if (lvalue) {
			size_t nchars;
			struct attrString *asVal = _allocString(lvalue,
								nchars);
			xmlbyte_t *dest = asVal->s;
			NsUtil::nsToUTF8(&dest, lvalue, nchars, nchars*3);
			return asVal->s;
		}
	}
	return 0;
}

const xmlbyte_t *
NsEventAttrListIndexer::uri(int index) const
{
	if (_alist) {
		const xmlch_t *luri = _alist->uri(index);
		if (luri) {
			size_t nchars;
			struct attrString *asUri = _allocString(luri,
								nchars);
			xmlbyte_t *dest = asUri->s;
			NsUtil::nsToUTF8(&dest, luri, nchars, nchars*3);
			return asUri->s;
		}
	}
	return 0;
}

NsXercesIndexer::NsXercesIndexer(EventWriter *writer) :
	_writer(writer)
{
}

NsXercesIndexer::~NsXercesIndexer()
{
}

void NsXercesIndexer::startDocument(const xmlch_t *sniffedEncoding)
{
	if(_writer) {
		_writer->writeStartDocument(0, 0, 0);
	}
}

void NsXercesIndexer::endDocument()
{
	if(_writer) {
		_writer->writeEndDocument();
	}
}

void
NsXercesIndexer::startElement(const xmlch_t *localName,
			      const xmlch_t *prefix,
			      const xmlch_t *uri,
			      NsEventAttrList16 *attrs,
			      const uint32_t attrCount,
			      bool isEmpty)
{
	if (_writer) {
		if (uri && !(*uri))
			uri = 0;
		// Indexer does not use prefix or isEmpty, but set
		// prefix so that it can be added to the container's
		// dictionary to avoid write operations during queries.
		NsEventAttrListIndexer alist(attrs, localName, uri);
		XMLChToUTF8Null xlname(localName);
		XMLChToUTF8Null xuri(uri);
		XMLChToUTF8Null xpfx(prefix);
		_writer->writeStartElementWithAttrs(
			(const xmlbyte_t *)xlname.str(),
			(const xmlbyte_t *)xpfx.str(),			
			(const xmlbyte_t *)xuri.str(),
			attrCount,
			&alist,
			0, // no node
			isEmpty);
	}
}

void
NsXercesIndexer::endElement(const xmlch_t *localName,
			    const xmlch_t *prefix,
			    const xmlch_t *uri)
{
	if (_writer) {
		// indexer doesn't use args, so ignore them
		_writer->writeEndElementWithNode(0, 0, 0, 0);
	}
}

void
NsXercesIndexer::characters(const xmlch_t *characters,
			       size_t len, bool isCDATA,
			       bool needsEscape)
{
	if (_writer) {
		XMLChToUTF8 chars(characters, len);
		XmlEventReader::XmlEventType type =
			(isCDATA ? XmlEventReader::CDATA :
			 XmlEventReader::Characters);
		_writer->writeTextWithEscape(type,
					     (const unsigned char *)chars.str(),
					     chars.len(), false);
	}
}

