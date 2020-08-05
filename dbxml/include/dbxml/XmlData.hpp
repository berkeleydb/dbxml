//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLDATA_HPP
#define	__XMLDATA_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"

#include "db.h" // for size_t primarily

namespace DbXml
{

class Buffer;

class DBXML_EXPORT XmlData
{
public:
	XmlData();
	//Create an XmlData as a wrapper for the given data
	XmlData(void *data, size_t size);
	//Copy the given data into the new XmlData
	XmlData(const XmlData &o);
	XmlData &operator=(const XmlData &o);
	virtual ~XmlData();

	//Copy size bytes of data into the buffer
	void set(const void *data, size_t size);
	//Copy size bytes of data to the end of the data in the buffer
	void append(const void *data, size_t size);

	//Get the data and its size
	void *get_data() const;
	size_t get_size() const;
	
	//Sets the size of the data in the buffer
	void set_size(size_t size);

	/// Allocate a buffer of size bytes
	void reserve(size_t size);
	size_t getReservedSize() const;

	/// Take the buffer from src and give it to this XmlData
	void adoptBuffer(XmlData &src);

	/** @name Private Methods (for internal use) */
	// @{
	Buffer **getBuffer()
	{
		return &buffer_;
	}
private:
	Buffer *buffer_;
};

}

#endif
