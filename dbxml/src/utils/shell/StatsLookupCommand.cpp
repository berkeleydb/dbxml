//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "StatsLookupCommand.hpp"

using namespace DbXml;
using namespace std;

string StatsLookupCommand::getCommandName() const
{
	return "lookupStats";
}

string StatsLookupCommand::getCommandNameCompat() const
{
	return "statslookup";
}

string StatsLookupCommand::getBriefHelp() const
{
	return "Look up index statistics on the default container";
}

string StatsLookupCommand::getMoreHelp() const
{
	return
		string("Usage: lookupStats <indexDescription> <namespaceUri> <nodeName>\n") +
		string("\t[<parentNodeNamespaceUri> <parentNodeName> <value> ]\n") +
		string("Example -- lookup statistics  in document name index:\n") +
		string("\tlookupStats node-metadata-equality-string \\\n") +
		string("\t    http://www.sleepycat.com/2002/dbxml name\n") +
		string("Example -- lookup stats for a specific document in name index:\n") +
		string("\tlookupStats node-metadata-equality-string \\\n") +
		string("\t    http://www.sleepycat.com/2002/dbxml name \"\" \"\" myDocument\n") +
		string("The optional parent URI and name are used for edge indexes\n") +
		string("Indexes available can be found using the 'listIndexes' command.");
}

void StatsLookupCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 4 || args.size() > 7 || args.size() == 5) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();

	XmlValue value;
	if(args.size() == 7) {
		value = args[6];
	}

	if(env.txn()) {
		if(args.size() == 4 || args[5] == "") {
			XmlStatistics stats = env.container()->
				lookupStatistics(*env.txn(),
						 args[2], args[3],
						 args[1], value);

			cout << "Number of Indexed Keys: " <<
				stats.getNumberOfIndexedKeys() << endl;
			cout << "Number of Unique Keys:  " <<
				stats.getNumberOfUniqueKeys() << endl;
			cout << "Sum Key Value Size:     " <<
				stats.getSumKeyValueSize() << endl << endl;
		}
		else {
			XmlStatistics stats = env.container()->
				lookupStatistics(*env.txn(),
						 args[2], args[3],
						 args[4], args[5],
						 args[1], value);

			cout << "Number of Indexed Keys: " <<
				stats.getNumberOfIndexedKeys() << endl;
			cout << "Number of Unique Keys:  " <<
				stats.getNumberOfUniqueKeys() << endl;
			cout << "Sum Key Value Size:     " <<
				stats.getSumKeyValueSize() << endl << endl;
		}
	} else {
		if(args.size() == 4 || args[5] == "") {
			XmlStatistics stats = env.container()->
				lookupStatistics(args[2], args[3],
						 args[1], value);

			cout << "Number of Indexed Keys: " <<
				stats.getNumberOfIndexedKeys() << endl;
			cout << "Number of Unique Keys:  " <<
				stats.getNumberOfUniqueKeys() << endl;
			cout << "Sum Key Value Size:     " <<
				stats.getSumKeyValueSize() << endl << endl;
		} else {
			XmlStatistics stats = env.container()->
				lookupStatistics(args[2], args[3],
						 args[4], args[5],
						 args[1], value);

			cout << "Number of Indexed Keys: " <<
				stats.getNumberOfIndexedKeys() << endl;
			cout << "Number of Unique Keys:  " <<
				stats.getNumberOfUniqueKeys() << endl;
			cout << "Sum Key Value Size:     " <<
				stats.getSumKeyValueSize() << endl << endl;
		}
	}
}

