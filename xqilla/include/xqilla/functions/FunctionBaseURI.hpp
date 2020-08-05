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
 * $Id: FunctionBaseURI.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

/*

   base-uri function

*/

#ifndef _FUNCTIONBASEURI_HPP
#define _FUNCTIONBASEURI_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>

/** base-uri function */
class XQILLA_API FunctionBaseURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionBaseURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

	/* returns the base-URI property of document or element nodes,
	 * and the empty sequence for all other kinds of nodes */
  Sequence createSequence(DynamicContext* context, int flags=0) const;
};

#endif
