//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DICTIONARYDATABASE_HPP
#define	__DICTIONARYDATABASE_HPP

#include <dbxml/XmlPortability.hpp>
#include <string>
#include <db.h>
#include "DbWrapper.hpp"
#include "NameID.hpp"
#include "ScopedPtr.hpp"
#include "DictionaryCache.hpp"
#include "OperationContext.hpp"

namespace DbXml
{

class Name;
class Transaction;

/**
 * Implements the name dictionary for a container
 */
class DBXML_EXPORT DictionaryDatabase
{
public:
	typedef SharedPtr<DictionaryDatabase> Ptr;

	DictionaryDatabase(DB_ENV *env, Transaction *txn,
			   const std::string &name, const ContainerConfig &config, bool useMutex=false);
	~DictionaryDatabase();

	// lookup through cache
	const char *lookupName(OperationContext &context,
			       const NameID &id) const;

	// lookups in primary
	int lookupNameFromID(OperationContext &context, const NameID &id,
			     Name &name) const; ///< lookup by id

	// return goes into the OperationContext data DBT, and is
	// only valid until that is deleted, or overwritten by another
	// call
	int lookupStringNameFromID(OperationContext &context, const NameID &id,
				   const char **name) const; ///< lookup by id

	// return goes into the DBT passed in
	int lookupStringNameFromID(OperationContext &context, const NameID &id,
				   DbtOut &dbt) const; ///< lookup by id

	// lookups in secondary, optionally define new name
	int lookupIDFromName(OperationContext &context, const Name &name, NameID &id,
			     bool define); ///< lookup by Name and define
	int lookupIDFromQName(OperationContext &context, DbXmlDbt &qname,
			      NameID &id, bool define); ///< lookup by name:uri and define
	int lookupIDFromStringName(OperationContext &context, const char *name,
				   size_t namelen,
				   NameID &id, bool define); ///< lookup by name and define

	bool isTransacted() const { return isTransacted_; }

	void display(OperationContext &context, std::ostream &out) const;

	//
	// internals
	//
	const NameID &getNIDForName() const { return nidName_; }
	const NameID &getNIDForRoot() const { return nidRoot_; }
	void sync() {
		primary_->sync();
		secondary_->sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, *primary_);
		runner.run(txn, *secondary_);
	}

	DbWrapper *getPrimaryDatabase()
	{
		return primary_.get();
	}

	static int dump(DB_ENV *env, const std::string &name,
			std::ostream *out);
	static int load(DB_ENV *env, const std::string &name,
			std::istream *in, unsigned long *lineno);
	static int verify(DB_ENV *env, const std::string &name,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static void upgrade(const std::string &name,
			    const std::string &tname, Manager &mgr,
			    int old_version, int current_version);
protected:
	// internal addition to dictionary
	int defineName(OperationContext &context, const Name &name,
		       NameID &id); ///< define from Name
	int defineQName(OperationContext &context, const char *uriname,
			NameID &id); ///< define from uri:name
	int defineStringName(OperationContext &context, const char *name,
			     size_t namelen, NameID &id); ///< define from name
	// const lookups in secondary
	int lookupIDFromStringNameInternal(
		OperationContext &context,
		DbXmlDbt &dbt,
		NameID &id, bool define = false) const;
private:
	// no need for copy and assignment
	DictionaryDatabase(const DictionaryDatabase&);
	DictionaryDatabase &operator=(const DictionaryDatabase&);

	Transaction *getTxn(OperationContext &oc) const {
		return (primary_->getTxn(oc));
	}

	// internal lookups
	int lookupFromID(OperationContext &context,
		DbtOut &dbt, const NameID &id) const;

	void preloadDictionary(Transaction *txn, bool rdonly);
	bool stringCacheLookup(OperationContext &context,
			       DbXmlDbt &dbt, NameID &id) const;

	mutable DictionaryCache cache_;
	mutable DictionaryStringCache stringCache_;
	DB_ENV *environment_;
	std::string name_;

	PrimaryDatabase::Ptr primary_;
	SecondaryDatabase::Ptr secondary_;
	NameID nidName_;
	NameID nidRoot_;
	bool usePreloads_;
	bool isTransacted_;
	dbxml_mutex_t mutex_;
};

}

#endif

