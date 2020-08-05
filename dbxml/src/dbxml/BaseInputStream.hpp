//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __BASEINPUTSTREAM_HPP
#define	__BASEINPUTSTREAM_HPP

#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class InputSource;
class BinInputStream;
XERCES_CPP_NAMESPACE_END

#include "dbxml/XmlInputStream.hpp"

namespace DbXml
{

/**
 * Base class designed for the user to derive from, to implement
 * aspects of a streaming input source
 */
class BaseInputStream : public XmlInputStream
{
public:
	virtual ~BaseInputStream();

	virtual unsigned int curPos() const;
	virtual unsigned int readBytes(char *toFill, 
				       const unsigned int maxToRead);

protected:
	BaseInputStream(XERCES_CPP_NAMESPACE_QUALIFIER InputSource *inputStream);
	void setStream(XERCES_CPP_NAMESPACE_QUALIFIER InputSource *inputStream) {
		input_ = inputStream;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream *getBinInputStream() const;
private:
	XERCES_CPP_NAMESPACE_QUALIFIER InputSource *input_;
	mutable XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream *binInputStream_;
};

}

#endif
