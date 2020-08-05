//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __REMOVEALIASCOMMAND_HPP
#define	__REMOVEALIASCOMMAND_HPP

#include "ShellCommand.hpp"

class RemoveAliasCommand : public ShellCommand
{
public:
	RemoveAliasCommand() {};
	virtual ~RemoveAliasCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
