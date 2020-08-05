//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetVerboseCommand.hpp"
#include <cstdlib>

using namespace DbXml;
using namespace std;

string SetVerboseCommand::getCommandName() const
{
	return "setVerbose";
}

string SetVerboseCommand::getBriefHelp() const
{
	return "Set the verbosity of this shell";
}

string SetVerboseCommand::getMoreHelp() const
{
	return
		string("Usage: setVerbose <level> <category>\n") +
		string("Level is used for DbXml::setLogLevel()\n")+
		string("Category is used for DbXml::setLogCategory()\n")+
		string("Using 0 0 turns vebosity off.  Values of -1 -1 turn on\n")+
		string("maximum verbosity.  The values are masks, from enumerations\n")+
		string("in the BDB XML library, and can be combined.  For example, to\n")+
		string("turn on INDEXER and optimizer messages, use a category of 0x03.\n")+
		string("Output for XQuery's fn:trace() function is enabled using LEVEL_INFO and CATEGORY_QUERY.\n")+
		string("Numbers can be decimal or hex.  Non-zero level values are:\n") +
		string("\t0x01 -- LEVEL_DEBUG -- program execution tracing\n")+
		string("\t0x02 -- LEVEL_INFO -- informational messages\n")+
		string("\t0x04 -- LEVEL_WARNING -- recoverable warnings\n")+
		string("\t0x08 -- LEVEL_ERROR -- unrecoverable errors\n")+
		string("\t-1 -- LEVEL_ALL --everything\n")+
		string("Non-zero categories are:\n")+
		string("\t0x01 -- CATEGORY_INDEXER -- messages from the indexer\n")+
		string("\t0x02 -- CATEGORY_QUERY -- messages from the query processor\n")+
		string("\t0x04 -- CATEGORY_OPTIMIZER -- messages from the query optimizer\n")+
		string("\t0x08 -- CATEGORY_DICTIONARY -- messages from the name dictionary\n")+
		string("\t0x10 -- CATEGORY_CONTAINER -- messages from container management\n")+
		string("\t0x20 -- CATEGORY_NODESTORE -- messages from node storage management\n")+
		string("\t0x40 -- CATEGORY_MANAGER -- messages from the manager\n")+
		string("\t-1 -- CATEGORY_ALL -- everything\n");
}

void SetVerboseCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}

	// clear current settings
	setLogLevel(LEVEL_ALL, false);
	setLogCategory(CATEGORY_ALL, false);

	int level;
	int category;

	if (args[1].find("0x") == 0)
		::sscanf(args[1].c_str(), "%x", &level);
	else
		level = ::atoi(args[1].c_str());

	if (args[2].find("0x") == 0)
		::sscanf(args[2].c_str(), "%x", &category);
	else
		category = ::atoi(args[2].c_str());

	if (level == 0) {
		env.verbose() = false;
	} else {
		setLogLevel((LogLevel)level, true);
		env.verbose() = true;
	}
	if (category != 0)
		setLogCategory((LogCategory)category, true);
}

