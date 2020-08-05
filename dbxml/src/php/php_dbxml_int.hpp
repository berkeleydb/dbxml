/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// this is here to work around a PHP build issue on Windows
#include <iostream>

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#ifdef PHP_WIN32
#include "php_db4.h"
#else
#include "ext/db4/php_db4.h"
#endif
#include "php_dbxml.h"
}
#include <exception>
#include "dbxml/DbXml.hpp"

using namespace DbXml;

#ifndef PHP_METHOD
# define PHP_METHOD(classname, name)                ZEND_NAMED_FUNCTION(ZEND_FN(classname##_##name))
# define PHP_ME(classname, name, arg_info, flags)   ZEND_NAMED_FE(name, ZEND_FN(classname##_##name), arg_info)
#endif

#define PHP_DBXML_METHOD_BEGIN(classname, name)  PHP_METHOD(classname, name) { try {

#ifdef PHP_DBXML_EXCEPTIONS

#define PHP_DBXML_METHOD_END()		 \
} catch(XmlException &xe) {		 \
  php_dbxml_throw_exception(xe TSRMLS_CC);         \
}                                        \
catch(std::exception &e) {				 \
  php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what()); \
  RETURN_FALSE; \
} \
}

#else

#define PHP_DBXML_METHOD_END()  \
} catch(std::exception &e) {				 \
  php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what()); \
  RETURN_FALSE; \
} \
}

#endif

// encapsulate calls to RETURN_STRINGL macro in PHP
#define DBXML_RETURN_STRINGL(d, l, s) \
    RETURN_STRINGL((char*)(d),(int)(l), (s))

#define DBXML_RETURN_STRING(d, s) \
    RETURN_STRING((char*)(d),(int)(s))

#define DBXML_RETURN_LONG(l) RETURN_LONG((long)(l))

static inline void php_dbxml_set_object_pointer(zval *object, int type, void *ptr TSRMLS_DC)
{
  zval *z;

  MAKE_STD_ZVAL(z);
  ZEND_REGISTER_RESOURCE(z, ptr, type);
  zend_hash_index_update(Z_OBJPROP_P(object), 0, &z, sizeof(z), NULL);
}

static inline void *php_dbxml_get_object_pointer(zval *object, int expect_type, zend_class_entry *ce, const char *classname TSRMLS_DC)
{
  zval **prop;
  void *ptr;
  int type;
  zend_class_entry *pce;
  int class_ok = 0;

  if (!object) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "must be called in a method context, not statically");
    return NULL;
  }

  if (Z_TYPE_P(object) != IS_OBJECT) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "not a valid %s instance", classname);
    return NULL;
  }

  for (pce = ce, class_ok = 0; pce != NULL; pce = pce->parent) {
    /* ugly hack lifted from zend_check_class in zend_API.c */
    if (pce->refcount == ce->refcount) {
      class_ok = 1;
      break;
    }
  }

  if (!class_ok) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "not a valid %s instance", classname);
    return NULL;
  }

  if (zend_hash_index_find(Z_OBJPROP_P(object), 0, (void**)&prop) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid %s object; resource was not found", classname);
    return NULL;
  }

  ptr = zend_list_find(Z_RESVAL_PP(prop), &type);
  
  if (expect_type != type) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid %s object; wrong class", classname);
    return NULL;
  }

  return ptr;
}

#define PHP_DBXML_GET(thing, class_type)   (class_type*)php_dbxml_get_object_pointer(thing, le_##class_type, php_dbxml_##class_type##_ce, #class_type TSRMLS_CC)
#define PHP_DBXML_GETTHIS(class_type)      \
  class_type *This = PHP_DBXML_GET(getThis(), class_type); \
  if (!This) { return; }

#define PHP_DBXML_ADDREF(object) \
    zval **prop; \
    if (zend_hash_index_find(Z_OBJPROP_P(object), 0, (void**)&prop) == FAILURE) { \
      php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object; resource was not found"); \
    } \
    zend_list_addref(Z_RESVAL_PP(prop)); 

/*
 * macros to handle the Xml* objects
 * "object_pointer" really means object reference (e.g. XmlDocument &)
 */

#define PHP_DBXML_STUFFED_INSTANCES(class_type) \
  struct php_dbxml_stuffed_##class_type { \
  public: class_type stuffed; \
  php_dbxml_stuffed_##class_type(class_type &s): stuffed(s) {} \
  }; \
  static inline void php_dbxml_set_##class_type##_object_factory(zval *object, class_type &instance TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = new struct php_dbxml_stuffed_##class_type(instance); \
    object_init_ex(object, php_dbxml_##class_type##_ce); \
    php_dbxml_set_object_pointer(object, le_##class_type, s TSRMLS_CC); \
  } \
  static inline void php_dbxml_set_##class_type##_object_pointer(zval *object, class_type &instance TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = new struct php_dbxml_stuffed_##class_type(instance); \
    php_dbxml_set_object_pointer(object, le_##class_type, s TSRMLS_CC); \
  } \
  static inline class_type & php_dbxml_get_##class_type##_object_pointer(zval *object TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = (struct php_dbxml_stuffed_##class_type *) \
        php_dbxml_get_object_pointer(object, le_##class_type, php_dbxml_##class_type##_ce, #class_type TSRMLS_CC); \
    if (s) { return s->stuffed; } /* does internal addref */ \
    else { throw new std::exception; } \
  } \
  static inline void php_dbxml_delete_##class_type(void *thing) { \
    struct php_dbxml_stuffed_##class_type *s = (struct php_dbxml_stuffed_##class_type *)thing; \
    delete s; \
  }

/*
 * this is nearly identical to STUFFED_INSTANCES, above, with the exception
 * that it stores an object reference, not an actual instance, which
 * changes the member variable to a reference, along with the
 * accessor's return value.  Everything else is identical.
 */
#define PHP_DBXML_STUFFED_REFERENCES(class_type) \
  struct php_dbxml_stuffed_##class_type { \
  public: class_type &stuffed; \
  php_dbxml_stuffed_##class_type(class_type &s): stuffed(s) {} \
  }; \
  static inline void php_dbxml_set_##class_type##_object_factory(zval *object, class_type &instance TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = new struct php_dbxml_stuffed_##class_type(instance); \
    object_init_ex(object, php_dbxml_##class_type##_ce); \
    php_dbxml_set_object_pointer(object, le_##class_type, s TSRMLS_CC); \
  } \
  static inline void php_dbxml_set_##class_type##_object_pointer(zval *object, class_type &instance TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = new struct php_dbxml_stuffed_##class_type(instance); \
    php_dbxml_set_object_pointer(object, le_##class_type, s TSRMLS_CC); \
  } \
  static inline class_type &php_dbxml_get_##class_type##_object_ref(zval *object TSRMLS_DC) { \
    struct php_dbxml_stuffed_##class_type *s = (struct php_dbxml_stuffed_##class_type *) \
        php_dbxml_get_object_pointer(object, le_##class_type, php_dbxml_##class_type##_ce, #class_type TSRMLS_CC); \
    if (s) { return s->stuffed; } /* does internal addref */ \
    else { throw new std::exception; } \
  } \
  static inline void php_dbxml_delete_##class_type(void *thing) { \
    struct php_dbxml_stuffed_##class_type *s = (struct php_dbxml_stuffed_##class_type *)thing; \
    delete s; \
  }

#define PHP_DBXML_STUFFED_THIS(class_type)  \
  class_type &This = php_dbxml_get_##class_type##_object_pointer(getThis() TSRMLS_CC);

#define PHP_DBXML_STUFFED_REF(class_type)				\
  class_type &This = php_dbxml_get_##class_type##_object_ref(getThis() TSRMLS_CC);

#define PHP_DBXML_DECLARE_CLASS(class_type) \
  int le_##class_type; \
  zend_class_entry *php_dbxml_##class_type##_ce

#define PHP_DBXML_EXTERN_CLASS(class_type) \
  extern int le_##class_type; \
  extern zend_class_entry *php_dbxml_##class_type##_ce; \
  extern function_entry php_dbxml_##class_type##_methods[]; \
  extern ZEND_RSRC_DTOR_FUNC(php_dbxml_##class_type##_dtor);

static inline void _hack_the_method_table(zend_function_entry *fe)
{
    int i;
    for(i = 0; fe[i].handler; i++) {
        fe[i].fname = strdup(fe[i].fname);
        zend_str_tolower(fe[i].fname, (unsigned int)strlen(fe[i].fname));
    }
}

#define FIXUP_METHODS(m) _hack_the_method_table(m)
#define FIXUP_CE(c) zend_str_tolower(c.name, c.name_length);


#define PHP_DBXML_REGISTER_CLASS(class_type, rsrc_dtor) \
  FIXUP_METHODS(php_dbxml_##class_type##_methods); \
  INIT_CLASS_ENTRY(ce, #class_type, php_dbxml_##class_type##_methods); \
  FIXUP_CE(ce); \
  php_dbxml_##class_type##_ce = zend_register_internal_class(&ce TSRMLS_CC); \
  le_##class_type = zend_register_list_destructors_ex(rsrc_dtor, NULL, "dbxml " # class_type, module_number);

#define PHP_DBXML_ME_GETTER_AND_SETTER(class_type, propname)  \
  PHP_ME(class_type, get##propname, NULL, 0) \
  PHP_ME(class_type, set##propname, NULL, 0)

#define PHP_DBXML_BOOL_GETTER_AND_SETTER(class_type, propname, proptype, optional, defval) \
PHP_METHOD(class_type, get##propname)  \
{                                      \
  PHP_DBXML_GETTHIS(class_type);       \
  if (ZEND_NUM_ARGS()) {               \
    WRONG_PARAM_COUNT;                 \
  }                                    \
  RETURN_BOOL(This->get##propname());  \
}                                      \
PHP_METHOD(class_type, set##propname)  \
{                                      \
  bool val = defval;                   \
  PHP_DBXML_GETTHIS(class_type);       \
  if (optional) { \
    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &val)) { \
      return;                                                                      \
    }                                                                              \
  } else { \
    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val)) { \
      return;                                                                      \
    }                                                                              \
  } \
  This->set##propname((proptype)val); \
}

#define PHP_DBXML_INT_GETTER_AND_SETTER(class_type, propname, proptype, optional, defval) \
PHP_METHOD(class_type, get##propname)  \
{                                      \
  PHP_DBXML_GETTHIS(class_type);       \
  if (ZEND_NUM_ARGS()) {               \
    WRONG_PARAM_COUNT;                 \
  }                                    \
  RETURN_LONG(This->get##propname());  \
}                                      \
PHP_METHOD(class_type, set##propname)  \
{                                      \
  long val = defval;                   \
  PHP_DBXML_GETTHIS(class_type);       \
  if (optional) { \
    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &val)) { \
      return;                                                                      \
    }                                                                              \
  } else { \
    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val)) { \
      return;                                                                      \
    }                                                                              \
  } \
  This->set##propname((proptype)val); \
}

#define PHP_DBXML_STRING_GETTER_AND_SETTER(class_type, propname) \
PHP_METHOD(class_type, get##propname)  \
{                                      \
  PHP_DBXML_GETTHIS(class_type);       \
  if (ZEND_NUM_ARGS()) {               \
    WRONG_PARAM_COUNT;                 \
  }                                    \
  std::string str = This->get##propname(); \
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1); \
}                                      \
PHP_METHOD(class_type, set##propname)  \
{                                      \
  char *val;                           \
  int val_len;                         \
  PHP_DBXML_GETTHIS(class_type);       \
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &val_len)) { \
      RETURN_FALSE;                                                           \
  } \
  This->set##propname(std::string(val, val_len)); \
}

PHP_DBXML_EXTERN_CLASS(XmlContainer);
PHP_DBXML_EXTERN_CLASS(XmlContainerConfig);
PHP_DBXML_EXTERN_CLASS(XmlData);
PHP_DBXML_EXTERN_CLASS(XmlManager);
PHP_DBXML_EXTERN_CLASS(XmlResolver);
PHP_DBXML_EXTERN_CLASS(XmlDocument);
PHP_DBXML_EXTERN_CLASS(XmlEventReader);
PHP_DBXML_EXTERN_CLASS(XmlEventReaderToWriter);
PHP_DBXML_EXTERN_CLASS(XmlEventWriter);
PHP_DBXML_EXTERN_CLASS(XmlIndexLookup);
PHP_DBXML_EXTERN_CLASS(XmlIndexSpecification);
PHP_DBXML_EXTERN_CLASS(XmlQueryContext);
PHP_DBXML_EXTERN_CLASS(XmlQueryExpression);
PHP_DBXML_EXTERN_CLASS(XmlResults);
PHP_DBXML_EXTERN_CLASS(XmlStatistics);
PHP_DBXML_EXTERN_CLASS(XmlTransaction);
PHP_DBXML_EXTERN_CLASS(XmlUpdateContext);
PHP_DBXML_EXTERN_CLASS(XmlInputStream);
PHP_DBXML_EXTERN_CLASS(XmlValue);
#ifdef PHP_DBXML_EXCEPTIONS
extern void php_dbxml_init_exceptions(TSRMLS_D);
extern void php_dbxml_throw_exception(const XmlException &xe TSRMLS_DC);
PHP_DBXML_EXTERN_CLASS(XmlException);
PHP_DBXML_STUFFED_INSTANCES(XmlException);
#endif

PHP_DBXML_STUFFED_INSTANCES(XmlContainer);
PHP_DBXML_STUFFED_INSTANCES(XmlContainerConfig);
PHP_DBXML_STUFFED_INSTANCES(XmlDocument);
PHP_DBXML_STUFFED_INSTANCES(XmlIndexLookup);
PHP_DBXML_STUFFED_INSTANCES(XmlIndexSpecification);
PHP_DBXML_STUFFED_INSTANCES(XmlQueryContext);
PHP_DBXML_STUFFED_INSTANCES(XmlQueryExpression);
PHP_DBXML_STUFFED_INSTANCES(XmlResults);
PHP_DBXML_STUFFED_INSTANCES(XmlStatistics);
PHP_DBXML_STUFFED_INSTANCES(XmlUpdateContext);
PHP_DBXML_STUFFED_INSTANCES(XmlValue);
PHP_DBXML_STUFFED_INSTANCES(XmlTransaction);
PHP_DBXML_STUFFED_INSTANCES(XmlData);
PHP_DBXML_STUFFED_INSTANCES(XmlEventReaderToWriter);
PHP_DBXML_STUFFED_REFERENCES(XmlEventReader);
PHP_DBXML_STUFFED_REFERENCES(XmlEventWriter);

#if 0
class PHPDBXmlResolver : public XmlResolver {
public:
    zval *m_userspace;
    PHPDBXmlResolver(zval *userspace);
    PHPDBXmlResolver(const PHPDBXmlResolver &o);
    PHPDBXmlResolver &operator=(const PHPDBXmlResolver &o);
    ~PHPDBXmlResolver();
    bool resolveDocument(XmlTransaction *txn, XmlManager &mgr, std::string &uri, XmlValue &result);
    bool resolveCollection(XmlTransaction *txn, XmlManager &mgr, const std::string &uri, XmlResults &result);
    XmlInputStream *resolveSchema(const std::string &schemaLocation, const std::string &nameSpace,
                 std::string &result);
    XmlInputStream *resolveEntity(const std::string &systemId, const std::string &publicId,
                 std::string &result);
};
#endif 
extern XmlResolver *php_dbxml_wrap_XmlResolver(zval *user_resolver);

/* in php_dbxml_value.cpp */
extern XmlValue php_dbxml_wrap_zval(zval *val);



