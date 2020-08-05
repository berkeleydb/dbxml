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

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlDocumentConfig;

public class EventWriterTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private final String CON_NAME = "";
    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlEventWriter!");
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
	System.out.println("Finished test XmlEventWriter!");
    }

    /*
     * Test for close()
     * 
     */
    @Test
	public void testClose() throws Throwable {

    }

    /*
     * Test for writeAttribute()
     * 
     */
    @Test
	public void testWriteAttribute() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	doc.setName(docName);

	XmlUpdateContext uc = mgr.createUpdateContext();
	// Get an writer
	XmlEventWriter writer = null;
	try{
	    //Cannot access the document until write.close is called.
	    if(hp.isTransactional() || hp.isCDS()){
		if(hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
		cont.getDocument(txn, docName);
	    }else{
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
		cont.getDocument(docName);
	    }
	    if(txn != null) txn.commit();
	    fail("Cannot access the document until write.close is called.");
	}catch(XmlException e){
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.DOCUMENT_NOT_FOUND);
	}

	// can not write attribute in startDocument
	try {
	    writer.writeStartDocument(null, null, null);
	    writer.writeAttribute("a", null, null, "test", true);
	    if(txn != null) txn.commit();
	    fail("writeAttribute succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// Get a new writer
	config.setGenerateName(true);
	// can not write attribute in set 0 element
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, true);
	    writer.writeAttribute("a", null, null, "test", true);
	    if(txn != null) txn.commit();
	    fail("writeAttribute succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	// Get a new writer
	doc.setName(docName);
	// get the writer work well
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 1, false);
	    writer.writeAttribute("att_a", null, null, "test", true);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, docName);
	    else
		myDoc = cont.getDocument(docName);
	    // should get the document
	    if(hp.isNodeContainer())
		assertEquals(myDoc.getContentAsString(), "<root att_a=\"test\"/>");
	    else
		assertEquals(myDoc.getContentAsString(), "<root att_a=\"test\"></root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeDTD()
     * 
     */
    @Test
	public void testWriteDTD() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);
	//Test writeDTD(String, int)
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	//write before writeDocument
	try {
	    if(hp.isTransactional() || hp.isCDS()){
		if(hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    }else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    if(txn != null) txn.commit();
	    fail("writeDTD succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	//get new writer
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	//write in the content
	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 0, false);
	try {
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    if(txn != null) txn.commit();
	    fail("writeDTD succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	try {
	    doc = mgr.createDocument();
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    //write in the correct position
	    writer.writeStartDocument(null, null, null);
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    writer.writeStartElement("root", null, null, 0, true);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	// get the doc from container
	XmlDocument myDoc = null;
	if(hp.isTransactional() || hp.isCDS())
	    myDoc = cont.getDocument(txn, doc.getName());
	else
	    myDoc = cont.getDocument(doc.getName());
	assertEquals("<!DOCTYPE root [<!ELEMENT root EMPTY>]><root/>", myDoc.getContentAsString());

	//Test writeDTD(String)
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	//write before writeDocument
	try {
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    if(txn != null) txn.commit();
	    fail("writeDTD succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	}
	//get new writer
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	//write in the content
	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 0, false);
	try {
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    if(txn != null) txn.commit();
	    fail("writeDTD succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	try {
	    doc = mgr.createDocument();
	    //get new writer
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    //write in the correct position
	    writer.writeStartDocument(null, null, null);
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root EMPTY>]>");
	    writer.writeStartElement("root", null, null, 0, true);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	try{
	    // get the doc from container
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals("<!DOCTYPE root [<!ELEMENT root EMPTY>]><root/>", myDoc.getContentAsString());
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeEndDocument()
     * 
     */
    @Test
	public void testWriteEndDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);

	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 0, false);

	// wirteEndDocument not in the right position
	try {
	    writer.writeEndDocument();
	    if(txn != null) txn.commit();
	    fail("writeEndDocument succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	try {
	    doc = mgr.createDocument();
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeEndElement()
     * 
     */
    @Test
	public void testWriteEndElement() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);

	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	// write before startDocument
	try {
	    writer.writeEndElement(null, null, null);
	    if(txn != null) txn.commit();
	    fail("writeEndElement succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);
	writer.writeStartDocument(null, null, null);

	// write without startElement
	try {
	    writer.writeEndElement(null, null, null);
	    if(txn != null) txn.commit();
	    fail("writeEndElement succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	//write before all attribute writen
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);
	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 1, false);

	// write without startElement
	try {
	    writer.writeEndElement(null, null, null);
	    if(txn != null) txn.commit();
	    fail("writeEndElement succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// write in an empty element
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);
	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 1, true);

	// write without startElement
	try {
	    writer.writeEndElement(null, null, null);
	    if(txn != null) txn.commit();
	    fail("writeEndElement succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// get new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);

	    //write in the correct position
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    if(hp.isNodeContainer())
		assertEquals(myDoc.getContentAsString(), "<root/>");
	    else
		assertEquals(myDoc.getContentAsString(), "<root></root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeEndEntity()
     * 
     */
    @Test
	public void testWriteEndEntity() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);

	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	//write before startDocument
	try {
	    writer.writeEndEntity("name");
	    if(txn != null) txn.commit();
	    fail("testWriteEndEntity succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	//get new writer
	doc = mgr.createDocument();
	//write in the correct place
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root (#PCDATA)><!ENTITY name \"stuff\">]>");
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeStartEntity("name", false);
	    writer.writeEndEntity("name");
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals(myDoc.getContentAsString(),"<!DOCTYPE root [<!ELEMENT root (#PCDATA)><!ENTITY name \"stuff\">]><root>&name;</root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeProcessingInstruction()
     * 
     */
    @Test
	public void testWriteProcessingInstruction() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);

	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	// write before document
	try {
	    writer.writeProcessingInstruction("target", "doit");
	    if(txn != null) txn.commit();
	    fail("testWriteProcessingInstruction succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	//write before attribute finish
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	writer.writeStartDocument(null, null, null);
	writer.writeStartElement("root", null, null, 1, false);
	try {
	    writer.writeProcessingInstruction("target", "doit");
	    if(txn != null) txn.commit();
	    fail("testWriteProcessingInstruction succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	//get new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 1, false);
	    writer.writeAttribute("att", null, null, "test1", false);
	    writer.writeProcessingInstruction("target", "do");
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	//get the doc
	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals(myDoc.getContentAsString(), "<root><?target do?></root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeStartDocument()
     * 
     */
    @Test
	public void testWriteStartDocument() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);

	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	try {
	    writer.writeStartDocument(null, null, null);
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}

	// write after already writeStartDocument
	try {
	    writer.writeStartDocument(null, null, null);
	    if(txn != null) txn.commit();
	    fail("writeStartDocument succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// get new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument("1.0", "UTF-8", "yes");
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	// get the doc
	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc =  cont.getDocument(txn, doc.getName());
	    else
		myDoc =  cont.getDocument(doc.getName());
	    if(hp.isNodeContainer())
		assertEquals(myDoc.getContentAsString(),
			     "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root/>");
	    else
		assertEquals(myDoc.getContentAsString(),
			     "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root></root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeStartElement()
     * 
     */
    @Test
	public void testWriteStartElement() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	try {
	    if(hp.isTransactional() || hp.isCDS()){
		if(hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    }else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, true);
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}

	// write the same startElement
	try {
	    writer.writeStartElement("root", null, null, 0, true);
	    if(txn != null) txn.commit();
	    fail("writeStartDocument succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// get a new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	// Get the doc from container
	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    if(hp.isNodeContainer())
		assertEquals(myDoc.getContentAsString(), "<root/>");
	    else
		assertEquals(myDoc.getContentAsString(), "<root></root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeStartEntity()
     * 
     */
    @Test
	public void testWriteStartEntity() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	//write before startDocument
	try {
	    writer.writeStartEntity("name", true);
	    if(txn != null) txn.commit();
	    fail("writeStartEntity succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	//get new writer
	doc = mgr.createDocument();
	//write in the correct place
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeDTD("<!DOCTYPE root [<!ELEMENT root (#PCDATA)><!ENTITY name \"stuff\">]>");
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeStartEntity("name", true);
	    writer.writeEndEntity("name");
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	    writer = null;
	}

	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals(myDoc.getContentAsString(),"<!DOCTYPE root [<!ELEMENT root (#PCDATA)><!ENTITY name \"stuff\">]><root>&name;</root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for writeText((int type,String text,int length)
     * 
     */
    @Test
	public void testWriteText() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlDocumentConfig config = new XmlDocumentConfig();
	config.setGenerateName(true);
	//Test writeText(int, String, int)
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlEventWriter writer = null;
	if(hp.isTransactional() || hp.isCDS()){
	    if(hp.isCDS())
		txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
	    else
		txn = mgr.createTransaction();
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	}else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	try {
	    writer.writeText(XmlEventReader.Characters, "hi", "hi".length());
	    fail("Failure in EventWriterTest.testWriteStartEntity()");
	    fail("writeText succeeded when it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// get a new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeText(XmlEventReader.Characters, "hi", "hi".length());
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    writer.close();
	}

	// after close
	try {
	    writer.writeText(XmlEventReader.Characters, "hi", "hi".length());
	    if(txn != null) txn.commit();
	    fail("Failure in EventWriterTest.testWriteText()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	// get the document to compare
	XmlDocument myDoc = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals(myDoc.getContentAsString(), "<root>hi</root>");
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}

	//Test writeText(int, String)
	doc = mgr.createDocument();
	if(hp.isTransactional() || hp.isCDS())
	    writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	else
	    writer = cont.putDocumentAsEventWriter(doc, uc, config);

	try {
	    writer.writeText(XmlEventReader.Characters, "hi");
	    if(txn != null) txn.commit();
	    fail("Failure in EventWriterTest.testWriteText()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.EVENT_ERROR);
	}finally{
	    writer.close();
	    writer = null;
	}

	// get a new writer
	doc = mgr.createDocument();
	try {
	    if(hp.isTransactional() || hp.isCDS())
		writer = cont.putDocumentAsEventWriter(txn, doc, uc, config);
	    else
		writer = cont.putDocumentAsEventWriter(doc, uc, config);
	    writer.writeStartDocument(null, null, null);
	    writer.writeStartElement("root", null, null, 0, false);
	    writer.writeText(XmlEventReader.Characters, "hi");
	    writer.writeEndElement("root", null, null);
	    writer.writeEndDocument();
	} catch (XmlException e) {
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(writer != null) writer.close();
	}

	// after close
	try {
	    writer.writeText(XmlEventReader.Characters, "hi");
	    if(txn != null) txn.commit();
	    fail("Failure in EventWriterTest.testWriteText()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	// get the document to compare
	try{
	    if(hp.isTransactional() || hp.isCDS())
		myDoc = cont.getDocument(txn, doc.getName());
	    else
		myDoc = cont.getDocument(doc.getName());
	    assertEquals(myDoc.getContentAsString(), "<root>hi</root>");
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(txn != null) txn.commit();
	}
    }
}
