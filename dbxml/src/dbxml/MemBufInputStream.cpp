//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "MemBufInputStream.hpp"
#include "Document.hpp"
#include "ScopedDbt.hpp"
#include <xercesc/framework/MemBufInputSource.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

MemBufInputStream::MemBufInputStream(const char * srcDocBytes,
                                     const unsigned int	byteCount,
                                     const char *const bufId,
                                     bool adoptBuffer)
	: BaseInputStream(0), dbt_(0)
{
	InputSource *is = new MemBufInputSource(
		(XMLByte*)srcDocBytes,
		byteCount, bufId, adoptBuffer);
	setStream(is);
}

MemBufInputStream::MemBufInputStream(DbXmlDbt *dbt,
                                     const char *const bufId,
                                     bool adoptDbt)
	: BaseInputStream(0), dbt_(adoptDbt ? dbt : 0)
{
	InputSource *is = new MemBufInputSource(
		(XMLByte*)dbt->data,
		dbt->size, bufId, false);
	setStream(is);
}

MemBufInputStream::~MemBufInputStream()
{
	if (dbt_)
		delete dbt_;
}

