//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __COMMITCOMMAND_HPP
#define	__COMMITCOMMAND_HPP

#include "ShellCommand.hpp"

class CommitCommand : public ShellCommand
{
public:
	CommitCommand() {};
	virtual ~CommitCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
