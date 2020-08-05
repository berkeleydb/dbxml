/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Lauren Foutz <Lauren.Foutz@oracle.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlContainerConfig_dtor)
{
    php_dbxml_delete_XmlContainerConfig(rsrc->ptr);
}

/* {{{ proto XmlContainerConfig::XmlContainerConfig()
   Create a new container config object. */
PHP_DBXML_METHOD_BEGIN(XmlContainerConfig, XmlContainerConfig)
{
  XmlContainerConfig *This = NULL;
  This = new XmlContainerConfig();
  php_dbxml_set_object_pointer(getThis(), le_XmlContainerConfig, This TSRMLS_CC);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlContainerConfig::setCompression(string name)
   sets the compression name, use only "NONE" and "DEFAULT" */
PHP_DBXML_METHOD_BEGIN(XmlContainerConfig, setCompression)
{
  char *name;
  int name_len;
  PHP_DBXML_GETTHIS(XmlContainerConfig);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len)) {
    RETURN_FALSE;
  }
  This->setCompressionName(name);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto string XmlContainerConfig::getCompression()
   Gets the namespace */
PHP_DBXML_METHOD_BEGIN(XmlContainerConfig, getCompression)
{
  PHP_DBXML_GETTHIS(XmlContainerConfig);
  std::string str = This->getCompressionName();
  DBXML_RETURN_STRINGL((char*)str.data(), str.length(), 1);
} PHP_DBXML_METHOD_END()
/* }}} */

PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, Mode, int, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, ContainerType, XmlContainer::ContainerType, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, PageSize, u_int32_t, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, SequenceIncrement, u_int32_t, 5, 5);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, IndexNodes, XmlContainerConfig::ConfigState, 0, 0);
PHP_DBXML_INT_GETTER_AND_SETTER(XmlContainerConfig, Statistics, XmlContainerConfig::ConfigState, 0, 0);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, Checksum, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, Encrypted, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, AllowValidation, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, Transactional, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, AllowCreate, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, ExclusiveCreate, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, NoMMap, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, ReadOnly, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, Multiversion, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, ReadUncommitted, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, Threaded, bool, false, false);
PHP_DBXML_BOOL_GETTER_AND_SETTER(XmlContainerConfig, TransactionNotDurable, bool, false, false);

function_entry php_dbxml_XmlContainerConfig_methods[] = {
  PHP_ME(XmlContainerConfig, XmlContainerConfig, NULL, 0)
  PHP_ME(XmlContainerConfig, getCompression, NULL, 0)
  PHP_ME(XmlContainerConfig, setCompression, NULL, 0)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Mode)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, ContainerType)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, PageSize)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, SequenceIncrement)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, IndexNodes)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Statistics)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Checksum)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Encrypted)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, AllowValidation)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Transactional)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, AllowCreate)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, ExclusiveCreate)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, NoMMap)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, ReadOnly)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Multiversion)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, ReadUncommitted)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, Threaded)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlContainerConfig, TransactionNotDurable)
  {NULL,NULL,NULL}
};

