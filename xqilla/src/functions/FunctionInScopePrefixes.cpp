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
 * $Id: FunctionInScopePrefixes.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionInScopePrefixes.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XMLChCompare.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>

/**
 * fn:in-scope-prefixes($element as element) as xs:string*
**/

const XMLCh FunctionInScopePrefixes::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionInScopePrefixes::minArgs = 1;
const unsigned int FunctionInScopePrefixes::maxArgs = 1;


FunctionInScopePrefixes::FunctionInScopePrefixes(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "element()", args, memMgr)
{
  _src.getStaticType() = StaticType(StaticType::STRING_TYPE, 0, StaticType::UNLIMITED);
}

Sequence FunctionInScopePrefixes::createSequence(DynamicContext* context, int flags) const
{
  Sequence arg1 = getParamNumber(1,context)->toSequence(context);
  Node::Ptr node = (Node::Ptr)arg1.first();

  Sequence returnSeq(context->getMemoryManager());

  Result namespaces = node->dmNamespaceNodes(context, this);
  Node::Ptr ns;
  while((ns = (Node::Ptr)namespaces->next(context)).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      returnSeq.addItem(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context));
    }
    else {
      returnSeq.addItem(context->getItemFactory()->createString(((const ATQNameOrDerived*)name.get())->getName(), context));
    }
  }

  return returnSeq;
}
