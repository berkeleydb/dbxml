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
 * $Id: XQFunction.cpp 734 2009-01-30 18:08:37Z gmfeinberg $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLStringTokenizer.hpp>

#include <string>
#include <map>

#include <assert.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

/* http://www.w3.org/2005/xpath-functions */
const XMLCh XQFunction::XMLChFunctionURI[] =
{
    chLatin_h,       chLatin_t,       chLatin_t, 
    chLatin_p,       chColon,         chForwardSlash, 
    chForwardSlash,  chLatin_w,       chLatin_w, 
    chLatin_w,       chPeriod,        chLatin_w,
    chDigit_3,       chPeriod,        chLatin_o, 
    chLatin_r,       chLatin_g,       chForwardSlash, 
    chDigit_2,       chDigit_0,       chDigit_0, 
    chDigit_5,       chForwardSlash,  chLatin_x, 
    chLatin_p,       chLatin_a,       chLatin_t, 
    chLatin_h,       chDash,          chLatin_f, 
    chLatin_u,       chLatin_n,       chLatin_c,
    chLatin_t,       chLatin_i,       chLatin_o, 
    chLatin_n,       chLatin_s,       chNull
};

const size_t XQFunction::UNLIMITED = 10000; // A reasonably large number

XQFunction::XQFunction(const XMLCh* name, size_t argsFrom, size_t argsTo, const char* paramDecl,
                       const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(FUNCTION, memMgr),
    _fName(name), _fURI(XMLChFunctionURI), 
    _nArgsFrom(argsFrom), _nArgsTo(argsTo),
    _paramDecl(XQillaAllocator<SequenceType*>(memMgr)),
    _args(args)
{
  parseParamDecl(paramDecl, memMgr);

  if((argsFrom != UNLIMITED && argsFrom > args.size()) ||
     (argsTo != UNLIMITED && args.size() > argsTo)) {

    //cerr << "argsFrom: " << argsFrom << endl;
    //cerr << "argsTo: " << argsTo << endl;
    //cerr << "number of args: " << args.size() << endl;

    XQThrow(FunctionException,X("XQFunction::XQFunction"), X("Wrong number of arguments"));
  }
  const XMLCh* paramString=memMgr->getPooledString(paramDecl);
  if(argsFrom==argsTo)
    _signature=paramString;
  else
  {
    // place the optional arguments between [ and ]
    static const XMLCh comma[]={ chComma, chNull };
    static const XMLCh openSquare[]={ chSpace, chOpenSquare, chNull };
    static const XMLCh closSquare[]={ chCloseSquare, chNull };
    XMLStringTokenizer tokenizer(paramString,comma);
    unsigned int i;
    unsigned int nTokens=tokenizer.countTokens();
    _signature=XMLUni::fgZeroLenString;

    for(i=0;i<argsFrom;i++)
    {
      if(i==0)
        _signature=XPath2Utils::concatStrings(_signature,tokenizer.nextToken(),memMgr);
      else
        _signature=XPath2Utils::concatStrings(_signature,comma,tokenizer.nextToken(),memMgr);
    }
    if(i<nTokens)
    {
      _signature=XPath2Utils::concatStrings(_signature,openSquare,memMgr);
      for(;i<nTokens;i++)
        if(i==0)
          _signature=XPath2Utils::concatStrings(_signature,tokenizer.nextToken(),memMgr);
        else
          _signature=XPath2Utils::concatStrings(_signature,comma,tokenizer.nextToken(),memMgr);
      _signature=XPath2Utils::concatStrings(_signature,closSquare,memMgr);
    }
    if(argsTo==UNLIMITED)
    {
      static const XMLCh ellipsis[]={ chComma, chPeriod, chPeriod, chPeriod, chNull };
      _signature=XPath2Utils::concatStrings(_signature,ellipsis,memMgr);
    }
  }
}


// bool XQFunction::isSubsetOf(const ASTNode *other) const
// {
//   if(other->getType() != getType()) return false;

//   const XQFunction *o = (const XQFunction*)other;
//   if(!XPath2Utils::equals(_fURI, o._fURI) ||
//      !XPath2Utils::equals(_fName, o._fName) ||
//      _args.size() != o._args.size())
//     return false;

//   VectorOfASTNodes::iterator i = _args.begin();
//   VectorOfASTNodes::iterator j = o._args.begin();
//   for(; i != _args.end(); ++i, ++j) {
//     // TBD non-deterministic functions - jpcs
//     if(!(*i)->isEqualTo(*j))
//       return false;
//   }

//   return true;
// }

const XMLCh* XQFunction::getFunctionURI()const {
  return _fURI;
}

const XMLCh* XQFunction::getFunctionName() const {
  return _fName;
}

const XMLCh* XQFunction::getFunctionSignature() const
{
  return _signature;
}

ASTNode *XQFunction::resolveArguments(StaticContext *context, bool checkTimezone, bool numericFunction)
{
  size_t paramNumber = 0;
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = _paramDecl[paramNumber]->convertFunctionArg(*i, context, numericFunction, this);
    *i = (*i)->staticResolution(context);

    ++paramNumber;
    if(paramNumber >= _paramDecl.size()) {
      paramNumber = _paramDecl.size() - 1;
    }
  }

  return this;
}

ASTNode *XQFunction::calculateSRCForArguments(StaticContext *context, bool checkTimezone, bool numericFunction)
{
  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    _src.add((*i)->getStaticAnalysis());

    if((*i)->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("XQFunction::staticTyping"),
              X("It is a static error for an argument to a function "
                "to be an updating expression [err:XUST0001]"));
    }

    if(checkTimezone && (*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  return this;
}

Result XQFunction::getParamNumber(size_t number, DynamicContext* context, int flags) const
{
  assert(number > 0);
  assert(number <= getNumArgs());
  return _args[number - 1]->createResult(context, flags);
}

size_t XQFunction::getNumArgs() const
{
  return _args.size();
}

class CreateSequenceResult : public LazySequenceResult
{
public:
  CreateSequenceResult(const XQFunction *func, DynamicContext *context)
    : LazySequenceResult(func, context),
      func_(func)
  {
  }

  void getResult(Sequence &toFill, DynamicContext *context) const
  {
    try {
      toFill = func_->createSequence(context);
    }
    catch(XQException &e) {
      if(e.getXQueryLine() == 0)
        e.setXQueryPosition(this);
      throw e;
    }
  }

private:
  const XQFunction *func_;
};

Result XQFunction::createResult(DynamicContext* context, int flags) const
{
  return new CreateSequenceResult(this, context);
}

Sequence XQFunction::createSequence(DynamicContext* context, int flags) const
{
  return Sequence(context->getMemoryManager());
}

void XQFunction::parseParamDecl(const char* paramString, XPath2MemoryManager *mm)
{
  // Tokenise param string
  XMLCh* string = XMLString::transcode(paramString);
  static XMLCh delimiters[]={ chComma, chNull };
  XMLStringTokenizer tokenizer(string, delimiters);
  while(tokenizer.hasMoreTokens())
  {
    XMLCh* toParse=tokenizer.nextToken();
    //create SequenceType from curParam and append to vector

    SequenceType *sequenceType=NULL;
    XMLCh* tmpCurParam = XMLString::replicate(toParse);
    XMLString::collapseWS(tmpCurParam);

    // get the OccurrenceIndicator
    SequenceType::OccurrenceIndicator occurrence=SequenceType::EXACTLY_ONE;
    unsigned int len = XPath2Utils::uintStrlen(tmpCurParam);
    if(len>0)
      switch(tmpCurParam[len-1])
      {
        case chQuestion: occurrence=SequenceType::QUESTION_MARK; tmpCurParam[len-1]=0; break;
        case chAsterisk: occurrence=SequenceType::STAR; tmpCurParam[len-1]=0; break;
        case chPlus:     occurrence=SequenceType::PLUS; tmpCurParam[len-1]=0; break;
      }

    // check if it's a node(), element(), item() or empty()
    static XMLCh szNode[]={ chLatin_n, chLatin_o, chLatin_d, chLatin_e, chOpenParen, chCloseParen, chNull };
    static XMLCh szElement[]={ chLatin_e, chLatin_l, chLatin_e, chLatin_m, chLatin_e, chLatin_n, chLatin_t,
                               chOpenParen, chCloseParen, chNull };
    static XMLCh szItem[]={ chLatin_i, chLatin_t, chLatin_e, chLatin_m, chOpenParen, chCloseParen, chNull };
    static XMLCh szEmpty[]={ chLatin_e, chLatin_m, chLatin_p, chLatin_t, chLatin_y, chOpenParen, chCloseParen, chNull };
    static XMLCh szFunction[]={ chLatin_f, chLatin_u, chLatin_n, chLatin_c, chLatin_t, chLatin_i, chLatin_o, chLatin_n, chOpenParen, chCloseParen, chNull };

    if(XPath2Utils::equals(tmpCurParam, szNode))
      sequenceType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szElement))
      sequenceType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szItem))
      sequenceType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szFunction))
      sequenceType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_FUNCTION), occurrence);
    else if(XPath2Utils::equals(tmpCurParam, szEmpty))
      sequenceType = new (mm) SequenceType();
    else
      sequenceType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, tmpCurParam, occurrence, mm);
    XMLString::release(&tmpCurParam);
    _paramDecl.push_back(sequenceType);
  }
  XMLString::release(&string);
}

const VectorOfASTNodes &XQFunction::getArguments() const {
  return _args;
}

