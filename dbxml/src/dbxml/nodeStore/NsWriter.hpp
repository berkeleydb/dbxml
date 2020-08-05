//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSWRITER_HPP
#define __DBXMLNSWRITER_HPP

#include <iostream>
#include <vector>

#include "EventWriter.hpp"
#include "../Buffer.hpp"
#include "NsUtil.hpp"

namespace DbXml {

// forward
class EventReader;
	
class NsStream {
public:
	virtual ~NsStream() {}
	virtual void write(const xmlbyte_t *data) = 0;
	virtual void write(const xmlbyte_t *data, size_t len) = 0;
};

class OStreamNsStream : public NsStream {
public:
	OStreamNsStream(std::ostream &os) : _os(os) {}
	virtual void write(const xmlbyte_t *data)
		{ _os << (char*)data; }
	virtual void write(const xmlbyte_t *data, size_t len)
		{ _os << (char*)data; }
private:
	std::ostream &_os;
};

class StringNsStream : public NsStream {
public:
	StringNsStream(std::string &str) : _str(str) {}
	virtual void write(const xmlbyte_t *data)
		{ _str.append((char*)data); }
	virtual void write(const xmlbyte_t *data, size_t len)
		{ _str.append((char*)data, len); }
private:
	std::string &_str;
};

class BufferNsStream : public NsStream {
public:
	// give buffer some initial size to reduce reallocation
	BufferNsStream() : buffer(0, 512, false)  {}
	virtual ~BufferNsStream() {}
	virtual void write(const xmlbyte_t *data)
		{ write(data, NsUtil::nsStringLen(data)); }
	virtual void write(const xmlbyte_t *data, size_t len)
		{ buffer.write(data, len); }
	Buffer buffer;
};

/**
 * NsWriter
 *
 * An instance of EventWriter that is used to serialized node
 * storage documents. It uses the EventWriter interface.
 *
 */
class NsWriter : public EventWriter {
public:
	NsWriter(NsStream *stream,
		 bool writeRealEncoding = false);
	NsWriter(bool writeRealEncoding = false);
	virtual ~NsWriter();

	//
	// Write to stream from EventReader
	//
	void writeFromReader(EventReader &reader);

	//
	// the EventWriter interface
	//

	virtual void close() {} // no-op close

	// NsWriter doesn't care about sniffed encoding
	virtual void writeSniffedEncoding(const unsigned char *enc) {}

	// text, comments, CDATA, ignorable whitespace
	// length does not include trailing null
	virtual void writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       size_t length);
	virtual void writeTextWithEscape(XmlEventReader::XmlEventType type,
					 const unsigned char *text,
					 size_t length,
					 bool needsEscape);

	// processing Instruction
	virtual void writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data);

	// elements
	virtual void writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty);

	virtual void writeStartElementWithAttrs(const unsigned char *localName,
						const unsigned char *prefix,
						const unsigned char *uri,
						int numAttributes,
						NsEventAttrList *attrs,
						IndexNodeInfo *ninfo,
						bool isEmpty);

	virtual void writeEndElementWithNode(const unsigned char *localName,
					     const unsigned char *prefix,
					     const unsigned char *uri,
					     IndexNodeInfo *ninfo);
	
	virtual void writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri);
	
	// xml declaration, DTD
	virtual void writeDTD(const unsigned char *dtd, size_t length);
	virtual void writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone);
	virtual void writeEndDocument();

	// note the start of entity expansion
	virtual void writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows);

	// note the end of entity expansion
	virtual void writeEndEntity(const unsigned char *name);

	// end of EventWriter interface
	
	// class-specific implementation
	void setStream(NsStream *stream) { _stream = stream; }
	NsStream *getStream() const { return _stream; }

	void setExpandEntities(bool value); // defaults to false
	void reset(void);
private:
	struct ElementInfo {
		ElementInfo() : hasNamespaces(false), prefix(0) {}
		ElementInfo(const ElementInfo &other) :
			hasNamespaces(other.hasNamespaces), prefix(0) {
			if (other.prefix)
				prefix = NsUtil::nsStringDup(other.prefix, 0); 
		}
		~ElementInfo() {
			if (prefix) NsUtil::deallocate((void*)prefix);
		}
		void setPrefix(const xmlbyte_t *pfx) {
			if (pfx)
				prefix = NsUtil::nsStringDup(pfx, 0); 
		}
		bool hasNamespaces;
		const xmlbyte_t *prefix;
	};
	typedef std::vector<ElementInfo> ElementInfoVector;

	struct Binding {
		std::string prefix;
		std::string uri;
	};
	typedef std::vector<Binding*> Bindings;

	void storeElementBindings(NsEventAttrList *attrs,
				  const uint32_t attrCount,
				  bool hasNsInfo);
	void removeElementBindings();
	// Returns the prefix to use for the uri, and true if the namespace needs defining
	bool checkNamespace(const xmlbyte_t *&prefix, const xmlbyte_t *uri,
		bool isAttr, bool isNamespace);
	const xmlbyte_t *lookupUri(const xmlbyte_t *prefix, bool &thisScope);
	const xmlbyte_t *lookupPrefix(const xmlbyte_t *uri);
private:
	bool _writeRealEncoding;
	bool _expandEntities;
	uint32_t _entCount;
	NsStream *_stream;

	unsigned int _prefixCount;
	ElementInfoVector _elementInfo;
	Bindings _namespaces;
};

	
}

#endif
