//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __TRANSACTION_HPP
#define	__TRANSACTION_HPP

#include "dbxml/XmlManager.hpp"
#include "ReferenceCounted.hpp"
#include "db.h"
#include <vector>

extern "C" {
	typedef int (*db_abort_func)(DB_TXN*);
	typedef int (*db_commit_func)(DB_TXN*, u_int32_t);
	typedef struct {
		db_abort_func saved_abort_func;
		db_commit_func saved_commit_func;
	} db_txn_funcs;
};

namespace DbXml
{

class TransactionGuard;
class DictionaryDatabase;
class DictionaryStringCache;
class StringCacheNotify;

/// Reference counted by XmlTransaction
class Transaction : public ReferenceCounted
{
public:
	class Notify {
	public:
		virtual ~Notify() {}
		virtual void preNotify(bool commit) {}
		virtual void postNotify(bool commit) {}
	};

	Transaction(Manager &mgr, DB_TXN *dbtxn);
	Transaction(Manager &mgr, u_int32_t flags, bool cdb = false);
	/** Aborts the transaction, if it hasn't been committed or
	    aborted already. */
	~Transaction();

	void abort();
	void commit(u_int32_t flags = 0);

	Transaction *createChild(u_int32_t flags = 0);
	Transaction *createChildInternal(u_int32_t flags = 0);
	void childOp(bool isCommit, u_int32_t flags = 0);
	
	void registerNotify(Notify *notify) {
		notify_.push_back(notify);
	}
	void unregisterNotify(Notify *notify);
	
	DB_TXN *getDB_TXN();

	static DB_TXN *toDB_TXN(Transaction *t) {
		return t ? t->getDB_TXN() : 0;
	}
	// a special release function to handle wrapping of DB_TXN
	void releaseTransaction();
	
	// these need to be public for commit/abort C functions
	int runDbNotification(DB_TXN *dbtxn, bool isCommit,
			      bool fromDB, u_int32_t commitFlags);
	bool isOwned() const { return owned_; }
	u_int32_t getFlags() const { return flags_; }

	/* Each dictionary database has a string cache and there is a
	 * dictionary database for each container plus one for the XmlManager 
	 */
	DictionaryStringCache *getStringCache(DictionaryDatabase *ddb, bool create);

	static Transaction *autoTransact(Transaction *txn, Manager &mgr,
		TransactionGuard &txnGuard, bool usingTxns, bool usingCDB);

private:
	// TODO deal with child transactions, notifications etc. - jpcs
	Transaction(Manager &mgr, DB_TXN *parent, u_int32_t flags);
	Transaction(const Transaction &);
	Transaction &operator=(const Transaction &);
	void setDbNotification(DB_TXN *txn);
	void clearDbNotification(DB_TXN *txn);
	void preNotify(bool commit) const;
	void postNotify(bool commit) const;
private:
	friend class TransactionGuard;

	typedef std::vector<Notify*> NotifyList;
	typedef std::vector<StringCacheNotify*> CacheList;
	DB_TXN *txn_;
	Transaction *child_;
	XmlManager mgr_;
	NotifyList notify_;
	CacheList caches_;
	db_txn_funcs dbfuncs_;
	bool owned_;
	bool cdb_;
	u_int32_t flags_;
};

class TransactionGuard
{
public:
	TransactionGuard(Transaction *txn = 0);
	TransactionGuard &operator=(Transaction *txn);
	~TransactionGuard();

	void commit();

private:
	TransactionGuard(const TransactionGuard &o);
	TransactionGuard &operator=(const TransactionGuard &o);

	Transaction *txn_;
};

}

#endif
