//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ASTREPLACEOPTIMIZER_HPP
#define	__ASTREPLACEOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"
#include "../ReferenceMinder.hpp"

class DynamicContext;

namespace DbXml
{

class ASTReplaceOptimizer : public NodeVisitingOptimizer
{
public:
	ASTReplaceOptimizer(ReferenceMinder &minder, DynamicContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent),
		  xpc_(xpc),
		  minder_(minder)
	{}

	virtual ASTNode *optimizeFunction(XQFunction *item);
	virtual ASTNode *optimizeStep(XQStep *item);
	virtual ASTNode *optimizeValidate(XQValidate *item);

protected:
	virtual void resetInternal()
	{
		minder_.resetMinder();
	}

private:
	DynamicContext *xpc_;
	ReferenceMinder &minder_;
};

}

#endif
