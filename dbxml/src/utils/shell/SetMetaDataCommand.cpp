//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetMetaDataCommand.hpp"
#include <iostream>
#include <sstream>

using namespace DbXml;
using namespace std;

string SetMetaDataCommand::getCommandName() const
{
	return "setMetaData";
}

string SetMetaDataCommand::getBriefHelp() const
{
	return "Set a metadata item on the named document";
}

string SetMetaDataCommand::getMoreHelp() const
{
	return
		string("Usage: setMetaData <docName> <metaDataUri> <metaDataName>\n")+
		string("\t<metaDataType> <metaDataValue>\n") +
		string("Set a metaData item for the named document from the string value.\n")+
		string("The type must be specified as a string, and must be one of:\n")+
		string("\tanyURI, base64Binary, boolean, date, dateTime, dayTimeDuration,\n")+
		string("\tdecimal, double, duration, float, gDay, gMonth, gMonthDay, gYear,\n")+
		string("\tgYearMonth, hexBinary, NOTATION,  QName, string, time,\n")+
		string("\tyearMonthDuration, untypedAtomic.\n")+
		string("Example: setMetaData myDoc "" myMD decimal 100\n");
}

void SetMetaDataCommand::execute(Args &args, Environment &env)
{
	if (args.size() != 6)
		throw CommandException("Wrong number of arguments");
	env.testContainer();
	const string &docname = args[1];
	const string &uri = args[2];
	const string &name = args[3];
	const string &type = args[4];
	const string &value = args[5];

	// Figure out XmlValue::Type.
	// There is no direct type from string API at this
	// time, but there is for an index string.
	// This will throw for invalid types
	string indexType = "node-element-equality-" + type;
	XmlValue::Type valType =
		XmlIndexSpecification::getValueType(indexType);
	if (valType == XmlValue::NONE) {
		ostringstream oss;
		oss << "Type name \"" << type << "\" is not a known type";
		throw CommandException(oss.str());
	}

	XmlValue val(valType, value);

	XmlTransaction myTxn;
	if(env.txn()) {
		myTxn = env.childTransaction();
	}

	try {
		// Use DB_RMW -- we're modifying, but use
		// DBXML_LAZY_DOCS because we're not modifying content
		XmlDocument doc = env.container()->getDocument(
			myTxn, docname, DB_RMW|DBXML_LAZY_DOCS);
		doc.setMetaData(uri, name, val);
		env.container()->updateDocument(myTxn, doc, env.uc());
	} catch(XmlException &ex) {
		if(ex.getExceptionCode() == XmlException::DOCUMENT_NOT_FOUND) {
			XmlDocument doc = env.db().createDocument();
			doc.setName(docname);
			doc.setMetaData(uri, name, val);
			env.container()->putDocument(myTxn, doc, env.uc());
		} else {
			throw;
		}
	}

	if(env.txn()) {
		myTxn.commit();
	}

	if(env.verbose()) {
		string fullname = "";
		if (!uri.empty())
			fullname = uri + ":";
		fullname += name;
		cout << "MetaData item '" << fullname << "' added to document "
		     << docname << endl;
	}
}

