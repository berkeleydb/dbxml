# See the file LICENSE for redistribution information
#
# Copyright (c) 2002,2009 Oracle.  All rights reserved.
#
#

source ./include.tcl

load $db_tcllib
load $dbxml_tcllib

if { [file exists $testdir] != 1 } {
	file mkdir $testdir
}

#
# Test if utilities work to figure out the path.  Most systems
# use ., but QNX has a problem with execvp of shell scripts which
# causes it to break.
#
set stat [catch {exec $db_util_path/db_printlog -?} ret]
if { [string first "exec format error" $ret] != -1 } {
	set util_path $db_util_path/.libs
} else {
	set util_path $db_util_path
}

#
# Test if DB XML utilities work to figure out their path.
#
set stat [catch {exec $xmlutil_path/dbxml_dump -?} ret]
if { [string first "exec format error" $ret] != -1 } {
	set xmlutil_path $xmlutil_path/.libs
}

# Xmlparams.tcl is where the list of XML tests is maintained
# and where those tests and other files needed for XML testing
# are sourced.

source $test_path/xmlparams.tcl

# Set up any OS-specific values
global tcl_platform
set is_windows_test [is_substr $tcl_platform(os) "Win"]

# Set variables required to use standard testutils, even if they
# never get exercised by XML.
set dict $db_test_path/wordlist
set encrypt 0
set old_encrypt 0
set passwd test_passwd
set is_qnx_test 0
set is_je_test 0
set gen_upgrade 0
set ohandles {}

set dbxml_err 0
set dbxml_pending 0

# Error stream that (should!) always go to the console, even if we're
# redirecting to ALL.OUT.
set consoleerr stderr

# execute the entire test suite (all combinations of storage types and
# environments)
#   output is written to ALL.OUT in the cwd

proc run_xml { {test_list ""} } {
    global short_xml_test_names
    global all_xml_test_names
    global external_xml_test_names

    if { $test_list == "short" } {
	run_xml_internal $short_xml_test_names
    } elseif { $test_list == "external" } {
	run_xml_internal $external_xml_test_names
    } else {
	run_xml_internal $all_xml_test_names
    }
}

proc run_xml_internal { test_names } {
    source ./include.tcl

    fileremove -f ALL.OUT

    puts "going to run these tests:"
    foreach test $test_names {
	puts "$test"
    }

    set o [open ALL.OUT a]
    puts -nonewline "XML test run started at: "
    puts [clock format [clock seconds] -format "%H:%M %D"]
    puts [dbxml_version]
    puts -nonewline $o "XML test run started at: "
    puts $o [clock format [clock seconds] -format "%H:%M %D"]
    puts $o [dbxml_version]
    close $o

    # iterate over each of the storage types
    list set container_types {}
    lappend container_types $XmlContainer_WholedocContainer
    lappend container_types $XmlContainer_NodeContainer

    list set indexnodes_types {}
    lappend indexnodes_types 1
    lappend indexnodes_types 0

    foreach type $container_types {
        foreach indexnodes $indexnodes_types {
	    set o [open ALL.OUT a]
	    puts $o "**************************************************"
	    puts $o "**************************************************"
            if { $type == $XmlContainer_WholedocContainer } {
                puts -nonewline "Using whole document storage"
                puts -nonewline $o "Using whole document storage"
            } elseif { $type == $XmlContainer_NodeContainer } {
                puts -nonewline "Using node level storage"
                puts -nonewline $o "Using node level storage"
            }
            if { $indexnodes == 1} {
                puts " with indexed nodes"
                puts $o " with indexed nodes"
            } else {
                puts ""
                puts $o ""
            }

            # Run with no environment
            puts "  Running tests with no environment (DS)"
            puts $o "  Running tests with no environment (DS)"
	    close $o
            foreach test $test_names {
                if [catch {exec $tclsh_path \
                               << "source $test_path/test.tcl; run_xmltest $test {} $type $indexnodes" \
                               >>& ALL.OUT } res] {

                    set o [open ALL.OUT a]
                    puts $o "FAIL: $test: $res"
                    close $o

                    # test group is abandoned - forcibly clean up test environment
                    fileremove -f $testdir
                    file mkdir $testdir
                }
            }
	    set o [open ALL.OUT a]
            # Run under run_tdsmethod (transactional environment)
            puts "  Running tests in a transactional environment (TDS)"
            puts $o "  Running tests in a transactional environment (TDS)"
	    close $o
            foreach test $test_names {
                if [catch {exec $tclsh_path \
                               << "source $test_path/test.tcl; run_tdsmethod $test {} $type $indexnodes" \
                               >>& ALL.OUT } res] {
                    set o [open ALL.OUT a]
                    puts $o "FAIL: run_tdsmethod $test: $res"
                    close $o

                    # test group is abandoned - forcibly clean up test environment
                    fileremove -f $testdir
                    file mkdir $testdir
                }
            }

            # Run under run_cdsmethod (locking environment)
	    set o [open ALL.OUT a]
            puts "  Running tests in a concurrent environment (CDS)"
            puts $o "  Running tests in a concurrent environment (CDS)"
	    close $o
            foreach test $test_names {
                if [catch {exec $tclsh_path \
                               << "source $test_path/test.tcl; run_cdsmethod $test {} $type $indexnodes" \
                               >>& ALL.OUT } res] {
                    set o [open ALL.OUT a]
                    puts $o "FAIL: run_cdsmethod $test: $res"
                    close $o

                    fileremove -f $testdir
                    file mkdir $testdir
                }
            }

            # Run under run_tdsmethod with -thread
            # TODO: test whether berkdb_env supports -thread
            #puts "Running tests in a threaded environment"
            #foreach test $test_names(xml) {
            #	if [catch {exec $tclsh_path \
                #	    << "source $test_path/test.tcl; \
                #	    run_tdsmethod $test thread $XmlContainer_WholedocContainer 0" \
                #	    >>& ALL.OUT } res] {
            #		set o [open ALL.OUT a]
            #		puts $o "FAIL: run_tdsmethod $test thread: $res"
            #		close $o
            #	}
            #}
        }
    }

    # end of test run - now to analyse results...
    set failed 0
    set pending 0
    set warning 0
    check_test_run ALL.OUT failed pending warning

    set o [open ALL.OUT a]
    if { $failed == 0 } {
        puts "Regression Tests Succeeded"
        puts $o "Regression Tests Succeeded"
        set err 0
    } else {
        puts "Regression Tests Failed ($failed); see ALL.OUT for log"
        puts $o "Regression Tests Failed ($failed)"
        set err 1
    }
    if { $pending } {
        puts "Warning - $pending test(s) are pending; see ALL.OUT for log"
        puts $o "Warning - $pending  test(s) are pending; see ALL.OUT for log"
    }
    if { $warning } {
        puts "Warning - $warning test(s) are marked as WARNING; see ALL.OUT for log"
        puts $o "Warning - $warning test(s) are marked as WARNING; see ALL.OUT for log"
    }

    check_commented_out_tests

    puts -nonewline "Test suite run completed at: "
    puts [clock format [clock seconds] -format "%H:%M %D"]
    puts -nonewline $o "Test suite run completed at: "
    puts $o [clock format [clock seconds] -format "%H:%M %D"]
    close $o

    return $err
}

proc run_xqts { { path_to_test_suite 0 } { use_nls { 1 0 } } { indexing { node document none } } } {
    source ./include.tcl

    if { ![string compare $path_to_test_suite ?]} {
        puts ""
        puts "Usage:  run_xqts path_to_test_suite \{1|0\} \{node|document|none\}"
        puts ""
        puts "1 specifies NLS, 0 specifies DLS."
        puts ""
        puts "node, document, and none are the indexing types to use."
        puts ""
        return
    }

    set fail 0
    foreach i $indexing {
        foreach u $use_nls {
            set args ""

            puts ""
            puts "\#\#\#\#\#\#\#\#\#\#\#\#\#"
            if { $u == 1 } {
                puts -nonewline "\# Node Level"
                set storage "NLS"
            } else {
                puts -nonewline "\# Whole Document"
                set storage "DLS"
            }
            puts -nonewline " Storage - "

            if { ! [string compare $i "node"] } {
                puts -nonewline "Node Indexing"
                set storage "$storage+"
                set args "-i"
            } else {
                if { ! [string compare $i "document"] } {
                    puts -nonewline "Document Indexing"
                    set args "-i"
                } else {
                    puts -nonewline "Not Indexed"
                }
            }
            puts ""
            puts "\#\#\#\#\#\#\#\#\#\#\#\#\#"

            set args [concat $args "-s $storage"]
            set args [concat $args "-h $testdir"]
            set args [concat $args "-e"]
            set args [concat $args [file join $test_path "xqts-errors.xml"]]
            set args [concat $args $path_to_test_suite]

            set prog [file join "." "dbxml_xqts_runner"]

            # forcibly remove the TESTDIR to avoid inconsistent state
            fileremove -f $testdir
            file mkdir $testdir

            # puts "$xmltest_path/$prog $args 2>> xqts_tests.err"

            catch {
                set output [open "|$xmltest_path/$prog $args 2>> xqts_tests.err" r]
                while { [eof $output] != 1 } {
                    puts -nonewline [read $output 1]
                    flush stdout
                }
                close $output
            } ret;

            if { [string compare $ret ""] } {
                set fail 1
                dbxml_error_check_good "run_xqts $args" $ret ""
            }
        }
    }

    summary $fail
}

proc run_all { { id { 0 } } { env { t n } } { use_nls { 1 0 } } { index_nodes { 1 0 } } { nofork 1 } } {
    source ./include.tcl

    if { ![string compare $id ?]} {
        puts ""
        puts "Usage:  run_all \{ids\} \{t|n\} \{1|0\}"
        puts ""
        puts "\"ids\" is a list of test groups (use \"help\" to find out more about the test groups)."
        puts "If \"id\" is omitted (or is 0) then all test groups are run."
        puts ""
        puts "If \"n\" is specified then a NULL environment is used. Otherwise a transacted environment is used. This switch is reversed for Group 3, which only runs in a non-transacted environment."
        puts ""
        return
    }

    set fail 0
    foreach i $id {
        foreach e $env {
            foreach u $use_nls {
                foreach n $index_nodes {
                    puts ""
                    puts "\#\#\#\#\#\#\#\#\#\#\#\#\#"
                    if { $u == 1 } {
                        puts -nonewline "\# Node Level"
                    } else {
                        puts -nonewline "\# Whole Document"
                    }
                    puts -nonewline " Storage - "

                    if { ! [string compare $e "t"] } {
                        puts -nonewline "Transacted"
                    } else {
                        puts -nonewline "Non-transacted"
                    }
                    puts -nonewline " mode"
                    if { $n == 1 } {
                        puts -nonewline " - Nodes Indexed"
                    }
                    puts ""
                    puts "\#\#\#\#\#\#\#\#\#\#\#\#\#"

                    set fail [expr $fail || [run $i $e $u $n $nofork 0]]
                }
            }
        }
    }

    summary $fail
}

# execute a specific test group in the XML subsystem, transacted env option
#   uses node level storage, wraps "run"
proc run_nls { { id 0 } {env t } { nofork 1 } } {
	source ./include.tcl
  run $id $env 1 0 $nofork
  run $id $env 1 1 $nofork
}

# execute a specific test group in the XML subsystem (transacted environment)
#   allows quick testing of a particular feature
#   summary pass/fail message printed
proc run { { id 0 } {env t } {use_nls 0} {index_nodes 0} { nofork 1 } { output_summary 1 } } {
    source ./include.tcl
    global test_names
    global errorCode
    global all_xml_test_names
    # "use_nls" is a ghost option (only used internally)
    set storage_type $XmlContainer_WholedocContainer
    if { $use_nls } {
        set storage_type $XmlContainer_NodeContainer
    }

    # help option (this could be automated, i.e. read file headers...)
    if { ![string compare $id ?]} {
        puts ""
        puts "Usage:  run \[id\] \[t|c|n\]"
        puts "        run_nls \[id\] \[t|c|n\]"
        puts ""
        puts "\"id\" is a test group (use \"help\" to find out more about the test groups)."
        puts "If \"id\" is omitted (or is 0) then all test groups are run."
        puts ""
        puts "If \"n\" is specified then a NULL environment is used. If \"c\" is specified, a CDS environment is used.  Otherwise a transacted environment is used. This switch is reversed for Group 3, which only runs in a non-transacted environment."
        puts ""
        puts "The 'dbxml_dump' and 'dbxml_load' programs (plus 'db_dump') are run at the end of each test group, but only for a NULL environment."
        puts ""
	puts "Also available:"
	puts "run_xmltest|run_tdsmethod|run_cdsmethod id args storage_type index_nodes"
	puts ""
	puts "id is test number (e.g. xml011), args are ??? storage_type (0 or 1)"
	puts "where 1 is node, index_nodes 0 (false) or 1"
	puts ""
	puts "run_envmethod id 'cdb|txn' args storage_type index_nodes"
        return
    }

    # forcibly clean up test environment (in case something went wrong that
    # left env in a bad state)
    fileremove -f $testdir
    file mkdir $testdir

    # attempt to execute specific group if $id is set, all groups otherwise
    set group [format "xml%03d" $id]
    set found 0
    set fail 0
    foreach test $all_xml_test_names {

        if { !$id || ! [string compare $test $group] } {
            set found 1

            # swap environments for group 3 since this group only runs in a
            # non-transacted environment
            if { ! [string compare $env n] } {
		puts "Running in a DS (non-transacted) environment"
                # NULL environment
                if { [string match "*03" $test] || $id == 3} {
                    set run_method run_tdsmethod
                } else {
                    set run_method run_xmltest
                }
            } elseif { ! [string compare $env c] } {
		puts "Running in a CDS environment"
                if { [string match "*03" $test] || $id == 3} {
                    set run_method run_tdsmethod
                } else {
                    set run_method run_cdsmethod
                }
            } else {
		puts "Running in a TDS (transacted) environment"
                if { [string match "*03" $test] || $id == 3} {
                    set run_method run_xmltest
                } else {
                    set run_method run_tdsmethod
                }
            }

            if { $nofork == 0 } {
                # execute procedure in a separate process (empty arg intentional
                #  - need to adapt to support threading)
                set err 1
                catch {
                    puts [exec $tclsh_path \
                              << "source $test_path/test.tcl; $run_method $test \"\" $storage_type $index_nodes"]
                    set err 0
                } ret

                if { $err } {
                    # flush output (redirected to 'ret')
                    puts $ret

                    # anything written to stderr is treated as a failure so check
                    # for false alarms by looking at the global errorCode
                    if { ! [string equal $errorCode NONE] } {
                        puts "FAIL ($errorCode)"
                        set fail 1
                    }
                }
            } else {
                set args {}
                if {[catch {$run_method $test $args $storage_type $index_nodes} res] != 0 } {
                    puts $res
                    set fail 1
                }
            }
        }
    }
    if { ! $found } {
        puts "Error - test group \"$group\" is not defined in \"\$all_xml_test_names\" (xmlparams.tcl)"
    }

    # summarise status
    if { $output_summary == 1 } {
        summary $fail
    }

    return [expr $fail || $dbxml_err]
}

proc summary { fail } {
    source ./include.tcl

    puts "\n\n"
    puts "======================================================================="
    if { $fail || $dbxml_err} {
        puts "FAILURES ($dbxml_err) - check the output above for lines started with \"FAIL\""
    } else {
        puts "SUCCESS - tests pass"
    }
    if { $dbxml_pending } {
        puts "\nPENDING - $dbxml_pending tests are pending code fixes (lines starting with \"PENDING\")"
    }
    puts "======================================================================="
    check_commented_out_tests
}

proc check_failed_run { file {text "^FAIL"}} {
	set failed 0
	set o [open $file r]
	while { [gets $o line] >= 0 } {
		set ret [regexp $text $line]
		if { $ret != 0 } {
			set failed 1
		}
	}
	close $o

	return $failed
}

# checks file for failures and warnings
proc check_test_run { file failed_out pending_out warning_out {fail_text "^FAIL"} {pending_text "^PENDING"} {warning_text "^WARNING"} } {
    upvar 1 $failed_out failed
    upvar 1 $pending_out pending
    upvar 1 $warning_out warning

    set failed 0
    set pending 0
    set warning 0

    set o [open $file r]
    while { [gets $o line] >= 0 } {
        set ret [regexp $fail_text $line]
        if { $ret != 0 } {
            incr failed
        }

        # 2004-09-02 arw also check for TclSh "Wrong # args" - a Bad Thing
        set ret [regexp "^Wrong \# args" $line]
        if { $ret != 0 } {
            incr failed
        }

        set ret [regexp $pending_text $line]
        if { $ret != 0 } {
            incr pending
        }

        set ret [regexp $warning_text $line]
        if { $ret != 0 } {
            incr warning
        }
    }
    close $o
}

# Run a test in an environment configured for transactional data store.
proc run_tdsmethod { test args storage_type indexnodes} {
	run_envmethod $test txn $args $storage_type $indexnodes
}

proc run_cdsmethod { test args storage_type indexnodes} {
	run_envmethod $test cdb $args $storage_type $indexnodes
}

# Run a test in an environment configured for transactional data store.
proc run_envmethod { test {envtype ""} args storage_type indexnodes} {
    source ./include.tcl
    puts "run_envmethod test=$test envtype=$envtype args=$args storage_type=$storage_type indexnodes=$indexnodes"
    global dbxml_err
    env_cleanup $testdir
    set oflags " -create -lock_max_locks 10000 -lock_max_lockers 10000 -lock_max_objects 10000"
    
    if { [lsearch -exact $args thread] >= 0 } {
	append oflags " -thread "
    }
    
    if { $envtype == "tds" || $envtype == "txn" } {
	set envflag "-txn"
    } elseif { $envtype == "cdb" } {
	set envflag "-cdb -cdb_alldb"
    } elseif { $envtype == "" } {
	set envflag ""
    } else {
	puts "Environment type $envtype not recognized."
    }
    
    set stat [catch {
	set env [eval {berkdb_env} $oflags $envflag \
		     -mode 0644 -home $testdir -errfile errors.txt]
	error_check_good env_open [is_valid_env $env] TRUE
	append args " -env $env "
	append args " -storage $storage_type"
	if { $envtype == "cdb" } {
	    append args " -cdb -cdb_alldb"
	}
	
	if { $indexnodes == 1 } {
	    append args " -indexnodes "
	}
	puts "[timestamp]"
	
	eval $test $args
	
	flush stdout
	flush stderr
	error_check_good envclose [$env close] 0
	error_check_good envremove [berkdb envremove \
					-home $testdir] 0
    } res]
    
    if { $stat != 0} {
	global errorInfo;
	
	set fnl [string first "\n" $errorInfo]
	set theError [string range $errorInfo 0 [expr $fnl - 1]]
	if {[string first FAIL $errorInfo] == -1} {
	    puts "FAIL:[timestamp]\
			    run_envmethod with $envtype env: $test: $theError"
	    incr dbxml_err
	} else {
	    puts "FAIL:[timestamp] $theError"
	    incr dbxml_err
	}
    }
}

# runs a specific test group with no environment
proc run_xmltest { test args storage_type index_nodes} {
    source ./include.tcl
    global dbxml_err
    check_handles
    append args " -storage $storage_type"
    if { $index_nodes == 1} {
	append args " -indexnodes "
    }
    
    eval $test $args
    
    if { $dbxml_err == 0 } {
	set dbxml_err [xml_verify_dir $testdir "" 1 1]
    }
}

# run the conversion of the indexer/query processor tests
proc convert { sub } {
	source ./include.tcl

  # help option
  if { ![string compare $sub ?]} {
    puts ""
    puts "Usage:  convert ci|qp"
    puts ""
    puts "ci\tgenerate query plans for the queries/indexes in test group 10.2"
    puts "qp\tgenerate query plans for the queries/indexes in test group 11"
    puts ""
    puts "An XML file is generated in \"regression_results/\[indexer|query_processor\]\" for each test. This file contains the query plan, with the old-style results."
    puts ""
    puts "The external test program generates a separate log file in \"test_logs/\" for each test."
    puts ""
    puts "Output from DB XML is redirected to \"unit_tests.out\" (STDOUT) or \"unit_tests.err\" (STDERR)."
    puts ""
    return
  }

  set conversion_tests(ci) [list { xml006_2 6} { xml010_2_and_3 10 } {xml012_3 12.3}]
  set conversion_tests(qp) [list { xml011_2 11 }]

  dbxml_error_check_good "No conversion tests for subsystem \"$sub\" (try \"ci\" or \"qp\"))" [info exists conversion_tests($sub)] 1

  # no environment
  set env NULL
  set txnenv 0
  set oargs [eval {set_openargs} {} ]
  set regress 1

	foreach pair $conversion_tests($sub) {
		set tname [lindex $pair 0]
		set tnum  [lindex $pair 1]
  	if {[catch {eval $tname $env $txnenv $tnum $oargs $regress} res] != 0 } {
	  	puts $res
	  }
  }
}

# experimental help option
proc help { { id 0 } } {
	source ./include.tcl

  array set test_info {}
  get_test_info test_info

  if { ! $id } {
    # no id - general help
    puts ""
    puts "Use \"help N\" to find out more about test group N."
    puts ""
    puts "Use the \"run\" command  to execute individual test groups (enter \"run ?\" for more information)."
    puts ""
    puts "Use the \"run_nls\" command to do the same, but using node level storage."
    puts ""
    puts "The \"run_all\" command executes one or more test groups under all environment and container types (enter \"run_all ?\" for more information)."
    puts ""
    puts "The test groups are as follows:"
    foreach key [lsort -dictionary [array names test_info]] {
      set x $test_info($key)
      set desc [lindex $x 0]
      puts "\t$key: $desc"
    }
    puts ""
    puts "Use the \"convert\" command to generate XML query plans for the old indexer and query processor tests (enter \"convert ?\" for more information)."
    puts ""
    puts "Use \"run_xml\" to batch execute all test groups under all storage models (whole document and node level storage) and environments (currently NULL and transacted). All output is redirected to the file \"ALL.OUT\" in the CWD."
    puts ""
    puts "The tests can be run in non-interactive mode by executing the script \"run_tests.tcl\"."
    puts ""
  } else {
    # id - request for info on a specific test group
  	if { [info exists test_info($id)] != 1 } {
      set group [format "xml%03d" $id]
      puts "There is no such thing as test group $group"
      return
    } else {
      set x $test_info($id)
      set desc [lindex $x 0]
      puts "\t$id: $desc"
      set subgroups [lrange $x 1 end]
      puts ""
      puts "TODO add the ability to execute a sub-group directly; the available sub-groups are:"

      foreach sg $subgroups {
        puts -nonewline " "
        puts -nonewline $sg
      }
      puts -nonewline "\n"
      puts ""
    }
  }
}

# find descriptions/sub-group IDs for all test groups (one group per file)
proc get_test_info { details } {
	source ./include.tcl

  upvar 1 $details test_info

  set ret [catch { glob $test_path/xml???.tcl } result]
  if { $ret == 0 } {
    # iterate over test groups (one file per test group)
    foreach file $result {
      # get the test group base
      set tail [file tail $file]
      set idx [string last [file extension $file] $tail]
      set base [string range $tail 0 [expr $idx - 1]]

      set start [string length "xml"]
      set tnum [string range $base $start end]
      set tnum [string trimleft $tnum 0]

      set content [readFileUTF8 $file]

      # read line-by-line
      set desc {}
      set subgroups {}
      set lines [split $content "\r\n"]
      foreach line $lines {
        # look for the description of the test group
        if { [string match "\#*TEST*" $line] } {
          set found 1

          # tidy up the information
          regsub -all "TEST" $line {} tmp
          regsub -all "\#" $tmp {} tmp
          set tmp [string trim $tmp]

          # ignore test id line
          if { ! [string match "$base*" $tmp] } {
            append desc " " $tmp
          }
        }

        # look for procedures that correspond to sub-groups
        if { [string match "proc ${base}_*" $line] } {
          # extract procedure name (second token)
          set words [split $line]
          set name [lindex $words 1]

          # verify that this is a test sub-group
          set end_bits [string range $name [string length $base] end]
          if { [string match -nocase "*\[a-z\]*" $end_bits] } { continue }

          append subgroups " " $name
        }
      }

      # key is test group, value is description then list of sub-groups
      set test_info($tnum) [list $desc $subgroups]
    }
  }
}

# sanity check for procedures that are commented out
proc check_commented_out_tests {} {
    source ./include.tcl
    array set commented_out {}

    set found_global 0
    set ret [catch { glob $test_path/xml???.tcl } result]
    if { $ret == 0 } {
        foreach file $result {
            set tail [file tail $file]
            set idx [string last [file extension $file] $tail]
            set base [string range $tail 0 [expr $idx - 1]]

            set found_local 0
            set suspects {}

            # read line-by-line
            set content [readFileUTF8 $file]
            set lines [split $content "\r\n"]
            foreach line $lines {
                # look for test procedures that are commented out
                if { [regexp "^\\s*\#\\s*${base}_\[0-9\]+" $line] } {
                    incr found_local
                    incr found_global
                    append suspects $line "\n"
                }
            }
            if { $found_local } {
                string trim $suspects "\n"
                set commented_out($tail) $suspects
            }
        }
    }

    if { $found_global } {
        puts "\n=================================================================="
        puts "WARNING - some tests may be commented out:"
        foreach key [lsort -dictionary [array names commented_out]] {
            set tests $commented_out($key)
            puts "\tIn '$key':"
            puts "\t\t$tests"
        }
        puts "=================================================================="
    }
}

