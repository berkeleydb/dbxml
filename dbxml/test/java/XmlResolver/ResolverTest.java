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

import java.io.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlValue;
import com.sleepycat.dbxml.XmlExternalFunction;
import com.sleepycat.dbxml.XmlArguments;

public class ResolverTest {
    // Container name;
    private static final String CON_NAME = "testData.dbxml";
    private TestConfig hp = null;
    private XmlManager mgr = null;
    private XmlContainer cont = null;

    @BeforeClass
	public static void setupClass() {
	System.out.println("Begin test XmlResolver!");
    }

    @Before
	public void setUp() throws Throwable {
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
			    XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
	XmlContainerConfig conf = new XmlContainerConfig();
	conf.setAllowValidation(true);
	if(hp.isTransactional())
	    conf.setTransactional(true);
	if(!hp.isNodeContainer())
		conf.setContainerType(XmlContainer.WholedocContainer);
	cont = hp.createContainer(CON_NAME, mgr, conf);
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
	System.out.println("Finished test XmlResolver!");
    }

    @Test
    public void TestResolveDocument() throws Throwable {
	XmlTransaction txn = null;
	TestResolver test = new TestResolver("stuff.xml", "<root>stuff</root>");
	String queryString = "doc('http://fake.uri.com/stuff.xml')/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults res = null;
	try{
	    mgr.registerResolver(test);
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		res = mgr.query(txn, queryString, context);
	    }else
		res = mgr.query(queryString, context);
	    XmlValue reslt = res.next();
	    assertTrue(reslt.isNode());
	    assertTrue(reslt.asString().equals("<root>stuff</root>"));
	    reslt = new XmlValue();
	    assertTrue(test.resolveDocument(txn, mgr, "stuff.xml", reslt));
	    assertTrue(reslt.isNode());
	    assertTrue(reslt.asString().equals("<root>stuff</root>"));
	}catch(XmlException e){
	    throw e;
	}finally{
	    if(res != null) res.delete();
	    test.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test resolveCollection()
     * 
     * 
     */
    @Test
	public void testResolveCollection() throws Throwable {
	XmlTransaction txn = null;
	TestResolver test = new TestResolver("stuff.xml", "<root>stuff</root>");
	mgr.registerResolver(test);
	String queryString = "collection('http://fake.uri.com/dbxml')/*";
	XmlQueryContext context = mgr.createQueryContext();
	XmlResults res = null;
	try {
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		res = mgr.query(txn, queryString, context);
	    }else
		res = mgr.query(queryString, context);
	    assertEquals(res.size(), 1);
	    XmlValue val = res.next();
	    assertTrue(val.isNode());
	    assertTrue(val.asString().equals("<root>stuff</root>"));
	    res.delete();
	    res = null;
	    res = mgr.createResults();
	    assertTrue(test.resolveCollection(txn, mgr, "stuff.xml", res));
	    assertEquals(res.size(), 1);
	} catch (XmlException e) {
	    throw e;
	}finally{
	    res.delete();
	    test.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test resolveEntity()
     * 
     */
    @Test
	public void testResolveEntity() throws Throwable {
	XmlTransaction txn = null;
	TestResolver test = new TestResolver();
	XmlDocument doc = mgr.createDocument();
	doc.setName("doc1.xml");
	String str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	    + "<!DOCTYPE root SYSTEM \"http://fake.uri.com"
	    + "/my.dtd\"><root>&stuff;</root>";
	doc.setContent(str);

	try {
	    mgr.registerResolver(test);
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
		assertEquals(str, cont.getDocument(txn, "doc1.xml").getContentAsString());
	    }else{
		cont.putDocument(doc);
		assertEquals(str, cont.getDocument("doc1.xml").getContentAsString());
	    }
	} catch (XmlException e) {
	    throw e;
	}finally{
	    if(hp.isTransactional() && txn != null) txn.commit();
	    test.delete();
	}
    }

    /*
     * Test resolveSchema()
     * 
     */
    @Test
	public void testResolveSchema() throws Throwable {
	XmlTransaction txn = null;
	TestResolver test = new TestResolver();
	XmlDocument doc = mgr.createDocument();
	doc.setName("doc1.xml");
	String str = "<?xml version=\"1.0\"?><root xmlns=\"http://fake.uri."
	    + "schema\" xmlns:xsi=\"http://www.w3.org/2001/XML"
	    + "Schema-instance\" xsi:schemaLocation=\"http://"
	    + "fake.uri.schema root.xsd\"></root>";
	doc.setContent(str);
	try {
	    mgr.registerResolver(test);
	    if(hp.isTransactional()){
		txn = mgr.createTransaction();
		cont.putDocument(txn, doc);
	    }else
		cont.putDocument(doc);
	} catch (XmlException e) {
	    test.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	    throw e;
	}

	//put the not valid xml 
	XmlDocument doc2 = mgr.createDocument();
	str = "<a xmlns=\"http://fake.uri.schema\" xmlns:xsi=\"http://"
	    + "www.w3.org/2001/XMLSchema-instance\" xsi:schemaL"
	    + "ocation=\"http://fake.uri.schema note.xsd\"><b/></a>";
	doc2.setName("fail.xml");
	doc2.setContent(str);
	try {
	    if(hp.isTransactional())
		cont.putDocument(txn, doc2);
	    else
		cont.putDocument(doc2);
	    fail("Fail in testResolveSchema()");
	} catch (XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.INDEXER_PARSER_ERROR);
	}finally{
	    test.delete();
	    if(hp.isTransactional() && txn != null) txn.commit();
	}
    }

    /*
     * Test resolveModule & resolveModuleLocation
     * 
     */
    @Test
    @Ignore
	public void testResolveModule() throws Throwable {
	TestResolver test = new TestResolver();
	String qs = "import module namespace test = 'http://test.module';\n\n test:function()";
	XmlQueryContext context = mgr.createQueryContext();

	XmlResults res = null;
	try{
		mgr.registerResolver(test);
	    res = mgr.query(qs, context);
	    XmlValue vl = res.next();
	    assertEquals(res.size(), 1);
	    assertEquals(vl.asString(), "module");
	}catch (XmlException e){
	    test.delete();
	    throw e;
	}finally{
	    if(res != null)res.delete();
	    res = null;
	}

	//not vaild query module
	qs = "import module namespace test = 'http://test.module';\n\n test:no()";
	try{
	    res = mgr.query(qs, context);
	    fail("Fail in testResolveModule()");
	}catch (XmlException e){
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	}finally{
	    test.delete();
	    if(res != null)res.delete();
	    res = null;
	}
    }

    /*
     * Test resolveModuleLocation
     * 
     */
    @Test
    @Ignore
	public void testResolveModuleLocation() throws Throwable {
	TestResolver test = new TestResolver();
	XmlManager mgr = new XmlManager();
	mgr.registerResolver(test);
	String qs = "import module namespace test = 'http://test.module';\n\n test:function()";
	XmlQueryContext context = mgr.createQueryContext();

	XmlResults res = null;
	try{
	    res = mgr.query(qs, context);
	    XmlValue vl = res.next();
	    assertEquals(res.size(), 1);
	    assertEquals(vl.asString(), "module");
	}catch (XmlException e){
	    test.delete();
	    if(res != null) res.delete();
	    mgr.delete();
	    throw e;
	}
	res.delete();
	res = null;

	//not vaild query module
	qs = "import module namespace test = 'http://test.module';\n\n test:no()";
	try{
	    res = mgr.query(qs, context);
	    res.delete();
	    test.delete();
	    mgr.delete();
	    fail("Fail in testResolveModule()");
	}catch (XmlException e){
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	}
	XmlResults results = mgr.createResults();
	XmlTransaction txn = null;
	try{
	    test.resolveModuleLocation(txn, mgr, "", results);

	    assertEquals(results.size(), 1);
	    assertEquals(results.next().asString(), "http://dbxmltest.test/test");
	}catch(XmlException e){
	    throw e;
	}finally{
	    test.delete();
	    results.delete();
	    mgr.delete();
	}
    }

    /*
     * Test resolving an external function when not using an XmlResolver
     */
    @Test
    public void testResolveExternalFunction1() throws Throwable
    {
	String query =
	    "declare function local:myfunc($a as xs:string, $b as xs:decimal) as xs:string external;\n" +
	    "local:myfunc('hello', 5)";

	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    context = mgr.createQueryContext();
	    res = mgr.query(query, context);
	    fail("Resolution of the external function should fail");
	}
	catch(XmlException e) {
	    assertNotNull(e.getMessage());
	    assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	}
	finally {
	    if(res != null) res.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() returning null
     */
    @Test
    public void testResolveExternalFunction2() throws Throwable
    {
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    test = new TestResolver();
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:string, $b as xs:decimal) as xs:string external;\n" +
		"local:myfunc('hello', 5)";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		fail("Resolution of the external function should fail");
	    }
	    catch(XmlException e) {
		assertNotNull(e.getMessage());
		assertEquals(e.getErrorCode(), XmlException.QUERY_PARSER_ERROR);
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() returning an XmlExternalFunction instance
     */
    @Test
    public void testResolveExternalFunction3() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
			if (res != null) res.delete();
		    res = args.getArgument(0);
		    return res;
		    }

		    public void close() throws XmlException {
			if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:string, $b as xs:decimal) as xs:string external;\n" +
		"local:myfunc('hello', 5)";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 1);
		XmlValue vl = res.next();
		assertEquals(vl.asString(), "hello");
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() returning an XmlExternalFunction instance
     */
    @Test
    public void testResolveExternalFunction4() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
			if (res != null) res.delete();
		    res = mgr.createResults();

			XmlResults arg = args.getArgument(0);
			XmlValue val;
			while((val = arg.next()) != null) {
			    res.add(val);
			}
			arg.delete();

			arg = args.getArgument(1);
			while((val = arg.next()) != null) {
			    res.add(val);
			}
			arg.delete();

			return res;
		    }

		    public void close() throws XmlException {
			if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:string, $b as xs:decimal) as item()* external;\n" +
		"local:myfunc('hello', 5)";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 2);
		XmlValue vl = res.next();
		assertEquals(vl.asString(), "hello");
		vl = res.next();
		assertEquals((int)vl.asNumber(), 5);
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() and XmlArguments::getNumberOfArgs()
     */
    @Test
    public void testResolveExternalFunction5() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
		    if (res != null) res.delete();
		    res = mgr.createResults();
			res.add(new XmlValue(args.getNumberOfArgs()));
			return res;
		    }

		    public void close() throws XmlException {
			if(res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc0() as xs:double external;\n" +
		"declare function local:myfunc1($a as xs:string) as xs:double external;\n" +
		"declare function local:myfunc2($a as xs:string, $b as xs:decimal) as xs:double external;\n" +
		"local:myfunc2('hello', 5),\n" +
		"local:myfunc0(),\n" +
		"local:myfunc1('hello')";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 3);
		XmlValue vl = res.next();
		assertEquals((int)vl.asNumber(), 2);
		vl = res.next();
		assertEquals((int)vl.asNumber(), 0);
		vl = res.next();
		assertEquals((int)vl.asNumber(), 1);
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() passing bad arguments to the external function
     */
    @Test
    public void testResolveExternalFunctionBadArgs() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
			try {
			    return args.getArgument(0);
			}
			catch(XmlException e) {
				if (res != null) res.delete();
			    res = mgr.createResults();
			    res.add(new XmlValue("caught"));
			    return res;
			}
		    }

		    public void close() throws XmlException {
			if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:decimal) as item()* external;\n" +
		"local:myfunc('hello')";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		fail("Type checking of the function argument should fail.");
	    }
	    catch(XmlException e) {
		assertNotNull(e.getMessage().indexOf("XPTY0004") != -1);
		assertEquals(XmlException.QUERY_PARSER_ERROR, e.getErrorCode());
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test catching an error in the evaluation of an external function argument
     */
    @Test
    public void testResolveExternalFunctionArgCatch() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
			try {
			    return args.getArgument(0);
			}
			catch(XmlException e) {
			    assertNotNull(e.getMessage().indexOf("FOER0000") != -1);
			    assertEquals(XmlException.QUERY_EVALUATION_ERROR, e.getErrorCode());

			    if (res != null) res.delete();
			    res = mgr.createResults();
			    res.add(new XmlValue("caught"));
			    return res;
			}
		    }

		    public void close() throws XmlException {
		    if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:decimal) as item()* external;\n" +
		"local:myfunc(error())";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 1);
		XmlValue vl = res.next();
		assertEquals(vl.asString(), "caught");
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test XmlResolver.resolveExternalFunction() returning a result that fails type checking
     */
    @Test
    public void testResolveExternalFunctionBadReturn() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
		    if (res != null) res.delete();
		    res = mgr.createResults();
			res.add(new XmlValue("not decimal"));
			return res;
		    }

		    public void close() throws XmlException {
		    if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as xs:decimal) as xs:decimal external;\n" +
		"local:myfunc(5)";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		fail("Type checking of the function result should fail.");
	    }
	    catch(XmlException e) {
		assertNotNull(e.getMessage().indexOf("XPTY0004") != -1);
		assertEquals(XmlException.QUERY_EVALUATION_ERROR, e.getErrorCode());
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test handling nodes in an external function
     */
    @Test
    public void testResolveExternalFunctionNodes1() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
			if (res != null) res.delete();
		    res = args.getArgument(0);
			return res;
		    }

		    public void close() throws XmlException {
		    if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as node()*) as node()* external;\n" +
		"local:myfunc((<a><b/><c><d/></c></a>, <e><f/></e>))";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 2);

		XmlValue vl = res.next();
		assertTrue(vl.isNode());
		assertEquals(vl.getNodeName(), "a");
		assertTrue(vl.getParentNode().isNull());
		assertEquals(vl.getFirstChild().getNodeName(), "b");

		vl = res.next();
		assertTrue(vl.isNode());
		assertEquals(vl.getNodeName(), "e");
		assertTrue(vl.getParentNode().isNull());
		assertEquals(vl.getFirstChild().getNodeName(), "f");
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

    /*
     * Test handling nodes in an external function
     */
     
    @Test
    public void testResolveExternalFunctionNodes2() throws Throwable
    {
	XmlExternalFunction func = null;
	TestResolver test = null;
	XmlQueryContext context = null;
	XmlResults res = null;
	try {
	    func = new XmlExternalFunction() {
	    	private XmlResults res;
		    public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
		    if (res != null) res.delete();
		    res = mgr.createResults();

			XmlResults arg = args.getArgument(0);
			XmlValue val;
			while((val = arg.next()) != null) {
			    if(val.isNode()) {
				XmlValue child = val.getFirstChild();
				while(child != null && !child.isNull()) {
				    res.add(child);
				    child = child.getNextSibling();
				}
			    }
			}
			arg.delete();

			return res;
		    }

		    public void close() throws XmlException {
		    if (res != null) res.delete();
		    }
		};
	    test = new TestResolver(func);
	    mgr.registerResolver(test);

	    String query =
		"declare function local:myfunc($a as node()*) as node()* external;\n" +
		"local:myfunc((<a><b/><c><d/></c></a>, <e><f/></e>))";

	    context = mgr.createQueryContext();
	    try{
		res = mgr.query(query, context);
		assertEquals(res.size(), 3);

		XmlValue vl = res.next();
		assertTrue(vl.isNode());
		assertEquals(vl.getNodeName(), "b");
		assertEquals(vl.getParentNode().getNodeName(), "a");
		assertTrue(vl.getFirstChild().isNull());

		vl = res.next();
		assertTrue(vl.isNode());
		assertEquals(vl.getNodeName(), "c");
		assertEquals(vl.getParentNode().getNodeName(), "a");
		assertEquals(vl.getFirstChild().getNodeName(), "d");

		vl = res.next();
		assertTrue(vl.isNode());
		assertEquals(vl.getNodeName(), "f");
		assertEquals(vl.getParentNode().getNodeName(), "e");
		assertTrue(vl.getFirstChild().isNull());
	    }
	    catch(XmlException e) {
		throw e;
	    }
	}
	finally {
	    if(res != null) res.delete();
	    if(test != null) test.delete();
	    if(func != null) func.delete();
	}
    }

}
