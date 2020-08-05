//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// Test program for management of XmlManager objects

// System includes
#include <string>
#include <iostream>
#include <sstream>

// XQilla includes
#include <xqilla/exceptions/XQException.hpp>

// Xerces includes
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// test code includes
#include "../util/TestLogging.hpp"

using namespace DbXmlTest;
using namespace DbXml;
using namespace std;

// function prototypes (so that main() appears first...)
void doTest(TestLogger &log, const std::string &testid, bool transacted, bool nodeLevelStorage, bool indexNodes, const std::string &path2DbEnv1, const std::string &path2DbEnv2);
void usage(const std::string &progname, int exitCode);
DB_ENV *createEnv(const char *path, u_int32_t flags);

int main(int argc, char **argv)
{
  string testid;                   // arbitrary id for test (used for file names)
  bool transacted(false);          // true iff a transacted environment is used
  bool nodeLevelStorage(false);    // node level storage (whole docs otherwise)
  bool indexNodes(false);          // true if nodes should be indexed
  bool verifyLog(false);           // true iff log file is to be verified
  string path2DbEnv1, path2DbEnv2; // paths to DB XML env (must exist)
  string logDir("./");             // path to dir to contain log file

  int err(1);

  for(int i=1; i<argc; ++i) {
    if(argv[i][0] == '-') {
      // help
      if(!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6) || !strncmp(argv[i], "-?", 2)) {
        usage(argv[0], 0);
      }
      // DB XML environments (2)
      else if(!strncmp(argv[i], "--env1", 6)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No first environment specified!" << endl;
  	  		usage(argv[0], -1);
        }
        path2DbEnv1 = arg;
      }
      else if(!strncmp(argv[i], "--env2", 6)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No second environment specified!" << endl;
  	  		usage(argv[0], -1);
        }
        path2DbEnv2 = arg;
      }
      // transacted environment
      else if(!strncmp(argv[i], "-t", 2) || !strncmp(argv[i], "--transacted", 12)) {
        transacted = true;
      }
      // NLS
      else if(!strncmp(argv[i], "-n", 2) || !strncmp(argv[i], "--nls", 5)) {
        nodeLevelStorage = true;
      }
      // node indexes
      else if(!strncmp(argv[i], "--indexnodes", 12)) {
        indexNodes = true;
      }
      // test id
      else if(!strncmp(argv[i], "--id", 4)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No test ID specified!" << endl;
  	  		usage(argv[0], -1);
        }
        testid.assign(arg);
      }
      // log file directory
      else if(!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "--logdir", 8)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No log file directory specified!" << endl;
  	  		usage(argv[0], -1);
        }
        // ensure we have a trailing slash
        string tmp(arg);
        string::size_type pos = tmp.rfind("/");
        if(pos != tmp.length()-1) { //maybe on windows?
          pos = tmp.rfind("\\");
        }
        if (pos != tmp.length()-1) { //just add a slash
          tmp.append("/");
        }
        logDir.assign(tmp);
      }
      // verify log file
      else if(!strncmp(argv[i], "-v", 2) || !strncmp(argv[i], "--verify", 8)) {
        verifyLog = true;
      }
      // unknown switch
      else {
        cerr << "Unknown switch: " << argv[i] << endl;
        usage(argv[0], -1);
      }
    }
    // unknown argument
    else {
      cerr << "Unknown argument: " << argv[i] << endl;
      usage(argv[0], -1);
    }
  }

  if(path2DbEnv1.empty()) {
    cerr << "No path to first DB Environment specified" << endl;
    usage(argv[0], -1);
  }
  if(path2DbEnv2.empty()) {
    cerr << "No path to second DB Environment specified" << endl;
    usage(argv[0], -1);
  }

  // initiate logging for test harness
  string logFile(logDir);
  logFile.append("dbxml_test_databases");
  if(!testid.empty()) {
    logFile.append(string(".") + testid);
  }
  logFile.append(".log");
	TestLogger log(logFile);

  INFO_MSG(log, "Unit test driver running for test id %1") << testid;
  INFO_MSG(log, "Using DB environments '%1' / '%2'") << path2DbEnv1 << path2DbEnv2;
  INFO_MSG(log, "Using a %1 transacted environment") << (transacted ? "" : "non");
  if(nodeLevelStorage) {
    INFO_MSG(log, "Using a node level storage model");
  } else {
    INFO_MSG(log, "Using a document level storage model");
  }
  if(indexNodes) {
    INFO_MSG(log, "Indexing nodes");
  }

  try {
    doTest(log, testid, transacted, nodeLevelStorage, indexNodes, path2DbEnv1, path2DbEnv2);
    ENV_MSG(log, "Successful completion of test");
    err = 0;
  }
  catch(XERCES_CPP_NAMESPACE_QUALIFIER DOMException &e) {
    INFO_MSG(log, "Caught Xerces DOM Exception: %1") << XMLChToUTF8(e.msg).str();
  }
  catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
    INFO_MSG(log, "Caught Xerces Exception: %1") << XMLChToUTF8(e.getMessage()).str();
  }
  catch(XQException &e) {
    INFO_MSG(log, "Caught XQilla Exception: %1") << XMLChToUTF8(e.getError()).str();
  }
  catch(XmlException &e) {
    INFO_MSG(log, "Caught DB XML Exception: %1: %2") << e.getExceptionCode() << (char*)e.what();
  }
  catch(...) {
    INFO_MSG(log, "Caught unknown exception");
  }

  if(err) {
    cout << endl << "Tests did not pass - an exception was caught" << endl;
  } else {
    cout << "Completed the test program" << endl;
  }

  if(verifyLog) {
    cout << verifyLogFile(logFile);
  } else {
    cout << "Check the log \"" << logFile << "\" for more information, especially error and warning messages" << endl;
  }

  return err;
}

// the actual tests
// ASSUME the directories for the DB XML environments exist
void doTest(TestLogger &log, const std::string &testid,
	bool transacted, bool nodeLevelStorage, bool indexNodes,
	const std::string &path2DbEnv1, const std::string &path2DbEnv2)
{
	u_int32_t flags = (DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL);
	if(transacted) flags |= DB_INIT_TXN;

	// 8.5.1 - create two database environments, using the same directory
	//   create a container in each of the databases
	TEST_MSG(log, "8.5.1 - create 2 database environments using the same directory");
	{
		DB_ENV *env = createEnv(path2DbEnv1.c_str(), flags);
		XmlManager db1(env, DBXML_ADOPT_DBENV);
		db1.setDefaultContainerType(
			nodeLevelStorage ? XmlContainer::NodeContainer :
			XmlContainer::WholedocContainer);
		db1.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		XmlContainer *c1;
		if(transacted) {
			XmlTransaction txn = db1.createTransaction();
			c1 = new XmlContainer(db1.createContainer(
						      txn, "container_1_1.dbxml"));
			txn.commit();
		} else {
			c1 = new XmlContainer(db1.createContainer(
						      "container_1_1.dbxml"));
		}

		DB_ENV *env2 = createEnv(path2DbEnv1.c_str(), flags);
		XmlManager db2(env2, DBXML_ADOPT_DBENV);
		db2.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db2.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		XmlContainer *c2;
		if(transacted) {
			XmlTransaction txn = db2.createTransaction();
			c2 = new XmlContainer(db2.createContainer(
						      txn, "container_1_2.dbxml"));
			txn.commit();
		} else {
			c2 = new XmlContainer(db2.createContainer(
						      "container_1_2.dbxml"));
		}
		delete c1;
		delete c2;
	}
	TEST_MSG(log, "Completed test");

	// 8.5.2 - create two database environments, using separate directories
	//   create a container in each of the databases
	TEST_MSG(log, "8.5.2 - create 2 database environments using separate directories");
	{
		DB_ENV *env = createEnv(path2DbEnv1.c_str(), flags);
		XmlManager db1(env, DBXML_ADOPT_DBENV);
		db1.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db1.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		XmlContainer *c1;
		if(transacted) {
			XmlTransaction txn = db1.createTransaction();
			c1 = new XmlContainer(db1.createContainer(
						      txn, "container_2_1.dbxml"));
			txn.commit();
		} else {
			c1 = new XmlContainer(db1.createContainer(
						      "container_2_1.dbxml"));
		}

		DB_ENV *env2 = createEnv(path2DbEnv2.c_str(), flags);
		XmlManager db2(env2, DBXML_ADOPT_DBENV);
		db2.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db2.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		XmlContainer *c2;
		if(transacted) {
			XmlTransaction txn = db2.createTransaction();
			c2 = new XmlContainer(db2.createContainer(
						      txn, "container_2_2.dbxml"));
			txn.commit();
		} else {
			c2 = new XmlContainer(db2.createContainer(
						      "container_2_2.dbxml"));
		}
		delete c1;
		delete c2;
	}
	TEST_MSG(log, "Completed test");

	// 8.5.3 - test transaction notifications
	// a.  create new container, abort txn, try to open (no create) and use
	//     original container
	TEST_MSG(log, "8.5.3 - Test transaction notifications");
	{
		DB_ENV *tenv;
		db_env_create(&tenv, 0);
		tenv->remove(tenv, path2DbEnv1.c_str(), 0);

		DB_ENV *env = createEnv(path2DbEnv1.c_str(), flags|DB_INIT_TXN);
		XmlManager db1(env, DBXML_ADOPT_DBENV);
		db1.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db1.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		XmlTransaction txn = db1.createTransaction();
		XmlContainer c1 = db1.createContainer(txn, "container_3_1.dbxml");
		txn.abort();
		// try again...should fail
		try {
			XmlTransaction txn1 = db1.createTransaction();
			XmlContainer c2 = db1.openContainer(
				txn1, "container_3_1.dbxml");
// 			ERROR_MSG(log,"8.5.3: openContainer should have failed");
			txn1.abort();
		} catch (XmlException &e) {
// 			// good
			ERROR_MSG(log,"8.5.3: openContainer should not have failed");
		}
	}
	TEST_MSG(log, "Completed test");

	// 8.5.4 - test transaction notifications
	// a.  same as 8.5.3, but use externally created DB_TXN, and
	//     destroy original XmlTransaction, making sure the ref
	//     counting works.
	TEST_MSG(log, "8.5.4 - Test transaction notifications");
	{
		DB_ENV *env = createEnv(path2DbEnv1.c_str(), flags|DB_INIT_TXN);
		XmlManager db1(env, DBXML_ADOPT_DBENV);
		db1.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db1.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		DB_TXN *dbTxn;
		env->txn_begin(env, 0, &dbTxn, 0);
		XmlTransaction *xmlTxn;
		{
			XmlTransaction txn = db1.createTransaction(dbTxn);
			xmlTxn = new XmlTransaction(txn);
		}
		XmlContainer c1 = db1.createContainer(*xmlTxn,
						      "container_3_2.dbxml");
		delete xmlTxn;
		dbTxn->abort(dbTxn);
		// try again...should fail
		try {
			XmlTransaction txn = db1.createTransaction();
			XmlContainer c2 = db1.openContainer(
				txn, "container_3_2.dbxml");
// 			ERROR_MSG(log,"8.5.4: openContainer should have failed");
		} catch (XmlException &e) {
// 			// good
			ERROR_MSG(log,"8.5.4: openContainer should not have failed");
		}
	}
	TEST_MSG(log, "Completed test");
	// 8.5.5 - test transaction notifications
	// a.  same as 8.5.4, but commit the txn, actually create the container
	TEST_MSG(log, "8.5.5 - Test transaction notifications");
	{
		DB_ENV *env = createEnv(path2DbEnv1.c_str(), flags|DB_INIT_TXN);
		XmlManager db1(env, DBXML_ADOPT_DBENV);

		db1.setDefaultContainerType(nodeLevelStorage ?
					    XmlContainer::NodeContainer :
					    XmlContainer::WholedocContainer);
		db1.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);
		DB_TXN *dbTxn;
		env->txn_begin(env, 0, &dbTxn, 0);
		XmlTransaction *xmlTxn;
		{
			XmlTransaction txn = db1.createTransaction(dbTxn);
			xmlTxn = new XmlTransaction(txn);
		}
		XmlContainer c1 = db1.createContainer(
			*xmlTxn, "container_3_3.dbxml");
		delete xmlTxn;
		dbTxn->commit(dbTxn, 0);
		// open again...should work this time
		try {
			XmlTransaction txn = db1.createTransaction();
			XmlContainer c2 = db1.openContainer(
				txn, "container_3_3.dbxml");
		} catch (XmlException &e) {
			ERROR_MSG(log,"8.5.5: openContainer should have succeeded");
		}
	}
	TEST_MSG(log, "Completed test");
}

DB_ENV *createEnv(const char *path, u_int32_t flags)
{
	DB_ENV *env;
	int err = db_env_create(&env, 0);
	if (err == 0) {
		err = env->open(env, path, flags, 0);
		if (err == 0)
			return env;
	}
	cerr << "Failed to create/open DB_ENV: " << err << endl;
	exit(-1);
	return 0; // not-reached
}

void usage(const std::string &progname, int exitCode)
{
	string::size_type pos = progname.rfind('/');
	if(pos == string::npos) {
		pos = progname.rfind('\\');
	}
	if(pos == string::npos) {
		pos = 0;
	}
	else {
		++pos;
	}

	cerr << "Usage: " << progname.substr(pos) << " [OPTIONS] --env1 path_to_dbenv --env2 path_to_dbenv " << endl;
	cerr << "Options:" << endl;
	cerr << "\t-t|--transacted\t\tuse a transacted environment" << endl;
	cerr << "\t-n|--nls\t\tuse node level storage" << endl;
	cerr << "\t--indexnodes\t\tuse DBXML_INDEX_NODES flag" << endl;
	cerr << "\t--id id\t\tassign a (test) ID to this invocation" << endl;
	cerr << "\t-h|--help|-?\t\tprint this help" << endl;
	cerr << "\t-v|--verify\t\tverify the log file for warnings and errors" << endl;
	cerr << "\t-l|--logdir\t\tdirectory for the log files [.]" << endl;

	cerr << endl;

	exit(exitCode);
}
