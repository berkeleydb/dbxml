/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

#if PHP_MAJOR_VERSION <= 4
unsigned char second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
unsigned char third_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
#endif

class PHPDBXmlResolver: public XmlResolver {
public:
  zval *m_userspace;

  /* C++ magic to refcount m_userspace */

  PHPDBXmlResolver(zval *userspace)
  {
    m_userspace = userspace;
    ZVAL_ADDREF(m_userspace);
  }

  PHPDBXmlResolver(const PHPDBXmlResolver &o) {
    zval_ptr_dtor(&m_userspace);
    m_userspace = o.m_userspace;
    ZVAL_ADDREF(m_userspace);
  }

  PHPDBXmlResolver &operator=(const PHPDBXmlResolver &o) {
    zval_ptr_dtor(&m_userspace);
    m_userspace = o.m_userspace;
    ZVAL_ADDREF(m_userspace);
  }

  ~PHPDBXmlResolver() {
    zval_ptr_dtor(&m_userspace);
  }
  
  /* TODO: these need to invoke methods on m_userspace to obtain the results */
  bool getSecure() 
  {
    return false;
  }
 #define DEBUG()  fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
  bool resolveDocument(XmlTransaction *txn, XmlManager &mgr, const std::string &uri, XmlValue &result)
  {
DEBUG();
    TSRMLS_FETCH();
    bool status = false;
    zval *argv[2], *retval, *func;

    MAKE_STD_ZVAL(func);
    MAKE_STD_ZVAL(retval);
    MAKE_STD_ZVAL(argv[0]);
    MAKE_STD_ZVAL(argv[1]);
    
    ZVAL_STRING(func, "resolveDocument", (int)1);
    ZVAL_STRINGL(argv[0], (char*)uri.data(), (int)uri.length(), 1);

    if (SUCCESS == call_user_function(EG(function_table),
        &m_userspace, func, retval, 2, argv TSRMLS_CC)) {
      convert_to_long_ex(&retval);

      if (Z_LVAL_P(retval)) {
        /* convert argv[1] to an XmlResult */
        result = php_dbxml_wrap_zval(argv[1]);
        status = true;
      }
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&argv[0]);
    zval_ptr_dtor(&argv[1]);
    
    return status;
  }

  bool resolveCollection(XmlTransaction *txn, XmlManager &mgr, const std::string &uri, XmlResults &result)
  {
    bool status = false;
    TSRMLS_FETCH();
DEBUG();
    zval *argv[2], *retval, *func;
    MAKE_STD_ZVAL(func);
    MAKE_STD_ZVAL(retval);
    MAKE_STD_ZVAL(argv[0]);
    MAKE_STD_ZVAL(argv[1]);
    
    ZVAL_STRING(func, "resolveCollection", (int)1);
    ZVAL_STRINGL(argv[0], (char*)uri.data(), (int)uri.length(), 1);

    if (SUCCESS == call_user_function(EG(function_table),
        &m_userspace, func, retval, 2, argv TSRMLS_CC)) {

      if (Z_TYPE_P(retval) == IS_ARRAY) {
        /* convert argv[1] to an XmlResult collection */
        zval **datum;
        zend_hash_internal_pointer_reset(Z_ARRVAL_P(retval));
        while (SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(retval), (void**)&datum)) {
          result.add(php_dbxml_wrap_zval(*datum));
          zend_hash_move_forward(Z_ARRVAL_P(retval));
        }
        status = true;
      }
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&argv[0]);
    zval_ptr_dtor(&argv[1]);
    
    return status;
  }

  XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr, const std::string &schemaLocation, const std::string &nameSpace)
  {
    bool status = false;
    zval *argv[3], *retval, *func;
    TSRMLS_FETCH();
DEBUG();
    MAKE_STD_ZVAL(func);
    MAKE_STD_ZVAL(retval);
    MAKE_STD_ZVAL(argv[0]);
    MAKE_STD_ZVAL(argv[1]);
    MAKE_STD_ZVAL(argv[2]);
    
    ZVAL_STRING(func, "resolveSchema", (int)1);
    ZVAL_STRINGL(argv[0], (char*)schemaLocation.data(), (int)schemaLocation.length(), 1);
    ZVAL_STRINGL(argv[1], (char*)nameSpace.data(), (int)nameSpace.length(), 1);

    if (SUCCESS == call_user_function(EG(function_table),
        &m_userspace, func, retval, 3, argv TSRMLS_CC)) {
      convert_to_long_ex(&retval);

      if (Z_LVAL_P(retval)) {
        //result = std::string(Z_STRVAL_P(argv[2]), Z_STRLEN_P(argv[2]));
        status = true;
      }
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&argv[0]);
    zval_ptr_dtor(&argv[1]);
    zval_ptr_dtor(&argv[2]);
    
    //return status;
    return NULL;
  }

  XmlInputStream *resolveEntity(XmlTransaction *txn, XmlManager &mgr, const std::string &systemId, const std::string &publicId)
  {
    bool status = false;
    zval *argv[3], *retval, *func;
    TSRMLS_FETCH();
DEBUG();

    MAKE_STD_ZVAL(func);
    MAKE_STD_ZVAL(retval);
    MAKE_STD_ZVAL(argv[0]);
    MAKE_STD_ZVAL(argv[1]);
    MAKE_STD_ZVAL(argv[2]);
    
    ZVAL_STRING(func, "resolveEntity", (int)1);
    ZVAL_STRINGL(argv[0], (char*)systemId.data(), (int)systemId.length(), 1);
    ZVAL_STRINGL(argv[1], (char*)publicId.data(), (int)publicId.length(), 1);

    if (SUCCESS == call_user_function(EG(function_table),
        &m_userspace, func, retval, 3, argv TSRMLS_CC)) {
      convert_to_long_ex(&retval);
      if (Z_LVAL_P(retval)) {
        //result = std::string(Z_STRVAL_P(argv[2]), Z_STRLEN_P(argv[2]));
        status = true;
      }
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&argv[0]);
    zval_ptr_dtor(&argv[1]);
    zval_ptr_dtor(&argv[2]);
    
  //  return status;
    return NULL;
  }
};

XmlResolver *php_dbxml_wrap_XmlResolver(zval *user_resolver)
{
  return new PHPDBXmlResolver(user_resolver);
}

/* {{{ proto bool XmlResolver::resolveDocument(string uri, mixed &result)
   Resolve the given uri to an XmlValue. If the URI cannot be resolved, returns false, otherwise true */
PHP_METHOD(XmlResolver, resolveDocument)
{
  /* TODO: do a file_get_contents() ? */
  RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XmlResolver::resolveCollection(string uri, mixed &result)
   Resolve the given uri to an XmlResults. If the URI cannot be resolved, returns false, otherwise true */
PHP_METHOD(XmlResolver, resolveCollection)
{
  RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XmlResolver::resolveSchema(string schemaLocation, string namespace, string &result)
   Resolve the given schema location and namespace to a string. The string should be the contents of the schema that is resolved to. If the uri cannot be resolved by this XmlResolver, then the method should return false, otherwise it should return true. */
PHP_METHOD(XmlResolver, resolveSchema)
{
  RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool XmlResolver:;resolveEntity(string systemid, string publicid, string &result)
   Resolve the given system id and public id to a string. The string should be the contents of the entity (usually a DTD) that is resolved to. If the uri cannot be resolved by this XmlResolver, then the method should return false, otherwise it should return true. */
PHP_METHOD(XmlResolver, resolveEntity)
{
  RETURN_FALSE;
}
/* }}} */

function_entry php_dbxml_XmlResolver_methods[] = {
  PHP_ME(XmlResolver, resolveDocument,    second_arg_force_ref, 0)
  PHP_ME(XmlResolver, resolveCollection,  second_arg_force_ref, 0)
  PHP_ME(XmlResolver, resolveSchema,      third_arg_force_ref, 0)
  PHP_ME(XmlResolver, resolveEntity,      third_arg_force_ref, 0)
  {NULL, NULL, NULL}
};

