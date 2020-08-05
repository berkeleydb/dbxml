//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;

import com.sleepycat.dbxml.*;

class updateDocument
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program updates a document stored in a DB XML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the\n";
	usageMessage += "database environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.updateDocument -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Modifies an XML document that is stored in a DB XML container
    private static void doUpdateDocument(XmlManager mgr, XmlContainer container,
					 String query, XmlQueryContext context,
					 XmlTransaction txn )
	throws Throwable {
	String fullQuery = collection + query;
	System.out.println("Updating documents for expression: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//query for all the documents that we want to update
	XmlResults results = mgr.query(txn, fullQuery, context, null);
	System.out.println("Found " + results.size() + " matching the expression '"
			   + fullQuery + "'.");

	XmlValue value = results.next();
	while (value != null) {

	    XmlDocument document = value.asDocument();
	    //Retrieve the entire document as a single String object
	    String docString = document.getContentAsString();
	    System.out.println("Updating document: ");
	    System.out.println(docString);

	    //This next method just modifies the document string
	    //in a small way.
	    String newDocString = getNewDocument(mgr, document, context,
						 docString);

	    System.out.println("Updating document...");

	    //Set the document's content to be the new document string
	    document.setContent(newDocString);

	    //Now replace the document in the container
	    container.updateDocument(txn, document);
	    System.out.println("Document updated.");
	    value = results.next();
	}
	results.delete();
    }

    private static String getNewDocument( XmlManager dbXML, XmlDocument document,
					  XmlQueryContext context,
					  String docString )
	throws Throwable {
	//Get the substring that we want to replace
	String inventory = getValue(dbXML, document,
				    "fn:string(/*/inventory/inventory)", context);
	String retVal = new String();

	//Find the substring in the original document string
	int pos = docString.indexOf(inventory);
	if (pos == -1) {
	    System.err.println("Error: inventory string: '" + inventory +
			       "' not found in document string:");
	    System.err.println(docString);

	} else {
	    //Convert the String representation of the inventory level to an
	    // integer, increment by 1, and then convert back to a String for
	    // replacement on the document.
	    int newInventory = Integer.parseInt(inventory) + 1;
	    String newVal = Integer.toString(newInventory);

	    int inventoryLength = inventory.length();
	    int endPos = pos + inventoryLength;

	    StringBuffer strbuff = new StringBuffer(document.getContentAsString());

	    //Perform the replace
	    strbuff.replace(pos, endPos, newVal);
	    System.out.println("Inventory was " + inventory + ", it is now " +
			       newVal + ".");
	    retVal = strbuff.toString();
	}

	return retVal;
    }

    private static String getValue(XmlManager mgr, XmlDocument theDocument,
                                   String query, XmlQueryContext context )
	throws Throwable {

        //Perform the query
	// The document provides the context for the query
        XmlQueryExpression queryExpr = mgr.prepare(query, context);
	XmlValue docVal = new XmlValue(theDocument);
        XmlResults result = queryExpr.execute(docVal, context, null);

        //We require a result set size of 1.
        if (result.size() != 1) {
	    System.err.println("Error! query '" + query +
			       "' returned a result size size != 1");
	    throw new Exception( "getValue found result set not equal to 1.");
        }

        //Get the value. If we allowed the result set to be larger than size 1,
        //we would have to loop through the results, processing each as is
        //required by our application.
        XmlValue value = result.next();
	String retVal = value.asString();
	result.delete();
	queryExpr.delete();

        return retVal;

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
	XmlContainer openedContainer = null;
	XmlTransaction txn = null;
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

            XmlQueryContext context = theMgr.createQueryContext();
            context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
            context.setNamespace( "vegetables",
				  "http://groceryItem.dbxml/vegetables");
            context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

            //update the document that describes Zapote Blanco (a fruit)
            String query = "/fruits:item[fn:string(product) = 'Zapote Blanco']";
            doUpdateDocument(theMgr, openedContainer, query, context, txn);

            //Commit the writes. This causes the container write operations
            //  to be saved to the container.
            txn.commit();
	} catch (Exception e) {
	    System.err.println("Error performing document update against "
			       + theContainer);
	    System.err.println("   Message: " + e.getMessage());
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
