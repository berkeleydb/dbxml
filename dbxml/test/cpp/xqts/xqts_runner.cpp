/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1998,2009 Oracle.  All rights reserved.
 *
 */

#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include <dbxml/DbXml.hpp>
#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

#include <xqilla/xqts/TestSuiteParser.hpp>
#include <xqilla/xqts/TestSuiteResultListener.hpp>
#include <xqilla/xqts/TestSuiteRunner.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/utils/XStr.hpp>

#include <xercesc/util/Janitor.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const string TEST_URI_SCHEME = "foo://";

class DbXmlTestSuiteRunner : public TestSuiteRunner, private XmlResolver
{
public:
	DbXmlTestSuiteRunner(XmlManager mgr, std::string containerName, bool transacted,
		bool nodeStorage, bool nodeIndexes, bool projection, bool verbose,
		const string &singleTest, TestSuiteResultListener *results);

	virtual void unknownElement(const string &name, map<string, string> &attrs);

	virtual void addSource(const string &id, const string &filename, const string &schema);
	virtual void addSchema(const string &id, const string &filename, const string &uri);
	virtual void addModule(const string &id, const string &filename);
	virtual void addCollection(const string &id);
	virtual void addCollectionDoc(const string &id, const string &filename);

	virtual void startTestGroup(const string &name);
	virtual void endTestGroup();

	virtual void runTestCase(const TestCase &testCase);

private:
	virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
		const string &schemaLocation, const string &nameSpace) const;
	virtual bool resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
		const string &nameSpace, XmlResults &result) const;
	virtual XmlInputStream *resolveModule(XmlTransaction *txn, XmlManager &mgr,
		const string &moduleLocation, const string &nameSpace) const;
	virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
		const string &uri, XmlResults &result) const;
	virtual bool resolveDocument(XmlTransaction *txn, XmlManager &mgr,
		const string &uri, XmlValue &result) const;

	void revertSource(const string &id);

private:
	mutable DbXml::XmlContainer container_;
	bool projection_;
	bool verbose_;
	bool transacted_;
	
	string singleTest_;
	string fullTestName_;
	const TestCase* testCase_;

	// id -> filename
	map<string, string> inputFiles_;
	// id -> filename
	map<string, string> moduleFiles_;

	XmlContainer currentCollection_;
	map<string, DbXml::XmlContainer> collections_;
};

static void usage(const string &progname, int exitCode)
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

	cerr << "Usage: " << progname.substr(pos) << " [OPTIONS] <path_to_test_suite> (<specific test>)?" << endl;
	cerr << endl;
	cerr << "Options:" << endl;
	cerr << "-C <cachesize>\tset the cache size to the specified value, in Mb. (default: 64)" << endl;
	cerr << "-c <container>\tuse the specified container name. (default: xqts)" << endl;
	cerr << "-e <file>\tuse the given file as a known error file" << endl;
	cerr << "-E <file>\toutput an error file" << endl;
	cerr << "-h <home>\tuse specified directory as a Berkeley DB environment. (default: .)" << endl;
	cerr << "-p\t\tModify queries to use fn:doc() for input variables. Facilitates testing document projection. (default: off)" << endl;
	cerr << "-s <storage>\tuse the specified storage model, including DLS, DLS+, NLS and NLS+. (default: all)" << endl;
	cerr << "-V\t\tprint software version" << endl;
	cerr << "-v\t\tverbose. Using the switch a second time increases verbosity" << endl;
	cerr << "-x\t\toutput results as XML" << endl;
	cerr << "-?\t\thelp -- print this message" << endl;
	cerr << endl;
	exit(exitCode);
}

static inline void addIndex(XmlContainer &container, const std::string &uri, const std::string &name, const std::string &index)
{
	if(name == "") {
		XmlIndexSpecification is = container.getIndexSpecification();
		if(is.getDefaultIndex().find(index) == string::npos) {
			XmlUpdateContext uc = container.getManager().createUpdateContext();
			container.addDefaultIndex(index, uc);
		}
	}
	else {
		XmlIndexSpecification is = container.getIndexSpecification();
		string foundindex;
		if(!is.find(uri, name, foundindex) || foundindex.find(index) == string::npos) {
			XmlUpdateContext uc = container.getManager().createUpdateContext();
			container.addIndex(uri, name, index, uc);
		}
	}
}

int
main(int argc, char **argv)
{
	const char *progName = argv[0];

	int verbose = 0;
	int cacheSize = 64 * 1024 * 1024;
	bool allStorageModes = true;
	bool nodeStorage = true;
	bool nodeIndexes = true;
	string envDir = ".";
	string containerName = "xqts";
	string errorFile;
	string outputErrorFile;
	bool xmlResults = false;
	bool projection = false;
	bool transacted = false;

	int ch;
	while ((ch = getopt(argc, argv, "?vVh:s:C:c:E:e:xpt")) != -1) {
		switch (ch) {
		case 'C': {
			cacheSize = atoi(optarg) * 1024 * 1024;
			break;
		}
		case 'v': {
			++verbose;
			break;
		}
		case 'V': {
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", db_version(NULL, NULL, NULL));
			exit(0);
		}
		case 's': {
			string storage = optarg;
			if(storage == "DLS") {
				nodeStorage = false;
				nodeIndexes = false;
			}
			else if(storage == "DLS+") {
				nodeStorage = false;
				nodeIndexes = true;
			}
			else if(storage == "NLS") {
				nodeStorage = true;
				nodeIndexes = false;
			}
			else if(storage == "NLS+") {
				nodeStorage = true;
				nodeIndexes = true;
			}
			else {
				printf("Unknown storage type: %s\n", storage.c_str());
				usage(progName, 1);
			}
			allStorageModes = false;
			break;
		}
		case 'h': {
			envDir = optarg;
			break;
		}
		case 'c': {
			containerName = optarg;
			break;
		}
		case 'e': {
			errorFile = optarg;
			break;
		}
		case 'E': {
			outputErrorFile = optarg;
			break;
		}
		case 'x': {
			xmlResults = true;
			break;
		}
		case 'p': {
			projection = true;
			break;
		}
		case 't': {
			transacted = true;
			break;
		}
		case '?': {
			usage(progName, 0);
			break;
		}
		default: {
			usage(progName, 1);
			break;
		}
		}
	}

	if(argc - optind > 2) {
		printf("Too many arguments\n");
		usage(progName, 1);
	}

	if(argc == optind) {
		printf("Path to the XQuery Test Suite not specified\n");
		usage(progName, 1);
	}

	string testCatalog = string(argv[optind]);
	string singleTest;
	if(argc - optind > 1)
		singleTest = argv[optind + 1];

	// Turn on logging if extra verbose is specified
	if(verbose > 1) {
		setLogLevel(LEVEL_ALL, true);
		setLogCategory(CATEGORY_ALL, true);
		setLogCategory(CATEGORY_NODESTORE, verbose > 2);
	}

	bool passed = true;
	XQillaPlatformUtils::initialize();
	DB_ENV *env = 0;
	try {
		u_int32_t envFlags = DB_CREATE|DB_INIT_MPOOL;
		if(transacted) envFlags |= DB_INIT_TXN|DB_INIT_LOCK|DB_INIT_LOG;
		
		db_env_create(&env, 0);
		env->set_errfile(env, stderr);
		env->set_cachesize(env, 0, cacheSize, 1);
		env->set_lk_max_lockers(env, 50000);
		env->set_lk_max_locks(env, 50000);
		env->set_lk_max_objects(env, 50000);
		int err = env->open(env, envDir.c_str(), envFlags, 0);
		if (err) {
			cerr << "Could not open environment, " <<
				envDir.c_str() << ", errno: " << err << endl;
			throw XmlException(err);
		}

		// Set the numeric precision to the specified minimum -
		// this will make more tests pass unaltered, since the
		// results are formatted using the minimum precision.
		XQillaPlatformUtils::enableExtendedPrecision(false);

		Janitor<TestSuiteResultListener> results(0);
		if(xmlResults) {
			results.reset(new XMLReportResultListener());
			XMLReportResultListener *xmlreport = (XMLReportResultListener*)results.get();
			xmlreport->setImplementation("Oracle Berkeley DB XML", DbXml::dbxml_version(NULL, NULL, NULL));
			xmlreport->setOrganization("Oracle", "http://www.oracle.com");
			xmlreport->setSubmittor("John Snelson", "john.snelson@oracle.com");
			// xmlreport->setAnonymous(true);

			xmlreport->addImplementationDefinedItem("expressionUnicode", "UTF-8");
			xmlreport->addImplementationDefinedItem("implicitTimezone", "Defined by the system clock");
			xmlreport->addImplementationDefinedItem("XMLVersion", "1.0");
			xmlreport->addImplementationDefinedItem("axes", "Full axis support");
			xmlreport->addImplementationDefinedItem("defaultOrderEmpty", "empty least");
			xmlreport->addImplementationDefinedItem("normalizationForms", "NFC, NFD, NFKC, NFKD");
			xmlreport->addImplementationDefinedItem("docProcessing", "dtd validation");

			xmlreport->addFeature("Minimal Conformance", true);
			xmlreport->addFeature("Schema Import", true);
			xmlreport->addFeature("Schema Validation", false);
			xmlreport->addFeature("Static Typing", false);
			xmlreport->addFeature("Static Typing Extensions", false);
			xmlreport->addFeature("Full Axis", true);
			xmlreport->addFeature("Module", true);
			xmlreport->addFeature("Serialization", false);
			xmlreport->addFeature("Trivial XML Embedding", false);
		}
		else {
			results.reset(new ConsoleResultListener());
		}

		KnownErrorChecker knownErrors(results.get());
		if(errorFile != "" && !knownErrors.loadErrors(errorFile)) {
			throw XmlException(XmlException::INTERNAL_ERROR,
					   "problem loading known errors");
		}

		if(allStorageModes || (nodeStorage && nodeIndexes)) {
			XmlManager mgr(env, DBXML_ALLOW_EXTERNAL_ACCESS);
			// NLS+
			DbXmlTestSuiteRunner runner(mgr, containerName, transacted, /*nodeStorage*/true, /*nodeIndexes*/true, projection, verbose, singleTest, &knownErrors);
			TestSuiteParser parser(testCatalog, &runner);
			parser.handleUnknownElement("index");
			parser.handleUnknownElement("auto-index");

			runner.startTestGroup("NLS+");
			parser.run();
			runner.endTestGroup();
		}

		if(allStorageModes || (nodeStorage && !nodeIndexes)) {
			XmlManager mgr(env, DBXML_ALLOW_EXTERNAL_ACCESS);
			// NLS
			DbXmlTestSuiteRunner runner(mgr, containerName, transacted, /*nodeStorage*/true, /*nodeIndexes*/false, projection, verbose, singleTest, &knownErrors);
			TestSuiteParser parser(testCatalog, &runner);
			parser.handleUnknownElement("index");
			parser.handleUnknownElement("auto-index");

			runner.startTestGroup("NLS");
			parser.run();
			runner.endTestGroup();
		}

		if(allStorageModes || (!nodeStorage && nodeIndexes)) {
			XmlManager mgr(env, DBXML_ALLOW_EXTERNAL_ACCESS);
			// DLS+
			DbXmlTestSuiteRunner runner(mgr, containerName, transacted, /*nodeStorage*/false, /*nodeIndexes*/true, projection, verbose, singleTest, &knownErrors);
			TestSuiteParser parser(testCatalog, &runner);
			parser.handleUnknownElement("index");
			parser.handleUnknownElement("auto-index");

			runner.startTestGroup("DLS+");
			parser.run();
			runner.endTestGroup();
		}

		if(allStorageModes || (!nodeStorage && !nodeIndexes)) {
			XmlManager mgr(env, DBXML_ALLOW_EXTERNAL_ACCESS);
			// DLS
			DbXmlTestSuiteRunner runner(mgr, containerName, transacted, /*nodeStorage*/false, /*nodeIndexes*/false, projection, verbose, singleTest, &knownErrors);
			TestSuiteParser parser(testCatalog, &runner);
			parser.handleUnknownElement("index");
			parser.handleUnknownElement("auto-index");

			runner.startTestGroup("DLS");
			parser.run();
			runner.endTestGroup();
		}

		if(xmlResults) {
			((XMLReportResultListener*)results.get())->printReport();
		}
		else {
			passed = ((ConsoleResultListener*)results.get())->printReport();
		}

		if(errorFile != "") {
			passed = knownErrors.printReport();
		}

		if(outputErrorFile != "" && !knownErrors.saveErrors(outputErrorFile)) {
			throw XmlException(XmlException::INTERNAL_ERROR,
					   "problem opening saved errors file");
		}

	} catch (XmlException &xe) {
		cerr << "XmlException: " << xe.what() << endl;
		passed = false;
	} catch (...) {
		cerr << "Unknown exception" << endl;
		passed = false;
	}
	// note that this isn't called in error cases; I can live
	// with that.
	if (env)
		env->close(env, 0);
	XQillaPlatformUtils::terminate();
	return passed ? 0 : 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlTestSuiteRunner::DbXmlTestSuiteRunner(XmlManager mgr, std::string containerName, bool transacted,
	bool nodeStorage, bool nodeIndexes, bool projection, bool verbose,
	const string &singleTest, TestSuiteResultListener *results)
	: TestSuiteRunner(results),
	  container_(),
	  projection_(projection),
	  verbose_(verbose),
	  transacted_(transacted),
	  singleTest_(singleTest),
	  testCase_(0),
	  currentCollection_()
{
	if(nodeStorage) containerName += "_nls";
	else containerName += "_dls";

	if(nodeIndexes) containerName += "+";

	int cflags = DB_CREATE;

	if(transacted) cflags |= DBXML_TRANSACTIONAL;

	if(nodeIndexes) cflags |= DBXML_INDEX_NODES;
	else cflags |= DBXML_NO_INDEX_NODES;

	container_ = mgr.openContainer(containerName,
		cflags, (nodeStorage ? XmlContainer::NodeContainer :
			XmlContainer::WholedocContainer), 0);
	container_.addAlias("xqts.dbxml");

	addIndex(container_, "", "", "node-element-equality-string");
	addIndex(container_, "", "", "node-attribute-equality-string");
	addIndex(container_, "", "", "node-element-equality-double");
	addIndex(container_, "", "", "node-attribute-equality-double");
	addIndex(container_, "", "", "edge-element-presence");
	addIndex(container_, "", "", "edge-attribute-presence");
	addIndex(container_, "", "description", "node-element-substring-string");
	addIndex(container_, "", "title", "node-element-substring-string");

	container_.getManager().registerResolver(*this);
	currentCollection_ = container_;
}

void DbXmlTestSuiteRunner::startTestGroup(const string &name)
{
	if(fullTestName_ != "")
		fullTestName_ += ":";
	fullTestName_ += name;

	m_results->startTestGroup(name);
}

void DbXmlTestSuiteRunner::endTestGroup()
{
	int nColonPos = fullTestName_.find_last_of(":");
	if(nColonPos != -1)
		fullTestName_ = string(fullTestName_.c_str(), nColonPos);
	else fullTestName_ = "";

	m_results->endTestGroup();
}

void DbXmlTestSuiteRunner::addSource(const string &id, const string &filename, const string &schema)
{
	inputFiles_[id] = filename;

	XmlManager mgr = container_.getManager();
	XmlDocument doc = mgr.createDocument();
	doc.setName(id);
	doc.setContentAsXmlInputStream(mgr.createURLInputStream("", filename));
	doc.setMetaData("", "schema", XmlValue(schema));

	if(verbose_) {
		cout << "Adding source file: " << id << endl;
	}
	XmlUpdateContext uc = mgr.createUpdateContext();
	try {
		container_.putDocument(doc, uc);
	}
	catch(XmlException &xe) {
		if(xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			// It's already in there
		}
		else {
			cerr << "Problem with " << filename << endl;
			throw;
		}
	}
}

void DbXmlTestSuiteRunner::revertSource(const string &id)
{
	XmlManager mgr = container_.getManager();
	XmlUpdateContext uc = mgr.createUpdateContext();

	try {
		XmlDocument doc = container_.getDocument(id, DBXML_LAZY_DOCS);
		doc.setContentAsXmlInputStream(mgr.createURLInputStream("", inputFiles_[id]));

		container_.updateDocument(doc, uc);
	} catch(XmlException &ex) {
		if(ex.getExceptionCode() == XmlException::DOCUMENT_NOT_FOUND) {
			// Try re-adding the document
			XmlDocument doc = mgr.createDocument();
			doc.setName(id);
			doc.setContentAsXmlInputStream(mgr.createURLInputStream("", inputFiles_[id]));

			container_.putDocument(doc, uc);
		} else throw;
	}
}

void DbXmlTestSuiteRunner::addSchema(const string &id, const string &filename, const string &uri)
{
	XmlManager mgr = container_.getManager();
	XmlDocument doc = mgr.createDocument();
	doc.setName(id);
	doc.setContentAsXmlInputStream(mgr.createURLInputStream("", filename));
	doc.setMetaData("", "uri", XmlValue(uri));

	if(verbose_) {
		cout << "Adding schema file: " << id << endl;
	}
	XmlUpdateContext uc = mgr.createUpdateContext();
	try {
		container_.putDocument(doc, uc);
	}
	catch(XmlException &xe) {
		if(xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			// It's already in there
		}
		else {
			cerr << "Problem with " << filename << endl;
			throw;
		}
	}
}

void DbXmlTestSuiteRunner::addModule(const string &id, const string &filename)
{
	moduleFiles_[id] = filename;
}

void DbXmlTestSuiteRunner::addCollection(const string &id)
{
	string containerName = container_.getName() + "_" + id;

	XmlManager mgr = container_.getManager();
	currentCollection_ = mgr.openContainer(containerName,
		container_.getFlags(), container_.getContainerType(), 0);
	currentCollection_.addAlias(id);

	collections_[id] = currentCollection_;
}

void DbXmlTestSuiteRunner::unknownElement(const string &name, map<string, string> &attrs)
{
	if(name == "index") {
		addIndex(currentCollection_, attrs["uri"], attrs["name"], attrs["index"]);
	}
	else if(name == "auto-index") {
		XmlUpdateContext uc = currentCollection_.getManager().createUpdateContext();
		string foo = attrs["value"];
		currentCollection_.setAutoIndexing(foo == "on", uc);
	}
}

class DocumentInputStream : public XmlInputStream
{
public:
	DocumentInputStream(const XmlDocument &doc)
		: doc_(doc), stream_(doc.getContentAsXmlInputStream()) {}
	virtual ~DocumentInputStream() { delete stream_; }

	virtual unsigned int curPos () const { return stream_->curPos(); }
	virtual unsigned int readBytes (char *toFill, 
		const unsigned int maxToRead) { return stream_->readBytes(toFill, maxToRead); }
private:
	XmlDocument doc_;
	XmlInputStream *stream_;
};

void DbXmlTestSuiteRunner::addCollectionDoc(const string &id, const string &filename)
{
	try {
		XmlManager mgr = container_.getManager();
		XmlUpdateContext uc = mgr.createUpdateContext();

		XmlDocument doc = mgr.createDocument();
		doc.setName(filename);
		doc.setContentAsXmlInputStream(new DocumentInputStream(container_.getDocument(filename, DBXML_LAZY_DOCS)));

		collections_[id].putDocument(doc, uc);
	}
	catch(XmlException &xe) {
		if(xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			// It's already in there
		}
		else {
			cerr << "Problem with " << filename << endl;
			throw;
		}
	}
}

XmlInputStream *DbXmlTestSuiteRunner::resolveSchema(XmlTransaction *txn, XmlManager &mgr,
	const string &schemaLocation, const string &nameSpace) const
{
	// Resolve the schema to the one stored in our container
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults results = mgr.query(string("collection(\"xqts.dbxml\")[dbxml:metadata(\"uri\") = \"") +
		nameSpace + string("\"]"), qc);

	XmlValue val;
	if(results.next(val)) {
		return new DocumentInputStream(val.asDocument());
	}

	return 0;
}

bool DbXmlTestSuiteRunner::resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
	const string &nameSpace, XmlResults &result) const
{
	bool ret = false;
	list<pair<string, string> >::const_iterator i;
	for(i = testCase_->moduleFiles.begin(); i != testCase_->moduleFiles.end(); ++i) {
		if(i->first == nameSpace) {
			result.add(i->second);
			ret = true;
		}
	}
	return ret;
}

XmlInputStream *DbXmlTestSuiteRunner::resolveModule(XmlTransaction *txn, XmlManager &mgr,
	const string &moduleLocation, const string &nameSpace) const
{
	list<pair<string, string> >::const_iterator i;
	for(i = testCase_->moduleFiles.begin();
	    i != testCase_->moduleFiles.end(); ++i) {
		if(i->first == nameSpace && i->second == moduleLocation) {
			map<string, string>::const_iterator i2 =
				moduleFiles_.find(i->second);
			if(i2 != moduleFiles_.end()) {
				return mgr.createURLInputStream("", i2->second + ".xq");
			}
		}
	}

	return 0;
}

bool DbXmlTestSuiteRunner::resolveCollection(XmlTransaction *txn, XmlManager &mgr,
	const string &uri, XmlResults &result) const
{
	if(uri.find(TEST_URI_SCHEME) == 0) {
		string collectionname = uri.substr(TEST_URI_SCHEME.length());

		map<string, DbXml::XmlContainer>::const_iterator it = collections_.find(collectionname);
		if(it != collections_.end()) {
			if(txn == 0) result = ((XmlContainer&)it->second).getAllDocuments(DBXML_LAZY_DOCS);
			else result = ((XmlContainer&)it->second).getAllDocuments(*txn, DBXML_LAZY_DOCS);
			return true;
		}
	}
	return false;
}

bool DbXmlTestSuiteRunner::resolveDocument(XmlTransaction *txn, XmlManager &mgr,
	const string &uri, XmlValue &result) const
{
	if(uri.find(TEST_URI_SCHEME) == 0) {
		string docname = uri.substr(TEST_URI_SCHEME.length());
		if(txn == 0)
			result = container_.getDocument(docname, DBXML_LAZY_DOCS);
		else result = container_.getDocument(*txn, docname, DBXML_LAZY_DOCS);
		return true;
	}
	return false;
}

void characterEscape(const string &input, ostringstream &dest)
{
	for(string::const_iterator i = input.begin();
	    i != input.end(); ++i) {
		if(*i == '>') {
			dest << "&gt;";
		}
		else if(*i == '<') {
			dest << "&lt;";
		}
		else if(*i == '&') {
			dest << "&amp;";
		}
		else {
			dest << *i;
		}
	}
}

string findAndReplace(const string &input, const string &find, const string &replace)
{
        string result = input;

        string::size_type search_from = 0;
        string::size_type pos = 0;
        while((pos = result.find(find, search_from)) != string::npos) {
                // Only perform the substitution if this is the complete lexical token
                // (checked in a hacky, but adequate way).
                string::size_type pos_end = pos + find.length();
                if(result.size() > pos_end) {
                        if((result[pos_end] >= '0' && result[pos_end] <= '9') ||
                                (result[pos_end] >= 'A' && result[pos_end] <= 'Z') ||
                                (result[pos_end] >= 'a' && result[pos_end] <= 'z')) {
                                search_from = pos + 1;
                                continue;
                        }
                }
                                
                result.replace(pos, find.size(), replace);
        }

        return result;
}

void DbXmlTestSuiteRunner::runTestCase(const TestCase &testCase)
{
	if(singleTest_ != "" &&
		testCase.name.find(singleTest_) == string::npos &&
		fullTestName_.find(singleTest_) == string::npos) {
		m_results->reportSkip(testCase, "Not run");
		return;
	}

	if(fullTestName_.find("Optional:StaticTyping") != string::npos && !testCase.updateTest) {
		m_results->reportSkip(testCase, "Static typing not supported");
		return;
	}
	if(fullTestName_.find("Optional:SchemaValidation") != string::npos) {
		m_results->reportSkip(testCase, "Schema validation not supported");
		return;
	}
	if(fullTestName_.find("Optional:TrivialEmbedding") != string::npos) {
		m_results->reportSkip(testCase, "Trivial embedding not supported");
		return;
	}
	if(testCase.updateTest && fullTestName_.find("Validation") != string::npos) {
		m_results->reportSkip(testCase, "Revalidation not supported");
		return;
	}

	testCase_ = &testCase;

 	//cout << "Test" << (transacted_ ? "(txn): " : ":  ") << fullTestName_ << ": " << testCase.name << "("
	//    << testCase.stateTime << ")" << endl;
	try {
		XmlManager mgr = container_.getManager();

		string &query = const_cast<string&>(testCase.query);

		XmlQueryContext qc = mgr.createQueryContext();
		qc.setBaseURI("");
		qc.setNamespace("myfn", "http://www.w3.org/2005/xpath-functions");

		XmlTransaction txn;
		if (transacted_)
			txn = mgr.createTransaction();
		// Set the input variables
		map<string, string>::const_iterator v;
		for(v = testCase.inputVars.begin(); v != testCase.inputVars.end(); ++v) {
			if(testCase.updateTest && testCase.stateTime == 0) {
				// Revert the file back to it's original state, because
				// a previous update test may have modified it
				revertSource(v->second);
			}

                        if(!projection_ || v->first == "local:var") {
                                // Do local:var as a variable
				if (transacted_)
					qc.setVariableValue(v->first, container_.getDocument(
								    txn, v->second, DBXML_LAZY_DOCS));
				else
					qc.setVariableValue(v->first, container_.getDocument(
								    v->second, DBXML_LAZY_DOCS));
                        } else if(v->second == "emptydoc") {
                                query = findAndReplace(query,
                                        string("declare variable $") + v->first + " external;",
                                        string("declare variable $") + v->first + " := ();");
                        } else {
                                query = findAndReplace(query,
                                        string("declare variable $") + v->first + " external;",
                                        string("declare variable $") + v->first + " := myfn:doc(\"dbxml:///xqts.dbxml/" + v->second + "\");");
                        }
		}

		// Execute the auxilliary queries, and set up the external variable values
		for(v = testCase.extraVars.begin(); v != testCase.extraVars.end(); ++v) {
			Janitor<BinFileInputStream> stream((BinFileInputStream*)URLInputSource(0, X(v->second.c_str())).makeStream());
			unsigned int dwSize = stream->getSize();
			string extraQuery;
			extraQuery.resize(dwSize);
			stream->readBytes((XMLByte*)extraQuery.c_str(), dwSize);

			XmlResults value = mgr.query(extraQuery, qc);
			qc.setVariableValue(v->first, value);
		}

		// Set the URI variables
		for(v = testCase.inputURIVars.begin(); v != testCase.inputURIVars.end(); ++v) {
			qc.setVariableValue(v->first, TEST_URI_SCHEME + v->second);
		}

		// Set up the context item
		XmlDocument contextDocument;
		if(!testCase.contextItem.empty()) {
			if (transacted_)
				contextDocument =
					container_.getDocument(txn, testCase.contextItem, DBXML_LAZY_DOCS);
			else
				contextDocument =
					container_.getDocument(testCase.contextItem, DBXML_LAZY_DOCS);
		}

		// Set up the default collection
		if(!testCase.defaultCollection.empty()) {
			qc.setDefaultCollection("dbxml:/" + testCase.defaultCollection);
		}

		XmlQueryExpression expr = mgr.prepare(query, qc);
		XmlResults results;
		if (transacted_) {
			if(contextDocument.isNull())
				results = expr.execute(
					txn, qc, DBXML_LAZY_DOCS | DBXML_DOCUMENT_PROJECTION);
			else results = expr.execute(
				contextDocument, qc, DBXML_LAZY_DOCS | DBXML_DOCUMENT_PROJECTION);
		} else {
			if(contextDocument.isNull())
				results = expr.execute(
					txn, qc, DBXML_LAZY_DOCS | DBXML_DOCUMENT_PROJECTION);
			else results = expr.execute(
				txn, contextDocument, qc, DBXML_LAZY_DOCS | DBXML_DOCUMENT_PROJECTION);
		}

		// Emulate the XQuery serialization spec

		XmlResults tmpResults = mgr.createResults();
		XmlValue val;
		while(results.next(val)) {
			if(!val.isNode()) {
				tmpResults.add(val.asString());
			} else {
				tmpResults.add(val);
			}
		}

		static const char *serializeQuery = "declare copy-namespaces preserve, inherit; "
			"declare variable $results external; document { $results }";

		qc.setVariableValue("results", tmpResults);
		XmlResults serializedResults;
		if (transacted_)
			serializedResults = mgr.query(txn, serializeQuery, qc);
		else
			serializedResults = mgr.query(serializeQuery, qc);

		ostringstream output;
		while(serializedResults.next(val)) {
			string nodeValue = val.asString();

			// Remove the xml decl from a document node
			string::size_type start = nodeValue.find("<?xml");
			if(start == 0) {
				string::size_type end = nodeValue.find("?>\n");
				if(end == string::npos) {
					end = nodeValue.find("?>\r\n");
					if(end == string::npos) {
						end = nodeValue.find("?>\r");
						if(end == string::npos) {
							end = nodeValue.find("?>");
							if(end != string::npos)
								end += 2;
						}
						else end += 3;
					} else end += 4;
				} else end += 3;
				if(end != string::npos) {
					nodeValue.erase(start, end - start);
				}
			}

			output << nodeValue;
		}
		txn.commit(); // this works with empty txn
		
		testResults(testCase, output.str());
	}
	catch(XmlException &xe) {
		testErrors(testCase, xe.what());
	}
	catch(...) {
		testErrors(testCase, "[Unknown exception]");
	}

	testCase_ = 0;
}
