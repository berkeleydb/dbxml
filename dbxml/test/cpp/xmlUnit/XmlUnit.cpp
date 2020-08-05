//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
#include "XmlUnit.hpp"

using namespace DbXml;
using namespace std;

//
// XmlUnitSuite
//
XmlUnitSuite::~XmlUnitSuite()
{
	// XmlUnit instances are donated and need to be cleaned up
	for (unsigned int i = 0; i < testCases_.size(); i++)
		delete (testCases_[i]);
}

//
// NOTE: progress output will be formatted a bit oddly if
// normal test cases are mixed with suites in a single
// XmlUnitSuite.  This is possible but generally suites should
// be either all unit tests or all sub-suites.
//
int XmlUnitSuite::run()
{
	std::ostream &out = getOutput();
	out << endl;
	for (int i = 0; i < getIndent(); i++)
		out << " ";
	out << getName() << ":";
	for (unsigned int i = 0; i < testCases_.size(); i++) {
		try {
			testCases_[i]->preRun();
			numCasesRun_ += testCases_[i]->run();
			testCases_[i]->postRun();
			testCases_[i]->outputSuccess(out);
		} catch (XmlUnitException &) {
                        // for now don't use the exception
                        // message; assume it's been recorded
			testCases_[i]->outputFailure(out);
		} catch (...) {
			reportFailure(testCases_[i]->getName(),
				      "Uncaught exception thrown from test");
		}
	}
	return numCasesRun_;
}

void XmlUnitSuite::report() const
{
	std::ostream &out = getOutput();
	out << endl << "Suite '" << getName() << "' (" << numCasesRun_ << " cases): ";
	if (failures_.size() == 0) {
		out << "PASS" << std::endl;
	} else {
		unsigned int size = (unsigned int)failures_.size();
		out << size << " FAILURE(S)" << std::endl;
		// detail goes to error output
		std::ostream &errOut = getErrorOutput();
		errOut << "Suite '" << getName() << "' failure detail ("
		    << size << "):";
		for (unsigned int i = 0; i < size; i++) {
			errOut << "\t" << failures_[i].name_ << ": " <<
				failures_[i].msg_ << std::endl;
		}
	}
}

int XmlUnitSuite::getIndent()
{
	static const int indentIncr = 4;
	if (suite_)
		return suite_->getIndent() + indentIncr;
	return 0;
}

//
// use parent suite's output if nested
//
std::ostream &XmlUnitSuite::getOutput() const
{
	if (suite_)
		return suite_->getOutput();
	else
		return *out_;
}

std::ostream &XmlUnitSuite::getErrorOutput() const
{
	if (suite_)
		return suite_->getErrorOutput();
	else
		return *errOut_;
}

//
// XmlUnit
//

int XmlUnit::fail(const char *msg,
		  const char *file,
		  int line, bool isAssertion) const
{
	std::ostringstream out;
	out << "FAILURE: '" << getName();
	if (file)
		out << "' in file " << file;
	if (line >= 0)
		out << ", line " << line;
	
	if (isAssertion)
		out << ", Assertion: " << msg;
	else
		out << ": " << msg;
	if (suite_)
		suite_->reportFailure(getName(), out.str());
	else
		std::cerr << out.str() << std::endl;

	// do not continue after failure
	throw XmlUnitException(out.str().c_str());
	return 0;
}

