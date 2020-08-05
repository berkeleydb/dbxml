//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "StructuralStatsDatabase.hpp"
#include "ScopedDbt.hpp"
#include "NameID.hpp"
#include "OperationContext.hpp"
#include "Cursor.hpp"
#include "ContainerBase.hpp"
#include "DictionaryDatabase.hpp"
#include "nodeStore/NsFormat.hpp"
#include "ContainerConfig.hpp"

using namespace DbXml;
using namespace std;

#define KEY_PREFIX_BYTE 0

#define DATA_NODE_PREFIX_BYTE 0
#define DATA_DESCENDANT_PREFIX_BYTE 1

static const char *structural_stats_name = "structural_stats";

StructuralStats::StructuralStats()
	: numberOfNodes_(0),
	  sumSize_(0),
	  sumChildSize_(0),
	  sumDescendantSize_(0),
	  sumNumberOfChildren_(0),
	  sumNumberOfDescendants_(0)
{
}

#define ELEMENTS_IN_DOCUMENT 500000
#define CHILDREN_PER_ELEMENT 8
#define DIFFERENT_ELEMENT_NAMES 4
#define NODE_SIZE 100

StructuralStats::StructuralStats(bool id1Set, bool id2Set)
	: numberOfNodes_(ELEMENTS_IN_DOCUMENT / (id1Set ? DIFFERENT_ELEMENT_NAMES : 1)),
	  sumSize_(numberOfNodes_ * NODE_SIZE),
	  sumChildSize_(sumSize_ * CHILDREN_PER_ELEMENT),
	  sumDescendantSize_(ELEMENTS_IN_DOCUMENT * NODE_SIZE),
	  sumNumberOfChildren_((numberOfNodes_ * CHILDREN_PER_ELEMENT) / (id2Set ? DIFFERENT_ELEMENT_NAMES : 1)),
	  sumNumberOfDescendants_(ELEMENTS_IN_DOCUMENT / (id2Set ? DIFFERENT_ELEMENT_NAMES : 1))
{
}

void StructuralStats::reset()
{
	numberOfNodes_ = 0;
	sumSize_ = 0;
	sumChildSize_ = 0;
	sumDescendantSize_ = 0;
	sumNumberOfChildren_ = 0;
	sumNumberOfDescendants_ = 0;
}

void StructuralStats::add(const StructuralStats &o)
{
	numberOfNodes_ += o.numberOfNodes_;
	sumSize_ += o.sumSize_;
	sumChildSize_ += o.sumChildSize_;
	sumDescendantSize_ += o.sumDescendantSize_;
	sumNumberOfChildren_ += o.sumNumberOfChildren_;
	sumNumberOfDescendants_ += o.sumNumberOfDescendants_;
}

void StructuralStats::subtract(const StructuralStats &o)
{
	numberOfNodes_ -= o.numberOfNodes_;
	sumSize_ -= o.sumSize_;
	sumChildSize_ -= o.sumChildSize_;
	sumDescendantSize_ -= o.sumDescendantSize_;
	sumNumberOfChildren_ -= o.sumNumberOfChildren_;
	sumNumberOfDescendants_ -= o.sumNumberOfDescendants_;
}

void StructuralStats::marshal(DbtOut &dbt, bool nodeStats) const
{
	int count = marshal(0, /*count*/true, nodeStats);
	dbt.set(0, count);
	marshal((xmlbyte_t*)dbt.data, /*count*/false, nodeStats);
}

bool StructuralStats::unmarshal(const DbXmlDbt &dbt)
{
	return unmarshal((const xmlbyte_t*)dbt.data);
}

int StructuralStats::marshal(xmlbyte_t *ptr, bool count, bool nodeStats) const
{
	int size = 0;
	
	// none of these should ever go negative
	DBXML_ASSERT(sumNumberOfDescendants_ >= 0);
	DBXML_ASSERT(sumNumberOfChildren_ >= 0);
	DBXML_ASSERT(numberOfNodes_ >= 0);
	DBXML_ASSERT(sumSize_ >= 0);
	DBXML_ASSERT(sumChildSize_ >= 0);
	DBXML_ASSERT(sumDescendantSize_ >= 0);
	if(nodeStats) {
		if(count) {
			size += 1; // For the prefix byte
		
			size += NsFormat::countInt64((uint64_t)numberOfNodes_);
			size += NsFormat::countInt64((uint64_t)sumSize_);
			size += NsFormat::countInt64((uint64_t)sumChildSize_);
			size += NsFormat::countInt64((uint64_t)sumDescendantSize_);
		} else {
			*ptr++ = DATA_NODE_PREFIX_BYTE;

			ptr += NsFormat::marshalInt64(ptr, (uint64_t)numberOfNodes_);
			ptr += NsFormat::marshalInt64(ptr, (uint64_t)sumSize_);
			ptr += NsFormat::marshalInt64(ptr, (uint64_t)sumChildSize_);
			ptr += NsFormat::marshalInt64(ptr, (uint64_t)sumDescendantSize_);
		}
	} else {
		if(count) {
			size += 1; // For the prefix byte
		
			size += NsFormat::countInt64((uint64_t)sumNumberOfChildren_);
			size += NsFormat::countInt64((uint64_t)sumNumberOfDescendants_);
		} else {
			*ptr++ = DATA_DESCENDANT_PREFIX_BYTE;

			ptr += NsFormat::marshalInt64(ptr, (uint64_t)sumNumberOfChildren_);
			ptr += NsFormat::marshalInt64(ptr, (uint64_t)sumNumberOfDescendants_);
		}
	}

	return size;
}

bool StructuralStats::unmarshal(const xmlbyte_t *ptr)
{
	if(*ptr == DATA_NODE_PREFIX_BYTE) {
		++ptr; // Skip the prefix
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&numberOfNodes_);
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&sumSize_);
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&sumChildSize_);
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&sumDescendantSize_);
		return true;
	} else if(*ptr == DATA_DESCENDANT_PREFIX_BYTE) {
		++ptr; // Skip the prefix
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&sumNumberOfChildren_);
		ptr += NsFormat::unmarshalInt64(ptr, (uint64_t*)&sumNumberOfDescendants_);
		return false;
	}

	DBXML_ASSERT(false);
	return false;
}

void StructuralStats::display(ostream &out) const
{
#if defined(_MSC_VER) && _MSC_VER < 1300
	out << "(" << (int)numberOfNodes_ << "," << (int)sumSize_ << ","
	    << (int)sumChildSize_ << "," << (int)sumDescendantSize_ << ","
	    << (int)sumNumberOfChildren_ << "," << (int)sumNumberOfDescendants_ << ")";
#else
	out << "(" << numberOfNodes_ << "," << sumSize_ << ","
	    << sumChildSize_ << "," << sumDescendantSize_ << ","
	    << sumNumberOfChildren_ << "," << sumNumberOfDescendants_ << ")";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

StructuralStatsDatabase::StructuralStatsDatabase(DB_ENV *env, Transaction *txn,
                                                 const std::string &name,
                                                 const ContainerConfig &config,
                                                 bool usingCDB)
	: db_(env, name, "", structural_stats_name, DEFAULT_CONFIG),
	  inMemory_(false)
{
        // child txn is required because of [#16136] -- a DB issue
        // the requires we close failed transactional opens *after*
        // a commit or abort (issue is 4.6.x only but safe for 4.7+)
        TransactionGuard txnGuard;
        Transaction *child = 0;
        if (txn != 0 && !usingCDB) {
                child = txn->createChildInternal(txn->getFlags());
                txnGuard = child;
        }

	int err = db_.open((child ? child : txn), DB_BTREE, config);
        txnGuard.commit();
        
	if (err != 0) {
		if (err == EINVAL)
			err = ENOENT; // 4.4 will return EINVAL for
		                       // non-existent in-memory DBs
		if (err != ENOENT && txn)
			txn->abort();
		// unconditionally cleanup -- ignore errors
		db_.close(0);
		if(err == EEXIST)
			throw XmlException(
				XmlException::CONTAINER_EXISTS,
				db_strerror(err));
		throw XmlException(err); // this creates a DB error
	}
}

StructuralStatsDatabase::StructuralStatsDatabase()
	: db_(0, "", "", "", DEFAULT_CONFIG),
	  inMemory_(true)
{
	ContainerConfig config;
	config.setAllowCreate(true);
	int err = db_.open(0, DB_BTREE, config);

	if (err != 0) throw XmlException(err);
}

static void marshalKey(const NameID &id1, const NameID &id2, DbtOut &dbt)
{
	DBXML_ASSERT(id1 != 0 || id2 == 0);

	int size = 1;
	if(id1 != 0) size += id1.marshalSize();
	if(id2 != 0) size += id2.marshalSize();

	dbt.set(0, size);

	xmlbyte_t *ptr = (xmlbyte_t*)dbt.data;
	*ptr++ = KEY_PREFIX_BYTE;
	if(id1 != 0) ptr += id1.marshal(ptr);
	if(id2 != 0) ptr += id2.marshal(ptr);
}

static void unmarshalKey(const DbtOut &dbt, NameID &id1, NameID &id2)
{
	const xmlbyte_t *ptr = (const xmlbyte_t*)dbt.data;
	const xmlbyte_t *end = ptr + dbt.size;

	++ptr; // Skip the prefix
	if(ptr < end) ptr += id1.unmarshal(ptr);
	if(ptr < end) ptr += id2.unmarshal(ptr);
}

Transaction *StructuralStatsDatabase::getTxn(OperationContext &oc) const
{
	return inMemory_ ? 0 : oc.txn();
}

int StructuralStatsDatabase::getStats(OperationContext &context, const NameID &id1, const NameID &id2,
	StructuralStats &stats) const
{
	if(id1 == 0) return getStats(context, stats);
	if(id2 == 0) return getStats(context, id1, stats);

	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_READ, 0);
	if(myCursor.error() != 0) return myCursor.error();

	// Lookup the descendant information
	marshalKey(id1, id2, context.key());
	int err = myCursor.get(context.key(), context.data(), DB_SET);
	if(err == DB_NOTFOUND) return getStats(context, id1, stats);
	if(err != 0) return err;

	stats.unmarshal(context.data());

	// Lookup the node information
	marshalKey(id1, 0, context.key());
	err = myCursor.get(context.key(), context.data(), DB_SET);
	DBXML_ASSERT(err != DB_NOTFOUND);
	if(err != 0) return err;

	stats.unmarshal(context.data());

	if(stats.sumSize_ == 0 && stats.numberOfNodes_ != 0) {
		// Fill in an estimate for the missing size values that you get with DLS
		StructuralStats allStats;
		err = getStats(context, id1, allStats);
		if(err != 0) return err;

		stats.sumSize_ = allStats.sumSize_;
		stats.sumChildSize_ = allStats.sumChildSize_;
		stats.sumDescendantSize_ = allStats.sumDescendantSize_;
	}

	return 0;
}

int StructuralStatsDatabase::getStats(OperationContext &context, const NameID &id1, StructuralStats &stats) const
{
	if(id1 == 0) return getStats(context, stats);

	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_READ, 0);
	if(myCursor.error() != 0) return myCursor.error();

	NameID cid1, cid2;
	StructuralStats current;

	// Find the node information
	marshalKey(id1, 0, context.key());
	int err = myCursor.get(context.key(), context.data(), DB_SET);
	if(err == DB_NOTFOUND) return getStats(context, stats);

	// Loop over the node information and all it's descendant information
	while(err == 0) {
		cid1.reset();
		unmarshalKey(context.key(), cid1, cid2);
		if(id1 != cid1) break;

		current.reset();
		current.unmarshal(context.data());
		stats.add(current);

		err = myCursor.get(context.key(), context.data(), DB_NEXT);
	}

	if(err != DB_NOTFOUND && err != 0) return err;

	if(stats.sumSize_ == 0 && stats.numberOfNodes_ != 0) {
		// Fill in an estimate for the missing size values that you get with DLS
		stats.sumSize_ = NODE_SIZE * stats.numberOfNodes_;
		stats.sumChildSize_ = NODE_SIZE * stats.sumNumberOfChildren_;
		stats.sumDescendantSize_ = NODE_SIZE * stats.sumNumberOfDescendants_;
	}

	return 0;
}

int StructuralStatsDatabase::getStats(OperationContext &context, StructuralStats &stats) const
{
	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_READ, 0);
	if(myCursor.error() != 0) return myCursor.error();

	StructuralStats current;

	// Loop over every node and every node's descendant information
	int err;
	while((err = myCursor.get(context.key(), context.data(), DB_NEXT)) == 0) {
		current.reset();
		current.unmarshal(context.data());
		stats.add(current);
	}

	if(err != DB_NOTFOUND && err != 0) return err;

	if(stats.sumSize_ == 0 && stats.numberOfNodes_ != 0) {
		// Fill in an estimate for the missing size values that you get with DLS
		stats.sumSize_ = NODE_SIZE * stats.numberOfNodes_;
		stats.sumChildSize_ = NODE_SIZE * stats.sumNumberOfChildren_;
		stats.sumDescendantSize_ = NODE_SIZE * stats.sumNumberOfDescendants_;
	}

	return 0;
}

int StructuralStatsDatabase::addStats(OperationContext &context, const NameID &id1, const NameID &id2,
	const StructuralStats &stats)
{
	DBXML_ASSERT(id1 != 0 || id2 == 0);

	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_WRITE, 0);
	if(myCursor.error() != 0) return myCursor.error();

	marshalKey(id1, id2, context.key());
	int err = myCursor.get(context.key(), context.data(), DB_SET);
	if(err != DB_NOTFOUND && err != 0) return err;

	if(err == DB_NOTFOUND) {
		stats.marshal(context.data(), id2 == 0);
		err = myCursor.put(context.key(), context.data(), DB_KEYLAST);
	} else {
		StructuralStats current;
		current.unmarshal(context.data());
		current.add(stats);

		current.marshal(context.data(), id2 == 0);
		err = myCursor.put(context.key(), context.data(), DB_CURRENT);
	}

	return err;
}

int StructuralStatsDatabase::subtractStats(OperationContext &context, const NameID &id1, const NameID &id2,
	const StructuralStats &stats)
{
	DBXML_ASSERT(id1 != 0 || id2 == 0);

	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_WRITE, 0);
	if(myCursor.error() != 0) return myCursor.error();

	marshalKey(id1, id2, context.key());
	int err = myCursor.get(context.key(), context.data(), DB_SET);
	if(err != DB_NOTFOUND && err != 0) return err;

	StructuralStats current;

	if(err == DB_NOTFOUND) {
		current.subtract(stats);

		current.marshal(context.data(), id2 == 0);
		err = myCursor.put(context.key(), context.data(), DB_KEYLAST);
	} else {
		current.unmarshal(context.data());
		current.subtract(stats);

		current.marshal(context.data(), id2 == 0);
		err = myCursor.put(context.key(), context.data(), DB_CURRENT);
	}

	return err;
}

int StructuralStatsDatabase::addStats(OperationContext &context, const StructuralStatsDatabase &sdb)
{
	Cursor myCursor(db_, getTxn(context), CURSOR_WRITE, 0);
	if(myCursor.error() != 0) return myCursor.error();

	Cursor sdbCursor(const_cast<DbWrapper&>(sdb.db_), sdb.getTxn(context), CURSOR_READ, 0);
	if(sdbCursor.error() != 0) return sdbCursor.error();

	int err = 0;
	StructuralStats stats;
	StructuralStats current;
	bool nodeStats;

	while((err = sdbCursor.get(context.key(), context.data(), DB_NEXT)) == 0) {

		stats.reset();
		nodeStats = stats.unmarshal(context.data());

		err = myCursor.get(context.key(), context.data(), DB_SET);
		if(err == DB_NOTFOUND) {
			stats.marshal(context.data(), nodeStats);
			err = myCursor.put(context.key(), context.data(), DB_KEYLAST);
		} else if(err == 0) {
			current.unmarshal(context.data());
			current.add(stats);
			current.marshal(context.data(), nodeStats);
			err = myCursor.put(context.key(), context.data(), DB_CURRENT);
		} else break;
	}

	if(err != DB_NOTFOUND) return err;

	return 0;
}

int StructuralStatsDatabase::addStats(OperationContext &context, const StructuralStatsWriteCache &cache)
{
	Cursor myCursor(db_, getTxn(context), CURSOR_WRITE, 0);
	if(myCursor.error() != 0) return myCursor.error();

	int err = 0;
	StructuralStats current;

	StructuralStatsWriteCache::Map::const_iterator it = cache.cache_.begin();
	for(; err == 0 && it != cache.cache_.end(); ++it) {

		StructuralStatsWriteCache::InnerMap::const_iterator it2 = it->second.begin();
		for(; it2 != it->second.end(); ++it2) {

			marshalKey(it->first, it2->first, context.key());

			err = myCursor.get(context.key(), context.data(), DB_SET);
			if(err == DB_NOTFOUND) {
				it2->second.marshal(context.data(), it2->first == 0);
				err = myCursor.put(context.key(), context.data(), DB_KEYLAST);
			} else if(err == 0) {
				current.unmarshal(context.data());
				current.add(it2->second);
				current.marshal(context.data(), it2->first == 0);
				err = myCursor.put(context.key(), context.data(), DB_CURRENT);
			} else break;
		}
	}

	if(err != DB_NOTFOUND) return err;

	return 0;
}

void StructuralStatsDatabase::display(OperationContext &context, ostream &out, const DictionaryDatabase *ddb) const
{
	Cursor myCursor(const_cast<DbWrapper&>(db_), getTxn(context), CURSOR_WRITE, 0);
	if(myCursor.error() != 0) throw XmlException(myCursor.error());

	NameID id1;
	NameID id2;
	StructuralStats stats;

	int err = 0;
	while((err = myCursor.get(context.key(), context.data(), DB_NEXT)) == 0) {
		id1.reset();
		id2.reset();
		unmarshalKey(context.key(), id1, id2);
		stats.reset();
		stats.unmarshal(context.data());

		if(ddb) out << ddb->lookupName(context, id1);
		else out << id1;

		if(id2 != 0) {
			if(ddb) out << " -> " << ddb->lookupName(context, id2);
			else out << " -> " << id2;
		}

		out << ": ";

		stats.display(out);

		out << endl;
	}
}

int StructuralStatsDatabase::remove(DB_ENV *env, Transaction *txn, const std::string &name)
{
	int err = env->dbremove(env, Transaction::toDB_TXN(txn), name.c_str(), structural_stats_name, 0);
	if (err == ENOENT) err = 0; // The database didn't exist
	return err;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

StructuralStats StructuralStatsCache::get(const ContainerBase *container, OperationContext &oc,
	const NameID &id1, const NameID &id2)
{
	Key key(container->getContainerID(), id1, id2);

	Map::iterator i = cache_.find(key);
	if (i == cache_.end()) {
		i = cache_.insert(Map::value_type(key, container->getStructuralStats(oc, id1, id2))).first;
	}

	return i->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void StructuralStatsWriteCache::add(const NameID &id1, const map<NameID, StructuralStats> &values)
{
	InnerMap &mp = cache_[id1];
	InnerMap::iterator hint = mp.begin();

	for(InnerMap::const_iterator it = values.begin(); it != values.end(); ++it) {
		hint = mp.insert(hint, InnerMap::value_type(it->first, StructuralStats()));
		hint->second.add(it->second);
	}
}

void StructuralStatsWriteCache::subtract(const NameID &id1, const map<NameID, StructuralStats> &values)
{
	InnerMap &mp = cache_[id1];
	InnerMap::iterator hint = mp.begin();

	for(InnerMap::const_iterator it = values.begin(); it != values.end(); ++it) {
		hint = mp.insert(hint, InnerMap::value_type(it->first, StructuralStats()));
		hint->second.subtract(it->second);
	}
}

void StructuralStatsWriteCache::clear()
{
	cache_.clear();
}

