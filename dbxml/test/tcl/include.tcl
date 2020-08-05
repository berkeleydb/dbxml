# Automatically built by dist/s_test; may require local editing.

set tclsh_path @TCL_TCLSH@

set src_root ..
set test_path $src_root/test/tcl
source $test_path/globals.tcl

set dbxml_tcllib .libs/libdbxml_tcl-@DBXML_VERSION_MAJOR@.@DBXML_VERSION_MINOR@@LIBTSO_MODSUFFIX@

set db_tcllib @DB_TCLLIB@@LIBTSO_MODSUFFIX@
set db_test_path @DB_TEST_PATH@
set db_util_path @DB_UTIL_PATH@
set xmlutil_path .
set xmltest_path .

set KILL "@db_cv_path_kill@"
