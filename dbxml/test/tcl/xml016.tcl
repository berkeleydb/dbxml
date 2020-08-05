# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml016
# TEST	XmlEvent*
# TEST	Test use, and abuse of the XmlEvent* classes

proc xml016 { args } {
    source ./include.tcl
    puts "\nxml016: XmlEvent* ($args)"
    
    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set txnenv 0
    set tnum 16
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
    
    xml016_1 $env $txnenv $basename.1 $oargs
    xml016_2 $env $txnenv $basename.2 $oargs
    xml016_3 $env $txnenv $basename.3 $oargs
}

#
# basic XmlEventReader
#
proc xml016_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test basic XmlEventReader"

    xml_database mgr $testdir $env

    set docname "doc"
    set newname "doc1"
    set content "<root><a a1=\"val1\">a text</a><b><c cattr=\"c1\"><d/></c></b></root>"
    set isCDS 0
    if {$txnenv == 1} {
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
    } else {
	# check for CDS -- not transactional, but has env
	if {$env != "NULL"} {
	    set isCDS 1
	}
    }

    # insert a document
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        $container putDocument $txn $docname $content $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $docname $content $uc
    }

    # get the document
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlDocument doc [$container getDocument $txn $docname]
    } elseif {$isCDS == 1} {
	set dbtxn [$env cdsgroup]
	wrap XmlTransaction txn [$mgr createTransaction $dbtxn]
	wrap XmlDocument doc [$container getDocument $txn $docname]
    } else {
	wrap XmlDocument doc [$container getDocument $docname]
    }
    # get the content as event reader
    set reader [$doc getContentAsEventReader]

    # walk the events, and close the reader
    while { [$reader hasNext] } {
	set etype [$reader next]
    }
    dbxml_error_check_good "$basename" $etype $XmlEventReader_EndDocument
    $reader close

    # get content as reader again, but this time by way of
    # XmlValue.asEventReader
    new XmlValue docVal $doc
    set reader1 [$docVal asEventReader]
    wrap XmlDocument newdoc [$mgr createDocument]
    $newdoc setName $newname
    $newdoc setContentAsEventReader $reader1

    if {$txnenv == 1 || $isCDS == 1} {
        $container putDocument $txn $newdoc $uc
	wrap XmlDocument doc1 [$container getDocument $txn $newname]
    } else {
        $container putDocument $newdoc $uc
	wrap XmlDocument doc1 [$container getDocument $newname]
    }

    set newContent [$doc1 getContent]
    dbxml_error_check_good $basename $newContent $content
    if {$txnenv == 1 || $isCDS == 1} {
	$txn commit
	delete txn
    }

    delete doc
    delete newdoc
    delete doc1
    delete container
    delete mgr
}

#
# basic XmlEventWriter
#
proc xml016_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test basic XmlEventWriter"
    
    xml_database mgr $testdir $env
    
    set docname "doc1"
    set docname2 "doc2"
    set expected "<root><a a1=\"val1\">a text</a><b><c cattr=\"c1\"><d/></c>b textplus</b></root>"
    
    set isCDS 0
    if {$txnenv == 1} {
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
    } else {
	# check for CDS -- not transactional, but has env
	if {$env != "NULL"} {
	    set isCDS 1
	}
    }


    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlDocument doc [$mgr createDocument]
    $doc setName $docname
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        set writer [$container putDocumentAsEventWriter $txn $doc $uc]
    } elseif {$isCDS == 1} {
	set dbtxn [$env cdsgroup]
	wrap XmlTransaction txn [$mgr createTransaction $dbtxn]
        set writer [$container putDocumentAsEventWriter $txn $doc $uc]
    } else {
	set writer [$container putDocumentAsEventWriter $doc $uc]
    }
    $writer writeStartDocument "" "" ""
    $writer writeStartElement "root" "" "" 0 false
    $writer writeStartElement "a" "" "" 1 false
    $writer writeAttribute "a1" "" "" "val1" true
    $writer writeText $XmlEventReader_Characters "a text" 6
    $writer writeEndElement "a" "" ""
    $writer writeStartElement "b" "" "" 0 false
    $writer writeStartElement "c" "" "" 1 false
    $writer writeAttribute "cattr" "" "" "c1" true
    $writer writeStartElement "d" "" "" 0 true
    $writer writeEndElement "c" "" ""
    $writer writeText $XmlEventReader_Characters "b text" 6
    $writer writeText $XmlEventReader_Characters "plus" 4
    $writer writeEndElement "b" "" ""
    $writer writeEndElement "root" "" ""
    $writer writeEndDocument 
    $writer close
    if {$txnenv == 1 || $isCDS == 1} {
        $txn commit
        delete txn
    }
    delete doc

    # try an exception case
    wrap XmlDocument doc [$mgr createDocument]
    $doc setName $docname2
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        set writer [$container putDocumentAsEventWriter $txn $doc $uc]
    } elseif {$isCDS == 1} {
	set dbtxn [$env cdsgroup]
	wrap XmlTransaction txn [$mgr createTransaction $dbtxn]
        set writer [$container putDocumentAsEventWriter $txn $doc $uc]
    } else {
	set writer [$container putDocumentAsEventWriter $doc $uc]
    }
    $writer writeStartDocument "" "" ""
    $writer writeStartElement "root" "" "" 0 false
    catch {
	$writer writeText $XmlEventReader_Characters "abc" 4
    } ret
    $writer close
    dbxml_error_check_good $basename \
	[is_substr $ret "Invalid string length"] 1
    if {$txnenv == 1} {
	$txn abort
	delete txn
    }
    delete doc
    # make sure document wasn't created
    catch {
	wrap XmlDocument doc [$container getDocument $docname2]
    } ret
    dbxml_error_check_good $basename \
	[is_substr $ret "Document not found"] 1

    # verify original content
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlDocument doc [$container getDocument $txn $docname]
	set content [$doc getContent]
	$txn commit
	delete txn
    } else {
	wrap XmlDocument doc [$container getDocument $docname]
	set content [$doc getContent]
    }
    dbxml_error_check_good $basename $content $expected
    delete doc
    delete container
    delete mgr
}

proc xml016_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: XmlEventReader from XmlValue and nextTag iteration"

    xml_database mgr $testdir $env

    set docname "doc"
    set newname "doc1"
    set content "<root><a>1</a><a>2</a><a>3</a></root>"
    set isCDS 0
    if {$txnenv == 1} {
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
    } else {
	# check for CDS -- not transactional, but has env
	if {$env != "NULL"} {
	    set isCDS 1
	}
    }

    # insert a document
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlQueryContext qc [$mgr createQueryContext]
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
        $container putDocument $txn $docname $content $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $docname $content $uc
    }

    # query
    set query "collection('[$container getName]')/root"
    if {$txnenv == 1} {
        wrap XmlTransaction txn [$mgr createTransaction]
	wrap XmlResults res [$mgr query $txn $query $qc]
    } elseif {$isCDS == 1} {
	set dbtxn [$env cdsgroup]
	wrap XmlTransaction txn [$mgr createTransaction $dbtxn]
	wrap XmlResults res [$mgr query $txn $query $qc]
    } else {
	wrap XmlResults res [$mgr query $query $qc]
    }

    # get the result as event reader
    dbxml_error_check_good "$basename result size" [$res size] 1
    new XmlValue xv
    $res next $xv
    set reader [$xv asEventReader]

    # walk the events, test and close the reader
    set val 1
    while { [$reader hasNext] } {
	set etype [$reader nextTag]
	if { $etype == $XmlEventReader_StartElement} {
	    set lname [$reader getLocalName]
	    if { $lname == "a" } {
		$reader next
		set lval [$reader getValue]
		dbxml_error_check_good "$basename iter value" $lval $val
		incr val
	    }
	}
    }
    $reader close

    if {$txnenv == 1 || $isCDS == 1} {
	$txn commit
	delete txn
    }

    delete xv
    delete res
    delete container
    delete mgr
}

