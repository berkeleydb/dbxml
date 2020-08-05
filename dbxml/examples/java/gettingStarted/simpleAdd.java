//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class simpleAdd
{
    private static String theContainer = "simpleExampleData.dbxml";
    private static void usage() {
	String usageMessage = "\nThis program adds a few simple XML documents to a specified container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.simpleAdd -h examplesEnvironment\n";

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
	XmlTransaction txn = null;
	XmlContainer openedContainer = null;
	try {
	    // Open an environment
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    // open a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer =
		theMgr.openContainer(theContainer, config);

	    // Start a transaction
	    txn = theMgr.createTransaction();

            //Create String contents for documents.
            String document1 = "<aDoc><title>doc1</title><color>green</color></aDoc>";
            String document2 = "<aDoc><title>doc2</title><color>yellow</color></aDoc>";
            //Put the document, asking DB XML to generate a name
	    XmlDocumentConfig docConfig = new XmlDocumentConfig();
	    docConfig.setGenerateName(true);
            String nm = openedContainer.putDocument(txn, "", document1,
						    docConfig);
	    System.out.println("Put document: " + nm);
            //Do it again for the second document
            nm = openedContainer.putDocument(txn, "", document2,
					     docConfig);
	    System.out.println("Put document: " + nm);
            //Commit the writes. This causes the container write operations
            //  to be saved to the container.
            txn.commit();
	} catch (Exception e) {
            System.err.println("Error performing document add against " + theContainer);
            System.err.println("   Message: " + e.getMessage() );
            if ( txn != null ) {
		txn.abort();
            }
            throw e;
	}
	finally {
	    cleanup(env, openedContainer);
	}

    } //End main

}
