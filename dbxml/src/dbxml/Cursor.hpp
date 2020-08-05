//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CURSOR_HPP
#define	__CURSOR_HPP

#include "ScopedDbt.hpp"
#include "CacheDatabase.hpp"
#include "query/NodeIterator.hpp"

#define DB_TXN_FLAGS DB_RMW|DB_READ_COMMITTED|DB_READ_UNCOMMITTED
#define DB_CURSOR_CREATE_MASK(db, flags) (db).isTransacted() ? ((flags) & ~(DB_RMW)) : ((flags) & ~(DB_TXN_FLAGS))
#define DB_CURSOR_GET_MASK(db, flags) (db).isTransacted() ? ((flags) & ~(DB_READ_COMMITTED)) : ((flags) & ~(DB_TXN_FLAGS))
// provide consistent handling of deadlock and DB errors
#define HANDLE_DB_ERRORS \
if (dberr == DB_LOCK_DEADLOCK) \
	throw XmlException(dberr); \
return dberr;

// Counters
#define INCR_CURSOR_STATS(ctr) Globals::incrementCounter(ctr)

namespace DbXml
{

class ContainerBase;
class Manager;
	
typedef enum CursorType { CURSOR_READ, CURSOR_WRITE } CursorType;

/// Provides exception safety for a Berkeley Db Cursor.
class DBXML_EXPORT Cursor
{
public:
	Cursor();
	Cursor(DbWrapper &db, Transaction *txn, CursorType type,
	       const char *name = 0, u_int32_t flags = 0);
	~Cursor();

	int open(DbWrapper &db, Transaction *txn, CursorType type,
		u_int32_t flags = 0);
	void close()
	{
		if (dbc_ && (error_ == 0)) {
			DBC *tdbc = dbc_;
			dbc_ = 0;
			tdbc->close(tdbc);
		}
	}
	int error() const
	{
		return error_;
	}
	int get(DbXmlDbt &key, DbXmlDbt &data, u_int32_t flags)
	{
		int dberr = dbc_->get(dbc_, &key, &data, flags);
		INCR_CURSOR_STATS(Counters::num_dbcget);
		if (dberr == DB_LOCK_DEADLOCK)
			throw XmlException(dberr);
		if ((dberr == ENOMEM) && (flags|DB_MULTIPLE))
			dberr = DB_BUFFER_SMALL;
		return dberr;
	}
	int put(DbXmlDbt &key, DbXmlDbt &data, u_int32_t flags)
	{
		int dberr = dbc_->put(dbc_, &key, &data, flags);
		INCR_CURSOR_STATS(Counters::num_dbcput);
		HANDLE_DB_ERRORS;
	}
	int del(u_int32_t flags)
	{
		int dberr = dbc_->del(dbc_, flags);
		INCR_CURSOR_STATS(Counters::num_dbcdel);
		HANDLE_DB_ERRORS;
	}
	const char *getCursorName() const { return cursorName_; }
	void setCursorName(const char *name) { cursorName_ = name; }
private:
	// no need for copy and assignment
	Cursor(const Cursor&);
	Cursor &operator = (const Cursor &);

	DBC *dbc_;
	int error_;
	const char *cursorName_;
};

// Wraps a Cursor for a DocumentDatabase
// Virtual behavior is implemented by specific DocumentDatabase impl.
// Construction is via DocumentDatabase::createDocumentCursor()
class DocumentCursor
{
public:
	virtual ~DocumentCursor() {}
	virtual int first(DocID &id) = 0; // done when id == 0
	virtual int next(DocID &id) = 0; // done when id == 0
	virtual int seek(DocID &id) = 0; // seek to >= id, done when id == 0
};

/// Wraps a smart Cursor, for a Secondary (Index) Database.
class IndexCursor
{
public:
	IndexCursor(IndexDatabase &db, Transaction *txn, bool initBulk = true);
	virtual ~IndexCursor();

	int error() const
	{
		return cursor_.error();
	}

	int first(IndexEntry &ie);
	int next(IndexEntry &ie);

	virtual int first() = 0;
	virtual int next() = 0;

	const DbXmlDbt &getData() const { return data_; }
	bool isDone() const { return done_; }

protected:
	int nextEntries(u_int32_t flags);
	int prevEntry();
	// find the last matching prefix for specified key
	int findLastPrefix(DbXmlDbt &key);

	Cursor cursor_;
	DbtOut key_;
	DbXmlDbt bulk_;
	DbXmlDbt tmpKey_;
	DbXmlDbt data_;
	bool done_;
	void *p_;
	
private:
	// no need for copy and assignment
	IndexCursor(const IndexCursor&);
	IndexCursor & operator = (const IndexCursor &);
};

class InequalityIndexCursor : public IndexCursor
{
public:
	InequalityIndexCursor(IndexDatabase &db, Transaction *txn,
			      DbWrapper::Operation operation, const Key *k1,
			      const Syntax *syntax);
	InequalityIndexCursor(IndexDatabase &db, Transaction *txn,
			      DbWrapper::Operation gto, const Key *k1,
			      DbWrapper::Operation lto, const Key *k2,
			      const Syntax *syntax);

	virtual int first();
	virtual int next();

private:
	const Syntax *syntax_; // The syntax for the cursor
	DbWrapper::Operation operation_; // Operation passed through the ctor
	// Ranges: The upper bound operation. GTX|GTE
	DbWrapper::Operation greaterThanOperation_;
	// Ranges: The lower bound operation. LTX|LTE
	DbWrapper::Operation lessThanOperation_; 
	DbtOut key2_; // Range: The end of the range.
};

class PrefixIndexCursor : public IndexCursor
{
public:
	PrefixIndexCursor(IndexDatabase &db, Transaction *txn,
			  const Key *k1);

	virtual int first();
	virtual int next();
};

class EqualsIndexCursor : public IndexCursor
{
public:
	EqualsIndexCursor(IndexDatabase &db, Transaction *txn,
			  const Key *k1);

	virtual int first();
	virtual int next();
};

// reverse prefix cursor
class ReversePrefixIndexCursor : public IndexCursor
{
public:
	ReversePrefixIndexCursor(IndexDatabase &db, Transaction *txn,
				 const Key *k1);

	virtual int first();
	virtual int next();
};

// reverse sort inequality cursor.
class ReverseInequalityIndexCursor : public IndexCursor
{
public:
	ReverseInequalityIndexCursor(
		IndexDatabase &db, Transaction *txn,
		DbWrapper::Operation operation, const Key *k1,
		const Syntax *syntax);
	ReverseInequalityIndexCursor(
		IndexDatabase &db, Transaction *txn,
		DbWrapper::Operation gto, const Key *k1,
		DbWrapper::Operation lto, const Key *k2,
		const Syntax *syntax);

	virtual int first();
	virtual int next();

private:
	int compare();
	// find the last matching value
	int findLastValue(DbXmlDbt &key, bool excl);
	
	const Syntax *syntax_; // The syntax for the cursor
	DbWrapper::Operation operation_; // Operation passed through the ctor
	// Ranges: The upper bound operation. GTX|GTE
	DbWrapper::Operation greaterThanOperation_;
	// Ranges: The lower bound operation. LTX|LTE
	DbWrapper::Operation lessThanOperation_; 
	DbtOut key2_; // Range: The end of the range.
};

class EqualsIndexIterator : public IndexEntryIterator
{
public:
	EqualsIndexIterator(ContainerBase *container, IndexDatabase &db, Transaction *txn,
			    const Key *k1, const LocationInfo *location);

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
	virtual void adoptMemory();
protected:
	Cursor cursor_;
	DbtOut key_;
	DbtOut data_;
	DbtOut tmp_;
	bool toDo_;
};

class EqualsDocumentIndexIterator : public EqualsIndexIterator
{
public:
	EqualsDocumentIndexIterator(ContainerBase *container, IndexDatabase &db, Transaction *txn,
		const Key *k1, const LocationInfo *location)
		: EqualsIndexIterator(container, db, txn, k1, location) {}

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

private:
	DocID docid_;
};

class SortingIndexIterator : public IndexEntryIterator
{
public:
	SortingIndexIterator(ContainerBase *container, IndexCursor *inputCursor,
		const LocationInfo *location);

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);
	
	virtual void adoptMemory();
protected:
	virtual void sortIndex(DynamicContext *context);

	AutoDelete<IndexCursor> inputCursor_;
	Manager &manager_;

	CacheDatabaseHandle sorted_;
	Cursor cursor_;

	DbtOut key_;
	DbtOut data_;
	DbtOut tmp_;
	bool toDo_;
};

class SortingDocumentIndexIterator : public SortingIndexIterator
{
public:
	SortingDocumentIndexIterator(ContainerBase *container, IndexCursor *inputCursor,
		const LocationInfo *location)
		: SortingIndexIterator(container, inputCursor, location) {}

protected:
	virtual void sortIndex(DynamicContext *context);
};

}

#endif

