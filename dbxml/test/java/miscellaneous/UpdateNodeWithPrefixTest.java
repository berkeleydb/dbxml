package dbxmltest;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;

public class UpdateNodeWithPrefixTest {

    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;
    private static final String CON_NAME = "testData.dbxml";
    private static String docString = "<a_node atr1=\"test\" atr2=\"test2\""
    	+ "><b_node/><c_node>Other text</c_node><d_node/>"
    	+ "</a_node>";

    private static String docName = "testDoc.xml";
    
    @BeforeClass
    public static void setupClass() {
	System.out.println("Begin UpdateNodeWithPrefix test!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
    public void setUp() throws Throwable {
        hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
		XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
        mgr = hp.createManager();
        cont = hp.createContainer(CON_NAME, mgr);
        cont.putDocument(docName, docString);
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
        System.out.println("Finish UpdateNodeWithPrefix test!");
    }
    
    @Test
    public void testRenameNode() throws Throwable{
        XmlQueryContext qc = mgr.createQueryContext();
        String query = "rename node collection('testData.dbxml')/a_node/c_node as 'foo:bar'";
        String verfiyQuery = "collection('testData.dbxml')/a_node/foo:bar";
        String verfiyContent = "<foo:bar xmlns:foo=\"http://foo.bar\">Other text</foo:bar>";
        XmlTransaction txn = null;
        XmlResults re = null;
        //test without prefix namespace set
        try {           
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
            } else {
                mgr.query(query, qc);
            }
            fail("Fail to throw exception in testRenameNode()");    
        } catch (XmlException e){
            assertNotNull(e);
            assertEquals(e.getErrorCode(),XmlException.QUERY_EVALUATION_ERROR);
        } finally {
            if (txn != null) txn.commit();
        }
        
        //test success
        qc.setNamespace("foo", "http://foo.bar");
        try {
            
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
                re = mgr.query(txn, verfiyQuery, qc);              
            } else {
                mgr.query(query, qc);
                re = mgr.query(verfiyQuery, qc);
            }
            
            assertEquals(re.size(), 1);
            assertEquals(re.next().asString(), verfiyContent);               
        } catch (XmlException e){
            throw e;
        } finally {
            if (txn != null) txn.commit();
            if (re != null) re.delete();
        }       
    }
    
    @Test
    public void testRepalceNode() throws Throwable{
        XmlQueryContext qc = mgr.createQueryContext();
        String query = "replace node collection('testData.dbxml')/a_node/c_node with element foo:bar {'text'}";
        String verfiyQuery = "collection('testData.dbxml')/a_node/foo:bar";
        String verfiyContent = "<foo:bar xmlns:foo=\"http://foo.bar\">text</foo:bar>";
        XmlTransaction txn = null;
        XmlResults re = null;
        //test without prefix namespace set
        try {          
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
            } else {
                mgr.query(query, qc);
            }
            fail("Fail to throw exception in testReplaceNode()");    
        } catch (XmlException e){
            assertNotNull(e);
            assertEquals(e.getErrorCode(),XmlException.QUERY_PARSER_ERROR);
        } finally {
            if (txn != null) txn.commit();
        }
        
        //test success
        qc.setNamespace("foo", "http://foo.bar");
        try {
            
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
                re = mgr.query(txn, verfiyQuery, qc);
                
            } else {
                mgr.query(query, qc);
                re = mgr.query(verfiyQuery, qc);
            }
            
            assertEquals(re.size(), 1);
            assertEquals(re.next().asString(), verfiyContent);                
        } catch (XmlException e){
            throw e;
        } finally {
            if (txn != null) txn.commit();
            if (re != null) re.delete();
        }  
    }
    
    @Test
    public void testInsertNode() throws Throwable{
        XmlQueryContext qc = mgr.createQueryContext();
        String query = "insert node element foo:bar {'text'} into collection('testData.dbxml')/a_node/c_node";
        String verfiyQuery = "collection('testData.dbxml')/a_node/c_node/foo:bar";
        String verfiyContent = "<foo:bar xmlns:foo=\"http://foo.bar\">text</foo:bar>";
        XmlTransaction txn = null;
        XmlResults re = null;
        //test without prefix namespace set
        try {          
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
            } else {
                mgr.query(query, qc);
            }
            fail("Fail to throw exception in testInsertNode()");    
        } catch (XmlException e){
            assertNotNull(e);
            assertEquals(e.getErrorCode(),XmlException.QUERY_PARSER_ERROR);
        } finally {
            if (txn != null) txn.commit();
        }
        
        //test success
        qc.setNamespace("foo", "http://foo.bar");
        try {
            
            if (hp.isTransactional()){
                txn = mgr.createTransaction();
                mgr.query(txn, query, qc);
                re = mgr.query(txn, verfiyQuery, qc);
                
            } else {
                mgr.query(query, qc);
                re = mgr.query(verfiyQuery, qc);
            }
            
            assertEquals(re.size(), 1);
            assertEquals(re.next().asString(), verfiyContent);                
        } catch (XmlException e){
            throw e;
        } finally {
            if (txn != null) txn.commit();
            if (re != null) re.delete();
        }  
    }
}
