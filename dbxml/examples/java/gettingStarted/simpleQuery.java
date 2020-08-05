//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class simpleQuery
{

    private static String theContainer = "simpleExampleData.dbxml";
    private static String collection="collection('simpleExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program performs various queries against a DBXML container.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.simpleQuery -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    private static void doQuery( XmlManager mgr, String query ) throws Throwable
    {
	//Perform a single query against the referenced container.
	// No context is used for this query.
	String fullQuery = collection + query;
	System.out.println("Exercising query: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//Perform the query

	XmlQueryContext context = mgr.createQueryContext();
	XmlResults results = mgr.query(fullQuery, context, null);
	//Iterate over the results of the query using an XmlValue object
	XmlValue value;
	while ((value = results.next()) != null){
            System.out.println(value.asString());
	}
	System.out.println(results.size() + " results returned for query '" +
			   fullQuery + "'.");
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
	try {
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    //Open a non-transactional container
	    openedContainer =
		theMgr.openContainer(theContainer);

            //Find all the Vendor documents in the database
            doQuery(theMgr, "/vendor");

            //Find all the vendors that are wholesale shops
            doQuery(theMgr, "/vendor[@type=\"wholesale\"]");

            //Find the product document for "Lemon Grass"
            doQuery(theMgr, "/product/item[.=\"Lemon Grass\"]");

            //Find all the products where the price is less than or equal to 0.11
            doQuery(theMgr, "/product/inventory[number(price)<=0.11]");

            //Find all the vegetables where the price is less than or equal to 0.11
            doQuery(theMgr,
		    "/product[number(inventory/price)<=0.11 and category=\"vegetables\"]");
	} catch (Exception e) {
            System.err.println("Error performing query against " + theContainer);
            System.err.println("   Message: " + e.getMessage());
            throw e;
       }
       finally {
	   cleanup(env, openedContainer);
       }
    } //End main
}
