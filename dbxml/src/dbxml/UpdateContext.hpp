//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __UPDATECONTEXT_HPP
#define	__UPDATECONTEXT_HPP

#include "ReferenceCounted.hpp"
#include "Container.hpp"
#include "Indexer.hpp"
#include "IndexSpecification.hpp"
#include "OperationContext.hpp"
#include "KeyStash.hpp"

namespace DbXml
{

class DBXML_EXPORT UpdateContext : public ReferenceCounted
{
public:
	/// Constructor.
	UpdateContext(XmlManager &mgr);
	virtual ~UpdateContext();

	void init(Transaction *txn, Container *container);

	Indexer &getIndexer();
	KeyStash &getKeyStash(bool reset = true);
	IndexSpecification &getIndexSpecification();
	OperationContext &getOperationContext();
private:
	// no need for copy and assignment
	UpdateContext(const UpdateContext&);
	UpdateContext &operator=(const UpdateContext &);

	// Hold a reference to the XmlManager, so it isn't destroyed before we are
	XmlManager mgr_;

	// Avoid re-making each of these objects
	Indexer indexer_;
	KeyStash stash_;
	IndexSpecification is_;
	OperationContext oc_;
};

}

#endif

