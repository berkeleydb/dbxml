//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "Statistics.hpp"
#include "SyntaxManager.hpp"
#include "Container.hpp"
#include "Manager.hpp"
#include "OperationContext.hpp"
#include "Cursor.hpp"
#include "Value.hpp"

#include <string>
#include <sstream>

using namespace std;
using namespace DbXml;

Statistics::Statistics(const KeyStatistics &keyStats,
		       Index &index, const XmlValue &value)
	: numIndexedKeys_(keyStats.numIndexedKeys_),
	  numUniqueKeys_(keyStats.numUniqueKeys_),
	  sumKeyValueSize_(keyStats.sumKeyValueSize_)
{
	if(index.getKey() == Index::KEY_EQUALITY && !value.isNull() &&
	   numUniqueKeys_ != 0.0) {
		numIndexedKeys_ /= numUniqueKeys_;
		sumKeyValueSize_ /= numUniqueKeys_;
		numUniqueKeys_ = 1.0;
	}
}
	

/////////////////////////////////
// Cost

void Cost::unionOp(const Cost &cost)
{
	pagesOverhead += cost.pagesOverhead;
	pagesForKeys += cost.pagesForKeys;
	keys += cost.keys;
}

void Cost::intersectOp(const Cost &cost)
{
	pagesOverhead += cost.pagesOverhead;

	if(cost.keys < keys) {
		pagesForKeys = (pagesForKeys / keys) * cost.keys;
		keys = cost.keys;
		pagesForKeys += cost.pagesForKeys;
	} else {
		pagesForKeys += (cost.pagesForKeys / cost.keys) * keys;
	}
}

int Cost::compare(const Cost &cost)
{
	if(keys < cost.keys) return -1;
	if(keys > cost.keys) return +1;

	if(totalPages() < cost.totalPages()) return -1;
	if(totalPages() > cost.totalPages()) return +1;

	return 0;
}


StatisticsWriteCache::StatisticsWriteCache()
	: dkv_(SyntaxManager::getInstance()->size())
{}

StatisticsWriteCache::~StatisticsWriteCache()
{
	reset();
}

void StatisticsWriteCache::reset()
{
	Dbt2KSMapVector::iterator i;
	for (i = dkv_.begin();i != dkv_.end();++i) {
		delete *i;
		*i = 0;
	}
}

// We assume that key has the correct endianness.
void StatisticsWriteCache::addToKeyStatistics(const Index &index, const DbXmlDbt &key,
					      const DbXmlDbt &data, bool unique)
{
	const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)index.getSyntax());

	Dbt2KSMap* dk = dkv_[syntax->getType()];
	if (dk == 0) {
		dk = new Dbt2KSMap;
		dkv_[syntax->getType()] = dk;
	}

	size_t skl = Key::structureKeyLength(index, key);
	if (skl > 0 && skl <= key.size) {
		DbtIn k(key.data, skl);
		Dbt2KSMap::iterator i = dk->find(k);
		KeyStatistics *ps = 0;
		if (i == dk->end()) {
			KeyStatistics s;
			dk->insert(Dbt2KSMap::value_type(k, s));
			i = dk->find(k);
			ps = &i->second;
		} else {
			ps = &i->second;
		}
		if(index.indexerAdd()) {
			ps->numIndexedKeys_++;
			ps->sumKeyValueSize_ += key.size + data.size;
			if (unique) {
				ps->numUniqueKeys_++;
			}
		} else {
			ps->numIndexedKeys_--;
			ps->sumKeyValueSize_ -= key.size + data.size;
			if (unique) {
				ps->numUniqueKeys_--;
			}
		}
	}
}

int StatisticsWriteCache::updateContainer(OperationContext &context, Container &container) const
{
	int err = 0;
	int syntax = 0;
	Dbt2KSMapVector::const_iterator i;
	for (i = dkv_.begin();err == 0 && i != dkv_.end();++i) {
		Dbt2KSMap* dk = *i;
		if (dk != 0) {
			Dbt2KSMap::iterator i2;
			for (i2 = dk->begin();i2 != dk->end();++i2) {
				DbtIn &k = (DbtIn&)i2->first;  // jcm const_cast
				SyntaxDatabase *database =
					container.getIndexDB(
						(Syntax::Type)syntax);
				if (database)
					err = database->
						updateStatistics(
							context, k, i2->second);
			}
		}
		syntax++;
	}
	return err;
}

