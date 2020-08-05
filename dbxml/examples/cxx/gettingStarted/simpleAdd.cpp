/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* simpleAdd
*
* This program adds a few simple XML documents to container 'simpleExampleData.dbxml'
* You are only required to pass this program the path to where you want the
* database environment to be located.
* 
* Usage: simpleAdd -h <dbenv directory> 
*
* This program demonstrates:
*  How to insert XML data
*  How to open a Berkeley DB XML transactional container
*  How add XML data to an XmlDocument
*  How to add the XmlDocument to a container
*  How use automatically generated names when inserting documents into a container
*
*/

#include <cstdlib>
#include <stdlib.h>
#include <iostream>

#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
	std::cerr << "This program adds a few simple XML documents to container 'simpleExampleData.dbxml'\n"
		<< "You are only required to pass this program the path to where you want the\n"
		<< "database environment to be located: \n\n"
		<< "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
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

	// Get an XmlUpdateContext. Useful from a performance perspective.
	XmlUpdateContext updateContext = db.createUpdateContext();

	//Get a transaction
	XmlTransaction txn = db.createTransaction();

	std::string document1 = "<aDoc><title>doc1</title><color>green</color></aDoc>";
	std::string document2 = "<aDoc><title>doc2</title><color>yellow</color></aDoc>";

	//Add the documents
	XmlDocument myXMLDoc = db.createDocument();

	/* Set the XmlDocument to the relevant string and then put it into the container.
	* Using the flag DBXML_GEN_NAME means that a generated name will be assigned
	* to the document if it does not have one.  An exception will be thrown if
	* a document is inserted without a name or the DBXML_GEN_NAME flag. 
	*/
	myXMLDoc.setContent( document1 );
	container.putDocument(txn, myXMLDoc, updateContext, DBXML_GEN_NAME);

	//do it again for the second document
	myXMLDoc.setContent( document2 );
	container.putDocument(txn, myXMLDoc, updateContext, DBXML_GEN_NAME);

	//Normally we would use a try/catch block to trap any exceptions.
	// In the catch, we should call txn->abort() to avoid leaving the
	// database in an indeterminate state in the event of an error.
	// However, this simple example avoids error handling so as to
	// highlite basic concepts, so that step if omitted here as well.

	//Commit the writes. This causes the container write operations
	//  to be saved to the container.
	txn.commit();

	return 0;
}
