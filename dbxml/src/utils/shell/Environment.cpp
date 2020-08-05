//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "Environment.hpp"
#include "ShellCommand.hpp"

using namespace DbXml;
using namespace std;

Environment::Environment(XmlManager &db, SigBlock &sigblock)
	: sigBlock_(sigblock),
	  db_(db),
	  txn_(0),
	  context_(db.createQueryContext()),
	  uc_(db.createUpdateContext()),
	  container_(0),
	  query_(0),
	  results_(0),
	  lineNo_(0),
	  documentProjection_(true),
	  interactive_(false),
	  ignoreErrors_(false),
	  transactions_(false),
	  debugging_(false),
	  verbose_(false),
	  quit_(false),
	  shell_(0)
{
	sigBlock_.setListener(this);
}

Environment::~Environment()
{
	sigBlock_.setListener(0);

	delete query_;
	delete results_;
	delete txn_;
	delete container_;
}

void Environment::signal(int signo)
{
	if(!debugging_) {
		context_.interruptQuery();
	}
}

SigBlock &Environment::sigBlock()
{
	return sigBlock_;
}

XmlManager &Environment::db()
{
	return db_;
}

XmlTransaction *&Environment::txn()
{
	return txn_;
}

XmlQueryContext &Environment::context()
{
	return context_;
}

XmlUpdateContext &Environment::uc()
{
	return uc_;
}

XmlContainer *&Environment::container()
{
	return container_;
}

XmlQueryExpression *&Environment::query()
{
	return query_;
}

XmlResults *&Environment::results()
{
	return results_;
}

bool &Environment::documentProjection()
{
	return documentProjection_;
}

bool &Environment::interactive()
{
	return interactive_;
}

bool &Environment::ignoreErrors()
{
	return ignoreErrors_;
}

bool &Environment::transactions()
{
	return transactions_;
}

bool &Environment::debugging()
{
	return debugging_;
}

bool &Environment::verbose()
{
	return verbose_;
}

bool &Environment::quit()
{
	return quit_;
}

string &Environment::streamName()
{
	return streamName_;
}

int &Environment::lineNo()
{
	return lineNo_;
}

void Environment::testContainer()
{
	if(!container()) {
		throw CommandException("There is no default container");
	}
}

void Environment::deleteContainer()
{
	delete container();
	container() = 0;
}

void Environment::testQuery()
{
	if(!query()) {
		throw CommandException("There is no current query");
	}
}

void Environment::deleteQuery()
{
	delete query();
	query() = 0;
}

void Environment::testResults()
{
	if(!results()) {
		throw CommandException("The last query returned no results");
	}
}

void Environment::deleteResults()
{
	delete results();
	results() = 0;
}

void Environment::testTransaction()
{
	if(!txn()) {
		throw CommandException("There is no current transaction");
	}
}

void Environment::commitTransaction()
{
	txn()->commit();
	delete txn();
	txn() = 0;
}

void Environment::abortTransaction()
{
	txn()->abort();
	delete txn();
	txn() = 0;
}

DbXml::XmlTransaction Environment::childTransaction()
{
	return txn()->createChild();
}

const Shell *&Environment::shell()
{
	return shell_;
}
