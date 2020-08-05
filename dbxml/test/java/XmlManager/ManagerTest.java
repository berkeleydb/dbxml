/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static com.sleepycat.dbxml.XmlException.CONTAINER_EXISTS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import com.sleepycat.db.Environment;
import com.sleepycat.db.Transaction;
import com.sleepycat.db.TransactionConfig;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlManagerConfig;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

/*
 * Some getXXX method need not to be tested
 * 
 */

public class ManagerTest {
    private static final String CON_NAME = "testData.dbxml";
    private XmlManager mgr;
    private TestConfig hp;

    private static String docString = "<?xml version=\"1.0\" "
    	+ "encoding=\"UTF-8\"?><a_node xmlns:old=\""
    	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
    	+ "><b_node/><c_node>Other text</c_node><d_node/>"
    	+ "</a_node>";

    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlManager!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
	public void setUp() throws Throwable {
	if (hp != null)
	    TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
    }

    @After
	public void tearDown() throws Throwable {
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finished test XmlManager!");
    }

    /*
     * Test for XmlManager constructor
     * 
     */
    @Test
	public void testMangerConstruct() throws Exception{
	XmlManager mgr = null;
	// XmlManager() constructor
	try {
	    mgr = new XmlManager();
	    assertTrue(mgr.existsContainer("notHere.dbxml") == 0);
	} catch (XmlException e) {
	    throw e;
	} catch (FileNotFoundException e) {
	    throw e;
	} finally {
	    if (mgr != null) mgr.delete();
	    mgr = null;
	}

	// XmlManager(XmlManagerConfig config) constructor
	try {
	    XmlManagerConfig conf = new XmlManagerConfig();
	    mgr = new XmlManager(conf);
	    assertTrue(mgr.existsContainer("notHere.dbxml") == 0);
	} catch (XmlException e) {
	    throw e;
	} catch (FileNotFoundException e) {
	    throw e;
	} finally {
	    if (mgr != null) mgr.delete();
	    mgr = null;
	}

	// With null XmlManagerConfig
	Environment env = null;
	try {
	    env = hp.createEnvironment(hp.getEnvironmentPath());
	    mgr = new XmlManager(env, null);
	    assertTrue(mgr.existsContainer("notHere.dbxml") == 0);
	} catch (XmlException e) {
	    throw e;
	} catch (NullPointerException e) {
	    throw e;
	} finally {
	    if (mgr != null) mgr.delete();
	    mgr = null;
	    if (env != null) env.close();
	    env = null;
	}

	// XmlManager(Environment dbenv,XmlManagerConfig config)
	try {
	    env = hp.createEnvironment(hp.getEnvironmentPath());
	    XmlManagerConfig conf = new XmlManagerConfig();
	    mgr = new XmlManager(env, conf);
	    assertTrue(mgr.existsContainer("notHere.dbxml") == 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (mgr != null) mgr.delete();
	    mgr = null;
	    if (env != null) env.close();
	    env = null;
	}
    }

    /*
     * Test for createContainer(String name)
     * 
     */
    @Test
	public void testCreateContainer() throws Throwable {
	XmlTransaction txn = null;
	XmlContainer cont = null;
	try{
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont = mgr.createContainer(txn, CON_NAME);
	    } else
		cont = mgr.createContainer(CON_NAME);
	    assertNotNull(cont);
	    assertEquals(CON_NAME, cont.getName());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}

	// Test exception create the same name container
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont = mgr.createContainer(txn, CON_NAME);
	    } else
		cont = mgr.createContainer(CON_NAME);
	    fail("Failure in ManagerTest.testCreateContainer()");
	} catch (XmlException e) {
	    assertEquals(CONTAINER_EXISTS, e.getErrorCode());
	} finally{
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}

	// create a anonymous container
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont = mgr.createContainer(txn, null);
		txn.commit();
		txn = null;
		cont.delete();
		txn = mgr.createTransaction();
		cont = mgr.openContainer(txn, null);
	    } else {
		cont = mgr.createContainer(null);
		cont.delete();
		cont = mgr.openContainer(null);
	    }
	} catch (XmlException e) {
	    throw e;
	} finally{
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}

	// test if the path is not exist
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont = mgr.createContainer(txn, "/pathNotExist/notHere.dbxml");
	    } else
		cont = mgr.createContainer("/pathNotExist/notHere.dbxml");
	    fail("Failure in ManagerTest.testCreateContainer()");
	} catch (XmlException e) {
	    assertEquals(XmlException.CONTAINER_NOT_FOUND, e.getErrorCode());
	} finally{
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}
    }

    /*
     * Test for creatContainer(String name,XmlContainerConfig config)
     * 
     * 
     */
    @Test
	public void testCreatContainer_StrXmlconcfg() throws XmlException,
							     FileNotFoundException {
	XmlTransaction txn = null;
	XmlContainer cont = null;
	XmlContainerConfig config = new XmlContainerConfig();

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		config.setTransactional(true);
		cont = mgr.createContainer(txn, CON_NAME, config);
	    } else
		cont = mgr.createContainer(CON_NAME, config);
	    assertNotNull(cont);
	    assertEquals(CON_NAME, cont.getName());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}

	// Test for not exist path
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		config.setTransactional(true);
		cont = mgr.createContainer(txn, "/pathNotExist/notHere.dbxml", config);
	    } else
		cont = mgr.createContainer("/pathNotExist/notHere.dbxml", config);
	    fail("Failure in ManagerTest.testCreatContainer_StrXmlconcfg()");
	} catch (XmlException e) {
	    assertEquals(XmlException.CONTAINER_NOT_FOUND, e.getErrorCode());
	} finally{
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (cont != null) cont.delete();
	    cont = null;
	}
    }
  
    /*
     * Test for createDocument()
     * 
     * 
     */
    @Test
	public void testCreateDocument() throws Throwable {
	try {
	    XmlDocument doc = mgr.createDocument();
	    doc.setName("myDoc");
	    assertEquals("myDoc", doc.getName());
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for createIndexLookup()
     * 
     * 
     */
    @Ignore("not implement yet")
	@Test
	public void testCreateIndexLookup() throws Throwable {

	// String uri = "http://www.sleepycat.com/2002/dbxml";
	// String name = "name";

	assertTrue(true);
    }

    /*
     * Test for createInputStream(InputStream is)
     * 
     * 
     */
    @Test
	public void testCreateInputStream() throws Throwable {
	String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node></a_node>";
	ByteArrayInputStream is = new ByteArrayInputStream(docString.getBytes());
	XmlInputStream xin = null;

	try {
	    xin = mgr.createInputStream(is);
	    assertNotNull(xin);
	    assertEquals(xin.curPos(), 0);
	    XmlDocument doc = mgr.createDocument();
	    doc.setContentAsXmlInputStream(xin);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (xin != null) xin.delete();
	}
    }

    /*
     * Test for createQueryContext()
     * 
     * 
     */
    @Test
	public void testCreateQueryContext() throws Throwable {
	XmlQueryContext context = null;
	try {
	    context = mgr.createQueryContext();
	    context.setNamespace("test", "http://querycontext/test");
	    assertNotNull(context);
	    assertEquals("http://querycontext/test", context.getNamespace("test"));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (context != null) context.delete();
	    context = null;
	}
    }

    /*
     * Test for createQueryContext(int rt)
     * 
     * 
     */
    @Test
	public void testCreateQueryContext_Int() throws Throwable {
	XmlQueryContext context = null;
	try {
	    context = mgr.createQueryContext(XmlQueryContext.LiveValues);
	    assertNotNull(context);
	    assertEquals(XmlQueryContext.LiveValues, context.getReturnType());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (context != null) context.delete();
	    context = null;
	}

	try {
	    context = mgr.createQueryContext(987);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (context != null) context.delete();
	    context = null;
	}
    }

    /*
     * Test for createQueryContext(int rt,int et)
     * 
     * 
     */
    @Test
	public void testCreateQueryContext_IntInt() throws Throwable {
	XmlQueryContext context = null;
	try{
	    context = mgr.createQueryContext(XmlQueryContext.LiveValues, XmlQueryContext.Eager);
	    assertNotNull(context);
	    assertEquals(XmlQueryContext.LiveValues, context.getReturnType());
	    assertEquals(XmlQueryContext.Eager, context.getEvaluationType());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (context != null) context.delete();
	    context = null;
	}
    }

    /*
     * Test for createResults()
     * 
     * 
     */
    @Test
	public void testCreateResults() throws Throwable {
	XmlResults results = null;
	try{
	    results = mgr.createResults();
	    assertNotNull(results);
	    assertTrue(results.size() == 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (results != null) results.delete();
	    results = null;
	}
    }

    /*
     * Test for createStdInInputStream()
     * 
     * 
     */
    @Ignore
	@Test
	public void testCreateStdInInputStream() throws Throwable {
	assertTrue(true);
    }

    /*
     * Test for createTransaction()
     * 
     * 
     */
    @Test
	public void testCreateTransaction() throws Throwable {
	if (!hp.isTransactional()) return;

	XmlTransaction txn = null;
	try {
	    txn = mgr.createTransaction();
	    assertNotNull(txn);
	    assertTrue((txn instanceof XmlTransaction));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for createTransaction(Transaction toUse)
     * 
     * 
     */
    @Test
	public void testCreateTransaction_Txn() throws Throwable {
	if (!hp.isTransactional()) return;
    
	Environment env = mgr.getEnvironment();
	Transaction dbTxn = env.beginTransaction(null, null);
	XmlTransaction txn_Use = null;
	try {
	    txn_Use = mgr.createTransaction(dbTxn);
	    assertNotNull(txn_Use);
	    assertTrue((txn_Use instanceof XmlTransaction));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (txn_Use != null) txn_Use.commit();
	}

	// Test fail, if set dbtxn is null,should throw a exception
	XmlTransaction txn_Use2 = null;
	Transaction dbTxn2 = null;
	try {
	    txn_Use2 = mgr.createTransaction(dbTxn2);
	    assertNull(txn_Use2);
	} catch (NullPointerException e) {
	    assertNotNull(e);
	} finally {
	    if (txn_Use2 != null) txn_Use2.commit();
	}
    }

    /*
     * Test for createTransaction(Transaction toUse,TransactionConfig config)
     * 
     * 
     */
    @Test
	public void testCreateTransaction_TxnTxnconf() throws Throwable {
	if (!hp.isTransactional()) return;

	Environment env = mgr.getEnvironment();
	TransactionConfig txnConf = new TransactionConfig();
	Transaction dbTxn = env.beginTransaction(null, txnConf);
	XmlTransaction txn_Use = null;
	try {
	    txn_Use = mgr.createTransaction(dbTxn, txnConf);
	    assertNotNull(txn_Use);
	    assertTrue((txn_Use instanceof XmlTransaction));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (txn_Use != null) txn_Use.commit();
	    txn_Use = null;
	    dbTxn.commit();
	}

	try {
	    txn_Use = mgr.createTransaction(null, null);
	    assertNotNull(txn_Use);
	    assertTrue((txn_Use instanceof XmlTransaction));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (txn_Use != null) txn_Use.commit();
	    txn_Use = null;
	}
    }

    /*
     * Test for createUpdateContext()
     * 
     * 
     */
    @Test
	public void testCreateUpdateContext() throws Throwable {
	XmlUpdateContext upc = mgr.createUpdateContext();
	assertNotNull(upc);
	assertTrue(upc instanceof XmlUpdateContext);
    }

    /*
     * Test for createURLInputStream(String baseId, String systemId) @TODO:Which
     * URL to test?
     * 
     */
    @Ignore("Need some prerequire condition")
	@Test
	public void testCreateURLInputStream() throws Throwable {
	String baseId = "http://dbxml.dbxml";
	String systemId = "hellotest";
	XmlInputStream in = mgr.createURLInputStream(baseId, systemId);
	assertNotNull(in);
	assertTrue(in instanceof XmlInputStream);
    }

    /*
     * Test for dumpContainer(String name, String filename)
     * 
     */
    @Test
	public void testDumpContainer() throws Throwable {
    XmlContainerConfig config = new XmlContainerConfig();
    if (hp.isTransactional()) config.setTransactional(true);
    if(!hp.isNodeContainer()) config.setContainerType(XmlContainer.WholedocContainer);
	XmlContainer cont = hp.createContainer(CON_NAME, mgr, config);
	
	cont.putDocument(docName, docString);

	// Test container not closed
	try {
	    mgr.dumpContainer(CON_NAME, "dump.txt");
	    fail("Failure in ManagerTest.testDumpContainer()");
	} catch (XmlException e) {
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	} finally {
	    cont.close();
	}
    
	try {
	    mgr.dumpContainer(CON_NAME, "dump.txt");
	} catch (XmlException e) {
	    throw e;
	}
	File fi = new File("dump.txt");
	assertTrue(fi.exists());
	fi.delete();

	// Test for container not exist
	try {
	    mgr.dumpContainer("ContainerNotExist.dbxm", "dump.txt");
	    fail("Failure in ManagerTest.testDumpContainer()");
	} catch (XmlException e) {
	    assertEquals(XmlException.DATABASE_ERROR, e.getErrorCode());
	} finally {
		fi = new File("dump.txt");
		if(fi.exists()) fi.delete();
	}
    }

    /*
     * Test for existsContainer(String name)
     * 
     * 
     */
    @Test
	public void testExistsContainer() throws Throwable {
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);
	try{
	    assertTrue(mgr.existsContainer(CON_NAME) != 0);
	    assertTrue(mgr.existsContainer("ContainerNotExists.dbxml") == 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    cont.delete();
	}
    }

    /*
     * Test for setDefaultContainerConfig(XmlContainerConfig config)
     * 
     * 
     */
    @Test
	public void testSetDefaultContainerConfig() throws Throwable {
	XmlContainerConfig config = new XmlContainerConfig();
	config.setContainerType(XmlContainer.WholedocContainer);
	mgr.setDefaultContainerConfig(config);
	assertTrue(mgr.getDefaultContainerConfig().getContainerType() == XmlContainer.WholedocContainer);
    }

    /*
     * Test for set/getDefaultContainerType()
     * 
     * 
     */
    @Test
	public void testGetDefaultContainerType() throws Throwable {
	mgr.setDefaultContainerType(XmlContainer.WholedocContainer);
	assertTrue(mgr.getDefaultContainerType() == XmlContainer.WholedocContainer);
    }
    
    /*
     * Test for set/getDefaultCompression()
     * 
     * 
     */
    @Test
	public void testGetSetDefaultCompression() throws Throwable {
    TestCompression testComp = new TestCompression();
    String compName = "testName";
	mgr.setDefaultCompression(compName);
	mgr.registerCompression(compName, testComp);
	assertEquals(compName, mgr.getDefaultCompression());
	XmlContainer cont = null;
	if (hp.isNodeContainer()) {
		try {
			cont = mgr.createContainer(CON_NAME);
			fail("Cannot create a node container with compression.");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		} finally {
			if (cont != null) cont.delete();
		}	
	} else {
		try {
			mgr.setDefaultContainerType(XmlContainer.WholedocContainer);
			cont = mgr.createContainer(CON_NAME);
			XmlContainerConfig xcc = cont.getContainerConfig();
			assertEquals(compName, xcc.getCompression());
		} catch (XmlException e) {
			throw e;
		} finally {
			if (cont != null) cont.delete();
		}
	}
    testComp.delete();
    }

    /*
     * Test for loadContainer(String name, String filename, XmlUpdateContext uc)
     * 
     * 
     */
    @Test
	public void testLoadContainer() throws Throwable {
    if (hp.isCDS()) return; //CDS hangs on load for now
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);
	cont.putDocument(docName, docString);
	cont.delete();
    
	mgr.dumpContainer(CON_NAME, "dump.txt");
	mgr.removeContainer(CON_NAME);
    
	File fi = new File("dump.txt");
	assertTrue(fi.exists());
	XmlUpdateContext uc = mgr.createUpdateContext();
	try {
	    mgr.loadContainer(CON_NAME, "dump.txt", uc);
	    cont = mgr.openContainer(CON_NAME);
	    XmlDocument doc = cont.getDocument(docName);
	    assertEquals(doc.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	} finally {
		cont.delete();
		mgr.removeContainer(CON_NAME);
	}
	
	try {
	    mgr.loadContainer(CON_NAME, "dump.txt");
	    cont = mgr.openContainer(CON_NAME);
	    XmlDocument doc = cont.getDocument(docName);
	    assertEquals(doc.getContentAsString(), docString);
	} catch (XmlException e) {
		cont.delete();
		mgr.removeContainer(CON_NAME);
	    throw e;
	}
	
	//Test that it throws on an open container
	try {
	    mgr.loadContainer(CON_NAME, "dump.txt");
	    fail("Failure in ManagerTest.testLoadContainer.");
	} catch (XmlException e) {
	    assertEquals(XmlException.DATABASE_ERROR, e.getErrorCode());
	} finally {
		cont.delete();
		mgr.removeContainer(CON_NAME);
	    fi.delete();
	}
    }

    /*
     * Test for openContainer(String name)
     * 
     */
    @Test
	public void testOpenContainer_Str() throws Throwable {
	XmlTransaction txn = null;
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);

	// Why the two container are not same
	XmlContainer contReopen = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		contReopen = mgr.openContainer(txn, CON_NAME);
	    } else
		contReopen = mgr.openContainer(CON_NAME);
	    assertEquals(CON_NAME, contReopen.getName());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    if (contReopen != null) contReopen.delete();
	    contReopen = null;
	    cont.delete();
	}
    
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		contReopen = mgr.openContainer(txn, CON_NAME);
	    } else
		contReopen = mgr.openContainer(CON_NAME);
	    assertNotNull(contReopen);
	    assertEquals(CON_NAME, contReopen.getName());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    if (contReopen != null) contReopen.delete();
	    contReopen = null;
	}
    
	// Test open not exist container
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		contReopen = mgr.openContainer(txn, "containerNotExist.dbxml");
	    } else
		contReopen = mgr.openContainer("containerNotExist.dbxml");
	    fail("Failure in ManagerTest.testOpenContainer_Str()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    if (contReopen != null) contReopen.delete();
	    contReopen = null;
	}
    }

    /*
     * Test for openContainer(String name, XmlContainerConfig config)
     * 
     */
    @Test
	public void testOpenContainer_StrConconf() throws Throwable {
	XmlTransaction txn = null;
	XmlContainerConfig containerConf = new XmlContainerConfig();
	containerConf.setAllowCreate(true);
	XmlContainer cont = null;
    
	try{
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont = mgr.openContainer(txn, CON_NAME, containerConf);
	    } else
		cont = mgr.openContainer(CON_NAME, containerConf);
	    assertEquals(CON_NAME, cont.getName());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    if (cont != null) cont.delete();
	    cont = null;
	}
    }
    
    /*
     * Test for registerCompression(String name, XmlCompression compression)
     * 
     */
	@Test
	public void testRegisterCompression() throws Throwable {
		TestCompression test = new TestCompression();
		String testName = "test";
		
		try {
			mgr.registerCompression(XmlContainerConfig.DEFAULT_COMPRESSSION, test);
			fail("Cannot register compression with an existing name.");
		} catch (XmlException e) {
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		
		//Tests that a container cannot be created/opened if the compression is not registered
		XmlContainer cont = null;
		XmlContainerConfig config = new XmlContainerConfig();
		config.setContainerType(XmlContainer.WholedocContainer);
		config.setCompression(testName);
		try {
			cont = mgr.createContainer(CON_NAME, config);
			fail("Cannot open a container with user compression unless it is registered.");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		} finally {
			if (cont != null) cont.delete();
			cont = null;
		}
		try { mgr.removeContainer(CON_NAME); } catch (XmlException e) {}

		mgr.registerCompression(testName, test);
		
		//Create a container with the compression, then open that container
		try {
			config.setCompression(testName);
			cont = mgr.createContainer(CON_NAME, config);
		} catch (XmlException e) {
			throw e;
		} finally {
			if (cont != null) cont.delete();
			cont = null;
		}
		
		try {
			cont = mgr.openContainer(CON_NAME);
		} catch (XmlException e) {
			throw e;
		} finally {
			if (cont != null) cont.delete();
			cont = null;
		}
        test.delete();
    }

    /**
     * TBD: Test for query method, prepare what to query and the results , need
     * some xml data*
     */

    /** ******************************************************************** */

    /*
     * Test for registerResolver(XmlResolver resolver) Not Clear how to write
     * this test
     * 
     */
    @Ignore
	@Test
	public void testRegisterResolver() throws Throwable {

    }

    /*
     * Test for reindexContainer(String name, XmlUpdateContext context,
     * 							XmlContainerConfig config)
     * reindexContainer(XmlTransaction txn, String name,
     * 					XmlUpdateContext context, XmlContainerConfig config)
     * 
     * Simple test that just reindexes a container with one document
     */
    @Test
	public void testReindexContainer() throws Throwable {

    if (hp.isCDS()) return;// in CDS will hang, see [#16238]
	String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node><b_node>Other text</b_node></a_node>";
	
	XmlContainer cont = mgr.createContainer(CON_NAME);

	XmlUpdateContext context = mgr.createUpdateContext();
	cont.putDocument("test", docString, context);
	cont.delete();

	XmlContainerConfig conf = new XmlContainerConfig();
	conf.setIndexNodes(XmlContainerConfig.Off);
	XmlTransaction txn = null;
	
	
	try {
		if(hp.isTransactional()){
			txn = mgr.createTransaction();
			mgr.reindexContainer(txn,CON_NAME, null, conf);
		}else {	
			mgr.reindexContainer(CON_NAME, null, conf);
		}
	} catch (XmlException e) {
		throw e;
	}finally {
		if (hp.isTransactional() && txn != null) txn.commit();
	}
	
	cont = mgr.openContainer(CON_NAME);
	assertEquals(cont.getIndexNodes(), false);
	cont.close();
	
	
	//test remove the statistics db then reindex
	try {
		if(hp.isTransactional()){
			txn = mgr.createTransaction();
			conf.setStatisticsEnabled(XmlContainerConfig.Off);
			mgr.reindexContainer(txn,CON_NAME, null, conf);//remove statistics db
			txn.commit();
			txn = mgr.createTransaction();
			conf.setStatisticsEnabled(XmlContainerConfig.On);
			mgr.reindexContainer(txn,CON_NAME, null, conf);
		}else {	
			conf.setStatisticsEnabled(XmlContainerConfig.Off);
			mgr.reindexContainer(CON_NAME, null, conf);//remove statistics db
			conf.setStatisticsEnabled(XmlContainerConfig.On);
			mgr.reindexContainer(CON_NAME, null, conf);
		}
	} catch (XmlException e) {
		throw e;
	}finally {
		if (hp.isTransactional() && txn != null) txn.commit();
	}
		
    }

    /*
     * Test for 
     * compactContainer(String name, XmlUpdateContext context)
     * compactContainer(XmlTransaction txn, String name,XmlUpdateContext context)
     * 
     * Just test the function works
     */
    @Test
	public void testCompactContainer() throws Throwable {

	String docString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><a_node atr=\"test\"><b_node>Some text</b_node><b_node>Other text</b_node></a_node>";
	
	XmlContainer cont = mgr.createContainer(CON_NAME);

	XmlUpdateContext context = mgr.createUpdateContext();
	cont.putDocument("test", docString, context);
	cont.delete();

	XmlTransaction txn = null;
	
	try {
		if(hp.isTransactional()){
			txn = mgr.createTransaction();
			mgr.compactContainer(txn,CON_NAME, context);
		}else {	
			mgr.compactContainer(CON_NAME, context);
		}
	} catch (XmlException e) {
		if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}finally {
		if (hp.isTransactional() && txn != null) txn.commit();
	}
	
	cont = mgr.openContainer(CON_NAME);
	assertEquals(cont.getDocument("test").getContentAsString(), docString);
	cont.close();
	
    }
    
    
    /*
     * Test for removeContainer(String name)
     * 
     * 
     */
    @Test
	public void testRemoveContainer_Str() throws Throwable {
	XmlTransaction txn = null;
	// Create an container, then remove it
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);
	cont.delete();
	cont = null;
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.removeContainer(txn, CON_NAME);
		txn.commit();
		txn = null;
	    } else
		mgr.removeContainer(CON_NAME);
	    assertTrue(mgr.existsContainer(CON_NAME) == 0);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// Test container not exist
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.removeContainer(txn, "containerThatNotExsits");
	    } else
		mgr.removeContainer("containerThatNotExsits");
	    fail("Failure in ManagerTest.testRemoveContainer_Str()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	}

	// Test container not closed
	XmlContainer cont2 = hp.createContainer(CON_NAME, mgr);
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.removeContainer(txn, CON_NAME);
		txn.commit();
		txn = null;
	    } else
		mgr.removeContainer(CON_NAME);
	    fail("Failure in ManagerTest.testRemoveContainer_Str()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    cont2.delete();
	}
    }

    /*
     * Test for renameContainer(String oldName, String newName)
     * 
     * 
     */
    @Test
	public void testRenameContainer_StrStr() throws Throwable {
	XmlTransaction txn = null;
	String name = "newName.dbxml";
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);
	cont.delete();
    
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.renameContainer(txn, CON_NAME, name);
		txn.commit();
		txn = null;
	    } else
		mgr.renameContainer(CON_NAME, name);
	    assertTrue(mgr.existsContainer(CON_NAME) == 0);
	    assertTrue(mgr.existsContainer(name) != 0);
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.removeContainer(txn, name);
		txn.commit();
		txn = null;
	    } else
		mgr.removeContainer(name);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.renameContainer(txn, "containerThatNotExsits", name);
		txn.delete();
		txn = null;
	    } else
		mgr.renameContainer("containerThatNotExsits", name);
	    fail("Failure in ManagerTest.testRenameContainer_StrStr()");
	} catch (XmlException e) {
	    assertEquals(XmlException.CONTAINER_NOT_FOUND, e.getErrorCode());
	    assertNotNull(e.getMessage());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	}

	// Test container not closed
	XmlContainer cont2 = hp.createContainer(CON_NAME, mgr);
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		mgr.renameContainer(txn, CON_NAME, name);
		txn.commit();
		txn = null;
	    } else
		mgr.renameContainer(CON_NAME, name);
	    fail("Failure in ManagerTest.testRenameContainer_StrStr()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    cont2.delete();
	}
    }

    /*
     * Tests for container manipulation using XmlContainerConfig:
     *  o setIndexNodes()
     *  o setStatistics()
     * 
     */
    @Test
	public void testContainer_config() throws Throwable {
	String docString = "<root/>";;
	XmlManager mgr = new XmlManager();
	// default configuration
	XmlContainer cont = mgr.createContainer(CON_NAME, new XmlContainerConfig());
	XmlContainerConfig cconfig = cont.getContainerConfig();
	// verify defaults
	//  node container, node indexes, structural stats, no compression
	assertTrue(cconfig.getContainerType() == XmlContainer.NodeContainer);
	assertTrue(cconfig.getIndexNodesFlag() == XmlContainerConfig.On);
	assertTrue(cconfig.getStatisticsEnabledFlag() == XmlContainerConfig.On);

	XmlUpdateContext context = mgr.createUpdateContext();
	cont.putDocument("test", docString, context);
	cont.delete();

	// reindex and turn off node indexes and stats
	XmlContainerConfig reindexConfig = new XmlContainerConfig();
	reindexConfig.setStatisticsEnabled(XmlContainerConfig.Off);
	reindexConfig.setIndexNodes(XmlContainerConfig.Off);
	reindexConfig.setContainerType(XmlContainer.WholedocContainer); // will be no-op
	mgr.reindexContainer(CON_NAME, reindexConfig);
	
	cont = mgr.openContainer(CON_NAME, reindexConfig);
	XmlContainerConfig newConfig = cont.getContainerConfig();
	assertTrue(newConfig.getContainerType() == XmlContainer.NodeContainer);
	assertTrue(newConfig.getIndexNodesFlag() == XmlContainerConfig.Off);
	assertTrue(newConfig.getStatisticsEnabledFlag() == XmlContainerConfig.Off);
	cont.delete();

	// reindex again, using default config
	mgr.reindexContainer(CON_NAME, new XmlContainerConfig());
	cont = mgr.openContainer(CON_NAME, reindexConfig);
	newConfig = cont.getContainerConfig();
	assertTrue(newConfig.getContainerType() == XmlContainer.NodeContainer);
	assertTrue(newConfig.getIndexNodesFlag() == XmlContainerConfig.Off);
	assertTrue(newConfig.getStatisticsEnabledFlag() == XmlContainerConfig.Off);
	cont.delete();

	mgr.delete();
    }

    /*
     * Test for 
     * truncateContainer(String name)
     * truncateContainer(XmlTransaction txn, String name)
     * 
     */
    @Test
	public void testTruncateContainer() throws XmlException {

    	String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node><b_node>Other text</b_node></a_node>";
    	XmlTransaction txn = null;
    	
    	XmlContainer cont = mgr.createContainer(CON_NAME);

    	XmlUpdateContext context = mgr.createUpdateContext();
    	cont.putDocument("test.xml", docString, context);
    	
    	// truncate on open container throw exception
    	try {
    		if(hp.isTransactional()){
    			txn = mgr.createTransaction();
    			mgr.truncateContainer(txn,CON_NAME, context);
    		}else {	
    			mgr.truncateContainer(CON_NAME, context);
    		}
    		fail("Fail to throw exception when trancat at open container.");
    	} catch (XmlException e) {
    	    assertNotNull(e);
    	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
    	}finally {
    		if (hp.isTransactional() && txn != null) txn.commit();
    	}   	       
    	cont.close();
	
    	context = mgr.createUpdateContext();
    	try {
    		if(hp.isTransactional()){
    			txn = mgr.createTransaction();
    			mgr.truncateContainer(txn,CON_NAME, context);
    		}else {	
    			mgr.truncateContainer(CON_NAME, context);
    		}
    	} catch (XmlException e) {
    	    throw e;
    	}finally {
    		if (hp.isTransactional() && txn != null) txn.commit();
    	}
    	
    	cont = mgr.openContainer(CON_NAME);
    	
    	// after truncate, can't find the document
    	try {
    		cont.getDocument("test.xml");
    		fail("Fail to throw exception when getDocument on a truncated container.");
    	} catch (XmlException e){
    		assertNotNull(e);
    		assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
    	}
    	cont.close();
    	
    	
    }

    
    /*
     * Test for upgradeContainer(String name, XmlUpdateContext context) (String
     * name, XmlUpdateContext context) How to test it? Need get an old version
     * file to test ,find the old version file
     */
    @Ignore
	@Test
	public void testUpgradeContainer() {

    }

    /*
     * Test for verifyContainer(String name, String filename)
     * 
     * 
     */
    @Test
	public void testVerifyContainer_StrStr() throws XmlException,
							FileNotFoundException {
	XmlTransaction txn = null;
	String docString = "<?xml version=\"1.0\" ?><a_node atr=\"test\"><b_node>Some text</b_node><b_node>Other text</b_node></a_node>";
	XmlContainer cont = hp.createContainer(CON_NAME, mgr);
	XmlUpdateContext context = mgr.createUpdateContext();
	if (hp.isTransactional()) {
	    txn = mgr.createTransaction();
	    cont.putDocument(txn, "test", docString, context);
	} else
	    cont.putDocument("test", docString, context);

	// Test for container not closed, throw exception
	try {
	    mgr.verifyContainer(CON_NAME, CON_NAME);
	    fail("Failure in ManagerTest.testVerifyContainer_StrStr()");
	} catch (XmlException e) {
	    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    txn = null;
	    cont.delete();
	}

	try {
	    mgr.verifyContainer(CON_NAME, "file1.txt");
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for verifyContainer(String name, String filename, VerifyConfig
     * config)
     * 
     * 
     */
    @Ignore
	@Test
	public void testVerifyContainer_StrStrConf() {

    }

}
