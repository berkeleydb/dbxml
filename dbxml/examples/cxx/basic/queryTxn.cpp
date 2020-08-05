/*
* See the file LICENSE for redistribution information.
*
* Copyright (c) 2004,2009 Oracle.  All rights reserved.
*
*
*******
*
* queryTxn
*
* A very simple Berkeley DB XML program
* that does a query and handles results, using
* transactions.
*
* This program demonstrates:
*  Transactional initialization
*  Transactional container creation
*  Transactional document insertion
*  Transactional query creation and execution
*  Use of a variable in a query and context
*  Transactional results handling
*
*  Usage: queryTxn [-h environmentDirectory]
*
*/

#include <cstdlib>
#include <iostream>
#include <dbxml/DbXml.hpp>
#include <db.h>

using namespace DbXml;

// A standard Berkeley DB error handler to get extra information
// on database errors
static void errcall(const DB_ENV *dbenv, const char *errPfx, const char *errMsg)
{
	std::cerr << "Error from Berkeley DB: " << errMsg << std::endl;
}

static void usage(const char *progname)
{
	std::cerr << "Usage:" << progname << " [-h environmentDirectory]"
		<< std::endl;
	exit(1);
}

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
	std::string environmentDir = ".";

	// Berkeley DB environment flags
	u_int32_t envFlags = DB_RECOVER|DB_CREATE|DB_INIT_MPOOL|
		DB_INIT_LOCK|DB_INIT_TXN|DB_INIT_LOG;
	// Berkeley DB cache size (25 MB).  The default is quite small
	u_int32_t envCacheSize = 25*1024*1024;

	// argument parsing should really use getopt(), but
	// avoid it for platform-independence
	if (argc == 3) {
		if (std::string(argv[1]) != std::string("-h"))
			usage(argv[0]);
		environmentDir = argv[2];
	} else if (argc != 1)
		usage(argv[0]);

	// Create and open a Berkeley DB Transactional Environment.
	int dberr;
	DB_ENV *dbEnv = 0;
	dberr = db_env_create(&dbEnv, 0);
	if (dberr == 0) {
		dbEnv->set_cachesize(dbEnv, 0, envCacheSize, 1);
		dbEnv->set_errcall(dbEnv, errcall); // set error callback
		dbEnv->set_lk_detect(dbEnv, DB_LOCK_DEFAULT); // handle deadlocks
		dberr = dbEnv->open(dbEnv, environmentDir.c_str(), envFlags, 0);
	}
	if (dberr) {
		std::cout << "Unable to create environment handle due to the following error: " <<
			db_strerror(dberr) << std::endl;
		if (dbEnv) dbEnv->close(dbEnv, 0);
		return -1;
	}

	try {
		// All BDB XML programs require an XmlManager instance.
		// Create it from the DbEnv
		XmlManager mgr(dbEnv, DBXML_ADOPT_DBENV);

		// Because the container will exist on disk, remove it
		// first if it exists
		if (mgr.existsContainer(containerName))
			mgr.removeContainer(containerName);

		/* Create a container that is transactional.  The container
		* type is NodeContainer, which is the default container type,
		* and the index is on nodes, which is the default for a
		* NodeContainer container.  XmlContainerConfig can be used
		* to set the container type and index type.
		*/
		XmlContainerConfig config;
		config.setTransactional(true);
		XmlContainer cont = mgr.createContainer(
			containerName,
			config);

		// All Container modification operations need XmlUpdateContext
		XmlUpdateContext uc = mgr.createUpdateContext();

		// The following putDocument call will auto-transact
		// and will abort/cleanup if the operation deadlocks or
		// otherwise fails
		cont.putDocument(docName, content, uc);

		// Querying requires an XmlQueryContext
		XmlQueryContext qc = mgr.createQueryContext();

		// Add a variable to the query context, used by the query
		qc.setVariableValue("name", "mary");

		// Use try/catch and while to handle deadlock/retry
		int retry = 0;
		while (retry < 5) { // hard-code 5 retries
			// Create a new transaction for the query
			XmlTransaction txn = mgr.createTransaction();

			try {
				// o Note the passing of txn to both methods
				// o Often the XmlQueryExpression object will be created and
				// saved for reuse in order to amortize the cost of parsing a query
				XmlQueryExpression expr = mgr.prepare(txn, queryString, qc);
				XmlResults res = expr.execute(txn, qc);

				// Note use of XmlQueryExpression::getQuery() and
				// XmlResults::size()
				std::cout << "The query, '" << expr.getQuery() << "' returned " <<
					(unsigned int)res.size() << " result(s)" << std::endl;

				// Process results -- just print them
				XmlValue value;
				std::cout << "Result: " << std::endl;
				while (res.next(value)) {
					std::cout << "\t" << value.asString() << std::endl;
				}

				// done with the transaction
				txn.commit();
				break;
			} catch (XmlException &x) {
				txn.abort(); // unconditional
				if ((x.getExceptionCode() == XmlException::DATABASE_ERROR) &&
					(x.getDbErrno() == DB_LOCK_DEADLOCK)) {
						++retry;
						continue; // try again
				}
				throw; // re-throw -- not deadlock
			}
		}
		// In C++, resources are released as objects go out
		// of scope.
	} catch (XmlException &xe) {
		std::cout << "XmlException: " << xe.what() << std::endl;
	}

	return 0;
}

