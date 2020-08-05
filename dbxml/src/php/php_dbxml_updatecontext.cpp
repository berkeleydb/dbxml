/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlUpdateContext_dtor)
{
    php_dbxml_delete_XmlUpdateContext(rsrc->ptr);
}

/* {{{ proto XmlUpdateContext::XmlUpdateContext()
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlUpdateContext, XmlUpdateContext)
{
  zend_error(E_ERROR, "XmlUpdateContext can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

function_entry php_dbxml_XmlUpdateContext_methods[] = {
  PHP_ME(XmlUpdateContext, XmlUpdateContext, NULL, 0)
  {NULL,NULL,NULL}
};
