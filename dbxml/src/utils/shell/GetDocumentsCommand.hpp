//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __GETDOCUMENTSCOMMAND_HPP
#define	__GETDOCUMENTSCOMMAND_HPP

#include "ShellCommand.hpp"

class GetDocumentsCommand : public ShellCommand
{
public:
	GetDocumentsCommand() {};
	virtual ~GetDocumentsCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
