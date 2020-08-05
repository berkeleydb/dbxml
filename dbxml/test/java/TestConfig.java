/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;
import java.io.File;
import java.io.FileNotFoundException;

import com.sleepycat.db.DatabaseException;
import com.sleepycat.db.Environment;
import com.sleepycat.db.EnvironmentConfig;
import com.sleepycat.dbxml.*;

/*
 * Each @Test case will typically create an instance of
 * this object and use it to factor out common operations
 * such as construction of Environment and XmlManager objects
 */
class TestConfig {

    String defaultEnvironment;
    boolean defaultNodeContainer;
    String defaultPath;
    Environment env = null;
    boolean cleanupEnv = true;

    public TestConfig(){
	defaultEnvironment = XmlTestRunner.getEnvironmentType();
	defaultNodeContainer = XmlTestRunner.isNodeContainer();
	defaultPath = XmlTestRunner.getEnvironmentPath();
    }

    public TestConfig(String env, boolean nodeContainer, String path){
	defaultEnvironment = env;
	defaultNodeContainer = nodeContainer;
	defaultPath = path;
    }

    public boolean isNodeContainer(){
	return defaultNodeContainer;
    }

    public String getEnvironmentType(){
	return defaultEnvironment;
    }

    public String getEnvironmentPath(){
	return defaultPath;
    }

    public boolean isTransactional(){
	return defaultEnvironment.equalsIgnoreCase("txn");
    }
  
    public boolean isCDS(){
	return defaultEnvironment.equalsIgnoreCase("cds");
    }

    public XmlManager createManager() 
	throws FileNotFoundException, XmlException, DatabaseException{
	return createManager(defaultPath, null);
    }

    public XmlManager createManager(String path, XmlManagerConfig config) 
	throws FileNotFoundException, XmlException, DatabaseException{
	if(defaultEnvironment.equalsIgnoreCase("none"))
	    return new XmlManager(config);
	else if(defaultEnvironment.equalsIgnoreCase("txn")){
	    env = createTxnEnv(defaultPath);
	    return new XmlManager(env, config);
	}else{
	    env = createCDBEnv(defaultPath);
	    return new XmlManager(env, config);
	}
    } 

    public XmlContainer createContainer(String name, XmlManager mgr)
	throws XmlException {
	XmlContainerConfig config = new XmlContainerConfig();
	config.setNodeContainer(defaultNodeContainer);
	if(defaultEnvironment.equalsIgnoreCase("txn"))
	    config.setTransactional(true);
	else
	    config.setTransactional(false);
	return mgr.createContainer(name, config);
    }

    public XmlContainer createContainer(String name, XmlManager mgr, 
					XmlContainerConfig config)
	throws XmlException {
	return mgr.createContainer(name, config);
    }

    public void closeEnvironment()
	throws DatabaseException {
	if (env != null && cleanupEnv) {
	    try {
		env.close();
	    } catch (DatabaseException e) {}
	    env = null;
	}
    }

    public void closeContainer(XmlContainer cont)
    {
	if (cont != null) {
	    try {
		cont.delete();
	    } catch (Throwable t) {}
	}
    }
    public void closeManager(XmlManager mgr)
    {
	if (mgr != null) {
	    try {
		mgr.delete();
	    } catch (Throwable t) {}
	}
    }

    /*
     * Clear the Environment, remove logXXXX,__dbXX,xx.dbxml,etc.
     */
    public static void fileRemove(String envPath) {
	if (envPath == null)
	    envPath = XmlTestRunner.getEnvironmentPath();
	File dir = new File(envPath);
	if (dir.isDirectory() == true) {

	    String[] fileNames = dir.list();

	    for (String file : fileNames) {

		if (file.startsWith("__db") || file.startsWith("log")
		    || file.endsWith("dbxml")) {
		    File f = new File(envPath + File.separator + file);
		    f.delete();
		}
	    }
	}
    }

    public static void clearEnv(String envPath, String containerName, Environment env,
				XmlManager mgr) {

	if (mgr != null) {
	    try {
		if (mgr.existsContainer(containerName) != 0) {
		    mgr.removeContainer(containerName);
		}
	    } catch (XmlException e) {
		System.err.println(e.getMessage());
	    }
	    mgr.delete();
	}
	if (env != null) {
	    try {
		env.close();
		Environment.remove(new File(envPath), false,
				   EnvironmentConfig.DEFAULT);
	    } catch (DatabaseException e) {
		System.err.println(e.getMessage());
	    } catch (FileNotFoundException e) {
		System.err.println(e.getMessage());
	    } catch (Exception e) {
		System.err.println(e.getMessage());
	    }

	}

	fileRemove(envPath);
    }

    /*
     * Create a transactional Environment
     */
    public Environment createTxnEnv(String envPath) 
	throws XmlException, FileNotFoundException, DatabaseException{

	File envHome;
	if (envPath != null && envPath.length() != 0) {
	    envHome = new File(envPath);
	} else {
	    envHome = new File(".");
	}

	EnvironmentConfig envConf = new EnvironmentConfig();
	envConf.setAllowCreate(true);
	envConf.setInitializeCache(true);
	envConf.setCacheSize(5*1024*1024); // 5 MB cache
	envConf.setInitializeLocking(true);
	envConf.setInitializeLogging(true);
	envConf.setTransactional(true);

	return new Environment(envHome, envConf);
    }
    /*
     * Create a CDB Environment
     */
    public Environment createCDBEnv(String envPath)
	throws XmlException, FileNotFoundException, DatabaseException{

	File envHome;
	if (envPath != null && envPath.length() != 0) {
	    envHome = new File(envPath);
	} else {
	    envHome = new File(".");
	}

	EnvironmentConfig envConf = new EnvironmentConfig();
	envConf.setAllowCreate(true);
	envConf.setInitializeCache(true);
	envConf.setCacheSize(5*1024*1024); // 5 MB cache
	envConf.setInitializeCDB(true);
	envConf.setCDBLockAllDatabases(true);
	envConf.setPrivate(true); // make it private, too

	return new Environment(envHome, envConf);
    }

    public Environment createEnvironment(String envPath)
	throws XmlException, FileNotFoundException, DatabaseException{
	if(defaultEnvironment.equalsIgnoreCase("none")){
	    File file = new File(envPath);
	    EnvironmentConfig conf = new EnvironmentConfig();
	    conf.setAllowCreate(true);
	    conf.setInitializeCache(true);
	    conf.setCacheSize(5*1024*1024); // 5 MB cache
	    return new Environment(file, conf);
	}else if(defaultEnvironment.equalsIgnoreCase("txn"))
	    return createTxnEnv(envPath);
	else if(defaultEnvironment.equalsIgnoreCase("cds"))
	    return createCDBEnv(envPath);
	else 
	    return null;
    }
}

