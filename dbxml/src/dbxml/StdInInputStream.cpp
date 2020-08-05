//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "StdInInputStream.hpp"
#include <xercesc/framework/StdInInputSource.hpp>

using namespace DbXml;

StdInInputStream::StdInInputStream() : BaseInputStream(new XERCES_CPP_NAMESPACE_QUALIFIER StdInInputSource()) {
}

StdInInputStream::~StdInInputStream() {
}
