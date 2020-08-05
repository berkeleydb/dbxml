//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "ReturnTypeCommand.hpp"

using namespace DbXml;
using namespace std;

string ReturnTypeCommand::getCommandName() const
{
	return "setReturnType";
}

string ReturnTypeCommand::getCommandNameCompat() const
{
	return "returntype";
}

string ReturnTypeCommand::getBriefHelp() const
{
	return "Sets the return type on the default context";
}

string ReturnTypeCommand::getMoreHelp() const
{
	return
		string("Usage: setReturnType <lv>\n") +
		string("This command is currently a no-op");
}

void ReturnTypeCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}

	string type;
	XmlQueryContext::ReturnType t = (XmlQueryContext::ReturnType)-1;

	if(args[1] == "lv") {
		t = XmlQueryContext::LiveValues;
		type = "LiveValues";
	}

	if(t == (XmlQueryContext::ReturnType)-1) {
		throw CommandException("Unknown return type: " + args[1]);
	}

	if(env.verbose()) cout << "Setting return type to: " << type << endl;
	env.context().setReturnType(t);
}

