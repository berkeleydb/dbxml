/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 */

#include "php_dbxml_int.hpp"

#define CHAR_STRING_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlEventReader, method) \
{ \
  PHP_DBXML_STUFFED_REF(XmlEventReader); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_STRING((char *)This.method(), 1); \
} PHP_DBXML_METHOD_END()

#define INT_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlEventReader, method) \
{ \
  PHP_DBXML_STUFFED_REF(XmlEventReader); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_LONG((long)This.method()); \
} PHP_DBXML_METHOD_END()

#define BOOL_GETTER(method)		 \
PHP_DBXML_METHOD_BEGIN(XmlEventReader, method) \
{ \
  PHP_DBXML_STUFFED_REF(XmlEventReader); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_BOOL(This.method()); \
} PHP_DBXML_METHOD_END()

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlEventReader_dtor)
{

}

PHP_DBXML_METHOD_BEGIN(XmlEventReader, XmlEventReader)
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "This class cannot be instantiated directly");
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, close)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	This.close();
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getValue)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	size_t len;
	const unsigned char *val = This.getValue(len);
	DBXML_RETURN_STRINGL((char *)val, len, 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getValueLen)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
        size_t len;
	(void) This.getValue(len);
	DBXML_RETURN_LONG(len);
} PHP_DBXML_METHOD_END()
PHP_DBXML_METHOD_BEGIN(XmlEventReader, isAttributeSpecified)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		RETURN_FALSE;
	}
	RETURN_BOOL(This.isAttributeSpecified(index));
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getAttributeLocalName)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		RETURN_FALSE;
	}
	DBXML_RETURN_STRING((char *)This.getAttributeLocalName(index), 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getAttributeNamespaceURI)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		RETURN_FALSE;
	}
	DBXML_RETURN_STRING((char *)This.getAttributeNamespaceURI(index), 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getAttributePrefix)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		RETURN_FALSE;
	}
	DBXML_RETURN_STRING((char *)This.getAttributePrefix(index), 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, getAttributeValue)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		RETURN_FALSE;
	}
	DBXML_RETURN_STRING((char *)This.getAttributeValue(index), 1);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, needsEntityEscape)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	long index = 0;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &index)) {
		RETURN_FALSE;
	}
	RETURN_BOOL(This.needsEntityEscape(index));
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, setReportEntityInfo)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	bool val;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val)) {
		return;
	}
	This.setReportEntityInfo(val);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReader, setExpandEntities)
{
	PHP_DBXML_STUFFED_REF(XmlEventReader);
	bool val;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val)) {
		return;
	}
	This.setExpandEntities(val);
} PHP_DBXML_METHOD_END()

BOOL_GETTER(hasNext);
BOOL_GETTER(isStandalone);
BOOL_GETTER(standaloneSet);
BOOL_GETTER(encodingSet);
BOOL_GETTER(hasEntityEscapeInfo);
BOOL_GETTER(hasEmptyElementInfo);
BOOL_GETTER(isEmptyElement);
BOOL_GETTER(isWhiteSpace);
BOOL_GETTER(getReportEntityInfo);
BOOL_GETTER(getExpandEntities);
INT_GETTER(next);
INT_GETTER(nextTag);
INT_GETTER(getEventType);
INT_GETTER(getAttributeCount);
CHAR_STRING_GETTER(getNamespaceURI);
CHAR_STRING_GETTER(getLocalName);
CHAR_STRING_GETTER(getPrefix);
CHAR_STRING_GETTER(getEncoding);
CHAR_STRING_GETTER(getVersion);
CHAR_STRING_GETTER(getSystemId);

function_entry php_dbxml_XmlEventReader_methods[] = {
	PHP_ME(XmlEventReader, XmlEventReader, NULL, 0)
	PHP_ME(XmlEventReader, close, NULL, 0)
	PHP_ME(XmlEventReader, hasNext, NULL, 0)
	PHP_ME(XmlEventReader, next, NULL, 0)
	PHP_ME(XmlEventReader, nextTag, NULL, 0)
	PHP_ME(XmlEventReader, getEventType, NULL, 0)
	PHP_ME(XmlEventReader, getNamespaceURI, NULL, 0)
	PHP_ME(XmlEventReader, getLocalName, NULL, 0)
	PHP_ME(XmlEventReader, getPrefix, NULL, 0)
	PHP_ME(XmlEventReader, getValue, NULL, 0)
	PHP_ME(XmlEventReader, getValueLen, NULL, 0)
	PHP_ME(XmlEventReader, getAttributeCount, NULL, 0)
	PHP_ME(XmlEventReader, isAttributeSpecified, NULL, 0)
	PHP_ME(XmlEventReader, getAttributeLocalName, NULL, 0)
	PHP_ME(XmlEventReader, getAttributeNamespaceURI, NULL, 0)
	PHP_ME(XmlEventReader, getAttributePrefix, NULL, 0)
	PHP_ME(XmlEventReader, getAttributeValue, NULL, 0)
	PHP_ME(XmlEventReader, getEncoding, NULL, 0)
	PHP_ME(XmlEventReader, getVersion, NULL, 0)
	PHP_ME(XmlEventReader, getSystemId, NULL, 0)
	PHP_ME(XmlEventReader, isStandalone, NULL, 0)
	PHP_ME(XmlEventReader, standaloneSet, NULL, 0)
	PHP_ME(XmlEventReader, encodingSet, NULL, 0)
	PHP_ME(XmlEventReader, hasEntityEscapeInfo, NULL, 0)
	PHP_ME(XmlEventReader, needsEntityEscape, NULL, 0)
	PHP_ME(XmlEventReader, hasEmptyElementInfo, NULL, 0)
	PHP_ME(XmlEventReader, isEmptyElement, NULL, 0)
	PHP_ME(XmlEventReader, isWhiteSpace, NULL, 0)
	PHP_ME(XmlEventReader, setReportEntityInfo, NULL, 0)
	PHP_ME(XmlEventReader, getReportEntityInfo, NULL, 0)
	PHP_ME(XmlEventReader, setExpandEntities, NULL, 0)
	PHP_ME(XmlEventReader, getExpandEntities, NULL, 0)
	{NULL, NULL, NULL}
};
