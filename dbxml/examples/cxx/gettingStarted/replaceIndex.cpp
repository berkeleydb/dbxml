/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* replaceIndex
*
* This program replaces the index for a document node.  You should pre-load the
* container using loadExamplesData.[sh|cmd]  before running this example. You
* are only required to pass this command the path on of the database environment
* that you specified when you pre-loaded the examples data.
*
* For best results run addIndex before running this program.
* 
* Usage: replaceIndex -h <dbenv directory> 
*
* This program demonstrates:
*  How to replace an index
*  How to open a Berkeley DB XML transactional container
*  How to view the existing indexes in a container
*  How to repleace an index
*  How to update multiple indexes for a node at once
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
	std::cerr <<    "This program replaces the index for a document node.  You should pre-load the\n"
		<<    "container using loadExamplesData.[sh|cmd]  before running this example. You\n"
		<<    "are only required to pass this command the path on of the database environment\n"
		<<    "that you specified when you pre-loaded the examples data:\n\n"

		<<     "\t-h <dbenv directory>\n\n"

		<<    "Also, for best results run addIndex before running this program.\n" << std::endl;
	exit( -1 );
}

void replaceIndex( XmlContainer &container, const std::string &URI,
				  const std::string &nodeName, const std::string &indexType,
				  XmlTransaction &txn, XmlUpdateContext &uc)
{
	std::cout << "Replacing index on node: " << nodeName << std::endl;
	try
	{
		//Retrieve the XmlIndexSpecification from the container
		XmlIndexSpecification idxSpec = container.getIndexSpecification( txn );

		//Lets see what indexes exist on this container
		std::string uri, name, index;
		int count = 0;
		std::cout << "Before index add." << std::endl;
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
			count ++;
		}

		std::cout << count << " indexes found." << std::endl;

		//Replace the indexes for the specified node
		idxSpec.replaceIndex( URI, nodeName, indexType );

		//Set the specification back to the container
		container.setIndexSpecification( txn, idxSpec, uc );

		//Look at the indexes again to make sure our replacement took.
		count = 0;
		idxSpec.reset();
		std::cout << "After index add." << std::endl;
		while( idxSpec.next(uri, name, index) )
		{
			// Obtain the value as a string and print it to the console
			std::cout << "\tFor node '" << name << "', found index: '" << index << "'." << std::endl;
			count ++;
		}

		std::cout << count << " indexes found." << std::endl;

	}
	//Catches XmlException
	catch(std::exception &e)
	{
		std::cerr << "Index replace failed: \n";
		std::cerr << e.what() << "\n";
		txn.abort();

		exit( -1 );
	}
	std::cout << "Index replaced successfully." << std::endl;

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

	// replace the index on the "product" node with two indexes that are space delimited
	replaceIndex( container, "", "product",
		"node-attribute-substring-string node-element-equality-string",
		txn, uc );

	//commit transaction
	txn.commit();

	return 0;
}

