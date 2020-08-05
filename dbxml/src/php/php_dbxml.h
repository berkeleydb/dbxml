/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

*/

#ifndef PHP_DBXML_H
#define PHP_DBXML_H
extern zend_module_entry dbxml_module_entry;
#define phpext_dbxml_ptr &dbxml_module_entry

#ifdef PHP_WIN32
#define PHP_DBXML_API __declspec(dllexport)
#else
#define PHP_DBXML_API
#endif

#ifdef ZEND_ENGINE_2
#define PHP_DBXML_EXCEPTIONS 1
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef PHP_DBXML_EXCEPTIONS
#include "zend_exceptions.h"
#endif

PHP_MINIT_FUNCTION(dbxml);
PHP_MSHUTDOWN_FUNCTION(dbxml);
PHP_RINIT_FUNCTION(dbxml);
PHP_RSHUTDOWN_FUNCTION(dbxml);
PHP_MINFO_FUNCTION(dbxml);

PHP_FUNCTION(confirm_dbxml_compiled);	/* For testing, remove later. */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(dbxml)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(dbxml)
*/

/* In every utility function you add that needs to use variables 
   in php_dbxml_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as DBXML_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define DBXML_G(v) TSRMG(dbxml_globals_id, zend_dbxml_globals *, v)
#else
#define DBXML_G(v) (dbxml_globals.v)
#endif

#endif	/* PHP_DBXML_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
