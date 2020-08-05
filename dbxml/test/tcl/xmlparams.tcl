# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#

#set test_names(xml)	[list xml002 xml003 xml006 xml008 xml009 xml010 xml011 xml012 xml013 xml014 xml016 xml017]
set all_xml_test_names	[list xml002 xml003 xml006 xml008 xml009 xml010 xml011 xml012 xml013 xml014 xml016 xml017]
set short_xml_test_names [list xml002 xml003 xml006 xml008 xml009 xml010 xml012 xml013 xml014 xml016 xml017]
set external_xml_test_names [list xml011]

foreach test $all_xml_test_names {
    source $test_path/$test.tcl
}

source $test_path/xmlutils.tcl
source $test_path/xmlshelltest.tcl
source $db_test_path/testutils.tcl
source $db_test_path/upgrade.tcl

# The params file allows you to set default values for 
# parameters passed to test functions.  None are in use for 
# XML testing yet, but here's an example of the expected format.
# set parms(test001) {10000 0 "01" 0}

# XML shell script tests.  Each list entry is a {directory filename} pair,
# invoked with "/bin/sh filename".
set xmlshelltest_list {
	{ xmlscr001		chk.code }
}
