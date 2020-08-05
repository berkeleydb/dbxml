/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* simpleQuery
*
* This program performs various XQuery queries against a Berkeley DB XML container. You should
* pre-load the container using loadExamplesData.[sh|cmd] before running this example. You
* are only required to pass this command the path location of the database environment
* that you specified when you pre-loaded the examples data.
* 
* Usage: simpleQuery -h <dbenv directory> 
*
* This program demonstrates:
*  How to query XML data
*  How to open a Berkeley DB XML transactional container
*  How to query a container
*  How to iterate through query results
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
	std::cerr <<   "This program performs various XPath queries against a DBXML container. You should\n"
		<< "pre-load the container using loadExamplesData.[sh|cmd] before running this example. You\n"
		<< "are only required to pass this command the path location of the database environment\n"
		<< "that you specified when you pre-loaded the examples data:\n\n"

		<< "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

void doQuery( XmlTransaction &txn, XmlManager &db, const XmlContainer &container, const std::string &query )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs a simple query (no context) against the   ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlQueryContext context = db.createQueryContext();

		XmlResults results( db.query( txn, fullQuery, context) );
		XmlValue value;
		while( results.next(value) ) {
			// Obtain the value as a string and print it to stdout
			std::cout << value.asString() << std::endl;
		}

		std::cout << (unsigned int) results.size()
			<< " objects returned for expression '"
			<< fullQuery << "'\n" << std::endl;

	}
	catch(XmlException &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		txn.abort();
		exit( -1 );
	}
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
	std::string theContainer = "simpleExampleData.dbxml";
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
	XmlManager db(dbEnv, DBXML_ADOPT_DBENV);

	//Configure the container to use transactions
	XmlContainerConfig config;
	config.setTransactional(true);

	//Open a container in the db environment
	XmlContainer container = db.openContainer(theContainer, config);

	//Create a transaction
	XmlTransaction txn = db.createTransaction();

	//perform the queries

	//find all the Vendor documents in the database
	doQuery( txn, db, container, "/vendor" );

	//find all the vendors that are wholesale shops
	doQuery( txn, db, container, "/vendor[@type=\"wholesale\"]");

	//find the product document for "Lemon Grass"
	doQuery( txn, db, container, "/product/item[.=\"Lemon Grass\"]");

	//find all the products where the price is less than or equal to 0.11
	doQuery( txn, db, container, "/product/inventory[price<=0.11]");

	//find all the vegetables where the price is less than or equal to 0.11
	doQuery( txn, db, container, "/product[inventory/price<=0.11 and category=\"vegetables\"]");

	//commit the transaction
	txn.commit();

	return 0;
}
