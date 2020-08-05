//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __PARENTOFCHILDJOINQP_HPP
#define	__PARENTOFCHILDJOINQP_HPP

#include "StructuralJoinQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class ParentOfChildJoinQP : public StructuralJoinQP
{
public:
	ParentOfChildJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class ParentOfChildIterator : public DbXmlNodeIterator
{
public:
	ParentOfChildIterator(NodeIterator *children, NodeIterator *parents, const LocationInfo *o);
	~ParentOfChildIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	NodeIterator *children_;
	NodeIterator *parents_;

	std::vector<DbXmlNodeImpl::Ptr> ancestorStack_;

	// TBD replace this with a temporary BTree - jpcs
	std::vector<DbXmlNodeImpl::Ptr> results_;
	std::vector<DbXmlNodeImpl::Ptr>::iterator it_;

	enum { INIT, RUNNING, DONE } state_;
};

}

#endif
