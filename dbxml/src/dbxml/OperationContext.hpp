//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __OPERATIONCONTEXT_HPP
#define	__OPERATIONCONTEXT_HPP

#include <dbxml/XmlPortability.hpp>
#include "ScopedDbt.hpp"
#include "Transaction.hpp"
#include "BulkPut.hpp"

namespace DbXml
{
class OperationContext
{
public:
	OperationContext(Transaction *t = 0) : txn_(t), bp_(0)
	{
		if(txn_) txn_->acquire();
	}
	
	~OperationContext()
	{
		if(txn_) txn_->release();
		if (bp_)
			delete bp_;
	}

	Transaction *txn()
	{
		return txn_;
	}
	DB_TXN *db_txn()
	{
		return Transaction::toDB_TXN(txn_);
	}
	DbtOut &key()
	{
		return key_;
	}
	DbtOut &data()
	{
		return data_;
	}

	void set(Transaction *t)
	{
		if(txn_) txn_->release();
		txn_ = t;
		if(txn_) txn_->acquire();
	}
	OperationContext *copy() const
	{
		return new OperationContext(*this);
	}

	Transaction *saveTxn() {
		Transaction *tmp = txn_;
		txn_ = 0;
		return tmp;
	}

	void restoreTxn(Transaction *txn) {
		txn_ = txn;
	}
	BulkPut *getBulkPut(bool allocate); // implemented in BulkPut.cpp
private:
	OperationContext(const OperationContext &o)
		: key_(o.key_),
		  data_(o.data_),
		  txn_(o.txn_)
	{
		if(txn_) txn_->acquire();
	}
	// no need for assignment
	OperationContext &operator=(const OperationContext &);

	DbtOut key_;
	DbtOut data_;
	Transaction *txn_;
	BulkPut *bp_;
};
	
}

#endif
