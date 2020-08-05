# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml002
# TEST	Container upgrade testing
#
# There is a test matrix required for testing upgrade.
# Here are the "from" container types and releases that
# need to be handled.  Note that upgrade from 1.x releases
# is not supported
# 1. 2.0.x doc storage
# 2. 2.0.x node storage
# 3. 2.0.x node storage with node indexes
#
# 4. 2.1.x doc storage
# 5. 2.1.x node storage
# 6. 2.1.x node storage with node indexes
#
# 7. 2.2.8 doc storage
# 8. 2.2.8 doc storage with node indexes  (this was added in 2.2)
# 9. 2.2.8 node storage
# 10. 2.2.8 node storage with node indexes
#
# 11. 2.2.x doc storage
# 12. 2.2.x doc storage with node indexes
# 13. 2.2.x node storage
# 14. 2.2.x node storage with node indexes
#
# TBD: create containers

proc xml002 { {args ""} } {
    puts "\nXml002: Upgrade testing ($args)"
    source ./include.tcl

#    set cdbindex [lsearch -exact $args "-cdb"]
#    if { $cdbindex > 0 } {
#	puts "skipping upgrade for CDB due to DB bug (#14185)"
#	return
#    }
    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 2
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

    # no upgrade required
    set containers_2_3 {
	"2.3_doc"
	"2.3_doc_ni"
	"2.3_node"
	"2.3_node_ni"
    }

    # upgrade required
    # NOTE: 2.2.8 has been left out.  2 reasons:
    #  1.  I didn't have it handy
    #  2.  It's not very widespread
    # It should probably be added.  If it is, it includes
    # support for the doc_ni variant -- it's the first release to 
    # do so.
    set containers_2_2 {
	"2.2.13_doc"
	"2.2.13_doc_ni"
	"2.2.13_node"
	"2.2.13_node_ni"
    }
    set containers_2_1 {
	"2.1.8_doc"
	"2.1.8_node"
	"2.1.8_node_ni"
    }
    set containers_2_0 {
	"2.0.9_doc"
	"2.0.9_node"
	"2.0.9_node_ni"
    }

    set basename $tnum
    set id 0

    xml002_upgrade $env $txnenv $basename.[incr id] $oargs "2.0.9" $containers_2_0
    xml002_upgrade $env $txnenv $basename.[incr id] $oargs "2.1.8" $containers_2_1
    xml002_upgrade $env $txnenv $basename.[incr id] $oargs "2.2.13" $containers_2_2 1
    xml002_no_upgrade $env $txnenv $basename.[incr id] $oargs "2.3" $containers_2_3
    xml002_misc $env $txnenv $basename.[incr id] $oargs
}

proc xml002_upgrade { {env "NULL"} {txnenv 0} {basename $tnum} oargs version containers {index_test 0} } {
    source ./include.tcl
    puts "\t$basename: Basic upgrade from $version."
    xml_cleanup $testdir $env

    # where to find containers
    set cdir "$test_path/data_set_2"
    set zipfile "containers.zip"
    set UNZIP unzip

    # test content.  Each upgraded container has a document
    # named "doc1" with the following content, which is verified
    set doc_name "doc1"
    set doc_content "<root><my:name xmlns:my='my://myuri'>joe</my:name><age>45</age></root>"
    set doc_content_node "<root><my:name xmlns:my=\"my://myuri\">joe</my:name><age>45</age></root>"

    xml_database mgr $testdir $env
    wrap XmlUpdateContext uc [$mgr createUpdateContext]	

    set id 0

    # test containers that should require upgrade
    foreach container $containers {
	incr id
	puts "\t\t$basename.$id: Container $container"

	# containers are kept in a .zip file.
	# On Unix unzip them; on windows, do it manually.
	# Could unzip directly to TESTDIR, but that's harder
	# for windows, so unzip to data_set_2, and copy.
	if { $is_windows_test } {
	    # check for existence of one of the containers
	    if { [ file exists $cdir/$container ] == 0 } {
		puts "FAIL: Please unzip the file, $cdir/$zipfile into $cdir and re-run $basename"
		return 0
	    }
	} else {
	    exec $UNZIP -n $cdir/$zipfile -d $cdir
	}

	# copy container to $testdir
	set dest $testdir/$container
	file copy $cdir/$container $dest

	# attempt to open the container, verify an exception
	catch {
	    wrap XmlContainer cont [$mgr openContainer $container]
	} ret
	dbxml_error_check_good "$basename.$id" [string match "Error*Upgrade*" $ret] 1

	# upgrade the container
	$mgr upgradeContainer $container $uc

	# open the container to verify upgrade
	wrap XmlContainer cont [$mgr openContainer $container]

	# get a document, and verify that it can be read
	wrap XmlResults res [$cont getAllDocuments 0]
	new XmlValue xv
	$res next $xv
	# content must be non-zero in length
	set content [$xv asString]
	dbxml_error_check_bad "$basename.$id" [string length $content] 0

	# Check that indices of type anyURI, QName and NOTATION
	# have been upgraded to string correctly
	if { $index_test } {
	    wrap XmlIndexSpecification xis [$cont getIndexSpecification]

	    dbxml_error_check_good index [$xis getDefaultIndex] "node-element-equality-string"

	    wrap XmlIndexDeclaration xid [$xis find "http://foo.com" "mavis"]
	    dbxml_error_check_good index [$xid get_index] "edge-attribute-equality-string"
	    delete xid

	    wrap XmlIndexDeclaration xid [$xis find "" "frank"]
	    dbxml_error_check_good index [$xid get_index] "node-metadata-equality-string"
	    delete xid

	    delete xis
	}

	delete xv
	delete res

	# get a specific document, and verify its content
	wrap XmlDocument doc [$cont getDocument $doc_name]
	set content [$doc getContent]
	if { [string match "*doc*" $container] } {
	    dbxml_error_check_good "$basename" $content $doc_content
	} else {
	    dbxml_error_check_good "$basename" $content $doc_content_node
	}

	delete doc

	# close container
	delete cont

	# Now, test compaction, by compacting the upgraded container
	# ignore exceptions caused by older DB releases
	catch {
	    $mgr compactContainer $container $uc
	} ret

	# re-open the container to verify upgrade
	wrap XmlContainer cont [$mgr openContainer $container]

	# get a document, and verify that it can be read
	wrap XmlResults res [$cont getAllDocuments 0]
	new XmlValue xv
	$res next $xv
	# content must be non-zero in length
	set content [$xv asString]
	dbxml_error_check_bad "$basename.$id" [string length $content] 0

	delete xv
	delete res

	# close container
	delete cont

	# One more time -- test truncate
	$mgr truncateContainer $container $uc

	# re-open the container to verify upgrade
	wrap XmlContainer cont [$mgr openContainer $container]

	# verify that there are no documents
	wrap XmlResults res [$cont getAllDocuments 0]
	new XmlValue xv
	dbxml_error_check_good "$basename.$id" [$res next $xv] 0

	delete xv
	delete res

	# close container
	delete cont


	# cleanup -- remove the container from $testdir
	$mgr removeContainer $container

    }

    # clean up
    delete uc
    delete mgr
}

proc xml002_no_upgrade { {env "NULL"} {txnenv 0} {basename $tnum} oargs version containers } {
    source ./include.tcl
    puts "\t$basename: Upgrade not required."
    xml_cleanup $testdir $env

    # where to find containers
    set cdir "$test_path/data_set_2"
    set zipfile "containers.zip"
    set UNZIP unzip

    xml_database mgr $testdir $env
#    xml_database mgr $testdir "NULL"
    wrap XmlUpdateContext uc [$mgr createUpdateContext]	

    set id 0

    # test containers that should require upgrade
    foreach container $containers {
	incr id
	puts "\t\t$basename.$id: Container $container"

	# containers are kept in a .zip file.
	# On Unix unzip them; on windows, do it manually.
	# Could unzip directly to TESTDIR, but that's harder
	# for windows, so unzip to data_set_2, and copy.
	if { $is_windows_test } {
	    # check for existence of one of the containers
	    if { [ file exists $cdir/$container ] == 0 } {
		puts "FAIL: Please unzip the file, $cdir/$zipfile into $cdir and re-run $basename"
		return 0
	    }
	} else {
	    exec $UNZIP -n $cdir/$zipfile -d $cdir
	}

	# copy container to $testdir
	set dest $testdir/$container
	file copy $cdir/$container $dest

	# open the container to verify no problems
	wrap XmlContainer cont [$mgr openContainer $container]

	# get a document, and verify that it can be read
	wrap XmlResults res [$cont getAllDocuments 0]
	new XmlValue xv
	$res next $xv
	# content must be non-zero in length
	set content [$xv asString]
	dbxml_error_check_bad "$basename.$id" [string length $content] 0

	delete xv
	delete res
	delete cont

	# cleanup -- remove the container from $testdir
	file delete $dest
    }

    # clean up
    delete uc
    delete mgr
}

# miscellaneous upgrade tests
proc xml002_misc { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\tXml002_misc: Miscellaneous upgrade tests"

    xml002_misc_1 $env $txnenv $basename.1 $oargs
    #xml002_misc_2 $env $txnenv $basename.2 $oargs
}

proc xml002_misc_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\tXml002_misc_1: old (pre-2.4) substring indexes"
    xml_cleanup $testdir $env
    xml_database mgr $testdir $env

    # ignore transactions for now.  This test just addresses
    # [#16135]
    set cname "2.3_node_ni_substring"
    set dname "doc"
    set query "collection('2.3_node_ni_substring')/root/surname\[contains(.,'nam')\]"
    # copy required container(s)
    set cdir "$test_path/data_set_2"
    file copy $cdir/$cname $testdir/$cname

    wrap XmlContainer container [$mgr openContainer $cname]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    # NOTE: inserting a new node results in reindexing that
    # makes the problem go away so do not do that here

    # remove the document (this would fail with the bug)
    $container deleteDocument $dname $uc

    # make sure it disappeared
    wrap XmlResults res [$mgr query $query $qc]
    dbxml_error_check_good $basename [$res size] 0
    delete res

    delete qc;
    delete uc;
    delete container;
    delete mgr;

    file delete $cname;
}
