//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "InfoCommand.hpp"
#include "Shell.hpp"
#include "PreLoadCommand.hpp"

using namespace DbXml;
using namespace std;

string InfoCommand::getCommandName() const
{
	return "info";
}

string InfoCommand::getBriefHelp() const
{
	return "Get info on default container";
}

string InfoCommand::getMoreHelp() const
{
	return
		string("Usage: info [preload]\n")+
		string("This command returns information on the shell state and\n")+
		string("the default container.  If the 'preload' argument is used\n")+
		string("information on preloaded containers, if any, will be returned\n");
}

static void doPreload(Environment &env)
{
	std::string name = "preload";
	PreLoadCommand *preload =
		(PreLoadCommand *) (env.shell()->findCommand(name));
	if (preload) {
		preload->info();
	}
}

void InfoCommand::execute(Args &args, Environment &env)
{
	if (args.size() == 2 && args[1] == "preload") {
		doPreload(env);
		return;
	}
	cout << "Version: " << DBXML_VERSION_STRING << endl;
	cout << "         " << DB_VERSION_STRING << endl;
	if (env.container()) {
		const std::string &cname = env.container()->getName();
		XmlContainer::ContainerType ctype = env.container()->getContainerType();
		XmlIndexSpecification is;
		if(env.txn())
			is = env.container()->getIndexSpecification(*env.txn());
		else
			is = env.container()->getIndexSpecification();
		cout << "Default container name: " << cname << endl;
		cout << "Type of default container: " <<
			((ctype == XmlContainer::WholedocContainer) ?
			 "WholedocContainer" : "NodeContainer")  << endl;
		cout << "Index Nodes: " <<
			(env.container()->getIndexNodes() ? "on" : "off") << endl;
		cout << "Auto-indexing: " <<
			(is.getAutoIndexing() ? "on" : "off") << endl;
	} else {
		cout << "No default container" << endl;
	}
	// return type
	// verbosity
	// transacted
	cout << "Shell and XmlManager state:" << endl;
	if (env.transactions()) {
		cout << "\tTransactional, ";
		if (env.txn())
			cout << "active transaction" << endl;
		else
			cout << "no active transaction" << endl;
	} else
		cout << "\tNot transactional" << endl;;
	cout << "\tVerbose: " << (env.verbose() ? "on" : "off") << endl;
	XmlQueryContext::EvaluationType etype = env.context().getEvaluationType();
	cout << "\tQuery context state: LiveValues," <<
		((etype == XmlQueryContext::Eager)? "Eager" : "Lazy") << endl;
}

