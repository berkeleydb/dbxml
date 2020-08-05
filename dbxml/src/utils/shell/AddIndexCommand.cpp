//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "AddIndexCommand.hpp"

using namespace DbXml;
using namespace std;

string AddIndexCommand::getCommandName() const
{
	return "addIndex";
}

string AddIndexCommand::getCommandNameCompat() const
{
	return "addindex";
}

string AddIndexCommand::getBriefHelp() const
{
	return "Add an index to the default container";
}

string AddIndexCommand::getMoreHelp() const
{
	return
		string("Usage: addIndex [<nodeNamespaceUri> <nodeName>] <indexDescription>\n") +
		string("If the namespaceUri and name are not set, then this command\n") +
		string("adds to the default index. This command uses XmlContainer::addIndex()\n") +
		string("and XmlContainer::addDefaultIndex()\n");
}

void AddIndexCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2 && args.size() != 4) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	
	if(env.verbose()) {
		if(args.size() == 2) cout << "Adding default index type: " << args[1] << endl;
		else cout << "Adding index type: " << args[3] << " to node: {" << args[1] << "}:" << args[2] << endl;
	}
	
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		if(args.size() == 2) env.container()->addDefaultIndex(myTxn, args[1], env.uc());
		else env.container()->addIndex(myTxn, args[1], args[2], args[3], env.uc());
		myTxn.commit();
	} else {
		if(args.size() == 2) env.container()->addDefaultIndex(args[1], env.uc());
		else env.container()->addIndex(args[1], args[2], args[3], env.uc());
	}
}

