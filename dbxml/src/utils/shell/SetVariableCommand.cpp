//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetVariableCommand.hpp"

using namespace DbXml;
using namespace std;

string SetVariableCommand::getCommandName() const
{
	return "setVariable";
}

string SetVariableCommand::getCommandNameCompat() const
{
	return "varset";
}

string SetVariableCommand::getBriefHelp() const
{
	return "Set a variable in the default context";
}

string SetVariableCommand::getMoreHelp() const
{
	return
		"Usage: setVariable <varName> [<value> (<value>...)]\n"
		"This command uses XmlQueryContext::setVariableValue()\n"
		"to set a variable value (untyped).  Use setTypedVariable\n"
		"to set a typed variable."
		"If no value is provided, the variable is set to the value\n"
		"of the most recent results.\n";
}

void SetVariableCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 2) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(env.verbose()) cout << "Setting $" << args[1] << " =";

	XmlResults results = env.db().createResults();

	if(args.size() > 2) {
		for(unsigned int arg = 2; arg < args.size(); ++arg) {
			if(env.verbose()) cout << " " << args[arg];
			results.add(args[arg]);
		}
	} else {
		if(env.verbose()) cout << " <previous results>";
		env.testResults();
		env.results()->reset();
		XmlValue value;
		while(env.results()->next(value)) {
			if(env.sigBlock().isInterrupted()) {
				env.sigBlock().reset();
				throw CommandException("setVariable interrupted");
			}

			results.add(value);
		}
	}

	if(env.verbose()) cout << endl;

	env.context().setVariableValue(args[1], results);
}

