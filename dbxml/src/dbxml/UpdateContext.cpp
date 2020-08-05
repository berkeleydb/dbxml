//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "UpdateContext.hpp"
#include "Manager.hpp"

using namespace DbXml;

UpdateContext::UpdateContext(XmlManager &mgr)
	: mgr_(mgr),
	  indexer_(((Manager&)mgr).getImplicitTimezone())
{}

UpdateContext::~UpdateContext()
{}

void UpdateContext::init(Transaction *txn, Container *container)
{
	indexer_.resetContext(container, &oc_);
	is_.read(container->getConfigurationDB(), txn, /*lock=*/false);
	oc_.set(txn);
}

Indexer &UpdateContext::getIndexer()
{
	return indexer_;
}

KeyStash &UpdateContext::getKeyStash(bool reset)
{
	if(reset) stash_.reset();
	return stash_;
}

IndexSpecification &UpdateContext::getIndexSpecification()
{
	return is_;
}

OperationContext &UpdateContext::getOperationContext()
{
	return oc_;
}
