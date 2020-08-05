/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* deleteIndex
*
* This program deletes an index from a document node in a Berkeley DB XML container.  You
* should pre-load the container using loadExamplesData.[sh|cmd] before running
* this example. You are only required to pass this command the path location of
* the database environment that you specified when you pre-loaded the examples
* data.
*
* For best results run addIndex before running this program.
* 
* Usage: deleteIndex -h <dbenv directory>
*
* This program demonstrates:
*  How to delete an index
*  How to open a Berkeley DB XML transactional container
*  How to view the existing indexes in a container
*  How to delete an index from a container
*
*/

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

#include "dbxml/DbXml.hpp"
#include <db.h>

using namespace DbXml;

void usage()
{
	std::cerr   << "This program deletes an index from a document node in a DBXML container.  You\n"
		<< "should pre-load the container using loadExamplesData.[sh|cmd] before running\n"
		<< "this example. You are only required to pass this command the path location of\n"
		<< "the database environment that you specified when you pre-loaded the examples\n"
		<< "data:\n\n"

		<<  "\t-h <dbenv directory>\n"

		<<    "Also, for best results run addIndex before running this program.\n" << std::endl;
	exit( -1 );
}

void deleteIndex( XmlContainer &container, const std::string &URI,
				 const std::string &nodeName, const std::string &indexType,
				 XmlTransaction &txn, XmlUpdateContext &uc )
{
	std::cout << "Deleting index type: '" << indexType << ""
		<< " from node: '" << nodeName << "'." << std::endl;
	try
	{
		//Retrieve the XmlIndexSpecification from the container
		XmlIndexSpecification idxSpec = container.getIndexSpecification( txn );

		std::cout << "Before the delete, the following indexes are maintained for the container:" << std::endl;
		std::string uri, name, index;
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
		}
		std::cout << "\n" << std::endl;

		//Delete the indexes from the specification.
		idxSpec.deleteIndex( URI, nodeName, indexType );

		//Set the specification back to the container
		container.setIndexSpecification( txn, idxSpec, uc );

		//Show the remaining indexes in the container, if any.
		std::cout << "After the delete, the following indexes exist for the container:" << std::endl;
		idxSpec.reset();
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
		}
		std::cout << "\n" << std::endl;
	}
	//Catches XmlException.
	catch(std::exception &e)
	{
		std::cerr << "Index delete failed: \n";
		std::cerr << e.what() << "\n";
		txn.abort();

		exit( -1 );
	}
	std::cout << "Index deleted successfully.\n" << std::endl;

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
	XmlManager db(dbEnv, DBXML_ADOPT_DBENV);

	//Configure the container to use transactions
	XmlContainerConfig config;
	config.setTransactional(true);

	//Open a container in the db environment
	XmlContainer container = db.openContainer(theContainer, config);

	//Get a transaction
	XmlTransaction txn = db.createTransaction();
	XmlUpdateContext uc = db.createUpdateContext();

	//add an string equality index for the "product" element node.
	deleteIndex( container, "", "product", "node-element-equality-string", txn, uc );

	//Do these deletes in two different transactions
	// for no particular reason.
	txn.commit();
	txn = db.createTransaction();

	//add an edge presence index for the product node
	deleteIndex( container, "", "product", "edge-element-presence-none", txn, uc );

	txn.commit();

	return 0;
}

