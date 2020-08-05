//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "SetTypedVariableCommand.hpp"

using namespace DbXml;
using namespace std;

string SetTypedVariableCommand::getCommandName() const
{
	return "setTypedVariable";
}

string SetTypedVariableCommand::getCommandNameCompat() const
{
	return "tvarset";
}

string SetTypedVariableCommand::getBriefHelp() const
{
	return "Set a variable to the specified type in the default context";
}

string SetTypedVariableCommand::getMoreHelp() const
{
	return
		string("Usage: setTypedVariable <varName> <varType> <value> (<value>...)\n") +
		string("The varType may be any one of:\n") +
		string("anyURI, base64Binary, boolean, date, dateTime,\n") +
		string("dayTimeDuration, decimal, double, duration, float, gDay, gMonth,\n") +
		string("gMonthDay, gYear, gYearMonth, hexBinary, QName, string, time,\n") +
		string("yearMonthDuration, and untypedAtomic.") +
		string("This command calls XmlQueryContext::setVariableValue().");
}

XmlValue::Type SetTypedVariableCommand::getValueType(string type)
{
	if(type == "anyURI") return XmlValue::ANY_URI;
	if(type == "base64Binary") return XmlValue::BASE_64_BINARY;
	if(type == "boolean") return XmlValue::BOOLEAN;
	if(type == "date") return XmlValue::DATE;
	if(type == "dateTime") return XmlValue::DATE_TIME;
	if(type == "dayTimeDuration") return XmlValue::DAY_TIME_DURATION;
	if(type == "decimal") return XmlValue::DECIMAL;
	if(type == "double") return XmlValue::DOUBLE;
	if(type == "duration") return XmlValue::DURATION;
	if(type == "float") return XmlValue::FLOAT;
	if(type == "gDay") return XmlValue::G_DAY;
	if(type == "gMonth") return XmlValue::G_MONTH;
	if(type == "gMonthDay") return XmlValue::G_MONTH_DAY;
	if(type == "gYear") return XmlValue::G_YEAR;
	if(type == "gYearMonth") return XmlValue::G_YEAR_MONTH;
	if(type == "hexBinary") return XmlValue::HEX_BINARY;
	if(type == "QName") return XmlValue::QNAME;
	if(type == "string") return XmlValue::STRING;
	if(type == "time") return XmlValue::TIME;
	if(type == "yearMonthDuration") return XmlValue::YEAR_MONTH_DURATION;
	if(type == "untypedAtomic") return XmlValue::UNTYPED_ATOMIC;
	return XmlValue::NONE;
}

void SetTypedVariableCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 4) {
		throw CommandException("Wrong number of arguments");
	}
	
	XmlValue::Type type = getValueType(args[2]);
	if(type == XmlValue::NONE) {
		throw CommandException("Unknown value type: " + args[2]);
	}
	
	if(env.verbose()) cout << "Setting $" << args[1] << " as " << args[2] << " =";

	XmlResults results = env.db().createResults();

	for(unsigned int arg = 3; arg < args.size(); ++arg) {
		if(env.verbose()) cout << " " << args[arg];
		results.add(XmlValue(type, args[arg]));
	}

	if(env.verbose()) cout << endl;

	env.context().setVariableValue(args[1], results);
}

