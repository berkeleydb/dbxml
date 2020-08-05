//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __REINDEXCOMMAND_HPP
#define	__REINDEXCOMMAND_HPP

#include "ShellCommand.hpp"

class ReindexCommand : public ShellCommand
{
public:
	ReindexCommand() {};
	virtual ~ReindexCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
