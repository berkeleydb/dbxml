/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* exampleLoadContainer
*
* This program loads XML data into an identified container and environment.
* Provide the directory where you want to place your database environment, the
* name of the container to use, and the xml files you want inserted into the
* container.
* 
* Usage: exampleLoadContainer -h <dbenv directory> -c <container> -f <filelist> -p <filepath> file1.xml file2.xml file3.xml ....
*
* This program demonstrates:
*  How to insert XML data
*  How to create a Berkeley DB XML transactional container
*  How to set the name and content of an XmlDocument
*  How to set meta data for the XmlDocument
*  How to insert an xml document into a container
*
*/

#include <dbxml/DbXml.hpp>

#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <db.h>

using namespace DbXml;
//some exception handling omitted for clarity

void usage()
{
	std::cerr << "This program loads XML data into an identified container and environment.\n"
		<< "Provide the directory where you want to place your database environment, the\n"
		<< "name of the container to use, and the xml files you want inserted into the\n"
		<< "container.\n\n"

		<< "\t-h <dbenv directory> -c <container> -f <filelist> -p <filepath> file1.xml file2.xml file3.xml .... " << std::endl;
	exit( -1 );
}

int main(int argc, char **argv)
{
	//Used for metadata example
	const std::string mdURI = "http://dbxmlExamples/timestamp";
	const std::string mdName = "timeStamp";

	std::string path2DbEnv;
	std::string theContainer;
	std::string filePath;
	std::vector<std::string> files2add;

	for ( int i=1; i<argc; i++ )
	{
		if ( argv[i][0] == '-' )
		{
			switch(argv[i][1])
			{
			case 'h':
				path2DbEnv = argv[++i];
				break;
			case 'c':
				theContainer = argv[++i];
				break;
			case 'p':
				filePath = argv[++i];
				break;
			case 'f':
				{
					std::ifstream inFile(argv[++i],std::ios::in);
					if ( !inFile )
					{
						std::cerr << "Could not open file " << argv[++i] << ". Giving up." << std::endl;
						exit( -1 );
					}
					while ( !inFile.eof() )
					{
						std::string stringBuf;
						std::getline( inFile, stringBuf );
						if(!stringBuf.empty())
						{
							files2add.push_back( filePath + stringBuf );
						}
					}
				}
				break;
			default:
				usage();
			}

		} else {
			files2add.push_back( argv[i] );
		}
	}

	//You must provide a container name, a path to a database environment,
	//and at least one file to add to the container.
	if ( ! path2DbEnv.length() || ! theContainer.length() || files2add.empty() )
	{
		usage();
	}

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
		// Boost some default resource sizes (locks only work
		//   at environment creation time)
		dbEnv->set_cachesize(dbEnv, 0, envCacheSize, 1);
		dbEnv->set_lk_max_lockers(dbEnv, 10000);
		dbEnv->set_lk_max_locks(dbEnv, 20000);
		dbEnv->set_lk_max_objects(dbEnv, 20000);
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
	config.setAllowCreate(true);

	//Open a container in the db environment
	XmlContainer container = db.openContainer(theContainer, config);

	// Get an update context.
	XmlUpdateContext updateContext = db.createUpdateContext();

	//Iterate through the list of files to add, read each into a string and
	//  put that string into the now opened container.

	//Get a transaction
	XmlTransaction txn = db.createTransaction();
	try {
		std::vector<std::string>::iterator theFile;
		for ( theFile = files2add.begin(); theFile != files2add.end(); ++theFile )
		{
			/* Load the xml document into a string */
			std::string xmlString;
			/* Open as binary to preserve whitespace */
			std::ifstream inFile( (*theFile).c_str(), std::ios::in | std::ios::binary );
			if ( !inFile )
			{
				std::cerr << "Could not open file '" << *theFile << "'. Giving up." << std::endl;
				throw std::exception();
			}

			while ( !inFile.eof() )
			{
				std::string stringBuf;
				std::getline( inFile, stringBuf );
				// use \xA for the eol given that these are XML files.
				stringBuf += '\xA';
				xmlString += stringBuf;
			}

			inFile.close();

			/* declare an xml document */
			XmlDocument xmlDoc = db.createDocument();

			/* Set the xml document's content to be the xmlString we just obtained. */
			xmlDoc.setContent( xmlString );

			// Get the document name. this strips off any path information.
			std::string tmpName = *theFile;
			std::string theName;
			std::string::size_type pos = tmpName.rfind("/");
			if (pos == std::string::npos ) //maybe on windows?
			{
				pos = tmpName.rfind("\\");
			}
			if ( pos == std::string::npos ) //giving up
			{
				theName = tmpName;
			} else {
				theName.assign( tmpName, pos+1, tmpName.length() );
			}

			//Set the document name
			xmlDoc.setName( theName );

			//Set sample metadata who's value is the current date and time.
			//the variables used here are defined in gettingStarted.hpp

			//Get the local time
			time_t now = time( 0 );
			char timeString[100];
			strftime(timeString, 100, "%d/%m/%Y:%H:%M:%S", localtime( &now ) );

			//Set the localtime onto the timeStamp metadata attribute
			xmlDoc.setMetaData( mdURI, mdName, (std::string)timeString );

			/* place that document into the container */
			container.putDocument(txn, xmlDoc, updateContext);

			std::cout << "\tAdded " << *theFile << " to container " << theContainer << "." << std::endl;

		} //end files2add iterator

		txn.commit();
	}
	catch(std::exception &e)
	{
		std::cerr << "Error adding XML data to container " << theContainer << std::endl;
		std::cerr << e.what() << std::endl;
		txn.abort();

		exit( -1 );
	}

	return 0;
}
