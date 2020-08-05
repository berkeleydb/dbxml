//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class queryForDocumentValue
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program retrieves DBXML documents and then retrieves information of\n";
	usageMessage += "interest from the retrieved document(s). You should run exampleLoadContainer\n";
	usageMessage += "before running this example. You are only required to pass this command the path\n";
	usageMessage += "location of the database environment that you specified when you loaded the\n";
	usageMessage += "examples data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.queryForDocumentValue -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    private static String getValue( XmlManager mgr, XmlDocument document,
                                    String query, XmlQueryContext context )
	throws Throwable {
        ///////////////////////////////////////////////////////////////////////////
        ////////    Return specific information from a document. //////////////////
        ////////          Assumes a result set of size 1         //////////////////
        ///////////////////////////////////////////////////////////////////////////

        //Perform the query
	// The document provides the context for the query, so neither
	// collection() nor doc() needs to be part of the query.
        XmlQueryExpression queryExpr = mgr.prepare(query, context);
	XmlValue docVal = new XmlValue(document);
        XmlResults result = queryExpr.execute(docVal, context, null);

        //We require a result set size of 1.
        if (result.size() != 1) {
	    System.err.println("Error!  query '" + query +
			       "' returned a result size size != 1");
	    throw new Exception( "getValue found result set not equal to 1.");
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

    private static void getDetails(XmlManager mgr,
				   String query, XmlQueryContext context)
	throws Throwable {
	String fullQuery = collection + query;
	//Perform a single query against the referenced container using
	// the referenced context.
	System.out.println("Exercising query: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//Perform the query
	XmlResults results = mgr.query(fullQuery, context, null);

	System.out.println("\n\tProduct : Price : Inventory Level\n");

	XmlValue value = results.next();
	while (value != null) {

            /// Retrieve the value as a document
            XmlDocument theDocument = value.asDocument();

            //Obtain information of interest from the document. Note that the
            //  wildcard in the query expression allows us to not worry about what
            //  namespace this document uses.
            String item = getValue(mgr, theDocument, "fn:string(/*/product)",
				   context);
            String price = getValue(mgr, theDocument,
				    "fn:string(/*/inventory/price)", context);
            String inventory = getValue(mgr, theDocument,
					"fn:string(/*/inventory/inventory)", context);

            System.out.println("\t" + item + " : " + price + " : " + inventory);
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

	//This example does not use transactions
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

            //Get details on Zulu Nuts
            getDetails(theMgr,
		       "/fruits:item[fn:string(product) = 'Zulu Nut']", context);

            //Get details on all fruits that start with 'A'
            getDetails(theMgr,
		       "/vegetables:item[starts-with(fn:string(product),'A')]",
		       context);

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
