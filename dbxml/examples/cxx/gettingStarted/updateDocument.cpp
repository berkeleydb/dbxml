/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* updateDocument
*
* This program updates a document found in a Berkeley DB XML container. You should
* pre-load the container using loadExamplesData.[sh|cmd] before running this
* example. You are only required to pass this command the path location of the
* database environment that you specified when you pre-loaded the examples
* data.
* 
* Usage: updateDocument -h <dbenv directory> 
*
* This program demonstrates:
*  How to retrieve an xml document by its metadata name
*  How to open a Berkeley DB XML transactional container
*  How to use namespace information in a query
*  How to query the XML data
*  How to get the XmlDocument from the query results
*  How to change the content of an XmlDocument
*  How to update the document content in the container
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
	std::cerr  <<  "This program updates a document found in a DBXML container. You should\n"
		<<  "pre-load the container using loadExamplesData.[sh|cmd] before running this\n"
		<<  "example. You are only required to pass this command the path location of the\n"
		<<  "database environment that you specified when you pre-loaded the examples\n"
		<<  "data:\n\n"

		<<   "\t-h <dbenv directory>" << std::endl;
	exit( -1 );
}

//forward declaration
std::string getValue( XmlTransaction &txn, XmlManager &mgr, const XmlDocument &document,
					 const std::string &query, XmlQueryContext &context );

void getNewDocument( XmlTransaction &txn, XmlManager &mgr, XmlDocument &theDocument,
					XmlQueryContext &context, std::string &docString);

void doUpdateDocument( XmlContainer &container, const std::string &query, XmlQueryContext &context,
					  XmlTransaction &txn, XmlManager &mgr )
{
	////////////////////////////////////////////////////////////////////////
	//////  Updates a document stored in a DBXML container.          ///////
	////////////////////////////////////////////////////////////////////////

	///// some defensive code eliminated for clarity //
	std::string fullQuery = "collection('" + container.getName() + "')" + query;
	try {
		std::cout << "Updating document for expression: '" << query << "' " << std::endl;
		std::cout << "Return to continue: ";
		getc(stdin);

		XmlResults results( mgr.query(txn, fullQuery, context ) );
		std::cout << "Found " << (unsigned int) results.size()
			<< " documents matching the expression '" << fullQuery << ". "
			<< std::endl;
		std::cout << "\n";

		//create an update context. This is optional but since we could be performing more than
		// one update, this could offer us a potential performance benefit.
		XmlUpdateContext updateContext = mgr.createUpdateContext();

		XmlDocument theDocument = mgr.createDocument();
		while( results.next(theDocument) ) {
			std::string docString;
			//Retrieve the document's value as a string.
			theDocument.getContent(docString);

			std::cout << "Updating document: \n" << docString << std::endl;
			std::cout << "Return to continue: ";
			getc(stdin);

			//Update the document. We do it as a string.

			//This next function just modifies the document string
			//in a small way.
			getNewDocument( txn, mgr, theDocument, context, docString);

			std::cout << "Updating document..." <<  std::endl;

			//Set the document's content to be the new document string
			theDocument.setContent( docString );

			//Now replace the document in the container
			container.updateDocument(txn, theDocument, updateContext);
			std::cout << "Document updated." <<  std::endl;
		}
	}
	//Catches XmlException
	catch(std::exception &e) {
		std::cerr << "Document deletion failed. ";
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

	//Get a transaction
	XmlTransaction txn = mgr.createTransaction();

	//create a context and declare the namespaces
	XmlQueryContext context = mgr.createQueryContext();
	context.setNamespace( "fruits", "http://groceryItem.dbxml/fruits");
	context.setNamespace( "vegetables", "http://groceryItem.dbxml/vegetables");
	context.setNamespace( "desserts", "http://groceryItem.dbxml/desserts");

	//update the document that describes Zapote Blanco (a fruit)
	doUpdateDocument( container, "/fruits:item[fn:string(product) = 'Zapote Blanco']",
		context, txn, mgr );

	//commit transaction
	txn.commit();

	return 0;
}

std::string getValue( XmlTransaction &txn, XmlManager &mgr, const XmlDocument &document, const std::string &query, XmlQueryContext &context )
{
	/////////////////////////////////////////////////////////////////////////////////
	///////////    Return specific information from a document. /////////////////////
	///////////   !!!!!! Assumes a result set of size 1 !!!!!!! /////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	// Exception handling omitted....

	//Perform the query
	XmlQueryExpression doc_expr = mgr.prepare(txn, query, context);
	//Query from a context item
	XmlResults result = doc_expr.execute(txn, XmlValue(document), context);

	//We require a result set size of 1.
	assert( result.size() == 1 );

	//Get the value. If we allowed the result set to be larger than size 1,
	//we would have to loop through the results, processing each as is
	//required by our application.
	XmlValue value;
	result.next(value);

	return value.asString();

}

//Simple little function to replace part of a string with a new value.
//All this does is add an 'A' to the end of the document's inventory
//value. So each time you run this program, the inventory value should
//get longer by one 'A'.
void getNewDocument( XmlTransaction &txn, XmlManager &mgr, XmlDocument &theDocument, XmlQueryContext &context, std::string &docString)
{
	//get the substring that we want to replace
	std::string inventory = getValue( txn, mgr, theDocument, "fn:string(/*/inventory/inventory)", context );
	size_t inventoryLength = inventory.length();

	//find the substring in the original document string
	std::string::size_type pos = docString.find(inventory);
	if (pos == std::string::npos)
	{
		std::cerr << "Error: inventory string: '" << inventory << "' not found in document string:" << std::endl;
		std::cerr << docString << std::endl;
	}
	size_t insertPos = pos + inventoryLength;
	docString.insert(insertPos,"A");
}
