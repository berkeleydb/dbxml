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


import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlIndexSpecification;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlStatistics;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;

public class StatisticsTest {
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";
    private static String docName = "testDoc.xml";

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlStatistics!");
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
	System.out.println("Finished test XmlStatistics!");
    }

    /*
     * Test for getNumberOfIndexedKeys() 
     * 
     */
    @Test
	public void testGetNumberOfIndexedKeys() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	XmlIndexSpecification is = null;
	XmlStatistics sta = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		is = cont.getIndexSpecification(txn);
	    }else{
		cont.putDocument(doc, uc);
		is = cont.getIndexSpecification();
	    }
	    is.addIndex("", "name", "node-element-equality-string");
	    if(hp.isTransactional()){
		cont.setIndexSpecification(txn, is, uc);
		sta = cont.lookupStatistics(txn, "", "name","node-element-equality-string");
	    }else{
		cont.setIndexSpecification(is, uc);
		sta = cont.lookupStatistics("", "name","node-element-equality-string");
	    }
	    assertEquals(sta.getNumberOfIndexedKeys(), 3.0, 0);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(is != null) is.delete();
	    if(sta != null) sta.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getNumberOfUniqueKeys() 
     * 
     */
    @Test
	public void testGetNumberOfUniqueKeys() throws Throwable {
	XmlTransaction txn = null;
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name><name>Joe</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	XmlIndexSpecification is = null;
	XmlStatistics sta = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc, uc);
		is = cont.getIndexSpecification(txn);
	    }else{
		cont.putDocument(doc, uc);
		is = cont.getIndexSpecification();
	    }
	    is.addIndex("", "name", "node-element-equality-string");
	    if(hp.isTransactional()){
		cont.setIndexSpecification(txn, is, uc);
		sta = cont.lookupStatistics(txn, "", "name", "node-element-equality-string");
	    }else{
		cont.setIndexSpecification(is, uc);
		sta = cont.lookupStatistics("", "name", "node-element-equality-string");
	    }
	    assertEquals(sta.getNumberOfUniqueKeys(), 3.0, 0);
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(is != null) is.delete();
	    if(sta != null) sta.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }
}
