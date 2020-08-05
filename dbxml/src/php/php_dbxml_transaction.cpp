/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlTransaction_dtor)
{
    php_dbxml_delete_XmlTransaction(rsrc->ptr);
}

/* {{{ proto XmlTransaction::XmlTransaction(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlTransaction, XmlTransaction)
{
  zend_error(E_ERROR, "XmlTransaction can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlTransaction::abort()
   Abort the transaction */
PHP_DBXML_METHOD_BEGIN(XmlTransaction, abort)
{
  PHP_DBXML_STUFFED_THIS(XmlTransaction);
  This.abort();
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlTransaction::commit([int flags])
   Commit the transaction */
PHP_DBXML_METHOD_BEGIN(XmlTransaction, commit)
{
  long flags = 0;
  PHP_DBXML_STUFFED_THIS(XmlTransaction);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)) {
    RETURN_FALSE;
  }
  This.commit(flags);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlTransaction XmlTransaction::createChild([int flags])
   Create a nested transaction */
PHP_DBXML_METHOD_BEGIN(XmlTransaction, createChild)
{
  long flags = 0;
  PHP_DBXML_STUFFED_THIS(XmlTransaction);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)) {
    RETURN_FALSE;
  }

  object_init_ex(return_value, php_dbxml_XmlTransaction_ce);
  XmlTransaction t = This.createChild(flags);
  php_dbxml_set_XmlTransaction_object_pointer(return_value, t TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */


function_entry php_dbxml_XmlTransaction_methods[] = {
  PHP_ME(XmlTransaction, XmlTransaction, NULL, 0)
  PHP_ME(XmlTransaction, abort, NULL, 0)
  PHP_ME(XmlTransaction, commit, NULL, 0)
  PHP_ME(XmlTransaction, createChild, NULL, 0)
  {NULL,NULL,NULL}
};
