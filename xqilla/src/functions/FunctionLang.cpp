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
 * $Id: FunctionLang.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionLang.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>

const XMLCh FunctionLang::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionLang::minArgs = 1;
const unsigned int FunctionLang::maxArgs = 2;

/**
 * fn:lang($testlang as xs:string?) as xs:boolean
 * fn:lang($testlang as xs:string?, $node as node()) as xs:boolean
**/

FunctionLang::FunctionLang(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?,node()", args, memMgr)
{
}

ASTNode* FunctionLang::staticResolution(StaticContext *context) {
  if(_args.size() == 2 && _args[1]->getType() == ASTNode::CONTEXT_ITEM)
    _args.pop_back();
  return resolveArguments(context);
}

ASTNode *FunctionLang::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::BOOLEAN_TYPE;
  if(_args.size()==1)
    _src.contextItemUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionLang::createSequence(DynamicContext* context, int flags) const
{
    XPath2MemoryManager* memMgr = context->getMemoryManager();

    Sequence arg1=getParamNumber(1, context)->toSequence(context);
    const XMLCh *param1 = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    if(!arg1.isEmpty())
        param1=arg1.first()->asString(context);

    const XMLCh* xmlLangValue = XPath2Utils::toLower(param1, context->getMemoryManager());
    bool isSubLang = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(xmlLangValue, XERCES_CPP_NAMESPACE_QUALIFIER chDash) > 0;

    Node::Ptr ctxNode;
    if(getNumArgs() == 2)
    {
        Sequence arg=getParamNumber(2,context)->toSequence(context);
        ctxNode=arg.first();
    }
    else
    {
        const Item::Ptr item = context->getContextItem();
        if(item==NULLRCP)
            XQThrow(FunctionException, X("FunctionLang::createSequence"),X("Undefined context item in fn:lang [err:XPDY0002]"));
        if(!item->isNode())
            XQThrow(FunctionException, X("FunctionLang::createSequence"),X("The context item is not a node [err:XPTY0004]"));
        ctxNode=item;
    }

    static const XMLCh xmlLang[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                     XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,
                                     XERCES_CPP_NAMESPACE_QUALIFIER chNull};

    static const XMLCh* xmlLangNamespace = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName;

    Node::Ptr node = ctxNode;
    while(node.notNull())
    {
      Result attrs = node->dmAttributes(context, this);
      Node::Ptr att;
      while((att = (Node::Ptr)attrs->next(context)).notNull()) {
        ATQNameOrDerived::Ptr name = att->dmNodeName(context);
        if(name.notNull()) {
          const XMLCh *node_uri = ((const ATQNameOrDerived*)name.get())->getURI();
          const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

          if(XPath2Utils::equals(xmlLang, node_name) &&
             XPath2Utils::equals(xmlLangNamespace, node_uri)) {
            const XMLCh *value = att->dmStringValue(context);

            const XMLCh* asLower = XPath2Utils::toLower(value, context->getMemoryManager());
            int dashLocation = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(asLower, XERCES_CPP_NAMESPACE_QUALIFIER chDash);

            if(dashLocation!=-1 && !isSubLang) {
              asLower = XPath2Utils::subString(asLower, 0, dashLocation, memMgr);
            }

            return Sequence(context->getItemFactory()->createBoolean(XPath2Utils::equals(asLower, xmlLangValue), context), memMgr);
          }
        }
      }

      node = node->dmParent(context);
    }

    return Sequence(context->getItemFactory()->createBoolean(false, context), memMgr);
}
