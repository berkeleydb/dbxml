//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PROJECTIONCOMMAND_HPP
#define	__PROJECTIONCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class ProjectionCommand : public ShellCommand
{
public:
	ProjectionCommand() {};
	virtual ~ProjectionCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

}

#endif
