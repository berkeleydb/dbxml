//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DEBUGCOMMAND_HPP
#define	__DEBUGCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class DebugCommand : public ShellCommand
{
public:
	DebugCommand() {};
	virtual ~DebugCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

std::string showErrorContext(const XmlException &e, const char *query);

}

#endif
