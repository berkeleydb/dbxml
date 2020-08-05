//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __COMMENTCOMMAND_HPP
#define	__COMMENTCOMMAND_HPP

#include "ShellCommand.hpp"

class CommentCommand : public ShellCommand
{
public:
	CommentCommand() {};
	virtual ~CommentCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	// override matches() from ShellCommand
	bool matches(const std::string &command) const;

	virtual void execute(Args &args, Environment &env);
};

#endif
