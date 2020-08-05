# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml006
# TEST	I18N & L10N
# TEST	Test storage, querying, and retrieval of XML documents
# TEST	that contain multi-byte characters.  UTF-8 should be
# TEST	accepted, others should be rejected.

proc xml006 { args } {
    source ./include.tcl
    puts "\nxml006: I18N & L10N ($args)"

    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 6
    set oargs [eval {set_openargs} $args]

    if { $eindex == -1 } {
	set env NULL
    } else {
	incr eindex
	set env [lindex $args $eindex]
	set txnenv [is_txnenv $env]
	set testdir [get_home $env]
    }

    # assign the container type (default to whole document)
    set tindex [lsearch -exact $args "-storage"]
    if { $tindex == -1 } {
	set global_container_type $XmlContainer_WholedocContainer
    } else {
	incr tindex
	set global_container_type [lindex $args $tindex]
    }

    if { [lsearch -exact $args "-indexnodes"] >= 0 } {
	set global_container_flags $DBXML_INDEX_NODES
    } else {
	set global_container_flags $DBXML_NO_INDEX_NODES
    }

    set basename $tnum

    xml006_1 $env $txnenv $basename.1 $oargs
    xml006_2 $env $txnenv $basename.2 $oargs
}

proc xml006_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\tXml006.1: Test of multi-byte characters."

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn "$basename.1.dbxml" $oargs]
	$xtxn commit
	delete xtxn
	wrap XmlTransaction txn [$db createTransaction]
    } else {
	wrap XmlContainer container [$db createContainer "$basename.1.dbxml" $oargs]
    }

    # Test Group 6.1

    # 6.1.1 UTF-8 encoded XML document with characters in the range [#x9 | #xA | #xD | [#x20-#xff]].
    puts "\t\tXml006.1.1: UTF-8 (regular)"
    xml006_assertDocumentPut "utf_8_regular" $db $container \
	$test_path/document_set_6_1/utf_8_regular.xml $env $txn

    # 6.1.2 UTF-8 encoded XML document with characters in the range [#x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]]
    puts "\t\tXml006.1.2: UTF-8 (special)"
    xml006_assertDocumentPut "utf_8_special" $db $container \
	$test_path/document_set_6_1/utf_8_special.xml $env $txn

    # 6.1.3 UTF-16 (little endian byte order) encoded XML document
    puts "\t\tXml006.1.3: UTF-16 (little Endian)"
    xml006_assertDocumentPut "utf_16_le" $db $container \
	$test_path/document_set_6_1/utf_16_le.xml  $env $txn

    # 6.1.4 UTF-16 (big endian byte order) encoded XML document
    puts "\t\tXml006.1.4: UTF-16 (big Endian)"
    xml006_assertDocumentPut "utf_16_be" $db $container \
	$test_path/document_set_6_1/utf_16_be.xml  $env $txn

    # 6.1.5 ISO-10646-UCS-4 (little endian byte order) encoded XML document
    # Removed - "not put" tests depend upon Xerces configuration
    #puts "\t\tXml006.1.5: UCS4 (little Endian)"
    #xml006_assertDocumentNotPut "ucs_4_le" $db $container \
	$test_path/document_set_6_1/ucs_4_le.xml  $env $txn

    # 6.1.6 ISO-10646-UCS-4 (big endian byte order) encoded XML document
    # Removed - "not put" tests depend upon Xerces configuration
    #puts "\t\tXml006.1.6.1: UCS4 (big Endian)"
    #xml006_assertDocumentNotPut "ucs_4_be" $db $container \
	$test_path/document_set_6_1/ucs_4_be.xml  $env $txn

    # Removed - depends on Xerces configuration
    #puts "\t\tXml006.1.6.1: Big5"
    #xml006_assertDocumentPut "big_5" $db $container \
	$test_path/document_set_6_1/big_5.xml  $env $txn

    # Disabled - depends on Xerces configuration
    #puts "\t\tXml006.1.6.2: UTF-8 ISO-8859 (Russian)"
    #xml006_assertDocumentPut "iso_8859_5_russian" $db $container \
	$test_path/document_set_6_1/iso_8859_5_russian.xml  $env $txn

    # 6.1.7 Shift-JIS (Japanese)  encoded XML document
    # Disabled - depends on Xerces configuration
    #puts "\t\tXml006.1.7: Japanese (Shift-JIS)"
    #xml006_assertDocumentPut "shift_jis_japanese" $db $container \
	$test_path/document_set_6_1/shift_jis_japanese.xml $env $txn

    # 6.1.8 (UCS-4 Japanese) encoded XML document
    # Removed - "not put" tests depend upon Xerces configuration
    #puts "\t\tXml006.1.8: Japanese (UCS-4)"
    #xml006_assertDocumentNotPut "ucs_4_japanese" $db $container \
	$test_path/document_set_6_1/ucs_4_japanese.xml $env $txn

    # 6.1.9 (UTF-16 Japanese) encoded XML document
    # Removed - depends upon run-time environment
    #puts "\t\tXml006.1.9: Japanese (UTF-16)"
    #xml006_assertDocumentPut "utf_16_japanese" $db $container \
	$test_path/document_set_6_1/utf_16_japanese.xml $env $txn

    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    delete container
    delete db
}

proc xml006_assertDocumentPut { testid db container filename {env "NULL"} {txn "NULL"} } {
    set ok [putFileInContainer $db $container $filename $txn]
    dbxml_error_check_bad $testid $ok 0
}

proc xml006_assertDocumentNotPut { testid db container filename {env "NULL"} {txn "NULL"} } {
    catch {set ok [putFileInContainer $db $container $filename $txn]}
    dbxml_error_check_good $testid $ok 0
}

proc xml006_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs {regress 0} } {
    source ./include.tcl
    set fd [open "$test_path/xml006_utf8.tcl" r]
    fconfigure $fd -encoding utf-8
    set script [read $fd]
    close $fd
    eval $script
    xml006_2_utf8 $env $txnenv $basename $oargs $regress
}
