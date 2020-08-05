# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml009
# TEST	Container Document Storage

proc xml009 { args } {
    puts "\nxml009: Container Document Storage and Content ($args)"
    source ./include.tcl

    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 9
    set oargs [eval {set_openargs} $args]

    if { $eindex == -1 } {
	set env NULL
    } else {
	incr eindex
	set env [lindex $args $eindex]
	set txnenv [is_txnenv $env]
	if { $txnenv == 1 } {
	}
	set testdir [get_home $env]
    }

    # assign the container type (default to whole document)
    set tindex [lsearch -exact $args "-storage"]
    if { $tindex == -1 } {
	set global_container_type $XmlContainer_NodeContainer
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

    # put and get documents (no namespace)
    xml009_1 $env $txnenv $basename.1 $oargs

    # put and get documents (with namespace)
    xml009_2 $env $txnenv $basename.2 $oargs

    # resolve external DTD on local file system (same directory)
    xml009_3 $env $txnenv $basename.3 $oargs

    # document naming (including unique names, no names)
    xml009_4 $env $txnenv $basename.4 $oargs

    # "put document as" methods
    xml009_5 $env $txnenv $basename.5 $oargs

    # misc document tests
    xml009_6 $env $txnenv $basename.6 $oargs

    # getContent* document tests
    xml009_7 $env $txnenv $basename.7 $oargs

    # put/updateDocument tests
    xml009_8 $env $txnenv $basename.8 $oargs

    # no-content document tests
    xml009_9 $env $txnenv $basename.9 $oargs

    return
}

proc xml009_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml009.1: Returned document identical to original document"
    source ./include.tcl
    set txn NULL

    xml_cleanup $testdir $env

    xml_database db $testdir $env
    set valargs [expr $oargs + $DBXML_ALLOW_VALIDATION]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn "$basename.1.dbxml" $oargs $global_container_type]
	wrap XmlContainer valContainer [$db createContainer $xtxn "$basename.1.val.dbxml" $valargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer "$basename.1.dbxml" $oargs $global_container_type ]
	wrap XmlContainer valContainer [$db createContainer "$basename.1.val.dbxml" $valargs $global_container_type ]
    }

    wrap XmlQueryContext context [$db createQueryContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }

    puts "\t\txml009.1.1 Document with prolog"
    xml009_assertDocumentPut "9.1.1: Document with prolog" $db $container \
	$test_path/document_set_9_1/9_1_1_with_prolog.xml $env $txn

    puts "\t\txml009.1.2 Document without prolog"
    xml009_assertDocumentPut "9.1.2: Document without prolog" $db $container \
	$test_path/document_set_9_1/9_1_2_without_prolog.xml $env $txn

    puts "\t\txml009.1.3 Well-formed document"
    xml009_assertDocumentPut "9.1.3: Well-formed document" $db $container \
	$test_path/document_set_9_1/9_1_3_well_formed.xml $env $txn

    puts "\t\txml009.1.4 Reject badly-formed document"
    xml009_assertDocumentNotPut "9.1.4: Reject badly-formed document" $db $container \
	$test_path/document_set_9_1/9_1_4_not_well_formed.xml $env $txn

    puts "\t\txml009.1.5 Document with CDATA"
    xml009_assertDocumentPut "9.1.5: Document with CDATA" $db $container \
	$test_path/document_set_9_1/9_1_5_with_cdata.xml $env $txn

    puts "\t\txml009.1.6 Document with comments"
    xml009_assertDocumentPut "9.1.6: Document with comments" $db $container \
	$test_path/document_set_9_1/9_1_6_with_comment.xml $env $txn

    puts "\t\txml009.1.7 Document with processing instruction"
    xml009_assertDocumentPut "9.1.7: Doc with processing instruction" $db $container \
	$test_path/document_set_9_1/9_1_7_with_pi.xml $env $txn

    puts "\t\txml009.1.8 Document with internal DTD"
    # use valContainer to get DTD in Node storage
    xml009_assertDocumentPut "9.1.8: Doc with internal DTD" $db $valContainer \
	$test_path/document_set_9_1/9_1_8_internal_dtd.xml $env $txn

    # 2004-07-21 arw moved to the resolver unit test
    #puts "\t\txml009.1.9 Document with external DTD"
    #xml009_assertDocumentPut "9.1.9: Doc with external DTD" $db $container \
	$test_path/document_set_9_1/9_1_9_external_dtd.xml $env $txn

    puts "\t\txml009.1.10 Standalone document definition"
    xml009_assertDocumentPut "9.1.10: Standalone document definition" $db $container \
	$test_path/document_set_9_1/9_1_10_standalone.xml $env $txn

    puts "\t\txml009.1.11 Elements"
    xml009_assertDocumentPut "9.1.11: Elements" $db $container \
	$test_path/document_set_9_1/9_1_11_elements.xml $env $txn

    puts "\t\txml009.1.12 Empty elements"
    xml009_assertDocumentPut "9.1.12: Empty elements" $db $container \
	$test_path/document_set_9_1/9_1_12_empty_elements.xml $env $txn

    puts "\t\txml009.1.13 Mixed content"
    xml009_assertDocumentPut "9.1.13: Mixed content" $db $container \
	$test_path/document_set_9_1/9_1_13_mixed_content.xml $env $txn

    puts "\t\txml009.1.14 Attributes"
    xml009_assertDocumentPut "9.1.14: Attributes" $db $container \
	$test_path/document_set_9_1/9_1_14_attributes.xml $env $txn

    puts "\t\txml009.1.15 Character references"
    if { $global_container_type == $XmlContainer_NodeContainer } {
	# NLS uses a different document, as it can't handle round tripping of
	# numeric entity references - jpcs
	xml009_assertDocumentPut "9.1.15: Character references" $db $container \
	    $test_path/document_set_9_1/9_1_15_character_references_nls.xml $env $txn
    } else {
	xml009_assertDocumentPut "9.1.15: Character references" $db $container \
	    $test_path/document_set_9_1/9_1_15_character_references.xml $env $txn
    }

    puts "\t\txml009.1.16 Internal entities"
    xml009_assertDocumentPut "9.1.16: Internal entities" $db $valContainer \
	$test_path/document_set_9_1/9_1_16_internal_entities.xml $env $txn

    puts "\t\txml009.1.17 Reject external entities "
    xml009_assertDocumentNotPut "9.1.17: Reject external entities" $db $container \
	$test_path/document_set_9_1/9_1_17_external_entities.xml $env $txn

    puts "\t\txml009.1.18 Reject syntax error"
    xml009_assertDocumentNotPut "9.1.18: Reject syntax error" $db $container \
	$test_path/document_set_9_1/9_1_18_syntax_error.xml $env $txn

    puts "\t\txml009.1.19 Document with external DTD missing"
    xml009_assertDocumentNotPut "9.1.19: Doc with external DTD missing" $db $valContainer \
	$test_path/document_set_9_1/9_1_19_external_dtd_missing.xml $env $txn

    puts "\t\txml009.1.20 Document with XML Schema"
    xml009_assertDocumentPut "9.1.20: Doc with XML Schema" $db $container \
	$test_path/document_set_9_1/9_1_20_xml_schema.xml $env $txn

    puts "\t\txml009.1.21 Document with XML Schema missing"
    xml009_assertDocumentNotPut "9.1.21: Doc with XML Schema missing" $db $valContainer \
	$test_path/document_set_9_1/9_1_21_xml_schema_missing.xml $env $txn

    puts "\t\txml009.1.22 Deep document"
    xml009_assertDocumentPut "9.1.22: Deep document" $db $container \
	$test_path/document_set_9_1/9_1_22_deep.xml $env $txn

    if {$txnenv == 1} {
	delete txn
    }

    # clean up
    delete container
    delete valContainer
    delete context
    delete db
}

proc xml009_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml009.2: Returned document identical to original - with namespaces"
    source ./include.tcl
    set txn NULL

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn "$basename.2.dbxml" $oargs $global_container_type ]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer "$basename.2.dbxml" $oargs $global_container_type ]
    }

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }

    puts "\t\txml009.2.1 Document with namespace declaration"
    xml009_assertDocumentPut "9.2.1: Document with namespace declaration" $db $container $test_path/document_set_9_2/9_2_1_namespace.xml $env $txn

    puts "\t\txml009.2.2 Default namespace declaration"
    xml009_assertDocumentPut "9.2.2: Default namespace declaration" $db $container $test_path/document_set_9_2/9_2_2_default_namespace.xml $env $txn

    if {$txnenv == 1} {
	delete txn
    }

    delete container
    delete db
}

proc xml009_assertDocumentPut { testid database container filename {env "NULL"} {txn "NULL"} } {

    catch {set ret [putFileInContainer $database $container $filename $txn]}
    dbxml_error_check_bad $testid $ret 0

    set data_in [readFile $filename]
    set data_out [getDocumentContent $container $txn [file tail $filename] ]
    dbxml_error_check_good $testid $data_out $data_in
}

proc xml009_assertDocumentNotPut { testid database container filename {env "NULL"} {txn "NULL"} } {
    catch {set ret [putFileInContainer $database $container $filename $txn]}
    dbxml_error_check_good $testid $ret 0
}

proc xml009_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml009.3: Resolver - handle DTDs on local file system"
    source ./include.tcl

    # get test instructions
    set index_file [file join $test_path document_set_9_3 index.xml]

    set id {}
    set program {}
    set args {}
    set ret [parse_index_file $index_file id program args]
    dbxml_error_check_good "WARNING \"$index_file\" was not recognized as a valid test driver" $ret 1

    # set arguments to external program
    set args {}

    # basic arguments - test materials are in the same directory as the index
    set d [file dirname $index_file]
    set p [file normalize $d]
    set args [concat $args "--datadir $p"]

    # basic arguments - DB XML environment, testid
    set args [concat $args "--env $testdir"]

    # start with a clean DB environment
    xml_cleanup $testdir $env

    run_external_test_program $txnenv $id $program $args

    puts ""
}

# document naming - API, unique naming feature
proc xml009_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml009.4: Document naming"

    xml009_4_1 $env $txnenv $basename.1 $oargs
    xml009_4_2 $env $txnenv $basename.2 $oargs
    xml009_4_3 $env $txnenv $basename.3 $oargs
    xml009_4_4 $env $txnenv $basename.4 $oargs
    xml009_4_5 $env $txnenv $basename.5 $oargs
    xml009_4_6 $env $txnenv $basename.6 $oargs
}

# document naming API (originally 13.1)
proc xml009_4_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml009.4.1: basic use of getName() and setName()"
    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, container
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

    # create and name a document, add to container
    set name "Vegetables"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<Root/>"
    $xd setName $name

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    delete xd

    # get document by name
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $name]
	set new_name [$xd getName]
	delete xd
	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$container getDocument $name]
	set new_name [$xd getName]
	delete xd
    }

    # verify name
    dbxml_error_check_good $basename $new_name $name

    #  delete xd

    delete container
    delete uc
    delete db
}

# adding two documents with the same name to a container (failure path)
proc xml009_4_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml009.4.2: non-unique names / single container (failure path)"
    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, container
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

    # create 2 documents with the same name
    set name "my document"

    wrap XmlDocument xd1 [$db createDocument]
    $xd1 setContent "<foo1/>"
    $xd1 setName $name

    wrap XmlDocument xd2 [$db createDocument]
    $xd2 setContent "<foo2/>"
    $xd2 setName $name

    # attempt to load both documents - first should load (name unchanged),
    # second should fail
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd1 $uc
        $txn commit
        delete txn

        dbxml_error_check_good $basename [$xd1 getName] $name

        wrap XmlTransaction txn [$db createTransaction]
        catch {$container putDocument $txn $xd2 $uc } ret
        $txn commit
        delete txn
    } else {
        $container putDocument $xd1 $uc
        dbxml_error_check_good $basename [$xd1 getName] $name
        catch {$container putDocument $xd2 $uc} ret
    }

    # exception should complain about uniqueness constraint on the name
    dbxml_error_check_good $basename [string match "Error*$name*" $ret] 1

    # clean up
    delete xd1
    delete xd2

    delete container
    delete uc
    delete db
}

# adding multiple documents with the same name to a container (success case)
proc xml009_4_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml009.4.3: non-unique names / single container (success case)"
    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, container
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

    # create 2 documents with the same name
    set name "my document"

    wrap XmlDocument xd1 [$db createDocument]
    set content_xd1 "<foo1/>"
    $xd1 setContent $content_xd1
    $xd1 setName $name

    wrap XmlDocument xd2 [$db createDocument]
    set content_xd2 "<foo2/>"
    $xd2 setContent $content_xd2
    $xd2 setName $name

    # add the documents, requesting that a unique name is generated (verify
    #  that each name has changed after loading)
    set new_name_xd1 {}
    set new_name_xd2 {}
    set flags $DBXML_GEN_NAME
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd1 $uc $flags
	$txn commit
	delete txn

	set new_name_xd1 [$xd1 getName]
	dbxml_error_check_good $basename [string match "${name}_*" $new_name_xd1] 1

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd2 $uc $flags
	$txn commit
	delete txn

	set new_name_xd2 [$xd2 getName]
	dbxml_error_check_good $basename [string match "${name}_*" $new_name_xd2] 1

    } else {
	$container putDocument $xd1 $uc $flags

	set new_name_xd1 [$xd1 getName]
	dbxml_error_check_good $basename [string match "${name}_*" $new_name_xd1] 1

	$container putDocument $xd2 $uc $flags

	set new_name_xd2 [$xd2 getName]
	dbxml_error_check_good $basename [string match "${name}_*" $new_name_xd2] 1
    }
    delete xd1
    delete xd2

    # get the documents by (modified) name
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd1 [$container getDocument $txn $new_name_xd1]
	set new_content_xd1 [$xd1 getContentAsString]
	delete xd1
	wrap XmlDocument xd2 [$container getDocument $txn $new_name_xd2]
	set new_content_xd2 [$xd2 getContentAsString]
	delete xd2
	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd1 [$container getDocument $new_name_xd1]
	set new_content_xd1 [$xd1 getContentAsString]
	delete xd1
	wrap XmlDocument xd2 [$container getDocument $new_name_xd2]
	set new_content_xd2 [$xd2 getContentAsString]
	delete xd2
    }

    # verify contents
    dbxml_error_check_good $basename $new_content_xd1 $content_xd1
    dbxml_error_check_good $basename $new_content_xd2 $content_xd2

    # clean up
    delete container
    delete uc
    delete db
}

# adding documents with the same name to separate containers
proc xml009_4_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\txml009.4.4: non-unique names / multiple containers"
	source ./include.tcl
	xml_cleanup $testdir $env

  # create database, containers
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container_1 [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    wrap XmlContainer container_2 [$db createContainer $xtxn $basename.2.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container_1 [$db createContainer $basename.1.dbxml $oargs $global_container_type]
    wrap XmlContainer container_2 [$db createContainer $basename.2.dbxml $oargs $global_container_type]
  }

  # create 2 documents with the same name
  set name "my document"

  wrap XmlDocument xd1 [$db createDocument]
  set content_xd1 "<foo1/>"
  $xd1 setContent $content_xd1
  $xd1 setName $name

  wrap XmlDocument xd2 [$db createDocument]
  set content_xd2 "<foo2/>"
  $xd2 setContent $content_xd2
  $xd2 setName $name

  # add the documents to the separate containers
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container_1 putDocument $txn $xd1 $uc
    $txn commit
    delete txn

    dbxml_error_check_good $basename [$xd1 getName] $name

    wrap XmlTransaction txn [$db createTransaction]
    $container_2 putDocument $txn $xd2 $uc
    $txn commit
    delete txn

    dbxml_error_check_good $basename [$xd2 getName] $name

  } else {
    $container_1 putDocument $xd1 $uc
    dbxml_error_check_good $basename [$xd1 getName] $name

    $container_2 putDocument $xd2 $uc
    dbxml_error_check_good $basename [$xd2 getName] $name
  }
  delete xd1
  delete xd2

  # get the documents by (same) name
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd1 [$container_1 getDocument $txn $name]
    set new_content_xd1 [$xd1 getContentAsString]
    delete xd1
    wrap XmlDocument xd2 [$container_2 getDocument $txn $name]
    set new_content_xd2 [$xd2 getContentAsString]
    delete xd2
    $txn commit
    delete txn
  } else {
    wrap XmlDocument xd1 [$container_1 getDocument $name]
    set new_content_xd1 [$xd1 getContentAsString]
    delete xd1
    wrap XmlDocument xd2 [$container_2 getDocument $name]
    set new_content_xd2 [$xd2 getContentAsString]
    delete xd2
  }

  # verify contents
  dbxml_error_check_good $basename $new_content_xd1 $content_xd1
  dbxml_error_check_good $basename $new_content_xd2 $content_xd2

  # clean up
  delete container_1
  delete container_2
  delete uc
  delete db
}

# adding multiple documents with no name
proc xml009_4_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml009.4.5: documents with no name"
    source ./include.tcl
    set id 0
    set txn NULL
    xml_cleanup $testdir $env

    # create database, container
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

    ############################################################
    # uniqueness not specified
    set testid $basename.[incr id]
    puts "\t\t\t$testid: verify that name, or generated name, is required"

    # create a document, no name, add to container
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<foo/>"

    set ret ""
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	catch {$container putDocument $txn $xd $uc} ret
	set name_1 [$xd getName]
	delete xd
	$txn commit
	delete txn
    } else {
	catch {$container putDocument $xd $uc} ret
	set name_1 [$xd getName]
	delete xd
    }

    # name should be empty, ret should not
    dbxml_error_check_bad "$testid - first unnamed document" $ret ""
    dbxml_error_check_good "$testid - first unnamed document" $name_1 ""

    ############################################################
    # uniqueness specified
    set testid $basename.[incr id]
    puts "\t\t\t$testid: requesting a unique name"

    set flags $DBXML_GEN_NAME

    # first unnamed document, with a unique name requested
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<tom/>"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc $flags
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc $flags
    }
    set name_1 [$xd getName]
    delete xd

    # second unnamed document, with a unique name requested
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<dick/>"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc $flags
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc $flags
    }
    set name_2 [$xd getName]
    delete xd

    # get documents and verify contents
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    set data_out_1 [getDocumentContent $container $txn $name_1]
    set data_out_2 [getDocumentContent $container $txn $name_2]

    if {$txnenv == 1} {
	delete txn
    }

    dbxml_error_check_good "$testid - 1st doc" $data_out_1 "<tom/>"
    dbxml_error_check_good "$testid - 2nd doc" $data_out_2 "<dick/>"

    # clean up
    delete container
    delete uc
    delete db
}

# test DBXML_LAZY_DOCS and renaming
proc xml009_4_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml009.4.6: DBXML_LAZY_DOCS and renaming"
    source ./include.tcl
    set id 0
    set txn NULL
    xml_cleanup $testdir $env

    # create database, container
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

    ############################################################
    # Rename without DBXML_LAZY_DOCS
    set testid $basename.[incr id]
    puts "\t\t\t$testid: verify that document renaming works"

    set name1 "docname"
    set name2 "docname2"
    set content "<foo/>"
    # create a document, add to container
    wrap XmlDocument xd [$db createDocument]
    $xd setContent $content
    $xd setName $name1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    # rename using a new document
    wrap XmlDocument xd [$container getDocument $name1]
    $xd setName $name2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container updateDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container updateDocument $xd $uc
    }
    delete xd

    # verify rename by checking content
    wrap XmlDocument xd [$container getDocument $name2]
    set new_content [$xd getContentAsString]

    delete xd
    dbxml_error_check_good "$testid - basic rename" $new_content $content

    # verify that original is gone
    set ret ""
    catch {wrap XmlDocument xd [$container getDocument $name1]} ret
    dbxml_error_check_bad "$testid - basic rename" $ret ""

    ############################################################
    # Rename using DBXML_LAZY_DOCS
    set testid $basename.[incr id]
    puts "\t\t\t$testid: verify document rename using DBXML_LAZY_DOCS"

    set name1 "lazyname"
    set name2 "lazyname2"
    set content "<lazyfoo/>"
    # create a document, add to container
    wrap XmlDocument xd [$db createDocument]
    $xd setContent $content
    $xd setName $name1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    # rename using a new document, but get it using DBXML_LAZY_DOCS
    wrap XmlDocument xd [$container getDocument $name1 $DBXML_LAZY_DOCS]
    $xd setName $name2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container updateDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container updateDocument $xd $uc
    }
    delete xd

    # verify rename by checking content
    wrap XmlDocument xd [$container getDocument $name2]
    set new_content [$xd getContentAsString]
    dbxml_error_check_good "$testid - lazy rename" $new_content $content
    delete xd

    # verify that original is gone
    set ret ""
    catch {wrap XmlDocument xd [$container getDocument $name1]} ret
    dbxml_error_check_bad "$testid - lazy rename" $ret ""

    ############################################################
    # Copy a document
    set testid $basename.[incr id]
    puts "\t\t\t$testid: verify document copy"

    set name1 "cname"
    set name2 "cname2"
    set content "<cfoo/>"
    # create a document, add to container
    wrap XmlDocument xd [$db createDocument]
    $xd setContent $content
    $xd setName $name1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    # get doc, change name, and put the renamed doc, which amounts to a copy
    wrap XmlDocument xd [$container getDocument $name1]
    $xd setName $name2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    # verify copy by checking content
    wrap XmlDocument xd [$container getDocument $name2]
    set new_content [$xd getContentAsString]
    dbxml_error_check_good "$testid - basic copy" $new_content $content

    delete xd

    # verify that original is still present
    wrap XmlDocument xd [$container getDocument $name1]
    delete xd

    ############################################################
    # Copy a document, using DBXML_LAZY_DOC
    set testid $basename.[incr id]
    puts "\t\t\t$testid: verify document copy using DBXML_LAZY_DOCS"

    set name1 "lcname"
    set name2 "lcname2"
    set content "<lcfoo/>"
    # create a document, add to container
    wrap XmlDocument xd [$db createDocument]
    $xd setContent $content
    $xd setName $name1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    # get doc, change name, and put the renamed doc, which amounts to a copy,
    # but use DBXML_LAZY_DOCS
    wrap XmlDocument xd [$container getDocument $name1 $DBXML_LAZY_DOCS]
    $xd setName $name2

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    wrap XmlDocument xd [$container getDocument $name2]
    set new_content [$xd getContentAsString]
    dbxml_error_check_good "$testid - lazy copy" $new_content $content

    delete xd

    # verify that original is still present
    wrap XmlDocument xd [$container getDocument $name1]
    delete xd

    ###########################
    # clean up
    delete container
    delete uc
    delete db
}

# putting documents in containers
# XmlContainer::putDocument(XmlDocument) is tested elsewhere, e.g. in the
# utility method putFileInContainer()
# 2004/09/28 ARW putDocument() using an input stream is tested in group 17
proc xml009_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\txml009.5: Putting documents in containers"
    xml009_5_1 $env $txnenv $basename.1 $oargs
}

# XmlContainer::putDocument() - strings, files
# Also test XmlManager::getHome()
proc xml009_5_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Content as Strings and Local Files, and getHome"
    source ./include.tcl

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    # create database, container
    xml_database db $testdir $env
    # test XmlManager::getHome
    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: XmlManager::getHome"
    set home [$db getHome]
    dbxml_error_check_good $testid $home $testdir

    wrap XmlUpdateContext uc [$db createUpdateContext]

    # allow validation to make DTD round-tripping work
    set oargs [expr $oargs + $DBXML_ALLOW_VALIDATION]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # put document using string (success case)
    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: Contents as string"

    set content "<foo att=\"hello\"><bar>foobar</bar></foo>"
    set name "foobar"
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	set name_out [$container putDocument $xtxn $name $content $uc]
	$xtxn commit
	delete xtxn
    } else {
	set name_out [$container putDocument $name $content $uc]
    }
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
    }

    set data_out [getDocumentContent $container $xtxn $name_out]
    set data_in $content
    dbxml_error_check_good $testid $data_out $data_in $xtxn
    if {$txnenv == 1} {
	$xtxn commit
	delete xtxn
    }

    # Test round-trip of some files
    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: Round-tripping tests"
    puts "\t\t\t\tdoc1"
    set testfile1 [file join $test_path document_set_9_5 doc1.xml]
    set ret [test_round_trip $db $uc $container $testfile1 $testid]

    puts "\t\t\t\tdoc2"
    set testfile2 [file join $test_path document_set_9_5 doc2.xml]
    set ret [test_round_trip $db $uc $container $testfile2 $testid.2]

    puts "\t\t\t\tdoc3"
    set testfile3 [file join $test_path document_set_9_5 doc3.xml]
    set ret [test_round_trip $db $uc $container $testfile3 $testid.3]

    puts "\t\t\t\tdoc4 -- default attrs internal subset"
    # test isSpecified by using internal DTD with default attrs
    set testfile4 [file join $test_path document_set_9_5 doc4.xml]
    set ret [test_round_trip $db $uc $container $testfile4 $testid.4]

    # document has a pile of namespace prefixes [#12458]
    puts "\t\t\t\tmanyNs -- many namespace prefixes"
    set testfile5   [file join $test_path document_set_9_5 manyNS.xml]
    set testfile5_2 [file join $test_path document_set_9_5 manyNS_alt.xml]
    set ret [test_round_trip $db $uc $container $testfile5 $testid.5 $testfile5_2]

    puts "\t\t\t\tdoc6 -- entity escapes"
    # test entity escaping rules
    set testfile6 [file join $test_path document_set_9_5 doc6.xml]
    set testfile6_out [file join $test_path document_set_9_5 doc6.out.xml]
    set ret [test_round_trip $db $uc $container $testfile6 $testid.6 $testfile6_out]

    puts "\nPENDING -- arrange to round-trip a doc that references schema"
    #puts "\t\t\t\tdoc5 -- many namespace prefixes"
    # test isSpecified by using schema with default attrs
#    set testfile5 [file join $test_path document_set_9_5 doc5.xml]
#    set ret [test_round_trip $db $uc $container $testfile6 $testid.6]


    # clean up
    delete container
    delete uc
    delete db
}

# simple round-trip tester.   Put a doc from a file,
#  get it, and compare to the original file
#
# NOTE: do NOT use files with
# non-ascii characters here -- Tcl does odd things with them
# in the file reading/comparision code.
#
# TBD: coerce Tcl to do the right thing, and use more complex
# documents
#
# Note: explicit transactions are not used, but if the
# container is transactional, the put operations will be
# auto-committed.
#
proc test_round_trip { mgr uc container filename testid {alt_filename 0} } {
    set xis [$mgr createLocalFileInputStream $filename]
    wrap XmlDocument doc [$mgr createDocument]
    $doc setContentAsXmlInputStream $xis
    $doc setName $testid
    $container putDocument $doc $uc

    delete doc

    wrap XmlDocument doc1 [$container getDocument $testid]
    set content [$doc1 getContentAsString]
    delete doc1

    # compare content to file
    set ret [compare_to_file $content $filename $testid $alt_filename]
}

proc compare_to_file { data filename testid alt_filename } {
    set fid [open $filename r]
    fconfigure $fid -translation binary
    set fdata [read $fid]
    close $fid

    if { [binary_compare $data $fdata] != 0 } {
        if { $alt_filename != 0 } {
            set fid [open $alt_filename r]
            fconfigure $fid -translation binary
            set fdata [read $fid]
            close $fid

            dbxml_error_check_good $testid $data $fdata
        } else {
            dbxml_error_check_good $testid $data $fdata
        }
    }
}

#
# Miscellaneous document tests
#
proc xml009_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\txml009.6: Miscellanous document tests"
    xml009_6_1 $env $txnenv $basename.1 $oargs
    xml009_6_2 $env $txnenv $basename.2 $oargs
    xml009_6_3 $env $txnenv $basename.3 $oargs
    xml009_6_4 $env $txnenv $basename.4 $oargs
    xml009_6_5 $env $txnenv $basename.5 $oargs
    xml009_6_6 $env $txnenv $basename.6 $oargs
    xml009_6_7 $env $txnenv $basename.7 $oargs
    xml009_6_8 $env $txnenv $basename.8 $oargs
    xml009_6_9 $env $txnenv $basename.9 $oargs
    xml009_6_10 $env $txnenv $basename.10 $oargs
}

proc xml009_6_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Document and node identity"
    source ./include.tcl

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    # create manager, container
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }

    # put document
    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: Document identity"

    set content "<foo att=\"hello\"><bar>foobar</bar></foo>"
    set name "foobar"
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	set name_out [$container putDocument $xtxn $name $content $uc]
	$xtxn commit
	delete xtxn
    } else {
	set name_out [$container putDocument $name $content $uc]
    }

    # ignore transactions -- they won't affect these results

    # check document equality/inequality
    wrap XmlDocument doc1 [$container getDocument $name_out]
    wrap XmlDocument doc2 [$container getDocument $name_out]
    wrap XmlDocument doc3 [$mgr createDocument]
    set eq [$doc1 equals $doc2]
    dbxml_error_check_good $testid $eq 1
    set eq [$doc1 equals $doc3]
    dbxml_error_check_good $testid $eq 0

    # check node equality/inequality
    set queryExp "collection('$basename.dbxml')/foo/bar"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlResults res1 [$mgr query $queryExp $qc]
    wrap XmlResults res2 [$mgr query $queryExp $qc]

    new XmlValue xv1
    new XmlValue xv2
    $res1 next $xv1
    $res2 next $xv2

    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: Node identity"

    set eq [$xv1 equals $xv2]
    dbxml_error_check_good $testid $eq 1

    # unequal nodes
    set queryExp "collection('$basename.dbxml')/foo"
    delete res1
    wrap XmlResults res1 [$mgr query $queryExp $qc]
    $res1 next $xv1
    set eq [$xv1 equals $xv2]
    dbxml_error_check_good $testid $eq 0

    delete xv1
    delete xv2
    delete res1
    delete res2
    delete qc
    delete doc1
    delete doc2
    delete doc3
    # clean up
    delete container
    delete uc
    delete mgr
}

#
# This test exercises code that checks to be sure that an
# operation is not incorrectly performed on a container
# from which a document did not belong.  While this can technically
# succeed or fail on its own, it's almost certainly an application
# bug, and one that can lead to perceived data corruption
#
proc xml009_6_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Document and container affinity"
    source ./include.tcl

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    set cname1 $basename.1.dbxml
    set cname2 $basename.2.dbxml

    # create manager, containers
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container1 [$mgr createContainer $xtxn $cname1 $oargs $global_container_type]
	wrap XmlContainer container2 [$mgr createContainer $xtxn $cname2 $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container1 [$mgr createContainer $cname1 $oargs $global_container_type]
	wrap XmlContainer container2 [$mgr createContainer $cname2 $oargs $global_container_type]
    }

    # put the same documents in both
    set content "<foo att=\"hello\"><bar>foobar</bar></foo>"
    set name1 "foobar1"
    set name2 "foobar2"

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	$container1 putDocument $xtxn $name1 $content $uc
	$container1 putDocument $xtxn $name2 $content $uc
	$container2 putDocument $xtxn $name1 $content $uc
	$container2 putDocument $xtxn $name2 $content $uc
	$xtxn commit
	delete xtxn
    } else {
	$container1 putDocument $name1 $content $uc
	$container1 putDocument $name2 $content $uc
	$container2 putDocument $name1 $content $uc
	$container2 putDocument $name2 $content $uc
    }

    # get document from container1, and remove it from container2
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlDocument doc_to_delete [$container1 getDocument $xtxn $name1]
	catch { $container2 deleteDocument $xtxn $doc_to_delete $uc 
	    puts "FAIL: should not get here"
	} ret
	dbxml_error_check_good $basename [is_substr $ret "different container"] 1
	catch { $container2 updateDocument $xtxn $doc_to_delete $uc 
	    puts "FAIL: should not get here"
	} ret
	dbxml_error_check_good $basename [is_substr $ret "different container"] 1
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlDocument doc_to_delete [$container1 getDocument $name1]
	catch { $container2 deleteDocument $doc_to_delete $uc
	    puts "FAIL: should not get here"
	} ret
	dbxml_error_check_good $basename [is_substr $ret "different container"] 1
	catch { $container2 updateDocument $doc_to_delete $uc
	    puts "FAIL: should not get here"
	} ret
	dbxml_error_check_good $basename [is_substr $ret "different container"] 1
    }

    delete doc_to_delete
    delete container2
    delete container1
    delete uc
    delete mgr
}

proc xml009_6_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Results as query context"
    source ./include.tcl

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    # create manager, container
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }

    set content "<foo att=\"hello\"><bar><baz/><boof>boo</boof></bar></foo>"
    set name "test"
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	$container putDocument $xtxn $name $content $uc $DBXML_GEN_NAME
	$container putDocument $xtxn $name $content $uc $DBXML_GEN_NAME
	$container putDocument $xtxn $name $content $uc $DBXML_GEN_NAME
	$xtxn commit
	delete xtxn
    } else {
	$container putDocument $name $content $uc $DBXML_GEN_NAME
	$container putDocument $name $content $uc $DBXML_GEN_NAME
	$container putDocument $name $content $uc $DBXML_GEN_NAME
    }

    # initial query
    set queryExp "collection('$basename.dbxml')/foo/bar"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $xtxn $queryExp $qc]
	new XmlValue xv
	set query1 "./boof"
	wrap XmlQueryExpression exp [$mgr prepare $xtxn $query1 $qc]
	while { [ $res next $xv ] } {
	    new XmlValue xv1
	    wrap XmlResults res1 [$exp execute $xtxn $xv $qc]
	    while { [ $res1 next $xv1 ] } {
		dbxml_error_check_good $basename [$xv1 asString] "<boof>boo</boof>"
	    }
	    delete xv1
	    delete res1
	}
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults res [$mgr query $queryExp $qc]
	new XmlValue xv
	set query1 "./boof"
	wrap XmlQueryExpression exp [$mgr prepare $query1 $qc]
	while { [ $res next $xv ] } {
	    new XmlValue xv1
	    wrap XmlResults res1 [$exp execute $xv $qc]
	    while { [ $res1 next $xv1 ] } {
		dbxml_error_check_good $basename [$xv1 asString] "<boof>boo</boof>"
	    }
	    delete xv1
	    delete res1
	}
    }

    delete exp
    delete xv
    delete res
    delete qc

    delete container
    delete uc
    delete mgr
}

proc xml009_6_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Document and node identity for temporary documents"
    source ./include.tcl

    xml_cleanup $testdir $env
    set id 0

    # use the same construction query twice and compare nodes.
    # they should not match (testing the fact that 2 constructed
    # documents do not match)
    set query "<a><b><c>c text</c><d a='b'/></b></a>/descendant-or-self::node()"

    # create manager, container
    xml_database mgr $testdir $env
    set testid $basename.$id
    puts "\t\t\t$testid: Node and doc identity across documents"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlResults res1 [$mgr query $query $qc]

    new XmlValue xv1

    while { [$res1 next $xv1] } {
	wrap XmlDocument doc1 [$xv1 asDocument]
	wrap XmlResults res2 [$mgr query $query $qc]
	new XmlValue xv2
	while { [$res2 next $xv2] } {
	    wrap XmlDocument doc2 [$xv2 asDocument]
	    set eqdoc [$doc1 equals $doc2]
	    set eq [$xv1 equals $xv2]
	    dbxml_error_check_good $testid $eq 0
	    dbxml_error_check_good $testid $eqdoc 0
	    delete doc2
	}
	delete res2
	delete xv2
	delete doc1
    }
    delete xv1
    delete res1

    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: Node identity within a document"
    set query "for \$foo in <a><b>12</b><b>13</b><b>14</b></a> return \$foo/b\[.=13\] is (\$foo/b)\[2\]"
    wrap XmlResults res [$mgr query $query $qc]
    new XmlValue xv
    $res next $xv
    dbxml_error_check_good $testid [$xv asString] "true"
    delete xv
    delete res
    # same as above, with 'false' result
    set query "for \$foo in <a><b>12</b><b>13</b><b>14</b></a> return \$foo/b\[.=13\] is (\$foo/b)\[1\]"
    wrap XmlResults res [$mgr query $query $qc]
    new XmlValue xv
    $res next $xv
    dbxml_error_check_good $testid [$xv asString] "false"
    delete xv
    delete res

    delete qc
    delete mgr
}

proc xml009_6_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Test default element namespace in queries"
    source ./include.tcl

    xml_cleanup $testdir $env
    set id 0

    # query with default element NS in query
    set query1 "declare default element namespace 'http://test';<pre:a xmlns:pre='http://test'><pre:b>b text</pre:b><pre:c><pre:d>1</pre:d><pre:d>2</pre:d></pre:c></pre:a>/c/d\[.=2\]"
    # query that assumes prefix mapping is set in context
    set query2 "<pre:a xmlns:pre='http://test'><pre:b>b text</pre:b><pre:c><pre:d>1</pre:d><pre:d>2</pre:d></pre:c></pre:a>/pre:c/pre:d\[.=2\]"

    # query that assumes default element NS is set in context
    set query3 "<pre:a xmlns:pre='http://test'><pre:b>b text</pre:b><pre:c><pre:d>1</pre:d><pre:d>2</pre:d></pre:c></pre:a>/c/d\[.=2\]"

    xml_database mgr $testdir $env
    set testid $basename.$id
    puts "\t\t\t$testid: verify that default element NS works in query"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlResults res1 [$mgr query $query1 $qc]

    dbxml_error_check_good $testid [$res1 size] 1

    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: verify that an explicit prefix mapping works"
    $qc setNamespace "pre" "http://test"
    wrap XmlResults res2 [$mgr query $query2 $qc]

    dbxml_error_check_good $testid [$res2 size] 1

    incr id
    set testid $basename.$id
    puts "\t\t\t$testid: verify that default element NS works in XmlQueryContext"
    wrap XmlQueryContext qc1 [$mgr createQueryContext]
    $qc1 setNamespace "" "http://test"
    wrap XmlResults res3 [$mgr query $query3 $qc1]

    dbxml_error_check_good $testid [$res3 size] 1

    delete res1
    delete res2
    delete res3
    delete qc
    delete qc1
    delete mgr
}

proc xml009_6_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Test queries of transient docs in a transaction"
    source ./include.tcl

    xml_cleanup $testdir $env
    set id 0
    # skip if not transactional
    if {$txnenv == 1} {
	# query root of document
	set query "/pr:root"
	set content	"<pr:root xmlns:pr='http://foo' name='fred'/>"
	xml_database mgr $testdir $env
	
	wrap XmlQueryContext qc [$mgr createQueryContext]
	$qc setNamespace "pr" "http://foo"
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	wrap XmlUpdateContext uc [$mgr createUpdateContext]
	wrap XmlDocument doc [$mgr createDocument]

	$doc setContent $content
	$doc setName "doc"
	wrap XmlTransaction txn [$mgr createTransaction]
	$container putDocument $txn $doc $uc

	wrap XmlQueryExpression qexpr [$mgr prepare $query $qc]
	# bug [#15905] (2.3.10) was: this query would hang/self-deadlock
	new XmlValue val $doc
	wrap XmlResults res [$qexpr execute $txn $val $qc]

	$txn commit

	delete val
	delete txn
	delete qexpr
	delete res
	delete doc
	delete uc
	delete qc
	delete container
	delete mgr
    } else {
	puts " -- skip for non-transactional environment"
    }
}

proc xml009_6_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Problem queries"
    source ./include.tcl

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    set cname $basename.dbxml
    set query "collection('$cname')/foo/baz\[.='abc'\]"
    set query1 "collection('$cname')/foo/bar\[.='abc'\]"
    set query2 "string(collection('$cname')/foo/baz)"
    set query3 "string(collection('$cname')/foo/bar)"
 

    # create manager, containers
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $cname $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $cname $oargs $global_container_type]
    }

    # put in some interesting content
    set content "<foo att=\"hello\"><baz><!\[CDATA\[\]\]></baz><bar><!\[CDATA\[\]\]>foobar</bar></foo>"
    set name "foo"

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
	wrap XmlResults res [$mgr query $xtxn $query $qc]
	delete res
	wrap XmlResults res [$mgr query $xtxn $query1 $qc]
	delete res
	wrap XmlResults res [$mgr query $xtxn $query2 $qc]
	delete res
	wrap XmlResults res [$mgr query $xtxn $query3 $qc]
	delete res
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults res [$mgr query $query $qc]
	delete res
	wrap XmlResults res [$mgr query $query1 $qc]
	delete res
	wrap XmlResults res [$mgr query $query2 $qc]
	delete res
	wrap XmlResults res [$mgr query $query3 $qc]
	delete res
    }

    delete container
    delete uc
    delete qc
    delete mgr
}

proc xml009_6_8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Document rename from query"
    source ./include.tcl

    # This was [#15993] -- disappearing documents

    xml_cleanup $testdir $env

    set xtxn NULL
    set id 0

    set cname $basename.dbxml
    set query "collection('$cname')/*\[dbxml:metadata('dbxml:name')='doc'\]"

    # create manager, containers
    xml_database mgr $testdir $env

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $cname $oargs $global_container_type]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $cname $oargs $global_container_type]
    }

    # content
    set content "<root/>"
    set name "doc"
    set name1 "doc1"

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
	wrap XmlResults res [$mgr query $xtxn $query $qc]
	new XmlValue xv
	$res next $xv
	wrap XmlDocument doc [$xv asDocument]
	$doc setName $name1
	$container updateDocument $xtxn $doc $uc
	delete doc
	delete xv
	delete res
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults res [$mgr query $query $qc]
	new XmlValue xv
	$res next $xv
	wrap XmlDocument doc [$xv asDocument]
	$doc setName $name1
	$container updateDocument $doc $uc
	delete doc
	delete xv
	delete res
    }
    # don't bother with txn to verify that the new doc exists
    wrap XmlDocument doc1 [$container getDocument $name1]
    dbxml_error_check_good $basename [$doc1 getName] $name1
    delete doc1
    delete container
    delete uc
    delete qc
    delete mgr
}
proc xml009_6_9 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Test queries of constructed XML"
    # SR [#16230]
    source ./include.tcl

    xml_cleanup $testdir $env
    set id 0
    set content  "<a><b>btext</b></a>"
    set queryDoc  "collection()/a/b"
    set queryConstruct  "<a><b>text</b></a>/b"
    set queryBad  "/a"
    
    xml_database mgr $testdir $env
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    $container putDocument "doc" $content $uc
    $qc setDefaultCollection "$basename.dbxml"

    wrap XmlQueryExpression exprDoc [$mgr prepare $queryDoc $qc]
    wrap XmlQueryExpression exprConstruct [$mgr prepare $queryConstruct $qc]
    wrap XmlQueryExpression exprBad [$mgr prepare $queryBad $qc]

    wrap XmlResults resDoc [$exprDoc execute $qc]
    wrap XmlResults resConstruct [$exprConstruct execute $qc]
    new XmlValue docValue
    new XmlValue constructValue
    # set contexts for next queries
    $resDoc next $docValue
    $resConstruct next $constructValue

    # this will work -- against real document
    wrap XmlResults res [$exprBad execute $docValue $qc]
    dbxml_error_check_good $basename [$res size] 1
    delete res

    # this will throw because there is no document node in
    # constructed XML
    catch { $exprBad execute $constructValue $qc } ret
    dbxml_error_check_good $basename [is_substr $ret "Sequence does not match type"] 1

    delete docValue
    delete constructValue
    delete resDoc
    delete resConstruct
    delete exprDoc
    delete exprConstruct
    delete exprBad
    delete container
    delete uc
    delete qc
    delete mgr
}

proc xml009_6_10 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t\t$basename: Test query of read-only content"
    # SR [#17212]
    source ./include.tcl

    xml_cleanup $testdir $env
    set id 0
    set content "<?xml version=\"1.0\"?><Test xmlns:test=\"urn:test\" test:test=\"test\"/>";
    set modifiedContent "<?xml version=\"1.0\"?><Test xmlns:test1=\"urn:test1\" test1:test=\"test\"/>";
    set query "collection()/\*";
    
    xml_database mgr $testdir $env
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    $container putDocument "doc" $content $uc
    $container putDocument "doc1" $content $uc
    wrap XmlDocument doc [$container getDocument "doc1"]
    $doc setContent $modifiedContent
    $container updateDocument $doc $uc
    $qc setDefaultCollection "$basename.dbxml"
    delete container

    # reopen read-only
    set oargs [expr $oargs + $DB_RDONLY]
    wrap XmlContainer container [$mgr openContainer $basename.dbxml $oargs]

    wrap XmlResults res [$mgr query $query $qc]
    dbxml_error_check_good $basename [$res size] 2
    delete res
    delete container
    delete qc
    delete uc
    delete mgr
}

#
# getContent* document tests
#
proc xml009_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\txml009.7: getContent* document tests"

    xml009_7_1 $env $txnenv $basename.1 $oargs
    xml009_7_2 $env $txnenv $basename.2 $oargs
    xml009_7_3 $env $txnenv $basename.3 $oargs
}

# Test get/setContentAsEventReader
proc xml009_7_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\t9.7.1: get/setContentAsEventReader"
  source ./include.tcl
  xml_cleanup $testdir $env

  set testid $basename
  # create manager, container
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container_1 [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container_1 [$db createContainer $basename.1.dbxml $oargs $global_container_type]
  }

  # create temporary documents an test the getContent* functions
  set name1 "doc1"
  set name2 "doc2"

  puts "\t\t\t$testid.1: Set document content and call getContentAsEventReader"
  wrap XmlDocument xd1 [$db createDocument]
  set content_xd1 "<foo1/>"
  $xd1 setContent $content_xd1
  $xd1 setName $name1

  puts "\t\t\t$testid.2: Set content as event reader then get it."
  wrap XmlDocument xd2 [$db createDocument]
  $xd2 setContentAsEventReader [$xd1 getContentAsEventReader]
  $xd2 setName $name2
  wrap XmlEventReader rdr2 [$xd2 getContentAsEventReader]
  dbxml_error_check_good $testid [$rdr2 hasNext] 1
  delete rdr2

  #These fail because the XmlEventReader has been consummed.
  catch { [$xd2 getContentAsEventReader] } ret
  dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
  catch { [$xd2 getContentAsString] } ret
  dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
  catch { [$xd2 getContentAsXmlInputStream] } ret
  dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
  catch { [$xd2 getContent] } ret
  dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1

  $xd2 setContent $content_xd1
  $xd1 setContent $content_xd1
  dbxml_error_check_good $testid [$xd2 getContentAsString] $content_xd1
  dbxml_error_check_good $testid [$xd1 getContentAsString] $content_xd1

  # add the documents to a container
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container_1 putDocument $txn $xd1 $uc
    $container_1 putDocument $txn $xd2 $uc
    $txn commit
    delete txn

  } else {
    $container_1 putDocument $xd1 $uc
    $container_1 putDocument $xd2 $uc
  }
  delete xd1
  delete xd2

  puts "\t\t\t$testid.3: Add documents to a container and test getContentAsEventReader"
  # get the documents from the container and check content
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd1 [$container_1 getDocument $txn $name1]
    set new_content_xd1 [$xd1 getContentAsString]
    wrap XmlEventReader rdr3 [$xd1 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr3 hasNext] 1
    delete rdr3
    wrap XmlEventReader rdr4 [$xd1 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr4 hasNext] 1
    delete rdr4
    delete xd1

    wrap XmlDocument xd2 [$container_1 getDocument $txn $name2]
    set new_content_xd2 [$xd2 getContentAsString]
    wrap XmlEventReader rdr5 [$xd2 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr5 hasNext] 1
    delete rdr5
    wrap XmlEventReader rdr6 [$xd2 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr6 hasNext] 1
    delete rdr6
    delete xd2
    $txn commit
    delete txn
  } else {
    wrap XmlDocument xd1 [$container_1 getDocument $name1]
    set new_content_xd1 [$xd1 getContentAsString]
    wrap XmlEventReader rdr7 [$xd1 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr7 hasNext] 1
    delete rdr7
    wrap XmlEventReader rdr8 [$xd1 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr8 hasNext] 1
    delete rdr8
    delete xd1

    wrap XmlDocument xd2 [$container_1 getDocument $name2]
    set new_content_xd2 [$xd2 getContentAsString]
    wrap XmlEventReader rdr9 [$xd2 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr9 hasNext] 1
    delete rdr9
    wrap XmlEventReader rdr10 [$xd2 getContentAsEventReader]
    dbxml_error_check_good $testid [$rdr10 hasNext] 1
    delete rdr10
    delete xd2
  }

  # verify contents
  dbxml_error_check_good $testid $new_content_xd1 $content_xd1
  dbxml_error_check_good $testid $new_content_xd2 $content_xd1
  # clean up
  delete container_1
  delete uc
  delete db
}

# Test get/setContentAsXmlInputStream
proc xml009_7_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\t9.7.2: get/setContentAsXmlInputStream"
  source ./include.tcl
  xml_cleanup $testdir $env
  
  set testid $basename
  # create database, container
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container_1 [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container_1 [$db createContainer $basename.1.dbxml $oargs $global_container_type]
  }

  # create temporary documents an test the getContent* functions
  set name1 "doc1"
  set name2 "doc2"

  puts "\t\t\t$testid.1: Set document content and call getContentAsInputStream"
  wrap XmlDocument xd1 [$db createDocument]
  set content_xd1 "<foo1/>"
  $xd1 setContent $content_xd1
  $xd1 setName $name1

  set rdr1 [$xd1 getContentAsXmlInputStream]
  dbxml_error_check_good $testid [$rdr1 curPos] 0
  $xd1 setContent $content_xd1
  dbxml_error_check_good $testid [$xd1 getContentAsString] $content_xd1
  $xd1 setContent $content_xd1

  puts "\t\t\t$testid.2: Set document content as InputStream then get the content"
  wrap XmlDocument xd2 [$db createDocument]
  $xd2 setContentAsXmlInputStream $rdr1
  $xd2 setName $name2

  #Once the stream is consumed all attempts to get content should
  #result in an exception
  set rdr2 [$xd2 getContentAsXmlInputStream]
  dbxml_error_check_good $testid [$rdr2 curPos] 0
  delete rdr2
  
  #These fail because the XmlEventReader has been consummed.
  catch { [$xd2 getContentAsEventReader] } ret
  dbxml_error_check_good $testid [string match "*XmlInputStream*" $ret] 1
  catch { [$xd2 getContentAsString] } ret
  dbxml_error_check_good $testid [string match "*XmlInputStream*" $ret] 1
  catch { [$xd2 getContentAsXmlInputStream] } ret
  dbxml_error_check_good $testid [string match "*XmlInputStream*" $ret] 1
  catch { [$xd2 getContent] } ret
  dbxml_error_check_good $testid [string match "*XmlInputStream*" $ret] 1
    
  $xd2 setContent $content_xd1
  dbxml_error_check_good $testid [$xd2 getContentAsString] $content_xd1
  $xd2 setContent $content_xd1

  # add the documents to a container
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container_1 putDocument $txn $xd1 $uc
    $container_1 putDocument $txn $xd2 $uc
    $txn commit
    delete txn
  } else {
    $container_1 putDocument $xd1 $uc
    $container_1 putDocument $xd2 $uc
  }
  delete xd1
  delete xd2

  puts "\t\t\t$testid.3: Test getContentAsXmlInputStream on documents in a container"
  # get the documents from the container and check content
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd1 [$container_1 getDocument $txn $name1]
    set new_content_xd1 [$xd1 getContentAsString]
    wrap XmlInputStream rdr3 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr3 curPos] 0
    delete rdr3 
    wrap XmlInputStream rdr4 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr4 curPos] 0
    delete rdr4 
    delete xd1

    wrap XmlDocument xd2 [$container_1 getDocument $txn $name2]
    set new_content_xd2 [$xd2 getContentAsString]
    wrap XmlInputStream rdr5 [$xd2 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr5 curPos] 0
    delete rdr5 
    wrap XmlInputStream rdr6 [$xd2 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr6 curPos] 0
    delete rdr6 
    delete xd2
    $txn commit
    delete txn
  } else {
    wrap XmlDocument xd1 [$container_1 getDocument $name1]
    set new_content_xd1 [$xd1 getContentAsString]
    wrap XmlInputStream rdr7 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr7 curPos] 0
    delete rdr7 
    wrap XmlInputStream rdr8 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr8 curPos] 0
    delete rdr8 
    delete xd1

    wrap XmlDocument xd2 [$container_1 getDocument $name2]
    set new_content_xd2 [$xd2 getContentAsString]
    wrap XmlInputStream rdr9 [$xd2 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr9 curPos] 0
    delete rdr9 
    wrap XmlInputStream rdr10 [$xd2 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr10 curPos] 0
    delete rdr10 
    delete xd2
  }

  # verify contents
  dbxml_error_check_good $testid $new_content_xd1 $content_xd1
  dbxml_error_check_good $testid $new_content_xd2 $content_xd1

  # clean up
  delete container_1
  delete uc
  delete db
}

# Test getContent* on empty documents
proc xml009_7_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\t9.7.3: Test getContent* on empty documents "
  source ./include.tcl
  xml_cleanup $testdir $env

  set testid $basename
  # create database, container
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container_1 [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container_1 [$db createContainer $basename.1.dbxml $oargs $global_container_type]
  }

  # create temporary empty document and test the getContent* functions
  set name1 "doc1"
  set empty ""
 
  #Create an empty document
  wrap XmlDocument xd1 [$db createDocument]
  $xd1 setName $name1

  puts "\t\t\t$testid.1: Test getContent* on empty temporary documents"
  wrap XmlInputStream rdr1 [$xd1 getContentAsXmlInputStream]
  dbxml_error_check_good $testid [$rdr1 curPos] 0
  delete rdr1
  catch { [$xd1 getContentAsEventReader] } ret
  dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
  dbxml_error_check_good $testid [$xd1 getContentAsString] $empty
   # add the document to a container
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container_1 putDocument $txn $xd1 $uc
    $txn commit
    delete txn
  } else {
    $container_1 putDocument $xd1 $uc
  }
  delete xd1

  puts "\t\t\t$testid.2: Test getContent* on empty documents in a container"
  # get the document from the container and check content
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd1 [$container_1 getDocument $txn $name1]
    wrap XmlInputStream rdr3 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr3 curPos] 0
    delete rdr3
    if [catch {wrap XmlEventReader rdr4 [$xd1 getContentAsEventReader] } ret] {
      dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
    } else {
      dbxml_error_check_good $testid [$rdr4 hasNext] 0
      delete rdr4
    }
    dbxml_error_check_good $testid [$xd1 getContentAsString] $empty
    delete xd1
  } else {
    wrap XmlDocument xd1 [$container_1 getDocument $name1]
    wrap XmlInputStream rdr3 [$xd1 getContentAsXmlInputStream]
    dbxml_error_check_good $testid [$rdr3 curPos] 0
    delete rdr3
    if [catch {wrap XmlEventReader rdr4 [$xd1 getContentAsEventReader] } ret] {
      dbxml_error_check_good $testid [string match "*XmlEventReader*" $ret] 1
    } else {
      dbxml_error_check_good $testid [$rdr4 hasNext] 0
      delete rdr4
    }
    dbxml_error_check_good $testid [$xd1 getContentAsString] $empty
    delete xd1
  }

  # clean up
  delete container_1
  delete uc
  delete db
}


#
# put/updateDocument tests
#
proc xml009_8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\txml009.8: put/updateDocument tests"

    xml009_8_1 $env $txnenv $basename.1 $oargs
    xml009_8_2 $env $txnenv $basename.2 $oargs
}

# Test putDocument
proc xml009_8_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\t9.8.1: putDocument test"
  source ./include.tcl
  xml_cleanup $testdir $env

  set testid $basename
  # create database, container
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container [$db createContainer $basename.1.dbxml $oargs $global_container_type]
  }

  # create temporary empty document and test the getContent* functions
  set name "doc1"
  set content "<a><b>c</b></a>"
 
  #Create an empty document
  wrap XmlDocument xd1 [$db createDocument]
  $xd1 setName $name
  $xd1 setContent $content

  # add the document to a container
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd1 $uc
    wrap XmlDocument xd2 [$container getDocument $txn [$xd1 getName] ]
    dbxml_error_check_good $testid [$xd2 getContent] $content
    dbxml_error_check_good $testid [$xd2 getName] $name
    delete xd2
    $txn commit
    delete txn
    catch { 
	set newContent [$xd1 getContent]
        puts "FAIL: should not have gotten here"
	dbxml_error_check_bad "$basename" 1 1
    } ret
    dbxml_error_check_good $testid [string match "*DB_TXN*" $ret] 1
  } else {
    $container putDocument $xd1 $uc
    dbxml_error_check_good $testid [$xd1 getContent] $content
    dbxml_error_check_good $testid [$xd1 getName] $name
  }
  delete xd1

  # clean up
  delete container
  delete uc
  delete db
}

# Test updateDocument
proc xml009_8_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
  puts "\t\t9.8.2: updateDocument test"
  source ./include.tcl
  xml_cleanup $testdir $env

  set testid $basename
  # create database, container
  xml_database db $testdir $env
  wrap XmlUpdateContext uc [$db createUpdateContext]

  if {$txnenv == 1} {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $xtxn $basename.1.dbxml $oargs $global_container_type]
    $xtxn commit
    delete xtxn
  } else {
    wrap XmlContainer container [$db createContainer $basename.1.dbxml $oargs $global_container_type]
  }

  # create temporary empty document and test the getContent* functions
  set name "doc1"
  set content "<a><b>c</b></a>"

  set name2 "doc2"
  set content2 "<d><e>f</e></d>"
 
  #Create an empty document
  wrap XmlDocument xd1 [$db createDocument]
  $xd1 setName $name
  $xd1 setContent $content

  # add the document to a container
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd1 $uc
    $txn commit
    delete txn
  } else {
    $container putDocument $xd1 $uc
  }
  delete xd1

  wrap XmlDocument xd1 [$container getDocument $name]
  $xd1 setName $name2
  $xd1 setContent $content2

  # update the document
  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container updateDocument $txn $xd1 $uc
    wrap XmlDocument xd2 [$container getDocument $txn [$xd1 getName] ]
    dbxml_error_check_good $testid [$xd2 getContent] $content2
    dbxml_error_check_good $testid [$xd2 getName] $name2
    delete xd2
    $txn commit
    delete txn
    catch { 
	set newContent [$xd1 getContent]
        puts "FAIL: should not have gotten here"
	dbxml_error_check_bad "$basename" 1 1
    } ret
    dbxml_error_check_good $testid [string match "*DB_TXN*" $ret] 1
  } else {
    $container updateDocument $xd1 $uc
    dbxml_error_check_good $testid [$xd1 getContent] $content2
    dbxml_error_check_good $testid [$xd1 getName] $name2
  }
  delete xd1
  

  # clean up
  delete container
  delete uc
  delete db
}


#
# no-content document tests
#
proc xml009_9 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\txml009.9: no-content document tests"

    xml009_9_1 $env $txnenv $basename.1 $oargs
}

# Test putDocument
proc xml009_9_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\t9.9.1: no-content query"
    source ./include.tcl
    xml_cleanup $testdir $env
    
    set testid $basename
    # create database, container
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    set noStatsArgs [expr $oargs + $DBXML_NO_STATISTICS]

    foreach arg "$oargs $noStatsArgs" {

	if {$txnenv == 1} {
	    wrap XmlTransaction xtxn [$mgr createTransaction]
	    wrap XmlContainer container [$mgr createContainer $xtxn $basename.1.dbxml $arg $global_container_type]
	    $xtxn commit
	    delete xtxn
	} else {
	    wrap XmlContainer container [$mgr createContainer $basename.1.dbxml $arg $global_container_type]
	}

	# create temporary empty document and test the getContent* functions
	set name "empty"
	set content ""
 
	#Create an empty document
	wrap XmlDocument xd [$mgr createDocument]
	$xd setName $name
	$xd setContent $content
	wrap XmlDocument xd1 [$mgr createDocument]
	$xd1 setName $name.1
	$xd1 setContent $content
	wrap XmlDocument xd2 [$mgr createDocument]
	$xd2 setName $name.2
	$xd2 setContent "<root/>"
	wrap XmlDocument xd3 [$mgr createDocument]
	$xd3 setName $name.3
	$xd3 setContent $content
	wrap XmlDocument xd4 [$mgr createDocument]
	$xd4 setName $name.4
	$xd4 setContent "<root4/>"
	
	# add the document to a container
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    $container putDocument $txn $xd $uc
	    $container putDocument $txn $xd1 $uc
	    $container putDocument $txn $xd2 $uc
	    $container putDocument $txn $xd3 $uc
	    $container putDocument $txn $xd4 $uc
	    wrap XmlDocument xd7 [$container getDocument $txn [$xd getName] ]
	    dbxml_error_check_good $testid [$xd7 getContent] $content
	    dbxml_error_check_good $testid [$xd7 getName] $name
	    delete xd7
	    $txn commit
	    delete txn
	} else {
	    $container putDocument $xd $uc
	    $container putDocument $xd1 $uc
	    $container putDocument $xd2 $uc
	    $container putDocument $xd3 $uc
	    $container putDocument $xd4 $uc
	    dbxml_error_check_good $testid [$xd getContent] $content
	    dbxml_error_check_good $testid [$xd getName] $name
	}
	delete xd
	delete xd1
	delete xd2
	delete xd3
	delete xd4

	# try a problem query from [#16338] (ignore transactions)
	# add alias for query
	$container addAlias "c"
	set query "fn:count(fn:collection('c')/*)"
	wrap XmlQueryContext qc [$mgr createQueryContext]
	wrap XmlResults res [$mgr query $query $qc]
	new XmlValue value
	$res next $value
	# only 2 of 5 docs have content
	dbxml_error_check_good $testid [$value asString] "2"

	# clean up
	delete value
	delete res
	delete qc
	delete container
	$mgr removeContainer $basename.1.dbxml
    }
    # clean up
    delete uc
    delete mgr
}

