//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XQUERYTEST_HPP
#define __XQUERYTEST_HPP

#include <string>

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

namespace DbXmlTest
{

class XQueryTest : public UnitTest
{
public:
	XQueryTest(TestEnvironment &env, const Iterator<std::string> &args = EMPTY(std::string));
	virtual ~XQueryTest();

	virtual void execute();

private:
	void doTest(const std::string &file, const std::string &xquery);
	bool getResult(const std::string &file, const std::string &new_extension, std::string &expected);
	bool setResult(const std::string &file, const std::string &new_extension, const std::string &expected);

	XQueryTest(const XQueryTest &);
	XQueryTest &operator=(const XQueryTest &);
};

}

#endif
