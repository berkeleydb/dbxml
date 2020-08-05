//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class queryWithDocumentNames
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program retrieves DB XML documents using their document names.\n";
	usageMessage += "You should run exampleLoadContainer before running this example.\n";
	usageMessage += "You are only required to pass this command the path location of the database\n";
	usageMessage += "environment that you specified when you loaded the examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.queryWithDocumentNames -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Performs an XPath query against a container using the given context.
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
	//We're going to iterate over the results using both an XmlValue and
	// an XmlDocument
	XmlValue value = results.next();
	while (value != null) {
            XmlDocument document = value.asDocument();
            System.out.println("Document name: " + document.getName());
            System.out.println(value.asString());
            value = results.next();
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

            //Create a context and declare the namespaces
            XmlQueryContext context = theMgr.createQueryContext();
            context.setNamespace("fruits", "http://groceryItem.dbxml/fruits");
            context.setNamespace("vegetables", "http://groceryItem.dbxml/vegetables");
            context.setNamespace("desserts", "http://groceryItem.dbxml/desserts");

            //Query for documents using their document names.
            // The name is stored on the root node of the document using the
	    // name attribute in the dbxml namespace. Notice that you do
	    // NOT have to declare the dbxml namespace on the
            // XmlQueryContext used for this query. Also, each document name
	    // was set by exampleLoadContainer when the document was loaded into
	    // the XmlContainer.
            doContextQuery(theMgr, "/*[dbxml:metadata('dbxml:name')='ZuluNut.xml']",
			   context);
            doContextQuery(theMgr,
			   "/*[dbxml:metadata('dbxml:name')='TrifleOrange.xml']",
			   context);
            doContextQuery(theMgr,
			   "/*[dbxml:metadata('dbxml:name')='TriCountyProduce.xml']",
			   context);
            doContextQuery(theMgr,
			   "/fruits:item/product[text()=\"Zulu Nut\"]",
			   context);
	} catch (Exception e) {
            System.err.println("Error performing query against " + theContainer);
            System.err.println("   Message: " + e.getMessage());
            throw e;
	}
	finally {
	    cleanup(env, openedContainer);
	}
    }
} //end main
