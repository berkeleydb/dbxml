//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "CommentCommand.hpp"

using namespace DbXml;
using namespace std;

string CommentCommand::getCommandName() const
{
  return "#";
}

string CommentCommand::getBriefHelp() const
{
  return "Comment. Does nothing";
}

string CommentCommand::getMoreHelp() const
{
  return "";
}

// think about handling leading white space...
bool CommentCommand::matches(const string &command) const
{
	return (!command.empty() && (command[0] == '#'));
}

void CommentCommand::execute(Args &args, Environment &env)
{
  // Do nothing
}
