/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlEventWriter_dtor)
{

}

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, XmlEventWriter)
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR,
			 "This class cannot be instantiated directly");
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, close)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	This.close();
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeAttribute)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *lname, *prefix, *uri, *value;
	int lLen, pLen, uLen, vLen;
	bool specified = true; // optional in PHP interface
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss|b",
				  &lname, &lLen, &prefix, &pLen, &uri, &uLen,
				  &value, &vLen, &specified)) {
		This.writeAttribute((const unsigned char *)lname,
				    (const unsigned char *)prefix,
				    (const unsigned char *)uri,
				    (const unsigned char *)value,
				    specified);
	}
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeText)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	int type;
	char *text;
	int unused; // length is ignored if passed to PHP's writeText
	int tLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|l", &type,
				  &text, &tLen, &unused))
		This.writeText((XmlEventReader::XmlEventType)type,
			       (const unsigned char *)text, tLen);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeProcessingInstruction)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *target, *data;
	int tLen, dLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &target,
				  &tLen, &data, &dLen))
		This.writeProcessingInstruction((const unsigned char *)target,
						(const unsigned char *)data);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeStartElement)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *lname, *prefix, *uri;
	int lLen, pLen, uLen, nattrs;
	bool empty = false; // default in PHP
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|b",
				  &lname, &lLen, &prefix, &pLen, &uri, &uLen,
				  &nattrs, &empty)) {
		This.writeStartElement((const unsigned char *)lname,
				       (const unsigned char *)prefix,
				       (const unsigned char *)uri,
				       nattrs, empty);
	}
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeEndElement)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *lname, *prefix, *uri;
	int lLen, pLen, uLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
				  &lname, &lLen, &prefix, &pLen, &uri, &uLen)) {
		This.writeEndElement((const unsigned char *)lname,
				     (const unsigned char *)prefix,
				     (const unsigned char *)uri);
	}
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeDTD)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *text;
	int unused; // length is ignored if passed 
	int tLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
				  &text, &tLen, &unused))
		This.writeDTD((const unsigned char *)text, tLen);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeStartDocument)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	// all args are optional
	char *version = 0;
	char *encoding = 0;
	char *standalone = 0;
	int vLen, eLen, sLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss",
				  &version, &vLen, &encoding, &eLen,
				  &standalone, &sLen)) {
		This.writeStartDocument((const unsigned char *)version,
					(const unsigned char *)encoding,
					(const unsigned char *)standalone);
	}
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeEndDocument)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	This.writeEndDocument();
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeStartEntity)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *name;
	int tLen;
	bool expandedInfoFollows = true; // default this
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b",
				  &name, &tLen, &expandedInfoFollows))
		This.writeStartEntity((const unsigned char *)name,
				      expandedInfoFollows);
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventWriter, writeEndEntity)
{
	PHP_DBXML_STUFFED_REF(XmlEventWriter);
	char *name;
	int tLen;
	if (SUCCESS ==
	    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				  &name, &tLen))
		This.writeEndEntity((const unsigned char *)name);
} PHP_DBXML_METHOD_END()

function_entry php_dbxml_XmlEventWriter_methods[] = {
	PHP_ME(XmlEventWriter, XmlEventWriter, NULL, 0)
	PHP_ME(XmlEventWriter, close, NULL, 0)
	PHP_ME(XmlEventWriter, writeAttribute, NULL, 0)
	PHP_ME(XmlEventWriter, writeText, NULL, 0)
	PHP_ME(XmlEventWriter, writeProcessingInstruction, NULL, 0)
	PHP_ME(XmlEventWriter, writeStartElement, NULL, 0)
	PHP_ME(XmlEventWriter, writeEndElement, NULL, 0)
	PHP_ME(XmlEventWriter, writeDTD, NULL, 0)
	PHP_ME(XmlEventWriter, writeStartDocument, NULL, 0)
	PHP_ME(XmlEventWriter, writeEndDocument, NULL, 0)
	PHP_ME(XmlEventWriter, writeStartEntity, NULL, 0)
	PHP_ME(XmlEventWriter, writeEndEntity, NULL, 0)
	{NULL, NULL, NULL}
};
