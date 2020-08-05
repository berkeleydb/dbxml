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
 * $Id: FunctionSum.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

/*
 * Sum function -- takes a sequence, returns a value of the same type as the items which is 
 *                 the sum of the values.
*/

#ifndef _FUNCTIONSUM_HPP
#define _FUNCTIONSUM_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Sum operator function*/
class XQILLA_API FunctionSum : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSum(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  /** Takes a list of nodes and sums their values */
  Sequence createSequence(DynamicContext* context, int flags=0) const;

  static Item::Ptr sum(const Result &result, DynamicContext *context, const LocationInfo *info,
                       unsigned int *count = 0);

};
#endif // _FUNCTIONSUM_HPP
