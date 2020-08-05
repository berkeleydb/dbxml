//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PREPARECOMMAND_HPP
#define	__PREPARECOMMAND_HPP

#include "ShellCommand.hpp"

class PrepareCommand : public ShellCommand
{
public:
	PrepareCommand() {};
	virtual ~PrepareCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
