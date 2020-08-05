//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// System includes
#include <string>
#include <iostream>
#include <vector>

// XQilla includes
#include <xqilla/exceptions/XQException.hpp>

// Xerces includes
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>

// test code includes
#include "TestEnvironment.hpp"
#include "../util/TestLogging.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;

// function prototypes (so that main() appears first...)
void usage(const string &progname, int exitCode);

int main(int argc, char **argv)
{
  bool validation(false);	 // true iff docs are to be validated
  bool secureMode(false);        // true iff manaager is created in secure mode
  bool transacted(false);        // true iff a transacted environment is used
  bool nodeLevelStorage(false);  // node level storage (whole docs otherwise)
  bool indexNodes(false);        // use the DBXML_INDEX_NODES flag
  bool verifyLog(false);         // true iff log file is to be verified
  bool debug(false);             // true iff all dbxml debugging is on
  string logDir("./");           // path to dir to contain log file
  string dataDirectory;          // path to dir containing unit test data
  string path2DbEnv;             // path to DB XML env (must exist)
  string testid;                 // arbitrary id for test (used for file names)
  string container;              // open this container or create one
  vector<string> unknownArgs;    // args passed on to test objects

  int err(1);

  for(int i=1; i<argc; ++i) {
    if(argv[i][0] == '-') {
      // help
      if(!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6) || !strncmp(argv[i], "-?", 2)) {
        usage(argv[0], 0);
      }
      // DB XML environment
      else if(!strncmp(argv[i], "-e", 2) || !strncmp(argv[i], "--env", 5)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No environment specified!" << endl;
  	  		usage(argv[0], -1);
        }
        path2DbEnv = arg;
      }
      // directory containing test data
      else if(!strncmp(argv[i], "-d", 2) || !strncmp(argv[i], "--datadir", 9)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No data directory specified!" << endl;
  	  		usage(argv[0], -1);
        }
        // ensure we have a trailing slash
        // FIXME x-platform compatability?
        string tmp(arg);
        string::size_type pos = tmp.rfind("/");
        if(pos != tmp.length()-1) { //maybe on windows?
          pos = tmp.rfind("\\");
        }
        if (pos != tmp.length()-1) { //just add a slash
          tmp.append("/");
        }
        dataDirectory.assign(tmp);
      }
      // container (must exist if specified)
      else if(!strncmp(argv[i], "-c", 2) || !strncmp(argv[i], "--container", 11)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No container specified!" << endl;
  	  		usage(argv[0], -1);
        }
        container.assign(arg);
      }
      // transacted environment
      else if(!strncmp(argv[i], "-t", 2) || !strncmp(argv[i], "--transacted", 12)) {
        transacted = true;
      }
      // secure mode
      else if(!strncmp(argv[i], "-s", 2) || !strncmp(argv[i], "--secure", 8)) {
        secureMode = true;
      }
      // validation
      else if(!strncmp(argv[i], "-a", 2) || !strncmp(argv[i], "--validate", 8)) {
        validation = true;
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
        // FIXME x-platform compatability?
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
      // dbxml debug logging
      else if(!strncmp(argv[i], "-x", 2) || !strncmp(argv[i], "--debug", 7)) {
        debug = true;
      }
      else {
        // cache unknown switches (pass onto test environment)
        unknownArgs.push_back(argv[i]);
      }
    }
    else {
      // cache unknown args (pass onto test environment)
      unknownArgs.push_back(argv[i]);
    }
  }

  if(path2DbEnv.empty()) {
    cerr << "No path to DB Environment specified" << endl;
    usage(argv[0], -1);
  }
  if(dataDirectory.empty()) {
    cerr << "No data directory specified" << endl;
    usage(argv[0], -1);
  }

  // initiate logging for test harness
  string logFile(logDir);
  logFile.append("dbxml_test_driver");
  if(!testid.empty()) {
    logFile.append(string(".") + testid);
  }
  logFile.append(".log");
	TestLogger log(logFile);

  INFO_MSG(log, "Unit test driver running for test id %1") << testid;
  INFO_MSG(log, "Using DB environment '%1'") << path2DbEnv;
  INFO_MSG(log, "Using data directory '%1'") << dataDirectory;
  INFO_MSG(log, "Using a %1 transacted environment") << (transacted ? "" : "non");
  if(secureMode) INFO_MSG(log, "Using DB XML \"secure mode\"");
  if(nodeLevelStorage) {
    INFO_MSG(log, "Using a node level storage model");
  } else {
    INFO_MSG(log, "Using a document level storage model");
  }
  if(indexNodes) {
    INFO_MSG(log, "Indexing nodes");
  }
  if(!container.empty()) INFO_MSG(log, "Using the container '%1'") << container;

  try {
    // create and execute the test environment
    bool wholeDocumentStorage = !nodeLevelStorage;
    TestEnvironment env(&log, path2DbEnv, debug, transacted, wholeDocumentStorage, indexNodes, secureMode, validation);
    env.run(testid, dataDirectory, container, Iterator<string>(unknownArgs));

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

void usage(const string &progname, int exitCode)
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

  cerr << "Usage: " << progname.substr(pos) << " [OPTIONS] -e|--env path_to_dbenv " << endl;
  cerr << "Options:" << endl;
  cerr << "\t-t|--transacted\t\tuse a transacted environment" << endl;
  cerr << "\t-s|--secure\t\tuse secure mode" << endl;
  cerr << "\t-a|--validate\t\tuse validation mode" << endl;
  cerr << "\t-n|--nls\t\tuse node level storage" << endl;
  cerr << "\t--indexnodes\t\tuse DBXML_INDEX_NODES flag" << endl;
  cerr << "\t-c|--container ctr\topen the specified container" << endl;
  cerr << "\t--id id\t\tassign a (test) ID to this invocation" << endl;
  cerr << "\t-d|--datadir dir\tdirectory containing data files (test input)" << endl;
  cerr << "\t-l|--logdir\t\tdirectory for the log files [.]" << endl;
  cerr << "\t-v|--verify\t\tverify the log file for warnings and errors" << endl;
  cerr << "\t-x|--debug|-?\t\tdebug logging on" << endl;
  cerr << "\t-h|--help|-?\t\tprint this help" << endl;
  cerr << endl;
  cerr << "Additional options may be supported by specific test classes." << endl;
  cerr << endl;

  exit(exitCode);
}

