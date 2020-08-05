//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __TESTENVIRONMENT_HPP
#define __TESTENVIRONMENT_HPP

#include <map>
#include <string>
#include <vector>

#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/xerces/XercesConfiguration.hpp>
#include <xercesc/dom/DOM.hpp>
#include "dbxml/DbXml.hpp"

class XPath2MemoryManager;
class DynamicContext;

namespace DbXmlTest {

class TestLogger;
	
// Java style Iterator interfaces
template<class T> class Iterator {
public:
	Iterator();
	Iterator(const std::vector<T>& v) : vector_(v) { iter_ = vector_.begin(); }
	std::vector<T> clone() const;
	bool hasNext() const { return iter_ != vector_.end(); }
	const T& next() const { return *(iter_++); }
	size_t size() const { return vector_.size(); }
	const T& operator[](size_t i) const { return vector_[i]; }
	void reset() const { iter_ = vector_.begin(); }
	
protected:
	const std::vector<T> & vector_;
	mutable typename std::vector<T>::const_iterator iter_;
	
private:
	static std::vector<T> emptyVector_;
};
	
#define EMPTY(T) Iterator<T>()
	
class TestEnvironment {
public:
	enum TestType {
		NONE,
		INDEXER,
		RESOLVER,
		XQUERY,
		QUERY_PLAN
	};
	
	TestEnvironment(TestLogger *log, const std::string &db_home,
		bool debug = false, bool transacted = true,
		bool wholeDocumentStorage = true, bool indexNodes = false,
		bool secure = false, bool validate = false);
	virtual ~TestEnvironment();
	
	void run(const std::string &id, const std::string &dir,
		 const std::string &container = "",
		 const Iterator<std::string> &args = EMPTY(std::string));
	
	// helper methods for test classes
	DbXml::XmlTransaction createTransaction();
	void commitTransaction(DbXml::XmlTransaction txn);
	
	Sequence query(const std::string &query,
		       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
		       *contextNode = 0);
	
	void addIndexSpecifications(Sequence &result);
	void addVariables(Sequence &result);
	
	////
	void getTextFromChildNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
				  const std::string &child, std::string &text);
	///
	
	void getIndexSpecification(
		const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *is,
		std::string &uri, std::string &node, std::string &index) const;
	void getQuery(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *q,
		      std::string &file, std::string &xquery) const;
	void getVariable(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *var,
			 std::string &name, std::string &value) const;
	
	void putFileInDocument(const std::string &file,
			       DbXml::XmlDocument &doc);
	
	// expose external objects
	const DynamicContext *xqContext() const { return xqContext_; }
	TestLogger *&log();
	DbXml::XmlManager *&db();
	DbXml::XmlQueryContext *&context();
	DbXml::XmlUpdateContext *&uc();
	DbXml::XmlContainer *&container();
	DB_ENV *dbenv();
	
	// expose configuration settings for the test environment
	bool &transacted();
	DbXml::XmlContainer::ContainerType &storageModel();
	u_int32_t flags();
	bool &secure();
	bool &validate();
	std::string &id();
	std::string &dataDir();
	
	// test infrastructure information - loaded documents
	size_t documentCount() const;
	void reloadContainer(DbXml::XmlTransaction *xtxn = 0);
	
private:
	TestEnvironment(const TestEnvironment &);
	TestEnvironment &operator=(const TestEnvironment &);
	
	void createDBXMLEnvironment(const std::string &db_home);
	Sequence parseTestInstructions();
	void openContainer(const std::string &name);
	void addGroupContext();
	void addGroupDocuments();
	void addGroupIndexes();
	
	TestType getTestType();
	
	TestLogger *log_;
	DbXml::XmlManager *db_;
	DbXml::XmlContainer *container_;
	DbXml::XmlQueryContext *context_;
	DbXml::XmlUpdateContext *uc_;
	
	XPath2MemoryManager *memoryManager_;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *testDOM_;
	XercesConfiguration xercesConf_;
	DynamicContext *xqContext_;
	
	bool transacted_;
	DbXml::XmlContainer::ContainerType storageModel_;
	u_int32_t flags_;
	bool secure_;
	bool validate_;
	std::string testId_;
	std::string dataDirectory_;
	
	std::map<std::string, std::string> documents_;
};
}

#endif
