//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "Transaction.hpp"
#include "Manager.hpp"
#include "Log.hpp"
#include "DictionaryDatabase.hpp"

#if DBVER > 44
#define CDB_ALLOWED 1
#endif

namespace DbXml {
	
/* Holds the string cache created for each transaction.  This is done
 * too speed up name->id lookups in the dictionary database and to 
 * prevent aborted inserts from sticking around in the cache.  When
 * the transaction is committed the cache is discarded, since the 
 * inserted values can now be safely read from the dictionary
 * database.
 */
class StringCacheNotify : public Transaction::Notify
{
public:
	StringCacheNotify(Transaction *txn, DictionaryDatabase *ddb)
		: ddb_(ddb), txn_(txn), dsc_(false) {}
	DictionaryStringCache *getStringCache() { return &dsc_; }
	DictionaryDatabase *getDictionaryDatabase() { return ddb_; }
	virtual void postNotify(bool commit) { delete this; }
private:
	DictionaryDatabase *ddb_;
	Transaction *txn_;
	DictionaryStringCache dsc_;
};

}

using namespace DbXml;

extern "C" {
	static int abortFunction(DB_TXN *dbtxn);
	static int commitFunction(DB_TXN *dbtxn, u_int32_t flags);
};

// NOTE: Because a DbTxn is external, this constructor
// creates a hard reference on the
// object, which is only released by a commit or abort
// operation.  This allows the caller to commit or abort
// directly on the DbTxn object, even after a containing
// XmlTransaction object has been deleted.
Transaction::Transaction(Manager &mgr, DB_TXN *txn)
	: txn_(txn), child_(0), mgr_(mgr), owned_(false), cdb_(false), flags_(0)
{
	// caller ensures txn is non-null
	setDbNotification(txn_);
	acquire();
}

Transaction::Transaction(Manager &mgr, u_int32_t flags, bool cdb)
	: txn_(0), child_(0), mgr_(mgr), owned_(true), cdb_(cdb), flags_(flags)
{
	int err = 0;
	DB_ENV *env = mgr.getDB_ENV();
	if(cdb)
		err = env->cdsgroup_begin(env, &txn_);
	else
		err = env->txn_begin(env, 0, &txn_, flags);

	if(err)
		throw XmlException(err);
	setDbNotification(txn_);
}

Transaction::Transaction(Manager &mgr, DB_TXN *parent, u_int32_t flags)
	: txn_(0), child_(0), mgr_(mgr), owned_(true), cdb_(false), flags_(flags)
{
	int err = 0;
	DB_ENV *env = mgr.getDB_ENV();
	err = env->txn_begin(env, parent, &txn_, flags);
	if(err)
		throw XmlException(err);
	setDbNotification(txn_);
}

Transaction::~Transaction()
{
	// Make the transaction exception safe
	if(txn_) {
		// don't allow a destructor to throw
		try { abort(); } catch (...) {}
	}
}

void Transaction::abort()
{
	if (child_) {
		childOp(false);
		return;
	}

	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot abort, transaction already committed or aborted");
	}
	int err = runDbNotification(txn_,
				    /*isCommit*/ cdb_, // (an abort acts like a commit if this is a CDB txn)
				    /*fromDB*/ false, 0);
	if(err)
		throw XmlException(err);
}

void Transaction::commit(u_int32_t flags)
{
	if (child_) {
		childOp(true, flags);
		return;
	}
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot commit, transaction already committed or aborted");
	}
	int err = runDbNotification(txn_, /*isCommit*/ true, /*fromDB*/ false, flags);
	if(err)
		throw XmlException(err);
}

void Transaction::childOp(bool isCommit, u_int32_t flags)
{
	try {
		if (isCommit)
			child_->commit();
		else
			child_->abort();
	}
	catch (...) {
		child_->releaseTransaction();
		child_ = 0;
		throw;
	}
	child_->releaseTransaction();
	child_ = 0;
}

Transaction *Transaction::createChild(u_int32_t flags)
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot create child, transaction already committed or aborted");
	}
	((Manager&)mgr_).checkFlags(Log::misc_flag_info,
				    "XmlTransaction::createChild()", flags,
				    DB_READ_UNCOMMITTED|DB_TXN_NOSYNC|
				    DB_TXN_NOWAIT|DB_TXN_SNAPSHOT|
				    DB_TXN_SYNC|DB_READ_COMMITTED);
	return new Transaction(mgr_, txn_, flags);
}

Transaction *Transaction::createChildInternal(u_int32_t flags)
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot create child, transaction already committed or aborted");
	}
	if (child_) {
#if 1
		return child_->createChildInternal(flags);
#else
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "Cannot create internal child transaction if a child already exists");
#endif
	}

	((Manager&)mgr_).checkFlags(Log::misc_flag_info,
				    "XmlTransaction::createChild()", flags,
				    DB_READ_UNCOMMITTED|DB_TXN_NOSYNC|
				    DB_TXN_NOWAIT|DB_TXN_SNAPSHOT|
				    DB_TXN_SYNC|DB_READ_COMMITTED);
	child_ = new Transaction(mgr_, txn_, flags);
	child_->acquire();
	return this;
}

DB_TXN *Transaction::getDB_TXN()
{
	if (child_)
		return child_->getDB_TXN();
	// if no txn_ and using CDB, the txn was auto-committed, ignore
	// the situation and return NULL.
	if(!txn_ && !cdb_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot get DB_TXN, transaction already committed or aborted");
	}
	return txn_;
}

void Transaction::preNotify(bool commit) const
{
	NotifyList::const_iterator end = notify_.end();
	NotifyList::const_iterator i = notify_.begin();
	for(; i != end; ++i) {
		(*i)->preNotify(commit);
	}
}

void Transaction::postNotify(bool commit) const
{
	NotifyList::const_iterator end = notify_.end();
	NotifyList::const_iterator i = notify_.begin();
	for(; i != end; ++i) {
		(*i)->postNotify(commit);
	}
}

void Transaction::unregisterNotify(Notify *notify)
{
	NotifyList::iterator end = notify_.end();
	NotifyList::iterator i = notify_.begin();
	while(i != end) {
		if(*i == notify) {
			i = notify_.erase(i);
			end = notify_.end();
		}
		else {
			++i;
		}
	}
}

void Transaction::setDbNotification(DB_TXN *txn)
{
	// cannot wrap the same DB_TXN twice -- there is only
	// one slot available
	if (txn->abort == abortFunction)
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot create XmlTransaction:  DbTxn is used by existing XmlTransaction");
	dbfuncs_.saved_abort_func = txn->abort;
	dbfuncs_.saved_commit_func = txn->commit;
	txn->abort = abortFunction;
	txn->commit = commitFunction;
	txn->xml_internal = (void *)this;
}

void Transaction::clearDbNotification(DB_TXN *txn)
{
	// caller will "release()" the reference
	txn->abort = dbfuncs_.saved_abort_func;
	txn->commit = dbfuncs_.saved_commit_func;
	txn->xml_internal = 0;
	txn_ = 0;
}

int Transaction::runDbNotification(DB_TXN *dbtxn, bool isCommit,
				   bool fromDB, u_int32_t commitFlags)

{
	int err = 0;
	DB_TXN *dbTxn = txn_;
	clearDbNotification(dbtxn); // nulls txn_
	preNotify(isCommit);
	// The DbTxn method will delete the DbTxn.  This should only
	// happen if both (1) we own the DbTxn and (2) the call is not
	// coming via DB.  In the latter case, the DbTxn is going to
	// be deleted by DbTxn::commit().
	if (isCommit) {
		if (owned_ && !fromDB)
			err = dbTxn->commit(dbTxn, commitFlags);
		else
			err = dbfuncs_.saved_commit_func(dbtxn, commitFlags);
	} else {
		if (owned_ && !fromDB)
			err = dbTxn->abort(dbTxn);
		else
			err = dbfuncs_.saved_abort_func(dbtxn);
	}
	postNotify(isCommit);
	// If this object doesn't own the DbTxn, the hard reference
	// created during construction must be released
	if (!isOwned()) {
		owned_ = true; // keep state consistent
		release();
	}
	return err;
}

/*
 * A string cache can exist for each transaction/dictionary database pair.  The cache
 * is created during the first insert into the dictionary database under the transaction.
 * When the transaction is committed or abortted the cache is destroyed.  In this function
 * it attempts to find the string cache for the given dictionary database in the caches_
 * vector.  Entries in the cache vector are uniquely identified by dictionary
 * database pointer value.
 */
DictionaryStringCache *Transaction::getStringCache(DictionaryDatabase *ddb, bool create)
{
	if (caches_.size()) {
		for (CacheList::iterator itr = caches_.begin(); itr != caches_.end(); ++itr) {
			if ((*itr)->getDictionaryDatabase() == ddb)
				return (*itr)->getStringCache();
		}
	}
	if (create) {
		StringCacheNotify *scn = new StringCacheNotify(this, ddb);
		caches_.push_back(scn);
		registerNotify(scn);
		return scn->getStringCache();
	}
	return NULL;
}

// Normally, this is a simple release()
// If
//  1) a DbTxn is wrapped (!isOwned)  and
//  2) there are no notifications registered and
//  3) there are only 2 references
// it is OK to release the extra hard reference
// created by the wrapping of DbTxn
// TBD -- if ReferenceCounted::release were virtual, that'd be better
void Transaction::releaseTransaction()
{
	if (!owned_ && (count() == 2) && (notify_.size() == 0)) {
		if (txn_)
			clearDbNotification(txn_); // nulls txn_
		release();
	}
	release();
}

extern "C" {
static int abortFunction(DB_TXN *txn)
{
	Transaction *xmlTxn = (Transaction *)txn->xml_internal;
	int err = 0;
	if (xmlTxn) {
		err = xmlTxn->runDbNotification(txn, false,
						true, // fromDB
						0);
	}
	return err;
}

static int commitFunction(DB_TXN *txn, u_int32_t flags)
{
	Transaction *xmlTxn = (Transaction *)txn->xml_internal;
	int err = 0;
	if (xmlTxn) {
		err = xmlTxn->runDbNotification(txn, true,
						true, // fromDB
						flags);
	}
	return err;
}
}

Transaction *Transaction::autoTransact(Transaction *txn, Manager &mgr,
	TransactionGuard &txnGuard, bool usingTxns, bool usingCDB)
{
	if(txn == 0) {
		if(usingTxns
#ifdef CDB_ALLOWED
			|| usingCDB
#endif
			) {
			txn = new Transaction(mgr, 0, usingCDB);
			txnGuard = txn;
		}
	} else if(!usingCDB) {
		txn->createChildInternal();
		txnGuard = txn;
	}
	return txn;
}

TransactionGuard::TransactionGuard(Transaction *txn)
	: txn_(txn)
{
	if(txn_ != 0) txn_->acquire();
}

TransactionGuard &TransactionGuard::operator=(Transaction *txn)
{
	if(txn_ != txn) {
		if(txn_ != 0) {
			if(txn_->txn_) txn_->abort();
			txn_->releaseTransaction();
		}
		txn_ = txn;
		if(txn_ != 0) txn_->acquire();
	}
	return *this;
}

TransactionGuard::~TransactionGuard()
{
	if(txn_ != 0) {
		// if this destructor is being run it is
		// likely an exception is being thrown,
		// so don't throw another
		if (txn_->txn_) {
			try { txn_->abort(); } catch (...){}
		}
		txn_->releaseTransaction();
	}
}

void TransactionGuard::commit()
{
	if(txn_ != 0) {
		if(txn_->txn_) txn_->commit();
		txn_->releaseTransaction();
		txn_ = 0;
	}
}
