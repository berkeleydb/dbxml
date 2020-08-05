//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLCONSTANTS_HPP
#define __DBXMLCONSTANTS_HPP

/*
 * External declaration of node storage globals
 */

#include "NsTypes.hpp"

static const xmlch_t xmlchColon = ':';

namespace DbXml {

extern const char *_xmlnsUri8;
extern const xmlch_t *_xmlnsUri16;
extern const char *_xmlUri8;
extern const char *_xmlnsPrefix8;
extern const xmlch_t *_xmlnsPrefix16;
extern const char *_xmlPrefix8;
extern const xmlch_t *_xmlPrefix16;
extern const char *_dbxmlPrefix8;
extern const xmlch_t _dbxmlPrefix16[];
extern const xmlbyte_t *_standYes;
extern const xmlbyte_t *_standNo;
extern const xmlbyte_t _decl8_1_0[];
extern const xmlbyte_t _decl8_1_1[];
extern const xmlch_t _decl_1_0[];
extern const xmlch_t _decl_1_1[];
#define NS_DECL_1_0 0
#define NS_DECL_1_1 1
extern const xmlch_t *_NsDecl[];
extern const xmlbyte_t *_NsDecl8[];
extern const char _docRootId[];
}

#endif
