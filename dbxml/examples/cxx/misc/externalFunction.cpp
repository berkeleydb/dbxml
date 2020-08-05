/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2008,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * external function
 *
 * A very simple Berkeley DB XML program that demonstrates 
 * external function API usage.
 *
 * This program demonstrates:
 *  XmlResolver used to resolve multiple external functions 
 *  XmlExternalFunction implementations
 *  External function results handling
 *	
 *  MyFunResolver implements the XmlResolver interface, it is used to 
 *  resolve 2 distinct external function classes -- MyExternalFunctionPow
 *  and MyexternalFunctionSqrt. In this example MyFunResolver will always
 *  return new instance of each external function class, that means the 
 *  external function class must call "delete this" in their close functions.
 *  Another, legitimate (and arguably better) choice would be to not return 
 *  new instances each time. This choice is up to the implementor of the 
 *  resolver and external function classes.   
 *  
 *  MyExternalFunctionPow implements the XmlExternalFunction interface, and
 *  implement the functionality of pow() function.
 *
 *  MyExternalFunctionSqrt implements the XmlExternalFunction interface, and
 *  implement the functionality of sqrt() function.
 *
 */    

#include <iostream>
#include <math.h>
#include <dbxml/DbXml.hpp>

using namespace DbXml;
using namespace std;

class MyExternalFunctionPow : public XmlExternalFunction
{
public:
	XmlResults execute(XmlTransaction &txn, XmlManager &mgr, const XmlArguments &args) const;
	void close();

};

class MyExternalFunctionSqrt : public XmlExternalFunction
{
public:
	XmlResults execute(XmlTransaction &txn, XmlManager &mgr, const XmlArguments &args) const;
	void close();
};

class MyFunResolver : public XmlResolver
{
public:
	MyFunResolver();
	XmlExternalFunction *resolveExternalFunction(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, const std::string &name, size_t numberOfArgs) const; 
	string getUri(){ return uri_; }
private:
	const string uri_;
};

/* External function pow() implementation */
XmlResults MyExternalFunctionPow::execute(XmlTransaction &txn, XmlManager &mgr, const XmlArguments &args) const
{
	XmlResults argResult1 = args.getArgument(0);
	XmlResults argResult2 = args.getArgument(1);

	XmlValue arg1;
	XmlValue arg2;
	
	// Retrieve argument as XmlValue 
	argResult1.next(arg1);
	argResult2.next(arg2);
	
	// Call pow() from C++ 
	double result = pow(arg1.asNumber(),arg2.asNumber());
	
	// Create an XmlResults for return
	XmlResults results = mgr.createResults();
	XmlValue va(result);
	results.add(va);
	
	return results;
}

/* 
 * MyFunResolver returns a new instance of this object for each Resolution, so
 * that instance must be deleted here 
 */
void MyExternalFunctionPow::close()
{
	delete this;
}

/* External function sqrt() implementation */
XmlResults MyExternalFunctionSqrt::execute(XmlTransaction &txn, XmlManager &mgr, const XmlArguments &args) const
{
	XmlResults argResult1 = args.getArgument(0);
	XmlValue arg1;
	argResult1.next(arg1);
	
	// Call sqrt() from C++ 
	double result = sqrt(arg1.asNumber());
		
	XmlResults results = mgr.createResults();
	XmlValue va(result);
	results.add(va);
	
	return results;
	
}

/* 
 * MyFunResolver returns a new instance of this object for each Resolution, so
 * that instance must be deleted here 
 */
void MyExternalFunctionSqrt::close()
{
	delete this;
}


MyFunResolver::MyFunResolver()
	:uri_("my://my.fun.resolver")
{
}


/* 
 * Returns a new instance of either MyExternalFunctionPow or 
 * MyExternalFuncitonSqrt if the URI, function name, and number of
 * arguments match. 
 */
XmlExternalFunction* MyFunResolver::resolveExternalFunction(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, const std::string &name, size_t numberOfArgs) const 
{
	XmlExternalFunction *fun = 0;

	if (uri == uri_ && name == "pow" && numberOfArgs == 2 )
		fun = new MyExternalFunctionPow();
	else if (uri == uri_ && name == "sqrt" && numberOfArgs == 1)
		fun = new MyExternalFunctionSqrt();

	return fun;
}


int 
main(int argc, char **argv)
{
	// Query that calls the external function pow() 
	// The function must be declared in the query's preamble
	string query1 = 
		"declare function my:pow($a as xs:double, $b as xs:double) as xs:double external;\nmy:pow(2,3)";
		
	// Query that calls the external function sqrt() 
	string query2 = 
		"declare function my:sqrt($a as xs:double) as xs:double external;\nmy:sqrt(16)";

	try {
		
		// Create an XmlManager
		XmlManager mgr;
		
		// Create an function resolver
		MyFunResolver resolver;

		// Register the function resolver to XmlManager
		mgr.registerResolver(resolver); 

		XmlQueryContext context = mgr.createQueryContext();
		
		// Set the prefix URI
		context.setNamespace("my", resolver.getUri());

		// The first query returns the result of pow(2,3)
		XmlResults results = mgr.query(query1, context);

		XmlValue va;
		while (results.next(va)) {
			cout << "The result of pow(2,3) is : " << va.asNumber() << endl;
		}

		// The second query returns the result of sqrt(16)
		results = mgr.query(query2, context);
		while (results.next(va)) {
			cout << "The result of sqrt(16) is : " << va.asNumber() << endl;
		}

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
	} 
	return 0;
}
