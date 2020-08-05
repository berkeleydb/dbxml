/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlIndexLookup_dtor)
{
    php_dbxml_delete_XmlIndexLookup(rsrc->ptr);
}

/* {{{ proto XmlIndexLookup::XmlIndexLookup()
   Create a new index specification object */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, XmlIndexLookup)
{
  zend_error(E_ERROR, "XmlIndexLookup can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::getIndex()
   returns the index */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getIndex)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getIndex();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::setIndex()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setIndex)
{
  char *i;
  int i_len;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &i, &i_len)) {
    RETURN_FALSE;
  }
  This.setIndex(std::string(i, i_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::getNodeURI()
   returns the index */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getNodeURI)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getNodeURI();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::getNodeName()
   returns the index */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getNodeName)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getNodeName();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::setNode()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setNode)
{
  char *u, *n;
  int u_len, n_len;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &u, &u_len, &n, &n_len)) {
    RETURN_FALSE;
  }
  This.setNode(std::string(u, u_len), std::string(n, n_len));
} PHP_DBXML_METHOD_END()

/* }}} */
/* {{{ proto string XmlIndexLookup::getParentURI()
   returns the index */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getParentURI)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getParentURI();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::setParent()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setParent)
{
  char *u, *n;
  int u_len, n_len;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &u, &u_len, &n, &n_len)) {
    RETURN_FALSE;
  }
  This.setParent(std::string(u, u_len), std::string(n, n_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexLookup::getParentName()
   returns the index */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getParentName)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getParentName();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue getLowBoundValue()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getLowBoundValue)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  XmlValue value = This.getLowBoundValue();
  php_dbxml_set_XmlValue_object_pointer(return_value, value TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto long getLowBoundOperation()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getLowBoundOperation)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  XmlIndexLookup::Operation op = This.getLowBoundOperation();
  RETURN_LONG(op);
} PHP_DBXML_METHOD_END()
/* }}} */


/* {{{ proto null setLowBound()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setLowBound)
{
  zval *zv;
  long op;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);
  if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Ol", &zv, php_dbxml_XmlValue_ce, &op)) 
  {
    WRONG_PARAM_COUNT;
  }
  XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
  This.setLowBound(v, (XmlIndexLookup::Operation) op);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue getHighBoundValue()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getHighBoundValue)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  XmlValue value = This.getHighBoundValue();
  php_dbxml_set_XmlValue_object_pointer(return_value, value TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto long getHighBoundOperation()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getHighBoundOperation)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  XmlIndexLookup::Operation op = This.getHighBoundOperation();
  RETURN_LONG(op);
} PHP_DBXML_METHOD_END()
/* }}} */


/* {{{ proto null setHighBound()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setHighBound)
{
  zval *zv;
  long op;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);
  if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Ol", &zv, php_dbxml_XmlValue_ce, &op)) 
  {
    WRONG_PARAM_COUNT;
  }
  XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
  This.setHighBound(v, (XmlIndexLookup::Operation) op);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto long getContainer()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, getContainer)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  XmlContainer cont = This.getContainer();
  php_dbxml_set_XmlContainer_object_factory(return_value, cont TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */


/* {{{ proto null setContainer()
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, setContainer)
{
  zval *zv;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);
  if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O", &zv, php_dbxml_XmlContainer_ce)) 
  {
    WRONG_PARAM_COUNT;
  }
  XmlContainer c = php_dbxml_get_XmlContainer_object_pointer(zv TSRMLS_CC);
  This.setContainer(c);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlResults execute() 
 */
PHP_DBXML_METHOD_BEGIN(XmlIndexLookup, execute)
{
  zval *zqc,*zt;
  long flags = 0;
  PHP_DBXML_STUFFED_THIS(XmlIndexLookup);

#define SET_PTR(expr)  \
  XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC); \
  XmlResults r = expr; \
  object_init_ex(return_value, php_dbxml_XmlResults_ce); \
  php_dbxml_set_XmlResults_object_pointer(return_value, r TSRMLS_CC);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "O|l", &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {

    SET_PTR(This.execute(c, flags));
    php_dbxml_set_XmlResults_object_pointer(return_value, r TSRMLS_CC);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "OO", &zt, php_dbxml_XmlTransaction_ce, &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {

    XmlTransaction t = php_dbxml_get_XmlTransaction_object_pointer(zt TSRMLS_CC);
    SET_PTR(This.execute(t, c, flags));	  
  } else {
    WRONG_PARAM_COUNT;
  } 
} PHP_DBXML_METHOD_END()

function_entry php_dbxml_XmlIndexLookup_methods[] = {
  PHP_ME(XmlIndexLookup, XmlIndexLookup, NULL, 0)
  PHP_ME(XmlIndexLookup, getIndex, NULL, 0)
  PHP_ME(XmlIndexLookup, setIndex, NULL, 0)
  PHP_ME(XmlIndexLookup, getNodeURI, NULL, 0)
  PHP_ME(XmlIndexLookup, getNodeName, NULL, 0)
  PHP_ME(XmlIndexLookup, setNode, NULL, 0)
  PHP_ME(XmlIndexLookup, getParentURI, NULL, 0)
  PHP_ME(XmlIndexLookup, setParent, NULL, 0)
  PHP_ME(XmlIndexLookup, getParentName, NULL, 0)
  PHP_ME(XmlIndexLookup, getLowBoundValue, NULL, 0)
  PHP_ME(XmlIndexLookup, getLowBoundOperation, NULL, 0)
  PHP_ME(XmlIndexLookup, setLowBound, NULL, 0)
  PHP_ME(XmlIndexLookup, getHighBoundValue, NULL, 0)
  PHP_ME(XmlIndexLookup, getHighBoundOperation, NULL, 0)
  PHP_ME(XmlIndexLookup, setHighBound, NULL, 0)
  PHP_ME(XmlIndexLookup, getContainer, NULL, 0)
  PHP_ME(XmlIndexLookup, setContainer, NULL, 0)
  PHP_ME(XmlIndexLookup, execute, NULL, 0)
  {NULL,NULL,NULL}
};
