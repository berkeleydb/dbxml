/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* queryForDocumentValue
*
* This program performs various XQuery queries against a Berkeley DB XML container and then
* retrieves information of interest from the retrieved document(s).  You should
* pre-load the container using loadExamplesData.[sh|cmd] before running this
* example.  You are only required to pass this command the path location of the
* database environment that you specified when you pre-loaded the examples data.
* 
* Usage: queryForDocumentValue -h <dbenv directory> 
*
* This program demonstrates:
*  How to query the XML data
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information in a query
*  How to view the number of query results returned
*  How to iterate through query results
*  How to print out query results
*
*/

#include <fstream>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;

void usage()
{
	std::cerr   <<  "This program performs various XPath queries against a DBXML container and then\n"
		<<  "retrieves information of interest from the retrieved document(s).  You should\n"
		<<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
		<<  "example.  You are only required to pass this command the path location of the\n"
		<<  "database environment that you specified when you pre-loaded the examples data:\n\n"

		<<  "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

std::string getValue( XmlTransaction &txn, XmlManager &mgr, const XmlDocument &document,
					 const std::string &XPath, XmlQueryContext &context )
{
	/////////////////////////////////////////////////////////////////////////////////
	///////////    Return specific information from a document. /////////////////////
	///////////   !!!!!! Assumes a result set of size 1 !!!!!!! /////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	// Exception handling omitted....

	//Perform the query
	XmlQueryExpression doc_expr = mgr.prepare(txn, XPath, context);
	XmlResults result = doc_expr.execute( txn, XmlValue(document), context);

	//We require a result set size of 1.
	assert( result.size() == 1 );

	//Get the value. If we allowed the result set to be larger than size 1,
	//we would have to loop through the results, processing each as is
	//required by our application.
	XmlValue value;
	result.next(value);

	return value.asString();

}

void getDetails( XmlTransaction &txn, XmlManager &mgr, const XmlContainer &container, const std::string &query, XmlQueryContext &context )
{
	////////////////////////////////////////////////////////////////////////
	//////  Performs an query (in context) against the         ///////
	//////  provided container.                                      ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	// Perform the query. Result type is by default Result Document
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Exercising query '" << fullQuery << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query( txn, fullQuery, context ) );
		XmlValue value;
		std::cout << "\n\tProduct : Price : Inventory Level\n";
		while( results.next(value) ) {
			/// Retrieve the value as a document
			XmlDocument theDocument = value.asDocument();

			/// Obtain information of interest from the document. Note that the
			//  wildcard in the query expression allows us to not worry about what
			//  namespace this document uses.
			std::string item = getValue( txn, mgr, theDocument, "fn:string(/*/product)", context);
			std::string price = getValue( txn, mgr, theDocument, "fn:string(/*/inventory/price)", context);
			std::string inventory = getValue( txn, mgr, theDocument, "fn:string(/*/inventory/inventory)", context);

			std::cout << "\t" << item << " : " << price << " : " << inventory << std::endl;

		}
		std::cout << "\n";
		std::cout << (unsigned int) results.size()
			<< " objects returned for expression '"
			<< fullQuery << "'\n" << std::endl;
	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Query " << fullQuery << " failed\n";
		std::cerr << e.what() << "\n";
		txn.abort();
		exit(-1);
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

	//Create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//create a transaction
	XmlTransaction txn = mgr.createTransaction();

	//get details on Zulu Nuts
	getDetails( txn, mgr, container, "/fruits:item[fn:string(product) = 'Zulu Nut']", context);

	//get details on all fruits that start with 'A'
	getDetails( txn, mgr, container, "/vegetables:item[starts-with(fn:string(product),'A')]", context);

	//commit transaction
	txn.commit();

	return 0;
}
