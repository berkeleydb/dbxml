# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml012
# TEST	Delete Document

proc xml012 { args } {
    puts "\nxml012: Delete and Update Document ($args)"
    source ./include.tcl

    # Determine whether test has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 12
    set oargs [eval {set_openargs} $args]

    if { $eindex == -1 } {
        set env NULL
    } else {
        incr eindex
        set env [lindex $args $eindex]
        set txnenv [is_txnenv $env]
        set testdir [get_home $env]
	set cdbenv [is_cdbenv $env]
	if { $cdbenv == 1 } {
	    set txnenv 2
	}
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

    # delete docs
    xml012_1 $env $txnenv $basename.1 $oargs
    # update docs
    xml012_2 $env $txnenv $basename.2 $oargs
    # indexing tests, with delete flag
    xml012_3 $env $txnenv $basename.3 $oargs
    # update with indexes and bug fixes
    xml012_4 $env $txnenv $basename.4 $oargs

    return
}

proc xml012_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml012.1 Document Deletions"
    xml012_1_1 $env $txnenv $basename.1 $oargs
    xml012_1_2 $env $txnenv $basename.2 $oargs
    xml012_1_3 $env $txnenv $basename.3 $oargs
    xml012_1_4 $env $txnenv $basename.4 $oargs
}

proc xml012_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\txml012.2 Document Updates"

    # basic success case from regression suite
    xml012_2_1 $env $txnenv $basename.1 $oargs

    # update a non-existent document (failure cases)
    xml012_2_2 $env $txnenv $basename.2 $oargs

    # malformed updates
    xml012_2_3 $env $txnenv $basename.3 $oargs

    # updates and metadata
    xml012_2_4 $env $txnenv $basename.4 $oargs

    # updates, lazy evaluation, and query results
    # motivated by SR [#11838]
    xml012_2_5 $env $txnenv $basename.5 $oargs
}

# 12.3 repeat container indexing tests but with the 'delete' flag set
proc xml012_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs {regress 0} } {
    source ./include.tcl
    if {$txnenv == 2} {
          dbxml_pending_test $basename 15466 "CDS configuration self-deadlocks on this test"
	return
    }
    # same document set as 10.2
    set documents {
	document_set_10_2/document_10_2_match.xml
	document_set_10_2/document_10_2_not_match_1.xml
	document_set_10_2/document_10_2_not_match_2.xml
    }

    set dir "$test_path/data_set_10"
    set exclude "42"

    container_indexing_tests "10_2" "10.2 12.3" $dir $documents 1 $exclude $env $txnenv $oargs $regress
}

proc xml012_1_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml012.1.1: Get/Delete document with document name that does not exist (should fail)."
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

    # get doc
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	catch {$container getDocument $txn ghost} ret
	$txn commit
	delete txn
    } else {
	catch {$container getDocument ghost } ret
    }

    dbxml_error_check_good $basename  \
	[is_substr $ret "Document not found"] 1

    set ret "yyy"
    # now try delete
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	catch {$container deleteDocument $txn ghost $uc} ret
	$txn commit
	delete txn
    } else {
	catch {$container deleteDocument ghost $uc} ret
    }

    dbxml_error_check_good $basename  \
	[is_substr $ret "Document not found"] 1

    delete container
    delete uc
    delete db
}

proc xml012_1_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml012.1.2: Delete document with document name."
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

    set doc_name doc_$basename

    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    $xd setName $doc_name

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	catch {$container deleteDocument $txn $doc_name $uc} ret
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
	catch {$container deleteDocument $doc_name $uc} ret
    }

    dbxml_error_check_good $basename $ret ""
    delete xd

    delete container
    delete uc
    delete db
}

proc xml012_1_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml012.1.3: Delete document with XmlDocument object."
    source ./include.tcl
    set txn NULL
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

    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"
    $xd setName "Hello"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	catch {$container deleteDocument $txn $xd $uc} ret
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
	catch {$container deleteDocument $xd $uc} ret
    }

    dbxml_error_check_good $basename $ret ""
    delete xd

    delete container
    delete uc
    delete db
}

proc xml012_1_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\txml012.1.4: Delete document with XmlDocument object that does not exist (should fail)."
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

    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<hello/>"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	catch {$container deleteDocument $txn $xd $uc} ret
	$txn commit
	delete txn
    } else {
	catch {$container deleteDocument $xd $uc} ret
    }

    dbxml_error_check_good $basename  \
		[is_substr $ret "Document not found"] 1
    delete xd

    delete container
    delete uc
    delete db
}

#  12.2.1 Create a container, add indexing strategies so that one is unused, add a document. Verify the indexing statistics.
#  Update the document content such that all indexing strategies are used.  Verify the indexing statistics.
proc xml012_2_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\t$basename: updateDocument."

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env

    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    set id 0
    set testid $basename.[incr id]
    puts "\t\t\t$testid: check indexes after put document."

    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        $container addIndex $xtxn "" "a" "node-element-presence-none" $uc
        $container addIndex $xtxn "" "b" "node-element-presence-none" $uc
        $container addIndex $xtxn "" "c" "node-element-presence-none" $uc
        $xtxn commit
        delete xtxn
    } else {
        $container addIndex "" "a" "node-element-presence-none" $uc
        $container addIndex "" "b" "node-element-presence-none" $uc
        $container addIndex "" "c" "node-element-presence-none" $uc
    }

    wrap XmlDocument xd [$db createDocument]
    set name "foo"
    $xd setName $name
    $xd setContent "<a><b/></a>"
    new XmlValue xv $XmlValue_DATE "2004-09-13"
    $xd setMetaData "" "bar" $xv

    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        $container putDocument $xtxn $xd $uc
        $xtxn commit
        delete xtxn
    } else {
        $container putDocument $xd $uc
    }
    delete xd
    delete xv

    # verify indexes (should be defined on nodes 'a' and 'b' but not 'c')
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "a" "node-element-presence-none" 1 "foo" 1 1
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "b" "node-element-presence-none" 1 "foo" 1 1
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "c" "node-element-presence-none" 0 "" 0 0

    set testid $basename.[incr id]
    puts "\t\t\t$testid: check indexes after update document."

    # get document and update content so that 'b' is replaced by 'c'
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $xtxn $name]
        $xd setContent "<a><c/></a>"
        $container updateDocument $xtxn $xd $uc
        $xtxn commit
        delete xtxn
        delete xd
    } else {
        wrap XmlDocument xd [$container getDocument $name]
        $xd setContent "<a><c/></a>"
        $container updateDocument $xd $uc
        delete xd
    }

    # verify document content
    xml012_verify_document $testid $txnenv $db $container "foo" "<a><c/></a>" "bar" "2004-09-13"

    # now there should be indexes defined on nodes 'a' and 'c' but not 'b'
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "a" "node-element-presence-none" 1 "foo" 1 1
    # we no longer delete stats but the index lookup will not
    # find the deleted content... [#17393]
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "b" "node-element-presence-none" 0 "" 1 1
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "c" "node-element-presence-none" 1 "foo" 1 1

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# 12.2.2 - update a non-existing document (failure case)
proc xml012_2_2 { env txnenv basename oargs } {
    source ./include.tcl
    puts "\t\t$basename: Document that does not exist"

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # update a document that has not been added to the container
    wrap XmlDocument xd [$db createDocument]
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction txn [$db createTransaction]
            $xd setContent "<a><c/></a>"
            $container updateDocument $txn $xd $uc
            $xtxn commit
            delete xtxn
        } else {
            $xd setContent "<a><c/></a>"
            $container updateDocument $xd $uc
        }
    } ret
    # except an exception
    dbxml_error_check_good $basename [string match "*Document not found*" $ret] 1
    delete xd

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# 12.2.3 - malformed updates (failure cases)
proc xml012_2_3 { env txnenv basename oargs } {
    puts "\t\t$basename: Malformed updates"
    xml012_2_3_1 $env $txnenv $basename.1 $oargs
    xml012_2_3_2 $env $txnenv $basename.2 $oargs
}

# updating with malformed content
proc xml012_2_3_1 { env txnenv basename oargs } {
    source ./include.tcl

    set testid $basename
    puts "\t\t\t$testid: Malformed content"

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        $container addIndex $xtxn "" "a" "node-element-presence-none" $uc
        $container addIndex $xtxn "" "c" "node-element-presence-none" $uc
        $xtxn commit
        delete xtxn
    } else {
        $container addIndex "" "a" "node-element-presence-none" $uc
        $container addIndex "" "c" "node-element-presence-none" $uc
    }

    # create and add a document
    wrap XmlDocument xd [$db createDocument]
    $xd setName "docA"
    set content "<a><b/></a>"
    $xd setContent $content
    new XmlValue xv $XmlValue_DATE "2004-09-13"
    $xd setMetaData "" "bar" $xv

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }
    delete xd
    delete xv

    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction txn [$db createTransaction]
            wrap XmlDocument xd [$container getDocument $txn "docA"]
            $xd setContent "<a><c></c>"
            $container updateDocument $txn $xd $uc
            delete xd
            $txn commit
            delete txn
        } else {
            wrap XmlDocument xd [$container getDocument "docA"]
            $xd setContent "<a><c></c>"
            $container updateDocument $xd $uc
            delete xd
        }
    } ret
    # expect a (parse) exception
    dbxml_error_check_good $basename [string match "*Error*" $ret] 1

    if { $txnenv == 1 } {
        $txn abort
    }

    if { $global_container_type == $XmlContainer_NodeContainer } {
        if { $txnenv != 1 } {
            # 2004-10-14 JPCS
          dbxml_pending_test $basename 11190 "Failed update leaves DB in inconsistant state under NLS with no transactions"

            # clean up
            delete container
            delete qc
            delete uc
            delete db

            return
        }
    }

    # verify that the document is unchanged
    xml012_verify_document $testid $txnenv $db $container "docA" $content "bar" "2004-09-13"

    # verify that the indexes and statistics on the container are unchanged
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "a" "node-element-presence-none" 1 "docA" 1 1
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "c" "node-element-presence-none" 0 "" 0 0

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# updating with a non-unique name
proc xml012_2_3_2 { env txnenv basename oargs } {
    source ./include.tcl

    set testid $basename
    puts "\t\t\t$basename: Non-unique name"

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container addIndex $txn "" "a" "node-element-presence-none" $uc
        $container addIndex $txn "" "d" "node-element-presence-none" $uc
        $txn commit
        delete txn
    } else {
        $container addIndex "" "a" "node-element-presence-none" $uc
        $container addIndex "" "d" "node-element-presence-none" $uc
    }

    # create and add 2 documents
    wrap XmlDocument xd_a [$db createDocument]
    $xd_a setName "docA"
    set content_a "<a><b/></a>"
    $xd_a setContent $content_a
    new XmlValue xv_a $XmlValue_DATE "2004-12-25"
    $xd_a setMetaData "" "bar" $xv_a

    wrap XmlDocument xd_b [$db createDocument]
    $xd_b setName "docB"
    set content_b "<d><e/></d>"
    $xd_b setContent $content_b
    new XmlValue xv_b $XmlValue_DATE "2004-12-26"
    $xd_b setMetaData "" "bar" $xv_b

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd_a $uc
        $container putDocument $txn $xd_b $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd_a $uc
        $container putDocument $xd_b $uc
    }
    delete xd_a
    delete xd_b
    delete xv_a
    delete xv_b

    # attempt to update so that both documents have the same name
    catch {
        if { $txnenv == 1 } {

            wrap XmlTransaction txn [$db createTransaction]
            wrap XmlDocument xd [$container getDocument $txn "docA"]
            $xd setName "docB"
            $container updateDocument $txn $xd $uc
            delete xd
            $txn commit
            delete txn
        } else {
            wrap XmlDocument xd [$container getDocument "docA"]
            $xd setName "docB"
            $container updateDocument $xd $uc
            delete xd
        }
    } ret
    # expect a (uniquess constraint) exception
    dbxml_error_check_good $basename [string match "*Uniqueness*violation*" $ret] 1

    if { $txnenv == 1 } {
        $txn abort
    }

    # verify that the documents are unchanged
    xml012_verify_document $testid $txnenv $db $container "docA" $content_a "bar" "2004-12-25"
    xml012_verify_document $testid $txnenv $db $container "docB" $content_b "bar" "2004-12-26"

    # verify that the indexes and statistics on the container are unchanged
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "a" "node-element-presence-none" 1 "docA" 1 1
    xml012_verify_container_indexing $testid $txnenv $db $container $qc "" "d" "node-element-presence-none" 1 "docB" 1 1

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# helper method to verify that document has not changed after a bad update
proc xml012_verify_document {testid txnenv db container name content {md_name ""} {md_value ""} } {

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $name]
    } else {
        wrap XmlDocument xd [$container getDocument $name]
    }

    # verify content
    dbxml_error_check_good "$testid - doc content" [$xd getContentAsString] $content

    # verify metadata
    if { $md_name != "" } {
        new XmlValue xv
        $xd getMetaData "" $md_name $xv
        dbxml_error_check_good "$testid - doc metadata" [$xv asString] $md_value
        delete xv
    }

    delete xd

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
}

# helper method to verify that container indexing has not changed after a
# bad update
proc xml012_verify_container_indexing {testid txnenv db container qc is_uri is_name is_index count doc_name keys_indexed keys_unique} {
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlResults xr [$container lookupIndex $txn $qc $is_uri $is_name $is_index]
        wrap XmlStatistics xs [$container lookupStatistics $txn $is_uri $is_name $is_index]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc $is_uri $is_name $is_index]
        wrap XmlStatistics xs [$container lookupStatistics $is_uri $is_name $is_index]
    }
    xml012_check_results $testid $xr $count $doc_name
    xml012_check_statistics $testid $xs $keys_indexed $keys_unique

    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
}

# updates involving changes to metadata (motivated by observed buggy behaviour)
proc xml012_2_4 { env txnenv basename oargs } {
    source ./include.tcl

    set testid $basename
    puts "\t\t$testid: Updates and Metadata"

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # create and add a document that contains metadata
    wrap XmlDocument xd [$db createDocument]
    set name "my_doc"
    $xd setName $name
    set content "<a><b/></a>"
    $xd setContent $content
    new XmlValue xv $XmlValue_DATE "2004-09-17"
    $xd setMetaData "" "foo" $xv

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }
    delete xd
    delete xv

    # retrieve document, add more metadata, and update
    new XmlValue xv $XmlValue_ANY_URI "http://www.example.com/schema/"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $name]

        $xd setMetaData "" "bar" $xv
        $container updateDocument $txn $xd $uc

        $txn commit
        delete txn

        delete xd
    } else {
        wrap XmlDocument xd [$container getDocument $name]

        $xd setMetaData "" "bar" $xv
        $container updateDocument $xd $uc

        delete xd
    }
    delete xv

    # now retrieve again and verify that the update worked
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $name]

        # content
        dbxml_error_check_good "$testid - content" [$xd getContentAsString] $content

        # metadata
        new XmlValue xv
        $xd getMetaData "" "foo" $xv
        dbxml_error_check_good "$testid - old metadata found" [$xv isNull] 0
        dbxml_error_check_good "$testid - old metadata value" [$xv asString] "2004-09-17"
        delete xv

        new XmlValue xv
        $xd getMetaData "" "bar" $xv
        dbxml_error_check_good "$testid - updated metadata found" [$xv isNull] 0
        dbxml_error_check_good "$testid - updated metadata value" [$xv asString] "http://www.example.com/schema/"
        delete xv

        $txn commit
        delete txn

        delete xd
    } else {
        wrap XmlDocument xd [$container getDocument $name]

        # content
        dbxml_error_check_good "$testid - content" [$xd getContentAsString] $content

        # metadata
        new XmlValue xv
        $xd getMetaData "" "foo" $xv
        dbxml_error_check_good "$testid - old metadata found" [$xv isNull] 0
        dbxml_error_check_good "$testid - old metadata value" [$xv asString] "2004-09-17"
        delete xv

        new XmlValue xv
        $xd getMetaData "" "bar" $xv
        dbxml_error_check_good "$testid - updated metadata found" [$xv isNull] 0
        dbxml_error_check_good "$testid - updated metadata value" [$xv asString] "http://www.example.com/schema/"
        delete xv

        delete xd
    }

    # now, attempt to remove the "name" metadata item.
    # this MUST fail
    set uri "http://www.sleepycat.com/2002/dbxml"
    set node "name"

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $name]
	catch {
	    $xd removeMetaData $uri $node
	} ret
	$txn commit
	delete txn
	delete xd
    } else {
        wrap XmlDocument xd [$container getDocument $name]
	catch {
	    $xd removeMetaData $uri $node
	} ret
	delete xd
    }
    dbxml_error_check_good $basename [string match "*cannot remove*metadata*" $ret] 1

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# updates involving lazy evaluation, node storage
# motivated by sr [#11838] and [#13933]
proc xml012_2_5 { env txnenv basename oargs } {
    source ./include.tcl

    set testid $basename
    puts "\t\t$testid: Updates and Lazy Evaluation"

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add several documents
    wrap XmlDocument xd [$db createDocument]
    set name "my_doc"
    $xd setName $name
    set content "<a><b/></a>"
    set newContent "<root><a>x</a><b>x</b><c><d>x</d></c></root>"
    $xd setContent $content
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
	# 3 more docs
	$container putDocument $txn $name $content $uc $DBXML_GEN_NAME
	$container putDocument $txn $name $content $uc $DBXML_GEN_NAME
	$container putDocument $txn $name $content $uc $DBXML_GEN_NAME
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
	# 3 more docs
	$container putDocument $name $content $uc $DBXML_GEN_NAME
	$container putDocument $name $content $uc $DBXML_GEN_NAME
	$container putDocument $name $content $uc $DBXML_GEN_NAME
    }
    delete xd
    # retrieve document, using lazy evaluation, and update
    $qc setEvaluationType $XmlQueryContext_Lazy
    set queryString "collection('[$container getName]')/a/b"
    if { $txnenv != 0 } {
	if { $txnenv == 1} {
	    wrap XmlTransaction txn [$db createTransaction]
	} else {
	    wrap Transaction dbtxn [$env cdsgroup]
	    wrap XmlTransaction txn [$db createTransaction $dbtxn]
	}
        wrap XmlResults xr [$db query $txn $queryString $qc]
        new XmlValue xv
        while { [ $xr next $xv ] } {
	    dbxml_error_check_good "results exist" [$xv isNull] 0
	    wrap XmlDocument xd [$xv asDocument]
	    set val [$xv asString]
	    $xd setContent $newContent
	    $container updateDocument $txn $xd $uc
	    delete xd
	}
        $txn commit
        delete txn
    } else {
        wrap XmlResults xr [$db query $queryString $qc]
        new XmlValue xv
        while { [ $xr next $xv ] } {
	    dbxml_error_check_good "results exist" [$xv isNull] 0
	    wrap XmlDocument xd [$xv asDocument]
	    set val [$xv asString]
	    $xd setContent $newContent
	    $container updateDocument $xd $uc
	    delete xd
	}
    }
    delete xv
    delete xr
    # now retrieve again and verify that the update worked
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $name]
        # content
        dbxml_error_check_good "$testid - content" [$xd getContentAsString] $newContent
        $txn commit
        delete txn
        delete xd
    } else {
        wrap XmlDocument xd [$container getDocument $name]
        # content
        dbxml_error_check_good "$testid - content" [$xd getContentAsString] $newContent
        delete xd
    }
    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# 12.4.1 - update where index values are the same [#14276] and [#14173]
proc xml012_4_1 { env txnenv basename oargs } {
    source ./include.tcl
    puts "\t\t$basename: update with duplicate index entries"
    set testid $basename
    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }

    # this test case adds identical index entries
    set content "<index><w n='XXX'><lk/></w><w n='XXX'><lk/></w></index>"
    set newcontent "<index><w n='XXX'><lk/></w><w n='XXX'><lk/></w><w n='XXX'><lk/></w></index>"
    set docname "foo"
    set uri ""
    set nodename "n"
    set idx "node-attribute-equality-string" 
    # add an index and put document and verify index entries
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        $container addIndex $xtxn $uri $nodename $idx $uc
        $container putDocument $xtxn $docname $content $uc
	wrap XmlIndexLookup il [$mgr createIndexLookup $container $uri $nodename $idx]
	wrap XmlResults xr [$il execute $xtxn $qc]
	dbxml_error_check_bad "$testid - expected non-zero results (1)" [$xr size] 0
        $xtxn commit
        delete xtxn
	delete il
	delete xr
    } else {
        $container addIndex "" "n" "node-attribute-equality-string" $uc
        $container putDocument $docname $content $uc
	wrap XmlIndexLookup il [$mgr createIndexLookup $container $uri $nodename $idx]
	wrap XmlResults xr [$il execute $qc]
	dbxml_error_check_bad "$testid - expected non-zero results (1)" [$xr size] 0
	delete il
	delete xr
    }

    # now update the doc
    wrap XmlDocument xd [$mgr createDocument]
    $xd setName $docname
    $xd setContent $newcontent

    # perform the update and verify index entries
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        $container updateDocument $xtxn $xd $uc
	wrap XmlIndexLookup il [$mgr createIndexLookup $container $uri $nodename $idx]
	wrap XmlResults xr [$il execute $xtxn $qc]
	dbxml_error_check_bad "$testid - expected non-zero results (2)" [$xr size] 0
        $xtxn commit
        delete xtxn
	delete il
	delete xr
    } else {
        $container updateDocument $xd $uc
	wrap XmlIndexLookup il [$mgr createIndexLookup $container $uri $nodename $idx]
	wrap XmlResults xr [$il execute $qc]
	dbxml_error_check_bad "$testid - expected non-zero results (2)" [$xr size] 0
	delete il
	delete xr
    }

    delete xd
    # clean up
    delete container
    delete qc
    delete uc
    delete mgr
}

# 12.4  Updates and indexes and bug fixes
proc xml012_4 { env txnenv basename oargs } {
    puts "\t\t$basename: Updates and indexes"
    xml012_4_1 $env $txnenv $basename.1 $oargs
}


# 2004-09-13 ARW maybe put these methods in xmlutils.tcl

# check results by verifying count, a document name
proc xml012_check_results { testid results count { doc_name "" } } {
    set c 0
    set err 0
    set found_named_doc 0
    while { ! $err } {
        new XmlValue xv
        $results next $xv
        if { ! [$xv getType] } {
            set err 1
        } else {
            incr c
            wrap XmlDocument xd [$xv asDocument]
            if { ! [string compare [$xd getName] $doc_name] } {
                set found_named_doc 1
            }
            delete xd
        }
        delete xv
    }
    dbxml_error_check_good "$testid - result count" $c $count
    if { $count && [string compare $doc_name ""] } {
        dbxml_error_check_good "$testid - found document" $found_named_doc 1
    }
}

# check statistics by verifying count. ...
proc xml012_check_statistics { testid statistics indexed unique } {
    # allow for decimals...
    dbxml_error_check_good "$testid - indexed keys" [$statistics getNumberOfIndexedKeys] $indexed.0
    dbxml_error_check_good "$testid - unique keys" [$statistics getNumberOfUniqueKeys] $unique.0
}

