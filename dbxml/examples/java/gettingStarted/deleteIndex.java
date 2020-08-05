//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;

import com.sleepycat.dbxml.*;

class deleteIndex
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static void usage() {
	String usageMessage = "\nThis program deletes several indexes from a DBXML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n\n";

	usageMessage += "For best results, run addIndex before running this program.\n\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.deleteIndex -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Deletes an index from the specified container
    private static void deleteAnIndex( XmlManager xmlDB, XmlContainer container,
				       String URI, String nodeName,
				       String indexType, XmlTransaction txn )
	throws Throwable {
	System.out.println("Deleting index type " + indexType + " from node "
			   + nodeName);

	//Retrieve the index specification from the container
	XmlIndexSpecification idxSpec = container.getIndexSpecification(txn);

	//See what indexes exist on the container
	int count = 0;
	System.out.println("Before the delete, the following indexes are maintained for the container:");
	//Use an XmlIndexDeclaration to loop over the indexes and
	// report what's there.
	XmlIndexDeclaration idxDecl = null;
	while((idxDecl = (idxSpec.next())) != null) {
	    System.out.println("\tFor node '" + idxDecl.name + "', found index: '"
			       + idxDecl.index + "'.");
	    count ++;
	}

	System.out.println(count + " indexes found.");

	//Delete the indexes from the specification.
	idxSpec.deleteIndex(URI, nodeName, indexType);

	//Set the specification back to the container
	container.setIndexSpecification(txn, idxSpec, xmlDB.createUpdateContext());

	//Look at the indexes again to make sure our deletion took.
	count = 0;
	idxSpec.reset();
	System.out.println("After the delete, the following indexes are maintained for the container:");
	while((idxDecl = (idxSpec.next())) != null) {
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
       XmlContainer openedContainer = null;
       XmlTransaction txn = null;
       try {
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    // open a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer =
		theMgr.openContainer(theContainer, config);

	    // Start a transaction
	    txn = theMgr.createTransaction();

	    //Delete an string equality index for the "product" element node.
	    deleteAnIndex(theMgr, openedContainer, "", "product",
			"node-element-equality-string", txn);

	    //Perform the deletes in two different transactions for
	    // no particular reason

	    //Commit the index delete
	    txn.commit();
	    txn = theMgr.createTransaction();
	    //Delete an edge presence index for the product node
	    deleteAnIndex(theMgr, openedContainer, "", "product",
			"edge-element-presence-none", txn);

	    //Commit the index delete
	    txn.commit();
       } catch (Exception e) {
           System.err.println("Error deleting indexes from container "
			      + theContainer );
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
