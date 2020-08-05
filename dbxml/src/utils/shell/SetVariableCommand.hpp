//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SETVARIABLECOMMAND_HPP
#define	__SETVARIABLECOMMAND_HPP

#include "ShellCommand.hpp"

class SetVariableCommand : public ShellCommand
{
public:
	SetVariableCommand() {};
	virtual ~SetVariableCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
