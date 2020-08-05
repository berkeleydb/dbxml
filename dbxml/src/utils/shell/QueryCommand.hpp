//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYCOMMAND_HPP
#define	__QUERYCOMMAND_HPP

#include "ShellCommand.hpp"

namespace DbXml
{

class QueryCommand : public ShellCommand
{
public:
	QueryCommand() {};
	virtual ~QueryCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

}

#endif
