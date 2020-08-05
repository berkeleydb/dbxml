/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 * $Id: ResultsTest.java,v 1.12 2008/04/30 13:02:58 lauren Exp $
 */
package dbxmltest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlValue;

public class ResultsTest {
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private TestConfig hp;
    private static final String CON_NAME = "testData.dbxml";
    private static String docString = "<?xml version=\"1.0\" ?>"
	+ "<a_node atr1=\"test\" atr2=\"test2\"><b_node/>"
	+ "<c_node>Other text</c_node><d_node/></a_node>";
    private static String docName = "testDoc.xml";

    private static String aName  = "a";
    private static String bName  = "b";
    private static String cName  = "c";
    private static String aText  = "a node text";
    private static String bText  = "b node text";
    private static String PI_target  = "PI_target";
    private static String PI_data  = "PI_data";
    private static String aName1 = "attr1";
    private static String aName2 = "attr2";
    private static String aText1 = "one";
    private static String aText2 = "two";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlResults!");
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
	System.out.println("Finished test XmlResults!");
    }

    /*
     * Test for constructor
     * 
     */
    @Test
	public void testConstructor() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	XmlResults results_copy = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		results = mgr.query(txn, myQuery, context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		results = mgr.query(myQuery, context);
	    }
	    results_copy = new XmlResults(results);
	    assertEquals(results_copy.size(), 3);
	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(results_copy != null) results_copy.delete();
	    results_copy = null;
	    if(results != null) results.delete();
	    results = null;
	}

	try {
	    results_copy = new XmlResults(null);
	} catch (XmlException e) {
	    throw e;
	} catch (NullPointerException e) {
	    assertNotNull(e.getMessage());
	}finally{
	    if(results_copy != null) results_copy.delete();
	    results_copy = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for add()
     * 
     */
    @Test
	public void testAdd() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = mgr.createResults();
	//test add a null object
	XmlValue va = new XmlValue();
	try{
	    results.add(va);
	    fail("Failure in ResultsTest.testAdd()");
	}catch(XmlException e){
	    assertNotNull(e.getMessage());
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	}

	XmlValue value = new XmlValue(XmlValue.STRING, "ddd");
	try{
	    results.add(value);
	    assertEquals(results.size(), 1);
	    results.add(value);
	    assertEquals(results.size(), 2);
	    results.add(value);
	    assertEquals(results.size(), 3);
	}catch(XmlException e){
	    throw e;
	}finally{
	    results.delete();
	}

	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results2 = null;
	XmlResults resultsLazy = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		results2 = mgr.query(txn, myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		results2 = mgr.query(myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }
	    results2.add(value);
	    assertEquals(results2.size(), 4);

	    /* Lazy results should throw */
	    try {
		resultsLazy.add(value);                        
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.LAZY_EVALUATION);
	    }

	} catch (XmlException e){
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(results2 != null) results2.delete();
	    if(resultsLazy != null) resultsLazy.delete();
	}

	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	value = new XmlValue(doc);
	XmlResults results3 = mgr.createResults();
	try{
	    results3.add(value);
	    XmlDocument doc2 = mgr.createDocument();
	    results3.next(doc2);
	    assertEquals(doc2.getContentAsString(), docString);
	    results3.reset();
	    value = results3.next();
	    doc2 = value.asDocument();
	    assertEquals(doc2.getContentAsString(), docString);
	}catch (XmlException e){
	    throw e;
	}finally{
	    results3.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }
    /*
     * Test for delete()
     * 
     */
    @Test
	public void testDelete() throws Throwable {
	XmlResults resultsEager = mgr.createResults();
	resultsEager.delete();
	// after delete
	try {
	    resultsEager.size();
	    fail("Fail to throw exception after results deleted.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	}

	/* test lazy results*/
	XmlQueryContext cx = mgr.createQueryContext();
	cx.setEvaluationType(XmlQueryContext.Lazy);
	XmlResults resultsLazy = mgr.query("1,2", cx);
	resultsLazy.delete();
    }

    /*
     * Test for getEvaluationType()
     * 
     */
    @Test
	public void testGetEvaluationType() throws Throwable {
	XmlResults resultsEager = mgr.createResults();
	try {
	    assertEquals(resultsEager.getEvaluationType(), XmlQueryContext.Eager);
	} finally{
	    resultsEager.delete();
	}

	/* test lazy results*/
	XmlQueryContext cx = mgr.createQueryContext();
	cx.setEvaluationType(XmlQueryContext.Lazy);
	XmlResults resultsLazy = mgr.query("1,2", cx);
	try {
	    assertEquals(resultsLazy.getEvaluationType(), XmlQueryContext.Lazy);
	} finally{
	    resultsLazy.delete();
	}

    }

    /*
     * Test for hasNext()
     * 
     */
    @Test
	public void testHasNext() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	context.setEvaluationType(XmlQueryContext.Eager);
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy =  null;

	boolean bool;
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "collection('testData.dbxml')/a_node/f_node", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "collection('testData.dbxml')/a_node/f_node", context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		resultsEager = mgr.query(myQuery, context);
		emptyEager = mgr.query("collection('testData.dbxml')/a_node/f_node", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("collection('testData.dbxml')/a_node/f_node", context);
	    }

	    /* test eager results */
	    /* At the begin of result set */
	    bool = resultsEager.hasNext();
	    assertTrue(bool);

	    /* in the middle */
	    resultsEager.next();
	    assertTrue(resultsEager.hasNext());

	    /* in the end of result set*/
	    resultsEager.next();
	    assertEquals(resultsEager.peek().asString(),"<d_node/>");
	    assertTrue(resultsEager.hasNext());

	    /* after last */
	    resultsEager.next();
	    assertFalse(resultsEager.hasNext());

	    /* test lazy results */
	    /* at the begin of result set */
	    bool = resultsLazy.hasNext();
	    assertTrue(bool);

	    /* in the middle */
	    resultsLazy.next();
	    assertTrue(resultsLazy.hasNext());

	    /* at the end of result set*/
	    resultsLazy.next();
	    assertEquals(resultsLazy.peek().asString(),"<d_node/>");
	    assertTrue(resultsLazy.hasNext());

	    /* after last */
	    resultsLazy.next();
	    assertFalse(resultsLazy.hasNext());

	    /* empty lazy*/
	    assertFalse(emptyLazy.hasNext());

	    /* empty eager*/
	    assertFalse(emptyLazy.hasNext());

	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;

	    if(hp.isTransactional() && txn != null) txn.commit();
	} 
    }

    /*
     * Test for hasPrevious()
     * 
     */
    @Test
	public void testHasPrevious() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	boolean bool;
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);       
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);    
	    }

	    /* test eager*/
	    /* at the begin of results set*/
	    bool = resultsEager.hasPrevious();
	    assertFalse(bool);

	    /* in the middle*/
	    resultsEager.next();
	    assertTrue(resultsEager.hasPrevious());

	    /* at last */
	    resultsEager.next();
	    assertTrue(resultsEager.hasPrevious());

	    /* after last */
	    resultsEager.next();
	    assertTrue(resultsEager.hasPrevious());

	    /* empty eager*/
	    assertFalse(emptyEager.hasPrevious());

	    /* test lazy exception*/
	    try {
		resultsLazy.hasPrevious();
	    }catch(XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	    /* test empty lazy results*/
	    try {
		emptyLazy.hasPrevious();
	    }catch(XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}



	// only one elements in the results
	myQuery = "collection('testData.dbxml')/a_node/d_node";
	try {
	    context.setEvaluationType(XmlQueryContext.Eager);
	    if(hp.isTransactional())
		resultsEager = mgr.query(txn, myQuery, context);
	    else
		resultsEager = mgr.query(myQuery, context);
	    resultsEager.next();
	    bool = resultsEager.hasPrevious();
	    assertTrue(bool);

	    resultsEager.next();
	    bool = resultsEager.hasPrevious();
	    assertTrue(bool);

	    resultsEager.next();
	    bool = resultsEager.hasPrevious();
	    assertTrue(bool);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for isNull()
     * 
     */
    @Test
	public void testIsNull() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	boolean bool;
	// test empty results
	try {
	    results = mgr.createResults();
	    bool = results.isNull();
	    assertFalse(bool);
	} finally{
	    if(results != null) results.delete();
	    results = null;
	}

	// test non-empty results
	String myQuery = "collection('testData.dbxml')/a_node/*";
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		results = mgr.query(txn, myQuery, context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		results = mgr.query(myQuery, context); 
	    }
	    bool = results.isNull();
	    assertFalse(bool);
	} finally{
	    if(results != null) results.delete();
	    results = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for : XmlValue next()
     * 
     */
    @Test
	public void testNext_value() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	XmlValue value = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context); 
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);        
	    }

	    /* test eager results*/
	    /* at begin */
	    value = resultsEager.next();
	    assertEquals("<b_node/>", value.asString());

	    /* in the middle*/
	    value = resultsEager.next();
	    assertEquals("<c_node>Other text</c_node>",value.asString());

	    /* at last*/
	    value = resultsEager.next();
	    assertEquals("<d_node/>",value.asString());

	    /* after last*/
	    value = resultsEager.next();
	    assertNull(value);

	    /* empty eager*/
	    value = emptyEager.next();
	    assertNull(value);

	    /* test lazy results*/
	    /* at begin */
	    value = resultsLazy.next();
	    assertEquals("<b_node/>", value.asString());

	    /* in the middle*/
	    value = resultsLazy.next();
	    assertEquals("<c_node>Other text</c_node>",value.asString());

	    /* at last*/
	    value = resultsLazy.next();
	    assertEquals("<d_node/>",value.asString());

	    /* after last*/
	    value = resultsLazy.next();
	    assertNull(value);

	    /* empty eager*/
	    value = emptyLazy.next();
	    assertNull(value);

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}

	try {
	    resultsEager = mgr.createResults();
	    value = resultsEager.next();
	    assertNull(value);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for : boolean next(XmlDocument document)
     * 
     */
    @Test
	public void testNext_bool() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	boolean bool;
	XmlDocument document = mgr.createDocument();

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, "testdoc1.xml", docString, theContext);
		cont.putDocument(txn, "testdoc2.xml", docString, theContext);
		cont.putDocument(txn, "testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument("testdoc1.xml", docString, theContext);
		cont.putDocument("testdoc2.xml", docString, theContext);
		cont.putDocument("testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);
	    }

	    /* test eager results*/
	    bool = resultsEager.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc1.xml");

	    bool = resultsEager.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc2.xml");

	    /* middle */
	    bool = resultsEager.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc3.xml");

	    /* at the last place */
	    bool = resultsEager.next(document);
	    assertFalse(bool);

	    /* after last */
	    assertFalse(resultsEager.next(document));

	    /* test empty eager*/
	    assertFalse(resultsEager.next(document));

	    /* test last results*/
	    /* at begin*/
	    bool = resultsLazy.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc1.xml");

	    bool = resultsLazy.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc2.xml");

	    /* middle */
	    bool = resultsLazy.next(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc3.xml");

	    /* at the last place */
	    bool = resultsLazy.next(document);
	    assertFalse(bool);

	    /* after last */
	    assertFalse(resultsLazy.next(document));

	    /* test empty lazy*/
	    assertFalse(emptyLazy.next(document));

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}
	// Test not query as an document
	myQuery = "collection('testData.dbxml')/a_node/f_node";
	XmlDocument doc = mgr.createDocument();
	try {
	    if(hp.isTransactional())
		resultsEager = mgr.query(txn, myQuery, context);
	    else
		resultsEager = mgr.query(myQuery, context); 
	    bool = resultsEager.next(doc);
	    assertFalse(bool);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for: XmlValue peek()
     * 
     */
    @Test
	public void testPeek_value() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	XmlValue value = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);  
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);
	    }

	    /* test eager*/
	    /* at begin*/
	    value = resultsEager.peek();
	    assertEquals("<b_node/>", value.asString());

	    /* in the middle*/
	    resultsEager.next();
	    value = resultsEager.peek();
	    assertEquals("<c_node>Other text</c_node>",value.asString());

	    /* at last */
	    resultsEager.next();
	    value = resultsEager.peek();
	    assertEquals("<d_node/>",value.asString());

	    /* after last */
	    resultsEager.next();
	    value = resultsEager.peek();
	    assertNull(value);

	    /* empty eager*/
	    value = emptyEager.peek();
	    assertNull(value);

	    /* test lazy*/
	    /* at begin*/
	    value = resultsLazy.peek();
	    assertEquals("<b_node/>", value.asString());

	    /* in the middle*/
	    resultsLazy.next();
	    value = resultsLazy.peek();
	    assertEquals("<c_node>Other text</c_node>",value.asString());

	    /* at last */
	    resultsLazy.next();
	    value = resultsLazy.peek();
	    assertEquals("<d_node/>",value.asString());

	    /* after last */
	    resultsLazy.next();
	    value = resultsLazy.peek();
	    assertNull(value);

	    /* empty eager*/
	    value = emptyLazy.peek();
	    assertNull(value);

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}

	try {
	    resultsEager = mgr.createResults();
	    value = resultsEager.peek();
	    assertNull(value);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for : boolean peek(XmlDocument document)
     * 
     */
    @Test
	public void testPeek_bool() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	boolean bool;
	XmlDocument document = mgr.createDocument();

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, "testdoc1.xml", docString, theContext);
		cont.putDocument(txn, "testdoc2.xml", docString, theContext);
		cont.putDocument(txn, "testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument("testdoc1.xml", docString, theContext);
		cont.putDocument("testdoc2.xml", docString, theContext);
		cont.putDocument("testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);
	    }

	    /* test eager results*/
	    /* at begin*/
	    bool = resultsEager.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc1.xml");

	    /* middle*/
	    resultsEager.next();
	    bool = resultsEager.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc2.xml");

	    /* at last*/
	    resultsEager.next();
	    bool = resultsEager.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc3.xml");

	    /* after last */
	    resultsEager.next();
	    bool = resultsEager.peek(document);
	    assertFalse(bool);

	    /* empty eager*/
	    assertFalse(emptyEager.peek(document));

	    /* test lazy results*/
	    /* at begin*/
	    bool = resultsLazy.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc1.xml");

	    /* middle*/
	    resultsLazy.next();
	    bool = resultsLazy.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc2.xml");

	    /* at last*/
	    resultsLazy.next();
	    bool = resultsLazy.peek(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc3.xml");

	    /* after last */
	    resultsLazy.next();
	    bool = resultsLazy.peek(document);
	    assertFalse(bool);

	    /* empty eager*/
	    assertFalse(emptyLazy.peek(document));

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}

	// Test not query as an document
	myQuery = "collection('testData.dbxml')/a_node/f_node";
	XmlDocument doc = mgr.createDocument();
	try {
	    if(hp.isTransactional()){
		context.setEvaluationType(XmlQueryContext.Eager);    
		resultsEager = mgr.query(txn, myQuery, context); 
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    }
	    else{
		context.setEvaluationType(XmlQueryContext.Eager);    
		resultsEager = mgr.query(myQuery, context); 
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }
	    bool = resultsEager.peek(doc);
	    assertFalse(bool);

	    bool = resultsLazy.peek(doc);
	    assertFalse(bool);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for: XmlValue previous()
     * 
     */
    @Test
	public void testPrevious_value() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	XmlValue value = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context); 
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);   
	    }

	    /* test eager result*/
	    /* at begin*/
	    value = resultsEager.previous();
	    assertNull(value);

	    /* in middle */
	    resultsEager.next();
	    value = resultsEager.previous();
	    assertNotNull(value);
	    assertEquals("<b_node/>", value.asString());//get first node

	    /* at last*/
	    resultsEager.next();
	    resultsEager.next();
	    value = resultsEager.previous();
	    assertNotNull(value);
	    assertEquals("<c_node>Other text</c_node>", value.asString());//get second node

	    /* at after last*/
	    resultsEager.next();
	    resultsEager.next();
	    value = resultsEager.previous();
	    assertNotNull(value);
	    assertEquals("<d_node/>", value.asString());//get third node

	    /* empty eager*/
	    value = emptyEager.previous();
	    assertNull(value);

	    /* test lazy results*/
	    try {
		resultsLazy.previous();
		fail("Fail to throw exception");
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	    /* test empty lazy results*/
	    try {
		emptyLazy.previous();
		fail("Fail to throw exception");
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}

	try {
	    resultsEager = mgr.createResults();
	    value = resultsEager.previous();
	    assertNull(value);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for: boolean previous(XmlDocument document)
     * 
     */
    @Test
	public void testPrevious_bool() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults emptyEager = null;
	XmlResults emptyLazy = null;
	boolean bool;
	XmlDocument document = mgr.createDocument();

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, "testdoc1.xml", docString, theContext);
		cont.putDocument(txn, "testdoc2.xml", docString, theContext);
		cont.putDocument(txn, "testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		emptyEager = mgr.query(txn, "()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
		emptyLazy = mgr.query(txn, "()", context);
	    }else{
		cont.putDocument("testdoc1.xml", docString, theContext);
		cont.putDocument("testdoc2.xml", docString, theContext);
		cont.putDocument("testdoc3.xml", docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context); 
		emptyEager = mgr.query("()", context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
		emptyLazy = mgr.query("()", context);  
	    }

	    /* test eager results*/
	    /* at begin*/
	    bool = resultsEager.previous(document);
	    assertFalse(bool);

	    /* middle */
	    resultsEager.next();
	    bool = resultsEager.previous(document);
	    assertTrue(bool);
	    assertEquals(document.getName(),"testdoc1.xml");

	    /* at last*/ 
	    resultsEager.next();
	    resultsEager.next();
	    bool = resultsEager.previous(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc2.xml");

	    /* after last*/
	    resultsEager.next();
	    resultsEager.next();
	    bool = resultsEager.previous(document);
	    assertTrue(bool);
	    assertEquals(document.getName(), "testdoc3.xml");

	    /* empty eager*/
	    bool = emptyEager.previous(document);
	    assertFalse(bool);

	    /* lazy throw exception*/
	    try {
		resultsLazy.previous(document);
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	    /* test empty lazy results*/
	    try {
		emptyLazy.previous(document);
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(),XmlException.INVALID_VALUE);
	    }

	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;

	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;

	    if(emptyEager != null) emptyEager.delete();
	    emptyEager = null;

	    if(emptyLazy != null) emptyLazy.delete();
	    emptyLazy = null;
	}

	// Test not query as an document
	myQuery = "collection('testData.dbxml')/a_node/f_node";
	XmlDocument doc = mgr.createDocument();
	try {
	    context.setEvaluationType(XmlQueryContext.Eager);
	    if(hp.isTransactional())
		resultsEager = mgr.query(txn, myQuery, context);
	    else
		resultsEager = mgr.query(myQuery, context);
	    bool = resultsEager.previous(doc);
	    assertFalse(bool);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for reset()
     * 
     */
    @Test
	public void testReset() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlValue value = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context); 
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }

	    /* test eager*/
	    value = resultsEager.next();
	    assertEquals("<b_node/>", value.asString());
	    resultsEager.reset();
	    value = resultsEager.next();
	    assertEquals("<b_node/>", value.asString());

	    resultsEager.next();
	    resultsEager.next();
	    resultsEager.next();
	    resultsEager.reset();
	    value = resultsEager.next();
	    assertEquals("<b_node/>", value.asString());

	    resultsEager.reset();
	    resultsEager.reset();
	    value = resultsEager.next();
	    assertEquals("<b_node/>", value.asString());

	    /* test lazy*/
	    value = resultsLazy.next();
	    assertEquals("<b_node/>", value.asString());
	    resultsLazy.reset();
	    value = resultsLazy.next();
	    assertEquals("<b_node/>", value.asString());

	    resultsLazy.next();
	    resultsLazy.next();
	    resultsLazy.next();
	    resultsLazy.reset();
	    value = resultsLazy.next();
	    assertEquals("<b_node/>", value.asString());

	    resultsLazy.reset();
	    resultsLazy.reset();
	    value = resultsLazy.next();
	    assertEquals("<b_node/>", value.asString());

	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for size()
     * 
     */
    @Test
	public void testSize() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	XmlResults resultsLazy = null;

	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		results = mgr.query(txn, myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    }else{
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		results = mgr.query(myQuery, context); 
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }
	    assertEquals(results.size(), 3);

	    /* Lazy results should throw */
	    try {
		resultsLazy.size();
		fail("Lazy results should throw exception");
	    }catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.LAZY_EVALUATION);
	    }
	} catch (XmlException e) {
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally{
	    if(results != null) results.delete();
	    results = null;
	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;
	}

	myQuery = "collection('testData.dbxml')/a_node/d";
	try {
	    context.setEvaluationType(XmlQueryContext.Eager);
	    if(hp.isTransactional())
		results = mgr.query(txn, myQuery, context);
	    else
		results = mgr.query(myQuery, context);
	    assertEquals(results.size(), 0);
	} finally{
	    if(results != null) results.delete();
	    results = null;
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for asEventWriter(): single top node & empty element
     */
    @Test
	public void testAsEventWriter1() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";
	XmlResults vres = null;

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<b_node/>" +
		    "<a attr1=\"one\" attr2=\"two\">a node text<b>b node text</b><c/></a>" +
		    "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();

		w.writeStartElement(aName, null, null, 2, false);
		w.writeAttribute(aName1, null, null, aText1, true);
		w.writeAttribute(aName2, null, null, aText2, true);
		w.writeText(XmlEventReader.Characters, aText);
		w.writeStartElement(bName, null, null, 0, false);
		w.writeText(XmlEventReader.Characters, bText);
		w.writeEndElement(bName, null, null);
		w.writeStartElement(cName, null, null, 0, true);
		w.writeEndElement(aName, null, null);

		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null) results.delete();
		results = null;
		if (vres != null) vres.delete();
		vres = null;
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter(): test multiple top-layer nodes
     */
    @Test
	public void testAsEventWriter2() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults vres = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<b_node/>" +
		    "<a attr1=\"one\">a node text</a><b/><c/>" +
		    "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();

		w.writeStartElement(aName, null, null, 1, false);
		w.writeAttribute(aName1, null, null, aText1, true);
		w.writeText(XmlEventReader.Characters, aText);
		w.writeEndElement(aName, null, null);
		w.writeStartElement(bName, null, null, 0, false);
		w.writeEndElement(bName, null, null);
		w.writeStartElement(cName, null, null, 0, true);

		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null)
		    results.delete();
		results = null;
		if (vres != null) vres.delete();
		vres = null;
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter(): test single document with single root node
     */
    @Test
	public void testAsEventWriter3() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults vres = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<b_node/>" +
		    "<a attr1=\"one\" attr2=\"two\">a node text<b>b node text</b><c/></a>" +
		    "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();

		w.writeStartDocument(null, null, null);
		w.writeStartElement(aName, null, null, 2, false);
		w.writeAttribute(aName1, null, null, aText1, true);
		w.writeAttribute(aName2, null, null, aText2, true);
		w.writeText(XmlEventReader.Characters, aText);
		w.writeStartElement(bName, null, null, 0, false);
		w.writeText(XmlEventReader.Characters, bText);
		w.writeEndElement(bName, null, null);
		w.writeStartElement(cName, null, null, 0, true);
		w.writeEndElement(aName, null, null);
		w.writeEndDocument();

		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if (vres != null) vres.delete();
		vres = null;
		if(results != null)
		    results.delete();
		results = null;
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter():
     *   test single document with multiple root node(exception should be thrown)
     */
    @Test
	public void testAsEventWriter4() throws Throwable {
	XmlResults newNode = null;
	XmlEventWriter w = null;
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	for(int i=0; i<evaluationType.length; i++){
	    try {
		newNode = mgr.createResults();
		w = newNode.asEventWriter();

		w.writeStartDocument(null, null, null);
		w.writeStartElement(aName, null, null, 1, false);
		w.writeAttribute(aName1, null, null, aText1, true);
		w.writeText(XmlEventReader.Characters, aText);
		w.writeEndElement(aName, null, null);
		w.writeStartElement(bName, null, null, 0, false);
		w.writeEndElement(bName, null, null);
		w.writeStartElement(cName, null, null, 0, true);
		w.writeEndDocument();
		w.close();
		fail("Fail to throw exception");
	    } catch (XmlException e) {
		try { 
		    if (w != null) w.close();
		} catch (XmlException e2) {}
	    }finally {
		if (newNode != null) newNode.delete();

	    }
	}
    }

    /*
     * Test for asEventWriter(): test multiple documents
     */
    @Test
	public void testAsEventWriter5() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults newNode = null;
	XmlResults vres = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<b_node/>" +
		    "<a attr1=\"one\" attr2=\"two\">a node text<c/></a><b>b node text</b>" +
		    "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();

		w.writeStartDocument(null, null, null);
		w.writeStartElement(aName, null, null, 2, false);
		w.writeAttribute(aName1, null, null, aText1, true);
		w.writeAttribute(aName2, null, null, aText2, true);
		w.writeText(XmlEventReader.Characters, aText);
		w.writeStartElement(cName, null, null, 0, true);
		w.writeEndElement(aName, null, null);
		w.writeEndDocument();
		w.writeStartDocument(null, null, null);
		w.writeStartElement(bName, null, null, 0, false);
		w.writeText(XmlEventReader.Characters, bText);
		w.writeEndElement(bName, null, null);
		w.writeEndDocument();

		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null)
		    results.delete();
		results = null;
		if (newNode != null) newNode.delete();
		if (vres != null) vres.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter(): test top-layer text nodes
     */
    @Test
	public void testAsEventWriter6() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults vres = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\"><b_node/>" +
		    aText1 + "<!--" + bText + "-->" + aText2 +
		    "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();
		w.writeText(XmlEventReader.Characters, aText1);
		w.writeText(XmlEventReader.Comment, bText);
		w.writeText(XmlEventReader.CDATA, aText2);
		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null)
		    results.delete();
		results = null;
		if (vres != null) vres.delete();
		if (newNode != null)  newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter(): test multiple top-layer attribute nodes
     */
    @Test
	public void testAsEventWriter7() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults vres = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "insert node $newNode as first into $node";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\"><b_node/><c_node " +
		    aName1 + "=\"" + aText1 + "\" " +
		    aName2 + "=\"" + aText2 + "\">" +
		    "Other text</c_node><d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();
		w.writeAttribute(aName1, null, null, aText1, false);
		w.writeAttribute(aName2, null, null, aText2, false);
		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null)
		    results.delete();
		results = null;
		if (vres != null) vres.delete();
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter():
     *   multiple top-layer Processing Instructions nodes
     */
    @Test
	public void testAsEventWriter8() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults vres = null;
	XmlResults newNode = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node";
	String updateQuery = "insert node $newNode as first into $node";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<?PI_target PI_data?>" +
		    "<b_node/><c_node>Other text</c_node><d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();
		w.writeProcessingInstruction(PI_target, PI_data);
		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if (results != null)
		    results.delete();
		results = null;
		if (vres != null) vres.delete();
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter():
     *   test not-yet-complete elements(exception should be thrown)
     */
    @Test
	public void testAsEventWriter9() throws Throwable {
	XmlResults newNode = null;
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};
	XmlEventWriter w = null;

	for(int i=0; i<evaluationType.length; i++){
	    try {
		newNode = mgr.createResults();
		w = newNode.asEventWriter();

		w.writeStartElement( aName, null, null, 2, false);
		w.writeAttribute( aName1, null, null, aText1, true );
		w.writeEndElement( aName, null, null);
		w.close();
		fail("Fail to throw exception");
	    } catch (XmlException e) {
		try { 
		    if (w != null) w.close();
		} catch (XmlException e2) {}
	    } finally {
		if (newNode != null) newNode.delete();
	    }
	}
    }

    /*
     * Test for asEventWriter(): test close twice
     */
    @Test
	public void testAsEventWriter10() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	XmlResults newNode = null;
	XmlResults vres = null;
	boolean isTxn = hp.isTransactional();
	int []evaluationType = {XmlQueryContext.Eager, XmlQueryContext.Lazy};

	String query = "collection('testData.dbxml')/a_node/c_node";
	String updateQuery = "replace node $node with $newNode";

	for(int i=0; i<evaluationType.length; i++){
	    try {
		XmlUpdateContext uc = mgr.createUpdateContext();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(evaluationType[i]);

		if(isTxn){
		    txn = mgr.createTransaction();
		    cont.putDocument(txn, docName, docString, uc);
		    results = mgr.query(txn, query, qc);
		}else{
		    cont.putDocument(docName, docString, uc);
		    results = mgr.query(query, qc);
		}

		String verifyQuery = "collection('testData.dbxml')/a_node";
		String verifyContent =
		    "<a_node atr1=\"test\" atr2=\"test2\">" +
		    "<b_node/>" + aText + "<d_node/></a_node>";

		newNode = mgr.createResults();
		XmlEventWriter w = newNode.asEventWriter();
		w.writeText(XmlEventReader.Characters, aText);
		w.close();

		// Query update
		qc.setVariableValue("node", results);
		qc.setVariableValue("newNode", newNode);

		if(isTxn){
		    XmlResults res = mgr.query(txn, updateQuery, qc);
		    res.delete();
		    txn.commit();
		    txn = null;
		}else{
		    XmlResults res = mgr.query(updateQuery, qc);
		    res.delete();
		}

		// Verify
		XmlQueryContext vqc = mgr.createQueryContext();
		if (isTxn) {
		    txn = mgr.createTransaction();
		    vres = mgr.query(txn, verifyQuery, vqc);
		} else
		    vres = mgr.query(verifyQuery, vqc);	
		String verifyString = new String();
		while(vres.hasNext()) {
		    XmlValue va = vres.next();
		    verifyString += va.asString();
		}
		assertEquals(verifyString, verifyContent);
		// delete the old doc
		if (isTxn)
		    cont.deleteDocument(txn, docName, uc);
		else
		    cont.deleteDocument(docName, uc);
	    } finally {
		if(results != null)
		    results.delete();
		results = null;
		if (vres != null) vres.delete();
		if (newNode != null) newNode.delete();
		if (txn != null) txn.commit();
	    }
	}
    }

    /*
     * Test for asEventWriter():
     *   test using asEventWriter by a non-empty XmlResults
     */
    @Test
	public void testAsEventWriter11() throws Throwable {
	XmlTransaction txn = null;
	XmlResults results = null;
	boolean isTxn = hp.isTransactional();

	String query = "collection('testData.dbxml')/a_node/c_node";

	try {
	    XmlUpdateContext uc = mgr.createUpdateContext();
	    XmlQueryContext qc = mgr.createQueryContext();

	    if (isTxn) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, uc);
		results = mgr.query(txn, query, qc);
	    } else {
		cont.putDocument(docName, docString, uc);
		results = mgr.query(query, qc);
	    }

	    String verifyString = "<c_node>Other text</c_node>"
		+ "<a attr1=\"one\" attr2=\"two\">a node text<c/></a>"
		+ "<b>b node text</b>";

	    XmlEventWriter w = results.asEventWriter();
	    w.writeStartDocument(null, null, null);
	    w.writeStartElement(aName, null, null, 2, false);
	    w.writeAttribute(aName1, null, null, aText1, true);
	    w.writeAttribute(aName2, null, null, aText2, true);
	    w.writeText(XmlEventReader.Characters, aText);
	    w.writeStartElement(cName, null, null, 0, true);
	    w.writeEndElement(aName, null, null);
	    w.writeEndDocument();
	    w.writeStartDocument(null, null, null);
	    w.writeStartElement(bName, null, null, 0, false);
	    w.writeText(XmlEventReader.Characters, bText);
	    w.writeEndElement(bName, null, null);
	    w.writeEndDocument();

	    w.close();

	    String recontent = "";
	    while(results.hasNext()){
		recontent = recontent+results.next().asString();
	    }

	    assertEquals(recontent, verifyString);

	} finally {
	    if (isTxn && txn != null)
		txn.commit();
	    if (results != null)
		results.delete();
	    results = null;
	}

    }

    /*
     * Test for asEventWriter():
     *   test a few odd conditions
     */
    @Test
	public void testAsEventWriter12() throws Throwable {
	XmlResults results = null;
	String query = "collection('testData.dbxml')/a_node/c_node";

	try {
	    results = mgr.createResults();
	    XmlEventWriter w = results.asEventWriter();

	    w.writeStartDocument(null, null, null);
	    w.writeStartElement(aName, null, null, 0, false);
	    w.writeText(XmlEventReader.Characters, aText);
	    w.writeStartElement(cName, null, null, 0, true);
	    w.writeEndElement(aName, null, null);
	    w.writeEndDocument();
	    String recontent = "";
	    while(results.hasNext()){
		recontent = recontent+results.next().asString();
	    }
	    String verify1="<a>a node text<c/></a>";
	    assertEquals(recontent, verify1);

	    // reset should have no effect on writer, it just
	    // resets the "current" value in the results
	    results.reset();
	    w.writeStartElement(bName, null, null, 0, false);
	    w.writeText(XmlEventReader.Characters, bText);
	    w.writeStartElement(cName, null, null, 0, true);
	    w.writeEndElement(bName, null, null);
	    recontent = "";
	    while(results.hasNext()){
		recontent = recontent+results.next().asString();
	    }
	    // new content has been concatenated
	    String verify2="<a>a node text<c/></a><b>b node text<c/></b>";
	    assertEquals(recontent, verify2);

	    // test deleting results before closing writer -- this
	    // should work silently
	    results.delete();
	    results = null;
	    w.close();

        //test call asEventWriter twice.
        XmlResults res6 = mgr.createResults();
        XmlEventWriter w6 = res6.asEventWriter();
        w6.close();
        XmlEventWriter w7 = res6.asEventWriter();
        w7.close();
        res6.delete();

        // Test can't get XmlEventWriter twice without close first
        
        XmlResults res7 = mgr.createResults();
        XmlEventWriter w8 = null; 
        try {
            w8 = res7.asEventWriter();
            XmlEventWriter w9 = res6.asEventWriter();
        } catch (XmlException e) {
            assertNotNull(e);
            assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
        }
        w8.close();
        res7.delete();



	} finally {
	    if (results != null)
		results.delete();
	    results = null;
	}
    }

    @Test
	public void testCopyResults() throws Throwable{
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')//node()";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults resultCopied = null;
	XmlResults re = null;
	XmlResults empty = null;

	try {       	
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    } else {
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }

	    // copy from eager
	    resultCopied = resultsEager.copyResults();

	    while (resultCopied.hasNext()) {
		assertEquals(resultCopied.next().asString(), resultsEager.next().asString());
	    }
	    resultCopied.delete();

	    // copy form lazy
	    resultCopied = resultsLazy.copyResults();
	    resultsEager.reset();
	    while (resultCopied.hasNext()) {
		assertEquals(resultCopied.next().asString(), resultsEager.next().asString());
	    }

	    // The lazy result will iterate to last 
	    assertFalse(resultsLazy.hasNext());
	    resultCopied.delete();

	    resultCopied = resultsLazy.copyResults();
	    assertEquals(resultCopied.size(), 0);
	    resultCopied.delete();

	    // copy from empty results
	    empty = mgr.createResults();
	    resultCopied = empty.copyResults();
	    assertEquals(resultCopied.size(), 0);
	    empty.delete();
	    resultCopied.delete();

	    // copy on closed results
	    try {
		resultsEager.delete();
		resultCopied = resultsEager.copyResults();
		fail("Fail to throw exception when copyResults On closed results.");		
	    } catch (XmlException e) {
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		resultsEager = null;
	    }
	    resultCopied.delete();

	    // copy stand alone results
	    String verfiyStr = "aa<a>a node text</a>{}attr1=\"one\"a node text<b>a node text</b><?do ring?>";

	    re = mgr.createResults();
	    re.add(new XmlValue("aa"));
	    XmlEventWriter w = re.asEventWriter();
	    w.writeStartDocument(null, null, null);
	    w.writeStartElement(aName, null, null, 0, false);
	    w.writeText(XmlEventReader.Characters, aText);
	    w.writeEndElement(aName, null, null);
	    w.writeEndDocument();

	    w.writeAttribute(aName1, null, null, aText1, true);
	    w.writeText(XmlEventReader.Characters, aText);

	    w.writeStartElement(bName, null, null, 0, false);
	    w.writeText(XmlEventReader.Characters, aText);
	    w.writeEndElement(bName, null, null);
	    w.writeProcessingInstruction("do", "ring");

	    w.close();

	    re.reset();
	    resultCopied = re.copyResults();

	    String resultStr = "";
	    while(resultCopied.hasNext()){
		resultStr += resultCopied.next().asString();
	    }
	    assertEquals(verfiyStr, resultStr);
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;
	    if (resultCopied != null) resultCopied.delete();
	    if (re != null) re.delete();
	    if (empty != null) empty.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}

    }

    @Test
	public void testConcatResults() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext theContext = mgr.createUpdateContext();
	String myQuery = "collection('testData.dbxml')//node()";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults resultsEager = null;
	XmlResults resultsLazy = null;
	XmlResults compare = null;
	XmlResults re1 = null;
	XmlResults re2 = null;
	XmlResults re3 = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(txn, myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    } else {
		cont.putDocument(docName, docString, theContext);
		context.setEvaluationType(XmlQueryContext.Eager);
		resultsEager = mgr.query(myQuery, context);
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }

	    compare = resultsEager.copyResults();
	    resultsEager.reset();

	    //1. concatenates two empty results
	    re1 = mgr.createResults();
	    re2 = mgr.createResults();

	    re1.concatResults(re2);
	    assertEquals(re1.size(),0);
	    re1.delete();
	    re2.delete();

	    //2. concatenates eager/lazy into empty
	    re1 = mgr.createResults();
	    re1.concatResults(resultsEager);
	    resultsEager.reset();
	    while(re1.hasNext()) {
		assertEquals(re1.next().asString(), compare.next().asString());
	    }
	    re1.delete();

	    re1 = mgr.createResults();
	    re1.concatResults(resultsLazy);
	    compare.reset();
	    while(re1.hasNext()) {
		assertEquals(re1.next().asString(), compare.next().asString());
	    }
	    re1.delete();
	    resultsLazy.delete();

	    //3. concatenates empty into eager
	    re1 = mgr.createResults();
	    resultsEager.concatResults(re1);
	    compare.reset();
	    resultsEager.reset();
	    assertEquals(resultsEager.size(), compare.size());
	    while(resultsEager.hasNext()) {
		assertEquals(compare.next().asString(), resultsEager.next().asString());
	    }

	    re1.delete();

	    //4. call concatResults() on lazy (should fail) 
	    if (hp.isTransactional()) {
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    } else {
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }

	    re1 = mgr.createResults();

	    try {
		resultsLazy.concatResults(re1);
	    } catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.LAZY_EVALUATION);
	    }
	    re1.delete();
	    resultsLazy.delete();


	    //5. concatenates lazy into eager
	    if (hp.isTransactional()) {
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(txn, myQuery, context);
	    } else {
		context.setEvaluationType(XmlQueryContext.Lazy);
		resultsLazy = mgr.query(myQuery, context);
	    }
	    resultsEager.reset();
	    resultsEager.concatResults(resultsLazy);
	    assertEquals(resultsEager.size(), compare.size()*2);
	    for (int i=0; i<2; i++){
		compare.reset();
		while(compare.hasNext()){
		    assertEquals(compare.next().asString(), resultsEager.next().asString());
		}
	    }
	    resultsLazy.delete();

	    //6. concatenates eager into eager
	    compare.reset();
	    resultsEager.reset();
	    resultsEager.concatResults(compare);
	    assertEquals(resultsEager.size(), compare.size()*3);
	    for (int i=0; i<3; i++){
		compare.reset();
		while(compare.hasNext()){
		    assertEquals(compare.next().asString(), resultsEager.next().asString());
		}
	    }

	    //7. call concatResults() on closed results
	    try {
		resultsLazy.concatResults(compare);
		fail("Fail to throw exception when call concatResults on closed results.");
	    } catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	    }

	    //9  concatenates three results set
	    re1 = mgr.createResults();
	    re2 = mgr.createResults();
	    re3 = mgr.createResults();
	    re1.add(new XmlValue("1"));
	    re2.add(new XmlValue("2"));
	    re3.add(new XmlValue("3"));

	    re1.concatResults(re2);
	    re1.concatResults(re3);

	    re1.reset();
	    assertEquals(re1.size(), 3);
	    for (int i=0; i<3; i++){
		assertEquals(re1.next().asString(), String.valueOf(i+1));
	    }
	} finally{
	    if(resultsEager != null) resultsEager.delete();
	    resultsEager = null;
	    if(resultsLazy != null) resultsLazy.delete();
	    resultsLazy = null;
	    if (compare != null) compare.delete();
	    if (re1 != null) re1.delete();
	    if (re2 != null) re2.delete();
	    if (re3 != null) re3.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}

	//8. concateResults two results after transaction commit()
	if (hp.isTransactional()) {
	    re1 = null;
	    re2 = null;
	    try {     	
		txn = mgr.createTransaction();
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setEvaluationType(XmlQueryContext.Eager);
		re1 = mgr.query(txn, myQuery, qc);
		txn.commit();
		txn = null;

		re2 = mgr.createResults();				
		re2.concatResults(re1);

		if (hp.isNodeContainer())
		    fail("Fail to throw exception.");
		else {// if it is whole doc it could be success.
		    re1.reset();
		    assertEquals(re2.size(), re1.size());
		    while(re2.hasNext()) 
			assertEquals(re2.next().asString(),re1.next().asString());
		}
	    } catch(XmlException e) {
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.TRANSACTION_ERROR);
	    } finally {
		if (txn!=null) txn.commit();
		if (re1!= null) re1.delete();
		if (re2!= null) re2.delete();
	    }
	}
    }
}
