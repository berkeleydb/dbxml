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
 * $Id: FunctionBaseURI.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionBaseURI.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

const XMLCh FunctionBaseURI::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionBaseURI::minArgs = 0;
const unsigned int FunctionBaseURI::maxArgs = 1;

/**
 * fn:base-uri() as xs:anyURI?
 * fn:base-uri($arg as node()?) as xs:anyURI?
**/

FunctionBaseURI::FunctionBaseURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

ASTNode* FunctionBaseURI::staticResolution(StaticContext *context) {
  if(!_args.empty() && (*_args.begin())->getType()==ASTNode::CONTEXT_ITEM)
      _args.clear();
  return resolveArguments(context);
}

ASTNode *FunctionBaseURI::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  if(_args.empty())
    _src.contextItemUsed(true);
  _src.getStaticType() = StaticType(StaticType::ANY_URI_TYPE, 0, 1);
  return calculateSRCForArguments(context);
}

Sequence FunctionBaseURI::createSequence(DynamicContext* context, int flags) const
{
  Node::Ptr node = NULL;
  if(getNumArgs() == 1)
  {
    Sequence arg1=getParamNumber(1,context)->toSequence(context);
    if(arg1.isEmpty())
      return Sequence(context->getMemoryManager());
    node = (const Node::Ptr )arg1.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
        XQThrow(FunctionException, X("FunctionBaseURI::createSequence"),X("Undefined context item in fn:base-uri [err:XPDY0002]"));
    if(!item->isNode())
        XQThrow(FunctionException, X("FunctionBaseURI::createSequence"),X("The context item is not a node [err:XPTY0004]"));
    node = (const Node::Ptr )item;
  }
  return node->dmBaseURI(context);
}
