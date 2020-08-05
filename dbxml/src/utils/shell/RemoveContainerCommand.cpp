//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "RemoveContainerCommand.hpp"

using namespace DbXml;
using namespace std;

string RemoveContainerCommand::getCommandName() const
{
	return "removeContainer";
}

string RemoveContainerCommand::getCommandNameCompat() const
{
	return "removeContainer";
}

string RemoveContainerCommand::getBriefHelp() const
{
	return "Removes a container";
}

string RemoveContainerCommand::getMoreHelp() const
{
  return
	  string("Usage: removeContainer <containerName>\n") +
	  string("Removes the named container.  The container must not be\n")+
	  string("open, or the command will fail.  If the container is\n")+
	  string("the current container, the current results and container\n")+
	  string("are released in order to perform the operation\n");
}

void RemoveContainerCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}

	if (env.container()) {
		const std::string &cname = env.container()->getName();
		if (cname == args[1]) {
			// close current container if it's the one
			env.deleteResults(); // might ref container
			env.deleteContainer();
		}
	}

	if (env.verbose())
		cout << "Removing container: " << args[1] << endl;

	if (env.txn())
		env.db().removeContainer(*env.txn(), args[1]);
	else
		env.db().removeContainer(args[1]);

	if (env.verbose())
		cout << "Container removed" << endl;
}

