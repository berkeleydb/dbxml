/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static org.junit.Assert.assertEquals;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlDocumentConfig;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

public class InputStreamTest2 {

    private TestConfig hp = null;
    private static final String CON_NAME = "testData.dbxml";
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";

    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test InputStreamTest2!");
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
	System.out.println("Finished test InputStreamTest2!");
    }

    /*
     * Test XmlDocument.setContentAsInputStream() use user implement XmlInputStream
     * 
     */
    @Test
	public void tesetSetContentAsInputStream() throws Throwable{
	XmlTransaction txn = null;
	TestInputStream ins = new TestInputStream();
	XmlDocument doc = mgr.createDocument();
	XmlDocument doc2 = null;
	doc.setName(docName);
	doc.setContentAsXmlInputStream(ins);
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
		doc2 = cont.getDocument(txn, docName);
	    }else{
		cont.putDocument(doc);
		doc2 = cont.getDocument(docName);
	    }
	    assertEquals(doc2.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test XmlContainer.putDocument(String name, XmlInputStream 
     * 	input, XmlUpdateContext * context)
     * use user implement XmlInputStream
     * 
     */
    @Test 
	public void testPutDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlInputStream is = new TestInputStream();
	XmlDocument myDoc = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, is, uc);
		myDoc = cont.getDocument(txn, docName);
	    }else{
		cont.putDocument(docName, is, uc);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocument(String name, XmlInputStream input, XmlUpdateContext
     * context, XmlDocumentConfig config)
     * use user implement XmlInputStream
     * 
     */
    @Test
	public void testPutDocument_IsConf() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlInputStream is = new TestInputStream();
	XmlDocumentConfig config = new XmlDocumentConfig();
	XmlDocument myDoc = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, is, uc, config);
		myDoc = cont.getDocument(txn, docName);
	    }else{
		cont.putDocument(docName, is, uc, config);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }
}
