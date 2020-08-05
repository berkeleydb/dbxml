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
 * $Id: FunctionHoursFromTime.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _FUNCTIONGETHOURSFROMTIME_HPP
#define _FUNCTIONGETHOURSFROMTIME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/**
 * Getter function for hours component of time.
 *
 * xf:hours-from-time(time? $srcval) => integer?
 */
class XQILLA_API FunctionHoursFromTime : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionHoursFromTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  /**
   * Returns the integer representing the hours in the value of $srcval
   */
  Sequence createSequence(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETHOURSFROMTIME_HPP



