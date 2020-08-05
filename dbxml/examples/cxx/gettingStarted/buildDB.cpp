/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* buildDB
*
* This program retrieves node information from a specific set of XML 
* documents stored in a Berkeley DB XML container, and then saves relevant sample
* data to a Berkeley DB database. Both the Berkeley DB XML container and the Berkeley
* DB database are stored in the same database environment. The value of
* the text node found on the XML document is used as the Berkeley DB key for the
* sample data stored in the database.
* 
* Use the 'retrieveDB' sample program to retrieve the stored data.
* 
* Before running this program, run loadExamplesData.[sh|cmd] to pre-populate
* the Berkeley DB XML container with the appropriate data.
* 
* When you run this program, identify the directory where you told loadExamplesData
* to place the sample data.
* 
* Usage: buildDB -h <dbenv directory>
*
* This program demonstrates:
*  How to store non-XML data
*  How to open a Berkeley DB transactional database
*  How to retrieve data from a Berkeley DB XML container
*  How to format that data for insertion into a Berkeley DB database
*  How to insert data into a Berkeley DB database
*
*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <db.h>
#include <dbxml/DbXml.hpp>

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
	std::cerr << "This program retrieves node information from a specific set of XML \n"
		"documents stored in a DBXML container, and then saves relevant sample\n"
		"data to a Berkeley DB database. Both the DBXML container and the Berkeley\n"
		"DB database are stored in the same database environment. The value of\n"
		"the text node found on the XML document is used as the Berkeley DB key for the\n"
		"sample data stored in the database.\n\n"

		<< "Use the 'retrieveDB' sample program to retrieve the stored data.\n\n"

		<< "Before running this program, run loadExamplesData.[sh|cmd] to pre-populate\n"
		<< "the DBXML container with the appropriate data.\n\n"

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
		dbEnv->set_errcall(dbEnv, errcall); // set error callback
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
		openedDatabase->close(openedDatabase, 0);
		exit( -1 );
	}
	XmlTransaction txn = mgr.createTransaction(dbTxn);

	try {
		XmlQueryContext resultsContext = mgr.createQueryContext();

		//Find all the vendor documents.
		std::string query = "collection('" + container.getName() + "')/vendor";
		XmlResults results = mgr.query(txn, query, resultsContext);

		//Loop through the result set, setting data in the db for each document
		XmlValue value;
		while( results.next(value) ) {
			//Query on the document for the salesrep's name
			XmlQueryExpression doc_expr = mgr.prepare("fn:string(/vendor/salesrep/name)", resultsContext);
			XmlResults docResult = doc_expr.execute(value, resultsContext);

			//The result set from the document query should be of size 1. If not, the
			//document schema has changed and the query must be modified..
			assert( docResult.size() == 1 );

			//Pull the value out of the document query result set.
			XmlValue docValue;
			docResult.next(docValue);
			std::string theSalesRepKey = docValue.asString();
			std::cout << "theSalesRepKey : " << theSalesRepKey << std::endl;

			//This is the data that we're putting into the database. A real world example would probably
			//include a BLOB, such as a jpeg image of the salereps, a public key, or some other bit of
			//information that doesn't fit into the XML document schema. In our case, we'll just put in
			//a descriptive string so we can see what is going on when we retrieve this data.
			std::string theSalesRepData = "This is the data stored in the database for " + docValue.asString() + ".";
			DBT theKey, theData;
			memset(&theKey, 0, sizeof(DBT));
			memset(&theData, 0, sizeof(DBT));
			theKey.data = (void *)theSalesRepKey.c_str();
			theKey.size = (u_int32_t)theSalesRepKey.length() + 1;
			theData.data = (void *)theSalesRepData.c_str();
			theData.size = (u_int32_t)theSalesRepData.length() + 1;
			dberr = openedDatabase->put(openedDatabase, dbTxn, &theKey, &theData, DB_NOOVERWRITE);
			if (dberr) {
				std::cout << "Unable to put data into the database due to the following error: " <<
					db_strerror(dberr) << std::endl;
				dbTxn->abort(dbTxn);
				openedDatabase->close(openedDatabase, 0);
				return -1;
			}
		} //end results loop

		//commit the transaction and close the database
		dbTxn->commit(dbTxn, 0);
		openedDatabase->close(openedDatabase, 0);
	}
	//Catch standard exceptions and XmlException
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
