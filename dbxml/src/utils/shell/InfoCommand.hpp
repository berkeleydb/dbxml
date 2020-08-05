//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __INFOCOMMAND_HPP
#define	__INFOCOMMAND_HPP

#include "ShellCommand.hpp"

class InfoCommand : public ShellCommand
{
public:
	InfoCommand() {};
	virtual ~InfoCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
