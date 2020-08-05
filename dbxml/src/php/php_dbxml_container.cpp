/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlContainer_dtor)
{
  php_dbxml_delete_XmlContainer(rsrc->ptr);
}

/* {{{ proto XmlContainer::XmlContainer(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlContainer, XmlContainer)
{
  php_error_docref(NULL TSRMLS_CC, E_ERROR, "This class cannot be instantiated directly");
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, sync)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
    return;
  }
  This.sync();
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getName)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  std::string buffer = This.getName();
  DBXML_RETURN_STRINGL((char *)buffer.data(), buffer.length(), 1);
}
PHP_DBXML_METHOD_END()

/* {{{ Indexing and Introspection
 */
PHP_DBXML_METHOD_BEGIN(XmlContainer, getContainerType)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  RETURN_LONG(This.getContainerType());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getContainerConfig)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  RETURN_LONG(This.getContainerConfig());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getIndexNodes)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  RETURN_BOOL(This.getIndexNodes());
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getIndexSpecification)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) { return; }
  XmlIndexSpecification spec = This.getIndexSpecification();
  php_dbxml_set_XmlIndexSpecification_object_factory(return_value, spec TSRMLS_CC);
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, setIndexSpecification)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zidx = NULL, *zctx = NULL, *ztxn = NULL;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|O", 
      &zidx, php_dbxml_XmlIndexSpecification_ce, 
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &zidx, php_dbxml_XmlIndexSpecification_ce, 
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlIndexSpecification idx = php_dbxml_get_XmlIndexSpecification_object_pointer(zidx TSRMLS_CC);
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.setIndexSpecification(txn, idx, ctx);
  } else {
    This.setIndexSpecification(idx, ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getAutoIndexing)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL;
  if(SUCCESS != zend_parse_parameters(
	     ZEND_NUM_ARGS() TSRMLS_CC, "|O",
	     &ztxn, php_dbxml_XmlTransaction_ce)) {
    return;
  }

  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    RETURN_BOOL(This.getAutoIndexing(txn));
  } else {
    RETURN_BOOL(This.getAutoIndexing());
  }
}
PHP_DBXML_METHOD_END()


PHP_DBXML_METHOD_BEGIN(XmlContainer, setAutoIndexing)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  bool val;
  zval *zctx = NULL, *ztxn = NULL;
  if(FAILURE == zend_parse_parameters_ex(
	     ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "b|O", 
	     &val, &zctx, php_dbxml_XmlUpdateContext_ce)) {
	  
	  if(FAILURE == zend_parse_parameters(
		     ZEND_NUM_ARGS() TSRMLS_CC, "Ob|O", 
		     &ztxn, php_dbxml_XmlTransaction_ce, &val,
		     &zctx, php_dbxml_XmlUpdateContext_ce))
		  return;
  }
  
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
	  XmlTransaction txn =
		  php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
	  This.setAutoIndexing(txn, val, ctx);
  } else {
	  This.setAutoIndexing(val, ctx);
  }
}
PHP_DBXML_METHOD_END()


PHP_DBXML_METHOD_BEGIN(XmlContainer, addIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *uri, *name, *index;
  int urilen, namelen, indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss|O", 
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Osss|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.addIndex(txn, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  } else {
    This.addIndex(std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, deleteIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *uri, *name, *index;
  int urilen, namelen, indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss|O", 
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Osss|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.deleteIndex(txn, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  } else {
    This.deleteIndex(std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, replaceIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *uri, *name, *index;
  int urilen, namelen, indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss|O", 
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Osss|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.replaceIndex(txn, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  } else {
    This.replaceIndex(std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, addDefaultIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *index;
  int indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|O", 
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.addDefaultIndex(txn, std::string(index, indexlen), ctx);
  } else {
    This.addDefaultIndex(std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, deleteDefaultIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *index;
  int indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|O", 
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.deleteDefaultIndex(txn, std::string(index, indexlen), ctx);
  } else {
    This.deleteDefaultIndex(std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, replaceDefaultIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *zctx = NULL, *ztxn = NULL;
  char *index;
  int indexlen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|O", 
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|O", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &index, &indexlen,
      &zctx, php_dbxml_XmlUpdateContext_ce)) {
  }
  else {
    return;
  }
  XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    This.replaceDefaultIndex(txn, std::string(index, indexlen), ctx);
  } else {
    This.replaceDefaultIndex(std::string(index, indexlen), ctx);
  }
}
PHP_DBXML_METHOD_END()

/* }}} */

/* {{{ Document Retrieval
 */

PHP_DBXML_METHOD_BEGIN(XmlContainer, getDocument)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL;
  char *name;
  int namelen;
  long flags = 0;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|l", 
      &name, &namelen, &flags)) {
  } else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", 
      &ztxn, php_dbxml_XmlTransaction_ce,
      &name, &namelen, &flags)) {
  } else {
    return;
  }
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlDocument doc = This.getDocument(txn, std::string(name, namelen), flags);
    php_dbxml_set_XmlDocument_object_factory(return_value, doc TSRMLS_CC);
  } else {
    XmlDocument doc = This.getDocument(std::string(name, namelen), flags);
    php_dbxml_set_XmlDocument_object_factory(return_value, doc TSRMLS_CC);
  }
}
PHP_DBXML_METHOD_END()

/* {{{ Node Retrieval
 */

PHP_DBXML_METHOD_BEGIN(XmlContainer, getNode)
{
	PHP_DBXML_STUFFED_THIS(XmlContainer);
	zval *ztxn = NULL;
	char *handle;
	int handleLen;
	long flags = 0;
	if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|l", 
					       &handle, &handleLen, &flags)) {
	} else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", 
						 &ztxn, php_dbxml_XmlTransaction_ce,
						 &handle, &handleLen, &flags)) {
	} else {
		return;
	}
	if(ztxn) {
		XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
		XmlValue doc = This.getNode(txn, std::string(handle, handleLen), flags);
		php_dbxml_set_XmlValue_object_factory(return_value, doc TSRMLS_CC);
	} else {
		XmlValue doc = This.getNode(std::string(handle, handleLen), flags);
		php_dbxml_set_XmlValue_object_factory(return_value, doc TSRMLS_CC);
	}
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, putDocument)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zdoc = NULL, *zstream = NULL, *zctx = NULL;
  char *name, *contents;
  int namelen, contentslen;
  long flags = 0;
  /* XmlDocument */
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|Ol",
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(doc, ctx, flags);
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OO|Ol",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(txn, doc, ctx, flags);
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sO|Ol",
    &name, &namelen,
    &zstream, php_dbxml_XmlInputStream_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlInputStream *stm = PHP_DBXML_GET(zstream, XmlInputStream);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(std::string(name, namelen), stm, ctx, flags);
    zend_list_delete(Z_LVAL_P(zstream));
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OsO|Ol",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &name, &namelen,
    &zstream, php_dbxml_XmlInputStream_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlInputStream *stm = PHP_DBXML_GET(zstream, XmlInputStream);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(txn, std::string(name, namelen), stm, ctx, flags);
    zend_list_delete(Z_LVAL_P(zstream));
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ss|Ol",
    &name, &namelen,
    &contents, &contentslen,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(std::string(name, namelen), std::string(contents, contentslen), ctx, flags);
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Oss|Ol",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &name, &namelen,
    &contents, &contentslen,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.putDocument(txn, std::string(name, namelen), std::string(contents, contentslen), ctx, flags);
  }
  else {
    return;
  } 
}
PHP_DBXML_METHOD_END()

// This shares much of its code with putDocument, but
// returns XmlEventWriter
PHP_DBXML_METHOD_BEGIN(XmlContainer, putDocumentAsEventWriter)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zdoc = NULL, *zstream = NULL, *zctx = NULL;
  long flags = 0;
  /* XmlDocument */
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|Ol",
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    XmlEventWriter &writer = This.putDocumentAsEventWriter(doc, ctx, flags);
    php_dbxml_set_XmlEventWriter_object_factory(return_value, writer TSRMLS_CC);
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OO|Ol",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce, &flags)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    XmlEventWriter &writer = This.putDocumentAsEventWriter(txn, doc, ctx, flags);
    php_dbxml_set_XmlEventWriter_object_factory(return_value, writer TSRMLS_CC);
  }
  else {
	  php_error(E_WARNING, "usage: putDocumentAsEventWriter [txn] doc [context] [flags]");
  } 
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, deleteDocument)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zdoc = NULL, *zctx = NULL;
  char *name;
  int namelen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|O",
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.deleteDocument(doc, ctx);
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OO|O",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.deleteDocument(txn, doc, ctx);
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|O",
    &name, &namelen,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.deleteDocument(std::string(name, namelen), ctx);
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|O",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &name, &namelen,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.deleteDocument(txn, std::string(name, namelen), ctx);
  }
  else {
    return;
  } 
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, updateDocument)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zdoc = NULL, *zctx = NULL;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|O",
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.updateDocument(doc, ctx);
  }
  else if(SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO|O",
    &ztxn, php_dbxml_XmlTransaction_ce,
    &zdoc, php_dbxml_XmlDocument_ce,
    &zctx, php_dbxml_XmlUpdateContext_ce)) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlDocument doc = php_dbxml_get_XmlDocument_object_pointer(zdoc TSRMLS_CC);
    XmlUpdateContext ctx = zctx?php_dbxml_get_XmlUpdateContext_object_pointer(zctx TSRMLS_CC):This.getManager().createUpdateContext();
    This.updateDocument(txn, doc, ctx);
  }
  else { return; }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, lookupIndex)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zctx = NULL, *zvl = NULL;
  char *uri, *name, *index;
  int urilen, namelen, indexlen;
  char *puri, *pname;
  int purilen, pnamelen;
  long flags = 0;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Osss|Ol",
      &zctx, php_dbxml_XmlQueryContext_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce, &flags))
  {
    XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlResults re = This.lookupIndex(ctx, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), vl, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlResults re = This.lookupIndex(ctx, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), XmlValue(), flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOsss|Ol",
      &ztxn, php_dbxml_XmlTransaction_ce,
      &zctx, php_dbxml_XmlQueryContext_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce, &flags))
  {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlResults re = This.lookupIndex(txn, ctx, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), vl, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlResults re = This.lookupIndex(txn, ctx, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), XmlValue(), flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Ossssss|Ol",
      &zctx, php_dbxml_XmlQueryContext_ce,
      &uri, &urilen, &name, &namelen, 
      &puri, &purilen, &pname, &pnamelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce, &flags))
  {
    XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlResults re = This.lookupIndex(ctx, 
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          vl, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlResults re = This.lookupIndex(ctx,   
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          XmlValue(), flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "OOssssss|Ol",
      &ztxn, php_dbxml_XmlTransaction_ce,
      &zctx, php_dbxml_XmlQueryContext_ce,
      &uri, &urilen, &name, &namelen,
      &puri, &purilen, &pname, &pnamelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce, &flags))
  {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlQueryContext ctx = php_dbxml_get_XmlQueryContext_object_pointer(zctx TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlResults re = This.lookupIndex(txn, ctx, 
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          vl, flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlResults re = This.lookupIndex(txn, ctx, 
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          XmlValue(), flags);
      php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
    }
  }
}
PHP_DBXML_METHOD_END()


/* }}} */

PHP_DBXML_METHOD_BEGIN(XmlContainer, lookupStatistics)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL, *zctx = NULL, *zvl = NULL;
  char *uri, *name, *index;
  int urilen, namelen, indexlen;
  char *puri, *pname;
  int purilen, pnamelen;
  if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss|O",
      &uri, &urilen, &name, &namelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce))
  {
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlStatistics re = This.lookupStatistics(std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), vl);
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlStatistics re = This.lookupStatistics(std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), XmlValue());
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Osss|Ol",
      &ztxn, php_dbxml_XmlTransaction_ce,
      &uri, &urilen, &name, &namelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce))
  {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlStatistics re = This.lookupStatistics(txn, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), vl);
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlStatistics re = This.lookupStatistics(txn, std::string(uri, urilen), std::string(name, namelen), std::string(index, indexlen), XmlValue());
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ssssss|Ol",
      &uri, &urilen, &name, &namelen, 
      &puri, &purilen, &pname, &pnamelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce))
  {
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlStatistics re = This.lookupStatistics(std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          vl);
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlStatistics re = This.lookupStatistics(std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          XmlValue());
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    }
  }
  else if(SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "Ossssss|Ol",
      &ztxn, php_dbxml_XmlTransaction_ce,
      &uri, &urilen, &name, &namelen,
      &puri, &purilen, &pname, &pnamelen, &index, &indexlen, 
      &zvl, php_dbxml_XmlValue_ce))
  {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    if(zvl) {
      XmlValue vl = php_dbxml_get_XmlValue_object_pointer(zvl TSRMLS_CC);
      XmlStatistics re = This.lookupStatistics(txn,
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          vl);
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    } else {
      XmlStatistics re = This.lookupStatistics(txn, 
          std::string(uri, urilen), std::string(name, namelen), 
          std::string(puri, purilen), std::string(pname, pnamelen), std::string(index, indexlen),
          XmlValue());
      php_dbxml_set_XmlStatistics_object_factory(return_value, re TSRMLS_CC);
    }
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, addAlias)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  char *name;
  int namelen;
  if(FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen)) {
    return;
  }
  RETURN_BOOL(This.addAlias(std::string(name, namelen)));
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, removeAlias)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  char *name;
  int namelen;
  if(FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen)) {
    return;
  }
  RETURN_BOOL(This.removeAlias(std::string(name, namelen)));
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getAllDocuments)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL;
  u_int32_t flags = 0;
  if(FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|O!l",
      &ztxn, php_dbxml_XmlTransaction_ce, &flags))
  {
    return;
  } 
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    XmlResults re = This.getAllDocuments(txn, flags);
    php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
  } else {
    XmlResults re = This.getAllDocuments(flags);
    php_dbxml_set_XmlResults_object_factory(return_value, re TSRMLS_CC);
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, getNumDocuments)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  zval *ztxn = NULL;
  if(FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|O",
      &ztxn, php_dbxml_XmlTransaction_ce))
  {
    return;
  } 
  if(ztxn) {
    XmlTransaction txn = php_dbxml_get_XmlTransaction_object_pointer(ztxn TSRMLS_CC);
    DBXML_RETURN_LONG(This.getNumDocuments(txn));
  } else {
    DBXML_RETURN_LONG(This.getNumDocuments());
  }
}
PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlContainer, close)
{
  PHP_DBXML_STUFFED_THIS(XmlContainer);
  This.close();
}
PHP_DBXML_METHOD_END()
/* }}} */

function_entry php_dbxml_XmlContainer_methods[] = {
  PHP_ME(XmlContainer, XmlContainer, NULL, 0)
  PHP_ME(XmlContainer, sync, NULL, 0)
  PHP_ME(XmlContainer, getAutoIndexing, NULL, 0)
  PHP_ME(XmlContainer, setAutoIndexing, NULL, 0)
  PHP_ME(XmlContainer, getName, NULL, 0)
  PHP_ME(XmlContainer, getContainerType, NULL, 0)
  PHP_ME(XmlContainer, getContainerConfig, NULL, 0)
  PHP_ME(XmlContainer, getIndexNodes, NULL, 0)
  PHP_ME(XmlContainer, getIndexSpecification, NULL, 0)
  PHP_ME(XmlContainer, setIndexSpecification, NULL, 0)
  PHP_ME(XmlContainer, addIndex, NULL, 0)
  PHP_ME(XmlContainer, deleteIndex, NULL, 0)
  PHP_ME(XmlContainer, replaceIndex, NULL, 0)
  PHP_ME(XmlContainer, addDefaultIndex, NULL, 0)
  PHP_ME(XmlContainer, deleteDefaultIndex, NULL, 0)
  PHP_ME(XmlContainer, replaceDefaultIndex, NULL, 0)
  PHP_ME(XmlContainer, getDocument, NULL, 0)
  PHP_ME(XmlContainer, getNode, NULL, 0)
  PHP_ME(XmlContainer, putDocument, NULL, 0)
  PHP_ME(XmlContainer, putDocumentAsEventWriter, NULL, 0)
  PHP_ME(XmlContainer, deleteDocument, NULL, 0)
  PHP_ME(XmlContainer, updateDocument, NULL, 0)
  PHP_ME(XmlContainer, lookupIndex, NULL, 0)
  PHP_ME(XmlContainer, lookupStatistics, NULL, 0)
  PHP_ME(XmlContainer, addAlias, NULL, 0)
  PHP_ME(XmlContainer, removeAlias, NULL, 0)
  PHP_ME(XmlContainer, getAllDocuments, NULL, 0)
  PHP_ME(XmlContainer, getNumDocuments, NULL, 0)
  PHP_ME(XmlContainer, close, NULL, 0)
  {NULL,NULL,NULL}
};

/* vim: set ts=2 sts=2 expandtab ai bs=2 : */
