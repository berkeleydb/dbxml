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
 * $Id: FunctionIndexOf.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

/*
   index-of function: returns the location(s) of the specified item in a sequence
*/

#ifndef _FUNCTIONINDEXOF_HPP
#define _FUNCTIONINDEXOF_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>

class XPath2MemoryManager;
class Collation;

/** index-of function*/
class XQILLA_API FunctionIndexOf : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionIndexOf(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** Returns a sequence of indices where the item occurs in the sequence */
  Sequence indexOf(Sequence &list, const Item::Ptr &item, Collation* collation, DynamicContext* context) const;
  Sequence createSequence(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONINDEXOF_HPP
