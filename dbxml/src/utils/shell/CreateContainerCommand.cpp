//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "CreateContainerCommand.hpp"

using namespace DbXml;
using namespace std;

string CreateContainerCommand::getCommandName() const
{
	return "createContainer";
}

string CreateContainerCommand::getCommandNameCompat() const
{
	return "create";
}

string CreateContainerCommand::getBriefHelp() const
{
	return "Creates a new container, which becomes the default container";
}

string CreateContainerCommand::getMoreHelp() const
{
  return
	  string("Usage: createContainer <containerName> [n|in|d|id] [[no]validate]\n") +
	  string("Creates a new default container; the old default is closed.\n") +
	  string("The default is to create a node storage container, with node indexes.\n")+
	  string("A second argument of \"d\" creates a Wholedoc storage container, and\n") +
	  string("\"id\" creates a document storage container with node indexes.\n")+
	  string("A second argument of \"n\" creates a node storage container, and\n") +
	  string("\"in\" creates a node storage container with node indexes.\n")+
	  string("The optional third argument indicates whether or not to validate\n") +
	  string("documents on insertion\n") +
	  string("A containerName of \"\" creates an in-memory container.\n") +
	  string("This command uses the XmlManager::createContainer() method.\n");
}

void CreateContainerCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 2 || args.size() > 4) {
		throw CommandException("Wrong number of arguments");
	}
	XmlContainer::ContainerType cType = XmlContainer::NodeContainer;
	u_int32_t flags = 0;
	if (args.size() > 2) {
		if(args[2] == "n") {
			cType = XmlContainer::NodeContainer;
			flags |= DBXML_NO_INDEX_NODES;
		} else if(args[2] == "in") {
			cType = XmlContainer::NodeContainer;
			flags |= DBXML_INDEX_NODES;
		} else if (args[2] == "id") {
			cType = XmlContainer::WholedocContainer;
			flags |= DBXML_INDEX_NODES;
		} else if (args[2] == "d") {
			cType = XmlContainer::WholedocContainer;
			flags |= DBXML_NO_INDEX_NODES;
		} else {
			std::string exc("Unknown container type: ");
			exc += args[2];
			throw CommandException(exc);
		}
		if (args.size() == 4)
			if (args[3] == "validate")
				flags |= DBXML_ALLOW_VALIDATION;
	}

	if(env.verbose()) {
		if (cType == XmlContainer::WholedocContainer) {
			cout << "Creating document storage container";
			if (flags & DBXML_INDEX_NODES)
				cout << " with nodes indexed";
		} else {
			cout << "Creating node storage container";
			if (flags & DBXML_INDEX_NODES)
				cout << " with nodes indexed";
		}
		if (flags & DBXML_ALLOW_VALIDATION)
			cout << ", with validation";
		cout << endl;
	}
	
	if(env.transactions()) {
		if(env.txn())
			throw CommandException("In a transactional shell instance you cannot create a container within an explicit transaction.  Created transactions will be transactional. Commit your transaction and try again.");
		flags |= DBXML_TRANSACTIONAL;
	}
	// only modify env.container() on success
	XmlContainer *newContainer = new XmlContainer(
		env.db().createContainer(args[1], flags, cType));
	env.deleteContainer();
	env.container() = newContainer;

	try{
	env.context().setDefaultCollection("dbxml:///" + env.container()->getName() + "/");
	}catch(XmlException e){}
}

