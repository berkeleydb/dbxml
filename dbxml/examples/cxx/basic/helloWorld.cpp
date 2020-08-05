/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 *
 *******
 *
 * helloWorld
 *
 * The simplest possible Berkeley DB XML program
 * that does something.
 *
 *  Usage: helloWorld
 *
 * This program demonstrates:
 *  Basic initialization
 *  Container creation
 *  Document insertion
 *  Document retrieval by name
 *
 */

#include <iostream>
#include <dbxml/DbXml.hpp>

using namespace DbXml;

int
main(int argc, char **argv)
{
	// An empty string means an in-memory container, which
	// will not be persisted
	std::string containerName = "";
	std::string content = "<hello>Hello World</hello>";
	std::string docName = "doc";
	
	try {
		// All BDB XML programs require an XmlManager instance
		XmlManager mgr;
		{
		XmlContainer cont = mgr.createContainer(containerName);

		// All Container modification operations need XmlUpdateContext
		XmlUpdateContext uc = mgr.createUpdateContext();
		cont.putDocument(docName, content, uc);

		// Now, get the document
		XmlDocument doc = cont.getDocument(docName);
		std::string docContent;
		std::string name = doc.getName();
		docContent = doc.getContent(docContent);

		// print it
		std::cout << "Document name: " << name << "\nContent: " <<
			docContent << std::endl;

		// In C++, resources are released as objects go out
		// of scope.
		}

		
	} catch (XmlException &xe) {
		std::cout << "XmlException: " << xe.what() << std::endl;
	}
	return 0;
}

