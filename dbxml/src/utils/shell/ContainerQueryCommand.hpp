//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CONTAINERQUERYCOMMAND_HPP
#define	__CONTAINERQUERYCOMMAND_HPP

#include "ShellCommand.hpp"

class ContainerQueryCommand : public ShellCommand
{
public:
	ContainerQueryCommand() {};
	virtual ~ContainerQueryCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
