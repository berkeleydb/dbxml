//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LISTINDEXESCOMMAND_HPP
#define	__LISTINDEXESCOMMAND_HPP

#include "ShellCommand.hpp"

class ListIndexesCommand : public ShellCommand
{
public:
	ListIndexesCommand() {};
	virtual ~ListIndexesCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
