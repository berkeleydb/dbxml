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
 * $Id: MessageListener.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _INTERACTIVEDEBUGGER_HPP
#define _INTERACTIVEDEBUGGER_HPP

#include <string>
#include <vector>
#include <map>

#include <xqilla/debug/DebugListener.hpp>
#include <xqilla/ast/LocationInfo.hpp>

#include <xqilla/debug/InputParser.hpp>

class XQQuery;
class DynamicContext;
class DebugCommand;
class InteractiveDebugger;

class XQILLA_API BaseInteractiveDebugger
{
public:
  struct XQILLA_API Run {};
  struct XQILLA_API Continue {};
  struct XQILLA_API Quit {};

  static void outputLocation(const XMLCh *file, unsigned int line, unsigned int column,
                             unsigned int context = 0);
  static void outputLocationFromString(const XMLCh *query, unsigned int line, unsigned int column,
                                       unsigned int context = 0);
  static std::string regexFind(const char *regex, const std::string &str, int groupNo = 1);

  virtual ~BaseInteractiveDebugger();

  unsigned int setBreakPoint(const std::string &file, unsigned int line, unsigned int column, bool temporary);
  bool disableBreakPoint(unsigned int number);
  bool enableBreakPoint(unsigned int number);
  void listBreakPoints() const;

  void setStep();
  void setNext();
  bool queryStarted() const { return queryStarted_; }

  virtual void run() = 0;

  virtual bool changeFrame(unsigned int number) = 0;
  virtual unsigned int getStackSize() const = 0;
  virtual void stackTrace() const = 0;
  virtual bool outputCurrentFrame(unsigned int context = 0) const = 0;
  virtual void outputCurrentFrameQueryPlan() const = 0;
  virtual bool queryCurrentFrame(const char *queryString) const = 0;
  virtual bool currentFrameLocation(std::string &file, unsigned int &line, unsigned int &column) const = 0;

  virtual void setDoLazyEvaluation(bool lazy) = 0;
  virtual void setDoFocusOptimizationsn(bool opt) = 0;
  virtual void setDoProjection(bool opt) = 0;

protected:
  BaseInteractiveDebugger();

  DebugCommand *findCommand(std::string &command) const;
  void checkBreak(bool entering);
  void breakForError(const char *message);
  void interrupted();
  void readCommand();

  std::vector<DebugCommand*> commands_;
  DebugCommand *prevcmd_;

  bool queryStarted_;

  struct BreakPoint
  {
    BreakPoint(const std::string &f, unsigned int l, unsigned int c, bool t)
      : file(f), line(l), column(c), temporary(t), disabled(false) {}

    std::string file;
    unsigned int line, column;
    bool temporary;
    bool disabled;
  };

  std::vector<BreakPoint> breaks_;
  bool step_;
  unsigned int next_;
};

class XQILLA_API DebugCommand
{
public:
  virtual ~DebugCommand() {};

  virtual const char *getCommandName() const { return name_; }
  virtual const char *getCommandNameCompat() const { return compatName_; }
  virtual const char *getBriefHelp() const { return briefHelp_; }
  virtual const char *getMoreHelp() const { return moreHelp_; }

  static bool matches(const std::string &command,
          const std::string &toMatch);
  virtual bool matches(const std::string &command) const;

  virtual void execute(InputParser::Args &args, BaseInteractiveDebugger &env) = 0;

protected:
  DebugCommand(const char *name, const char *compatName,
               const char *briefHelp, const char *moreHelp)
    : name_(name), compatName_(compatName), briefHelp_(briefHelp), moreHelp_(moreHelp) {}

  const char *name_;
  const char *compatName_;
  const char *briefHelp_;
  const char *moreHelp_;
};

class XQILLA_API InteractiveDebugger :  private BaseInteractiveDebugger,
                                        private DebugListener
{
public:
  static void debugQuery(const XQQuery *query, DynamicContext *context);
  static void outputLocation(const LocationInfo *info, unsigned int context = 0);

private:
  InteractiveDebugger(const XQQuery *query, DynamicContext *context);

  virtual void enter(const StackFrame *stack, const DynamicContext *context);
  virtual void exit(const StackFrame *stack, const DynamicContext *context);
  virtual void error(const XQException &error, const StackFrame *stack, const DynamicContext *context);
  virtual bool doLazyEvaluation() const { return lazy_; }
  virtual bool doFocusOptimizations() const { return focusOptimzations_; }

  virtual void run();

  virtual bool changeFrame(unsigned int number);
  virtual unsigned int getStackSize() const;
  virtual void stackTrace() const;
  virtual bool outputCurrentFrame(unsigned int context) const;
  virtual void outputCurrentFrameQueryPlan() const;
  virtual bool queryCurrentFrame(const char *queryString) const;
  virtual bool currentFrameLocation(std::string &file, unsigned int &line, unsigned int &column) const;

  virtual void setDoLazyEvaluation(bool lazy) { lazy_ = lazy; }
  virtual void setDoFocusOptimizationsn(bool opt) { focusOptimzations_ = opt; }
  virtual void setDoProjection(bool opt);

  unsigned int getCurrentFrameNumber() const;
  void output(const StackFrame *frame) const;
  void report(const StackFrame *frame) const;

  const StackFrame *stack_;
  const StackFrame *currentFrame_;

  const XQQuery *query_;
  DynamicContext *context_;
  bool lazy_;
  bool focusOptimzations_;
};

#endif
