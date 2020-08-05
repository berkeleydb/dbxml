//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DebugOptimizationCommand.hpp"
#include "DebugCommand.hpp"

namespace DbXml {
DBXML_EXPORT XmlQueryExpression debugOptimization(XmlTransaction &txn, const std::string &query, XmlQueryContext &context);
}

using namespace DbXml;
using namespace std;

string DebugOptimizationCommand::getCommandName() const
{
	return "debugOptimization";
}

string DebugOptimizationCommand::getBriefHelp() const
{
	return "Debug optimization command -- internal use only";
}

string DebugOptimizationCommand::getMoreHelp() const
{
	return "";
}

void DebugOptimizationCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	XmlQueryExpression *newExpr = 0;
	try {
		XmlTransaction txn(env.txn() ? *env.txn() : XmlTransaction());
		newExpr = new XmlQueryExpression(debugOptimization(txn, args[1], env.context()));
	}
	catch(XmlException &e) {
		throw CommandException(showErrorContext(e, args[1].c_str()));
	}
	env.deleteQuery();
	env.query() = newExpr;

	if(env.verbose())
		cout << "Prepared expression '" << args[1]
		     << "'" << endl << endl;
}

