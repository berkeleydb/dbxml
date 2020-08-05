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
 * $Id: NodeComparison.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

/*static*/ const XMLCh NodeComparison::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

NodeComparison::NodeComparison(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr)
{
}

ASTNode* NodeComparison::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *seqType = new (mm) SequenceType(new (mm) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE),
                                                  SequenceType::QUESTION_MARK);
    seqType->setLocationInfo(this);

    *i = new (mm) XQTreatAs(*i, seqType, mm);
    (*i)->setLocationInfo(this);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode *NodeComparison::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType(StaticType::BOOLEAN_TYPE, 0, 1);

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    _src.add((*i)->getStaticAnalysis());

    if((*i)->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("NodeComparison::staticTyping"),
              X("It is a static error for an operand of an operator "
                "to be an updating expression [err:XUST0001]"));
    }
  }

  return this;
}

Result NodeComparison::createResult(DynamicContext* context, int flags) const
{
  return new NodeComparisonResult(this);
}

NodeComparison::NodeComparisonResult::NodeComparisonResult(const NodeComparison *op)
  : SingleResult(op),
    _op(op)
{
}

Item::Ptr NodeComparison::NodeComparisonResult::getSingleResult(DynamicContext *context) const
{
  Item::Ptr arg1 = _op->getArgument(0)->createResult(context)->next(context);
  if(arg1.isNull()) return 0;
  Item::Ptr arg2 = _op->getArgument(1)->createResult(context)->next(context);
  if(arg2.isNull()) return 0;
  
	return context->getItemFactory()->createBoolean(((Node*)arg1.get())->equals((Node*)arg2.get()), context);
}

