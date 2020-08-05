//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SHELL_HPP
#define	__SHELL_HPP
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma warning( disable : 4786 )
#endif

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>
#include <iostream>

#include <xqilla/debug/InputParser.hpp>

class ShellCommand;
class Environment;

class Shell
{
public:
	Shell();
	virtual ~Shell();
	
	/// Returns false if there were errors
	virtual bool mainLoop(std::istream &in, Environment &env) const;
	
	void addCommand(ShellCommand *command);
	void sortCommands();

	ShellCommand *findCommand(std::string &command) const;
private:
	Shell(const Shell &);
	Shell &operator=(const Shell &);

	std::vector<ShellCommand*> commands_;
};

#endif
