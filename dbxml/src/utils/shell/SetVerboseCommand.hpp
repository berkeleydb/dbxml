//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SETVERBOSECOMMAND_HPP
#define	__SETVERBOSECOMMAND_HPP

#include "ShellCommand.hpp"

class SetVerboseCommand : public ShellCommand
{
public:
	SetVerboseCommand() {};
	virtual ~SetVerboseCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
