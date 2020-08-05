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
 * $Id: FunctionIndexOf.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionIndexOf.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

const XMLCh FunctionIndexOf::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionIndexOf::minArgs = 2;
const unsigned int FunctionIndexOf::maxArgs = 3;

/**
 * fn:index-of($seqParam as xdt:anyAtomicType*, $srchParam as xdt:anyAtomicType) as xs:integer*
 * fn:index-of($seqParam as xdt:anyAtomicType*, $srchParam as xdt:anyAtomicType, $collation as xs:string) as xs:integer*
**/

FunctionIndexOf::FunctionIndexOf(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType*, anyAtomicType, string", args, memMgr)
{
  _src.getStaticType() = StaticType(StaticType::DECIMAL_TYPE, 0, StaticType::UNLIMITED);
}

Sequence FunctionIndexOf::indexOf(Sequence &list, const Item::Ptr &item, Collation* collation, DynamicContext* context) const
{
  Sequence result = Sequence(list.getLength(),context->getMemoryManager());
  int index = 1;
  AnyAtomicType::Ptr atom=(const AnyAtomicType::Ptr )item;
  // need to manually convert xdt:untypedAtomic to xs:string
  if(atom->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC)
    atom = atom->castAs(AnyAtomicType::STRING, context);
  for(Sequence::iterator i = list.begin(); i != list.end(); ++i,++index) {
    AnyAtomicType::Ptr current = (const AnyAtomicType::Ptr )*i;
    try {
        // need to manually convert xdt:untypedAtomic to xs:string
        if(current->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC)
            current = current->castAs(AnyAtomicType::STRING, context);

        if(Equals::equals(current,atom,collation,context,this))
        result.addItem(context->getItemFactory()->createInteger(index, context));
    } catch (IllegalArgumentException &e) {
        // if eq is not defined, they are different
    } catch (XPath2ErrorException &e) {
        // if eq is not defined, they are different
    }
  }
  return result;
}


Sequence FunctionIndexOf::createSequence(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	Sequence list=getParamNumber(1,context)->toSequence(context);
	if(list.isEmpty()) return Sequence(memMgr);
  const Item::Ptr srchparam = (const Item::Ptr )getParamNumber(2,context)->next(context);
  
  Collation* collation=NULL;
  if (getNumArgs() > 2) {
    const XMLCh* collName=getParamNumber(3,context)->next(context)->asString(context);
    try {
      context->getItemFactory()->createAnyURI(collName, context);
    } catch(XPath2ErrorException &e) {
      XQThrow(FunctionException, X("FunctionIndexOf::createSequence"), X("Invalid collationURI"));  
    }
	  collation=context->getCollation(collName, this);
  }
  else
    collation = context->getDefaultCollation(this);
  return indexOf(list, srchparam, collation, context);
}
