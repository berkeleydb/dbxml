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
import com.sleepycat.dbxml.XmlEventReaderToWriter;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

public class EventReaderToWriterTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private final String CON_NAME = "";

    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</a_node>";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlEventReaderToWriter!");
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
	System.out.println("Finished test XmlEventReaderToWriter!");
    }

    /*
     * Test constructor
     * 
     */
    @Test
	public void testConstructor() throws Throwable {
	XmlTransaction txn = null;
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlDocument doc1 = null;
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName("doc2.xml");
	XmlEventWriter writer = null;
	XmlEventReaderToWriter r2w1 = null;
	try{
	    if(hp.isTransactional() || hp.isCDS()){
		if(hp.isCDS())
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		else
		    txn = mgr.createTransaction();
		cont.putDocument(txn, "doc1.xml", docString, context);
		doc1 = cont.getDocument(txn, "doc1.xml");
		writer = cont.putDocumentAsEventWriter(txn, doc2, context);
	    }else{
		cont.putDocument("doc1.xml", docString, context);
		doc1 = cont.getDocument("doc1.xml");
		writer = cont.putDocumentAsEventWriter(doc2, context);
	    }
	    XmlEventReader reader = doc1.getContentAsEventReader();
	    r2w1 = new XmlEventReaderToWriter(reader, writer,
					      true);
	    assertNotNull(r2w1);

	    //Confirm that reader has been released from the program's control.
	    try{
		reader.next();
		fail("Reader can still be accessed after it was given to the XmlEventReaderToWriter.");
	    }catch (XmlException e){
		assertNotNull(e.getMessage());
	    }
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(r2w1 != null){
		r2w1.start();
		r2w1.delete();
	    }
	}

	doc2.setName("doc21.xml");
	XmlEventWriter writer2 = null;
	XmlEventReaderToWriter r2w2 = null;
	XmlEventReader reader2 = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		writer2 = cont.putDocumentAsEventWriter(txn, doc2, context);
	    else
		writer2 = cont.putDocumentAsEventWriter(doc2, context);
	    reader2 = doc1.getContentAsEventReader();
	    r2w2 = new XmlEventReaderToWriter(reader2, writer2,
					      false);
	    assertNotNull(r2w2);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(r2w2 != null){
		r2w2.start();
		r2w2.delete();
	    }
	    if(reader2 != null) reader2.close();
	}

	//test null pointer exception
	doc2.setName("doc22.xml");
	XmlEventWriter writer3 = null;
	if(hp.isTransactional() || hp.isCDS())
	    writer3 = cont.putDocumentAsEventWriter(txn, doc2, context);
	else
	    writer3 = cont.putDocumentAsEventWriter(doc2, context);

	XmlEventReader reader3 = doc1.getContentAsEventReader();
	try{
	    r2w1 = new XmlEventReaderToWriter(null, writer3,
					      true);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w1 = new XmlEventReaderToWriter(reader3, null,
					      true);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w1 = new XmlEventReaderToWriter(null, null,
					      true);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w1 = new XmlEventReaderToWriter(null, writer3,
					      false);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w1 = new XmlEventReaderToWriter(reader3, null,
					      false);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w1 = new XmlEventReaderToWriter(null, null,
					      false);
	    if(txn != null) txn.commit();
	    fail("Failure in EventReaderToWriterTest.testConstructor");
	}catch(NullPointerException e){
	    assertNotNull(e);
	}

	try{
	    r2w2 = new XmlEventReaderToWriter(reader3, writer3,
					      false);
	    assertNotNull(r2w2);
	}catch(XmlException e){
	    if(txn != null) txn.commit();
	    throw e;
	}finally{
	    if(r2w2 != null){
		r2w2.start();
		r2w2.delete();
	    }
	    r2w2 = null;
	    reader3.close();
	    writer3.close();
	}

	doc2.setName("doc23.xml");
	XmlEventWriter writer4 = null;
	XmlEventReader reader4 = null;
	try{
	    if(hp.isTransactional() || hp.isCDS())
		writer4 = cont.putDocumentAsEventWriter(txn, doc2, context);
	    else
		writer4 = cont.putDocumentAsEventWriter(doc2, context);
	    reader4 = doc1.getContentAsEventReader();
	    r2w2 = new XmlEventReaderToWriter(reader4, writer4,
					      false, false);
	}catch(XmlException e){
	    if( txn != null) txn.commit();
	    throw e;
	}finally{
	    if(r2w2 != null){
		r2w2.start();
		r2w2.delete();
	    }
	    r2w2 = null;
	    if(reader4 != null) reader4.close();
	    if(writer4 != null) writer4.close();
	}

	doc2.setName("doc24.xml");
	XmlEventWriter writer5 = null;
	if(hp.isTransactional() || hp.isCDS())
	    writer5 = cont.putDocumentAsEventWriter(txn, doc2, context);
	else
	    writer5 = cont.putDocumentAsEventWriter(doc2, context);
	XmlEventReader reader5 = doc1.getContentAsEventReader();
	r2w2 = new XmlEventReaderToWriter(reader5, writer5,
					  true, true);
	//Confirm that reader has been released from the program's control.
	try{
	    reader5.next();
	    if(txn != null) txn.commit();
	    fail("Reader can still be accessed after it was given to the XmlEventReaderToWriter.");
	}catch (XmlException e){
	    assertNotNull(e.getMessage());
	}
	//Confirm that writer has been released from the program's control.
	try{
	    writer5.writeStartDocument(null, null, null);
	    if( txn != null) txn.commit();
	    fail("Writer can still be accessed after it was given to the XmlEventReaderToWriter.");
	}catch (XmlException e){
	    assertNotNull(e.getMessage());
	}finally{
	    r2w2.start();
	    r2w2.delete();
	    if(txn != null) txn.commit();
	}
    }

    /*
     * Test for start()
     * 
     */
    @Test
	public void testStart() throws Throwable{
	XmlTransaction txn = null;
	XmlUpdateContext context = mgr.createUpdateContext();
	XmlDocument doc1 = null;
	XmlDocument doc2 = mgr.createDocument();
	doc2.setName("doc2.xml");
	XmlEventWriter writer = null;
	XmlEventReaderToWriter r2w1 = null;
	try{
	    if(hp.isTransactional() || hp.isCDS()){
		if(hp.isCDS()){
		    txn = mgr.createTransaction(mgr.getEnvironment().beginCDSGroup());
		}else
		    txn = mgr.createTransaction();
		cont.putDocument(txn, "doc1.xml", docString, context);
		doc1 = cont.getDocument(txn, "doc1.xml");
		writer = cont.putDocumentAsEventWriter(txn, doc2, context);
	    }else{
		cont.putDocument("doc1.xml", docString, context);
		doc1 = cont.getDocument("doc1.xml");
		writer = cont.putDocumentAsEventWriter(doc2, context);
	    }
	    XmlEventReader reader = doc1.getContentAsEventReader();
	    r2w1 = new XmlEventReaderToWriter(reader, writer,
					      true);
	    r2w1.start();
	    XmlDocument newDoc = null;
	    if(hp.isTransactional() || hp.isCDS())
		newDoc = cont.getDocument(txn, "doc2.xml");
	    else
		newDoc = cont.getDocument("doc2.xml");
	    assertEquals(newDoc.getContentAsString(), docString);
	}catch (XmlException e){
	    throw e;
	}finally{
	    if(r2w1 != null) r2w1.delete();
	    if(txn != null) txn.commit();
	}
    }
}
