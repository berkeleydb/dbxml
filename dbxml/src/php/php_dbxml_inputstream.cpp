/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

/* {{{ XmlInputStream should be destroyed by calling putDocument or updateDocument */
ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlInputStream_dtor)
{
  //if (rsrc->ptr) {
  //  delete (XmlInputStream*)rsrc->ptr;
  //}
}
/* }}} */

/* {{{ proto XmlInputStream::XmlInputStream(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlInputStream, XmlInputStream)
{
  zend_error(E_ERROR, "XmlInputStream can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto int XmlInputStream::curPos()
   Get current file position */
PHP_DBXML_METHOD_BEGIN(XmlInputStream, curPos)
{
  PHP_DBXML_GETTHIS(XmlInputStream);
  RETURN_LONG(This->curPos());
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlInputStream::readBytes(int toread)
   Read a number of bytes and return them as a string */
PHP_DBXML_METHOD_BEGIN(XmlInputStream, readBytes)
{
  int toread;
  PHP_DBXML_GETTHIS(XmlInputStream);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &toread)) {
    RETURN_FALSE;
  }

  /* TODO: realloc down to the returned length ? */
  Z_TYPE_P(return_value) = IS_STRING;
  Z_STRVAL_P(return_value) = (char*)emalloc(toread);
  Z_STRLEN_P(return_value) = This->readBytes(Z_STRVAL_P(return_value), toread);
  Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = '\0';
} PHP_DBXML_METHOD_END()
/* }}} */


function_entry php_dbxml_XmlInputStream_methods[] = {
  PHP_ME(XmlInputStream, XmlInputStream, NULL, 0)
  PHP_ME(XmlInputStream, curPos, NULL, 0)
  PHP_ME(XmlInputStream, readBytes, NULL, 0)
  {NULL,NULL,NULL}
};
