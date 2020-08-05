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

class buildDB
{

	private static String theContainer = "namespaceExampleData.dbxml";
	private static String theDB = "testBerkeleyDB";

	private static void usage() {
		String usageMessage = "This program retrieves node information from a specific set of XML \n";
	usageMessage += "documents stored in a DB XML container, and then saves relevant sample\n";
	usageMessage += "data to a Berkeley DB database. Both the DB XML container and the Berkeley\n";
	usageMessage += "DB database are stored in the same database environment. The value of\n";
	usageMessage += "the node found on the XML document is used as the Berkeley DB key for the\n";
	usageMessage += "sample data stored in the database.\n\n";

	usageMessage += "Use the 'retrieveDB' sample program to retrieve the stored data.\n\n";

	usageMessage += "Before running this program, run exampleLoadContainer to pre-populate\n";
	usageMessage += "the DB XML container with the appropriate data.\n\n";

	usageMessage += "When you run this program, identify the directory where you told exampleLoadContainer\n";
	usageMessage += "to place the sample data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.buildDB -h examplesEnvironment\n";

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
			if ( args[i].startsWith("-") ) {
				switch( args[i].charAt(1) ) {
				case 'h':
					path2DbEnv = new File(args[++i]);
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
	XmlTransaction txn = null;
	try {
		// Open an environment
		env = new myDbEnv(path2DbEnv);
		XmlManager theMgr = env.getManager();

		//Open a database in that environment (creates Transaction)
		openedDatabase = new myDb(theDB, env.getEnvironment());

		// open a transactional container
		XmlContainerConfig config = new XmlContainerConfig();
		config.setTransactional(true);
		openedContainer = theMgr.openContainer(theContainer, config);

		// Create a transaction, using DB.
		Transaction dbTxn =
		env.getEnvironment().beginTransaction(null, null);
		txn = theMgr.createTransaction(dbTxn);

		XmlQueryContext resultsContext = theMgr.createQueryContext();

		String theQuery=
		"distinct-values(collection('namespaceExampleData.dbxml')/vendor/salesrep/name)";
		//Perform the query against the XmlManager environment
		XmlResults results = theMgr.query(txn, theQuery,
						  resultsContext);

		//Pull the value out of the document query result set.
		XmlValue value = results.next();
		while (value != null) {
			String theSalesRepKey = value.asString();
			System.out.println("Putting data for key: " + theSalesRepKey);

			//This is the data that we're putting into the database. A real
			// world example would probably include a BLOB, such as a jpeg
			// image of the salereps, a public key, or some other bit of
			// information that doesn't fit into the XML document schema.
			// In our case, we'll just put in a descriptive string so we
			// can see what is going on when we retrieve this data.
			String theSalesRepData =
				"This is the data stored in the database for " +
				theSalesRepKey + ".";

			DatabaseEntry theKey =
				new DatabaseEntry(theSalesRepKey.getBytes());
			DatabaseEntry theData =
				new DatabaseEntry(theSalesRepData.getBytes());

			//Finally, write the actual data to the Berkeley DB.
			openedDatabase.getDatabase().
			putNoOverwrite(txn.getTransaction(), theKey, theData);
			value = results.next();
		}
		results.delete();
		//This causes the write to be made permanent.
		txn.commit();
	} catch (Exception e) {

		System.err.println("Error performing query against " + theContainer);
		System.err.println("   Message: " + e.getMessage());
		//In the event of an error, we abort the operation
		// The database is left in the same state as it was in
		// we started this operation.
		if ( txn != null ) {
		    txn.abort();
		}
		throw e;

	} finally {
		if (openedDatabase != null) {
		    openedDatabase.cleanup();
		}
		cleanup(env, openedContainer);
	   }

	} //End main
}
