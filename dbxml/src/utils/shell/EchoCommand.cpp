//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "EchoCommand.hpp"
#include "Shell.hpp"
#include "PreLoadCommand.hpp"

using namespace DbXml;
using namespace std;

string EchoCommand::getCommandName() const
{
	return "echo";
}

string EchoCommand::getBriefHelp() const
{
	return "Echo to output";
}

string EchoCommand::getMoreHelp() const
{
	return
		string("Usage: echo [text]\n")+
		string("This command echos the (optional) text, followed by a newline\n");
}

void EchoCommand::execute(Args &args, Environment &env)
{
	for (unsigned int i = 1; i < args.size(); i++) {
		if (i > 1)
			cout << " ";
		cout << args[i];
	}
	cout << endl;
}

