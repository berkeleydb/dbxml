//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "ListIndexesCommand.hpp"

using namespace DbXml;
using namespace std;

string ListIndexesCommand::getCommandName() const
{
	return "listIndexes";
}

string ListIndexesCommand::getCommandNameCompat() const
{
	return "listindexes";
}

string ListIndexesCommand::getBriefHelp() const
{
	return "List all indexes in the default container";
}

string ListIndexesCommand::getMoreHelp() const
{
	return
		string("Usage: listIndexes\n") +
		string("This command calls XmlContainer::getIndexSpecification() and iterates\n") +
		string("over the indexes in the XmlIndexSpecification.");
}

void ListIndexesCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 1) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	
	//Retrieve the XmlIndexSpecification from the container
	XmlIndexSpecification idxSpec;
	if(env.txn()) {
		idxSpec = env.container()->getIndexSpecification(*env.txn());
	} else {
		idxSpec = env.container()->getIndexSpecification();
	}
	
	//Lets see what indexes exist on this container
	int count = 0;
	string def = idxSpec.getDefaultIndex();
	if(def != "") {
		cout << "Default Index: " << def << endl;
		++count;
	}
	
	string uri, name, index;
	while(idxSpec.next(uri, name, index)) {
		// Obtain the value as a string and print it to the console
		cout << "Index: " << index << " for node {" << uri << "}:" << name << endl;
		++count;
	}
	
	cout << count << " indexes found." << endl;
}

