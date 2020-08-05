/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

#define BOOL_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, method) \
{ \
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_BOOL(This.method()); \
} PHP_DBXML_METHOD_END()

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlIndexSpecification_dtor)
{
    php_dbxml_delete_XmlIndexSpecification(rsrc->ptr);
}

/* {{{ proto XmlIndexSpecification::XmlIndexSpecification()
   Create a new index specification object */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, XmlIndexSpecification)
{
  XmlIndexSpecification This;

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  php_dbxml_set_XmlIndexSpecification_object_pointer(getThis(), This TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::addIndex(...)
   Adds an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, addIndex)
{
  char *uri, *name, *index;
  int uri_len, name_len, index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &uri, &uri_len, &name, &name_len, &type, &syntax)) {
    This.addIndex(std::string(uri, uri_len), std::string(name, name_len), (XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss", &uri, &uri_len, &name, &name_len, &index, &index_len)) {
    This.addIndex(std::string(uri, uri_len), std::string(name, name_len), std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::deleteIndex(...)
   deletes an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, deleteIndex)
{
  char *uri, *name, *index;
  int uri_len, name_len, index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &uri, &uri_len, &name, &name_len, &type, &syntax)) {
    This.deleteIndex(std::string(uri, uri_len), std::string(name, name_len), (XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss", &uri, &uri_len, &name, &name_len, &index, &index_len)) {
    This.deleteIndex(std::string(uri, uri_len), std::string(name, name_len), std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::replaceIndex(...)
   replaces an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, replaceIndex)
{
  char *uri, *name, *index;
  int uri_len, name_len, index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &uri, &uri_len, &name, &name_len, &type, &syntax)) {
    This.replaceIndex(std::string(uri, uri_len), std::string(name, name_len), (XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss", &uri, &uri_len, &name, &name_len, &index, &index_len)) {
    This.replaceIndex(std::string(uri, uri_len), std::string(name, name_len), std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::addDefaultIndex(...)
   Adds an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, addDefaultIndex)
{
  char *index;
  int index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ll", &type, &syntax)) {
    This.addDefaultIndex((XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_len)) {
    This.addDefaultIndex(std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::deleteDefaultIndex(...)
   deletes an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, deleteDefaultIndex)
{
  char *index;
  int index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ll", &type, &syntax)) {
    This.deleteDefaultIndex((XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_len)) {
    This.deleteDefaultIndex(std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::replaceDefaultIndex(...)
   replaces an index */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, replaceDefaultIndex)
{
  char *index;
  int index_len;
  long type, syntax;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ll", &type, &syntax)) {
    This.replaceDefaultIndex((XmlIndexSpecification::Type)type, (XmlValue::Type)syntax);
  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_len)) {
    This.replaceDefaultIndex(std::string(index, index_len));
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexSpecification::find(string uri, string name)
   returns the indexing strategy, or false */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, find)
{
  char *uri, *name;
  int uri_len, name_len;
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len)) {
    RETURN_FALSE;
  }

  if (This.find(std::string(uri, uri_len), std::string(name, name_len), index)) {
    DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
  } else {
    RETURN_FALSE;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlIndexSpecification::getDefaultIndex()
   returns the default indexing strategy */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, getDefaultIndex)
{
  std::string index;
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);

  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }

  index = This.getDefaultIndex();
  DBXML_RETURN_STRINGL((char*)index.data(), index.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlIndexSpecification::getIndexes(...)
   iterates indices */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, getIndexes)
{
  PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);
  if (ZEND_NUM_ARGS() == 0) {
    std::string uri, name, index;
    array_init(return_value);
    while(This.next(uri, name, index)) {
      zval *el;
      MAKE_STD_ZVAL(el);
      array_init(el);
      add_assoc_stringl(el, "uri", (char *) uri.data(), (int)uri.length(), (int)1);
      add_assoc_stringl(el, "name", (char *) name.data(), (int)name.length(), (int)1);
      add_assoc_stringl(el, "index", (char *) index.data(), (int)index.length(), (int)1);
      add_next_index_zval(return_value, el);
    } 
    return;
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

BOOL_GETTER(getAutoIndexing);

/* {{{ proto XmlIndexSpecification::setAutoIndexing(...)
   iterates indices */
PHP_DBXML_METHOD_BEGIN(XmlIndexSpecification, setAutoIndexing)
{
	PHP_DBXML_STUFFED_THIS(XmlIndexSpecification);
	bool val;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val)) {
		return;
	}
	This.setAutoIndexing(val);
} PHP_DBXML_METHOD_END()
/* }}} */


function_entry php_dbxml_XmlIndexSpecification_methods[] = {
  PHP_ME(XmlIndexSpecification, XmlIndexSpecification, NULL, 0)
  PHP_ME(XmlIndexSpecification, addIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, deleteIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, replaceIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, addDefaultIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, deleteDefaultIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, replaceDefaultIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, find, NULL, 0)
  PHP_ME(XmlIndexSpecification, getDefaultIndex, NULL, 0)
  PHP_ME(XmlIndexSpecification, getIndexes, NULL, 0)
  PHP_ME(XmlIndexSpecification, getAutoIndexing, NULL, 0)
  PHP_ME(XmlIndexSpecification, setAutoIndexing, NULL, 0)
  {NULL,NULL,NULL}
};
