/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* retrieveDB
*
* This program retrieves data from a Berkeley DB data based on information
* retrieved from an XML document. The document is first retrieved from an 
* XML container. Both the database and the container are stored in the same
* database environment, and all the necessary queries are performed under the
* protection of a transaction.
* 
* Use the 'retrieveDB' sample program to retrieve the stored data.
* 
* Before running this program, run loadExamplesData.[sh|cmd] to pre-populate
* the DBXML container with the appropriate data. Then, run buildDB to populate
* the database with the appropriate data.
* 
* When you run this program, identify the directory where you told loadExamplesData
* to place the sample data.
* 
* Usage: retrieveDB -h <dbenv directory> 
*
* This program demonstrates:
*  How to retrieve non-XML data
*  How to open a Berkeley DB transactional database
*  How to retrieve data from a Berkeley DB XML container
*  Hot to use that data as a key to get data from a Berkeley DB database
*
*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
	std::cerr << "This program retrieves data from a Berkeley DB data based on information\n"
		<< "retrieved from an XML document. The document is first retrieved from an \n"
		<< "XML container. Both the database and the container are stored in the same\n"
		<< "database environment, and all the necessary queries are performed under the\n"
		<< "protection of a transaction.\n\n"

		<< "Use the 'retrieveDB' sample program to retrieve the stored data.\n\n"

		<< "Before running this program, run loadExamplesData.[sh|cmd] to pre-populate\n"
		<< "the DBXML container with the appropriate data. Then, run buildDB to populate\n"
		<< "the database with the appropriate data.\n\n"

		<< "When you run this program, identify the directory where you told loadExamplesData\n"
		<< "to place the sample data:\n\n"

		<< "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

// A standard Berkeley DB error handler to get extra information
// on database errors
static void errcall(const DB_ENV *dbenv, const char *errPfx, const char *errMsg)
{
	std::cerr << "Error from Berkeley DB: " << errMsg << std::endl;
}

int main(int argc, char **argv)
{
	std::string path2DbEnv;
	std::string theDB = "testBerkeleyDB";
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

	if ( ! path2DbEnv.length() )
	{
		usage();
	}

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

	//Open a database in the db environment
	DB *openedDatabase;
	DB_TXN *dbTxn;
	dberr = db_create(&openedDatabase, dbEnv, 0);
	if (dberr == 0) {
		dbEnv->txn_begin(dbEnv, 0, &dbTxn, 0);
		openedDatabase->set_errcall(openedDatabase, errcall);
		dberr = openedDatabase->open(openedDatabase, dbTxn, theDB.c_str(), 0, DB_BTREE, DB_CREATE, 0);
		dbTxn->commit(dbTxn, 0);
	} 
	if (dberr) {
		std::cout << "Unable to create database handle due to the following error: " <<
			db_strerror(dberr) << std::endl;
		if (openedDatabase) openedDatabase->close(openedDatabase, 0);
		return -1;
	} 

	//Configure the container to use transactions
	XmlContainerConfig config;
	config.setTransactional(true);

	//Open a container in the db environment
	XmlContainer container = mgr.openContainer(theContainer, config);

	//Create an XmlTransaction based on the db transaction
	if (dbEnv->txn_begin(dbEnv, 0, &dbTxn, 0) != 0) {
		std::cerr << "Error creating a DB transaction"<< std::endl;
		exit( -1 );
	}
	XmlTransaction txn = mgr.createTransaction(dbTxn);

	try {
		XmlQueryContext resultsContext = mgr.createQueryContext();

		//Find all the vendor documents.
		std::string fullQuery = "collection('" + container.getName() + "')/vendor";
		XmlResults results( mgr.query(txn, fullQuery, resultsContext) );

		//Loop through the result set, retrieving the data we are interested in for each document.
		XmlValue value;
		while( results.next(value) ) {
			//Query the retrieved document for the salesrep's name
			XmlQueryExpression doc_expr = mgr.prepare("fn:string(salesrep/name)", resultsContext);
			XmlResults docResult = doc_expr.execute(value, resultsContext);

			//The result set from the document query should be of size 1. If not, the
			//document schema has changed and the query must be modified..
			assert( docResult.size() == 1 );

			//Pull the value out of the document query result set.
			XmlValue docValue;
			docResult.next(docValue);
			std::string theSalesRepKey = docValue.asString();

			//Reconstruct the key used to store the information relevant to this document
			DBT theKey;
			memset(&theKey, 0, sizeof(DBT));
			theKey.data = (void *)theSalesRepKey.c_str();
			theKey.size = (u_int32_t)theSalesRepKey.length() + 1;

			//This next structure is used to hold the data that is
			// retrieved from the db.
			DBT theData;
			memset(&theData, 0, sizeof(DBT));

			//Get the data and report the results
			dberr = openedDatabase->get(openedDatabase, dbTxn, &theKey, &theData, 0);
			if (dberr) {
				std::cout << "Unable to get data from the database due to the following error: " <<
					db_strerror(dberr) << std::endl;
				dbTxn->abort(dbTxn);
				openedDatabase->close(openedDatabase, 0);
				return -1;
			}
			std::cout << "For key: '" << (char *)theKey.data << "', retrieved:" << std::endl;
			std::cout << "\t" << (char *)theData.data << std::endl;
		}

		//commit the transaction and close the database
		dbTxn->commit(dbTxn, 0);
		openedDatabase->close(openedDatabase, 0);
	}
	//Catches XmlException
	catch(std::exception &e)
	{
		std::cerr << "Error writing to database: "<< std::endl;
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting transaction and exiting."<< std::endl;
		dbTxn->abort(dbTxn);
		openedDatabase->close(openedDatabase, 0);

		exit( -1 );
	}

	return 0;
}
