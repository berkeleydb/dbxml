//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "GetMetaDataCommand.hpp"

using namespace DbXml;
using namespace std;

string GetMetaDataCommand::getCommandName() const
{
	return "getMetaData";
}

string GetMetaDataCommand::getBriefHelp() const
{
	return "Get a metadata item from the named document";
}

string GetMetaDataCommand::getMoreHelp() const
{
	return
		string("Usage: getMetaData <docName> [<metaDataUri> <metaDataName>]\n")+
		string("Get a metaData item or a list of named metadata items,\n")+
		string("from the named document.  This method\n")+
		string("resets the default results to the returned value.\n")+
		string("This command, when used to get a specific item,\n")+
		string("is equivalent to the query expression:\n")+
		string("\tfor $i in doc('containerName/docName')\n")+
		string("\treturn dbxml:metadata('metaDataUri:metaDataName', $i)\n");
}

static void listMetaData(XmlDocument &doc)
{
	XmlMetaDataIterator it = doc.getMetaDataIterator();
	cout << "Metadata for document: " << doc.getName() << endl;
	XmlValue val;
	string uri, name;
	while (it.next(uri, name, val)) {
		cout << "\t";
		if (uri.size())
			cout << uri << ":";
		cout << name << endl;
	}
}

void GetMetaDataCommand::execute(Args &args, Environment &env)
{
	if ((args.size() != 4) && (args.size() != 2))
		throw CommandException("Wrong number of arguments");
	env.testContainer();
	env.deleteResults();
	const string &docname = args[1];
	string uri;
	string name;
	bool gotOne = false;
	XmlValue value;
	// DBXML_LAZY_DOCS because we're not getting content
	// Value is aggressively copied, so there's no
	// transaction issue.
	if(env.txn()) {
		XmlTransaction myTxn = env.childTransaction();
		XmlDocument doc = env.container()->getDocument(
			myTxn, docname, DBXML_LAZY_DOCS);
		if (args.size() == 4) {
			uri = args[2];
			name = args[3];
			gotOne = doc.getMetaData(uri, name, value);
		} else
			listMetaData(doc);
		myTxn.commit();
	} else {
		XmlDocument doc = env.container()->getDocument(
			docname, DBXML_LAZY_DOCS);
		if (args.size() == 4) {
			uri = args[2];
			name = args[3];
			gotOne = doc.getMetaData(uri, name, value);
		} else
			listMetaData(doc);
	}
	if (args.size() == 2)
		return;
	if (gotOne) {
		XmlResults res = env.db().createResults();
		res.add(value);
		env.results() = new XmlResults(res);
	}
	if(env.verbose()) {
		string fullname = "";
		if (!uri.empty())
			fullname = uri + ":";
		fullname += name;
		if (gotOne)
			cout << "Retrieved metadata item '";
		else
			cout << "Could not find metadata item '";
		cout << fullname << "' in document, " << docname << endl;
	}
}

