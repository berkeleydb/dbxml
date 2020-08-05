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
 * $Id: FunctionTokenize.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _FUNCTIONTOKENIZE_HPP
#define _FUNCTIONTOKENIZE_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** 
 * Breaks the $input string into a sequence of strings, 
 * treating any substring that matches $pattern as a separator. 
 * The separators themselves are not returned. 
 *
 * tokenize(string? $input, string $pattern) => string*
 * tokenize(string? $input, string $pattern, string $flags) => string*
 */
class XQILLA_API FunctionTokenize : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionTokenize(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /**
   * Returns $input broken into a sequence of strings 
   * using matches to $pattern as a separator. 
   */
  Sequence createSequence(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONTOKENIZE_HPP

