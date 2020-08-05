//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "QueryCommand.hpp"

#include <sstream>

using namespace DbXml;
using namespace std;

string QueryCommand::getCommandName() const
{
	return "query";
}

string QueryCommand::getBriefHelp() const
{
	return "Execute the given query expression, or the default pre-parsed query";
}

string QueryCommand::getMoreHelp() const
{
	return
		string("Usage: query [queryExpression]\n") +
		string("This command uses the XmlManager::query() method.");
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

void QueryCommand::execute(Args &args, Environment &env)
{
	if(args.size() > 2) {
		throw CommandException("Wrong number of arguments");
	}

	string query;
	if(args.size() == 1) {
		env.testQuery();
		query = env.query()->getQuery();
	} else {
		query = args[1];
	}

	u_int32_t flags = DBXML_LAZY_DOCS;
	if(env.documentProjection())
		flags |= DBXML_DOCUMENT_PROJECTION;

	XmlResults *newResults = 0;
	try {
		if(env.txn()) {
			if(args.size() == 1) {
				newResults = new XmlResults(env.query()->execute(*env.txn(), env.context(), flags));
			} else {
				newResults = new XmlResults(env.db().query(*env.txn(), args[1], env.context(), flags));
			}
		} else {
			if(args.size() == 1) {
				newResults = new XmlResults(env.query()->execute(env.context(), flags));
			} else {
				newResults = new XmlResults(env.db().query(args[1], env.context(), flags));
			}
		}
	}
	catch(XmlException &e) {
		if(e.getExceptionCode() == XmlException::OPERATION_INTERRUPTED)
			env.sigBlock().reset();
		throw CommandException(showErrorContext(e, query.c_str()));
	}
	env.deleteResults();
	env.results() = newResults;
	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose())
			cout << (unsigned int) env.results()->size()
			     << " objects returned for eager expression '"
			     << query << "'" << endl << endl;
	} else {
		if(env.verbose())
			cout << "Lazy expression '" << query
			     << "' completed" << endl << endl;
	}
}

