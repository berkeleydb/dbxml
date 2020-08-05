/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id$
 */

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/util/regx/Match.hpp>

// XQilla includes
#include <xqilla/xqilla-simple.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xqilla/debug/InteractiveDebugger.hpp>
#include <xqilla/debug/StackFrame.hpp>
#include <xqilla/debug/InputParser.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////

string BaseInteractiveDebugger::regexFind(const char *regex, const string &str, int groupNo)
{
  try {
    XStr str16(str.c_str());
    RegularExpression re(regex, "mH");

#ifdef HAVE_ALLMATCHES
    RefVectorOf<XERCES_CPP_NAMESPACE_QUALIFIER Match> matches(10, true);
    re.allMatches(str16.str(), 0, XMLString::stringLen(str16.str()), &matches);
    if(matches.size() >= 1) {
      XERCES_CPP_NAMESPACE_QUALIFIER Match *match = matches.elementAt(0);
      if(match->getNoGroups() >= groupNo && match->getStartPos(groupNo) != -1)
        return str.substr(match->getStartPos(groupNo), match->getEndPos(groupNo) - match->getStartPos(groupNo));
    }
#else
    XERCES_CPP_NAMESPACE_QUALIFIER Match match;
    if(re.matches(str16.str(), 0, XMLString::stringLen(str16.str()), &match)) {
      if(match.getNoGroups() >= groupNo && match.getStartPos(groupNo) != -1)
        return str.substr(match.getStartPos(groupNo), match.getEndPos(groupNo) - match.getStartPos(groupNo));
    }
#endif

  }
  catch(XMLException &ex) {
    cerr << "Regex exception: " << UTF8(ex.getMessage()) << endl;
  }
  return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// TBD:
// trace fulltext queries
// examine results

class HelpCommand : public DebugCommand
{
public:
  HelpCommand(std::vector<DebugCommand*> &commands)
    : DebugCommand("help", "?",
                   "Print help information.  Use 'help commandName' for extended help",
                   "Usage: help [commmand]\nIf no command is specified, help prints a summary of the\n"
                   "commands available."),
      commands_(commands)
  {
  }
  
  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    vector<DebugCommand*>::iterator end = commands_.end();
    if (args.size() == 1) {
      size_t maxLength = 4;
      for(vector<DebugCommand*>::iterator i = commands_.begin();
          i != end; ++i) {
        size_t commandLength = string((*i)->getCommandName()).length();
        if (commandLength > maxLength)
          maxLength = commandLength;
      }

      cout << endl << "Command Summary" << endl;
      cout << "---------------" << endl << endl;

      for(vector<DebugCommand*>::iterator j = commands_.begin();
          j != end; ++j) {
        string command = (*j)->getCommandName();
        cout << command << string(maxLength - command.length(), ' ') << " - " <<
          (*j)->getBriefHelp() << endl;
      }
    } else {
      bool foundOne = false;
      vector<DebugCommand*>::iterator i = commands_.begin();
      for(; i != end; ++i) {
        if((*i)->matches(args[1])) {
          string command = (*i)->getCommandName();
          cout << endl << command << " -- " <<
            (*i)->getBriefHelp() << endl << endl;
          cout << (*i)->getMoreHelp() << endl;
          foundOne = true;
          // continue, looking for more matches
        }
      }

      if(i == end && !foundOne) {
        cerr << "Unknown command: " << args[1] << endl;
        return;
      }
    }
  }
  
private:
  std::vector<DebugCommand*> &commands_;
};


class BacktraceCommand : public DebugCommand
{
public:
  BacktraceCommand()
    : DebugCommand("backtrace", "bt",
                   "Prints a stack trace of the currently evaluating query",
                   "Usage: backtrace\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.queryStarted()) {
      cerr << "No query execution in progress." << endl;
    }
    else {
      debugger.stackTrace();
    }
  }
};

class ContinueCommand : public DebugCommand
{
public:
  ContinueCommand()
    : DebugCommand("continue", "",
                   "Continues execution of the query",
                   "Usage: continue\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.queryStarted()) {
      cerr << "No query execution in progress." << endl;
    }
    else {
      throw BaseInteractiveDebugger::Continue();
    }
  }
};

class NextCommand : public DebugCommand
{
public:
  NextCommand()
    : DebugCommand("next", "",
                   "Skips query execution to the end of the current sub-expression",
                   "Usage: next\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.queryStarted()) {
      cerr << "No query execution in progress." << endl;
      return;
    }

    debugger.setNext();
    throw BaseInteractiveDebugger::Continue();
  }
};

class StepCommand : public DebugCommand
{
public:
  StepCommand()
    : DebugCommand("step", "",
                   "Skips query execution forward one sub-expression",
                   "Usage: step\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    debugger.setStep();

    if(debugger.queryStarted()) {
      throw BaseInteractiveDebugger::Continue();
    }

    debugger.run();
  }
};

class RunCommand : public DebugCommand
{
public:
  RunCommand()
    : DebugCommand("run", "",
                   "Runs the query. If the query is already running it will be re-started",
                   "Usage: run\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(debugger.queryStarted()) throw BaseInteractiveDebugger::Run();
    debugger.run();
  }
};

class QuitCommand : public DebugCommand
{
public:
  QuitCommand()
    : DebugCommand("quit", "exit",
                   "Exits the debugger",
                   "Usage: quit\n")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    throw BaseInteractiveDebugger::Quit();
  }
};

class FrameCommand : public DebugCommand
{
public:
  FrameCommand()
    : DebugCommand("frame", "",
                   "Changes the current frame",
                   "Usage: frame <number>\n"
                   "Frames are number from 0 - the currently executing frame.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.changeFrame(atoi(args[1].c_str()))) {
      cerr << "Invalid frame number: " << args[1] << endl;
    }
    else {
      debugger.outputCurrentFrame();
    }
  }
};

class QueryPlanCommand : public DebugCommand
{
public:
  QueryPlanCommand()
    : DebugCommand("queryplan", "qplan",
                   "Prints the query plan for the expression at the current frame",
                   "Usage: queryplan")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    debugger.outputCurrentFrameQueryPlan();
  }
};

class ListCommand : public DebugCommand
{
public:
  ListCommand()
    : DebugCommand("list", "",
                   "Prints the query at the current frame",
                   "Usage: list [line of context]\n"
                   "The optional parameter can be used to specify the number of lines of extra lines of\n"
                   "the queryto be output. This defaults to 6.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() > 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    unsigned int context = 6;
    if(args.size() == 2)
      context = atoi(args[1].c_str());

    if(!debugger.outputCurrentFrame(context)) {
      cerr << "There is no current frame" << endl;
    }
  }
};

class QueryCommand : public DebugCommand
{
public:
  QueryCommand()
    : DebugCommand("query", "print",
                   "Executes a query in the context of the current frame",
                   "Usage: query <query>\n"
                   "This command can be used to examine the in scope variables and context item\n"
                   "for a paticular stack frame.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.queryCurrentFrame(args[1].c_str())) {
      cerr << "There is no current frame" << endl;
    }
  }
};

class BreakCommand : public DebugCommand
{
public:
  BreakCommand()
    : DebugCommand("break", "tbreak",
                   "Sets a break point",
                   "Usage: break [file:line:column]\n"
                   "Usage: tbreak [file:line:column]\n"
                   "If the 'tbreak' variant of the command is given a temporary break point is set, which\n"
                   "becomes disabled when it is reached.\n"
                   "If no arguments are given a break point is set at the current stack frame's position.\n"
                   "If an argument is given it is parsed for colon separated file, line and column\n"
                   "information. If the filename is absent, the name of the current frame's file is used.\n"
                   "The column information can also be left absent to break at any sub-expression on the\n"
                   "specified line.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    bool temporary = args[0][0] == 't';

    string file;
    int line = 0, column = 0;

    string currentFile;
    unsigned int currentLine, currentColumn;
    bool frame = debugger.currentFrameLocation(currentFile, currentLine, currentColumn);

    if(args.size() == 1) {
      if(!debugger.queryStarted() || !frame) {
        cerr << "No query execution in progress." << endl;
        return;
      }
      else {
        file = currentFile;
        line = currentLine;
        column = currentColumn;
      }
    }
    else if(args.size() == 2) {
      static const char *BREAK_REGEX_1 = "([^:0-9][^:]*)(:([0-9]+)(:([0-9]+))?)?";
      static const char *BREAK_REGEX_2 = "([0-9]+)(:([0-9]+))?";

      file = BaseInteractiveDebugger::regexFind(BREAK_REGEX_1, args[1], 1);
      string lstr, cstr;
      if(file != "") {
        lstr = BaseInteractiveDebugger::regexFind(BREAK_REGEX_1, args[1], 3);
        cstr = BaseInteractiveDebugger::regexFind(BREAK_REGEX_1, args[1], 5);
      }
      else {
        lstr = BaseInteractiveDebugger::regexFind(BREAK_REGEX_2, args[1], 1);
        cstr = BaseInteractiveDebugger::regexFind(BREAK_REGEX_2, args[1], 3);
        if(lstr != "" && file == "") {
          if(!debugger.queryStarted() || !frame) {
            cerr << "No query execution in progress." << endl;
            return;
          }
          file = currentFile;
        }
      }
      if(lstr != "") line = atoi(lstr.c_str());
      if(cstr != "") column = atoi(cstr.c_str());
    }
    else {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(line == 0 && args.size() == 2) {
      cerr << "Breakpoint not recognized: " << args[1] << endl;
    }
    else {
      unsigned int bpnum = debugger.setBreakPoint(file, line, column, temporary);
      cout << "Breakpoint #" << bpnum << " set at " << file
           << ":" << line
           << ":" << column << endl;
    }
  }
};

class BreakpointsCommand : public DebugCommand
{
public:
  BreakpointsCommand()
    : DebugCommand("breakpoints", "bps",
                   "Lists the break points set",
                   "Usage: breakpoints")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 1) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    debugger.listBreakPoints();
  }
};

class DisableCommand : public DebugCommand
{
public:
  DisableCommand()
    : DebugCommand("disable", "",
                   "Disables the break point indentified by the numeric argument",
                   "Usage: disable <number>")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.disableBreakPoint(atoi(args[1].c_str()))) {
      cerr << "Invalid break point number: " << args[1] << endl;
    }
  }
};

class EnableCommand : public DebugCommand
{
public:
  EnableCommand()
    : DebugCommand("enable", "",
                   "Enables the break point indentified by the numeric argument",
                   "Usage: enable <number>")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(!debugger.enableBreakPoint(atoi(args[1].c_str()))) {
      cerr << "Invalid break point number: " << args[1] << endl;
    }
  }
};

class ProjectionCommand : public DebugCommand
{
public:
  ProjectionCommand()
    : DebugCommand("projection", "",
                   "Enables or disable document projection optimisations",
                   "Usage: projection <on | off>\nDocument projection is off by default whilst debugging.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(args[1] == "on") {
      debugger.setDoProjection(true);
      cout << "Document projection on" << endl;
    }
    else {
      debugger.setDoProjection(false);
      cout << "Document projection off" << endl;
    }
  }
};

class LazyCommand : public DebugCommand
{
public:
  LazyCommand()
    : DebugCommand("lazy", "",
                   "Enables or disable lazy evaluation optimisations",
                   "Usage: lazy <on | off>\nLazy evaluation is off by default whilst debugging.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(args[1] == "on") {
      debugger.setDoLazyEvaluation(true);
      cout << "Lazy evaluation on" << endl;
    }
    else {
      debugger.setDoLazyEvaluation(false);
      cout << "Lazy evaluation off" << endl;
    }
  }
};

class FocusOptimizationsCommand : public DebugCommand
{
public:
  FocusOptimizationsCommand()
    : DebugCommand("optimizeFocus", "",
                   "Enables or disable focus optimisations",
                   "Usage: optimizeFocus <on | off>\n"
                   "Focus optimizations are off by default whilst debugging. Enabling them can result\n"
                   "in the context item not being set to what the user expects it to be.")
  {
  }

  void execute(InputParser::Args &args, BaseInteractiveDebugger &debugger)
  {
    if(args.size() != 2) {
      cerr << "Wrong number of arguments" << endl;
      return;
    }

    if(args[1] == "on") {
      debugger.setDoFocusOptimizationsn(true);
      cout << "Focus optimizations on" << endl;
    }
    else {
      debugger.setDoFocusOptimizationsn(false);
      cout << "Focus optimizations off" << endl;
    }
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BaseInteractiveDebugger::BaseInteractiveDebugger()
  : prevcmd_(0),
    queryStarted_(false),
    step_(false),
    next_(0)
{
  commands_.push_back(new BacktraceCommand());
  commands_.push_back(new BreakCommand());
  commands_.push_back(new BreakpointsCommand());
  commands_.push_back(new ContinueCommand());
  commands_.push_back(new DisableCommand());
  commands_.push_back(new EnableCommand());
  commands_.push_back(new FocusOptimizationsCommand());
  commands_.push_back(new FrameCommand());
  commands_.push_back(new HelpCommand(commands_));
  commands_.push_back(new LazyCommand());
  commands_.push_back(new ListCommand());
  commands_.push_back(new NextCommand());
  commands_.push_back(new ProjectionCommand());
  commands_.push_back(new QueryCommand());
  commands_.push_back(new QueryPlanCommand());
  commands_.push_back(new QuitCommand());
  commands_.push_back(new RunCommand());
  commands_.push_back(new StepCommand());
}

BaseInteractiveDebugger::~BaseInteractiveDebugger()
{
  for(vector<DebugCommand*>::iterator i = commands_.begin();
      i != commands_.end(); ++i) {
    delete *i;
  }
}

void BaseInteractiveDebugger::readCommand()
{
  InputParser iParser;
  InputParser::Args args;

  do {
    cout << endl << "debug> " << flush;
    
    iParser.parse(cin, args);

    DebugCommand *cmd = prevcmd_;
    if(!args.empty())
      cmd = findCommand(args[0]);

    if(cmd) {
      prevcmd_ = cmd;
      try {
        cmd->execute(args, *this);
      }
      catch(Continue) {
        return;
      }
    }
    else if(!args.empty()) {
      cerr << "Unknown command: " << args[0] << endl;
    }

  } while(!cin.eof());
}

unsigned int BaseInteractiveDebugger::setBreakPoint(const std::string &file, unsigned int line, unsigned int column, bool temporary)
{
  breaks_.push_back(BreakPoint(file, line, column, temporary));
  return (unsigned int) breaks_.size();
}

void BaseInteractiveDebugger::listBreakPoints() const
{
  if(breaks_.empty()) {
    cerr << "No breakpoints set" << endl;
    return;
  }

  unsigned int bpnum = 1;
  vector<BreakPoint>::const_iterator i = breaks_.begin();
  for(; i != breaks_.end(); ++i, ++bpnum) {
    if(i->disabled) {
      cout << "[disabled] ";
    }
    if(i->temporary) {
      cout << "[temporary] ";
    }
    cout << "Breakpoint #" << bpnum << " set at " << i->file
         << ":" << i->line
         << ":" << i->column;
    cout << endl;
    outputLocation(X(i->file.c_str()), i->line, i->column);
  }
}

bool BaseInteractiveDebugger::disableBreakPoint(unsigned int number)
{
  if(number > breaks_.size()) return false;

  breaks_[number - 1].disabled = true;
  return true;
}

bool BaseInteractiveDebugger::enableBreakPoint(unsigned int number)
{
  if(number > breaks_.size()) return false;

  breaks_[number - 1].disabled = false;
  return true;
}

void BaseInteractiveDebugger::setNext()
{
  next_ = getStackSize();
}

void BaseInteractiveDebugger::setStep()
{
  step_ = true;
}

DebugCommand *BaseInteractiveDebugger::findCommand(std::string &command) const
{
  vector<DebugCommand*>::const_iterator end =
    commands_.end();
  vector<DebugCommand*>::const_iterator i =
    commands_.begin();
  for(; i != end; ++i) {
    if((*i)->matches(command)) {
      command = (*i)->getCommandName();
      return (*i);
    }
  }
  return 0;
}

void BaseInteractiveDebugger::checkBreak(bool entering)
{
  if(step_) {
    step_ = false;
  }
  else if(!entering && next_ == getStackSize()) {
    next_ = 0;
    step_ = true;
    return;
  }
  else {
    // Work out the filename and file basename
    string currentFile;
    unsigned int currentLine, currentColumn;
    currentFrameLocation(currentFile, currentLine, currentColumn);

    string file(currentFile);
    string basename = regexFind(".*/(.*)", file);

    // Search to see if we've hit a breakpoint
    int position = 1;
    vector<BreakPoint>::iterator i = breaks_.begin();
    for(; i != breaks_.end(); ++i, ++position) {
      if(i->disabled)
        continue;
      if(i->file != "" && i->file != file && (basename == "" || i->file != basename))
        continue;
      if(i->line != 0 && i->line != currentLine)
        continue;
      if(i->column != 0 && i->column != currentColumn)
        continue;
      break;
    }

    if(i == breaks_.end())
      return;

    // We've hit a breakpoint
    if(i->temporary) {
      i->disabled = true;
    }

    cerr << "Breakpoint #" << position << ", ";
  }

  cerr << (entering ? "Entering " : "Exiting ");
  outputCurrentFrame();

  readCommand();

  if(!entering && next_ == getStackSize()) {
    next_ = 0;
    step_ = true;
  }
}

void BaseInteractiveDebugger::breakForError(const char *message)
{
  cerr << "Error: " << message << endl;
  outputCurrentFrame();

  while(true) {
    // Clear any pending next or step operation
    next_ = 0;
    step_ = false;

    readCommand();

    cerr << "An error has occurred - query execution cannot resume" << endl;
  }
}

void BaseInteractiveDebugger::interrupted()
{
  cerr << "Interrupted!" << endl;
  outputCurrentFrame();

  // Clear any pending next or step operation
  next_ = 0;
  step_ = false;

  readCommand();
}

void InteractiveDebugger::outputLocation(const LocationInfo *info, unsigned int context)
{
  BaseInteractiveDebugger::outputLocation(info->getFile(), info->getLine(), info->getColumn(), context);
}

void BaseInteractiveDebugger::outputLocation(const XMLCh *file, unsigned int line, unsigned int column, unsigned int context)
{
  if(file == 0 || line == 0) return;

  Janitor<InputSource> srcToFill(0);
  try {
    XMLURL urlTmp(file);
    if (urlTmp.isRelative())
      throw MalformedURLException(__FILE__, __LINE__, XMLExcepts::URL_NoProtocolPresent);
    srcToFill.reset(new URLInputSource(urlTmp));
  }
  catch(const MalformedURLException&) {
    srcToFill.reset(new LocalFileInputSource(file));
  }
  XMLBuffer queryText;
  if(!XPath2Utils::readSource(*srcToFill.get(), XMLPlatformUtils::fgMemoryManager, queryText, /*sniffEncoding*/true))
    return;

  outputLocationFromString(queryText.getRawBuffer(), line, column, context);
}

void BaseInteractiveDebugger::outputLocationFromString(const XMLCh *query, unsigned int line, unsigned int column, unsigned int context)
{
  if(line == 0) return;

  unsigned int startLine = line - (context / 2);
  if(line <= (context / 2)) startLine = 1;
  unsigned int endLine = startLine + context + 1;

  unsigned int curLine = 1;
  while(curLine < startLine) {
    if(*query == 0) return;

    if(*query == '\n') {
      ++curLine;
    }
    else if(*query == '\r') {
      ++curLine;
      if(*(query + 1) == '\n') {
        ++query;
      }
    }
    ++query;
  }

  XMLBuffer query_line;

  if(context != 0) {
    while(curLine < endLine) {
      if(*query == 0) break;

      if(*query == '\n' || *query == '\r') {
        if(curLine < 100) cerr << " ";
        if(curLine < 10) cerr << " ";
        cerr << curLine << ": " << UTF8(query_line.getRawBuffer()) << endl;
        query_line.reset();
        ++curLine;
        if(*query == '\r' && *(query + 1) == '\n') {
          ++query;
        }
      }
      else {
        query_line.append(*query);
      }
      ++query;
    }

    if(!query_line.isEmpty()) {
      if(curLine < 100) cerr << " ";
      if(curLine < 10) cerr << " ";
      cerr << curLine << ": " << UTF8(query_line.getRawBuffer()) << endl;
    }
  }
  else {
    XMLBuffer marker_line;
    unsigned int curColumn = 1;
    while(*query != 0) {
      if(curColumn == column) {
        marker_line.append('^');
      }

      if(*query == '\n' || *query == '\r') break;
      query_line.append(*query);

      if(curColumn != column) {
        if(*query == '\t') {
          marker_line.append(*query);
        }
        else {
          marker_line.append(' ');
        }
      }

      ++query;
      ++curColumn;
    }

    cerr << UTF8(query_line.getRawBuffer()) << endl << UTF8(marker_line.getRawBuffer()) << endl;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractiveDebugger::debugQuery(const XQQuery *query, DynamicContext *context)
{
  InteractiveDebugger debugger(query, context);

  try {
    debugger.readCommand();
  }
  catch(Quit) {
  }
}

InteractiveDebugger::InteractiveDebugger(const XQQuery *query, DynamicContext *context)
  : stack_(0),
    currentFrame_(0),
    query_(query),
    context_(context),
    lazy_(false),
    focusOptimzations_(false)
{
  context->setDebugListener(this);
  context->setProjection(false);
}

void InteractiveDebugger::run()
{
  while(true) {
    next_ = 0;
    stack_ = 0;
    currentFrame_ = 0;

    try {
      queryStarted_ = true;

      StdOutFormatTarget target;
      EventSerializer writer((char*)"UTF-8", (char*)"1.1", &target, context_->getMemoryManager());
      writer.addNewlines(true);
      NSFixupFilter nsfilter(&writer, context_->getMemoryManager());
      query_->execute(&nsfilter, context_);

      queryStarted_ = false;
      cout << endl << "Query completed." << endl;
      return;
    }
    catch(Run) {
      queryStarted_ = false;
    }
    catch(Quit) {
      throw;
    }
    catch(XQException &e) {
      queryStarted_ = false;
      cerr << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn()
           << ": error: " << UTF8(e.getError()) << endl;
      BaseInteractiveDebugger::outputLocation(e.getXQueryFile(), e.getXQueryLine(), e.getXQueryColumn());
      break;
    }
    catch(...) {
      queryStarted_ = false;
      cerr << "Caught unknown exception" << endl;
      break;
    }
  }
}

void InteractiveDebugger::enter(const StackFrame *stack, const DynamicContext *context)
{
  AutoReset<const StackFrame *> reset(stack_);
  AutoReset<const StackFrame *> reset2(currentFrame_);
  stack_ = stack;
  currentFrame_ = stack;

  checkBreak(/*entering*/true);
}

void InteractiveDebugger::exit(const StackFrame *stack, const DynamicContext *context)
{
  AutoReset<const StackFrame *> reset(stack_);
  AutoReset<const StackFrame *> reset2(currentFrame_);
  stack_ = stack;
  currentFrame_ = stack;

  checkBreak(/*entering*/false);
}

void InteractiveDebugger::error(const XQException &error, const StackFrame *stack, const DynamicContext *context)
{
  AutoReset<const StackFrame *> reset(stack_);
  AutoReset<const StackFrame *> reset2(currentFrame_);
  stack_ = stack;
  currentFrame_ = stack;

  breakForError(UTF8(error.getError()));
}

bool InteractiveDebugger::changeFrame(unsigned int number)
{
  const StackFrame *frame = stack_;
  unsigned int count = 0;
  while(frame && count < number) {
    frame = frame->getPreviousFrame();
    ++count;
  }

  if(frame == 0 || count < number) return false;
  currentFrame_ = frame;
  return true;
}

unsigned int InteractiveDebugger::getCurrentFrameNumber() const
{
  const StackFrame *frame = stack_;
  unsigned int count = 0;
  while(frame && frame != currentFrame_) {
    frame = frame->getPreviousFrame();
    ++count;
  }
  return count;
}

unsigned int InteractiveDebugger::getStackSize() const
{
  const StackFrame *frame = stack_;
  unsigned int count = 0;
  while(frame) {
    frame = frame->getPreviousFrame();
    ++count;
  }
  return count;
}

void InteractiveDebugger::stackTrace() const
{
  int count = 0;
  const StackFrame *frame = stack_;
  while(frame) {
    cerr << "#" << count;
    output(frame);
    cerr << endl;

    frame = frame->getPreviousFrame();
    ++count;
  }
}

bool InteractiveDebugger::outputCurrentFrame(unsigned int context) const
{
  if(currentFrame_ == 0) return false;

	cerr << "#" << getCurrentFrameNumber();
  output(currentFrame_);
  cerr << endl << endl;
  outputLocation(currentFrame_->getLocationInfo(), context);

  return true;
}

void InteractiveDebugger::outputCurrentFrameQueryPlan() const
{
  if(currentFrame_ == 0) {
    cout << PrintAST::print(query_, context_, 0) << endl;;
  }
  else {
    cout << currentFrame_->getQueryPlan() << endl;;
  }
}

bool InteractiveDebugger::queryCurrentFrame(const char *queryString) const
{
  if(currentFrame_ == 0) return false;

  XStr query16(queryString);
  try {
    XPath2MemoryManager *memMgr = context_->getMemoryManager();

    StdOutFormatTarget target;
    EventSerializer writer((char*)"UTF-8", (char*)"1.1", &target, memMgr);
    writer.addNewlines(true);
    NSFixupFilter nsfilter(&writer, memMgr);

    currentFrame_->query(query16.str(), &nsfilter);
  }
  catch(XQException &e) {
    cerr << UTF8(e.getXQueryFile()) << ":" << e.getXQueryLine() << ":" << e.getXQueryColumn()
         << ": error: " << UTF8(e.getError()) << endl;
    if(e.getXQueryFile() == 0)
      BaseInteractiveDebugger::outputLocationFromString(query16.str(), e.getXQueryLine(), e.getXQueryColumn());
    else
      BaseInteractiveDebugger::outputLocation(e.getXQueryFile(), e.getXQueryLine(), e.getXQueryColumn());
  }
  catch(...) {
    cerr << "Caught unknown exception" << endl;
  }

  return true;
}

bool InteractiveDebugger::currentFrameLocation(string &file, unsigned int &line, unsigned int &column) const
{
  if(currentFrame_ == 0) {
    file = UTF8(query_->getFile());
    line = 0;
    column = 0;
    return false;
  }

  file = UTF8(currentFrame_->getLocationInfo()->getFile());
  line = currentFrame_->getLocationInfo()->getLine();
  column = currentFrame_->getLocationInfo()->getColumn();
  return true;
}

void InteractiveDebugger::setDoProjection(bool opt)
{
  context_->setProjection(opt);
}
  
void InteractiveDebugger::output(const StackFrame *frame) const
{
  string where = regexFind("(<[^>]+>)", frame->getQueryPlan());
  if(where != "") cerr << " in " << where;

  const LocationInfo *loc = frame->getLocationInfo();
  cerr << " at " << UTF8(loc->getFile()) << ":" << loc->getLine() << ":" << loc->getColumn();
}

void InteractiveDebugger::report(const StackFrame *frame) const
{
  const LocationInfo *loc = frame->getLocationInfo();
  cerr << UTF8(loc->getFile()) << ":" << loc->getLine() << ":" << loc->getColumn() << endl;
  outputLocation(loc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void lowerCase(string &s)
{
  for (unsigned int i = 0; i < s.length(); ++i)
    s[i] = tolower(s[i]);
}

bool DebugCommand::matches(const std::string &command,
                           const std::string &toMatch)
{
  // Be case-insensitive, for convenience
  if (command.length() == 0)
    return false;
  string l_command = command;
  string l_match = toMatch;
  lowerCase(l_command);
  lowerCase(l_match);
  
  return l_match.substr(0, l_command.length()) == l_command;
}

bool DebugCommand::matches(const std::string &command) const
{
  return matches(command, name_) || matches(command, compatName_);
}
