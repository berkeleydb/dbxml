//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

#include "URLInputStream.hpp"
#include "UTF8.hpp"
#include <xercesc/framework/URLInputSource.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

URLInputStream::URLInputStream(const std::string &baseId, 
                               const std::string &systemId, 
                               const std::string &publicId)
	: BaseInputStream(0)
{
	InputSource *is = new URLInputSource(UTF8ToXMLCh(baseId).str(),
					     UTF8ToXMLCh(systemId).str(),
					     UTF8ToXMLCh(publicId).str());
	setStream(is);
	
}

URLInputStream::URLInputStream(const std::string &baseId, 
                               const std::string &systemId)
	: BaseInputStream(0)
{
	InputSource *is = new URLInputSource(
		UTF8ToXMLCh(baseId).str(),
		UTF8ToXMLCh(systemId).str());
	setStream(is);
}

URLInputStream::~URLInputStream() {
}

