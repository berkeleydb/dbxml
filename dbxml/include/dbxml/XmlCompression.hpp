//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// 

#ifndef __XMLCOMPRESSION_HPP
#define	__XMLCOMPRESSION_HPP

#include "XmlPortability.hpp"
#include "XmlData.hpp"

namespace DbXml
{

class XmlTransaction;
	
class DBXML_EXPORT XmlCompression
{

public:

	XmlCompression(){}

	virtual ~XmlCompression();

	virtual bool compress(XmlTransaction &txn, const XmlData &source, XmlData &dest) = 0;

	virtual bool decompress(XmlTransaction &txn, const XmlData &source, XmlData &dest) = 0;
};
	
}

#endif
