//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __STDININPUTSTREAM_HPP
#define	__STDININPUTSTREAM_HPP

#include "BaseInputStream.hpp"

namespace DbXml
{

/**
 * Concrete implementation of a stdin streaming interface
 */
class StdInInputStream : public BaseInputStream
{
public:
	StdInInputStream();
	virtual ~StdInInputStream();

};

}

#endif
