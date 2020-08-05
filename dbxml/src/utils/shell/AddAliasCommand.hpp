//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ADDALIASCOMMAND_HPP
#define	__ADDALIASCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class AddAliasCommand : public ShellCommand
{
public:
	AddAliasCommand() {};
	virtual ~AddAliasCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

}

#endif
