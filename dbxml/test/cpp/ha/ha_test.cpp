/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 *******
 *
 * HA TestCases emulates some scenarios for testing DbXml HA working.
 *
 */

#include <dbxml/DbXml.hpp>
#include <cstdlib>
#include "SimpleTestCase.hpp"
#include "NormalTestCase.hpp"

using namespace DbXml;
using namespace std;

static void usage()
{
	cerr << "usage: " << endl;
	cerr << "\t -v \t turn on verbose message output." << endl;

	exit(EXIT_FAILURE);
}

static void execTestCase(TestCase& testcase)
{
	try {
	cout << endl << testcase.getName()
	     << " Begin:  ====================================" << endl;

	testcase.run();
	cout << testcase.getName() << " verify: ";
	if(testcase.verify())
		cout << "Verify success." << endl;
	else
		cout << "Verify failed." << endl;


	cout << testcase.getName() << " End."
	     << endl << endl;

	} catch (XmlException &xe) {
		cerr << xe.what() << endl;
		cerr << testcase.getName() << " failed." << endl;
		return;
	}
}

int main(int argc, char **argv)
{
	char ch;
	Config config;
	while ((ch = getopt(argc, argv, "v")) != EOF) {
		switch (ch) {
		case 'v':
			config.verbose_ = true;
			break;
		case '?':
		default:
			usage();
		}
	}

	try {

 		{
 		SimpleTestCase simpleTestCase(config);
 		execTestCase(simpleTestCase);
 		}

		{
		NormalTestCase normalTestCase(config);
		execTestCase(normalTestCase);
		}

	} catch (XmlException &xe) {
		cerr << xe.what() << endl;
		return -1;
	}

	return 0;
}
