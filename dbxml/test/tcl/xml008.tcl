# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml008
# TEST	Container Management

proc xml008 { args } {
    puts "\nXml008: Container and database management ($args)"
    source ./include.tcl
    
    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 8
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

    # multiple containers
    xml008_1 $env $txnenv $basename.1 $oargs
    
    # Miscellaneous container API
    xml008_2 $env $txnenv $basename.2 $oargs

    # XmlContainer API (regression test suite)
    xml008_3 $env $txnenv $basename.3 $oargs
    
    # multiple databases
    xml008_5 $env $txnenv $basename.5 $oargs
    
    # container type
    xml008_6 $env $txnenv $basename.6 $oargs
    
    # container aliases
    xml008_7 $env $txnenv $basename.7 $oargs

    # query timeout
    xml008_8 $env $txnenv $basename.8 $oargs

    # container not found
    xml008_9 $env $txnenv $basename.9 $oargs

    # container read only
    xml008_10 $env $txnenv $basename.10 $oargs

    # set container properties using XmlContainerConfig
    xml008_11 $env $txnenv $basename.11 $oargs
    
    return
}

# multiple containers existing simultaneously
proc xml008_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\tXml008.1: Create two containers"
    source ./include.tcl
    set txn NULL

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlContainer container1 [$db createContainer $txn "$basename.1.dbxml" $oargs $global_container_type]
	    wrap XmlContainer container2 [$db createContainer $txn "$basename.2.dbxml" $oargs $global_container_type]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlContainer container1 [$db createContainer "$basename.1.dbxml" $oargs $global_container_type]
	    wrap XmlContainer container2 [$db createContainer "$basename.2.dbxml" $oargs $global_container_type]
	}
	delete container1
	delete container2
    } ret

    dbxml_error_check_good "8.1: Multiple containers" $ret ""

    delete db
}

# miscellaneous container tests
proc xml008_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\tXml008.2: Miscellaneous container tests"
    xml008_2_1 $env $txnenv $basename.1 $oargs
}

proc xml008_2_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.2.1: Changing default page size"
    source ./include.tcl
    set txn NULL
    # transactions are not used, as they are not relevant
    # to this case
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    # create a container using the default page size
    wrap XmlContainer container [$db createContainer "$basename.1.dbxml" $oargs $global_container_type]

    delete container
    $db setDefaultPageSize 65536

    # The fact that this works is enough to indicate passing
    # Pass DB_CREATE to openContainer for existing container, and
    # be sure the pagesizes do not match [#13768]
    set targs [expr $oargs + $DB_CREATE]
    wrap XmlContainer container [$db openContainer "$basename.1.dbxml" $targs $global_container_type]

    # test getFlags interface (trivially)
    set flags [$container getFlags]
    dbxml_error_check_good "$basename: getFlags" $flags [expr $targs + $DBXML_STATISTICS]
    delete container
    delete db
}

# XmlContainer API (DB XML 1.x regression test suite)
proc xml008_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\tXml008.3: Test container methods"
    xml008_3_1 $env $txnenv $basename.1 $oargs
    xml008_3_2 $env $txnenv $basename.2 $oargs
    xml008_3_3 $env $txnenv	$basename.3 $oargs
    xml008_3_4 $env $txnenv	$basename.4 $oargs
    xml008_3_5 $env $txnenv	$basename.5 $oargs
    xml008_3_6 $env $txnenv	$basename.6 $oargs
    xml008_3_7 $env $txnenv	$basename.7 $oargs
}

proc xml008_3_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.1: Open, close, open and close again"
    source ./include.tcl
    set txn "0"

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml

    # make sure it doesn't exist
    set exists [$db existsContainer $container_name]
    dbxml_error_check_good "$basename: existsContainer" $exists 0

    # create a container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    delete container

    # make sure it's there now
    set exists [$db existsContainer $container_name]
    dbxml_error_check_bad "$basename: existsContainer" $exists 0

    # open the existing container
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlContainer container [$db openContainer $txn $container_name $oargs]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlContainer container [$db openContainer $container_name $oargs]
	}
	delete container
    } ret

    dbxml_error_check_good "$basename: reopen, reclose" $ret ""
    delete db
}

proc xml008_3_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.2: Open, close, rename, open and close"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml

    # create a container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    # close the container
    delete container

    # rename the closed container
    set new_container_name $basename.renamed

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$db renameContainer $txn $container_name $new_container_name
	$txn commit
	delete txn
    } else {
	$db renameContainer $container_name $new_container_name
    }

    # open the renamed container
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlContainer container [$db openContainer $txn $new_container_name $oargs]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlContainer container [$db openContainer $new_container_name $oargs]
	}
	# test getFlags while we're here
	dbxml_error_check_good "$basename: getFlags" [$container getFlags] [expr $oargs + $DBXML_STATISTICS]
	delete container
    } ret

    dbxml_error_check_good "$basename: open & close after rename" $ret ""
    delete db
}

proc xml008_3_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.3: Open, close, remove, open and close"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    # create a container
    set container_name $basename.dbxml
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    # close the container
    delete container

    # remove the container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$db removeContainer $txn $container_name
	$txn commit
	delete txn
    } else {
	$db removeContainer $container_name
    }

    # open the container by re-creating
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
	}

	delete container
    } ret

    dbxml_error_check_good "$basename: open & close after remove" $ret ""
    delete db
}

proc xml008_3_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.4: Verify that rename renames underlying files"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    delete container

    dbxml_error_check_good "$basename: container exists" \
	[file exists $testdir/8.3.4.dbxml] 1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$db renameContainer $txn $container_name $basename.renamed.dbxml
	$txn commit
	delete txn
    } else {
	$db renameContainer $container_name $basename.renamed.dbxml
    }

    dbxml_error_check_good "$basename: container renamed" \
	[file exists $testdir/$basename.dbxml] 0

    dbxml_error_check_good "$basename: renamed container exists" \
	[file exists $testdir/$basename.renamed.dbxml] 1

    delete db
}

proc xml008_3_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.5: Verify that remove removes underlying files"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    delete container

    dbxml_error_check_good "$basename: container exists" \
	[file exists $testdir/$basename.dbxml] 1

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$db removeContainer $txn $container_name
	$txn commit
	delete txn
    } else {
	$db removeContainer $container_name
    }

    dbxml_error_check_good "$basename: container removed" \
	[file exists $testdir/$basename.dbxml] 0

    delete db
}

proc xml008_3_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.6: Attempt to re-create an existing container"
    source ./include.tcl
    set txn "0"

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml

    # create a container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
    }

    # close the container
    delete container

    # attempt to re-create the container
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
	    $txn commit
	    delete txn
	} else {
	    wrap XmlContainer container [$db createContainer $container_name $oargs $global_container_type]
	}
	delete container
    } ret

    dbxml_error_check_good "$basename: re-create" [string match "Error*exists" $ret] 1
    delete db
}

proc xml008_3_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\tXml008.3.7: Miscellaneous container admin tests"
    source ./include.tcl
    set txn "0"

    xml_cleanup $testdir $env

    xml_database mgr $testdir $env

    set container_name $basename.dbxml

    # create a container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr createContainer $container_name $oargs $global_container_type]
    }

    # sync the container
    $container sync

    # lookup index and statistics on indexes that do not exist
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlStatistics stat [$container lookupStatistics $txn "http://xyz.com/" "dummy" "node-attribute-presence-none"]
	dbxml_error_check_good "No stats entries" [$stat getNumberOfIndexedKeys] 0.0
	wrap XmlQueryContext qc [$mgr createQueryContext]
	wrap XmlResults res [$container lookupIndex $txn $qc "" "dummy" "node-attribute-equality-float"]
	dbxml_error_check_good "No index entries" [$res size] 0
	$txn commit
	delete txn
    } else {
	wrap XmlStatistics stat [$container lookupStatistics "http://xyz.com/" "dummy" "node-attribute-presence-none"]
	dbxml_error_check_good "No stats entries" [$stat getNumberOfIndexedKeys] 0.0
	wrap XmlQueryContext qc [$mgr createQueryContext]
	wrap XmlResults res [$container lookupIndex $qc "" "dummy" "node-attribute-equality-float"]
	dbxml_error_check_good "No index entries" [$res size] 0
    }

    delete qc
    delete res
    delete stat
    delete container
    delete mgr
}

# multiple databases existing simultaneously
#  - uses an external C++ program since test framework is geared towards a
#    single database
proc xml008_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
	puts "\t8.5: Test database management methods (external program)"
  source ./include.tcl

  # get test instructions
  set index_file [file join $test_path document_set_8_5 index.xml]

  set id {}
  set program {}
  set args {}
  set ret [parse_index_file $index_file id program args]
  dbxml_error_check_good "WARNING \"$index_file\" was not recognized as a valid test driver" $ret 1

  # create directories for test environments
  set db_env1 $testdir.$basename.1
  if { [file exists $db_env1] != 1 } {
	  file mkdir $db_env1
  }

  set db_env2 $testdir.$basename.2
  if { [file exists $db_env2] != 1 } {
	  file mkdir $db_env2
  }

  # set arguments to external program
  set args {}

  # basic arguments - DB XML environment
  set args [concat $args "--env1 $db_env1"]
  set args [concat $args "--env2 $db_env2"]

  run_external_test_program $txnenv $id $program $args

  # remove directories
  file delete -force $db_env1 $db_env2
}

# container type
#   tests API only; the test suite is executed using both whole document and
#   node level storage
proc xml008_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: Container Type"

    set id 0

    #########################################################
    # default container type (XmlManager methods)
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Default container type"

    # prepare - create database (the default container type is assigned by
    # the "xml_database" method, and is determined by a global flag set by the
    # top level caller)
    xml_cleanup $testdir $env
    xml_database db $testdir $env

    # perform sanity check on the global flag
    dbxml_error_check_good $testid [$db getDefaultContainerType] $global_container_type

    # change type for this database
    $db setDefaultContainerType $XmlContainer_WholedocContainer
    dbxml_error_check_good $testid [$db getDefaultContainerType] $XmlContainer_WholedocContainer
    $db setDefaultContainerType $XmlContainer_NodeContainer
    dbxml_error_check_good $testid [$db getDefaultContainerType] $XmlContainer_NodeContainer

    # remove this database
    delete db

    ###########################
    # Create an XmlManager *without* using global_container_type, which
    # will use the built-in defaults.  Verify that this is NodeContainer.
    # No transactions -- use a simple default XmlManager
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Create containers using built-in default type (node)"
    new XmlManager mgr
    wrap XmlContainer container [$mgr createContainer $testid.dbxml]
    dbxml_error_check_good $testid [$container getContainerType] $XmlContainer_NodeContainer
    delete container
    # remove container, and do it again with a different overload
    $mgr removeContainer $testid.dbxml

    wrap XmlContainer container [$mgr createContainer $testid.dbxml 0]
    dbxml_error_check_good $testid [$container getContainerType] $XmlContainer_NodeContainer
    delete container
    $mgr removeContainer $testid.dbxml
    delete mgr

    #########################################################
    # setting and getting the container type (XmlContainer methods)
    #  all subsequent tests use a database that uses the global flag to assign
    #  the default container type; these tests open and create containers,
    #  sometimes overriding the default container type
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # create a container using the default type (NB ignores flags)
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Create a container (default type)"

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $testid.dbxml]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $testid.dbxml]
    }
    dbxml_error_check_good $testid [$container getContainerType] $global_container_type
    delete container

    # create a container using the default type, using
    # different method overload (with flags).  This defaults
    # the type, and doesn't use the global type.
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Create a container (default type in createContainer)"

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $testid.dbxml 0]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $testid.dbxml 0]
    }
    dbxml_error_check_good $testid [$container getContainerType] $XmlContainer_NodeContainer
    delete container

    # create a container by specifying the type
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Create a container (specify type)"
    if { $global_container_type == $XmlContainer_WholedocContainer } {
        set new_type $XmlContainer_NodeContainer
    } else {
        set new_type $XmlContainer_WholedocContainer
    }

    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $testid.dbxml $oargs $new_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $testid.dbxml $oargs $new_type]
    }
    dbxml_error_check_good $testid [$container getContainerType] $new_type
    delete container

    # create a container by using the "open" method with DB_CREATE
    # - this should use the default container type
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Open a new container"
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db openContainer $txn $testid.dbxml [expr $oargs|$DB_CREATE]]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db openContainer $testid.dbxml [expr $oargs|$DB_CREATE]]
    }

    dbxml_error_check_good $testid [$container getContainerType] $global_container_type
    delete container

    # clean up
    delete uc
    delete db
}

# Container aliases used for queries
proc xml008_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t8.7: Use container alias names"
    source ./include.tcl
    set txn ""

    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    set container_name $basename.dbxml

    # create a container
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $txn $container_name $oargs $global_container_type]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr createContainer $container_name $oargs $global_container_type]
    }

    # add alias (one will be removed, the other won't)
    $container addAlias "alias"
    $container addAlias "alias2"
    set queryExp "collection('alias')"
    wrap XmlQueryContext qc [$mgr createQueryContext]

    # query the container using alias (failure would
    # throw...)
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $txn $queryExp $qc]
	$txn commit
	delete txn
    } else {
	wrap XmlResults res [$mgr query $queryExp $qc]
    }
    delete res

    # try using dbxml:container.  This ignores
    # baseURI (failure would throw)
    set cname "dbxml:/$container_name"
    set queryExp1 "collection('$cname')"
    # query the container using alias
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $txn $queryExp1 $qc]
	$txn commit
	delete txn
    } else {
	wrap XmlResults res [$mgr query $queryExp1 $qc]
    }
    delete res

    # remove alias and try again (will fail)
    $container removeAlias "alias"

    # query the container using alias
    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlResults res [$mgr query $txn $queryExp $qc]
	    $txn commit
	    delete res
	} else {
	    wrap XmlResults res [$mgr query $queryExp $qc]
	    delete res
	}
    } ret
    if { $txnenv == 1 } {
	delete txn
    }
    dbxml_error_check_good $basename.7 [is_substr $ret "auto-open"] 1

    # close the container and try querying using the other alias
    delete container

    catch {
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlResults res [$mgr query $txn "collection('alias2')" $qc]
	    $txn commit
	    delete res
	} else {
	    wrap XmlResults res [$mgr query "collection('alias2')" $qc]
	    delete res
	}
    } ret
    if { $txnenv == 1 } {
	delete txn
    }
    dbxml_error_check_good $basename.7 [is_substr $ret "auto-open"] 1
    delete qc
    delete mgr
}

# Query timeout test
proc xml008_8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t8.8: Query timeout"
    source ./include.tcl
    set txn "0"

    xml_cleanup $testdir $env
    xml_database mgr $testdir $env

    # this is a very simple test that doesn't even create
    # a container.   It uses a long-running standalong query
    # to test timeout.

    set queryExp "for \$i in (1 to 800000) return \$i"
    wrap XmlQueryContext qc [$mgr createQueryContext]

    # first let it work
    wrap XmlResults res [$mgr query $queryExp $qc]
    dbxml_error_check_good $basename.8 [$res size] 800000
    delete res

    $qc setQueryTimeoutSeconds 1
    catch { 
	wrap XmlResults nores [$mgr query $queryExp $qc]
	puts "FAIL: should not have gotten here"
	dbxml_error_check_bad "$basename.8" 1 1
    } ret

    dbxml_error_check_good $basename.8 [is_substr $ret "Query timed"] 1

    delete qc
    delete mgr
}


# Container not found error
proc xml008_9 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t8.9: Container not found"
    source ./include.tcl
    set txn "0"

    # this test requires a real env because it uses
    # berkdb commands.  xml_database will create an env if
    # it is NULL, which creates problems later, so do it here first...
    if { $env == "NULL" } {
        set env [eval {berkdb_env -home $testdir -create}]
    }

    xml_cleanup $testdir $env
    xml_database mgr $testdir $env

    set nocontainer "not_a_container"
    set is_container_name "is_a_container"
    set dbnames {
	"secondary_dictionary"
	"secondary_document"
	"primary_dictionary"
	"secondary_configuration"
    }

    catch { 
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$mgr createTransaction]
	    wrap XmlContainer cont [$mgr openContainer $txn $nocontainer $oargs ]
	} else {
	    wrap XmlContainer cont [$mgr openContainer $nocontainer $oargs ]
	}
	puts "FAIL: should not have gotten here"
	dbxml_error_check_bad "$basename.9" 1 1
    } ret
    if { $txnenv == 1 } {
	$txn abort
	delete txn
    }
    dbxml_error_check_good $basename.9 [is_substr $ret "file not found"] 1

    foreach name $dbnames {
	wrap XmlContainer container [$mgr createContainer $is_container_name $oargs $global_container_type]

	# now close it
	delete container

	# remove a critical subdb
	berkdb dbremove -env $env $is_container_name $name
	catch { 
	    wrap XmlContainer cont [$mgr openContainer $is_container_name $oargs ]
	    puts "FAIL: should not have gotten here"
	    dbxml_error_check_bad "$basename.9.$name" 1 1
	} ret
	dbxml_error_check_good $basename.9 [is_substr $ret "file not found"] 1
	$mgr removeContainer $is_container_name
    }

    delete mgr
}

# Container read only
proc xml008_10 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t8.10: Container read only"
    source ./include.tcl
    set txn "0"

    xml_cleanup $testdir $env
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    set container_name "readonly.dbxml"
    set doc_name "doc"
    set doc_name1 "doc1"
    set doc_content "<root><a att='1'/></root>"

    # create a container and insert a document
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $txn $container_name $oargs $global_container_type]
	$container putDocument $txn $doc_name $doc_content $uc
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr createContainer $container_name $oargs $global_container_type]
	$container putDocument $doc_name $doc_content $uc
    }
    # close the container
    delete container

    # open it read-only
    set ronly_args [expr $oargs + $DB_RDONLY]
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr openContainer $txn $container_name $ronly_args]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr openContainer $container_name $ronly_args]
    }
    $container addAlias "alias"

    wrap XmlQueryContext qc [$mgr createQueryContext]

    # try to putdocument
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	catch { $container putDocument $txn $doc_name1 $doc_content $uc } ret
	$txn commit
	delete txn
    } else {
	catch { $container putDocument $doc_name1 $doc_content $uc } ret
    }
    dbxml_error_check_good $basename [string match "*read-only*" $ret] 1

    # try to add an index
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlIndexSpecification is [$container getIndexSpecification]
	catch { $container setIndexSpecification $txn $is $uc } ret
	$txn commit
	delete txn
    } else {
	wrap XmlIndexSpecification is [$container getIndexSpecification]
	catch { $container setIndexSpecification $is $uc } ret
    }
    delete is;
    dbxml_error_check_good $basename [string match "*read-only*" $ret] 1
    

    # basic query  (will work)
    set query "collection('alias')/root/a"
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $txn $query $qc]
    } else {
	wrap XmlResults res [$mgr query $query $qc]
    }
    
    new XmlValue value
    while { [$res next $value] } {
	set v [$value asString]
    }
    if { $txnenv == 1 } {
	$txn commit
	delete txn
    }
    delete res
    delete value

    # update query (will fail)
    set query "for \$i in collection('alias')/root/a return rename node \$i as 'b'"
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	catch {wrap XmlResults res [$mgr query $txn $query $qc]} ret
	$txn commit
	delete txn
    } else {
	catch {wrap XmlResults res [$mgr query $query $qc]} ret
    }
    dbxml_error_check_good $basename [string match "*read-only*" $ret] 1
    
    delete qc
    delete container
    delete uc
    delete mgr
}

# Set container properties with XmlContainerConfig
proc xml008_11 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t8.11: Test XmlContainerConfig"
    xml008_11_1 $env $txnenv $basename.1 $oargs
    xml008_11_2 $env $txnenv $basename.1 $oargs
}

proc xml008_11_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\t8.11.1: Test XmlContainerConfig functions"
    source ./include.tcl

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml

    #test setGetContainerType
    set config [XmlContainerConfig]
    $config setContainerType $XmlContainer_WholedocContainer
    dbxml_error_check_good "$basename container type" [$config getContainerType] $XmlContainer_WholedocContainer

    #test setGetMode
    $config setMode 755
    dbxml_error_check_good "$basename mode" [$config getMode] 755

    #test setGetPageSize
    $config setPageSize 1024
    dbxml_error_check_good "$basename pagesize" [$config getPageSize] 1024

    #test setGetSequenceIncrement
    $config setSequenceIncrement 20
    dbxml_error_check_good "$basename sequenceincrement" [$config getSequenceIncrement] 20

    #test setGetIndexNodes
    $config setIndexNodes $XmlContainerConfig_On
    dbxml_error_check_good "$basename indexnodes" [$config getIndexNodes] $XmlContainerConfig_On

    #test setGetStatistics
    $config setStatistics $XmlContainerConfig_On
    dbxml_error_check_good "$basename statistics" [$config getStatistics] $XmlContainerConfig_On

    #test setGetChecksum
    $config setChecksum 1
    dbxml_error_check_good "$basename checksum" [$config getChecksum] 1

    #test setGetEncrypted
    $config setEncrypted 1
    dbxml_error_check_good "$basename encrypted" [$config getEncrypted] 1

    #test setAllowValidation
    $config setAllowValidation 1
    dbxml_error_check_good "$basename validation" [$config getAllowValidation] 1
    
    #test setTransactional
    $config setTransactional 1
    dbxml_error_check_good "$basename Transactional" [$config getTransactional] 1

    #test setTransactional
    $config setTransactional 1
    dbxml_error_check_good "$basename Transactional" [$config getTransactional] 1

    #test setAllowCreate
    $config setAllowCreate 1
    dbxml_error_check_good "$basename AllowCreate" [$config getAllowCreate] 1

    #test setExclusiveCreate
    $config setExclusiveCreate 1
    dbxml_error_check_good "$basename ExclusiveCreate" [$config getExclusiveCreate] 1

    #test setNoMMap
    $config setNoMMap 1
    dbxml_error_check_good "$basename NoMMap" [$config getNoMMap] 1

    #test setReadOnly
    $config setReadOnly 1
    dbxml_error_check_good "$basename ReadOnly" [$config getReadOnly] 1

    #test setMultiversion
    $config setMultiversion 1
    dbxml_error_check_good "$basename Multiversion" [$config getMultiversion] 1

    #test setReadUncommitted
    $config setReadUncommitted 1
    dbxml_error_check_good "$basename ReadUncommitted" [$config getReadUncommitted] 1

    #test setThreaded
    $config setThreaded 1
    dbxml_error_check_good "$basename Threaded" [$config getThreaded] 1

    #test setTransactionNotDurable
    $config setTransactionNotDurable 1
    dbxml_error_check_good "$basename TransactionNotDurable" [$config getTransactionNotDurable] 1
}

proc xml008_11_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\t\t8.11.2: Test XmlContainerConfig with containers"
    source ./include.tcl

    xml_cleanup $testdir $env

    xml_database db $testdir $env

    set container_name $basename.dbxml
    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    set container_name "readonly.dbxml"
    set doc_name "doc"
    set doc_name1 "doc1"
    set doc_content "<root><a att='1'/></root>"

    # create a container using a XmlContainerConfig
    new XmlContainerConfig config $oargs
    $config setContainerType $global_container_type
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr createContainer $txn $container_name $config]
	dbxml_error_check_good "$basename container type" [$container getContainerType] $global_container_type
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr createContainer $container_name $config]
	dbxml_error_check_good "$basename container type" [$container getContainerType] $global_container_type
    }
    # close the container
    delete container

    # open it read-only
    $config setReadOnly 1
    dbxml_error_check_good $basename [$config getReadOnly] 1
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlContainer container [$mgr openContainer $txn $container_name $config]
	$txn commit
	delete txn
    } else {
	wrap XmlContainer container [$mgr openContainer $container_name $config]
    }

    set xcc [$container getContainerConfig]
    dbxml_error_check_good "$basename ReadOnly" [$xcc getReadOnly] 1

    # try to putdocument
    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$mgr createTransaction]
	catch { $container putDocument $txn $doc_name1 $doc_content $uc } ret
	$txn commit
	delete txn
    } else {
	catch { $container putDocument $doc_name1 $doc_content $uc } ret
    }
    dbxml_error_check_good $basename [string match "*read-only*" $ret] 1
}
