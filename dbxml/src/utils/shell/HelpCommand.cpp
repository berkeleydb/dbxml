//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "HelpCommand.hpp"

using namespace DbXml;
using namespace std;

string HelpCommand::getCommandName() const
{
	return "help";
}

string HelpCommand::getCommandNameCompat() const
{
	return "?";
}

string HelpCommand::getBriefHelp() const
{
	return "Print help information.  Use 'help commandName' for extended help";
}

string HelpCommand::getMoreHelp() const
{
	return
		string("Usage: help [commmand]\nIf no command is specified, help prints a summary of the\n") +
		string("commands available.");
}

void HelpCommand::execute(Args &args, Environment &env)
{
	if (args.size() > 2) {
		throw CommandException("Wrong number of arguments");
	}

	vector<ShellCommand*>::iterator end = commands_.end();
	if (args.size() == 1) {
		size_t maxLength = 4;
		for(vector<ShellCommand*>::iterator i = commands_.begin();
		    i != end; ++i) {
			size_t commandLength = (*i)->getCommandName().length();
			if (commandLength > maxLength)
				maxLength = commandLength;
		}

		cout << endl << "Command Summary" << endl;
		cout << "---------------" << endl << endl;

		for(vector<ShellCommand*>::iterator j = commands_.begin();
		    j != end; ++j) {
			string command = (*j)->getCommandName();
			cout << command << string(maxLength - command.length(), ' ') << " - " <<
				(*j)->getBriefHelp() << endl;
		}
	} else {
		bool foundOne = false;
		vector<ShellCommand*>::iterator i = commands_.begin();
		for(; i != end; ++i) {
			if((*i)->matches(args[1])) {
				string command = (*i)->getCommandName();
				cout << endl << command << " -- " <<
					(*i)->getBriefHelp() << endl << endl;
				cout << (*i)->getMoreHelp() << endl;
				foundOne = true;
				// continue, looking for more matches
			}
		}

		if(i == end && !foundOne) {
			throw CommandException("Unknown command: " + args[1]);
		}
	}
}
