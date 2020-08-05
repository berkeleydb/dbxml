//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __BULKPUT_HPP
#define	__BULKPUT_HPP

#include <dbxml/XmlPortability.hpp>
#include "DbXmlInternal.hpp"
#include "ScopedDbt.hpp"

// use bulk put for DB 4.8 and higher
#if DBVER >= 48
#define DBXML_BULK 1
#endif

namespace DbXml
{
class Transaction;
class DbXmlDbt;
class DbWrapper;
	
//
// define BulkPut class in OperationContext because that's
// where it lives.  Could be made a separate file or two.
//
class BulkPut {
public:
	BulkPut(size_t bufSize);
	~BulkPut();
	bool store(Transaction *txn, DbXmlDbt *key, DbXmlDbt *data);
	void flush(Transaction *txn);
	void setDbWrapper(DbWrapper *wrapper) { db_ = wrapper; }
private:
	DbXmlDbt multiple_;
	void *ptr_;
	DbWrapper *db_;
};
}
#endif
