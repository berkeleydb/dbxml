//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __STATISTICS_HPP
#define	__STATISTICS_HPP

#include <map>
#include <vector>
#include "KeyStatistics.hpp"
#include "ScopedDbt.hpp"
#include "Key.hpp"
#include "ReferenceCounted.hpp"

namespace DbXml
{

class Transaction;
class Container;
class Index;
class Syntax;
class OperationContext;

class Statistics : public ReferenceCounted
{
public:
	Statistics(const KeyStatistics &keyStats,
		   Index &index, const XmlValue &value);

	double getNumberOfIndexedKeys() const
	{
		return numIndexedKeys_;
	}
	double getNumberOfUniqueKeys() const
	{
		return numUniqueKeys_;
	}
	double getSumKeyValueSize() const
	{
		return sumKeyValueSize_;
	}

private:
	Statistics(const Statistics &);
	Statistics &operator=(const Statistics &);

	double numIndexedKeys_;
	double numUniqueKeys_;
	double sumKeyValueSize_;
};

// Cost represents the cost of a piece of a query, used
// by optimization
class Cost {
public:
	Cost() : keys(0), pagesOverhead(0), pagesForKeys(0) {}
	Cost(double k, double po) : keys(k), pagesOverhead(po), pagesForKeys(0) {}
	Cost(double k, double po, double pk) : keys(k), pagesOverhead(po), pagesForKeys(pk) {}
	
	double keys; ///< Max estimated keys returned
	double pagesOverhead; ///< Max estimated pages required no matter how many keys read
	double pagesForKeys; ///< Max estimated pages required if all keys are read

	double totalPages() const { return pagesOverhead + pagesForKeys; }
	void intersectOp(const Cost &cost);
	void unionOp(const Cost &cost);
	int compare(const Cost &cost);
};


/** The indexer makes use of this class to maintain statistics about
    the keys being generated. */
class StatisticsWriteCache
{
public:
	StatisticsWriteCache();
	~StatisticsWriteCache();
	// Reset the statistics object so that it can be reused.
	void reset();

	/// Add a key to the statistics. We assume that key has the correct endianness.
	void addToKeyStatistics(const Index &index, const DbXmlDbt &key,
				const DbXmlDbt &data, bool unique);

	/// Write the statistics to the statistics databases
	int updateContainer(OperationContext &context,
			    Container &container) const;

private:
	StatisticsWriteCache(const StatisticsWriteCache&);
	StatisticsWriteCache &operator=(const StatisticsWriteCache &);

	typedef std::map<DbtIn, KeyStatistics> Dbt2KSMap;
	typedef std::vector<Dbt2KSMap*> Dbt2KSMapVector;
	Dbt2KSMapVector dkv_;
};

}
#endif
