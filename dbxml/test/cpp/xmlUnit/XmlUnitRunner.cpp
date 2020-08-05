//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
#include "XmlUnit.hpp"

using namespace DbXml;
using namespace std;

//
// Add test cases and suites here.  Comment heavily to describe
// what tests are intended to achieve.
//

void addTestCases(XmlUnitSuite &suite)
{
}

//
// main program to drive suite
//
int main(int argc, char **argv)
{
	try {
		// put it in a suite
		XmlUnitSuite suite("XmlUnitRunner");
		addTestCases(suite);
		suite.run();
		suite.report();
	} catch (XmlUnitException &xue) {
		std::cerr << "exception: " << xue.getMessage() << std::endl;
	}
}
