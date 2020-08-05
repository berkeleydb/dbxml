//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "StartTransactionCommand.hpp"

using namespace DbXml;
using namespace std;

string StartTransactionCommand::getCommandNameCompat() const
{
	return "starttransaction";
}

string StartTransactionCommand::getCommandName() const
{
	return "transaction";
}

string StartTransactionCommand::getBriefHelp() const
{
	return "Create a transaction for all subsequent operations to use";
}

string StartTransactionCommand::getMoreHelp() const
{
	return
		string("Usage: transaction\nThis command uses XmlManager::createTransaction()\n")+
		string("Any transaction already in force is committed.");
}

void StartTransactionCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 1) {
		throw CommandException("Wrong number of arguments");
	}
	if (!env.transactions()) {
		throw CommandException("Cannot use transaction command in non-transacted environment");
	}
	env.deleteResults();
	
	if(env.txn()) {
		env.commitTransaction();
		delete env.txn();
		env.txn() = 0;
	}
	env.txn() = new XmlTransaction(env.db().createTransaction());
	
	if(env.verbose()) cout << "Transaction started" << endl;
}

