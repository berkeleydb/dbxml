//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlStaticTyper.hpp"

#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/context/VariableTypeStore.hpp>

using namespace DbXml;

ASTNode *DbXmlStaticTyper::optimizeDbXmlPredicate(DbXmlPredicate *item)
{
	item->setExpression(optimize(item->getExpression()));

	AutoContextItemTypeReset contextTypeReset(context_);
	VariableTypeStore* varStore = context_ ? context_->getVariableTypeStore() : 0;

	if(context_) {
		StaticAnalysis &varSrc = item->getVarSRC();

		varSrc.getStaticType() = item->getExpression()->getStaticAnalysis().getStaticType();
		varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
			StaticAnalysis::ONENODE | StaticAnalysis::SELF);

		if(item->getName() == 0) {
			context_->setContextItemType(varSrc.getStaticType());
		} else {
			varStore->addLogicalBlockScope();
			varStore->declareVar(item->getURI(), item->getName(), varSrc);
		}
	}

	item->setPredicate(optimize(const_cast<ASTNode *>(item->getPredicate())));

	if(context_ && item->getName() != 0) {
		varStore->removeScope();
	}

	return item;
}

QueryPlan *DbXmlStaticTyper::optimizeQP(QueryPlan *item)
{
	return item->staticTyping(context_, this);
}

