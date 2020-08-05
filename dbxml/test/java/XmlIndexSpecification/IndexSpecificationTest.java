/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlIndexDeclaration;
import com.sleepycat.dbxml.XmlIndexSpecification;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlValue;

public class IndexSpecificationTest {
    private static TestConfig hp;
    private static final String CON_NAME = "testData.dbxml";
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlIndexSpecification!");
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
	System.out.println("Finished test XmlIndexSpecification!");
    }

    /*
     * Test for addDefaultIndex(int type, int syntax)
     * 
     */
    @Test
	public void testAddDefaultIndex_IntInt() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;
	int syntaxType = XmlValue.STRING;
	//not valid index
	try {
	    is.addDefaultIndex(idxType, 100);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("Failure in IndexSpecificationTest.testAddDefaultIndex_IntInt()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	//not valid index
	try {
	    is.addDefaultIndex(100, syntaxType);
	    is.addDefaultIndex(idxType, 100);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("Failure in IndexSpecificationTest.testAddDefaultIndex_IntInt()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	try {
	    is.addDefaultIndex(idxType, syntaxType);
	    assertEquals(is.getDefaultIndex(), "node-metadata-equality-string");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for addDefaultIndex(String index)
     * 
     */
    @Test
	public void testAddDefaultIndex_Str() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	String index = " ";
	//empty index fails silently
	try {
	    is.addDefaultIndex(index);
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	index = "element-presence-none";
	//not valid index
	try {
	    is.addDefaultIndex(index);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("fail in testAddDefaultIndex_Str()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	index = "none";
	//
	try {
	    is.addDefaultIndex(index);
	    assertEquals(is.getDefaultIndex(), "none");
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	index = "node-metadata-equality-string";
	try {
	    is.addDefaultIndex(index);
	    assertEquals(is.getDefaultIndex(),
			 "none node-metadata-equality-string");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for addIndex(String uri, String name, int type, int syntax) 
     * 
     */
    @Test
	public void testAddIndex_StrStrIntInt() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;
	int syntaxType = XmlValue.STRING;
	//not valid index
	try {
	    is.addIndex("", "node1", 100, syntaxType);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	//not valid index
	try {
	    is.addIndex("", "node1", idxType, 100);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addtIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	//not valid index
	try {
	    is.addIndex("", "", idxType, syntaxType);
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addDefaultIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	//not valid index
        try {
            is.addIndex("", "", idxType, XmlValue.DAY_TIME_DURATION);
            is.delete();
            if(hp.isTransactional() && txn != null) txn.commit();
            fail("addIndex succeeded where it should have failed.");
        } catch (XmlException e) {
            assertNotNull(e.getMessage());
            assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
        }
        
        //not valid index
        try {
            is.addIndex("", "", idxType, XmlValue.YEAR_MONTH_DURATION);
            is.delete();
            if(hp.isTransactional() && txn != null) txn.commit();
            fail("addIndex succeeded where it should have failed.");
        } catch (XmlException e) {
            assertNotNull(e.getMessage());
            assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
        }
        
        //not valid index
        try {
            is.addIndex("", "", idxType, XmlValue.UNTYPED_ATOMIC);
            is.delete();
            if(hp.isTransactional() && txn != null) txn.commit();
            fail("addIndex succeeded where it should have failed.");
        } catch (XmlException e) {
            assertNotNull(e.getMessage());
            assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
        }

	try {
	    is.addIndex("", "node", idxType, syntaxType);
	    XmlIndexDeclaration id = is.find("", "node");
	    assertEquals(id.name, "node");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for addIndex(String uri, String name, String index)  
     * 
     */
    @Test
	public void testAddIndex_StrStrStr() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();

	//not valid index
	try {
	    is.addIndex("", "node", "node-metadata-string");
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}

	//not valid index
	try {
	    is.addIndex("", "", "node-metadata-string");
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	//not valid index
	try {
	    is.addIndex("", "", "node-metadata-equality-dayTimeDuration");
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	//not valid index
	try {
	    is.addIndex("", "", "node-metadata-equality-yearMonthDuration");
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	//not valid index
	try {
	    is.addIndex("", "", "node-metadata-equality-untypedAtomic");
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    fail("addIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}

	try {
	    is.addIndex("", "node", "node-metadata-equality-string");
	    XmlIndexDeclaration id = is.find("", "node");
	    assertEquals(id.name, "node");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for delete
     * 
     */
    @Test
	public void testDelete() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	is.delete();

	try {
	    is.getDefaultIndex();
	    fail("testDelete succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}finally{
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteDefaultIndex(int type,int syntax)
     * 
     */
    @Test
	public void testDeleteDefaultIndex_IntInt() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;
	int syntaxType = XmlValue.STRING;
	String index = "node-metadata-equality-string";
	try {
	    is.addDefaultIndex("none");
	    is.addDefaultIndex(index);
	    assertEquals(is.getDefaultIndex(),
			 "none node-metadata-equality-string");

	    is.deleteDefaultIndex(idxType, syntaxType);
	    assertEquals(is.getDefaultIndex(), "none");

	    //delete not exist defaultIndex
	    is.deleteDefaultIndex(idxType, syntaxType);
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	try {
	    is.deleteDefaultIndex(100, XmlValue.DATE);
	    fail("deleteDefaultIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for deleteDefaultIndex(String index)
     * 
     */
    @Test
	public void testDeleteDefaultIndex_Str() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();

	String index = "node-metadata-equality-string";
	try {
	    is.addDefaultIndex("none");
	    is.addDefaultIndex(index);
	    assertEquals(is.getDefaultIndex(),
			 "none node-metadata-equality-string");

	    is.deleteDefaultIndex(index);
	    assertEquals(is.getDefaultIndex(), "none");

	    //delete not exist defaultIndex
	    is.deleteDefaultIndex(index);
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//An empty index fails silently
	try {
	    is.deleteDefaultIndex("  ");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * deleteIndex(String uri, String name, int type, int syntax) 
     * 
     */
    @Test
	public void testDeleteIndex_StrStrIntInt() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;
	int syntaxType = XmlValue.STRING;
	try {
	    is.addIndex("", "node", "node-metadata-equality-string");
	    XmlIndexDeclaration id = is.find("", "node");
	    assertEquals(id.name, "node");
	    is.deleteIndex("", "node", idxType, syntaxType);
	    id = is.find("", "node");
	    assertNull(id);

	    //delete not exist index
	    is.deleteIndex("", "node2", idxType, syntaxType);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * deleteIndex(String uri, String name, String index)  
     * 
     */
    @Test
	public void testDeleteIndex_StrStrStr() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();

	try {
	    is.addIndex("", "node", "node-metadata-equality-string");
	    XmlIndexDeclaration id = is.find("", "node");
	    assertEquals(id.name, "node");
	    is.deleteIndex("", "node", "node-metadata-equality-string");
	    id = is.find("", "node");
	    assertNull(id);

	    //delete not exist index
	    is.deleteIndex("", "node2", "node-metadata-equality-string");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for find(String uri, String name)  
     * 
     */
    @Test
	public void testFind() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();

	try {
	    is.addIndex("", "node", "node-metadata-equality-string");
	    XmlIndexDeclaration id = is.find("", "node");
	    assertEquals(id.name, "node");

	    //find not exist index
	    id = is.find("", "node2");
	    assertNull(id);

	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for getDefaultIndex()  
     * 
     */
    @Test
	public void testGetDefaultIndex() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		is = cont.getIndexSpecification(txn);
	    }else
		is = cont.getIndexSpecification();
	    assertEquals(is.getDefaultIndex().length(), 0);

	    is.addDefaultIndex("none");
	    is.addDefaultIndex("node-metadata-equality-string");
	    assertEquals(is.getDefaultIndex(), "none node-metadata-equality-string");
	}catch(XmlException e){
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}

    }

    /*
     * Test for next() 
     * 
     */
    @Test
	public void testNext() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	XmlIndexDeclaration idxDe;
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		is = cont.getIndexSpecification(txn);
	    }else
		is = cont.getIndexSpecification();
	    idxDe = is.next();
	    assertEquals(idxDe.index, "unique-node-metadata-equality-string");

	    idxDe = is.next();
	    assertNull(idxDe);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * replaceDefaultIndex(int type, int syntax) 
     * 
     */
    @Test
	public void testReplaceDefaultIndex_IntInt() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;
	int syntaxType = XmlValue.STRING;

	try {
	    is.replaceDefaultIndex(idxType, syntaxType);	    
	    assertEquals(is.getDefaultIndex(), "node-metadata-equality-string");
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	try {
	    is.replaceDefaultIndex(100, syntaxType);	  
	    fail("Failure in testReplaceDefaultIndex_IntInt");
	} catch (XmlException e) {
	    assertNotNull(e.getErrno());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for replaceDefaultIndex(String index)  
     * 
     */
    @Test
	public void testReplaceDefaultIndex_Str() throws Throwable {
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	String index = "node-metadata-equality-string";
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		is = cont.getIndexSpecification(txn);
	    }else
		is = cont.getIndexSpecification();
	    is.replaceDefaultIndex(index);	    
	    assertEquals(is.getDefaultIndex(), "node-metadata-equality-string");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for replaceIndex(String uri, String name, int type, int syntax)  
     * 
     */
    @Test
	public void testReplaceIndex_StrStrIntInt() throws Throwable{
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();

	int idxType = XmlIndexSpecification.PATH_NODE
	    | XmlIndexSpecification.NODE_METADATA
	    | XmlIndexSpecification.KEY_EQUALITY;

	int syntaxType = XmlValue.STRING;

	try {
	    is.replaceIndex("", "node", idxType, syntaxType);	    
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	try {
	    is.replaceIndex("","node",100, syntaxType);	    
	    fail("replaceIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getErrno());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for replaceIndex(String uri, String name, String index)   
     * 
     */
    @Test
	public void testReplaceIndex_StrStrStr() throws Throwable{
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	if(hp.isTransactional()){
	    txn = mgr.createTransaction();
	    is = cont.getIndexSpecification(txn);
	}else
	    is = cont.getIndexSpecification();
	String index= "node-metadata-equality-string";
	try {
	    is.replaceIndex("", "node", index);	    
	} catch (XmlException e) {
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//not valid index
	try {
	    is.replaceIndex("","node","equality-string");	    
	    fail("replaceIndex succeeded where it should have failed.");
	} catch (XmlException e) {
	    assertNotNull(e.getErrno());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test for reset()   
     * 
     */
    @Test
	public void testReset() throws Throwable{
	XmlTransaction txn = null;
	XmlIndexSpecification is = null;
	try{
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		is = cont.getIndexSpecification(txn);
	    }else
		is = cont.getIndexSpecification();

	    is.addIndex("", "node", "none");
	    is.addIndex("", "node2", "node-metadata-equality-string");

	    XmlIndexDeclaration idxDe;
	    is.next();
	    idxDe = is.next();
	    is.reset();

	    idxDe = is.next();
	    assertEquals(idxDe.name, "name");
	}catch(XmlException e){
	    throw e;
	}finally{
	    is.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    
    @Test
    public void testGetValueType() throws Throwable {
    final String indexPrefix = "node-metadata-equality-";
    final String[] syntax = {"none", "base64Binary", "boolean", "date", "dateTime",
            "decimal", "double", "duration", "float", "gDay",
            "gMonth", "gMonthDay", "Year", "gYearMonth", "hexBinary", "string",
            "time" };
    final int[] syntaxType = { XmlValue.NONE, XmlValue.BASE_64_BINARY, XmlValue.BOOLEAN,
            XmlValue.DATE, XmlValue.DATE_TIME, XmlValue.DECIMAL,
            XmlValue.DOUBLE, XmlValue.DURATION, XmlValue.FLOAT, XmlValue.G_DAY, XmlValue.G_MONTH,
            XmlValue.G_MONTH_DAY, XmlValue.G_YEAR, XmlValue.G_YEAR_MONTH, XmlValue.HEX_BINARY,
            XmlValue.STRING, XmlValue.TIME };
    for (int i = 0; i < syntax.length; i ++) {
        assertEquals(syntaxType[i], XmlIndexSpecification.getValueType(indexPrefix + syntax[i]));
    }
    }
}
