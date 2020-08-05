//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "ScopedDbt.hpp"
#include "db_utils.h"
#include "Buffer.hpp"
#include "KeyStatistics.hpp"
#include "Manager.hpp"
#include "nodeStore/NsFormat.hpp"

#include <sstream>

using namespace DbXml;

#define PREFIX_BYTE 0

KeyStatistics::KeyStatistics()
{
	zero();
}

void KeyStatistics::zero()
{
	numIndexedKeys_ = 0;
	numUniqueKeys_ = 0;
	sumKeyValueSize_ = 0;
}

void KeyStatistics::add(const KeyStatistics& es)
{
	numIndexedKeys_ += es.numIndexedKeys_;
	numUniqueKeys_ += es.numUniqueKeys_;
	sumKeyValueSize_ += es.sumKeyValueSize_;
}

double KeyStatistics::averageKeyValueSize() const
{
	return (numIndexedKeys_ == 0 ? 0 : sumKeyValueSize_ / numIndexedKeys_);
}

void KeyStatistics::setThisFromDbt(const DbtOut &dbt)
{
	unmarshal((const xmlbyte_t*)dbt.data);
}

void KeyStatistics::setDbtFromThis(DbtOut &dbt) const
{
	int count = marshal(0, /*count*/true);
	dbt.set(0, count);
	marshal((xmlbyte_t*)dbt.data, /*count*/false);
}

int KeyStatistics::marshal(xmlbyte_t *ptr, bool count) const
{
	int size = 0;

	if(count) {
		size += 1; // For the prefix byte
		
		size += NsFormat::countInt(numIndexedKeys_);
		size += NsFormat::countInt(numUniqueKeys_);
		size += NsFormat::countInt(sumKeyValueSize_);
	} else {
		*ptr++ = PREFIX_BYTE;

		ptr += NsFormat::marshalInt(ptr, numIndexedKeys_);
		ptr += NsFormat::marshalInt(ptr, numUniqueKeys_);
		ptr += NsFormat::marshalInt(ptr, sumKeyValueSize_);
	}

	return size;
}

void KeyStatistics::unmarshal(const xmlbyte_t *ptr)
{
	++ptr; // Skip the prefix

	ptr += NsFormat::unmarshalInt(ptr, &numIndexedKeys_);
	ptr += NsFormat::unmarshalInt(ptr, &numUniqueKeys_);
	ptr += NsFormat::unmarshalInt(ptr, &sumKeyValueSize_);
}

std::string KeyStatistics::asString() const
{
	std::ostringstream s;
	s << "indexed=" << numIndexedKeys_;
	s << " unique=" << numUniqueKeys_;
	s << " size=" << sumKeyValueSize_;
	return s.str();
}

std::ostream& DbXml::operator<<(std::ostream& s, const KeyStatistics &ks)
{
	s << ks.asString();
	return s;
}
