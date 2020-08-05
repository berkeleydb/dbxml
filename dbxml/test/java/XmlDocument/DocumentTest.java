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

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlDocumentConfig;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlMetaData;
import com.sleepycat.dbxml.XmlMetaDataIterator;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlValue;

public class DocumentTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    // Container name;
    private static final String CON_NAME = "testData.dbxml";

    private static String docString = "<old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";

    private static String docName = "testDoc.xml";

    @BeforeClass
    public static void setupClass() {
	System.out.println("Begin test XmlDocument!");
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
	System.out.println("Finished test XmlDocument!");
    }

    /*
     * Test for XmlDocument(XmlDocument o)
     * 
     */

    @Test
    public void testConstructor() throws Throwable {
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	try {
	    XmlDocument doc2 = new XmlDocument(doc);
	    assertNotNull(doc2);
	    assertEquals(doc2.getName(), docName);
	    assertEquals(doc2.getContentAsString(), docString);
	} catch (XmlException e) {
	    throw e;
	}

	// test null
	try {
	    XmlDocument doc2 = new XmlDocument(null);
	    fail("Fail in testConstructor()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	} catch (NullPointerException e) {
	    assertNull(e.getMessage());
	}

    }
    /*
     * Test for fetchAllData()
     * 
     */
    @Test
    public void testFetchAllData() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	
	//nulls
	doc.fetchAllData();
	
	doc.setName(docName);
	doc.setContent(docString);
	byte[] arr;
	// test stand-alone XmlDocument
	try {
	    doc.fetchAllData();
	    arr = doc.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	    assertEquals(doc.getName(), docName);
	} catch (XmlException e) {
	    throw e;
	}

	// test the content be put into the container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.fetchAllData();
	    arr = doc.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	    assertEquals(doc.getName(), docName);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	if (hp.isTransactional())
	    doc2 = cont.getDocument(txn, docName);
	else
	    doc2 = cont.getDocument(docName);
	try {
	    doc2.fetchAllData();
	    arr = doc2.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	    assertEquals(doc2.getName(), docName);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after remove the document form the container
	XmlDocument doc3 = null;
	if (hp.isTransactional())
	    doc3 = cont.getDocument(txn, docName);
	else
	    doc3 = cont.getDocument(docName);
	try{
	    doc3.fetchAllData();
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc3, context);
	    else
		cont.deleteDocument(doc3, context);
	    arr = doc3.getContent();
	    assertEquals(arr.length, 0);
	    assertEquals(doc3.getName(), docName);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test empty content document
	doc = mgr.createDocument();
	doc.setName(docName);
	try {
	    // Test if there are no content
	    doc.fetchAllData();
	    arr = doc.getContent();
	    assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getContent()
     * 
     */
    @Test
    public void testGetContent() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	
	byte[] emptyByte = doc.getContent();
	assertEquals(0, emptyByte.length);
	
	doc.setName(docName);
	doc.setContent(docString);
	// byte array hold the content
	byte[] arr;

	// test stand-alone XmlDocument
	try {
	    arr = doc.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    throw e;
	}

	// test the content be put into the container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    arr = doc.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    arr = doc2.getContent();
	    assertEquals(arr.length, docString.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after remove the document form the container
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    arr = doc2.getContent();
	    assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//test on constructed Xml
	String query1 = "<Players><player><League>1</League></player></Players>";
	XmlValue val = null;
	XmlResults re = null;

	try {
	    if (hp.isTransactional())
		re = mgr.query(txn, query1, mgr.createQueryContext());
	    else
		re = mgr.query(query1, mgr.createQueryContext());
	    val = re.next();
	    doc = val.asDocument();
	    arr = doc.getContent();
	    assertEquals(arr.length, query1.getBytes().length);
	    // assert the content are the same
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], query1.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	}

	// test empty content document
	doc = mgr.createDocument();
	doc.setName(docName);
	try {
	    // Test if there are no content
	    arr = doc.getContent();
	    assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getContentAsEventReader()
     * 
     */
    @Test
    public void testGetContentAsEventReader() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	
	XmlEventReader reader = null;
	try {
		reader = doc.getContentAsEventReader();
		fail("Failure in DocumentTest.getContentAsEventReader");
	} catch (XmlException e) {
	} finally {
	    if (reader != null)
		reader.close();
	    reader = null;
	}
	
	doc.setName(docName);
	doc.setContent(docString);
	// test stand-alone XmlDocument
	try {
	    reader = doc.getContentAsEventReader();
	    reader.next();
	    int type = reader.next();

	    if (type == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeCount(), 3);
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (reader != null)
		reader.close();
	    reader = null;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    reader = doc.getContentAsEventReader();
	    reader.next();
	    int type = reader.next();
	    if (type == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeCount(), 3);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (reader != null)
		reader.close();
	    reader = null;
	}

	// test get the document out of the container
	XmlDocument doc2 = null; 
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    reader = doc2.getContentAsEventReader();
	    assertNotNull(reader);
	    reader.next();
	    int type = reader.next();
	    if (type == XmlEventReader.StartElement) {
		assertEquals(reader.getAttributeCount(), 3);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (reader != null)
		reader.close();
	    reader = null;
	}

	//test on constructed Xml
	String query1 = "<Players><player><League>1</League></player></Players>";
	XmlValue val = null;
	XmlResults re = null;

	try {
	    if (hp.isTransactional())
		re = mgr.query(txn, query1, mgr.createQueryContext());
	    else
		re = mgr.query(query1, mgr.createQueryContext());
	    val = re.next();
	    doc = val.asDocument();
	    reader = doc.getContentAsEventReader();
	    assertNotNull(reader);
	    reader.next();
	    int type = reader.next();
	    assertEquals(type, XmlEventReader.StartElement);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	    if (reader != null)
		reader.close();
	    reader = null;
	}

	// test after remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    try{
	    	reader = doc2.getContentAsEventReader();
	    	fail("Failure in DocumentTest.testGetContentAsEventReader()");
	    } catch (XmlException e) {
	    	assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (reader != null)
		reader.close();
	    reader = null;
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getContentAsEventWriter(XmlEventWriter writer)
     * 
     */
    @Test
    public void testGetContentAsEventWriter() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent("<root>test</root>");
	// test stand-alone XmlDocument
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlDocument docWriter = mgr.createDocument();
	docWriter.setName("writer");
	docWriter.setContent("<a>test</a>");
	XmlEventWriter writer = null;
	try {
	    if (hp.isTransactional() || hp.isCDS()) {
		if (hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		writer = cont.putDocumentAsEventWriter(txn, docWriter, context);
	    } else
		writer = cont.putDocumentAsEventWriter(docWriter, context);
	    doc.getContentAsEventWriter(writer);
	    XmlDocument temp = null;
	    if (hp.isTransactional() || hp.isCDS())
		temp = cont.getDocument(txn, "writer");
	    else
		temp = cont.getDocument("writer");
	    assertEquals(temp.getContentAsString(), "<root>test</root>");
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	} finally {
	    if (writer != null) writer.close();
	    writer = null;
	}

	// test after the document be put into container
	XmlDocument docWriter2 = mgr.createDocument();
	docWriter2.setName("writer2");
	docWriter2.setContent("<a>test</a>");
	try {
	    if (hp.isTransactional() || hp.isCDS()) {
		cont.putDocument(txn, doc, context);
		writer = cont.putDocumentAsEventWriter(txn, docWriter2, context);
	    } else {
		cont.putDocument(doc, context);
		writer = cont.putDocumentAsEventWriter(docWriter2, context);
	    }
	    doc.getContentAsEventWriter(writer);
	    XmlDocument temp = null;
	    if (hp.isTransactional() || hp.isCDS())
		temp = cont.getDocument(txn, "writer2");
	    else
		temp = cont.getDocument("writer2");
	    assertEquals(temp.getContentAsString(), "<root>test</root>");
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	} finally {
	    if (writer != null) writer.close();
	    writer = null;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	XmlDocument docWriter3 = mgr.createDocument();
	docWriter3.setName("writer3");
	docWriter3.setContent("<a>test</a>"); 
	try {
	    if (hp.isTransactional() || hp.isCDS()) {
		doc2 = cont.getDocument(txn, docName);
		writer = cont.putDocumentAsEventWriter(txn, docWriter3, context);
	    } else {
		doc2 = cont.getDocument(docName);
		writer = cont.putDocumentAsEventWriter(docWriter3, context);
	    }
	    doc2.getContentAsEventWriter(writer);
	    XmlDocument temp = null;
	    if (hp.isTransactional() || hp.isCDS())
		temp = cont.getDocument(txn, "writer3");
	    else
		temp = cont.getDocument("writer3");
	    assertEquals(temp.getContentAsString(), "<root>test</root>");
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	} finally {
	    if (writer != null) writer.close();
	    writer = null;
	    if (txn != null) txn.commit();
	}
    }

    /*
     * Test for getContentAsInputStream()
     * 
     */
    @Test
    public void testGetContentAsInputStream() throws Throwable {
	//if (hp.isTransactional() && !hp.isNodeContainer()) return;
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	InputStream is = null;
	BufferedReader in = null;
	StringBuffer buffer = null;
	String line = "";
	
	try {
	    is = doc.getContentAsInputStream();
	    in = new BufferedReader(new InputStreamReader(is));
	    buffer = new StringBuffer();
	    while ((line = in.readLine()) != null) {
		buffer.append(line);
	    }
	    assertEquals(buffer.toString(), "");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (in != null) in.close();
	    in = null;
	}
	
	doc.setName(docName);
	doc.setContent(docString);
	// test stand-alone XmlDocument
	try {
	    is = doc.getContentAsInputStream();
	    in = new BufferedReader(new InputStreamReader(is));
	    buffer = new StringBuffer();
	    while ((line = in.readLine()) != null) {
		buffer.append(line);
	    }
	    assertEquals(buffer.toString(), docString);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (in != null) in.close();
	    in = null;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    is = doc.getContentAsInputStream();
	    in = new BufferedReader(new InputStreamReader(is));
	    buffer = new StringBuffer();
	    line = "";
	    while ((line = in.readLine()) != null) {  //hangs here
		buffer.append(line);
	    }
	    assertEquals(buffer.toString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (in != null) in.close();
	    in = null;
	}


	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    is = doc2.getContentAsInputStream();
	    in = new BufferedReader(new InputStreamReader(is));
	    buffer = new StringBuffer();
	    line = "";
	    while ((line = in.readLine()) != null) {
		buffer.append(line);
	    }
	    assertEquals(buffer.toString(), docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (in != null) in.close();
	    in = null;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc, context);
	    else
		cont.deleteDocument(doc, context);
	    is = doc.getContentAsInputStream();
	    assertEquals(is.available(), 0);
	    is.close();
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.close();
	    is = null;
	}

	// test empty document
	XmlDocument empty = mgr.createDocument();
	try{
		is = empty.getContentAsInputStream();
		BufferedInputStream bs = new BufferedInputStream(is);
		assertEquals(bs.available(), 0);
		bs.close();
	} catch (XmlException e) {
		throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getContentAsString()
     * 
     */
    @Test
    public void testGetContentAsString() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	assertEquals("", doc.getContentAsString());
	
	String nullStr = null;
	doc.setContent(nullStr);
	assertEquals("", doc.getContentAsString());
	
	byte[] bytes = null;
	doc.setContent(bytes);
	assertEquals("", doc.getContentAsString());
	
	doc.setName(docName);
	doc.setContent(docString);
	// test stand-alone XmlDocument
		String content = doc.getContentAsString();
	    assertNotNull(content);
	    assertEquals(content, docString);

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    content = doc.getContentAsString();
	    assertNotNull(content);
	    assertEquals(content, docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    content = doc2.getContentAsString();
	    assertNotNull(content);
	    assertEquals(content, docString);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    content = doc2.getContentAsString();
	    assertNotNull(content);
	    assertEquals(content, "");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//test constructed Xml
	String query1 = "<Players><player><League>1</League></player></Players>";
	XmlValue val = null;
	XmlResults re = null;

	try {
	    if (hp.isTransactional())
		re = mgr.query(txn, query1, mgr.createQueryContext());
	    else
		re = mgr.query(query1, mgr.createQueryContext());
	    val = re.next();
	    doc = val.asDocument();
	    String str = doc.getContentAsString();
	    assertEquals(str, query1);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	}

	// test empty
	XmlDocument empty = mgr.createDocument();
	try{
	    String str = empty.getContentAsString();
	    assertEquals(str.length(), 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getContentAsXmlInputStream()
     * 
     */
    @Test
    public void testGetContentAsXmlInputStream() throws Throwable {
	//if (hp.isTransactional() && !hp.isNodeContainer()) return;
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	doc.setName(docName);
	doc.setContent(docString);
	byte[] toFill = new byte[docString.getBytes().length];
	XmlInputStream is = null;
	// test stand-alone document
	try {
	    is = doc.getContentAsXmlInputStream();
	    assertNotNull(is);
	    assertEquals(is.curPos(), 0);
	    is.readBytes(toFill, docString.getBytes().length);
	    for (int i = 0; i < toFill.length; i++) {
		assertEquals(toFill[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}
	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    is = doc.getContentAsXmlInputStream();
	    assertNotNull(is);
	    assertEquals(is.curPos(), 0);
	    is.readBytes(toFill, docString.getBytes().length);  //hangs here
	    for (int i = 0; i < toFill.length; i++) {
		assertEquals(toFill[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    is = doc2.getContentAsXmlInputStream();
	    assertNotNull(is);
	    assertEquals(is.curPos(), 0);
	    is.readBytes(toFill, docString.getBytes().length);
	    for (int i = 0; i < toFill.length; i++) {
		assertEquals(toFill[i], docString.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc, context);
	    else
		cont.deleteDocument(doc, context);
	    is = doc.getContentAsXmlInputStream();
	    assertEquals(0, is.readBytes(toFill, 1));
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (is != null) is.delete();
	    is = null;
	}

	//test constructed Xml
	String query1 = "<Players><player><League>1</League></player></Players>";
	XmlValue val = null;
	XmlResults re = null;

	try {
	    if (hp.isTransactional())
		re = mgr.query(txn, query1, mgr.createQueryContext());
	    else
		re = mgr.query(query1, mgr.createQueryContext());
	    val = re.next();
	    doc = val.asDocument();
	    is = doc.getContentAsXmlInputStream();
	    assertNotNull(is);
	    assertEquals(is.curPos(), 0);
	    is.readBytes(toFill, query1.getBytes().length);
	    for (int i = 0; i < query1.getBytes().length; i++) {
		assertEquals(toFill[i], query1.getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	} finally {
	    if (re != null) re.delete();
	    if (is != null) is.delete();
	    is = null;
	}

	//test empty
	XmlDocument empty = mgr.createDocument();
	XmlInputStream xis = null;
	try {
    	xis = empty.getContentAsXmlInputStream();
    	assertEquals(0, xis.readBytes(toFill, 1));
    } catch (XmlException e) {
    	throw e;
    } finally {
	    if (xis != null)xis.delete();
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for boolean getMetaData(String uri, String name, XmlValue value)
     * 
     */
    @Test
    public void testGetMetaData_bool() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	XmlValue value = new XmlValue();
	assertFalse(doc.getMetaData("http://dbxmltest.test", "testMetadata",
		    value));
	
	doc.setName(docName);
	doc.setContent(docString);
	XmlValue va = new XmlValue("Test");
	doc.setMetaData("http://dbxmltest.test", "testMetadata", va);
	// test stand-alone document
	try {
	    assertTrue(doc.getMetaData("http://dbxmltest.test", "testMetadata",
		    value));
	    assertEquals(value.asString(), "Test");
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    assertTrue(doc.getMetaData("http://dbxmltest.test", "testMetadata",
		    value));
	    assertEquals(value.asString(), "Test");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    assertTrue(doc2.getMetaData("http://dbxmltest.test",
		    "testMetadata", value));
	    assertEquals(value.asString(), "Test");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    assertFalse(doc2.getMetaData("http://dbxmltest.test",
		    "testMetadata", value));
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for byte[] getMetaData(String uri,String name)
     * 
     */
    @Test
    public void testGetMetaData_byte() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	assertNull(doc.getMetaData("http://dbxmltest.test", "test"));
	
	doc.setName(docName);
	doc.setContent(docString);
	byte[] arr;
	doc.setMetaData("http://dbxmltest.test", "testMetadata", "Test"
		.getBytes());
	// test stand-alone document
	try {
	    assertNull(doc.getMetaData("http://dbxmltest.test", "test"));
	    arr = doc.getMetaData("http://dbxmltest.test", "testMetadata");
	    assertNotNull(arr);
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    assertNull(doc.getMetaData("http://dbxmltest.test", "test"));
	    arr = doc.getMetaData("http://dbxmltest.test", "testMetadata");
	    assertNotNull(arr);
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    assertNull(doc2.getMetaData("http://dbxmltest.test", "test"));
	    arr = doc2.getMetaData("http://dbxmltest.test", "testMetadata");
	    assertNotNull(arr);
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    arr = doc2.getMetaData("http://dbxmltest.test", "testMetadata");
	    assertNull(arr);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getMetaDataIterator()
     * 
     */
    @Test
    public void testGetMetaDataIterator() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	XmlMetaDataIterator it = doc.getMetaDataIterator();
	assertNull(it.next());
	
	doc.setName(docName);
	doc.setContent(docString);
	it = null;
	doc.setMetaData("http://dbxmltest.test", "testMetadata_1",
		new XmlValue("Test_1"));
	doc.setMetaData("http://dbxmltest.test", "testMetadata_2",
		new XmlValue("Test_2"));
	doc.setMetaData("http://dbxmltest.test", "testMetadata_3",
		new XmlValue("Test_3"));
	// test stand-alone document
	try {
	    it = doc.getMetaDataIterator();
	    XmlMetaData md = it.next();
	    assertEquals(md.get_name(), "name");
	    assertEquals(md.get_uri(), "http://www.sleepycat.com/2002/dbxml");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_1");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_1");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_2");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_2");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_3");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_3");

	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    it = doc.getMetaDataIterator();
	    XmlMetaData md = it.next();
	    assertEquals(md.get_name(), "name");
	    assertEquals(md.get_uri(), "http://www.sleepycat.com/2002/dbxml");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_1");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_1");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_2");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_2");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_3");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_3");

	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    it = doc2.getMetaDataIterator();
	    XmlMetaData md = it.next();
	    assertEquals(md.get_name(), "name");
	    assertEquals(md.get_uri(), "http://www.sleepycat.com/2002/dbxml");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_1");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_1");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_2");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_2");

	    md = it.next();
	    assertEquals(md.get_name(), "testMetadata_3");
	    assertEquals(md.get_uri(), "http://dbxmltest.test");
	    assertEquals(md.get_value().asString(), "Test_3");

	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    it = doc2.getMetaDataIterator();
	    XmlMetaData md = it.next();
	    assertEquals(md.get_name(), "name");
	    assertEquals(md.get_uri(), "http://www.sleepycat.com/2002/dbxml");
	    assertNull(it.next());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for removeMetaData()
     * 
     */
    @Test
    public void testRemoveMetaData() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	doc.removeMetaData("http://dbxmltest.test", "testMetadata");
	
	doc.setName(docName);
	doc.setContent(docString);
	boolean bool;
	XmlValue value = new XmlValue();
	doc.setMetaData("http://dbxmltest.test", "testMetadata", new XmlValue(
	"Test"));
	// test stand-alone document
	try {
	    // remove metadata not exists
	    doc.removeMetaData("a", "b");
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata",
		    value);
	    assertTrue(bool);
	    doc.removeMetaData("http://dbxmltest.test", "testMetadata");
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata",
		    value);
	    // after remove
	    assertFalse(bool);
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setMetaData("http://dbxmltest.test", "testMetadata2", new XmlValue(
	    "Test"));
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata2",
		    value);
	    assertTrue(bool);
	    doc.removeMetaData("http://dbxmltest.test", "testMetadata2");
	    // after remove
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata2",
		    value);
	    assertFalse(bool);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    // remove metadata not exists
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setMetaData("http://dbxmltest.test", "testMetadata3", new XmlValue(
	    "Test"));
	    doc2.removeMetaData("a", "b");
	    bool = doc2.getMetaData("http://dbxmltest.test", "testMetadata3",
		    value);
	    assertTrue(bool);
	    doc2.removeMetaData("http://dbxmltest.test", "testMetadata3");
	    bool = doc2.getMetaData("http://dbxmltest.test", "testMetadata3",
		    value);
	    // after remove
	    assertFalse(bool);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	doc.setMetaData("http://dbxmltest.test", "testMetadata4", new XmlValue(
	"Test"));
	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc, context);
	    else
		cont.deleteDocument(doc, context);
	    // remove metadata not exists
	    doc.removeMetaData("a", "b");

	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata4",
		    value);
	    assertTrue(bool);
	    doc.removeMetaData("http://dbxmltest.test", "testMetadata4");
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata4",
		    value);

	    // after remove
	    assertFalse(bool);
	    doc.removeMetaData("http://dbxmltest.test", "testMetadata4");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setContent(String content)
     * 
     */

    @Test
    public void testSetContent_str() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	String nullStr = null;
	doc.setContent(nullStr);
	assertEquals("", doc.getContentAsString());
	
	doc.setName(docName);
	doc.setContent(docString);
	// test stand-alone document
	    doc.setContent("<a>test</a>");
	    assertEquals(doc.getContentAsString(), "<a>test</a>");

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setContent("<b>test</b>");
	    assertEquals(doc.getContentAsString(), "<b>test</b>");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setContent("<c>test</c>");
	    assertEquals(doc2.getContentAsString(), "<c>test</c>");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    assertEquals(doc2.getContentAsString(), "");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setContent(byte[] content)
     * 
     */
    @Test
    public void testSetContent_byte() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	byte[] nullBytes = null;
	doc.setContent(nullBytes);
	assertEquals(0, doc.getContent().length);
	
	doc.setName(docName);
	// test set null content
	byte[] content = null;
	    doc.setContent(content);

	// test stand-alone document
	    doc.setContent(docString.getBytes());
	    assertEquals(doc.getContentAsString(), docString);
	    doc.setContent("".getBytes());

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setContent(docString.getBytes());
	    assertEquals(doc.getContentAsString(), docString);
	    doc.setContent("".getBytes());
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setContent(docString.getBytes());
	    assertEquals(doc2.getContentAsString(), docString);
	    doc2.setContent("".getBytes());
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    assertEquals(doc2.getContentAsString(), "");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setContentAsEventReader()
     * 
     */
    @Test
    public void testSetContentAsEventReader() throws Throwable {
	//if (hp.isTransactional() && !hp.isNodeContainer()) return;
	// GMF
    XmlDocument doc = mgr.createDocument();
    XmlEventReader reader = null;
    try {
    	doc.setContentAsEventReader(reader);
    	fail("Failure in DocumentTest.testSetContentAsEventReader().");
    } catch (NullPointerException e) {}
	XmlTransaction txn = null;
	if (hp.isTransactional())
	    txn = mgr.createTransaction();
	if (hp.isCDS())
	    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	// create a stand-alone XmlDocument
	doc.setName(docName);
	doc.setContent(docString);
	// create a container and put a document in
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlDocument newdoc = null;
	// test stand-alone document
	// docTest is only used to check valid content
	XmlDocument docTest = mgr.createDocument();
	try {
	    if (txn != null) {
		cont.putDocument(txn, doc, context);
		newdoc = cont.getDocument(txn, docName);
	    } else {
		cont.putDocument(doc, context);
		newdoc = cont.getDocument(docName);
	    }
	    // verify the EventReader content matches original
	    reader = newdoc.getContentAsEventReader();
	    docTest.setContentAsEventReader(reader);
	    assertEquals(docString, docTest.getContentAsString()); //hangs here
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	}

	// put the document as an event reader
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName("doc2");
	// grab content from newdoc (again)
	reader = newdoc.getContentAsEventReader();
	doc2.setContentAsEventReader(reader);

	try {
	    if (txn != null) {
		cont.putDocument(txn, doc2, context);
		doc2 = cont.getDocument(txn, "doc2");
	    } else
		cont.putDocument(doc2, context);
	    assertEquals(docString, doc2.getContentAsString());
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc3 = mgr.createDocument();
	doc3.setName("doc3");
	doc3.setContent(docString);
	XmlDocument doc4 = null;
	// re-fetch newdoc content
	reader = newdoc.getContentAsEventReader();
	try {
	    if (txn != null) {
		cont.putDocument(txn, doc3, context);
		doc4 = cont.getDocument(txn, "doc3");
	    } else {
		cont.putDocument(doc3, context);
		doc4 = cont.getDocument("doc3");
	    }
	    // overwrite doc4's content (it'll be transient)
	    doc4.setContentAsEventReader(reader);
	    assertEquals(docString, doc4.getContentAsString());
	} catch (XmlException e) {
	    if (txn != null) txn.commit();
	    throw e;
	}

	// test removing the source event reader document
	XmlDocument doc5 = mgr.createDocument();
	doc5.setName("doc5");
	try {
	    // delete the reader document (docName)
	    if (txn != null) {
		newdoc = cont.getDocument(txn, docName);
		reader = newdoc.getContentAsEventReader();
		doc5.setContentAsEventReader(reader);
		cont.deleteDocument(txn, docName, context);
		cont.putDocument(txn, doc5, context);
	    } else {
		newdoc = cont.getDocument(docName);
		reader = newdoc.getContentAsEventReader();
		doc5.setContentAsEventReader(reader);
		cont.deleteDocument(docName, context);
		cont.putDocument(doc5, context);
	    }
	    assertEquals(docString, doc5.getContentAsString());
	    // "clean" exit
	    if (txn != null) {
		txn.commit();
		txn = null;
	    }

	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (txn != null) txn.commit();
	}
    }

    /*
     * Test for setContentAsXmlInputStream()
     * 
     */

    @Test
    public void testSetContentAsXmlInputStream() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	XmlInputStream ins = null;
	try {
    	doc.setContentAsXmlInputStream(ins);
    	fail("Failure in DocumentTest.testSetContentAsXmlInputStream().");
    } catch (NullPointerException e) {}
	
	doc.setName(docName);
	doc.setContent(docString);
	ByteArrayInputStream stream = new ByteArrayInputStream(docString
		.getBytes());
	ins = mgr.createInputStream(stream);
	// test stand-alone document
	try {
	    doc.setContentAsXmlInputStream(ins);
	    assertEquals(docString, doc.getContentAsString());
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	stream = new ByteArrayInputStream("<a>test</a>"
		.getBytes());
	ins = mgr.createInputStream(stream);
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setContentAsXmlInputStream(ins);
	    assertEquals("<a>test</a>", doc.getContentAsString());	  
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	stream = new ByteArrayInputStream("<b>test</b>"
		.getBytes());
	ins = mgr.createInputStream(stream);
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setContentAsXmlInputStream(ins);
	    assertEquals("<b>test</b>", doc2.getContentAsString());
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	stream = new ByteArrayInputStream("<c>test</c>"
		.getBytes());
	ins = mgr.createInputStream(stream);
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    doc2.setContentAsXmlInputStream(ins);
	    assertEquals("<c>test</c>", doc2.getContentAsString());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setMetaData(String uri, String name, byte[] value)
     * 
     */
    @Test
    public void testSetMetaData_byte() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	byte[] nullBytes = null;
	doc.setMetaData("http://dbxmltest.test", "nullName", nullBytes);
	assertEquals(0, doc.getMetaData("http://dbxmltest.test", "nullName").length);
	doc.setMetaData(null, null, nullBytes);
	assertEquals(0, doc.getMetaData("", "").length);
	doc = mgr.createDocument();
	
	doc.setName(docName);
	doc.setContent(docString);
	byte[] arr;
	// test stand-alone document
	try {
	    doc.setMetaData("http://dbxmltest.test", "testMetadata", "Test"
		    .getBytes());
	    arr = doc.getMetaData("http://dbxmltest.test", "testMetadata");
	    assertNotNull(arr);
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
        
        //test null byte
        byte[] b = null;
        doc.setMetaData("http://dbxmltest.test", "nullMD", b);
        arr = doc.getMetaData("http://dbxmltest.test","nullMD");
        assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setMetaData("http://dbxmltest.test", "testMetadata2", "Test"
		    .getBytes());
	    arr = doc.getMetaData("http://dbxmltest.test", "testMetadata2");
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
        //test null byte
        byte[] b = null;
        doc.setMetaData("http://dbxmltest.test", "nullMD", b);
        arr = doc.getMetaData("http://dbxmltest.test","nullMD");
        assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setMetaData("http://dbxmltest.test", "testMetadata3", "Test"
		    .getBytes());
	    arr = doc2.getMetaData("http://dbxmltest.test", "testMetadata3");
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
        //test null byte
        byte[] b = null;
        doc.setMetaData("http://dbxmltest.test", "nullMD", b);
        arr = doc.getMetaData("http://dbxmltest.test","nullMD");
        assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc, context);
	    else
		cont.deleteDocument(doc, context);
	    doc.setMetaData("http://dbxmltest.test", "testMetadata3", "Test"
		    .getBytes());
	    arr = doc.getMetaData("http://dbxmltest.test", "testMetadata3");
	    for (int i = 0; i < arr.length; i++) {
		assertEquals(arr[i], "Test".getBytes()[i]);
	    }
        //test null byte
        byte[] b = null;
        doc.setMetaData("http://dbxmltest.test", "nullMD", b);
        arr = doc.getMetaData("http://dbxmltest.test","nullMD");
        assertEquals(arr.length, 0);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setMetaData(String uri, String name, XmlValue value)
     * 
     */

    @Test
    public void testSetMetaData_value() throws Throwable {
	XmlTransaction txn = null;
	// create a stand-alone XmlDocument
	XmlDocument doc = mgr.createDocument();
	XmlValue nullVal = null;
	try {
		doc.setMetaData("http://dbxmltest.test", "nullName", nullVal);
		fail("Failure in DocumentTest.testSetMetaData_value().");
	} catch (NullPointerException e) {}
	doc = mgr.createDocument();
	
	doc.setName(docName);
	doc.setContent(docString);
	boolean bool;
	// test stand-alone document
	XmlValue value = new XmlValue("Test");
	XmlValue va = new XmlValue();

	try {
	    doc.setMetaData("http://dbxmltest.test", "testMetadata", value);
	    bool = doc.getMetaData("http://dbxmltest.test", "testMetadata", va);
	    assertTrue(bool);

	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setMetaData("http://dbxmltest.test", "testMetadata2", value);
	    bool = doc
	    .getMetaData("http://dbxmltest.test", "testMetadata2", va);
	    assertTrue(bool);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setMetaData("http://dbxmltest.test", "testMetadata3", value);
	    bool = doc2.getMetaData("http://dbxmltest.test", "testMetadata3",
		    va);
	    assertTrue(bool);
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    doc2.setMetaData("http://dbxmltest.test", "testMetadata4", value);
	    bool = doc2.getMetaData("http://dbxmltest.test", "testMetadata4", va);
	    assertTrue(bool);
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for setName()
     * 
     */
    @Test
    public void testSetName() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	// set the name to null
	doc.setName(null);
	assertEquals(doc.getName(), "");

	// test stand-alone document
	try {
	    doc.setName(docName);
	    assertEquals(doc.getName(), docName);
	} catch (XmlException e) {
	    throw e;
	}

	// test after the document be put into container
	XmlUpdateContext context = mgr.createUpdateContext();
	try {
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, context);
	    } else
		cont.putDocument(doc, context);
	    doc.setName("name1");
	    assertEquals(doc.getName(), "name1");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test get the document out of the container
	XmlDocument doc2 = null;
	try {
	    if (hp.isTransactional())
		doc2 = cont.getDocument(txn, docName);
	    else
		doc2 = cont.getDocument(docName);
	    doc2.setName("name2");
	    assertEquals(doc2.getName(), "name2");
	} catch (XmlException e) {
	    if (hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	// test after the container remove the document
	doc2.setName(docName);
	try {
	    if (hp.isTransactional())
		cont.deleteDocument(txn, doc2, context);
	    else
		cont.deleteDocument(doc2, context);
	    doc2.setName("name3");
	    assertEquals(doc2.getName(), "name3");
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (hp.isTransactional() && txn != null) txn.commit();
	}
    }
    
    @Test
    public void testGetResults() throws Throwable
    {
    	XmlTransaction txn = null;
    	XmlDocument doc = mgr.createDocument();
    	XmlDocument doc2 = mgr.createDocument();
    	XmlResults re = null;
    	
    	try {
    		assertNull(doc.getResults());
    		doc.setName(docName);
    		doc.setContent(docString);
    		if (hp.isTransactional()) {
    			txn = mgr.createTransaction();
    			cont.putDocument(txn, doc);
    			doc = cont.getDocument(txn, docName);
    		} else {
    			cont.putDocument(doc);
    			doc = cont.getDocument(docName);
    		}
    		assertNull(doc.getResults());
    		if (hp.isTransactional()) 
    			re = cont.getAllDocuments(txn, new XmlDocumentConfig());
    		 else
    			re = cont.getAllDocuments(new XmlDocumentConfig());
    		re.next(doc2);
    		assertEquals(doc2.getResults(), re);
    		try {
    			re.delete();
    			doc2.getContentAsString();
    			fail("Failusre in XmlDocument.testGetResults");
    		} catch (XmlException e) {
    			assertNotNull(e);
    		}
    	} catch (XmlException e) {
    		if (re != null) re.delete();
    	    throw e;
    	} finally {
    	    if (txn != null) txn.commit();
    	}
    }

    /*
     * [#16257] Tests that characters are properly encoded.
     */
    @Test
    public void testSetStringEncoding() throws Throwable
    {
	XmlTransaction txn = null;
	String content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><s>\u017d</s>";
	XmlDocument doc = mgr.createDocument();
	XmlDocument doc2 = null;

	try {
	    doc.setName("test.xml");
	    doc.setContent(content);
	    assertEquals(content, doc.getContentAsString());
	    if (hp.isTransactional()) {
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
		doc2 = cont.getDocument(txn, "test.xml");
	    } else {
		cont.putDocument(doc);
		doc2 = cont.getDocument("test.xml");
	    }
	    assertEquals(content, doc2.getContentAsString());
	} catch (XmlException e) {
	    throw e;
	} finally {
	    if (txn != null) txn.commit();
	}
    }
    
    /*
     * 
 		[#16593] Temporary documents created from input streams or event readers
     */
    @Test
    public void testTemporaryDocuments() throws Throwable
    {
    	//Input stream
    	String content = "<foo/>";
    	XmlDocument doc = mgr.createDocument();
    	XmlInputStream xis = mgr.createMemBufInputStream(content, 6, true);
    	doc.setContentAsXmlInputStream(xis);
    	XmlValue val = new XmlValue(doc);
    	assertEquals(content, val.asString());
    	assertEquals(content, val.asString());
    	doc = mgr.createDocument();
    	xis = mgr.createMemBufInputStream(content, 6, true);
    	doc.setContentAsXmlInputStream(xis);
    	val = new XmlValue(doc);
    	//Calling asEventReader consumes the content.
    	XmlEventReader re = null;
    	try {
    		re = val.asEventReader();
    		re.close();
    		re = null;
    		assertEquals("", val.asString());
    	} catch (XmlException e) {
    		throw e;
    	} finally {
    		if (re != null) re.close();
    	}

    	//Event reader
    	doc = mgr.createDocument();
    	XmlDocument doc2 = mgr.createDocument();
    	doc.setContent(content);
    	XmlEventReader reader = doc.getContentAsEventReader();
    	doc2.setContentAsEventReader(reader);
    	val = new XmlValue(doc2);
    	assertEquals(content, val.asString());
    	assertEquals(content, val.asString());
    	doc2 = mgr.createDocument();
    	reader = doc.getContentAsEventReader();
    	doc2.setContentAsEventReader(reader);
    	val = new XmlValue(doc2);
    	//Calling asEventReader consumes the content.
    	re = null;
    	try {
    		re = val.asEventReader();
    		re.close();
    		re = null;
    		assertEquals("", val.asString());
    	} catch (XmlException e) {
    		throw e;
    	} finally {
    		if (re != null) re.close();
    	}

    	//String
    	doc = mgr.createDocument();
    	doc.setContent(content);
    	val = new XmlValue(doc);
    	assertEquals(content, val.asString());

    	//Bytes
    	doc = mgr.createDocument();
    	doc.setContent(content.getBytes("utf-8"));
    	val = new XmlValue(doc);
    	assertEquals(content, val.asString());
    }
}
