//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetBaseURICommand.hpp"

using namespace DbXml;
using namespace std;

string SetBaseURICommand::getCommandName() const
{
	return "setBaseUri";
}

string SetBaseURICommand::getCommandNameCompat() const
{
	return "baseuriset";
}

string SetBaseURICommand::getBriefHelp() const
{
	return "Set/get the base uri in the default context";
}

string SetBaseURICommand::getMoreHelp() const
{
	return
		string("Usage: setBaseUri [<uri>]\n") +
		string("The base uri must be of the form, \"scheme:path\".\n") +
		string("If there is no argument, the current base URI is displayed.\n") +
		string("This command calls the XmlQueryContext::get/setBaseURI() methods.");
}

void SetBaseURICommand::execute(Args &args, Environment &env)
{
	if (args.size() == 1) {
		cout << "Current base URI: " << env.context().getBaseURI();
		return;
	} else if (args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	if(env.verbose()) cout << "Base URI = " << args[1] << endl;
	env.context().setBaseURI(args[1]);
}

