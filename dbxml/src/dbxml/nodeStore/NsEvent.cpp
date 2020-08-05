//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsEvent.hpp"
#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "../UTF8.hpp"

using namespace DbXml;

//
// NsPushEventSourceTranslator implementation
//
NsPushEventSourceTranslator::NsPushEventSourceTranslator(
	NsPushEventSource16 *source,
	NsEventTranslator *translator)
	: source_(source), translator_(translator)
{
	source_->setNsEventHandler16(translator_);
}

NsPushEventSourceTranslator::~NsPushEventSourceTranslator()
{
	delete source_;
	delete translator_;
}

void NsPushEventSourceTranslator::start()
{
	source_->start();
}

EventWriter *NsPushEventSourceTranslator::getEventWriter(void) const
{
	return translator_->getEventWriter();
}

void NsPushEventSourceTranslator::setEventWriter(EventWriter *writer)
{
	translator_->setEventWriter(writer);
}

//
// NsEventHandler(16) implementation
//
NsEventAttrList16::~NsEventAttrList16()
{
}

void
NsEventHandler16::docTypeDecl(const xmlch_t *, size_t)
{
	
}

void
NsEventHandler16::startEntity(const xmlch_t *, size_t)
{
}

void
NsEventHandler16::endEntity(const xmlch_t *, size_t)
{
}

//
// NsEventNodeAttrList implementation
// NOTE: little/no range, or existence checking performed on accessors
//
NsEventNodeAttrList::NsEventNodeAttrList(nsAttrList_t *list, NsDocument &doc,
					 bool isUTF16)
	: _list(list),
	  _doc(doc),
	  _isUTF16(isUTF16),
	  _localnames(0),
	  _values(0)
{
}

NsEventNodeAttrList::~NsEventNodeAttrList()
{
	xmlbyte_t **i, **end;

	if(_localnames) {
		end = _localnames + _list->al_nattrs;
		for(i = _localnames; i != end; ++i) {
			if(*i != 0) NsUtil::deallocate(*i);
		}
		NsUtil::deallocate(_localnames);
	}

	if(_values) {
		end = _values + _list->al_nattrs;
		for(i = _values; i != end; ++i) {
			if(*i != 0) NsUtil::deallocate(*i);
		}
		NsUtil::deallocate(_values);
	}
}

const xmlbyte_t *
NsEventNodeAttrList::localName(int index) const
{
	if(_isUTF16) {
		if(_localnames == 0) {
			size_t size = _list->al_nattrs * sizeof(xmlbyte_t*);
			_localnames = (xmlbyte_t**)
				NsUtil::allocate(size);
			memset(_localnames, 0, size);
		}

		xmlbyte_t *result = _localnames[index];
		if(result == 0) {
			// Lazily transcode
			const xmlch_t *name =
				(const xmlch_t*)_list->
				al_attrs[index].a_name8;
			NsUtil::nsToUTF8(&_localnames[index],
					 name,
					 NsUtil::nsStringLen(name) + 1, 0);
			result = _localnames[index];
		}
		return result;
	}
	else {
		return (xmlbyte_t *)(_list->al_attrs[index].a_name8);
	}
}

const xmlbyte_t *
NsEventNodeAttrList::value(int index) const
{
	if(_isUTF16) {
		if(_values == 0) {
			size_t size = _list->al_nattrs *
				sizeof(xmlbyte_t*);
			_values = (xmlbyte_t**)
				NsUtil::allocate(size);
			memset(_values, 0, size);
		}

		xmlbyte_t *result = _values[index];
		if(result == 0) {
			// Lazily transcode
			const xmlch_t *value = (const xmlch_t*)_list->al_attrs[index].a_value;
			NsUtil::nsToUTF8(&_values[index],
					 value,
					 NsUtil::nsStringLen(value) + 1, 0);
			result = _values[index];
		}
		return result;
	}
	else {
		return (const xmlbyte_t *)(_list->al_attrs[index].a_value);
	}
}

int
NsEventNodeAttrList::numAttributes(void) const
{
	if (_list)
		return (_list->al_nattrs);
	return 0;
}

const xmlbyte_t *
NsEventNodeAttrList::prefix(int index) const
{
	int32_t prefix = _list->al_attrs[index].a_prefix;
	if (prefix == NS_NOPREFIX)
		return 0;
	return (const xmlbyte_t *) _doc.getStringForID(prefix);
}

const xmlbyte_t *
NsEventNodeAttrList::uri(int index) const
{
	int32_t uri = _list->al_attrs[index].a_uri;
	if (uri == NS_NOURI)
		return 0;
	return (const xmlbyte_t *) _doc.getStringForID(uri);
}

#ifdef NS_USE_SCHEMATYPES
const xmlbyte_t *

NsEventNodeAttrList::typeName(int index) const
{
	int type = _list->al_attrs[index].a_type;
	// TBD
}

const xmlbyte_t *
NsEventNodeAttrList::typeUri(int index) const
{
	int type = _list->al_attrs[index].a_type;
	// TBD
}
#endif

bool
NsEventNodeAttrList::needsEscape(int index) const
{
	nsAttr_t *attr = &_list->al_attrs[index];
	if (attr->a_flags & NS_ATTR_ENT)
		return true;
	return false;
}

bool
NsEventNodeAttrList::isSpecified(int index) const
{
	nsAttr_t *attr = &_list->al_attrs[index];
	if (attr->a_flags & NS_ATTR_NOT_SPECIFIED)
		return false;
	return true;
}
