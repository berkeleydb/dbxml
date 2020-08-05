/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007, 2009 Oracle.  All rights reserved.
 *
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

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlDocumentConfig;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlIndexDeclaration;
import com.sleepycat.dbxml.XmlIndexSpecification;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlStatistics;
import com.sleepycat.dbxml.XmlValue;

public class ContainerTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";

    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</a_node>";

    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlContainer!");
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
	System.out.println("Finished test XmlContainer!");
    }

    /*
     * Test for addAlias()
     * 
     */
    @Test
	public void testAddAlias() throws Throwable {
	// create a stand-alone XmlDocument
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);

	XmlUpdateContext uc = mgr.createUpdateContext();
	String myQuery = "collection('myContainer')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
	    } else
		cont.putDocument(doc, uc);
	    assertTrue(cont.addAlias("myContainer"));
	    if (hp.isTransactional())
		results = mgr.query(txn, myQuery, context);
	    else
		results = mgr.query(myQuery, context);
	    assertEquals(results.size(), 3);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (results != null) results.delete();
	}
	
	// Test for not valid "/" "\\" exception alias
	try {
	    cont.addAlias("my/Container");
	    fail("Fail to throw exception when add alias use not vaild name");
	} catch (XmlException e) {
	    assertNotNull(e);
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} 

	try {
	    cont.addAlias("my\\Container");
	    fail("Fail to throw exception when add alias use not vaild name");
	} catch (XmlException e) {
	    assertNotNull(e);
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} 
	
	// Test for add same name twice
	try {
	    assertFalse(cont.addAlias("myContainer"));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for addDefaultIndex(String index, XmlUpdateContext context)
     * 
     */
    @Test
	public void testAddDefaultIndex() throws Throwable {
	XmlTransaction txn = null;
	String index = "element-presence-none";
	XmlIndexSpecification is = null;
	XmlUpdateContext uc = mgr.createUpdateContext();

	// not valid index
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.addDefaultIndex(txn, index, uc);
		txn.commit();
	    } else
		cont.addDefaultIndex(index, uc);
	    fail("fail in testAddDefaultIndex() when valid index.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}

	index = "none";
	//
	try {
	    if (hp.isTransactional()) {
		cont.addDefaultIndex(txn, index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.addDefaultIndex(index, uc);
		is = cont.getIndexSpecification();
	    }
	    assertEquals(is.getDefaultIndex(), "none");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}

	index = "node-metadata-equality-string";
	try {
	    if (hp.isTransactional()) {
		cont.addDefaultIndex(txn, index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.addDefaultIndex(index, uc);
		is = cont.getIndexSpecification();
	    }
	    assertEquals(is.getDefaultIndex(),
			 "none node-metadata-equality-string");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * test for addIndex(String uri, String name, String index, XmlUpdateContext
     * context)
     * 
     */
    @Test
	public void testAddIndex() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = null;
	XmlIndexSpecification is = null;
	// null index
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.addIndex(txn, "", "c_node", index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
	    cont.addIndex("", "c_node", index, uc);
	    is = cont.getIndexSpecification();
	    }
	    XmlIndexDeclaration id = is.find("", "c_node");
	    assertNull(id);
	} catch (XmlException e) {
		if (hp.isTransactional()) txn.abort();
	    throw e;
	} finally {
		if (is != null) is.delete();
	}
	index = "element-presence-none";
	// not valid index
	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "c_node", index, uc);
		txn.commit();
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "c_node", index, uc);
	    }
	    fail("fail in testAddIndex()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	index = "none";

	try {
	    if (hp.isTransactional())
		cont.addIndex(txn, "", "c_node", index, uc);
	    else
		cont.addIndex("", "c_node", index, uc);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	index = "node-metadata-equality-string";
	try {
	    if (hp.isTransactional()) {
		cont.addIndex(txn, "", "c_node", index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.addIndex("", "c_node", index, uc);
		is = cont.getIndexSpecification();
	    }
	    XmlIndexDeclaration id = is.find("", "c_node");
	    assertEquals(id.name, "c_node");
	} catch (XmlException e) {
	    throw e;
	}
	
	//test XmlUpdateContext as null
	try {
	    if (hp.isTransactional())
		cont.addIndex(txn, "", "c_node", index, null);
	    else
		cont.addIndex("", "c_node", index, null);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteDefaultIndex(String index, XmlUpdateContext context)
     * 
     */
    @Test
	public void testDeleteDefaultIndex() throws Throwable {
	XmlTransaction txn = null;
	String index = "node-metadata-equality-string";
	XmlIndexSpecification is = null;
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.addDefaultIndex(txn, index, uc);
	    } else
		cont.addDefaultIndex(index, uc);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// delete default index
	try {
	    if (hp.isTransactional()) {
		cont.deleteDefaultIndex(txn, index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.deleteDefaultIndex(index, uc);
		is = cont.getIndexSpecification();
	    }
	    assertEquals(is.getDefaultIndex(), "");

	    // delete not exist index
	    if (hp.isTransactional())
		cont.deleteDefaultIndex(txn, index, uc);
	    else
		cont.deleteDefaultIndex(index, uc);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
	
	//Test with null XmlUpdateContext
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.addDefaultIndex(txn, index, null);
	    } else
		cont.addDefaultIndex(index, null);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// delete default index
	try {
	    if (hp.isTransactional()) {
		cont.deleteDefaultIndex(txn, index, null);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.deleteDefaultIndex(index, null);
		is = cont.getIndexSpecification();
	    }
	    assertEquals(is.getDefaultIndex(), "");

	    // delete not exist index
	    if (hp.isTransactional())
		cont.deleteDefaultIndex(txn, index, uc);
	    else
		cont.deleteDefaultIndex(index, uc);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteDocument(String name, XmlUpdateContext context)
     * 
     */
    @Test
	public void testDeleteDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.deleteDocument(txn, docName, uc);
		cont.getDocument(txn, docName);
		txn.commit();
	    } else {
		cont.putDocument(doc, uc);
		cont.deleteDocument(docName, uc);
		cont.getDocument(docName);
	    }
	    fail("Fail to throw exception when get deleted doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}
	
	//Try with XmlUpdateContext as null
	String name = "test2.xml";
	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, name, docString);
		cont.deleteDocument(txn, name, null);
		cont.getDocument(txn, docName);
		txn.commit();
	    } else {
		cont.putDocument(name, docString);
		cont.deleteDocument(name, null);
		cont.getDocument(name);
	    }
	    fail("Fail to throw exception when get deleted doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}

	// delete not exist doc
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, docName, uc);
	    else
		cont.deleteDocument(docName, uc);
	    fail("Fail to throw exception when delete not exist doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteDocument(XmlDocument document,, XmlUpdateContext context)
     * 
     */
    @Test
	public void testDelteDocument_Doc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.deleteDocument(txn, doc, uc);
		cont.getDocument(txn, docName);
		txn.commit();
	    } else {
		cont.putDocument(doc, uc);
		cont.deleteDocument(doc, uc);
		cont.getDocument(docName);
	    }
	    fail("Fail to throw exception when get deleted doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}

	doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlDocument docDel = mgr.createDocument();
	docDel.setName(docName);
	docDel.setContent(docString);
	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, doc, uc);
		cont.deleteDocument(txn, docDel, uc);
		cont.getDocument(txn, docName);
		txn.commit();
	    } else {
		cont.putDocument(doc, uc);
		cont.deleteDocument(docDel, uc);
		cont.getDocument(docName);

	    }
	    fail("Fail to throw exception when get deleted doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}

	// delete not exist doc
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, docDel, uc);
	    else
		cont.deleteDocument(docDel, uc);
	    fail("Fail to throw exception when delete not exist doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteIndex(String uri, String name, String index,
     * XmlUpdateContext context)
     * 
     */
    @Test
	public void testDeleteIndex() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	String index = "node-metadata-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "c_node", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "c_node", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// delete the index
	try {
	    if (hp.isTransactional())
		cont.deleteIndex(txn, "", "c_node", index, uc);
	    else
		cont.deleteIndex("", "c_node", index, uc);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// delete not exist index
	try {
	    if (hp.isTransactional())
		cont.deleteIndex(txn, "", "node", index, uc);
	    else
		cont.deleteIndex("", "node", index, uc);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }
    /*
     * Test for getAllDocuments(XmlDocumentConfig config)
     * 
     */
    @Test
	public void testGetAllDocuments() throws Throwable {
	XmlTransaction txn = null;
	XmlDocumentConfig config = new XmlDocumentConfig();
	XmlResults re = null;

	// no doc in the container
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		re = cont.getAllDocuments(txn, config);
	    } else
		re = cont.getAllDocuments(config);
	    assertFalse(re.hasNext());
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	    re = null;
	}

	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	// test not empty container
	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, doc, uc);
		re = cont.getAllDocuments(txn, config);
	    } else {
		cont.putDocument(doc, uc);
		re = cont.getAllDocuments(config);
	    }
	    assertTrue(re.hasNext());
	    re.next();
	    assertFalse(re.hasNext());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (re != null) re.delete();
	    re = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getDocument(String name)
     * 
     */
    @Test
	public void testGetDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    XmlDocument myDoc = null;
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(doc, uc);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// get not exist doc
	try {
	    if (hp.isTransactional())
		cont.getDocument(txn, "my");
	    else
		cont.getDocument("my");
	    fail("Fail in testGetDocument() when get not exist doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getDocument(String name, XmlDocumentConfig config)
     * 
     */
    @Test
	public void testGetDocument_Dc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocumentConfig dc = new XmlDocumentConfig();
	try {
	    XmlDocument myDoc = null;
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		myDoc = cont.getDocument(txn, docName, dc);
	    } else {
		cont.putDocument(doc, uc);
		myDoc = cont.getDocument(docName, dc);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// get not exist doc
	try {
	    if (hp.isTransactional())
		cont.getDocument(txn, "my", dc);
	    else
		cont.getDocument("my", dc);
	    fail("Fail in testGetDocument_Dc()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getIndexNode()
     * 
     */
    @Test
	public void testGetIndexNode() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
	    } else
		cont.putDocument(doc, uc);
	    if (hp.isNodeContainer())
		assertTrue(cont.getIndexNodes());
	    else
		assertFalse(cont.getIndexNodes());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * get test for getIndexSpecification()
     * 
     */
    @Test
	public void testGetIndexSpecification() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = "node-metadata-equality-string";
	XmlIndexSpecification is = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "c_node", index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "c_node", index, uc);
		is = cont.getIndexSpecification();
	    }
	    XmlIndexDeclaration id = is.find("", "c_node");
	    assertEquals(id.name, "c_node");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * get test for getIndexSpecification_conf()
     * 
     */
    @Test
	public void testGetIndexSpecification_Conf() throws Throwable {
	if (!hp.isTransactional()) return;
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = "node-metadata-equality-string";
	XmlIndexSpecification is = null;
	XmlDocumentConfig conf = new XmlDocumentConfig();

	try {
	    txn = mgr.createTransaction();
	    cont.putDocument(txn, doc, uc);
	    cont.addIndex(txn, "", "c_node", index, uc);
	    is = cont.getIndexSpecification(txn, conf);
	    XmlIndexDeclaration id = is.find("", "c_node");
	    assertEquals(id.name, "c_node");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getName()
     * 
     */
    @Test
	public void testGetName() throws Throwable {
	try {
	    assertEquals(cont.getName(), CON_NAME);
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for getNode()
     * 
     */
    @Test
	public void testGetNode() throws Throwable {
	XmlTransaction txn = null;
	String myQuery = "collection('testData.dbxml')/a_node";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults re = null;
	XmlUpdateContext context = mgr.createUpdateContext();
	String nodeHandle = null;
	XmlResults res = null;
	XmlResults attrs = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, context);
		re = mgr.query(txn, myQuery, qc);
	    } else {
		cont.putDocument(docName, docString, context);
		re = mgr.query(myQuery, qc);
	    }
	    XmlValue vl = re.next();
	    nodeHandle = vl.getNodeHandle();

	    if (hp.isTransactional())
		res = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
	    else
		res = cont.getNode(nodeHandle);
	    XmlValue value = res.next();
	    assertTrue(value.equals(vl));
	    XmlValue child = value.getFirstChild();
	    assertEquals(child.getNodeName(), "b_node");
	    XmlValue sibling = child.getNextSibling();
	    assertEquals(sibling.getNodeName(), "c_node");
	    assertEquals(child.getParentNode().getNodeName(), "a_node");
	    attrs = value.getAttributes();
	    XmlValue attr = attrs.next();
	    assertEquals(attr.getNodeName(), "xmlns:old");
	    assertEquals(attr.getOwnerElement().getNodeName(), "a_node");
	    attrs.delete();
	    attrs = null;
	    res.delete();
	    res = null;
	    try{
		value.getFirstChild();
		fail("Fail in testGetNodeName()");
	    } catch (XmlException e) {
		assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (res != null) res.delete();
	    res = null;
	    if (attrs != null) attrs.delete();
	    attrs = null;
	    if (re != null) re.delete();
	    re = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Tests the XQilla function dbxml:handle-to-node
     */
    @Test
	public void testHandleToNode() throws Throwable {
	XmlTransaction txn = null;
	String myQuery = "collection('testData.dbxml')/a_node";
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults re = null;
	String nodeHandle = null;
	XmlResults res = null;
	XmlResults attrs = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, context);
		re = mgr.query(txn, myQuery, qc);
	    } else {
		cont.putDocument(docName, docString, context);
		re = mgr.query(myQuery, qc);
	    }
	    XmlValue vl = re.next();
	    nodeHandle = vl.getNodeHandle();
	    String handleToNodeQuery = "dbxml:handle-to-node(\""+CON_NAME+"\",\""+nodeHandle+"\")";
	    if (hp.isTransactional())
		res = mgr.query(txn, handleToNodeQuery, qc);
	    else
		res = mgr.query(handleToNodeQuery, qc);
	    XmlValue value = res.next();
	    assertTrue(value.equals(vl));
	    XmlValue child = value.getFirstChild();
	    assertEquals(child.getNodeName(), "b_node");
	    XmlValue sibling = child.getNextSibling();
	    assertEquals(sibling.getNodeName(), "c_node");
	    assertEquals(child.getParentNode().getNodeName(), "a_node");
	    attrs = value.getAttributes();
	    XmlValue attr = attrs.next();
	    assertEquals(attr.getNodeName(), "xmlns:old");
	    assertEquals(attr.getOwnerElement().getNodeName(), "a_node");
	    attrs.delete();
	    attrs = null;
	    res.delete();
	    res = null;
	    try{
		value.getFirstChild();
		fail("Fail in testGetNodeName()");
	    } catch (XmlException e) {
		assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (res != null) res.delete();
	    res = null;
	    if (attrs != null) attrs.delete();
	    attrs = null;
	    if (re != null) re.delete();
	    re = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }
    /*
     * Test for getNumDocuments()
     * 
     */
    @Test
	public void testGetNumDocuments() throws Throwable {
	XmlTransaction txn  = null;
	// no doc in container
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		assertEquals(cont.getNumDocuments(txn), 0);
	    } else
		assertEquals(cont.getNumDocuments(), 0);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, doc, uc);
		assertEquals(cont.getNumDocuments(txn), 1);
	    } else {
		cont.putDocument(doc, uc);
		assertEquals(cont.getNumDocuments(), 1);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	XmlDocument doc2 = mgr.createDocument();
	uc = mgr.createUpdateContext();
	doc2.setName("doc2.xml");
	doc2.setContent(docString);

	try {
	    if (hp.isTransactional()) {
		cont.putDocument(txn, doc2, uc);
		assertEquals(cont.getNumDocuments(txn), 2);
	    } else {
		cont.putDocument(doc2, uc);
		assertEquals(cont.getNumDocuments(), 2);
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for lookupStatistics(String uri, String name, String index)
     * 
     */
    @Test
	public void testLookupStatistics() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = "node-element-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "name", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "name", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	XmlStatistics sta = null;
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", index);
	    else
		sta = cont.lookupStatistics("", "name", index);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 3);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	}

	// not exist index
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "noexist", index);
	    else
		sta = cont.lookupStatistics("", "noexist", index);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for lookupStatistics(String uri, String name, String parentUri,
     * String parentName, String index)
     */
    @Test
	public void testLookupStatistics_Idx() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name><my><name>Kate</name></my></names>";
	doc.setContent(docString);
	doc.setName(docName);
	String index = "edge-element-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "name", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "name", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}
	XmlStatistics sta = null;
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", "", "my", index);
	    else
		sta = cont.lookupStatistics("", "name", "", "my", index);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 1);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	}

	// not exist index
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "noexist", "", "my", index);
	    else
		sta = cont.lookupStatistics("", "noexist", "", "my", index);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for lookupStatistics(String uri, String name, String parentUri,
     * String parentName, String index, XmlValue value))
     */
    @Test
	public void testLookupStatistics_IdxVal() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name><my><name>Kate</name></my></names>";
	doc.setContent(docString);
	doc.setName(docName);
	String index = "edge-element-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "name", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "name", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	XmlStatistics sta = null;
	XmlValue val = new XmlValue();
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", "", "my", index, val);
	    else
		sta = cont.lookupStatistics("", "name", "", "my", index, val);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(), 1);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	}

	// not exist index
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "noexist", "", "my", index, val);
	    else
		sta = cont.lookupStatistics("", "noexist", "", "my", index, val);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(), 0);
	} catch (XmlException e) {
        if (hp.isTransactional() && txn != null) txn.commit();
        throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;	    
	}
	
	// test exception: INVALID_VALUE, "A value has been specified for an index that does not require one."
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults re = mgr.query("<node />", context);
	val = re.next();
	
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", "", "my", "none", val);
	    else
		sta = cont.lookupStatistics("", "name", "", "my", "none", val);
	    fail("Fail to throw exception in testLookupStatistics_IdxVal().");	    
	} catch (XmlException e) {
	    assertNotNull(e);
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} finally {
		re.delete();
	    if (sta != null) sta.delete();
	    sta = null;
	}
	
	/* test parent is "", to cover:
	 * key.setNodeLookup(true); 
	 * in Container::getKeyStatistics(Transaction *txn, const Index &index,
	 *			const char *child, const char *parent,const XmlValue &value)
	 */
	val = new XmlValue();
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", "", "", index, val);
	    else
		sta = cont.lookupStatistics("", "name", "", "", index, val);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(), 4);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
	
    }

    /*
     * Test for lookupStatistics(String uri, String name, String index, XmlValue
     * value)
     * 
     */
    @Test
	public void testLookupStatistics_Val() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = "node-element-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "name", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "name", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	XmlStatistics sta = null;
	XmlValue val = new XmlValue();
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "name", index, val);
	    else
		sta = cont.lookupStatistics("", "name", index, val);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 3);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	}

	// not exist index
	try {
	    if (hp.isTransactional())
		sta = cont.lookupStatistics(txn, "", "noexist", index, val);
	    else
		sta = cont.lookupStatistics("", "noexist", index, val);
	    assertEquals((new Double(sta.getNumberOfIndexedKeys())).intValue(),
			 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (sta != null) sta.delete();
	    sta = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocument(String name, String contents, XmlUpdateContext
     * context)
     * 
     */
    @Test
	public void testPutDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, uc);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(docName, docString, uc);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test same name exception
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, docName, docString, uc);
	    else
		cont.putDocument(docName, docString, uc);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocument(String name, String content, XmlUpdateContext
     * context, XmlDocumentConfig config)
     */
    @Test
	public void testPutDocument_Conf() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocumentConfig conf = new XmlDocumentConfig();
	XmlDocument myDoc = null;
	
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, docString, uc, conf);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(docName, docString, uc, conf);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test same name exception
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, docName, docString, uc, conf);
	    else
		cont.putDocument(docName, docString, uc, conf);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} 
	
	// test document well formed only true
	try {
		conf.setWellFormedOnly(true);
	    if (hp.isTransactional()){
		cont.putDocument(txn, "1.xml", docString, uc, conf);
	    myDoc = cont.getDocument(txn, "1.xml");
	    }else{
		cont.putDocument("1.xml", docString, uc, conf);
		myDoc = cont.getDocument("1.xml");
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
	
    }

    /*
     * Test for putDocument(String name, XmlEventReader reader, XmlUpdateContext
     * context, XmlDocumentConfig config)
     * 
     */
    @Test
	public void testPutDocument_ReaderConf() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocumentConfig conf = new XmlDocumentConfig();
	XmlDocument inDoc = mgr.createDocument();
	inDoc.setContent(docString);
	XmlEventReader reader = inDoc.getContentAsEventReader();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, reader, uc, conf);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(docName, reader, uc, conf);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    reader.close();
	}

	// test same name exception
	XmlEventReader reader2 = inDoc.getContentAsEventReader();
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, docName, reader2, uc, conf);
	    else
		cont.putDocument(docName, reader2, uc, conf);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    reader2.close();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocument(String name, XmlInputStream input, XmlUpdateContext
     * context)
     * 
     */
    @Test
	public void testPutDocument_Is() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocument inDoc = mgr.createDocument();
	inDoc.setContent(docString);
	XmlInputStream is = inDoc.getContentAsXmlInputStream();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, is, uc);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(docName, is, uc);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    is.delete();
	}

	// test same name exception
	XmlInputStream is2 = inDoc.getContentAsXmlInputStream();
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, docName, is2, uc);
	    else
		cont.putDocument(docName, is2, uc);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    is2.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocument(String name, XmlInputStream input, XmlUpdateContext
     * context, XmlDocumentConfig config)
     * 
     */
    @Test
	public void testPutDocument_IsConf() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocument inDoc = mgr.createDocument();
	inDoc.setContent(docString);
	XmlInputStream is = inDoc.getContentAsXmlInputStream();
	XmlDocumentConfig config = new XmlDocumentConfig();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, is, uc, config);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(docName, is, uc, config);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    is.delete();
	}

	// test same name exception
	XmlInputStream is2 = inDoc.getContentAsXmlInputStream();
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, docName, is2, uc, config);
	    else
		cont.putDocument(docName, is2, uc, config);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    is2.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * test for putDocument(XmlDocument document, XmlUpdateContext context)
     * 
     */
    @Test
	public void testPutDocument_Doc() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(doc, uc);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test same name exception
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, doc, uc);
	    else
		cont.putDocument(doc, uc);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
	
	/* Test that a document can be put into two different containers */
	XmlContainer cont2 = hp.createContainer("test2.dbxml", mgr);
	String docName2 = "test2.xml";
	try {
		doc = mgr.createDocument();
		doc.setName(docName2);
		doc.setContent(docString);
	    if (hp.isTransactional()){
			txn = mgr.createTransaction();
			cont.putDocument(txn, doc, uc);
			cont2.putDocument(txn, doc, uc);
			assertEquals(docString, doc.getContentAsString());
			doc = cont2.getDocument(txn, docName2);
	    } else {
	    	cont.putDocument(doc, uc);
	    	cont2.putDocument(doc, uc);
	    	assertEquals(docString, doc.getContentAsString());
			doc = cont2.getDocument(docName2);
	    }
	    assertEquals(docString, doc.getContentAsString());
	} catch (XmlException e) {
	    throw e;
	} finally {
		if (hp.isTransactional() && txn != null) txn.commit();
		cont2.delete();
		mgr.removeContainer("test2.dbxml");
	}
	
    }

    /*
     * test for putDocument(XmlDocument document, XmlUpdateContext context,
     * XmlDocumentConfig config)
     * 
     */
    @Test
	public void testPutDocument_DocConf() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocumentConfig config = new XmlDocumentConfig();
	XmlDocument myDoc = null;

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc, config);
		myDoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(doc, uc, config);
		myDoc = cont.getDocument(docName);
	    }
	    assertEquals(myDoc.getContentAsString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test same name exception
	try {
	    if (hp.isTransactional())
		cont.putDocument(txn, doc, uc, config);
	    else
		cont.putDocument(doc, uc, config);
	    fail("Fail in throw exception when add same name doc");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNIQUE_ERROR);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for putDocumentAsEventWriter(XmlDocument document, XmlUpdateContext
     * context, XmlDocumentConfig config)
     */
    @Test
	public void testPutDocumentAsEventWriter() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	doc.setName(docName);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	try {
	    if (hp.isTransactional() || hp.isCDS()) {
		if (hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    } else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeStartElement("a", null, null, 0, true);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	    writer.close();
	    writer = null;

	    if (hp.isTransactional() || hp.isCDS())
		assertEquals(cont.getDocument(txn, docName).getContentAsString(), "<root><a/></root>");
	    else
		assertEquals(cont.getDocument(docName).getContentAsString(), "<root><a/></root>");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (writer != null) writer.close();
	    if (txn != null) txn.commit();
	}
	
	// test when putDocumentAsEventWriter in transaction env 
	// use null transaction will throw exception
	try {
		if (hp.isTransactional()) {
		doc = mgr.createDocument();
		doc.setName("1.xml");
		writer = cont.putDocumentAsEventWriter(null, doc, uc, config);
		writer.close();
		fail("Fail to throw exception when pass null transaction to putDocumentAsEventWriter(...) int transactional env");
		}
	} catch (XmlException e){
		assertNotNull(e);
		assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}
	
    }
    /*
     * Test for removeAlias(String alias)
     * 
     */
    @Test
	public void testRemoveAlias() throws Throwable {
	// create a stand-alone XmlDocument
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	String myQuery = "collection('myContainer')/a_node/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	// create a alias
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
	    } else
		cont.putDocument(doc, uc);
	    assertTrue(cont.addAlias("myContainer"));
	    if (hp.isTransactional())
		results = mgr.query(txn, myQuery, context);
	    else
		results = mgr.query(myQuery, context);
	    assertEquals(results.size(), 3);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (results != null) results.delete();
	    results = null;
	}

	// remove the alias
	try {
	    cont.removeAlias("myContainer");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//confirm that the removed alias can not be used again
	try {
	    if (hp.isTransactional())
		mgr.query(txn, myQuery, context);
	    else
		mgr.query(myQuery, context);
	    fail("Fail to throw exception when use removed alias in testRemoveAlias()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for replaceDefaultIndex(String index, XmlUpdateContext context)
     * 
     * 
     */
    @Test
	public void testReplaceDefaultIndex() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	XmlUpdateContext uc = mgr.createUpdateContext();
	String index = "node-metadata-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.addDefaultIndex(txn, index, uc);
	    } else
		cont.addDefaultIndex(index, uc);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// replace default index
	try {
	    if (hp.isTransactional()) {
		cont.replaceDefaultIndex(txn, "none", uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.replaceDefaultIndex("none", uc);
		is = cont.getIndexSpecification();
	    }
	    assertEquals(is.getDefaultIndex(), "none");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * replaceIndex(String uri,String name,String index,XmlUpdateContext
     * context)
     * 
     * 
     */
    @Test
	public void testReplaceIndex() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlIndexSpecification is = null;
	String index = "node-metadata-equality-string";
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		cont.addIndex(txn, "", "c_node", index, uc);
	    } else {
		cont.putDocument(doc, uc);
		cont.addIndex("", "c_node", index, uc);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// replace the index
	index = "node-element-equality-string";
	try {
	    if (hp.isTransactional()) {
		cont.replaceIndex(txn, "", "c_node", index, uc);
		is = cont.getIndexSpecification(txn);
	    } else {
		cont.replaceIndex("", "c_node", index, uc);
		is = cont.getIndexSpecification();
	    }
	    XmlIndexDeclaration id = is.find("", "c_node");
	    assertEquals(id.index, index);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setIndexSpecification(XmlIndexSpecification index,
     * XmlUpdateContext context)
     * 
     */
    @Test
	public void testSetIndexSpecification() throws Throwable {
	// create a stand-alone XmlDocument
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlIndexSpecification is = null;
	XmlIndexSpecification is2 = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		is = cont.getIndexSpecification(txn);
		is.addIndex("", "node", "node-element-equality-string");
		cont.setIndexSpecification(txn, is, uc);
		is2 = cont.getIndexSpecification(txn);
	    } else {
		cont.putDocument(doc, uc);
		is = cont.getIndexSpecification();
		is.addIndex("", "node", "node-element-equality-string");
		cont.setIndexSpecification(is, uc);
		is2 = cont.getIndexSpecification();
	    }
	    XmlIndexDeclaration id = is2.find("", "node");
	    assertEquals(id.index, "node-element-equality-string");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    if (is2 != null) is2.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for updateDocument(XmlDocument document, XmlUpdateContext context)
     * 
     */
    @Test
	public void testUpdateDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlDocument docUp = mgr.createDocument();
	docUp.setName(docName);
	docUp.setContent("<root><a/></root>");

	// updateDocument
	//Change content
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		doc.setContent("<root/>");
		cont.updateDocument(txn, doc, uc);
		assertEquals(cont.getDocument(txn, docName).getContentAsString(), "<root/>");
	    } else {
		cont.putDocument(doc, uc);
		doc.setContent("<root/>");
		cont.updateDocument(doc, uc);
		assertEquals(cont.getDocument(docName).getContentAsString(), "<root/>");
	    }
	} catch (XmlException e) {
		if (txn != null) txn.commit();
	    throw e;
	}
	//Change content using another document [#16491]
	try {
	    if (hp.isTransactional()) {
		cont.updateDocument(txn, docUp, uc);
		assertEquals(cont.getDocument(txn, docName).getContentAsString(), "<root><a/></root>");
	    } else {
		cont.updateDocument(docUp, uc);
		assertEquals(cont.getDocument(docName).getContentAsString(), "<root><a/></root>");
	    }
	} catch (XmlException e) {
		if (txn != null) txn.commit();
	    throw e;
	}
	//Change document name
	try {
		doc.setName(docName+"2");
	    if (hp.isTransactional()) {
		cont.updateDocument(txn, doc, uc);
		assertEquals(cont.getDocument(txn, docName+"2").getContentAsString(), "<root><a/></root>");
	    } else {
		cont.updateDocument(doc, uc);
		assertEquals(cont.getDocument(docName+"2").getContentAsString(), "<root><a/></root>");
	    }
	} catch (XmlException e) {
		if (txn != null) txn.commit();
	    throw e;
	} 
	//Test that the document cannot be found using the old name
	try {
		if (hp.isTransactional()) {
			cont.getDocument(txn, docName);
		} else {
			cont.getDocument(docName);
		}
		fail("The document should no longer exist in the container under its original name.");
	} catch (XmlException e) {
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	} 
	
	// Test if the document is eventReader can not be updated
	XmlEventReader reader = doc.getContentAsEventReader();
	XmlDocument doc2 = mgr.createDocument();
	doc2.setContentAsEventReader(reader);
	try {
		
	    if (hp.isTransactional()) {
		cont.updateDocument(txn, doc2, uc);
	    } else {
		cont.updateDocument(doc2, uc);
	    }
		fail("Fail to throw exception when update an XmlEventReader content document.");
	} catch (XmlException e) {
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	} finally {
		reader.close();
		if (hp.isTransactional() && txn != null) txn.commit();
	}
	
    }
    
    //Tests that the container config object reflects the real state of the container
    @Test
    public void testGetContainerConfig() throws XmlException
    {
    XmlContainerConfig config = new XmlContainerConfig();
    cont.delete();
    if(hp.isTransactional()) config.setTransactional(true);
    cont = mgr.openContainer(CON_NAME, config);
    config = cont.getContainerConfig();
    if(hp.isNodeContainer()) {
    	assertEquals(XmlContainer.NodeContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.On, config.getIndexNodesFlag());
    } else {
    	assertEquals(XmlContainer.WholedocContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.Off, config.getIndexNodesFlag());
    }
    assertEquals(XmlContainerConfig.On, config.getStatisticsEnabledFlag());
    assertEquals(hp.isTransactional(), config.getTransactional());
    
    cont.delete();
    config = new XmlContainerConfig();
    config.setIndexNodes(XmlContainerConfig.On);
    config.setStatisticsEnabled(XmlContainerConfig.On);
    config.setContainerType(XmlContainer.NodeContainer);
    if(hp.isTransactional()) config.setTransactional(true);
    cont = mgr.openContainer(CON_NAME, config);
    config = cont.getContainerConfig();
    if(hp.isNodeContainer()) {
    	assertEquals(XmlContainer.NodeContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.On, config.getIndexNodesFlag());
    } else {
    	assertEquals(XmlContainer.WholedocContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.Off, config.getIndexNodesFlag());
    }
    assertEquals(XmlContainerConfig.On, config.getStatisticsEnabledFlag());
    assertEquals(hp.isTransactional(), config.getTransactional());
    
    cont.delete();
    config = new XmlContainerConfig();
    config.setIndexNodes(XmlContainerConfig.Off);
    config.setStatisticsEnabled(XmlContainerConfig.Off);
    config.setContainerType(XmlContainer.WholedocContainer);
    if(hp.isTransactional()) config.setTransactional(true);
    cont = mgr.openContainer(CON_NAME, config);
    config = cont.getContainerConfig();
    if(hp.isNodeContainer()) {
    	assertEquals(XmlContainer.NodeContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.On, config.getIndexNodesFlag());
    } else {
    	assertEquals(XmlContainer.WholedocContainer, config.getContainerType());
    	assertEquals(XmlContainerConfig.Off, config.getIndexNodesFlag());
    }
    assertEquals(XmlContainerConfig.On, config.getStatisticsEnabledFlag());
    assertEquals(hp.isTransactional(), config.getTransactional());   
    
    
    XmlContainer cont2 = null;
    String contName = "test2.dbxml";
    try {
    	config = new XmlContainerConfig();
    	if(hp.isTransactional()) config.setTransactional(true);
    	if(!hp.isNodeContainer()) config.setContainerType(XmlContainer.WholedocContainer);
    	config.setChecksum(true);
    	config.setAllowValidation(true);
    	config.setStatisticsEnabled(XmlContainerConfig.Off);
    	cont2 = mgr.createContainer(contName, config);
    	config = cont2.getContainerConfig();
    	assertTrue(config.getChecksum());
    	assertTrue(config.getAllowValidation());
    	assertFalse(config.getEncrypted());
    	assertFalse(config.getReadOnly());
    	assertEquals(XmlContainerConfig.Off, config.getStatisticsEnabledFlag());
    	cont2.delete();
    } catch (XmlException e) {
    	if (cont2 != null) { 
    		cont2.delete();
    		mgr.removeContainer(contName);
    	}
    	throw e;
    }
    
    cont2 = null;
    try {
    	config = new XmlContainerConfig();
    	if(hp.isTransactional()) config.setTransactional(true);
    	if(!hp.isNodeContainer()) config.setContainerType(XmlContainer.WholedocContainer);
    	config.setStatisticsEnabled(XmlContainerConfig.On);
    	config.setChecksum(true);
    	config.setAllowValidation(true);
    	cont2 = mgr.openContainer(contName, config);
    	config = cont2.getContainerConfig();
    	assertTrue(config.getChecksum());
    	assertTrue(config.getAllowValidation());
    	assertFalse(config.getEncrypted());
    	assertFalse(config.getReadOnly());
    	assertEquals(XmlContainerConfig.Off, config.getStatisticsEnabledFlag());
    	cont2.delete();
    } catch (XmlException e) {
    	throw e;
    } finally {
    	if (cont2 != null) { 
    		cont2.delete();
    		mgr.removeContainer(contName);
    	}
    }
    
    }

    //Test various XmlContainerConfig settings
    @Test
    public void testContainerConfig() throws XmlException
    {
	XmlContainer cont2 = null;
	String configContainer = "config.dbxml";
	int pageSize = 1024; // probably not a default
	try {
	    // test default page size, then ignore the default container
	    assertFalse(cont.getContainerConfig().getPageSize() == pageSize);
	    assertFalse(cont.getContainerConfig().getPageSize() == 0);
	    cont.delete();
	    XmlContainerConfig config = new XmlContainerConfig();
	    assertEquals(config.getPageSize(), 0);
	    config.setPageSize(pageSize);
	    cont2 = mgr.createContainer(configContainer, config);
	    XmlContainerConfig newConfig = cont2.getContainerConfig();
	    assertEquals(pageSize, newConfig.getPageSize());
	    cont2.delete();
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (cont2 != null) { 
    		cont2.delete();
    		mgr.removeContainer(configContainer);
	    }
	}
    }
}
