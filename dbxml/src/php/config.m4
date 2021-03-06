dnl config.m4 for extension dbxml

PHP_ARG_WITH(dbxml, whether to enable dbxml support,
[  --with-dbxml           Enable dbxml support])
PHP_ARG_WITH(berkeleydb, where to find Berkeley DB,
[  --with-berkeleydb      where to find the Berkeley DB installation])
PHP_ARG_WITH(xerces, where to find the Xerces-c library,
[  --with-xerces      where to find the Xerces-c library])
PHP_ARG_WITH(xqilla, where to find the XQilla library,
[  --with-xqilla      where to find the XQilla library])

CC=g++
EXTRA_CXXFLAGS="-g -DHAVE_CONFIG_H -O2 -Wall"
if test "$PHP_DBXML" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, DBXML_SHARED_LIBADD)

  PHP_ADD_LIBRARY_WITH_PATH(db, $PHP_BERKELEYDB/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($PHP_BERKELEYDB/include)
  PHP_ADD_INCLUDE(../../../db-4.8.26/php_db4)

  PHP_ADD_LIBRARY_WITH_PATH(dbxml, $PHP_DBXML/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($PHP_DBXML/include)
   
  PHP_ADD_LIBRARY_WITH_PATH(xerces-c, $PHP_XERCES/lib, DBXML_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(xqilla, $PHP_XQILLA/lib, DBXML_SHARED_LIBADD)

  PHP_NEW_EXTENSION(dbxml, php_dbxml.cpp php_dbxml_manager.cpp php_dbxml_resolver.cpp php_dbxml_container.cpp php_dbxml_containerconfig.cpp php_dbxml_eventreader.cpp php_dbxml_eventwriter.cpp php_dbxml_eventreadertowriter.cpp php_dbxml_exception.cpp php_dbxml_inputstream.cpp php_dbxml_transaction.cpp php_dbxml_results.cpp php_dbxml_value.cpp php_dbxml_queryexpression.cpp php_dbxml_querycontext.cpp php_dbxml_updatecontext.cpp php_dbxml_indexspecification.cpp php_dbxml_data.cpp php_dbxml_document.cpp php_dbxml_statistics.cpp php_dbxml_indexlookup.cpp, $ext_shared)
  PHP_SUBST(DBXML_SHARED_LIBADD)
fi
