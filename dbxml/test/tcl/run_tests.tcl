# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#

# Tcl script to run the XML regression test suite

source ../test/test.tcl
set err [run_xml]

# TODO return codes for the query plan conversion scripts
convert ci
convert qp

exit $err
