# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
# TEST	xml014
# TEST	XQuery update query to modify document
# TEST	Must cover matrix of operations and types, against both containers
# TEST  and document, with and without transactions.

proc xml014 { args } {
	source ./include.tcl
	puts "\nxml014: Partial modifications of Documents ($args)"

	# Determine whether procedure has been called within an environment,
	# and whether it is a transactional environment.
	# If we are using an env, then the filename should just be the test
	# number.  Otherwise it is the test directory and the test number.
	set eindex [lsearch -exact $args "-env"]
	set txnenv 0
	set tnum 14
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
    xml014_1 $env $txnenv $basename.1 $oargs
    xml014_2 $env $txnenv $basename.2 $oargs
    xml014_3 $env $txnenv $basename.3 $oargs
    xml014_4 $env $txnenv $basename.4 $oargs
    xml014_5 $env $txnenv $basename.5 $oargs
}


#
# test XQuery Update
#
proc xml014_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test XQuery Update"

    xml014_1_1 $env $txnenv $basename.1 $oargs
    xml014_1_2 $env $txnenv $basename.2 $oargs
    xml014_1_3 $env $txnenv $basename.3 $oargs
    xml014_1_4 $env $txnenv $basename.4 $oargs
}

proc xml014_1_common_proc { {env "NULL"} {txnenv 0} oargs basename containerName docContent titles querys verifyQuerys Expectations {callback 0}} {
    source ./include.tcl
    source $test_path/xml014_globals.tcl

    set testid "xml0$basename"
    puts "\t\t$testid: [lindex $titles 0]"

    if {$callback != 0} { $callback }

    xml_database mgr $testdir $env

    # create the container
    if {$txnenv == 1} {
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
    }
    wrap XmlContainer container [$mgr createContainer $containerName $oargs $global_container_type]

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    # create the original docs (rely on auto-commit for transaction environments)
    foreach content $docContent {
	$container putDocument "" $content $uc $DBXML_GEN_NAME
    }
    delete uc

    # Modify Nodes Using XQuery Update
    set cnt 1
    wrap XmlQueryContext context [$mgr createQueryContext]
    foreach query $querys {
        puts "\t\t\t$testid.$cnt: [lindex $titles $cnt]"
        if { $txnenv == 1 } {
            wrap XmlTransaction xtxn [$mgr createTransaction]
            wrap XmlResults xr [$mgr query $xtxn $query $context 0]
            $xtxn commit
            delete xtxn
        } else {
            wrap XmlResults xr [$mgr query $query $context 0]
        }
        delete xr
        incr cnt
    }
    delete context

    #check the correctness.
    set cnt 0
    wrap XmlQueryContext qc [$mgr createQueryContext]
    foreach verifyQuery $verifyQuerys {
        wrap XmlResults xr [$mgr query $verifyQuery $qc 0]
        dbxml_error_check_good "$testid.$cnt" [$xr size] [lindex $Expectations $cnt]
        delete xr
        incr cnt
    }
    delete qc

    delete container
    delete mgr
}

# test Inserting Nodes Using XQuery Update
proc xml014_1_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl

    set title "Inserting Nodes Using XQuery Update";
    set containerName $basename.dbxml

    set sub_title1 "Insert a new node before the target node";
    set query1 "for \$insert in collection('$containerName')/root/a return insert nodes <B>B</B> before \$insert"
    set verifyQuery1 "collection('$containerName')/root/B"
    set Expectations1 2

    set sub_title2 "Insert a new node after the target node";
    set query2 "for \$insert in collection('$containerName')/root/a return insert nodes <C>C</C> after \$insert"
    set verifyQuery2 "collection('$containerName')/root/C"
    set Expectations2 2

    set sub_title3 "Insert a new node as a child of the target node";
    set query3 "for \$insert in collection('$containerName')/root/a return insert nodes <D>D</D> into \$insert"
    set verifyQuery3 "collection('$containerName')/root/a/D"
    set Expectations3 2

    set sub_title4 "Insert a new node as the first child of the target node";
    set query4 "for \$insert in collection('$containerName')/root/a return insert nodes <E>E</E> as first into \$insert"
    set verifyQuery4 "collection('$containerName')/root/a/E"
    set Expectations4 2

    set sub_title5 "Insert a new node as the last child of the target node";
    set query5 "for \$insert in collection('$containerName')/root/a return insert nodes <F>F</F> as last into \$insert"
    set verifyQuery5 "collection('$containerName')/root/a/F"
    set Expectations5 2

    set titles [list $title $sub_title1 $sub_title2 $sub_title3 $sub_title4 $sub_title5]
    set querys [list $query1 $query2 $query3 $query4 $query5]
    set verifyQuerys [list $verifyQuery1 $verifyQuery2 $verifyQuery3 $verifyQuery4 $verifyQuery5]
    set Expectations [list $Expectations1 $Expectations2 $Expectations3 $Expectations4 $Expectations5]

    set documents [list $rcontent1 $rcontent2]

    xml014_1_common_proc $env $txnenv $oargs $basename $containerName $documents $titles $querys $verifyQuerys $Expectations
}


# test Deleting Nodes Using XQuery Update
proc xml014_1_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl

    set title "Deleting Nodes Using XQuery Update";
    set containerName $basename.dbxml

    set sub_title1 "Delete the target node";
    set query1 "for \$delete in collection('$containerName')/root/b return delete nodes \$delete"
    set verifyQuery1 "collection('$containerName')/root/b"
    set Expectations1 0

    set sub_title2 "Delete the specific attribute of the target node";
    set query2 "for \$delete in collection('$containerName')/root/a/c/@cattr return delete nodes \$delete"
    set verifyQuery2 "collection('$containerName')/root/a/c/@cattr"
    set Expectations2 0

    set titles [list $title $sub_title1 $sub_title2]
    set querys [list $query1 $query2]
    set verifyQuerys [list $verifyQuery1 $verifyQuery2]
    set Expectations [list $Expectations1 $Expectations2]

    set documents [list $rcontent1 $rcontent2]

    xml014_1_common_proc $env $txnenv $oargs $basename $containerName $documents $titles $querys $verifyQuerys $Expectations
}

# test Replacing Nodes Using XQuery Update
proc xml014_1_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl

    set title "Replacing Nodes Using XQuery Update";
    set containerName $basename.dbxml

    set sub_title1 "Replace the the target node";
    set query1 "for \$replace in collection('$containerName')/root/b return replace node \$replace with <B>B</B>"
    set verifyQuery1 "collection('$containerName')/root/B"
    set Expectations1 2

    set sub_title2 "Replace value of the specific attribute";
    set query2 "for \$replace in collection('$containerName')/root/a/c/@cattr return replace value of node \$replace with 'CATTR'"
    set verifyQuery2 "collection('$containerName')/root/a/c\[@cattr = 'CATTR'\]"
    set Expectations2 1

    set sub_title3 "Replace value of the target node";
    set query3 "for \$replace in collection('$containerName')/root/a/c return replace value of node \$replace with 'C'"
    set verifyQuery3 "collection('$containerName')/root/a\[c/string() = 'C'\]"
    set Expectations3 2

    set titles [list $title $sub_title1 $sub_title2 $sub_title3]
    set querys [list $query1 $query2 $query3]
    set verifyQuerys [list $verifyQuery1 $verifyQuery2 $verifyQuery3]
    set Expectations [list $Expectations1 $Expectations2 $Expectations3]

    set documents [list $rcontent1 $rcontent2]

    xml014_1_common_proc $env $txnenv $oargs $basename $containerName $documents $titles $querys $verifyQuerys $Expectations
}

# test Renaming Nodes Using XQuery Update
proc xml014_1_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl

    set title "Renaming Nodes Using XQuery Update";
    set containerName $basename.dbxml

    set sub_title1 "Rename the target node";
    set query1 "for \$rename in collection('$containerName')/root/b  
return rename node \$rename as 'B'"
    set verifyQuery1 "collection('$containerName')/root/B"
    set Expectations1 2

    set sub_title2 "Rename the specific attribute of the target node";
    set query2 "for \$rename in collection('$containerName')/root/a/c/@cattr return rename node \$rename as 'C'"
    set verifyQuery2 "collection('$containerName')/root/a/c/@C"
    set Expectations2 1

    set titles [list $title $sub_title1 $sub_title2]
    set querys [list $query1 $query2]
    set verifyQuerys [list $verifyQuery1 $verifyQuery2]
    set Expectations [list $Expectations1 $Expectations2]

    set documents [list $rcontent1 $rcontent2]

    xml014_1_common_proc $env $txnenv $oargs $basename $containerName $documents $titles $querys $verifyQuerys $Expectations
}

#
# More testing of XQuery Update, specifically surrounding
# creation and deletion of document elements at the root of a document
#
proc xml014_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test XQuery Update Document Element"

    xml_database mgr $testdir $env
    set containerName $basename.dbxml

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $containerName $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $containerName $oargs $global_container_type]
    }
    $container addAlias "c"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    # let auto-commit handle transactions from here on...

    # test simple insert into empty doc (state: empty)
    $container putDocument "emptyDoc" "" $uc
    wrap XmlResults resu [$mgr query "for \$i in collection('c') return insert node <root><a/></root> into \$i" $qc]
    delete resu

    wrap XmlResults res [$mgr query "collection('c')/root" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res

    # test deleting the node twice and re-inserting (state: <root></a></root>)
    wrap XmlResults resu [$mgr query "for \$i in collection('c')/root let \$a:=\$i/a return (delete node \$i,delete node \$a/..,insert node <newroot><a/></newroot> before \$i)" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/newroot" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res

    # test delete of document element (state: <newroot><a/></newroot>)
    catch {
	$mgr query "for \$i in collection('c')/newroot return delete node \$i" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # test insertion of additional document element (state: <newroot><a/></newroot>)
    catch {
	$mgr query "for \$i in collection('c') return insert node <root/> into \$i" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # test insertion of additional document element in a query that 
    # deletes as well (state: <newroot><a/></newroot>)
    catch {
	$mgr query "for \$i in collection('c')/newroot let \$a:=\$i/a return (delete node \$i,delete node \$a/..,insert node <newroot/> before \$i,insert node <root/> into \$i/..)" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # test insert + delete (OK) (state: <newroot><a/></newroot>)
    wrap XmlResults resu [$mgr query "for \$i in collection('c')/newroot return (delete node \$i,insert node <root/> before \$i)" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/root" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res

    # test replace node cases (state: <root/>)
    # failures first: delete root
    catch {
	$mgr query "for \$i in collection('c')/root return replace node \$i with ''" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""
    # replace with multiple elements
    catch {
	$mgr query "for \$i in collection('c')/root return replace node \$i with (<a/>,<b/>)" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # replace with element plus text
    catch {
	$mgr query "for \$i in collection('c')/root return replace node \$i with (<a/>,'foo')" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # failed insert of non-whitespace text
    catch {
	$mgr query "for \$i in collection('c') return insert 'foo' into node \$i" $qc
    } exc
    dbxml_error_check_bad $basename $exc ""

    # a successful insert (insert into is an append in BDB XML)
    wrap XmlResults resu [$mgr query "for \$i in collection('c') return insert node <!--comment1--> into \$i" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/comment()" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res

    # successful replacement (state: <root/><!--comment1-->
    wrap XmlResults resu [$mgr query "for \$i in collection('c')/root return replace node \$i with <a/>" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/a" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res

    # successful replacement with text (state: <a/><!--comment1-->)
    wrap XmlResults resu [$mgr query "for \$i in collection('c')/a return replace node \$i with (<b/>,<!--comment-->)" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/b" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res
    wrap XmlResults res [$mgr query "collection('c')/comment()" $qc]
    dbxml_error_check_good "$basename" [$res size] 2
    delete res

    # successful replacement of comment with PI
    wrap XmlResults resu [$mgr query "for \$i in collection('c')/comment() return replace node \$i with <?mypi 'x'?>" $qc]
    delete resu
    wrap XmlResults res [$mgr query "collection('c')/b" $qc]
    dbxml_error_check_good "$basename" [$res size] 1
    delete res
    wrap XmlResults res [$mgr query "collection('c')/processing-instruction()" $qc]
    dbxml_error_check_good "$basename" [$res size] 2
    delete res


    #cleanup
    delete qc
    delete uc
    delete container
    delete mgr
}

#
# Test fn:put() -- added to verify fix to [#16808]
#
proc xml014_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test XQuery Update fn:put()"

    xml_database mgr $testdir $env
    set containerName $basename.dbxml
    set query "let \$d := 'dbxml:/c/foo' return fn:put(<foo></foo>, \$d)"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $containerName $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $containerName $oargs $global_container_type]
    }
    $container addAlias "c"
    wrap XmlQueryContext qc [$mgr createQueryContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $xtxn $query $qc]
        $xtxn commit
        delete xtxn
    } else {
	wrap XmlResults res [$mgr query $query $qc]
    }
    wrap XmlDocument doc [$container getDocument "foo"]
    dbxml_error_check_good $basename [$doc getContentAsString] "<foo/>"
    
    #cleanup
    delete doc
    delete res
    delete qc
    delete container
    delete mgr
}

#
# Test reindexing in the face of mixed content and namespace URIs.
# This tests the fix for [#17226]
#
proc xml014_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test XQuery Update with mixed content indexes"

    xml_database mgr $testdir $env
    set containerName $basename.dbxml
    set docName "doc"
    set content "<foo:item xmlns:foo='http://foo'><a>foo</a><del>delcontent</del></foo:item>"
    set deleteQuery "for \$i in collection('c')/foo:item/del return delete node \$i"
    set verifyQuery "collection('c')/foo:item/del"
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $containerName $oargs $global_container_type]
	$container putDocument $xtxn $docName $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $containerName $oargs $global_container_type]
	$container putDocument $docName $content $uc
    }
    $container addAlias "c"
    # delete
    wrap XmlQueryContext qc [$mgr createQueryContext]
    $qc setNamespace "foo" "http://foo"
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $xtxn $deleteQuery $qc]
        $xtxn commit
        delete xtxn
	delete res
    } else {
	wrap XmlResults res [$mgr query $deleteQuery $qc]
	delete res
    }

    # verify results (no txn)
    wrap XmlResults res [$mgr query $verifyQuery $qc]
    dbxml_error_check_good $basename [$res size] 0
    
    #cleanup
    delete res
    delete qc
    delete uc
    delete container
    delete mgr
}

#
# Test reindexing when deleting a node with an ancestor with a presence
# index. This tests the fix for [#17199]
#
proc xml014_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test XQuery Update delete node with ancestor with a presence index"
    
    xml_database mgr $testdir $env
    set containerName $basename.dbxml
    set docName "doc"
    set content "<root><a/></root>"
    set deleteQuery "delete node collection('c')/root/a"
    set verifyQuery "collection('c')/root"
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $containerName $oargs $global_container_type]
	wrap XmlIndexSpecification is [$container getIndexSpecification $xtxn]
	$is setAutoIndexing false
	$is addIndex "" "root" "node-element-presence-none"
	$container setIndexSpecification $xtxn $is $uc
	$container putDocument $xtxn $docName $content $uc
        $xtxn commit
        delete xtxn
	delete is
    } else {
        wrap XmlContainer container [$mgr createContainer $containerName $oargs $global_container_type]
	wrap XmlIndexSpecification is [$container getIndexSpecification]
	$is setAutoIndexing false
	$is addIndex "" "root" "node-element-presence-none"
	$container setIndexSpecification $is $uc
	$container putDocument $docName $content $uc
	delete is
    }
    $container addAlias "c"
    # delete
    wrap XmlQueryContext qc [$mgr createQueryContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $xtxn $deleteQuery $qc]
        $xtxn commit
        delete xtxn
	delete res
    } else {
	wrap XmlResults res [$mgr query $deleteQuery $qc]
	delete res
    }

    # verify results (no txn)
    wrap XmlResults res [$mgr query $verifyQuery $qc]
    dbxml_error_check_good $basename [$res size] 1
    
    #cleanup
    delete res
    delete qc
    delete uc
    delete container
    delete mgr
}
