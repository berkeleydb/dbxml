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

import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlMetaData;
import com.sleepycat.dbxml.XmlMetaDataIterator;
import com.sleepycat.dbxml.XmlValue;

public class MetaDataTest {

    private static String docString = "<?xml version=\"1.0\" ?>"
	+ "<old:a_node atr1=\"test\" atr2=\"test2\" xmlns:old="
	+ "\"http://dbxmltest.test/test\"><b_node/>"
	+ "<c_node>Other text</c_node><d_node/></old:a_node>";

    private static String docName = "testDoc.xml";
    private XmlManager mgr = null;
    private TestConfig hp = null;

    @BeforeClass
	public static void setupClass() {	
	System.out.println("Begin test XmlMetaData!");
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
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finished test XmlMetaData!");
    }

    /*
     * Test for delete()
     * 
     */
    @Test
	public void testDelete() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();

	it.next();
	XmlMetaData md = it.next();

	// before delete()
	assertEquals("testMetaData", md.get_name());
    }

    /*
     * Test for get_name()
     * 
     */
    @Test
	public void testGetName() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();

	XmlMetaData md = it.next();
	try {
	    assertEquals(md.get_name(), "name");
	} catch (XmlException e) {
	    throw e;
	}

	md = it.next();
	try {
	    assertEquals(md.get_name(), "testMetaData");
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for get_uri()
     * 
     */
    @Test
	public void testGetUri() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();

	XmlMetaData md = it.next();
	try {
	    assertEquals(md.get_uri(), "http://www.sleepycat.com/2002/dbxml");
	} catch (XmlException e) {
	    throw e;
	}

	md = it.next();
	try {
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for get_value()
     * 
     */
    @Test
	public void testGetValue() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();

	XmlMetaData md = it.next();
	XmlValue vl = null;
	try {
	    vl = md.get_value();
	} catch (XmlException e) {
	    throw e;
	}
	assertEquals(vl.asString(), docName);

	md = it.next();
	XmlValue vll = null;
	try {
	    vll = md.get_value();
	} catch (XmlException e) {
	    throw e;
	}
	assertEquals(vll.asString(), "test");
    }
}
