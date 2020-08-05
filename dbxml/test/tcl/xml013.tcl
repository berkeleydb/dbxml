# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml013
# TEST	Document Metadata (including XmlValue API)

proc xml013 { args } {
    puts "\nxml013: Document Metadata and Values ($args)"
    source ./include.tcl

    # Determine whether test has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 13
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

    # document content (as Dbt, string, empty)
    xml013_2 $env $txnenv $basename.2 $oargs

    # metadata as non-atomic types
    xml013_3 $env $txnenv $basename.3 $oargs

    # metadata as atomic types
    xml013_4 $env $txnenv $basename.4 $oargs

    # type conversion on XmlValues
    xml013_5 $env $txnenv $basename.5 $oargs

    # isType() methods on XmlValues
    xml013_6 $env $txnenv $basename.6 $oargs

    # validation of XmlValues
    xml013_7 $env $txnenv $basename.7 $oargs

    # indexing of metadata
    xml013_8 $env $txnenv $basename.8 $oargs

    # XmlMetaDataIterator
    xml013_9 $env $txnenv $basename.9 $oargs

    # metaData removal
    xml013_10 $env $txnenv $basename.10 $oargs

    # navigation on XmlValue nodes
    xml013_11 $env $txnenv $basename.11 $oargs

    # boundary cases - empty names and values, URIs, ...
    xml013_12 $env $txnenv $basename.12 $oargs

    # miscellaneous API tests for XmlResults and XmlValue
    xml013_13 $env $txnenv $basename.13 $oargs

    return
}

proc xml013_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.2: Document content"
    source ./include.tcl
    xml_cleanup $testdir $env
    set id 0

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs]
    }

    # put content as Dbt / get as String
    set testid $basename.[incr id]
    puts "\t\t$testid: Put content as Dbt / Get content as string"
    set doc_name "Dumb"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    $xd setName $doc_name

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	set xd_content [$xd getContentAsString]
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]
	set xd_content [$xd getContentAsString]
	delete xd
    }

    dbxml_error_check_good $testid $xd_content "<hello/>"

    # put content as Dbt / get as Dbt
    set testid $basename.[incr id]
    puts "\t\t$testid: Put content as Dbt / Get content as Dbt"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Dumber"
    $xd setName $doc_name

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	set xd_content [$xd getContentAsString]
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]
	set xd_content [$xd getContentAsString]
	delete xd
    }

    dbxml_error_check_good $testid $xd_content "<hello/>"

    # empty document
    set testid $basename.[incr id]
    set emptyName "empty"
    puts "\t\t$testid: Empty document"

    wrap XmlDocument xd [$db createDocument]
    $xd setName $emptyName
    $xd setContent {}

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd

    # query the empty document for content
    set testid $basename.[incr id]
    puts "\t\t$testid: Query empty document with collection()"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }

    wrap XmlQueryContext qc [$db createQueryContext]
    wrap XmlResults res [$db query $txn "collection('$basename.dbxml')" $qc]

    dbxml_error_check_good "$testid size" [$res size] 3

    delete res

    if { $txnenv == 1 } {
	$txn commit
    }
    delete txn


    # query the empty document for content
    set testid $basename.[incr id]
    puts "\t\t$testid: Query empty document with collection()/node()"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }

    wrap XmlResults res [$db query $txn "collection('$basename.dbxml')/node()" $qc]

    dbxml_error_check_good "$testid size" [$res size] 2

    delete res

    if { $txnenv == 1 } {
	$txn commit
    }
    delete txn


    # query the empty document for content
    set testid $basename.[incr id]
    puts "\t\t$testid: Query empty document with collection()/descendant-or-self::node()"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }

    wrap XmlResults res [$db query $txn "collection('$basename.dbxml')/descendant-or-self::node()" $qc]

    dbxml_error_check_good "$testid size" [$res size] 5

    delete res

    if { $txnenv == 1 } {
	$txn commit
    }
    delete txn

    delete qc

    # put content in the document (update)
    set testid $basename.[incr id]
    puts "\t\t$testid: Put content in empty document"
    set newContent "<a>xxx</a>"

    wrap XmlDocument xd1 [$container getDocument $emptyName ]
    dbxml_error_check_good $testid [$xd1 getContentAsString] ""
    $xd1 setContent $newContent

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container updateDocument $txn $xd1 $uc
	$txn commit
	delete txn
    } else {
	$container updateDocument $xd1 $uc
    }
    delete xd1
    wrap XmlDocument xd [$container getDocument $emptyName ]
    dbxml_error_check_good $testid [$xd getContentAsString] $newContent

    # put empty content in the document (update)
    set testid $basename.[incr id]
    puts "\t\t$testid: Put empty content in non-empty document"
    $xd setContent {}
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container updateDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container updateDocument $xd $uc
    }
    delete xd
    wrap XmlDocument xd1 [$container getDocument $emptyName ]
    dbxml_error_check_good $testid [$xd1 getContentAsString] ""
    delete xd1

    # remove empty document
    set testid $basename.[incr id]
    puts "\t\t$testid: Remove empty document"
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container deleteDocument $txn $emptyName $uc
	$txn commit
	delete txn
    } else {
	$container deleteDocument $emptyName $uc
    }
    catch { 
	wrap XmlDocument xd [$container getDocument $emptyName ]
    } ret
    dbxml_error_check_good $testid [is_substr $ret "Document not found"] 1

    # clean up
    delete container
    delete uc
    delete db
}

proc xml013_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.3: Metadata as non-atomic types"

    source ./include.tcl
    xml_cleanup $testdir $env
    set id 0

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs]
    }

    # 2004-07-15 arw disabled since XmlValues can no longer be documents
    set testid $basename.[incr id]
    #puts "\t\t$testid: Put/get type Document"

    set testid $basename.[incr id]
    puts "\t\t$testid: Put type Node"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    new XmlValue xv $XmlValue_NODE {}
    catch { $xd setMetaData "" "foo" $xv } ret
    dbxml_error_check_good $testid [is_substr $ret "setMetaData expects"] 1
    delete xd
    delete xv

    # 2004-07-15 arw disabled since XmlValues can no longer be variables
    set testid $basename.[incr id]
    #puts "\t\t$testid: Put type Variable"

    delete container
    delete uc
    delete db
}

proc xml013_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.4: Metadata as atomic types"

    # NOTE the XmlValue(Type, string) constructors do not validate values,
    # meaning that they are arbitrary strings.

    # prepare - database, context, container
    source ./include.tcl
    xml_cleanup $testdir $env
    set id 0

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs]
    }

    #################################################################
    # AnyURI
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type AnyURI"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "AnyURI"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_ANY_URI "http://www.sleepycat.com/2002/dbxml"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_ANY_URI "http://groceryItem.dbxml/fruits"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_ANY_URI] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "anyURI"
    dbxml_error_check_good $testid $foo_string http://www.sleepycat.com/2002/dbxml

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_ANY_URI] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "anyURI"
    dbxml_error_check_good $testid $bar_string http://groceryItem.dbxml/fruits

    #################################################################
    # Base 64 Binary
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Base 64 Binary"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Base64"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_BASE_64_BINARY "FFFF"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_BASE_64_BINARY "7474"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_BASE_64_BINARY] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "base64Binary"
    dbxml_error_check_good $testid $foo_string FFFF

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_BASE_64_BINARY] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "base64Binary"
    dbxml_error_check_good $testid $bar_string 7474

    #################################################################
    # Boolean
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Boolean"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Boolean"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_BOOLEAN "true"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_BOOLEAN "false"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_BOOLEAN] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "boolean"
    dbxml_error_check_good $testid $foo_string true

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_BOOLEAN] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "boolean"
    dbxml_error_check_good $testid $bar_string false

    #################################################################
    # Date
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type date"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Date"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DATE "2002-12-15"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DATE "1999-12-31"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DATE] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "date"
    dbxml_error_check_good $testid $foo_string 2002-12-15

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DATE] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "date"
    dbxml_error_check_good $testid $bar_string 1999-12-31

    #################################################################
    # Date-Time
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type date-time"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "DateTime"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DATE_TIME "2002-12-15T09:00:00"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DATE_TIME "1999-12-31T23:59:59"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DATE_TIME] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "dateTime"
    dbxml_error_check_good $testid $foo_string 2002-12-15T09:00:00

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DATE_TIME] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "dateTime"
    dbxml_error_check_good $testid $bar_string 1999-12-31T23:59:59

    #################################################################
    # Decimal
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type decimal"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Decimal"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DECIMAL "123.456"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DECIMAL "-5"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DECIMAL] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "decimal"
    dbxml_error_check_good $testid $foo_string 123.456

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DECIMAL] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "decimal"
    dbxml_error_check_good $testid $bar_string -5

    #################################################################
    # Double
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type double"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Double"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DOUBLE -.123E4
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DOUBLE  -INF
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DOUBLE] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "double"
    dbxml_error_check_good $testid $foo_string -.123E4

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DOUBLE] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "double"
    dbxml_error_check_good $testid $bar_string -INF

    #################################################################
    # Duration
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type duration"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Duration"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DURATION P1DT21H2M18S
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DURATION P9999Y
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DURATION] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "duration"
    dbxml_error_check_good $testid $foo_string P1DT21H2M18S

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DURATION] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "duration"
    dbxml_error_check_good $testid $bar_string P9999Y

    #################################################################
    # Float
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type float"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Float"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_FLOAT 123.4
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_FLOAT -.456
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_FLOAT] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "float"
    dbxml_error_check_good $testid $foo_string 123.4

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_FLOAT] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "float"
    dbxml_error_check_good $testid $bar_string -.456

    #################################################################
    # Gregorian Day
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Gregorian day"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "GDay"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_G_DAY ---31
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_G_DAY ---01
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_G_DAY] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "gDay"
    dbxml_error_check_good $testid $foo_string ---31

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_G_DAY] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "gDay"
    dbxml_error_check_good $testid $bar_string ---01

    #################################################################
    # Gregorian Month
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Gregorian month"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "GMonth"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_G_MONTH --12--
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_G_MONTH --02--
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_G_MONTH] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "gMonth"
    dbxml_error_check_good $testid $foo_string --12--

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_G_MONTH] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "gMonth"
    dbxml_error_check_good $testid $bar_string --02--

    #################################################################
    # Gregorian Month-Day
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Gregorian month-day"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "GMonthDay"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_G_MONTH_DAY --02-29
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_G_MONTH_DAY --04-03
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_G_MONTH_DAY] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "gMonthDay"
    dbxml_error_check_good $testid $foo_string --02-29

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_G_MONTH_DAY] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "gMonthDay"
    dbxml_error_check_good $testid $bar_string --04-03

    #################################################################
    # Gregorian Year
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Gregorian year"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "GYear"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_G_YEAR 1968
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_G_YEAR 1970
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_G_YEAR] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "gYear"
    dbxml_error_check_good $testid $foo_string 1968

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_G_YEAR] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "gYear"
    dbxml_error_check_good $testid $bar_string 1970

    #################################################################
    # Gregorian Year-Month
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type Gregorian year-month"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "GYearMonth"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_G_YEAR_MONTH 2004-01
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_G_YEAR_MONTH 2003-12
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_G_YEAR_MONTH] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "gYearMonth"
    dbxml_error_check_good $testid $foo_string 2004-01

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_G_YEAR_MONTH] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "gYearMonth"
    dbxml_error_check_good $testid $bar_string 2003-12

    #################################################################
    # Hex Binary
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type hex binary"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "HexBinary"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_HEX_BINARY CC
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_HEX_BINARY FF
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_HEX_BINARY] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "hexBinary"
    dbxml_error_check_good $testid $foo_string CC

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_HEX_BINARY] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "hexBinary"
    dbxml_error_check_good $testid $bar_string FF

    #################################################################
    # NOTATION
    set testid $basename.[incr id]
    dbxml_pending_test $testid 0000 "Need to create valid notations"
    if { 0 } {
    puts "\t\t$testid: Put/get type NOTATION"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Notation"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_NOTATION notation
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_NOTATION qname
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_NOTATION] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "NOTATION"
    dbxml_error_check_good $testid $foo_string notation

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_NOTATION] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "NOTATION"
    dbxml_error_check_good $testid $bar_string qname
    }

    #################################################################
    # QName
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type QName"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "QName"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_QNAME xs:decimal
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_QNAME xs:string
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_QNAME] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "QName"
    dbxml_error_check_good $testid $foo_string xs:decimal

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_QNAME] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "QName"
    dbxml_error_check_good $testid $bar_string xs:string

    #################################################################
    # String
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type String"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "String"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_STRING "bar"
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_STRING "foo"
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_STRING] "1"
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "string"
    dbxml_error_check_good $testid $foo_string "bar"

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_STRING] "1"
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "string"
    dbxml_error_check_good $testid $bar_string "foo"

    #################################################################
    # Time
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type time"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "Time"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_TIME 11:01:03
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_TIME 14:15:00
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_TIME] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "time"
    dbxml_error_check_good $testid $foo_string 11:01:03

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_TIME] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "time"
    dbxml_error_check_good $testid $bar_string 14:15:00

    #################################################################
    # Day-Time Duration
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type dayTimeDuration"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "DayTimeDuration"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_DAY_TIME_DURATION P1DT21H2M18S
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DAY_TIME_DURATION PT3600S
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_DAY_TIME_DURATION] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "dayTimeDuration"
    dbxml_error_check_good $testid $foo_string P1DT21H2M18S

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DAY_TIME_DURATION] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "dayTimeDuration"
    dbxml_error_check_good $testid $bar_string PT3600S

    #################################################################
    # Year-Month Duration
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get type yearMonthDuration"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "YearMonthDuration"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_YEAR_MONTH_DURATION P35Y10M
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_YEAR_MONTH_DURATION P24Y2M
    $xd setMetaData "" "bar" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_type_uri [$xv getTypeURI]
	set foo_type_name [$xv getTypeName]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_type_uri [$xv getTypeURI]
	set bar_type_name [$xv getTypeName]
	set bar_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_YEAR_MONTH_DURATION] 1
    dbxml_error_check_good $testid $foo_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $foo_type_name "yearMonthDuration"
    dbxml_error_check_good $testid $foo_string P35Y10M

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_YEAR_MONTH_DURATION] 1
    dbxml_error_check_good $testid $bar_type_uri "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good $testid $bar_type_name "yearMonthDuration"
    dbxml_error_check_good $testid $bar_string P24Y2M

    # clean up
    delete container
    delete uc
    delete db
}

proc xml013_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.5: Type conversion on Values"

    global is_windows_test

    # asString tested in the put/get meta data tests

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    puts "\t\txml013.5.1: Conversion to boolean"
    catch {
	new XmlValue xv $XmlValue_BOOLEAN "anything"
    } ret
    dbxml_error_check_good $basename.1.1.1 [is_substr $ret "The value"] 1
    new XmlValue xv $XmlValue_BOOLEAN "true"
    dbxml_error_check_good $basename.1.1.2 [$xv asBoolean] 1
    delete xv

    new XmlValue xv $XmlValue_DOUBLE "123"
    dbxml_error_check_good $basename.1.2.1 [$xv asBoolean] 1
    delete xv
    new XmlValue xv $XmlValue_DOUBLE "0"
    dbxml_error_check_good $basename.1.2.2 [$xv asBoolean] 0
    delete xv
    new XmlValue xv $XmlValue_DOUBLE "NaN"
    dbxml_error_check_good $basename.1.2.3 [$xv asBoolean] 0
    delete xv

    new XmlValue xv $XmlValue_STRING "hello"
    dbxml_error_check_good $basename.1.3.1 [$xv asBoolean] 1
    delete xv
    new XmlValue xv $XmlValue_STRING "false"
    dbxml_error_check_good $basename.1.3.2 [$xv asBoolean] 0
    delete xv
    new XmlValue xv $XmlValue_STRING ""
    dbxml_error_check_good $basename.1.3.3 [$xv asBoolean] 0
    delete xv

    new XmlValue xv $XmlValue_DECIMAL "0"
    dbxml_error_check_good $basename.1.4.1 [$xv asBoolean] 0
    delete xv

    puts "\t\txml013.5.2: Conversion to number"

    # create our own lexical values to avoid issues with platform-specific
    # representations of scientific notation

    catch {
	new XmlValue xv $XmlValue_BOOLEAN "anything"
    } ret
    dbxml_error_check_good $basename.2.1.1 [is_substr $ret "The value"] 1
    new XmlValue xv $XmlValue_BOOLEAN "true"
    dbxml_error_check_good $basename.2.1.2 [$xv asNumber] 1.0
    delete xv

    set v 123
    set expected [format "%6.2f" $v]
    new XmlValue xv $XmlValue_DECIMAL "123."
    set actual [format "%6.2f" [$xv asNumber]]
    dbxml_error_check_good $basename.2.2.1 $actual $expected
    delete xv

    set v -0.0000123
    set expected [format "%11.8f" $v]
    new XmlValue xv $XmlValue_FLOAT "-.123E-4"
    set actual [format "%11.8f" [$xv asNumber]]
    dbxml_error_check_good $basename.2.2.2 $actual $expected
    delete xv

    new XmlValue xv $XmlValue_DOUBLE "INF"

    set expected [getInfValue]
    dbxml_error_check_good $basename.2.2.3 [$xv asNumber] $expected
    delete xv

    set v 1234
    set expected [format "%6.2f" $v]

    new XmlValue xv $XmlValue_STRING 1234
    set actual [format "%6.2f" [$xv asNumber]]
    dbxml_error_check_good $basename.2.3.1 $actual $expected
    delete xv

    set v -.90E+14
    set expected [format "%10.2e" $v]
    new XmlValue xv $XmlValue_STRING "-.90E+14"
    set actual [format "%10.2e" [$xv asNumber]]
    dbxml_error_check_good $basename.2.3.2 $actual $expected
    delete xv

    set expected [getNanValue]
    new XmlValue xv $XmlValue_STRING "NaN"
    dbxml_error_check_good $basename.2.3.3 [$xv asNumber] $expected
    delete xv
    new XmlValue xv $XmlValue_STRING ""
    dbxml_error_check_good $basename.2.3.4 [$xv asNumber] $expected
    delete xv
    new XmlValue xv $XmlValue_STRING "not a number"
    dbxml_error_check_good $basename.2.3.5 [$xv asNumber] $expected
    delete xv
    delete db
}

proc xml013_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.6: Legacy API: isType() methods"

    # limited testing due to problems with overloaded CTORs on native types

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    puts "\t\txml013.6.1: Numeric values"

    new XmlValue xv $XmlValue_DOUBLE 123.4
    dbxml_error_check_good $basename.1.1 [$xv isNumber] 1
    delete xv

    new XmlValue xv $XmlValue_DECIMAL 123
    dbxml_error_check_good $basename.1.2 [$xv isNumber] 1
    delete xv

    new XmlValue xv $XmlValue_DECIMAL "123"
    dbxml_error_check_good $basename.1.3 [$xv isNumber] 1
    delete xv

    new XmlValue xv $XmlValue_FLOAT "123E1"
    dbxml_error_check_good $basename.1.4 [$xv isNumber] 1
    delete xv

    new XmlValue xv $XmlValue_BOOLEAN "0"
    dbxml_error_check_good $basename.1.5 [$xv isNumber] 0
    delete xv

    puts "\t\txml013.6.2: String values"

    new XmlValue xv $XmlValue_STRING 0.0
    dbxml_error_check_good $basename.2.1 [$xv isString] 1
    delete xv

    new XmlValue xv $XmlValue_ANY_URI "http://groceryItem.dbxml/fruits"
    dbxml_error_check_good $basename.2.2 [$xv isString] 0
    delete xv

    puts "\t\txml013.6.3: Boolean values"
    # limited tests since a boolean and a number are indistinguishable

    new XmlValue xv "false"
    dbxml_error_check_good $basename.3.1 [$xv isBoolean] 0
    delete xv

    new XmlValue xv $XmlValue_BOOLEAN "false"
    dbxml_error_check_good $basename.3.2 [$xv isBoolean] 1
    delete xv

    delete db

}

proc xml013_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml013.7: Validating values on the variable store"

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    puts "\t\txml013.7.0: create XmlValue without XmlManager (should fail)"
    catch {
	new XmlValue xv0
	delete xv0
    } ret
    dbxml_error_check_good $basename.0 [is_substr $ret "Cannot construct XmlValue"] 1
    catch {
	new XmlValue xv0 "string"
	delete xv0
    } ret
    dbxml_error_check_good $basename.0 [is_substr $ret "Cannot construct XmlValue"] 1

    # done with no-manager test

    xml_database db $testdir $env

    set exceptionSubstring "The value \""

    puts "\t\txml013.7.1: anyURI"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_ANY_URI "http://dmoz.org/World/Fran%e7ais"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.1 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_ANY_URI "\""
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.1 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.2: base64Binary"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_BASE_64_BINARY "0FB7"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.2 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_BASE_64_BINARY "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.2 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.3: boolean"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_BOOLEAN "false"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.3 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_BOOLEAN "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.3 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.4: date"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DATE "2004-03-10"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.4 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DATE "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.4 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.5: dateTime"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DATE_TIME "2004-03-10T11:25:00+00:00"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.5 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DATE_TIME "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.5 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.6: dayTimeDuration"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DAY_TIME_DURATION "P1DT21H2M18S"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.6 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DAY_TIME_DURATION "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.6 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.7: decimal"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DECIMAL "-1234"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.7 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DECIMAL "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.7 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.8: Doubles"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DOUBLE "-INF"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.8 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DOUBLE "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.8 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.9: duration"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_DURATION "P1DT21H2M18S"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.9 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_DURATION "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.9 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.10: float"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_FLOAT ".123e-4"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.10 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_FLOAT "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.10 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.11: gDay"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_G_DAY "---10"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.11 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_G_DAY "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.11 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.12: gMonth"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_G_MONTH "--12--"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.12 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_G_MONTH "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.12 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.13: gMonthDay"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_G_MONTH_DAY "--02-29"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.13 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_G_MONTH_DAY "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.13 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.14: gYear"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_G_YEAR "2004"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.14 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_G_YEAR "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.14 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.15: gYearMonth"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_G_YEAR_MONTH "2004-03"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.15 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_G_YEAR_MONTH "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.15 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.16: hexBinary"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_HEX_BINARY "0FB7"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.16 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_HEX_BINARY "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.16 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.17: NOTATION"
    wrap XmlQueryContext context [$db createQueryContext]

    # ARW do not know how / if it is worthwhile to test a "good" NOTATION
    set ret [catch {
	new XmlValue xv $XmlValue_NOTATION "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.17 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.18: QName"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_QNAME "xd:QName"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.18 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_QNAME "\0bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.18 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.19: string"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_STRING "not a bad string"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.19 [string first $exceptionSubstring $ex] -1
    delete xv

    # Bad string is "\0"
    set ret [catch {
	new XmlValue xv $XmlValue_STRING "\0"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.19 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.20: time"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_TIME "17:39:00"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.20 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_TIME "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.20 [string first $exceptionSubstring $ex] 1
    set ex ""
    delete context

    puts "\t\txml013.7.21: yearMonthDuration"
    wrap XmlQueryContext context [$db createQueryContext]

    new XmlValue xv $XmlValue_YEAR_MONTH_DURATION "P1Y1M"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.21 [string first $exceptionSubstring $ex] -1
    delete xv

    set ret [catch {
	new XmlValue xv $XmlValue_YEAR_MONTH_DURATION "bad"
        delete xv
    } ex ]
    dbxml_error_check_bad $basename.21 [string first $exceptionSubstring $ex] 1
    set ex ""

    new XmlValue xv $XmlValue_BINARY "binary_data"
    set ret [catch { [$context setVariableValue "foo" $xv] } ex ]
    dbxml_error_check_good $basename.22 [string match "Error*binary*" $ex] 1

    # try to use binary data as the context for a query

    wrap XmlQueryExpression qe [$db prepare "//foo" $context]
    set ret [catch {
	wrap XmlResults xr [$qe execute $xv $context 0]
        delete xv
    } ex ]
    dbxml_error_check_good $basename.22 [string match "Error*context*binary*" $ex] 1
    delete xv


    delete context

    delete db
}

# These tests were motivated by SR 10487/10365
# The issues was documents not being returned for queries on the presence of
# meta data when the meta data was indexed.
#
# Two documents are used. Metadata is added to one of the documents.
# The following matrix of tests are executed:
#
# 1) using XmlQueryContext::setWithMetaData(true)
#   A) add documents to an unindexed container
#     1) use the default namespace for the metadata
#       a) find the metadata using the API (getMetaData)
#       b) find the metadata using a query
#     2) use a user-provided namespace for the metadata
#       (repeat (a) and (b))
#
#   B) add documents to an indexed container
#       (repeat (1) and (2))
#
# 2) using XmlQueryContext::setWithMetaData(false)
#   (repeat (A) and (B))
#
proc xml013_8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\txml013.8: Indexing of Metadata"

    set documents {
	document_set_13_8/Yam.xml
	document_set_13_8/ZuluNut.xml
    }

    set indexes { 0 1 }
    set default_namespaces { 1 0 }
    set lazy_docs { 0 1 }

    set c 0
    foreach i $indexes {
	foreach j $default_namespaces {
	    foreach k $lazy_docs {
		incr c
		xml013_8_run $i $j $k $documents $env $txnenv $basename.$c $oargs
	    }
	}
    }
}

proc xml013_8_run { indexed default_ns lazy_docs documents env txnenv basename oargs } {
    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    # explain what is happening
    regsub $testdir/ $basename {} testid
    set msg "\t\t$testid: "
    if {$indexed} {
	append msg "Indexed"
    } else {
	append msg "Unindexed"
    }
    append msg " / "
    if {$default_ns} {
	append msg "Default"
    } else {
	append msg "User"
    }
    append msg " Namespace"
    append msg " / "
    if {$lazy_docs} {
	append msg "Lazy"
    } else {
	append msg "Eager"
    }
    append msg " Documents"

    puts $msg

    # preparation - create DB XML env, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn "$basename.dbxml" $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer "$basename.dbxml" $oargs]
    }

    # index the container
    if { $indexed } {
	if {$default_ns} {
	    set is_uri {}
	} else {
	    set is_uri {http://dbxmltests/metadata}
	}
	set is_node {foo}
	set is_index {metadata-presence}

	if {$txnenv == 1} {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn $is_uri $is_node $is_index $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex $is_uri $is_node $is_index $uc
	}
    }

    # add metadata to all but one of the documents, add documents to container
    if {$default_ns} {
	set mdURI {}
	set mdPrefix {}
    } else {
	set mdURI {http://dbxmltests/metadata}
	set mdPrefix {test}
    }
    set mdName {foo}
    set mdValue {veg}

    list set document_names {}
    list set documents_with_metadata {}
    set first 1

    foreach document $documents {
	wrap XmlDocument xd [$db createDocument]
	$xd setContent [readFile "$test_path/$document"]

	set name [file tail $document]
	$xd setName $name

	lappend document_names $name

	# add metadata
	if { ! $first } {
	    new XmlValue xv $XmlValue_STRING $mdValue
	    $xd setMetaData $mdURI $mdName $xv
	    lappend documents_with_metadata $name
	}

	# put document into container
	if {$txnenv == 1} {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container putDocument $txn $xd $uc
	    $txn commit
	    delete txn
	} else {
	    $container putDocument $xd $uc
	}

	delete xd
	if { ! $first } {
	    delete xv
	} else {
	    set first 0
	}
    }

    if { $lazy_docs } {
	set flags $DBXML_LAZY_DOCS
    } else {
	set flags 0
    }

    # TEST (i): find metadata using getMetaData()
    puts "\t\t\tFind metadata using API"
    foreach name $document_names {
	set hasMetadata 1
	if { [lsearch $documents_with_metadata $name] == -1 } {
	    set hasMetadata 0
	}

	if {$txnenv == 1} {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlDocument xd [$container getDocument $txn $name $flags]
	} else {
	    wrap XmlDocument xd [$container getDocument $name $flags]
	}

	new XmlValue xv
	$xd getMetaData $mdURI $mdName $xv
	delete xd

	if {$txnenv == 1} {
	    $txn commit
	    delete txn
	}

	if { $hasMetadata} {
	    dbxml_error_check_good $testid [$xv isString] 1
	    dbxml_error_check_good $testid [$xv asString] $mdValue
	} else {
	    dbxml_error_check_good $testid [$xv getType] 0
	}

	delete xv
    }

    # TEST (ii): find metadata using a query
    # All but one document should be returned if the withMetadata flag is set
    wrap XmlQueryContext context [$db createQueryContext]
    if { ! $default_ns} {
	$context setNamespace $mdPrefix $mdURI
    }

    if { $default_ns } {
	set query "/*\[dbxml:metadata('$mdName') = '$mdValue'\]"
    } else {
	set query "/*\[dbxml:metadata('$mdPrefix:$mdName') = '$mdValue'\]"
    }
    set query ${query}
    puts -nonewline "\t\t\tFind metadata using $query"
    puts -nonewline "\n"

    set fullQuery "collection('[$container getName]')$query"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlResults xr [$db query $txn $fullQuery $context $flags]
	set xr_size [$xr size]
	delete xr
	$txn commit
	delete txn
    } else {
	wrap XmlResults xr [$db query $fullQuery $context $flags]
	set xr_size [$xr size]
	delete xr
    }

    set expected [llength $documents_with_metadata]

    # remove conditional switch when issue is resolved
    dbxml_error_check_good "$testid: wrong number of documents returned" $xr_size $expected

    # cleanup
    catch {unset contents}
    delete uc
    delete context
    delete db
}

# testing for XmlMetaDataIterator
proc xml013_9 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t$basename: Metadata Iterators"

    # preparation - database, container
    source ./include.tcl
    set id 0

    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # create first document - no metadata
    set doc_name_1 "Vegetables"
    wrap XmlDocument xd_1 [$db createDocument]
    $xd_1 setContent "<hello/>"
    $xd_1 setName $doc_name_1

    # create second document - several metadatum
    set doc_name_2 "Meat"
    wrap XmlDocument xd_2 [$db createDocument]
    $xd_2 setContent "<goodbye/>"
    $xd_2 setName $doc_name_2

    set uri "http://www.example.com/test/"

    set metadata_count 5
    for {set i 0} {$i < $metadata_count} {incr i} {
	new XmlValue xv $XmlValue_STRING "Number $i"
	$xd_2 setMetaData $uri "foo_$i" $xv
	delete xv
    }

    # add documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd_1 $uc
	$container putDocument $txn $xd_2 $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd_1 $uc
	$container putDocument $xd_2 $uc
    }
    delete xd_1
    delete xd_2

    # get documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd_1 [$container getDocument $txn $doc_name_1]
	wrap XmlDocument xd_2 [$container getDocument $txn $doc_name_2]
    } else {
	wrap XmlDocument xd_1 [$container getDocument $doc_name_1]
	wrap XmlDocument xd_2 [$container getDocument $doc_name_2]
    }
    # sanity check on contents
    dbxml_error_check_good "$basename - doc content wrong" [$xd_1 getContentAsString] "<hello/>"
    dbxml_error_check_good "$basename - doc content wrong" [$xd_2 getContentAsString] "<goodbye/>"

    # iterate over metadata - first document (no metadata)
    #   ignore the "special" metadata (document name and content)
    set testid $basename.[incr id]
    puts "\t\t$testid: No user-defined metadata"

    wrap XmlMetaDataIterator xmdi [$xd_1 getMetaDataIterator]

    set err 0
    set count 0
    set x [$xmdi next]
    while { $x != "NULL" } {
	if {! [is_dbxml_metadata $x] } {
	    incr count
	}
	set x [$xmdi next]
    }
    dbxml_error_check_good "$basename - wrong number of metadata" $count 0

    delete xmdi
    delete xd_1

    # iterate over metadata - second document (many metadata)
    #   ignore the "special" metadata (document name)
    #   check values
    set testid $basename.[incr id]
    puts "\t\t$testid: User defined metadata"
    wrap XmlMetaDataIterator xmdi [$xd_2 getMetaDataIterator]

    set err 0
    set count 0
    set x [$xmdi next]
    while { $x != "NULL" } {
	if {! [is_dbxml_metadata $x] } {
	    # check name and value
	    dbxml_error_check_good "$testid - name" [string compare "foo_$count" [$x get_name]] 0
	    dbxml_error_check_good "$testid - name" [string compare "Number $count" [[$x get_value] asString]] 0
	    incr count
	}
	set x [$xmdi next]
    }
    dbxml_error_check_good "$basename - wrong number of metadata" $count $metadata_count

    delete xmdi

    # test metadata removal
    set testid $basename.[incr id]
    puts "\t\t$testid: Remove metadata, with index"
    set toRemove "foo_2"
    set is_index {metadata-presence}
    wrap XmlQueryContext qc [$db createQueryContext]

    # add index, first
    if {$txnenv == 1} {
	$container addIndex $txn $uri $toRemove $is_index $uc
	wrap XmlResults xr [$container lookupIndex $txn $qc $uri $toRemove $is_index]
    } else {
	$container addIndex $uri $toRemove $is_index $uc
	wrap XmlResults xr [$container lookupIndex $qc $uri $toRemove $is_index]
    }
    # make sure index is present
    dbxml_error_check_good "$basename -- remove metadata index 1" 1 [$xr size]
    delete xr

    $xd_2 removeMetaData $uri $toRemove
    if { $txnenv == 1 } {
	$container updateDocument $txn $xd_2 $uc
	wrap XmlDocument xd_u [$container getDocument $txn $doc_name_2]
	wrap XmlResults xr [$container lookupIndex $txn $qc $uri $toRemove $is_index]
    } else {
	$container updateDocument $xd_2 $uc
	wrap XmlDocument xd_u [$container getDocument $doc_name_2]
	wrap XmlResults xr [$container lookupIndex $qc $uri $toRemove $is_index]
    }
    # make sure index entry is gone
    dbxml_error_check_good "$basename -- remove metadata index 0" [$xr size] 0
    delete xr
    delete xd_2

    wrap XmlMetaDataIterator xmdi [$xd_u getMetaDataIterator]
    set x [$xmdi next]
    while { $x != "NULL" } {
	dbxml_error_check_bad "$testid " [string compare "foo_2" [$x get_name]] 0
	set x [$xmdi next]
    }

    delete xmdi
    delete xd_u

    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # clean up
    delete uc
    delete container
    delete db
}

# testing for metaData removal
proc xml013_10 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t$basename: Metadata manipulation"

    # preparation - database, container
    source ./include.tcl
    set id 0

    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # create first document - no metadata
    set doc_name_1 "Vegetables"
    wrap XmlDocument xd_1 [$db createDocument]
    $xd_1 setContent "<hello/>"
    $xd_1 setName $doc_name_1

    # create second document - several metadatum
    set doc_name_2 "Meat"
    wrap XmlDocument xd_2 [$db createDocument]
    $xd_2 setContent "<goodbye/>"
    $xd_2 setName $doc_name_2

    set uri "http://www.example.com/test/"

    set metadata_count 5
    for {set i 0} {$i < $metadata_count} {incr i} {
	new XmlValue xv $XmlValue_STRING "Number $i"
	$xd_2 setMetaData $uri "foo_$i" $xv
	delete xv
    }

    # add documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd_1 $uc
	$container putDocument $txn $xd_2 $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd_1 $uc
	$container putDocument $xd_2 $uc
    }
    delete xd_1
    delete xd_2

    # get document with m-d, LAZY
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd_2 [$container getDocument $txn $doc_name_2 $DBXML_LAZY_DOCS]
    } else {
	wrap XmlDocument xd_2 [$container getDocument $doc_name_2 $DBXML_LAZY_DOCS]
    }

    set testid $basename.[incr id]
    puts "\t\t$testid: lazy metadata get"
    wrap XmlMetaDataIterator xmdi [$xd_2 getMetaDataIterator]
    set err 0
    set count 0
    set x [$xmdi next]
    while { $x != "NULL" } {
	if {! [is_dbxml_metadata $x] } {
	    # check name and value
	    dbxml_error_check_good "$testid - name" [string compare "foo_$count" [$x get_name]] 0
	    dbxml_error_check_good "$testid - name" [string compare "Number $count" [[$x get_value] asString]] 0
	    incr count
	}
	set x [$xmdi next]
    }
    dbxml_error_check_good "$basename - wrong number of metadata" $count $metadata_count

    delete xmdi
    delete xd_2
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # get document again, to test LAZY removal
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd_2 [$container getDocument $txn $doc_name_2 $DBXML_LAZY_DOCS]
    } else {
	wrap XmlDocument xd_2 [$container getDocument $doc_name_2 $DBXML_LAZY_DOCS]
    }

    # remove several m-d items (not all)
    set metadata_count 3
    for {set i 0} {$i < $metadata_count} {incr i} {
	$xd_2 removeMetaData $uri foo_$i
    }

    if { $txnenv == 1 } {
	$container updateDocument $txn $xd_2 $uc
    } else {
	$container updateDocument $xd_2 $uc
    }

    delete xd_2

    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # get document again, not lazy, to verify m-d
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd_2 [$container getDocument $txn $doc_name_2]
    } else {
	wrap XmlDocument xd_2 [$container getDocument $doc_name_2]
    }

    set testid $basename.[incr id]
    puts "\t\t$testid: lazy metadata removal"
    wrap XmlMetaDataIterator xmdi [$xd_2 getMetaDataIterator]
    set err 0
    set count 3
    set x [$xmdi next]
    while { $x != "NULL" } {
	if {! [is_dbxml_metadata $x] } {
	    # check name and value
	    dbxml_error_check_good "$testid - name" [string compare "foo_$count" [$x get_name]] 0
	    dbxml_error_check_good "$testid - name" [string compare "Number $count" [[$x get_value] asString]] 0
	    incr count
	}
	set x [$xmdi next]
    }
    # count started at 3, ends at 5 -- 2 items left
    dbxml_error_check_good "$basename - wrong number of metadata" $count 5

    delete xmdi
    delete xd_2
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # clean up
    delete uc
    delete container
    delete db
}

# check if the metadata is a "special" dbxml item (document name and content)
proc is_dbxml_metadata { xmd } {
    source ./include.tcl
    if { [string compare [$xmd get_uri] $metaDataNamespace_uri] == 0 && \
	     [string compare [$xmd get_name] $metaDataName_name] == 0 } {
	return 1
    } else {
	return 0
    }
}

# Node methods on XmlValue nodes
proc xml013_11 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t$basename: Node methods on XmlValue nodes"

    # enumerations for node types
    global DOM_ELEMENT_NODE DOM_ATTRIBUTE_NODE DOM_TEXT_NODE \
        DOM_PROCESSING_INSTRUCTION_NODE DOM_COMMENT_NODE DOM_DOCUMENT_NODE
    set DOM_ELEMENT_NODE 1
    set DOM_ATTRIBUTE_NODE 2
    set DOM_TEXT_NODE 3
    set DOM_PROCESSING_INSTRUCTION_NODE 7
    set DOM_COMMENT_NODE 8
    set DOM_DOCUMENT_NODE 9

    # preparation - database, container
    source ./include.tcl

    xml_cleanup $testdir $env
    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # our document
    #  content is such that some nodes are whitespace (e.g. first child of
    #  parent)
    wrap XmlDocument xd [$db createDocument]
    set name "hello"
    $xd setName $name
    global xml013_document_content
    set xml013_document_content " \
<?xml-stylesheet type='text/xsl' href='href'?> \
<root xmlns:foo='http://www.example.com/foo'> \
	<grandparent> \
		<parent single=\"yes\"> \
			<child eldest=\"true\">Tom</child> \
			<child>Sally</child></parent><aunt> \
			<cousin>Jack</cousin> \
		</aunt> \
	</grandparent> \
	<grandparent> \
    <!-- no children for this grandparent --> \
	</grandparent> \
  <foo:godparent firm='mafia'>Tony</foo:godparent> \
</root> \
"

    $xd setContent $xml013_document_content

    wrap XmlUpdateContext uc [$db createUpdateContext]
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }

    delete xd

    set id 0
    xml013_11_information $db $container $txnenv $basename.[incr id]
    xml013_11_navigation $db $container $txnenv $basename.[incr id]

    # clean up
    delete uc
    delete container
    delete db
}

# test DOM node information methods on different node types
#   refer to the document defined in xml013
proc xml013_11_information { db container txnenv basename } {
    puts "\t\t$basename: Information methods"

    source ./include.tcl

    global DOM_ELEMENT_NODE DOM_ATTRIBUTE_NODE DOM_TEXT_NODE \
        DOM_PROCESSING_INSTRUCTION_NODE DOM_COMMENT_NODE DOM_DOCUMENT_NODE

    # execute a series of queries that will return different kinds of node
    #   a subroutine that returned a node given a query would be nice but
    #   I cannot work out how to do this in Tcl
    set id 0
    wrap XmlQueryContext context [$db createQueryContext]

    ########
    # document
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Document"
    set query {/root}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    # use parent of root, which ought to be the document node
    wrap XmlValue parent [$node getParentNode]
    dbxml_error_check_good "$testid - node type" [$parent getNodeType] $DOM_DOCUMENT_NODE
    dbxml_error_check_good "$testid - node name" [$parent getNodeName] "\#document"
    dbxml_error_check_good "$testid - node value" [$parent getNodeValue] {}
    dbxml_error_check_good "$testid - local name" [$parent getLocalName] {}
    dbxml_error_check_good "$testid - namespace URI" [$parent getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$parent getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$parent getTypeURI] ""
    dbxml_error_check_good "$testid - type name" [$parent getTypeName] ""

    delete parent
    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # element
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Element"
    set query {//aunt}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_ELEMENT_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "aunt"
    dbxml_error_check_good "$testid - node value" [$node getNodeValue] {}
    dbxml_error_check_good "$testid - local name" [$node getLocalName] "aunt"
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good "$testid - type name" [$node getTypeName] "untyped"

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # attribute
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Attribute"
    set query {//parent//@single}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_ATTRIBUTE_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "single"
    dbxml_error_check_good "$testid - node value" [$node getNodeValue] "yes"
    dbxml_error_check_good "$testid - local name" [$node getLocalName] "single"
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good "$testid - type name" [$node getTypeName] "untypedAtomic"

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # text
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Text"
    set query {//cousin/text()}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_TEXT_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "\#text"
    dbxml_error_check_good "$testid - node value" [$node getNodeValue] "Jack"
    dbxml_error_check_good "$testid - local name" [$node getLocalName] {}
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good "$testid - type name" [$node getTypeName] "untypedAtomic"

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # comment
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Comment"
    set query {//grandparent/comment()}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_COMMENT_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "\#comment"
    dbxml_error_check_good "$testid - node value" [string match "*children*grandparent*" [$node getNodeValue]] 1
    dbxml_error_check_good "$testid - local name" [$node getLocalName] {}
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] ""
    dbxml_error_check_good "$testid - type name" [$node getTypeName] ""

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # processing instruction
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Processing Instruction"
    set query {//processing-instruction()}
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
    }
    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_PROCESSING_INSTRUCTION_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "xml-stylesheet"
    dbxml_error_check_good "$testid - node value" [string match "*type*href*" [$node getNodeValue]] 1
    dbxml_error_check_good "$testid - local name" [$node getLocalName] {}
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] {}
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] {}
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] ""
    dbxml_error_check_good "$testid - type name" [$node getTypeName] ""

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # element with namespace and prefix
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Namespace URI and Prefix"
    set query {//node()[@firm]}
    set fullQuery "collection('[$container getName]')$query"
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]
            wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
        } else {
            wrap XmlResults xr [$db query $fullQuery $context 0]
        }
    } ret

    new XmlValue node
    $xr next $node
    dbxml_error_check_good $testid [$node isType $XmlValue_NODE] 1

    dbxml_error_check_good "$testid - node type" [$node getNodeType] $DOM_ELEMENT_NODE
    dbxml_error_check_good "$testid - node name" [$node getNodeName] "foo:godparent"
    dbxml_error_check_good "$testid - node value" [$node getNodeValue] {}
    dbxml_error_check_good "$testid - local name" [$node getLocalName] "godparent"
    dbxml_error_check_good "$testid - namespace URI" [$node getNamespaceURI] "http://www.example.com/foo"
    dbxml_error_check_good "$testid - prefix" [$node getPrefix] "foo"
    dbxml_error_check_good "$testid - type URI" [$node getTypeURI] "http://www.w3.org/2001/XMLSchema"
    dbxml_error_check_good "$testid - type name" [$node getTypeName] "untyped"

    delete xr
    delete node

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }

    #########
    # failure paths (use string, empty and binary values)
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Bad type conversions (failure paths)"

    new XmlValue xv1 $XmlValue_STRING "not a node"
    new XmlValue xv2
    new XmlValue xv3 $XmlValue_BINARY "binarydata"

    catch {
        set x [$xv1 asBinary]
    } ret
    dbxml_error_check_good $testid [string match "*convert*Binary" $ret] 1

    catch {
        set x [$xv2 asBinary]
    } ret
    dbxml_error_check_good $testid [string match "*convert*Binary" $ret] 1

    catch {
        set x [$xv3 asString]
    } ret
    dbxml_error_check_good $testid [string match "*convert*String" $ret] 1

    catch {
        set x [$xv1 getNodeName]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getNodeName]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getNodeValue]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getNodeValue]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getNamespaceURI]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getNamespaceURI]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getPrefix]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getPrefix]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getLocalName]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getLocalName]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getNodeType]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getNodeType]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    delete xv1
    delete xv2
    delete xv3

    # group clean up
    delete context
}

# test DOM node navigation methods on different node types
#   refer to the document defined in xml013
proc xml013_11_navigation { db container txnenv basename } {
    puts "\t\t$basename: Navigation methods"

    source ./include.tcl

    global DOM_ELEMENT_NODE DOM_ATTRIBUTE_NODE DOM_TEXT_NODE \
        DOM_PROCESSING_INSTRUCTION_NODE DOM_COMMENT_NODE
    global xml013_document_content

    # iterate over each of the possible return types
    list set return_types {}
    lappend return_types $XmlQueryContext_LiveValues

    set id_group 0
    foreach rt $return_types {
        set testid_group $basename.[incr id_group]

        # explain what is going on
        set msg "\t\t\t$testid_group: Using "
        if { $rt == $XmlQueryContext_LiveValues } {
            append msg "live values"
        } else {
            dbxml_error_check_good "$testid - unexpected return type" 1 0
        }
        puts $msg

        # prepare and execute query that hits the single parent
        wrap XmlQueryContext context [$db createQueryContext]
        $context setReturnType $rt
        set query {//parent[@single='yes']}
        set fullQuery "collection('[$container getName]')$query"
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]
            wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
        } else {
            wrap XmlResults xr [$db query $fullQuery $context 0]
        }
        new XmlValue node
        $xr next $node
        dbxml_error_check_good $testid_group [$node isType $XmlValue_NODE] 1
        # sanity checks - verify that we have the correct node
	dbxml_error_check_good $testid_group [$node getNodeType] $DOM_ELEMENT_NODE
        dbxml_error_check_good $testid_group [string match "*parent*single*" [$node asString]] 1
        set id 0

        ###############
        # parent
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: Parent"
        wrap XmlValue parent [$node getParentNode]
	# should be a "grandparent" element
	dbxml_error_check_good $testid [$parent isNull] 0
	dbxml_error_check_good $testid [$parent getNodeName] "grandparent"
        delete parent

        ###############
        # first child
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: First Child"
        wrap XmlValue first_child [$node getFirstChild]
	# should be a whitespace text node
	dbxml_error_check_good $testid [$first_child getNodeType] $DOM_TEXT_NODE
	dbxml_error_check_good $testid [regexp {\s+} [$first_child asString]] 1
        delete first_child

        ###############
        # last child
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: Last Child"
        wrap XmlValue last_child [$node getLastChild]
	# should be "Sally"
	dbxml_error_check_good $testid [$last_child getNodeType] $DOM_ELEMENT_NODE
	dbxml_error_check_good $testid [$last_child asString] "<child>Sally</child>"
        delete last_child

        ###############
        # next sibling
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: Next Sibling"
        wrap XmlValue next_sibling [$node getNextSibling]
	# should be "aunt"
	dbxml_error_check_good $testid [$next_sibling getNodeType] $DOM_ELEMENT_NODE
	dbxml_error_check_good $testid [$next_sibling getNodeName] "aunt"
        delete next_sibling

        ###############
        # previous sibling
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: Previous Sibling"
        wrap XmlValue previous_sibling [$node getPreviousSibling]
	# should be a whitespace text node
	dbxml_error_check_good $testid [$previous_sibling getNodeType] $DOM_TEXT_NODE
	dbxml_error_check_good $testid [regexp {\s+} [$previous_sibling asString]] 1
        delete previous_sibling

        ###############
        # attributes and owner element
        set testid $testid_group.[incr id]
        puts "\t\t\t\t$testid: Attributes and Owner Elements"
        # should be "single" -> "yes"
        wrap XmlResults attributes [$node getAttributes]
        new XmlValue att
        $attributes next $att
	dbxml_error_check_good $testid [$att getNodeType] $DOM_ATTRIBUTE_NODE
	dbxml_error_check_good $testid [$att getNodeName] "single"
	dbxml_error_check_good $testid [$att getNodeValue] "yes"

	# owner element should be "node"
	wrap XmlValue owner [$att getOwnerElement]
	dbxml_error_check_good $testid [$owner getNodeName] "parent"
	delete owner
        delete att
        delete attributes

        ###############
        # clean up
        delete node
        delete xr
        delete context

        if { $txnenv == 1 } {
            $xtxn commit
            delete xtxn
        }
    }
    catch {unset return_types}

    #############################################################
    # failure cases

    # navigation methods on non-nodes
    set id $id_group
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Non-nodes (failure paths)"

    new XmlValue xv1 $XmlValue_STRING "not a node"
    new XmlValue xv2

    catch {
        set x [$xv1 getParentNode]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getParentNode]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getFirstChild]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getFirstChild]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getLastChild]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getLastChild]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getNextSibling]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getNextSibling]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getPreviousSibling]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getPreviousSibling]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getAttributes]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getAttributes]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    catch {
        set x [$xv1 getOwnerElement]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1
    catch {
        set x [$xv2 getOwnerElement]
    } ret
    dbxml_error_check_good $testid [string match "Error*Node*" $ret] 1

    delete xv1
    delete xv2
}

# boundary cases
#  - empty values and names
#  - mismatched URIs
proc xml013_12 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t$basename: Boundary Cases"

    # prepare - database, context, container
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs]
    }

    set id 0

    #################################################################
    # Empty value
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get an empty value"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "EmptyValue"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_STRING ""
    $xd setMetaData "" "foo" $xv1
    new XmlValue xv2 $XmlValue_DECIMAL 0
    $xd setMetaData "" "bar" $xv2
    new XmlValue xv3 $XmlValue_BINARY "binary_data"
    $xd setMetaData "" "baz" $xv3
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2
    delete xv3

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_string [$xv asString]

	$xd getMetaData "" "baz" $xv
	set baz_type [$xv getType]
	set baz_bin [$xv asBinary]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "foo" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	$xd getMetaData "" "bar" $xv
	set bar_type [$xv getType]
	set bar_string [$xv asString]

	$xd getMetaData "" "baz" $xv
	set baz_type [$xv getType]
	set baz_bin [$xv asBinary]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_STRING] 1
    dbxml_error_check_good $testid $foo_string ""

    dbxml_error_check_good $testid [expr $bar_type == $XmlValue_DECIMAL] 1
    dbxml_error_check_good $testid $bar_string 0

    dbxml_error_check_good $testid [expr $baz_type == $XmlValue_BINARY] 1
    dbxml_error_check_good $testid [string compare $baz_bin "binary_data"] 1

    #################################################################
    # Empty name
    set testid $basename.[incr id]
    puts "\t\t$testid: Put/get an empty name"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "EmptyName"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_STRING "Goodbye"
    $xd setMetaData "" "" $xv1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "" "" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_STRING] 1
    dbxml_error_check_good $testid $foo_string "Goodbye"

    #################################################################
    # Names differing only in URI
    set testid $basename.[incr id]
    puts "\t\t$testid: Different URIs"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    set doc_name "DifferentURI"
    $xd setName $doc_name

    new XmlValue xv1 $XmlValue_STRING "hello"
    $xd setMetaData "http://www.sleepycat.com/2002/dbxml" "foo" $xv1
    new XmlValue xv2 $XmlValue_STRING "goodbye"
    $xd setMetaData "" "foo" $xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd
    delete xv1
    delete xv2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]

	new XmlValue xv {}
	$xd getMetaData "http://www.sleepycat.com/2002/dbxml" "foo" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	$xd getMetaData "" "foo" $xv
	set foo2_type [$xv getType]
	set foo2_string [$xv asString]

	delete xv
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $doc_name]

	new XmlValue xv {}
	$xd getMetaData "http://www.sleepycat.com/2002/dbxml" "foo" $xv
	set foo_type [$xv getType]
	set foo_string [$xv asString]

	$xd getMetaData "" "foo" $xv
	set foo2_type [$xv getType]
	set foo2_string [$xv asString]

	delete xv
	delete xd
    }

    dbxml_error_check_good $testid [expr $foo_type == $XmlValue_STRING] 1
    dbxml_error_check_good $testid $foo_string "hello"

    dbxml_error_check_good $testid [expr $foo2_type == $XmlValue_STRING] 1
    dbxml_error_check_good $testid $foo2_string "goodbye"

    # clean up
    delete container
    delete uc
    delete db
}

# miscellaneous API tests
proc xml013_13 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t$basename: API -- XmlResults, XmlValue"

    set id 0

    # previous() and next() on XmlResults
    xml013_13_1 $env $txnenv $basename.[incr id] $oargs

    # getNodeHandle() and getNode()
    xml013_13_2 $env $txnenv $basename.[incr id] $oargs
}

proc xml013_13_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\t$basename: previous() and next() on XmlResults"

    # prepare - manager
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env

    set items { "1" "2" "c" "d" "e" }

    wrap XmlResults res [$mgr createResults]

    foreach item $items {
	new XmlValue val $XmlValue_STRING $item
	$res add $val
	delete val
    }
    dbxml_error_check_good $basename.1 [$res size] 5
    new XmlValue xv
    while { [$res next $xv] } {
	#puts "[$xv asString]"
    }
    # when next is false, the cursor is at the "end" which
    # is actually past the last entry, so previous brings the
    # cursor back to the last entry.  This is what STL does.
    $res previous $xv
    dbxml_error_check_good $basename.2 [$xv asString] "e"
    $res previous $xv
    dbxml_error_check_good $basename.3 [$xv asString] "d"
    while { [$res previous $xv] } {
	#puts "[$xv asString]"
    }
    # previous lands on first entry, not before, call next  
    # should return the first item and move cursor to second 
    $res next $xv
    dbxml_error_check_good $basename.4 [$xv asString] "1"
    $res peek $xv
    dbxml_error_check_good $basename.5 [$xv asString] "2"
    delete xv
    delete res
}

proc xml013_13_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\t$basename: Node handles"

    puts "\t\t\t$basename.1: getNodeHandle() and getNode()"

    # prepare - manager
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs]
    }

    set name "document_name"
    set content "<element attribute=''><!-- comment --><element2 attribute2=''>text node</element2><?processing instruction?></element>"
    set query "collection('$basename.dbxml')/(descendant-or-self::node() union //attribute::node())"

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        $container putDocument $xtxn $name $content $uc
	$xtxn commit
	delete xtxn
    } else {
        $container putDocument $name $content $uc
    }

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $xtxn $query $qc 0]
    } else {
        wrap XmlResults res [$mgr query $query $qc 0]
    }

    set handles ""
    new XmlValue xv
    while { [$res next $xv] } {
	set handles "$handles [$xv getNodeHandle]"
    }

    $res reset
    foreach handle $handles {
        $res next $xv
        dbxml_error_check_good "$basename.1 same handle" [$xv getNodeHandle] $handle

        if {$txnenv == 1} {
            wrap XmlValue node [$container getNode $xtxn $handle]
        } else {
            wrap XmlValue node [$container getNode $handle]
        }
        dbxml_error_check_good "$basename.1 same node" [$xv equals $node] 1
        delete node
    }

    delete xv
    delete res
    if {$txnenv == 1} {
	$xtxn commit
	delete xtxn
    }

    foreach handle $handles {
        if {$txnenv == 1} {
            wrap XmlTransaction xtxn [$mgr createTransaction]
            wrap XmlValue node [$container getNode $xtxn $handle]
        } else {
            wrap XmlValue node [$container getNode $handle]
        }

        dbxml_error_check_good "$basename.1 not null" [$node isNull] 0
        dbxml_error_check_good "$basename.1 same handle 2" [$node getNodeHandle] $handle
        delete node

        if {$txnenv == 1} {
            $xtxn commit
            delete xtxn
        }
    }

    puts "\t\t\t$basename.2: dbxml:node-to-handle() and getNode()"

    set query2 "collection('$basename.dbxml')/(descendant-or-self::node() union //attribute::node())/dbxml:node-to-handle()"

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $xtxn $query2 $qc 0]
    } else {
        wrap XmlResults res [$mgr query $query2 $qc 0]
    }

    new XmlValue xv
    while { [$res next $xv] } {
	set handle [$xv asString]

        if {$txnenv == 1} {
            wrap XmlValue node [$container getNode $xtxn $handle]
        } else {
            wrap XmlValue node [$container getNode $handle]
        }
        dbxml_error_check_good "$basename.2 not null 2" [$node isNull] 0
        dbxml_error_check_good "$basename.2 same node handle 3" [$node getNodeHandle] $handle
        delete node
    }

    delete xv
    delete res
    if {$txnenv == 1} {
	$xtxn commit
	delete xtxn
    }

    puts "\t\t\t$basename.3: getNodeHandle() and dbxml:handle-to-node()"

    set query3_1 "dbxml:handle-to-node('$basename.dbxml', '"
    set query3_2 "')"

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $xtxn $query $qc 0]
    } else {
        wrap XmlResults res [$mgr query $query $qc 0]
    }

    new XmlValue xv
    while { [$res next $xv] } {

	set handle [$xv getNodeHandle]
        if {$txnenv == 1} {
            wrap XmlResults res2 [$mgr query $xtxn "$query3_1$handle$query3_2" $qc 0]
        } else {
            wrap XmlResults res2 [$mgr query "$query3_1$handle$query3_2" $qc 0]
        }

        new XmlValue node
        $res2 next $node

        dbxml_error_check_good "$basename.3 not null 3" [$node isNull] 0
        dbxml_error_check_good "$basename.3 same node handle 4" [$node getNodeHandle] $handle
        delete node
        delete res2
    }

    delete xv
    delete res
    if {$txnenv == 1} {
	$xtxn commit
	delete xtxn
    }

    puts "\t\t\t$basename.4: dbxml:node-to-handle() and dbxml:handle-to-node()"

    set query4 "every \$i in collection('$basename.dbxml')/(descendant-or-self::node() union //attribute::node())"
    set query4 "$query4 satisfies dbxml:handle-to-node('$basename.dbxml', dbxml:node-to-handle(\$i)) is \$i"

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $xtxn $query4 $qc 0]
    } else {
        wrap XmlResults res [$mgr query $query4 $qc 0]
    }

    new XmlValue xv
    $res next $xv

    dbxml_error_check_good "$basename.4 not null 4" [$xv isNull] 0
    dbxml_error_check_good "$basename.4 is true" [$xv asString] "true"

    delete xv
    delete res
    if {$txnenv == 1} {
	$xtxn commit
	delete xtxn
    }

    delete container
    delete qc
    delete uc
}

