//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "AbortCommand.hpp"

using namespace DbXml;
using namespace std;

string AbortCommand::getCommandName() const
{
  return "abort";
}

string AbortCommand::getBriefHelp() const
{
	return "Aborts the current transaction";
}

string AbortCommand::getMoreHelp() const
{
	return "Usage: abort\nThis command uses XmlTransaction::abort()";
}

void AbortCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 1) {
		throw CommandException("Wrong number of arguments");
	}
	env.testTransaction();
	env.abortTransaction();
	if(env.verbose()) cout << "Transaction aborted" << endl;
}

