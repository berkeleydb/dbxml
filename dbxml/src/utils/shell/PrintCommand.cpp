//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "PrintCommand.hpp"
#include <fstream>
#include <stdlib.h>

using namespace DbXml;
using namespace std;

string PrintCommand::getCommandName() const
{
	return "print";
}

string PrintCommand::getCommandNameCompat() const
{
	return "printNames";
}

string PrintCommand::getBriefHelp() const
{
	return "Prints most recent results, optionally to a file";
}

string PrintCommand::getMoreHelp() const
{
	return
		string("Usage: print | printNames [n <number>] [pathToFile]\n") +
		string("If \"print\" is used, the most recent results are printed,\n") +
		string("using XmlValue::asString().\n") +
		string("If the \"printNames\" form is used, the results are turned\n") +
		string("into document names and printed, if possible.  If the results\n") +
		string("cannot be converted, the command will fail.  If the optional\n")+
		string("argument \"n\" is specified followed by a number, then only the\n") +
		string("specified number of results are printed. If the optional\n") +
		string("pathToFile parameter is specified, the output is written to\n")+
		string("the named file, rather than to stdout.\n");
}

void PrintCommand::execute(Args &args, Environment &env)
{
	if(args.size() > 4) {
		throw CommandException("Wrong number of arguments");
	}

	bool content = false;
	bool toFile = false;
	ofstream outputFile;
	if (args[0] == "print" || args[0].length() <= 5)
		content = true;

	int number = -1;
	if(args.size() == 3 || args.size() == 4) {
		if(args[1] != "n") {
			throw CommandException("Bad argument: " + args[1]);
		}
		number = atoi(args[2].c_str());
	}

	if (args.size() == 2 || args.size() == 4) {
		outputFile.open(args[args.size() - 1].c_str(), ofstream::out|ofstream::trunc|
				ofstream::binary);
		toFile = true;
	}

	env.testResults();
	env.results()->reset();
	XmlValue value;
	ostream &output = (toFile ? (ostream &)outputFile : cout);
	int count = 0;
	while(env.results()->next(value) && (number == -1 || count < number)) {
		if(env.sigBlock().isInterrupted()) {
			env.sigBlock().reset();
			throw CommandException("Print interrupted");
		}

		if (content)
			output << value.asString() << endl;
		else {
			output << value.asDocument().getName() << endl;
		}
		++count;
	}
	if (toFile)
		outputFile.close();
}

