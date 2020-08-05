/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* simpleContainerInEnv
*
* This program shows how to create several Berkeley DB XML containers in a
* Berkeley DB environment.
*
* Edit the variable "evinronmentPath" to point to the directory in which you want the 
* containers to be created.
* 
* Usage: simpleContainerInEnv
*
* This program demonstrates:
*  How to create a transactional Berkeley DB environment
*  How to create multiple transactional containers in the environment
*  
*/

#include <cstdlib>
#include <stdlib.h>
#include <iostream>

#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;
//some exception handling omitted for clarity

int main(void)
{
	//The path the directory where you want to place the environment
	// must exist!!
	const char *environmentPath = "/path/to/environment/directory";

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
		dberr = dbEnv->open(dbEnv, environmentPath, envFlags, 0);
	}
	if (dberr) {
		std::cout << "Unable to create environment handle due to the following error: " <<
			db_strerror(dberr) << std::endl;
		if (dbEnv) dbEnv->close(dbEnv, 0);
		return -1;
	}

	//Have the XmlManager adopt the db environment
	XmlManager db(dbEnv, DBXML_ADOPT_DBENV);

	//Configure the containers to be transactional, and to be created
	//if they do not already exist
	XmlContainerConfig config;
	config.setTransactional(true);
	config.setAllowCreate(true);

	//multiple containers can be opened in the same database environment
	XmlContainer container1 = db.openContainer("myContainer1", config);

	XmlContainer container2 = db.openContainer("myContainer2", config);

	XmlContainer container3 = db.openContainer("myContainer3", config);

	// do work here //

	//The manager and containers will close themselves when they go out of scope
	return 0;
}
