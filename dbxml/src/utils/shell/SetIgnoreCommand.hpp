//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SETIGNORECOMMAND_HPP
#define	__SETIGNORECOMMAND_HPP

#include "ShellCommand.hpp"

class SetIgnoreCommand : public ShellCommand
{
public:
	SetIgnoreCommand() {};
	virtual ~SetIgnoreCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
