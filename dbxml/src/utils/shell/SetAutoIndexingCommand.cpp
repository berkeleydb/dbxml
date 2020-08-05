//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetAutoIndexingCommand.hpp"

using namespace DbXml;
using namespace std;

string SetAutoIndexingCommand::getCommandName() const
{
	return "setAutoIndexing";
}

string SetAutoIndexingCommand::getCommandNameCompat() const
{
	return "setAutoIndexing";
}

string SetAutoIndexingCommand::getBriefHelp() const
{
	return "Set auto-indexing state of the default container";
}

string SetAutoIndexingCommand::getMoreHelp() const
{
	return
		string("Usage: setAutoIndexing <on|off>\n") +
		string("Sets the auto-indexing state to the specified value.\n") +
		string("The info command returns the current state of auto-indexing.\n");
}

void SetAutoIndexingCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	bool newState, oldState;
	if (args[1].compare("on") == 0)
		newState = true;
	else if (args[1].compare("off") == 0)
		newState = false;
	else {
		string message = "Invalid argument: ";
		message += args[1];
		throw CommandException(message);
	}
	XmlUpdateContext uc = env.db().createUpdateContext();
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		XmlIndexSpecification is = env.container()->getIndexSpecification(myTxn);
		oldState = is.getAutoIndexing();
		is.setAutoIndexing(newState);
		env.container()->setIndexSpecification(myTxn, is, uc);
		myTxn.commit();
	} else {
		XmlIndexSpecification is = env.container()->getIndexSpecification();
		oldState = is.getAutoIndexing();
		is.setAutoIndexing(newState);
		env.container()->setIndexSpecification(is, uc);
	}
	if(env.verbose()) {
		cout << "Set auto-indexing state to " << (newState? "on" : "off") << ", was "
		     << (oldState ? "on" : "off") << endl;
	}
}

