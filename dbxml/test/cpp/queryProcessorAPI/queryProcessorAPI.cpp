//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// Test program for iterative methods in XmlResults (extension of 11.3.1/11.4.1)

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
void doTest(TestLogger &log, const std::string &testid, bool cdb, bool transacted,
	    bool nodeLevelStorage, bool indexNodes, const std::string &path2DbEnv);
void usage(const std::string &progname, int exitCode);

int main(int argc, char **argv)
{
  string testid;                 // arbitrary id for test (used for file names)
  bool transacted(false);        // true iff a transacted environment is used
  bool cdb(false);               // true iff a CDS environment is used
  bool nodeLevelStorage(false);  // node level storage (whole docs otherwise)
  bool indexNodes(false);        // use the DBXML_INDEX_NODES flag
  bool verifyLog(false);         // true iff log file is to be verified
  string path2DbEnv;             // path to DB XML env (must exist)
  string logDir("./");           // path to dir to contain log file

  int err(1);

  for(int i=1; i<argc; ++i) {
    if(argv[i][0] == '-') {
      // help
      if(!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6) || !strncmp(argv[i], "-?", 2)) {
        usage(argv[0], 0);
      }
      // DB XML environments
      else if(!strncmp(argv[i], "--env", 5)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No environment specified!" << endl;
  	  		usage(argv[0], -1);
        }
        path2DbEnv = arg;
      }
      // transacted environment
      else if(!strncmp(argv[i], "-t", 2) || !strncmp(argv[i], "--transacted", 12)) {
        transacted = true;
      }
      // CDS environment
      else if(!strncmp(argv[i], "-c", 2) || !strncmp(argv[i], "--cdb", 5)) {
        cdb = true;
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

  if(path2DbEnv.empty()) {
    cerr << "No path to DB Environment specified" << endl;
    usage(argv[0], -1);
  }

  // initiate logging for test harness
  string logFile(logDir);
  logFile.append("dbxml_test_qpapi");
  if(!testid.empty()) {
    logFile.append(string(".") + testid);
  }
  logFile.append(".log");
	TestLogger log(logFile);

  INFO_MSG(log, "Unit test driver running for test id %1") << testid;
  INFO_MSG(log, "Using DB environment '%1'") << path2DbEnv;
  INFO_MSG(log, "Using a %1 transacted environment") << (transacted ? "" : "non");
  if (cdb)
	  INFO_MSG(log, "Using a CDS environment");
  if(nodeLevelStorage) {
    INFO_MSG(log, "Using a node level storage model");
  } else {
    INFO_MSG(log, "Using a document level storage model");
  }
  if(indexNodes) {
    INFO_MSG(log, "Indexing nodes");
  }

  try {
    doTest(log, testid, cdb, transacted, nodeLevelStorage, indexNodes, path2DbEnv);
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
  cerr << "\t-cdb|-- CDS\t\tuse a CDS environment" << endl;
  cerr << "\t-n|--nls\t\tuse node level storage" << endl;
  cerr << "\t--indexnodes\t\tuse DBXML_INDEX_NODES flag" << endl;
  cerr << "\t--id id\t\tassign a (test) ID to this invocation" << endl;
  cerr << "\t-h|--help|-?\t\tprint this help" << endl;
  cerr << "\t-l|--logdir\t\tdirectory for the log files [.]" << endl;
  cerr << "\t-v|--verify\t\tverify the log file for warnings and errors" << endl;

  cerr << endl;

  exit(exitCode);
}

// the actual test - regression tests for XmlResults::next() and for basic
// lazy evaluation
void doTest(TestLogger &log, const std::string &testid, bool cdb, bool transacted,
	    bool nodeLevelStorage, bool indexNodes, const std::string &path2DbEnv)
{
	TEST_MSG(log, "Starting test");

	DB_ENV *tenv;
	(void) db_env_create(&tenv, 0);
	(void) tenv->remove(tenv, path2DbEnv.c_str(), 0);
  
	// prepare - database, container, document
	u_int32_t flags = (DB_CREATE|DB_INIT_MPOOL);
	if (cdb) {
		flags |= DB_INIT_CDB;
		
	} else if (transacted) {
		flags |=( DB_INIT_TXN|DB_INIT_LOG|DB_INIT_LOCK);
	}
	DB_ENV *env;
	(void) db_env_create(&env, 0);
	
	if (flags & DB_INIT_CDB)
		env->set_flags(env, DB_CDB_ALLDB, 1);
	int err = env->open(env, path2DbEnv.c_str(), flags, 0);
	if (err) {
		cerr << "Failed to open DB_ENV: " << err << endl;
		exit(-1);
	}

	XmlManager db(env, DBXML_ADOPT_DBENV);

	db.setDefaultContainerType(nodeLevelStorage ? XmlContainer::NodeContainer : XmlContainer::WholedocContainer);
	db.setDefaultContainerFlags(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES);

	XmlDocument doc = db.createDocument();
	string docContent("<a><b>c</b><d>e</d><d>f</d></a>");
	doc.setContent(docContent);
	doc.setName("foo");

	XmlUpdateContext uc = db.createUpdateContext();

	XmlTransaction *txn = 0;
	if(transacted) {
		txn = new XmlTransaction(db.createTransaction());
	}

	XmlContainer *container = 0;
	string containerName = testid + ".dbxml";
	if(transacted) {
		container = new XmlContainer(db.createContainer(*txn, containerName));
		container->putDocument(*txn, doc, uc);
	} else {
		container = new XmlContainer(db.createContainer(containerName));
		container->putDocument(doc, uc);
	}

	ostringstream os;
	os << "Created the container '" << container->getName() << "'";
	os << " (";
	switch (container->getContainerType()) {
	case XmlContainer::WholedocContainer:
		os << "whole document storage";
		break;
	case XmlContainer::NodeContainer:
		os << "node level storage";
		break;
	default:
		os << "unknown storage";
		break;
	}
	os << ")";
	TEST_MSG(log, os.str());

	/////////////////////////////////////////////////////////////////////////
	// iterative methods on XmlResults (eager evaluation)
	{
		TEST_MSG(log, "Using eager evaluation");

		// execute a simple query that gives a single result
		XmlQueryContext qc = db.createQueryContext();
		qc.setEvaluationType(XmlQueryContext::Eager);
		string queryString("collection('" + container->getName() +
				   "')/a/b");
		XmlResults *xr = 0;
		if(transacted) {
			xr = new XmlResults(db.query(*txn, queryString, qc));
		} else {
			xr = new XmlResults(db.query(queryString, qc));
		}

		if(xr->size() != 1) {
			ERROR_MSG(log, "Wrong number of results, expected 1 but got %1")
				<< (unsigned int) xr->size();
			throw std::exception();
		}

		// test that the document content contains the value as a substring, wheras
		// the document as a string should match the document content

		// TEST next(XmlValue)
		TEST_MSG(log, "Testing XmlResults::next(XmlValue)...");
		XmlValue xv;
		int err = 0;
		int count = 0;
		while(xr->next(xv)) {
			++count;
			if(docContent.find(xv.asString()) == string::npos) {
				ERROR_MSG(log, "result failed, expected to find '%1' within '%2'") << xv.asString() << docContent;
				++err;
			}
		}
		if(count != 1) {
			ERROR_MSG(log, "Wrong number of results, expected 1 but got %1") << count;
			++err;
		}

		if(!err)
			TEST_MSG(log, "...OK");

		// TEST next(XmlDocument)
		TEST_MSG(log, "Testing XmlResults::next(XmlDocument)...");
		xr->reset();
		XmlDocument xd = db.createDocument();;
		count = err = 0;
		string s;
		while(xr->next(xd)) {
			++count;
			if(docContent.compare(xd.getContent(s))) {
				ERROR_MSG(log, "result failed, expected '%1' but got '%2'") << docContent << xd.getContent(s);
				++err;
			}
		}
		if(count != 1) {
			ERROR_MSG(log, "Wrong number of results, expected 1 but got %1") << count;
			++err;
		}
		if(!err)
			TEST_MSG(log, "...OK");

		if(transacted) {
			txn->commit();
		}

		delete xr;
		delete txn;
	}

	/////////////////////////////////////////////////////////////////////////
	// iterative methods on XmlResults (lazy evaluation)
	{
		TEST_MSG(log, "Using lazy evaluation");

		if(transacted) {
			txn = new XmlTransaction(db.createTransaction());
		}

		// execute a simple query that gives a single result
		XmlQueryContext qc = db.createQueryContext();
		qc.setEvaluationType(XmlQueryContext::Lazy);
		string queryString("collection('" + container->getName() +
				   "')/a/b");
		// test that the value contains the document content as a substring, wheras
		// the document as a string should match the document content
		// Note that transaction is kept open for the iterations

		XmlResults *xr = 0;
		if(transacted) {
			xr = new XmlResults(db.query(*txn, queryString, qc));
		} else {
			xr = new XmlResults(db.query(queryString, qc));
		}

		// TEST next(XmlValue)
		TEST_MSG(log, "Testing XmlResults::next(XmlValue)...");
		XmlValue xv;
		int err = 0;
		int count = 0;
		while(xr->next(xv)) {
			++count;
			if(docContent.find(xv.asString()) == string::npos) {
				ERROR_MSG(log, "result failed, expected to find '%1' within '%2'") << xv.asString() << docContent;
				++err;
			}
		}
		if(count != 1) {
			ERROR_MSG(log, "Wrong number of results, expected 1 but got %1") << count;
			++err;
		}

		if(!err)
			TEST_MSG(log, "...OK");

		// TEST next(XmlDocument)
		TEST_MSG(log, "Testing XmlResults::next(XmlDocument)...");
		xr->reset();
		XmlDocument xd = db.createDocument();;
		count = err = 0;
		string s;
		while(xr->next(xd)) {
			++count;
			if(docContent.compare(xd.getContent(s))) {
				ERROR_MSG(log, "result failed, expected '%1' but got '%2'") << docContent << xd.getContent(s);
				++err;
			}
		}
		if(count != 1) {
			ERROR_MSG(log, "Wrong number of results, expected 1 but got %1") << count;
			++err;
		}
		if(!err)
			TEST_MSG(log, "...OK");

		if(transacted) {
			txn->commit();
		}

		delete xr;
		delete txn;
	}

	// clean up
	delete container;

	/////////////////////////////////////////////////////////////////////
	TEST_MSG(log, "Completed test");
}

