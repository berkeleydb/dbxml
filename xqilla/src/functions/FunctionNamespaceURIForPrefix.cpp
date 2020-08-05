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
 * $Id: FunctionNamespaceURIForPrefix.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

const XMLCh FunctionNamespaceURIForPrefix::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNamespaceURIForPrefix::minArgs = 2;
const unsigned int FunctionNamespaceURIForPrefix::maxArgs = 2;

/**
 * fn:namespace-uri-for-prefix($prefix as xs:string?, $element as element()) as xs:anyURI?
**/

/** Returns the namespace URI of one of the in-scope namespaces for $element, identified 
 * by its namespace prefix.
**/

FunctionNamespaceURIForPrefix::FunctionNamespaceURIForPrefix(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, element()", args, memMgr)
{
  _src.getStaticType() = StaticType(StaticType::ANY_URI_TYPE, 0, 1);
}

Sequence FunctionNamespaceURIForPrefix::createSequence(DynamicContext* context, int flags) const
{
  Item::Ptr first = getParamNumber(1, context)->next(context);
  const XMLCh* prefix = 0;
  if(first.notNull()) prefix = first->asString(context);

  if(prefix && !*prefix) prefix = 0; 

  return Sequence(uriForPrefix(prefix, (Node*)getParamNumber(2,context)->next(context).get(), context, this), context->getMemoryManager());
}

ATAnyURIOrDerived::Ptr FunctionNamespaceURIForPrefix::uriForPrefix(const XMLCh *prefix, const Node::Ptr &node, DynamicContext *context,
                                                                   const LocationInfo *location)
{
  Result namespaces = node->dmNamespaceNodes(context, location);

  Node::Ptr ns;
  while((ns = (Node*)namespaces->next(context).get()).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      if(prefix == 0) {
        return context->getItemFactory()->createAnyURI(ns->dmStringValue(context), context);
      }
    }
    else if(XPath2Utils::equals(((const ATQNameOrDerived*)name.get())->getName(), prefix)) {
      return context->getItemFactory()->createAnyURI(ns->dmStringValue(context), context);
    }
  }

  return 0;
}
