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

#ifndef _DEBUGLISTENER_HPP
#define _DEBUGLISTENER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xercesc/util/XercesDefs.hpp>

class StackFrame;
class DynamicContext;
class XQException;

/** 
 * A class used to listen for debugging information
 */
class XQILLA_API DebugListener {
public:
  virtual ~DebugListener() {}

  virtual void start(const StackFrame *stack, const DynamicContext *context) {}
  virtual void end(const StackFrame *stack, const DynamicContext *context) {}
  virtual void enter(const StackFrame *stack, const DynamicContext *context) {}
  virtual void exit(const StackFrame *stack, const DynamicContext *context) {}
  virtual void error(const XQException &error, const StackFrame *stack, const DynamicContext *context) { throw error; }

  virtual bool doLazyEvaluation() const { return true; }
  virtual bool doFocusOptimizations() const { return true; }
};

#endif
