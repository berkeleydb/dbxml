//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LAZYCOMMAND_HPP
#define	__LAZYCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class LazyCommand : public ShellCommand
{
public:
	LazyCommand() {};
	virtual ~LazyCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

}

#endif
