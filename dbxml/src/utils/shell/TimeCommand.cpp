//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "TimeCommand.hpp"
#include "Shell.hpp"
#include "Timer.hpp"

using namespace DbXml;
using namespace std;

string TimeCommand::getCommandName() const
{
	return "time";
}

string TimeCommand::getBriefHelp() const
{
	return "Wrap a command in a wall-clock timer";
}

string TimeCommand::getMoreHelp() const
{
	return
		string("Usage: time command [args]\n")+
		string("This command wraps a timer around the specified command\n")+
		string("and times its execution.  The result is sent to stdout\n");
}

void TimeCommand::execute(Args &args, Environment &env)
{
	if (args.size() < 2)
		throw CommandException("Time command needs at least one argument");
	string command = args[1];
	ShellCommand *cmd =
		env.shell()->findCommand(command);
	if (cmd) {
		// shuffle args
		ShellCommand::Args targs;
		for (unsigned int i = 1; i < args.size(); i++)
			targs.push_back(args[i]);
		try {
			Timer tmr;
			
			tmr.start();
			cmd->execute(targs, env);
			tmr.stop();
			fprintf(stdout,"Time in seconds for command '%s': %g\n",
				args[1].c_str(), tmr.durationInSeconds());
		}
		// exception code is duplicated from Shell.cpp in order
		// to spit out a better error
		catch(exception &e) {
			cerr << env.streamName() << ":" << env.lineNo() <<
				": " << command << " failed, ";
			cerr << e.what() << endl;
			if(!env.interactive()) env.quit() = true;
		}
		catch(...) {
			cerr << env.streamName() << ":" << env.lineNo() <<
				": " << command << " failed" << endl;
			if(!env.interactive()) env.quit() = true;
		}
	} else {
		throw CommandException("Unknown command passed to time command");
	}
}

