//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLTRANSACTION_HPP
#define	__XMLTRANSACTION_HPP

#include <db.h>
#include "DbXmlFwd.hpp"

namespace DbXml
{

class Transaction;
	
/** Constructed using the method on XmlManager. If the XmlTransaction
    has no more references left, and has not already been committed or
    aborted, it aborts itself. */
class DBXML_EXPORT XmlTransaction
{
public:
	/** @name For Reference Counting */
	XmlTransaction();
	XmlTransaction(const XmlTransaction &);
	XmlTransaction &operator=(const XmlTransaction &);
	XmlTransaction &operator=(Transaction *);
	~XmlTransaction();
	bool isNull() const { return transaction_ == 0; }
	
	/** @name Transaction Methods */
	void abort();
	void commit(u_int32_t flags = 0);
	
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_TXN_NOSYNC, DB_TXN_NOWAIT,
	 * DB_READ_COMMITTED, DB_TXN_SYNC, DB_TXN_SNAPSHOT
	 */
	XmlTransaction createChild(u_int32_t flags = 0);
	
	/** @name Berkeley DB Compatibility */
	DB_TXN *getDB_TXN();
	
	/** @name Private Methods (for internal use) */
	// @{
	
	XmlTransaction(Transaction *transaction);
	operator Transaction *();
	operator Transaction &();
	
	//@}
	
private:
	Transaction *transaction_;
};

}

#endif
