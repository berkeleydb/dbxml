//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;

import com.sleepycat.dbxml.*;
import com.sleepycat.db.*;

class retrieveDB
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String theDB = "testBerkeleyDB";

    private static void usage() {
        String usageMessage = "This program retrieves data from a Berkeley DB database based on information \n";
	usageMessage += "retrieved from a document in a DB XML container. Both the DB XML container and the\n";
	usageMessage += "Berkeley DB database are stored in the same database environment. The \n";
	usageMessage += "value of the node found on the XML document is used as the Berkeley DB key for the\n";
	usageMessage += "sample data stored in the database.\n\n";

	usageMessage += "Before running this program, run exampleLoadContainer to pre-populate\n";
	usageMessage += "the DB XML container with the appropriate data. Then, use the 'buildDB' sample\n";
	usageMessage += "program to build the Berkeley DB database that this program requires.\n\n";

	usageMessage += "When you run this program, identify the directory where you told exampleLoadContainer\n";
	usageMessage += "to place the sample data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.retrieveDB -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Utility function to clean up objects, exceptions or not,
    // containers and environments must be closed.
    private static void cleanup(myDbEnv env, XmlContainer openedContainer) {
	try {
	    if (openedContainer != null)
		openedContainer.delete();
	    if (env != null)
		env.cleanup();
	} catch (Exception e) {
	    // ignore exceptions on close
	}
    }

    public static void main(String args[])
	throws Throwable {

	File path2DbEnv = null;

	for(int i = 0; i < args.length; ++i) {
            if (args[i].startsWith("-")) {
		switch(args[i].charAt(1)) {
		case 'h':
		    path2DbEnv = new File( args[++i] );
		    break;
		default:
                    usage();
		}
            }
	}

	if (path2DbEnv == null || ! path2DbEnv.isDirectory()) {
            usage();
	}

	myDbEnv env = null;
	myDb openedDatabase = null;
	XmlContainer openedContainer = null;
	Transaction dbTxn = null;
	XmlTransaction txn = null;
	try {
	    // Open an environment
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    //Open a database in that environment
	    openedDatabase = new myDb(theDB, env.getEnvironment());

	    // open a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer =
		theMgr.openContainer(theContainer, config);

	    // Start a transaction via DB
	    dbTxn = env.getEnvironment().beginTransaction(null, null);
	    txn = theMgr.createTransaction(dbTxn);

	    //Obtain an XmlQueryContext with the result type set to
	    // values (not documents).
	    XmlQueryContext resultsContext = theMgr.createQueryContext();

	    String theQuery=
		"distinct-values(collection('namespaceExampleData.dbxml')/vendor/salesrep/name)";

	    //Get all the vendor documents out of the container
	    XmlResults results = theMgr.query(txn, theQuery,
					      resultsContext);
	    XmlValue value = results.next();
	    while (value != null) {
		    //Pull the value out of the document query result set.
		    String theSalesRepKey = value.asString();

		    DatabaseEntry theKey =
			new DatabaseEntry(theSalesRepKey.getBytes());
		    DatabaseEntry theData = new DatabaseEntry();

		    OperationStatus status = openedDatabase.getDatabase().get(txn.getTransaction(),
									      theKey, theData, null);
		    System.out.println("For key: " + theSalesRepKey +
				       ", retrieved:");
		    if (status == OperationStatus.NOTFOUND) {
			System.out.println("Key not found: run buildDB first");
		    } else
			System.out.println( new String(theData.getData(), 0,
						       theData.getSize()));
		    System.out.println("here");
		    value = results.next();
	    }
	    results.delete();
	    txn.commit();
	} catch (XmlException e) {
	    System.err.println("Xml error performing query against " + theContainer);
	    System.err.println("   Message: " + e.toString());
	    if ( txn != null ) {
		txn.abort();
	    }
	    throw e;
	}
	catch (DatabaseException de) {
	    System.err.println("Error performing query against " + theContainer);
	    System.err.println("   Message: " + de.toString());
	    if ( txn != null ) {
		txn.abort();
	    }
	    throw de;
	}
	finally {
	    if (openedDatabase != null)
		openedDatabase.cleanup();
	    cleanup(env, openedContainer);
	}
    } //End main

}
