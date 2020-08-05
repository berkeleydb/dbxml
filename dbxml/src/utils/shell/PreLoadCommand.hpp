//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PRELOADCOMMAND_HPP
#define	__PRELOADCOMMAND_HPP

#include "ShellCommand.hpp"

class PreLoadCommand : public ShellCommand
{
public:
	PreLoadCommand() {};
	virtual ~PreLoadCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);

	void info();
private:
	std::vector<DbXml::XmlContainer> containers_;
};

#endif
