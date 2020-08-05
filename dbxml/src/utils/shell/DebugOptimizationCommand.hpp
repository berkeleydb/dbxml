//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DEBUGOPTIMIZATIONCOMMAND_HPP
#define	__DEBUGOPTIMIZATIONCOMMAND_HPP

#include "ShellCommand.hpp"

class DebugOptimizationCommand : public ShellCommand
{
public:
	DebugOptimizationCommand() {};
	virtual ~DebugOptimizationCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
