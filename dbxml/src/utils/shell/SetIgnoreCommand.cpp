//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetIgnoreCommand.hpp"

using namespace DbXml;
using namespace std;

string SetIgnoreCommand::getCommandName() const
{
	return "setIgnore";
}

string SetIgnoreCommand::getBriefHelp() const
{
	return "Tell the shell to ignore script errors";
}

string SetIgnoreCommand::getMoreHelp() const
{
	return
		string("Usage: setIgnore <on|off>\n") +
		string("When set on, errors from commands in dbxml shell\n")+
		string("scripts will be ignored.  When off, they will cause\n")+
		string("termination of the script.  Default is off\n");
}

void SetIgnoreCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}

	if (args[1] == (std::string)"on")
		env.ignoreErrors() = true;
	else
		env.ignoreErrors() = false;
}

