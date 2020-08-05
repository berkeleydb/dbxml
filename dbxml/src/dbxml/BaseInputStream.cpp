//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "BaseInputStream.hpp"
#include <dbxml/XmlException.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/util/BinInputStream.hpp>

using namespace DbXml;

BaseInputStream::BaseInputStream(XERCES_CPP_NAMESPACE_QUALIFIER InputSource *input)
	: input_(input),
	  binInputStream_(0)
{
}

BaseInputStream::~BaseInputStream()
{
	if (input_)
		delete input_;
	if (binInputStream_)
		delete binInputStream_;
}

unsigned int BaseInputStream::curPos() const
{
	return (unsigned int)getBinInputStream()->curPos();
}

unsigned int BaseInputStream::readBytes(char *toFill, 
					const unsigned int maxToRead)
{
	return (unsigned int) getBinInputStream()->readBytes((XMLByte*)toFill, maxToRead);
}

XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream *BaseInputStream::getBinInputStream() const
{
	if(binInputStream_ == 0) {
		try {
			binInputStream_ = input_->makeStream();
		}
		catch (...) { }

		// Under certain circumstances (like the file not existing),
		// the binInputStream_ will be null.
		if(binInputStream_ == 0)
			throw XmlException(XmlException::INVALID_VALUE, "The resource does not exist: malformed or non-existent stream source");
	}
	return binInputStream_;
}
