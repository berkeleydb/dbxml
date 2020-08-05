//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.dbxml.*;

class queryForMetaData
{

    private static String theContainer = "namespaceExampleData.dbxml";
    private static String collection = "collection('namespaceExampleData.dbxml')";
    private static void usage() {
	String usageMessage = "\nThis program retrieves DBXML documents and then retrieves the date and time\n";
	usageMessage += "that the document was stored in the container. You should run exampleLoadContainer\n";
	usageMessage += "before running this example. You are only required to pass this command the path\n";
	usageMessage += "location of the database environment that you specified when you loaded the examples\n";
	usageMessage += "data:\n\n";

	usageMessage += "\t-h <dbenv directory>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.queryForMetaData -h examplesEnvironment\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    //Shows a timestamp for each record that matches the given XPath query.
    // The timestamp is stored as metadata on each document. This metadata was
    // added to the document when the example data was loaded into the container
    // using exampleLoadContainer.java. The timestamp represents the time
    // when the document was loaded into the container.
    private static void showTimeStamp( XmlManager mgr, String query,
				       XmlQueryContext context )
	throws Throwable {
	String fullQuery = collection + query;
	//Perform a single query against the referenced container using
	// the referenced context. The timestamp metadata attribute is then
	// displayed.
	System.out.println("Exercising query: '" + fullQuery + "'.");
	System.out.println("Return to continue: ");
	System.in.read();

	//Perform the query
	XmlResults results = mgr.query(fullQuery, context, null);

	XmlValue value = results.next();
	while (value != null) {
	    //We return the metadata as an XmlValue
	    XmlValue metaValue = new XmlValue();

	    XmlDocument document = value.asDocument();

	    //retrieve the metadata using the uri and name stored in the
	    // mdConst utility class. This uri and name are the same values
	    // used when the metadata was stored on the document.
	    document.getMetaData(mdConst.uri, mdConst.name, metaValue );
	    System.out.println("Document " + document.getName() + " stored on "
			       + metaValue.asString());
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
	    //Open an environment
	    env = new myDbEnv(path2DbEnv);
	    XmlManager theMgr = env.getManager();

	    //Open a container in that environment
	    openedContainer =
		theMgr.openContainer(theContainer);

            //Create a context and declare the namespaces
            XmlQueryContext context = theMgr.createQueryContext();

            context.setNamespace("fruits", "http://groceryItem.dbxml/fruits");
            context.setNamespace("vegetables",
				 "http://groceryItem.dbxml/vegetables");
            context.setNamespace("desserts", "http://groceryItem.dbxml/desserts");

            //Show all the vegetables
            showTimeStamp(theMgr, "/vegetables:item", context);
	} catch (Exception e) {
            System.err.println("Error performing query against " + theContainer );
            System.err.println("   Message: " + e.getMessage() );
            throw e;
	}
	finally {
	    cleanup(env, openedContainer);
	}
    }
} //End main

