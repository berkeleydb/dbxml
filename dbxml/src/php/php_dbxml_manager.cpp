/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"


ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlManager_dtor)
{
  delete (XmlManager*)rsrc->ptr;
}

/* {{{ proto XmlManager::XmlManager(object DB_ENV [, int flags])
   Create a new manager using the provided DB_ENV and flags */
PHP_DBXML_METHOD_BEGIN(XmlManager, XmlManager)
{
  XmlManager *This = NULL;
  DB_ENV *dbenv = NULL;
  zval *dbenv_obj = NULL;
  zend_class_entry *db_env_ce = db_env_ce_get();
  long flags = 0;
  
  if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC,
					  "|O!l", &dbenv_obj, db_env_ce, &flags)) {
	  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
					       "l", &flags))
		  RETURN_FALSE;
  }
  
  if (dbenv_obj) {
    dbenv = php_db4_getDbEnvFromObj(dbenv_obj TSRMLS_CC);
    This = new XmlManager(dbenv, (unsigned int)flags);
  } else {
    This = new XmlManager((unsigned int)flags);
  }
  php_dbxml_set_object_pointer(getThis(), le_XmlManager, This TSRMLS_CC);
}
PHP_DBXML_METHOD_END()
/* }}} */
#if 0
PHP_DBXML_METHOD_BEGIN(XmlManager, registerResolver)
{
  zval *user_resolver;
  PHP_DBXML_GETTHIS(XmlManager);
//  php_error_docref(NULL TSRMLS_CC, E_ERROR, "XmlManager::registerResolver() is not yet available");
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &user_resolver, php_dbxml_XmlResolver_ce)) {
    return;
  }
  XmlResolver *resolver = php_dbxml_wrap_XmlResolver(user_resolver);
  This->registerResolver(*resolver);
}
PHP_DBXML_METHOD_END()
#endif

PHP_DBXML_METHOD_BEGIN(XmlManager, existsContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  char *name;
  int namelen;
  int ret = 0;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen)) {
	  ret = This->existsContainer(std::string(name, namelen));
  }
  RETURN_LONG(ret);
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createContainer)
{
  char *name;
  // these must be the same as BDB XML defaults 
  int namelen;
  long flags = -1, mode = 0;
  XmlContainer::ContainerType type = XmlContainer::NodeContainer;
  XmlContainerConfig config;
  XmlContainer container;
  XmlTransaction txn;
  zval *ztxn = NULL, *zxcc = NULL;
  PHP_DBXML_GETTHIS(XmlManager);
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "sO",
					  &name, &namelen,
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce)) {
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->createContainer(std::string(name, namelen), config);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "OsO",
					  &ztxn, php_dbxml_XmlTransaction_ce, 
					  &name, &namelen, 
					  &zxcc, php_dbxml_XmlContainerConfig_ce)) {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->createContainer(txn, std::string(name, namelen), config);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "sOl|l",
					  &name, &namelen,
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce,
					  &type, &mode)) {
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->createContainer(std::string(name, namelen), config, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "OsOl|l",
					  &ztxn, php_dbxml_XmlTransaction_ce, 
					  &name, &namelen, 
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce,
					  &type, &mode)) {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->createContainer(txn, std::string(name, namelen), config, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "s|lll",
					  &name, &namelen, &flags, &type, &mode)) {
    if (flags == -1)
      container = This->createContainer(std::string(name, namelen));
    else
      container = This->createContainer(std::string(name, namelen),
				  (u_int32_t)flags, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
    PHP_DBXML_ADDREF(getThis());
    return;
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|lll", &ztxn, php_dbxml_XmlTransaction_ce, 
          &name, &namelen, &flags, &type, &mode)) 
  {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if (flags == -1)
      container = This->createContainer(txn, std::string(name, namelen));
    else
      container = This->createContainer(txn, std::string(name, namelen),
				  (u_int32_t)flags, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
    PHP_DBXML_ADDREF(getThis());
    return;
  }
  else { return; }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, openContainer)
{
  char *name;
  // these must be the same as BDB XML defaults 
  int namelen;
  long flags = -1, mode = 0;
  XmlContainer::ContainerType type = XmlContainer::NodeContainer;
  XmlContainerConfig config;
  XmlContainer container;
  XmlTransaction txn;
  zval *ztxn = NULL, *zxcc = NULL;
  PHP_DBXML_GETTHIS(XmlManager);
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "sO",
					  &name, &namelen,
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce)) {
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->openContainer(std::string(name, namelen), config);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "OsO",
					  &ztxn, php_dbxml_XmlTransaction_ce, 
					  &name, &namelen, 
					  &zxcc, php_dbxml_XmlContainerConfig_ce)) {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->openContainer(txn, std::string(name, namelen), config);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "sOl|l",
					  &name, &namelen,
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce,
					  &type, &mode)) {
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->openContainer(std::string(name, namelen), config, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "OsOl|l",
					  &ztxn, php_dbxml_XmlTransaction_ce, 
					  &name, &namelen, 
					  &zxcc, 
					  php_dbxml_XmlContainerConfig_ce,
					  &type, &mode)) {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
    container = This->openContainer(txn, std::string(name, namelen), config, type, mode);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
  }
  else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &name, &namelen, &flags)) {
    if (flags == -1)
      container = This->openContainer(std::string(name, namelen));
    else
      container = This->openContainer(std::string(name, namelen), (u_int32_t)flags);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
    PHP_DBXML_ADDREF(getThis());
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", &ztxn, php_dbxml_XmlTransaction_ce, 
          &name, &namelen, &flags)) 
  {
    txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if(flags == -1) 
      container = This->openContainer(txn, std::string(name, namelen));
    else
      container = This->openContainer(txn, std::string(name, namelen), (u_int32_t)flags);
    php_dbxml_set_XmlContainer_object_factory(return_value, container TSRMLS_CC);
    PHP_DBXML_ADDREF(getThis());
  }
  else { return; }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, removeContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  char *name;
  int namelen;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen)) {
    This->removeContainer(std::string(name, namelen));
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This->removeContainer(txn, std::string(name, namelen));
  } else {
    return;
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, renameContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  XmlTransaction *txn = NULL;
  char *name, *newname;
  int namelen, newnamelen;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &namelen, &newname, &newnamelen)) {
    This->renameContainer(std::string(name, namelen), std::string(newname, newnamelen));
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &newname, &newnamelen)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This->renameContainer(txn, std::string(name, namelen), std::string(newname, newnamelen));
  } else {
    return;
  }
}
PHP_DBXML_METHOD_END()

/* dumpContainer */
/* loadContainer */

PHP_DBXML_METHOD_BEGIN(XmlManager, createLocalFileInputStream)
{
  char *fileName;
  int filelen;
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fileName, &filelen)) {
    return;
  }
  XmlInputStream *stream = This->createLocalFileInputStream(std::string(fileName, filelen));
  object_init_ex(return_value, php_dbxml_XmlInputStream_ce);
  php_dbxml_set_object_pointer(return_value, le_XmlInputStream, stream TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createMemBufInputStream)
{
  char *buffer, *bufID;
  int bufferlen, bufIDlen;
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &buffer, &bufferlen, &bufID, &bufIDlen)) {
    return;
  }
  XmlInputStream *stream = This->createMemBufInputStream(buffer, bufferlen, bufID, false);
  object_init_ex(return_value, php_dbxml_XmlInputStream_ce);
  php_dbxml_set_object_pointer(return_value, le_XmlInputStream, stream TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createDocument)
{
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
    return;
  }
  XmlDocument document = This->createDocument();
  php_dbxml_set_XmlDocument_object_factory(return_value, document TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createQueryContext)
{
  PHP_DBXML_GETTHIS(XmlManager);
  int rt = (int) XmlQueryContext::LiveValues, et = (int) XmlQueryContext::Eager;
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &rt, &et)) {
    return;
  }
  XmlQueryContext context = This->createQueryContext((XmlQueryContext::ReturnType) rt, (XmlQueryContext::EvaluationType) et);
  PHP_DBXML_ADDREF(getThis());
  php_dbxml_set_XmlQueryContext_object_factory(return_value, context TSRMLS_CC);
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createUpdateContext)
{
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
    return;
  }
  XmlUpdateContext context = This->createUpdateContext();
  php_dbxml_set_XmlUpdateContext_object_factory(return_value, context TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, prepare)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  zval *zctx = NULL;
  char *query;
  int querylen;

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|O", &query, &querylen, &zctx, php_dbxml_XmlQueryContext_ce)) {
    if(zctx) {
      XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
      XmlQueryExpression qe = This->prepare(std::string(query, querylen), ctx);
      php_dbxml_set_XmlQueryExpression_object_factory(return_value, qe TSRMLS_CC);
    } else {
      XmlQueryContext ctx = This->createQueryContext();
      XmlQueryExpression qe = This->prepare(std::string(query, querylen), ctx);
      php_dbxml_set_XmlQueryExpression_object_factory(return_value, qe TSRMLS_CC);
    }
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|O", &ztxn, php_dbxml_XmlTransaction_ce, &query, &querylen, &zctx, php_dbxml_XmlQueryContext_ce)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if(zctx) {
      XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
      XmlQueryExpression qe = This->prepare(txn, std::string(query, querylen), ctx);
      php_dbxml_set_XmlQueryExpression_object_factory(return_value, qe TSRMLS_CC);
    } else {
      XmlQueryContext ctx = This->createQueryContext();
      XmlQueryExpression qe = This->prepare(txn, std::string(query, querylen), ctx);
      php_dbxml_set_XmlQueryExpression_object_factory(return_value, qe TSRMLS_CC);
    }
  } else {
    return;
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, query)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  zval *zctx = NULL;
  char *query;
  int querylen;
  long flags = 0;

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|Ol", &query, &querylen, &zctx, php_dbxml_XmlQueryContext_ce, &flags)) {
    if(zctx) {
      XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
      XmlResults re = This->query(std::string(query, querylen), ctx, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlQueryContext ctx = This->createQueryContext();
      XmlResults re = This->query(std::string(query, querylen), ctx, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|Ol", 
      &ztxn, php_dbxml_XmlTransaction_ce, &query, &querylen, &zctx, php_dbxml_XmlQueryContext_ce, &flags)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if(zctx) {
      XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
      XmlResults re = This->query(txn, std::string(query, querylen), ctx, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlQueryContext ctx = This->createQueryContext();
      XmlResults re = This->query(txn, std::string(query, querylen), ctx, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  } else {
    return;
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createResults)
{
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
    return;
  }
  XmlResults re = This->createResults();
  php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()


PHP_DBXML_METHOD_BEGIN(XmlManager, createTransaction)
{
  PHP_DBXML_GETTHIS(XmlManager);
  long flags = 0;
  if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags)) {
    return;
  }
  XmlTransaction txn = This->createTransaction(flags);
  php_dbxml_set_XmlTransaction_object_factory(return_value, txn TSRMLS_CC);
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, createIndexLookup)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *zc, *zv = NULL;
  char *uri, *name, *index;
  int uri_len, name_len, index_len;
  long flags = XmlIndexLookup::NONE;
  if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Osss|Ol", 
                  &zc, php_dbxml_XmlContainer_ce, &uri, &uri_len, &name, &name_len,
                  &index, &index_len, &zv, php_dbxml_XmlValue_ce, &flags)) {
    return;
  }
  XmlContainer cont = php_dbxml_get_XmlContainer_object_pointer(zc TSRMLS_CC);
  XmlIndexLookup lu;
  if(zv == NULL) {
    XmlValue value = XmlValue();
    lu = This->createIndexLookup(cont, std::string(uri, uri_len), std::string(name, name_len), std::string(index, index_len), value, (XmlIndexLookup::Operation) flags);
    php_dbxml_set_XmlIndexLookup_object_factory(return_value, lu TSRMLS_CC);
  } else {
    XmlValue value = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    lu = This->createIndexLookup(cont, std::string(uri, uri_len), std::string(name, name_len), std::string(index, index_len), value, (XmlIndexLookup::Operation) flags);
    php_dbxml_set_XmlIndexLookup_object_factory(return_value, lu TSRMLS_CC);
  }
  PHP_DBXML_ADDREF(getThis());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, upgradeContainer)
{ 
  char *name;
  int name_len;
  zval *zuc;
  PHP_DBXML_GETTHIS(XmlManager);
  
  if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "sO", &name, &name_len, &zuc, php_dbxml_XmlUpdateContext_ce)) {
    XmlUpdateContext u = php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC);
    This->upgradeContainer(std::string(name, name_len), u);
    RETURN_NULL();
  }
} 
PHP_DBXML_METHOD_END()

// [txn] name [uc] [config]
PHP_DBXML_METHOD_BEGIN(XmlManager, reindexContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  zval *zuc = NULL;
  zval *zxcc = NULL;
  char *name;
  long flags = 0;
  int namelen;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|OO", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|OO", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  }else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|Ol", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|Ol", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else {
    return;
  }
  
  XmlContainerConfig config((u_int32_t)flags);
  if (zxcc)
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
  XmlUpdateContext context = (zuc ? php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC): This->createUpdateContext());
  if (ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This->reindexContainer(txn, std::string(name, namelen), context, config);
  } else {
    This->reindexContainer(std::string(name, namelen), context, config);
  }
} 
PHP_DBXML_METHOD_END()

// [txn] name [uc] [config]
PHP_DBXML_METHOD_BEGIN(XmlManager, compactContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  zval *zuc = NULL;
  zval *zxcc = NULL;
  char *name;
  long flags = 0;
  int namelen;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|OO", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|OO", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  }else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|Ol", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|Ol", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else {
    return;
  }
  
  XmlContainerConfig config((u_int32_t)flags);
  if (zxcc)
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
  XmlUpdateContext context = (zuc ? php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC): This->createUpdateContext());
  if (ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This->compactContainer(txn, std::string(name, namelen), context, config);
  } else {
    This->compactContainer(std::string(name, namelen), context, config);
  }
} 
PHP_DBXML_METHOD_END()

// [txn] name [uc] [flags]
PHP_DBXML_METHOD_BEGIN(XmlManager, truncateContainer)
{
  PHP_DBXML_GETTHIS(XmlManager);
  zval *ztxn = NULL;
  zval *zuc = NULL;
  zval *zxcc = NULL;
  char *name;
  long flags = 0;
  int namelen;
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|OO", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|OO", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &zxcc, php_dbxml_XmlContainerConfig_ce)) {
  }else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|Ol", &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|Ol", &ztxn, php_dbxml_XmlTransaction_ce, &name, &namelen, &zuc, php_dbxml_XmlUpdateContext_ce, &flags)) {
  } else {
    return;
  }
  
  XmlContainerConfig config((u_int32_t)flags);
  if (zxcc)
    config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
  XmlUpdateContext context = (zuc ? php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC): This->createUpdateContext());
  if (ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This->truncateContainer(txn, std::string(name, namelen), context, config);
  } else {
    This->truncateContainer(std::string(name, namelen), context, config);
  }
} 
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, getHome)
{
  PHP_DBXML_GETTHIS(XmlManager);
  std::string buffer = This->getHome();
  DBXML_RETURN_STRINGL((char *)buffer.data(), buffer.length(), 1);
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, getDefaultContainerConfig)
{
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
	php_error(E_WARNING, "usage: getDefaultContainerConfig");
    return;
  }
  XmlContainerConfig xcc = This->getDefaultContainerConfig();
  php_dbxml_set_XmlContainerConfig_object_factory(return_value, xcc TSRMLS_CC);
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlManager, setDefaultContainerConfig)
{
  zval *zxcc = NULL;
  PHP_DBXML_GETTHIS(XmlManager);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zxcc, php_dbxml_XmlContainerConfig_ce)) {
    return;
  }
  XmlContainerConfig config = php_dbxml_get_XmlContainerConfig_object_pointer(zxcc TSRMLS_CC);
  This->setDefaultContainerConfig(config);
}
PHP_DBXML_METHOD_END()

/* {{{ proto void XmlManager::setDefaultContainerFlags([int flags])
   sets flags for container construction */
/* }}} */
/* {{{ proto int XmlManager:;getDefaultContainerFlags()
   gets the flags for container construction */
/* }}} */
PHP_DBXML_INT_GETTER_AND_SETTER(XmlManager, DefaultContainerFlags, u_int32_t, 1, 0)

/* {{{ proto int XmlManager:;getDefaultPageSize()
   gets the default page size */
/* }}} */
PHP_DBXML_INT_GETTER_AND_SETTER(XmlManager, DefaultPageSize, u_int32_t, 0, 0)

PHP_DBXML_INT_GETTER_AND_SETTER(XmlManager, DefaultContainerType, XmlContainer::ContainerType, 0, 0)

PHP_DBXML_INT_GETTER_AND_SETTER(XmlManager, ImplicitTimezone, int, 0, 0)


function_entry php_dbxml_XmlManager_methods[] = {
  PHP_ME(XmlManager, XmlManager, NULL, 0)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlManager, DefaultContainerFlags)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlManager, DefaultPageSize)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlManager, DefaultContainerType)
  PHP_DBXML_ME_GETTER_AND_SETTER(XmlManager, ImplicitTimezone)
#if 0
  PHP_ME(XmlManager, registerResolver, NULL, 0)
#endif
  PHP_ME(XmlManager, existsContainer, NULL, 0)
  PHP_ME(XmlManager, setDefaultContainerConfig, NULL, 0)
  PHP_ME(XmlManager, getDefaultContainerConfig, NULL, 0)
  PHP_ME(XmlManager, createContainer, NULL, 0)
  PHP_ME(XmlManager, openContainer, NULL, 0)
  PHP_ME(XmlManager, removeContainer, NULL, 0)
  PHP_ME(XmlManager, renameContainer, NULL, 0)
  PHP_ME(XmlManager, createLocalFileInputStream, NULL, 0)
  PHP_ME(XmlManager, createMemBufInputStream, NULL, 0)
  PHP_ME(XmlManager, createDocument, NULL, 0)
  PHP_ME(XmlManager, createQueryContext, NULL, 0)
  PHP_ME(XmlManager, createUpdateContext, NULL, 0)
  PHP_ME(XmlManager, prepare, NULL, 0)
  PHP_ME(XmlManager, query, NULL, 0)
  PHP_ME(XmlManager, createResults, NULL, 0)
  PHP_ME(XmlManager, createTransaction, NULL, 0)
  PHP_ME(XmlManager, createIndexLookup, NULL, 0)
  PHP_ME(XmlManager, upgradeContainer, NULL, 0)
  PHP_ME(XmlManager, reindexContainer, NULL, 0)
  PHP_ME(XmlManager, compactContainer, NULL, 0)
  PHP_ME(XmlManager, truncateContainer, NULL, 0)
  PHP_ME(XmlManager, getHome, NULL, 0)
  {NULL,NULL,NULL}
};
