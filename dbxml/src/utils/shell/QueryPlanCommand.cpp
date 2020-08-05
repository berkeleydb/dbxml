//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "QueryPlanCommand.hpp"
#include <fstream>

using namespace DbXml;
using namespace std;

string QueryPlanCommand::getCommandName() const
{
	return "queryPlan";
}

string QueryPlanCommand::getCommandNameCompat() const
{
	return "qplan";
}

string QueryPlanCommand::getBriefHelp() const
{
	return "Prints the query plan for the specified query expression";
}

string QueryPlanCommand::getMoreHelp() const
{
	return
		string("Usage: queryPlan <queryExpression> [pathToFile]\n") +
		string("This command parses the queryExpression into an\n") +
		string("XmlQueryExpression, and calls getQueryPlan().\n") +
		string("If the optional pathToFile paramter is specified\n")+
		string("the output is printed to the specified file.");
}

void QueryPlanCommand::execute(Args &args, Environment &env)
{
	if(args.size() > 3) {
		throw CommandException("Wrong number of arguments");
	}
	std::string plan;
	if(args.size() == 1) {
		env.testQuery();
		plan = env.query()->getQueryPlan();
	} else {
		if(env.txn()) {
			XmlQueryExpression expr = env.db().prepare(*env.txn(), args[1],
				env.context());
			plan = expr.getQueryPlan();
		} else {
			XmlQueryExpression expr = env.db().prepare(args[1],
				env.context());
			plan = expr.getQueryPlan();
		}
	}
	// where should output go?
	bool toFile = false;
	ofstream outputFile;
	if (args.size() == 3) {
		outputFile.open(args[2].c_str(), ofstream::out|ofstream::trunc|
				ofstream::binary);
		toFile = true;
	}
	ostream &output = (toFile ? (ostream &)outputFile : cout);
	output << plan;
	if (toFile)
		outputFile.close();
}

