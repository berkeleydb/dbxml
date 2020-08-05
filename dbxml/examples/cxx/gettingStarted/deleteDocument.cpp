/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* deleteDocument
*
* This program deletes a Berkeley DB XML document from a container. You should pre-load the
* container using loadExamplesData.[sh|cmd] before running this example. You are
* only required to pass this program the path location of the database
* environment that you specified when you pre-loaded the examples data.
* 
* Usage: deleteDocument -h <dbenv directory>
*
* This program demonstrates:
*  How to delete XML data
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information when quering a container
*  How to get the XmlDocument from query results
*  How to delete an XmlDocument from a container
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
	std::cerr  << "This program deletes a DBXML document from a container. You should pre-load the\n"
		<< "container using loadExamplesData.[sh|cmd] before running this example. You are\n"
		<< "only required to pass this this program the path location of the database\n"
		<< "environment that you specified when you pre-loaded the examples data:\n\n"

		<< "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

//Forward declare
std::string getValue( XmlManager &mgr, const XmlDocument &document, const std::string &query, XmlQueryContext &context );
void confirmDelete ( XmlManager &mgr, const std::string &query, XmlQueryContext &context);

void doDeleteDocument( XmlManager &mgr, XmlContainer &container, const std::string &query, XmlQueryContext &context, XmlTransaction& txn, XmlUpdateContext &uc)
{
	////////////////////////////////////////////////////////////////////////
	//////  Deletes a document from a DBXML container.               ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //

	try {
		std::cout << "Deleting documents for expression: '" << query
			<< "' " << std::endl;

		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query(txn, query, context ) );

		std::cout << "Found " << (unsigned int)results.size()
			<< " documents matching the expression '"
			<< query << ". " << std::endl;

		XmlValue value;
		std::cout << "\n";
		while( results.next( value ) ) {
			/// Retrieve the value as a document
			XmlDocument theDocument = value.asDocument();

			/// Obtain information of interest from the document. Note that the
			//  wildcard in the query expression allows us to not worry about what
			//  namespace this document uses.
			std::string item = getValue( mgr, theDocument, "fn:string(/*/product)", context);
			std::cout << "Deleting document: " << item << std::endl;

			container.deleteDocument( txn, theDocument, uc );

			std::cout << "Deleted document: " << item << std::endl;

		}
	}
	//Catches XmlException
	catch(std::exception &e)
	{
		std::cerr << "Document deletion failed. \n";
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

	//Get a transaction
	XmlTransaction txn = mgr.createTransaction();

	//create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//delete the document that describes Mabolo (a fruit)
	const std::string query = "/fruits:item[product = 'Mabolo']";
	XmlUpdateContext uc = mgr.createUpdateContext();
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	doDeleteDocument( mgr, container, fullQuery, context, txn, uc);

	//Commit the delete
	txn.commit();

	//The document should now no longer exist in the container. Just for fun,
	//  confirm the delete.
	confirmDelete ( mgr, fullQuery, context);

	return 0;
}

std::string getValue( XmlManager &mgr, const XmlDocument &document,
					 const std::string &query, XmlQueryContext &context )
{
	/////////////////////////////////////////////////////////////////////////////////
	///////////    Return specific information from a document. /////////////////////
	///////////   !!!!!! Assumes a result set of size 1 !!!!!!! /////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	// Exception handling omitted....

	//Perform the query
	XmlQueryExpression doc_expr = mgr.prepare(query, context);
	XmlResults result = doc_expr.execute(XmlValue(document), context);

	//We require a result set size of 1.
	assert( result.size() == 1 );

	//Get the value. If we allowed the result set to be larger than size 1,
	//we would have to loop through the results, processing each as is
	//required by our application.
	XmlValue value;
	result.next(value);

	return value.asString();

}

void confirmDelete ( XmlManager &mgr, const std::string &query, XmlQueryContext &context)
{
	// Exception handling omitted....

	std::cout << "Confirming the delete." << std::endl;
	std::cout << "The query: '" << query << "' should get result set size 0." << std::endl;
	XmlResults resultsConfirm( mgr.query(query, context) );
	if ( ! resultsConfirm.size() ) {
		std::cout << "No documents found matching query query: '"
			<< query << "'." << std::endl;
		std::cout << "Deletion confirmed." << std::endl;
	} else {
		std::cout << "Found documents matching '" << query << "'! "
			<< (unsigned int)resultsConfirm.size() << " documents found." << std::endl;
		std::cout << "Document deletion failed." << std::endl;
	}
	std::cout << "\n";

}
