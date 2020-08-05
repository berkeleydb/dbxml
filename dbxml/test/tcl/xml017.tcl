# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# TEST	xml017
# TEST	Input Streams

proc xml017 { args } {
    puts "\nxml017: Input Streams ($args)"
    source ./include.tcl

    set tnum 17

    # Determine whether procedure has been called within an environment,
    # and whether it is a transactional environment.
    # If we are using an env, then the filename should just be the test
    # number.  Otherwise it is the test directory and the test number.
    set eindex [lsearch -exact $args "-env"]
    set cdbindex [lsearch -exact $args "-cdb"]
    set txnenv 0
    set cdbenv ""
    set oargs [eval {set_openargs} $args]

    if { $cdbindex != -1 } {
	set cdbenv "--cdb"
    }
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

    # local file
    xml017_1 $env $txnenv $cdbenv $basename.1 $oargs

    # URL
    xml017_2 $env $txnenv $cdbenv $basename.2 $oargs

    # Memory Buffer
    xml017_3 $env $txnenv $cdbenv $basename.3 $oargs

    return
}

# local file as an input stream
#   - Tcl code is used for basic failure case of file not existing and
#       for basic creation and initial seek
#   - C++ code is used for more complex cases, and also for XmlContainer and
#       XmlDocument methods that take an input stream
proc xml017_1 { {env "NULL"} {txnenv 0} {cdbenv ""} basename oargs } {
    puts "\t$basename: Local file"
    source ./include.tcl

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    set id 0

    # failure case - file does not exist
    #  the stream will be created, but complain when it is used
    set testid $basename.[incr id]
    puts "\t\t$testid: File not existing (failure case)"
    set file [file join $test_path document_set_17 no_such_file.xml]
    set xis [$db createLocalFileInputStream $file]
    catch {
        set pos [$xis curPos]
    } ret
    dbxml_error_check_good $testid [string match "*Error*stream*" $ret] 1
    $xis freeMemory

    # success case - create the input stream and confirm start position
    set testid $basename.[incr id]
    puts "\t\t$testid: Create / Start Position"
    set file [file join $test_path document_set_17 control.xml]
    set xis [$db createLocalFileInputStream $file]

    set pos [$xis curPos]
    dbxml_error_check_good "$testid  Initial position" $pos 0

    $xis freeMemory

    # call C++ program to test manipulation of file based input streams
    #  - unit test of local file input stream
    #  - container and document methods that use input stream
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Manipulation of input streams"
    set program "dbxml_test_input_streams"
    set args "--env $testdir --stream file --file $file $cdbenv"

    # clean up
    delete db

    run_external_test_program $txnenv $testid $program $args
}

# URL as an input stream
#   - Tcl code is used for basic failure case of file not existing and
#       for basic creation and initial seek
#   - C++ code is used for more complex cases, and also for XmlContainer and
#       XmlDocument methods that take an input stream
proc xml017_2 { {env "NULL"} {txnenv 0} {cdbenv ""} basename oargs } {
    puts "\t$basename: URL"
    source ./include.tcl
    set txn NULL

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlQueryContext context [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn "$basename.1.dbxml" $oargs]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer "$basename.1.dbxml" $oargs]
    }
    set id 0

    # failure case - file-based URL does not exist
    #  - the stream will be created, but complain when it is used
    set testid $basename.[incr id]
    # construct a common baseId
    set fileDir [file join $test_path document_set_17]
    set fileDir [file normalize $fileDir]
    # ensure leading / for Windows
    set c [string index $fileDir 0]
    if { $c != "/" } {
	set fileDir /$fileDir
    }
    set base_id "file://$fileDir/"

    puts "\t\t$testid: File (system id) not existing (failure case)"
    set system_id "no_such_file.xml"
    set xis [$db createURLInputStream $base_id $system_id]
    catch {
        set pos [$xis curPos]
    } ret
    dbxml_error_check_good $testid [string match "*Error*stream*" $ret] 1
    $xis freeMemory

    # success case - create the input stream and confirm start position
    #  - no public ID specified
    set testid $basename.[incr id]
    puts "\t\t$testid: Create / Start Position (no public id)"

    # re-use base_id
    set system_id "control.xml"
    set xis [$db createURLInputStream $base_id $system_id]

    set pos [$xis curPos]
    dbxml_error_check_good "$testid  Initial position" $pos 0

    $xis freeMemory

    # call C++ program to test manipulation of file based input streams
    #  - unit test of URL input stream
    #  - container and document methods that use input stream
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Manipulation of input streams"
    set program "dbxml_test_input_streams"
    set args "--env $testdir --stream url --base $base_id --file $system_id $cdbenv"
    # clean up
    delete container
    delete context
    delete db

    run_external_test_program $txnenv $testid $program $args
}

# Memory buffer  as an input stream
#   - C++ code is used throughout due to problems with memory buffers
#      in the Tcl - C++ bindinfs
#   - C++ code also tests XmlContainer and XmlDocument methods that take an
#     input stream
proc xml017_3 { {env "NULL"} {txnenv 0} {cdbenv ""} basename oargs } {
    puts "\t$basename: Memory buffer"
    source ./include.tcl
    set txn NULL

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlQueryContext context [$db createQueryContext]

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn "$basename.1.dbxml" $oargs]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer "$basename.1.dbxml" $oargs]
    }
    set id 0

    # call C++ program to test manipulation of memory buffer input streams
    #  - unit test of input stream
    #  - container methods that use input stream
    set testid $basename
    set program "dbxml_test_input_streams"
    set args "--env $testdir --stream membuf $cdbenv"

    # clean up
    delete container
    delete context
    delete db

    run_external_test_program $txnenv $testid $program $args
}

