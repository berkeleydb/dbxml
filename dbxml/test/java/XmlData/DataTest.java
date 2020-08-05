package dbxmltest;

import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import com.sleepycat.dbxml.*;


//ToDo: Cannot do anything interesting with this until compression is implemented
public class DataTest {
	private TestConfig hp = null;
	private XmlManager mgr = null;
	private XmlContainer cont = null;
	private static final String CON_NAME = "testData.dbxml";
	private String compressionName = "TestCompression";
	private static String docString = "<old:a_node xmlns:old=\""
		+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
		+ "><b_node/><c_node>Other text</c_node><d_node/>"
		+ "</old:a_node>";

	private static String docName = "testDoc.xml";

	@BeforeClass
	public static void setupClass() {
		System.out.println("Begin test XmlData!");
		TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
	}

	@Before
	public void setUp() throws Throwable {
		hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
				XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
		mgr = hp.createManager();
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
		System.out.println("Finished test XmlData!");
	}

	@Test
	public void testXmlData() throws Exception
	{
		assertTrue(HelperFunctions.testXmlData());
	}


	/*
	 * Java XmlData objects can only be created by calls to compress and decompress,
	 * so create an XmlCompression class that tests it.
	 */
	public class DataCompression extends XmlCompression {
		public byte[] srcData;
		public boolean compress(XmlTransaction txn, XmlData source, XmlData dest) throws XmlException {
			byte[] src = source.get_data();
			if (source.get_size() != srcData.length) return false;
			for (int i = 0; i < src.length; i++) {
				if (src[i] != srcData[i]) return false;
			}
			int size = dest.getReservedSize();
			dest.reserve(size + 1);
			if (dest.getReservedSize() < (size + 1)) return false;
			dest.append(src);
			size = dest.get_size();
			if (size != srcData.length) return false;
			byte[] src2 = dest.get_data();
			for (int i = 0; i < src2.length; i++) {
				if (src2[i] != srcData[i]) return false;
			}
			dest.set(src);
			if (dest.get_size() != srcData.length) return false;
			src2 = dest.get_data();
			for (int i = 0; i < src2.length; i++) {
				if (src2[i] != srcData[i]) return false;
			}
			return true;
		}
		public boolean decompress(XmlTransaction txn, XmlData source, XmlData dest) throws XmlException {
			dest.adoptBuffer(source);
			return true;
		}

	}

	@Test
	public void testJavaXmlData() throws Exception
	{
		if (hp.isNodeContainer()) return; //No compression yet for node storage
		DataCompression test = new DataCompression();
		test.srcData = docString.getBytes("utf-8");
		XmlTransaction txn = null;
		mgr.registerCompression(compressionName, test);
		XmlContainerConfig config = new XmlContainerConfig();
		config.setCompression(compressionName);
		config.setTransactional(hp.isTransactional());
		if(!hp.isNodeContainer())
			config.setContainerType(XmlContainer.WholedocContainer);
		cont = hp.createContainer(CON_NAME, mgr, config);
		XmlDocument doc = mgr.createDocument();
		doc.setName(docName);
		doc.setContent(docString);
		try {
			if (hp.isTransactional()) {
				txn = mgr.createTransaction();
				cont.putDocument(txn, doc);
			} else 
				cont.putDocument(doc);
		} catch (XmlException e) {
			throw e;
		} finally {
			if (txn != null) txn.commit();
			test.delete();
		}
	}

}
