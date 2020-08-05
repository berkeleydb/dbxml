//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class queryWithContext
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program illustrates how to query for documents that require namespace\n";
	usageMessage += "usage in the query. You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.queryWithContext -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Performs a query against a document using an XmlQueryContext.
    private static void doContextQuery( XmlManager mgr, String query,
					XmlQueryContext context )
	throws Throwable {
	String fullQuery = collection + query;
	//Perform a single query against the referenced container using
	// the referenced context.
	System.out.println("Exercising query: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//Perform the query
	XmlResults results = mgr.query(fullQuery, context, null);
	//Iterate over the results with an XmlValue
	XmlValue value = results.next();
	while (value != null) {
            System.out.println(value.asString());
            value = results.next();
	}
	System.out.println(results.size() + " results returned for query '"
			   + fullQuery + "'.");
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
	try {
	    //Create an environment
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    //Open a container in that environment
	    openedContainer =
		theMgr.openContainer(theContainer);

            //Create a context and use it to declare the namespaces
            XmlQueryContext context = theMgr.createQueryContext();

            context.setNamespace("fruits", "http://groceryItem.dbxml/fruits");
            context.setNamespace("vegetables",
				 "http://groceryItem.dbxml/vegetables");
            context.setNamespace("desserts", "http://groceryItem.dbxml/desserts");

            //Set a variable
            context.setVariableValue("aDessert",
				     new XmlValue("Blueberry Boy Bait"));

            //Perform the queries

            //Find all the Vendor documents in the database. Vendor documents do
            // not use namespaces, so this query returns documents.
            doContextQuery(theMgr, "/vendor", context);

            //Find the product document for "Lemon Grass".
	    // This query returns no documents
            // because a namespace prefix is not identified for the 'item' node.
            doContextQuery(theMgr,
			   "/item/product[.=\"Lemon Grass\"]", context);

            //Find the product document for "Lemon Grass" using the namespace
	    // prefix 'fruits'. This query successfully returns a document.
            doContextQuery(theMgr,
			   "/fruits:item/product[.=\"Lemon Grass\"]", context);

            //Find all the vegetables
            doContextQuery(theMgr, "/vegetables:item", context);

            //Find the  dessert called Blueberry Boy Bait.
            // Note the use of a variable.
            doContextQuery(theMgr,
			   "/desserts:item/product[.=$aDessert]", context);

       } catch (Exception e) {
           System.err.println("Error performing query against " + theContainer );
           System.err.println("   Message: " + e.getMessage() );
           throw e;
       }
	finally {
	    cleanup(env, openedContainer);
	}
    } //End main
}
