//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLINPUTSTREAM_HPP
#define	__XMLINPUTSTREAM_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"

namespace DbXml
{

class DBXML_EXPORT XmlInputStream
{
public:
	virtual ~XmlInputStream();

	virtual unsigned int curPos () const = 0;
	virtual unsigned int readBytes (char *toFill, 
		const unsigned int maxToRead) = 0;
protected:
	XmlInputStream() {}
private:
	XmlInputStream(const XmlInputStream &);
	XmlInputStream &operator=(const XmlInputStream &);
};

}

#endif

