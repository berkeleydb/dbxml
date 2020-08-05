/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * Debug API
 * A very simple Berkeley DB XML program that demonstrates
 * debug API usage.
 *
 * This program demonstrates how to set a custom debug listener
 * class on the XmlQueryContext, and how to derive a class form
 * XmlDebugListener. The derived class, MyDebugListener, simply
 * prints out the execution information when a query is executed.
 *
 */

#include <dbxml/DbXml.hpp>
#include <iostream>

using namespace DbXml;
using namespace std;

class MyDebugListener : public XmlDebugListener
{
public:
	MyDebugListener(const string query)
		: depth_(-1),
		  query_(query)
	{}

	void start(const XmlStackFrame *stack);
	void end(const XmlStackFrame *stack);
	void enter(const XmlStackFrame *stack);
	void exit(const XmlStackFrame *stack);
	void error(const XmlException error, const XmlStackFrame *stack);

private:
	int depth_;
	string query_;
	void report(string stage, const XmlStackFrame *stack);
};

void MyDebugListener::report(string stage, const XmlStackFrame *stack)
{
	// Function return indentation spaces
	string depthStr("");
	for(int i=0; i<depth_; i++)
		depthStr += string("    ");

	// Get the query expression by line and column
	// Cut the string starting from column number to the the end of the line
	int line = stack->getQueryLine();
	int column = stack->getQueryColumn();

	string::size_type start=0, end=0;
	for (int i=0; i<line ; i++) {
		start = end;
		end = query_.find_first_of('\n', start+1);
		if (end == start+1 || end == string::npos)
			end = query_.length();
	}
	end--;
	if (start != 0)
		start++;

	string exprStr = query_.substr(start+column-1, (end+1)-(start+column-1));

	// Output debug report
	cout << depthStr
	     << stage << ":\""
	     << exprStr << "\" at "
	     << stack->getQueryFile() << ":"
	     << line << ":"
	     << column << endl;
}

void MyDebugListener::start(const XmlStackFrame *stack)
{
	depth_++;
	report("{START}", stack);
}

void MyDebugListener::end(const XmlStackFrame *stack)
{
	report("{END}", stack);
	depth_--;
}

void MyDebugListener::enter(const XmlStackFrame *stack)
{
	depth_++;
	report("[ENTER]", stack);
}

void MyDebugListener::exit(const XmlStackFrame *stack)
{
	report("[EXIT]", stack);
	depth_--;
}

void MyDebugListener::error(const XmlException error, const XmlStackFrame *stack)
{
	cout << "An error occurred in expression at line:" << stack->getQueryLine()
	     << " column:" << stack->getQueryColumn() <<  endl;
	throw error;
}

int main(int argc, char **argv)
{
	string containerName = "debugContainer.dbxml";
	string query = string() +
		       "for $a in collection('debugContainer.dbxml')/root/a\n" +
		       "where $a>'2'\n" +
		       "return $a";
	string docContent = "<root><a>1</a><a>2</a><a>3</a><a>4</a></root>";

	try {
		// Create an XmlManager
		XmlManager mgr;

		// Because the containers will exist on disk, remove them
		// first if they exist
		if (mgr.existsContainer(containerName) != 0)
			mgr.removeContainer(containerName);

		//create container
		XmlContainer cont = mgr.createContainer(containerName);
		XmlUpdateContext uc = mgr.createUpdateContext();

		XmlIndexSpecification is = cont.getIndexSpecification();
		is.setAutoIndexing(false);
		cont.setIndexSpecification(is, uc);

		XmlDocument doc = mgr.createDocument();
		doc.setContent(docContent);
		doc.setName("test.xml");
		cont.putDocument(doc, uc);

		// Set the custom debug listener on XmlQueryContext
		MyDebugListener dl(query);
		XmlQueryContext qc = mgr.createQueryContext();
		qc.setDebugListener(&dl);

		cout << query << endl << endl
		     << "Execution process:" << endl << endl;

		XmlQueryExpression expr = mgr.prepare(query, qc);
		XmlResults re = expr.execute(qc);

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
	}
	return 0;
}
