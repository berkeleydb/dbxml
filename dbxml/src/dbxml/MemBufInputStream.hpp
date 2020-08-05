//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

#ifndef __MEMBUFINPUTSTREAM_HPP
#define	__MEMBUFINPUTSTREAM_HPP

#include "BaseInputStream.hpp"
#include <string>

namespace DbXml
{
	
class DbXmlDbt;
class Document;

/**
 * Concrete implementation of a memory buffer streaming interface.
 */
class MemBufInputStream : public BaseInputStream
{
public:

	MemBufInputStream(const char *srcDocBytes,
			  const unsigned int byteCount,
			  const char *const bufId,
			  bool adoptBuffer = false);
	
	MemBufInputStream(DbXmlDbt *dbt,
			  const char *const bufId,
			  bool adoptDbt = false);

	virtual ~MemBufInputStream();
private:
	DbXmlDbt *dbt_;
};

}

#endif
