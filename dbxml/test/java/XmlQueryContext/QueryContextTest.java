package dbxmltest;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;
import static org.junit.Assert.assertNull;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.*;

public class QueryContextTest {
    private TestConfig hp;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";
    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</a_node>";
    private static String docName = "testDoc.xml";
    private static String query = "collection('" + CON_NAME + "')/a_node/c_node";

    @BeforeClass
    public static void setupClass() {
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	System.out.println("Begin test XmlQueryContext!");
    }

    @Before
    public void setUp() throws Throwable {
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
		XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	XmlManagerConfig conf= new XmlManagerConfig();
	conf.setAllowExternalAccess(true);
	mgr = hp.createManager(hp.getEnvironmentPath(), conf);
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
	System.out.println("Finished test XmlQueryContext!");
    }

    @Test
    public void testCopyConstructor() throws XmlException {
	String prefix = "old";
	XmlQueryContext original = mgr.createQueryContext(XmlQueryContext.LiveValues, XmlQueryContext.Lazy);
	original.setBaseURI("http://dbxmltest.test");
	original.setDefaultCollection(CON_NAME);
	original.setQueryTimeoutSeconds(10);
	original.setNamespace(prefix, "http://dbxmltest.test/test");
	XmlValue testValue = new XmlValue("test");
	original.setVariableValue("name", testValue);

	XmlQueryContext copied = new XmlQueryContext(original);
	assertEquals(original.getReturnType(), copied.getReturnType());
	assertEquals(original.getEvaluationType(), copied.getEvaluationType());
	assertEquals(original.getBaseURI(), copied.getBaseURI());
	assertEquals(original.getDefaultCollection(), copied.getDefaultCollection());
	assertEquals(original.getQueryTimeoutSeconds(), copied.getQueryTimeoutSeconds());
	assertEquals(original.getNamespace(prefix), copied.getNamespace(prefix));
	XmlValue origValue = original.getVariableValue("name");
	XmlValue copiedValue = copied.getVariableValue("name");
	assertTrue(origValue.equals(copiedValue));
	
	String docString = "<?xml version=\"1.0\" "
	    + "encoding=\"UTF-8\"?><a_node xmlns=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</a_node>";
	String query = "collection('" + CON_NAME + "')/old:a_node/old:c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, copied);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, copied);
	    }    
	    assert(res.size() > 0);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	    res = null;
	}

	try{
	    copied = new XmlQueryContext(null);
	    fail("Failure in QueryContextTest.testCopyConstructor()");
	}catch (NullPointerException e){}
    }

    @Test
    public void testGetSetBaseURI() throws XmlException {
	String badURI = "slijfla;s";
	String baseURI = "dbxml:/" + CON_NAME;
	XmlQueryContext context = mgr.createQueryContext();

	//Test getBaseURI when nothing is set
	assertEquals("dbxml:/", context.getBaseURI());

	//Test setBaseURI with a bad uri
	try{
	    context.setBaseURI(badURI);
	    fail("Failure in QueryContextTest.testGetSetBaseURI");
	}catch (XmlException e){
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	}

	//Test setBaseURI with a valid URI
	context.setBaseURI(baseURI);
	//Test getBaseURI after it has been set
	assertEquals(baseURI, context.getBaseURI());

	//Test that the base URI is applied in a query
	context.setDefaultCollection("");
	String query = "collection()/a_node/c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }    
	    assert(res.size() > 0);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Show that it fails without the baseURI set
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testGetSetBaseURI");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	    res = null;
	}
    }

    @Test
    public void testGetSetDefaultCollection() throws XmlException 
    {
	String badURI = "slijfla;s";
	XmlQueryContext context = mgr.createQueryContext();

	//Test getDefaultCollection when nothing is set
	assertEquals("", context.getDefaultCollection());

	//Test setDefaultCollection with a bad uri
	try{
	    context.setDefaultCollection(badURI + "dbxml:/");
	    fail("Failure in QueryContextTest.testGetSetBaseURI");
	}catch (XmlException e){
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	}
	
	//Test setDefaultCollection with a null
	context.setDefaultCollection(null);

	//Test setDefaultCollection with a valid URI
	context.setDefaultCollection(CON_NAME);
	//Test getDefaultCollectio after it has been set
	assertEquals("dbxml:/" + CON_NAME, context.getDefaultCollection());

	//Test that the default collection is applied in a query
	String query = "collection()/a_node/c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assert(res.size() > 0);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}
	
	//Test with a null value
	context.setDefaultCollection(null);
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, context);
	    else
		res = mgr.query(query, context);  
	    if(txn != null) txn.commit();
	    fail("Error in QueryContextTest.testGetSetDefaultCollection().");
	}catch(XmlException e){
		assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Test that it fails without the default collection
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testGetSetDefaultCollection");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }
    @Test
    public void testGetSetEvaluationType() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();

	//Test getEvaluationType on the default
	assertEquals(XmlQueryContext.Eager, context.getEvaluationType());

	//Test set and get after set
	context.setEvaluationType(XmlQueryContext.Lazy);
	assertEquals(XmlQueryContext.Lazy, context.getEvaluationType());

	//Test that the query is now lazy
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assertEquals(res.getEvaluationType(), XmlQueryContext.Lazy);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testGetSetReturnType() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();

	//Test getReturnType on the default
	assertEquals(XmlQueryContext.LiveValues, context.getReturnType());

	//Test set and get after set
	context.setEvaluationType(XmlQueryContext.LiveValues);
	assertEquals(XmlQueryContext.LiveValues, context.getReturnType());
    }

    @Test
    public void testGetSetNamespace() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();
	String name = "old";
	String namespaceURI = "http://dbxmltest.test/test";

	//Test get before adding any namespaces
	String uri = context.getNamespace("doesNotExist");
	assertEquals("", uri);

	//Test get after adding a namespace
	context.setNamespace(name, namespaceURI);
	uri = context.getNamespace(name);
	assertEquals(namespaceURI, uri);
	uri = context.getNamespace("doesNotExist");
	assertEquals("", uri);
	
	//test null
	try {
		context.setNamespace(null, null);
		fail("failure in QueryContextTest.testGetSetNamespace().");
	} catch (NullPointerException e) {}
	
	context.setNamespace(null, namespaceURI);
	uri = context.getNamespace("");
	assertEquals(namespaceURI, uri);
	
	//Test the default namespace
	String docString = "<?xml version=\"1.0\" "
	    + "encoding=\"UTF-8\"?><a_node xmlns=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</a_node>";
	String query = "collection('" + CON_NAME + "')/" +
			"a_node/c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assertTrue(res.size() > 0);
	    res.delete();
	    context = mgr.createQueryContext();
	    if (hp.isTransactional())
	    	res = mgr.query(txn, query, context);
	    else
	    	res = mgr.query(query, context);
	    assertTrue(res.size() == 0);
	    if (hp.isTransactional()) 
	    	cont.deleteDocument(txn, docName);
	    else
	    	cont.deleteDocument(docName);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if (txn != null) txn.commit();
	}
	
	context = mgr.createQueryContext();
	context.setNamespace(name, namespaceURI);

	//Test that the namespace is used in a query
	docString = "<?xml version=\"1.0\" "
	    + "encoding=\"UTF-8\"?><old:a_node xmlns:old=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</old:a_node>";
	query = "collection('" + CON_NAME + "')/" +
			"old:a_node/c_node";
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assertTrue(res.size() > 0);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Test that it fails without the namespace
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testGetSetNamespace");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testRemoveNamespace() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();
	String name = "old";
	String namespaceURI = "http://dbxmltest.test/test";

	//Remove a namespace from an empty namespace set
	context.removeNamespace("doesNotExist");

	//Add a namespace, and assert that it is there and queries use it
	context.setNamespace(name, namespaceURI);
	assertEquals(namespaceURI, context.getNamespace(name));

	String docString = "<?xml version=\"1.0\" "
	    + "encoding=\"UTF-8\"?><old:a_node xmlns:old=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</old:a_node>";
	String query = "collection('" + CON_NAME + "')/old:a_node/c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assertTrue(res.size() > 0);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Remove a namespace that does not exist
	context.removeNamespace("doesNotExist");
	
	//Remove a null namespace
	context.removeNamespace(null);

	//Remove the namespace, assert that it has been removed and that the query fails without it
	context.removeNamespace(name);
	assertEquals("", context.getNamespace(name));
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testRemoveNamespace");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testClearNamespace() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();
	String name = "old";
	String namespaceURI = "http://dbxmltest.test/test";

	//Clear an empty namespace set
	context.clearNamespaces();

	//Add a namespace, and assert that it is there and queries use it
	context.setNamespace(name, namespaceURI);
	assertEquals(namespaceURI, context.getNamespace(name));

	String docString = "<?xml version=\"1.0\" "
	    + "encoding=\"UTF-8\"?><old:a_node xmlns:old=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</old:a_node>";
	String query = "collection('" + CON_NAME + "')/old:a_node/c_node";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    assertTrue(res.size() > 0);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Assert that the query now fails without the namespaces
	context.clearNamespaces();
	assertEquals("", context.getNamespace(name));
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testClearNamespace");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testGetSetQueryTimeoutSeconds() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();

	assertEquals(0, context.getQueryTimeoutSeconds());
	context.setQueryTimeoutSeconds(1);
	assertEquals(1, context.getQueryTimeoutSeconds());

	//Test that the query is interrupted
	XmlTransaction txn = null;
	XmlResults res = null;
	String query = "for $i in (1 to 100000) return <a/>";
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    if(txn != null) txn.commit();
	    //Note, this test could fail on a fast computer, may have to make 100000 bigger
	    fail("Failure in QueryContextTest.testGetSetQueryTimeoutSeconds");
	}catch(XmlException e){
	    assertEquals(XmlException.OPERATION_TIMEOUT , e.getErrorCode());
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//Test that the query is not interrupted
	context.setQueryTimeoutSeconds(1000000);
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, context);
	    else
		res = mgr.query(query, context);
	    assertTrue(res.size() > 0);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testQueryInterrupt() throws XmlException, Exception {
	String query = "for $i in (1 to 1000000) return $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlTransaction txn = null;
	XmlResults res = null;

	//Call interrupt query when there is none
	qc.interruptQuery();

	try {
	    Thread intr = new Thread(new TestInterrupt(qc));
	    intr.start(); // will delay a couple seconds
	    try {
		if(hp.isTransactional()){
		    txn = mgr.createTransaction();
		    res = mgr.query(txn, query, qc);
		}else
		    res = mgr.query(query, qc);
		if(txn != null) txn.commit();
		fail("Failure in QueryContextTest.testQueryInterrupt");
	    } catch (XmlException xe) {
		assertEquals(XmlException.OPERATION_INTERRUPTED, xe.getErrorCode());
	    }
	    // clean up
	    intr.join();
	} catch (Exception e) {
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if (txn != null) txn.commit();
	}
    }

    @Test
    public void testGetSetVariableValue() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();
	String name = "mary";
	String text = "Other text";

	//Test get on an empty variable set
	XmlValue value = context.getVariableValue("doesNotExist");
	assertTrue(value.isNull());

	//Set the variable then get it
	value = new XmlValue(text);
	context.setVariableValue(name, value);
	value = context.getVariableValue(name);
	assertEquals(text, value.asString());

	//Test setting an invalid value
	byte[] bytes = {1, 0, 1};
	value = new XmlValue(XmlValue.BINARY, bytes);
	try{
	    context.setVariableValue("fail", value);
	    fail("Failure in QueryContextTest.testGetSetVariableValue");
	}catch(XmlException e){
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	}
	
	//set a null value
	value = null;
	try{
	    context.setVariableValue(null, value);
	    fail("Failure in QueryContextTest.testGetSetVariableValue");
	} catch(NullPointerException e){}
	value = new XmlValue(text);
	try {
		context.setVariableValue(null, value);
		fail("Failure in QueryContextTest.testGetSetVariableValue");
	} catch(NullPointerException e){}

	//Test that the variable works in a query
	String query = "$mary";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    XmlValue val = res.next();
	    assertEquals(text, val.asString());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}
	//Test with a node value
	XmlResults res2 = null;
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, "collection('" + CON_NAME + "')/a_node", mgr.createQueryContext());
	    else
		res = mgr.query("collection('" + CON_NAME + "')/a_node", mgr.createQueryContext());
	    XmlValue val = res.next();
	    context.setVariableValue("node", val);
	    if(hp.isTransactional())
		res2 = mgr.query(txn, "$node/c_node", context);
	    else
		res2 = mgr.query("$node/c_node", context);
	    XmlValue node = res2.next();
	    assertEquals("c_node", node.getLocalName());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	}
	
	//[#16583] Test with update query
	res2 = null;
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, "collection('" + CON_NAME + "')/a_node/c_node", mgr.createQueryContext());
	    else
		res = mgr.query("collection('" + CON_NAME + "')/a_node/c_node", mgr.createQueryContext());
	    XmlValue val = res.next();
	    context.setVariableValue("node", val);
	    XmlQueryContext qc = mgr.createQueryContext();
	    if(hp.isTransactional()) {
		res2 = mgr.query(txn, "replace node $node with <a>a</a>", context);
		res2.delete();
		res2 = mgr.query(txn, "collection('testData.dbxml')/a_node/a", qc);
	    }  else {
		res2 = mgr.query("replace node $node with <a>a</a>", context);
		res2.delete();
		res2 = mgr.query("collection('testData.dbxml')/a_node/a", qc);
	    }
	    assertTrue(res2.hasNext());
	    val = res2.next();
	    assertEquals("<a>a</a>", val.asString());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	}

	//Test that the query fails without the variable
	try{
	    if(hp.isTransactional()) {
		res = mgr.query(txn, query, mgr.createQueryContext());
	    } else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testGetSetVariableValue");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    @Test
    public void testGetSetVariableValues() throws XmlException {
	XmlQueryContext context = mgr.createQueryContext();
	String name = "mary";
	String text = "Other text";

	//Test get on an empty variable set
	XmlResults result = context.getVariableValues("doesNotExist");
	assertNull(result);

	//Set the variable then get it
	XmlValue value = new XmlValue(text);
	result = mgr.createResults();
	result.add(value);
	context.setVariableValue(name, result);
	XmlResults result2 = context.getVariableValues(name);
	value = result2.next();
	assertEquals(text, value.asString());
	result2.delete();
	
	//test null
	XmlResults results3 = null;
	try {
		context.setVariableValue(null, results3);
		fail("Failure in QueryContextTest.testGetSetVariableValues().");
	} catch (NullPointerException e){}
	results3 = mgr.createResults();
	try {
		context.setVariableValue(null, results3);
		fail("Failure in QueryContextTest.testGetSetVariableValue");
	} catch(NullPointerException e){
	} finally {
		results3.delete();
	}

	//Test that the variable works in a query
	String query = "$mary";
	XmlTransaction txn = null;
	XmlResults res = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString);
		res = mgr.query(txn, query, context);
	    }else{
		cont.putDocument(docName, docString);
		res = mgr.query(query, context);
	    }
	    XmlValue val = res.next();
	    assertEquals(text, val.asString());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	}

	//test that it works with multiple entries
	XmlValue value2 = new XmlValue("Other text 2");
	result.add(value2);
	context = mgr.createQueryContext();
	context.setVariableValue(name, result);
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, query, context);
	    else
		res = mgr.query(query, context);
	    XmlValue values = res.next();
	    assertEquals(text, values.asString());
	    values = res.next();
	    assertEquals("Other text 2", values.asString());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    result.delete();
	}

	//test with a node value
	XmlResults res2 = null;
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, "collection('" + CON_NAME + "')/a_node", mgr.createQueryContext());
	    else
		res = mgr.query("collection('" + CON_NAME + "')/a_node", mgr.createQueryContext());
	    context = mgr.createQueryContext();
	    context.setVariableValue("node", res);
	    if(hp.isTransactional())
		res2 = mgr.query(txn, "$node/c_node", context);
	    else
		res2 = mgr.query("$node/c_node", context);
	    XmlValue node = res2.next();
	    assertEquals("c_node", node.getLocalName());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	    res2 = null;
	}
	
	// [#16583] Test with update query
	res2 = null;
	try{
	    if(hp.isTransactional())
		res = mgr.query(txn, "collection('" + CON_NAME + "')/a_node/c_node", mgr.createQueryContext());
	    else
		res = mgr.query("collection('" + CON_NAME + "')/a_node/c_node", mgr.createQueryContext());
	    context.setVariableValue("node", res);
	    XmlQueryContext qc = mgr.createQueryContext();
	    if(hp.isTransactional()) {
		res2 = mgr.query(txn, "replace node $node with <a>a</a>", context);
		res2.delete();
		res2 = mgr.query(txn, "collection('testData.dbxml')/a_node/a", qc);
	    }  else {
		res2 = mgr.query("replace node $node with <a>a</a>", context);
		res2.delete();
		res2 = mgr.query("collection('testData.dbxml')/a_node/a", qc);
	    }
	    assertTrue(res2.hasNext());
	    XmlValue val = res2.next();
	    assertEquals("<a>a</a>", val.asString());
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	}

	//Test that the query fails without the variable
	try{
	    if(hp.isTransactional()) 
		res = mgr.query(txn, query, mgr.createQueryContext());
	    else
		res = mgr.query(query, mgr.createQueryContext());
	    fail("Failure in QueryContextTest.testGetSetVariableValue");
	}catch(XmlException e){
	    assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	}finally{
	    if(txn != null) txn.commit();
	    if(res != null) res.delete();
	}
    }

    // test modifications and use of variables and results ([#16583])
    void cleanupVariablesTest(XmlContainer cont) throws XmlException{
	String content1 = "<root><a attr1='a1'>atext</a><b>btext<c/></b><!--comment--></root>";
	String content2 = "<root1><a1 attr1='a11'>a1text</a1><b1>btext</b1>rootText1<!--comment--></root1>";
	String content3 = "<root2><a2 attr1='a11' attr2='a12' attr3='a13'>a2text</a2><b2>btext</b2><!--comment--></root2>";
	String myDocName1 = "doc1.xml";
	String myDocName2 = "doc2.xml";
	String myDocName3 = "doc3.xml";
	// remove content
	try { cont.deleteDocument(myDocName1); } catch (Throwable t) {}
	try { cont.deleteDocument(myDocName2); } catch (Throwable t) {}
	try { cont.deleteDocument(myDocName3); } catch (Throwable t) {}
	// add content
	cont.putDocument(myDocName1, content1);
	cont.putDocument(myDocName2, content2);
	cont.putDocument(myDocName3, content3);
    }


    void testBadVariable(XmlContainer cont, String varQuery, String updateQuery,
			 String verifyQuery, boolean shouldFail) throws XmlException {
	XmlResults res = null;
	XmlResults res2 = null;
	XmlResults res3 = null;
	XmlTransaction txn = null;
	cleanupVariablesTest(cont);
	XmlManager mgr = cont.getManager();
	try{
	    XmlQueryContext qc = mgr.createQueryContext();
	    qc.setDefaultCollection(CON_NAME);
	    if(hp.isTransactional()) {
		txn = mgr.createTransaction();
		res = mgr.query(txn, varQuery, qc);
	    } else {
		res = mgr.query(varQuery, qc);
	    }
	    assertTrue(res.hasNext());
	    qc.setVariableValue("node", res);

	    if(hp.isTransactional()) {
		// update query, then validation
		res2 = mgr.query(txn, updateQuery, qc);
		res3 = mgr.query(txn, verifyQuery, qc);
	    } else {
		// update query, then validation
		res2 = mgr.query(updateQuery, qc);
		res3 = mgr.query(verifyQuery, qc);
	    }
	    if (shouldFail)
		fail("Referencing bad variable node should throw");
	    assertTrue(res3.hasNext());
	}catch(XmlException e){
	    if (!shouldFail)
		fail("Referencing variable node should not throw");
	    else
		assertTrue(e.getErrorCode() == XmlException.INVALID_VALUE);
	} finally{
	    if(txn != null) txn.abort();
	    txn = null;
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	    res2 = null;
	    if(res3 != null) res3.delete();
	    res3 = null;
	}
    }
    @Test
    public void testBadVariables() throws XmlException {
	// variable that references deleted content
	testBadVariable(cont, "collection()/root/b/c",
			"for $i in collection()/root/b return delete node $i",
			"$node", true);
	// variable that references single deleted element
	testBadVariable(cont, "collection()/root/a",
			"for $i in collection()/root/a return delete node $i",
			"$node", true);
	// variable that references deleted comments
	testBadVariable(cont, "collection()//comment()",
			"for $i in collection()//comment() return delete node $i",
			"$node", true);
	// variable that references single deleted attribute
	testBadVariable(cont, "collection()/root/a/@attr1",
			"for $i in collection()/root/a/@attr1 return delete node $i",
			"$node", true);

	// variable that references single deleted text, leaving a valid text node
	// in its index, but of the wrong type -- this is caught and thrown
	testBadVariable(cont, "collection()/root1/text()",
			"for $i in collection()/root1/text() return delete node $i",
			"$node", true);

	// cases that really should fail but don't because they cannot be detected.
	// if the behavior of the system changes (deliberately), these cases
	// will fail and need changing

	// variable that references single deleted attribute but there's still an attribute
	// where it used to be...
	testBadVariable(cont, "collection()/root2/a2/@attr2",
			"for $i in collection()/root2/a2/@attr2 return delete node $i",
			"$node", false);

	// cases using constructed XML vs from a container
	String vquery = "<root><a><b/></a></root>/a/b";
	String uquery = "delete node $node";
	testBadVariable(cont, vquery, uquery, "$node", true);

	// cases using constructed text node vs from a container
	// should not throw -- cannot delete these nodes
	vquery = "text{'this is text'}";
	uquery = "delete node $node";
	testBadVariable(cont, vquery, uquery, "$node", false);

	// cases using constructed attribute node vs from a container
	// should not throw -- cannot delete these nodes
	vquery = "attribute{'attr'}{'attrVal'}";
	uquery = "delete node $node";
	testBadVariable(cont, vquery, uquery, "$node", false);
	
    }

    // Assign the same node acquired via 2 independent queries to 
    // 2 independent variables.  Make the node invalid, reference
    // the 2 variables via query and ensure that this fails.  This is
    // also for [#16583]
    @Test
    public void testBadVariables1() throws XmlException {
	XmlResults res = null;
	XmlResults res2 = null;
	XmlResults res3 = null;
	XmlTransaction txn = null;
	cleanupVariablesTest(cont);
	XmlManager mgr = cont.getManager();
	String varQuery = "collection()/root/b/c";
	String updateQuery = "for $i in collection()/root/b return delete node $i";
	String failQuery = "$node";
	String failQuery2 = "$node2";
	try{
	    XmlQueryContext qc = mgr.createQueryContext();
	    qc.setDefaultCollection(CON_NAME);
	    if(hp.isTransactional()) {
		txn = mgr.createTransaction();
		res = mgr.query(txn, varQuery, qc);
		res2 = mgr.query(txn, varQuery, qc);
	    } else {
		res = mgr.query(varQuery, qc);
		res2 = mgr.query(varQuery, qc);
	    }
	    assertTrue(res.hasNext());
	    assertTrue(res2.hasNext());
	    qc.setVariableValue("node", res);
	    qc.setVariableValue("node2", res2);

	    // do the validation (ensure failure)
	    if(hp.isTransactional()) {
		try {
		    // update, then validation
		    res2.delete();
		    res2 = mgr.query(txn, updateQuery, qc);
		    res3 = mgr.query(txn, failQuery, qc);
		    fail("Referencing bad variable node should throw");
		} catch (XmlException xe) {
		}
		try {
		    res3 = mgr.query(txn, failQuery2, qc);
		    fail("Referencing bad variable node should throw");
		} catch (XmlException xe) {
		}

	    } else {
		try {
		    // update, then validation
		    res2.delete();
		    res2 = mgr.query(updateQuery, qc);
		    res3 = mgr.query(failQuery, qc);
		    fail("Referencing bad variable node should throw");
		} catch (XmlException xe) {
		}
		try {
		    res3 = mgr.query(failQuery2, qc);
		    fail("Referencing bad variable node should throw");
		} catch (XmlException xe) {
		}
	    }
	} catch(XmlException e){
	    fail("Unexpected exception from test");
	} finally{
	    if(txn != null) txn.abort();
	    txn = null;
	    if(res != null) res.delete();
	    res = null;
	    if(res2 != null) res2.delete();
	    res2 = null;
	    if(res3 != null) res3.delete();
	    res3 = null;
	}
    }

    class TestInterrupt implements Runnable {
	private XmlQueryContext qc_;

	public TestInterrupt(XmlQueryContext qc) {
	    qc_ = qc;
	}

	public void run() {
	    try {
		Thread.sleep(2000); // 2 seconds
		qc_.interruptQuery();
	    } catch (Exception e) {
		e.printStackTrace();    
	    }
	}
    }
}

