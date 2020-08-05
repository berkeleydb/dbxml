//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "EdgeIndexLookupCommand.hpp"

using namespace DbXml;
using namespace std;

string EdgeIndexLookupCommand::getCommandName() const
{
	return "lookupEdgeIndex";
}

string EdgeIndexLookupCommand::getBriefHelp() const
{
	return "Performs an edge index lookup in the default container";
}

string EdgeIndexLookupCommand::getMoreHelp() const
{
	return
		string("Usage: lookupEdgeIndex <indexDescription> <namespaceUri> <nodeName>\n") +
		string("\t<parentNamespaceUri> <parentNodeName> [[<operation>] <value>]\n") +
		string("Valid operations are '<', '<=', '>', '>=' and '=', and the default\n") +
		string("operation is '='. Indexes available can be found using the\n") +
		string("'listIndexes' command.\n\n") +
		string("Example -- look up elements called \"a\" with a parent called \"b\"\n") +
		string("on an edge presence index:\n\n") +
		string("\tlookupEdgeIndex edge-element-presence \"\" a \"\" b\n\n") +
		string("Example -- look up elements called \"a\" with a parent called \"b\"\n") +
		string("and a value smaller than 5 on an edge equality decimal index:\n\n") +
		string("\tlookupEdgeIndex edge-element-equality-decimal \\\n") +
		string("\t    \"\" a \"\" b < 5\n");
}

void EdgeIndexLookupCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 6 || args.size() > 8) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.deleteResults();

	XmlValue value;
	XmlIndexLookup::Operation op = XmlIndexLookup::EQ;
	if(args.size() > 6) {
		// need to use type coercion in order
		// to support lookup on non-string indexes
		XmlValue::Type type = XmlIndexSpecification::getValueType(
			args[1]);

		value = XmlValue(type, args[args.size() - 1]);

		if(args.size() == 8) {
			if(args[6] == "=") {
				op = XmlIndexLookup::EQ;
			}
			else if(args[6] == "<") {
				op = XmlIndexLookup::LT;
			}
			else if(args[6] == "<=") {
				op = XmlIndexLookup::LTE;
			}
			else if(args[6] == ">") {
				op = XmlIndexLookup::GT;
			}
			else if(args[6] == ">=") {
				op = XmlIndexLookup::GTE;
			}
			else {
				throw CommandException("Invalid operation: " + args[6]);
			}
		}
	}

	XmlIndexLookup il = env.db().createIndexLookup(*env.container(),
		args[2], args[3], args[1], value, op);
	il.setParent(args[4], args[5]);

	if(env.txn()) {
		env.results() = new XmlResults(il.execute(*env.txn(), env.context(),
						       DBXML_LAZY_DOCS));
	} else {
		env.results() = new XmlResults(il.execute(env.context(),
						       DBXML_LAZY_DOCS));
	}

	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose())
			cout << (unsigned int) env.results()->size()
			     << " objects returned for eager index lookup '"
			     << args[1] << "'" << endl << endl;
	} else {
		if(env.verbose())
			cout << "Lazy index lookup '" << args[1]
			     << "' completed" << endl << endl;
	}
}

