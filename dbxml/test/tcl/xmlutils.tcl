# See the file LICENSE for redistribution information.
#
# Copyright (c) 1996,2009 Oracle.  All rights reserved.
#
#
# DB XML test system utilities
#

proc xml_cleanup { dir env { quiet 0 } } {
    global old_encrypt
    global passwd
    source ./include.tcl

    xml_database mgr $dir $env

    set ret [catch { glob $dir/*.dbxml } result]
    if { $ret == 0 } {
	foreach fileorig $result {
	    set file [file tail $fileorig]

	    set ret [catch {
		$mgr removeContainer $file
	    } res]

	    if { $ret != 0 && $quiet == 0 } {
		puts "FAIL: xml_cleanup failed for $file: $res"
	    }
	}
    }
    delete mgr
}

# Verify all XmlContainers in the specified directory.
proc xml_verify_dir { {directory $testdir} \
	  { pref "" } { noredo 0 } { quiet 0 } { nodump 0 } { cachesize 0 } } {
	global encrypt
	global passwd

	# If we're doing database verification between tests, we don't
	# want to do verification twice without an intervening cleanup--some
	# test was skipped.  Always verify by default (noredo == 0) so
	# that explicit calls to xml_verify_dir during tests don't require
	# cleanup commands.
	if { $noredo == 1 } {
		if { [file exists $directory/NOREVERIFY] == 1 } {
			if { $quiet == 0 } {
				puts "Skipping verification."
			}
			return 0
		}
		set f [open $directory/NOREVERIFY w]
		close $f
	}

	if { [catch {glob $directory/*.dbxml} containers] != 0 } {
		# No files matched
		return 0
	}
	if { [file exists /dev/stderr] == 1 } {
		set errfilearg "-errfile /dev/stderr "
	} else {
		set errfilearg ""
	}
	set errpfxarg {-errpfx "FAIL: verify" }
	set errarg $errfilearg$errpfxarg
	set ret 0

	# Open an env, so that we have a large enough cache.  Pick
	# a fairly generous default if we haven't specified something else.

	if { $cachesize == 0 } {
		set cachesize [expr 1024 * 1024]
	}
	set encarg ""
	if { $encrypt != 0 } {
		set encarg "-encryptaes $passwd"
	}

	set env [eval {berkdb_env -home $directory -create -private} $encarg \
	    {-cachesize [list 0 $cachesize 0]}]

        xml_database mgr $directory $env

	foreach cpath $containers {
		set container [file tail $cpath]

		if { [catch {$mgr verifyContainer $container ""} res] != 0 } {
			puts $res
			puts "FAIL:[timestamp] $container verification failed."
			set ret 1
			continue
		}

		if { $quiet == 0 } {
			puts "${pref}Verification of $container succeeded."
		}
		# Skip the dump if it's dangerous to do it.
		if { $nodump == 0 } {
			if { [catch {xml_dumploadtest $directory $container $pref $quiet} res] != 0 } {
				puts $res
				puts "FAIL:[timestamp] Dump/load of $container failed."
				set ret 1
				continue
			}

			error_check_good dumpload:$container $res 0
			if { $quiet == 0 } {
				puts \
				    "${pref}Dump/load of $container succeeded."
			}
		}
	}

        delete mgr
	error_check_good vrfyenv_close [$env close] 0

	return $ret
}

proc xml_dumploadtest { directory container pref quiet } {
	global util_path
	global xmlutil_path
	global encrypt
	global passwd

	set ret 0
	set newname $container-dumpload

	set utilflag "-h $directory"
	if { $encrypt != 0 } {
		set utilflag "$utilflag -P $passwd"
	}

	# Do a db_dump test.  Dump/load each file.
	set rval [catch {eval {exec $xmlutil_path/dbxml_dump} $utilflag \
	        $container | $xmlutil_path/dbxml_load $utilflag $newname} res]
	error_check_good dbxml_dump/dbxml_load($container:$res) $rval 0

	foreach dbname [eval {exec $util_path/db_dump} $utilflag -l $container] {
		# Don't check the dictionary or statistics databases - it's
		# okay if the are different.  We can't easily check
		# indexes - they have a custom bt_compare, which isn't
		# accessible from Tcl.
		if { [ string first "primary_dictionary" $dbname ] != -1 ||
		     [ string first "document_statistics" $dbname ] != -1 ||
		     [ string first "structural_stats" $dbname ] != -1 ||
		     [ string first "document_index" $dbname ] != -1 } {
			continue
		}

		if { [catch {eval xml_dbcmp $dbname $directory/$container $directory/$newname} res] != 0 } {
			puts $res
			puts "FAIL:[timestamp] Check of $dbname failed."
			set ret 1
			continue
		}

		error_check_good xml_dbcmp:$dbname $res 0
		if { $quiet == 0 } {
			puts "${pref}Check of $dbname succeeded."
		}
	}

	eval berkdb dbremove $directory/$newname

	return $ret
}

proc xml_dbcmp { dbname oldfile newfile } {
	global encrypt
	global passwd

	# Open original database
	set dbarg ""
	if { $encrypt != 0 } {
		set dbarg "-encryptany $passwd"
	}

	# Open old database.
	set olddb [eval {berkdb_open -rdonly} $dbarg $oldfile $dbname]
	error_check_good olddb($oldfile:$dbname) [is_valid_db $olddb] TRUE

	# Now open new database.
  catch {eval {berkdb_open -rdonly} $dbarg $newfile $dbname} newdb
  set valid [is_valid_db $newdb]

  # Sometimes reindexing after a dbxml_load doesn't create the same
  # index databases, as empty ones aren't recreated.
  # In this case, we say that they databases compare equal
  if { [binary_compare $valid TRUE] != 0 } {
    if { [string equal -length 24 $dbname "secondary_document_index"] == 1 } {
      return 0;
    }
  }
	error_check_good newdb($newfile:$dbname) $valid TRUE

	# Walk through olddb and newdb and make sure their contents
	# are identical.
	set oc [$olddb cursor]
	set nc [$newdb cursor]
	error_check_good orig_cursor($oldfile:$dbname) \
	    [is_valid_cursor $oc $olddb] TRUE
	error_check_good new_cursor($newfile:$dbname) \
	    [is_valid_cursor $nc $newdb] TRUE

	set ret 0
	for { set odbt [$oc get -first] } { [llength $odbt] > 0 } \
	    { set odbt [$oc get -next] } {
		set ndbt [$nc get -get_both \
		    [lindex [lindex $odbt 0] 0] [lindex [lindex $odbt 0] 1]]
		if { [ catch { error_check_good \
		    db_compare($oldfile:$dbname) $ndbt $odbt } res ] != 0 } {
			puts $res
			set ret 1
		}
	}

	for { set ndbt [$nc get -first] } { [llength $ndbt] > 0 } \
	    { set ndbt [$nc get -next] } {
		set odbt [$oc get -get_both \
		    [lindex [lindex $ndbt 0] 0] [lindex [lindex $ndbt 0] 1]]
		if { [ catch { error_check_good \
		    db_compare_back($oldfile:$dbname) $odbt $ndbt } res ] != 0 } {
			puts $res
			set ret 1
		}
	}

	error_check_good orig_cursor_close($oldfile:$dbname) [$oc close] 0
	error_check_good new_cursor_close($newfile:$dbname) [$nc close] 0

	error_check_good orig_db_close($$oldfile:$dbname) [$olddb close] 0
	error_check_good new_db_close($newfile:$dbname) [$newdb close] 0

	return $ret
}

proc readFile { filename } {
    set fd [open $filename r]
    fconfigure $fd -encoding binary
    fconfigure $fd -translation binary
    set data [read $fd]
    close $fd
    return $data
}

proc readFileUTF8 { filename } {
    set fd [open $filename r]
    fconfigure $fd -encoding utf-8
    set data [read $fd]
    close $fd
    return $data
}

# returns 1 iff ok, 0 otherwise
# if the debug flag is set then the cause of failure is printed
proc putFileInContainer { database container filename {txn "NULL"} {debug 0} } {
    if { [catch {
	wrap XmlUpdateContext uc [$database createUpdateContext]
	wrap XmlDocument xd [$database createDocument]
	$xd setContent [readFile $filename]
	$xd setName [file tail $filename]
	if { $txn == "NULL" } {
	    $container putDocument $xd $uc
	} else {
	    $container putDocument $txn $xd $uc
	}
	delete xd
	delete uc
    } ret] != 0 } {
	if { $debug } {
	    puts "FAIL: file not put in container:\n$ret"
	}
	return 0;
    }
    return 1;
}

proc getDocumentContent { container txn name } {
    if { [catch {
	if { $txn == "NULL" } {
	    wrap XmlDocument xd [$container getDocument $name]
	} else {
	    wrap XmlDocument xd [$container getDocument $txn $name]
	}
	set data [$xd getContentAsString]
	delete xd
    } ret] != 0 } {
	puts "exception: xmlutils.tcl getDocumentContent: $ret"
	set data ""
    }
    return $data
}

proc enableDebugLog {} {
    source ./include.tcl
    setLogLevel $LEVEL_INFO 1
    setLogLevel $LEVEL_DEBUG 1
    setLogCategory $CATEGORY_INDEXER 1
    setLogCategory $CATEGORY_QUERY 1
    setLogCategory $CATEGORY_OPTIMIZER 1
    setLogCategory $CATEGORY_DICTIONARY 1
    setLogCategory $CATEGORY_CONTAINER 1
#    setLogCategory $CATEGORY_NODESTORE 1
}

proc disableDebugLog {} {
    source ./include.tcl
    #	setLogLevel $LEVEL_DEBUG 0
    setLogCategory $CATEGORY_INDEXER 0
    setLogCategory $CATEGORY_QUERY 0
    setLogCategory $CATEGORY_OPTIMIZER 0
    setLogCategory $CATEGORY_DICTIONARY 0
    setLogCategory $CATEGORY_CONTAINER 0
    setLogCategory $CATEGORY_NODESTORE 0
}

# create a container management (XmlManager) object
# too many tests currently depend on the environment
# directory acting as current working directory, so always
# create a minimal environment
proc xml_database { handle_r dir env { env_flags 0 } } {
    source ./include.tcl

    # create the new object (CTR depends upon environment)
    upvar $handle_r handle
    set adopt 0
    if { $env == "NULL" } {
        set env [eval {berkdb_env -home $dir -create}]
	set adopt $DBXML_ADOPT_DBENV
    }

    #eval set handle \[new_XmlManagerFromEnv $env 0\]
    eval set handle \[new_XmlManager $env $adopt\]

    # the default container type is set globally by each test group
    $handle setDefaultContainerType $global_container_type
    $handle setDefaultContainerFlags $global_container_flags

    # set up procedure (so that overloaded name works)
    proc $handle {cmd args} "eval XmlManager_\$cmd $handle \$args"

    # set up the trace
    uplevel trace variable $handle_r uw "{deleteObject XmlManager $handle}"

    return $handle
}

#
# Provides a simple object oriented interface using
# SWIG's low level interface.
#

proc new {objectType handle_r args} {
    # Creates a new SWIG object of the given type,
    # returning a handle in the variable "handle_r".
    #
    # Also creates a procedure for the object and a trace on
    # the handle variable that deletes the object when the
    # handle varibale is overwritten or unset
    upvar $handle_r handle
    #
    # Create the new object
    #
    eval set handle \[new_$objectType $args\]
    #
    # Set up the object procedure
    #
    proc $handle {cmd args} "eval ${objectType}_\$cmd $handle \$args"
    #
    # And the trace ...
    #
    uplevel trace variable $handle_r uw "{deleteObject $objectType $handle}"
    #
    # Return the handle so that 'new' can be used as an argument to a procedure
    #
    return $handle
}

proc wrap {objectType handle_r obj} {
    # Creates a new SWIG object of the given type,
    # returning a handle in the variable "handle_r".
    #
    # Also creates a procedure for the object and a trace on
    # the handle variable that deletes the object when the
    # handle varibale is overwritten or unset
    if {$obj == "NULL"} {
        error "Attempt to wrap NULL object"
    }
    upvar $handle_r handle
    #
    # Setup the object
    #
    eval set handle {$obj}
    #
    # Set up the object procedure
    #
    proc $handle {cmd args} "eval ${objectType}_\$cmd $handle \$args"
    #
    # And the trace ...
    #
    uplevel trace variable $handle_r uw "{deleteObject $objectType $handle}"
    #
    # Return the handle so that 'new' can be used as an argument to a procedure
    #
    return $handle
}

proc deleteObject {objectType handle name element op} {
    #
    # Check that the object handle has a reasonable form
    #
    if {![regexp {_[0-9a-f]*_p_(.+)} $handle]} {
        error "deleteObject: not a valid object handle: $handle"
    }
    #
    # Remove the object procedure
    #
    catch {rename $handle {}}
    #
    # Delete the object
    #
    delete_$objectType $handle
}

proc delete {handle_r} {
    #
    # A synonym for unset that is more familiar to C++ programmers
    #
    uplevel unset $handle_r
}

proc dbxml_error_check_bad { func result bad {txn 0}} {
#	error_check_bad $func $result $bad $txn
# 2004-08-18 arw failures are non-fatal
	global dbxml_err
	if { [binary_compare $result $bad] == 0 } {
		if { $txn != 0 } {
			$txn abort
		}
		flush stdout
		flush stderr
		puts "FAIL:[timestamp] $func returned error value $bad"
    incr dbxml_err
		return 1
	}
	return 0
}

proc dbxml_error_check_good { func result desired {txn 0} } {
#	error_check_good $func $result $desired $txn
# 2004-08-18 arw failures are non-fatal
	global dbxml_err
	if { [binary_compare $desired $result] != 0 } {
		if { $txn != 0 } {
			$txn abort
		}
		flush stdout
		flush stderr
		puts "FAIL:[timestamp] $func: expected \"$desired\", got \"$result\""
    incr dbxml_err
		return 1
	}
	return 0
}

# output information about a "pending" test
proc dbxml_pending_test { testid sr {msg {} }} {
    global dbxml_pending
    puts -nonewline "PENDING \[SR \#$sr\] Test $testid"

    if { [string length $msg] > 0 } {
        puts -nonewline " ($msg)"
    }
    puts -nonewline "\n"
    incr dbxml_pending
}

proc set_openargs { args } {
    source ./include.tcl
    
    # Default to use of DB_CREATE flag.
    #	set openargs $DB_CREATE
    set openargs 0
    
    # Turn on threads.
    #
    # Tcl interface is NOT safe for multi-threading!
    # This is just for testing the API.
    #
    if { [lsearch -exact $args thread] >= 0 } {
	set openargs [expr $openargs + $DB_THREAD]
    }
    
    if { [lsearch -exact $args "-indexnodes"] >= 0 } {
	set openargs [expr $openargs + $DBXML_INDEX_NODES]
    } else {
	set openargs [expr $openargs + $DBXML_NO_INDEX_NODES]
    }
    
    return $openargs
}

proc start_txn { env } {
	set txn [$env txn]
	dbxml_error_check_good start_txn [is_valid_txn $txn $env] TRUE

	return $txn
}

proc commit_txn { txn } {
	dbxml_error_check_good txn_commit [$txn commit] 0
}

# executes an external program
#
# exec on "program" using "args"
#
proc run_external_test_program { txnenv test_id program args} {
    source ./include.tcl
    global dbxml_err
    global dbxml_pending

    # set the test id (used only to name a container and the log file)
    set args [concat $args "--id $test_id"]

    # set the log file directory
    set log_dir [file normalize test_logs]
    set args [concat $args "--logdir $log_dir"]
    if { ! [file exists $log_dir] } {
	file mkdir $log_dir
    }

    # set the transactioned environment flag
    if {$txnenv == 1} {
	set args [concat $args " --transacted"]
    }

    # set the storage model (a global variable assigned at the Tcl main caller)
    if { $global_container_type == $XmlContainer_NodeContainer } {
	set args [concat $args "--nls"]
    }

    if { $global_container_flags == $DBXML_INDEX_NODES } {
	set args [concat $args "--indexnodes"]
    }

    # escape square brackets and dollar signs
    regsub -all {\[} $args {\[} args
    regsub -all {\]} $args {\]} args

    regsub -all {\$} $args {\$} args

    set args [join $args]
    set prog [file join "." $program]

    set now [clock format [clock seconds] -format "%H:%M:%S"]
    set time_at_start_of_test [clock scan $now]
    puts "\t\tExecuting '$prog'"

  #  puts "\t\t$xmltest_path/$prog $args"

    set ret [catch { eval exec $xmltest_path/$prog $args >> unit_tests.out 2>> unit_tests.err } res]

    # verify success (1) - return code of external program
    set failed 0
    if { $ret != 0 } {
	dbxml_error_check_good $res $ret 0

    puts "\t\tChecking results"

	# forcibly remove the TESTDIR to avoid inconsistent state for next test
	fileremove -f $testdir
	file mkdir $testdir

	set failed 1
    }

    # verify success (2) - error messages in the log file
    set log_file {}
    set ret [catch { glob $log_dir/*$test_id.log } result]
    if { $ret == 0 } {
	foreach file $result {
	    set failures 0
	    set o [open $file r]
	    while { [gets $o line] >= 0 } {
		# ignore if the output refers to an earlier test run
		# each line starts with "YYYY-MM-DD HH:MM:SS "
		if {[regexp {([0-9]+-?){3} ([0-9]+:?){3}} $line]} {
		    set timestamp_substring [string range $line 0 18]
		    set log_time [clock scan $timestamp_substring]
		    if { $log_time < $time_at_start_of_test } {
			continue
		    }
		}

		set e [regexp {([0-9]+-?){3} ([0-9]+:?){3} ERROR} $line]
		if { $e != 0 } {
		    incr failures
		    incr dbxml_err
		}

		set e [regexp {([0-9]+-?){3} ([0-9]+:?){3} (PENDING.*)} $line "" "" "" message]
		if { $e != 0 } {
		    puts $message
		    incr dbxml_pending
		}
	    }
	    if { $failures } {
		set failed 1
		puts "FAIL $test_id reported $failures errors ($file)"
	    }

	    # assumes a single log file
	    set log_file [file join test_logs [file tail $file]]
	}
    } else {
	incr dbxml_err
	set failed 1
	puts "FAIL: No log file found for test $test_id (program '$program')"
    }

    if { [string compare $log_file ""] } {
	puts "\t\tLogged to '$log_file'"
    }
    if { ! $failed } {
	puts "\t\tOK"
    }
}

# recursively search for index.xml files
#
# start in "dir" and append paths to "files_found"
proc find_indexes { dir files_found } {
  upvar 1 $files_found res

  # look for index files in the current directory
  set indexes [glob -nocomplain -directory $dir index.xml]
  foreach item $indexes {
    lappend res $item
 }

  # recurse
  set dirs [glob -nocomplain -type d -directory $dir *]
  foreach item $dirs {
    find_indexes $item res
  }
}

# parses an index file for a unit test
#
# parses "file" and writes the test id, program name and any additional program
# arguments to "id", "prog" and "args" respectively
#
# prints the test id and description to STDOUT
#
# returns 1 if everything is OK, 0 otherwise
proc parse_index_file { file id prog args } {
	source ./include.tcl

  set status 0

  upvar 1 $id testid
  upvar 1 $prog program_name
  upvar 1 $args additional_args

  set content [readFile $file]

  # basic XML parsing - find id (attribute of 'test' element)
  set ret [getAttribute $content "test" "id" testid ]
  if { ! $ret } { return $status }

  puts -nonewline "\n\t$testid: "

  # basic XML parsing - find description
  set desc "\[No Description\]"
  set ret [getText $content "description" desc ]
  if { ! $ret } { return $status }
  puts -nonewline $desc
  puts -nonewline \n

  # basic XML parsing - find program name
  set ret [getAttribute $content "program" "name" program_name ]
  if { ! $ret } { return $status }

  # basic XML parsing - find additional arguments to program
  set buffer $content
  set $ret 1
  while { $ret } {
    set argName {}
    set ret [getText $buffer "addArg" argName ]
    if { $ret } {
      set additional_args [concat additional_args $argName]
      set buffer [string range $buffer $ret end]
    }
  }

  set status 1
  return $status
}

# _very_ simple XML parsing methods for "index.xml" files
# don't try to be clever or awkward when editing those files!

# get value of attribute from element (first and only value)
#   returns 0 upon error, "cursor position" upon success
proc getAttribute { xml element attribute val } {

  set status 0

  upvar 1 $val result

  # find the start of the element
  set stok [string first "<$element" $xml]
  if { $stok == -1 } { return $status }

  # find start of the attribute
  set stok [string first " $attribute" $xml $stok]
  if { $stok == -1 } { return $status }

  # extract the first quoted value found after the attribute
  set ret [regexp -indices -start $stok {(\"|\')(\S+)(\"|\')} $xml quotes]
  if { !$ret } { return $status }
  set result [string range $xml [lindex $quotes 0] [lindex $quotes 1] ]
  set result [string trim $result \"\']

  set status [lindex $quotes 1]
  return $status
}

# get value of a text node
#   returns 0 upon error, "cursor position" upon success
proc getText { xml element val } {

  set status 0

  upvar 1 $val result

  # find closing delimiter for the start of the element
  set stok [string first "<$element" $xml]
  if { $stok == -1 } { return $status }
  set stok [string first ">" $xml $stok]
  if { $stok == -1 } { return $status }

  # find opening delimiter for the end of the element
  set etok [string first "<" $xml $stok]
  if { $etok == -1 } { return $status }

  # extract what lies between these delimiters
  set result [string range $xml [expr $stok+1] [expr $etok-1] ]

  #set ret [regexp -indices -start $stok {(\>)(.*?)(\<)} $xml x1]
  #if { !$ret } { return $status }
  #set result [string range $xml [expr [lindex $x1 0]+1]  [expr [lindex $x1 1]-1] ]

  set status $etok
  return $status
}

# get system Nan value definition
proc getNanValue { } {

  # Windows represents Nan and infinity differently than UNIX.
  global is_windows_test
  set nanValue 0
  if { $is_windows_test == 1 } {
    set nanValue "-1.#IND"
  } else {
    # The inf value is "-NaN" on UNIX when Tcl version >= 8.5
    if { [info tclversion] >= 8.5 } {
      set nanValue "-NaN"
    } else {
      set nanValue "nan"
    }
  }
  return $nanValue
}

# get system Inf value definition
proc getInfValue { } {

  # Windows represents Inf and infinity differently than UNIX.
  global is_windows_test
  set infValue 0
  if { $is_windows_test == 1 } {
    set infValue "0.0"
  } else {
    # The inf value is "Inf" on UNIX when Tcl version >= 8.5
    if { [info tclversion] >= 8.5 } {
      set infValue "Inf"
    } else {
      set infValue "inf"
    }
  }
  return $infValue
}

