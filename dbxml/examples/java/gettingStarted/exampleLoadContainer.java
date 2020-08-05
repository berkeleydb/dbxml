//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import java.util.*;

import com.sleepycat.db.*;
import com.sleepycat.dbxml.*;

class exampleLoadContainer
{

    private static void usage() {
	String usageMessage = "\nThis program loads the examples XML data into the examples container.\n";
	usageMessage += "Provide the directory where you want to place your database environment, \n";
	usageMessage += "and the path to the xmlData directory (this exists in your DB XML examples\n";
	usageMessage += "directory).\n\n";

	usageMessage += "\t-h <dbenv directory> -p <filepath>\n";

	usageMessage += "For example:\n";
	usageMessage += "\tjava com.sleepycat.dbxml.examples.gettingStarted.exampleLoadContainer -h examplesEnvironment -p /home/user1/dbxml-1.1.0/examples/xmlData\n";

	System.out.println(usageMessage);
	System.exit( -1 );
    }

    public static void main(String args[])
	throws Throwable {

	File path2DbEnv = null;
	File filePath = null;

	for(int i = 0; i < args.length; ++i)  {
	    if (args[i].startsWith("-")) {
                switch(args[i].charAt(1)) {
		case 'h':
                    path2DbEnv = new File(args[++i]);
                    break;
		case 'p':
                    filePath = new File(args[++i]);
                    break;
		default:
                    usage();
                }
            }
	}

	if (path2DbEnv == null || filePath == null) {
	    usage();
	}

	if (! filePath.isDirectory()) {
	    usage();
	}

	// This vector will hold a File object for each XML file that we will load
	// into the examples container
	List<File> files2add = new LinkedList<File>();

	// Within .../examples/xmlData there should be two subdirectories, nsData
	// and simpleData.  These contain the XML files that we want to load.
	// So check to make sure they exist.
	File nsData = new File(filePath.getPath() + File.separator + "nsData");
	confirmDirectory(nsData);

	File simpleData = new File(filePath.getPath() + File.separator + "simpleData");
	confirmDirectory(simpleData);

	//Load the first set of examples xml files into our vector
	getXmlFiles(nsData, files2add);
	//Add these files to the namespace container.

	loadXmlFiles(path2DbEnv, "namespaceExampleData.dbxml", files2add);

	files2add.clear();
	//Repeat for the XML example data that does not use namespaces
	getXmlFiles(simpleData, files2add);
	loadXmlFiles(path2DbEnv, "simpleExampleData.dbxml", files2add);

    } // End method main()

    //Convienance method used to make sure -p points to a directory that exists
    private static void confirmDirectory(File directory) {
	if ( ! directory.isDirectory() ) {
	    System.out.println( "\nError. Directory " + directory.getPath() +
				" does not exist.");
	    System.out.println( "       -p must point to the xmlData directory.");
	    usage();
	}
    }

    //Find all the xml files in a specified directory and store them in a vector
    private static void getXmlFiles(File filePath, List<File> files2add) {
	boolean filesFound = false;
	String [] dirContents = filePath.list();
	if (dirContents != null) {
	    for (int i = 0; i < dirContents.length; i++) {
		File entry = new File(filePath + File.separator + dirContents[i]);
		if (entry.isFile() && entry.toString().toLowerCase().endsWith(".xml")) {
		    files2add.add(entry);
		    filesFound = true;
		}
	    }
	}

	if (! filesFound) {
	    System.out.println("\nError: No XML files found at " +
			       filePath.getPath());
	    usage();
	}

    }

    //Utility function to clean up objects, exceptions or not
    // XmlContainer and XmlManager objects must be closed.
    private static void cleanup(XmlManager theMgr, XmlContainer openedContainer) {
	try {
	    if (openedContainer != null)
		openedContainer.delete();
	    if (theMgr != null)
		theMgr.delete();
	} catch (Exception e) {
	    // ignore exceptions on close
	}
    }

    // create an environment.  Will throw if home doesn't exist
    private static Environment createEnv(File home)
	throws DatabaseException, FileNotFoundException {

        EnvironmentConfig config = new EnvironmentConfig();
        config.setCacheSize(50 * 1024 * 1024);
        config.setAllowCreate(true);
        config.setInitializeCache(true);
        config.setTransactional(true);
        config.setInitializeLocking(true);
	config.setInitializeLogging(true);
        return new Environment(home, config);
    }

    //Take a vector of Files and load each element into a DB XML container
    private static void loadXmlFiles(File path2DbEnv, String theContainer,
				     List files2add)
	throws Throwable {
	//Open a container in the db environment
	XmlManager theMgr = null;
	XmlContainer openedContainer = null;
	XmlTransaction txn = null;
	Environment env = null;
	try {
	    env = createEnv(path2DbEnv);
	    theMgr = new XmlManager(env, new XmlManagerConfig());
	    // create a transactional container
	    XmlContainerConfig config = new XmlContainerConfig();
	    config.setTransactional(true);
	    openedContainer = theMgr.createContainer(theContainer, config);

	    // Get another transaction, via DB.  This just
	    // demonstrates that a Transaction created from DB can be
	    // passed to XmlManager.createTransaction.
	    Transaction dbtxn = env.beginTransaction(null,null);
	    txn = theMgr.createTransaction(dbtxn);
	    Iterator filesIterator = files2add.iterator();
	    while(filesIterator.hasNext()) {
		File file = (File) filesIterator.next();
		String theFile = file.toString();

		//Load the contents of the XML file into a String
		String theLine = null;
		String xmlString = new String();
		FileInputStream fis = new FileInputStream(theFile);
		BufferedReader br = new BufferedReader(new InputStreamReader(fis));
		while((theLine=br.readLine()) != null) {
		    xmlString += theLine;
		    xmlString += "\n";
		}
		br.close();

		//Declare an xml document
		XmlDocument xmlDoc = theMgr.createDocument();
		//Set the xml document's content to the xmlString we just obtained.
		xmlDoc.setContent(xmlString);

		//Set the document name
		xmlDoc.setName(file.getName());

		Date theDate = new Date();
		xmlDoc.setMetaData(mdConst.uri, mdConst.name,
				   new XmlValue(theDate.toString()));

		//Place that document into the container */
		openedContainer.putDocument(txn, xmlDoc);
		System.out.println("Added " + theFile + " to container" +
				   theContainer);
	    }
	    txn.commit();
	    //XmlException extends DatabaseException, which in turn extends Exception.
	    //  Catching Exception catches them all.
	} catch (Exception e) {

	    System.err.println("Error loading files into container " + theContainer);
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
	    cleanup(theMgr, openedContainer);
	}
    }

} // End class exampleLoadData
