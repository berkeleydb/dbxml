//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LOCALFILEINPUTSTREAM_HPP
#define	__LOCALFILEINPUTSTREAM_HPP

#include "BaseInputStream.hpp"
#include <string>

namespace DbXml
{

/**
 * Concrete implementation of a local file streaming interface
 */
class LocalFileInputStream : public BaseInputStream
{
public:
	LocalFileInputStream(const std::string &filename);
	virtual ~LocalFileInputStream();
};

}

#endif
