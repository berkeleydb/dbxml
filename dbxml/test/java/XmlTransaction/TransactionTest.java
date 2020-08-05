/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007, 2008 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.db.Transaction;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.HelperFunctions;

public class TransactionTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";

    private static String docName = "testDoc.xml";
    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlTransaction!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
	public void setUp() throws Throwable {
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
	cont = hp.createContainer(CON_NAME, mgr);
    }

    @After
	public void tearDown() throws Throwable {
	hp.closeContainer(cont);
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finished test XmlTransaction!");
    }

    /*
     * Test for abort()
     * 
     */
    @Test
	public void testAbort() throws Throwable{
	if(!hp.isTransactional()) return;

	XmlTransaction txn = mgr.createTransaction();
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	try{
	    cont.putDocument(txn, doc, xuc);
	}catch(XmlException e){
	    throw e;
	}finally{
	    txn.abort();
	}
    
	try{
	    doc = cont.getDocument(docName);
	    fail("Failure in TransactionTest.testAbort()");
	}catch(XmlException e){
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}
    }

    /*
     * Test for commit()
     * 
     */
    @Test
	public void testCommit() throws Throwable{
	if(!hp.isTransactional()) return;

	XmlTransaction txn = mgr.createTransaction();
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	try{
	    cont.putDocument(txn, doc, xuc);
	    txn.commit();
	    txn = null;
	    doc = cont.getDocument(docName);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for commitSync()
     * 
     */
    @Test
	public void testCommitSync() throws Throwable{
	if(!hp.isTransactional()) return;

	XmlTransaction txn = mgr.createTransaction();
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	try{
	    cont.putDocument(txn, doc, xuc);
	    txn.commitSync();
	    txn = null;
	    doc = cont.getDocument(docName);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for commitNoSync()
     * 
     */
    @Test
	public void testCommitNoSync() throws Throwable{
	if(!hp.isTransactional()) return;

	XmlTransaction txn = mgr.createTransaction();
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	try{
	    cont.putDocument(txn, doc, xuc);
	    txn.commitNoSync();
	    txn = null;
	    doc = cont.getDocument(docName);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for getTransaction()
     * 
     */
    @Test
	public void testGetTransaction() throws Throwable{
	if(!hp.isTransactional())return;
    
	XmlTransaction txn = mgr.createTransaction();
	Transaction dbtxn = txn.getTransaction();
	assertNotNull(dbtxn);
	String name = "txn";
	dbtxn.setName(name);
	assertEquals(dbtxn.getName(), name);
	txn.commit();
    }

    /* Tests that an isNull() XmlTransaction works in C++ */
    @Test
	public void testNullTransaction() throws Exception 
    {
	assertTrue(HelperFunctions.testNullTxn(mgr));
    }
}
