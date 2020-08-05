//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DeleteIndexCommand.hpp"

using namespace DbXml;
using namespace std;

string DeleteIndexCommand::getCommandName() const
{
	return "delIndex";
}

string DeleteIndexCommand::getCommandNameCompat() const
{
	return "delindex";
}

string DeleteIndexCommand::getBriefHelp() const
{
	return "Delete an index from the default container";
}

string DeleteIndexCommand::getMoreHelp() const
{
	return
		string("Usage: delIndex [<nodeNamespaceUri> <nodeName>] <indexDescription>\n") +
		string("If the namespaceUri and name are not set, then this command\n") +
		string("deletes from the default index. This command uses XmlContainer::deleteIndex()\n") +
		string("and XmlContainer::deleteDefaultIndex()\n");
}

void DeleteIndexCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2 && args.size() != 4) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	
	if(env.verbose()) {
		if(args.size() == 2) cout << "Deleting default index type: " << args[1] << endl;
		else cout << "Deleting index type: " << args[3] << " from node: {" << args[1] << "}:" << args[2] << endl;
	}
	
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		if(args.size() == 2) env.container()->deleteDefaultIndex(myTxn, args[1], env.uc());
		else env.container()->deleteIndex(myTxn, args[1], args[2], args[3], env.uc());
		myTxn.commit();
	} else {
		if(args.size() == 2) env.container()->deleteDefaultIndex(args[1], env.uc());
		else env.container()->deleteIndex(args[1], args[2], args[3], env.uc());
	}
}
