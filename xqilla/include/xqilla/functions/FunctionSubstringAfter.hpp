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
 * $Id: FunctionSubstringAfter.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

/*

  Substring-after function

*/

#ifndef _FUNCTIONSUBSTRINGAFTER_HPP
#define _FUNCTIONSUBSTRINGAFTER_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Substring-after function. */
class XQILLA_API FunctionSubstringAfter : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstringAfter(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns the substring of the first string following the first occurrence of the second string within that string **/
  Sequence createSequence(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRINGAFTER_HPP
