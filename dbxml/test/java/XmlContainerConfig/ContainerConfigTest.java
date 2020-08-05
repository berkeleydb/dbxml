/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import java.io.File;

import com.sleepycat.db.Environment;
import com.sleepycat.db.EnvironmentConfig;
import com.sleepycat.db.ErrorHandler;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.HelperFunctions;
import com.sleepycat.dbxml.XmlQueryContext;
import com.sleepycat.dbxml.XmlResults;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class ContainerConfigTest {
private TestConfig hp = null;
private XmlManager mgr = null;
private XmlContainer cont = null;
private static final String CON_NAME = "testData.dbxml";
    
    @BeforeClass
    public static void setupClass() {
	System.out.println("Begin test XmlContainerConfig!");
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @Before
    public void setUp() throws Throwable {
	hp = new TestConfig(XmlTestRunner.getEnvironmentType(),
		XmlTestRunner.isNodeContainer(), XmlTestRunner.getEnvironmentPath());
	mgr = hp.createManager();
	cont = null;
    }

    @After
    public void tearDown() throws Throwable {
	hp.closeContainer(cont);
	cont = null;
	hp.closeManager(mgr);
	hp.closeEnvironment();
	TestConfig.fileRemove(XmlTestRunner.getEnvironmentPath());
    }

    @AfterClass
    public static void tearDownClass() {
	System.out.println("Finished test XmlContainerConfig!");
    }
    
    @Test
    public void testContainerConfigConstructor() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	assertNotNull(config);
	assertTrue(config instanceof XmlContainerConfig);
	cont = mgr.createContainer(CON_NAME, config);
    }
    
    @Test
    public void testGetSetCompression() throws Exception
    {
	String name = "compression";
	XmlContainerConfig config = new XmlContainerConfig();
	assertEquals(XmlContainerConfig.DEFAULT_COMPRESSSION, config.getCompression());
	config.setCompression(name);
	TestCompression testComp = new TestCompression();
	mgr.registerCompression(name, testComp);
	assertEquals(name, config.getCompression());
	if (!hp.isNodeContainer()) {
		config.setContainerType(XmlContainer.WholedocContainer);
		cont = mgr.createContainer(CON_NAME, config);
		assertEquals(cont.getContainerConfig().getCompression(), config.getCompression());
		cont.delete();
		
		//Test that opening the container with a false compression name will be silently ignored
		config.setCompression("fakeCompression");
		cont = mgr.openContainer(CON_NAME, config);
		assertEquals(cont.getContainerConfig().getCompression(), name);
	} else {
		try {
			cont = mgr.createContainer(CON_NAME, config);
			fail("Cannot create a node container with compression.");
		} catch (XmlException e) {}
	}
    }
    
    @Test
    public void testGetSetIndexNodes() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	assertEquals(XmlContainerConfig.DEFAULT.getIndexNodesFlag(), config.getIndexNodesFlag());
	config.setIndexNodes(XmlContainerConfig.UseDefault);
	assertEquals(XmlContainerConfig.UseDefault, config.getIndexNodesFlag());
	config.setIndexNodes(XmlContainerConfig.Off);
	assertEquals(XmlContainerConfig.Off, config.getIndexNodesFlag());
	config.setIndexNodes(XmlContainerConfig.On);
	assertEquals(XmlContainerConfig.On, config.getIndexNodesFlag());
	cont = mgr.createContainer(CON_NAME, config);
	assertEquals(cont.getContainerConfig().getIndexNodesFlag(), config.getIndexNodesFlag());
    }
    
    @Test
    public void testGetSetStatisticsEnabled() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	assertEquals(XmlContainerConfig.DEFAULT.getStatisticsEnabledFlag(), config.getStatisticsEnabledFlag());
	config.setStatisticsEnabled(XmlContainerConfig.UseDefault);
	assertEquals(XmlContainerConfig.UseDefault, config.getStatisticsEnabledFlag());
	config.setStatisticsEnabled(XmlContainerConfig.On);
    assertEquals(XmlContainerConfig.On, config.getStatisticsEnabledFlag());
	config.setStatisticsEnabled(XmlContainerConfig.Off);
	assertEquals(XmlContainerConfig.Off, config.getStatisticsEnabledFlag());
	config.setTransactional(hp.isTransactional());
	int containerType = XmlContainer.WholedocContainer;
	if (hp.isNodeContainer())
	    containerType = XmlContainer.NodeContainer;
	config.setContainerType(containerType);
	cont = mgr.createContainer(CON_NAME, config);
	assertEquals(cont.getContainerConfig().getStatisticsEnabledFlag(), config.getStatisticsEnabledFlag());
	
	//[#17898] Test that update queries do not crash when statistics are turned off
	XmlQueryContext context = mgr.createQueryContext();
	cont.putDocument("test.xml", "<root id=\"25\"/>");
	XmlResults res = mgr.query("replace value of node collection('" + CON_NAME + "')/root/@id with 26", context);
	res.delete();
    }
    
    @Test
    public void testGetSetNodeContainer() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	config.setContainerType(XmlContainer.NodeContainer);
	assertEquals(XmlContainer.NodeContainer, config.getContainerType());
	config.setContainerType(XmlContainer.WholedocContainer);
	assertEquals(XmlContainer.WholedocContainer, config.getContainerType());
	cont = mgr.createContainer(CON_NAME, config);
	assertEquals(cont.getContainerType(), config.getContainerType());
    }
    
    @Test
    public void testGetSetAllowValidation() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	assertEquals(XmlContainerConfig.DEFAULT.getAllowValidation(), config.getAllowValidation());
	config.setAllowValidation(false);
	assertEquals(false, config.getAllowValidation());
	
	config.setAllowValidation(true);
	assertEquals(true, config.getAllowValidation());
	//Create a container that does validation and insert a valid and invalid Xml document
	String valid = "<!DOCTYPE a [<!ELEMENT a    (b)><!ELEMENT b      (#PCDATA)>]><a><b>1</b></a>";
	String invalid = "<!DOCTYPE a [<!ELEMENT a    (b)><!ELEMENT b      (#PCDATA)>]><a><c>1</c></a>";
	cont = mgr.createContainer(CON_NAME, config);
	assertEquals(cont.getContainerConfig().getAllowValidation(), config.getAllowValidation());
	cont.putDocument("test1.xml", valid);
	try {
	    cont.putDocument("test2.xml", invalid);
	    fail("Failed in ContainerConfigTest.testGetSetAllowValidation().");
	} catch (XmlException e) {
	    assertNotNull(e);
	    assertEquals(XmlException.INDEXER_PARSER_ERROR, e.getErrorCode());
	}
    }

    //Test that the default values match the C++ default values
    @Test 
    public void testDefaultConfig() throws Exception
    {
	XmlContainerConfig config = new XmlContainerConfig();
	assertTrue(HelperFunctions.testConfigDefault(config));
	assertTrue(HelperFunctions.testConfigDefault(XmlContainerConfig.DEFAULT));	
    }	
    
    //Tests that the C++ bitwise operator for XmlContainerConfig works
    @Test 
    public void testConfigBitOperator() throws Exception
    {
	assertTrue(HelperFunctions.testConfigBitOperator(mgr));	
    }
    
    //Catch the error that DB prints when encryption is used in the Container but not the Environment
    //This is done in order to keep errorLog.txt clean if there are no real exceptions or errors
    class MaskEncryptionError implements ErrorHandler {
    	public void error(Environment arg0, String arg1, String arg2) {	
    	}
    }

    @Test
    public void testEncryption() throws Throwable {
    	//clean the environment
    	tearDown();

    	Environment env = null;
    	XmlContainer cont = null;
    	XmlManager mgr = null;
    	try {
    		EnvironmentConfig envConf = new EnvironmentConfig();
    		envConf.setEncrypted("abcd");
    		File file = new File(hp.getEnvironmentPath());
    		envConf.setAllowCreate(true);
    		envConf.setInitializeCache(true);
    		envConf.setCacheSize(5*1024*1024); // 5 MB cache
    		env = new Environment(file, envConf);
    		mgr = new XmlManager(env, null);
    		XmlContainerConfig conf = new XmlContainerConfig();
    		conf.setEncrypted("abcd");
    		cont = mgr.createContainer(CON_NAME, conf);
    	} finally {
    		if (cont != null) cont.delete();
    		cont = null;
    		if(mgr != null) mgr.delete();
    		mgr = null;
    		if (env != null) env.close();
    		env = null;
    	}

    	tearDown();

    	//It is an error to set encryption without setting it in the Environment
    	try {
    		EnvironmentConfig envConf = new EnvironmentConfig();
    		File file = new File(hp.getEnvironmentPath());
    		envConf.setAllowCreate(true);
    		envConf.setInitializeCache(true);
    		envConf.setCacheSize(5*1024*1024); // 5 MB cache
    		envConf.setErrorHandler(new MaskEncryptionError());
    		env = new Environment(file, envConf);
    		mgr = new XmlManager(env, null);
    		XmlContainerConfig conf = new XmlContainerConfig();
    		conf.setEncrypted("abcd");
    		cont = mgr.createContainer(CON_NAME, conf);
    		fail("Failure in ManagerConfigTest.testEncryption().");
    	}catch (XmlException e) {
    		assertEquals(XmlException.DATABASE_ERROR, e.getErrorCode());
    	} finally {
    		if (cont != null) cont.delete();
    		if(mgr != null) mgr.delete();
    		mgr = null;
    		if (env != null) env.close();
    		env = null;
    	}
    }
    
    @Test
    public void testTxnNotDurable() throws Throwable {
    	if (!hp.isTransactional()) return; //Test only makes sense in transactional environment
    	XmlContainerConfig config = new XmlContainerConfig();
    	config.setTransactional(true);
    	if (!hp.isNodeContainer())
    		config.setContainerType(XmlContainer.WholedocContainer);
    	config.setTransactionNotDurable(true);
    	
    	Environment env = null;
    	XmlContainer cont = null;
    	XmlManager mgr = null;
    	try {
    		env = hp.createTxnEnv(hp.getEnvironmentPath());
    		mgr = new XmlManager(env, null);
    		cont = hp.createContainer(CON_NAME, mgr, config);
    	} finally {
    		if (cont != null) cont.delete();
    		if(mgr != null) mgr.delete();
    		mgr = null;
    		if (env != null) env.close();
    		env = null;
    	}
    }
    
    @Test
    public void testReadUncommitted() throws Throwable {
    	if (!hp.isTransactional()) return; //Test only makes sense in transactional environment
    	XmlContainerConfig config = new XmlContainerConfig();
    	config.setTransactional(true);
    	if (!hp.isNodeContainer())
    		config.setContainerType(XmlContainer.WholedocContainer);
    	config.setReadUncommitted(true);

    	Environment env = null;
    	XmlContainer cont = null;
    	XmlManager mgr = null;
    	try {
    		env = hp.createTxnEnv(hp.getEnvironmentPath());
    		mgr = new XmlManager(env, null);
    		cont = hp.createContainer(CON_NAME, mgr, config);
    		config = cont.getContainerConfig();
    		assertTrue(config.getReadUncommitted());
    		//in 16904 these two setting where treated as the same value
    		assertFalse(config.getTransactionNotDurable());
    	}  finally {
    		if (cont != null) cont.delete();
    		if(mgr != null) mgr.delete();
    		mgr = null;
    		if (env != null) env.close();
    		env = null;
    	}
    }
}
