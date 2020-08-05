//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLUPDATEFACTORY_HPP
#define	__DBXMLUPDATEFACTORY_HPP

#include "DbXmlNodeImpl.hpp"

#include <xqilla/context/UpdateFactory.hpp>
#include "../nodeStore/NsUpdate.hpp"

namespace DbXml
{

class Document;
class Transaction;
	
class DbXmlUpdateFactory : public UpdateFactory
{
public:
	virtual void applyPut(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertInto(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertAttributes(const PendingUpdate &update, DynamicContext *context);
	virtual void applyReplaceValue(const PendingUpdate &update, DynamicContext *context);
	virtual void applyRename(const PendingUpdate &update, DynamicContext *context);
	virtual void applyDelete(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertBefore(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertAfter(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertAsFirst(const PendingUpdate &update, DynamicContext *context);
	virtual void applyInsertAsLast(const PendingUpdate &update, DynamicContext *context);
	virtual void applyReplaceNode(const PendingUpdate &update, DynamicContext *context);
	virtual void applyReplaceAttribute(const PendingUpdate &update, DynamicContext *context);
	virtual void applyReplaceElementContent(const PendingUpdate &update, DynamicContext *context);
	
	virtual void checkUpdates(const PendingUpdateList &pul, DynamicContext *context, DocumentCache::ValidationMode valMode);
	virtual void completeUpdate(DynamicContext *context);
protected:
	void applyInserts(const PendingUpdate &update,
			  const DbXmlNodeImpl *parent,
			  const NsDomNode *next,
			  DynamicContext *context,
                          bool firstOrAfter);
	void insertAttributes(const PendingUpdate &update,
			      const DbXmlNodeImpl *parent,
			      DynamicContext *context);
	void renameAttribute(const PendingUpdate &update,
			     ATQNameOrDerived *qname,
			     DynamicContext *context);
	void renamePI(const PendingUpdate &update,
		      const XMLCh *name,
		      DynamicContext *context);
protected:
	class NodeSetCompare
	{
	public:
		bool operator()(const DbXmlNodeImpl::Ptr &first, const DbXmlNodeImpl::Ptr &second) const;
	};

	typedef std::set<DbXmlNodeImpl::Ptr, NodeSetCompare> NodeSet;

	NodeSet forDeletion_;
	NsUpdate update_;
};

}

#endif
