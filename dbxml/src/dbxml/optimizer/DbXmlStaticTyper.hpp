//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLSTATICTYPER_HPP
#define	__DBXMLSTATICTYPER_HPP

#include "NodeVisitingOptimizer.hpp"

#include <xqilla/optimizer/StaticTyper.hpp>

namespace DbXml
{

class DbXmlStaticTyper : public ASTVisitorExtender<StaticTyper>
{
public:
	DbXmlStaticTyper() {}
	DbXmlStaticTyper(StaticContext *context, Optimizer *parent = 0)
		: ASTVisitorExtender<StaticTyper>(context, parent) {}

protected:
	virtual ASTNode *optimizeDbXmlPredicate(DbXmlPredicate *item);
	virtual QueryPlan *optimizeQP(QueryPlan *item);
};

}

#endif
