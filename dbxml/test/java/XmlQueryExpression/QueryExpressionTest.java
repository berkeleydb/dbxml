/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007, 2008 Oracle.  All rights reserved.
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

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlDocumentConfig;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlQueryExpression;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlValue;

public class QueryExpressionTest {
    private TestConfig hp;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";
    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><a_node atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</a_node>";
    private static String docName = "testDoc.xml";
    private static String query = "collection('" + CON_NAME + "')/a_node/c_node";

    @BeforeClass
    public static void setupClass() {
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	System.out.println("Begin test XmlQueryExpression!");
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
	System.out.println("Finished test XmlQueryExpression!");
    }

    /*
     * Test for execute(XmlQueryContext queryContext) 
     * 
     */
    @Test
    public void testExecute() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		qe = mgr.prepare(txn, query, context);
		re = qe.execute(txn, context);
	    } else {
		cont.putDocument(doc, xuc);
		qe = mgr.prepare(query, context);
		re = qe.execute(context);
	    }
	    assertEquals(re.size(), 1);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null) qe.delete();
	    if (re != null) re.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for execute(XmlQueryContext queryContext, XmlDocumentConfig config)  
     * 
     */
    @Test
    public void testExecute_Dc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;
	XmlDocumentConfig xdc = new XmlDocumentConfig();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		qe = mgr.prepare(txn, query, context);
		re = qe.execute(txn, context, xdc);
	    } else {
		cont.putDocument(doc, xuc);
		qe = mgr.prepare(query, context);
		re = qe.execute(context, xdc);
	    }
	    assertEquals(re.size(), 1);
	} catch (XmlException e) {
	    if (qe != null) qe.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	}

	//not valid XmlDocumentConfig flag
	xdc.setDocumentsOnly(true);
	try {
	    if (hp.isTransactional())
		re = qe.execute(txn, context, xdc);
	    else
		re = qe.execute(context, xdc);
	    fail("execute succeeded with an invalid document configuration.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} finally {
	    qe.delete();
	    if (re != null) re.delete();
	    if (txn != null) txn.commit();
	}
    }

    /*
     * Test for execute(XmlValue contextItem, XmlQueryContext queryContext)  
     * 
     */
    @Test
    public void testExecute_ValQc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;
	XmlValue val = new XmlValue();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		qe = mgr.prepare(txn, query, context);
		re = qe.execute(txn, val, context);
	    } else {
		cont.putDocument(doc, xuc);
		qe = mgr.prepare(query, context);
		re = qe.execute(val, context);
	    }
	    assertEquals(re.size(), 1);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re != null) re.delete();
	    if (txn != null) txn.commit();
	}
    }
    
    /*
     * Test for bug [#16224] Which is queries from the root on 
     * constructed XML succeeding when they should throw an exception.
     */
    @Test
    public void testExecute_ContextItem() throws Throwable {
	XmlTransaction txn = null;
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;
	XmlResults re2 = null;
	XmlValue val = new XmlValue();
	
	/* 
	 * This takes the constructed XML return from one query and uses
	 * it as the context for another query.
	 */
	String query1 = "<Players><player><League>1</League></player></Players>";
	String query2 = "player/League";

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	}
	
	
	/* 
	 * This takes the constructed XML return from one query and uses
	 * it as the context for another query that starts at the root.
	 * The second query needs to fail because constructed XML does
	 * not have a document node.
	 */
	query1 = "<Players><player><numstats>1</numstats></player></Players>";
	query2 = "/Players/player";

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		try {
		    re2 = qe.execute(txn, val, context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
		try {
		    re2 = qe.execute(txn, val.getFirstChild(), context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		try {
		    re2 = qe.execute(val, context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
		try {
		    re2 = qe.execute(val.getFirstChild(), context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	}
	
	txn = null;
	
	/* 
	 * This takes XML return from a container and uses
	 * it as the context for another query that starts at the root.
	 * The second query succeeds because XML in a container must have
	 * a document node.
	 */
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(query1);
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
		val = new XmlValue(doc);
		val = val.getFirstChild();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		cont.putDocument(doc);
		val = new XmlValue(doc);
		val = val.getFirstChild();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
	
	/*
	 * Execute a secondary query on a container document returned
	 * by a query, should succeed.
	 */
	query1 = "doc(\"" + CON_NAME + "/" + docName + "\")";
	query2 = "/Players/player";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null) qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
	
	/*
	 * Test that update queries can be executed on a context item
	 */
	query1 = "doc(\"" + CON_NAME + "/" + docName + "\")/Players";
	query2 = "replace node player with <a>a</a>";
	String query3 = "doc(\"" + CON_NAME + "/" + docName + "\")/Players/a";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
		re2.delete();
		re2 = mgr.query(txn, query3, context);
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
		re2.delete();
		re2 = mgr.query(query3, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null) qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
    }
    
    /*
     * Test for bug [#16236], which is queries on XML constructed from
     * another query are failing when they should succeed.
     */
    @Test
    public void testExecute_16236() throws Throwable {
	XmlTransaction txn = null;
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;
	XmlResults re2 = null;
	XmlValue val = new XmlValue();
	XmlDocument doc = mgr.createDocument();
	String content = "<player><League>1</League></player>";
	doc.setContent(content);
	doc.setName(docName);
	
	
	/*
	 * Construct Xml from Xml in a container, then use that as the
	 * context for another query that should succeed.
	 */
	String query1 = "<Players>{" +
			"(collection(\"" + CON_NAME + "\")/player)" +
			"}</Players>";
	String query2 = "player/League";

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		cont.putDocument(doc);
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
	
	/*
	 * Construct Xml from Xml in a container, then use that as the
	 * context for another query that starts at the nonexistant document
	 * node.  The second query should fail.
	 */
	query2 = "/Players/player";

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		try {
		    re2 = qe.execute(txn, val, context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
		try {
		    re2 = qe.execute(txn, val.getFirstChild(), context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		try {
		    re2 = qe.execute(val, context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
		try {
		    re2 = qe.execute(val.getFirstChild(), context);
		    fail("Should have failed in QueryExpressionTest.testExecute_ContextItem");
		} catch (XmlException e) {}
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
	
	/*
	 * Use Xml from a document as the context of another query,
	 * should succeed.
	 */
	query2 = "player/League";
	
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		doc = cont.getDocument(txn, docName);
		val = new XmlValue(doc);
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		doc = cont.getDocument(docName);
		val = new XmlValue(doc);
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null)qe.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	}
	
	/*
	 * Execute a secondary query on a container document returned
	 * by a query, should succeed.
	 */
	query1 = "doc(\"" + CON_NAME + "/" + docName + "\")";
	query2 = "player/League";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = mgr.query(txn, query1, context);
		val = re.next();
		qe = mgr.prepare(txn, query2, context);
		re2 = qe.execute(txn, val, context);
	    } else {
		re = mgr.query(query1, context);
		val = re.next();
		qe = mgr.prepare(query2, context);
		re2 = qe.execute(val, context);
	    }
	    assertEquals(1, re2.size());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null) qe.delete();
	    if (re != null) re.delete();
	    if (re2 != null) re2.delete();
	    if (txn != null) txn.commit();
	    txn = null;
	}
    }

    /*
     * Test for execute(XmlValue contextItem, XmlQueryContext queryContext, XmlDocumentConfig config) 
     * 
     */
    @Test
    public void testExecute_ValQcDc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	XmlResults re = null;
	XmlDocumentConfig xdc = new XmlDocumentConfig();
	XmlValue val = new XmlValue();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		qe = mgr.prepare(txn, query, context);
		re = qe.execute(txn, val, context , xdc);
	    } else {
		cont.putDocument(doc, xuc);
		qe = mgr.prepare(query, context);
		re = qe.execute(val, context , xdc);
	    }
	    assertEquals(re.size(), 1);
	} catch (XmlException e) {
	    if (qe != null) qe.delete();
	    if (txn != null) txn.commit();
	    throw e;
	} finally {
	    re.delete();
	    re = null;
	}
	
	//not valid XmlDocumentConfig flag
	xdc.setDocumentsOnly(true);
	try {
	    if (hp.isTransactional())
		re = qe.execute(txn, val, context, xdc);
	    else
		re = qe.execute(val, context, xdc);
	    fail("Not valid DocumentConfig");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} finally {
	    if (qe != null) qe.delete();
	    if (re != null) re.delete();
	    if (txn != null) txn.commit();
	}
    }

    /*
     * Test for getQuery()  
     * 
     */
    @Test
    public void testGetQuery() throws Throwable {
	XmlTransaction txn = null;
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		qe = mgr.prepare(txn, query, context);
	    } else {
		qe = mgr.prepare(query, context);
	    }
	    assertEquals(qe.getQuery(), query);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (qe != null) qe.delete();
	    if (txn != null) txn.commit();
	}
    }

    /*
     * Test for getQueryPlan()  
     * 
     */
    @Test
    public void testGetQueryPlan() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext xuc = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlQueryExpression qe = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, xuc);
		qe = mgr.prepare(txn, query, context);
	    } else {
		cont.putDocument(doc, xuc);
		qe = mgr.prepare(query, context);
	    }
	    assertNotNull(qe.getQueryPlan());
	    // there used to be a comparison to an expected plan,
	    // but that is too fragile.
	} catch (XmlException e) {
	    throw e;
	} finally {
	    qe.delete();
	    if (txn != null) txn.commit();
	}
    }

}
