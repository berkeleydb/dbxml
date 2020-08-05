//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "AddAliasCommand.hpp"

using namespace DbXml;
using namespace std;

string AddAliasCommand::getCommandName() const
{
	return "addAlias";
}

string AddAliasCommand::getBriefHelp() const
{
	return "Add an alias to the default container";
}

string AddAliasCommand::getMoreHelp() const
{
	return
		string("Usage: addAlias <alias>\n")+
		string("This command uses the XmlContainer::addAlias() method.\n");
}

void AddAliasCommand::execute(Args &args, Environment &env)
{
	if (args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.container()->addAlias(args[1]);
	if(env.verbose()) cout << "Added alias: " << args[1] << endl;
}

