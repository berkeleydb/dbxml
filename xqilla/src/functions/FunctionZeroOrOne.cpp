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
 * $Id: FunctionZeroOrOne.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/functions/FunctionZeroOrOne.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>

const XMLCh FunctionZeroOrOne::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionZeroOrOne::minArgs = 1;
const unsigned int FunctionZeroOrOne::maxArgs = 1;

/*
 * fn:zero-or-one($arg as item()*) as item()?
 */

FunctionZeroOrOne::FunctionZeroOrOne(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

static const XMLCh err_FORG0003[] = { 'e', 'r', 'r', ':', 'F', 'O', 'R', 'G', '0', '0', '0', '3', 0 };

ASTNode* FunctionZeroOrOne::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING),
                                                SequenceType::QUESTION_MARK);
  seqType->setLocationInfo(this);

  ASTNode *result = new (mm) XQTreatAs(_args.front(), seqType, mm, err_FORG0003);
  result->setLocationInfo(this);
  return result->staticResolution(context);
}

