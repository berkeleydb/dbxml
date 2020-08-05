//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ANCESTORJOINQP_HPP
#define	__ANCESTORJOINQP_HPP

#include "StructuralJoinQP.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class AncestorJoinQP : public StructuralJoinQP
{
public:
	AncestorJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class AncestorOrSelfJoinQP : public StructuralJoinQP
{
public:
	AncestorOrSelfJoinQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();

protected:
	virtual std::string getLongName() const;
	virtual std::string getShortName() const;
};

class AncestorIterator : public ProxyIterator
{
public:
	AncestorIterator(bool orSelf, NodeIterator *descendants, NodeIterator *ancestors, const LocationInfo *o);
	~AncestorIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	bool orSelf_;

	NodeIterator *descendants_;
	NodeIterator *ancestors_;

	enum { INIT, RUNNING, DONE } state_;
};

}

#endif
