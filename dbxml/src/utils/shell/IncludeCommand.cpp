//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <fstream>

#include "IncludeCommand.hpp"
#include "Shell.hpp"

using namespace DbXml;
using namespace std;

string IncludeCommand::getCommandNameCompat() const
{
  return "include";
}

string IncludeCommand::getCommandName() const
{
  return "run";
}

string IncludeCommand::getBriefHelp() const
{
  return "Runs the given file as a script";
}

string IncludeCommand::getMoreHelp() const
{
  return "Usage: run <scriptFile>";
}

void IncludeCommand::execute(Args &args, Environment &env)
{
  if(args.size() != 2) {
    throw CommandException("Wrong number of arguments");
  }

  string oldStreamName = env.streamName();
  int oldLineNo = env.lineNo();
  bool oldInteractive = env.interactive();
  try {

    ifstream scriptFile(args[1].c_str(), ios::in);
    if(!scriptFile) {
      throw CommandException("Cannot open file: " + args[1]);
    }
    else {
      env.streamName() = args[1];
      env.lineNo() = 0;
      env.interactive() = false;

      bool success = env.shell()->mainLoop(scriptFile, env);

      scriptFile.close();

      if(!success) {
        // There was an error!
        throw CommandException("error in included file");
      }
    }

  }
  catch(...) {
    env.streamName() = oldStreamName;
    env.lineNo() = oldLineNo;
    env.interactive() = oldInteractive;
    env.quit() = false;
    throw;
  }
  env.streamName() = oldStreamName;
  env.lineNo() = oldLineNo;
  env.interactive() = oldInteractive;
  env.quit() = false;
}
