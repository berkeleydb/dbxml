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
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertTrue;
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
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlQueryExpression;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlValue;
import com.sleepycat.dbxml.XmlDocumentConfig;
import java.util.*;
/*
 * Test for class XmlValue
 * 
 * 
 */

public class ValueTest {
	private XmlContainer cont = null;
	private XmlManager mgr = null;
	private TestConfig hp = null;
	private XmlTransaction txn = null;
	private Vector<XmlResults>res = null;

	// Test all types of XmlValue
	XmlValue vl_NONE;

	// Test all NODE types
	/*
	 * ELEMENT_NODE = 1, ATTRIBUTE_NODE = 2, TEXT_NODE = 3, CDATA_SECTION_NODE =
	 * 4, ENTITY_REFERENCE_NODE = 5, ENTITY_NODE = 6,
	 * PROCESSING_INSTRUCTION_NODE = 7, COMMENT_NODE = 8, DOCUMENT_NODE = 9,
	 * DOCUMENT_TYPE_NODE = 10, DOCUMENT_FRAGMENT_NODE = 11, NOTATION_NODE = 12
	 * 
	 * Except DOCUMENT_TYPE_NODE, DOCUMENT_FRAGMENT_NODE, NOTATION_NODE
	 */
	XmlValue vl_NODE_ELE;
	XmlValue vl_NODE_ATT;
	XmlValue vl_NODE_TEX;
	XmlValue vl_NODE_CDA;
	XmlValue vl_NODE_ENTR;
	XmlValue vl_NODE_ENTN;
	XmlValue vl_NODE_PRO;
	XmlValue vl_NODE_COM;
	XmlValue vl_NODE_DOCN;

	// Other types XmlValue

	XmlValue vl_ANY_URI;
	XmlValue vl_BASE_64_BINARY;
	XmlValue vl_BOOLEAN;
	XmlValue vl_DATE;
	XmlValue vl_DATE_TIME;
	XmlValue vl_DAY_TIME_DURATION;
	XmlValue vl_DECIMAL;
	XmlValue vl_DECIMAL_SHORT;
	XmlValue vl_DOUBLE;
	XmlValue vl_DURATION;
	XmlValue vl_FLOAT;
	XmlValue vl_G_DAY;
	XmlValue vl_G_MONTH;
	XmlValue vl_G_MONTH_DAY;
	XmlValue vl_G_YEAR;
	XmlValue vl_G_YEAR_MONTH;
	XmlValue vl_HEX_BINARY;
	XmlValue vl_NOTATION;
	XmlValue vl_QNAME;
	XmlValue vl_STRING;
	XmlValue vl_TIME;
	XmlValue vl_YEAR_MONTH_DURATION;
	XmlValue vl_UNTYPED_ATOMIC;

	//Nodes constructed from XQuery constructors
	XmlValue cn_NODE_ELE;
	XmlValue cn_NODE_ATT;
	XmlValue cn_NODE_TEX;
	XmlValue cn_NODE_PRO;
	XmlValue cn_NODE_COM;
	XmlValue cn_NODE_DOCN;
	
	//Nodes with prefix setting constructed from XQuery constructors
	// for test AtomicNodeValue.cpp
	XmlValue cn_NODE_ELE_P;
	XmlValue cn_NODE_ATT_P;
	
	//XmlEventWriter constructed stand alone CDATA node
	XmlValue cn_NODE_CDATA;

	//Other nodes
	XmlValue vl_NODE_EMPTY_DOCN;

	//Null Values
	XmlValue vl_NULL_STRING;
	XmlValue vl_NULL_BYTES;

	// Container name;
	private static final String CON_NAME = "testData.dbxml";

	// The xml document string
	private static String docString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		+ "<old:a_node xmlns:old=\"http://dbxmltest.test/test\" old:atr1=\"test\" atr2=\"test2\""
		+ "><!-- comment text --><b_node/><?audio-output beep?>"
		+ "<c_node>Other text</c_node><d_node><![CDATA[ text < or > or &]]></d_node></old:a_node>";

	// The xml document name
	private static String docName = "testDoc.xml";

	@BeforeClass
	public static void setupClass() {
		System.out.println("Begin test XmlValue!");
		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	}

	@Before
	public void setUp() throws Throwable {
		hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
				XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
		mgr = hp.createManager();
		cont = hp.createContainer(CON_NAME, mgr);
		if(XmlTestRunner.isTransactional())
			txn = mgr.createTransaction();

		String myQuery;
		XmlUpdateContext context = mgr.createUpdateContext();
		if(hp.isTransactional())
			cont.putDocument(txn, docName, docString, context);
		else
			cont.putDocument(docName, docString, context);

		// initial all types XmlValue
		try {
			XmlResults re = null;
			res = new Vector<XmlResults>();
			XmlQueryContext qc = mgr.createQueryContext();
			qc.setNamespace("old", "http://dbxmltest.test/test");
			// initial all types NODE types
			// construct them using query

			myQuery = "collection('testData.dbxml')/*:a_node";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_ELE = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')/old:a_node/@old:atr1";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_ATT = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')/old:a_node/c_node/text()";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_TEX = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')/old:a_node/d_node/text()";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_CDA = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')/old:a_node/processing-instruction()";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_PRO = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')/old:a_node/comment()";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_COM = re.next();
			res.add(re);

			myQuery = "collection('testData.dbxml')";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			vl_NODE_DOCN = re.next();
			res.add(re);

			//constructed element
			myQuery = "element foo { \"bar\" }";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_ELE = re.next();
			res.add(re);

			//constructed attribute
			myQuery = "attribute foo { \"bar\" }";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_ATT = re.next();
			res.add(re);

			//constructed text
			myQuery = "text { \"bar\" }";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_TEX = re.next();
			res.add(re);

			//constructed processing instruction
			myQuery = "<?foo bar?>";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_PRO = re.next();
			res.add(re);

			//constructed comment
			myQuery = "<!-- comment -->";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_COM = re.next();
			res.add(re);

			//constructed document
			myQuery = "document {<foo>bar</foo>}";
			if(hp.isTransactional())
				re = mgr.query(txn, myQuery, qc);
			else
				re = mgr.query(myQuery, qc);
			cn_NODE_DOCN = re.next();
			res.add(re);
					
			//constructed element with prefix
			myQuery = "element old:foo { \"bar\" }";
			if(hp.isTransactional())
			re = mgr.query(txn, myQuery, qc);
			else
			re = mgr.query(myQuery, qc);
			cn_NODE_ELE_P = re.next();
			res.add(re);
							
			//constructed attribute with prefix
			myQuery = "attribute old:foo { \"bar\" }";
			if(hp.isTransactional())
			re = mgr.query(txn, myQuery, qc);
			else
			re = mgr.query(myQuery, qc);
			cn_NODE_ATT_P = re.next();
			res.add(re);
			//constructed stand alone CDATA node
			XmlResults wResults = mgr.createResults();
			XmlEventWriter writer = wResults.asEventWriter();
			writer.writeText(XmlEventReader.CDATA, "cdata < and >");
			writer.close();
			cn_NODE_CDATA = wResults.next();
			res.add(wResults);

			// NONE
			vl_NONE = new XmlValue(XmlValue.NONE, (new String("test string")));

			// ANY_URI
			vl_ANY_URI = new XmlValue(XmlValue.ANY_URI, (new String(
			"http://dbxml.dbxml")));

			// BASE_64_BINARY
			vl_BASE_64_BINARY = new XmlValue(XmlValue.BASE_64_BINARY,
					(new String("FFFF")));

			// type BOOLEAN
			vl_BOOLEAN = new XmlValue(XmlValue.BOOLEAN, (new String("true")));

			// type DATE
			vl_DATE = new XmlValue(XmlValue.DATE, (new String("2007-09-10")));

			// type DATE_TIME
			vl_DATE_TIME = new XmlValue(XmlValue.DATE_TIME, (new String(
			"2002-10-08T03:46:27.278Z")));

			// DAY_TIME_DURATION
			vl_DAY_TIME_DURATION = new XmlValue(XmlValue.DAY_TIME_DURATION,
					(new String("P3DT10H30M")));

			// DECIMAL
			vl_DECIMAL = new XmlValue(XmlValue.DECIMAL, (new String(
			"00012345678.00")));

			// DECIMAL SHORT
			vl_DECIMAL_SHORT = new XmlValue("http://www.w3.org/2001/XMLSchema", "short", "43");

			// DOUBLE
			vl_DOUBLE = new XmlValue(XmlValue.DOUBLE, (new String("123456E5")));

			// DURATION
			vl_DURATION = new XmlValue(XmlValue.DURATION,
					(new String("P1Y2M4D")));

			// FLOAT
			vl_FLOAT = new XmlValue(XmlValue.FLOAT, (new String("1.23456E5")));

			// G_DAY
			vl_G_DAY = new XmlValue(XmlValue.G_DAY, (new String("---31")));

			// G_MONTH
			vl_G_MONTH = new XmlValue(XmlValue.G_MONTH, (new String("--02--")));

			// G_MONTH_DAY
			vl_G_MONTH_DAY = new XmlValue(XmlValue.G_MONTH_DAY, (new String(
			"--02-13")));

			// G_YEAR
			vl_G_YEAR = new XmlValue(XmlValue.G_YEAR, (new String("2007")));

			// G_YEAR_MONTH
			vl_G_YEAR_MONTH = new XmlValue(XmlValue.G_YEAR_MONTH, (new String(
			"2007-12")));

			// HEX_BINARY
			vl_HEX_BINARY = new XmlValue(XmlValue.HEX_BINARY, (new String(
			"0FB7")));

			// NOTION
			vl_NOTATION = new XmlValue(XmlValue.NOTATION, (new String(
			"http://www.xml.org:stuff.txt")));

			// QNAME
			vl_QNAME = new XmlValue(XmlValue.QNAME, (new String("xml:stuff")));

			// STRING
			vl_STRING = new XmlValue(XmlValue.STRING, (new String("for test")));

			// TIME
			vl_TIME = new XmlValue(XmlValue.TIME, (new String("20:24:50")));

			// YEAR_MONTH_DURATION
			vl_YEAR_MONTH_DURATION = new XmlValue(XmlValue.YEAR_MONTH_DURATION,
					(new String("P1Y2M")));

			// UNTYPED_ATOMIC
			vl_UNTYPED_ATOMIC = new XmlValue(XmlValue.UNTYPED_ATOMIC,
					(new String("Test String")));

			XmlDocument doc = mgr.createDocument();
			vl_NODE_EMPTY_DOCN = new XmlValue(doc);

			String str = null;
			vl_NULL_STRING = new XmlValue(str);

			byte[] bytes = null;
			vl_NULL_BYTES = new XmlValue(XmlValue.BINARY, bytes);

		} catch (XmlException e) {
			throw e;
		}

	}

	@After
	public void tearDown() throws Throwable {
		for(int i = 0; i < res.size(); i++)
			res.get(i).delete();
		res.clear();
		if(XmlTestRunner.isTransactional())
			txn.commit();
		hp.closeContainer(cont);
		hp.closeManager(mgr);
		hp.closeEnvironment();
		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	}

	@AfterClass
	public static void tearDownClass() {
		System.out.println("Finished test XmlValue!");
	}

	/*
	 * Test all kinds of XmlValue constructor
	 * 
	 */
	@Test
	public void testConstructor() throws Throwable {
		XmlValue vl = null;

		// test no parameter constructor
		vl = new XmlValue();
		assertTrue(vl.isNull());

		// test XmlValue(boolean v)
		vl = new XmlValue(true);
		assertTrue(vl.asBoolean());
		vl = new XmlValue(false);
		assertFalse(vl.asBoolean());

		// XmlValue(double v)
		vl = new XmlValue(1.1);
		assertEquals(1.1, vl.asNumber(), 0);

		/*---------------------------------------------------------------*/
		// Test for XmlValue(int type, byte[] dbt)
		// Every type should be tested
		/*
		 * NONE = 0, NODE = 3,
		 * 
		 * ANY_SIMPLE_TYPE = 10,
		 * 
		 * ANY_URI = 11, BASE_64_BINARY = 12, BOOLEAN = 13, DATE = 14, DATE_TIME =
		 * 15, DAY_TIME_DURATION = 16, DECIMAL = 17, DOUBLE = 18, DURATION = 19,
		 * FLOAT = 20, G_DAY = 21, G_MONTH = 22, G_MONTH_DAY = 23, G_YEAR = 24,
		 * G_YEAR_MONTH = 25, HEX_BINARY = 26, NOTATION = 27, QNAME = 28, STRING =
		 * 29, TIME = 30, YEAR_MONTH_DURATION= 31 UNTYPED_ATOMIC = 32
		 */

		// NODE
		vl = new XmlValue(XmlValue.NODE, (new String("test string")).getBytes());
		assertTrue(vl.isNull());

		// NONE
		vl = new XmlValue(XmlValue.NONE, (new String("test string"))
				.getBytes());
		assertTrue(vl.isNull());

		// ANY_URI
		vl = new XmlValue(XmlValue.ANY_URI, (new String(
		"http://dbxml.dbxml")).getBytes());
		assertEquals("http://dbxml.dbxml", vl.asString());

		// not valid XmlValue.ANY_URI
		try {
			vl = new XmlValue(XmlValue.ANY_URI, (new String("test stirng:"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// BASE_64_BINARY
		vl = new XmlValue(XmlValue.BASE_64_BINARY, (new String("FFFF"))
				.getBytes());
		assertEquals("FFFF", vl.asString());

		// type BOOLEAN
		vl = new XmlValue(XmlValue.BOOLEAN, (new String("true")).getBytes());
		assertTrue(vl.asBoolean());

		vl = new XmlValue(XmlValue.BOOLEAN, (new String("false"))
				.getBytes());
		assertFalse(vl.asBoolean());

		// test not valid boolean
		try {
			vl = new XmlValue(XmlValue.BOOLEAN, (new String("not a bool"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// type DATE
		vl = new XmlValue(XmlValue.DATE, (new String("2007-09-10"))
				.getBytes());
		assertEquals("2007-09-10", vl.asString());

		// not valid DATE
		try {
			vl = new XmlValue(XmlValue.DATE, (new String("not a date"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// type DATE_TIME
		vl = new XmlValue(XmlValue.DATE_TIME, (new String(
		"2002-10-08T03:46:27.278Z")).getBytes());
		assertEquals("2002-10-08T03:46:27.278Z", vl.asString());

		// not valid DATE_TIME
		try {
			vl = new XmlValue(XmlValue.DATE_TIME,
					(new String("not a datetime")).getBytes());
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DAY_TIME_DURATION
		vl = new XmlValue(XmlValue.DAY_TIME_DURATION, (new String(
		"P3DT10H30M")).getBytes());
		assertEquals("P3DT10H30M", vl.asString());

		// not valid DAY_TIME_DURATION
		try {
			vl = new XmlValue(XmlValue.DAY_TIME_DURATION, (new String(
					"not a datetime_duration")).getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DECIMAL
		vl = new XmlValue(XmlValue.DECIMAL, (new String("00012345678.00"))
				.getBytes());
		assertEquals("00012345678.00", vl.asString());

		// not valid DECIMAL
		try {
			vl = new XmlValue(XmlValue.DECIMAL, (new String("a string"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DOUBLE
		vl = new XmlValue(XmlValue.DOUBLE, (new String("123456E5"))
				.getBytes());
		assertEquals("123456E5", vl.asString());

		// not valid DOUBLE
		try {
			vl = new XmlValue(XmlValue.DOUBLE, (new String("a string"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DURATION
		vl = new XmlValue(XmlValue.DURATION, (new String("P1Y2M4D"))
				.getBytes());
		assertEquals("P1Y2M4D", vl.asString());

		// not valid DURATION
		try {
			vl = new XmlValue(XmlValue.DURATION, (new String("not duration"))
					.getBytes());
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// FLOAT
		vl = new XmlValue(XmlValue.FLOAT, (new String("1.23456E5"))
				.getBytes());
		assertEquals("1.23456E5", vl.asString());

		// not valid FLOAT
		try {
			vl = new XmlValue(XmlValue.FLOAT, (new String("not float"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_DAY
		vl = new XmlValue(XmlValue.G_DAY, (new String("---31")).getBytes());
		assertEquals("---31", vl.asString());
		vl = new XmlValue(XmlValue.G_DAY, (new String("---01")).getBytes());
		assertEquals("---01", vl.asString());

		// not valid G_DAY
		try {
			vl = new XmlValue(XmlValue.G_DAY, (new String("---35")).getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_DAY
		try {
			vl = new XmlValue(XmlValue.G_DAY, (new String("not a gday"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_MONTH
		vl = new XmlValue(XmlValue.G_MONTH, (new String("--02--"))
				.getBytes());
		assertEquals("--02--", vl.asString());

		// not valid G_MONTH
		try {
			vl = new XmlValue(XmlValue.G_MONTH, (new String("--18--"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_MONTH
		try {
			vl = new XmlValue(XmlValue.G_MONTH, (new String("not a gday"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_MONTH_DAY
		vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String("--02-13"))
				.getBytes());
		assertEquals("--02-13", vl.asString());

		// not valid G_MONTH_DAY
		try {
			vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String("--18-35"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_MONTH_DAY
		try {
			vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String(
					"not a g_month_day")).getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_YEAR
		vl = new XmlValue(XmlValue.G_YEAR, (new String("2007")).getBytes());
		assertEquals("2007", vl.asString());

		// not valid G_YEAR
		try {
			vl = new XmlValue(XmlValue.G_YEAR, (new String("not a gyear"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_YEAR_MONTH
		vl = new XmlValue(XmlValue.G_YEAR_MONTH, (new String("2007-12"))
				.getBytes());
		assertEquals("2007-12", vl.asString());

		// not valid G_YEAR_MONTH
		try {
			vl = new XmlValue(XmlValue.G_YEAR_MONTH, (new String(
					"not a g_year_month")).getBytes());
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// HEX_BINARY
		vl = new XmlValue(XmlValue.HEX_BINARY, (new String("0FB7"))
				.getBytes());
		assertEquals("0FB7", vl.asString());

		// not valid HEX_BINARY
		try {
			vl = new XmlValue(XmlValue.HEX_BINARY, (new String("test string"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// NOTION
		vl = new XmlValue(XmlValue.NOTATION, (new String(
		"http://www.xml.org:stuff.txt")).getBytes());
		assertEquals("http://www.xml.org:stuff.txt", vl.asString());

		// not valid NOTATION
		try {
			vl = new XmlValue(XmlValue.NOTATION, (new String("bad notation"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// QNAME
		vl = new XmlValue(XmlValue.QNAME, (new String("xml:stuff"))
				.getBytes());
		assertEquals("xml:stuff", vl.asString());

		// not valid QNAME
		try {
			vl = new XmlValue(XmlValue.QNAME, (new String("bad qname"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// STRING
		vl = new XmlValue(XmlValue.STRING, (new String("for test"))
				.getBytes());
		assertEquals("for test", vl.asString());

		// TIME
		vl = new XmlValue(XmlValue.TIME, (new String("20:24:50"))
				.getBytes());
		assertEquals("20:24:50", vl.asString());

		// not valid TIME
		try {
			vl = new XmlValue(XmlValue.TIME, (new String("35:24:50"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// YEAR_MONTH_DURATION
		vl = new XmlValue(XmlValue.DURATION, (new String("P1Y2M"))
				.getBytes());
		assertEquals("P1Y2M", vl.asString());

		// not valid YEAR_MONTH_DURATION
		try {
			vl = new XmlValue(XmlValue.YEAR_MONTH_DURATION, (new String("xyz"))
					.getBytes());
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// UNTYPED_ATOMIC
		vl = new XmlValue(XmlValue.UNTYPED_ATOMIC, (new String(
		"Test String")).getBytes());
		assertEquals("Test String", vl.asString());

		// End of Test for XmlValue(int type, byte[] dbt)
		/*---------------------------------------------------------------*/

		/*---------------------------------------------------------------*/
		// Test for XmlValue(int type, String v)
		// Test every possible type
		// NODE
		vl = new XmlValue(XmlValue.NODE, (new String("test string")));
		assertTrue(vl.isNull());

		// NONE
		vl = new XmlValue(XmlValue.NONE, (new String("test string")));
		assertTrue(vl.isNull());

		// ANY_URI
		vl = new XmlValue(XmlValue.ANY_URI, (new String(
		"http://dbxml.dbxml")));
		assertEquals("http://dbxml.dbxml", vl.asString());

		// not valid XmlValue.ANY_URI
		try {
			vl = new XmlValue(XmlValue.ANY_URI, (new String("test stirng")));
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// BASE_64_BINARY
		vl = new XmlValue(XmlValue.BASE_64_BINARY, (new String("FFFF")));
		assertEquals("FFFF", vl.asString());

		// type BOOLEAN
		vl = new XmlValue(XmlValue.BOOLEAN, (new String("true")));
		assertTrue(vl.asBoolean());

		vl = new XmlValue(XmlValue.BOOLEAN, (new String("false"))
				.getBytes());
		assertFalse(vl.asBoolean());

		// test not valid boolean
		try {
			vl = new XmlValue(XmlValue.BOOLEAN, (new String("not a bool")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// type DATE
		vl = new XmlValue(XmlValue.DATE, (new String("2007-09-10")));
		assertEquals("2007-09-10", vl.asString());

		// not valid DATE
		try {
			vl = new XmlValue(XmlValue.DATE, (new String("not a date")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// type DATE_TIME
		vl = new XmlValue(XmlValue.DATE_TIME, (new String(
		"2002-10-08T03:46:27.278Z")));
		assertEquals("2002-10-08T03:46:27.278Z", vl.asString());

		// not valid DATE_TIME
		try {
			vl = new XmlValue(XmlValue.DATE_TIME,
					(new String("not a datetime")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DAY_TIME_DURATION
		vl = new XmlValue(XmlValue.DAY_TIME_DURATION, (new String(
		"P3DT10H30M")));
		assertEquals(vl.asString(), "P3DT10H30M");

		// not valid DAY_TIME_DURATION
		try {
			vl = new XmlValue(XmlValue.DAY_TIME_DURATION, (new String(
					"not a datetime")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DECIMAL
		vl = new XmlValue(XmlValue.DECIMAL, (new String("00012345678.00")));
		assertEquals("00012345678.00", vl.asString());

		// not valid DECIMAL
		try {
			vl = new XmlValue(XmlValue.DECIMAL, (new String("a string")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DOUBLE
		vl = new XmlValue(XmlValue.DOUBLE, (new String("123456E5")));
		assertEquals("123456E5", vl.asString());

		// not valid DOUBLE
		try {
			vl = new XmlValue(XmlValue.DOUBLE, (new String("a string")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// DURATION
		vl = new XmlValue(XmlValue.DURATION, (new String("P1Y2M4D")));
		assertEquals("P1Y2M4D", vl.asString());

		// not valid DURATION
		try {
			vl = new XmlValue(XmlValue.DURATION, (new String("not duration")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// FLOAT
		vl = new XmlValue(XmlValue.FLOAT, (new String("1.23456E5")));
		assertEquals("1.23456E5", vl.asString());

		// not valid FLOAT
		try {
			vl = new XmlValue(XmlValue.FLOAT, (new String("not float")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_DAY
		vl = new XmlValue(XmlValue.G_DAY, (new String("---31")));
		assertEquals("---31", vl.asString());

		// not valid G_DAY
		try {
			vl = new XmlValue(XmlValue.G_DAY, (new String("---35")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_DAY
		try {
			vl = new XmlValue(XmlValue.G_DAY, (new String("not a gday")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_MONTH
		vl = new XmlValue(XmlValue.G_MONTH, (new String("--02--")));
		assertEquals("--02--", vl.asString());

		// not valid G_MONTH
		try {
			vl = new XmlValue(XmlValue.G_MONTH, (new String("--18--")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_MONTH
		try {
			vl = new XmlValue(XmlValue.G_MONTH, (new String("not a gday")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_MONTH_DAY
		vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String("--02-13")));
		assertEquals("--02-13", vl.asString());

		// not valid G_MONTH_DAY
		try {
			vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String("--18-35")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// not valid G_MONTH_DAY
		try {
			vl = new XmlValue(XmlValue.G_MONTH_DAY, (new String(
					"not a g_month_day")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_YEAR
		vl = new XmlValue(XmlValue.G_YEAR, (new String("2007")));
		assertEquals("2007", vl.asString());

		// not valid G_YEAR
		try {
			vl = new XmlValue(XmlValue.G_YEAR, (new String("not a gyear")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// G_YEAR_MONTH
		vl = new XmlValue(XmlValue.G_YEAR_MONTH, (new String("2007-12")));
		assertEquals("2007-12", vl.asString());

		// not valid G_YEAR_MONTH
		try {
			vl = new XmlValue(XmlValue.G_YEAR_MONTH, (new String(
					"not a g_year_month")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// HEX_BINARY
		vl = new XmlValue(XmlValue.HEX_BINARY, (new String("0FB7")));
		assertEquals("0FB7", vl.asString());

		// not valid HEX_BINARY
		try {
			vl = new XmlValue(XmlValue.HEX_BINARY, (new String("test string")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// NOTATION
		vl = new XmlValue(XmlValue.NOTATION, (new String(
		"http://www.xml.org:stuff.txt")));
		assertEquals("http://www.xml.org:stuff.txt", vl.asString());

		// not valid NOTATION
		try {
			vl = new XmlValue(XmlValue.NOTATION, (new String("bad notation")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// QNAME
		vl = new XmlValue(XmlValue.QNAME, (new String("xml:stuff")));
		assertEquals("xml:stuff", vl.asString());

		// not valid QNAME
		try {
			vl = new XmlValue(XmlValue.QNAME, (new String("bad qname")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// STRING
		vl = new XmlValue(XmlValue.STRING, (new String("for test")));
		assertEquals("for test", vl.asString());

		// TIME
		vl = new XmlValue(XmlValue.TIME, (new String("20:24:50")));
		assertEquals("20:24:50", vl.asString());

		// not valid TIME
		try {
			vl = new XmlValue(XmlValue.TIME, (new String("35:24:50")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// YEAR_MONTH_DURATION
		vl = new XmlValue(XmlValue.DURATION, (new String("P1Y2M")));
		assertEquals("P1Y2M", vl.asString());

		// not valid YEAR_MONTH_DURATION
		try {
			vl = new XmlValue(XmlValue.DURATION, (new String("xyz")));
			fail("Fail in testConstructor()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// UNTYPED_ATOMIC
		vl = new XmlValue(XmlValue.UNTYPED_ATOMIC, (new String(
		"Test String")));
		assertEquals("Test String", vl.asString());
		// End of Test for XmlValue(int type, String dbt)
		/*---------------------------------------------------------------*/

		// XmlValue(String v)
		vl = new XmlValue("test string");
		assertEquals("test string", vl.asString());

		// XmlValue(XmlDocument document)
		XmlDocument doc = mgr.createDocument();
		doc.setContent(docString);
		vl = new XmlValue(doc);
		assertEquals(docString, vl.asDocument().getContentAsString());

		// XmlValue(XmlValue other), Copy constructor.
		vl = new XmlValue("test string");
		XmlValue vvl = new XmlValue(vl);
		assertNotSame(vvl, vl);
		assertEquals("test string", vvl.asString());

		//Nulls
		String str = null;
		vl = new XmlValue(str);
		vl = new XmlValue(XmlValue.STRING, str);
		byte[] bytes = null;
		vl = new XmlValue(XmlValue.BINARY, bytes);
	}

	/*
	 * Test asBoolean()
	 * 
	 */
	@Test
	public void testAsBoolean() throws Throwable {

		// Test for null XmlValue
		try {
			vl_NONE.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// all node types
		try {
			vl_NODE_ELE.asBoolean();
			assertTrue(vl_NODE_ELE.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_ATT.asBoolean();
			assertTrue(vl_NODE_ATT.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_TEX.asBoolean();
			assertTrue(vl_NODE_TEX.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_CDA.asBoolean();
			assertTrue(vl_NODE_CDA.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_PRO.asBoolean();
			assertTrue(vl_NODE_PRO.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_COM.asBoolean();
			assertTrue(vl_NODE_PRO.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_DOCN.asBoolean();
			assertTrue(vl_NODE_DOCN.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertTrue(cn_NODE_ELE.asBoolean());

		assertTrue(cn_NODE_ATT.asBoolean());

		assertTrue(cn_NODE_TEX.asBoolean());

		assertTrue(cn_NODE_PRO.asBoolean());

		assertTrue(cn_NODE_PRO.asBoolean());

		assertTrue(cn_NODE_DOCN.asBoolean());

		// All value types
		try {
			vl_ANY_URI.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			assertTrue(vl_DECIMAL.asBoolean());
			vl_DECIMAL = new XmlValue(XmlValue.DECIMAL, "0");
			assertFalse(vl_DECIMAL.asBoolean());
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertTrue(vl_DECIMAL_SHORT.asBoolean());

		try {
			assertTrue(vl_DOUBLE.asBoolean());
			vl_DOUBLE = new XmlValue(XmlValue.DOUBLE, "0");
			assertFalse(vl_DOUBLE.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_DURATION.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			assertTrue(vl_FLOAT.asBoolean());
			vl_FLOAT = new XmlValue(XmlValue.FLOAT, "0");
			assertFalse(vl_FLOAT.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_G_DAY.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			this.vl_NOTATION.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			this.vl_QNAME.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			assertTrue(vl_STRING.asBoolean());
			vl_STRING = new XmlValue(XmlValue.STRING, "false");
			assertFalse(vl_STRING.asBoolean());
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_TIME.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.asBoolean();
			fail("Fail in testAsBoolean()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals(false, vl_NODE_EMPTY_DOCN.asBoolean());

		assertEquals(false, vl_NULL_STRING.asBoolean());
		try {
			assertEquals(false, vl_NULL_BYTES.asBoolean());
			fail("Failure in ValueTest.testAsBoolean().");
		} catch (XmlException e) {}
	}

	/*
	 * Test asDocument()
	 * 
	 */
	@Test
	public void testAsDocument() throws Throwable {

		XmlDocument doc = null;

		try {
			vl_NONE.asDocument();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// all node types
		try {
			doc = vl_NODE_ELE.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_ATT.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_TEX.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_CDA.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_PRO.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_COM.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			doc = vl_NODE_DOCN.asDocument();
			assertNotNull(doc);
			assertEquals(docName, doc.getName());
			doc = null;
		} catch (XmlException e) {
			throw e;
		}

		doc = cn_NODE_ELE.asDocument();
		assertNotNull(doc);

		doc = cn_NODE_ATT.asDocument();
		assertNull(doc);

		doc = cn_NODE_TEX.asDocument();
		assertNull(doc);

		doc = cn_NODE_PRO.asDocument();
		assertNull(doc);

		doc = cn_NODE_COM.asDocument();
		assertNull(doc);

		doc = cn_NODE_DOCN.asDocument();
		assertNotNull(doc);

		try {
			vl_ANY_URI.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			this.vl_NOTATION.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			this.vl_QNAME.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("", vl_NODE_EMPTY_DOCN.asDocument().getContentAsString());

		try {
			vl_NULL_STRING.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.asDocument();
			fail("Fail in testAsDocument()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for asEventReader()
	 * 
	 */
	@Test
	public void testAsEventReader() throws Throwable {

		try {
			vl_NONE.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			XmlEventReader reader = vl_NODE_ELE.asEventReader();
			assertNotNull(reader);
			reader.close();
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_ATT.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_TEX.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_CDA.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_PRO.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_COM.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			XmlEventReader myReader = vl_NODE_DOCN.asEventReader();
			myReader.next();
			assertEquals(XmlEventReader.StartDocument, myReader.getEventType());
			myReader.close();
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		XmlEventReader reader = null;
		try {
			reader = cn_NODE_ELE.asEventReader();
			assertNotNull(reader);
		} catch (XmlException e) {
			throw e;
		} finally {
			if (reader != null) reader.close();
			reader = null;
		}

		try {
			reader = cn_NODE_ATT.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		} finally {
			if (reader != null) reader.close();
			reader = null;
		}

		try {
			reader = cn_NODE_TEX.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}  finally {
			if (reader != null) reader.close();
			reader = null;
		}

		try {
			reader = cn_NODE_PRO.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}  finally {
			if (reader != null) reader.close();
			reader = null;
		}

		try {
			reader = cn_NODE_COM.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		} finally {
			if (reader != null) reader.close();
			reader = null;
		}

		try {
			reader = cn_NODE_DOCN.asEventReader();
			reader.next();
			assertEquals(XmlEventReader.StartDocument, reader.getEventType());
		} catch (XmlException e) {
			throw e;
		} finally {
			if (reader != null) reader.close();
			reader = null;
		}

		// Other types XmlValue
		try {
			vl_ANY_URI.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.asEventReader();
			fail("Failure in ValueTest.testAsEventReader().");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		} 

		try {
			vl_NULL_STRING.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.asEventReader();
			fail("Fail in testAsEventReader()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for asNumber()
	 * 
	 */
	@Test
	public void testAsNumber() throws Throwable {

		try {
			vl_NONE.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		//
		try {
			vl_NODE_ELE.asNumber();
			assertEquals(vl_NODE_ELE.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_ATT.asNumber();
			assertEquals(vl_NODE_ATT.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_TEX.asNumber();
			assertEquals(vl_NODE_TEX.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_CDA.asNumber();
			assertEquals(vl_NODE_CDA.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_PRO.asNumber();
			assertEquals(vl_NODE_PRO.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_COM.asNumber();
			assertEquals(vl_NODE_COM.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_NODE_DOCN.asNumber();
			assertEquals(vl_NODE_DOCN.asNumber(), Double.NaN, 0);
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.asNumber(), Double.NaN, 0);

		assertEquals(cn_NODE_ATT.asNumber(), Double.NaN, 0);

		assertEquals(cn_NODE_TEX.asNumber(), Double.NaN, 0);

		assertEquals(cn_NODE_PRO.asNumber(), Double.NaN, 0);

		assertEquals(cn_NODE_COM.asNumber(), Double.NaN, 0);

		assertEquals(cn_NODE_DOCN.asNumber(), Double.NaN, 0);

		// Other types XmlValue
		try {
			vl_ANY_URI.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.asNumber();
			assertEquals(vl_BOOLEAN.asNumber(), 1, 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_DATE.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.asNumber();
			assertEquals(1.2345678E7, vl_DECIMAL.asNumber(), 0);
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(43, vl_DECIMAL_SHORT.asNumber(), 0);

		try {
			vl_DOUBLE.asNumber();
			assertEquals(1.23456E10, vl_DOUBLE.asNumber(), 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_DURATION.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.asNumber();
			assertEquals(123456.0f, vl_FLOAT.asNumber(), 0);
		} catch (XmlException e) {
			throw e;
		}

		try {
			vl_G_DAY.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.asNumber();
			assertEquals(vl_STRING.asNumber(), Double.NaN, 0);
			vl_STRING = new XmlValue(XmlValue.STRING, "100");
			assertEquals(vl_STRING.asNumber(), 100, 0);
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.asNumber();
			fail("Fail in testAsNumber()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		vl_UNTYPED_ATOMIC.asNumber();
		assertEquals(vl_UNTYPED_ATOMIC.asNumber(), Double.NaN, 0);

		assertEquals(vl_NODE_EMPTY_DOCN.asNumber(), Double.NaN, 0);

		vl_NULL_STRING.asNumber();
		assertEquals(vl_NULL_STRING.asNumber(), Double.NaN, 0);

		try {
			vl_NULL_BYTES.asNumber();
			fail("Failure in ValueTest.asNumber()");
		} catch (XmlException e) {}
	}

	/*
	 * Test for asString()
	 * 
	 */
	@Test
	public void testAsString() throws Throwable {

		// test null XmlValue
		try {
			vl_NONE.asString();
			fail("Fail in testAsString()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		//
		try {
			vl_NODE_ELE.asString();
			assertEquals(
					vl_NODE_ELE.asString(),
					"<old:a_node xmlns:old=\"http://dbxmltest.test/test\""
					+ " old:atr1=\"test\" atr2=\"test2\"><!-- comment text -->"
					+ "<b_node/><?audio-output beep?><c_node>Other text</c_node>"
					+ "<d_node><![CDATA[ text < or > or &]]></d_node></old:a_node>");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_ATT.asString();
			assertEquals(vl_NODE_ATT.asString(), "{http://dbxmltest.test/test}atr1=\"test\"");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_TEX.asString();
			assertEquals(vl_NODE_TEX.asString(), "Other text");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_CDA.asString();

			assertEquals(vl_NODE_CDA.asString(),
					"<![CDATA[ text < or > or &]]>");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_PRO.asString();
			assertEquals(vl_NODE_PRO.asString(), "<?audio-output beep?>");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_COM.asString();
			assertEquals(vl_NODE_COM.asString(), "<!-- comment text -->");
		} catch (XmlException e) {
			throw e;

		}

		try {
			vl_NODE_DOCN.asString();
			assertEquals(vl_NODE_DOCN.asString(), docString);
		} catch (XmlException e) {
			throw e;

		}

		assertEquals(cn_NODE_ELE.asString(),"<foo>bar</foo>");

		assertEquals(cn_NODE_ATT.asString(), "{}foo=\"bar\"");

		assertEquals(cn_NODE_TEX.asString(), "bar");

		assertEquals(cn_NODE_PRO.asString(), "<?foo bar?>");

		assertEquals(cn_NODE_COM.asString(), "<!-- comment -->");

		assertEquals(cn_NODE_DOCN.asString(), "<foo>bar</foo>");
		
		assertEquals(cn_NODE_CDATA.asString(),"<![CDATA[cdata < and >]]>");

		// Other types XmlValue
		vl_ANY_URI.asString();
		assertEquals(vl_ANY_URI.asString(), "http://dbxml.dbxml");

		vl_BASE_64_BINARY.asString();
		assertEquals(vl_BASE_64_BINARY.asString(), "FFFF");

		vl_BOOLEAN.asString();
		assertEquals(vl_BOOLEAN.asString(), "true");

		vl_DATE.asString();
		assertEquals(vl_DATE.asString(), "2007-09-10");

		vl_DATE_TIME.asString();
		assertEquals(vl_DATE_TIME.asString(), "2002-10-08T03:46:27.278Z");

		vl_DAY_TIME_DURATION.asString();
		assertEquals(vl_DAY_TIME_DURATION.asString(), "P3DT10H30M");

		vl_DECIMAL.asString();
		assertEquals("00012345678.00", vl_DECIMAL.asString());

		assertEquals("43", vl_DECIMAL_SHORT.asString());

		vl_DOUBLE.asString();
		assertEquals("123456E5", vl_DOUBLE.asString());

		vl_DURATION.asString();
		assertEquals("P1Y2M4D", vl_DURATION.asString());

		vl_FLOAT.asString();
		assertEquals("1.23456E5", vl_FLOAT.asString());

		vl_G_DAY.asString();
		assertEquals("---31", vl_G_DAY.asString());

		vl_G_MONTH.asString();
		assertEquals("--02--", vl_G_MONTH.asString());

		vl_G_MONTH_DAY.asString();
		assertEquals("--02-13", vl_G_MONTH_DAY.asString());

		vl_G_YEAR.asString();
		assertEquals("2007", vl_G_YEAR.asString());

		vl_G_YEAR_MONTH.asString();
		assertEquals("2007-12", vl_G_YEAR_MONTH.asString());

		vl_HEX_BINARY.asString();
		assertEquals("0FB7", vl_HEX_BINARY.asString());

		vl_NOTATION.asString();
		assertEquals("http://www.xml.org:stuff.txt", vl_NOTATION.asString());

		vl_QNAME.asString();
		assertEquals("xml:stuff", vl_QNAME.asString());

		vl_STRING.asString();
		assertEquals(vl_STRING.asString(), "for test");

		vl_TIME.asString();
		assertEquals("20:24:50", vl_TIME.asString());

		vl_YEAR_MONTH_DURATION.asString();
		assertEquals("P1Y2M", vl_YEAR_MONTH_DURATION.asString());

		vl_UNTYPED_ATOMIC.asString();
		assertEquals("Test String", vl_UNTYPED_ATOMIC.asString());

		assertEquals("", vl_NODE_EMPTY_DOCN.asString());

		vl_NULL_STRING.asString();
		assertEquals("", vl_NULL_STRING.asString());

		try {
			vl_NULL_BYTES.asString();
			fail("Failure in ValueTest.asString()");
		} catch (XmlException e) {}

		
		// test after close the container, all the database node will throw exception
		if (!hp.isTransactional()){
		cont.delete();
		cont = null;
		
		try {
			vl_NODE_ELE.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}

		try {
			vl_NODE_ATT.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}

		try {
			vl_NODE_TEX.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}

		try {
			vl_NODE_CDA.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}

		try {
			vl_NODE_PRO.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}

		try {
			vl_NODE_COM.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.CONTAINER_CLOSED);
		}
		
		try {
			vl_NODE_DOCN.asString();
			fail("Fail to throw exception when call asString on a node value after container close.");
		} catch (XmlException e) {
			assertNotNull(e);
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		
		
		}

	}

	/*
	 * Test for equals(XmlValue value)
	 * 
	 */
	@Test
	public void testEquals() throws Throwable {

		try {
			assertFalse(vl_NODE_ELE.equals(vl_NONE));
			assertTrue(vl_NODE_ELE.equals(vl_NODE_ELE));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_ATT.equals(vl_NONE));
			assertTrue(vl_NODE_ATT.equals(vl_NODE_ATT));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_TEX.equals(vl_NONE));
			assertTrue(vl_NODE_TEX.equals(vl_NODE_TEX));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_CDA.equals(vl_NONE));
			assertTrue(vl_NODE_CDA.equals(vl_NODE_CDA));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_PRO.equals(vl_NONE));
			assertTrue(vl_NODE_PRO.equals(vl_NODE_PRO));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_COM.equals(vl_NONE));
			assertTrue(vl_NODE_COM.equals(vl_NODE_COM));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NODE_DOCN.equals(vl_NONE));
			assertTrue(vl_NODE_DOCN.equals(vl_NODE_DOCN));
		} catch (XmlException e) {
			throw e;

		}

		assertFalse(cn_NODE_ELE.equals(vl_NONE));
		assertTrue(cn_NODE_ELE.equals(cn_NODE_ELE));

		assertFalse(cn_NODE_ATT.equals(vl_NONE));
		assertTrue(cn_NODE_ATT.equals(cn_NODE_ATT));

		assertFalse(cn_NODE_TEX.equals(vl_NONE));
		assertTrue(cn_NODE_TEX.equals(cn_NODE_TEX));

		assertFalse(cn_NODE_PRO.equals(vl_NONE));
		assertTrue(cn_NODE_PRO.equals(cn_NODE_PRO));

		assertFalse(cn_NODE_COM.equals(vl_NONE));
		assertTrue(cn_NODE_COM.equals(cn_NODE_COM));

		assertFalse(cn_NODE_DOCN.equals(vl_NONE));
		assertTrue(cn_NODE_DOCN.equals(cn_NODE_DOCN));

		try {
			assertFalse(vl_ANY_URI.equals(vl_NODE_DOCN));
			assertTrue(vl_ANY_URI.equals(vl_ANY_URI));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_BASE_64_BINARY.equals(vl_ANY_URI));
			assertTrue(vl_BASE_64_BINARY.equals(vl_BASE_64_BINARY));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_BOOLEAN.equals(vl_ANY_URI));
			assertTrue(vl_BOOLEAN.equals(vl_BOOLEAN));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_DATE.equals(vl_ANY_URI));
			assertTrue(vl_DATE.equals(vl_DATE));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_DATE_TIME.equals(vl_ANY_URI));
			assertTrue(vl_DATE_TIME.equals(vl_DATE_TIME));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.equals(vl_ANY_URI));
			assertTrue(vl_DAY_TIME_DURATION.equals(vl_DAY_TIME_DURATION));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_DECIMAL.equals(vl_ANY_URI));
			assertTrue(vl_DECIMAL.equals(vl_DECIMAL));
		} catch (XmlException e) {
			throw e;

		}

		assertFalse(vl_DECIMAL_SHORT.equals(vl_ANY_URI));
		assertTrue(vl_DECIMAL_SHORT.equals(vl_DECIMAL_SHORT));

		try {
			assertFalse(vl_DOUBLE.equals(vl_ANY_URI));
			assertTrue(vl_DOUBLE.equals(vl_DOUBLE));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_DURATION.equals(vl_ANY_URI));
			assertTrue(vl_DURATION.equals(vl_DURATION));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_FLOAT.equals(vl_ANY_URI));
			assertTrue(vl_FLOAT.equals(vl_FLOAT));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_G_DAY.equals(vl_ANY_URI));
			assertTrue(vl_G_DAY.equals(vl_G_DAY));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_G_MONTH.equals(vl_ANY_URI));
			assertTrue(vl_G_MONTH.equals(vl_G_MONTH));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_G_MONTH_DAY.equals(vl_ANY_URI));
			assertTrue(vl_G_MONTH_DAY.equals(vl_G_MONTH_DAY));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_G_YEAR.equals(vl_ANY_URI));
			assertTrue(vl_G_YEAR.equals(vl_G_YEAR));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_G_YEAR_MONTH.equals(vl_ANY_URI));
			assertTrue(vl_G_YEAR_MONTH.equals(vl_G_YEAR_MONTH));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_HEX_BINARY.equals(vl_ANY_URI));
			assertTrue(vl_HEX_BINARY.equals(vl_HEX_BINARY));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_NOTATION.equals(vl_ANY_URI));
			assertTrue(vl_NOTATION.equals(vl_NOTATION));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_QNAME.equals(vl_ANY_URI));
			assertTrue(vl_QNAME.equals(vl_QNAME));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_STRING.equals(vl_ANY_URI));
			assertTrue(vl_STRING.equals(vl_STRING));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_TIME.equals(vl_ANY_URI));
			assertTrue(vl_TIME.equals(vl_TIME));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.equals(vl_ANY_URI));
			assertTrue(vl_YEAR_MONTH_DURATION.equals(vl_YEAR_MONTH_DURATION));
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertFalse(vl_UNTYPED_ATOMIC.equals(vl_ANY_URI));
			assertTrue(vl_UNTYPED_ATOMIC.equals(vl_UNTYPED_ATOMIC));
		} catch (XmlException e) {
			throw e;

		}

		assertFalse(vl_NODE_EMPTY_DOCN.equals(vl_ANY_URI));
		assertTrue(vl_NODE_EMPTY_DOCN.equals(vl_NODE_EMPTY_DOCN));

		assertFalse(vl_NULL_STRING.equals(vl_NULL_BYTES));
		assertTrue(vl_NULL_STRING.equals(vl_NULL_STRING));

		assertFalse(vl_NULL_BYTES.equals(vl_NULL_STRING));
		assertTrue(vl_NULL_BYTES.equals(vl_NULL_BYTES));
	}

	/*
	 * Test for getAttributes()
	 * 
	 */
	@Test
	public void testGetAttributes() throws Throwable {
		XmlResults results = null;
		XmlQueryContext context = mgr.createQueryContext();
		context.setEvaluationType(XmlQueryContext.Lazy);
		context.setNamespace("old", "http://dbxmltest.test/test");
		XmlResults tempResults = null;
		XmlValue tempValue = null;
		XmlValue value = null;

		try{
			if(hp.isTransactional())
				results = mgr.query(txn, "collection('"+ CON_NAME+"')/old:a_node", context);
			else
				results = mgr.query("collection('"+ CON_NAME+"')/old:a_node", context);

			value = results.next();
			tempResults = value.getAttributes();
			while (tempResults.hasNext()) {
				tempValue = tempResults.next(); 
				assertTrue(tempValue.getNodeName().matches("old:atr1") || tempValue.getNodeName().matches("atr2") || tempValue.getNodeName().matches("xmlns:old"));
				assertTrue(tempValue.getNodeValue().matches("test") || tempValue.getNodeValue().matches("test2") || tempValue.getNodeValue().matches("http://dbxmltest.test/test"));
			}
		}finally{
			if(tempResults != null) tempResults.delete();
			tempResults = null;
			if(results != null)results.delete();
			results = null;
		}

		try{
			if(hp.isTransactional())
				results = mgr.query(txn, "collection('"+ CON_NAME+"')/old:a_node", context);
			else
				results = mgr.query("collection('"+ CON_NAME+"')/old:a_node", context);

			value = results.next();
			tempResults = value.getAttributes();
			tempValue = tempResults.next(); 
			assertTrue(tempValue.getNodeName().matches("atr1") || tempValue.getNodeName().matches("atr2") || tempValue.getNodeName().matches("xmlns:old"));
			assertTrue(tempValue.getNodeValue().matches("test") || tempValue.getNodeValue().matches("test2") || tempValue.getNodeValue().matches("http://dbxmltest.test/test"));
			results.delete();
			results = null;
			try{
				tempValue = tempResults.next();
				tempValue.getNodeName();
			}catch(XmlException e){
				throw e;
			}
		}finally{
			if(tempResults != null) tempResults.delete();
			tempResults = null;
			if(results != null)results.delete();
			results = null;
		}

		XmlDocument contextDoc = mgr.createDocument();
		contextDoc.setContent(docString);
		XmlValue docValue = new XmlValue(contextDoc);
		String myQuery = "/old:a_node";
		XmlQueryExpression expr = null;
		try{
			if(hp.isTransactional()){
				expr = mgr.prepare(txn, myQuery, context);
				results = expr.execute(txn, docValue, context);
			}else{
				expr = mgr.prepare(myQuery, context);
				results = expr.execute(docValue, context);
			}
			value = results.next();
			tempResults = value.getAttributes();
			while (tempResults.hasNext()) {
				tempValue = tempResults.next(); 
				assertTrue(tempValue.getNodeName().matches("old:atr1") || tempValue.getNodeName().matches("atr2") || tempValue.getNodeName().matches("xmlns:old"));
				assertTrue(tempValue.getNodeValue().matches("test") || tempValue.getNodeValue().matches("test2") || tempValue.getNodeValue().matches("http://dbxmltest.test/test"));
			}
		}finally{
			if(expr != null) expr.delete();
			expr = null;
			if(tempResults != null) tempResults.delete();
			tempResults = null;
			if(results != null)results.delete();
			results = null;
		}

		XmlResults atts = null;
		try{
			if(hp.isTransactional())
				contextDoc = cont.getDocument(txn, docName);
			else
				contextDoc = cont.getDocument(docName);
			docValue = new XmlValue(contextDoc);
			XmlValue xmlValue = docValue.getFirstChild();
			atts = xmlValue.getAttributes();
			while (atts.hasNext()) {
				tempValue = atts.next();
				assertTrue(tempValue.getNodeName().matches("old:atr1") || tempValue.getNodeName().matches("atr2") || tempValue.getNodeName().matches("xmlns:old"));
				assertTrue(tempValue.getNodeValue().matches("test") || tempValue.getNodeValue().matches("test2") || tempValue.getNodeValue().matches("http://dbxmltest.test/test"));
			}
		}finally{
			if(atts != null)atts.delete();
			atts = null;
		}

		// test null XmlValue
		try {
			vl_NONE.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		// element node has attributes
		try {
			results = vl_NODE_ELE.getAttributes();
			assertEquals(results.size(), 3);
		} catch (XmlException e) {
			results.delete();
			throw e;
		}

		// test getOwnerElement ,getParentNode return the right node
		try {
			XmlValue attValue = results.next();
			XmlValue owner = attValue.getOwnerElement();
			assertEquals(owner.getLocalName(), "a_node");
			XmlValue parent = attValue.getParentNode();
			assertEquals(parent.getLocalName(), "a_node");
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		// element node has no attributes
		String query = "collection('testData.dbxml')/old:a_node/b_node";
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setNamespace("old", "http://dbxmltest.test/test");
		XmlResults re = null;
		try {
			if(hp.isTransactional())
				re= mgr.query(txn, query, qc);
			else
				re= mgr.query(query, qc);
			vl_NODE_ELE = re.next();
			results = vl_NODE_ELE.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(re != null) re.delete();
			re = null;
			if(results != null) results.delete();
			results = null;
		}
		
		//#17796
		XmlResults attrRes = null;
		try {
		    if (hp.isTransactional()) 
		        re = mgr.query(txn, query, qc);
		    else
		        re = mgr.query(query, qc);
		    XmlValue temp;
		    while(re.hasNext()) {
		        temp = re.next();
		        attrRes = temp.getAttributes();
		        attrRes.delete();
		    }
		} finally {
		    if (attrRes != null) attrRes.delete();
		    if (re != null) re.delete();
		}

		try {
			results = vl_NODE_ATT.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = vl_NODE_TEX.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = vl_NODE_CDA.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = vl_NODE_PRO.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = vl_NODE_COM.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = vl_NODE_DOCN.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_ELE.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_ATT.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_TEX.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_PRO.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_COM.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		try {
			results = cn_NODE_DOCN.getAttributes();
			assertEquals(results.size(), 0);
		}finally{
			if(results != null) results.delete();
			results = null;
		}

		// Other types XmlValue
		try {
			vl_ANY_URI.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getAttributes();
			fail("Fail in testGetAttributes()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
		try {
			vl_NODE_EMPTY_DOCN.getAttributes();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getAttributes();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getAttributes();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getFirstChild()
	 */
	@Test
	public void testGetFirstChild() throws Throwable {
		XmlValue value = null;

		// test null XmlValue
		try {
			value = vl_NONE.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getFirstChild();
			assertEquals(value.asString(), "<!-- comment text -->");

			// test get parentnode return the right node
			value = value.getParentNode();
			assertEquals(value.getLocalName(), "a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_ATT.getFirstChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getFirstChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_CDA.getFirstChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_PRO.getFirstChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_COM.getFirstChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_DOCN.getFirstChild();
			assertTrue(value.isNode());
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		value = cn_NODE_ELE.getFirstChild();
		assertEquals(value.asString(), "bar");

		value = cn_NODE_ATT.getFirstChild();
		assertTrue(value.isNull());

		value = cn_NODE_TEX.getFirstChild();
		assertTrue(value.isNull());

		value = cn_NODE_PRO.getFirstChild();
		assertTrue(value.isNull());

		value = cn_NODE_COM.getFirstChild();
		assertTrue(value.isNull());

		value = cn_NODE_DOCN.getFirstChild();
		assertEquals(value.asString(), "<foo>bar</foo>");

		// Other types XmlValue
		try {
			vl_ANY_URI.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getFirstChild();
			fail("Fail in testGetFirstChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.getFirstChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getFirstChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getFirstChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getLastChild()
	 * 
	 */
	@Test
	public void testGetLastChild() throws Throwable {
		XmlValue value = null;

		// test null XmlValue
		try {
			value = vl_NONE.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getLastChild();
			assertEquals(value.getLocalName(), "d_node");

			// test get parentnode return the right node
			value = value.getParentNode();
			assertEquals(value.getLocalName(), "a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_ATT.getLastChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getLastChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_CDA.getLastChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_PRO.getLastChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_COM.getLastChild();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_DOCN.isNull());
			value = vl_NODE_DOCN.getLastChild();
			assertTrue(value.isNode());
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		value = cn_NODE_ELE.getLastChild();
		assertEquals(value.asString(), "bar");

		value = cn_NODE_ATT.getLastChild();
		assertTrue(value.isNull());

		value = cn_NODE_TEX.getLastChild();
		assertTrue(value.isNull());

		value = cn_NODE_PRO.getLastChild();
		assertTrue(value.isNull());

		value = cn_NODE_COM.getLastChild();
		assertTrue(value.isNull());

		value = cn_NODE_DOCN.getLastChild();
		assertEquals(value.asString(), "<foo>bar</foo>");

		// Other types XmlValue
		try {
			vl_ANY_URI.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getLastChild();
			fail("Fail in testGetLastChild()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.getLastChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getLastChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getLastChild();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getLocalName()
	 * 
	 */
	@Test
	public void testGetLocalName() throws Throwable {
		// test null XmlValue
		try {
			vl_NONE.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			assertEquals(vl_NODE_ELE.getLocalName(), "a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getLocalName(), "atr1");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getLocalName(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getLocalName(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getLocalName(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getLocalName(), "");
		} catch (XmlException e) {
			throw e;
		}

		// test for document
		try {
			assertEquals(vl_NODE_DOCN.getLocalName(), "#document");
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getLocalName(), "foo");

		assertEquals(cn_NODE_ATT.getLocalName(), "foo");

		assertEquals(cn_NODE_TEX.getLocalName(), "");

		assertEquals(cn_NODE_PRO.getLocalName(), "");

		assertEquals(cn_NODE_COM.getLocalName(), "");

		assertEquals(cn_NODE_DOCN.getLocalName(), "#document");

		// Other types XmlValue
		try {
			vl_ANY_URI.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getLocalName();
			fail("Fail in testGetLocalName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("#document", vl_NODE_EMPTY_DOCN.getLocalName());

		try {
			vl_NULL_STRING.getLocalName();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getLocalName();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getNamespaceURI()
	 * 
	 */
	@Test
	public void testGetNamespaceURI() throws Throwable {
		// test null XmlValue
		try {
			vl_NONE.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			assertEquals(vl_NODE_ELE.getNamespaceURI(),
					"http://dbxmltest.test/test");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getNamespaceURI(), "http://dbxmltest.test/test");
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertEquals(vl_NODE_TEX.getNamespaceURI(), "");
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertEquals(vl_NODE_CDA.getNamespaceURI(), "");
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertEquals(vl_NODE_PRO.getNamespaceURI(), "");
		} catch (XmlException e) {
			throw e;

		}

		try {
			assertEquals(vl_NODE_COM.getNamespaceURI(), "");
		} catch (XmlException e) {
			throw e;

		}

		// test for document
		try {
			assertEquals(vl_NODE_DOCN.getNamespaceURI(), "");
		} catch (XmlException e) {
			throw e;

		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getNamespaceURI(),"");

		assertEquals(cn_NODE_ATT.getNamespaceURI(), "");

		assertEquals(cn_NODE_TEX.getNamespaceURI(), "");

		assertEquals(cn_NODE_PRO.getNamespaceURI(), "");

		assertEquals(cn_NODE_COM.getNamespaceURI(), "");

		assertEquals(cn_NODE_DOCN.getNamespaceURI(), "");

		// Other types XmlValue
		try {
			vl_ANY_URI.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getNamespaceURI();
			fail("Fail in testGetNamespaceURI()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("", vl_NODE_EMPTY_DOCN.getNamespaceURI());

		try {
			vl_NULL_STRING.getNamespaceURI();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getNamespaceURI();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getNextSibling()
	 * 
	 */
	@Test
	public void testGetNextSibling() throws Throwable {
		XmlValue value = null;

		// test null XmlValue
		try {
			value = vl_NONE.getNextSibling();
			fail("Fail in testGetNextSibling()");
			value = null;
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getNextSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_ATT.getNextSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getNextSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_CDA.getNextSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_PRO.getNextSibling();
			assertEquals(value.getNodeName(), "c_node");
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_COM.getNextSibling();
			assertEquals(value.getNodeName(), "b_node");
			value = value.getPreviousSibling();
			assertEquals(value.getNodeName(), "#comment");
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_DOCN.getNextSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		value = cn_NODE_ELE.getNextSibling();
		assertTrue(value.isNull());

		value = cn_NODE_ATT.getNextSibling();
		assertTrue(value.isNull());

		value = cn_NODE_TEX.getNextSibling();
		assertTrue(value.isNull());

		value = cn_NODE_PRO.getNextSibling();
		assertTrue(value.isNull());

		value = cn_NODE_COM.getNextSibling();
		assertTrue(value.isNull());

		value = cn_NODE_DOCN.getNextSibling();
		assertTrue(value.isNull());

		// Other types XmlValue
		try {
			vl_ANY_URI.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getNextSibling();
			fail("Fail in testGetNextSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.getNextSibling();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getNextSibling();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getNextSibling();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for getNodeHandle()
	 */
	@Test
	public void testGetNodeHandle() throws Throwable {
		String nodeHandle;

		// test null XmlValue
		try {
			nodeHandle = vl_NONE.getNodeHandle();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
		
		// test constructed node
		try {
		    nodeHandle = cn_NODE_ATT.getNodeHandle();
		    fail("Fail to throw exception in testGetNodeHandle(), when getNodeHandle on a constructed node.");
		} catch (XmlException e) {
		    assertNotNull(e.getMessage());
		    assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		XmlResults nodeRes = null;
		try {
			nodeHandle = vl_NODE_ELE.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			XmlValue value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_ELE));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_ATT.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_ATT));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_TEX.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_TEX));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_CDA.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_CDA));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_PRO.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_PRO));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_COM.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_COM));
			nodeRes.delete();
			
			nodeHandle = vl_NODE_DOCN.getNodeHandle();
			if(hp.isTransactional())
				nodeRes = cont.getNode(txn, nodeHandle, new XmlDocumentConfig());
			else
				nodeRes = cont.getNode(nodeHandle);
			value = nodeRes.next();
			assertTrue(value.equals(vl_NODE_DOCN));
			nodeRes.delete();
			
		} catch (XmlException e) {
			throw e;
		}finally{
		}
	}

	/*
	 * Test for getNodeName()
	 * 
	 */
	@Test
	public void testGetNodeName() throws Throwable {

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getNodeName(), XmlValue.NONE);
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		//
		try {
			assertEquals(vl_NODE_ELE.getNodeName(), "old:a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getNodeName(), "old:atr1");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getNodeName(), "#text");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getNodeName(), "#cdata-section");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getNodeName(), "audio-output");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getNodeName(), "#comment");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getNodeName(), "#document");
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getNodeName(), "foo");

		assertEquals(cn_NODE_ATT.getNodeName(), "foo");

		assertEquals(cn_NODE_TEX.getNodeName(), "#text");

		assertEquals(cn_NODE_PRO.getNodeName(), "foo");

		assertEquals(cn_NODE_COM.getNodeName(), "#comment");

		assertEquals(cn_NODE_DOCN.getNodeName(), "#document");
		
		//constructed cdata node
		assertEquals(cn_NODE_CDATA.getNodeName(), "#cdata-section");
		
		//construted nodes with prefix
		assertEquals(cn_NODE_ELE_P.getNodeName(), "old:foo");
		
		assertEquals(cn_NODE_ATT_P.getNodeName(), "old:foo");

		// Other types XmlValue
		try {
			vl_ANY_URI.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getNodeName();
			fail("Fail in testGetNodeName()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("#document", vl_NODE_EMPTY_DOCN.getNodeName());

		try {
			vl_NULL_STRING.getNodeName();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getNodeName();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for getNodeType()
	 * 
	 */
	@Test
	public void testGetNodeType() throws Throwable {

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getNodeType(), XmlValue.NONE);
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		//
		try {
			assertEquals(vl_NODE_ELE.getNodeType(), XmlValue.ELEMENT_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getNodeType(), XmlValue.ATTRIBUTE_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getNodeType(), XmlValue.TEXT_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getNodeType(), XmlValue.CDATA_SECTION_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getNodeType(),
					XmlValue.PROCESSING_INSTRUCTION_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getNodeType(), XmlValue.COMMENT_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getNodeType(), XmlValue.DOCUMENT_NODE);
		} catch (XmlException e) {
			fail("Fail in testGetNodeType()");
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getNodeType(), XmlValue.ELEMENT_NODE);

		assertEquals(cn_NODE_ATT.getNodeType(), XmlValue.ATTRIBUTE_NODE);

		assertEquals(cn_NODE_TEX.getNodeType(), XmlValue.TEXT_NODE);

		assertEquals(cn_NODE_PRO.getNodeType(),XmlValue.PROCESSING_INSTRUCTION_NODE);

		assertEquals(cn_NODE_COM.getNodeType(), XmlValue.COMMENT_NODE);

		assertEquals(cn_NODE_DOCN.getNodeType(), XmlValue.DOCUMENT_NODE);

		// Other types XmlValue
		try {
			vl_ANY_URI.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getNodeType();
			fail("Fail in testGetNodeType()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals(vl_NODE_EMPTY_DOCN.getNodeType(), XmlValue.DOCUMENT_NODE);

		try {
			vl_NULL_STRING.getNodeType();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getNodeType();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getNodeValue()
	 * 
	 */
	@Test
	public void testGetNodeValue() throws Throwable {

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getNodeValue(), XmlValue.NONE);
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			assertEquals(vl_NODE_ELE.getNodeValue(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getNodeValue(), "test");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getNodeValue(), "Other text");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getNodeValue(), " text < or > or &");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getNodeValue(), "beep");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getNodeValue(), " comment text ");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getNodeValue(), docString);
		} catch (XmlException e) {
			e.printStackTrace();
			throw e;
		}
		
		//constructed nodes
		assertEquals(cn_NODE_ELE.getNodeValue(), "");

		assertEquals(cn_NODE_ATT.getNodeValue(), "bar");

		assertEquals(cn_NODE_TEX.getNodeValue(), "bar");

		assertEquals(cn_NODE_PRO.getNodeValue(), "bar");

		assertEquals(cn_NODE_COM.getNodeValue(), " comment ");

		assertEquals(cn_NODE_DOCN.getNodeValue(), "<foo>bar</foo>");

		// Other types XmlValue
		try {
			vl_ANY_URI.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getNodeValue();
			fail("Fail in testGetNodeValue()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("", vl_NODE_EMPTY_DOCN.getNodeValue());

		try {
			vl_NULL_STRING.getNodeValue();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getNodeValue();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
		
		//test after container close
		XmlContainer mycont = null;
		XmlTransaction mytxn = null;
		XmlResults re1 = null;
		XmlValue va1 = null;
		try {
			 mycont = hp.createContainer("cont2.dbxml", mgr);
			 XmlUpdateContext uc = mgr.createUpdateContext();
			 XmlQueryContext qc = mgr.createQueryContext();
			 qc.setNamespace("old", "http://dbxmltest.test/test");
			 String myQuery = "collection('cont2.dbxml')/*:a_node";
			 if(hp.isTransactional()){
				 mytxn = mgr.createTransaction();
				 mycont.putDocument(mytxn, docName, docString, uc);
				 re1 = mgr.query(mytxn,myQuery, qc);
			 } else {
				 mycont.putDocument(docName, docString, uc);
				 re1 = mgr.query(myQuery, qc);
			 }
			
			 if (mytxn != null) {
				 mytxn.commit();
				 mytxn = null;
			 }
			 va1 = re1.next();
			 
			 mycont.delete();
			 mycont = null;
			 
			 try {
				 va1.getNodeValue();
				 fail("Fail to throw exception when getNodeValue on closed container's XmlValue. ");
			 } catch (XmlException e) {
				 assertNotNull(e);
				 assertEquals(XmlException.CONTAINER_CLOSED, e.getErrorCode());
			 }
			re1.delete();
		} catch (XmlException e){
			throw e;
		} finally {
			if (mytxn != null) mytxn.commit();
			if (mycont != null) mycont.delete();
			mgr.removeContainer("cont2.dbxml");
		}
	}

	/*
	 * Test for getOwnerElement()
	 * 
	 */
	@Test
	public void testGetOwnerElement() throws Throwable {

		XmlValue value = null;

		// test null XmlValue
		try {
			value = vl_NONE.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_ATT.getOwnerElement();
			assertEquals(value.getNodeName(), "old:a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_CDA.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_PRO.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_COM.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_DOCN.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		//constructed nodes
		try {
			value = cn_NODE_ELE.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		value = cn_NODE_ATT.getOwnerElement();
		assertTrue(value.isNull());

		try {
			value = cn_NODE_TEX.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = cn_NODE_PRO.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = cn_NODE_COM.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = cn_NODE_DOCN.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// Other types XmlValue
		try {
			vl_ANY_URI.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getOwnerElement();
			fail("Fail in testGetOwnerElement()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			value = vl_NODE_EMPTY_DOCN.getOwnerElement();
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getOwnerElement();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getOwnerElement();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for getParentNode()
	 * 
	 */
	@Test
	public void testGetParentNode() throws Throwable {
		XmlValue value = null;

		// test null XmlValue
		try {
			vl_NONE.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getParentNode();
			assertEquals(value.getNodeName(), "#document");
			assertEquals(value.getNodeType(), XmlValue.DOCUMENT_NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_ATT.getParentNode();
			// assertFalse(value.isNull());
			// assertEquals(value.getNodeName(), "old:a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getParentNode();
			assertEquals(value.getNodeName(), "c_node");
			assertEquals(value.getNodeType(), XmlValue.ELEMENT_NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_CDA.getParentNode();
			assertEquals(value.getLocalName(), "d_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_PRO.getParentNode();
			assertEquals(value.getNodeName(), "old:a_node");
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_COM.getParentNode();
			assertEquals(value.getNodeName(), "old:a_node");
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		// test for document node
		try {
			value = vl_NODE_DOCN.getParentNode();
			assertTrue(value.isNull());
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		value = cn_NODE_ELE.getParentNode();
		assertTrue(value.isNull());

		value = cn_NODE_ATT.getParentNode();
		assertTrue(value.isNull());

		value = cn_NODE_TEX.getParentNode();
		assertTrue(value.isNull());

		value = cn_NODE_PRO.getParentNode();
		assertTrue(value.isNull());

		value = cn_NODE_COM.getParentNode();
		assertTrue(value.isNull());

		value = cn_NODE_DOCN.getParentNode();
		assertTrue(value.isNull());

		// Other types XmlValue
		try {
			vl_ANY_URI.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getParentNode();
			fail("Fail in testGetParentNode()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.getParentNode();
			fail("Falure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getParentNode();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getParentNode();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

	}

	/*
	 * Test for getPrefix()
	 * 
	 */
	@Test
	public void testGetPrefix() throws Throwable {
		// test null XmlValue
		try {
			vl_NONE.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			assertEquals(vl_NODE_ELE.getPrefix(), "old");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getPrefix(), "old");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getPrefix(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getPrefix(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getPrefix(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getPrefix(), "");
		} catch (XmlException e) {
			throw e;
		}

		// test for document
		try {
			assertEquals(vl_NODE_DOCN.getPrefix(), "");
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getPrefix(), "");

		assertEquals(cn_NODE_ATT.getPrefix(), "");

		assertEquals(cn_NODE_TEX.getPrefix(), "");

		assertEquals(cn_NODE_PRO.getPrefix(), "");

		assertEquals(cn_NODE_COM.getPrefix(), "");

		assertEquals(cn_NODE_DOCN.getPrefix(), "");

		// Other types XmlValue
		try {
			vl_ANY_URI.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getPrefix();
			fail("Fail in testGetPrefix()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		assertEquals("", vl_NODE_EMPTY_DOCN.getPrefix());

		try {
			vl_NULL_STRING.getPrefix();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getPrefix();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getPreviousSibling()
	 * 
	 */
	@Test
	public void testGetPreviousSibling() throws Throwable {
		XmlValue value = null;

		// test null XmlValue
		try {
			value = vl_NONE.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
			value = null;
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		// node types
		try {
			value = vl_NODE_ELE.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_ATT.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_TEX.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_CDA.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_PRO.getPreviousSibling();
			assertEquals(value.getNodeName(), "b_node");
			value = value.getNextSibling();
			assertEquals(value.getNodeName(), "audio-output");
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_COM.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		try {
			value = vl_NODE_DOCN.getPreviousSibling();
			assertTrue(value.isNull());
			value = null;
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		value = cn_NODE_ELE.getPreviousSibling();
		assertTrue(value.isNull());

		value = cn_NODE_ATT.getPreviousSibling();
		assertTrue(value.isNull());

		value = cn_NODE_TEX.getPreviousSibling();
		assertTrue(value.isNull());

		value = cn_NODE_PRO.getPreviousSibling();
		assertTrue(value.isNull());

		value = cn_NODE_COM.getPreviousSibling();
		assertTrue(value.isNull());

		value = cn_NODE_DOCN.getPreviousSibling();
		assertTrue(value.isNull());

		// Other types XmlValue
		try {
			vl_ANY_URI.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BASE_64_BINARY.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_BOOLEAN.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DATE_TIME.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DAY_TIME_DURATION.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DECIMAL_SHORT.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DOUBLE.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_DURATION.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_FLOAT.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_DAY.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_MONTH_DAY.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_G_YEAR_MONTH.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_HEX_BINARY.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NOTATION.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_QNAME.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_STRING.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_TIME.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_YEAR_MONTH_DURATION.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_UNTYPED_ATOMIC.getPreviousSibling();
			fail("Fail in testGetPreviousSibling()");
		} catch (XmlException e) {
			assertNotNull(e.getMessage());
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NODE_EMPTY_DOCN.getPreviousSibling();
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_STRING.getPreviousSibling();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}

		try {
			vl_NULL_BYTES.getPreviousSibling();
			fail("Failure in ValueTest");
		} catch (XmlException e) {
			assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
		}
	}

	/*
	 * Test for getType()
	 * 
	 */
	@Test
	public void testGetType() throws Throwable {

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getType(), XmlValue.NONE);
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertEquals(vl_NODE_ELE.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getType(), XmlValue.NODE);
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			assertEquals(vl_ANY_URI.getType(), XmlValue.ANY_URI);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BASE_64_BINARY.getType(), XmlValue.BASE_64_BINARY);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BOOLEAN.getType(), XmlValue.BOOLEAN);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE.getType(), XmlValue.DATE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE_TIME.getType(), XmlValue.DATE_TIME);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DAY_TIME_DURATION.getType(),
					XmlValue.DAY_TIME_DURATION);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DECIMAL.getType(), XmlValue.DECIMAL);
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_DECIMAL_SHORT.getType(), XmlValue.DECIMAL);

		try {
			assertEquals(vl_DOUBLE.getType(), XmlValue.DOUBLE);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DURATION.getType(), XmlValue.DURATION);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_FLOAT.getType(), XmlValue.FLOAT);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_DAY.getType(), XmlValue.G_DAY);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH.getType(), XmlValue.G_MONTH);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH_DAY.getType(), XmlValue.G_MONTH_DAY);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR.getType(), XmlValue.G_YEAR);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR_MONTH.getType(), XmlValue.G_YEAR_MONTH);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_HEX_BINARY.getType(), XmlValue.HEX_BINARY);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NOTATION.getType(), XmlValue.NOTATION);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_QNAME.getType(), XmlValue.QNAME);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_STRING.getType(), XmlValue.STRING);
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_TIME.getType(), XmlValue.TIME);
		} catch (XmlException e) {
			throw e;
		}

		// YEAR_MONTH_DURATION equals to DURATION
		try {
			assertEquals(vl_YEAR_MONTH_DURATION.getType(),
					XmlValue.YEAR_MONTH_DURATION);
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_UNTYPED_ATOMIC.getType(), XmlValue.UNTYPED_ATOMIC);

		assertEquals(XmlValue.NODE, vl_NODE_EMPTY_DOCN.getType());

		assertEquals(XmlValue.STRING, vl_NULL_STRING.getType());
		assertEquals(XmlValue.BINARY, vl_NULL_BYTES.getType());
	}

	/*
	 * Test for getTypeName()
	 * 
	 */
	@Test
	public void testGetTypeName() throws Throwable {

		String query = "xs:short(5)";
		XmlQueryContext qc = mgr.createQueryContext();
		XmlResults re = null;
		try {
			re = mgr.query(query, qc);;
			XmlValue va = re.next();
			assertEquals("short", va.getTypeName());
		} catch (XmlException e) {
			throw e;
		} finally {
			if (re != null) re.delete();
			re = null;
		}

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getTypeName(), "");
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertEquals(vl_NODE_ELE.getTypeName(), "untyped");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getTypeName(), "untypedAtomic");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getTypeName(), "untypedAtomic");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getTypeName(), "untypedAtomic");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getTypeName(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getTypeName(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getTypeName(), "");
		} catch (XmlException e) {
			throw e;
		}

		//constructed nodes
		assertEquals(cn_NODE_ELE.getTypeName(), "untyped");

		assertEquals(cn_NODE_ATT.getTypeName(), "untypedAtomic");

		assertEquals(cn_NODE_TEX.getTypeName(), "untypedAtomic");

		assertEquals(cn_NODE_PRO.getTypeName(), "");

		assertEquals(cn_NODE_COM.getTypeName(), "");

		assertEquals(cn_NODE_DOCN.getTypeName(), "");

		// Other types XmlValue
		try {
			assertEquals(vl_ANY_URI.getTypeName(), "anyURI");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BASE_64_BINARY.getTypeName(), "base64Binary");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BOOLEAN.getTypeName(), "boolean");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE.getTypeName(), "date");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE_TIME.getTypeName(), "dateTime");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DAY_TIME_DURATION.getTypeName(), "dayTimeDuration");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DECIMAL.getTypeName(), "decimal");
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_DECIMAL_SHORT.getTypeName(), "short");

		try {
			assertEquals(vl_DOUBLE.getTypeName(), "double");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DURATION.getTypeName(), "duration");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_FLOAT.getTypeName(), "float");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_DAY.getTypeName(), "gDay");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH.getTypeName(), "gMonth");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH_DAY.getTypeName(), "gMonthDay");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR.getTypeName(), "gYear");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR_MONTH.getTypeName(), "gYearMonth");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_HEX_BINARY.getTypeName(), "hexBinary");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NOTATION.getTypeName(), "NOTATION");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_QNAME.getTypeName(), "QName");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_STRING.getTypeName(), "string");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_TIME.getTypeName(), "time");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_YEAR_MONTH_DURATION.getTypeName(),
					"yearMonthDuration");
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_UNTYPED_ATOMIC.getTypeName(), "untypedAtomic");

		assertEquals("", vl_NODE_EMPTY_DOCN.getTypeName());

		assertEquals("string", vl_NULL_STRING.getTypeName());
		assertEquals("", vl_NULL_BYTES.getTypeName());
	}

	/*
	 * Test for getTypeURI()
	 * 
	 */
	@Test
	public void testGetTypeURI() throws Throwable {
		String query = "xs:short(5)";
		XmlQueryContext qc = mgr.createQueryContext();
		XmlResults re = null;
		try {
			re = mgr.query(query, qc);;
			XmlValue va = re.next();
			assertEquals("http://www.w3.org/2001/XMLSchema", va.getTypeURI());
		} catch (XmlException e) {
			throw e;
		} finally {
			if (re != null) re.delete();
			re = null;
		}

		// test null XmlValue
		try {
			assertEquals(vl_NONE.getTypeURI(), "");
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertEquals(vl_NODE_ELE.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_ATT.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_TEX.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_CDA.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_PRO.getTypeURI(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_COM.getTypeURI(), "");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NODE_DOCN.getTypeURI(), "");
		} catch (XmlException e) {
			throw e;
		}

		//constructed node
		assertEquals(cn_NODE_ELE.getTypeURI(),"http://www.w3.org/2001/XMLSchema");

		assertEquals(cn_NODE_ATT.getTypeURI(),"http://www.w3.org/2001/XMLSchema");

		assertEquals(cn_NODE_TEX.getTypeURI(),"http://www.w3.org/2001/XMLSchema");

		assertEquals(cn_NODE_PRO.getTypeURI(), "");

		assertEquals(cn_NODE_COM.getTypeURI(), "");

		assertEquals(cn_NODE_DOCN.getTypeURI(), "");

		// Other types XmlValue
		try {
			assertEquals(vl_ANY_URI.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BASE_64_BINARY.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_BOOLEAN.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DATE_TIME.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DAY_TIME_DURATION.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DECIMAL.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_DECIMAL_SHORT.getTypeURI(), "http://www.w3.org/2001/XMLSchema");

		try {
			assertEquals(vl_DOUBLE.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_DURATION.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_FLOAT.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_DAY.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_MONTH_DAY.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_G_YEAR_MONTH.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_HEX_BINARY.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_NOTATION.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_QNAME.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_STRING.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_TIME.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertEquals(vl_YEAR_MONTH_DURATION.getTypeURI(),
					"http://www.w3.org/2001/XMLSchema");
		} catch (XmlException e) {
			throw e;
		}

		assertEquals(vl_UNTYPED_ATOMIC.getTypeURI(), "http://www.w3.org/2001/XMLSchema");

		assertEquals("", vl_NODE_EMPTY_DOCN.getTypeURI());

		assertEquals("http://www.w3.org/2001/XMLSchema", vl_NULL_STRING.getTypeURI());
		assertEquals("", vl_NULL_BYTES.getTypeURI());
	}

	/*
	 * Test for isBoolean()
	 * 
	 */
	@Test
	public void testIsBoolean() throws Throwable {

		// test null XmlValue
		try {
			assertFalse(vl_NONE.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		//
		try {
			assertFalse(vl_NODE_ELE.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_ATT.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_TEX.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_CDA.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_PRO.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_COM.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NODE_DOCN.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		// Other types XmlValue
		try {
			assertFalse(vl_ANY_URI.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_BASE_64_BINARY.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertTrue(vl_BOOLEAN.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_DATE.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_DATE_TIME.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_DECIMAL.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		assertFalse(vl_DECIMAL_SHORT.isBoolean());	

		try {
			assertFalse(vl_DOUBLE.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_DURATION.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_FLOAT.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_G_DAY.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH_DAY.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR_MONTH.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_HEX_BINARY.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_NOTATION.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_QNAME.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_STRING.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_TIME.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.isBoolean());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		assertFalse(vl_UNTYPED_ATOMIC.isBoolean());

		assertFalse(vl_NODE_EMPTY_DOCN.isBoolean());

		assertFalse(vl_NULL_STRING.isBoolean());
		assertFalse(vl_NULL_BYTES.isBoolean());
	}

	/*
	 * Test for isNode()
	 * 
	 */
	@Test
	public void testIsNode() throws Throwable {

		// test null XmlValue
		try {
			assertFalse(vl_NONE.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsBoolean()");
			throw e;
		}

		//
		try {
			assertTrue(vl_NODE_ELE.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_ATT.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_TEX.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_CDA.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_PRO.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_COM.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertTrue(vl_NODE_DOCN.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		// Other types XmlValue
		try {
			assertFalse(vl_ANY_URI.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_BASE_64_BINARY.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_BOOLEAN.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_DATE.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_DATE_TIME.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_DECIMAL.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		assertFalse(vl_DECIMAL_SHORT.isNode());

		try {
			assertFalse(vl_DOUBLE.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_DURATION.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_FLOAT.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_G_DAY.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH_DAY.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR_MONTH.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_HEX_BINARY.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_NOTATION.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_QNAME.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_STRING.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_TIME.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.isNode());
		} catch (XmlException e) {
			fail("Fail in testIsNode()");
			throw e;
		}
		assertFalse(vl_UNTYPED_ATOMIC.isNode());

		assertTrue(vl_NODE_EMPTY_DOCN.isNode());

		assertFalse(vl_NULL_STRING.isNode());
		assertFalse(vl_NULL_BYTES.isNode());
	}

	/*
	 * Test for isNull()
	 * 
	 */
	@Test
	public void testIsNull() throws Throwable {

		// test null XmlValue
		try {
			assertTrue(vl_NONE.isNull());
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertFalse(vl_NODE_ELE.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_ATT.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_TEX.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_CDA.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_PRO.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_COM.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_DOCN.isNull());
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			assertFalse(vl_ANY_URI.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BASE_64_BINARY.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BOOLEAN.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE_TIME.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DECIMAL.isNull());
		} catch (XmlException e) {
			throw e;
		}

		assertFalse(vl_DECIMAL_SHORT.isNull());

		try {
			assertFalse(vl_DOUBLE.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DURATION.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_FLOAT.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_DAY.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH_DAY.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR_MONTH.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_HEX_BINARY.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NOTATION.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_QNAME.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_STRING.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_TIME.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.isNull());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_UNTYPED_ATOMIC.isNull());
		} catch (XmlException e) {
			throw e;
		}

		assertFalse(vl_NODE_EMPTY_DOCN.isNull());

		assertFalse(vl_NULL_STRING.isNull());
		assertFalse(vl_NULL_BYTES.isNull());
	}

	/*
	 * Test for isNumber()
	 * 
	 */
	@Test
	public void testIsNumber() throws Throwable {

		// test null XmlValue
		try {
			assertFalse(vl_NONE.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertFalse(vl_NODE_ELE.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_ATT.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_TEX.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_CDA.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_PRO.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_COM.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_DOCN.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			assertFalse(vl_ANY_URI.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BASE_64_BINARY.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BOOLEAN.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE_TIME.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DECIMAL.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		assertTrue(vl_DECIMAL_SHORT.isNumber());

		try {
			assertTrue(vl_DOUBLE.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DURATION.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_FLOAT.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_DAY.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH_DAY.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR_MONTH.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_HEX_BINARY.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NOTATION.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_QNAME.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_STRING.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_TIME.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_UNTYPED_ATOMIC.isNumber());
		} catch (XmlException e) {
			throw e;
		}

		assertFalse(vl_NODE_EMPTY_DOCN.isNumber());

		assertFalse(vl_NULL_STRING.isNumber());
		assertFalse(vl_NULL_BYTES.isNumber());
	}

	/*
	 * Test for isString()
	 * 
	 */
	@Test
	public void testIsString() throws Throwable {

		// test null XmlValue
		try {
			assertFalse(vl_NONE.isString());
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertFalse(vl_NODE_ELE.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_ATT.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_TEX.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_CDA.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_PRO.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_COM.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NODE_DOCN.isString());
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			assertFalse(vl_ANY_URI.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BASE_64_BINARY.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_BOOLEAN.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DATE_TIME.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DAY_TIME_DURATION.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DECIMAL.isString());
		} catch (XmlException e) {
			throw e;
		}

		assertFalse(vl_DECIMAL_SHORT.isString());

		try {
			assertFalse(vl_DOUBLE.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_DURATION.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_FLOAT.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_DAY.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_MONTH_DAY.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_G_YEAR_MONTH.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_HEX_BINARY.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_NOTATION.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_QNAME.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_STRING.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_TIME.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_YEAR_MONTH_DURATION.isString());
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertFalse(vl_UNTYPED_ATOMIC.isString());
		} catch (XmlException e) {
			throw e;
		}

		assertFalse(vl_NODE_EMPTY_DOCN.isString());
		assertTrue(vl_NULL_STRING.isString());
		assertFalse(vl_NULL_BYTES.isString());
	}

	/*
	 * Test for isType(int type)
	 * 
	 */
	@Test
	public void testIsType() throws Throwable {

		// test null XmlValue
		try {
			assertTrue(vl_NONE.isType(XmlValue.NONE));
		} catch (XmlException e) {
			throw e;
		}

		//
		try {
			assertTrue(vl_NODE_ELE.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_ATT.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_TEX.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_CDA.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_PRO.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_COM.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NODE_DOCN.isType(XmlValue.NODE));
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			assertTrue(vl_ANY_URI.isType(XmlValue.ANY_URI));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_BASE_64_BINARY.isType(XmlValue.BASE_64_BINARY));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_BOOLEAN.isType(XmlValue.BOOLEAN));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DATE.isType(XmlValue.DATE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DATE_TIME.isType(XmlValue.DATE_TIME));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DAY_TIME_DURATION.isType(XmlValue.DAY_TIME_DURATION));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DECIMAL.isType(XmlValue.DECIMAL));
		} catch (XmlException e) {
			throw e;
		}

		assertTrue(vl_DECIMAL_SHORT.isType(XmlValue.DECIMAL));

		try {
			assertTrue(vl_DOUBLE.isType(XmlValue.DOUBLE));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_DURATION.isType(XmlValue.DURATION));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_FLOAT.isType(XmlValue.FLOAT));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_G_DAY.isType(XmlValue.G_DAY));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_G_MONTH.isType(XmlValue.G_MONTH));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_G_MONTH_DAY.isType(XmlValue.G_MONTH_DAY));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_G_YEAR.isType(XmlValue.G_YEAR));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_G_YEAR_MONTH.isType(XmlValue.G_YEAR_MONTH));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_HEX_BINARY.isType(XmlValue.HEX_BINARY));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_NOTATION.isType(XmlValue.NOTATION));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_QNAME.isType(XmlValue.QNAME));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_STRING.isType(XmlValue.STRING));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_TIME.isType(XmlValue.TIME));
		} catch (XmlException e) {
			throw e;
		}

		try {
			assertTrue(vl_YEAR_MONTH_DURATION
					.isType(XmlValue.YEAR_MONTH_DURATION));
		} catch (XmlException e) {
			throw e;
		}

		assertTrue(vl_UNTYPED_ATOMIC.isType(XmlValue.UNTYPED_ATOMIC));

		assertTrue(vl_NODE_EMPTY_DOCN.isType(XmlValue.NODE));
		assertTrue(vl_NULL_STRING.isType(XmlValue.STRING));
		assertTrue(vl_NULL_BYTES.isType(XmlValue.BINARY));
	}

	/*
	 * Test for setValue()
	 * 
	 */
	@Test
	public void testSetValue() throws Throwable {
		XmlValue value = new XmlValue();

		try {
			XmlValue.setValue(value, vl_NODE_ELE);
			assertEquals(value.getNodeName(), "old:a_node");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_ATT);
			assertEquals(value.getNodeName(), "old:atr1");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_TEX);
			assertEquals(value.getNodeName(), "#text");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_CDA);
			assertEquals(value.getNodeName(), "#cdata-section");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_PRO);
			assertEquals(value.getNodeName(), "audio-output");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_COM);
			assertEquals(value.getNodeName(), "#comment");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NODE_DOCN);
			assertEquals(value.getNodeName(), "#document");
		} catch (XmlException e) {
			throw e;
		}

		// Other types XmlValue
		try {
			XmlValue.setValue(value, vl_ANY_URI);
			assertEquals(value.asString(), "http://dbxml.dbxml");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_BASE_64_BINARY);
			assertEquals(value.asString(), "FFFF");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_BOOLEAN);
			assertEquals(value.asString(), "true");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_DATE);
			assertEquals(value.asString(), "2007-09-10");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_DATE_TIME);
			assertEquals(value.asString(), "2002-10-08T03:46:27.278Z");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_DAY_TIME_DURATION);
			assertEquals(value.asString(), "P3DT10H30M");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_DECIMAL);
			assertEquals(value.asString(), "00012345678.00");
		} catch (XmlException e) {
			throw e;
		}

		XmlValue.setValue(value, vl_DECIMAL_SHORT);
		assertEquals(value.getTypeName(), "short");

		try {
			XmlValue.setValue(value, vl_DOUBLE);
			assertEquals(value.asString(), "123456E5");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_DURATION);
			assertEquals(value.asString(), "P1Y2M4D");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_FLOAT);
			assertEquals(value.asString(), "1.23456E5");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_G_DAY);
			assertEquals(value.asString(), "---31");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_G_MONTH);
			assertEquals(value.asString(), "--02--");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_G_MONTH_DAY);
			assertEquals(value.asString(), "--02-13");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_G_YEAR);
			assertEquals(value.asString(), "2007");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_G_YEAR_MONTH);
			assertEquals(value.asString(), "2007-12");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_HEX_BINARY);
			assertEquals(value.asString(), "0FB7");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_NOTATION);
			assertEquals(value.asString(), "http://www.xml.org:stuff.txt");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_QNAME);
			assertEquals(value.asString(), "xml:stuff");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_STRING);
			assertEquals(value.asString(), "for test");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_TIME);
			assertEquals(value.asString(), "20:24:50");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_YEAR_MONTH_DURATION);
			assertEquals(value.asString(), "P1Y2M");
		} catch (XmlException e) {
			throw e;
		}

		try {
			XmlValue.setValue(value, vl_UNTYPED_ATOMIC);
			assertEquals(value.asString(), "Test String");
		} catch (XmlException e) {
			throw e;
		}

	}

	@Test
	public void testGetResults() throws Throwable {
		assertNotNull(vl_NODE_ELE.getResults());
		assertNotNull(vl_NODE_ATT.getResults());
		assertNotNull(vl_NODE_TEX.getResults());
		assertNotNull(vl_NODE_CDA.getResults());
		assertNotNull(vl_NODE_PRO.getResults());
		assertNotNull(vl_NODE_COM.getResults());
		assertNotNull(vl_NODE_DOCN.getResults());

		// Other types XmlValue

		assertNull(vl_ANY_URI.getResults());
		assertNull(vl_BASE_64_BINARY.getResults());
		assertNull(vl_BOOLEAN.getResults());
		assertNull(vl_DATE.getResults());
		assertNull(vl_DATE_TIME.getResults());
		assertNull(vl_DAY_TIME_DURATION.getResults());
		assertNull(vl_DECIMAL.getResults());
		assertNull(vl_DECIMAL_SHORT.getResults());
		assertNull(vl_DOUBLE.getResults());
		assertNull(vl_DURATION.getResults());
		assertNull(vl_FLOAT.getResults());
		assertNull(vl_G_DAY.getResults());
		assertNull(vl_G_MONTH.getResults());
		assertNull(vl_G_MONTH_DAY.getResults());
		assertNull(vl_G_YEAR.getResults());
		assertNull(vl_G_YEAR_MONTH.getResults());
		assertNull(vl_HEX_BINARY.getResults());
		assertNull(vl_NOTATION.getResults());
		assertNull(vl_QNAME.getResults());
		assertNull(vl_STRING.getResults());
		assertNull(vl_TIME.getResults());
		assertNull(vl_YEAR_MONTH_DURATION.getResults());
		assertNull(vl_UNTYPED_ATOMIC.getResults());
		assertNull(vl_NULL_STRING.getResults());
		assertNull(vl_NULL_BYTES.getResults());

		//Nodes constructed from XQuery constructors
		assertNotNull(vl_NODE_ELE.getResults());
		assertNotNull(vl_NODE_ATT.getResults());
		assertNotNull(vl_NODE_TEX.getResults());
		assertNotNull(vl_NODE_PRO.getResults());
		assertNotNull(vl_NODE_COM.getResults());
		assertNotNull(vl_NODE_DOCN.getResults());

		assertNull(vl_NODE_EMPTY_DOCN.getResults());

		XmlResults re = null;
		XmlResults re2 = null;
		XmlResults re3 = null;
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setNamespace("old", "http://dbxmltest.test/test");
		try {
			if(hp.isTransactional()) 
				re = mgr.query(txn, "collection('" + CON_NAME + "')/old:a_node", qc);
			else
				re = mgr.query("collection('" + CON_NAME + "')/old:a_node", qc);
			XmlValue val = re.next();
			assertEquals(val.getResults(), re);

			re2 = mgr.createResults();
			re2.add(val);
			XmlValue val2 = re2.next();
			assertFalse(val2.getResults() == re);
			val.getNodeHandle();

			if(hp.isTransactional()) 
				re3 = cont.getAllDocuments(txn, (new XmlDocumentConfig()));
			else
				re3 = cont.getAllDocuments(new XmlDocumentConfig());
			XmlDocument doc3 = mgr.createDocument();
			re3.next(doc3);
			XmlValue val3 = new XmlValue(doc3);
			assertEquals(val3.getResults(), re3);
			try {
				re3.delete();
				val3.asString();
				fail("Failure in XmlValue.testGetResults()");
			} catch (XmlException e) {
				assertNotNull(e);
			}
		} finally {
			if (re != null) re.delete();
			if (re2 != null) re2.delete();
			if (re3 != null) re3.delete();
		}
	}

	/*
	 * Tests whether a document node can be navigated if it is
	 * returned by a query
	 */
	@Test
	public void testNavigateDocumentNode() throws Throwable {
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setNamespace("old", "http://dbxmltest.test/test");
		XmlResults re = null;
		XmlResults attrs = null;
		try {
			if(hp.isTransactional())
				re = mgr.query(txn, "collection('" + CON_NAME + "')/old:a_node/parent::node()", qc);
			else
				re = mgr.query("collection('" + CON_NAME + "')/old:a_node/parent::node()", qc);
			XmlValue value = re.next();
			assertEquals(value.getNodeType(), XmlValue.DOCUMENT_NODE);
			XmlValue child = value.getFirstChild();
			assertEquals(child.getNodeName(), "old:a_node");
			XmlValue child2 = child.getFirstChild();
			assertEquals(child2.getNodeName(), "#comment");
			XmlValue sibling = child2.getNextSibling();
			assertEquals(sibling.getNodeName(), "b_node");
			assertEquals(child2.getParentNode().getNodeName(), "old:a_node");
			attrs = child.getAttributes();
			XmlValue attr = attrs.next();
			assertEquals(attr.getNodeName(), "xmlns:old");
			assertEquals(attr.getOwnerElement().getNodeName(), "old:a_node");
			re.delete();
			re = null;
			try{
				value.getFirstChild();
				fail("Fail in testNavigateDocumentNode()");
			}catch(XmlException e){
				assertEquals(XmlException.INVALID_VALUE, e.getErrorCode());
			}
		} catch (XmlException e) {
			throw e;
		}finally{
			if(attrs != null) attrs.delete();
			attrs = null;
			if(re != null) re.delete();
			re = null;
		}

		XmlDocument transientDoc = mgr.createDocument();
		transientDoc.setName(docString);
		transientDoc.setContent(docString);
		XmlValue docValue = new XmlValue(transientDoc);
		try{
			docValue.getFirstChild();
			fail("Cannot get the first child on a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		try{
			docValue.getLastChild();
			fail("Cannot get the last child on a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		try{
			docValue.getNextSibling();
			fail("Cannot get the next sibling on a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		try{
			docValue.getParentNode();
			fail("Cannot get the parent on a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		try{
			docValue.getAttributes();
			fail("Cannot get the attributes of a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
		try{
			docValue.getOwnerElement();
			fail("Cannot get the owner element on a transient document");
		}catch(XmlException e){
			assertEquals(e.getErrorCode(), XmlException.INVALID_VALUE);
		}
	}
}
