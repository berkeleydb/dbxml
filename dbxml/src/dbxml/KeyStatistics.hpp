//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __KEYSTATISTICS_HPP
#define	__KEYSTATISTICS_HPP

#include <string>
#include <iostream>

#include "nodeStore/NsTypes.hpp"

namespace DbXml
{
class DbtOut;

class KeyStatistics
{
public:
	KeyStatistics();

	void add(const KeyStatistics& es);
	double averageKeyValueSize() const;
	void zero();

	void setThisFromDbt(const DbtOut &dbt);
	void setDbtFromThis(DbtOut &dbt) const;

	int marshal(xmlbyte_t *ptr, bool count) const;
	void unmarshal(const xmlbyte_t *ptr);

	std::string asString() const;

	int32_t numIndexedKeys_;
	int32_t numUniqueKeys_;
	int32_t sumKeyValueSize_;
};

std::ostream& operator<<(std::ostream& s, const KeyStatistics &ks);

}

#endif

