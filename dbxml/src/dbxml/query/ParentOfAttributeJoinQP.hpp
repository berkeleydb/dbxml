//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PARENTOFATTRIBUTEJOINQP_HPP
#define	__PARENTOFATTRIBUTEJOINQP_HPP

#include "IntersectQP.hpp"
#include "StructuralJoinQP.hpp"

namespace DbXml
{

class ParentOfAttributeJoinQP : public StructuralJoinQP
{
public:
	ParentOfAttributeJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class ParentOfAttributeIterator : public IntersectIterator
{
public:
	ParentOfAttributeIterator(NodeIterator *parents, NodeIterator *attributes, const LocationInfo *o);

protected:
	virtual bool doJoin(DynamicContext *context);
};

}

#endif
