//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "Shell.hpp"
#include "ShellCommand.hpp"
#include "QuitCommand.hpp"
#include "HelpCommand.hpp"
#include "CommentCommand.hpp"
#include "IncludeCommand.hpp"
#include <algorithm> // for sort()
#include <functional> // for less() override, below
#include "common_utils.h" // for signal handling

using namespace DbXml;
using namespace std;

namespace std {
	template<> struct less<ShellCommand*> {
		bool operator()(const ShellCommand *c1,
				const ShellCommand *c2) const {
			if(!c1)
				return true;
			if(!c2)
				return false;
			return *c1 < c2;
		}
	};
};
	
Shell::Shell()
{
	addCommand(new QuitCommand());
	addCommand(new HelpCommand(commands_));
	addCommand(new CommentCommand);
	addCommand(new IncludeCommand);
}

Shell::~Shell()
{
	for(vector<ShellCommand*>::iterator i = commands_.begin();
	    i != commands_.end(); ++i) {
		delete *i;
	}
}

bool Shell::mainLoop(std::istream &in, Environment &env) const
{
	InputParser iParser;
	ShellCommand::Args args;
	
	const Shell *oldShell = env.shell();
	env.shell() = this;
	
	bool success = true;
	do {
		if(env.interactive()) cout << endl << "dbxml> ";
		env.lineNo() += iParser.parse(in, args);
		string command = args.empty() ? string("") : args.front();
		try {
			ShellCommand *cmd = findCommand(command);
			if (cmd)
				cmd->execute(args, env);
			else if(!args.empty()) {
				throw CommandException("Unknown command");
			}
		}
		//catches XmlException
		catch(exception &e) {
			cerr << env.streamName() << ":" << env.lineNo() <<
				": " << command << " failed, ";
			cerr << e.what() << endl;
			success = false;
			if(!env.interactive() && !env.ignoreErrors())
				env.quit() = true;
		}
		catch(...) {
			cerr << env.streamName() << ":" << env.lineNo() <<
				": " << command << " failed" << endl;
			success = false;
			if(!env.interactive() && !env.ignoreErrors())
				env.quit() = true;
		}
	} while(!env.quit() && !in.eof() && !env.sigBlock().isInterrupted());
	
	env.shell() = oldShell;
	return success;
}

void Shell::addCommand(ShellCommand *command)
{
	commands_.push_back(command);
}

void Shell::sortCommands()
{
	sort(commands_.begin(), commands_.end(),
	     less<ShellCommand*>());
}

ShellCommand *Shell::findCommand(std::string &command) const
{
	vector<ShellCommand*>::const_iterator end =
		commands_.end();
	vector<ShellCommand*>::const_iterator i =
		commands_.begin();
	for(; i != end; ++i) {
		if((*i)->matches(command)) {
			command = (*i)->getCommandName();
			return (*i);
		}
	}
	return 0;
}
