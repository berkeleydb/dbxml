//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#ifndef __NSDOCUMENTDATABASE_HPP
#define	__NSDOCUMENTDATABASE_HPP

#include "../DocumentDatabase.hpp"
#include "NsNode.hpp"

namespace DbXml
{
class Transaction;

/**
 * Implements the ID generator, content and
 * metadata store for a container
 */
class NsDocumentDatabase : public DocumentDatabase
{
public:
	NsDocumentDatabase(DB_ENV *env, Transaction *txn,
			   const std::string &name, const ContainerConfig &config,
			   XmlCompression *compression);
	virtual ~NsDocumentDatabase();

	/** @name Interface to Databases */
	virtual DbWrapper *getNodeDatabase() {
		return nodeStorage_;
	}

	virtual u_int32_t getPageSize() const {
		return nodeStorage_->getPageSize();
	}
	virtual unsigned long getNumberOfPages() const {
		return nodeStorage_->getNumberOfPages();
	}

	virtual int getContent(OperationContext &context, Document *document,
			       u_int32_t flags) const;
	virtual int updateContentAndIndex(Document &new_document,
					  UpdateContext &context,
					  KeyStash &stash, bool validate,
					  bool updateStats);
	/// Also deallocates the ID
	virtual int removeContentAndIndex(const Document &document,
					  UpdateContext &context,
					  KeyStash &stash, bool updateStats);

	virtual void sync() {
		nodeStorage_->sync();
		secondary_.sync();
	}

	virtual NsDocumentDatabase *getNsDocumentDatabase() {
		return this;
	}

	virtual int addContent(Document &document, UpdateContext &context) {
		return 0;
	}

	virtual void addContentException(Document &document,
					 UpdateContext &context, bool isDbExc);
	
	virtual void run(Transaction *txn,
			 DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, secondary_);
		runner.run(txn, *nodeStorage_);
	}
	static int dump(DB_ENV *env, const std::string &name,
			std::ostream *out);
	static int load(DB_ENV *env, const std::string &name,
			std::istream *in, unsigned long *lineno);
	static int verify(DB_ENV *env, const std::string &name,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static void upgrade(const std::string &name,
			    const std::string &tname, Manager &mgr,
			    int old_version, int current_version);
private:
	int deleteAllNodes(OperationContext &context, const DocID &did);

	// Private methods for upgrade from 2.2 to 2.3.  This is non-trivial
	NsDocumentDatabase(DB_ENV *env, const std::string &name,
			   DbWrapper *db);
	static void upgrade23(const std::string &name,
			      const std::string &tname, Manager &mgr);
	
	mutable DbWrapper *nodeStorage_;
	bool dbIsOwned_;
};

}

#endif
