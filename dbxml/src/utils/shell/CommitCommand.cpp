//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "CommitCommand.hpp"

using namespace DbXml;
using namespace std;

string CommitCommand::getCommandName() const
{
	return "commit";
}

string CommitCommand::getBriefHelp() const
{
	return "Commits the current transaction, and starts a new one";
}

string CommitCommand::getMoreHelp() const
{
	return "Usage: commit\nThis command uses XmlTransaction::commit()\n";
}

void CommitCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 1) {
		throw CommandException("Wrong number of arguments");
	}

	env.testTransaction();
	env.deleteResults();
	env.commitTransaction();
	delete env.txn();
	env.txn() = 0;
	if(env.verbose())
		cout << "Transaction committed" << endl;
}

