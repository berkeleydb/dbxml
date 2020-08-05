//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLINDEXSPECIFICATION_HPP
#define	__XMLINDEXSPECIFICATION_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include "XmlValue.hpp"

#include <string>

namespace DbXml
{

class IndexSpecification;
class IndexSpecificationIterator;

class DBXML_EXPORT XmlIndexSpecification
{
public:
	enum Type {
		UNIQUE_OFF = 0x00000000,
		UNIQUE_ON = 0x10000000,

		PATH_NONE = 0x00000000,
		PATH_NODE = 0x01000000,
		PATH_EDGE = 0x02000000,

		NODE_NONE = 0x00000000,
		NODE_ELEMENT = 0x00010000,
		NODE_ATTRIBUTE = 0x00020000,
		NODE_METADATA = 0x00030000,

		KEY_NONE = 0x00000000,
		KEY_PRESENCE = 0x00000100,
		KEY_EQUALITY = 0x00000200,
		KEY_SUBSTRING = 0x00000300
	};

	/** @name Constructor */
	XmlIndexSpecification();

	/** @name For Reference Counting */
	XmlIndexSpecification(const XmlIndexSpecification &);
	XmlIndexSpecification &operator=(const XmlIndexSpecification &);
	virtual ~XmlIndexSpecification();

	/** @name Manipulation Methods */
	void addIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax);
	void addIndex(const std::string &uri, const std::string &name, const std::string &index);
	void deleteIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax);
	void deleteIndex(const std::string &uri, const std::string &name, const std::string &index);
	void replaceIndex(const std::string &uri, const std::string &name, Type type, XmlValue::Type syntax);
	void replaceIndex(const std::string &uri, const std::string &name, const std::string &index);

	/** @name Default Index Manipulation Methods */
	void addDefaultIndex(Type type, XmlValue::Type syntax);
	void addDefaultIndex(const std::string &index);
	void deleteDefaultIndex(Type type, XmlValue::Type syntax);
	void deleteDefaultIndex(const std::string &index);
	void replaceDefaultIndex(Type type, XmlValue::Type syntax);
	void replaceDefaultIndex(const std::string &index);

	bool getAutoIndexing() const;
	void setAutoIndexing(bool value);
	
	/** @name Search Methods */
	bool find(const std::string &uri, const std::string &name, std::string &index) const;
	std::string getDefaultIndex() const;

	/** @name Iteration Methods */
	bool next(std::string &uri, std::string &name, Type &type, XmlValue::Type &syntax);
	bool next(std::string &uri, std::string &name, std::string &index);
	void reset();

	/** @name Utility Methods */
	static XmlValue::Type getValueType(const std::string &index);
	
	/** @name Private Methods (for internal use) */
	// @{

	operator IndexSpecification &()
	{
		return *is_;
	}
	operator const IndexSpecification &() const
	{
		return *is_;
	}

	//@}

private:
	IndexSpecification *is_;
	IndexSpecificationIterator *isi_;
};

}

#endif

