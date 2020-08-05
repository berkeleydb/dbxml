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
 * $Id: FunctionMatches.hpp 480 2008-01-29 02:25:38Z jpcs $
 */

#ifndef _FUNCTIONMATCHES_HPP
#define _FUNCTIONMATCHES_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionMatches : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionMatches(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function: returns true if string1 matches the regExp supplied as string2, otherwise returns false. **/
  Sequence createSequence(DynamicContext* context, int flags=0) const;

  static bool matches(const XMLCh *input, const XMLCh *pattern, const XMLCh *options = 0);

};

#endif
