/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlQueryContext_dtor)
{
    php_dbxml_delete_XmlQueryContext(rsrc->ptr);
}

/* {{{ proto XmlQueryContext::XmlQueryContext(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, XmlQueryContext)
{
  zend_error(E_ERROR, "XmlQueryContext can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlQueryContext::setNamespace(string prefix, string uri)
   sets the namespace */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, setNamespace)
{
  char *prefix, *uri;
  int prefix_len, uri_len;
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &prefix_len, &uri, &uri_len)) {
    RETURN_FALSE;
  }
  This.setNamespace(std::string(prefix, prefix_len), std::string(uri, uri_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlQueryContext::getNamespace(string prefix)
   Gets the namespace */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, getNamespace)
{
  char *prefix;
  int prefix_len;
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &prefix, &prefix_len)) {
    RETURN_FALSE;
  }
  std::string str = This.getNamespace(std::string(prefix, prefix_len));
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlQueryContext::removeNamespace(string prefix)
   Removes the namespace */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, removeNamespace)
{
  char *prefix;
  int prefix_len;
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &prefix, &prefix_len)) {
    RETURN_FALSE;
  }
  This.removeNamespace(std::string(prefix, prefix_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlQueryContext::clearNamespaces()
   Removes all namespaces */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, clearNamespaces)
{
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  This.clearNamespaces();
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlQueryContext::setVariableValue(string name,
   XmlValue value | XmlResults res)
   Sets the value of a variable */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, setVariableValue)
{
  char *name;
  int name_len;
  zval *zv, *zres;
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "sO",
					  &name, &name_len,
					  &zv, php_dbxml_XmlValue_ce)) {
    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    This.setVariableValue(std::string(name, name_len), v);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &zv)) {
	  This.setVariableValue(std::string(name, name_len), php_dbxml_wrap_zval(zv));
	  
  } else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sO",
					      &name, &name_len,
					      &zres, php_dbxml_XmlResults_ce)) {
    XmlResults res = php_dbxml_get_XmlResults_object_pointer(zres TSRMLS_CC);
    This.setVariableValue(std::string(name, name_len), res);
  } else {
    return;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue XmlQueryContext::getVariableValue(string name)
   Gets the value of a variable */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, getVariableValue)
{
  char *name;
  int name_len;
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len)) {
    RETURN_FALSE;
  }
  XmlValue v;
  
  if (This.getVariableValue(std::string(name, name_len), v)) {
    object_init_ex(return_value, php_dbxml_XmlValue_ce);
    php_dbxml_set_XmlValue_object_pointer(return_value, v TSRMLS_CC);
  } else {
    RETURN_FALSE;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlQueryContext::interruptQuery()
   Interrupt a query */
PHP_DBXML_METHOD_BEGIN(XmlQueryContext, interruptQuery)
{
  PHP_DBXML_STUFFED_THIS(XmlQueryContext);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  This.interruptQuery();
} PHP_DBXML_METHOD_END()
/* }}} */

PHP_DBXML_INT_GETTER_AND_SETTER(XmlQueryContext, ReturnType, XmlQueryContext::ReturnType, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlQueryContext, EvaluationType, XmlQueryContext::EvaluationType, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlQueryContext, QueryTimeoutSeconds, unsigned int, 0, 0);
PHP_DBXML_STRING_GETTER_AND_SETTER(XmlQueryContext, BaseURI);
PHP_DBXML_STRING_GETTER_AND_SETTER(XmlQueryContext, DefaultCollection);

function_entry php_dbxml_XmlQueryContext_methods[] = {
  PHP_ME(XmlQueryContext, XmlQueryContext, NULL, 0)
  PHP_ME(XmlQueryContext, getNamespace, NULL, 0)
  PHP_ME(XmlQueryContext, setNamespace, NULL, 0)
  PHP_ME(XmlQueryContext, removeNamespace, NULL, 0)
  PHP_ME(XmlQueryContext, clearNamespaces, NULL, 0)
  PHP_ME(XmlQueryContext, setVariableValue, NULL, 0)
  PHP_ME(XmlQueryContext, getVariableValue, NULL, 0)
  PHP_ME(XmlQueryContext, interruptQuery, NULL, 0)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlQueryContext, ReturnType)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlQueryContext, EvaluationType)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlQueryContext, QueryTimeoutSeconds)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlQueryContext, BaseURI)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlQueryContext, DefaultCollection)
  {NULL,NULL,NULL}
};

