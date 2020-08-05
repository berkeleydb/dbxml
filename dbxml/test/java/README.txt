		Java JUnit testing
		------------------


Prerequisites
-------------
o  JUnit 4.x or later (using annotations)
    see www.junit.org and/or junit.sourceforge.net
    o download and install somewhere
o  Java that supports annotations (Java 5 or later)

Build
-----

o use --with-junit=<path_to_junit_jar> in configure arguments
  e.g. --enable-java --with-junit=/home/gmf/junit4.4/junit-4.4.jar

o test classes should be in package com.sleepycat.dbxmltest

Run
---
Usage: java dbxmltest.XmlTestRunner [-c whole|node|all] [-e none|txn|cds|all] [test1 [test2 ..]]

1. Add build_unix/dbxmltest.jar to your CLASSPATH (as well as
dbxml.jar and db.jar)
2. Run a test for a specific container and environment type:
   java dbxmltest.XmlTestRunner [-c container] [-e environment] [test1]
   e.g.
   java dbxmltest.XmlTestRunner -c node -e txn DocumentTest
3. Run all tests:
   java dbxmltest.XmlTestRunner

To add a new test class
-----------------------

1.  Write the test per JUnit requirements (cut/paste an existing
    class).  JUnit uses annotations to mark functions for testing.
2.  Add the file to the build:
  a. Edit dist/Makefile.in and add the file to the
  JAVA_TESTSRCS macro
  b. running "make" in build_unix should rebuild the required
  files to add the test to the build

Automated scripts
-----------------
	TBD -- group tests into suites

Windows
-------
        Use ant release 1.7.0 or higher
          (junit-4.4.jar, db.jar and dbxml.jar must be in CLASSPATH)
        o ant (no arguments -- cleans, builds, runs tests)
        Other targets:
        o ant compile (compiles the classes)
        o ant jar (creates dbxmltest.jar in build/jar)
        o ant runtest (compiles, creates jar, runs tests)
        
        o ant -p gives you the target options in build.xml

        TBD: maybe add install target
        





