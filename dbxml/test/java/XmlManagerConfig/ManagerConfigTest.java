/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.sleepycat.db.DatabaseException;
import com.sleepycat.db.Environment;
import com.sleepycat.db.EnvironmentConfig;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlManagerConfig;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlValue;

public class ManagerConfigTest {
    private TestConfig hp = null;
    private final String CON_NAME = "testData.dbxml";

    private static String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node></a_node>";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Start test XmlManagerConfig!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
	public void setUp() throws Throwable {
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
    }

    @After
	public void tearDown() throws Throwable {
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finish test XmlManagerConfig!");
    }

    /*
     * Test for adoptEnvironment
     * 
     * 
     */
    @Test
	public void testAdoptEnvironment() throws Exception{
	Environment env = null;

	XmlManagerConfig conf = new XmlManagerConfig();
	conf.setAdoptEnvironment(true);
	XmlManager mgr = null;

	try {
	    env = hp.createEnvironment(hp.getEnvironmentPath());
	    mgr = new XmlManager(env, conf);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    if(mgr != null) mgr.delete();
	    mgr = null;
	}

	try {
	    env.close();
	    fail("Closing an already closed environment should fail");
	} catch (XmlException e) {
	    throw e;
	} catch (DatabaseException e) {
	    throw e;
	} catch (IllegalArgumentException e) {
	    assertNotNull(e.getMessage());
	} catch (NullPointerException e){}
	finally{
	    Environment.remove(new File(hp.getEnvironmentPath()), true, EnvironmentConfig.DEFAULT);
	    TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	}

	// do not use adoptEnvironment
	XmlManager mgr2 = null;
	conf.setAdoptEnvironment(false);
	try {
	    env = hp.createEnvironment(hp.getEnvironmentPath());
	    mgr2 = new XmlManager(env, new XmlManagerConfig());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    if(mgr2 != null) mgr2.delete();
	    mgr2 = null;
	}

	try {
	    env.close();
	} catch (XmlException e) {
	    throw e;
	} catch (DatabaseException e) {
	    throw e;
	} catch (IllegalArgumentException e) {
	    throw e;
	} 

	// the environment has been closed, getHome() throw an exception
	try {
	    env.getHome();
	    fail("Accessing a closed environment should fail");
	} catch (XmlException e) {
	    throw e;
	} catch (DatabaseException e) {
	    throw e;
	} catch (IllegalArgumentException e) {
	    assertNotNull(e.getMessage());
	} catch (NullPointerException e) {}
    }

    /*
     * Test for AllowExternalAccess
     * 
     * 
     */
    @Test
    public void testAllowExternalAccess() throws UnsupportedEncodingException,
    IOException, XmlException, DatabaseException {
    	// prepare a file for test
    	File xmlFile = new File("test.xml");
    	if (xmlFile.exists() == false) {
    		FileOutputStream out = new FileOutputStream(xmlFile, true);
    		out.write(docString.getBytes("utf-8"));
    		out.close();
    	}

    	XmlManagerConfig conf = new XmlManagerConfig();
    	conf.setAllowExternalAccess(true);
    	XmlManager mgr = hp.createManager(hp.getEnvironmentPath(), conf);

    	String myQuery = "doc('"+xmlFile.toURI()+"')/*";
    	XmlQueryContext qc = mgr.createQueryContext();

    	XmlResults results = null;
    	XmlTransaction txn = null;

    	try {
    		if(hp.isTransactional()){
    			txn = mgr.createTransaction();
    			results = mgr.query(txn, myQuery, qc);
    		} else
    			results = mgr.query(myQuery, qc);
    		XmlValue va = results.next();
    		assertEquals(va.asString(),"<a_node atr=\"test\"><b_node>Some text</b_node></a_node>");
    	} catch (XmlException e) {
    		if (txn != null) txn.abort();
    		throw e;
    	} finally {
    		if (results != null) results.delete();
    		results = null;
    	}

    	/* query not exist uri */
    	try {
    		if(hp.isTransactional())
    			results = mgr.query(txn, "file:/notExsist/file", qc);
    		else
    			results = mgr.query("file:/notExsist/file", qc);
    		fail("Fail to throw exception when query not exsit external file");
    	} catch (XmlException e) {
    		assertNotNull(e);
    		assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
    	} finally {
    		if (txn != null) txn.commit();
    		txn = null;
    		if(results != null ) results.delete();
    		results = null;
    		mgr.delete();
    		Environment.remove(new File(hp.getEnvironmentPath()), true, EnvironmentConfig.DEFAULT);
    		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    	}

    	/* test not allow external access*/
    	conf = new XmlManagerConfig();
    	conf.setAllowExternalAccess(false);
    	mgr = hp.createManager(hp.getEnvironmentPath(), conf);
    	qc = mgr.createQueryContext();
    	try {
    		if (hp.isTransactional()) {
    			txn = mgr.createTransaction();
    			results = mgr.query(txn, myQuery, qc);
    		} else
    			results = mgr.query(myQuery, qc);
    		fail("Fail to throw exception while not allow query on external file");
    	} catch (XmlException e) {
    		assertNotNull(e);
    		assertEquals(e.getErrorCode(),XmlException.QUERY_EVALUATION_ERROR);
    	} finally {
    		if(txn != null) txn.commit();
    		if(results != null) results.delete();
    		results = null;
    		mgr.delete();
    		Environment.remove(new File(hp.getEnvironmentPath()), true, EnvironmentConfig.DEFAULT);
    		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    	}

    	/*[#16563] Test that query fails if the manager of the QueryContext has been deleted*/
    	conf = new XmlManagerConfig();
    	conf.setAllowExternalAccess(false);
    	mgr = hp.createManager(hp.getEnvironmentPath(), conf);
    	try {
    		results = mgr.query(myQuery, qc);
    		fail("Fail to throw exception when the manager of the QueryContext has been deleted.");
    	} catch (XmlException e) {
    		assertNotNull(e);
    		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
    	} finally {
    		if (results != null) results.delete();
    		mgr.delete();
    		if(xmlFile.exists() == true)
    			xmlFile.delete();
    		Environment.remove(new File(hp.getEnvironmentPath()), true, EnvironmentConfig.DEFAULT);
    		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    	}

    }

    /*
     * Test for adoptEnvironment
     * 
     * 
     */
    @Test
	public void testAllowAutoOpen() 
	throws XmlException, FileNotFoundException, DatabaseException {
	XmlManagerConfig conf = new XmlManagerConfig();
	conf.setAllowAutoOpen(true);
	XmlManager mgr = null;
	XmlContainer cont = null;
	XmlResults results = null;
	String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node></a_node>";
	XmlTransaction txn = null;
	String myQuery = "collection('" + CON_NAME +"')/a_node/b_node";
	XmlQueryContext qc = null;
	try{
	    mgr = hp.createManager(hp.getEnvironmentPath(), conf);
	    cont = hp.createContainer(CON_NAME, mgr);
	    XmlUpdateContext uc = mgr.createUpdateContext();
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, "test", docString, uc);
		txn.commit();
		txn = null;
	    }else
		cont.putDocument("test", docString, uc);
	    hp.closeContainer(cont);
	    cont = null;

	    qc = mgr.createQueryContext();
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		results = mgr.query(txn, myQuery, qc);
	    }else
		results = mgr.query(myQuery, qc);
	    XmlValue value = results.next();
	    String content = value.asString();
	    assertEquals(content, "<b_node>Some text</b_node>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(results != null) results.delete();
	    results = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    hp.closeContainer(cont);
	    hp.closeManager(mgr);
	    hp.closeEnvironment();
	}

	// test without allowAutoOpen
	// It throw an exception
	XmlManagerConfig conf2 = new XmlManagerConfig();
	XmlManager mgr2 = null;
	XmlQueryContext qc2 = null;
	try {
	    mgr2 = hp.createManager(hp.getEnvironmentPath(), conf2);
	    qc2 = mgr2.createQueryContext();
	    if(hp.isTransactional()){
		txn = mgr2.createTransaction();
		results = mgr2.query(txn, myQuery, qc2);
	    }else
		results = mgr2.query(myQuery, qc2);
	    fail("Failure in testAllowAutoOpen()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(XmlException.CONTAINER_CLOSED, e.getErrorCode());
	}finally{
	    if(results != null) results.delete();
	    results = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    hp.closeManager(mgr2);
	    hp.closeEnvironment();
	}
    }
}
