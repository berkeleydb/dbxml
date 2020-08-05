//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "ProjectionCommand.hpp"

using namespace DbXml;
using namespace std;

string ProjectionCommand::getCommandName() const
{
	return "setProjection";
}

string ProjectionCommand::getCommandNameCompat() const
{
	return "projection";
}

string ProjectionCommand::getBriefHelp() const
{
	return "Enables or disables the use of the document projection optimization";
}

string ProjectionCommand::getMoreHelp() const
{
	return
		string("Usage: setProjection <on | off>\n") +
		string("This command sets a flag that triggers the use of the DBXML_DOCUMENT_PROJECTION\n"
			"flag on calls to XmlQueryExpression::execute(). The default is to use document\n"
			"projection.");
}

void ProjectionCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(args[1] == "on") {
		env.documentProjection() = true;
		if(env.verbose()) cout << "Document projection on" << endl;
	} else {
		env.documentProjection() = false;
		if(env.verbose()) cout << "Document projection off" << endl;
	}
}

