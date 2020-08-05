//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __STRUCTURALSTATSDATABASE_HPP
#define	__STRUCTURALSTATSDATABASE_HPP

#include "DbWrapper.hpp"
#include "NameID.hpp"

#include <map>

namespace DbXml
{

class Transaction;
class DictionaryDatabase;
class ContainerBase;
class StructuralStatsWriteCache;

class StructuralStats
{
public:
	StructuralStats();
	/// Constructs an object with default values filled in
	StructuralStats(bool id1Set, bool id2Set);

	void reset();
	void add(const StructuralStats &o);
	void subtract(const StructuralStats &o);

	// Marshal, either as node stats or as descendant stats
	void marshal(DbtOut &dbt, bool nodeStats) const;
	/// Returns true if the dbt was node stats
	bool unmarshal(const DbXmlDbt &dbt);

	// Marshal, either as node stats or as descendant stats
	int marshal(unsigned char *ptr, bool count, bool nodeStats) const;
	/// Returns true if the dbt was node stats
	bool unmarshal(const unsigned char *ptr);

	void display(std::ostream &out) const;

	int64_t numberOfNodes_;
	int64_t sumSize_;
	int64_t sumChildSize_;
	int64_t sumDescendantSize_;

	int64_t sumNumberOfChildren_;
	int64_t sumNumberOfDescendants_;
};

class StructuralStatsDatabase
{
public:
	typedef SharedPtr<StructuralStatsDatabase> Ptr;

	// Constructor for the database that is part of a container
	StructuralStatsDatabase(DB_ENV *env, Transaction *txn, const std::string &name,
		const ContainerConfig &config, bool usingCDB);

	// Constructor for the anonymous in memory version
	StructuralStatsDatabase();

	int getStats(OperationContext &context, const NameID &id1, const NameID &id2,
		StructuralStats &stats) const;
	int getStats(OperationContext &context, const NameID &id1, StructuralStats &stats) const;
	int getStats(OperationContext &context, StructuralStats &stats) const;

	int addStats(OperationContext &context, const NameID &id1, const NameID &id2,
		const StructuralStats &stats);
	int subtractStats(OperationContext &context, const NameID &id1, const NameID &id2,
		const StructuralStats &stats);

	int addStats(OperationContext &context, const StructuralStatsDatabase &sdb);
	int addStats(OperationContext &context, const StructuralStatsWriteCache &cache);

	int truncate(Transaction *txn, u_int32_t *countp, u_int32_t flags) {
		return db_.truncate(txn, countp, flags);
	}

	void sync() {
		db_.sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, db_);
	}

	void display(OperationContext &context, std::ostream &out, const DictionaryDatabase *ddb = 0) const;

	static int remove(DB_ENV *env, Transaction *txn, const std::string &name);

private:
	Transaction *getTxn(OperationContext &oc) const;

	DbWrapper db_;
	bool inMemory_;
};

class StructuralStatsCache
{
public:
	/// Lookup the stats if they aren't in the cache
	StructuralStats get(const ContainerBase *container, OperationContext &oc,
		const NameID &id1, const NameID &id2);

private:
	struct Key
	{
		Key(int c, const NameID &i1, const NameID &i2)
			: containerID(c), id1(i1), id2(i2) {}

		int containerID;
		NameID id1;
		NameID id2;

		bool operator<(const Key &o) const
		{
			if(containerID < o.containerID) return true;
			if(containerID > o.containerID) return false;
			if(id1 < o.id1) return true;
			if(id1 > o.id1) return false;
			return id2 < o.id2;
		}
	};
	typedef std::map<Key, StructuralStats> Map;

	Map cache_;
};

class StructuralStatsWriteCache
{
public:
	void add(const NameID &id1, const std::map<NameID, StructuralStats> &values);
	void subtract(const NameID &id1, const std::map<NameID, StructuralStats> &values);

	/// Clears all the cached statistics
	void clear();

private:
	typedef std::map<NameID, StructuralStats> InnerMap;
	typedef std::map<NameID, InnerMap> Map;

	Map cache_;

	friend class StructuralStatsDatabase;
};

};

#endif
