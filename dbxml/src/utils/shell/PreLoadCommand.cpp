//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "PreLoadCommand.hpp"

using namespace DbXml;
using namespace std;

string PreLoadCommand::getCommandName() const
{
	return "preload";
}

string PreLoadCommand::getBriefHelp() const
{
	return "Pre-loads (opens) a container";
}

string PreLoadCommand::getMoreHelp() const
{
	return
		string("Usage: preload <container>\n") +
		string("Calls XmlManager::openContainer() method to open the container and\n") +
		string("store the resulting object in a vector.  This holds the\n") +
		string("container open for the lifetime of the program.  There is no\n") +
		string("corresponding unload or close command.");
}

void PreLoadCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	containers_.push_back(env.db().openContainer(args[1], env.transactions() ? (u_int32_t)DBXML_TRANSACTIONAL : 0));
}

void PreLoadCommand::info()
{
	int num = 0;
	cout << "Preloaded containers:" << endl;
	for (vector<XmlContainer>::iterator i = containers_.begin();
	     i != containers_.end();
	     i++) {
		++num;
		XmlContainer &c = *i;
		const std::string &cname = c.getName();
		XmlContainer::ContainerType ctype = c.getContainerType();
		cout << "\tContainer name: " << cname << endl;
		cout << "\tType: " <<
			((ctype == XmlContainer::WholedocContainer) ?
			 "WholedocContainer" : "NodeContainer")  << endl;
		cout << "\tIndex Nodes: " <<
			(c.getIndexNodes() ? "on" : "off") << endl;
	}
	if (num == 0)
		cout << "\tNo preloaded containers" << endl;
}
