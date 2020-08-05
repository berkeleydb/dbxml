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
 * $Id: FunctionAnalyzeString.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionAnalyzeString.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/ast/XQAnalyzeString.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/items/FunctionRef.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

XERCES_CPP_NAMESPACE_USE;

const XMLCh FunctionAnalyzeString::name[] = {
  chLatin_a, chLatin_n, chLatin_a, chLatin_l, chLatin_y, chLatin_z, chLatin_e, chDash, chLatin_s, chLatin_t, chLatin_r, chLatin_i, chLatin_n, chLatin_g, chNull 
};
const unsigned int FunctionAnalyzeString::minArgs = 3;
const unsigned int FunctionAnalyzeString::maxArgs = 4;

/**
 * xqilla:analyze-string($input as xs:string?, $pattern as xs:string,
 *   $action as function(xs:string, xs:boolean) as item()*) as item()*
 * xqilla:analyze-string($input as xs:string?, $pattern as xs:string,
 *   $action as function(xs:string, xs:boolean) as item()*, $flags as xs:string) as item()*
 */
FunctionAnalyzeString::FunctionAnalyzeString(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQillaFunction(name, minArgs, maxArgs, "string?, string, item()*, string", args, memMgr)
{
}

ASTNode *FunctionAnalyzeString::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  VectorOfSequenceTypes *args = new (mm) VectorOfSequenceTypes(XQillaAllocator<SequenceType*>(mm));
  SequenceType *arg1Type = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                 SchemaSymbols::fgDT_STRING,
                                                 SequenceType::EXACTLY_ONE, mm);
  arg1Type->setLocationInfo(this);
  args->push_back(arg1Type);
  SequenceType *arg2Type = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                 SchemaSymbols::fgDT_BOOLEAN,
                                                 SequenceType::EXACTLY_ONE, mm);
  arg2Type->setLocationInfo(this);
  args->push_back(arg2Type);
  SequenceType *returnType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR);
  returnType->setLocationInfo(this);
  SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(args, returnType), SequenceType::EXACTLY_ONE);
  seqType->setLocationInfo(this);

  _paramDecl[2] = seqType;

  return resolveArguments(context);
}

ASTNode *FunctionAnalyzeString::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.setProperties(0);

  ASTNode *result = calculateSRCForArguments(context);
  if(result != this) return result;

  // TBD Precompile the regex - jpcs

  if(_args[2]->getStaticAnalysis().getStaticType().getReturnType()) {
    _src.getStaticType() = *_args[2]->getStaticAnalysis().getStaticType().getReturnType();
    _src.getStaticType().setCardinality(0, StaticType::UNLIMITED);
  }
  else {
    _src.getStaticType() = StaticType(StaticType::ITEM_TYPE, 0, StaticType::UNLIMITED);
  }

  return this;
}

class FunctionAnalyzeStringResult : public AnalyzeStringResult
{
public:
  FunctionAnalyzeStringResult(const FunctionAnalyzeString *ast)
    : AnalyzeStringResult(ast),
      ast_(ast),
      func_(0)
  {
  }

  const XMLCh *getInput(DynamicContext *context)
  {
    return ast_->getParamNumber(1, context)->next(context)->asString(context);
  }

  const XMLCh *getPattern(DynamicContext *context)
  {
    return ast_->getParamNumber(2, context)->next(context)->asString(context);
  }

  const XMLCh *getFlags(DynamicContext *context)
  {
    if(ast_->getNumArgs() == 4)
      return ast_->getParamNumber(4, context)->next(context)->asString(context);
    return XMLUni::fgZeroLenString;
  }

  Result getMatchResult(const XMLCh *matchString, size_t matchPos,
                        size_t numberOfMatches, bool match, DynamicContext *context)
  {
    if(func_.isNull())
      func_ = (FunctionRef*)ast_->getParamNumber(3, context)->next(context).get();

    VectorOfResults args;
    args.push_back(Result(context->getItemFactory()->createString(matchString, context)));
    args.push_back(Result(context->getItemFactory()->createBoolean(match, context)));

    return func_->execute(args, context, this);
  }

private:
  const FunctionAnalyzeString *ast_;
  FunctionRef::Ptr func_;
};

Result FunctionAnalyzeString::createResult(DynamicContext* context, int flags) const
{
  return new FunctionAnalyzeStringResult(this);
}
