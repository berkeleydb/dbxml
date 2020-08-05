//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SETQUERYTIMEOUTCOMMAND_HPP
#define	__SETQUERYTIMEOUTCOMMAND_HPP

#include "ShellCommand.hpp"

class SetQueryTimeoutCommand : public ShellCommand
{
public:
	SetQueryTimeoutCommand() {};
	virtual ~SetQueryTimeoutCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
