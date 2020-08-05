/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlQueryExpression_dtor)
{
    php_dbxml_delete_XmlQueryExpression(rsrc->ptr);
}

/* {{{ proto XmlQueryExpression::XmlQueryExpression(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlQueryExpression, XmlQueryExpression)
{
  zend_error(E_ERROR, "XmlQueryExpression can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlQueryExpression::getQuery()
   Gets the query */
PHP_DBXML_METHOD_BEGIN(XmlQueryExpression, getQuery)
{
  PHP_DBXML_STUFFED_THIS(XmlQueryExpression);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  std::string str = This.getQuery();
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlQueryExpression::getQueryPlan()
   Gets the query plan */
PHP_DBXML_METHOD_BEGIN(XmlQueryExpression, getQueryPlan)
{
  PHP_DBXML_STUFFED_THIS(XmlQueryExpression);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  std::string str = This.getQueryPlan();
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto bool XmlQueryExpression::isUpdateExpression()
   returns true if query is update */
PHP_DBXML_METHOD_BEGIN(XmlQueryExpression, isUpdateExpression) 
{
  PHP_DBXML_STUFFED_THIS(XmlQueryExpression);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  RETURN_BOOL(This.isUpdateExpression());
} PHP_DBXML_METHOD_END()
 /* }}} */

/* {{{ proto XmlResults XmlQueryExpression::execute()
   executes the query */
PHP_DBXML_METHOD_BEGIN(XmlQueryExpression, execute)
{
  zval *zqc = NULL, *zv = NULL, *zt = NULL;
  long flags = 0;
  PHP_DBXML_STUFFED_THIS(XmlQueryExpression);

#define SET_PTR(expr)  \
  XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC); \
  XmlResults r = expr; \
  object_init_ex(return_value, php_dbxml_XmlResults_ce); \
  php_dbxml_set_XmlResults_object_pointer(return_value, r TSRMLS_CC);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|l",
      &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {
    
    SET_PTR(This.execute(c, flags));

  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OO|l",
      &zv, php_dbxml_XmlValue_ce, &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {
    
    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    SET_PTR(This.execute(v, c, flags));
    
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OO|l",
      &zt, php_dbxml_XmlTransaction_ce, &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {

    XmlTransaction t = php_dbxml_get_XmlTransaction_object_pointer(zt TSRMLS_CC);
    SET_PTR(This.execute(t, c, flags));

  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOO|l",
      &zt, php_dbxml_XmlTransaction_ce, &zv, php_dbxml_XmlValue_ce, &zqc, php_dbxml_XmlQueryContext_ce, &flags)) {

    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    XmlTransaction t = php_dbxml_get_XmlTransaction_object_pointer(zt TSRMLS_CC);
    SET_PTR(This.execute(t, v, c, flags));
 
  } else {
    WRONG_PARAM_COUNT;
  }

#undef SET_PTR
} PHP_DBXML_METHOD_END()
/* }}} */

function_entry php_dbxml_XmlQueryExpression_methods[] = {
  PHP_ME(XmlQueryExpression, XmlQueryExpression, NULL, 0)
  PHP_ME(XmlQueryExpression, getQuery, NULL, 0)
  PHP_ME(XmlQueryExpression, getQueryPlan, NULL, 0)
  PHP_ME(XmlQueryExpression, isUpdateExpression, NULL, 0)
  PHP_ME(XmlQueryExpression, execute, NULL, 0)
  {NULL,NULL,NULL}
};
