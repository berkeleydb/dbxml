//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANHOLDER_HPP
#define	__QUERYPLANHOLDER_HPP

#include "QueryPlan.hpp"

namespace DbXml
{

class ContainerBase;

class QueryPlanRoot
{
public:
	QueryPlanRoot(ContainerBase *c) : container_(c), documentUri_(0) {}
	virtual ~QueryPlanRoot() {}

	ContainerBase *getContainerBase() const { return container_; }
	const XMLCh *getDocumentURI() const { return documentUri_; }

protected:
	ContainerBase *container_;
	const XMLCh *documentUri_;
};

}

#endif
