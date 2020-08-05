# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml006_utf8

proc xml006_2_utf8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs {regress 0} } {
  source ./include.tcl

	set documents {
		document_set_6_2/document_6_2_match.xml
		document_set_6_2/document_6_2_not_match_1.xml
		document_set_6_2/document_6_2_not_match_2.xml
	}

  # basename ignored
  set dir "$test_path/data_set_6"
  set exclude 0
  container_indexing_tests "6" "6.2" $dir $documents 0 $exclude $env $txnenv $oargs $regress

	return
}
