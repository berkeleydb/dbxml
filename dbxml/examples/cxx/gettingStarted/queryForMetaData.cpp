/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* queryForMetaData
*
* This program retrieves Berkeley DB XML documents and then retrieves the day and time the
* document was stored in the container. The date/time information was set as
* metadata information when the document was loaded. See the
* exampleLoadContainer.cpp example for how this metadata is set.
* 
* Before running this example, use loadExamplesData.[sh|cmd] to pre-load the
* appropriate container with documents and metadata. 
* 
* You are only required to pass this command the path location of the database
* environment that you specified when you pre-loaded the examples data.
* 
* Usage: queryForMetaData -h <dbenv directory> 
*
* This program demonstrates:
*  How to view meta data
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information in a query
*  How to query the container
*  How to get an XmlDocument from the query results
*  How to access the metadata from the XmlDocument
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
	std::cerr   << "This program retrieves DBXML documents and then retrieves the day and time the\n"
		<< "document was stored in the container. The date/time information was set as\n"
		<< "metadata information when the document was loaded. See the\n"
		<< "exampleLoadContainer.cpp example for how this metadata is set.\n\n"

		<< "Before running this example, use loadExamplesData.[sh|cmd] to pre-load the\n"
		<< "appropriate container with documents and metadata. \n\n"

		<< "You are only required to pass this command the path location of the database\n"
		<< "environment that you specified when you pre-loaded the examples data:\n\n"

		<< "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

void showTimeStamp( XmlTransaction &txn, XmlManager &mgr,
				   const XmlContainer &container,
				   const std::string &query, XmlQueryContext &context )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs a simple query (with context) against the ///////
	//////  provided container. The timestamp metadata attribute     ///////
	//////  value is then displayed.                                 ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	//Used for metadata query
	const std::string mdURI = "http://dbxmlExamples/timestamp";
	const std::string mdPrefix = "time";
	const std::string mdName = "timeStamp";

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query( txn, fullQuery, context ) );
		XmlValue value;
		while( results.next(value) )
		{
			XmlDocument theDocument = value.asDocument();

			//Get this document's name
			const std::string docName( theDocument.getName() );

			//Get the timestamp on the document (stored as metadata)
			// and print it to the console.
			XmlValue metaValue;
			theDocument.getMetaData( mdURI, mdName, metaValue );
			std::cout << "Document '" << docName << "' stored on " << metaValue.asString() << std::endl;
		}

		std::cout << (unsigned int)results.size()
			<< " objects returned for expression '" << fullQuery
			<< "'\n" << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e)
	{
		std::cerr << "Query " << fullQuery << " failed\n";
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

	//create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//create a transaction
	XmlTransaction txn = mgr.createTransaction();

	//Find all the vegetables
	showTimeStamp( txn, mgr, container, "/vegetables:item", context);

	//commit transaction
	txn.commit();

	return 0;
}
