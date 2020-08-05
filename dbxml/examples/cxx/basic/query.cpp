/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 *
 *******
 *
 * query
 *
 * A very simple Berkeley DB XML program
 * that does a query and handles results.
 *
 * This program demonstrates:
 *  Basic initialization
 *  Container creation
 *  Document insertion
 *  Query creation and execution
 *  Use of a variable in a query and context
 *  Results handling
 *
 *  Usage: query
 *
 */

#include <iostream>
#include <dbxml/DbXml.hpp>

using namespace DbXml;

int
main(int argc, char **argv)
{
	// This program uses a named container, which will apear
	// on disk
	std::string containerName = "people.dbxml";
	std::string content = "<people><person><name>joe</name></person><person><name>mary</name></person></people>";
	std::string docName = "people";
	// Note that the query uses a variable, which must be set
	// in the query context
	std::string queryString =
		"collection('people.dbxml')/people/person[name=$name]";

	try {
		// All BDB XML programs require an XmlManager instance
		XmlManager mgr;

		// Because the container will exist on disk, remove it
		// first if it exists
		if (mgr.existsContainer(containerName))
			mgr.removeContainer(containerName);

		// Now it is safe to create the container
		XmlContainer cont = mgr.createContainer(containerName);

		// All Container modification operations need XmlUpdateContext
		XmlUpdateContext uc = mgr.createUpdateContext();
		cont.putDocument(docName, content, uc);

		// Querying requires an XmlQueryContext
		XmlQueryContext qc = mgr.createQueryContext();

		// Add a variable to the query context, used by the query
		qc.setVariableValue("name", "mary");

		// Note: these two calls could be replaced by a shortcut
		// mgr.query(queryString, qc), but under the covers,
		// this is what is happening.
		XmlQueryExpression expr = mgr.prepare(queryString, qc);
		XmlResults res = expr.execute(qc);

		// Note use of XmlQueryExpression::getQuery() and
		// XmlResults::size()
		std::cout << "The query, '" << expr.getQuery() << "' returned " <<
			(unsigned int) res.size() << " result(s)" << std::endl;

		// Process results -- just print them
		XmlValue value;
		std::cout << "Result: " << std::endl;
		while (res.next(value)) {
			std::cout << "\t" << value.asString() << std::endl;
		}
		
		// In C++, resources are released as objects go out
		// of scope.

		
	} catch (XmlException &xe) {
		std::cout << "XmlException: " << xe.what() << std::endl;
	}
	return 0;
}

