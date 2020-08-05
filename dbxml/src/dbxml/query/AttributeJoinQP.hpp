//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ATTRIBUTEJOINQP_HPP
#define	__ATTRIBUTEJOINQP_HPP

#include "StructuralJoinQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class AttributeJoinQP : public StructuralJoinQP
{
public:
	AttributeJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class AttributeIterator : public ProxyIterator
{
public:
	AttributeIterator(NodeIterator *parents, NodeIterator *attributes, const LocationInfo *o);
	~AttributeIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	NodeIterator *parents_;
	NodeIterator *attributes_;

	enum { INIT, RUNNING, DONE } state_;
};

}

#endif
