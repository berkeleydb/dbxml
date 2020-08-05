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
 * $Id: FunctionDocAvailable.cpp 659 2008-10-06 00:11:22Z jpcs $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionDocAvailable::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDocAvailable::minArgs = 1;
const unsigned int FunctionDocAvailable::maxArgs = 1;

/**
 * fn:doc-available($uri as xs:string?) as xs:boolean
 **/
FunctionDocAvailable::FunctionDocAvailable(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

ASTNode* FunctionDocAvailable::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionDocAvailable::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::BOOLEAN_TYPE;
  _src.availableDocumentsUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionDocAvailable::createSequence(DynamicContext* context, int flags) const {
  Sequence uriArg = getParamNumber(1,context)->toSequence(context);
  
  if (uriArg.isEmpty()) {
    return Sequence(context->getMemoryManager());
  }
  
  const XMLCh* uri = uriArg.first()->asString(context);
  // on Windows, we can have URIs using \ instead of /; let's normalize them
  XMLCh backSlash[]={ XERCES_CPP_NAMESPACE_QUALIFIER chBackSlash, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::findAny(uri,backSlash))
  {
	  XMLCh* newUri=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(uri,context->getMemoryManager());
	  for(unsigned int i=0;i<XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(newUri);i++)
		  if(newUri[i]==XERCES_CPP_NAMESPACE_QUALIFIER chBackSlash)
			  newUri[i]=XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash;
	  uri=newUri;
  }
  if(!XPath2Utils::isValidURI(uri, context->getMemoryManager()))
    XQThrow(FunctionException, X("FunctionDocAvailable::createSequence"), X("Invalid argument to fn:doc-available function [err:FODC0005]"));

  bool bSuccess=false;
  try {
    bSuccess = !context->resolveDocument(uri, this).isEmpty();
  } 
  catch(...) {
  }
  return Sequence(context->getItemFactory()->createBoolean(bSuccess, context), context->getMemoryManager());
}
