//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "dbxml/XmlPortability.hpp"

#ifdef DBXML_COMPRESSION

#ifndef __COMPRESSIONZLIB_HPP
#define	__COMPRESSIONZLIB_HPP

#include <dbxml/XmlData.hpp>
#include <dbxml/XmlCompression.hpp>

namespace DbXml
{
	
class CompressionZlib : public XmlCompression
{
public:
	CompressionZlib(){}
	virtual ~CompressionZlib(){}

	virtual bool compress(XmlTransaction &txn, const XmlData &source, XmlData &dest);

	virtual bool decompress(XmlTransaction &txn, const XmlData &source, XmlData &dest);
};
	
}

#endif

#endif

