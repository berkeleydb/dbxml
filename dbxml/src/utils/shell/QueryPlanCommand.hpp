//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANCOMMAND_HPP
#define	__QUERYPLANCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class QueryPlanCommand : public ShellCommand
{
public:
	QueryPlanCommand() {};
	virtual ~QueryPlanCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

}

#endif
