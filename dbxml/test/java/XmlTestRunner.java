/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2009 Oracle.  All rights reserved.
 *
 */
package dbxmltest;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runner.*;
import org.junit.runner.notification.*;
import java.util.*;
import java.io.*;

/*
 * XmlTestRunner wraps the various test classes and
 * is required for running a class instance.  It allows
 * some or all tests to be run using several different
 * configurations.  By default, *all* available configurations
 * will be run.
 *
 * Usage: java dbxmltest.XmlTestRunner [-c whole|node|all] 
 *        [-e none|txn|cds|all] [test1 [test2 ..]]
 *
 * Failures/error output will go to the file, ENV_PATH/errorLog.txt
 * To add a new test class add it to the allTests array, below.
 */

public class XmlTestRunner {
    private static boolean NODE_CONTAINER = true;
    private static String ENV_TYPE ="none";
    private static String ENV_PATH = ".";
    private static Class[] allTests = {
	AutoOpenTest.class,
	AutoIndexTest.class,
	DocumentTest.class,
	ManagerConfigTest.class,
	ManagerTest.class,
	MetaDataIteratorTest.class,
	MetaDataTest.class, 
	ResolverTest.class,
	ResultsTest.class,
	ValueTest.class,
	EventReaderTest.class,
	EventReaderToWriterTest.class,
	EventWriterTest.class,
	InputStreamTest.class,
	InputStreamTest2.class,
	StatisticsTest.class,
	IndexSpecificationTest.class,
	IndexLookupTest.class,
	ExceptionTest.class,
	QueryExpressionTest.class,
	TransactionTest.class,
	ContainerTest.class,
	QueryContextTest.class,
	NodeHandleTest.class,
	DataTest.class,
	CompressionTest.class,
	ContainerConfigTest.class,
	UpdateNodeWithPrefixTest.class
    };
    private static final String ALL = "all";
    private static final String NONE = "none";
    private static final String TXN = "txn";
    private static final String CDS = "cds";
    private static final String NODE = "node";
    private static final String WHOLE = "whole";

    public static boolean isNodeContainer() {
	return NODE_CONTAINER;
    }

    public static String getEnvironmentType() {
	return ENV_TYPE;
    }

    public static String getEnvironmentPath() {
	return ENV_PATH;
    }

    public static boolean isTransactional() {
	return ENV_TYPE.equalsIgnoreCase(TXN);
    }

    public static void usage() {
	System.out.println("usage: java dbxmltest.XmlTestRunner" +
		" [-c whole|node|all] [-e none|txn|cds|all]" +
	" [test1 [test2 ..]]");
	System.exit( -1 );
    }

    public static void main(String[] arg) {
	Vector<Boolean> containerTypes = new Vector<Boolean>();
	Vector<String> environmentTypes = new Vector<String>();
	Vector<String> testNames = new Vector<String>();
	Vector<Class> argTests = new Vector<Class>();
	Class[] tests = {};

	for(int i = 0; i < arg.length; i++) {
	    if(arg[i].startsWith("-")){
		switch(arg[i].charAt(1)) {
		case 'c':
		    String cType = arg[++i];
		    if(cType.equalsIgnoreCase(NODE))
			containerTypes.add(true);
		    else if(cType.equalsIgnoreCase(WHOLE))
			containerTypes.add(false);
		    else if(cType.equalsIgnoreCase(ALL)) {
			containerTypes.add(true);
			containerTypes.add(false);
		    } else
			usage();
		    break;
		case 'e':
		    String eType = arg[++i];
		    if(eType.equalsIgnoreCase(NONE) 
			    || eType.equalsIgnoreCase(TXN) 
			    || eType.equalsIgnoreCase(CDS))
			environmentTypes.add(eType);
		    else if(eType.equalsIgnoreCase(ALL)) {
			environmentTypes.add(NONE);
			environmentTypes.add(TXN);
			environmentTypes.add(CDS);
		    } else
			usage();
		    break;
		default:
		    usage();
		}
	    } else
		testNames.add(arg[i]);
	}
	if(containerTypes.size() == 0) {
	    containerTypes.add(true);
	    containerTypes.add(false);
	}
	if(environmentTypes.size() == 0) {
	    environmentTypes.add(NONE);
	    environmentTypes.add(TXN);
	    environmentTypes.add(CDS);
	}

	//Get the tests to run
	if(testNames.size() != 0) {
	    for(int i = 0; i < testNames.size(); i++) {
		Class testClass = null;
		try {
		    testClass = Class.forName("dbxmltest." + testNames.get(i));
		} catch (ClassNotFoundException e) {
		    System.out.println("Skipping test " + testClass + ". Test not found.");
		}
		if(testClass != null)
		    argTests.add(testClass);
	    }
	    if(argTests.size() != 0)
		tests = argTests.toArray(tests);
	    else
		tests = allTests;
	} else
	    tests = allTests;

	// Run the tests
	int failureCount = 0;
	boolean success = true;
	File errorFile = new File(getEnvironmentPath() + "/errorLog.txt");
	try {
	    if(errorFile.exists())
		errorFile.delete();
	    errorFile.createNewFile();
	    System.setErr(new PrintStream(new FileOutputStream(errorFile)));
	} catch (IOException e) {}
	for(int j = 0; j < environmentTypes.size(); j++) {
	    System.out.println("Testing env type " + environmentTypes.get(j));
	    for(int i = 0; i < containerTypes.size(); i++) {
		System.out.println("\tContainer type " + 
			(containerTypes.get(i) ? "node" : "whole"));
		NODE_CONTAINER = containerTypes.get(i);
		ENV_TYPE = environmentTypes.get(j);
		Result res = org.junit.runner.JUnitCore.runClasses(tests);
		if(!res.wasSuccessful()) {
		    System.err.println("Number of failures for environment type " + 
			    ENV_TYPE + " and is node container " + 
			    NODE_CONTAINER + " are " + 
			    res.getFailureCount());
		    List<Failure> testFailures = res.getFailures();
		    ListIterator<Failure>iter = testFailures.listIterator();
		    while(iter.hasNext()) {
			Failure fail = iter.next();
			System.err.println(fail.getDescription());
			Throwable e = fail.getException();
			if(e != null)
			    e.printStackTrace();
			else
			    System.err.println(fail.getTrace());
		    }
		    failureCount += res.getFailureCount();
		    success = res.wasSuccessful();
		}
	    }
	}
	if(success)
	    System.out.println("All tests successful.");
	else
	    System.out.println(failureCount + " tests failed.");
	System.out.println("Failures printed to " + errorFile.getName());
    }
}
