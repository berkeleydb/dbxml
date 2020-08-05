//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetNamespaceCommand.hpp"

using namespace DbXml;
using namespace std;

string SetNamespaceCommand::getCommandName() const
{
	return "setNamespace";
}

string SetNamespaceCommand::getCommandNameCompat() const
{
	return "nsset";
}

string SetNamespaceCommand::getBriefHelp() const
{
	return "Create a prefix->namespace binding in the default context";
}

string SetNamespaceCommand::getMoreHelp() const
{
	return
		string("Usage: setNamespace <prefix> <namespaceUri>\n") +
		string("This command calls the XmlQueryContext::setNamespace() method.");
}

void SetNamespaceCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(env.verbose()) cout << "Binding " << args[1] << " -> " << args[2] << endl;
	env.context().setNamespace(args[1], args[2]);
}

