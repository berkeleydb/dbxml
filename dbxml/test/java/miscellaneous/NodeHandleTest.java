package dbxmltest;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.*;

public class NodeHandleTest{
    // Container name;
    private static final String CON_NAME = "testData.dbxml";
    private static String docString = "<root>"
	+ "<a_node atr1='test' atr2='test2'><b_node/>"
	+ "<c_node>Other text</c_node><d_node>dtext</d_node>atext</a_node>"
	+"</root>";

    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test Node Handle");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
	public void setUp() throws Throwable {
	if(hp != null)
	    TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager(hp.getEnvironmentPath(), null);
	cont = hp.createContainer(CON_NAME, mgr);
	cont.addAlias("container");
	// use the same document for all tests so put it here
	cont.putDocument("test", docString);
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
	System.out.println("Finished test Node Handle");
    }

    @Test
	public void NodeHandleContainer()throws XmlException {
	// transactions are not relevant to this case so ignore them
	String contextQuery = "collection('testData.dbxml')/root/a_node";
	String elemQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare variable $handle external;"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/* return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(dbxml:handle-to-node('container', $handle))";
	String attrQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare variable $handle external;"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/@* return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(dbxml:handle-to-node('container', $handle))";
	String textQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare variable $handle external;"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/text() return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(dbxml:handle-to-node('container', $handle))";

	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = null;
	XmlResults res = null;
	try{
	    results = mgr.query(contextQuery, context);
	    assertTrue(results.size() == 1);
	    XmlValue v = results.next();
	    String h = v.getNodeHandle();
	    context.setVariableValue("handle", new XmlValue(h));

	    res = mgr.query(elemQuery, context);
	    assertTrue(res.size() == 3); // a_node has 3 child elements
	    res.delete();

	    res = mgr.query(attrQuery, context);
	    assertTrue(res.size() == 2); // a_node has 2 attributes
	    res.delete();

	    res = mgr.query(textQuery, context);
	    assertTrue(res.size() == 1); // a_node has 1 text child
	    res.delete();
	    results.delete();
	}catch(XmlException e){
	    throw e;
	} finally {
	    if (results != null)
		results.delete();
	    if (res != null)
		res.delete();
	}
    }

    @Test
	public void NodeHandleConstructed()throws XmlException {
	// transactions are not relevant to this case so ignore them
	String elemQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/* return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(<a><b/></a>)";
	String attrQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/@* return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(<a a1='a1' a2='a2'><b/></a>)";
	String textQuery="declare namespace ma = 'http://www.xxx.ag/xquery/extensions';"+ 
	    "declare namespace dbxml = 'http://www.sleepycat.com/2002/dbxml';"+
	    "declare function ma:start($node){"+
	      "for $nextnode in $node/text() return "+
	        "dbxml:node-to-handle($nextnode)"+ 
	    "};"+ 
	    "ma:start(<a a1='a1' a2='a2'>text1<b/>text2</a>)";

	XmlQueryContext context = mgr.createQueryContext();
	XmlResults res = null;
	try{
	    res = mgr.query(elemQuery, context);
	    res.delete();
	    fail("Query for transient node handles should have failed");
	}catch(XmlException e){
	    //ignore error
	}
	try{
	    res = mgr.query(attrQuery, context);
	    res.delete();
	    fail("Query for transient node handles should have failed");
	}catch(XmlException e){
	    //ignore error
	}
	try{
	    res = mgr.query(textQuery, context);
	    res.delete();
	    fail("Query for transient node handles should have failed");
	}catch(XmlException e){
	    //ignore error
	}
	finally {
	    if (res != null)
		res.delete();
	}

    }
    
    
    private String docName = "insert.xml";
    private String docStr = "<doc><table><element>0</element></table><footer/></doc>";
    private void runUpdateQuery(String updateQuery, int resultsSize, int iterations) throws XmlException {
        /*
         * If the size of the node handle of any of the inserted nodes is longer than
         * 3 times the size of the node handle of /doc/table, then the nid is growing
         * out of control.
         */
        String lengthTestQuery = 
            "let $size := fn:string-length(dbxml:node-to-handle(fn:doc('dbxml:" + CON_NAME + "/" + docName + "')/doc/table)) " +
            "let $nodes := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')/doc/table/element " +
            "return " +
            "for $node in $nodes " +
            "let $newSize := fn:string-length(dbxml:node-to-handle($node)) " +
            "where ($size * 3) < $newSize " +
            "return <failure>{$node, $newSize}</failure>";
        String validateUpdateQuery = "fn:doc('dbxml:" + CON_NAME + "/" + docName + "')/doc/table/element";
        XmlQueryContext context = mgr.createQueryContext();
        XmlResults res = null;
        cont.putDocument(docName, docStr);
        XmlQueryExpression exp = null;
        try {
            exp = mgr.prepare(updateQuery, context);
            for (int i = 0; i < iterations; i++) {
                res = exp.execute(context);
                res.delete();
            }
            res = mgr.query(validateUpdateQuery, context);
            assertEquals(resultsSize, res.size());
            res.delete();
            res = mgr.query(lengthTestQuery, context);
            assertEquals(0, res.size());
        } finally {
            if (exp != null) exp.delete();
            if (res != null) res.delete();
        }
        cont.deleteDocument(docName);
    }
    
    // Uses the node handle to test uncontrolled nid growth due to inserts [#17844]
    @Test
    public void testNodeHandleSize()throws XmlException {   
        cont.setAutoIndexing(false);
        // Test inserting 1000 nodes in a single query
        String insertBefore = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table/element " +
        "return for $i in (1 to 1000) " +
        "return insert node <element>{$i}</element> before $node";
        String insertAfter = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table/element " +
        "return for $i in (1 to 1000) " +
        "return insert node <element>{$i}</element> after $node";
        String insertInto = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return for $i in (1 to 1000) " +
        "return insert node <element>{$i}</element> into $node";
        String insertAsFirst = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return for $i in (1 to 1000) " +
        "return insert node <element>{$i}</element> as first into $node";
        String insertAsLast = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return for $i in (1 to 1000) " +
        "return insert node <element>{$i}</element> as last into $node";
        String replace = "for $i in (1 to 1000) " +
        "return insert node <element/> as first into fn:doc('dbxml:" + 
        CON_NAME + "/" + docName + "')" + "/doc/table , " + 
        "replace node  fn:doc('dbxml:" +  CON_NAME + "/" + docName + "')" + 
        "/doc/table/element[1] with <element/>";
        
        runUpdateQuery(insertBefore, 1001, 1);
        runUpdateQuery(insertAfter, 1001, 1);
        runUpdateQuery(insertInto, 1001, 1);
        runUpdateQuery(insertAsFirst, 1001, 1);
        runUpdateQuery(insertAsLast, 1001, 1);
        runUpdateQuery(replace, 1001, 1);
        
        docStr = "<doc><table><element><number>0</number></element></table><footer/></doc>";
        runUpdateQuery(insertBefore, 1001, 1);
        runUpdateQuery(insertAfter, 1001, 1);
        
        docStr = "<doc><table></table><footer/></doc>";
        runUpdateQuery(insertInto, 1000, 1);
        runUpdateQuery(insertAsFirst, 1000, 1);
        runUpdateQuery(insertAsLast, 1000, 1);
        
        // Test inserting one node 1000 times
        insertBefore = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table/element[fn:last()] " +
        "return insert node <element/> before $node";
        insertAfter = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table/element[1] " +
        "return insert node <element/> after $node";
        insertInto = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return insert node <element/> into $node";
        insertAsFirst = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return insert node <element/> as first into $node";
        insertAsLast = "let $node := fn:doc('dbxml:" + CON_NAME + "/" + docName + "')" +
        "/doc/table " +
        "return insert node <element/> as last into $node";
        replace = "insert node <element/> as first into fn:doc('dbxml:" +
        CON_NAME + "/" + docName + "')/doc/table ," + 
        "replace node  fn:doc('dbxml:" +  CON_NAME + "/" + docName + "')" + 
        "/doc/table/element[1] with <element/>";
        
        docStr = "<doc><table><element>0</element></table><footer/></doc>";
        runUpdateQuery(insertBefore, 1001, 1000);
        runUpdateQuery(insertAfter, 1001, 1000);
        runUpdateQuery(insertInto, 1001, 1000);
        runUpdateQuery(insertAsFirst, 1001, 1000);
        runUpdateQuery(insertAsLast, 1001, 1000);
        runUpdateQuery(replace, 1001, 1000);
        
        docStr = "<doc><table><element><number>0</number></element></table><footer/></doc>";
        runUpdateQuery(insertBefore, 1001, 1000);
        runUpdateQuery(insertAfter, 1001, 1000);
        
        docStr = "<doc><table></table><footer/></doc>";
        runUpdateQuery(insertInto, 1000, 1000);
        runUpdateQuery(insertAsFirst, 1000, 1000);
        runUpdateQuery(insertAsLast, 1000, 1000);
    }
};
