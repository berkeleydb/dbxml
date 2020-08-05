//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLDOCUMENT_HPP
#define	__XMLDOCUMENT_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class Document;
class XmlEventReader;
	
class DBXML_EXPORT XmlDocument
{
public:
	/** @name For Reference Counting */
	XmlDocument();
	XmlDocument(const XmlDocument&);
	XmlDocument &operator=(const XmlDocument &);
	virtual ~XmlDocument();
	bool isNull() const { return document_ == 0; }
	
	/** @name Identifier Methods */
	void setName(const std::string &name);
	std::string getName() const;
	
	/** @name Content Methods */
	std::string &getContent(std::string &content) const;
	void setContent(const std::string &content);

	XmlData getContent() const;
	void setContent(const XmlData &content);

	/** @name ContentAs* Methods */
	void setContentAsXmlInputStream(XmlInputStream *adopted_str);
	// XmlInputStream is donated to caller
	XmlInputStream *getContentAsXmlInputStream() const;
	
	// get content as events for a reader
	void setContentAsEventReader(XmlEventReader &reader);
	XmlEventReader &getContentAsEventReader() const;

	// get content as events written to the XmlEventWriter
	void getContentAsEventWriter(XmlEventWriter &writer);
	
	/** @name MetaData Methods */
	/// Turns a lazy document into an eager document
	void fetchAllData();
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlValue &value) const;
	void setMetaData(const std::string &uri, const std::string &name,
			 const XmlValue &value);
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlData &value) const;
	void setMetaData(const std::string &uri, const std::string &name,
			 const XmlData &value);
	void removeMetaData(const std::string &uri, const std::string &name);

	/** @name MetaData Iterator Methods */
	XmlMetaDataIterator getMetaDataIterator() const;

	/** @name Private Methods (for internal use) */
	// @{
	
	XmlDocument(Document *document);
	operator Document &() const {
		return *document_;
	}
	operator Document *() const {
		return document_;
	}

	bool operator == (const XmlDocument &other) const;
	bool operator != (const XmlDocument &other) const;
	
	//@}
	
private:
	Document *document_;
};

}

#endif
