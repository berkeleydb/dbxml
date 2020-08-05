# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml003
# TEST	Transaction testing

proc xml003 { {args ""} } {
    puts "\nXml003: Transaction testing ($args)"
    source ./include.tcl
    # Determine whether procedure has been called within an environment.
    # Skip test if we are given an env.  It needs its own.
    set eindex [lsearch -exact $args "-env"]
    if { $eindex != -1 } {
	puts "Skipping xml003 for non-NULL environments"
	return
    }

    # oargs is used for arguments to XmlContainer open
    set oargs [eval {set_openargs} $args]
    # eargs is used for argument to berkdb_env
    set eargs " -create "
    if { [lsearch -exact $args thread] >= 0 } {
	append eargs " -thread "
    }

    # Set up test env for explicit txn testing.
    env_cleanup $testdir
    set env [eval {berkdb_env} $eargs -txn -mode 0644 -home $testdir]
    dbxml_error_check_good open_txn_env [is_valid_env $env] TRUE

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

    set tnum 3
    set basename $tnum

    puts "\tXml003.1: basic commit and abort tests"
    set basename $tnum.1

    catch {
  	xml003_1_1_1 $env $basename.1.1 $oargs
  	xml003_1_2 $env $basename.2 $oargs
  	xml003_1_3 $env $basename.3 $oargs
  	xml003_1_4_1 $env $basename.4.1 $oargs
  	xml003_1_4_2 $env $basename.4.2 $oargs
  	xml003_1_4_3 $env $basename.4.3 $oargs
  	xml003_1_5_1 $env $basename.5.1 $oargs
  	xml003_1_5_2 $env $basename.5.2 $oargs
  	xml003_1_5_3 $env $basename.5.3 $oargs
  	xml003_1_6_1 $env $basename.6.1 $oargs
  	xml003_1_6_2 $env $basename.6.2 $oargs
  	xml003_1_7 $env $basename.7 $oargs
   	xml003_1_8 $env $basename.8 $oargs
  } ret

    dbxml_error_check_good no_exceptions $ret ""
    dbxml_error_check_good env_close [$env close] 0
    berkdb envremove -home $testdir

    puts "\tXml003.2: auto_commit tests"
    set basename $tnum.2

    # The xml003_2 tests exercise auto-commit.  This is triggered
    # implicitly by creating/opening a container with transactions,
    # and using non-transacted methods for access.

    # Set up a new txn env for the bulk of auto_commit testing.
    # 3.2.1, 3.2.2 don't currently exist.
    env_cleanup $testdir
    set env [eval {berkdb_env} $eargs -txn -mode 0644 -home $testdir]
    dbxml_error_check_good open_txn_env [is_valid_env $env] TRUE

    catch {
  	xml003_2_1 $env $basename.1 $oargs
  	xml003_2_2 $env $basename.2 $oargs
  	xml003_2_3 $env $basename.3 $oargs
  	xml003_2_4 $env $basename.4 $oargs
  	xml003_2_5_1 $env $basename.5.1 $oargs
  	xml003_2_5_2 $env $basename.5.2 $oargs
  	xml003_2_6_1 $env $basename.6.1 $oargs
  	xml003_2_6_2 $env $basename.6.2 $oargs
  	xml003_2_7_1 $env $basename.7.1 $oargs
  	xml003_2_7_2 $env $basename.7.2 $oargs
  	xml003_2_8 $env $basename.8 $oargs
	xml003_2_9 $env $basename.9 $oargs
    } ret

    dbxml_error_check_good no_exceptions $ret ""
    dbxml_error_check_good env_close [$env close] 0
    berkdb envremove -home $testdir

    puts "\tXml003.3: Transactionally linked DB-XML and BDB records."
    set basename $tnum.3

    # Open a transactional env.
    set env [eval {berkdb_env} $eargs -txn -mode 0644 -home $testdir]
    dbxml_error_check_good open_txn_env [is_valid_env $env] TRUE

    xml003_3 $env $basename.1 $oargs

    dbxml_error_check_good env_close [$env close] 0
    berkdb envremove -home $testdir

    puts "\tXml003.4: Ensure DB_AUTO_COMMIT Environment fails."
    set basename $tnum.4

    # Open a transactional env, wth auto-commit
    set env [eval {berkdb_env} $eargs -txn -auto_commit -mode 0644 -home $testdir]
    dbxml_error_check_good open_txn_env [is_valid_env $env] TRUE

    # For now, it works... See comments in Manager.cpp (DB_AUTO_COMMIT)
    catch {
	xml_database mgr $testdir $env
	#puts "FAIL: should not have gotten here"
	#dbxml_error_check_bad "$basename.4" 1 1
	delete mgr
    } ret

    dbxml_error_check_good env_close [$env close] 0
    berkdb envremove -home $testdir

    return
}

proc xml003_1_1_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.1.1: Container open"
    source ./include.tcl

    xml_database db $testdir $env

    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $basename.dbxml]
    $xtxn commit
    delete xtxn

    delete container
    delete db
}

proc xml003_1_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.2: addIndex"
    source ./include.tcl

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "abort commit" {
	puts "\t\t\tTest $op"

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	set uri www.uri.com
	set name foo
	set index node-element-presence-none

	wrap XmlTransaction txn [$db createTransaction]
	$container addIndex $txn $uri $name $index $uc
	$txn $op
	delete txn

	# Test that index spec was persisted.
	wrap XmlIndexSpecification indexes [$container getIndexSpecification]
	set idecl [$indexes find $uri $name]
	delete indexes

	if { $op == "abort" } {
	    # If addIndex was aborted, the index should not
	    # have been found.
	    dbxml_error_check_good declare_abort $idecl NULL
	} else {
	    # If addIndex was committed, the index specification
	    # should persist.
	    dbxml_error_check_bad declare_commit $idecl NULL
	    wrap XmlIndexDeclaration xid $idecl
	    set retindex [$xid get_index]
	    dbxml_error_check_good index $retindex $index
	    delete xid
	}
	delete container
    }

    delete uc
    delete db
}

proc xml003_1_3 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.3: putDocument."
    source ./include.tcl

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
        puts "\t\t\tTest $op"
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
        set doc_name doc_$basename

        wrap XmlDocument xd [$db createDocument]
        set data_in "<a><b>c</b></a>"
        $xd setContent $data_in
        $xd setName $doc_name
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn $op
        delete txn
        delete xd
        catch {
            wrap XmlDocument xd [$container getDocument $doc_name]
            set data_out [$xd getContentAsString]
            delete xd
        } ret
        if { $op == "abort" } {
            # If the put was aborted, the attempt to get a document should
            # give a 'not found' error.
            dbxml_error_check_good check_abort \
                [is_substr $ret "Document not found"] 1
        } else {
            # If the put was committed, the retrieved data should match
            # the loaded data.
            dbxml_error_check_good content_correct $data_out $data_in
        }
        delete container
    }
    delete uc
    delete db
}

# Test 3.4 - state doesn't change in the case of an aborted getDocument
# so we are just testing that the interface doesn't create unexpected errors.
proc xml003_1_4_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.4.1: Commit and abort on eager getDocument."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	puts "\t\t\tTest $op"

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	set name_in "test_doc"
	set data_in "<a><b>c</b></a>"

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $name_in $data_in $uc
	$txn commit
	delete txn

	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $name_in]
	# force retrieval of NLS data
	$xd fetchAllData
	$txn $op
	delete txn

	# The retrieved data should match the loaded data.
	set name_out [$xd getName]
	set data_out [$xd getContentAsString]
	dbxml_error_check_good name_correct $name_out $name_in
	dbxml_error_check_good content_correct $data_out $data_in

	delete xd

	delete container
    }
    delete uc
    delete db
}

proc xml003_1_4_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.4.2: Commit and abort on lazy getDocument."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	foreach usesync "sync nosync" {
	    if { $usesync == "sync" } {
		puts "\t\t\tTest $op using fetchAllData()"
	    } else {
		puts "\t\t\tTest $op"
	    }

	    wrap XmlTransaction xtxn [$db createTransaction]
	    wrap XmlContainer container [$db createContainer $xtxn $basename.$op.$usesync.dbxml $oargs $global_container_type]
	    $xtxn commit
	    delete xtxn

	    set name_in "test_doc"
	    set data_in "<a><b>c</b></a>"

	    wrap XmlTransaction txn [$db createTransaction]
	    $container putDocument $txn $name_in $data_in $uc
	    $txn commit
	    delete txn

	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlDocument xd [$container getDocument $txn $name_in $DBXML_LAZY_DOCS]
	    if { $usesync == "sync" } {
		$xd fetchAllData
		$txn $op
		delete txn
	    }

	    # The retrieved data should match the loaded data.
	    set name_out [$xd getName]
	    set data_out [$xd getContent]
	    dbxml_error_check_good name_correct $name_out $name_in
	    dbxml_error_check_good content_correct $data_out $data_in

	    if { $usesync != "sync" } {
		$txn $op
		delete txn
	    }

	    delete xd
	    delete container
	}
    }
    delete uc
    delete db
}

proc xml003_1_4_3 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.4.3: Commit and abort on lazy getDocument before reading"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	puts "\t\t\tTest $op"

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	set name_in "test_doc"
	set data_in "<a><b>c</b></a>"

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $name_in $data_in $uc
	$txn commit
	delete txn

	wrap XmlTransaction txn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $txn $name_in $DBXML_LAZY_DOCS]

	$txn $op
	delete txn

	# getName() should work - name is cached metadata
	catch {set name_out [$xd getName]} ret
	dbxml_error_check_good name_failed $name_out $name_in

	# getContentAsString() should throw
	set data_out ""
	catch {set data_out [$xd getContentAsString]} ret
	dbxml_error_check_bad content_failed $data_out $data_in

        # getContent() (as XmlData/Dbt) should also throw
        catch { wrap XmlData xml_data_out [$xd getContent]} ret
        dbxml_error_check_good $basename [string match "Error*transaction*" $ret] 1

	delete xd
	delete container
    }
    delete uc
    delete db
}

proc xml003_1_5_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.5.1: Delete with eager XmlDocument object."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
        puts "\t\t\tTest $op"

        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn

        wrap XmlDocument xd [$db createDocument]
        set data_in "<a><b>c</b></a>"
        $xd setContent $data_in

        set doc_name doc_$basename
        $xd setName $doc_name

        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn

        wrap XmlTransaction txn [$db createTransaction]
        catch {$container deleteDocument $txn $doc_name $uc} ret
        $txn $op
        delete txn
        delete xd

        wrap XmlTransaction txn [$db createTransaction]
        catch {
            wrap XmlDocument xd [$container getDocument $txn $doc_name]
	    $xd fetchAllData
        } ret
        $txn commit
        delete txn

        if { $op == "abort" } {
            # If the delete was aborted, the 'get' should have been successful,
            # and we can check the data.
            set data_out [$xd getContentAsString]
            dbxml_error_check_good content_correct $data_in $data_out
            delete xd
        } else {
            # op was "commit".  Check for appropriate error on the getDocument.
            dbxml_error_check_good doc_deleted \
                [is_substr $ret "Document not found"] 1
        }
        delete container
    }
    delete uc
    delete db
}

proc xml003_1_5_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.5.2: Delete with lazy XmlDocument object."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	foreach usesync "sync nosync" {
	    if { $usesync == "sync" } {
		puts "\t\t\tTest $op using fetchAllData()"
	    } else {
		puts "\t\t\tTest $op"
	    }

	    wrap XmlTransaction xtxn [$db createTransaction]
	    wrap XmlContainer container [$db createContainer $xtxn $basename.$op.$usesync.dbxml $oargs $global_container_type]
	    $xtxn commit
	    delete xtxn

	    wrap XmlDocument xd [$db createDocument]
	    set data_in "<a><b>c</b></a>"
	    $xd setContent $data_in

	    set doc_name doc_$basename
	    $xd setName $doc_name

	    wrap XmlTransaction txn [$db createTransaction]
	    $container putDocument $txn $xd $uc
	    $txn commit
	    delete txn
	    delete xd

	    wrap XmlTransaction txn [$db createTransaction]
	    wrap XmlDocument xd [$container getDocument $txn $doc_name $DBXML_LAZY_DOCS]
	    if { $usesync == "sync" } {
		$xd fetchAllData
		$txn commit
		delete txn
		wrap XmlTransaction txn [$db createTransaction]
	    }
	    catch {$container deleteDocument $txn $xd $uc} ret
	    $txn $op
	    delete txn
	    delete xd

	    wrap XmlTransaction txn [$db createTransaction]
	    catch {
		wrap XmlDocument xd [$container getDocument $txn $doc_name]
		$xd fetchAllData
	    } ret
	    $txn commit
	    delete txn

	    if { $op == "abort" } {
		# If the delete was aborted, the 'get' should have been successful,
		# and we can check the data.
		set data_out [$xd getContentAsString]
		dbxml_error_check_good content_correct $data_in $data_out
		delete xd
	    } else {
		# op was "commit".  Check for appropriate error on the getDocument.
		dbxml_error_check_good doc_deleted  \
		[is_substr $ret "Document not found"] 1
	    }
	    delete container
	}
    }
    delete uc
    delete db
}

proc xml003_1_5_3 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.5.3: Delete with document name."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	puts "\t\t\tTest $op"

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $basename.$op.dbxml $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	wrap XmlDocument xd [$db createDocument]
	set data_in "<a><b>c</b></a>"
	$xd setContent $data_in

	set doc_name doc_$basename
	$xd setName $doc_name

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
	delete xd

	wrap XmlTransaction txn [$db createTransaction]
	catch {$container deleteDocument $txn $doc_name $uc} ret
	$txn $op
	delete txn

	wrap XmlTransaction txn [$db createTransaction]
	catch {
	    wrap XmlDocument xd [$container getDocument $txn $doc_name]
	    $xd fetchAllData
	} ret
	$txn commit
	delete txn

	if { $op == "abort" } {
	    # If the delete was aborted, the 'get' should have been successful,
	    # and we can check the data.
	    set data_out [$xd getContentAsString]
	    dbxml_error_check_good content_correct $data_in $data_out
	    delete xd
	} else {
	    # op was "commit".  Check for appropriate error on getDocument.
	    dbxml_error_check_good doc_deleted \
		[is_substr $ret "Document not found"] 1
	}
	delete container
    }
    delete uc
    delete db
}

# Test 3.1.6 - state doesn't change in the case of an aborted queryWithXPath
# so we are just testing that the interface doesn't create unexpected errors.
proc xml003_1_6_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.6.1: queryWithXPath using XPathExpression."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	puts "\t\t\tTest $op"

	set container_name $basename.$op.dbxml

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $container_name $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	wrap XmlDocument xd [$db createDocument]
	set data_in "<a><b>c</b><d/></a>"
	$xd setContent $data_in
	$xd setName "foo"

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn

	wrap XmlQueryContext context [$db createQueryContext]
	$context setReturnType $XmlQueryContext_LiveValues
	$context setEvaluationType $XmlQueryContext_Eager

	# Run transacted query and check return value
	set fullQuery "collection('[$container getName]')/a/b"
	new XmlValue xv
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
	wrap XmlResults xr [$xe execute $xtxn $context]

	dbxml_error_check_good "size_of_result_set" [$xr size] 1

	$xr next $xv
	new XmlValue newxv [$xv asString]
	dbxml_error_check_good "check_contents" [$newxv asString] [$xv asString]
  delete newxv

  # check we can navigate from the result (parent should be document)
  wrap XmlValue parent [$xv getParentNode]
  dbxml_error_check_good "check parent" [$parent asString] $data_in
  delete parent

	$xtxn $op
	delete xtxn

	delete xr
	delete xe
	delete xv
	delete context
	delete xd

	delete container
	# Remove container so size of result set does not change.
	$db removeContainer $container_name
    }

    delete uc
    delete db
}

proc xml003_1_6_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.6.2: queryWithXPath using XmlQueryContext."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    foreach op "commit abort" {
	puts "\t\t\tTest $op"

	set container_name $basename.$op.dbxml

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $container_name $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	wrap XmlDocument xd [$db createDocument]
	set data_in "<a><b>c</b><d/></a>"
	$xd setContent $data_in
	$xd setName "foo"

	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn

	wrap XmlQueryContext context [$db createQueryContext]
	$context setReturnType $XmlQueryContext_LiveValues
	$context setEvaluationType $XmlQueryContext_Eager

	# Run transacted query and check return value
	set fullQuery "collection('[$container getName]')/a/b"
	new XmlValue xv
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]

	dbxml_error_check_good "size_of_result_set" [$xr size] 1

	$xr next $xv
	new XmlValue newxv [$xv asString]
	dbxml_error_check_good "check_contents" [$newxv asString] [$xv asString]
	delete newxv

  # check we can navigate from the result (parent should be document)
  wrap XmlValue parent [$xv getParentNode]
  dbxml_error_check_good "check parent" [$parent asString] $data_in
  delete parent

	$xtxn $op
	delete xtxn

	delete xv
	delete xr
	delete xd
	delete context
	delete container
	# Remove container so size of result set does not change.
	$db removeContainer $container_name
    }
    delete uc
    delete db
}

proc xml003_1_7 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.7: Remove container"
    source ./include.tcl

    foreach op "commit abort" {
	xml_cleanup $testdir $env
	xml_database db $testdir $env

	puts "\t\t\tTest $op"

	set container_name $basename.$op.dbxml

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $container_name $oargs $global_container_type]
	$xtxn commit
	delete xtxn

	delete container

	wrap XmlTransaction txn [$db createTransaction]
	$db removeContainer $txn $container_name
	$txn $op
	delete txn

	if { $op == "abort" } {
	    # Underlying files should still exist.
	    dbxml_error_check_good "original primary_dict exists" \
		[file exists $testdir/$basename.$op.dbxml] 1
	} else {
	    # $op was a commit, underlying files should be gone.
	    dbxml_error_check_good "original primary_dict exists" \
		[file exists $testdir/$basename.$op.dbxml] 0
	}
	delete db
    }
}

proc xml003_1_8 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.1.8: Rename container"
    source ./include.tcl

    foreach op "commit abort" {
	puts "\t\t\tTest $op"
	xml_cleanup $testdir $env
	xml_database db $testdir $env

	set container_name $basename.$op.dbxml

	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db createContainer $xtxn $container_name $oargs $global_container_type]
	$xtxn commit
	delete xtxn
	delete container

	wrap XmlTransaction txn [$db createTransaction]
	$db renameContainer $txn $container_name $basename.$op.renamed.dbxml
	$txn $op
	delete txn

	if { $op == "abort" } {
	    # Renamed file will not exist, and original file will.
	    dbxml_error_check_good "renamed primary_dict exists" \
		[file exists $testdir/$basename.$op.renamed.dbxml] 0
	    dbxml_error_check_good "original primary_dict exists" \
		[file exists $testdir/$basename.$op.dbxml] 1
	} else {
	    # $op was a commit
	    # Renamed file will exist, and original file will not.
	    dbxml_error_check_good "renamed primary_dict exists" \
		[file exists $testdir/$basename.$op.renamed.dbxml] 1
	    dbxml_error_check_good "original primary_dict exists" \
		[file exists $testdir/$basename.$op.dbxml] 0
	}
	delete db
    }
}

proc xml003_2_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.1: TBD"

    # removed this test case; may replace it one day.
    source ./include.tcl
    global errorInfo
}

proc xml003_2_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.2: TBD"
    source ./include.tcl
    # removed this test case; may replace it one day.
}

proc xml003_2_3 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.3: Transacted addIndex after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    set uri www.uri.com
    set name foo
    set index node-element-presence-none
    # Run addIndex within an explicit transaction.
    wrap XmlTransaction txn [$db createTransaction]
    $container addIndex $txn $uri $name $index $uc
    $txn commit
    delete txn

    delete container

    # Test that index spec was persisted.  No transaction used here
    wrap XmlContainer container [$db openContainer $basename.dbxml $oargs]

    wrap XmlIndexSpecification indexes [$container getIndexSpecification]
    set idecl [$indexes find $uri $name]
    delete indexes
    dbxml_error_check_bad foundindex $idecl NULL

    wrap XmlIndexDeclaration xid $idecl
    set retindex [$xid get_index]
    delete xid
    dbxml_error_check_good index $retindex $index

    delete container
    delete uc
    delete db
}

proc xml003_2_4 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.4: Transacted putDocument after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b></a>"
    $xd setContent $data_in

    set doc_name doc_$basename
    $xd setName $doc_name

    # put Document within an explicit transaction
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd $uc
    $txn commit
    delete txn

    delete xd

    # Verify that the put was successful by retrieving the data.
    wrap XmlDocument xd [$container getDocument $doc_name]
    set data_out [$xd getContentAsString]
    delete xd
    dbxml_error_check_good content_correct $data_in $data_out

    delete container
    delete uc
    delete db
}

proc xml003_2_5_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.5.1: Transacted eager getDocument after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b></a>"
    $xd setContent $data_in

    set doc_name doc_$basename
    $xd setName $doc_name

    $container putDocument $xd $uc
    delete xd

    # getDocument within an explicit transaction
    # (NOTE: getDocument isn't subject to auto-commit, regardless)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd [$container getDocument $txn $doc_name]
    $xd fetchAllData
    $txn commit
    delete txn

    # Verify that data is correct
    set data_out [$xd getContentAsString]
    delete xd
    dbxml_error_check_good content_correct $data_in $data_out

    delete container
    delete uc
    delete db
}

proc xml003_2_5_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.5.2: Transacted lazy getDocument after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b></a>"
    $xd setContent $data_in

    set doc_name doc_$basename
    $xd setName $doc_name

    $container putDocument $xd $uc
    delete xd

    # getDocument within an explicit transaction
    # NOTE: getDocument isn't subject to auto-commit, regardless
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlDocument xd [$container getDocument $txn $doc_name $DBXML_LAZY_DOCS]

    # Verify that data is correct
    set data_out [$xd getContentAsString]
    delete xd
    dbxml_error_check_good content_correct $data_in $data_out

    $txn commit
    delete txn

    delete container
    delete uc
    delete db
}

proc xml003_2_6_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.6.1: Auto-commit deleteDocument after auto_commit"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<hello/>"
    $xd setContent $data_in

    set doc_name doc_$basename
    $xd setName $doc_name

    $container putDocument $xd $uc
    delete xd

    # deleteDocument via auto-commit
    set ret [$container deleteDocument $doc_name $uc]

    # Verify that document has been deleted.
    catch {
	wrap XmlDocument xd [$container getDocument $doc_name]
	delete xd
    } ret
    dbxml_error_check_good document_deleted  \
		[is_substr $ret "Document not found"] 1

    delete container
    delete uc
    delete db
}

proc xml003_2_6_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.6.2: Transacted delete after auto_commit\
	    \n\t\t\t(using XmlDocument)"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<hello/>"
    $xd setContent $data_in

    set doc_name doc_$basename
    $xd setName $doc_name

    $container putDocument $xd $uc

    # delete with XmlDocument within an explicit transaction
    wrap XmlTransaction txn [$db createTransaction]
    $container deleteDocument $txn $xd $uc
    $txn commit
    delete txn

    # Verify that document has been deleted.
    catch {
	wrap XmlDocument xd [$container getDocument $doc_name]
    } ret
    dbxml_error_check_good document_deleted \
		[is_substr $ret "Document not found"] 1

    delete xd
    delete container
    delete uc
    delete db
}

proc xml003_2_7_1 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.7.1: Transacted query after\
	    auto_commit \n\t\t\t(using XmlQueryContext)"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $basename.dbxml $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b><d/></a>"
    $xd setContent $data_in
    $xd setName "foo"
    $container putDocument $xd $uc

    wrap XmlQueryContext context [$db createQueryContext]
    $context setReturnType $XmlQueryContext_LiveValues
    $context setEvaluationType $XmlQueryContext_Eager

    # Run transacted query and check return value
    set fullQuery "collection('[$container getName]')/a/b"
    new XmlValue xv
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]

    dbxml_error_check_good "size_of_result_set" [$xr size] 1

    $xr next $xv
    new XmlValue newxv [$xv asString]
    dbxml_error_check_good "check_contents" [$newxv asString] [$xv asString]
    delete newxv

    # check we can navigate from the result (parent should be document)
    wrap XmlValue parent [$xv getParentNode]
    dbxml_error_check_good "check parent" [$parent asString] $data_in
    delete parent

    $xtxn commit
    delete xtxn

    delete xr
    delete xv
    delete xd
    delete context

    delete container
    delete uc
    delete db
}

proc xml003_2_7_2 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.7.2: Explicitly-transacted query after\
	    auto_commit \n\t\t\t(using XPathExpression)"
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    # Use DBXML_TRANSACTIONAL flag to enable auto-commit
    set newargs [expr $oargs + $DBXML_TRANSACTIONAL]
    wrap XmlContainer container [$db createContainer $basename.dbxml $newargs $global_container_type]

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b><d/></a>"
    $xd setContent $data_in
    $xd setName "foo"
    $container putDocument $xd $uc

    wrap XmlQueryContext context [$db createQueryContext]
    $context setReturnType $XmlQueryContext_LiveValues
    $context setEvaluationType $XmlQueryContext_Eager

    # Run explicitly transacted query and check return value.
    # If the use of DBXML_TRANSACTIONAL did not work, this would throw
    set fullQuery "collection('[$container getName]')/a/b"
    wrap XmlTransaction xtxn [$db createTransaction]
    new XmlValue xv
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    $xr next $xv
    $xtxn commit
    delete xtxn

    dbxml_error_check_good "size_of_result_set" [$xr size] 1

    new XmlValue newxv [$xv asString]
    dbxml_error_check_good "check_contents" [$newxv asString] [$xv asString]
    delete newxv

    # check we can navigate from the result (parent should be document)
    wrap XmlValue parent [$xv getParentNode]
    dbxml_error_check_good "check parent" [$parent asString] $data_in
    delete parent

    delete xe
    delete xr
    delete xv
    delete xd
    delete context

    delete container
    delete uc
    delete db
}

proc xml003_2_8 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.8: Container remove after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    set container_name $basename.dbxml
    # Use DBXML_TRANSACTIONAL flag to enable auto-commit
    set newargs [expr $oargs + $DBXML_TRANSACTIONAL]
    wrap XmlContainer container [$db createContainer $container_name $newargs $global_container_type]

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b></a>"
    $xd setContent $data_in
    $xd setName "foo"

    # this will auto-commit
    $container putDocument $xd $uc
    delete xd

    delete container

    # Now do a transacted removeContainer
    wrap XmlTransaction txn [$db createTransaction]
    $db removeContainer $txn $container_name
    $txn commit
    delete txn

    # Underlying files should be gone.
    dbxml_error_check_good "original primary_dict exists" \
	[file exists $testdir/$basename.dbxml] 0

    delete uc
    delete db
}

proc xml003_2_9 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.2.9: Non-transacted rename after auto_commit."
    source ./include.tcl
    xml_cleanup $testdir $env

    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]

    set container_name $basename.dbxml
    # Open container in transaction (enables auto-commit)
    wrap XmlTransaction txn [$db createTransaction]
    wrap XmlContainer container [$db createContainer $txn $container_name $oargs $global_container_type]
    $txn commit
    delete txn

    wrap XmlDocument xd [$db createDocument]
    set data_in "<a><b>c</b></a>"
    $xd setContent $data_in
    $xd setName "foo"

    set doc_name doc_$basename
    $xd setName $doc_name

    # this will auto-commit
    $container putDocument $xd $uc
    delete xd

    delete container

    # Now do a non-transacted container rename
    set new_container_name $basename.renamed.dbxml
    $db renameContainer $container_name $new_container_name

    # Underlying files should be gone.
    dbxml_error_check_good "original primary_dict exists" \
	[file exists $testdir/$basename.dbxml] 0

    # Renamed files should exist ...
    dbxml_error_check_good "renamed primary_dict exists" \
	[file exists $testdir/$basename.renamed.dbxml] 1

    # ... now retrieve the contents ...
    wrap XmlContainer container [$db openContainer $new_container_name $oargs]
    catch {
	wrap XmlDocument xd [$container getDocument $doc_name]
	set data_out [$xd getContentAsString]
	delete xd
    } ret

    # ... and verify they match the original loaded data.
    dbxml_error_check_good content_correct $data_in $data_out

    delete container
    delete uc
    delete db
}

proc xml003_3 { {env "NULL"} {basename $tnum} oargs } {
    puts "\t\tXml003.3.1: DB-XML in combination with regular Berkeley DB"
    source ./include.tcl
    global dict
    xml_cleanup $testdir $env

    #
    # When sharing with DB, Environment and txns must be created via
    # DB API, not DB XML
    #

    # Open regular BDB database.
    puts "\t\t\tXml003.3.1a: Set up BDB database."
    set testfile $basename.db
    set db [eval {berkdb_open -create -btree -env $env\
		      -auto_commit -mode 0644 $testfile}]
    error_check_good dbopen [is_valid_db $db] TRUE

    # Open DB-XML container, pass it DB env
    puts "\t\t\tXml003.3.1b: Set up DB-XML database."
    xml_database xmldb $testdir $env
    wrap XmlUpdateContext uc [$xmldb createUpdateContext]

    wrap XmlTransaction xtxn [$xmldb createTransaction]
    wrap XmlContainer container [$xmldb createContainer $xtxn $basename.dbxml $oargs]
    $xtxn commit
    delete xtxn

    # Populate BDB and DBXML databases with records from the same source.
    set count 1
    set nentries 100
    set did [open $dict]

    puts "\t\t\tXml003.3.1c: Put/get on both DB's"
    set testnum 0
    while { [gets $did str] != -1 && $count < $nentries } {
	# Skip dictionary entries containing apostrophes (these
	# result in badly formed XML) and the null string (DB-XML
	# cannot verify the encoding).
	if { $str == "" || [string first ' $str] != -1 } {
	    continue
	}

	incr testnum

	set key $str
	wrap XmlDocument xd [$xmldb createDocument]
	set data_in "<$str/>"

	$xd setContent $data_in
	set doc_name doc_$basename.$testnum
	$xd setName $doc_name
	# Put data in XML and non-XML databases within a txn.
	# Must start txn from DB's env
	set dbtxn [start_txn $env]
	wrap XmlTransaction txn [$xmldb createTransaction $dbtxn]
	$container putDocument $txn $xd $uc
	set ret [eval {$db put} -txn $dbtxn $key $str]
	# commit via DB
	$dbtxn commit
	delete txn
	delete xd
	# Get data from both DB's within a txn and make sure it matches.
	set dbtxn [start_txn $env]
	wrap XmlTransaction txn [$xmldb createTransaction $dbtxn]
	set ret [eval {$db get} -txn $dbtxn {$key}]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]
	set data_out [$xd getContentAsString]
	delete xd
	# commit via BDB XML this time
	#$dbtxn commit
	$txn commit
	delete txn
	# Now check that both values were updated and that they match.
	#		error_check_good bdb_get $ret [list [list $key $str]]

	dbxml_error_check_good content_correct $data_in $data_out
	dbxml_error_check_good db_matches_dbxml \
	    [is_substr $data_out $str] 1

	# Now test with an abort.  Update the values, and abort.
	# State should be the same as it was.
	wrap XmlDocument xd [$xmldb createDocument]
	set bad_data_in "<badstring/>"

	$xd setContent $bad_data_in
	set doc_name doc_$basename.$testnum
	$xd setName $doc_name
	# Put data in XML and non-XML databases within a txn.
	# Must start txn from DB's env
	set dbtxn [start_txn $env]
	wrap XmlTransaction txn [$xmldb createTransaction $dbtxn]
	# use update for in-place modification
	$container updateDocument $txn $xd $uc
	set ret [eval {$db put} -txn $dbtxn $key $bad_data_in]
	# abort (use XmlTransaction this time)
	#$dbtxn abort
	$txn abort
	delete txn
	delete xd

	# validate original content
	# Get data from both DB's within a txn and make sure it matches.
	set dbtxn [start_txn $env]
	wrap XmlTransaction txn [$xmldb createTransaction $dbtxn]
	set ret [eval {$db get} -txn $dbtxn {$key}]
	wrap XmlDocument xd [$container getDocument $txn $doc_name]
	set data_out [$xd getContentAsString]
	delete xd
	# commit via DbTxn this time
	$dbtxn commit

	# validate error if attempt to commit XmlTransaction
	catch { $txn commit } ret
	dbxml_error_check_good check_abort \
	    [is_substr $ret "already committed"] 1
	delete txn
	# Now check that both values were updated and that they match.
	#		error_check_good bdb_get $ret [list [list $key $str]]

	dbxml_error_check_good content_correct $data_in $data_out
	dbxml_error_check_good db_matches_dbxml \
	    [is_substr $data_out $str] 1

	incr count
    }
    close $did
    error_check_good db_close [$db close] 0
    berkdb dbremove -env $env $testfile

    delete container
    delete uc
    delete xmldb
}

