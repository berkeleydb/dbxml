//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetQueryTimeoutCommand.hpp"
#include <cstdlib>

using namespace DbXml;
using namespace std;

string SetQueryTimeoutCommand::getCommandName() const
{
	return "setQueryTimeout";
}

string SetQueryTimeoutCommand::getBriefHelp() const
{
	return "Set a query timeout in seconds in the default context";
}

string SetQueryTimeoutCommand::getMoreHelp() const
{
	return
		string("Usage: setQueryTimeout <seconds>\n") +
		string("This command calls the XmlQueryContext::setQueryTimeoutSeconds() method.");
}

void SetQueryTimeoutCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	int secs = atoi(args[1].c_str());
	


	if(env.verbose()) cout << "Setting query timeout to "
			       << secs << " seconds" << endl;
	env.context().setQueryTimeoutSeconds(secs);
}

