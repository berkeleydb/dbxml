/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* queryWithContext
*
* This program illustrates how to query for documents that require namespace
* usage in the query, and how to use variables in the query.  You should pre-load the 
* container using loadExamplesData.[sh|cmd] before running this example.  You are only required
* to pass this command the path location of the database environment that you
* specified when you pre-loaded the examples data.
* 
* Usage: queryWithContext -h <dbenv directory> 
*
* This program demonstrates:
*  How to use namespace information and variables in a query
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information in a query
*  How to set a variable value in a query
*  How to query the container
*  How iterate through query results
*  How to view query results
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
	std::cerr  <<   "This program illustrates how to query for documents that require namespace\n"
		<<   "usage in the query.  You should pre-load the container using\n"
		<<   "loadExamplesData.[sh|cmd] before running this example.  You are only required\n"
		<<   "to pass this command the path location of the database environment that you\n"
		<<   "specified when you pre-loaded the examples data:\n\n"

		<<   "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

void doContextQuery( XmlTransaction &txn, XmlManager &mgr,
					const std::string &cname, const std::string &query,
					XmlQueryContext &context )
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

		XmlResults results( mgr.query(txn, fullQuery, context ) );
		XmlValue value;
		while( results.next(value) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << value.asString() << std::endl;
		}

		std::cout << (unsigned int) results.size()
			<< " objects returned for expression '"
			<< fullQuery << "'\n" << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e) {
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

	//Create a transaction
	XmlTransaction txn = mgr.createTransaction();

	//Create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//Set a variable
	context.setVariableValue( (std::string)"aDessert", (std::string)"Blueberry Boy Bait");

	//Perform the queries

	//Find all the Vendor documents in the database. Vendor documents do
	// not use namespaces, so this query returns documents.
	doContextQuery( txn, mgr, container.getName(), "/vendor", context );

	//Find the product document for "Lemon Grass". This query returns no documents
	// because a namespace prefix is not identified for the 'item' node.
	doContextQuery( txn, mgr, container.getName(),
		"/item/product[.=\"Lemon Grass\"]", context);

	//Find the product document for "Lemon Grass" using the namespace prefix 'fruits'. This
	// query successfully returns a document.
	doContextQuery( txn, mgr, container.getName(),
		"/fruits:item/product[.=\"Lemon Grass\"]", context);

	//Find all the vegetables
	doContextQuery( txn, mgr, container.getName(), "/vegetables:item", context);

	//Find the  dessert called Blueberry Boy Bait
	// Note the use of a variable
	doContextQuery( txn, mgr, container.getName(),
		"/desserts:item/product[.=$aDessert]", context);

	//commit transaction
	txn.commit();

	return 0;
}
