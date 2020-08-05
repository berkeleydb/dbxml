//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CONTAINERBASE_HPP
#define	__CONTAINERBASE_HPP

#include <string>
#include <sstream>
#include <db.h>
#include "ScopedPtr.hpp"
#include "DbWrapper.hpp"
#include "SyntaxDatabase.hpp"
#include "ReferenceCounted.hpp"
#include "Log.hpp"
#include "Statistics.hpp"
#include "StructuralStatsDatabase.hpp"

class DynamicContext;

namespace DbXml
{

class NodeIterator;
class NamedNodeIterator;
class IndexSpecification;
class NameID;
class OperationContext;
class Container;
class DictionaryDatabase;
class Manager;
	
class ContainerBase : public Log, public ReferenceCountedProtected
{
public:
	ContainerBase(Manager &mgr,
		      const std::string &name = (std::string)"unknown")
		: name_(name), mgr_(mgr) {}
	virtual ~ContainerBase() {}

	// lock should be held when this is called
	bool isReferenced() const { return (count_ != 0); }

	// Some non-virtual behavior
	const std::string &getName() const { return name_; }
	Manager &getManager() { return mgr_; }
	DB_ENV *getDB_ENV() const;

	// Log methods
	void log(ImplLogCategory c, ImplLogLevel l,
		 const std::ostringstream &s) const;
	void log(ImplLogCategory c, ImplLogLevel l,
		 const std::string &s) const;
	void checkFlags(const FlagInfo *flag_info, const char *function,
			u_int32_t flags, u_int32_t mask) const;

	// new virtual behavior, with some defaults for "non" containers
	virtual Container * getContainer() { return 0; }
	virtual bool nodesIndexed() const { return false; }
	virtual int getContainerID() const { return 0; }
	virtual void getIndexSpecification(Transaction *txn,
					   IndexSpecification &index);
	virtual void checkReadOnly() const { return; }

	// get the estimated cost of an operation
	virtual Cost getDocumentSSCost(OperationContext &oc, StructuralStatsCache &cache);
	virtual Cost getIndexCost(OperationContext &oc,
		DbWrapper::Operation op1, const Key &key1,
		DbWrapper::Operation op2 = DbWrapper::NONE,
		const Key &key2 = Key(0)) { return Cost(); }
	virtual Cost getAttributeSSCost(OperationContext &oc, StructuralStatsCache &cache,
		const char *childUriName);
	virtual Cost getElementSSCost(OperationContext &oc, StructuralStatsCache &cache,
		const char *childUriName);

	virtual StructuralStats getStructuralStats(OperationContext &oc,
		const NameID &id1, const NameID &id2) const
	{ return StructuralStats(id1 != 0, id2 != 0); }

	// Containers have a docdb and dictionary
	virtual DbWrapper *getDbWrapper(DynamicContext *context = 0);
	virtual DictionaryDatabase *getDictionaryDatabase() const;
	
	// Hide some of the dictionary
	virtual const NameID &getNIDForRoot() const;
	virtual bool lookupID(OperationContext &oc, const char *name,
			      size_t namelen, NameID &retVal);
	virtual bool lookupID(OperationContext &oc, const Name &name,
			      NameID &retVal);

	// Hide iterators over indexes and storage databases
	virtual NodeIterator *createDocumentIterator(
		DynamicContext *context, const LocationInfo *location,
		const char *docName = 0, size_t docNameLen = 0) const
		{ return 0; }
	virtual NodeIterator *createIndexIterator(
		Syntax::Type type, DynamicContext *context,
		bool documentIndex, const LocationInfo *l,
		DbWrapper::Operation op1, const Key &key1,
		DbWrapper::Operation op2 = DbWrapper::NONE,
		const Key &key2 = Key(0)) const
		{ return 0; }
	virtual NamedNodeIterator *createAttributeIterator(
		DynamicContext *context, const LocationInfo *location,
		const NameID &nsUriID) const;
	virtual NamedNodeIterator *createElementIterator(
		DynamicContext *context, const LocationInfo *location) const;

protected:
	std::string name_;
	Manager &mgr_;
};

class ScopedContainer {
public:
	ScopedContainer(Manager &mgr, int id, bool mustExist);
	~ScopedContainer() {
		if (c_)
			c_->release();
	}
	ContainerBase *get() {
		return c_;
	}
	Container *getContainer() {
		if (c_)
			return c_->getContainer();
		return 0;
	}
private:
	ContainerBase *c_;
};

}	
#endif

