//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __COUNTERS_HPP
#define	__COUNTERS_HPP

#include <iostream>
#include <string.h>

namespace DbXml
{
// Counters
//  A simple class to count internal statistics.
//  It counts in integers, and if they wrap, they wrap.
//  Counters are not thread-safe, and rely on atomic update of
//  integer values.  At this time, they are used as internal
//  statistics and reporting counters
//
class Counters
{
public:
	enum CounterNames {
		// API
		num_putdoc = 0,
		num_getdoc = 1,
		num_updatedoc = 2,
		num_prepare = 3,
		num_query = 4,
		num_queryExec = 5, // may be counted twice
		// Internal
		num_dbget = 6,
		num_dbput = 7,
		num_dbdel = 8,
		num_dbcget = 9,
		num_dbcput = 10,
		num_dbcdel = 11,
		num_docparse = 12,
		NCOUNTERS = 13
	};
	Counters() { reset(); }
	
	void reset(){ memset(counters_, 0, sizeof(counters_)); }
	void incr(int which) {
		++counters_[which];
	}
	int get(int which) const {
		if (which >= 0 && which < NCOUNTERS)
			return counters_[which];
		return -1;
	}

	void set(int which, int value) {
		if (which >= 0 && which < NCOUNTERS)
			counters_[which] = value;
	}

	void dumpToStream(std::ostream &out) const;
	void dump() const;
	
private:
	static const char *names_[NCOUNTERS];
	int counters_[NCOUNTERS];
};

}
#endif
