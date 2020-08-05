//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsUtil.hpp"
#include "NsWriter.hpp"
#include "NsConstants.hpp"
#include "EventReaderToWriter.hpp"
#include "EventReader.hpp"
#include "NsEvent.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace DbXml;

static void *NsWriterAllocate(size_t len)
{
	void *ret = ::malloc(len);
	if (!ret)
		throw XmlException(
			XmlException::NO_MEMORY_ERROR,
			"Malloc failed in NsWriter");
	return ret;
}

#define NsWriterFree ::free

//
// NsWriter implementation
//

NsWriter::NsWriter(NsStream *stream, bool writeRealEncoding)
	: _writeRealEncoding(writeRealEncoding),
	  _expandEntities(false), _stream(stream)
{
	reset();
}

NsWriter::NsWriter(bool writeRealEncoding)
	: _writeRealEncoding(writeRealEncoding),
	  _expandEntities(false), _stream(0)
{
	reset();
}

NsWriter::~NsWriter()
{
	// delete pre-defined namespace bindings
	Bindings::iterator end = _namespaces.end();
	for(Bindings::iterator it = _namespaces.begin();
	    it != end; ++it) {
		delete *it;
	}
}

void
NsWriter::reset()
{
	_entCount = 0;

	_prefixCount = 0;
	Bindings::iterator end = _namespaces.end();
	for(Bindings::iterator it = _namespaces.begin();
	    it != end; ++it) {
		delete *it;
	}
	_namespaces.clear();
	_elementInfo.clear();

	// Set the pre-defined namespaces
	Binding *new_binding = new Binding();
	new_binding->prefix = _xmlnsPrefix8;
	new_binding->uri = _xmlnsUri8;
	_namespaces.push_back(new_binding);

	new_binding = new Binding();
	new_binding->prefix = _xmlPrefix8;
	new_binding->uri = _xmlUri8;
	_namespaces.push_back(new_binding);

	// Default namespace defaults to no namespace
	new_binding = new Binding();
	new_binding->prefix = "";
	new_binding->uri = "";
	_namespaces.push_back(new_binding);
}

//
// Write to stream from NsEventReader
//
void
NsWriter::writeFromReader(EventReader &reader)
{
	DBXML_ASSERT(_stream);
	reader.setExpandEntities(_expandEntities);
	reader.setReportEntityInfo(!_expandEntities);
	EventReaderToWriter r2w(reader, false, true); /* don't own reader */
	r2w.setEventWriter(this);
	r2w.start();
}

void
NsWriter::writeStartDocument(const unsigned char *version,
			     const unsigned char *encoding,
			     const unsigned char *standalone)
{
	// trigger whether to write an XML decl based on existence
	// of version string
	if(_entCount == 0 && version) {
		_stream->write((const xmlbyte_t*)"<?xml version=\"");
		if (version)
			_stream->write(version);
		else
			_stream->write((const xmlbyte_t*)"1.1", 3); // Deflt to 1.1
		_stream->write((const xmlbyte_t*)"\"", 1);
		if(_writeRealEncoding) {
			// todo: add encoding interface(s), and put transcoding
			// support into the "stream" classes used for output.
			if (encoding) {
				_stream->write((const xmlbyte_t*)" encoding=\"");
				_stream->write(encoding);
				_stream->write((const xmlbyte_t*)"\"", 1);
			}
		} else {
			// For now, only UTF-8 is supported by NsWriter.
			// UTF-8 is the native encoding of node storage
			_stream->write((const xmlbyte_t*)" encoding=\"UTF-8\"");
		}
		if (standalone) {
			_stream->write((const xmlbyte_t*)" standalone=\"");
			_stream->write(standalone);
			_stream->write((const xmlbyte_t*)"\"", 1);
		}
		_stream->write((const xmlbyte_t*)"?>", 2);
	}
}

void
NsWriter::writeEndDocument()
{
}

void
NsWriter::writeStartElement(const unsigned char *localName,
			    const unsigned char *prefix,
			    const unsigned char *uri,
			    int numAttributes,
			    bool isEmpty)
{
	throw XmlException(XmlException::EVENT_ERROR,
			   "writeStartElement version not implemented by NsWriter");
}

void
NsWriter::writeStartElementWithAttrs(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri,
				     int attrCount,
				     NsEventAttrList *attrs,
				     IndexNodeInfo *ninfo,
				     bool isEmpty)
{
	if(_entCount == 0) {
		_elementInfo.push_back(ElementInfo());

		// check namespace binding
		bool define = checkNamespace(prefix, uri, /*isAttr*/false, /*isNamespace*/false);
		_elementInfo.back().setPrefix(prefix);

		// output element name
		_stream->write((const xmlbyte_t*)"<", 1);
		if(prefix && *prefix) {
			_stream->write(prefix);
			_stream->write((const xmlbyte_t*)":", 1);
		}
		_stream->write(localName);

		// output namespace declaration if needed
		if(define) {
			_stream->write((const xmlbyte_t*)" ", 1);
			_stream->write((const xmlbyte_t*)_xmlnsPrefix8);
			if(prefix && *prefix) {
				_stream->write((const xmlbyte_t*)":", 1);
				_stream->write(prefix);
			}
			_stream->write((const xmlbyte_t*)"=\"", 2);
			if(uri != 0 && *uri != 0) {
				// Escape the URI
				size_t uriLen = NsUtil::nsStringLen(uri);
				uriLen<<= 3; // *8
				xmlbyte_t *buf = (xmlbyte_t*)NsWriterAllocate(uriLen);
				uriLen = NsUtil::nsEscape((char*)buf, uri,
							  uriLen, /*isAttr*/true);
				_stream->write(buf, uriLen);
				NsWriterFree((void*)buf);
			}
			_stream->write((const xmlbyte_t*)"\"", 1);
		}

		// output attributes
		if(attrCount) {
			// TBD GMF -- think about how to push the writing in this
			// loop back to the source of the attributes -- it'd be
			// significantly  more efficient.  Either that, or a single
			// call to fill in a given attribute's info.
			for (int i = 0; i < attrs->numAttributes(); ++i) {
				if (!attrs->isSpecified(i))
					continue;
				const xmlbyte_t *value = attrs->value(i);
				size_t valLen = NsUtil::nsStringLen(value);
				bool freeValue = false;
				// check for escaping
				if (attrs->needsEscape(i)) {
					valLen <<= 3; // *8
					char *buf = (char*)NsWriterAllocate(valLen);
					valLen = NsUtil::nsEscape(buf, value,
								  valLen, true);
					value = (const xmlbyte_t *)buf;
					freeValue = true;
				}
				// check namespace binding.  Need temp
				// storage for uri, since attrs->prefix()
				// and attrs->uri() will return the same
				// pointer, overwritten
				const unsigned char *uri = attrs->uri(i);

				if(NsUtil::nsStringEqual(uri, (const xmlbyte_t*)_xmlnsUri8)) {
					// It's a namespace declaration
					prefix = attrs->prefix(i);
					prefix = (prefix == 0 || *prefix == 0) ? 0 : attrs->localName(i);
					uri = attrs->value(i);
					define = checkNamespace(prefix, uri, /*isAttr*/false, /*isNamespace*/true);
				} else {
					if(uri) {
						prefix = attrs->prefix(i);
						DBXML_ASSERT(uri != prefix);
						define = checkNamespace(prefix, uri, /*isAttr*/true, /*isNamespace*/false);
					} else {
						define = false;
						prefix = 0;
					}

					// output attribute
					_stream->write((const xmlbyte_t*)" ", 1);
					if(prefix && *prefix) {
						_stream->write(prefix);
						_stream->write((const xmlbyte_t*)":", 1);
					}
					_stream->write(attrs->localName(i));
					_stream->write((const xmlbyte_t*)"=\"", 2);
					_stream->write(value, valLen);
					_stream->write((const xmlbyte_t*)"\"", 1);
				}

				// output namespace declaration if needed
				if(define) {
					_stream->write((const xmlbyte_t*)" ", 1);
					_stream->write((const xmlbyte_t*)_xmlnsPrefix8);
					if(prefix && *prefix) {
						_stream->write((const xmlbyte_t*)":", 1);
						_stream->write(prefix);
					}
					_stream->write((const xmlbyte_t*)"=\"", 2);
					if(uri != 0 && *uri != 0) {
						// Escape the URI
						size_t uriLen = ::strlen(
							(const char *)uri);
						uriLen<<= 3; // *8
						xmlbyte_t *buf = (xmlbyte_t*)NsWriterAllocate(uriLen);
						uriLen = NsUtil::nsEscape((char*)buf, uri, uriLen, /*isAttr*/true);
						_stream->write(buf, uriLen);
						NsWriterFree((void*)buf);
					}
					_stream->write((const xmlbyte_t*)"\"", 1);
				}
				if (freeValue)
					NsWriterFree((void*)value);
			}
		}
		if (isEmpty) {
			_stream->write((const xmlbyte_t*)"/>", 2);
			removeElementBindings(); // endElement will NOT be called
		} else {
			_stream->write((const xmlbyte_t*)">", 1);
		}
	}
}
void
NsWriter::writeEndElement(const unsigned char *localName,
			  const unsigned char *prefix,
			  const unsigned char *uri)
{
	writeEndElementWithNode(localName, prefix, uri, 0);
}


void
NsWriter::writeEndElementWithNode(const xmlbyte_t *localName,
				 const xmlbyte_t *prefix,
				 const xmlbyte_t *uri,
				 IndexNodeInfo *ninfo)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"</");

		// lookup the prefix we used for the start tag
		prefix = _elementInfo.back().prefix;
		if (prefix && *prefix) {
			_stream->write(prefix);
			_stream->write((const xmlbyte_t*)":", 1);
		}
		_stream->write(localName);
		_stream->write((const xmlbyte_t*)">", 1);

		removeElementBindings();
	}
}

void
NsWriter::writeText(XmlEventReader::XmlEventType type,
		    const unsigned char *chars,
		    size_t length)
{
	bool needsEscape;
	if (type == XmlEventReader::CDATA || type == XmlEventReader::Comment ||
	    type == XmlEventReader::Whitespace)
		needsEscape = false;
	else
		needsEscape = true;
	if (length == 0 && chars)
		length = ::strlen((const char *)chars);
	writeTextWithEscape(type, chars, length, needsEscape);
}

void
NsWriter::writeTextWithEscape(XmlEventReader::XmlEventType type,
			      const unsigned char *chars,
			      size_t len,
			      bool needsEscape)
{
	if(_entCount == 0) {
		if (len == 0 && chars)
			len = ::strlen((const char *)chars);
		if ((type == XmlEventReader::Characters) ||
		    (type == XmlEventReader::Whitespace)) {
			char *buf = 0;
			if (needsEscape) {
				len <<= 3; // *8
				buf = (char*)NsWriterAllocate(len);
				len = NsUtil::nsEscape(buf, chars, len);
				chars = (const xmlbyte_t *)buf;
			}
			_stream->write(chars, len);
			if (buf)
				NsWriterFree(buf);
		} else if (type == XmlEventReader::CDATA) {
			_stream->write((const xmlbyte_t*)"<![CDATA[", 9);
			_stream->write(chars, len);
			_stream->write((const xmlbyte_t*)"]]>", 3);

		} else if(type == XmlEventReader::Comment) {
			_stream->write((const xmlbyte_t*)"<!--", 4);
			_stream->write(chars, len);
			_stream->write((const xmlbyte_t*)"-->", 3);
		} else {
		}
	}
}

void
NsWriter::writeProcessingInstruction(const unsigned char *target,
				     const unsigned char *data)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"<?", 2);
		_stream->write(target);
		if(data && *data) {
			_stream->write((const xmlbyte_t*)" ", 1);
			_stream->write(data);
		}
		_stream->write((const xmlbyte_t*)"?>", 2);
	}
}

void
NsWriter::writeDTD(const unsigned char *data, size_t len)
{
	if(_entCount == 0) {
		if (len == 0 && data)
			len = ::strlen((const char *)data);
		_stream->write(data, len);
	}
}

void
NsWriter::writeStartEntity(const unsigned char *name,
			   bool expandedInfoFollows)
{
	if (!_expandEntities) {
		if(_entCount == 0) {
			_stream->write((const xmlbyte_t*)"&", 1);
			_stream->write(name);
			_stream->write((const xmlbyte_t*)";", 1);
		}
		++_entCount;
	}
}

void
NsWriter::writeEndEntity(const unsigned char *name)
{
	if (!_expandEntities)
		--_entCount;
}

void
NsWriter::setExpandEntities(bool value)
{
	_expandEntities = value;
}

static inline std::string toString(const unsigned char *x)
{
	return x == 0 ? "" : (const char*)x;
}

void
NsWriter::removeElementBindings()
{
	bool hadNamespaces = _elementInfo.back().hasNamespaces;
	_elementInfo.pop_back();
	if(hadNamespaces) {
		Binding *binding = _namespaces.back();
		while(binding != 0) {
			delete binding;
			_namespaces.pop_back();
			binding = _namespaces.back();
		}
		_namespaces.pop_back(); // Pop the null
	}
}

static inline const xmlbyte_t *nullToEmpty(const xmlbyte_t *str)
{
	return str == 0 ? (const xmlbyte_t*)"" : str;
}

// Returns the prefix to use for the uri, and true if the namespace needs defining
bool
NsWriter::checkNamespace(const xmlbyte_t *&prefix,
	const xmlbyte_t *uri, bool isAttr, bool isNamespace)
{
	if(uri == 0 || *uri == 0) {
		if(prefix != 0 && *prefix != 0 && !isNamespace)
			throw XmlException(XmlException::EVENT_ERROR,
				"Prefix given with no namespace in NsWriter");

		if(isAttr) return false;
	}

	bool thisScope = false;
	if(NsUtil::nsStringEqual(lookupUri(nullToEmpty(prefix), thisScope), nullToEmpty(uri))) {
		// The prefix is already defined
		return false;
	}
	if(!thisScope && ((prefix != 0 && *prefix != 0) || !isAttr)) {
		// Define the prefix requested
		if(!_elementInfo.back().hasNamespaces) {
			_namespaces.push_back(0);
			_elementInfo.back().hasNamespaces = true;
		}

		Binding *new_binding = new Binding();
		new_binding->prefix = toString(prefix);
		new_binding->uri = toString(uri);
		_namespaces.push_back(new_binding);

		return true;
	}

	// Is there a prefix already bound to this namespace?
	const xmlbyte_t *found_prefix = lookupPrefix(uri);
	if(found_prefix) {
		prefix = found_prefix;
		return false;
	}

	// Otherwise redefine a new prefix for the namespace
	if(isNamespace) return false;

	// Try the default namespace
	bool useDefault = false;
	if(!isAttr) {
		lookupUri((const xmlbyte_t*)"", thisScope);
		useDefault = !thisScope;
	}

	if(!_elementInfo.back().hasNamespaces) {
		_namespaces.push_back(0);
		_elementInfo.back().hasNamespaces = true;
	}

	Binding *new_binding = new Binding();
	if(uri != 0)
		new_binding->uri = toString(uri);

	// If we can't use the default namespace,
	// make up a prefix
	if(!useDefault) {
		char buf[10];
		// Make up a new prefix
		std::string pfx = "ns_";
		::sprintf(buf, "%d", _prefixCount);
		pfx.append(buf);
		++_prefixCount;

		new_binding->prefix = pfx;
	}
	_namespaces.push_back(new_binding);

	prefix = (const xmlbyte_t*)new_binding->prefix.c_str();
	return true;
}

const xmlbyte_t *
NsWriter::lookupUri(const xmlbyte_t *prefix, bool &thisScope)
{
	// Nearer prefix bindings override further ones... - jpcs
	thisScope = _elementInfo.back().hasNamespaces;
	Bindings::reverse_iterator rend = _namespaces.rend();
	for(Bindings::reverse_iterator it = _namespaces.rbegin();
	    it != rend; ++it) {
		if(*it != 0) {
			if(NsUtil::nsStringEqual((const xmlbyte_t*)(*it)->
						 prefix.c_str(), prefix))
				return (const xmlbyte_t*)(*it)->uri.c_str();
		} else {
			thisScope = false;
		}
	}
	return 0;
}

const xmlbyte_t *
NsWriter::lookupPrefix(const xmlbyte_t *uri)
{
	// You can't bind a prefix to no namespace
	if(uri == 0 || *uri == 0) return 0;

	// It is assumed the binding is more likely to be nearer
	// the prefix use... - jpcs
	bool thisScope;
	Bindings::reverse_iterator rend = _namespaces.rend();
	for(Bindings::reverse_iterator it = _namespaces.rbegin();
	    it != rend; ++it) {
		if(*it != 0 && NsUtil::nsStringEqual((const xmlbyte_t *)(*it)->
						     uri.c_str(), uri)) {
			// Make sure the prefix has not been overridden
			if(NsUtil::nsStringEqual(lookupUri((const xmlbyte_t*)(*it)->
							   prefix.c_str(),
							   thisScope), uri)) {
				return (const xmlbyte_t*)(*it)->prefix.c_str();
			}
		}
	}
	return 0;
}
