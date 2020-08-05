//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SYNCCOMMAND_HPP
#define	__SYNCCOMMAND_HPP

#include "ShellCommand.hpp"

class SyncCommand : public ShellCommand
{
public:
	SyncCommand() {};
	virtual ~SyncCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
