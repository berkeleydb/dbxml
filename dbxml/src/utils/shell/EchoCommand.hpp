//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ECHOCOMMAND_HPP
#define	__ECHOCOMMAND_HPP

#include "ShellCommand.hpp"

class EchoCommand : public ShellCommand
{
public:
	EchoCommand() {};
	virtual ~EchoCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
