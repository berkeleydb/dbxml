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
#include <fstream>
#include <sstream>
#include <cassert>

// Xerces includes
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>

// XQilla includes
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/xqilla-simple.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>

// test code includes
#include "../util/TestLogging.hpp"
#include "../util/TestResolver.hpp"
#include "UnitTest.hpp"

#include "ResolverUnitTest.hpp"
#include "XQueryTest.hpp"
#include "QueryPlanTest.hpp"
//#include "UpdateDocumentTest.hpp"

#include "TestEnvironment.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;
XERCES_CPP_NAMESPACE_USE

// string literals (index.xml vocabulary)
namespace Literals
{
	const XMLCh base_str[] =
	{
			chLatin_b, chLatin_a, chLatin_s, chLatin_e, chNull
		};
	const XMLCh category_str[] =
		{
			chLatin_c, chLatin_a, chLatin_t, chLatin_e, chLatin_g, chLatin_o, chLatin_r, chLatin_y, chNull
		};
	const XMLCh document_str[] =
		{
			chLatin_d, chLatin_o, chLatin_c, chLatin_u, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chNull
		};
	const XMLCh id_str[] =
		{
			chLatin_i, chLatin_d, chNull
		};
	const XMLCh index_str[] =
		{
			chLatin_i, chLatin_n, chLatin_d, chLatin_e, chLatin_x, chNull
		};
	const XMLCh indexSpecification_str[] =
		{
			chLatin_i, chLatin_n, chLatin_d, chLatin_e, chLatin_x, chLatin_S, chLatin_p, chLatin_e, chLatin_c, chLatin_i, chLatin_f, chLatin_i, chLatin_c, chLatin_a, chLatin_t, chLatin_i, chLatin_o, chLatin_n, chNull
		};
	const XMLCh name_str[] =
		{
			chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull
		};
	const XMLCh node_str[] =
		{
			chLatin_n, chLatin_o, chLatin_d, chLatin_e, chNull
		};
	const XMLCh query_str[] =
		{
			chLatin_q, chLatin_u, chLatin_e, chLatin_r, chLatin_y, chNull
		};
	const XMLCh returnType_str[] =
		{
			chLatin_r, chLatin_e, chLatin_t, chLatin_u, chLatin_r, chLatin_n, chLatin_T, chLatin_y, chLatin_p, chLatin_e, chNull
		};
	const XMLCh test_str[] =
		{
			chLatin_t, chLatin_e, chLatin_s, chLatin_t, chNull
		};
	const XMLCh uri_str[] =
		{
			chLatin_u, chLatin_r, chLatin_i, chNull
		};
	const XMLCh value_str[] =
		{
			chLatin_v, chLatin_a, chLatin_l, chLatin_u, chLatin_e, chNull
		};
	const XMLCh variable_str[] =
		{
			chLatin_v, chLatin_a, chLatin_r, chLatin_i, chLatin_a, chLatin_b, chLatin_l, chLatin_e, chNull
		};
}

// Hack alert.  Use a single global variable for logging
// DB error messages.  This works for all tests currently
// being used.  The variable is set in the TestEnvironment constructor
TestLogger *glog;

static void errcall(const DB_ENV *dbenv, const char *errpfx, const char *msg)
{
	if (glog) {
		std::string error = "DB errcall: ";
		error += msg;
		ENV_MSG(*glog, error);
	}
}


// utility methods (internal)
namespace {
	// convert test type string to enum
	TestEnvironment::TestType convertTestType(const std::string &type)
	{
		TestEnvironment::TestType t = TestEnvironment::NONE;
		if(!type.compare("resolver"))
			t = TestEnvironment::RESOLVER;
		else if(!type.compare("xquery"))
			t = TestEnvironment::XQUERY;
		else if(!type.compare("queryPlan"))
			t = TestEnvironment::QUERY_PLAN;
		return t;
	}

	// convert return type from string to enum
	XmlQueryContext::ReturnType convertReturnType(const std::string &str)
	{
		// default to result docs - be tolerant
		XmlQueryContext::ReturnType rt = XmlQueryContext::LiveValues;
		return rt;
	}
}

TestEnvironment::TestEnvironment(TestLogger *log, const std::string &db_home,
	bool debug, bool transacted, bool wholeDocumentStorage, bool indexNodes,
	bool secure, bool validate)
	: log_(log),
	  db_(0),
	  container_(0),
	  context_(0),
	  uc_(0),
	  memoryManager_(0),
	  testDOM_(0),
	  transacted_(transacted),
	  storageModel_(wholeDocumentStorage ? XmlContainer::WholedocContainer
			: XmlContainer::NodeContainer),
	  flags_(indexNodes ? DBXML_INDEX_NODES : DBXML_NO_INDEX_NODES),
	  secure_(secure),
	  validate_(validate)
{
	glog = log;
	if(debug) {
		DbXml::setLogLevel(LEVEL_ALL, true);
		DbXml::setLogCategory(CATEGORY_ALL, true);
	}

	createDBXMLEnvironment(db_home);

	memoryManager_ = new XPath2MemoryManagerImpl;

	xqContext_ = XQilla::createContext(XQilla::XQUERY, &xercesConf_, memoryManager_);

	if(log_) ENV_MSG(*log_, "Test preparation completed");
}

TestEnvironment::~TestEnvironment()
{
	delete uc_;
	delete context_;
	delete container_;
	delete db_;
	delete memoryManager_;
}

// runs all tests that are grouped in the specified directory
//
// Parameters
//  id          - identifier for test
//  dir         - path to directory that contains the test data
//  container   - if specified then this container will be opened
//  args        - command line arguments passed on by caller
//
void TestEnvironment::run(const std::string &id, const std::string &dir,
			  const std::string &container,
			  const Iterator<std::string> &args)
{
	testId_ = id;
	dataDirectory_ = dir;

	// prerequisites
	if(testId_.empty() || dataDirectory_.empty()) {
		if(log_) ERROR_MSG(*log_,
				   "The test information is incomplete - have id '%1' and data directory '%2'")
			<< testId_ << dataDirectory_;
		throw std::exception();
	}
	openContainer(container);

	// ok - instruct the environment as to what we're about to do
	// The returned Sequence object is to hold onto the DOM tree
	// for the parsed index document
	Sequence indexDoc = parseTestInstructions();

	// add a resolver for external DTDs in the local folder
	TestResolver resolver;
	resolver.setDir(dataDirectory_);
	db_->registerResolver(resolver);
	if(log_) ENV_MSG(*log_, "Registered test resolver for local directory '%1'") << dataDirectory_;

	// set up the database for this group of tests
	addGroupIndexes();
	addGroupDocuments();
	addGroupContext();

	if(log_) ENV_MSG(*log_, "Context has a return type of '%1'") << context_->getReturnType();

	// run...
	UnitTest *test = 0;
	switch(getTestType()) {
	case TestEnvironment::RESOLVER:
		test = new ResolverUnitTest(*this, args);
		break;
	case TestEnvironment::XQUERY:
		test = new XQueryTest(*this, args);
		break;
	case TestEnvironment::QUERY_PLAN:
		test = new QueryPlanTest(*this, args);
		break;
	default:
		if(log_) ERROR_MSG(*log_, "Test type not recognised (check index file in '%1')") << dataDirectory_;
		break;
	}
	if(test) {
		test->prepare();
		if(log_) ENV_MSG(*log_, "Finished preparing test");

		test->execute();
		if(log_) ENV_MSG(*log_, "Finished executing test");

		delete test;
	}
}

XmlTransaction TestEnvironment::createTransaction()
{
	return db_->createTransaction();
}

void TestEnvironment::commitTransaction(XmlTransaction txn)
{
	txn.commit();
}

Sequence TestEnvironment::query(
	const std::string &query, DOMNode *contextNode)
{
	assert(testDOM_);
	DOMNode *n = contextNode ? contextNode : testDOM_->getDocumentElement();

	XQilla xqilla;
	XQQuery *parsedQuery = xqilla.parse(UTF8ToXMLCh(query).str(),
		xqContext_, 0, XQilla::NO_ADOPT_CONTEXT);

	// set node context and evaluate query
	xqContext_->setContextItem(xercesConf_.createNode(n, xqContext_));
	xqContext_->setContextPosition(1);
	xqContext_->setContextSize(1);
	Sequence result = parsedQuery->
		execute(xqContext_)->toSequence(xqContext_);
	return result;
}

// adds index specifications from a query result
void TestEnvironment::addIndexSpecifications(Sequence &result)
{
	// construct the index specification and add to the container
	XmlIndexSpecification indexSpec;
	if (result.getLength() == 0)
		return;
	for (Sequence::iterator it = result.begin(); it != result.end(); it++) {
		if((*it)->isNode()) {
			const DOMNode *domnode = (const DOMNode*)(*it)->getInterface(XercesConfiguration::gXerces);
			assert(domnode != 0);

			string uri, node, index;
			getIndexSpecification(domnode, uri, node, index);
			indexSpec.addIndex(uri, node, index);

			if(log_) ENV_MSG(*log_, "Added the index specification '%1' / '%2' / '%3'") << uri << node << index;
		}
	}
// don't transact this operation -- it reindexes the world
//	if(transacted_) {
//		XmlTransaction myTxn = db_->createTransaction();
//		container_->setIndexSpecification(myTxn, indexSpec, *uc_);
//		myTxn.commit();
//	} else {
	container_->setIndexSpecification(indexSpec, *uc_);
//	}
}

// adds variables from a query result
void TestEnvironment::addVariables(Sequence &result)
{
	if (result.getLength() == 0)
		return;
	for (Sequence::iterator it = result.begin(); it != result.end(); it++) {
		if((*it)->isNode()) {
			const DOMNode *domnode = (const DOMNode*)(*it)->getInterface(XercesConfiguration::gXerces);
			assert(domnode != 0);

			string name, value;
			getVariable(domnode, name, value);

			XmlValue v(value);
			context_->setVariableValue(name, v);

			if(log_) ENV_MSG(*log_, "Added the variable: '%1' -> '%2'") << name << value;
		}
	}
}

// gets query from a DOM node (reads contents of named file)
//
//  q              - query node
//  file           - name of file containing query (out)
//  query          - XQuery 1.0 expression (out)
//
// NOTE: could be made more general, i.e. read contents of a local file
//
void TestEnvironment::getQuery(const DOMNode *q, std::string &file, std::string &xquery) const
{
	// verification - node
	if (XMLString::compareString(Literals::query_str, q->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "Incompatible or missing query node");
		throw std::exception();
	}

	// get name of file
	const XMLCh* xstr = q->getFirstChild()->getNodeValue();
	file.assign(XMLChToUTF8(xstr).str());

	// read XQuery expression from file
	string path = dataDirectory_ + file;
	string content;
	ifstream ifs(path.c_str(), std::ios::in|std::ios::binary);
	if(!ifs) {
		if(log_) ERROR_MSG(*log_, "Could not open '%1' for reading") << path;
		throw std::exception();
	}
	while (!ifs.eof()) {
		string buf;
		getline(ifs, buf);
		buf += '\xA';
		content += buf;
	}
	ifs.close();

	xquery.assign(content);
}

// gets text content of the first child element with a give name, from a node
void TestEnvironment::getTextFromChildNode(DOMNode *node,
					   const std::string &name,
					   std::string &text)
{
	assert(node);

	string xpath = "./" + name;
	Sequence result = query(xpath, node);
	if(result.getLength() == 0) {
		if(log_) ERROR_MSG(*log_, "No child '%1' found") << name;
		throw std::exception();
	}
	Item::Ptr theNode = result.first();
	if(!theNode->isNode()) {
		if(log_) ERROR_MSG(*log_, "Child '%1' is not a node") << name;
		throw std::exception();
	}

	const DOMNode *n = (const DOMNode*)theNode->getInterface(XercesConfiguration::gXerces);
	assert(n != 0);

	if(n->getFirstChild()) {
		const XMLCh* xstr = n->getFirstChild()->getNodeValue();
		text.assign(XMLChToUTF8(xstr).str());
	} else {
		if(log_) ERROR_MSG(*log_, "The child '%1' does not have child content") << name;
		throw std::exception();
	}
}

// gets index specification from a DOM node
//
//  is             - index specification node
//  uri/node/index - index specification components (out)
//
// Logs an error and throws a std::exception upon error
//
void TestEnvironment::getIndexSpecification(const DOMNode *is, std::string &uri, std::string &node, std::string &index) const
{
	// verification - node
	if (XMLString::compareString(Literals::indexSpecification_str, is->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "Incompatible or missing indexSpecification node");
		throw std::exception();
	}

	DOMNode *n = is->getFirstChild();
	while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
		n = n->getNextSibling();

	// uri (optional)
	if(!XMLString::compareString(Literals::uri_str, n->getLocalName())) {
		if(n->getFirstChild()) {
			const XMLCh* xstr = n->getFirstChild()->getNodeValue();
			uri.assign(XMLChToUTF8(xstr).str());
		}

		// move on to the node
		n = n->getNextSibling();
		while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
			n = n->getNextSibling();
	}

	// node
	if(XMLString::compareString(Literals::node_str, n->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "An index must contain a 'node' child");
		throw std::exception();
	}
	const XMLCh* xstr = n->getFirstChild()->getNodeValue();
	node.assign(XMLChToUTF8(xstr).str());

	n = n->getNextSibling();
	while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
		n = n->getNextSibling();

	// index
	if(XMLString::compareString(Literals::index_str, n->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "An index must contain a 'index' child");
		throw std::exception();
	}
	xstr = n->getFirstChild()->getNodeValue();
	index.assign(XMLChToUTF8(xstr).str());
}

// gets variable from a DOM node
//
//  var        - variable node
//  name/value - variable components (out)
//
// Logs an error and throws a std::exception upon error
//
void TestEnvironment::getVariable(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *var, std::string &name, std::string &value) const
{
	// verification - node
	if (XMLString::compareString(Literals::variable_str, var->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "Incompatible or missing variable node");
		throw std::exception();
	}

	DOMNode *n = var->getFirstChild();
	while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
		n = n->getNextSibling();

	// name
	if(XMLString::compareString(Literals::name_str, n->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "A variable must contain a 'name' child");
		throw std::exception();
	}
	const XMLCh* xstr = n->getFirstChild()->getNodeValue();
	name.assign(XMLChToUTF8(xstr).str());

	n = n->getNextSibling();
	while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
		n = n->getNextSibling();

	// value
	if(XMLString::compareString(Literals::value_str, n->getLocalName())) {
		if(log_) ERROR_MSG(*log_, "A variable must contain a 'value' child");
		throw std::exception();
	}
	xstr = n->getFirstChild()->getNodeValue();
	value.assign(XMLChToUTF8(xstr).str());
}

// putFileInDocument
//
// utility method that pre-dated input streams
// TODO deprecate in favour of input streams
void TestEnvironment::putFileInDocument(const std::string &file, DbXml::XmlDocument &doc)
{
	string path = dataDirectory_ + file;

	// slurp the contents of the file into a string (\xA line breaks)
	string content;
	ifstream ifs(path.c_str(), std::ios::in|std::ios::binary);
	if(!ifs) {
		if(log_) ERROR_MSG(*log_, "Could not open '%1' for reading") << path;
		throw std::exception();
	}
	while (!ifs.eof()) {
		string buf;
		getline(ifs, buf);
		buf += '\xA';
		content += buf;
	}
	ifs.close();

	doc.setContent(content);

	// set name of document (attempt to strip off path information)
	string name(file);
	string::size_type pos = name.rfind("/");
	if(pos == string::npos)  // windows
		pos = name.rfind("\\");
	if(pos != string::npos)
		name.assign(file, pos+1, file.length());

	doc.setName(name);
}

XmlManager *&TestEnvironment::db()
{
	return db_;
}

XmlContainer *&TestEnvironment::container()
{
	return container_;
}

XmlQueryContext *&TestEnvironment::context()
{
	return context_;
}

XmlUpdateContext *&TestEnvironment::uc()
{
	return uc_;
}

DB_ENV *TestEnvironment::dbenv()
{
	return db_->getDB_ENV();
}

TestLogger *&TestEnvironment::log()
{
	return *&log_;
}

bool &TestEnvironment::secure()
{
	return secure_;
}

bool &TestEnvironment::validate()
{
	return validate_;
}

bool &TestEnvironment::transacted()
{
	return transacted_;
}

XmlContainer::ContainerType &TestEnvironment::storageModel()
{
	return storageModel_;
}

u_int32_t TestEnvironment::flags()
{
	return flags_;
}

std::string &TestEnvironment::id()
{
	return testId_;
}

std::string &TestEnvironment::dataDir()
{
	return dataDirectory_;
}

size_t TestEnvironment::documentCount() const
{
	return documents_.size();
}

// assumes all documents were loaded from local files
void TestEnvironment::reloadContainer(XmlTransaction *xtxn)
{
	bool transacted = (xtxn != 0);
	map<string, string>::const_iterator i;
	for(i = documents_.begin(); i != documents_.end(); ++i) {
		if(transacted) {
			container_->deleteDocument(*xtxn, i->first, *uc_);
		} else {
			container_->deleteDocument(i->first, *uc_);
		}
		if(log_) ENV_MSG(*log_, "(Reset) Deleted document '%1'") << i->first;

		XmlDocument xd(db_->createDocument());
		putFileInDocument(i->second, xd);

		if(transacted) {
			container_->putDocument(*xtxn, xd, *uc_);
		} else {
			container_->putDocument(xd, *uc_);
		}
		if(log_) ENV_MSG(*log_, "(Reset) Put document '%1' from file '%2'") << i->first << i->second;
	}

	// everything worked if reached here - leave state of documents_ unchanged
}

void TestEnvironment::createDBXMLEnvironment(const std::string &db_home)
{
	// create the environment and manager
	DB_ENV *env = 0;
	db_env_create(&env, 0);
	u_int32_t dbxmlFlags = DBXML_ADOPT_DBENV;
	int dberr = 0;
	u_int32_t dbFlags = (DB_CREATE|DB_INIT_MPOOL);
	if(transacted_) dbFlags |= (DB_INIT_TXN|DB_INIT_LOCK|DB_INIT_LOG);
	if(!secure_) dbxmlFlags |= DBXML_ALLOW_EXTERNAL_ACCESS;
	env->set_cachesize(env, 0, 64 * 1024 * 1024, 1);
	env->set_errfile(env, stderr);
	env->set_errcall(env, errcall);
	env->set_lk_max_lockers(env, 50000);
	env->set_lk_max_locks(env, 50000);
	env->set_lk_max_objects(env, 50000);
	// will join existing env by default
	if(log_) ENV_MSG(*log_, "Opening environment");
	dberr = env->open(env, db_home.c_str(), dbFlags, 0);
	if (dberr) {
		if(log_) ENV_MSG(*log_, "Failed to open environment");
		throw;
	}
	db_ = new XmlManager(env, dbxmlFlags);
	db_->setDefaultContainerType(storageModel_);
	db_->setDefaultContainerFlags(flags_);

	// create other bits and pieces for the environment
	context_ = new XmlQueryContext(db_->createQueryContext());
	uc_ = new XmlUpdateContext(db_->createUpdateContext());
}

Sequence TestEnvironment::parseTestInstructions()
{
	assert(!testDOM_);

	// open the "index.xml" file (test instructions)
	string indexFile = dataDirectory_  + "/index.xml";
	if(log_) ENV_MSG(*log_, "Reading index file '%1'") << indexFile;

	UTF8ToXMLCh ifile(indexFile.c_str());
	Sequence dseq = xqContext_->resolveDocument(ifile.str(), 0);

	const DOMNode *dnode = (const DOMNode*)dseq.first()->getInterface(XercesConfiguration::gXerces);
	assert(dnode != 0);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *document =
		(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *)dnode;

	if (!document || !document->getDocumentElement()) {
		if(log_) ERROR_MSG(*log_, "Unable to parse (no document element found)");
		throw std::exception();
	}

	testDOM_ = document;

	// verification - root element
	if (XMLString::compareString(Literals::test_str,
				     testDOM_->getDocumentElement()->
				     getLocalName())) {
		if(log_) ERROR_MSG(*log_,
				   "Incompatible or missing 'test' root element");
		throw std::exception();
	}

	// verification - test id match (non fatal)
	DOMAttr *attr = testDOM_->getDocumentElement()->
		getAttributeNode(Literals::id_str);
	if(attr) {
		const XMLCh *xstr = attr->getValue();
		string id(XMLChToUTF8(xstr).str());
		if(testId_.compare(id)) {
			if(log_) WARNING_MSG(*log_, "Mismatched test IDs - invoked with '%1' but found '%2' in index file") << testId_ << id;
		}
	} else {
		if(log_) WARNING_MSG(*log_, "Mismatched test IDs - invoked with '%1' but none found in index file") << testId_;
	}

	return dseq;
}

// name - if specified then open this container, otherwise create
void TestEnvironment::openContainer(const std::string &name)
{
	string containerName(name);
	if(name.empty()) containerName = testId_ + ".dbxml";

	u_int32_t oflags = flags_;
	if(name.empty())
		oflags |= DB_CREATE;
	if (validate_)
		oflags |= DBXML_ALLOW_VALIDATION;

	if(transacted_) {
		XmlTransaction myTxn = db_->createTransaction();
		container_ = new XmlContainer(db_->openContainer(myTxn, containerName, oflags));
		myTxn.commit();
	} else {
		container_ = new XmlContainer(db_->openContainer(containerName, oflags));
	}
	context_->setDefaultCollection(containerName);

	ostringstream os;
	os << "Opened the container '" << container_->getName() << "'";
	os << " (";
	switch (container_->getContainerType()) {
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
	if(log_) ENV_MSG(*log_, os.str());
}

// add context information that applies to a test group
void TestEnvironment::addGroupContext()
{
	assert(testDOM_);
	assert(container_);

	// find optional top-level context element
	Sequence result = query("//test/context");
	if(result.getLength() == 0) return;

	const DOMNode *context = (const DOMNode*)result.first()->getInterface(XercesConfiguration::gXerces);
	assert(context != 0);

	// return type (optional)
	DOMNode *n = context->getFirstChild();
	while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
		n = n->getNextSibling();

	if(!XMLString::compareString(Literals::returnType_str,
				     n->getLocalName())) {
		string returnType;
		if(n->getFirstChild()) {
			const XMLCh* xstr = n->getFirstChild()->getNodeValue();
			returnType.assign(XMLChToUTF8(xstr).str());
		}

		XmlQueryContext::ReturnType rt = convertReturnType(returnType);
		context_->setReturnType(rt);

		if(log_) ENV_MSG(*log_, "Setting return type to '%1' (%2)") << returnType << rt;

		// move on to the next element
		n = n->getNextSibling();
		while (n && n->getNodeType() != DOMNode::ELEMENT_NODE)
			n = n->getNextSibling();
	}
}

// add documents that apply to a test group (assume local files)
void TestEnvironment::addGroupDocuments()
{
	assert(testDOM_);
	assert(container_);

	// iterate over top-level document instructions
	Sequence result = query("//test/document");
	if(result.getLength() == 0) return;

	for (Sequence::iterator it = result.begin(); it != result.end(); it++) {
		if((*it)->isNode()) {
			const DOMNode *domnode = (const DOMNode*)(*it)->getInterface(XercesConfiguration::gXerces);
			assert(domnode != 0);

			const XMLCh *xstr = domnode->getFirstChild()->getNodeValue();

			// create a document from the contents and
			// add to container
			XmlDocument xd(db_->createDocument());
			string file(XMLChToUTF8(xstr).str());
			putFileInDocument(file, xd);

			if(transacted_) {
				XmlTransaction myTxn = db_->createTransaction();
				container_->putDocument(myTxn, xd, *uc_);
				myTxn.commit();
			} else {
				container_->putDocument(xd, *uc_);
			}

			// cache the local file name (allows clients to reload)
			documents_[xd.getName()] = file;

			if(log_) ENV_MSG(*log_, "Added the document '%1' (read from '%2'") << xd.getName() << file;
		}
	}
}

// add index specifications that apply to a test group
void TestEnvironment::addGroupIndexes()
{
	Sequence result = query("//test/indexSpecification");
	addIndexSpecifications(result);
}

TestEnvironment::TestType TestEnvironment::getTestType()
{
	TestType type(NONE);

	Sequence result = query("//test/category");
	const DOMNode *n = (const DOMNode*)result.first()->getInterface(XercesConfiguration::gXerces);
	assert(n != 0);

	if(n->getFirstChild()) {
			const XMLCh* xstr = n->getFirstChild()->getNodeValue();
			type = convertTestType(XMLChToUTF8(xstr).str());
		}
	return type;
}

