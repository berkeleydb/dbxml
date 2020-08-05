//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
#ifndef __XMLUNIT_HPP
#define __XMLUNIT_HPP

/*
 * XmlUnit -- a simple, small unit test framework for C++ programs.
 *
 * Use cases:
 * 1.  create a single unit test case or set of closely related tests:
 *  a.  subclass XmlUnit
 *  b.  implement the run() method
 *  c.  optionally implement preRun() and postRun() methods
 *  d.  use XmlUnit_assert() macro for test assertions or
 *  XmlUnit_fail() macro for simple failures.
 * 2.  create a test suite made of multiple sets of test cases:
 *  a.  do above to implement one or more XmlUnit subclasses and test cases
 *  b.  create an instance of XmlSuite
 *  c.  add XmlUnit instances to the XmlSuite instance (addTest() method)
 *  d.  run the suite
 *
 * TBD:
 *  o output as XML or in more machine-readable form
 */

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace DbXml {

class XmlUnitSuite;

//
// XmlUnitException
//
// Exception class for BDB XML unit tests
//
class XmlUnitException {
public:
	XmlUnitException(const char *message = 0)
		: msg_(message ? message : "") {}
	const std::string &getMessage() const {
		return msg_;
	}
private:
	std::string msg_;
};

//
// XmlUnit
//
// Base class for test case instances.  Each instance
// can be a single case or a number of related cases
//
class XmlUnit {
public:
	XmlUnit(const char *name = 0) :
		suite_(0),
		name_(name ? name : "") {}
	virtual ~XmlUnit() {}
	virtual int run() = 0;
	virtual void preRun() {}
	virtual void postRun() {}
	virtual XmlUnitSuite *getSuite() {
		return suite_;
	}
	virtual const std::string &getName() const {
		return name_;
	}
	// allow classes to override fail method
	virtual int fail(const char *msg,
			 const char *file, int line,
			 bool isAssert) const;
	virtual void outputSuccess(std::ostream &out) { out << "."; }
	virtual void outputFailure(std::ostream &out) { out << "!"; }
	
	void setSuite(XmlUnitSuite *suite) {
		suite_ = suite;
	}
protected:
	XmlUnitSuite *suite_;
	std::string name_;
};

#define XmlUnit_assert(expr)((void)((expr) ? 0 : (fail(#expr, __FILE__, __LINE__,true))))
#define XmlUnit_fail(msg) (fail((msg), __FILE__, __LINE__, false))
	
//
// XmlUnitSuite
//
// A base class for suites of test cases.  It is intended
// to store a number of unit test cases.
//
class XmlUnitSuite : public XmlUnit {
public:
	XmlUnitSuite(const char *name = 0) :
		suiteName_(name ? name : "(unknown)"),
		out_(&std::cout),
		errOut_(&std::cerr),
		numCasesRun_(0) {}
	virtual ~XmlUnitSuite();
	virtual void close() { delete this; }
	virtual void addTest(XmlUnit *testCase) {
		testCases_.push_back(testCase);
		testCase->setSuite(this);
	}
	virtual void report() const;
	virtual int getNumFailures() const {
		return (int) failures_.size();
	}
	
	// non-virtual methods
	void setOutput(std::ostream *out) { out_ = out; }
	void setErrorOutput(std::ostream *out) { errOut_ = out; }
	std::ostream & getOutput() const;
	std::ostream & getErrorOutput() const;
	int getIndent(); // recursive
	
	// XmlUnit implementation
	int run();
	// don't override preRun(), postRun()
	const std::string &getName() const { return suiteName_; }
	void outputSuccess(std::ostream &out) {} // override
	void outputFailure(std::ostream &out) {} // override
	XmlUnitSuite *getSuite() { return this; }

	void reportFailure(const std::string &name,
			   const std::string &msg) {
		failures_.push_back(failure(name,msg));
	}
protected:
	class failure {
	public:
		failure() {}
		failure(const std::string &name,
			const std::string &msg) :
			name_(name), msg_(msg) {}
		std::string name_;
		std::string msg_;
	};
protected:
	std::string suiteName_;
	std::vector<failure> failures_;
	std::vector<XmlUnit*> testCases_;
	std::ostream *out_;
	std::ostream *errOut_;
	int numCasesRun_;
};
}

//
// the macros below are made available to make it easier to
// implement a "class-per-testcase" set of tests
// In the case of XMLUNIT_CASE, once the macro is used the
// implementor only needs to implement the classname::run() method.
// In the case of XMLUNIT_CASE_PREPOST implementations of preRun(),
// run() and postRun() are required.
// For example, this is sufficient to create an empty test case:
//   XMLUNIT_CASE(test)
//   void test::run() {}
//

#define XMLUNIT_CASE(classname)			\
	class classname : public XmlUnit	\
	{					\
	public:					\
		classname() : XmlUnit(#classname) {}	\
		int run();				\
	};

#define XMLUNIT_CASE_PREPOST(classname)		\
	class classname : public XmlUnit	\
	{					\
	public:						\
		classname() : XmlUnit(#classname) {}	\
		int run();				\
		void preRun();				\
		void postRun();				\
	};

#endif
