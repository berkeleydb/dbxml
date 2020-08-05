/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlStatistics_dtor)
{
    php_dbxml_delete_XmlStatistics(rsrc->ptr);
}

/* {{{ proto XmlStatistics::XmlStatistics(object dbenv [, string name])
   Create a new XmlStatistics object -- INVALID */
PHP_METHOD(XmlStatistics, XmlStatistics)
{
  zend_error(E_ERROR, "XmlStatistics can not be instantiated from PHP");
}
/* }}} */

#define DOUBLE_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlStatistics, method) \
{ \
  PHP_DBXML_STUFFED_THIS(XmlStatistics); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_DOUBLE(This.method()); \
} PHP_DBXML_METHOD_END()

DOUBLE_GETTER(getNumberOfIndexedKeys)
DOUBLE_GETTER(getNumberOfUniqueKeys)
DOUBLE_GETTER(getSumKeyValueSize)

function_entry php_dbxml_XmlStatistics_methods[] = {
  PHP_ME(XmlStatistics, XmlStatistics, NULL, 0)
  PHP_ME(XmlStatistics, getNumberOfIndexedKeys, NULL, 0)
  PHP_ME(XmlStatistics, getNumberOfUniqueKeys, NULL, 0)
  PHP_ME(XmlStatistics, getSumKeyValueSize, NULL, 0)
  {NULL,NULL,NULL}
};
