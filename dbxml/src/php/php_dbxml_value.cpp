/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

XmlValue php_dbxml_wrap_zval(zval *val)
{
  switch (Z_TYPE_P(val)) {
    case IS_LONG:     return XmlValue((double)Z_LVAL_P(val));
    case IS_DOUBLE:   return XmlValue(Z_DVAL_P(val));
    case IS_STRING:   return XmlValue(std::string(Z_STRVAL_P(val), Z_STRLEN_P(val)));
    case IS_BOOL:     return XmlValue(Z_LVAL_P(val) ? true : false);
    default:
      return XmlValue(false);
  }
}

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlValue_dtor)
{
    php_dbxml_delete_XmlValue(rsrc->ptr);
}

PHP_DBXML_METHOD_BEGIN(XmlValue, XmlValue)
{
  zval *val;
  XmlValue This;
  long type;
  char *value;
  char *URI;
  char * name;
  int valueLen, URILen, nameLen;
  
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O", &val, php_dbxml_XmlDocument_ce)) {
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(val TSRMLS_CC);
    This = XmlValue(doc);
    php_dbxml_set_XmlValue_object_pointer(getThis(), This TSRMLS_CC);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss", &URI, &URILen, &name, &nameLen, &value, &valueLen)) {
    This = XmlValue(std::string(URI, URILen), std::string(name, nameLen), std::string(value, valueLen));
    php_dbxml_set_XmlValue_object_pointer(getThis(), This TSRMLS_CC);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "z!", &val)) {
    This = php_dbxml_wrap_zval(val);
    php_dbxml_set_XmlValue_object_pointer(getThis(), This TSRMLS_CC);
  } else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &type, &value, &valueLen)) {
    This = XmlValue((XmlValue::Type)type, value);
    php_dbxml_set_XmlValue_object_pointer(getThis(), This TSRMLS_CC);
  } 
  RETURN_FALSE;
} PHP_DBXML_METHOD_END()
/* }}} */

#define STD_STRING_GETTER(method) \
static PHP_DBXML_METHOD_BEGIN(XmlValue, method) { \
  PHP_DBXML_STUFFED_THIS(XmlValue); \
  if (ZEND_NUM_ARGS()) { WRONG_PARAM_COUNT; } \
  std::string str = This.method(); \
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1); \
} PHP_DBXML_METHOD_END()

STD_STRING_GETTER(getTypeURI);
STD_STRING_GETTER(getTypeName);

STD_STRING_GETTER(getNodeName);
STD_STRING_GETTER(getNodeValue);
STD_STRING_GETTER(getNamespaceURI);
STD_STRING_GETTER(getPrefix);
STD_STRING_GETTER(getLocalName);

#define XMLVALUE_GETTER(method)  \
static PHP_DBXML_METHOD_BEGIN(XmlValue, method) { \
  PHP_DBXML_STUFFED_THIS(XmlValue); \
  if (ZEND_NUM_ARGS()) { WRONG_PARAM_COUNT; } \
  object_init_ex(return_value, php_dbxml_XmlValue_ce); \
  XmlValue v = This.method(); \
  php_dbxml_set_XmlValue_object_pointer(return_value, v TSRMLS_CC); \
} PHP_DBXML_METHOD_END()

XMLVALUE_GETTER(getParentNode);
XMLVALUE_GETTER(getFirstChild);
XMLVALUE_GETTER(getLastChild);
XMLVALUE_GETTER(getPreviousSibling);
XMLVALUE_GETTER(getNextSibling);
XMLVALUE_GETTER(getOwnerElement);

#define INT_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlValue, method) \
{ \
  PHP_DBXML_STUFFED_THIS(XmlValue); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_LONG((long)This.method()); \
} PHP_DBXML_METHOD_END()

INT_GETTER(getNodeType);
INT_GETTER(getType);

#define BOOL_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlValue, method) \
{ \
  PHP_DBXML_STUFFED_THIS(XmlValue); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_BOOL(This.method()); \
} PHP_DBXML_METHOD_END()

BOOL_GETTER(isNull);
BOOL_GETTER(isNumber);
BOOL_GETTER(isString);
BOOL_GETTER(isBoolean);
BOOL_GETTER(asBoolean);
BOOL_GETTER(isNode);

PHP_DBXML_METHOD_BEGIN(XmlValue, isType)
{
  long type;
  PHP_DBXML_STUFFED_THIS(XmlValue);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type)) {
    RETURN_FALSE;
  }

  RETURN_BOOL(This.isType((XmlValue::Type)type));
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, equals)
{
  zval *v;
  PHP_DBXML_STUFFED_THIS(XmlValue);
  XmlValue other;

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &v, php_dbxml_XmlValue_ce)) {
    RETURN_FALSE;
  }

  other = php_dbxml_get_XmlValue_object_pointer(v TSRMLS_CC);

  RETURN_BOOL(This.equals(other));
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, getAttributes)
{
  PHP_DBXML_STUFFED_THIS(XmlValue);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  object_init_ex(return_value, php_dbxml_XmlResults_ce);
  XmlResults r = This.getAttributes();
  php_dbxml_set_XmlResults_object_pointer(return_value, r TSRMLS_CC);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, asDocument)
{
  PHP_DBXML_STUFFED_THIS(XmlValue);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  object_init_ex(return_value, php_dbxml_XmlDocument_ce);
  XmlDocument d = This.asDocument();
  php_dbxml_set_XmlDocument_object_pointer(return_value, d TSRMLS_CC);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, asEventReader)
{
  PHP_DBXML_STUFFED_THIS(XmlValue);
  XmlEventReader &reader =  This.asEventReader();
  php_dbxml_set_XmlEventReader_object_factory(return_value, reader TSRMLS_CC);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, getNodeHandle)
{
  PHP_DBXML_STUFFED_THIS(XmlValue);
  std::string handle = This.getNodeHandle();
  DBXML_RETURN_STRINGL((char*)handle.data(), handle.length(), 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, asNumber)
{
  PHP_DBXML_STUFFED_THIS(XmlValue);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  RETURN_DOUBLE(This.asNumber());
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlValue, asString)
{
  std::string str;
  PHP_DBXML_STUFFED_THIS(XmlValue);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  str = This.asString();
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1);
} PHP_DBXML_METHOD_END()


function_entry php_dbxml_XmlValue_methods[] = {
  PHP_ME(XmlValue, XmlValue,        NULL, 0)
  PHP_ME(XmlValue, getTypeURI,      NULL, 0)
  PHP_ME(XmlValue, getTypeName,     NULL, 0)
  PHP_ME(XmlValue, getNodeName,     NULL, 0)
  PHP_ME(XmlValue, getNodeValue,    NULL, 0)
  PHP_ME(XmlValue, getNamespaceURI, NULL, 0)
  PHP_ME(XmlValue, getPrefix,       NULL, 0)
  PHP_ME(XmlValue, getLocalName,    NULL, 0)
  PHP_ME(XmlValue, getNodeType,     NULL, 0)
  PHP_ME(XmlValue, getParentNode,   NULL, 0)
  PHP_ME(XmlValue, getFirstChild,   NULL, 0)
  PHP_ME(XmlValue, getLastChild,    NULL, 0)
  PHP_ME(XmlValue, getPreviousSibling,    NULL, 0)
  PHP_ME(XmlValue, getNextSibling,        NULL, 0)
  PHP_ME(XmlValue, getAttributes,         NULL, 0)
  PHP_ME(XmlValue, getOwnerElement,       NULL, 0)
  PHP_ME(XmlValue, getType,       NULL, 0)
  PHP_ME(XmlValue, isNull,       NULL, 0)
  PHP_ME(XmlValue, isType,       NULL, 0)
  PHP_ME(XmlValue, isNumber,       NULL, 0)
  PHP_ME(XmlValue, isString,       NULL, 0)
  PHP_ME(XmlValue, isBoolean,       NULL, 0)
  PHP_ME(XmlValue, isNode,       NULL, 0)
  PHP_ME(XmlValue, asNumber,       NULL, 0)
  PHP_ME(XmlValue, asString,       NULL, 0)
  PHP_ME(XmlValue, asBoolean,       NULL, 0)
  PHP_ME(XmlValue, asDocument,       NULL, 0)
  PHP_ME(XmlValue, asEventReader,      NULL, 0)
  PHP_ME(XmlValue, getNodeHandle,      NULL, 0)
  PHP_ME(XmlValue, equals,       NULL, 0)
  {NULL,NULL,NULL}
};

