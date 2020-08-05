//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// Test program for XmlInputStream (unit tests for the class, plus some functional tests

// System includes
#include <string>
#include <iostream>
#include <sstream>
#include "string.h"

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

// utility stuff for stream type manipulation
namespace DbXmlInputStreamTest {
  enum StreamType {
    NONE,
    LOCALFILE,
    URL,
    MEMBUF,
    STDIN
  };

  std::string typeToString(StreamType type) {
    std::string str("unknown");
    switch(type) {
    case LOCALFILE:
      str.assign("Local File");
      break;
    case URL:
      str.assign("URL");
      break;
    case MEMBUF:
      str.assign("Memory Buffer");
      break;
    case STDIN:
      str.assign("Standard Input");
      break;
    default:
      break;
    }
    return str;
  }
}

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;
using namespace DbXmlInputStreamTest;

void doTest(TestLogger &log, const std::string &testid, bool cdb, bool transacted,
	    bool nodeLevelStorage, bool indexNodes, const std::string &path2DbEnv,
	    StreamType type, const std::string &localFile, const std::string &baseId);
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
  StreamType streamType(NONE);   // type of input stream
  string localFile;              // path to an XML file
  string baseId;                 // baseId for URL

  int err(1);

  cout << "inputstreams" << endl;
  cerr << "inputstreams" << endl;
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
      // CDB environment
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
      // stream type
      else if(!strncmp(argv[i], "-s", 2) || !strncmp(argv[i], "--stream", 8)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No stream type specified!" << endl;
  	  		usage(argv[0], -1);
        }
        string str(arg);
        if(!str.compare("file")) {
          streamType = LOCALFILE;
        } else if(!str.compare("url")) {
          streamType = URL;
        } else if(!str.compare("membuf")) {
          streamType = MEMBUF;
        } else if(!str.compare("stdin")) {
          streamType = STDIN;
        } else {
          cerr << "Unrecognised stream type: " << str << endl;
  	  		usage(argv[0], -1);
        }
      }
      // local file (needed for local file and URL streams)
      else if(!strncmp(argv[i], "-f", 2) || !strncmp(argv[i], "--file", 6)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No file name specified!" << endl;
  	  		usage(argv[0], -1);
        }
        localFile.assign(arg);
      }
      // local file (needed for local file and URL streams)
      else if(!strncmp(argv[i], "-b", 2) || !strncmp(argv[i], "--base", 6)) {
        const char* const arg = argv[++i];
        if(!arg || !strlen(arg)) {
          cerr << "No baseId specified!" << endl;
  	  		usage(argv[0], -1);
        }
        baseId.assign(arg);
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

  if(streamType == NONE) {
    cerr << "No stream type specified" << endl;
    usage(argv[0], -1);
  }

  // initiate logging for test harness
  string logFile(logDir);
  logFile.append("dbxml_test_input_streams");
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

  TEST_MSG(log, "Testing an input stream that uses ") << typeToString(streamType);

  try {
    doTest(log, testid, cdb, transacted, nodeLevelStorage, indexNodes, path2DbEnv, streamType, localFile, baseId);
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
  cerr << "\t-c|--CDB\t\tuse a CDS environment" << endl;
  cerr << "\t-t|--transacted\t\tuse a transacted environment" << endl;
  cerr << "\t-n|--nls\t\tuse node level storage" << endl;
  cerr << "\t--indexnodes\t\tuse DBXML_INDEX_NODES flag" << endl;
  cerr << "\t--id id\t\tassign a (test) ID to this invocation" << endl;
  cerr << "\t-h|--help|-?\t\tprint this help" << endl;
  cerr << "\t-s|--stream\t\tstream type [file|url|membuf|stdin]" << endl;
  cerr << "\t-b|--base\t\tbaseId for URL type" << endl;
  cerr << "\t-f|--file\t\tpath to an XML file (needed for some tests)" << endl;
  cerr << "\t-v|--verify\t\tverify the log file for warnings and errors" << endl;

  cerr << endl;

  exit(exitCode);
}

void testLocalFile(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container, const std::string &localFile);
void testURL(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container, const std::string &baseId, const std::string &localFile);
void testMemoryBuffer(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container);

void basicStreamManipulation(TestLogger &log, XmlInputStream *xis);
void loadDocument(TestLogger &log, XmlInputStream *xis, XmlContainer *container, XmlUpdateContext &uc, XmlTransaction &txn, bool transacted);
void streamDocument(TestLogger &log, XmlInputStream *xis, XmlManager &db);

// the actual test
void doTest(TestLogger &log, const std::string &testid, bool cdb, bool transacted,
	    bool nodeLevelStorage, bool indexNodes, const std::string &path2DbEnv,
	    StreamType type, const std::string &localFile, const std::string &baseId)
{
  TEST_MSG(log, "Starting test");

  ///////////////////////////////////////////////////////
  // prepare - database, container
  u_int32_t flags = (DB_CREATE|DB_INIT_MPOOL);
  if (cdb) {
	  flags |= DB_INIT_CDB;
	  cout << "Using CDB" << endl;
	  cerr << "Using CDB" << endl;
  } else if (transacted) {
	  flags |=( DB_INIT_TXN|DB_INIT_LOG|DB_INIT_LOCK);
	  cout << "Using txns" << endl;
	  cerr << "Using txns" << endl;
  }
  // remove original env first
  DB_ENV *tenv;
  (void) db_env_create(&tenv, 0);
  (void) tenv->remove(tenv, path2DbEnv.c_str(), 0);
  
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

  XmlTransaction *txn = 0;
  if(transacted) {
    txn = new XmlTransaction(db.createTransaction());
  }

  XmlContainer *container = 0;
  string containerName = testid + ".dbxml";
  if(transacted) {
    container = new XmlContainer(db.createContainer(*txn, containerName));
  } else {
    container = new XmlContainer(db.createContainer(containerName));
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

  if(transacted) {
    txn->commit();
    delete txn;
  }

  ///////////////////////////////////////////////////////
  switch(type) {
  case LOCALFILE:
    testLocalFile(log, transacted, db, container, localFile);
    break;
  case URL:
    testURL(log, transacted, db, container, baseId, localFile);
    break;
  case MEMBUF:
    testMemoryBuffer(log, transacted, db, container);
    break;
  case STDIN:
    TEST_MSG(log, "Tests for stdin streams TBD");
    cerr << "Tests for stdin streams TBD" << endl;
    break;
  default:
    TEST_MSG(log, "No recognised stream type specified: %1") << type;
    cerr << "No recognised stream type specified: " << type << endl;
    break;
  }

  ///////////////////////////////////////////////////////
  // clean up

  delete container;

  TEST_MSG(log, "Completed test");
}

void testLocalFile(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container, const std::string &localFile) {
  if(localFile.length() == 0) {
    ERROR_MSG(log, "No local file specified!");
    return;
  }

  // basic manipulation of a stream
  XmlInputStream *xis = db.createLocalFileInputStream(localFile);
  TEST_MSG(log, "Created a stream on the file '%1'") << localFile;

  basicStreamManipulation(log, xis);

  delete xis;

  // load a document from the input stream
  {
    XmlInputStream *xis = db.createLocalFileInputStream(localFile);
    TEST_MSG(log, "Created a stream on the file '%1'") << localFile;

    XmlTransaction *txn = 0;
    if(transacted) {
      txn = new XmlTransaction(db.createTransaction());
    }

    XmlUpdateContext uc = db.createUpdateContext();
    loadDocument(log, xis, container, uc, *txn, transacted);

    if(transacted) {
      txn->commit();
      delete txn;
    }

    // container has adopted stream to put document - do not delete here
  }

  {
    // fail case - file not existing is tested in Tcl code
  }

  // use the input stream to read a document
  {
    XmlInputStream *xis = db.createLocalFileInputStream(localFile);
    TEST_MSG(log, "Created a stream on the file '%1'") << localFile;

    streamDocument(log, xis, db);
  }
}

void testURL(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container, const std::string &baseId, const std::string &localFile) {
  if(localFile.length() == 0) {
    ERROR_MSG(log, "No local file specified!");
    return;
  }
  if(baseId.length() == 0) {
    ERROR_MSG(log, "No baseId specified!");
    return;
  }

  string systemId(localFile);

  // basic manipulation of a stream
  XmlInputStream *xis = db.createURLInputStream(baseId, systemId);
  TEST_MSG(log, "Created a stream using base ID '%1' and system ID '%2'") << baseId << systemId;

  basicStreamManipulation(log, xis);

  delete xis;

  // load a document from the input stream
  {
    XmlInputStream *xis = db.createURLInputStream(baseId, systemId);
    TEST_MSG(log, "Created a stream using base ID '%1' and system ID '%2'") << baseId << systemId;

    XmlTransaction *txn = 0;
    if(transacted) {
      txn = new XmlTransaction(db.createTransaction());
    }

    XmlUpdateContext uc = db.createUpdateContext();
    loadDocument(log, xis, container, uc, *txn, transacted);

    if(transacted) {
      txn->commit();
      delete txn;
    }

    // container has adopted stream to put document - do not delete here
  }

  // use the input stream to read a document
  {
    XmlInputStream *xis = db.createURLInputStream(baseId, systemId);
    TEST_MSG(log, "Created a stream using base ID '%1' and system ID '%2'") << baseId << systemId;

    streamDocument(log, xis, db);
  }

  // TODO - use a public ID

  {
    // fail case - file not existing is tested in Tcl code
  }
}

void testMemoryBuffer(TestLogger &log, bool transacted, XmlManager &db, XmlContainer *container) {

  // basic manipulation of a stream
  const char *buf = "\
<?xml version='1.0' encoding='UTF-8'?> \
<foobar> \
  <bar att='hello'>barfoobar</bar> \
</foobar> \
";
  const char *bufId = "TESTBUF";
  bool adoptBuffer(false);
  XmlInputStream *xis = db.createMemBufInputStream(buf, (unsigned int)::strlen(buf),
						   bufId, adoptBuffer);

  TEST_MSG(log, "Created a stream on a memory buffer of length %1") <<
	  (unsigned int)::strlen(buf);

  basicStreamManipulation(log, xis);

  delete xis;

  // load a document from the input stream
  {
    XmlInputStream *xis = db.createMemBufInputStream(buf, (unsigned int)::strlen(buf), bufId, adoptBuffer);
    TEST_MSG(log, "Created a stream on a memory buffer of length %1") <<
	    (unsigned int)::strlen(buf);

    XmlTransaction *txn = 0;
    if(transacted) {
      txn = new XmlTransaction(db.createTransaction());
    }

    XmlUpdateContext uc = db.createUpdateContext();
    loadDocument(log, xis, container, uc, *txn, transacted);

    if(transacted) {
      txn->commit();
      delete txn;
    }

    // container has adopted stream to put document - do not delete here
  }

  // use the input stream to read a document
  {
    XmlInputStream *xis = db.createMemBufInputStream(buf, (unsigned int)::strlen(buf), bufId, adoptBuffer);
    TEST_MSG(log, "Created a stream on a memory buffer of length %1") <<
	    (unsigned int)::strlen(buf);

    streamDocument(log, xis, db);
  }
}

// basic manipulation of a stream - get position, read, get position
void basicStreamManipulation(TestLogger &log, XmlInputStream *xis)
{
   unsigned int pos = xis->curPos();
   if(pos == 0) {
     TEST_MSG(log, "Initial position is 0");
   } else {
     ERROR_MSG(log, "Initial position should be 0 but is %1") << pos;
   }
   char buf[1024];
   unsigned int count = xis->readBytes(buf, 1023);
   if(count > 0) {
     TEST_MSG(log, "Read %1 bytes") << count;
   } else {
     ERROR_MSG(log, "Could not read from the stream");
   }
   pos = xis->curPos();
   if(pos > 0) {
     TEST_MSG(log, "Current position is %1") << pos;
   } else {
     ERROR_MSG(log, "Position still stuck at 0");
   }
}

// use a stream to load a document into a container
void loadDocument(TestLogger &log, XmlInputStream *xis, XmlContainer *container, XmlUpdateContext &uc, XmlTransaction &txn, bool transacted)
{
  string name("mydoc");
  string given_name;
  if(transacted) {
    given_name.assign(container->putDocument(txn, name, xis, uc));
  } else {
    given_name.assign(container->putDocument(name, xis, uc));
  }

  TEST_MSG(log, "Loaded document, given name is '%1'") << given_name;

  // sanity check
  XmlDocument *doc = 0;
  if(transacted) {
    doc = new XmlDocument(container->getDocument(txn, given_name));
  } else {
    doc = new XmlDocument(container->getDocument(given_name));
  }
  string buf;
  unsigned int len = (unsigned int)doc->getContent(buf).length();
  TEST_MSG(log, "Got document, has string length of %1") << len;
  delete doc;
}

// stream a document directly
void streamDocument(TestLogger &log, XmlInputStream *xis, XmlManager &db)
{
  // stream content as an XmlData
  XmlDocument doc = db.createDocument();
  doc.setName("foo");
  doc.setContentAsXmlInputStream(xis);
  TEST_MSG(log, "Created a document and attached the input stream");

  XmlData content = doc.getContent();
  unsigned int count = (unsigned int)content.get_size();
  if(count != 0) {
    TEST_MSG(log, "Streamed an XmlData object of size %1") << count;
  } else {
    ERROR_MSG(log, "Unable to stream document? XmlData is empty");
  }

  if(count < 1024) {
    char buf[1024];
    strncpy(buf, (char*)content.get_data(), count);
    buf[count] = '\0';
    ostringstream os;
    os << "Content streamed is:\n" << buf  << "\n";
    TEST_MSG(log, os.str());
  } else {
    WARNING_MSG(log, "Not looking at content - buffer too small");
  }
}
