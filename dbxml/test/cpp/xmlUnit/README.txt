README for BDB XML's C++ Unit Testing Framework

Objectives/requirements
-----------------------

1.  Simple, easy to use
2.  Not BDB XML dependent at the top-level
3.  Able to handle hierarchies of tests
4.  Simple, easy to use (yes, already stated, but important)

Terms
-----

"Test Case" -- a class instance that implements XmlUnit, may have
one or more actual internal test cases.

"Suite" -- an instance of XmlUnit that is able to run its own
test cases.  In this case the class is XmlUnitSuite but others could 
be written

Classes/Hierarchy
-----------------

XmlUnit -- base class for all test cases and "suites"
	It has virtual methods for run(), preRun(), postRun(),
	as well as some other virtual behavior.  The only
	method that is truly important and must be implemented
	is run().

XmlUnitSuite -- an instance of XmlUnit that is used to hold
	one or more XmlUnit instances (see addTest() method).
        When run it will run all of the added XmlUnit instances.
	This enables a hierarchy to be created.
	The report() method will report on successes and failures.

Files in test/cpp/xmlUnit
-------------------------
      XmlUnit.hpp -- the interface
      XmlUnit.cpp -- the implementation
      XmlUnitRunner.cpp -- a main program that drives unit tests.
      Test cases are added here in the obvious place.

      New test cases and files will be added here as they are
      developed.

Examples of adding a new test class
-----------------------------------

1.  Explicit class declaration/creation

class MyTest : public XmlUnit
{
public:
	MyTest(bool fail) : XmlUnit("mytest"), fail_(fail) {}
	int run() {
		if (fail_) {
			XmlUnit_assert((false)); // or XmlUnit_fail("test");
		}
		return 1;
	}
	bool fail_;
};

An example fragment of code to use/run this:
XmlUnitSuite suite("mysuite");
suite.addTest(new MyTest(false)); // instance is donated to the suite
suite.run(); // run the suite
suite.report();  // report 

Generally test cases should be added in the XmlUnitRunner.cpp file in
the addTestCases() method.

2.  Using macros

XMLUNIT_CASE("MyTest")

MyTest::run()
{
	// implement this
}

The XMLUNIT_CASE macro generates the boilerplate for the instance and
makes it easy to use a "class-per-testcase" paradigm.


More information
----------------

Because the framework can only count instances of XmlUnit it 
can be best to use a "class-per-testcase" method of implementation
where a related group of tests are implemented in their own
suite and each test case is implemented as its own XmlUnit instance.
An example of that might be:

XMLUNIT_CASE("mySuite1")
mySuite1::run() {}
XMLUNIT_CASE("mySuite2")
mySuite2::run() {}
...
XMLUNIT_CASE("mySuiteN")
mySuiteN::run() {}

XmlUnitSuite *mySuite = new XmlUnitSuite("mySuite");
mySuite->addTest(new mySuite1());
mySuite->addTest(new mySuite2());
...
mySuite->addTest(new mySuiteN());

Then, in XmlUnitRunner, do:
      suite.addTest(mySuite);

The best thing to do is experiment with the classes as well as output
and see what works for a group of tests.







