//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __HELPCOMMAND_HPP
#define	__HELPCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class HelpCommand : public ShellCommand
{
public:
	HelpCommand(std::vector<ShellCommand*> &commands) : commands_(commands) {};
	virtual ~HelpCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
	
private:
	std::vector<ShellCommand*> &commands_;
};

}

#endif
