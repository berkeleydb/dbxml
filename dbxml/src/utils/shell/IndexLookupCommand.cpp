//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "IndexLookupCommand.hpp"

using namespace DbXml;
using namespace std;

string IndexLookupCommand::getCommandNameCompat() const
{
	return "indexlookup";
}

string IndexLookupCommand::getCommandName() const
{
	return "lookupIndex";
}

string IndexLookupCommand::getBriefHelp() const
{
	return "Performs an index lookup in the default container";
}

string IndexLookupCommand::getMoreHelp() const
{
	return
		string("Usage: lookupIndex <indexDescription> <namespaceUri> <nodeName>\n") +
		string("\t[[<operation>] <value>]\n") +
		string("Valid operations are '<', '<=', '>', '>=' and '=', and the default\n") +
		string("operation is '='. Indexes available can be found using the\n") +
		string("'listIndexes' command.\n\n") +
		string("Example -- look up all documents in document name index:\n\n") +
		string("\tlookupIndex node-metadata-equality-string \\\n") +
		string("\t    http://www.sleepycat.com/2002/dbxml name\n\n") +
		string("Example -- look up a specific document in document name index:\n\n") +
		string("\tlookupIndex node-metadata-equality-string \\\n") +
		string("\t    http://www.sleepycat.com/2002/dbxml name myDocument\n");
}

void IndexLookupCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 4 || args.size() > 6) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();

	XmlValue value;
	XmlIndexLookup::Operation op = XmlIndexLookup::EQ;
	if(args.size() > 4) {
		// need to use type coercion in order
		// to support lookup on non-string indexes
		XmlValue::Type type = XmlIndexSpecification::getValueType(
			args[1]);

		value = XmlValue(type, args[args.size() - 1]);

		if(args.size() == 6) {
			if(args[4] == "=") {
				op = XmlIndexLookup::EQ;
			}
			else if(args[4] == "<") {
				op = XmlIndexLookup::LT;
			}
			else if(args[4] == "<=") {
				op = XmlIndexLookup::LTE;
			}
			else if(args[4] == ">") {
				op = XmlIndexLookup::GT;
			}
			else if(args[4] == ">=") {
				op = XmlIndexLookup::GTE;
			}
			else {
				throw CommandException("Invalid operation: " + args[4]);
			}
		}
	}

	XmlIndexLookup il = env.db().createIndexLookup(*env.container(),
		args[2], args[3], args[1], value, op);

	XmlResults *newResults = 0;
	if(env.txn()) {
		newResults = new XmlResults(il.execute(*env.txn(),
						       env.context(),
						       DBXML_LAZY_DOCS));
	} else {
		newResults = new XmlResults(il.execute(env.context(),
						       DBXML_LAZY_DOCS));
	}
	env.deleteResults();
	env.results() = newResults;

	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose())
			cout << (unsigned int) env.results()->size()
			     << " objects returned for eager index lookup '"
			     << args[1] << "'" << endl << endl;
	} else {
		if(env.verbose())
			cout << "Lazy index lookup '"
			     << args[1] << "' completed" << endl << endl;
	}
}

