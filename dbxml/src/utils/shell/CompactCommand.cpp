//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "CompactCommand.hpp"

using namespace DbXml;
using namespace std;

string CompactCommand::getCommandName() const
{
	return "compactContainer";
}

string CompactCommand::getBriefHelp() const
{
	return "Compact a container to shrink it's size";
}

string CompactCommand::getMoreHelp() const
{
	return
		string("Usage: compactContainer <containerName>\n")+
		string("This command uses the XmlManager::compactContainer() method.\n")+
		string("This command can take a long time on large containers.\n")+
		string("Containers should be backed up before running this command.\n");
}

void CompactCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	if(env.txn()) {
		env.db().compactContainer(*(env.txn()), args[1],
					  env.uc(), 0);
	} else {
		env.db().compactContainer(args[1], env.uc(), 0);
	}
	
	if(env.verbose()) cout << "Container compacted: " << args[1] << endl;
}

