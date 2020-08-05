/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 */

package basic;

import java.io.*;

import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlManagerConfig;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlContainerConfig;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlUpdateContext;
import com.sleepycat.dbxml.XmlQueryContext;

import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.db.Environment;
import com.sleepycat.db.EnvironmentConfig;
import com.sleepycat.db.DatabaseException;


/**
 * HelloWorld is the simplest possible Berkeley DB XML program
 * that does something.
 * This program demonstrates transactional initialization and container creation,
 * transactional document insertion and retrieval by name.
 * <p>
 *
 * The length of this program is due to the use of configuration
 * objects, and the need for explicit deletion/cleanup of Java
 * objects in order to release resources.
 * <p>
 * After running this program, you will notice a number of files
 * in the environment directory:
 * <pre>
 *   __db.* files are the BDB environment, including cache
 *   log.* files are BDB log files for transactions
 * </pre>
 * <p>
 * The actual BDB XML container is not present, since it's only
 * created in-memory, and will disappear at the end of the program.
 * To run the example:
 * <pre>
 * java basic.HelloWorldTxn [-h environmentDirectory]
 * </pre>
 */

class HelloWorldTxn
{
    static void usage()
    {
	System.err.println("Usage: java basic.HelloWorldTxn [-h environmentDirectory]");
	System.exit(-1);
    }

    // This function is used to ensure that databases are
    // properly closed, even on exceptions
    private static void cleanup(Environment env,
				XmlManager mgr, 
				XmlContainer cont) {
	try {
	    if (cont != null)
		cont.delete();
	    if (mgr != null)
		mgr.delete();
	    if (env != null)
		env.close();
	} catch (Exception e) {
	    // ignore exceptions in cleanup
	}
    }

    private static Environment createEnvironment(String home)
	throws Throwable {
	EnvironmentConfig config = new EnvironmentConfig();
	config.setTransactional(true);
	config.setAllowCreate(true);
	config.setRunRecovery(true);
	config.setInitializeCache(true);
	config.setCacheSize(25 * 1024 * 1024); // 25MB cache
	config.setInitializeLocking(true);
	config.setInitializeLogging(true);
	config.setErrorStream(System.err);
	File f = new File(home);
	return new Environment(f, config);
    }

    public static void main(String args[])
	throws Throwable {

	// An empty string means an in-memory container, which
	// will not be persisted
	String containerName = "";
	String content = "<hello>Hello World</hello>";
	String docName = "doc";
	String environmentDir = ".";

	if (args.length == 2) {
	    environmentDir = args[1];
	} else if (args.length != 0)
	    usage();

	XmlManager mgr = null;
	XmlContainer cont = null;
	Environment env = null;
	try {
	    // Create and open a Berkeley DB Transactional Environment.
	    env = createEnvironment(environmentDir);

	    // All BDB XML programs require an XmlManager instance.
	    // Create it from the DB Environment, but do not adopt the
	    // Environment
	    XmlManagerConfig mconfig = new XmlManagerConfig();
	    mconfig.setAllowExternalAccess(true);
	    mgr = new XmlManager(env, mconfig);

	    
	    // Create a container that is transactional.  Specify
	    // that it is also a Node Storage container, with nodes
	    // indexed
	    XmlContainerConfig cconfig = new XmlContainerConfig();
	    cconfig.setContainerType(XmlContainer.NodeContainer);
	    cconfig.setIndexNodes(XmlContainerConfig.On);
	    cconfig.setTransactional(true);
	    cont = mgr.createContainer(containerName, cconfig);
	    
	    // Perform the putDocument in a transaction, created
	    // from the XmlManager
	    XmlTransaction txn = mgr.createTransaction();
	    
	    cont.putDocument(txn, docName, content);

	    // commit the Transaction
	    txn.commit();

	    // Now, get the document
	    txn = mgr.createTransaction();
	    XmlDocument doc = cont.getDocument(txn, docName);
	    String name = doc.getName();
	    String docContent = doc.getContentAsString();

	    // commit the transaction.  Note that this is done
	    // after getting the document content.  This is necessary
	    // as getting the content will touch the database.
	    // Also, given that this is a read-only operation, abort()
	    // would be just as appropriate.
	    txn.commit();
	    
	    // print it
	    System.out.println("Document name: " + name + "\nContent: " +
			       docContent);
	} 
	catch (XmlException xe) {
	    System.err.println("XmlException during HelloWorldTxn: " + 
			       xe.getMessage());
	    throw xe;
	}
	catch (DatabaseException de) {
	    System.err.println("DatabaseException during HelloWorldTxn: " + 
			       de.getMessage());
	    throw de;
	}
	finally {
	    cleanup(env, mgr, cont);
	}
    }
}

