# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
# TEST	xml010
# TEST	XML Indexer and XmlIndexLookup

proc xml010 { args } {
    puts "\nxml010: Container Indexing ($args)"
    source ./include.tcl

    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set oargs [eval {set_openargs} $args]

    if { $eindex == -1 } {
	set env NULL
    } else {
	incr eindex
	set env [lindex $args $eindex]
	set txnenv [is_txnenv $env]
	set testdir [get_home $env]
    }

    # assign the container type (default to container storage)
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

    set tnum 10
    set basename $tnum

    # API tests
    xml010_1 $env $txnenv $basename.1 $oargs

    # old data driven tests (restricted to comparing results for (un)indexed c'r)
    xml010_2_and_3 $env $txnenv $basename.2 $oargs

    # new regression tests, based on bugs found and extensions
    xml010_4 $env $txnenv $basename.4 $oargs

    # index BTree comparison function tests
    xml010_5 $env $txnenv $basename.5 $oargs

    # miscellaneous index tests
    xml010_6 $env $txnenv $basename.6 $oargs
}

# API tests - base procedure
proc xml010_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\txml010.1: API testing"
    set id 0

    # 1.x regression tests
    xml010_1_1 $env $txnenv $basename.[incr id] $oargs

    # default indexes
    xml010_1_2 $env $txnenv $basename.[incr id] $oargs

    # unique indexes
    xml010_1_3 $env $txnenv $basename.[incr id] $oargs

    # indexing a non-empty container
    xml010_1_4 $env $txnenv $basename.[incr id] $oargs

    # decimal index on a zero value etc.
    xml010_1_5 $env $txnenv $basename.[incr id] $oargs

    # lookup methods on indexes and statistics
    xml010_1_6 $env $txnenv $basename.[incr id] $oargs

    # default index (former bug)
    xml010_1_7 $env $txnenv $basename.[incr id] $oargs

    # former index spec bugs
    xml010_1_8 $env $txnenv $basename.[incr id] $oargs
}

# API tests - regression test suite
proc xml010_1_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Regression Testing"
    xml010_1_1_2 $env $txnenv $basename.2 $oargs
    xml010_1_1_4 $env $txnenv $basename.4 $oargs
    xml010_1_1_5 $env $txnenv $basename.5 $oargs
    xml010_1_1_6 $env $txnenv $basename.6 $oargs
    xml010_1_1_8 $env $txnenv $basename.8 $oargs
    xml010_1_1_9 $env $txnenv $basename.9 $oargs
    xml010_1_1_10 $env $txnenv $basename.10 $oargs
    xml010_1_1_11 $env $txnenv $basename.11 $oargs
    xml010_1_1_12 $env $txnenv $basename.12 $oargs
    xml010_1_1_13 $env $txnenv $basename.13 $oargs
    xml010_1_1_14 $env $txnenv $basename.14 $oargs
    xml010_1_1_15 $env $txnenv $basename.15 $oargs
    xml010_1_1_16 $env $txnenv $basename.16 $oargs
}

proc xml010_1_1_2 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: getIndexSpecification on an open container."

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs]
    }

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification is [$container getIndexSpecification]
	}
	delete is
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete db
}

proc xml010_1_1_4 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: setIndexSpecification on an open container."

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

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]

	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addIndex "" "foo" "node-element-presence-none"
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addIndex "" "foo" "node-element-presence-none"
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_5 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: setIndexSpecification on non-empty container."
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

    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $txn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn "" "foo" "node-element-presence-none" $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex "" "foo" "node-element-presence-none" $uc
	}
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_6 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Index specification persists after close."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    set container_name $basename.dbxml
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    set uri www.uri.com
    set name foo
    set index node-element-presence-none
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn $uri $name $index $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex $uri $name $index $uc
	}
    } ret
    delete container
    dbxml_error_check_good $basename $ret ""

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db openContainer $txn $container_name $oargs]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db openContainer $container_name $oargs]
    }

    # Test that index spec was persisted.
    wrap XmlIndexSpecification xis [$container getIndexSpecification]
    set idecl [$xis find $uri $name]
    delete xis
    dbxml_error_check_bad foundindex $idecl NULL

    wrap XmlIndexDeclaration xid $idecl
    set retindex [$xid get_index]
    delete xid
    dbxml_error_check_good index $index $retindex

    delete container
    delete uc
    delete db
}

proc xml010_1_1_8 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: deleteIndex on non-empty container."
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

    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $txn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]

	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addIndex "" "person" "node-element-presence-none"
	    $xis addIndex "" "person" "node-element-equality-string"
	    $xis addIndex "" "name" "node-element-presence-none"
	    $container setIndexSpecification $txn $xis $uc
	    delete xis
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis deleteIndex "" "person" "node-element-equality-string"
	    $xis deleteIndex "" "name" "node-element-presence-none"
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addIndex "" "person" "node-element-presence-none"
	    $xis addIndex "" "person" "node-element-equality-string"
	    $xis addIndex "" "name" "node-element-presence-none"
	    $container setIndexSpecification $xis $uc
	    delete xis
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis deleteIndex "" "person" "node-element-equality-string"
	    $xis deleteIndex "" "name" "node-element-presence-none"
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # Test that index spec was persisted.
    wrap XmlIndexSpecification xis [$container getIndexSpecification]
    set xid [$xis find "" "name"]
    if { $xid != "NULL" } {
	dbxml_error_check_good index [is_substr [$xid get_index] "node-element-presence-none"] 0
	delete xid
    }
    wrap XmlIndexDeclaration xid [$xis find "" "person"]
    dbxml_error_check_good index [$xid get_index] "node-element-presence-none"
    delete xid
    delete xis

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_9 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: replaceIndex on non-empty container."
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

    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $txn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]

	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addIndex "" "person" "node-element-presence-none"
	    $xis addIndex "" "person" "node-element-equality-string"
	    $container setIndexSpecification $txn $xis $uc
	    delete xis
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis replaceIndex "" "person" "edge-element-presence-none edge-element-equality-string"
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addIndex "" "person" "node-element-presence-none"
	    $xis addIndex "" "person" "node-element-equality-string"
	    $container setIndexSpecification $xis $uc
	    delete xis
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis replaceIndex "" "person" "edge-element-presence-none edge-element-equality-string"
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # Test that index spec was persisted.
    wrap XmlIndexSpecification xis [$container getIndexSpecification]
    wrap XmlIndexDeclaration xid [$xis find "" "person"]
    dbxml_error_check_good index [$xid get_index] "edge-element-presence-none edge-element-equality-string"
    delete xid
    delete xis

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_10 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: addIndex on an open container."
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

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn "" "foo" "node-element-presence-none" $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex "" "foo" "node-element-presence-none" $uc
	}
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_11 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: deleteIndex on an open container."
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

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container deleteIndex $txn "" "foo" "node-element-presence-none" $uc
	    $txn commit
	    delete txn
	} else {
	    $container deleteIndex "" "foo" "node-element-presence-none" $uc
	}
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_12 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: replaceIndex on an open container."
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

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container replaceIndex $txn "" "foo" "node-element-presence-none" $uc
	    $txn commit
	    delete txn
	} else {
	    $container replaceIndex "" "foo" "node-element-presence-none" $uc
	}
    } ret

    dbxml_error_check_good $basename $ret ""
    delete container
    delete uc
    delete db
}

proc xml010_1_1_13 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: addIndex and reindex"

    # prepare - create database, contexts, container
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlQueryContext qc [$db createQueryContext]
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add a document to the unindexed container
    set xtxn "NULL"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $xtxn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
        $xtxn commit
        delete xtxn
    }

    # now index the container
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]

            wrap XmlIndexSpecification xis [$container getIndexSpecification $xtxn]
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xtxn $xis $uc
            delete xis

            wrap XmlResults xr [$container lookupIndex $xtxn $qc "" "person" "node-element-equality-string"]
            set xr_size [$xr size]
            delete xr

            wrap XmlStatistics xs [$container lookupStatistics $xtxn "" "person" "node-element-equality-string"]
            set xs_keys [$xs getNumberOfIndexedKeys]
            delete xs

            $xtxn commit
            delete xtxn
        } else {
            wrap XmlIndexSpecification xis [$container getIndexSpecification]
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xis $uc
            delete xis

            wrap XmlResults xr [$container lookupIndex $qc "" "person" "node-element-equality-string"]
            set xr_size [$xr size]
            delete xr

            wrap XmlStatistics xs [$container lookupStatistics "" "person" "node-element-equality-string"]
            set xs_keys [$xs getNumberOfIndexedKeys]
            delete xs
        }

        # verify that the index was added and will be used
        dbxml_error_check_good $basename $xr_size 1
        dbxml_error_check_good $basename $xs_keys 1.0

        set foo ""
    } ret

    # verify that everything worked
    dbxml_error_check_good $basename $ret ""

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

proc xml010_1_1_14 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: deleteIndex and reindex"

    # prepare - create database, contexts, container
    source ./include.tcl
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

    # add a document to the unindexed container
    set xtxn "NULL"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $xtxn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
        $xtxn commit
        delete xtxn
    }

    # add an index to the container; verify index; delete this index; verify
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]

            wrap XmlIndexSpecification xis [$container getIndexSpecification $xtxn]
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xtxn $xis $uc

            # verify index
            wrap XmlResults xr [$container lookupIndex $xtxn $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs [$container lookupStatistics $xtxn "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr size] 1
            dbxml_error_check_good $basename [$xs getNumberOfIndexedKeys] 1.0
            delete xr
            delete xs

            # delete index
            $xis deleteIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xtxn $xis $uc
            delete xis

            # verify that index has gone
            wrap XmlResults xr [$container lookupIndex $xtxn $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs [$container lookupStatistics $xtxn "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr size] 0
            dbxml_error_check_good $basename [$xs getNumberOfIndexedKeys] 0.0
            delete xr
            delete xs

            $xtxn commit
            delete xtxn
        } else {
            wrap XmlIndexSpecification xis [$container getIndexSpecification]
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xis $uc

            # verify index
            wrap XmlResults xr [$container lookupIndex $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs [$container lookupStatistics "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr size] 1
            dbxml_error_check_good $basename [$xs getNumberOfIndexedKeys] 1.0
            delete xr
            delete xs

            # delete index
            $xis deleteIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xis $uc
            delete xis

            # verify that index has gone
            wrap XmlResults xr [$container lookupIndex $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs [$container lookupStatistics "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr size] 0
            dbxml_error_check_good $basename [$xs getNumberOfIndexedKeys] 0.0
            delete xr
            delete xs
        }
    } ret

    # verify that everything worked
    dbxml_error_check_good $basename $ret ""

    # clean up
    delete container
    delete qc
    delete uc
    delete db

}

proc xml010_1_1_15 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: reindexing"

    # prepare - create database, contexts, container
    source ./include.tcl
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

    # add a document to the unindexed container
    set xtxn "NULL"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $xtxn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
        $xtxn commit
        delete xtxn
    }

    # add two indexes; verify; remove one and add another; verify
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]

            # add indexes on the "name" and "person" nodes
            wrap XmlIndexSpecification xis [$container getIndexSpecification $xtxn]
            $xis addIndex "" "name" "node-element-presence-none"
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xtxn $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $xtxn $qc "" "name" "node-element-presence-none"]
            wrap XmlResults xr_2 [$container lookupIndex $xtxn $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics $xtxn "" "name" "node-element-presence-none"]
            wrap XmlStatistics xs_2 [$container lookupStatistics $xtxn "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xs_1
            delete xs_2

            # delete the "person" index, add an "age" index
            wrap XmlIndexSpecification xis [$container getIndexSpecification $xtxn]
            $xis deleteIndex "" "person" "node-element-equality-string"
            $xis addIndex "" "age" "node-attribute-equality-string"
            $container setIndexSpecification $xtxn $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $xtxn $qc "" "name" "node-element-presence-none"]
            wrap XmlResults xr_2 [$container lookupIndex $xtxn $qc "" "person" "node-element-equality-string"]
            wrap XmlResults xr_3 [$container lookupIndex $xtxn $qc "" "age" "node-attribute-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics $xtxn "" "name" "node-element-presence-none"]
            wrap XmlStatistics xs_2 [$container lookupStatistics $xtxn "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs_3 [$container lookupStatistics $xtxn "" "age" "node-attribute-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 0
            dbxml_error_check_good $basename [$xr_3 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 0.0
            dbxml_error_check_good $basename [$xs_3 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xr_3
            delete xs_1
            delete xs_2
            delete xs_3

            $xtxn commit
            delete xtxn
        } else {
            wrap XmlIndexSpecification xis [$container getIndexSpecification]

            # add indexes on the "name" and "person" nodes
            $xis addIndex "" "name" "node-element-presence-none"
            $xis addIndex "" "person" "node-element-equality-string"
            $container setIndexSpecification $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $qc "" "name" "node-element-presence-none"]
            wrap XmlResults xr_2 [$container lookupIndex $qc "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics "" "name" "node-element-presence-none"]
            wrap XmlStatistics xs_2 [$container lookupStatistics "" "person" "node-element-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xs_1
            delete xs_2

            # delete the "person" index, add an "age" index
            wrap XmlIndexSpecification xis [$container getIndexSpecification]
            $xis deleteIndex "" "person" "node-element-equality-string"
            $xis addIndex "" "age" "node-attribute-equality-string"
            $container setIndexSpecification $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $qc "" "name" "node-element-presence-none"]
            wrap XmlResults xr_2 [$container lookupIndex $qc "" "person" "node-element-equality-string"]
            wrap XmlResults xr_3 [$container lookupIndex $qc "" "age" "node-attribute-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics "" "name" "node-element-presence-none"]
            wrap XmlStatistics xs_2 [$container lookupStatistics "" "person" "node-element-equality-string"]
            wrap XmlStatistics xs_3 [$container lookupStatistics "" "age" "node-attribute-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 0
            dbxml_error_check_good $basename [$xr_3 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 0.0
            dbxml_error_check_good $basename [$xs_3 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xr_3
            delete xs_1
            delete xs_2
            delete xs_3
        }
    } ret

    # verify that everything worked
    dbxml_error_check_good $basename $ret ""

    # clean up
    delete container
    delete qc
    delete uc
    delete db
}

# indexes on multiple attributes of a node - motivated by a bug
proc xml010_1_1_16 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: index multiple attributes of element"

    # prepare - create database, contexts, container
    source ./include.tcl
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

    # add a document to the unindexed container
    set xtxn "NULL"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $xtxn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
        $xtxn commit
        delete xtxn
    }

    # add indexes on two attributes
    catch {
	set fullQuery "collection('[$container getName]')/person"
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]

            wrap XmlIndexSpecification xis [$container getIndexSpecification $xtxn]
            $xis addIndex "" "age" "node-attribute-equality-string"
            $xis addIndex "" "drink" "node-attribute-equality-string"
            $container setIndexSpecification $xtxn $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $xtxn $qc "" "age" "node-attribute-equality-string"]
            wrap XmlResults xr_2 [$container lookupIndex $xtxn $qc "" "drink" "node-attribute-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics $xtxn "" "age" "node-attribute-equality-string"]
            wrap XmlStatistics xs_2 [$container lookupStatistics $xtxn "" "drink" "node-attribute-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xs_1
            delete xs_2

            # query for the owner element (should be 1 hit)
            wrap XmlResults xr [$db query $xtxn $fullQuery $qc 0]
            dbxml_error_check_good "$basename: size_of_result_set" [$xr size] 1
            delete xr

            $xtxn commit
            delete xtxn
        } else {
            wrap XmlIndexSpecification xis [$container getIndexSpecification]
            $xis addIndex "" "age" "node-attribute-equality-string"
            $xis addIndex "" "drink" "node-attribute-equality-string"
            $container setIndexSpecification $xis $uc
            delete xis

            # verify these indexes
            wrap XmlResults xr_1 [$container lookupIndex $qc "" "age" "node-attribute-equality-string"]
            wrap XmlResults xr_2 [$container lookupIndex $qc "" "drink" "node-attribute-equality-string"]
            wrap XmlStatistics xs_1 [$container lookupStatistics "" "age" "node-attribute-equality-string"]
            wrap XmlStatistics xs_2 [$container lookupStatistics "" "drink" "node-attribute-equality-string"]
            dbxml_error_check_good $basename [$xr_1 size] 1
            dbxml_error_check_good $basename [$xr_2 size] 1
            dbxml_error_check_good $basename [$xs_1 getNumberOfIndexedKeys] 1.0
            dbxml_error_check_good $basename [$xs_2 getNumberOfIndexedKeys] 1.0
            delete xr_1
            delete xr_2
            delete xs_1
            delete xs_2

            # query for the owner element (should be 1 hit)
            wrap XmlResults xr [$db query $fullQuery $qc 0]
            dbxml_error_check_good "$basename: size_of_result_set" [$xr size] 1
            delete xr
        }
    } ret

    # verify that everything worked
    dbxml_error_check_good $basename $ret ""

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

proc xml010_1_2 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Special Indexes"
    xml010_1_2_1 $env $txnenv $basename.1 $oargs
    xml010_1_2_2 $env $txnenv $basename.2 $oargs
    xml010_1_2_3 $env $txnenv $basename.3 $oargs
    xml010_1_2_4 $env $txnenv $basename.4 $oargs
    xml010_1_2_5 $env $txnenv $basename.5 $oargs
}

proc is_default_index { xid } {
    if { [string match "*www.sleepycat.com*" [$xid get_uri] ] == 1 &&  \
	     [string compare "name" [$xid get_name] ] == 0  && \
	     [string compare "unique-node-metadata-equality-string" [$xid get_index] ] == 0 } {
	return 1
    } else {
	return 0
    }
}

# default index - verify that it has been added
proc xml010_1_2_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Verify permanent index on a new container"

    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # verify that default index has been added
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    set found 0
    set err 0
    while { ! $err } {
	set x [$is next]
	if { $x == "NULL" } {
	    set err 1
	} else {
	    wrap XmlIndexDeclaration xid $x
	    if {[is_default_index $xid]} {
		set found 1
	    }
	    delete xid
	    if { $found } { break }
	}
    }
    delete is

    dbxml_error_check_bad "$basename: Cannot find default index" $found 0

    delete uc
    delete container
    delete db
}

# default index - add indexes that clashes with the default index
#   - expected behaviour is for the new index to be ignored
proc xml010_1_2_2 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Clash with the permanent index"
    set unique_index { 1 0 }
    set c 0
    foreach i $unique_index {
	incr c
	xml010_1_2_2_run $i $env $txnenv $basename.$c $oargs
    }
}

proc xml010_1_2_2_run { unique env  txnenv basename oargs } {
    source ./include.tcl
    xml_cleanup $testdir $env

    # explain what is happening
    puts -nonewline "\t\t\t\t$basename: "
    if {$unique} {
	puts -nonewline "Unique Index"
    } else {
	puts -nonewline "Non-unique Index"
    }
    puts -nonewline "\n"

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add an index that clashes with the permanent index
    set uri "http://www.sleepycat.com/2002/dbxml"
    set node "name"
    set index ""
    if {$unique} {
	append index "unique-"
    }
    append index "metadata-equality-string"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addIndex $uri $node $index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addIndex $uri $node $index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good $basename $ret ""

    # only the permanent index should remain
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }
    set count 0
    set found 0
    set err 0
    while { ! $err } {
	set x [$is next]
	if { $x == "NULL" } {
	    set err 1
	} else {
	    incr count
	    wrap XmlIndexDeclaration xid $x
	    if {[is_default_index $xid]} {
		set found 1
	    }
	    delete xid
	}
    }
    delete is

    dbxml_error_check_good "$basename: Wrong number of indexes found" $count 1
    dbxml_error_check_bad "$basename: Cannot find default index" $found 0

    delete uc
    delete container
    delete db
}

proc xml010_1_2_3 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Default index"

    source ./include.tcl
    set id 0
    xml_cleanup $testdir $env

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    set ok [putFileInContainer $db $container "$test_path/document_set_10_2/document_10_2_match.xml" $txn]
    dbxml_error_check_bad "$basename: Error putting file in container" $ok 0
    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    incr id
    puts "\t\t\t\t$basename.$id: Add default index"

    set index "edge-element-presence-none"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addDefaultIndex $index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addDefaultIndex $index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good "$basename.$id exception occured" $ret ""

    # verify that default index has been added
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    dbxml_error_check_good "$basename.$id index does not exist" $index [$is getDefaultIndex]
    delete is

    incr id
    puts "\t\t\t\t$basename.$id: Replace default index"

    set index "node-element-presence-none"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis replaceDefaultIndex $index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis replaceDefaultIndex $index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good "$basename.$id exception occured" $ret ""

    # verify that default index has been added
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    dbxml_error_check_good "$basename.$id index does not exist" $index [$is getDefaultIndex]
    delete is

    incr id
    puts "\t\t\t\t$basename.$id: Override default index"

    # add an index that clashes with the permanent index
    set uri ""
    set node "name"
    set new_index "edge-element-equality-string"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis addIndex $uri $node $new_index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis addIndex $uri $node $new_index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good "$basename.$id exception occured" $ret ""

    # verify that default index still exists
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    wrap XmlIndexDeclaration decl [$is find $uri $node]
    dbxml_error_check_bad "$basename.$id index does not exist" $decl 0
    dbxml_error_check_good "$basename.$id index does not exist" [is_substr [$decl get_index] $new_index] 1

    dbxml_error_check_good "$basename.$id default index does not exist" [$is getDefaultIndex] $index
    delete is

    incr id
    puts "\t\t\t\t$basename.$id: Delete overriding index"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis deleteIndex $uri $node $new_index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis deleteIndex $uri $node $new_index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good "$basename.$id exception occured" $ret ""

    # verify that default index still exists
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    set xid [$is find $uri $node]
    if { $xid != "NULL" } {
	dbxml_error_check_good "$basename.$id index exists" [is_substr [$xid get_index] $new_index] 0
	delete xid
    }
    dbxml_error_check_good "$basename.$id default index does not exist" $index [$is getDefaultIndex]
    delete is

    incr id
    puts "\t\t\t\t$basename.$id: Delete default index"

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	    $xis deleteDefaultIndex $index
	    $container setIndexSpecification $txn $xis $uc
	    delete xis

	    $txn commit
	    delete txn
	} else {
	    wrap XmlIndexSpecification xis [$container getIndexSpecification]
	    $xis deleteDefaultIndex $index
	    $container setIndexSpecification $xis $uc
	    delete xis
	}
    } ret

    # no exceptions should occur
    dbxml_error_check_good "$basename.$id exception occured" $ret ""

    # verify that default index has been added
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }

    dbxml_error_check_good "$basename.$id index exists" "" [$is getDefaultIndex]
    delete is

    delete uc
    delete container
    delete db
}

proc xml010_1_2_4 { {env "NULL"} {txnenv 0} basename oargs } {
    source ./include.tcl
    xml_cleanup $testdir $env

    puts "\t\t\t$basename: test permanent (name) index removal"

    # create database, context, container
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }

    # attempt to remove the permanent index via XmlContainer API
    # 1) via XmlContainer API directly
    # 2) via XmlIndexSpecification
    set uri "http://www.sleepycat.com/2002/dbxml"
    set node "name"
    # NOTE: the "unique" part doesn't matter, both fail to
    # be removed, which is the correct behavior.
    set index "unique-metadata-equality-string"
    #set index "metadata-equality-string"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	catch {
	    $xis deleteIndex $uri $node $index
	    $container setIndexSpecification $txn $xis $uc
	} ret
	delete xis
	$txn commit
	delete txn
    } else {

	wrap XmlIndexSpecification xis [$container getIndexSpecification]
	catch {
	    $xis deleteIndex $uri $node $index
	    $container setIndexSpecification $xis $uc
	} ret
	delete xis
    }

    # exception should have been thrown
    dbxml_error_check_good $basename [string match "*cannot disable*" $ret] 1

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction xtxn [$mgr createTransaction]
	    $container deleteIndex $xtxn $uri $node $index $uc
	    $xtxn commit
	    delete xtxn
	} else {
	    $container deleteIndex $uri $node $index $uc
	}
    } ret

    # exception should have been thrown
    dbxml_error_check_good $basename [string match "*cannot disable*" $ret] 1

    # only the permanent index should remain
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification $txn]
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
    }
    set count 0
    set found 0
    set err 0
    while { ! $err } {
	set x [$is next]
	if { $x == "NULL" } {
	    set err 1
	} else {
	    incr count
	    wrap XmlIndexDeclaration xid $x
	    if {[is_default_index $xid]} {
		set found 1
	    }
	    delete xid
	}
    }
    delete is

    dbxml_error_check_good "$basename: Wrong number of indexes found" $count 1
    dbxml_error_check_bad "$basename: Cannot find default index" $found 0

    delete uc
    delete container
    delete mgr
}

proc xml010_1_2_5 { {env "NULL"} {txnenv 0} basename oargs } {
    source ./include.tcl
    xml_cleanup $testdir $env

    puts "\t\t\t$basename: test default metadata indexes"

    set mduri "http://myuri"

    # create database, context, container
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]

    $qc setDefaultCollection [$container getName]

    # create a document with 2 types of metadata
    wrap XmlDocument doc [$mgr createDocument]
    $doc setName "doc"
    $doc setContent "<root/>"
    new XmlValue xv $XmlValue_STRING "joe"
    new XmlValue xv1 $XmlValue_DOUBLE "1.5"

    $doc setMetaData "" "name" $xv
    $doc setMetaData $mduri "time" $xv1

    $container putDocument $doc $uc

    delete doc
    delete xv
    delete xv1

    # ensure that queries work for both MD items
    wrap XmlResults res [$mgr query "collection()/*\[dbxml:metadata('name') = 'joe'\]" $qc]
    dbxml_error_check_good "$basename: Bad query result count" [$res size] 1
    delete res
    $qc setNamespace "p" $mduri
    wrap XmlResults res [$mgr query "collection()/*\[dbxml:metadata('p:time') = xs:double(1.5)\]" $qc]
    dbxml_error_check_good "$basename: Bad query result count" [$res size] 1
    delete res

    # add a default index for one and regular index for the other
    $container addIndex $mduri "time" "node-metadata-equality-double" $uc
    $container addDefaultIndex "node-metadata-equality-string" $uc

    # re-validate queries
    wrap XmlResults res [$mgr query "collection()/*\[dbxml:metadata('name') = 'joe'\]" $qc]
    dbxml_error_check_good "$basename: Bad query result count" [$res size] 1
    delete res
    wrap XmlResults res [$mgr query "collection()/*\[dbxml:metadata('p:time') = xs:double(1.5)\]" $qc]
    dbxml_error_check_good "$basename: Bad query result count" [$res size] 1

    delete res
    delete qc
    delete uc
    delete container
    delete mgr
}

# API tests - unique indexes
proc xml010_1_3 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Unique indexes"
    xml010_1_3_1 $env $txnenv $basename.1 $oargs
    xml010_1_3_2 $env $txnenv $basename.2 $oargs
}

# adding a unique index but not using "equality" - expected behavour is to throw an exception
proc xml010_1_3_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Adding an illegal unique index (failure case)"

    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add illegal unique index
    set uri www.uri.com
    set node foo
    set index unique-node-element-presence

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn $uri $node $index $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex $uri $node $index $uc
	}
    } ret
    # exception (unknown index spec) expected
    dbxml_error_check_good $basename [string match "*index specification*${index}*" $ret] 1

    delete uc
    delete container
    delete db
}

# adding the same document twice - expected behaviour is to fail when a unique index is specified and pass otherwise
proc xml010_1_3_2 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Legal unique indexes"
    set unique_index { 1 0 }
    set c 0
    foreach i $unique_index {
	incr c

	puts -nonewline "\t\t\t\t$basename.$c: "
    if {$i} {
	puts "Unique Index (failure)"
    } else {
	puts "Non-unique Index (success)"
    }

	xml010_1_3_2_run $i $env $txnenv $basename.$c $oargs
    }
}

proc xml010_1_3_2_run { unique env txnenv basename oargs } {
    source ./include.tcl
    xml_cleanup $testdir $env

    # explain what is happening
    puts "\t\t\t\t\t$basename.1: Adding the same document twice"

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add index
    set uri {}
    set node foo
    set index ""
    if {$unique} {
	append index "unique-"
    }
    append index "node-element-equality-string"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container addIndex $txn $uri $node $index $uc
	$txn commit
	delete txn
    } else {
	$container addIndex $uri $node $index $uc
    }

    # add document twice
    set flags $DBXML_GEN_NAME

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]

	wrap XmlDocument xd [$db createDocument]
	$xd setContent {<foo>bar</foo>}
	$xd setName hello

	$container putDocument $txn $xd $uc $flags
	delete xd

	$txn commit
	delete txn
    } else {
	wrap XmlDocument xd [$db createDocument]
	$xd setContent {<foo>bar</foo>}
	$xd setName hello
	$container putDocument $xd $uc $flags
	delete xd
    }

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    }
    wrap XmlDocument xd [$db createDocument]
    $xd setContent {<foo>bar</foo>}
    $xd setName hello

    catch {
	if { $txnenv == 1 } {
	    $container putDocument $txn $xd $uc $flags
	} else {
	    $container putDocument $xd $uc $flags
	}
    } ret

    delete xd
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # success depends upon uniqueness of index specification
    if {$unique} {
	dbxml_error_check_good $basename [string match "*Uniqueness*violation*" $ret] 1
    } else {
	dbxml_error_check_good $basename $ret ""
    }

    # explain what is happening
    puts "\t\t\t\t\t$basename.2: Adding a document with the same value twice"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    }
    wrap XmlDocument xd [$db createDocument]
    $xd setContent {<jim><foo>noggin</foo><foo>noggin</foo></jim>}
    $xd setName hello

    catch {
	if { $txnenv == 1 } {
	    $container putDocument $txn $xd $uc $flags
	} else {
	    $container putDocument $xd $uc $flags
	}
    } ret

    delete xd
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # success depends upon uniqueness of index specification
    if {$unique} {
	dbxml_error_check_good $basename [string match "*Uniqueness*violation*" $ret] 1
    } else {
	dbxml_error_check_good $basename $ret ""
    }

    delete uc
    delete container
    delete db
}

# indexing a container that contains documents - operations that should fail gracefully if they are illegal
proc xml010_1_4 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Indexing a non-empty container"
    xml010_1_4_1 $env $txnenv $basename.1 $oargs
}

proc xml010_1_4_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: Adding an index for a non-existent node"

    source ./include.tcl
    xml_cleanup $testdir $env

    # create database, context, container
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # add a document
    wrap XmlDocument xd [$db createDocument]
    $xd setContent {<foo>bar</foo>}
    $xd setName "foo"
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }

    # add an index for a node that does not exist in the document
    set uri {}
    set name cost
    set index node-element-equality-string
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn $uri $name $index $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex $uri $name $index $uc
	}
    } ret

    # expected behaviour is to succeed
    dbxml_error_check_good $basename $ret ""

    delete uc
    delete container
    delete db
}

proc xml010_1_5 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Finding zero targets"

    # prepare - create database, contexts, container
    source ./include.tcl
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

    # create and load document
    set content " \
<root> \
  <a>0</a> \
  <b></b> \
  <c attc='0'>ccc</c> \
  <d attd=''>ddd</d> \
</root> \
"
    wrap XmlDocument xd [$db createDocument]
    $xd setContent $content
    $xd setName "foo"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }

    # query the unindexed container - all queries should hit
    set id 0
    set cname "collection('[$container getName]')"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        puts "\t\t\t$basename.[incr id]: element with zero value (unindexed)"
        wrap XmlResults xr [$db query $txn "$cname//a\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  attribute with zero value (unindexed)"
        wrap XmlResults xr [$db query $txn "$cname//@attc\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]: empty element (unindexed)"
        wrap XmlResults xr [$db query $txn "$cname//b\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  empty attribute (unindexed)"
        wrap XmlResults xr [$db query $txn "$cname//@attd\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        $txn commit
        delete txn
    } else {
        puts "\t\t\t$basename.[incr id]:  element with zero value (unindexed)"
        wrap XmlResults xr [$db query "$cname//a\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  attribute with zero value (unindexed)"
        wrap XmlResults xr [$db query "$cname//@attc\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id.b [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]: empty element (unindexed)"
        wrap XmlResults xr [$db query "$cname//b\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  empty attribute (unindexed)"
        wrap XmlResults xr [$db query "$cname//@attd\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr
    }

    # add indexes on the zero and empty element
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        $container addIndex $txn "" "a" "node-element-equality-decimal" $uc

        $txn commit
        delete txn
    } else {
        $container addIndex "" "a" "node-element-equality-decimal" $uc
    }

    # query the indexed container - all queries should hit
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        puts "\t\t\t$basename.[incr id]:  element with zero value (indexed)"
        wrap XmlResults xr [$db query $txn "$cname//a\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  attribute with zero value (indexed)"
        wrap XmlResults xr [$db query $txn "$cname//@attc\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]: empty element (indexed)"
        wrap XmlResults xr [$db query $txn "$cname//b\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  empty attribute (indexed)"
        wrap XmlResults xr [$db query $txn "$cname//@attd\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        $txn commit
        delete txn
    } else {
        puts "\t\t\t$basename.[incr id]:  element with zero value (indexed)"
        wrap XmlResults xr [$db query "$cname//a\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  attribute with zero value (indexed)"
        wrap XmlResults xr [$db query "$cname//@attc\[.=0\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]: empty element (indexed)"
        wrap XmlResults xr [$db query "$cname//b\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr

        puts "\t\t\t$basename.[incr id]:  empty attribute (indexed)"
        wrap XmlResults xr [$db query "$cname//@attd\[.=''\]" $qc 0]
        dbxml_error_check_good $basename.$id [$xr size] 1
        delete xr
    }

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# look up methods on indexes and statistics
proc xml010_1_6 { {env "NULL"} {txnenv 0} basename oargs } {
    source ./include.tcl
    puts "\t\t$basename: Lookup methods on indexes and statistics"

    # execute all tests using both eager and lazy evaluation
    list set eval_types {}
    lappend eval_types "$XmlQueryContext_Eager $oargs"
    lappend eval_types "$XmlQueryContext_Lazy $oargs"

    set c 0
    foreach et $eval_types {
        # indexes and statistics - basic success cases
        xml010_1_6_1 $env $txnenv $basename.[incr c] [lindex $et 1] [lindex $et 0]

        # indexes - failure cases
        xml010_1_6_2 $env $txnenv $basename.[incr c] [lindex $et 1] [lindex $et 0]

        # statistics - failure cases
        xml010_1_6_3 $env $txnenv $basename.[incr c] [lindex $et 1] [lindex $et 0]

        # test XmlIndexLookup -- success cases
        xml010_1_6_4 $env $txnenv $basename.[incr c] [lindex $et 1] [lindex $et 0]

        # test dbxml:lookup-index() -- success cases
        xml010_1_6_5 $env $txnenv $basename.[incr c] [lindex $et 1] [lindex $et 0]
    }
}

# lookupIndex and lookupStatistics - basic success cases
proc xml010_1_6_1 { env txnenv basename oargs et } {
    source ./include.tcl

    # explain what is happening
    set msg "\t\t\t$basename: Basic success cases / "
    if { $et == $XmlQueryContext_Eager } {
        append msg "eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
        return
    }
    if { [expr $oargs & $DBXML_INDEX_NODES] } {
        set index_nodes 1
        append msg " / node indexes"
    } else {
        set index_nodes 0
        append msg " / document indexes"
    }
    puts $msg

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    $qc setEvaluationType $et

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # prepare - add documents and index the container
    set contentA "\
<docA> \
  <foo>hello</foo>
  <foo>charlie</foo>
  <foo>brown</foo>
</docA> \
"

    set contentB "\
<docB xmlns:bar='http://www.example.com/schema'> \
  <bar:foo>hello</bar:foo>
  <bar:foo>charlie</bar:foo>
</docB> \
"

    set contentC "\
<docC> \
  <foobar>
    <baz>goodbye</baz>
    <baz>charlie</baz>
    <baz>brown</baz>
  </foobar>
</docC> \
"

    set contentD "\
<docD xmlns:bar='http://www.example.com/schema'> \
  <bar:foobar>
    <bar:baz>goodbye</bar:baz>
    <bar:baz>charlie</bar:baz>
  </bar:foobar>
</docD> \
"

    wrap XmlDocument xdA [$db createDocument]
    $xdA setName "docA"
    $xdA setContent $contentA

    wrap XmlDocument xdB [$db createDocument]
    $xdB setName "docB"
    $xdB setContent $contentB

    wrap XmlDocument xdC [$db createDocument]
    $xdC setName "docC"
    $xdC setContent $contentC

    wrap XmlDocument xdD [$db createDocument]
    $xdD setName "docD"
    $xdD setContent $contentD

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        $container putDocument $txn $xdA $uc
        $container putDocument $txn $xdB $uc
        $container putDocument $txn $xdC $uc
        $container putDocument $txn $xdD $uc

        $container addIndex $txn "" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "" "foo" "node-element-presence" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex $txn "" "baz" "edge-element-equality-string" $uc
        $container addIndex $txn "http://www.example.com/schema" "baz" "edge-element-equality-string" $uc
        $container addIndex $txn "" "baz" "edge-element-presence" $uc
        $container addIndex $txn "http://www.example.com/schema" "baz" "edge-element-presence" $uc
        $container addIndex $txn "" "foobar" "edge-element-presence-none" $uc
        $container addIndex $txn "" "docC" "edge-element-presence-none" $uc

        $txn commit
        delete txn
    } else {

        $container putDocument $xdA $uc
        $container putDocument $xdB $uc
        $container putDocument $xdC $uc
        $container putDocument $xdD $uc

        $container addIndex "" "foo" "node-element-equality-string" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex "" "foo" "node-element-presence" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex "" "baz" "edge-element-equality-string" $uc
        $container addIndex "http://www.example.com/schema" "baz" "edge-element-equality-string" $uc
        $container addIndex "" "baz" "edge-element-presence" $uc
        $container addIndex "http://www.example.com/schema" "baz" "edge-element-presence" $uc
        $container addIndex "" "foobar" "edge-element-presence-none" $uc
        $container addIndex "" "docC" "edge-element-presence-none" $uc
    }
    delete xdA
    delete xdB
    delete xdC
    delete xdD

    set id 0
    set txn NULL

    #####################################################
    # node presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node presence"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    # without URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "foo" "node-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "foo" "node-element-presence"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "foo" "node-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics "" "foo" "node-element-presence"]
    }
    if { $index_nodes } {
        xml010_check_results $testid.a $xr 3 "docA" "foo"
        xml010_check_statistics $testid.a $xs 3 1
    } else {
        xml010_check_results $testid.a $xr 1 "docA"
        xml010_check_statistics $testid.a $xs 1 1
    }
    delete xr
    delete xs

    # with URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "http://www.example.com/schema" "foo" "node-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "http://www.example.com/schema" "foo" "node-element-presence"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "http://www.example.com/schema" "foo" "node-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics "http://www.example.com/schema" "foo" "node-element-presence"]
    }

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 2 "docB" "bar:foo"
        xml010_check_statistics $testid.b $xs 2 1
    } else {
        xml010_check_results $testid.a $xr 1 "docB"
        xml010_check_statistics $testid.b $xs 1 1
    }
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # node presence on equality index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node presence on equality index"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    # without URI
    new XmlValue xv
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "foo" "node-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $txn $qc "" "foo" "node-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "foo" "node-element-equality-string"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "foo" "node-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $qc "" "foo" "node-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics "" "foo" "node-element-equality-string"]
    }
    # equality indexes now (as of 2.2) return one entry for
    # each unique value for both node and document indexes
    if { $index_nodes } {
        xml010_check_results $testid.a $xr 3 "docA" "foo" "s"
        xml010_check_results $testid.a $xrr 3 "docA" "foo" "r"
    } else {
        xml010_check_results $testid.a $xr 3 "docA"
    }
    xml010_check_statistics $testid.a $xs 3 3
    delete xr
    delete xrr
    delete xs

    # with URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "http://www.example.com/schema" "foo" "node-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $txn $qc "http://www.example.com/schema" "foo" "node-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics $txn "http://www.example.com/schema" "foo" "node-element-equality-string"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "http://www.example.com/schema" "foo" "node-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $qc "http://www.example.com/schema" "foo" "node-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics "http://www.example.com/schema" "foo" "node-element-equality-string"]
    }

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 2 "docB" "bar:foo" "s"
        xml010_check_results $testid.a $xrr 2 "docB" "bar:foo" "r"
    } else {
        xml010_check_results $testid.a $xr 2 "docB"
    }
    xml010_check_statistics $testid.b $xs 2 2
    delete xr
    delete xrr
    delete xv
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # edge presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge presence"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    # without URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "baz" "" "foobar" "edge-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "baz" "" "foobar" "edge-element-presence"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "baz" "" "foobar" "edge-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics "" "baz" "" "foobar" "edge-element-presence"]
    }

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 3 "docC" "baz"
        xml010_check_statistics $testid.a $xs 3 1
    } else {
        xml010_check_results $testid.a $xr 1 "docC"
        xml010_check_statistics $testid.a $xs 1 1
    }
    delete xr
    delete xs

    # with URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-presence"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-presence"]
        wrap XmlStatistics xs [$container lookupStatistics "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-presence"]
    }

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 2 "docD" "bar:baz"
        xml010_check_statistics $testid.b $xs 2 1
    } else {
        xml010_check_results $testid.a $xr 1 "docD"
        xml010_check_statistics $testid.b $xs 1 1
    }
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # edge presence on equality index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge presence on equality index"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    new XmlValue xv
    # without URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "baz" "" "foobar" "edge-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $txn $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "baz" "" "foobar" "edge-element-equality-string"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "baz" "" "foobar" "edge-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics "" "baz" "" "foobar" "edge-element-equality-string"]
    }
    # equality indexes now (as of 2.2) return one entry for
    # each unique value for both node and document indexes
    if { $index_nodes } {
        xml010_check_results $testid.a $xr 3 "docC" "baz" "s"
        xml010_check_results $testid.a $xrr 3 "docC" "baz" "r"
    } else {
        xml010_check_results $testid.a $xr 3 "docC"
    }
    xml010_check_statistics $testid.a $xs 3 3
    delete xr
    delete xrr
    delete xs

    # with URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $txn $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics $txn "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string"]
        wrap XmlResults xrr [$container lookupIndex $qc "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string" $xv $DBXML_REVERSE_ORDER]
        wrap XmlStatistics xs [$container lookupStatistics "http://www.example.com/schema" "baz" "http://www.example.com/schema" "foobar" "edge-element-equality-string"]
    }

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 2 "docD" "bar:baz" "s"
        xml010_check_results $testid.a $xrr 2 "docD" "bar:baz" "r"
    } else {
        xml010_check_results $testid.a $xr 2 "docD"
    }
    xml010_check_statistics $testid.b $xs 2 2
    delete xr
    delete xrr
    delete xv
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # node equality
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node equality"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    # hit the target
    new XmlValue xv "hello"
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "foo" "node-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "foo" "node-element-equality-string" $xv]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "foo" "node-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics "" "foo" "node-element-equality-string" $xv]
    }
    delete xv
    xml010_check_results $testid.a $xr 1 "docA"
    xml010_check_statistics $testid.a $xs 1 1
    delete xr
    delete xs

    # miss the target
    #  - index will have 0 results, but stats estimates are unchanged
    new XmlValue xv "ooops"
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "foo" "node-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "foo" "node-element-equality-string" $xv]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "foo" "node-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics "" "foo" "node-element-equality-string" $xv]
    }
    delete xv

    xml010_check_results $testid.b $xr 0
    xml010_check_statistics $testid.b $xs 1 1
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # edge equality
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge equality"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    # hit the target
    new XmlValue xv "goodbye"
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "baz" "" "foobar" "edge-element-equality-string" $xv]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics "" "baz" "" "foobar" "edge-element-equality-string" $xv]
    }
    delete xv
    xml010_check_results $testid.a $xr 1
    xml010_check_statistics $testid.a $xs 1 1
    delete xr
    delete xs

    # miss the target
    #  - index will have 0 results, but stats estimates are unchanged
    new XmlValue xv "foo"
    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "baz" "" "foobar" "edge-element-equality-string" $xv]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "baz" "" "foobar" "edge-element-equality-string" $xv]
        wrap XmlStatistics xs [$container lookupStatistics "" "baz" "" "foobar" "edge-element-equality-string" $xv]
    }
    delete xv

    xml010_check_results $testid.b $xr 0
    xml010_check_statistics $testid.b $xs 1 1
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # dbxml:root
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: dbxml:root"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "" "docC" "http://www.sleepycat.com/2002/dbxml" "root" "edge-element-presence-none"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "" "docC" "http://www.sleepycat.com/2002/dbxml" "root" "edge-element-presence-none"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "" "docC" "http://www.sleepycat.com/2002/dbxml" "root" "edge-element-presence-none"]
        wrap XmlStatistics xs [$container lookupStatistics "" "docC" "http://www.sleepycat.com/2002/dbxml" "root" "edge-element-presence-none"]
    }
    xml010_check_results $testid $xr 1 "docC"
    xml010_check_statistics $testid $xs 1 1
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# lookupIndex - failure cases
proc xml010_1_6_2 { env txnenv basename oargs et } {
    source ./include.tcl

    # explain what is happening
    set msg "\t\t\t$basename: Index failure cases / "
    if { $et == $XmlQueryContext_Eager } {
        append msg "eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
        return
    }
    if { [expr $oargs & $DBXML_INDEX_NODES] } {
        append msg " / node indexes"
    } else {
        append msg " / document indexes"
    }
    puts $msg

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    $qc setEvaluationType $et

    set txn 0
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }
    set id 0

    # start with tests on an unindexed container

    #####################################################
    # look up an index that does not exist (should not complain)
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Unindexed container"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    if { $txnenv == 1 } {
        wrap XmlResults xr [$container lookupIndex $txn $qc "url" "node" "node-element-equality-string"]
        wrap XmlStatistics xs [$container lookupStatistics $txn "url" "node" "node-element-equality-string"]
    } else {
        wrap XmlResults xr [$container lookupIndex $qc "url" "node" "node-element-equality-string"]
        wrap XmlStatistics xs [$container lookupStatistics "url" "node" "node-element-equality-string"]
    }

    # should return zero hits
    xml010_check_results $testid $xr 0
    xml010_check_statistics $testid $xs 0 0
    delete xr
    delete xs

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    #####################################################
    # look up an index and statistics with an illegal index specfication
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Illegal index specification"
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction txn2 [$db createTransaction]
            wrap XmlResults xr [$container lookupIndex $txn2 $qc "url" "node" "node-element-substring-none"]
            $txn2 commit
            delete txn2
        } else {
            wrap XmlResults xr [$container lookupIndex $qc "url" "node" "node-element-substring-none"]
        }
    } ret
    # should have raised an exception
    dbxml_error_check_good $basename [string match "Error*index*" $ret] 1

    # now add documents and index the container
    set contentA " \
<docA> \
  <foo>hello</foo> \
</docA> \
"

    set contentB " \
<docB> \
  <foo> \
    <bar>goodbye</bar> \
  </foo> \
</docB> \
"

    set contentC " \
<docC> \
  <foobar> \
    <baz>hmmm</baz> \
  </foobar> \
</docC> \
"

    wrap XmlDocument xdA [$db createDocument]
    $xdA setName "docA"
    $xdA setContent $contentA
    
    wrap XmlDocument xdB [$db createDocument]
    $xdB setName "docB"
    $xdB setContent $contentB
    
    wrap XmlDocument xdC [$db createDocument]
    $xdC setName "docC"
    $xdC setContent $contentC
    
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
	
        $container putDocument $txn $xdA $uc
        $container putDocument $txn $xdB $uc
        $container putDocument $txn $xdC $uc

        $container addIndex $txn "" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "" "foo" "edge-element-equality-string" $uc
        $container addIndex $txn "" "bar" "node-element-presence-none" $uc
        $container addIndex $txn "" "bar" "edge-element-equality-string" $uc
        $container addIndex $txn "" "baz" "node-element-equality-string" $uc

        $txn commit
        delete txn
    } else {
        $container putDocument $xdA $uc
        $container putDocument $xdB $uc
        $container putDocument $xdC $uc

        $container addIndex "" "foo" "node-element-equality-string" $uc
        $container addIndex "" "foo" "edge-element-equality-string" $uc
        $container addIndex "" "bar" "node-element-presence-none" $uc
        $container addIndex "" "bar" "edge-element-equality-string" $uc
        $container addIndex "" "baz" "node-element-equality-string" $uc
    }
    delete xdA
    delete xdB
    delete xdC

    #####################################################
    # equality look up with the wrong type of value
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Value of wrong type"
    catch {
        new XmlValue xv $XmlValue_BOOLEAN "true"
        if { $txnenv == 1 } {
            wrap XmlTransaction txn4 [$db createTransaction]
            wrap XmlResults xr [$container lookupIndex $txn4 $qc "" "foo" "node-element-equality-string" $xv]
            $txn4 commit
            delete txn4
        } else {
            wrap XmlResults xr [$container lookupIndex $qc "" "foo" "node-element-equality-string" $xv]
        }
    } ret
    # should have raised an exception
    dbxml_error_check_good $basename [string match "Error*type*" $ret] 1

    #####################################################
    # equality look-up on a presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Equality lookup on presence"
    catch {
        new XmlValue xv2 "goodbye"
        if { $txnenv == 1 } {
            wrap XmlTransaction txn [$db createTransaction]
            wrap XmlResults xr [$container lookupIndex $txn $qc "" "bar" "node-element-presence-none" $xv2]
            $txn commit
            delete txn
        } else {
            wrap XmlResults xr [$container lookupIndex $qc "" "bar" "node-element-presence-none" $xv2]
        }
    } ret
    # should have raised an exception
    dbxml_error_check_good $basename [string match "Error*type*" $ret] 1

    #####################################################
    # edge lookup on a node index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge lookup on a node index"

    if { $txnenv == 1 } {
        wrap XmlTransaction txn3 [$db createTransaction]
        wrap XmlResults xr3 [$container lookupIndex $txn3 $qc "" "baz" "" "foobar" "node-element-equality-string"]
        xml010_check_results $testid $xr3 0
        delete xr3
        $txn3 commit
        delete txn3
    } else {
        wrap XmlResults xr3 [$container lookupIndex $qc "" "baz" "" "foobar" "node-element-equality-string"]
        xml010_check_results $testid $xr3 0
        delete xr3
    }

    #####################################################
    # node equality on edge equality
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node equality on edge equality"

    new XmlValue xv4 "hello"
    if { $txnenv == 1 } {
        wrap XmlResults xr4 [$container lookupIndex $txn $qc "" "foo" "edge-element-equality-string" $xv4]
    } else {
        wrap XmlResults xr4 [$container lookupIndex $qc "" "foo" "edge-element-equality-string" $xv4]
    }
    delete xv4

    # should have returned zero results
    xml010_check_results $testid $xr4 0
    delete xr4

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# lookupStatistics - failure cases
#  2004-09-10 arw these tests are not grouped with lookup index failures
#  since I cannot figure out how Tcl should cope with memory etc. after
#  an exception is thrown
proc xml010_1_6_3 { env txnenv basename oargs et } {
    source ./include.tcl

    # explain what is happening
    set msg "\t\t\t$basename: Stats failure cases / "
    if { $et == $XmlQueryContext_Eager } {
        append msg "eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
        return
    }
    if { [expr $oargs & $DBXML_INDEX_NODES] } {
        append msg " / node indexes"
    } else {
        append msg " / document indexes"
    }
    puts $msg

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    $qc setEvaluationType $et

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }
    set id 0
    set txn 0

    # start with tests on an unindexed container

    #####################################################
    # look up statistics with an illegal index specfication
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Illegal index specification"
    catch {
        if { $txnenv == 1 } {
            wrap XmlTransaction txn [$db createTransaction]
            wrap XmlStatistics xs [$container lookupStatistics $txn "url" "node" "node-element-substring-none"]
            $txn commit
            delete txn
        } else {
            wrap XmlStatistics xs [$container lookupStatistics "url" "node" "node-element-substring-none"]
        }
    } ret

    # should have raised an exception
    dbxml_error_check_good $basename [string match "Error*index*" $ret] 1

    # now add a document and index the container
    wrap XmlDocument xd [$db createDocument]
    set content " \
<root> \
  <foo>hello</foo> \
</root> \
"
    $xd setContent $content
    $xd setName "foo"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn2 [$db createTransaction]

        $container putDocument $txn2 $xd $uc
        $container addIndex $txn2 "" "foo" "node-element-presence-none" $uc

        $txn2 commit
        delete txn2
    } else {
        $container putDocument $xd $uc
        $container addIndex "" "foo" "node-element-presence-none" $uc
    }

    #####################################################
    # equality look up with the wrong type of value
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Value of wrong type"

    catch {
        new XmlValue xv $XmlValue_BOOLEAN "true"
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$db createTransaction]
            wrap XmlStatistics xs [$container lookupStatistics $xtxn "" "foo" "node-element-presence-none" $xv]
            $xtxn commit
            delete xtxn
        } else {
            wrap XmlStatistics xs [$container lookupStatistics "" "foo" "node-element-presence-none" $xv]
        }
    } ret
    # should have raised an exception
    dbxml_error_check_good $basename [string match "Error*type*" $ret] 1

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# XmlIndexLookup success cases
proc xml010_1_6_4 { env txnenv basename oargs et } {
    source ./include.tcl
    # explain what is happening
    set msg "\t\t\t$basename: XmlIndexLookup simple cases / "
    if { $et == $XmlQueryContext_Eager } {
        append msg "eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
        return
    }
    if { [expr $oargs & $DBXML_INDEX_NODES] } {
        set index_nodes 1
        append msg " / node indexes"
    } else {
        set index_nodes 0
        append msg " / document indexes"
    }
    puts $msg
    
    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    $qc setEvaluationType $et
    
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }
    
    # prepare - add documents and index the container
    set contentA "\
<docA> \
  <foo>hello</foo>
  <foo>charlie</foo>
  <foo>brown</foo>
  <foo>aNd</foo>
  <foo>Lucy</foo>
</docA> \
"
    set contentB "\
<docB xmlns:bar='http://www.example.com/schema'> \
  <bar:foo>hello</bar:foo>
  <bar:foo>charlie</bar:foo>
  <bar:foo>brown</bar:foo>
  <bar:foo>aNd</bar:foo>
  <bar:foo>Lucy</bar:foo>
</docB> \
"
    
    set contentC "\
<docC> \
  <foobar>
    <baz len='6.7'>tall guy</baz>
    <baz len='75'>30 yds</baz>
    <baz len='75'>30 yds</baz>
    <baz len='5.0'>five feeet</baz>
    <baz len='0.2'>point two</baz>
    <baz len='60.2'>five feet</baz>
  </foobar>
</docC> \
"
    set contentD "\
<docD> \
 <dates1>
  <date>2005-08-02</date>
  <date>2003-06-12</date>
  <date>1005-12-12</date>
 </dates1>
 <dates2>
  <date>1492-05-30</date>
  <date>2000-01-01</date>
  <date>1984-12-25</date>
 </dates2>
</docD> \
"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
	
        $container addIndex $txn "" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "" "foo" "node-element-presence" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex $txn "" "len" "edge-attribute-equality-decimal" $uc
        $container addIndex $txn "" "len" "edge-attribute-presence" $uc
        $container addIndex $txn "" "date" "edge-element-equality-date" $uc
	
        $container putDocument $txn "docA" $contentA $uc
        $container putDocument $txn "docB" $contentB $uc
        $container putDocument $txn "docC" $contentC $uc
        $container putDocument $txn "docD" $contentD $uc
	
        $txn commit
        delete txn
    } else {
        $container addIndex "" "foo" "node-element-equality-string" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex "" "foo" "node-element-presence" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex "" "len" "edge-attribute-equality-decimal" $uc
        $container addIndex "" "len" "edge-attribute-presence" $uc
        $container addIndex "" "date" "edge-element-equality-date" $uc
	
        $container putDocument "docA" $contentA $uc
        $container putDocument "docB" $contentB $uc
        $container putDocument "docC" $contentC $uc
        $container putDocument "docD" $contentD $uc
	
    }
    set id 0
    set txn NULL
    
    #####################################################
    # node presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node presence"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "foo" "node-element-presence"]
    
    # with and without URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$il execute $txn $qc]
	$il setNode "http://www.example.com/schema" [$il getNodeName]
        wrap XmlResults xru [$il execute $txn $qc]
    } else {
        wrap XmlResults xr [$il execute $qc]
	$il setNode "http://www.example.com/schema" [$il getNodeName]
        wrap XmlResults xru [$il execute $qc]
    }
    if { $index_nodes } {
        xml010_check_results $testid.a $xr 5 "docA" "foo"
        xml010_check_results $testid.a $xru 5 "docB" "bar:foo"
    } else {
        xml010_check_results $testid.a $xr 1 "docA"
        xml010_check_results $testid.a $xru 1 "docB"
    }
    # trivial test of getEvaluationType method on XmlResults
    dbxml_error_check_good "$basename - eager results" [$xr getEvaluationType]  $et
    dbxml_error_check_good "$basename - eager results" [$xru getEvaluationType]  $et
    delete xr
    delete xru
    delete il
    
    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
    
    
    #####################################################
    # Inequality lookup on string equality index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Inequality lookup on string equality index"
    
    new XmlValue xv "charlie"
    
    list set op_results {}
    lappend op_results "$XmlIndexLookup_LT 3"
    lappend op_results "$XmlIndexLookup_LTE 4"
    lappend op_results "$XmlIndexLookup_GT 1"
    lappend op_results "$XmlIndexLookup_GTE 2"
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "foo" "node-element-equality-string"]
    
    foreach opres $op_results {
	# set the operation and value for the lookup
	$il setLowBound $xv [lindex $opres 0]
	# reset name to no URI
	$il setNode "" [$il getNodeName]
	
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	}
	if { $txnenv == 1 } {
	    wrap XmlResults xr [$il execute $txn $qc]
	    wrap XmlResults xrr [$il execute $txn $qc $DBXML_REVERSE_ORDER]
	    $il setNode "http://www.example.com/schema" [$il getNodeName]
	    wrap XmlResults xru [$il execute $txn $qc]
	    wrap XmlResults xrru [$il execute $txn $qc $DBXML_REVERSE_ORDER]
	} else {
	    wrap XmlResults xr [$il execute $qc]
	    wrap XmlResults xrr [$il execute $qc $DBXML_REVERSE_ORDER]
	    $il setNode "http://www.example.com/schema" [$il getNodeName]
	    wrap XmlResults xru [$il execute $qc]
	    wrap XmlResults xrru [$il execute $qc $DBXML_REVERSE_ORDER]
	}
	if { $index_nodes } {
	    xml010_check_results $testid.a $xr [lindex $opres 1] "docA" "foo" "s"
	    xml010_check_results $testid.b $xrr [lindex $opres 1] "docA" "foo" "r"
	    xml010_check_results $testid.c $xru [lindex $opres 1] "docB" "bar:foo" "s"
	    xml010_check_results $testid.d $xrru [lindex $opres 1] "docB" "bar:foo" "r"
	} else {
	    xml010_check_results $testid.a $xr [lindex $opres 1] "docA"
	    xml010_check_results $testid.b $xrr [lindex $opres 1] "docA"
	    xml010_check_results $testid.c $xru [lindex $opres 1] "docB"
	    xml010_check_results $testid.d $xrru [lindex $opres 1] "docB"
	}
	delete xr
	delete xrr
	delete xru
	delete xrru
	if { $txnenv == 1 } {
	    $txn commit
	    delete txn
	}
    }
    delete il
    delete xv

    #####################################################
    # edge presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge presence"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "len" "edge-attribute-presence"]
    
    # with and without URI
    if { $txnenv == 1 } {
        wrap XmlResults xr [$il execute $txn $qc]
    } else {
        wrap XmlResults xr [$il execute $qc]
    }
    if { $index_nodes } {
        xml010_check_results $testid.a $xr 6 "docC" "len"
    } else {
        xml010_check_results $testid.a $xr 1 "docC"
    }
    delete xr
    delete il
    
    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
    
    #####################################################
    # Inequality lookup on decimal/attribute equality index
    # this is an edge index, so the parent must be included
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Inequality lookup on decimal equality index"
    
    new XmlValue xv $XmlValue_DECIMAL "40"
    
    unset op_results
    list set op_results {}
    lappend op_results "$XmlIndexLookup_LT 3 3"
    lappend op_results "$XmlIndexLookup_LTE 3 3"
    lappend op_results "$XmlIndexLookup_GT 3 2"
    lappend op_results "$XmlIndexLookup_GTE 3 2"
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "len" "edge-attribute-equality-decimal" $xv]
    $il setParent "" "baz"
    
    foreach opres $op_results {
	set dnum  [lindex $opres 2]
	set op [lindex $opres 0]
	set nnum  [lindex $opres 1]
	# set the operation and value for the lookup
	$il setLowBound $xv $op
	
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	}
	if { $txnenv == 1 } {
	    wrap XmlResults xr [$il execute $txn $qc]
	    wrap XmlResults xrr [$il execute $txn $qc $DBXML_REVERSE_ORDER]
	} else {
	    wrap XmlResults xr [$il execute $qc]
	    wrap XmlResults xrr [$il execute $qc $DBXML_REVERSE_ORDER]
	}
	if { $index_nodes } {
	    # don't use sort yet because there's no way to compare non-string
	    # values.  Consider adding XmlValue methods...
	    #	    xml010_check_results $testid.a $xr [lindex $opres 1] "docC" "len" "s"
	    #	    xml010_check_results $testid.b $xrr [lindex $opres 1] "docC" "len" "r"
	    xml010_check_results $testid.a $xr $nnum "docC" "len"
	    xml010_check_results $testid.b $xrr $nnum "docC" "len"
	} else {
	    xml010_check_results $testid.a $xr $dnum "docC"
	    xml010_check_results $testid.b $xrr $dnum "docC"
	}
	delete xr
	delete xrr
	if { $txnenv == 1 } {
	    $txn commit
	    delete txn
	}
    }
    delete il
    delete xv

    #####################################################
    # Equality lookup on date/edge index
    # this is an edge index, so the parent must be included
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Equality lookup on date/edge equality index"
    
    new XmlValue xv $XmlValue_DATE "2003-06-12"
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "date" "edge-element-equality-date" $xv $XmlIndexLookup_EQ]
    $il setParent "" "dates1"
    
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    }
    if { $txnenv == 1 } {
	wrap XmlResults xr [$il execute $txn $qc]
	$il setParent "" "dates2"
	wrap XmlResults xrr [$il execute $txn $qc $DBXML_REVERSE_ORDER]
    } else {
	wrap XmlResults xr [$il execute $qc]
	$il setParent "" "dates2"
	wrap XmlResults xrr [$il execute $qc $DBXML_REVERSE_ORDER]
    }
    if { $index_nodes } {
	xml010_check_results $testid.a $xr 1 "docD" "date"
	xml010_check_results $testid.b $xrr 0
    } else {
	xml010_check_results $testid.a $xr 1 "docD"
	xml010_check_results $testid.b $xrr 0
    }
    delete xr
    delete xrr
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }
    delete il
    delete xv

    #####################################################
    # Some error conditions
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Error test: type mismatch"
    
    new XmlValue xvdate $XmlValue_DATE "2003-06-12"
    new XmlValue xvd $XmlValue_DECIMAL "2003"
    
    wrap XmlIndexLookup il [$db createIndexLookup $container "" "date" "edge-element-equality-date" $xvd $XmlIndexLookup_EQ]
    $il setParent "" "dates1"
    
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    }
    catch {
	if { $txnenv == 1 } {
	    wrap XmlResults xr [$il execute $txn $qc]
	} else {
	    wrap XmlResults xr [$il execute $qc]
	}
    } ret
    set res [string match "*match*syntax*" $ret]
    dbxml_error_check_good $testid $res 1

    #Invalid arguments -- bad operations
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Error test: bad operation arguments to XmlIndexLookup"
    $il setLowBound $xvdate $XmlIndexLookup_LT
    $il setHighBound $xvdate $XmlIndexLookup_GT
    catch {
	if { $txnenv == 1 } {
	    wrap XmlResults xr [$il execute $txn $qc]
	} else {
	    wrap XmlResults xr [$il execute $qc]
	}
    } ret
    set res [string match "*invalid*range*" $ret]
    dbxml_error_check_good $testid $res 1

    #Invalid arguments -- mismatch of range types
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Error test: mismatched types for range"
    $il setLowBound $xvdate $XmlIndexLookup_GT
    $il setHighBound $xvd $XmlIndexLookup_LT
    catch {
	if { $txnenv == 1 } {
	    wrap XmlResults xr [$il execute $txn $qc]
	} else {
	    wrap XmlResults xr [$il execute $qc]
	}
    } ret
    set res [string match "*types*same*" $ret]
    dbxml_error_check_good $testid $res 1

    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }
    delete il
    delete xvdate
    delete xvd

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# dbxml:lookup-index() success cases
proc xml010_1_6_5 { env txnenv basename oargs et } {
    source ./include.tcl
    # explain what is happening
    set msg "\t\t\t$basename: dbxml:lookup-index() simple cases / "
    if { $et == $XmlQueryContext_Eager } {
        append msg "eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
        return
    }
    if { [expr $oargs & $DBXML_INDEX_NODES] } {
        set index_nodes 1
        append msg " / node indexes"
    } else {
        set index_nodes 0
        append msg " / document indexes"
    }
    puts $msg
    
    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]
    $qc setEvaluationType $et
    
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }
    
    # prepare - add documents and index the container
    set contentA "\
<docA> \
  <foo>hello</foo>
  <foo>charlie</foo>
  <foo>brown</foo>
  <foo>aNd</foo>
  <foo>Lucy</foo>
</docA> \
"
    set contentB "\
<docB xmlns:bar='http://www.example.com/schema'> \
  <bar:foo>hello</bar:foo>
  <bar:foo>charlie</bar:foo>
  <bar:foo>brown</bar:foo>
  <bar:foo>aNd</bar:foo>
  <bar:foo>Lucy</bar:foo>
</docB> \
"
    
    set contentC "\
<docC> \
  <foobar>
    <baz len='6.7'>tall guy</baz>
    <baz len='75'>30 yds</baz>
    <baz len='75'>30 yds</baz>
    <baz len='5.0'>five feeet</baz>
    <baz len='0.2'>point two</baz>
    <baz len='60.2'>five feet</baz>
  </foobar>
</docC> \
"
    set contentD "\
<docD> \
 <dates1>
  <date>2005-08-02</date>
  <date>2003-06-12</date>
  <date>1005-12-12</date>
 </dates1>
 <dates2>
  <date>1492-05-30</date>
  <date>2000-01-01</date>
  <date>1984-12-25</date>
 </dates2>
</docD> \
"
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
	
        $container addIndex $txn "" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex $txn "" "foo" "node-element-presence" $uc
        $container addIndex $txn "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex $txn "" "len" "edge-attribute-equality-double" $uc
        $container addIndex $txn "" "len" "edge-attribute-presence" $uc
        $container addIndex $txn "" "date" "edge-element-equality-date" $uc
	
        $container putDocument $txn "docA" $contentA $uc
        $container putDocument $txn "docB" $contentB $uc
        $container putDocument $txn "docC" $contentC $uc
        $container putDocument $txn "docD" $contentD $uc
	
        $txn commit
        delete txn
    } else {
        $container addIndex "" "foo" "node-element-equality-string" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-equality-string" $uc
        $container addIndex "" "foo" "node-element-presence" $uc
        $container addIndex "http://www.example.com/schema" "foo" "node-element-presence" $uc
        $container addIndex "" "len" "edge-attribute-equality-double" $uc
        $container addIndex "" "len" "edge-attribute-presence" $uc
        $container addIndex "" "date" "edge-element-equality-date" $uc
	
        $container putDocument "docA" $contentA $uc
        $container putDocument "docB" $contentB $uc
        $container putDocument "docC" $contentC $uc
        $container putDocument "docD" $contentD $uc
	
    }
    set id 0
    set txn NULL
    
    #####################################################
    # node presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Node presence"

    set query "dbxml:lookup-index('[$container getName]', 'foo')"
    set query2 "declare namespace bar='http://www.example.com/schema';"
    set query2 "$query2 dbxml:lookup-index('[$container getName]', 'bar:foo')"

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }
    
    # with and without URI
    wrap XmlResults xr [$db query $txn $query $qc 0]
    wrap XmlResults xru [$db query $txn $query2 $qc 0]

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 5 "docA" "foo"
        xml010_check_results $testid.a $xru 5 "docB" "bar:foo"
    } else {
        xml010_check_results $testid.a $xr 1 "docA"
        xml010_check_results $testid.a $xru 1 "docB"
    }

    # trivial test of getEvaluationType method on XmlResults
    dbxml_error_check_good "$basename - eager results" [$xr getEvaluationType]  $et
    dbxml_error_check_good "$basename - eager results" [$xru getEvaluationType]  $et
    delete xr
    delete xru
    
    if { $txnenv == 1 } {
        $txn commit
    }
    delete txn

    
    #####################################################
    # Inequality lookup on string equality index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Inequality lookup on string equality index"
    
    set query "dbxml:lookup-index('[$container getName]', 'foo')"
    set query2 "declare namespace bar='http://www.example.com/schema';"
    set query2 "$query2 dbxml:lookup-index('[$container getName]', 'bar:foo')"

    list set op_results {}
    lappend op_results "< 3"
    lappend op_results "<= 4"
    lappend op_results "> 1"
    lappend op_results ">= 2"
    
    foreach opres $op_results {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	} else {
            new XmlTransaction txn
        }

        set op [lindex $opres 0]

       if { $index_nodes } {
            wrap XmlResults xr [$db query $txn "$query\[. $op 'charlie']" $qc 0]
            wrap XmlResults xru [$db query $txn "$query2\[. $op 'charlie']" $qc 0]
            xml010_check_results $testid.a $xr [lindex $opres 1] "docA" "foo"
            xml010_check_results $testid.c $xru [lindex $opres 1] "docB" "bar:foo"
       } else {
            wrap XmlResults xr [$db query $txn "$query" $qc 0]
            wrap XmlResults xru [$db query $txn "$query2" $qc 0]
           xml010_check_results $testid.a $xr 1 "docA"
           xml010_check_results $testid.c $xru 1 "docB"
       }

	delete xr
	delete xru
	if { $txnenv == 1 } {
	    $txn commit
	}
        delete txn
    }

    #####################################################
    # edge presence
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Edge presence"

    set query "dbxml:lookup-attribute-index('[$container getName]', 'len')"

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }
    
    wrap XmlResults xr [$db query $txn $query $qc 0]

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 6 "docC" "len"
    } else {
        xml010_check_results $testid.a $xr 1 "docC"
    }

    delete xr
    
    if { $txnenv == 1 } {
        $txn commit
    }
    delete txn
    
    #####################################################
    # Inequality lookup on decimal/attribute equality index
    # this is an edge index, so the parent must be included
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Inequality lookup on double equality index"
    
    set query "dbxml:lookup-attribute-index('[$container getName]', 'len', 'baz')"

    unset op_results
    list set op_results {}
    lappend op_results "< 3 1"
    lappend op_results "<= 3 1"
    lappend op_results "> 3 1"
    lappend op_results ">= 3 1"
    
    foreach opres $op_results {
	set op [lindex $opres 0]
	set nnum  [lindex $opres 1]
	set dnum  [lindex $opres 2]

	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
        } else {
            new XmlTransaction txn
	}

       if { $index_nodes } {
            wrap XmlResults xr [$db query $txn "$query\[. $op 40]" $qc 0]
            wrap XmlResults xr2 [$db query $txn "$query\[xs:decimal(.) $op 40]" $qc 0]
            xml010_check_results $testid.a $xr $nnum "docC" "len"
            delete xr2
            delete xr
       } else {
            wrap XmlResults xr [$db query $txn "$query" $qc 0]
           xml010_check_results $testid.a $xr $dnum "docC"
            delete xr
       }

	if { $txnenv == 1 } {
	    $txn commit
	}
        delete txn
    }

    #####################################################
    # Equality lookup on date/edge index
    # this is an edge index, so the parent must be included
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Equality lookup on date/edge equality index"
    
    set query "dbxml:lookup-index('[$container getName]', 'date', 'dates1')"
    set query2 "dbxml:lookup-index('[$container getName]', 'date', 'dates2')"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }

    if { $index_nodes } {
        wrap XmlResults xr [$db query $txn "$query\[. = xs:date('2003-06-12')]" $qc 0]
        wrap XmlResults xrr [$db query $txn "$query2\[. = xs:date('2003-06-12')]" $qc 0]
        xml010_check_results $testid.a $xr 1 "docD" "date"
        xml010_check_results $testid.b $xrr 0
    } else {
        wrap XmlResults xr [$db query $txn $query $qc 0]
        wrap XmlResults xrr [$db query $txn $query2 $qc 0]
       xml010_check_results $testid.a $xr 1 "docD"
       xml010_check_results $testid.b $xrr 1
    }

    delete xr
    delete xrr
    if { $txnenv == 1 } {
	$txn commit
    }
    delete txn

    #####################################################
    # Presence lookup on a metadata index
    set testid $basename.[incr id]
    puts "\t\t\t\t$testid: Presence lookup on a metadata index"
    
    set query "dbxml:lookup-metadata-index('[$container getName]', 'dbxml:name')"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
    } else {
        new XmlTransaction txn
    }

    wrap XmlResults xr [$db query $txn $query $qc 0]

    if { $index_nodes } {
        xml010_check_results $testid.a $xr 4
    } else {
       xml010_check_results $testid.a $xr 4
    }

    delete xr
    if { $txnenv == 1 } {
	$txn commit
    }
    delete txn

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

proc xml010_1_7 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Default index test (former bug)"

    set id 0

    # prepare - create database, contexts, container
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    $qc setDefaultCollection $basename-foo.dbxml

    wrap XmlContainer container [$mgr createContainer $basename-foo.dbxml $oargs $global_container_type]

    # Try to add and then delete an index on an empty container
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Create default and non-default indexes of same type"

    # add index and default index
    $container addIndex "" "a" "node-element-presence" $uc
    $container addDefaultIndex "node-element-presence" $uc

    # add some documents
    $container putDocument "doc" "<a id='1'/>" $uc $DBXML_GEN_NAME
    $container putDocument "doc" "<a id='2'/>" $uc $DBXML_GEN_NAME
    $container putDocument "doc" "<a id='3'/>" $uc $DBXML_GEN_NAME

    # ensure that a query returns results
    wrap XmlResults res [$mgr query "collection()//a" $qc]
    dbxml_error_check_good "$testid result size" [$res size] 3
    delete res

    # delete default index and query again, ensuring results
    $container deleteDefaultIndex "node-element-presence" $uc

    wrap XmlResults res [$mgr query "collection()//a" $qc]
    dbxml_error_check_good "$testid result size" [$res size] 3

    # clean up
    delete res
    delete container
    delete uc
    delete qc
    delete mgr
}

proc xml010_1_8 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: Former index specification bugs"

    set id 0

    # prepare - create database, contexts, container
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $basename-foo.dbxml $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $basename-foo.dbxml $oargs $global_container_type]
    }

    # Try to add and then delete an index on an empty container
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Add and then delete an index on an empty container"

    # add index
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        $container addIndex $txn "" "node" "node-element-presence" $uc

        $txn commit
        delete txn
    } else {
        $container addIndex "" "node" "node-element-presence" $uc
    }

    # delete index
    catch {
      if { $txnenv == 1 } {
          wrap XmlTransaction txn [$db createTransaction]

          $container deleteIndex $txn "" "node" "node-element-presence" $uc

          $txn commit
          delete txn
      } else {
          $container deleteIndex "" "node" "node-element-presence" $uc
      }
    } ret

    dbxml_error_check_good "$testid - no exceptions" $ret ""

    # clean up
    delete container
    delete uc
    delete qc
    delete db
}

# check results by verifying count, a document name
proc xml010_check_results { testid results count { doc_name "" } { node_name "" } {checkSort ""} } {
    set c 0
    set err 0
    set found_named_doc 0
    set lastValue ""
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
            if {[string compare $node_name ""]} {
                dbxml_error_check_good "$testid - node name" [$xv getNodeName] $node_name
            }
	    if { $checkSort != "" } {
		set curValue [$xv asString]		
		if { $c > 1 } {
		    set res "ge"
		    set sortRes [string compare $lastValue $curValue]
		    if { $sortRes < 0 } {
			set res "le"
		    } 
		    if { $sortRes == 0 } {
			if { $checkSort != "r" } {
			    set res "le"
			}
		    }
		    if  { $checkSort == "r" } {
			dbxml_error_check_good "$testid - rev sort" $res "ge"
		    } else {
			dbxml_error_check_good "$testid - sort" $res "le"
		    }
		}
		set lastValue $curValue

	    }
            #puts ">>>>>>>>>\n[$xv asString]\n<<<<<<<<\n"
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
proc xml010_check_statistics { testid statistics indexed unique } {
    # allow for decimals...
    dbxml_error_check_good "$testid - indexed keys" [$statistics getNumberOfIndexedKeys] $indexed.0
    dbxml_error_check_good "$testid - unique keys" [$statistics getNumberOfUniqueKeys] $unique.0

    #puts "Check stats for $testid:\nIndexed keys = [$statistics getNumberOfIndexedKeys], unique keys = [$statistics getNumberOfUniqueKeys], key size = [$statistics getSumKeyValueSize]"
}

# Container Indexing (Restricted)
#
# The tests are now restricted to comparing the results for an indexed and
# unindexed container
#
# If the "regress" flag is set then an external program is used to dump
# the query plans to disk.
#
# The new container indexing tests are implemented in group 10.4

proc xml010_2_and_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs {regress 0} } {
    source ./include.tcl

    # documents apply to the entire test suite
    set documents {
	document_set_10_2/document_10_2_match.xml
	document_set_10_2/document_10_2_not_match_1.xml
	document_set_10_2/document_10_2_not_match_2.xml
    }

    # basename ignored
    set dir "$test_path/data_set_10"
    set exclude 0
    container_indexing_tests "10_2" "10.2" $dir $documents 0 $exclude $env $txnenv $oargs $regress
    container_indexing_tests "10_3" "10.3" $dir $documents 0 $exclude $env $txnenv $oargs $regress
}

# Drives one or more test suites based on a glob of files in a directory
#
# All files of the form "data_set_<file_match_chars>*.txt" in <dir> are processed.
#
# Also called by 12.3 (same tests, but with the "match" document deleted)
#
proc container_indexing_tests {file_match_chars basename dir documents delete exclude env txnenv oargs regress} {
    set ret [catch { glob $dir/data_set_$file_match_chars*.txt } result]
    if { $ret == 0 } {
	foreach file [lsort -dictionary $result] {
	    xml010_run $file $file_match_chars $basename $documents $delete $exclude $env $txnenv $oargs $regress
	}
    }
}

# Executes a group of tests
#
# Tests take the form of a container query using result documents. The
# container contains a number of documents, of which one will provide a match.
# Each test specifies an index and a query. The index is added to the container
# and the query is executed. The query plan, execution order and results are
# verified. The index is then removed from the container.
#
# The query is repeated on an unindexed container and the results are verified
# to be the same as those from the first query.
#
# The input data is read from a text file and written to the global array
# 'test_data'. See the "getIndexingTestData" procedure for a description of the file
# format and the 'test_data' array.
#
# 2004/07/05 ARW Tests restricted to comparing the results for indexed and
#                unindexed containers. This is because the query plans can
#                no longer be verifed using the mechanisms deployed in this
#                test suite.
#
#                If this method is called with the "regress" flag then
#                an external program is used to dump query plan information
#                to disk (for manual inspection).

proc xml010_run {file file_match_chars basename documents delete exclude {env "NULL"} {txnenv 0} oargs regress} {
    source ./include.tcl

    # refresh test environment
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # get data for this group of tests (written to global array 'test_data')
    global test_data
    array set test_data {}

    set id {}
    set title {}
    set test_count [getIndexingTestData $file $basename $exclude id title]

    # test header

    # 2004/06/25 ARW restrict tests to comparing results for [un]indexed
    #  containers (query plans verified in once-only regression method)
    if { ! $regress} {
	puts "\n\t$id: $title ($test_count tests)"
	puts "\tNOTE: tests restricted to comparing \[un\]indexed results"
	puts "\t(output supressed)"
    }

    # create two containers - one of which will be unindexed
    regsub -all "\[ \(\)\]" $id _ name

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn "$name.dbxml" $oargs]
	wrap XmlContainer container_unindexed [$db createContainer $xtxn "$name.unindexed.dbxml" $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db createContainer "$name.dbxml" $oargs]
	wrap XmlContainer container_unindexed [$db createContainer "$name.unindexed.dbxml" $oargs]
    }

    # add the test documents to each of the containers
    set txn "NULL"
    if {$txnenv == 1} {
	wrap XmlTransaction txn [$db createTransaction]
    }
    foreach file $documents {
	set ok [putFileInContainer $db $container "$test_path/$file" $txn 1]

	if { $ok != 0 } {
	    set ok [putFileInContainer $db $container_unindexed "$test_path/$file" $txn 1]
	}

	dbxml_error_check_bad "$basename: Error putting $file in container" $ok 0
    }
    if {$txnenv == 1} {
	$txn commit
	delete txn
    }

    set match_name document_${file_match_chars}_match.xml

    # create query context
    wrap XmlQueryContext context [$db createQueryContext $XmlQueryContext_LiveValues $XmlQueryContext_Eager]
    $context setNamespace "xs" "http://www.w3.org/2001/XMLSchema"

    # set the test information for the regression option (name of, and args for,
    # the external program)
    set regress_args {}
    set regress_program {}
    if { $regress} {
	# parse the index file to get program name, additional args (ignore test)
	set f [file join $test_path document_set_regression indexer index.xml]
	set dummy {}
	set program {}
	set args {}
	set ret [parse_index_file $f dummy program args]
	dbxml_error_check_good "Could not execute indexer regression tests - \"$f\" was not recognized as a valid test driver" $ret 1

	set regress_program $program

	# basic arguments - test materials are in the same directory as the index
	set d [file dirname $f]
	set p [file normalize $d]
	set args [concat $args "--datadir $p"]

	# basic arguments - DB XML environment
	set args [concat $args "--env $testdir"]

	# arguments - switch on transactioned environment
	if {$txnenv == 1} {
	    set args [concat $args " --transacted"]
	}

	# arguments - switch on NLS
	# arguments - switch on secure mode?

	# additional arguments - outputs directory
	set d [file join regression_results indexer]
	set d [file normalize $d]
	set args [concat $args "--outdir $d"]
	if { ! [file exists $d] } {
	    file mkdir $d
	}

	set regress_args $args
    }

    # iterate over the individual test specifications
    foreach key [lsort -dictionary [array names test_data -regexp "^$key_groupid" ] ] {
	# value is a list: id, operation, (index spec), query, oqp, eqp, stats
	set testid     [lindex $test_data($key) 0]
	set operation  [lindex $test_data($key) 1]
	set uri        [lindex $test_data($key) 2]
	set node       [lindex $test_data($key) 3]
	set index      [lindex $test_data($key) 4]
	set query      [lindex $test_data($key) 5]
	set oqp        [lindex $test_data($key) 6]
	set eqp        [lindex $test_data($key) 7]
	set keys       [lindex $test_data($key) 8]
	set indexcount [lindex $test_data($key) 9]

	# add index
	if { [string compare $node ""] == 0 } {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container addDefaultIndex $txn $index $uc
		$txn commit
		delete txn
	    } else {
		$container addDefaultIndex $index $uc
	    }
	} else {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container addIndex $txn $uri $node $index $uc
		$txn commit
		delete txn
	    } else {
		$container addIndex $uri $node $index $uc
	    }
	}

	# if requested, delete "match" document from each container (i.e. the one
	# document that would have been found)
	if { $delete } {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container deleteDocument $txn $match_name $uc
		$container_unindexed deleteDocument $txn $match_name $uc
		$txn commit
		delete txn
	    } else {
		$container deleteDocument $match_name $uc
		$container_unindexed deleteDocument $match_name $uc
	    }
	}

	# substitute '$test_path' string if it appears in the query
	set pattern {\$test_path}
	if { [regexp $pattern $query] == 1 } {
	    regsub -all $pattern $query $test_path query
	    regsub -all $pattern $oqp $test_path oqp
	    regsub -all $pattern $eqp $test_path eqp
	}

	# diagnostics
	puts "\t\t$testid: $operation / '$index'"

	#################################################################
	if { $regress} {

	    # append test id, container, query flags, old result to the arguments
	    set args [concat $regress_args "--container [$container getName]"]
	    set args [concat $args "--query \"$query\""]
	    set args [concat $args "--oldresult \"$oqp\""]

	    # this will dump the query plans to disk
	    run_external_test_program $txnenv $testid $regress_program $args

	} else {
	    #puts "\t\t\t\t query is $query"

	    # compare the query results for indexed and unindexed containers
	    set results_indexed {}
	    queryContainer $db $container $context $query $env $txnenv results_indexed
	    set results_unindexed {}
	    queryContainer $db $container_unindexed $context $query $env $txnenv results_unindexed
	    #puts "\t\t\t\t($results_indexed, $results_unindexed)"

	    dbxml_error_check_good "$testid Results Comparison" $results_indexed $results_unindexed
	}

	#################################################################

	# put back first document if it was deleted
	if { $delete } {
	    set file [lindex $documents 0]

	    set txn "NULL"
	    if {$txnenv == 1} {
		wrap XmlTransaction txn [$db createTransaction]
	    }

	    set ok [putFileInContainer $db $container "$test_path/$file" $txn]

	    dbxml_error_check_bad "Couldn't reload $file into container" $ok 0

	    set ok [putFileInContainer $db $container_unindexed "$test_path/$file" $txn]

	    dbxml_error_check_bad "Couldn't reload $file into container" $ok 0

	    if {$txnenv == 1} {
		$txn commit
		delete txn
	    }

	}

	# delete index from the container
	if { [string compare $node ""] == 0 } {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container deleteDefaultIndex $txn $index $uc
		$txn commit
		delete txn
	    } else {
		$container deleteDefaultIndex $index $uc
	    }
	} else {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container deleteIndex $txn $uri $node $index $uc
		$txn commit
		delete txn
	    } else {
		$container deleteIndex $uri $node $index $uc
	    }
	}

    }

    # housekeeping
    catch {unset test_data}

    delete context
    delete container
    delete container_unindexed
    delete uc
    delete db
}

# Executes a container query using a return type of result documents
#
# The 'results' parameter is overwritten by the number of hits
proc queryContainer { db container context query {env "NULL"} {txnenv 0} results} {
    source ./include.tcl
    set txn NULL

    upvar $results hits

    # TODO allow for deletion

    # execute query
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
	set hits [$xr size]
	delete xr
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults xr [$db query $fullQuery $context 0]
	set hits [$xr size]
	delete xr
    }
}

# Reads the input data for a group of tests from a text file
#
# The file is formatted using line-separated records. Data that applies to
# all tests is specified first. Individual test specifications then follow.
#
# Data that applies to all tests in the file is prefixed by a keyword:
#  - id of test group [mandatory]
#  - title of test group [mandatory]
#
# Individual test specifications do not have keywords. The data is expected
# in the following order:
#  - test id
#  - operation (test type)
#  - index specifcation of the form "uri "node" "index"
#  - query
#  - expected optimized query plan
#  - expected execution order
#  - statistics keys (space delimited, use "" if none)
#  - statistics entry count
#
# Blank lines and lines beginning with '#' are ignored.
# A comment block is enclosed by lines starting with "SKIP BEGIN" and
# "SKIP END" (nesting does not work).
# An isolated SKIP ends reading of the file.
#
# The data is written to the global array 'test_data'. The keys have a prefix
# appropriate to the type of data. The test specification keys are ordered
# to allow the correct execution order.
#
# The parameters are overwritten as follows:
#  'id'    -> test group id
#  'title' -> test group title
#
# The 'basename' corresponds to the prefix of each of the test ids. If two
# arguments are present then the first is the prefix read from the data source
# and the second is the prefix of the actual test id.
#
# Tests with ids that include any items from 'exclude' are skipped.
#
# Returns the number of tests.

proc getIndexingTestData {file basename exclude id title} {
    source ./include.tcl

    upvar $id test_group_id
    upvar $title test_group_title

    # reset the outputs
    global test_data
    catch { unset test_data }
    set test_group_id {}
    set test_group_title {}

    set state STATE_NONE
    set skip 0

    # tests have a prefix that normally that of the group name
    set data_prefix $basename
    set test_prefix $basename

    # sometimes a separate test group is using this data source
    if { [regexp " " $basename] == 1} {
	set data_prefix [lindex [split $basename] 0]
	set test_prefix [lindex [split $basename] 1]
    }

    # open file and read sequentially
    dbxml_error_check_good "Cannot open $file" [file exists $file] 1
    set fh [open $file r]

    # file has strings in utf-8 encoding
    fconfigure $fh -encoding utf-8
    set lineno 0
    set testcount 0
    set errmsg {}
    while { [gets $fh line] >= 0 } {
	set lineno [expr $lineno+1]

	# skip blank lines and comments
	if { $line == "" || [regexp "^#" $line] == 1 } { continue }

	# manage the skip mode
	if { [regexp "^$key_skip" $line] == 1 } {
	    if { [regexp BEGIN $line] == 1 } {
		set skip 1
	    } elseif { [regexp END $line] == 1 } {
		set skip 0
	    }

	    if [ string equal [string trim $line] $key_skip ] { break }
	}

	if { $skip } { continue }

	# group data - id of test group
	if { [regexp "^$key_groupid" $line] == 1 } {
	    regsub $key_groupid $line {} test_group_id
	    set test_group_id [string trim $test_group_id]
	    continue
	}

	# group data - title
	if { [regexp "^$key_grouptitle" $line] == 1 } {
	    regsub $key_grouptitle $line {} test_group_title
	    set test_group_title [string trim $test_group_title]
	    continue
	}

	# test specification - a test id marks the start
	if { [regexp "^$data_prefix\." $line] == 1 } {

	    if { $state != "STATE_NONE" } {
		set errmsg "Previous test specification incomplete?"
		break
	    }

	    # check if this test should be excluded
	    if { $exclude != 0 } {
		set exclude_test 0
		set start [string length "$data_prefix."]
		foreach item [split $exclude] {
		    if { [string first $item $line $start] != -1 } {
			set exclude_test 1
			continue
		    }
		}
		if { $exclude_test} { continue }
	    }

	    # substitute in the test prefix if this differs from the data source
	    if {$data_prefix != $test_prefix} {
		regsub $data_prefix $line $test_prefix line
	    }
	    set testid $line
	    set state STATE_TESTID
	}

	# test specification - expect consecutive lines in the correct order
	switch $state {
	    STATE_TESTID {
		set state STATE_OPERATION
	    }
	    STATE_OPERATION {
		set operation $line
		set state STATE_INDEX
	    }
	    STATE_INDEX {
		# "uri" "node" "index"
		set idx_spec [split $line]
		set uri [string trim [lindex $idx_spec 0] \"]
		set node [string trim [lindex $idx_spec 1] \"]
		set index [string trim [lindex $idx_spec 2] \"]

		set state STATE_QUERY
	    }
	    STATE_QUERY {
		set query $line
		set state STATE_OQP
	    }
	    STATE_OQP {
		set oqp $line
		set state STATE_EQP
	    }
	    STATE_EQP {
		set eqp $line
		set state STATE_STATISTICS
	    }
	    STATE_STATISTICS {
		# key1 [key2] [keyN] indexcount
		set statistics [split $line]

		if {[string first \"\" $line] == 0} {
		    # special case of empty keys - pair of quotes ""
		    set keys {}
		} else {
		    # space delimited list of words
		    set key_count [expr [llength $statistics] - 1]
		    if { [expr $key_count < 1] } {
			set errmsg "No statistics keys found"
			break
		    }
		    set keys [lrange $statistics 0 [expr $key_count - 1]]
		}

		set indexcount [lindex $statistics end]
		if { ! [string is integer $indexcount] } {
		    set errmsg "No statistics index count found"
		    break
		}

		unset statistics
		set state STATE_COMPLETE
	    }
	}

	# cache details of a complete specification
	if { [string equal $state STATE_COMPLETE] } {

	    # the key prefix is not actually needed since only test specifications
	    # are stored in the hash. However, this is consistent with the QPG tests.
	    set test_data($key_groupid:$testid) \
		[list $testid $operation $uri $node $index $query $oqp $eqp $keys $indexcount]

	    set testcount [expr $testcount+1]
	    unset keys
	    set state STATE_NONE
	}
    }
    close $fh

    if { [string length $errmsg] } {
	append msg "$file (line $lineno): " $errmsg
	dbxml_error_check_good $msg BAD GOOD
    }

    # sanity checks
    dbxml_error_check_bad "$file: Cannot find group id" $test_group_id ""
    dbxml_error_check_bad "$file: Cannot find group title" $test_group_title ""

    return $testcount
}

# regression tests -- bugs found and fixed
proc xml010_4 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\txml010.4: Regression testing -- bugs fixed"
    set id 0

    # 1.x regression tests
    xml010_4_1 $env $txnenv $basename.[incr id] $oargs

    # bug 12645 -- null pointer ref when no index present
    xml010_4_2 $env $txnenv $basename.[incr id] $oargs

    # bug 13850 -- reindexContainer doesn't change index entries
    xml010_4_3 $env $txnenv $basename.[incr id] $oargs
}

proc xml010_4_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t\t$basename: equality lookups on substring indexes."

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    set uri ""
    set node "name"
    set node1 "aname"
    set index "node-element-substring-string"
    set index1 "node-attribute-substring-string"

    set content "<root><name>Bob</name></root>"
    set content1 "<root aname='Bob'/>"
    set content2 "<root><name>Bobo</name></root>"
    set content3 "<root aname='Bobo'/>"

    # create container
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs]
    }

    # add indexes
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container addIndex $txn $uri $node $index $uc
	$container addIndex $txn $uri $node1 $index1 $uc
	$txn commit
	delete txn
    } else {
	$container addIndex $uri $node $index $uc
	$container addIndex $uri $node1 $index1 $uc
    }

    # add documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container putDocument $txn "" $content $uc $DBXML_GEN_NAME
	$container putDocument $txn "" $content1 $uc $DBXML_GEN_NAME
	$container putDocument $txn "" $content2 $uc $DBXML_GEN_NAME
	$container putDocument $txn "" $content3 $uc $DBXML_GEN_NAME
	$txn commit
	delete txn
    } else {
	$container putDocument "" $content $uc $DBXML_GEN_NAME
	$container putDocument "" $content1 $uc $DBXML_GEN_NAME
	$container putDocument "" $content2 $uc $DBXML_GEN_NAME
	$container putDocument "" $content3 $uc $DBXML_GEN_NAME
    }

    set queries {
	"collection('10.4.1.dbxml')/root[name='Bob']"
	"collection('10.4.1.dbxml')/root[name='Bobo']"
	"collection('10.4.1.dbxml')/root[@aname='Bob']"
	"collection('10.4.1.dbxml')/root[@aname='Bobo']"
    }
    wrap XmlQueryContext context [$mgr createQueryContext]
    foreach query $queries {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlResults xr [$mgr query $txn $query $context 0]
	    set hits [$xr size]
	    delete xr
	    $txn commit
	    delete txn
	} else {
	    wrap XmlResults xr [$mgr query $query $context 0]
	    set hits [$xr size]
	    delete xr
	}
        dbxml_error_check_good "$query hit" $hits 1
    }

    delete context
    delete container
    delete uc
    delete mgr
}

proc xml010_4_2 { {env "NULL"} {txnenv 0} basename oargs } {
    # test fix for SR [#12645]
    puts "\t\t\t$basename: test queries using indexes with no entries."

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    set uri ""
    set node "bar"
    set attr "id"
    set nodeIndex "node-element-equality-decimal"
    set attrIndex "node-attribute-equality-decimal"

    # create container
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs]
    }

    # add indexes
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container addIndex $txn $uri $node $nodeIndex $uc
	$container addIndex $txn $uri $attr $attrIndex $uc
	$txn commit
	delete txn
    } else {
	$container addIndex $uri $node $nodeIndex $uc
	$container addIndex $uri $attr $attrIndex $uc
    }

    set queries {
	"collection('10.4.2.dbxml')//bar[@id < xs:decimal('6')]"
    }
    wrap XmlQueryContext context [$mgr createQueryContext]
    foreach query $queries {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlResults xr [$mgr query $txn $query $context 0]
	    set hits [$xr size]
	    delete xr
	    $txn commit
	    delete txn
	} else {
	    wrap XmlResults xr [$mgr query $query $context 0]
	    set hits [$xr size]
	    delete xr
	}
        dbxml_error_check_good "$query hit" $hits 0
    }

    delete context
    delete container
    delete uc
    delete mgr
}

proc xml010_4_3 { {env "NULL"} {txnenv 0} basename oargs } {
    # test fix for SR [#12645]
    puts "\t\t\t$basename: test XmlManager::reindexContainer()."

    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    set uri ""
    set node "bar"
    set attr "id"
    set nodeIndex "node-element-equality-decimal"
    set attrIndex "node-attribute-equality-decimal"

    set doc1 "<root><bar id='100'>1</bar><bar id='200'>1</bar></root>"
    set doc2 "<root><bar id='300'>3</bar><bar id='400'>3</bar></root>"

    set nbarDoc 2
    set nbarNode 4
    set nidDoc 4
    set nidNode 4

    # create container
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs]
    }
    # add indexes
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container addIndex $txn $uri $node $nodeIndex $uc
	$container addIndex $txn $uri $attr $attrIndex $uc
	$txn commit
	delete txn
    } else {
	$container addIndex $uri $node $nodeIndex $uc
	$container addIndex $uri $attr $attrIndex $uc
    }
    # insert a couple of documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container putDocument $txn "" $doc1 $uc $DBXML_GEN_NAME
	$container putDocument $txn "" $doc2 $uc $DBXML_GEN_NAME
	$txn commit
	delete txn
    } else {
	$container putDocument "" $doc1 $uc $DBXML_GEN_NAME
	$container putDocument "" $doc2 $uc $DBXML_GEN_NAME
    }
    
    # perform lookup
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlResults xr1 [$container lookupIndex $txn $qc $uri $node $nodeIndex]
        wrap XmlResults xr2 [$container lookupIndex $txn $qc $uri $attr $attrIndex]
	$txn commit
	delete txn
    } else {
        wrap XmlResults xr1 [$container lookupIndex $qc $uri $node $nodeIndex]
        wrap XmlResults xr2 [$container lookupIndex $qc $uri $attr $attrIndex]
    }

    # get the number of results
    set in [$container getIndexNodes]
    set nbar [$xr1 size]
    set nid [$xr2 size]
    delete xr1
    delete xr2

    if { $in == 1 } {
        dbxml_error_check_good "nodeIndex bar" $nbar $nbarNode
        dbxml_error_check_good "nodeIndex id" $nid $nidNode
	set flags $DBXML_NO_INDEX_NODES
    } else {
        dbxml_error_check_good "docIndex bar" $nbar $nbarDoc
        dbxml_error_check_good "docIndex id" $nid $nidDoc
	set flags $DBXML_INDEX_NODES
    }
    set nbar 0
    set nid 0
    # reindex -- test reindex of open container (fails)
    catch {
	$mgr reindexContainer $basename.dbxml $uc 0
    } res

    # reindex, close container this time
    delete container

    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
        $mgr reindexContainer $xtxn $basename.dbxml $uc $flags
	$xtxn commit
	delete xtxn
    } else {
	$mgr reindexContainer $basename.dbxml $uc $flags
    }

    # reopen container
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr openContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr openContainer $basename.dbxml $oargs]
    }
    
    # perform lookup again
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlResults xr1 [$container lookupIndex $txn $qc $uri $node $nodeIndex]
        wrap XmlResults xr2 [$container lookupIndex $txn $qc $uri $attr $attrIndex]
	$txn commit
	delete txn
    } else {
        wrap XmlResults xr1 [$container lookupIndex $qc $uri $node $nodeIndex]
        wrap XmlResults xr2 [$container lookupIndex $qc $uri $attr $attrIndex]
    }

    # get the number of results
    set in [$container getIndexNodes]
    set nbar [$xr1 size]
    set nid [$xr2 size]
    delete xr1
    delete xr2

    if { $in == 1 } {
        dbxml_error_check_good "nodeIndex bar" $nbar $nbarNode
        dbxml_error_check_good "nodeIndex id" $nid $nidNode
	set flags $DBXML_NO_INDEX_NODES
    } else {
        dbxml_error_check_good "docIndex bar" $nbar $nbarDoc
        dbxml_error_check_good "docIndex id" $nid $nidDoc
	set flags $DBXML_INDEX_NODES
    }

    delete uc
    delete qc
    delete container
    delete mgr
}

# index BTree comparison function tests
proc xml010_5 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\txml010.5: Index BTree comparison function tests"

    source ./include.tcl
    set id 0

    set string_documents {
	"<root><foo value='charlie'/></root>"
	"<root value='goat'/>"
	"<root><foo value='zebra'/><bar value='longfellow'/></root>"
	"<root><foo value='Upper'/><bar value='Case'/></root>"
	"<root value=''/>"
	"<root value='goat'/>"
	"<frank value='bad_value'/>"
    }
    set string_results {
	""
	"Case"
	"Upper"
	"bad_value"
	"charlie"
	"goat"
	"goat"
	"longfellow"
	"zebra"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_STRING "string" \
	$string_documents $string_results

    ############################

    set base64Binary_documents {
	"<root><foo value='AZaz1234'/></root>"
	"<root value=''/>"
	"<root><foo value='AZ  az1234'/><bar value='54321Q= ='/></root>"
	"<root><foo value='frank/and/jim+ok'/><bar value='5  432 1Q=='/></root>"
	"<root value='abgDluFsTTk='/>"
	"<root value='54321Q==='/>"
	"<frank value='bad_value'/>"
    }
    set base64Binary_results {
	"54321Q==="
	"bad_value"
	""
	"AZaz1234"
	"AZ  az1234"
	"abgDluFsTTk="
	"frank/and/jim+ok"
	"54321Q= ="
	"5  432 1Q=="
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_BASE_64_BINARY "base64Binary" \
	$base64Binary_documents $base64Binary_results

    ############################

    set boolean_documents {
	"<root><foo value='         1'/></root>"
	"<root value='false'/>"
	"<root><foo value='0  '/><bar value='true'/></root>"
	"<root><foo value='1'/><bar value='true'/></root>"
	"<root value='false'/>"
	"<frank value='bad_value'/>"
    }
    set boolean_results {
	"bad_value"
	"false"
	"0  "
	"false"
	"         1"
	"true"
	"1"
	"true"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_BOOLEAN "boolean" \
	$boolean_documents $boolean_results

    ############################

    set date_documents {
	"<root><foo value='0001-01-01'/></root>"
	"<root value='-0001-12-31Z'/>"
	"<root><foo value='-0030-06-15Z'/><bar value='2000-02-29Z'/></root>"
	"<root><foo value='    0001-01-01Z'/><bar value='2000-03-01Z'/></root>"
	"<root value='-0030-06-15-05:30'/>"
	"<root value=' 2006-08-24  '/>"
	"<root value='0001-01-01+01:00'/>"
	"<frank value='bad_value'/>"
    }
    set date_results {
	"bad_value"
	"-0030-06-15Z"
	"-0030-06-15-05:30"
	"-0001-12-31Z"
	"0001-01-01+01:00"
	"0001-01-01"
	"    0001-01-01Z"
	"2000-02-29Z"
	"2000-03-01Z"
	" 2006-08-24  "
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_DATE "date" \
	$date_documents $date_results

    ############################

    set dateTime_documents {
	"<root><foo value='0001-01-01T00:00:00'/></root>"
	"<root value='-0001-12-31T00:00:00Z'/>"
	"<root><foo value='-0030-06-15T00:00:00Z'/><bar value='2000-02-29T00:00:00Z'/></root>"
	"<root><foo value='0001-01-01T00:00:00Z'/><bar value='2000-03-01T00:00:00Z'/></root>"
	"<root value='-0030-06-15T00:00:00-05:30'/>"
	"<root value='2006-08-24T00:00:00'/>"
	"<root value='0001-01-01T00:00:00+01:00'/>"

	"<root><foo value='0001-01-01T12:30:05'/></root>"
	"<root value='-0001-12-31T16:43:00Z'/>"
	"<root><foo value='-0030-06-15T02:01:10Z'/><bar value='2000-02-29T23:59:59Z'/></root>"
	"<root><foo value='0001-01-01T12:30:06Z'/><bar value='2000-03-01T00:01:00Z'/></root>"
	"<root value='-0030-06-15T00:00:32-05:30'/>"
	"<root value='2006-08-24T17:43:29'/>"
	"<root value='0001-01-01T13:30:05+01:00'/>"

	"<frank value='bad_value'/>"
    }
    set dateTime_results {
	"bad_value"
	"-0030-06-15T00:00:00Z"
	"-0030-06-15T02:01:10Z"
	"-0030-06-15T00:00:00-05:30"
	"-0030-06-15T00:00:32-05:30"
	"-0001-12-31T00:00:00Z"
	"-0001-12-31T16:43:00Z"
	"0001-01-01T00:00:00+01:00"
	"0001-01-01T00:00:00"
	"0001-01-01T00:00:00Z"
	"0001-01-01T12:30:05"
	"0001-01-01T13:30:05+01:00"
	"0001-01-01T12:30:06Z"
	"2000-02-29T00:00:00Z"
	"2000-02-29T23:59:59Z"
	"2000-03-01T00:00:00Z"
	"2000-03-01T00:01:00Z"
	"2006-08-24T00:00:00"
	"2006-08-24T17:43:29"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_DATE_TIME "dateTime" \
	$dateTime_documents $dateTime_results

    ############################

    set day_documents {
	"<root><foo value='---29'/></root>"
	"<root value='---31'/>"
	"<root><foo value='---32'/><bar value='---29+03:00'/></root>"
	"<root><foo value='---01'/><bar value='---01-14:00'/></root>"
	"<root value='---02Z'/>"
	"<root value='---02+14:00'/>"
	"<root value='---31Z'/>"
	"<root value='---10-01:00'/>"
	"<frank value='bad_value'/>"
    }
    set day_results {
	"---32"
	"bad_value"
	"---01"
	"---02+14:00"
	"---01-14:00"
	"---02Z"
	"---10-01:00"
	"---29+03:00"
	"---29"
	"---31"
	"---31Z"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_G_DAY "gDay" \
	$day_documents $day_results

    ############################

    set decimal_documents {
	"<root><foo value='-29'/></root>"
	"<root value='5'/>"
	"<root><foo value='-INF'/><bar value='--3267890'/></root>"
	"<root><foo value='0.888e-21'/><bar value='-3267890'/></root>"
	"<root value='590.63e37'/>"
	"<root value='NaN'/>"
	"<root value='-0.29e2'/>"
	"<root value='5'/>"
	"<root value='123456789012345678901234567890'/>"
	"<root value='-0.12345678901234567890123456789'/>"
	"<root value='0'/>"
	"<root value='2006'/>"
	"<root value='-0'/>"
	"<frank value='bad_value'/>"
    }
    set decimal_results {
	"-INF"
	"--3267890"
	"0.888e-21"
	"590.63e37"
	"NaN"
	"-0.29e2"
	"bad_value"
	"-3267890"
	"-29"
	"-0.12345678901234567890123456789"
	"0"
	"-0"
	"5"
	"5"
	"2006"
	"123456789012345678901234567890"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_DECIMAL "decimal" \
	$decimal_documents $decimal_results

    ############################

    set double_documents {
	"<root><foo value='-29'/></root>"
	"<root value='5'/>"
	"<root value='-1.7976931348623158e+308'/>"
	"<root value='-1.7976931348623157e+308'/>"
	"<root><foo value='-INF'/><bar value='--3267890'/></root>"
	"<root><foo value='0.888e-21'/><bar value='-3267890'/></root>"
	"<root value='590.63e37'/>"
	"<root value='NaN'/>"
	"<root value='-0.29e2'/>"
	"<root value='INF'/>"
	"<root value='1.7976931348623158e+308'/>"
	"<root value='1.7976931348623157e+308'/>"
	"<root value='5'/>"
	"<root value='123456789012345678901234567890'/>"
	"<root value='-0.12345678901234567890123456789'/>"
	"<root value='2.2250738585072013e-308'/>"
	"<root value='2.2250738585072014e-308'/>"
	"<root value='0'/>"
	"<root value='2006'/>"
	"<root value='-2.2250738585072013e-308'/>"
	"<root value='-2.2250738585072014e-308'/>"
	"<root value='-0'/>"
	"<frank value='bad_value'/>"
    }
    set double_results {
	"--3267890"
	"bad_value"
	"-1.7976931348623158e+308"
	"-INF"
	"-1.7976931348623157e+308"
	"-3267890"
	"-29"
	"-0.29e2"
	"-0.12345678901234567890123456789"
	"-2.2250738585072014e-308"
	"2.2250738585072013e-308"
	"0"
	"-2.2250738585072013e-308"
	"-0"
	"2.2250738585072014e-308"
	"0.888e-21"
	"5"
	"5"
	"2006"
	"123456789012345678901234567890"
	"590.63e37"
	"1.7976931348623157e+308"
	"INF"
	"1.7976931348623158e+308"
	"NaN"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_DOUBLE "double" \
	$double_documents $double_results

    ############################

    # PnYnMnDTnHnMnS
    set duration_documents {
	"<root><foo value='PT0S'/></root>"
	"<root value='P0D'/>"
	"<root><foo value='-P10D'/><bar value='-P10DT1S'/></root>"
	"<root value='-PT0S'/>"
	"<root><foo value='P1Y3M'/><bar value='P16M'/></root>"
	"<root value='frank'/>"
	"<root value='P1M'/>"
	"<root value=''/>"
	"<root value='-P1M'/>"
	"<root value='-P42D'/>"
	"<root value='P1Y2DT1H5M19S'/>"
	"<root value='P400DT1H5M19S'/>"
	"<frank value='bad_value'/>"
    }

    set duration_results {
	"frank"
	""
	"bad_value"
	"-P1M"
	"-P42D"
	"-P10DT1S"
	"-P10D"
	"PT0S"
	"P0D"
	"-PT0S"
	"P400DT1H5M19S"
	"P1M"
	"P1Y2DT1H5M19S"
	"P1Y3M"
	"P16M"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_DURATION "duration" \
	$duration_documents $duration_results

    ############################

    set float_documents {
	"<root><foo value='-29'/></root>"
	"<root value='5'/>"
	"<root value='-3.4028236e+38'/>"
	"<root value='-3.4028235e+38'/>"
	"<root><foo value='-INF'/><bar value='--3267890'/></root>"
	"<root><foo value='0.888e-21'/><bar value='-3267890'/></root>"
	"<root value='590.63e30'/>"
	"<root value='NaN'/>"
	"<root value='-0.29e2'/>"
	"<root value='INF'/>"
	"<root value='3.4028236e+38'/>"
	"<root value='3.4028235e+38'/>"
	"<root value='5'/>"
	"<root value='123456789012345678901234567890'/>"
	"<root value='-0.12345678901234567890123456789'/>"
	"<root value='1.1754943e-38'/>"
	"<root value='1.1754944e-38'/>"
	"<root value='0'/>"
	"<root value='2006'/>"
	"<root value='-1.1754943e-38'/>"
	"<root value='-1.1754944e-38'/>"
	"<root value='-0'/>"
	"<frank value='bad_value'/>"
    }
    set float_results {
	"--3267890"
	"bad_value"
	"-3.4028236e+38"
	"-INF"
	"-3.4028235e+38"
	"-3267890"
	"-29"
	"-0.29e2"
	"-0.12345678901234567890123456789"
	"-1.1754944e-38"
	"1.1754943e-38"
	"0"
	"-1.1754943e-38"
	"-0"
	"1.1754944e-38"
	"0.888e-21"
	"5"
	"5"
	"2006"
	"123456789012345678901234567890"
	"590.63e30"
	"3.4028235e+38"
	"INF"
	"3.4028236e+38"
	"NaN"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_FLOAT "float" \
	$float_documents $float_results

    ############################

    set hexBinary_documents {
	"<root><foo value='feed'/></root>"
	"<root value='ab12EF'/>"
	"<root><foo value='FEED'/><bar value='0836'/></root>"
	"<root><foo value='28'/><bar value='9fa'/></root>"
	"<root value=''/>"
	"<root value='7f9a2b76927d7900'/>"
	"<root value='A1'/>"
	"<root value='91'/>"
	"<frank value='bad_value'/>"
    }
    set hexBinary_results {
	"9fa"
	"bad_value"
	""
	"0836"
	"28"
	"7f9a2b76927d7900"
	"91"
	"A1"
	"ab12EF"
	"feed"
	"FEED"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_HEX_BINARY "hexBinary" \
	$hexBinary_documents $hexBinary_results

    ############################

    set month_documents {
	"<root><foo value='--11'/></root>"
	"<root value='--12'/>"
	"<root><foo value='--13'/><bar value='--11+03:00'/></root>"
	"<root><foo value='--01'/><bar value='--01-14:00'/></root>"
	"<root value='--02Z'/>"
	"<root value='--02+14:00'/>"
	"<root value='--12Z'/>"
	"<root value='--10-01:00'/>"
	"<frank value='bad_value'/>"
    }
    set month_results {
	"--13"
	"bad_value"
	"--01"
	"--01-14:00"
	"--02+14:00"
	"--02Z"
	"--10-01:00"
	"--11+03:00"
	"--11"
	"--12"
	"--12Z"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_G_MONTH "gMonth" \
	$month_documents $month_results

    ############################

    set monthDay_documents {
	"<root><foo value='--11-17'/></root>"
	"<root value='--12-25'/>"
	"<root><foo value='--02-30'/><bar value='--11-17+03:00'/></root>"
	"<root><foo value='--01-03'/><bar value='--01-31-14:00'/></root>"
	"<root value='--02-01Z'/>"
	"<root value='--02-01+14:00'/>"
	"<root value='--12-25Z'/>"
	"<root value='--10-09-01:00'/>"
	"<frank value='bad_value'/>"
    }
    set monthDay_results {
	"--02-30"
	"bad_value"
	"--01-03"
	"--02-01+14:00"
	"--01-31-14:00"
	"--02-01Z"
	"--10-09-01:00"
	"--11-17+03:00"
	"--11-17"
	"--12-25"
	"--12-25Z"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_G_MONTH_DAY "gMonthDay" \
	$monthDay_documents $monthDay_results

    ############################

    set time_documents {
	"<root><foo value='00:00:00Z'/></root>"
	"<root><foo value='12:30:05'/></root>"
	"<root value='16:43:00Z'/>"
	"<root><foo value='02:01:10Z'/><bar value='23:59:59Z'/></root>"
	"<root><foo value='12:30:06Z'/><bar value='00:01:00Z'/></root>"
	"<root value='00:00:32-05:30'/>"
	"<root value='00:00:00+14:00'/>"
	"<root value='17:43:29'/>"
	"<root value='13:30:05+01:00'/>"
	"<root value='24:00:00Z'/>"
	"<root><foo value='12:30:05Z'/></root>"
	"<root value='24:00:00-14:00'/>"
	"<root value='23:59:59-14:00'/>"
	"<frank value='bad_value'/>"
    }
    set time_results {
	"bad_value"
	"00:00:00+14:00"
	"00:00:00Z"
	"24:00:00Z"
	"00:01:00Z"
	"02:01:10Z"
	"00:00:32-05:30"
	"12:30:05"
	"13:30:05+01:00"
	"12:30:05Z"
	"12:30:06Z"
	"24:00:00-14:00"
	"16:43:00Z"
	"17:43:29"
	"23:59:59Z"
	"23:59:59-14:00"
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_TIME "time" \
	$time_documents $time_results

    ############################

    set year_documents {
	"<root><foo value='0001'/></root>"
	"<root value='-0001Z'/>"
	"<root><foo value='-0030Z'/><bar value='2000Z'/></root>"
	"<root><foo value='    0001Z'/><bar value='2000-14:00'/></root>"
	"<root value='-0030-05:30'/>"
	"<root value=' 2006  '/>"
	"<root value='0001+01:00'/>"
	"<frank value='bad_value'/>"
    }
    set year_results {
	"bad_value"
	"-0030Z"
	"-0030-05:30"
	"-0001Z"
	"0001+01:00"
	"0001"
	"    0001Z"
	"2000Z"
	"2000-14:00"
	" 2006  "
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_G_YEAR "gYear" \
	$year_documents $year_results

    ############################

    set yearMonth_documents {
	"<root><foo value='0001-02'/></root>"
	"<root value='-0001-05Z'/>"
	"<root><foo value='-0030-01Z'/><bar value='2000-12Z'/></root>"
	"<root><foo value='    0001-02Z'/><bar value='2000-11-14:00'/></root>"
	"<root value='-0030-01-05:30'/>"
	"<root value='0001-13'/>"
	"<root value=' 2006-08  '/>"
	"<root value='0001-02+01:00'/>"
	"<frank value='bad_value'/>"
    }
    set yearMonth_results {
	"0001-13"
	"bad_value"
	"-0030-01Z"
	"-0030-01-05:30"
	"-0001-05Z"
	"0001-02+01:00"
	"0001-02"
	"    0001-02Z"
	"2000-11-14:00"
	"2000-12Z"
	" 2006-08  "
    }
    xml010_5_x $env $txnenv $basename.[incr id] $oargs $XmlValue_G_YEAR_MONTH "gYearMonth" \
	$yearMonth_documents $yearMonth_results
}

proc xml010_5_x { {env "NULL"} {txnenv 0} basename oargs type typeName documents results} {
    puts "\t\t$basename: Index Check for $typeName"

    set id 0
    
    source ./include.tcl
    set txn NULL
    xml_cleanup $testdir $env

    xml_database mgr $testdir $env
    $mgr setImplicitTimezone 0

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]

    # create container
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs]
    }
    # add indexes
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	$container addIndex $txn "" "value" "node-attribute-equality-$typeName" $uc
	$txn commit
	delete txn
    } else {
	$container addIndex "" "value" "node-attribute-equality-$typeName" $uc
    }
    # insert the documents
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
    }
    foreach doc $documents {
	if { $txnenv == 1 } {
	    $container putDocument $txn "" $doc $uc $DBXML_GEN_NAME
	} else {
	    $container putDocument "" $doc $uc $DBXML_GEN_NAME
	}
    }
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }

    # Look up each value that should be present
    puts "\t\t\t$basename.[incr id]: Checking each value is present"
    foreach res $results {
	catch {
	    new XmlValue xv $type $res
	} ret

	# Only do the test if $res is a valid string for the type
	if {[string match "Error*not*valid*" $ret] == 0} {
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$mgr createTransaction]
		wrap XmlResults xr [$container lookupIndex $txn $qc "" "value" "node-attribute-equality-$typeName" $xv]
		set size [$xr size]
		delete xr
		$txn commit
		delete txn
	    } else {
		wrap XmlResults xr [$container lookupIndex $qc "" "value" "node-attribute-equality-$typeName" $xv]
		set size [$xr size]
		delete xr
	    }
	    delete xv

	    dbxml_error_check_bad "$basename no results for value index lookup ($res)" $size 0
	}
    }
    
    # Check the order of the results
    # (We can only check the value of the results
    # if our container is node indexed)
    if { [$container getIndexNodes] == 1 } {
	puts "\t\t\t$basename.[incr id]: Checking values are in the correct order"
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlResults xr [$container lookupIndex $txn $qc "" "value" "node-attribute-equality-$typeName"]
	} else {
	    wrap XmlResults xr [$container lookupIndex $qc "" "value" "node-attribute-equality-$typeName"]
	}

	new XmlValue xv
	foreach res $results {
	    $xr next $xv
	    dbxml_error_check_good "$basename ($xv isNull)" [$xv isNull] 0
	    dbxml_error_check_good "$basename value not correct (order wrong)" [$xv getNodeValue] $res
#	    puts [$xv getNodeValue]
	}

	if { $txnenv == 1 } {
	    $txn commit
	    delete txn
	}

	delete xv
	delete xr
    }
    
    delete container
    delete uc
    delete qc
    delete mgr
}

# miscellaneous indexing tests
proc xml010_6 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\txml010.6: Miscellaneous index tests"
    set id 0
    
    # Test default index updates [#15943]
    xml010_6_1 $env $txnenv $basename.[incr id] $oargs
    # Test reindexing near document node/root elements [#16500]
    xml010_6_2 $env $txnenv $basename.[incr id] $oargs
}

proc xml010_6_1 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: regression test for index update bug \[#15943\]"
    set id 0

    # prepare - create database, contexts, container
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    set content "<a attr='s2'/>"
    set query "for \$i in collection('$basename.dbxml')//@attr return replace value of node \$i with ''"

    set indexes {
	"node-attribute-equality-string"
	"node-attribute-equality-double"
    }

    # create container, add indexes, put the content
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $txn $basename.dbxml $oargs $global_container_type]
	wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	foreach idx $indexes {
	    $xis addDefaultIndex $idx
	}
	$container setIndexSpecification $txn $xis $uc
	$container putDocument $txn "foo" $content $uc
        $txn commit
        delete txn
	delete xis
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	wrap XmlIndexSpecification xis [$container getIndexSpecification]
	foreach idx $indexes {
	    $xis addDefaultIndex $idx
	}
	$container setIndexSpecification $xis $uc
	delete xis
	$container putDocument "foo" $content $uc
    }

    # verify index entries (no txn)
    xml10_6_verify $mgr $container $indexes "" "attr" $basename

    # do the update    
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $txn $query $qc]
        delete res
        $txn commit
        delete txn
    } else {
        wrap XmlResults res [$mgr query $query $qc]
        delete res
    }

    # verify index entries (no txn)
    xml10_6_verify $mgr $container $indexes "" "attr" $basename

    # clean up
    delete container
    delete uc
    delete qc
    delete mgr
}


proc xml010_6_2 { {env "NULL"} {txnenv 0} basename oargs } {
    puts "\t\t$basename: regression test for root element index update bug \[#16500\]"
    set id 0

    # prepare - create database, contexts, container
    source ./include.tcl
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    set rootContent "<root/>"
    set root1Content "<root1/>"
    set queryReplace "for \$i in collection('$basename.dbxml')/root return replace node \$i with <root1/>"
    set queryReplace1 "for \$i in collection('$basename.dbxml')/root1 return replace node \$i with <root/>"
    set queryInsert "for \$i in collection('$basename.dbxml') return insert node $rootContent into \$i"
    set queryInsert1 "for \$i in collection('$basename.dbxml') return insert node $root1Content into \$i"

    set indexes {
	"node-element-equality-string"
	"node-element-equality-double"
    }

    # assumes auto-indexing is ON
    # create container, add indexes, put the content
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $txn $basename.dbxml $oargs $global_container_type]
	wrap XmlIndexSpecification xis [$container getIndexSpecification $txn]
	dbxml_error_check_good "$basename" [$xis getAutoIndexing] 1
	$container putDocument $txn "doc" $rootContent $uc
        $txn commit
        delete txn
	delete xis
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	wrap XmlIndexSpecification xis [$container getIndexSpecification]
	dbxml_error_check_good "$basename" [$xis getAutoIndexing] 1
	delete xis
	$container putDocument "doc" $rootContent $uc
    }

    # verify index entries (no txn)
    xml10_6_verify $mgr $container $indexes "" "root" $basename

    # remove then replace root content
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlResults res [$mgr query $txn $queryReplace $qc]
        wrap XmlResults res1 [$mgr query $txn $queryReplace1 $qc]
        delete res
        delete res1
        $txn commit
        delete txn
    } else {
        wrap XmlResults res [$mgr query $queryReplace $qc]
	# in non-txn path, verify that the index entries were removed for root
	xml10_6_verify $mgr $container $indexes "" "root" $basename 0
	xml10_6_verify $mgr $container $indexes "" "root1" $basename
        wrap XmlResults res1 [$mgr query $queryReplace1 $qc]
        delete res
        delete res1
    }

    # verify index entries (no txn)
    xml10_6_verify $mgr $container $indexes "" "root1" $basename 0
    xml10_6_verify $mgr $container $indexes "" "root" $basename

    # verify basic query (no txn)
    wrap XmlResults res [$mgr query "collection('$basename.dbxml')/root" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res
    #
    # TBD -- verify that a second root element is a problem (or not)
    #

    # verify basic query (no txn)
    wrap XmlResults res [$mgr query "collection('$basename.dbxml')/root" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res
    

    # clean up
    delete container
    delete uc
    delete qc
    delete mgr
}

proc xml10_6_verify { mgr container indexes uri name basename {size 1} } {
    wrap XmlQueryContext qc [$mgr createQueryContext]
    foreach idx $indexes {
	wrap XmlIndexLookup il [$mgr createIndexLookup $container $uri $name $idx]
	wrap XmlResults res [$il execute $qc]
	dbxml_error_check_good "$basename" [$res size] $size
	delete il
	delete res
    }
    delete qc
}



