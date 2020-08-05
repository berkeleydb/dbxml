/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
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

import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;

public class InputStreamTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";
    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlInputStream!");
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
	System.out.println("Finished test XmlInputStream!");
    }

    /*
     * Test for curPos()
     * 
     */
    @Test
	public void testCurPos() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);

	XmlInputStream is = doc.getContentAsXmlInputStream();

	try {
	    assertEquals(is.curPos(), 0);
	} catch (XmlException e) {
	    throw e;
	}

	byte[] toFill = new byte[docString.length()];

	is.readBytes(toFill, 30);

	try {
	    assertEquals(is.curPos(), 30);
	} catch (XmlException e) {
	    throw e;
	}

	is.readBytes(toFill, 500);

	//read to the end
	try {
	    assertEquals(is.curPos(), 172);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	}
	//test no content stream
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName(docName);

	XmlInputStream is2 = doc2.getContentAsXmlInputStream();
	try {
		assertEquals(0, is2.curPos());
	} catch (XmlException e) {
		throw e;
	} finally {
		is2.delete();
	}
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

	XmlInputStream is = doc.getContentAsXmlInputStream();

	is.delete();

	//test after delete()
	try {
	    is.curPos();
	    fail("curPos succeeded after stream deleted.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}
    }

    /*
     * Test for readBytes()
     * 
     */
    @Test
	public void testReadBytes() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);

	XmlInputStream is = doc.getContentAsXmlInputStream();

	byte[] toFill = new byte[docString.length()];

	is.readBytes(toFill, docString.length());

	for (int i = 0; i < docString.length(); i++)
	    assertEquals(toFill[i], docString.getBytes()[i]);
	is.delete();

	//test no content stream
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName(docName);

	XmlInputStream is2 = doc2.getContentAsXmlInputStream();

	try {
		assertEquals(0, is2.readBytes(toFill, 10));
	} catch (XmlException e) {
		throw e;
	} finally {
		is2.delete();
	}
    }
}
