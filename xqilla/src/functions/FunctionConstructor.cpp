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
 * $Id: FunctionConstructor.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

/**
 * pref:TYPE($arg as xdt:anyAtomicType) as pref:TYPE
**/

 /* http://www.w3.org/2001/XMLSchema */
const XMLCh FunctionConstructor::XMLChXPath2DatatypesURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,       XERCES_CPP_NAMESPACE_QUALIFIER chColon,         XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_S,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

FunctionConstructor::FunctionConstructor(const XMLCh* nsURI, const XMLCh* typeName,
                                         const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) 
: ConstantFoldingFunction( typeName, 1, 1, "item()", args, memMgr)
{
  _fURI = nsURI;
}

ASTNode* FunctionConstructor::staticResolution(StaticContext *context)
{
  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(_fName, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgNotationString) &&
     XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(_fURI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
    XQThrow(StaticErrorException,X("FunctionConstructor::staticResolution"), X("Only subtypes of xs:NOTATION items can be created [err:XPST0017]"));

  XPath2MemoryManager *mm = context->getMemoryManager();  

  SequenceType *seqType = new (mm) SequenceType(_fURI, _fName, SequenceType::QUESTION_MARK, mm);
  seqType->setLocationInfo(this);

  ASTNode *result = new (mm) XQCastAs(_args[0], seqType, mm);
  result->setLocationInfo(this);

  return result->staticResolution(context);
}

ASTNode *FunctionConstructor::staticTypingImpl(StaticContext *context)
{
  // Should never be called
  return this;
}

