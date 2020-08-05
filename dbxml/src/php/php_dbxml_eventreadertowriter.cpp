/*-
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlEventReaderToWriter_dtor)
{
	php_dbxml_delete_XmlEventReaderToWriter(rsrc->ptr);
}

PHP_DBXML_METHOD_BEGIN(XmlEventReaderToWriter, XmlEventReaderToWriter)
{
	zval *rval = NULL;
	zval *wval = NULL;
	bool ownsReader = true;
	if (SUCCESS == zend_parse_parameters(
		    ZEND_NUM_ARGS() TSRMLS_CC, "OO|b",
		    &rval, php_dbxml_XmlEventReader_ce,
		    &wval, php_dbxml_XmlEventWriter_ce,
		    &ownsReader)) {
		XmlEventReader &reader =
			php_dbxml_get_XmlEventReader_object_ref(rval TSRMLS_CC);
		XmlEventWriter &writer =
			php_dbxml_get_XmlEventWriter_object_ref(wval TSRMLS_CC);
		XmlEventReaderToWriter r2w(reader, writer, ownsReader);
		php_dbxml_set_XmlEventReaderToWriter_object_factory(
			getThis(), r2w TSRMLS_CC);
	} else {
		php_error(E_ERROR, "Could not parse args to r2w");
	}
	RETURN_FALSE;
} PHP_DBXML_METHOD_END()

PHP_DBXML_METHOD_BEGIN(XmlEventReaderToWriter, start)
{
	PHP_DBXML_STUFFED_THIS(XmlEventReaderToWriter);
	This.start();
} PHP_DBXML_METHOD_END()

function_entry php_dbxml_XmlEventReaderToWriter_methods[] = {
	PHP_ME(XmlEventReaderToWriter, XmlEventReaderToWriter, NULL, 0)
	PHP_ME(XmlEventReaderToWriter, start, NULL, 0)
	{NULL, NULL, NULL}
};
