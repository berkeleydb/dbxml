/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* queryWithDocumentNames
*
* This program retrieves Berkeley DB XML documents using their document names. You should
* pre-load the container using loadExamplesData.[sh|cmd] before running this
* example.  You are only required to pass this command the path location of the
* database environment that you specified when you pre-loaded the examples
* data.
* 
* Usage: queryWithDocumentNames -h <dbenv directory> 
*
* This program demonstrates:
*  How to retrieve an xml document by its metadata name
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information in a query
*  How to get the document by quering for the document metadata name
*  How to get the document from the container using the document name
*
*/

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;

void usage()
{
	std::cerr <<  "This program retrieves DBXML documents using their document names. You should\n"
		<<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
		<<  "example.  You are only required to pass this command the path location of the\n"
		<<  "database environment that you specified when you pre-loaded the examples\n"
		<<  "data:\n\n"

		<<  "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

void doContextQuery( XmlTransaction &txn, XmlManager &mgr, const std::string cname,
					const std::string &query, XmlQueryContext &context )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs a simple query (with context) against the ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + cname + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);
		std::cout << "\n";

		XmlResults results( mgr.query(txn, fullQuery, context ) );
		XmlValue value;
		while(results.next(value)) {
			// Get the document's name and print it to the console
			XmlDocument theDocument = value.asDocument();
			std::cout << "Document name: " << theDocument.getName() << std::endl;
			std::cout << value.asString() << std::endl;
		}

		std::cout << (unsigned int)results.size()
			<< " objects returned for expression '" << fullQuery
			<< "'\n" << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		txn.abort();
		exit( -1 );
	}
}

//Get a document from the container using the document name
void doGetDocument( XmlTransaction &txn, XmlContainer &container, const std::string docname)
{

	try {
		std::cout << "Getting document '" << docname << "' from the container." << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);
		std::cout << "\n";

		//Get the document from the container using the document name
		XmlDocument theDocument = container.getDocument(txn, docname);
		std::string content;
		std::cout << "Document name: " << theDocument.getName() << std::endl;
		std::cout << theDocument.getContent(content) << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Get document from container failed.\n";
		std::cerr << e.what() << "\n";
		txn.abort();
		exit( -1 );
	}
}

int main(int argc, char **argv)
{
	std::string path2DbEnv;
	std::string theContainer = "namespaceExampleData.dbxml";
	for ( int i=1; i<argc; i++ )
	{
		if ( argv[i][0] == '-' )
		{
			switch(argv[i][1])
			{
			case 'h':
				path2DbEnv = argv[++i];
				break;
			default:
				usage();
			}
		}
	}

	if (! path2DbEnv.length() )
		usage();

	// Berkeley DB environment flags
	u_int32_t envFlags = DB_RECOVER|DB_CREATE|DB_INIT_MPOOL|
		DB_INIT_LOCK|DB_INIT_TXN|DB_INIT_LOG;
	// Berkeley DB cache size (64 MB).  The default is quite small
	u_int32_t envCacheSize = 64*1024*1024;

	// Create and open a Berkeley DB Transactional Environment.
	int dberr;
	DB_ENV *dbEnv = 0;
	dberr = db_env_create(&dbEnv, 0);
	if (dberr == 0) {
		dbEnv->set_cachesize(dbEnv, 0, envCacheSize, 1);
		dberr = dbEnv->open(dbEnv, path2DbEnv.c_str(), envFlags, 0);
	}
	if (dberr) {
		std::cout << "Unable to create environment handle due to the following error: " <<
			db_strerror(dberr) << std::endl;
		if (dbEnv) dbEnv->close(dbEnv, 0);
		return -1;
	}

	//Have the XmlManager adopt the db environment
	XmlManager mgr(dbEnv, DBXML_ADOPT_DBENV);

	//Configure the container to use transactions
	XmlContainerConfig config;
	config.setTransactional(true);

	//Open a container in the db environment
	XmlContainer container = mgr.openContainer(theContainer, config);

	//create a transaction
	XmlTransaction txn = mgr.createTransaction();

	//create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//Query for documents by their document names.
	doContextQuery( txn, mgr, container.getName(),
		"/*[dbxml:metadata('dbxml:name')='ZuluNut.xml']", context );
	doContextQuery( txn, mgr, container.getName(),
		"/*[dbxml:metadata('dbxml:name')='TrifleOrange.xml']", context );
	doContextQuery( txn, mgr, container.getName(),
		"/*[dbxml:metadata('dbxml:name')='TriCountyProduce.xml']", context );

	//Get the document from the container using the document name
	doGetDocument(txn, container, "ZuluNut.xml");
	doGetDocument(txn, container, "TrifleOrange.xml");
	doGetDocument(txn, container, "TriCountyProduce.xml");

	//commit the transaction
	txn.commit();

	return 0;
}
