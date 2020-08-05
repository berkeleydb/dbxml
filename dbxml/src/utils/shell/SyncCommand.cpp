//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SyncCommand.hpp"
#include "Shell.hpp"
#include "PreLoadCommand.hpp"

using namespace DbXml;
using namespace std;

string SyncCommand::getCommandName() const
{
	return "sync";
}

string SyncCommand::getBriefHelp() const
{
	return "Sync current container to disk";
}

string SyncCommand::getMoreHelp() const
{
	return
		string("Usage: sync\n")+
		string("This command syncs the current container to disk, using XmlContainer::sync()\n");
}

void SyncCommand::execute(Args &args, Environment &env)
{
	if (args.size() != 1) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.container()->sync();
}

