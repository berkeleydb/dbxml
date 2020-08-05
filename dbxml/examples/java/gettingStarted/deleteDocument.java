//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class deleteDocument
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program deletes an XML document from a DBXML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the\n";
	usageMessage += "database environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.deleteDocument -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Method that deletes all documents from a DB XML container that match a given
    // XPath query.
    private static void doDeleteDocument( XmlManager mgr, XmlContainer container,
                                          String query, XmlQueryContext context,
                                          XmlTransaction txn )
	throws Throwable {
	String fullQuery = collection + query;
	System.out.println("Deleting documents for expression: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//Perform our query. We'll delete any document contained in this result set.
	XmlResults results = mgr.query(txn, fullQuery, context, null);
	System.out.println("Found " + results.size() +
			   " documents matching the expression '" + query + "'.");

	XmlValue value;
	value = results.next();
	while (value != null){

	    /// Retrieve the value as a document
	    XmlDocument document = value.asDocument();

	    //Obtain a unique feature off the document for reporting purposes
	    String item = getValue(mgr, document, "fn:string(/*/product)", context);
	    System.out.println("Deleting document: " + item + ".");

	    //Peform the delete
	    container.deleteDocument( txn, document);
	    System.out.println("Deleted document: " + item + ".");
	    value = results.next();
	}
	results.delete();
    }

    //Utility method that we use to obtain a specific value off of a XML document
    private static String getValue( XmlManager mgr, XmlDocument theDocument,
				    String query, XmlQueryContext context )
	throws Throwable {
        ///////////////////////////////////////////////////////////////////////////
        ////////    Return specific information from a document. //////////////////
        ////////   !!!!!! Assumes a result set of size 1 !!!!!!! //////////////////
        ///////////////////////////////////////////////////////////////////////////

        //Perform the query
        XmlQueryExpression queryExpr = mgr.prepare(query, context);
	XmlValue docVal = new XmlValue(theDocument);
        XmlResults result = queryExpr.execute(docVal, context, null);

        //We require a result set size of 1. If the result set size is
	// something else, then either the XML schema isn't what we
	// expected or we got the XPath query expression wrong.
        if (result.size() != 1) {
	    System.err.println("Error! XPath query '" + query +
			       "' returned a result size size != 1");
	    throw new Exception("getValue found result set not equal to 1.");
        }

        //Get the value. If we allowed the result set to be larger than size 1,
        // we would have to loop through the results, processing each as is
        // required by our application.
        XmlValue value = result.next();
	String retVal = value.asString();
	result.delete();
	queryExpr.delete();

        return retVal;
    }

    //Utility method that we use to make sure the documents that we thought
    // were deleted from the container are in fact deleted.
    private static void confirmDelete( XmlManager mgr, String query,
				       XmlQueryContext context )
	throws Throwable {
	String fullQuery = collection + query;
	System.out.println("Confirming the delete.");
	System.out.println("The query: '" + fullQuery +
			   "' should return result set size 0.");
	XmlResults results = mgr.query(fullQuery, context, null);
	if ( results.size() == 0) {
            System.out.println("Result set size is 0. Deletion confirmed.");
	} else {
            System.out.println("Result set size is " + results.size() +
			       ". Deletion failed.");
	}
	results.delete();
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
	    if (args[i].startsWith("-")){
		switch(args[i].charAt(1)){
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

            //Create a context and declare the namespaces
            XmlQueryContext context = theMgr.createQueryContext();
            context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
            context.setNamespace( "vegetables",
				  "http://groceryItem.dbxml/vegetables");
            context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

            //Delete the document that describes Mabolo (a fruit)
            String query = "/fruits:item[product = 'Mabolo']";

            //If doDeleteDocument throws an exception we want to abort the
	    // transaction. Otherwise, we want to commit the transaction
	    // and continue with other activities.
            try {
                doDeleteDocument(theMgr, openedContainer, query, context, txn);
            } catch (Exception e) {
		// The database is left in the same state as it was in before
		// we started this operation.
		if (txn != null) {
		    txn.abort();
		}
		throw e;
	    }

            //Commit the writes. This causes the container write operations
            //  to be saved to the container.
            txn.commit();
            //The document should now no longer exist in the container. Just for
	    // fun, confirm the delete.
            confirmDelete (theMgr, query, context);
	} catch (Exception e) {
	    System.err.println("Error performing document delete against " + theContainer);
	    System.err.println("   Message: " + e.getMessage());
	    if (txn != null) {
		txn.abort();
	    }
	    throw e;
	}
	finally {
	    cleanup(env, openedContainer);
	}
    } //End main

}
