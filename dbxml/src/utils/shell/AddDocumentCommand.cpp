//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "AddDocumentCommand.hpp"

using namespace DbXml;
using namespace std;

string AddDocumentCommand::getCommandName() const
{
	return "putDocument";
}

string AddDocumentCommand::getCommandNameCompat() const
{
	return "adddocument";
}

string AddDocumentCommand::getBriefHelp() const
{
	return "Insert a document into the default container";
}

string AddDocumentCommand::getMoreHelp() const
{
	return
		string("Usage: putDocument <namePrefix> <string> [f|s|q]\n") +
		string("Insert a new document in one of 3 ways:\n") +
		string("\t By string content (the default, specify \"s\")\n") +
		string("\t By filename.  String is a file name, specify \"f\"\n") +
		string("\t By XQuery.  String is an XQuery expression, specify \"q\"\n") +
		string("If adding by XQuery, a document is added for each result\n") +
		string("returned by the expression, and DBXML_GEN_NAME is used to\n") +
		string("create unique names.  If adding by string or file, DBXML_GEN_NAME\n") +
		string("is not used, and the insertion will fail if the name is not unique\n") +
		string("within the default container\n");
}

void AddDocumentCommand::execute(Args &args, Environment &env)
{
	if ((args.size() < 3) || (args.size() > 4)){
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.deleteResults();

	if ((args.size() == 4) && (args[3] == "q")) {
		if(env.txn()) {
			XmlTransaction myTxn = env.childTransaction();
			env.results() = new XmlResults(
				env.db().query(myTxn, args[2], env.context()));

			XmlValue value;
			while(env.results()->next(value)) {
				string v = value.asString();
				string name = env.container()->
					putDocument(myTxn, args[1], v, env.uc(),
						    DBXML_GEN_NAME);
				if(env.verbose())
					cout << "Document added, name = " << name << endl;
			}

			myTxn.commit();
		} else {
			env.results() = new XmlResults(env.db().query(args[2],
								      env.context()));
			XmlValue value;
			while(env.results()->next(value)) {
				string v = value.asString();
				string name = env.container()->
					putDocument(args[1], v, env.uc(), DBXML_GEN_NAME);
				if(env.verbose())
					cout << "Document added, name = " << name << endl;
			}

		}
	} else {
		// by string or by file
		bool byString = true;
		if (args.size() == 4 && args[3] == "f")
			byString = false;
		XmlDocument doc = env.db().createDocument();
		doc.setName(args[1]);
		if (byString)
			doc.setContent(args[2]);
		else {
			XmlInputStream *is = env.db().createLocalFileInputStream(args[2]);
			doc.setContentAsXmlInputStream(is);
		}
		if(env.txn()) {
			XmlTransaction myTxn = env.childTransaction();
			env.container()->putDocument(myTxn, doc, env.uc());
			myTxn.commit();
		} else {
			env.container()->putDocument(doc, env.uc());
		}

		// put doc content in last results
		XmlResults res = env.db().createResults();
		res.add(XmlValue(doc));
		env.results() = new XmlResults(res);

		if(env.verbose())
			cout << "Document added, name = " << args[1] << endl;
	}
}

