package dbxmltest;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertFalse;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.sleepycat.dbxml.*;

public class AutoIndexTest {
    private static final String containerName = "testAutoIndex.dbxml";
    private static String indexStringElem = 
	"node-element-equality-string node-element-equality-double";
    private static String indexStringAttr = 
	"node-attribute-equality-string node-attribute-equality-double";
    private static String contentRoot = "<root/>";
    private static String contentRoot_a = "<root><a/></root>";
    private static String contentRootAttr = "<root a1='1' a2='2'/>";
    private static String contentTree_c = "<root><a><b><c>c</c></b></a></root>";
    private static String contentMixedTree_c = "<root> <a> <b> <c>c<!--comment--></c>"
	+" </b></a></root>";
    private static String contentMixedCDATA = "<root><a>atext<!--comment--><![CDATA[acdata]]></a></root>";
    private static String contentMixed = "<r><a><b/><b/>2</a><a>1</a><c>2</c><c>3</c></r>";
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test Auto Index");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
	public void setUp() throws Throwable {
	if(hp != null)
	    TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), 
			    XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager(hp.getEnvironmentPath(), null);
    }

    @After
	public void tearDown() throws Throwable {
	if (cont != null) 
	    hp.closeContainer(cont);
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
	public static void tearDownClass() {
	System.out.println("Finished test Auto Index");
    }


    @Test
	public void basicAutoIndex() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, true, true);
	closeContainer(true, true); // re-create container
	cont.putDocument("doc", contentRoot_a);
	verifyIndex("", "root", false, false, true); // didn't add "root" index
	verifyIndex("", "a", false, true, true);  // added "a" index
	closeContainer(true, true); // re-create container
	cont.putDocument("doc", contentMixed);
	verifyIndex("", "a", false, true, true);  // added "a" index
	verifyIndex("", "b", false, true, true);  // added "b" index
	verifyIndex("", "c", false, true, true);  // added "c" index
    }

    @Test
	public void onoffAutoIndex() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, true, true);
	// turn off auto-indexing
	changeAutoIndexValue(false);

	// add more content that *would* auto-index
	cont.putDocument("doc1", contentRoot_a);
	verifyIndex("", "root", false,  // isAttr
		    true,  // exists (didn't remove it)
		    false); // autoindex is off
	verifyIndex("", "a", false,  // isAttr
		    false,  // did not get added
		    false); // autoindex is off
    }

    // test using container-based interface
    @Test
	public void onoffAutoIndex1() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentRoot); // will auto-commit
	verifyIndex("", "root", false, true, true);
	boolean val;
	XmlTransaction txn = null;
	if (hp.isTransactional())
	    txn = mgr.createTransaction();

	if (txn != null)
	    val = cont.getAutoIndexing(txn);
	else
	    val = cont.getAutoIndexing();

	assertTrue(val);
	// turn off auto-indexing
	changeAutoIndexValue1(txn, false);
	if (txn != null)
	    val = cont.getAutoIndexing(txn);
	else
	    val = cont.getAutoIndexing();
	assertFalse(val);

	// add more content that *would* auto-index
	if (txn != null) {
	    cont.putDocument(txn, "doc1", contentRoot_a);
	    txn.commit();
	} else
	    cont.putDocument("doc1", contentRoot_a);

	verifyIndex("", "root", false,  // isAttr
		    true,  // exists (didn't remove it)
		    false); // autoindex is off
	verifyIndex("", "a", false,  // isAttr
		    false,  // did not get added
		    false); // autoindex is off
    }


    @Test
	public void attrAutoIndex() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentRootAttr);
	verifyIndex("", "root", false /*isattr*/ , 
		    true /*exists*/, 
		    true /*autoindexon*/);
	verifyIndex("", "a1", true /*isattr*/ , 
		    true /*exists*/, 
		    true /*autoindexon*/);
	verifyIndex("", "a2", true /*isattr*/ , 
		    true /*exists*/, 
		    true /*autoindexon*/);
    }

    @Test
	public void treeAutoIndex() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentTree_c);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "a", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "b", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "c", false, // isAttr
		    true, // exists
		    true); // autoindex on
    }

    @Test
	public void treeAutoIndexMixed() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentMixedTree_c);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "a", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "b", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "c", false, // isAttr
		    true, // exists
		    true); // autoindex on
    }

    @Test
	public void treeAutoIndexCDATA() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentMixedCDATA);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	verifyIndex("", "a", false, // isAttr
		    true, // exists
		    true); // autoindex on
    }

    @Test
	public void updateDocAutoIndex() throws XmlException {
	closeContainer(true, true); // create container
	changeAutoIndexValue(false); // turn it off
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off
	XmlDocument doc = cont.getDocument("doc");
	// turn it back on and update a document
	changeAutoIndexValue(true);
	doc.setContent(contentRoot_a);
	cont.updateDocument(doc);
	verifyIndex("", "a", false, // isAttr
		    true, // index exists
		    true); // autoindex on
    }

    // XQuery Update test cases

    @Test // insert simple node
	public void xqInsertElement() throws XmlException {
	closeContainer(true, true); // create container
	changeAutoIndexValue(false); // turn it off to insert root
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off

	// turn it back on and insert a node.
	changeAutoIndexValue(true);
	String query = "for $i in collection('container')/root" +  
	    " return insert node <a>text</a> into $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	verifyIndex("", "a", false, // isAttr
		    true, // index exists
		    true); // autoindex on
	// ensure an index wasn't added for root
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
    }

    @Test // insert simple attribute
	public void xqInsertAttribute() throws XmlException {
	closeContainer(true, true); // create container
	changeAutoIndexValue(false); // turn it off to insert root
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off

	// turn it back on and insert an attribute
	changeAutoIndexValue(true);
	String query = "for $i in collection('container')/root" +  
	    " return insert node attribute {'attr'} {'value'} into $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	verifyIndex("", "attr", true, // isAttr
		    true, // index exists
		    true); // autoindex on
	// ensure an index wasn't added for root
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
    }

    @Test // insert attribute via content
	public void xqInsertAttributeContent() throws XmlException {
	closeContainer(true, true); // create container
	changeAutoIndexValue(false); // turn it off to insert root
	cont.putDocument("doc", contentRoot);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off

	// turn it back on and insert an attribute
	changeAutoIndexValue(true);
	String query = "for $i in collection('container')/root" +  
	    " return insert node <a attr='value'><b/></a> into $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	verifyIndex("", "attr", true, // isAttr
		    true, // index exists
		    true); // autoindex on
	// ensure an index wasn't added for root
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	// index should exist for 'b' as well
	verifyIndex("", "b", false, // isAttr
		    true, // index exists
		    true); // autoindex on
    }

    @Test // replace content and ensure new index(es) added
	public void xqReplaceNode() throws XmlException {
	closeContainer(true, true); // create container
	changeAutoIndexValue(false); // turn it off to insert root
	cont.putDocument("doc", contentTree_c);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off
	verifyIndex("", "c", false, // isAttr
		    false, // doesn't exist
		    false); // autoindex off

	// turn it back on and replace content
	changeAutoIndexValue(true);
	String query = "for $i in collection('container')/root/a/b" +  
	    " return replace node $i with <x><y>ytext</y><z>ztext</z></x>";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	// ensure root did not get an index
	verifyIndex("", "root", false, // isAttr
		    false, // index doesn't exist
		    true); // autoindex on
	// ensure indexes for y and z
	verifyIndex("", "y", false, // isAttr
		    true, // exists
		    true); // autoindex on
	verifyIndex("", "z", false, // isAttr
		    true, // exists
		    true); // autoindex on
    }

    @Test // make a new leaf from non-leaf -- this case
    // explicitly does not add an index on the assumption
    // that the "leaf-ness" of the new node is not long-term
	public void xqRemoveNode() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", contentTree_c);
	verifyIndex("", "root", false, // isAttr
		    false, // doesn't exist
		    true); // autoindex on
	// 'c' is a leaf
	verifyIndex("", "c", false, // isAttr
		    true, // doesn't exist
		    true); // autoindex on

	// remove b, making 'a' a leaf
	String query = "for $i in collection('container')/root/a/b" +  
	    " return delete node $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	/*
	 * Documents in whole document containers are fully reindexed
	 * on an update, so if it is a whole document container "a"
	 * gets an index, if not it does not.
	 */
	boolean indexExists = !hp.isNodeContainer();
	// ensure root did not get an index
	verifyIndex("", "root", false, // isAttr
		    false, // no index
		    true); // autoindex on
	// ensure a is indexed properly
	verifyIndex("", "a", false, // isAttr
		    indexExists, // new index
		    true); // autoindex on
    }


    @Test // insert node into empty document
	public void xqInsertElementIntoEmptyDoc() throws XmlException {
	closeContainer(true, true); // create container
	cont.putDocument("doc", ""); // no content
	String query = "for $i in collection('container')" +  
	    " return insert node <a>text</a> into $i";
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults res = mgr.query(query, qc);
	res.delete();
	verifyIndex("", "a", false, // isAttr
		    true, // index exists
		    true); // autoindex on
    }

    /*
     * Utility methods for these test cases
     */
    protected void closeContainer(boolean remove, boolean create) throws XmlException {
	if (cont != null)
	    hp.closeContainer(cont);
	if (remove) {
	    // ignore exceptions for removal
	    try { mgr.removeContainer(containerName); } catch (XmlException e) {}
	}
	cont = null;
	if (create) {
	    cont = hp.createContainer(containerName, mgr);
	    cont.addAlias("container");
	}
    }

    protected void changeAutoIndexValue(boolean value)
	throws XmlException {
	assertTrue(cont != null);
	XmlIndexSpecification is = cont.getIndexSpecification();
	is.setAutoIndexing(value);
	cont.setIndexSpecification(is);
    }

    protected void changeAutoIndexValue1(XmlTransaction txn, boolean value)
	throws XmlException {
	assertTrue(cont != null);
	if (txn == null)
	    cont.setAutoIndexing(value);
	else
	    cont.setAutoIndexing(txn, value);
    }

    protected boolean checkIndex(String index, boolean isAttr) {
	if (isAttr) return (index.equals(indexStringAttr));
	else return (index.equals(indexStringElem));
    }
    
    // verify a particular name has an index (or not)
    // does not require transaction (assumes single threaded
    // and no active txns)
    protected void verifyIndex(String uri, String name, 
			       boolean isAttr, boolean verifyExists,
			       boolean autoIndexOn)
	throws XmlException {
	assertTrue(cont != null);
	XmlIndexSpecification is = cont.getIndexSpecification();
	assertTrue(is.getAutoIndexing() == autoIndexOn);
	XmlIndexDeclaration id = is.find(uri, name);
	assertTrue((verifyExists && checkIndex(id.index, isAttr)) ||
		   (id == null));
	is.delete();
    }

};
