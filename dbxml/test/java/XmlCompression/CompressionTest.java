/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import com.sleepycat.dbxml.XmlCompression;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlData;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class CompressionTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";
    private String compressionName = "TestCompression";
    private static String docString = "<old:a_node xmlns:old=\""
    	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
    	+ "><b_node/><c_node>Other text</c_node><d_node/>"
    	+ "</old:a_node>";

    private static String docName = "testDoc.xml";
    
    @BeforeClass
    public static void setupClass() {
	System.out.println("Begin test XmlCompression!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
    public void setUp() throws Throwable {
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
		XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
    }

    @After
    public void tearDown() throws Throwable {
	if (cont != null) hp.closeContainer(cont);
	cont = null;
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
    public static void tearDownClass() {
	System.out.println("Finished test XmlCompression!");
    }
    
    @Test
    public void testCompressionConstructor() throws Exception
    {
	TestCompression test = new TestCompression();
	assertNotNull(test);
	assertTrue(test instanceof TestCompression);
	test.delete();
    }
    
    @Test
    public void testDelete() throws Exception
    {
	TestCompression test = new TestCompression();
	assertNotNull(test);
	test.delete();
    }
    
    @Test
    public void testCompressDecompression() throws Exception
    {
    if (hp.isNodeContainer()) return;  //no compression yet for node storage
	TestCompression test = new TestCompression();
	XmlTransaction txn = null;
	XmlDocument doc2;
	mgr.registerCompression(compressionName, test);
	XmlContainerConfig config = new XmlContainerConfig();
	config.setCompression(compressionName);
	config.setTransactional(hp.isTransactional());
	config.setContainerType(XmlContainer.WholedocContainer);
	cont = hp.createContainer(CON_NAME, mgr, config);
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	try {
		if (hp.isTransactional()) {
			txn = mgr.createTransaction();
			cont.putDocument(txn, doc);
			doc2 = cont.getDocument(txn, docName);
		} else {
			cont.putDocument(doc);
			doc2 = cont.getDocument(docName);
		}
		assertEquals(doc2.getContentAsString(), docString);
	} catch (XmlException e) {
		throw e;
	} finally {
		if (txn != null) txn.commit();
		test.delete();
	}
    }
    
   public class ErrorCompression extends XmlCompression {
	   public boolean compress(XmlTransaction txn, XmlData source, XmlData dest) throws XmlException {
		   return false;
	   }
	   
	   public boolean decompress(XmlTransaction txn, XmlData source, XmlData dest) throws XmlException {
		   return false;
	   }
    }
    
    @Test
    public void testCompressionError() throws Exception
    {
    	if (hp.isNodeContainer()) return;  //no compression yet for node storage
    	ErrorCompression test = new ErrorCompression();
    	XmlTransaction txn = null;
    	mgr.registerCompression(compressionName, test);
    	XmlContainerConfig config = new XmlContainerConfig();
    	config.setCompression(compressionName);
    	config.setTransactional(hp.isTransactional());
    	config.setContainerType(XmlContainer.WholedocContainer);
    	cont = hp.createContainer(CON_NAME, mgr, config);
    	XmlDocument doc = mgr.createDocument();
    	doc.setName(docName);
    	doc.setContent(docString);
    	try {
    		if (hp.isTransactional()) {
    			txn = mgr.createTransaction();
    			cont.putDocument(txn, doc);

    		} else 
    			cont.putDocument(doc);
    		fail("Failure in CompressionTest.testCompressionError()");
    	} catch (XmlException e) {
    		assertNotNull(e);
    	} finally {
    		if (txn != null) txn.commit();
    		test.delete();
    	}
    }

}
