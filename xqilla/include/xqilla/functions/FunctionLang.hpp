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
 * $Id: FunctionLang.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

/*

  lang function

*/

#ifndef _FUNCTIONLANG_HPP
#define _FUNCTIONLANG_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/** lang function. */
class XQILLA_API FunctionLang : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionLang(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  /** The lang function returns true or false depending on whether the language of
      the context node as specified by xml:lang attributes is the same as or is a
      sublanguage of the language specified by the argument string. The language of 
      the context node is determined by the value of the xml:lang attribute on the 
      context node, or, if the context node has no xml:lang attribute, by the value 
      of the xml:lang attribute on the nearest ancestor of the context node that has 
      an xml:lang attribute.**/

  Sequence createSequence(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONLANG_HPP

