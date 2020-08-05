/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
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
import org.junit.Ignore;
import org.junit.Test;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

public class EventReaderTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private final String CON_NAME = "test.dbxml";
    private XmlContainer cont = null;

    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";

    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlEventReader!");
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
	System.out.println("Finished test XmlEventReader!");
    }

    /*
     * Test for close()
     * 
     */
    @Test
	public void testClose() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	assertNotNull(reader);
	reader.close();

	// after close should throw exception
	try {
	    reader.next();
	    fail("reader succeeded after it was closed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}
    }

    /*
     * Test for encodingSet()
     * 
     */
    @Test
	public void testEncodingSet() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	assertTrue(reader.encodingSet());
	reader.close();

	XmlDocument doc2 = mgr.createDocument();
	doc2.setName("test2");
	String content = "<?xml version=\"1.0\" "
	    + "?><old:a_node xmlns:old=\""
	    + "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</old:a_node>";
	doc2.setContent(content);
	XmlEventReader reader2 = doc2.getContentAsEventReader();
	assertFalse(reader2.encodingSet());
	reader2.close();
    }

    /*
     * Test for getAttributeCount()
     * 
     */
    @Test
	public void testGetAttributeCount() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	//test not StartElement 
	try {
	    reader.getAttributeCount();
	    fail("reader got an attribute count on a start element.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeCount(), 3);
		break;
	    }
	}

	reader.close();
    }

    /*
     * Test for getAttributeLocalName()
     * 
     */
    @Test
	public void testGetAttributeLocalName() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	try {
	    reader.getAttributeLocalName(0);
	    fail("reader succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeLocalName(0), "old");
		assertEquals(reader.getAttributeLocalName(1), "atr1");
		assertEquals(reader.getAttributeLocalName(2), "atr2");

		try {
		    reader.getAttributeLocalName(6);
		    reader.close();
		    fail("reader succeeded when it should have failed.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
		break;
	    }
	}
	reader.close();
    }

    /*
     * Test for getAttributeNamespaceURI()
     * 
     */
    @Test
	public void testGetAttributeNamespaceURI() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	try {
	    reader.getAttributeNamespaceURI(0);
	    fail("reader found attribute namespace uri when there are no attributes.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}

	//get a new reader
	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeNamespaceURI(0),
			     "http://www.w3.org/2000/xmlns/");

		try {
		    reader.getAttributeNamespaceURI(6);
		    reader.close();
		    fail("reader found attribute namespace uri on a non-existant attribute.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
		break;
	    }
	}

	reader.close();
    }

    /*
     * Test for getAttributePrefix()
     * 
     */
    @Test
	public void testGetAttributePrefix() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	try {
	    reader.getAttributePrefix(0);
	    fail("reader found attribute prefix when there are no attributes.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributePrefix(0), "xmlns");
		assertEquals(reader.getAttributePrefix(1), null);
		assertEquals(reader.getAttributePrefix(2), null);
		try {
		    reader.getAttributePrefix(6);
		    reader.close();
		    fail("reader found attribute prefix on non-existant attribute.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
		break;
	    }
	}

	reader.close();
    }

    /*
     * Test for getAttributeValue()
     * 
     */
    @Test
	public void testGetAttributeValue() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	// test not startElement
	try {
	    reader.getAttributeValue(0);
	    fail("reader found attribute value when there are no attributes.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeValue(0),
			     "http://dbxmltest.test/test");
		assertEquals(reader.getAttributeValue(1), "test");
		assertEquals(reader.getAttributeValue(2), "test2");
		try {
		    // test out of range
		    reader.getAttributeValue(6);
		    reader.close();
		    fail("reader found attribute value on non-existent attribute.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
		break;
	    }
	}

	reader.close();
    }

    /*
     * Test for getEncoding()
     * 
     */
    @Test
	public void testGetEncoding() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	try {
	    assertEquals(reader.getEncoding(), "UTF-8");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    reader.close();
	}

	// no encoding set in XmlDocument
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName("test2");
	String content = "<?xml version=\"1.0\" "
	    + "?><old:a_node xmlns:old=\""
	    + "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><b_node/><c_node>Other text</c_node><d_node/>"
	    + "</old:a_node>";
	doc2.setContent(content);
	XmlEventReader reader2 = doc2.getContentAsEventReader();
	assertEquals(reader2.getEncoding(), null);

	reader2.close();
    }

    /*
     * Test for getEventType()
     * 
     */
    @Test
	public void testGetEventType() throws Throwable {
	XmlTransaction txn = null;
	// Set a content string
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlDocument doc = null;
	XmlEventReader reader = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, docName, str, context);
		doc = cont.getDocument(txn, docName);
	    }else{
		cont.putDocument(docName, str, context);
		doc = cont.getDocument(docName);
	    }

	    reader = doc.getContentAsEventReader();
	    assertEquals(reader.getEventType(), XmlEventReader.StartDocument);
	    if (reader.hasNext()) {
		reader.next();
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.StartElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.Comment);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.StartElement);
		reader.next();
		assertEquals(reader.getEventType(),
			     XmlEventReader.ProcessingInstruction);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.StartElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.Characters);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.EndElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.StartElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.CDATA);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.EndElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.EndElement);
		reader.next();
		assertEquals(reader.getEventType(), XmlEventReader.EndDocument);
	    }
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(reader != null) reader.close();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getLocalName()
     * 
     */
    @Test
	public void testGetLocalName() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";

	doc.setName(docName);
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// test not startElement, endElement, processingInstruction
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() != XmlEventReader.StartElement
		&& reader.getEventType() != XmlEventReader.EndElement
		&& reader.getEventType() != XmlEventReader.ProcessingInstruction) {
		try {
		    assertEquals(reader.getLocalName(), null);
		} catch (XmlException e) {
		    throw e;
		}
	    }
	}

	reader.close();
	reader = doc.getContentAsEventReader();
	// test processingInstruction
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() == XmlEventReader.ProcessingInstruction) {
		try {
		    assertEquals(reader.getLocalName(), "audio-output");
		} catch (XmlException e) {
		    throw e;
		}
	    }
	}

	reader.close();
	reader = doc.getContentAsEventReader();
	// test startElement
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() == XmlEventReader.StartElement) {
		try {
		    assertEquals(reader.getLocalName(), "a_node");
		} catch (XmlException e) {
		    throw e;
		}
		break;
	    }
	}

	reader.close();
	reader = doc.getContentAsEventReader();
	// test endElement
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() == XmlEventReader.EndElement) {
		try {
		    assertEquals(reader.getLocalName(), "c_node");
		} catch (XmlException e) {
		    throw e;
		}
		break;
	    }
	}
	reader.close();
    }

    /*
     * Test for getNamespaceURI()
     * 
     */
    @Test
	public void testGetNamespaceURI() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";

	doc.setName(docName);
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	//test not StartElement
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() != XmlEventReader.StartElement && reader.getEventType() != XmlEventReader.EndElement) {
		try {
		    assertEquals(reader.getNamespaceURI(), null);
		} catch (XmlException e) {
		    throw e;
		}
	    }
	}
	reader.close();

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() == XmlEventReader.StartElement) {
		try {
		    assertEquals(reader.getNamespaceURI(), "http://dbxmltest.test/test");
		} catch (XmlException e) {
		    throw e;
		}
		break;//finish this case
	    }
	}
	reader.close();
    }

    /*
     * Test for getPrefix()
     * 
     */
    @Test
	public void testGetPrefix() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";

	doc.setName(docName);
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// test not StartElement
	while (reader.hasNext()) {
	    if (reader.next() != XmlEventReader.StartElement) {
		assertEquals(reader.getPrefix(), null);
		break;
	    }
	}
	reader.close();

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertEquals(reader.getPrefix(), "old");
		break;
	    }
	}
	reader.close();
    }

    /*
     * Test for getSystemId()
     * 
     */
    @Test
	public void testGetSystemId() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	// always be null
	assertNull(reader.getSystemId());
	reader.close();
    }

    /*
     * Test for getValue()
     * 
     */
    @Test
	public void testGetValue() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	    + "><!-- comment text --><b_node/><?audio-output beep?>"
	    + "<c_node>Other text</c_node><d_node><![CDATA[ text < or > or &]]></d_node></old:a_node>";
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	while (reader.hasNext()) {

	    // for StartDocument,EndDocument,StartElement,EndElement throw
	    // exception
	    if (reader.getEventType() == XmlEventReader.StartDocument) {
		try {
		    reader.getValue();
		    fail("Fail in testGetValue()");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }

	    if (reader.getEventType() == XmlEventReader.EndDocument) {
		try {
		    reader.getValue();
		    fail("Fail in testGetValue()");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }

	    if (reader.getEventType() == XmlEventReader.StartElement) {
		try {
		    reader.getValue();
		    fail("Fail in testGetValue()");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }

	    if (reader.getEventType() == XmlEventReader.EndElement) {
		try {
		    reader.getValue();
		    fail("Fail in testGetValue()");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }

	    if (reader.getEventType() == XmlEventReader.Comment) {
		try {
		    assertEquals(reader.getValue(), " comment text ");
		} catch (XmlException e) {
		    throw e;
		}
	    }

	    if (reader.getEventType() == XmlEventReader.CDATA) {
		try {
		    assertEquals(reader.getValue(), " text < or > or &");
		} catch (XmlException e) {
		    throw e;
		}
	    }

	    if (reader.getEventType() == XmlEventReader.Characters) {
		try {
		    assertEquals(reader.getValue(), "Other text");
		} catch (XmlException e) {
		    throw e;
		}
	    }

	    if (reader.getEventType() == XmlEventReader.ProcessingInstruction) {
		try {
		    assertEquals(reader.getValue(), "beep");
		} catch (XmlException e) {
		    throw e;
		}
	    }
	    reader.next();
	}
	reader.close();
    }

    /*
     * Test for getVersion()
     * 
     */
    @Test
	public void testGetVersion() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	assertEquals(reader.getVersion(), "1.0");
	reader.close();

	//test no version set
	doc = mgr.createDocument();
	doc.setContent("<root><a/></root>");
	reader = doc.getContentAsEventReader();
	assertEquals(reader.getVersion(), null);
	reader.close();
    }

    /*
     * Test for hasEmptyElementInfo()
     * 
     */
    @Test
	public void testHasEmptyElementInfo() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	assertTrue(reader.hasEmptyElementInfo());
	reader.close();
    }

    /*
     * Test for hasNext()
     * 
     */
    @Test
	public void testHasNext() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();
	assertTrue(reader.hasNext());
	while (reader.hasNext()) {
	    reader.next();
	}
	assertFalse(reader.hasNext());
	reader.close();
    }

    /*
     * Test for isAttributeSpecified()
     * 
     */
    @Test
	public void testIsAttributeSpecified() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";
	doc.setName(docName);
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// Test is not StartElement
	while(reader.hasNext()){
	    reader.next();
	    if(reader.getEventType() != XmlEventReader.StartElement){
		try {
		    reader.isAttributeSpecified(0);
		    fail("isAttributeSpecified succeeded when it should have failed.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }	
	}
	reader.close();

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    if (reader.next() == XmlEventReader.StartElement) {
		assertTrue(reader.isAttributeSpecified(0));
		assertTrue(reader.isAttributeSpecified(1));
		assertTrue(reader.isAttributeSpecified(2));

		try {
		    assertTrue(reader.isAttributeSpecified(3));
		    reader.close();
		    fail("isAttributeSpecified succeeded when it should have failed.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}

		break;
	    }
	}

	reader.close();
    }

    /*
     * Test for isEmptyElement()
     * 
     */
    @Test
	public void testIsEmptyElement() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" "
	    + "atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
	    + "<b_node/><?audio-output beep?><c_node>Other text"
	    + "</c_node><d_node><![CDATA[ text < or > or &]]></d_node>"
	    + "</old:a_node>";

	doc.setName(docName);
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// Test is not StartElement
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.getEventType() != XmlEventReader.StartElement) {
		try {
		    reader.isEmptyElement();
		    reader.close();
		    fail("isEmptyElement succeeded where it should have failed.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
		}
	    }
	}
	reader.close();

	reader = doc.getContentAsEventReader();
	while (reader.hasNext()) {
	    reader.next();
	    if (reader.next() == XmlEventReader.StartElement) {
		assertFalse(reader.isEmptyElement());

		// come to an empty element

		if (reader.getLocalName().equalsIgnoreCase("b_node"))
		    assertTrue(reader.isEmptyElement());

		break;
	    }
	}
	reader.close();
    }

    /*
     * Test for isStandalone()
     * 
     */
    @Test
	public void testIsStandalone() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	// Test standalone is not set
	try {
	    assertFalse(reader.isStandalone());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    reader.close();
	}

	XmlDocument doc2 = mgr.createDocument();
	doc2.setName(docName);
	String str = "<?xml version=\"1.0\" standalone=\"yes\"?><root></root>";
	doc2.setContent(str);
	XmlEventReader reader2 = doc2.getContentAsEventReader();

	try {
	    assertTrue(reader2.isStandalone());
	    reader2.next();
	    assertTrue(reader2.isStandalone());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    reader2.close();
	}

	// test standalone no
	XmlDocument doc3 = mgr.createDocument();
	doc3.setName(docName);
	String str1 = "<?xml version=\"1.0\" standalone=\"no\"?><root></root>";
	doc3.setContent(str1);
	XmlEventReader reader3 = doc3.getContentAsEventReader();

	try {
	    assertFalse(reader3.isStandalone());
	    reader3.next();
	    assertFalse(reader3.isStandalone());
	} catch (XmlException e) {
	    throw e;
	}

	// test if not StartDocument
	reader3.next();
	try {
	    reader3.isStandalone();
	    fail("isStandalone succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader3.close();
	}
    }

    /*
     * Test for isWhiteSpace()
     * 
     */
    @Test
	public void testIsWhiteSpace() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	String str = "<root><a> </a></root>";
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// test is not proper event type
	try {
	    reader.isWhiteSpace();
	    reader.close();
	    fail("isWhiteSpace succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}
	// StartDocument
	reader.next();

	try {
	    reader.isWhiteSpace();
	    reader.close();
	    fail("isWhiteSpace succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}

	// in <root> StartElement
	reader.next();
	// in <a> StartElement
	reader.next();
	// in white space
	reader.next();

	try {
	    assertTrue(reader.isWhiteSpace());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    reader.close();
	}
    }

    /*
     * Test for needsEntityEscape()
     * 
     */
    @Ignore("later")
	@Test
	public void testNeedsEntityEscape() throws Throwable {

    }

    /*
     * Test for needsEntityEscape(int index)
     * 
     */
    @Ignore("later")
	@Test
	public void testNeedsEntityEscape_int() throws Throwable {

    }

    /*
     * Test for next()
     * 
     */
    @Test
	public void testNext() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	String str = "<root><a/><b><!--comment--><![CDATA[ text < or > "
	    + "or &]]><?audio-output beep?></b>  </root>";
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.StartDocument);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.StartElement);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.StartElement);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.StartElement);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.Comment);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.CDATA);
	reader.next();
	assertEquals(reader.getEventType(),
		     XmlEventReader.ProcessingInstruction);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.EndElement);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.Characters);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.EndElement);
	reader.next();
	assertEquals(reader.getEventType(), XmlEventReader.EndDocument);

	// go to the end throw exception
	try {
	    reader.next();
	    fail("next succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}
    }

    /*
     * Test for nextTag()
     * 
     */
    @Test
	public void testNextTag() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	String str = "<root><a/><b><!--comment--><![CDATA[ text < or > "
	    + "or &]]><?audio-output beep?></b>  </root>";
	doc.setContent(str);
	XmlEventReader reader = doc.getContentAsEventReader();

	// StartDocument
	reader.nextTag();
	reader.nextTag();
	assertEquals(reader.getLocalName(), "root");

	reader.nextTag();
	assertEquals(reader.getLocalName(), "a");

	reader.nextTag();
	assertEquals(reader.getLocalName(), "b");

	reader.nextTag();
	assertEquals(reader.getLocalName(), "b");

	reader.nextTag();
	assertEquals(reader.getLocalName(), "root");

	// EndDocument
	reader.nextTag();

	// after the document, throws an exception
	try {
	    reader.nextTag();
	    fail("nextTag succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader.close();
	}
    }

    /*
     * Test for standaloneSet()
     * 
     */
    @Test
	public void testStandaloneSet() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	XmlEventReader reader = doc.getContentAsEventReader();

	// Test standalone is not set
	try {
	    assertFalse(reader.standaloneSet());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    reader.close();
	}

	XmlDocument doc2 = mgr.createDocument();
	doc2.setName(docName);
	String str = "<?xml version=\"1.0\" standalone=\"yes\"?><root></root>";
	doc2.setContent(str);
	XmlEventReader reader2 = doc2.getContentAsEventReader();

	try {
	    assertTrue(reader2.standaloneSet());
	    reader2.next();
	    assertTrue(reader2.standaloneSet());
	} catch (XmlException e) {
	    throw e;
	}

	// test if not StartDocument
	reader2.next();
	try {
	    reader2.standaloneSet();
	    fail("Fail in testStandaloneSet()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    reader2.close();
	}
    }
}
