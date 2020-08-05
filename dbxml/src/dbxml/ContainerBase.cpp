//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "Manager.hpp"
#include "ContainerBase.hpp"
#include "CacheDatabaseMinder.hpp"
#include "dataItem/DbXmlConfiguration.hpp"
#include "Log.hpp"
#include "query/SequentialScanQP.hpp" // for iterators GMF TBD -- eliminate this
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;

/////////////////////////////////
// ContainerBase


// Log methods
void ContainerBase::log(ImplLogCategory c, ImplLogLevel l,
			const std::ostringstream &s) const
{
	Log::log(getDB_ENV(), c, l, name_.c_str(), s.str().c_str());
}

void ContainerBase::log(ImplLogCategory c, ImplLogLevel l,
			const std::string &s) const
{
	Log::log(getDB_ENV(), c, l, name_.c_str(), s.c_str());
}

void ContainerBase::checkFlags(const FlagInfo *flag_info, const char *function,
			       u_int32_t flags, u_int32_t mask) const
{
	Log::checkFlags(function, flag_info, flags, mask);
}


// ContainerBase implementation
DB_ENV *ContainerBase::getDB_ENV() const {
	return const_cast<Manager&>(mgr_).getDB_ENV();
}

void ContainerBase::getIndexSpecification(Transaction *txn,
					  IndexSpecification &index)
{
	index.clear();
}

const NameID &ContainerBase::getNIDForRoot() const
{
	return getDictionaryDatabase()->getNIDForRoot();
}

bool ContainerBase::lookupID(OperationContext &oc, const char *name,
			 size_t namelen, NameID &retVal)
{
	DictionaryDatabase *ddb = getDictionaryDatabase();
	int err = ddb->lookupIDFromStringName(oc, name,
					      namelen, retVal,
					      /*define*/false);
	if(err != DB_NOTFOUND && err != 0)
		throw XmlException(err);
	return (err == 0);
}

bool ContainerBase::lookupID(OperationContext &oc, const Name &name,
			 NameID &retVal)
{
	DictionaryDatabase *ddb = getDictionaryDatabase();
	int err = ddb->lookupIDFromName(oc,  name,
					retVal, /*define*/false);
	if(err != DB_NOTFOUND && err != 0)
		throw XmlException(err);
	return (err == 0);
}


DictionaryDatabase *ContainerBase::getDictionaryDatabase() const
{
	return mgr_.getDictionary();
}

DbWrapper *ContainerBase::getDbWrapper(DynamicContext *context) {
	if (context) {
		CacheDatabaseMinder &dbMinder =
			GET_CONFIGURATION(context)->
			getDbMinder();
		if (!dbMinder.isNull()) {
			CacheDatabase *cdb =
				dbMinder.findOrAllocate(
				mgr_, getContainerID());
			if (cdb)
				return cdb->getDb();
		}
	}
	return 0;
}

Cost ContainerBase::getDocumentSSCost(OperationContext &oc, StructuralStatsCache &cache)
{
	throw XmlException(XmlException::INTERNAL_ERROR,
		"Sequential scan over document nodes in the temporary container.");
}

Cost ContainerBase::getAttributeSSCost(OperationContext &oc, StructuralStatsCache &cache, const char *childUriName)
{
	StructuralStats docStats(false, false);

	Cost cost;
	cost.keys = (double) docStats.sumNumberOfDescendants_;
	cost.pagesOverhead = (double) docStats.sumDescendantSize_ * 1000; // Make the cost BIG, so the SS doesn't get chosen

	return cost;
}

Cost ContainerBase::getElementSSCost(OperationContext &oc, StructuralStatsCache &cache, const char *childUriName)
{
	StructuralStats docStats(false, false);

	Cost cost;
	cost.keys = (double) docStats.sumNumberOfDescendants_;
	cost.pagesOverhead = (double) docStats.sumDescendantSize_ * 1000; // Make the cost BIG, so the SS doesn't get chosen

	return cost;
}

NamedNodeIterator *ContainerBase::createAttributeIterator(
	DynamicContext *context, const LocationInfo *location,
	const NameID &nsUriID) const
{
	throw XmlException(XmlException::INTERNAL_ERROR,
		"Sequential scan over attribute nodes in the temporary container.");
}

NamedNodeIterator *ContainerBase::createElementIterator(
	DynamicContext *context, const LocationInfo *location) const
{
	throw XmlException(XmlException::INTERNAL_ERROR,
		"Sequential scan over element nodes in the temporary container.");
}

