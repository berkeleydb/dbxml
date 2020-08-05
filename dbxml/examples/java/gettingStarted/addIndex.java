//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class addIndex
{
    private static void usage() {
	String usageMessage = "\nThis program adds several indexes to a DBXML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.addIndex -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    private static void addAnIndex( XmlContainer container, String URI,
				    String nodeName,
				    String indexType, XmlTransaction txn,
				    XmlUpdateContext updateContext )
	throws Throwable {
	System.out.println("Adding index type " + indexType + " to node " +
			   nodeName);

	//Retrieve the index specification from the container
	XmlIndexSpecification idxSpec = container.getIndexSpecification(txn);

	//See what indexes exist on the container
	int count = 0;
	System.out.println("Before index add.");
	XmlIndexDeclaration idxDecl = null;
	while((idxDecl = (idxSpec.next())) != null) {
	    System.out.println("\tFor node '" + idxDecl.name + "', found index: '"
			       + idxDecl.index + "'.");
	    count ++;
	}

	System.out.println(count + " indexes found.");

	//Add the index to the specification.
	//If it already exists, then this does nothing.
	idxSpec.addIndex(URI, nodeName, indexType);

	//Set the specification back to the container
	container.setIndexSpecification(txn, idxSpec, updateContext);

	//Look at the indexes again to make sure our replacement took.
	count = 0;
	idxSpec.reset();
	System.out.println("After index add.");
	while((idxDecl = ( idxSpec.next() ) ) != null) {
	    System.out.println("\tFor node '" + idxDecl.name + "', found index: '"
			       + idxDecl.index + "'.");
	    count ++;
	}
	idxSpec.delete();
	System.out.println(count + " indexes found.");
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
	String theContainer = "namespaceExampleData.dbxml";
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

	XmlTransaction txn = null;
	XmlContainer openedContainer = null;
	myDbEnv env = null;
	try {
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    // open a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer =
		theMgr.openContainer(theContainer, config);

	    // need update context for modifications
	    XmlUpdateContext uc = theMgr.createUpdateContext();

	    // Start a transaction
	    txn = theMgr.createTransaction();

	    //Add an string equality index for the "product" element node.
	    addAnIndex( openedContainer, "", "product",
			"node-element-equality-string", txn, uc );
            //Add an edge presence index for the product node
	    addAnIndex( openedContainer, "", "product", "edge-element-presence",
			txn, uc );

	    //Commit the index adds
	    txn.commit();
	} catch (Exception e) {

	    //Both DB XML and Berkeley DB throw exceptions that are inherited from
	    // Exception. Therefore, catching Exception catches everything.
	    System.err.println("Error adding index to container " + theContainer);
	    System.err.println("   Message: " + e.getMessage() );
	    //In the event of an error, we abort the operation
	    // The database is left in the same state as it was in before
	    // we started this operation.
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
