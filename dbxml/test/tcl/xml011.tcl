# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
# Test 11: Query Processor
#
#
# TEST	xml011
# TEST	Query Processor and XmlQueryContext API

proc xml011 { args } {
    puts "\nxml011: Query Processor ($args)"
    source ./include.tcl
    
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
    
    set tnum 11
    set basename $tnum
    
    # API
    xml011_1 $env $txnenv $cdbenv $basename.1 $oargs
    
    # Deprecated QP tests (used only to dump plans to file)
    xml011_2 $env $txnenv $cdbenv $basename.2 $oargs
    
    # Query bugs
    xml011_3 $env $txnenv $cdbenv $basename.3 $oargs
    
    # just-in-time query optimizations
    xml011_6 $env $txnenv $cdbenv $basename.6 $oargs
    
    # Benchmark queries
    xml011_8 $env $txnenv $cdbenv $basename.8 $oargs
}

# API - top level wrapper method
proc xml011_1 { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} oargs } {
	puts "\n\t$basename: API"

  # Namespace and metadata methods
	xml011_1_1 $env $txnenv $cdbenv $basename.1 $oargs

  # Basic query expressions, variable store, base URI
	xml011_1_2 $env $txnenv $cdbenv $basename.2 $oargs

  # Eager evaluation
	xml011_1_3 $env $txnenv $cdbenv $basename.3 $oargs

  # Lazy evaluation
	xml011_1_4 $env $txnenv $cdbenv $basename.4 $oargs

  # XmlResults::next() (external C++ program)
	xml011_1_5 $env $txnenv $cdbenv $basename.5 $oargs
}

# API - XmlQueryContext interface (namespace and metadata methods)
#   - tests 11.12.1 - 11.12.7 from regression test suite
#
# 2004-08-31 ARW these tests are unit tests on the API layer, i.e. namespace
#   is actually used in e.g. a query
proc xml011_1_1 { env txnenv cdbenv basename oargs } {
    puts "\n\t\t$basename: Namespace and metadata methods"
    source ./include.tcl
    set txn NULL
    set id 0
    
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    
    # Getting namespace before setting namespace
    #   test 11.12.1 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get namespace before setting namespace."
    wrap XmlQueryContext context [$db createQueryContext]
    set n [$context getNamespace "foo"]
    dbxml_error_check_good "get_before_set" $n ""
    delete context
    
    # Getting namespace after setting namespace
    #   test 11.12.2 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get namespace after setting namespace."
    wrap XmlQueryContext context [$db createQueryContext]
    $context setNamespace "foo" "bar"
    set n [$context getNamespace "foo"]
    dbxml_error_check_good "get_namespace" $n "bar"
    delete context
    
    # Removing namespace
    #   test 11.12.3 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Remove namespace."
    wrap XmlQueryContext context [$db createQueryContext]
    set n [$context getNamespace "foo"]
    dbxml_error_check_good $testid $n ""
    $context removeNamespace "foo"
    set n [$context getNamespace "foo"]
    dbxml_error_check_good "remove_namespace" $n ""
    delete context
    
    # Getting namespace after removing namespace
    #   test 11.12.4 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get after remove of namespace."
    wrap XmlQueryContext context [$db createQueryContext]
    $context setNamespace "foo" "bar"
    $context removeNamespace "foo"
    set n [$context getNamespace "foo"]
    dbxml_error_check_good "get_after_remove" $n ""
    delete context
    
    # Getting namespace after clearing namespace
    #   test 11.12.5 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get after clear of namespace."
    wrap XmlQueryContext context [$db createQueryContext]
    $context setNamespace "foo" "bar"
    $context clearNamespaces
    set n [$context getNamespace "foo"]
    dbxml_error_check_good "get_after_clear" $n ""
    delete context
    
    # Getting a variable before setting a variable
    #   test 11.12.6 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get variable before setting."
    wrap XmlQueryContext context [$db createQueryContext]
    set n [$context get "foo"]
    dbxml_error_check_good "get_variable_before_set" $n ""
    delete context
    
    # Getting a variable after setting a variable
    #   test 11.12.7 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Get variable after setting."
    wrap XmlQueryContext context [$db createQueryContext]
    $context set "foo" "bar"
    set n [$context get "foo"]
    dbxml_error_check_good "get_variable" $n "bar"
    delete context
    
    # clean up
    delete db
}

# API - query expressions, variable store, base URI
#   - tests 11.12.17 - 11.12.23 from regression test suite
proc xml011_1_2 { env txnenv cdbenv basename oargs } {
  puts "\n\t\t$basename: query expressions, variable store, baseURI, default collection, doc-available"
  source ./include.tcl
  set txn NULL
  set id 0

  # preparation - database, contexts, container
  xml_cleanup $testdir $env
#  xml_cleanup [file join $testdir "a"] $env
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

  wrap XmlUpdateContext uc [$db createUpdateContext]
  wrap XmlQueryContext context [$db createQueryContext]

  # create and add a simple document
  wrap XmlDocument xd [$db createDocument]
  $xd setContent "<a><b>c</b></a>"
  $xd setName "fooBar"

  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd $uc
    $txn commit
    delete txn
  } else {
    $container putDocument $xd $uc
  }
  delete xd

  wrap XmlDocument xd [$container getDocument "fooBar" ]

  # Prepare and execute a query expression (Results Documents / Eager)
  #   test 11.12.17 from regression test suite
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Prepare and execute a query expression"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "collection('[$container getName]')/a/b"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $newxd_content [$xd getContentAsString]

  # Reuse a compiled expression
  #   test 11.12.18 from regression test suite
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Reuse a compiled expression."

  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $newxd_content [$xd getContentAsString]

	delete xe

  # Re-compile and execute query expression (what about "no context"?)
  #   test 11.12.19 from regression test suite
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Recompile and execute expression (no context?)"
	$context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "collection('[$container getName]')/a/b"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $newxd_content [$xd getContentAsString]

	delete xe

  # Quuery for document in a container (what about "no context"?)
  #   test 11.12.20 from regression test suite
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Query for document in container (no context)."

  set fullQuery "collection('[$container getName]')/a/b"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlResults xr [$db query $fullQuery $context 0]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $newxd_content [$xd getContentAsString]

	delete context

  # Modifying the variable store - basic success and failure cases
  #   test 11.12.21 from regression test suite
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Modifying the variable store."

  wrap XmlQueryContext context [$db createQueryContext]
	new XmlValue xv $XmlValue_DECIMAL "123"
  $context setVariableValue "foo" $xv

	set xv_store [$context getVariableValue "foo"]
	dbxml_error_check_bad "$testid - no variable found" $xv_store "NULL"
	dbxml_error_check_good "$testid - wrong type" [$xv_store isType $XmlValue_DECIMAL] 1
  delete xv_store

	set xv_store [$context getVariableValue "bar"]
	dbxml_error_check_good "$testid - found non existent variable" $xv_store "NULL"

  delete xv
  delete context

  # Setting a variable to a sequence
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Setting a variable to a sequence"

  wrap XmlQueryContext context [$db createQueryContext]
	new XmlValue xv1 $XmlValue_DECIMAL "123"
	new XmlValue xv2 $XmlValue_STRING "hello"

	wrap XmlResults var_value [$db createResults]
  $var_value add $xv1
  $var_value add $xv2
  $context setVariableValue "foo" $var_value
  delete var_value

  # Check the variable value
  wrap XmlResults var_value [$context getVariableValues "foo"]
  dbxml_error_check_good "$testid - variable value not found or wrong size" [$var_value size] 2

  new XmlValue val
  $var_value next $val
  dbxml_error_check_good "$testid - wrong variable value" [$val asString] [$xv1 asString]
  $var_value next $val
  dbxml_error_check_good "$testid - wrong variable value" [$val asString] [$xv2 asString]

  delete val
  delete var_value

  # Querying using a sequence variable
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Querying using a sequence variable"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "\$foo"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue val
    $xr next $val
    set val1 [$val asString]
    $xr next $val
    set val2 [$val asString]

    delete val
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue val
    $xr next $val
    set val1 [$val asString]
    $xr next $val
    set val2 [$val asString]

    delete val
    delete xr
    delete xe
  }
    
  dbxml_error_check_good "$testid - result size" $xr_size 2
  dbxml_error_check_good "$testid - result contents(1)" $val1 [$xv1 asString]
  dbxml_error_check_good "$testid - result contents(2)" $val2 [$xv2 asString]

  delete xv2
  delete xv1

  # Setting a variable to an empty sequence
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Setting a variable to an empty sequence"

  wrap XmlResults var_value1 [$db createResults]
  new XmlValue var_value2
  new XmlResults var_value3
  $context setVariableValue "foo1" $var_value1
  $context setVariableValue "foo2" $var_value2
  $context setVariableValue "foo3" $var_value3
  delete var_value3
  delete var_value2
  delete var_value1

  # Check the variable value
  wrap XmlResults var_value1 [$context getVariableValues "foo1"]
  wrap XmlResults var_value2 [$context getVariableValues "foo2"]
  wrap XmlResults var_value3 [$context getVariableValues "foo3"]

  dbxml_error_check_good "$testid - variable value not found or wrong size" [$var_value1 size] 0
  dbxml_error_check_good "$testid - variable value not found or wrong size" [$var_value2 size] 0
  dbxml_error_check_good "$testid - variable value not found or wrong size" [$var_value3 size] 0

  delete var_value3
  delete var_value2
  delete var_value1

  # Setting bad (binary) variable values
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Setting invalid variable values"

  wrap XmlResults bad_results [$db createResults]
  new XmlValue bad_value $XmlValue_BINARY "binary_data"
  $bad_results add $bad_value  
  set ret [catch {
     $context setVariableValue "bad1" $bad_value
  } bad1 ]
  set ret [catch {
     $context setVariableValue "bad2" $bad_results
  } bad2 ]

  delete bad_results
  delete bad_value

  dbxml_error_check_good "$testid" [string match "Error*value*binary*" $bad1] 1
  dbxml_error_check_good "$testid" [string match "Error*value*binary*" $bad2] 1
  delete context

  # Basic API test for accessing the base URI
  #   test 11.12.23 from regression test suite
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Getting and setting the base URI."
  wrap XmlQueryContext context [$db createQueryContext]
  set default_uri [$context getBaseURI]
  dbxml_error_check_good $testid $default_uri "dbxml:/"

  set file_uri "file:/[file join $test_path subdir]"
  $context setBaseURI $file_uri
  dbxml_error_check_good $testid [$context getBaseURI] $file_uri

  delete context

  # Basic API test for setting and retrieving the
  # default collection
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Getting and setting the default collection."
  wrap XmlQueryContext context [$db createQueryContext]
  set default_uri [$context getDefaultCollection]
  set base_uri [$context getBaseURI]
  dbxml_error_check_good $testid $default_uri ""

  set file_uri "[$container getName]"
  $context setDefaultCollection $file_uri
  dbxml_error_check_good $testid [$context getDefaultCollection] $base_uri$file_uri

  # Run a query that uses the default collection
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Querying using the default collection."
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "collection()/a/b"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_content [$newxd getContentAsString]

    delete newxd
    delete xv
    delete xr
    delete xe
  }

  dbxml_error_check_good "$testid - result size" $xr_size 1
  dbxml_error_check_good "$testid - result contents" $newxd_content [$xd getContentAsString]

  # More interesting Base URI and default collection tests:
  #  o create some directory structure and a container
  #  o ensure absolute URI ignores base URI
  #  o ensure relative URIs use the base during runtime querying,
  #    and during setting of default collection
  #  o ensure that doc() ignores default collection, but uses
  #    the base as necessary
  #  Ignore transactions for these tests for now
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Complex base URI and default collection tests."

  set tsubdir $testdir
  set subdir [file join $tsubdir a]
  file delete -force $subdir
  file mkdir -p $subdir
  set relcname $basename.dbxml
  # container name is relative to TESTDIR, which holds the environment
  set cname [file join a $relcname]
  wrap XmlContainer subcont [$db createContainer $cname $global_container_type]
  wrap XmlUpdateContext subuc [$db createUpdateContext]
  wrap XmlQueryContext subqc [$db createQueryContext]
  $subcont putDocument "foo" "<empty/>" $subuc

  # set base uri and default collection to something bad and make 
  # sure they're ignored
  $subqc setBaseURI "dbxml:/noplace"
  $subqc setDefaultCollection "nothing.dbxml"
  wrap XmlResults xr [$db query "collection('dbxml:/$cname')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr
  wrap XmlResults xr [$db query "doc('dbxml:/$cname/foo')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # set base and make sure success works
  # also set default collection correctly; it is ignored by absolute URIs
  $subqc setBaseURI "dbxml:/a/"
  $subqc setDefaultCollection "dbxml:/$cname"
  wrap XmlResults xr [$db query "collection('$relcname')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr
  wrap XmlResults xr [$db query "doc('$relcname/foo')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # reset base URI to the actual container name
  $subqc setBaseURI "dbxml:/$cname/"
  # now try using default collection in query
  wrap XmlResults xr [$db query "collection()" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr
  # now try using base URI for the "foo" doc
  wrap XmlResults xr [$db query "doc('foo')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # test that using a relative URI for default collection resolves
  # using the base URI at the time of setting
  $subqc setBaseURI "dbxml:/"
  $subqc setDefaultCollection "$cname"
  $subqc setBaseURI "dbxml:/foo/"
  # try using default collection in query
  wrap XmlResults xr [$db query "collection()" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # test file:* URIs for docs and absolute (dbxml://) URIs
  # Need to close/reopen the container to use absolute
  delete subcont
  set dirname [file join [pwd] TESTDIR/a ]
  set cname $dirname/$relcname
  set fcname dbxml:///$cname
  wrap XmlContainer subcont [$db openContainer $cname]
  # use absolute uri
  wrap XmlResults xr [$db query "collection('$fcname')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # make sure base is used for resolution up front
  $subqc setBaseURI dbxml:///$dirname/
  $subqc setDefaultCollection "$relcname"
  set tdef [$subqc getDefaultCollection]
  dbxml_error_check_good "$testid - check default collection" [string match $tdef $fcname] 1
  $subqc setBaseURI "dbxml:/foo/"

  # use default collection
  wrap XmlResults xr [$db query "collection()" $subqc]

  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # use relative path + base URI
  $subqc setBaseURI dbxml:///$dirname/
  wrap XmlResults xr [$db query "collection('$relcname')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # use relative path + base URI for doc()
  $subqc setBaseURI dbxml:///$dirname/
  wrap XmlResults xr [$db query "doc('$relcname/foo')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  # use absolute path for doc() -- use // instead of //.  Set base bad first
  $subqc setBaseURI dbxml:/boo
  wrap XmlResults xr [$db query "doc('dbxml:///$dirname/$relcname/foo')" $subqc]
  dbxml_error_check_good "$testid - result size" [$xr size] 1
  delete xr

  delete subcont
  delete subqc
  delete subuc
  #
  # end of complex base URI and default collection cases
  #

  # Run a query that uses the doc-available function
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Querying using the doc-available function."
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "doc-available('[$container getName]/[$xd getName]')"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $result "true"

  # Run a query that fails using the doc-available function
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Querying using the doc-available function. (failure)"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "doc-available('[$container getName]/noggin')"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $result "false"

  # Run another query that fails using the doc-available function
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Querying using the doc-available function. (failure2)"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "doc-available('[$container getName]')"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $result "false"

  # Run another query that fails using the doc-available function
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Querying using the doc-available function. (failure3)"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "doc-available('file://doesntexist')"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    set result [$xv asString]

    delete xv
    delete xr
    delete xe
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $result "false"

  delete context
	delete xd

  # clean up
  delete uc
	delete container
  delete db
}

# API - eager evaluation
#   - XmlResults::size() also tested in assertQueryReturnsValue()
proc xml011_1_3 { env txnenv cdbenv basename oargs } {
    puts "\n\t\t$basename: Eager evalation"
    source ./include.tcl
    set txn NULL
    set id 0

    # prepare - database, contexts, container
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

    # create and add a simple document
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<a><b>c</b></a>"
    $xd setName "foo"

    if { $txnenv == 1 } {
	wrap XmlTransaction txn [$db createTransaction]
	$container putDocument $txn $xd $uc
	$txn commit
	delete txn
    } else {
	$container putDocument $xd $uc
    }
    delete xd

    wrap XmlDocument xd [$container getDocument "foo" ]

    # basic query (live values (default), eager)
    #  test 11.12.8 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Query for document in container"
    wrap XmlQueryContext context [$db createQueryContext]
    $context setEvaluationType $XmlQueryContext_Eager
    assertQueryReturnsValue $testid $db $container {/a/b} \
	"<a><b>c</b></a>" $context $env $txnenv $cdbenv $oargs 1

    delete context

    # basic query (result values, eager)
    #  test 11.12.9 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Query for result values in container"
    wrap XmlQueryContext context [$db createQueryContext]
    $context setEvaluationType $XmlQueryContext_Eager
    assertQueryReturnsValue $testid $db $container {/a/b} \
	"<b>c</b>" $context $env $txnenv $cdbenv $oargs 0
    delete context

    ########################################################################
    # iteration methods
    # 2004-09-01 arw next() methods tested using C++ code
    # &&
    # methods on eager results (size() and add())
    ########################################################################

    # create a results vector with one entry (equal to the document)
    wrap XmlQueryContext context [$db createQueryContext]
    set fullQuery "collection('[$container getName]')/a/b"
    if { $txnenv == 1 } {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]

	xml011_1_3_x $basename $id $xr $xd
	delete xr

	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults xr [$db query $fullQuery $context 0]
	xml011_1_3_x $basename $id $xr $xd
	delete xr
    }

    # clean up
    delete context
    delete xd
    delete container
    delete uc
    delete db
}

proc xml011_1_3_x { basename id xr xd } {
    source ./include.tcl

    # these cases test against whole document content.
    # iteration methods
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Iteration methods (reset)"

    dbxml_error_check_good "$testid (size before reset)" [$xr size] 1
    new XmlValue xv
    $xr next $xv
    dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0
    $xr next $xv
    dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 1

    $xr reset
    $xr next $xv
    dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0

    # methods on eager results (size() and add())
    set testid $basename.[incr id]
    set sub_id 0
    puts "\t\t\t$testid: Methods on eager results"

    puts "\t\t\t\t$testid.1: size()"
    dbxml_error_check_good "$testid (size() before add())" [$xr size] 1

    puts "\t\t\t\t$testid.2: add()"
    new XmlValue xv2 $XmlValue_STRING "hello"
    $xr add $xv2
    dbxml_error_check_good "$testid (size() after add())" [$xr size] 2
    $xr reset
    $xr next $xv
    wrap XmlDocument xd1 [$xv asDocument]
    dbxml_error_check_good "$testid (check content - 1)" [string match "*[$xd getContentAsString]" [$xd1 getContentAsString]] 1
    delete xd1
    $xr next $xv
    dbxml_error_check_good "$testid (check content - 2)" [$xv asString] "hello"
    delete xv
    delete xv2
}

# API - lazy evaluation
proc xml011_1_4 { env txnenv cdbenv basename oargs } {
    puts "\n\t\t$basename: Lazy evalation"
    source ./include.tcl
    set txn NULL
    set id 0

    # prepare - database, contexts, container
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

    # create and add a simple document
    wrap XmlDocument xd [$db createDocument]
    $xd setContent "<a><b>c</b></a>"
    $xd setName "foo"

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }
    
    delete xd
    wrap XmlDocument xd [$container getDocument "foo" ]

    # basic query (live values, lazy)
    #  test 11.12.10 from regression test suite
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Query for document in container (lazy)"
    wrap XmlQueryContext context [$db createQueryContext]
    $context setEvaluationType $XmlQueryContext_Lazy

    assertQueryReturnsValue $testid $db $container {/a/b} \
        "<a><b>c</b></a>"  $context $env $txnenv $cdbenv $oargs 1

    delete context

    # basic query (result values, lazy)
    #  test 11.12.11 from regression test suite

    set testid $basename.[incr id]
    puts "\t\t\t$testid: Query for result values in container (lazy)"
    wrap XmlQueryContext context [$db createQueryContext]
    $context setEvaluationType $XmlQueryContext_Lazy
    assertQueryReturnsValue $testid $db $container {/a/b} \
        "<b>c</b>" $context $env $txnenv $cdbenv $oargs 0
    delete context

    ########################################################################
    # iteration methods
    # 2004-09-01 arw next() methods tested using C++ code
    # &&
    # methods on eager results (size() and add()) (failure paths)
    ########################################################################

    # create a results vector with one entry (equal to the document)
    wrap XmlQueryContext context [$db createQueryContext]
    $context setEvaluationType $XmlQueryContext_Lazy
    set fullQuery "collection('[$container getName]')/a/b"
    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
        xml011_1_4_x $basename $id $xr
        delete xr
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlResults xr [$db query $fullQuery $context 0]
        xml011_1_4_x $basename $id $xr
        delete xr
    }

    # clean up
    delete context
    delete xd

    delete container
    delete uc
    delete db
}

proc xml011_1_4_x { basename id xr } {
    source ./include.tcl

    # iteration methods
    set testid $basename.[incr id]
    puts "\t\t\t$testid: Iteration methods (reset)"

    new XmlValue xv
    $xr next $xv
	  dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0
    $xr next $xv
	  dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 1

    $xr reset
    $xr next $xv
	  dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0

    # methods on eager results (size() and add()) (failure paths)
    set testid $basename.[incr id]
    set sub_id 0
    puts "\t\t\t$testid: \"eager\" methods on lazy results"

    puts "\t\t\t\t$testid.1: size()"
    catch {
        set n [$xr size]
    } ret
    dbxml_error_check_good $testid [string match "*Error*lazily*" $ret] 1

    puts "\t\t\t\t$testid.2: add()"
    new XmlValue xv2 $XmlValue_STRING "hello"
    catch {
        $xr add $xv2
    } ret
    dbxml_error_check_good $testid [string match "*Error*lazily*" $ret] 1

    # clean up
    delete xv
    delete xv2
}

# API - iterative methods on XmlResults
#  tests 11.12.14, 11.12.15 plus tests missing from regression suite
#
#  Use a C++ program because of diffulties in overloaded next() methods
proc xml011_1_5 { env txnenv cdbenv basename oargs } {
    puts "\t$basename: C++ programs for regression tests"

    source ./include.tcl
    xml_cleanup $testdir $env

    set id $basename
    set program dbxml_test_query_processor_api
    # set arguments to external program
    set args {}
    set args [concat $args "--env $testdir"]
    set args [concat $args "$cdbenv"]

    run_external_test_program $txnenv $id $program $args
}

# assumes that 1 hit is always expected
proc assertQueryReturnsValue { testid db container query result context \
				   {env "NULL"} {txnenv 0} {cdbenv ""} oargs asDoc } {
    source ./include.tcl
    set xtxn NULL

    # keep transaction open - required for lazy evaluation
    set fullQuery "collection('[$container getName]')$query"
    if { $txnenv == 1 } {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlResults xr [$db query $xtxn $fullQuery $context 0]
	# verify that one result is returned (eager evaluation only)
	#   - this is to keep regression test
	if { [$context getEvaluationType] == $XmlQueryContext_Eager } {
	    dbxml_error_check_good "$testid (results size)" [$xr size] 1
	}
	# verify that the result is as expected
	new XmlValue xv
	$xr next $xv
	dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0
	if { $asDoc == 1 } {
	    wrap XmlDocument xd [$xv asDocument]
	    dbxml_error_check_good "$testid ($xv asString)" [$xd getContentAsString] $result
	    delete xd
	} else {
	    dbxml_error_check_good "$testid ($xv asString)" [$xv asString] $result
	}
	delete xv
	delete xr

	$xtxn commit
	delete xtxn
    } else {
	wrap XmlResults xr [$db query $fullQuery $context 0]
	# verify that one result is returned (eager evaluation only)
	if { [$context getEvaluationType] == $XmlQueryContext_Eager } {
	    dbxml_error_check_good "$testid (results size)" [$xr size] 1
	}
	# verify that the result is as expected
	new XmlValue xv
	$xr next $xv
	dbxml_error_check_good "$testid ($xv isNull)" [$xv isNull] 0
	if { $asDoc == 1 } {
	    wrap XmlDocument xd [$xv asDocument]
	    dbxml_error_check_good "$testid ($xv asString)" [$xd getContentAsString] $result
	    delete xd
	} else {
	    dbxml_error_check_good "$testid ($xv asString)" [$xv asString] $result
	}
	delete xv
	delete xr
    }
}

# Preparation and execution of benchmark queries
proc xml011_8 { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} oargs } {
    puts "\txml011.8: Query Processor - XQuery (Benchmark Queries)"
    source ./include.tcl

    xml011_run_queries $env $txnenv $cdbenv $basename "document_set_11_8" $oargs
}

# Preparation and execution of XQuery expressions
proc xml011_run_queries { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} document_set oargs } {
#    puts "\txml011_run_queries: basename is $basename, doc set $document_set"
    source ./include.tcl

    # find all test instructions ("index.xml" files)
    list set index_files {}
    set root [file join $test_path $document_set]
    find_indexes $root index_files

    # each file corresponds to a set of related use cases
    foreach file [lsort $index_files] {
	# get test id, program name
	set id {}
	set program {}
	set args {}
	set ret [parse_index_file $file id program args]
	dbxml_error_check_good "WARNING \"$file\" was not recognized as a valid test driver" $ret 1

	# basic arguments - test materials are in the same directory as the index
	set d [file dirname $file]
	set p [file normalize $d]
	set args [concat $args "--datadir $p"]

	# basic arguments - DB XML environment
	set args [concat $args "--env $testdir"]
	set args [concat $args "$cdbenv"]

	# start with a clean DB environment
	xml_cleanup $testdir $env

	run_external_test_program $txnenv $id $program $args
    }
}

# Just-in-time query optimization
proc xml011_6 { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} oargs } {
    puts "\t$basename: Query Processor: Just-in-time optimizations"
    source ./include.tcl

    # iterate over evaluation and return types
    list set eval_types {}
    lappend eval_types $XmlQueryContext_Eager
    lappend eval_types $XmlQueryContext_Lazy

    list set return_types {}
    lappend return_types $XmlQueryContext_LiveValues

    set c 0
    foreach et $eval_types {
        foreach rt $return_types {
            incr c
            xml011_6_run $env $txnenv $basename.$c $oargs $et $rt
        }
    }
}

proc xml011_6_run { env txnenv basename oargs et rt } {
    source ./include.tcl

    # explain what is happening
    set msg "\t\t$basename: "
    if { $et == $XmlQueryContext_Eager } {
        append msg "Eager"
    } elseif { $et == $XmlQueryContext_Lazy } {
        append msg "Lazy"
    } else {
        dbxml_error_check_good "$basename - unexpected eval type $et" 1 0
    }
    append msg " / "
    if { $rt == $XmlQueryContext_LiveValues } {
        append msg "Live Values"
    } else {
        dbxml_error_check_good "$basename - unexpected return type $rt" 1 0
    }
    puts $msg

    # prepare - database, contexts, container
    xml_cleanup $testdir $env
    xml_database db $testdir $env

    wrap XmlUpdateContext uc [$db createUpdateContext]
    wrap XmlQueryContext qc [$db createQueryContext]

    $qc setEvaluationType $et
    $qc setReturnType $rt

    set cname $basename.dbxml
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $txn $cname $oargs $global_container_type]
        $txn commit
        delete txn
    } else {
        wrap XmlContainer container [$db createContainer $cname $oargs $global_container_type]
    }

    # create and add document, index container
    set nameA "docA"
    wrap XmlDocument xdA [$db createDocument]
    $xdA setContent "<docA><foo>hello</foo></docA>"
    $xdA setName $nameA

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]

        $container putDocument $txn $xdA $uc
        $container addIndex $txn "" "foo" "node-element-equality-string" $uc

        $txn commit
        delete txn
    } else {
        $container putDocument $xdA $uc
        $container addIndex "" "foo" "node-element-equality-string" $uc
    }

    # prepare and execute query plan for [non-] constant query on "foo"
    set txn NULL
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
    }

    set query_constant "collection('$cname')//docA/foo"
    set query_non_constant "collection(<bar>$cname</bar>)//docA/foo"

    if { $txnenv == 1 } {
        wrap XmlQueryExpression xqe_c [$db prepare $txn $query_constant $qc]
        wrap XmlQueryExpression xqe_nc [$db prepare $txn $query_non_constant $qc]
    } else {
        wrap XmlQueryExpression xqe_c [$db prepare $query_constant $qc]
        wrap XmlQueryExpression xqe_nc [$db prepare $query_non_constant $qc]
    }

    set testid $basename

    # test - the "constant" query plan will be optimized with knowledge of the
    # index on "foo"
    set qp_constant [$xqe_c getQueryPlan]
    dbxml_error_check_good "$testid - query plan not optimized" [string match "*<UnknownContainer>*" $qp_constant] 0

    # test - the "non constant" query plan will be raw
    set qp_non_constant [$xqe_nc getQueryPlan]
    dbxml_error_check_good "$testid - query plan not raw" [string match "*<UnknownContainer>*" $qp_non_constant] 1

    # test - verify that the actual result is the same
    if { $txnenv == 1 } {
        wrap XmlResults xr_c [$xqe_c execute $txn $qc]
        wrap XmlResults xr_nc [$xqe_nc execute $txn $qc]
    } else {
        wrap XmlResults xr_c [$xqe_c execute $qc]
        wrap XmlResults xr_nc [$xqe_nc execute $qc]
    }

    set count_constant_query 0
    set result_constant_query {}
    set err 0
    while { ! $err } {
        new XmlValue xv
        $xr_c next $xv
        if { ! [$xv getType] } {
            set err 1
        } else {
            incr count_constant_query
            set result_constant_query [$xv asString]
        }
        delete xv
    }

    set count_non_constant_query 0
    set result_non_constant_query {}
    set err 0
    while { ! $err } {
        new XmlValue xv
        $xr_nc next $xv
        if { ! [$xv getType] } {
            set err 1
        } else {
            incr count_non_constant_query
            set result_non_constant_query [$xv asString]
        }
        delete xv
    }

    dbxml_error_check_good "$testid - result count" $count_non_constant_query $count_constant_query
    dbxml_error_check_good "$testid - result value" $result_non_constant_query $result_constant_query

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    # clean up
    delete xdA
    delete xr_c
    delete xqe_c
    delete xr_nc
    delete xqe_nc
    delete container
    delete qc
    delete uc
    delete db
}

# Query Processor Tests
#
# These tests are now deprecated since the mechanism used to verify the query
# plans no longer exists.
#
# If the "regress" flag is set then an external program is used to dump
# the query plans to disk.
#

proc xml011_2 { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} oargs {regress 0} } {
	source ./include.tcl
	puts "\n\txml011.2: Query Processor Tests."

  # 2004/06/25 ARW tests skipped unless the regression option is set
  if { ! $regress} {
    puts "\t\tDEPRECATED - Skipping"
    return
  }

  global test_data
  array set test_data {}

  # turn on for debug outut
  set debug 0

  # data driven tests - process all files in the appropriate test directory
  set dir "$test_path/data_set_$basename"
  set exclude 0
	set ret [catch { glob $dir/data_set*.txt } result]
	if { $ret == 0 } {
    foreach file [lsort -dictionary $result] {
      xml011_run $file $basename $exclude $env $txnenv $cdbenv $oargs $debug $regress
    }
  }

  # specific invocations of the unit test program
  xml011_2_1 $env $txnenv $cdbenv $basename $oargs $regress

  catch {unset test_data}
}

# Executes a group of tests
#
# The input data is read from a text file and written to the global array
# 'test_data'.
# See the "getQPGTestData" procedure for a description of the file format and
# the 'test_data' array.
#
# 2004/07/07 ARW no-op, unless the "regress" flag is set in which case an
#                external program is used to dump the query plans to disk
#
proc xml011_run {file basename exclude {env "NULL"} {txnenv 0} {cdbenv ""} oargs debug regress } {
    source ./include.tcl
    
    # 2004/06/25 ARW we should only be here if the regression option is set
    dbxml_error_check_good "QP tests should be skipped!" $regress 1
    
    # refresh test environment
    xml_cleanup $testdir $env
    xml_database db $testdir $env
    wrap XmlUpdateContext uc [$db createUpdateContext]
    
    # get data for this group of tests (written to global array 'test_data')
    global test_data
    array set test_data {}
    
    set id {}
    set title {}
    set evaluationtype $XmlQueryContext_Eager
    set test_count [getQPGTestData $file $basename $exclude id title returntype evaluationtype]
    
    # test header
    puts "\n\t$id: $title ($test_count tests)"
    
    # create container
    regsub -all "\[ \(\)\]" $id _ name
    set txn NULL
    
    if {$txnenv == 1} {
	wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlContainer container [$db openContainer $xtxn "$name.dbxml" $oargs]
	$xtxn commit
	delete xtxn
    } else {
	wrap XmlContainer container [$db openContainer "$name.dbxml" $oargs]
    }
    
    # create query context
    wrap XmlQueryContext context [$db createQueryContext $returntype $evaluationtype]
    
    # add indexes that apply to the group of tests
    foreach key [array names test_data -regexp "^$key_index:GROUP" ] {
	# value is a list of uri, node and index
	set uri [lindex $test_data($key) 0]
	set node [lindex $test_data($key) 1]
	set index [lindex $test_data($key) 2]
	
	if { $txnenv == 1 } {
	    wrap XmlTransaction txn [$db createTransaction]
	    $container addIndex $txn $uri $node $index $uc
	    $txn commit
	    delete txn
	} else {
	    $container addIndex $uri $node $index $uc
	}
	
	if { $debug } { puts "\tAdded group index '$uri', '$node', '$index'" }
    }
    
    # set variable values
    # 2004-07-16 ARW ignored by the regression tests
    foreach key [array names test_data -regexp "^$key_variable" ] {
	# value is a list of name and value
	set name [lindex $test_data($key) 0]
	set value [lindex $test_data($key) 1]
	
	new XmlValue xv $value
	$context setVariableValue $name $xv
	if { $debug } { puts "\tAdded variable '$name' -> '[$xv asString]'" }
	delete xv
    }
    
    # set the test information for the regression option (name of, and args for,
    # the external program)
    set regress_args {}
    set regress_program {}
    
    # parse the index file to get program name, additional args (ignore test)
    set f [file join $test_path document_set_regression query_processor index.xml]
    set dummy {}
    set program {}
    set args {}
    set ret [parse_index_file $f dummy program args]
    dbxml_error_check_good "Could not execute QP regression tests - \"$f\" was not recognized as a valid test driver" $ret 1
    
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
    set d [file join regression_results query_processor]
    set d [file normalize $d]
    set args [concat $args "--outdir $d"]
    if { ! [file exists $d] } {
	file mkdir $d
    }
    
    set regress_args $args
    
    # execute the individual tests
    foreach key [lsort -dictionary [array names test_data -regexp "^$key_groupid" ] ] {
	
	# value is a list of test id, query and oqp
	set testid [lindex $test_data($key) 0]
	set query [lindex $test_data($key) 1]
	set oqp [lindex $test_data($key) 2]
	
	# 2004-07-17 arw hack to exeucte 11.4 separatly
	if { [string match "TEST*11.4*" $key] } {
	    11_4_run $testid $query $oqp $env $txnenv $cdbenv
	    continue
	}
	
	# output test id and the query...
	puts "\t\t$testid: $query";
	
	# ... but allow for hack of indicating a skipped test
	if { [string first SKIPPED $key] != -1 } {
	    puts "SKIPPING:  obsolete test data"
	    continue
	}
	
	# add indexes that apply to this test
	catch {unset indexes}
	foreach subkey [array names test_data -regexp "^$key_index:$testid:" ] {
	    set uri [lindex $test_data($subkey) 0]
	    set node [lindex $test_data($subkey) 1]
	    set index [lindex $test_data($subkey) 2]
	    
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container addIndex $txn $uri $node $index $uc
		$txn commit
		delete txn
	    } else {
		$container addIndex $uri $node $index $uc
	    }
	    
	    if { $debug } { puts "\tAdded index '$uri', '$node', '$index'" }
	    
	    set indexes($subkey) [list $uri $node $index]
	}
	
	# pass the query to the external program
	set args [concat $args "--container [$container getName]"]
	set args [concat $args "--query \"$query\""]
	set args [concat $args "--oldresult \"$oqp\""]
	set args [concat $args "$cdbenv"]
	
	run_external_test_program $txnenv $testid $regress_program $args
	
	# remove indexes that apply to this test
	foreach subkey [array names indexes] {
	    set uri [lindex $indexes($subkey) 0]
	    set node [lindex $indexes($subkey) 1]
	    set index [lindex $indexes($subkey) 2]
	    
	    if { $txnenv == 1 } {
		wrap XmlTransaction txn [$db createTransaction]
		$container deleteIndex $txn $uri $node $index $uc
		$txn commit
		delete txn
	    } else {
		$container deleteIndex $uri $node $index $uc
	    }
	    
	    if { $debug } { puts "\tRemoved index '$uri', '$node', '$index'" }
	}
    }
    
    # housekeeping
    delete context
    delete container
    delete uc
    delete db
}

# specific invocations of the external C++ framework (regression tests)
#   11.3.9  using a variable
proc xml011_2_1 { env txnenv cdbenv basename oargs regress } {
    source ./include.tcl
    puts "\n\t11.2.1 Query Plans with Variables "
    
    dbxml_error_check_good "QP tests should be skipped!" $regress 1
    
    # get test instructions
    set index_file [file join $test_path document_set_regression query_processor 11_3 index.xml]
    
    set id {}
    set program {}
    set args {}
    set ret [parse_index_file $index_file id program args]
    dbxml_error_check_good "WARNING \"$index_file\" was not recognized as a valid test driver" $ret 1
    
    # set arguments to external program
    set args {}
    
    # basic arguments - test materials are in the same directory as the index
    set d [file dirname $index_file]
    set p [file normalize $d]
    set args [concat $args "--datadir $p"]
    
    # basic arguments - DB XML environment
    set args [concat $args "--env $testdir"]
    
    # specific arguments - query, old result
    set query {$foo = 'foo'}
    set args [concat $args "--query \"$query\""]
    
    set oqp "Root(RA(SS()))"
    set args [concat $args "--oldresult \"$oqp\""]
    
    # specific arguments - outputs directory
    set d [file join regression_results query_processor]
    set d [file normalize $d]
    set args [concat $args "--outdir $d"]
    set args [concat $args "$cdbenv"]
    if { ! [file exists $d] } {
	file mkdir $d
    }
    
    # start with a clean DB environment
    xml_cleanup $testdir $env
    
    run_external_test_program $txnenv $id $program $args
}

proc xml011_3 { {env "NULL"} {txnenv 0} {cdbenv ""} {basename $tnum} oargs } {
	puts "\n\t$basename: Query bugs"

	xml011_3_1 $env $txnenv $cdbenv $basename.1 $oargs
}

proc xml011_3_1 { env txnenv cdbenv basename oargs } {
  puts "\n\t\t$basename: Use element URI before inserting it into the container"
  source ./include.tcl
  set txn NULL
  set id 0

  # preparation - database, contexts, container
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

  wrap XmlUpdateContext uc [$db createUpdateContext]
  wrap XmlQueryContext context [$db createQueryContext]

  # create and add a simple document
  wrap XmlDocument xd [$db createDocument]
  $xd setContent "<a><b>c</b></a>"
  $xd setName "fooBar"

  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd $uc
    $txn commit
    delete txn
  } else {
    $container putDocument $xd $uc
  }
  delete xd

  # Prepare and execute a query expression using a URI that the container hasn't seen yet.
  set testid $basename.[incr id]
  puts "\t\t\t$testid: Prepare and execute using an unseen URI"
  $context setEvaluationType $XmlQueryContext_Eager

  set fullQuery "declare default element namespace 'http://foo.ns'; collection('[$container getName]')/a/b"
  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlQueryExpression xe [$db prepare $xtxn $fullQuery $context]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlQueryExpression xe [$db prepare $fullQuery $context]
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 0


  # Reuse a compiled expression
  set testid $basename.[incr id]
	puts "\t\t\t$testid: Reuse the prepared query after adding the URI"

  # create and add a simple document using the URI
  set ns_doc_content "<a xmlns='http://foo.ns'><b>c</b></a>"
  wrap XmlDocument xd [$db createDocument]
  $xd setContent $ns_doc_content
  $xd setName "fooBar2"

  if { $txnenv == 1 } {
    wrap XmlTransaction txn [$db createTransaction]
    $container putDocument $txn $xd $uc
    $txn commit
    delete txn
  } else {
    $container putDocument $xd $uc
  }
  delete xd

  if { $txnenv == 1 } {
    wrap XmlTransaction xtxn [$db createTransaction]
    wrap XmlResults xr [$xe execute $xtxn $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_name [$newxd getName]

    delete newxd
    delete xv
    delete xr

    $xtxn commit
    delete xtxn
  } else {
    wrap XmlResults xr [$xe execute $context]
    set xr_size [$xr size]

    new XmlValue xv
    $xr next $xv
    wrap XmlDocument newxd [$xv asDocument]
    set newxd_name [$newxd getName]

    delete newxd
    delete xv
    delete xr
  }

	dbxml_error_check_good "$testid - result size" $xr_size 1
	dbxml_error_check_good "$testid - result contents" $newxd_name "fooBar2"

  # clean up
	delete xe
	delete context
	delete uc
	delete container
  delete db
}

# specific invocations of the external C++ framework (regression tests)
#  for 11.4.*  using result values
proc 11_4_run {testid query oqp env txnenv cdbenv} {
    source ./include.tcl
    puts "\n\t$testid (query using result values)"
    
    # get test instructions
    set index_file [file join $test_path document_set_regression query_processor 11_4 index.xml]
    
    set dummy {}
    set program {}
    set args {}
    set ret [parse_index_file $index_file dummy program args]
    dbxml_error_check_good "WARNING \"$index_file\" was not recognized as a valid test driver" $ret 1
    
    # set arguments to external program
    set args {}
    
    # basic arguments - test materials are in the same directory as the index
    set d [file dirname $index_file]
    set p [file normalize $d]
    set args [concat $args "--datadir $p"]
    
    # basic arguments - DB XML environment
    set args [concat $args "--env $testdir"]
    
    # specific arguments - outputs directory
    set d [file join regression_results query_processor]
    set d [file normalize $d]
    set args [concat $args "--outdir $d"]
    if { ! [file exists $d] } {
	file mkdir $d
    }
    
    # specific arguments - query, old result
    set args [concat $args "--query \"$query\""]
    set args [concat $args "--oldresult \"$oqp\""]
    set args [concat $args "$cdbenv"]
    
    run_external_test_program $txnenv $testid $program $args
}

# Reads the input data for a group of tests from a text file
#
# The file is formatted using line-separated records. Data that applies to
# all tests is specified first. Individual test specifications then follow.
#
# Data that applies to all tests in the file is prefixed by a keyword:
#  - id of test group [mandatory]
#  - title of test group [mandatory]
#  - index specifications [optional, multiple]
#  - variable name/value pairs [optional, multiple]
#  - return type [optional - defaults to documents]
#  - evaluation type [optional - defaults to eager]
#
# Individual test specifications do not have keywords. The data is expected
# in the following order:
#    - test id
#    - index specification that applies only to individual test [optional]
#    - query
#    - expected optimized query plan
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
#  'returntype' -> result return type
#  'evaltype'   -> result evaluation type
#
# The 'basename' corresponds to the prefix of each of the test ids. If two
# arguments are present then the first is the prefix read from the data source
# and the second is the prefix of the actual test id.
#
# Tests with ids that include any items from 'exclude' are skipped.
#
# Returns the number of tests.

proc getQPGTestData {file basename exclude id title rettype evaltype} {
	source ./include.tcl

  upvar $id test_group_id
  upvar $title test_group_title
  upvar $rettype returntype
  upvar $evaltype evaluationtype

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

    #if { $skip } { continue }
    if { $skip } {
      continue
    }

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

    # group data - index specifications ("uri" "node" "index")
    if { [regexp "^$key_index" $line] == 1 } {
      # ignore if we are processing an individual test specification
      if { $state == "STATE_NONE" } {
        regsub $key_index $line {} index
        set index [string trim $index]
        cacheIndex $index
        continue
      }
    }

    # group data - variable ("name" "value")
    if { [regexp "^$key_variable" $line] == 1 } {
      regsub $key_variable $line {} var
      set var [string trim $var]
      cacheVariable $var
      continue
    }

    # group data - return  type
    if { [regexp "^$key_returntype" $line] == 1 } {
      regsub $key_returntype $line {} type
      set returntype [convertReturnType [string trim $type]]
      continue
    }

    # group data - evaluation type
    if { [regexp "^$key_evalulationtype" $line] == 1 } {
      regsub $key_evalulationtype $line {} type
      set evaluationtype [convertEvalulationType [string trim $type]]
      continue
    }

    # test specification - a test id marks the start
    if { [regexp "^$data_prefix\." $line] == 1 } {
      if { $state != "STATE_NONE" } {
        # hack - if state is OQP then we assume that the current test is
        # marked as being skipped
        if { $state == "STATE_OQP" } {
          set test_data($key_groupid:$testid:SKIPPED) [list $testid "(check $file)" {}]
          set state STATE_NONE
        } else {
          set errmsg "Previous test specification incomplete?"
          break
        }
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

    # hack to allow for optional index
    if { $state == "STATE_INDEX" && [regexp "^$key_index" $line] == 0} {
     set state STATE_QUERY
    }

    # test specification - expect consecutive lines in the correct order
    switch $state {
      STATE_TESTID {
        set state STATE_INDEX
      }
      STATE_INDEX {
        regsub $key_index $line {} index
        set index [string trim $index]
        cacheIndex $index $testid

        set state STATE_QUERY
      }
      STATE_QUERY {
        set query $line
        set state STATE_OQP
      }
      STATE_OQP {
        set oqp $line
        set state STATE_COMPLETE
      }
    }

    # cache details of a complete specification
    if { [string equal $state STATE_COMPLETE] } {

      # the keys are ordered
      set test_data($key_groupid:$testid) [list $testid $query $oqp]

      set testcount [expr $testcount+1]
      set state STATE_NONE
    }
  }
  close $fh

  if { [string length $errmsg] } {
    append msg "$file (line $lineno): " $errmsg
    dbxml_error_check_good $msg BAD GOOD
  }

  return $testcount
}

# Cache an index specification in the global array 'test_data'. The key has
# the prefix 'INDEX:<subkey>', where '<subkey>' is either a constant
# to signify that the index specification applies to the current group of
# tests, or is the (specified) test id.
# The record is a line of the form '"uri" "node" "index"'.
proc cacheIndex {record {subkey {} } } {
	source ./include.tcl
  global test_data

  # extract uri, node and index (remember to strip quotes)
  set list [split $record]
  set uri [string trim [lindex $list 0] \"]
  set node [string trim [lindex $list 1] \"]
  set index [string trim [lindex $list 2] \"]

  # key contains information about the scope of the index specification
  set pos [array size test_data]
  if { [string length $subkey] > 0 } {
    # subkey given - this index is specific to a test
    set key $key_index:$subkey:$pos
  } else {
    # no subkey - this index applies to a group of tests
    set key $key_index:GROUP:$pos
  }

  set test_data($key) [list $uri $node $index]
}

# Cache a variable value in the global array 'test_data'. The key has
# the prefix 'VARIABLE'.
# The record is a line of the form '"name" "value".
proc cacheVariable {record} {
  source ./include.tcl
  global test_data

  # extract name and value (remember to strip quotes)
  set list [split $record]
  set name [string trim [lindex $list 0] \"]
  set value [string trim [lindex $list 1] \"]

  set pos [array size test_data]
  set test_data($key_variable:$pos) [list $name $value]
}

# convert string representation of return type (based on the enum in the
# C++ header) to the correct Tcl global constant (defaults to documents)
proc convertReturnType { stringtype } {
	source ./include.tcl
  set type $XmlQueryContext_LiveValues
  return $type
}

# convert string representation of evaluation type (based on the enum in the
# C++ header) to the correct Tcl global constant (defaults to eager)
proc convertEvalulationType { stringtype } {
	source ./include.tcl
  set type $XmlQueryContext_Eager
  if { [string equal $stringtype Eager] } {
    set type $XmlQueryContext_Eager
  } elseif { [string equal $stringtype Lazy] } {
    set type $XmlQueryContext_Lazy
  }

  return $type
}

