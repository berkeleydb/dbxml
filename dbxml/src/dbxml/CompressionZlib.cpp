//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "CompressionZlib.hpp"

#ifdef DBXML_COMPRESSION

#include "zlib.h"
#include "nodeStore/NsFormat.hpp"

using namespace DbXml;

// 9 bytes are added to store the size of the decompressed data in the compressed data.
const static int BUFSIZE = 5;

bool CompressionZlib::compress(XmlTransaction &txn, 
							 const XmlData &source, 
							 XmlData &dest)
{
	//maximum size the compressed data can be

	uLongf destLen = (uLongf)(source.get_size() * 1.1) + 12;
	int err = 0;
	try {
		if (dest.getReservedSize() < ((size_t)destLen + BUFSIZE))
			dest.reserve((size_t)destLen + BUFSIZE);
		//Stored the size of the decompressed data before the compressed data
		xmlbyte_t buf[BUFSIZE];
		NsFormat::marshalInt(buf, (uint32_t)source.get_size());
		dest.set(&buf, BUFSIZE);
		if (source.get_size() == 0) //empty document
			return true;
		Bytef *input = (Bytef*)dest.get_data();
		input += BUFSIZE;
		uLong srcLen = (uLong)source.get_size();
		err = ::compress(input, &destLen, (Bytef *)source.get_data(), srcLen);
		if (err != Z_OK)
			return false;
		dest.set_size((size_t)destLen + BUFSIZE);
	} catch (...) {
		return false;
	}
	return true;
}

bool CompressionZlib::decompress(XmlTransaction &txn,
							   const XmlData &source, 
							   XmlData &dest)
{
	int err = 0;
	try {
		//Get the size of the uncompressed data
		uLongf destLen;
		uint32_t tlen;
		NsFormat::unmarshalInt((xmlbyte_t*)source.get_data(), &tlen);
		if (tlen == 0) { //empty document
			dest.set("\0", 0);
			return true;
		}
		destLen = (uLongf)tlen;
		if (dest.getReservedSize() < (size_t)destLen)
			dest.reserve((size_t)destLen);
		Bytef *input = (Bytef *)source.get_data();
		input += BUFSIZE;
		uLong srcLen = (uLong)(source.get_size() - BUFSIZE);
		err = ::uncompress((Bytef *)dest.get_data(), &destLen, input, srcLen);
		dest.set_size((size_t)destLen);
		if (err != Z_OK) 
			return false;
	} catch (...) {
		return false;
	}
	return true;
}

#endif
