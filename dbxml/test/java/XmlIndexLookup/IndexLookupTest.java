/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlDocumentConfig;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlIndexLookup;
import com.sleepycat.dbxml.XmlIndexSpecification;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlValue;

public class IndexLookupTest {
    private static final String cname = "testData.dbxml";
    private static String docName = "testDoc.xml";

    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlIndexLookup");
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finished test XmlIndexLookup");
    }

    @Before
	public void setUp() throws Throwable {
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), 
			    XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
	cont = hp.createContainer(cname, mgr);
    }

    @After
	public void tearDown() throws Throwable {
	hp.closeContainer(cont);
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(null);
    }

    /*
     * Test for execute(XmlQueryContext context)
     * 
     */
    @Test
	public void testExecute() throws Throwable {

	XmlContainerConfig xcc = new XmlContainerConfig();
	xcc.setContainerType(XmlContainer.NodeContainer);
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlQueryContext xqc = mgr.createQueryContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name><my><name>Kate</name></my></names>";
	doc.setContent(docString);
	doc.setName(docName);

	cont.putDocument(doc, uc);

	XmlIndexSpecification is = cont.getIndexSpecification();
	is.addIndex("", "name", "node-element-equality-string");
	is.addIndex("", "name", "node-element-substring-string");
	is.addIndex("", "name", "edge-element-equality-string");

	// create index
	cont.setIndexSpecification(is, uc);

	String uri = "";
	String name = "name";
	String idxStrategy = "node-element-equality-string";

	XmlIndexLookup xil =
	    mgr.createIndexLookup(cont, uri, name, idxStrategy);
	XmlResults xr = null;

	try {
	    xr = xil.execute(xqc);
	    assertEquals(xr.size(), 4);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}

	// EvaluationType Lazy
	xil = mgr.createIndexLookup(cont, uri, name, idxStrategy);
	try {
	    xqc.setEvaluationType(XmlQueryContext.Lazy);
	    xr = xil.execute(xqc);
	    assertTrue(xr.hasNext());
	} catch (XmlException e) {
	    throw e;
	}finally{
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}

	// set the queryContext to eager
	xqc.setEvaluationType(XmlQueryContext.Eager);
	// Unknown index specification
	xil = mgr.createIndexLookup(cont, uri, name,
				    "node-string-equality-string");
	try {
	    xr = xil.execute(xqc);
	    fail("Index lookup succeeded on non-existent index.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}

	// substring not available
	xil = mgr.createIndexLookup(cont, uri, name,
				    "node-element-substring-string");
	try {
	    xr = xil.execute(xqc);
	    fail("Index lookup succeeded on unknown index.");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.UNKNOWN_INDEX);
	}finally{
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}

	// set parent
	xil = mgr.createIndexLookup(cont, "", "name",
				    "edge-element-equality-string");
	xil.setParent("", "my");
	try {
	    xr = xil.execute(xqc);
	    assertEquals(xr.size(), 1);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}
    }

    /*
     * Test for execute(XmlQueryContext context, XmlDocumentConfig config)
     * 
     */
    @Test
	public void testExecute_Conf() throws Throwable {

	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();
	XmlQueryContext xqc = mgr.createQueryContext();
	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	cont.putDocument(doc, uc);

	XmlIndexSpecification is = cont.getIndexSpecification();
	is.addIndex("", "name", "node-element-equality-string");

	cont.setIndexSpecification(is, uc);

	String uri = "";
	String name = "name";
	String idxStrategy = "node-element-equality-string";

	XmlIndexLookup xil =
	    mgr.createIndexLookup(cont, uri, name, idxStrategy);
	XmlDocumentConfig xdc = new XmlDocumentConfig();
	xdc.setLazyDocs(true);

	XmlResults xr = null;
	try {
	    xr = xil.execute(xqc, xdc);
	    assertEquals(xr.size(), 3);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}

	xil = mgr.createIndexLookup(cont, uri, name, idxStrategy);
	xdc = new XmlDocumentConfig();
	xdc.setDocumentProjection(true);

	try {
	    xr = xil.execute(xqc, xdc);
	    fail("Execute succeeded with an invalid configuration.");
	} catch (XmlException e) {
	    assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
	}finally{
	    xil.delete();
	    if(xr != null) xr.delete();
	    xr = null;
	}
    }

    /*
     * Test for getContainer()
     * 
     * 
     */
    @Test
	public void testGetContainer() throws Throwable {
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();

	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	cont.putDocument(doc, uc);

	XmlIndexSpecification is = cont.getIndexSpecification();
	is.addIndex("", "name", "node-element-equality-string");

	cont.setIndexSpecification(is, uc);

	String uri = "";
	String name = "name";
	String idxStrategy = "node-element-equality-string";

	XmlIndexLookup xil =
	    mgr.createIndexLookup(cont, uri, name, idxStrategy);

	XmlContainer myCont = null;

	//test getContainer return the right container
	try {
	    myCont = xil.getContainer();
	    assertEquals(myCont.getName(), "testData.dbxml");
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    xil.delete();
	}
    }
    
    /*
     * Test getIndex()
     * 
     */
    @Test
	public void testGetIndex() throws Throwable{
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();

	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	cont.putDocument(doc, uc);

	XmlIndexSpecification is = cont.getIndexSpecification();
	is.addIndex("", "name", "node-element-equality-string");

	cont.setIndexSpecification(is, uc);

	String uri = "";
	String name = "name";
	String idxStrategy = "node-element-equality-string";

	XmlIndexLookup xil =
	    mgr.createIndexLookup(cont, uri, name, idxStrategy);

	//test getContainer return the right container
	try {
	    String myIdx = xil.getIndex();
	    assertEquals(myIdx, idxStrategy);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    xil.delete();
	}
    }
    
    /*
     * Test setIndex()
     * 
     */
    @Test
	public void testSetIndex() throws Throwable{
	XmlDocument doc = mgr.createDocument();
	XmlUpdateContext uc = mgr.createUpdateContext();

	String docString = "<names><name>Jack</name><name>Joe"
	    + "</name><name>Mike</name></names>";
	doc.setContent(docString);
	doc.setName(docName);

	cont.putDocument(doc, uc);

	XmlIndexSpecification is = cont.getIndexSpecification();
	is.addIndex("", "name", "node-element-equality-string");

	cont.setIndexSpecification(is, uc);

	String uri = "";
	String name = "name";
	String idxStrategy = "node-element-equality-string";

	XmlIndexLookup xil =
	    mgr.createIndexLookup(cont, uri, name, idxStrategy);

	//test getContainer return the right container
	try {
	    xil.setIndex(idxStrategy);
	    assertEquals(xil.getIndex(), idxStrategy);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    is.delete();
	    xil.delete();
	}
    }
    
    /*
     * [#17671] Attribute indexes are not properly updated when
     * insert/replace are used in the same query on a container
     * that has only attribute indexes.
     * 
     */
    @Test
    public void SR17671() throws Throwable{
        XmlUpdateContext uc = mgr.createUpdateContext();
        XmlResults res = null;
        String content = 
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
            "<root>" +
            "<atrSiteConfigEntry siteConfigSiteID=\"1\">" +
            "<siteConfigSiteAlias>Site1</siteConfigSiteAlias>" +
            "<siteConfigResourcesICCapable>0</siteConfigResourcesICCapable>" +
            "</atrSiteConfigEntry>" +
            "</root>";
        String query = "insert node attribute modifier{\"modify\"} into collection('" + 
        cname + "')/root/atrSiteConfigEntry[@siteConfigSiteID=\"1\"], " +
        "replace node collection('" + cname + 
        "')/root/atrSiteConfigEntry[@siteConfigSiteID=\"1\"]" +
        "/siteConfigSiteAlias with <siteConfigSiteAlias>site_1_mod</siteConfigSiteAlias>";
        XmlQueryContext qc = mgr.createQueryContext();
        XmlTransaction txn = null;
        try {
            if (hp.isTransactional()) {
                txn = mgr.createTransaction();
                cont.setAutoIndexing(txn, false);
                cont.addIndex(txn, "", "siteConfigSiteID","node-attribute-equality-string ", uc);
                cont.putDocument(txn, "atr", content, uc);
                res = mgr.query(txn, query, qc);
                res.delete();
                res = mgr.query(txn, "collection('" + cname + 
                        "')//atrSiteConfigEntry[@siteConfigSiteID=\"1\"]", qc);
            } else {
                cont.setAutoIndexing(false);
                cont.addIndex("", "siteConfigSiteID","node-attribute-equality-string ", uc);
                cont.putDocument("atr", content, uc);
                res = mgr.query(query, qc);
                res.delete();
                res = mgr.query("collection('" + cname + 
                        "')//atrSiteConfigEntry[@siteConfigSiteID=\"1\"]", qc);
            }
            assertEquals(1, res.size());
        } finally {
            if (res != null) res.delete();
            if (txn != null) txn.commit();
        }
    }

    void initXQueryUpdateTests(String uri, String name, String index) throws XmlException
    {
        String str = "<root><elem attr=\"0\"/><elem attr=\"1\"/></root>";
        cont.addIndex(uri, name, index);
        cont.putDocument(docName, str);
    }

    void executeXQueryUpdate(String query) throws XmlException
    {
        XmlQueryContext context = mgr.createQueryContext();
        XmlTransaction txn = null;
        XmlResults res = null;
        try {
            if (hp.isTransactional()) {
                txn = mgr.createTransaction();
                res = mgr.query(txn, query, context);
            } else {
                res = mgr.query(query, context); 
            }
        } finally {
            if (res != null) res.delete();
            if (txn != null) txn.commit();
        }
    }

    void testXQueryUpdateIndex(String index, String uri, String nodeName, 
            int results, XmlValue val) throws XmlException
            {
        XmlQueryContext context = mgr.createQueryContext();
        XmlTransaction txn = null;
        XmlIndexLookup xil = null;
        XmlResults res = null;
        try {
            xil = mgr.createIndexLookup(cont, uri, nodeName, index, val);
            if (hp.isTransactional()) {
                txn = mgr.createTransaction();
                res = xil.execute(txn, context);
            } else {
                res = xil.execute(context);
            }
            assertEquals(results, res.size());
        } finally {
            if (res != null) res.delete();
            if (xil != null) xil.delete();
            if (txn != null) txn.commit();
        }
            }

    @Test
    public void testDeleteElement() throws Throwable {
        String query = "delete node collection('" + cname + "')/root/elem[1]";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());

        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());
    }

    @Test
    public void testDeleteAttribute() throws Throwable{
        String query = "delete node collection('" + cname + "')/root/elem/@attr[. = \"1\"]";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));

        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));
    }

    @Test
    public void testReplaceElement() throws Throwable{
        String query = "replace node collection('" + cname + "')/root/elem[1] with <elem1/>";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());


        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());
    }

    @Test
    public void testReplaceAttribute() throws Throwable{
        String query = "replace node collection('" + cname + "')/root/elem/@attr[. = \"1\"] with attribute attr1 {\"2\"}";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));

        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));
    }

    @Test
    public void testRenameElement() throws Throwable{
        String query = "rename node collection('" + cname + "')/root/elem[1] as \"elem1\"";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());


        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "elem", "node-element-presence-node");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 1, new XmlValue());
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-element-presence-node", 
                "", "elem", 0, new XmlValue());
    }

    @Test
    public void testRenameAttribute() throws Throwable{
        String query = "rename node collection('" + cname + "')/root/elem/@attr[. = \"1\"] as \"attr1\"";
        // Test with default index type for the given container
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));

        XmlContainerConfig config = cont.getContainerConfig();
        cont.delete();
        mgr.removeContainer(cname);

        // Test with non-default index type for the given container
        int itype;
        if (hp.isNodeContainer()) 
            itype = XmlContainerConfig.Off;
        else
            itype = XmlContainerConfig.On;
        config.setIndexNodes(itype);
        cont = hp.createContainer(cname, mgr, config);
        initXQueryUpdateTests("", "attr", "node-attribute-equality-string");
        executeXQueryUpdate(query);
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 1, new XmlValue("0"));
        testXQueryUpdateIndex("node-attribute-equality-string", 
                "", "attr", 0, new XmlValue("1"));
    }
}
