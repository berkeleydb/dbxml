/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * Compression API
 *
 * A very simple Berkeley DB XML program that demonstrates
 * compression API usage.
 *
 * This program demonstrates:
 *  Registration of a custom compression class in XmlManager
 *  Compression configuration for XmlContainer
 *  Turn off default compression
 *  Using default compression
 *  Custom compression class implementation
 *
 *  MyCompression is an subclass of XmlCompression. It is a very simple
 *  implementation and not intended to be particularly efficient. As an example,
 *  it uses buffer inverse permutation to simulate the compression process.
 *
 *  Compression only works with whole document containers. User compression must
 *  always be registered with the manager in order to open/create the container.
 *
 */

#include "dbxml/DbXml.hpp"
#include <iostream>

using namespace DbXml;
using namespace std;

/*
 * MyCompression implements the XmlCompression interface: required
 * member functions are compress() and decompress(). The two member functions
 * are required to return tree for success and false for error.
 * This class must be thread-safe in multi-thread environment.
 *
 */
class MyCompression : public XmlCompression
{
	bool compress(XmlTransaction &txn,
			        const XmlData &source,
			        XmlData &dest);
	bool decompress(XmlTransaction &txn,
				   const XmlData &source,
				   XmlData &dest);
};

bool MyCompression::compress(XmlTransaction &txn,
			     const XmlData &source,
			     XmlData &dest)
{
	try {
	// Get the data to compress
	char *pSrc = (char *)source.get_data();
	size_t size = source.get_size();

	// Use inverse permutation to simulate the compression process
	dest.reserve(size);
	char *buf = (char *)dest.get_data();
	for(size_t i=0; i<size; i++)
		buf[i] = pSrc[size-1-i];
	dest.set_size(size);

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
		return false;
	}
	return true;
}

bool MyCompression::decompress(XmlTransaction &txn,
			       const XmlData &source,
			       XmlData &dest)
{
	try {
	// Get the data to decompress
	char *pSrc = (char *)source.get_data();
	size_t size = source.get_size();

	// Use inverse permutation to simulate the decompression process
	dest.reserve(size);
	char *buf = (char *)dest.get_data();
	for(size_t i=0; i<size; i++)
		buf[i] = pSrc[size-1-i];
	dest.set_size(size);

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
		return false;
	}
	return true;
}

void putDocWithoutDefaultCompression(XmlManager& mgr,
				     const string& containerName,
				     XmlDocument& xdoc,
				     XmlUpdateContext& uc)
{
	// Set the container type as WholedocContainer and turn off
	// the default compression
	XmlContainerConfig contConf;
	contConf.setAllowCreate(true);
	contConf.setContainerType(XmlContainer::WholedocContainer);
	contConf.setCompressionName(XmlContainerConfig::NO_COMPRESSION);

	// Create container
	XmlContainer cont = mgr.createContainer(containerName, contConf);
	
	// Put Document
	cont.putDocument(xdoc, uc);
}

void putDocWithDefaultCompression(XmlManager& mgr,
				  const string& containerName,
				  XmlDocument& xdoc,
				  XmlUpdateContext& uc)
{
	// Set the container type as WholedocContainer and
	// use the default compression
	XmlContainerConfig contConf;
	contConf.setAllowCreate(true);
	contConf.setContainerType(XmlContainer::WholedocContainer);

	// The following line is unnecessary because default compression
	// would take effect if user do not turn off it explicitly.
	contConf.setCompressionName(XmlContainerConfig::DEFAULT_COMPRESSION);

	// Create container
	XmlContainer cont = mgr.createContainer(containerName, contConf);

	// Put Document
	cont.putDocument(xdoc, uc);
}

void putDocWithMyCompression(XmlManager& mgr,
			     const string& containerName,
			     XmlDocument& xdoc,
			     XmlUpdateContext& uc,
			     XmlCompression& myCompression)
{
	// Define an unique name to use for registering the compression
	string compressionName = "myCompression";
	
	// Register custom class
	mgr.registerCompression(compressionName.c_str(), myCompression);

	// Set the container type as WholedocContainer
	// and use the custom compression
	XmlContainerConfig contConf;
	contConf.setAllowCreate(true);
	contConf.setContainerType(XmlContainer::WholedocContainer);
	contConf.setCompressionName(compressionName.c_str());

	// Create container
	XmlContainer cont = mgr.createContainer(containerName, contConf);

	// Put Document
	cont.putDocument(xdoc, uc);
}

string getContent(XmlManager& mgr,
		  const string& containerName,
		  const string& docName)
{
	XmlContainer cont = mgr.openContainer(containerName);
	string content;
	cont.getDocument(docName).getContent(content);
	return content;
}

int main(int argc, char **argv)
{
	string containerName1 = "compressionContainer1.dbxml";
	string containerName2 = "compressionContainer2.dbxml";
	string containerName3 = "compressionContainer3.dbxml";
	string docName = "doc1.xml";
	string content = "<root><a></a></root>";
	try{

	// Create manager and update context. It's good to reuse update context.
	XmlManager mgr;
	XmlUpdateContext uc = mgr.createUpdateContext();

	// Remove the existing container if any
	if( mgr.existsContainer(containerName1) )
		mgr.removeContainer(containerName1);
	if( mgr.existsContainer(containerName2) )
		mgr.removeContainer(containerName2);
	if( mgr.existsContainer(containerName3) )
		mgr.removeContainer(containerName3);

	// Create document and put it into the container without compression.
	XmlDocument xdoc1 = mgr.createDocument();
	xdoc1.setName(docName);
	xdoc1.setContent(content);
	putDocWithoutDefaultCompression(mgr, containerName1, xdoc1, uc);

	// Verify Content
	cout << "Content of the document: "
	     << getContent(mgr, containerName1, xdoc1.getName())
	     << endl;

	// Create document and put it into the container with default compression.
	XmlDocument xdoc2 = mgr.createDocument();
	xdoc2.setName(docName);
	xdoc2.setContent(content);
	putDocWithDefaultCompression(mgr, containerName2, xdoc2, uc);

	// Verify Content
	cout << "Content of the document: "
	     << getContent(mgr, containerName2, xdoc2.getName())
	     << endl;

	// Create an instance of custom compression
	MyCompression myCompression;

	// Create document and put it into the container with custom compression.
	XmlDocument xdoc3 = mgr.createDocument();
	xdoc3.setName(docName);
	xdoc3.setContent(content);
	putDocWithMyCompression(mgr, containerName3, xdoc3, uc, myCompression);

	// Verify Content
	cout << "Content of the document: "
	     << getContent(mgr, containerName3, xdoc3.getName())
	     << endl;

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
		return 1;
	}
	return 0;
}
