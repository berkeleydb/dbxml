/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* simpleContainer
*
* This program shows how to create a Berkeley DB XML container for storing XML data.
* 
* Usage: simpleContainer
*
* This program demonstrates:
*  How to create a container for storing XML data
*  How to create an XmlManager
*  How to create a temporary container
*
*/

#include <cstdlib>
#include <stdlib.h>

#include <dbxml/DbXml.hpp>

using namespace DbXml;
//exception handling omitted for clarity

//Very simple program that creates a container
int main(void)
{
	//Every DB XML program requires a manager
	XmlManager manager;

	//Create the non-transactional container. A container without a name exists only in
	//memory and is not persistant
	XmlContainer myContainer = manager.createContainer("");

	//do DB XML work here

	//The manager and container will close themselves when they go out of scope
}
