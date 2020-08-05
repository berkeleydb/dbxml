//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ABORTCOMMAND_HPP
#define	__ABORTCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class AbortCommand : public ShellCommand
{
public:
	AbortCommand() {};
	virtual ~AbortCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

}

#endif
