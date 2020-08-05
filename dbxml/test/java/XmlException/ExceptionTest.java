/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.db.Environment;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

public class ExceptionTest {
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
	System.out.println("Begin test XmlException!");
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
	System.out.println("Finished test XmlException!");
    }

    /*
     * Test for getDatabaseException()
     * 
     */
    @Test
	public void testGetDatabaseException() throws Throwable {
	XmlManager mgr2 = null;
	try {
	    Environment env = hp.createEnvironment(hp.getEnvironmentPath());
	    env.close();
	    mgr2 = new XmlManager(env, null);
	    fail("A manager was successfully created when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DATABASE_ERROR);
	    assertNotNull(e.getDatabaseException());
	}finally{
	    if(mgr2 != null) mgr2.delete();
	}
    }

    /*
     * Test for getErrorCode()
     * 
     */
    @Test
	public void testGetErrorCode() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent("<root>test</root>");
	XmlInputStream is = doc.getContentAsXmlInputStream();
	is.delete();

	// test after delete()
	try {
	    is.curPos();
	    fail("XmlInputStream was accessed after it was deleted.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}
    }

    /*
     * Test for getQueryLine()
     * 
     */
    @Test
	public void testGetQueryLine() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext xqc = mgr.createQueryContext();
	String query = "collection('testData.dbxml')/d_node/";
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		mgr.query(txn, query, xqc);
	    }else{
		cont.putDocument(doc, xuc);
		mgr.query(query, xqc);
	    }
	    fail("An invalid query succeeded.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getQueryLine(), 1);
	    assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	}finally{
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getQueryColumn()
     * 
     */
    @Test
	public void testGetQueryColumn() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext xqc = mgr.createQueryContext();
	String query = "collection('testData.dbxml')/d_node/";
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		mgr.query(txn, query, xqc);
	    }else{
		cont.putDocument(doc, xuc);
		mgr.query(query, xqc);
	    }
	    fail("An invalid query succeeded.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getQueryColumn(), 37);
	    assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	}finally{
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

}
