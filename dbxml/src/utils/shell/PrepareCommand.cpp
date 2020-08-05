//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "PrepareCommand.hpp"

#include <sstream>

using namespace DbXml;
using namespace std;

string PrepareCommand::getCommandName() const
{
	return "prepare";
}

string PrepareCommand::getBriefHelp() const
{
	return "Prepare the given query expression as the default pre-parsed query";
}

string PrepareCommand::getMoreHelp() const
{
	return
		string("Usage: prepare <queryExpression>\n") +
		string("This command uses the XmlManager::prepare() method.");
}

static string showErrorContext(const XmlException &e, const char *query)
{
	ostringstream msg;
	msg << e.what();
	if(e.getQueryLine() != 0 && e.getQueryFile() == 0) {
		msg << endl;
		int line = 1;
		while(line != e.getQueryLine() && *query != 0) {
			if(*query == '\n' ||
				(*query == '\r' && *(query + 1) != '\n'))
				++line;
			++query;
		}
		if(line == e.getQueryLine()) {
			while(*query != 0) {
				msg << (*query);
				if(*query == '\n' ||
					(*query == '\r' && *(query + 1) != '\n'))
					break;
				++query;
			}
			if(e.getQueryColumn() != 0) {
				if(*query == 0) msg << endl;
				msg << string(e.getQueryColumn() - 1, ' ') << "^";
			}
		}
	}
	return msg.str();
}

void PrepareCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	XmlQueryExpression *newExpr = 0;
	try {
		if(env.txn()) {
			newExpr = new XmlQueryExpression(env.db().prepare(*env.txn(), args[1], env.context()));
		} else {
			newExpr = new XmlQueryExpression(env.db().prepare(args[1], env.context()));
		}
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

