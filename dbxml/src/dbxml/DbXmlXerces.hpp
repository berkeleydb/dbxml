//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#ifndef __DBXMLXERCES_HPP
#define __DBXMLXERCES_HPP

//
// common configuration for Xerces-C
//
#include <xercesc/util/XercesVersion.hpp>
#if _XERCES_VERSION >= 30000
#define XercesSize XMLSize_t
#define XercesSizeUint XMLSize_t  // xerces 2.8 was not consistent
#define XercesFilePos XMLFilePos
#define XercesFileLoc XMLFileLoc

#else
#define XercesSize size_t
#define XercesSizeUint unsigned int
#define XercesFilePos unsigned int
#define XercesFileLoc XMLSSize_t
#endif

#endif
