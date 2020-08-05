/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007 Oracle.  All rights reserved.
 *
 */

package dbxmltest;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

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


public class MetaDataIteratorTest {

    private static String docString = "<?xml version=\"1.0\" ?>"
	+ "<old:a_node atr1=\"test\" atr2=\"test2\" xmlns:old="
	+ "\"http://dbxmltest.test/test\"><b_node/>"
	+ "<c_node>Other text</c_node><d_node/></old:a_node>";

    private static String docName = "testDoc.xml";
    private XmlManager mgr = null;
    private TestConfig hp = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlMetaDataIterator!");
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
	System.out.println("Finished test XmlMetaDataIterator!");
    }

    /*
     * Test for copy constructor,XmlMetaDataIterator(XmlMetaDataIterator o) 
     * 
     */
    @Test
	public void testConstructor() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();

	XmlMetaDataIterator itcp = null;

	try {
	    itcp = new XmlMetaDataIterator(it);
	} catch (XmlException e) {
	    e.printStackTrace();
	    fail("Failure in MetaDatainteratorTest.testConstructor()");
	}

	XmlMetaData md = itcp.next();
	assertEquals(md.get_name(), "name");

	md = itcp.next();
	assertEquals(md.get_name(), "testMetaData");
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
	XmlMetaData md = it.next();

	// before delete()
	assertEquals("name", md.get_name());
    }

    /*
     * Test for next() 
     * 
     */
    @Test
	public void testNext() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();
	XmlMetaData md = null;

	try {
	    md = it.next();
	    assertEquals("name", md.get_name());
	} catch (XmlException e) {
	    throw e;
	}

	try {
	    md = it.next();
	    assertEquals("testMetaData", md.get_name());
	} catch (XmlException e) {
	    throw e;
	}

	//in the last place
	try {
	    md = it.next();
	    assertNull(md);

	    md = it.next();
	    assertNull(md);
	} catch (XmlException e) {
	    throw e;
	}
    }

    /*
     * Test for reset() 
     * 
     */
    @Test
	public void testReset() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue value = new XmlValue(XmlValue.STRING, "test");
	doc.setMetaData("http://dbxmltest.test", "testMetaData", value);

	XmlMetaDataIterator it = doc.getMetaDataIterator();
	XmlMetaData md = null;

	//in the first place
	try {
	    it.reset();
	} catch (XmlException e) {
	    throw e;
	}

	md = it.next();
	assertEquals("name", md.get_name());

	try {
	    it.reset();
	} catch (XmlException e) {
	    throw e;
	}

	md = it.next();
	assertEquals("name", md.get_name());

	md = it.next();
	md = it.next();
	assertNull(md);

	//in the last place
	try {
	    it.reset();
	    md = it.next();
	    assertEquals("name", md.get_name());
	} catch (XmlException e) {
	    throw e;
	}
    }

}
