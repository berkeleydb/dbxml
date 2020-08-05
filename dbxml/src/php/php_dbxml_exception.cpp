/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * author: George Feinberg <george.feinberg@oracle.com>
 */

#include "php_dbxml_int.hpp"

#ifdef PHP_DBXML_EXCEPTIONS

#define PHP_DBXML_REGISTER_CLASS_WITH_PARENT(class_type, parent_class, rsrc_dtor) \
  FIXUP_METHODS(php_dbxml_##class_type##_methods); \
  INIT_CLASS_ENTRY(ce, #class_type, php_dbxml_##class_type##_methods); \
  FIXUP_CE(ce); \
  php_dbxml_##class_type##_ce = zend_register_internal_class_ex(&ce, parent_class, NULL TSRMLS_CC);


PHP_DBXML_DECLARE_CLASS(XmlException);

void php_dbxml_init_exceptions(TSRMLS_D)
{
	zend_class_entry ce;
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	PHP_DBXML_REGISTER_CLASS_WITH_PARENT(XmlException, zend_exception_get_default(), php_dbxml_XmlException_dtor);
#else
	PHP_DBXML_REGISTER_CLASS_WITH_PARENT(XmlException, zend_exception_get_default(TSRMLS_C), php_dbxml_XmlException_dtor);
#endif
	zend_declare_property_long(php_dbxml_XmlException_ce, "code",
				   sizeof("code")-1, 0,
				   ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(php_dbxml_XmlException_ce, "dbErrno",
				   sizeof("dbErrno")-1, 0,
				   ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(php_dbxml_XmlException_ce, "what",
				   sizeof("what")-1, "00000",
				   ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(php_dbxml_XmlException_ce, "queryLine",
				   sizeof("queryLine")-1, 0,
				   ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(php_dbxml_XmlException_ce, "queryColumn",
				   sizeof("queryColumn")-1, 0,
				   ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(php_dbxml_XmlException_ce, "queryFile",
				   sizeof("queryFile")-1, "00000",
				   ZEND_ACC_PROTECTED TSRMLS_CC);
}

void php_dbxml_throw_exception(const XmlException &xe TSRMLS_DC)
{
	zval *xml_ex;
	MAKE_STD_ZVAL(xml_ex);
	object_init_ex(xml_ex, php_dbxml_XmlException_ce);
	zend_update_property_string(php_dbxml_XmlException_ce, xml_ex,
				    "what", sizeof("what")-1,
				    (char *)xe.what() TSRMLS_CC);
	zend_update_property_string(php_dbxml_XmlException_ce, xml_ex,
				    "message", sizeof("message")-1,
				    (char *)xe.what() TSRMLS_CC);
	zend_update_property_long(php_dbxml_XmlException_ce, xml_ex,
				    "code", sizeof("code")-1,
				    xe.getExceptionCode() TSRMLS_CC);
	zend_update_property_long(php_dbxml_XmlException_ce, xml_ex,
				  "dbErrno", sizeof("dbErrno")-1,
				  xe.getDbErrno() TSRMLS_CC);
	zend_update_property_long(php_dbxml_XmlException_ce, xml_ex,
				  "queryLine", sizeof("queryLine")-1,
				  xe.getQueryLine() TSRMLS_CC);
	zend_update_property_long(php_dbxml_XmlException_ce, xml_ex,
				  "queryColumn", sizeof("queryColumn")-1,
				  xe.getQueryColumn() TSRMLS_CC);
	if (xe.getQueryFile())
		zend_update_property_string(php_dbxml_XmlException_ce, xml_ex,
					    "queryFile", sizeof("queryFile")-1,
					    (char *)xe.getQueryFile() TSRMLS_CC);
	zend_throw_exception_object(xml_ex TSRMLS_CC);
}

// utility fcn to return a property
static void dbxml_exception_get_entry(zval *obj, char *pname, int plen,
				      zval *retVal TSRMLS_DC)
{
	zval *value;

	value = zend_read_property(php_dbxml_XmlException_ce,
				   obj, pname, plen, 0 TSRMLS_CC);

	*retVal = *value;
	zval_copy_ctor(retVal);
	INIT_PZVAL(retVal);
}

/* {{{ proto long XmlException::getExceptionCode()
   Get the XmlException::ExceptionCode for the exception */
ZEND_METHOD(XmlException, getExceptionCode)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "code", sizeof("code")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto long XmlException::getDbErrno()
   Get the DB errno, which is valid for DATABASE_EXCEPTION */
ZEND_METHOD(XmlException, getDbErrno)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "dbErrno", sizeof("dbErrno")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto string XmlException::what()
   Get the string description of the exception */
ZEND_METHOD(XmlException, what)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "what", sizeof("what")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto long XmlException::getQueryLine()
   Get the queryLine for an XQuery parser exception */
ZEND_METHOD(XmlException, getQueryLine)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "queryLine", sizeof("queryLine")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto long XmlException::getQueryColumn()
   Get the queryColumn for an XQuery parser exception */
ZEND_METHOD(XmlException, getQueryColumn)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "queryColumn", sizeof("queryColumn")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto string XmlException::getQueryFile()
   Get the query file, if it exists for the XQuery parser exception */
ZEND_METHOD(XmlException, getQueryFile)
{
	if (ZEND_NUM_ARGS() > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	dbxml_exception_get_entry(getThis(), "queryFile", sizeof("queryFile")-1, return_value TSRMLS_CC);
}
/* }}} */

function_entry php_dbxml_XmlException_methods[] = {
  PHP_ME(XmlException, getExceptionCode, NULL, 0)
  PHP_ME(XmlException, getDbErrno, NULL, 0)
  PHP_ME(XmlException, what, NULL, 0)
  PHP_ME(XmlException, getQueryFile, NULL, 0)
  PHP_ME(XmlException, getQueryLine, NULL, 0)
  PHP_ME(XmlException, getQueryColumn, NULL, 0)
  {NULL,NULL,NULL}
};

#endif
