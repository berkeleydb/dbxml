//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#ifndef __DOCUMENTDATABASE_HPP
#define	__DOCUMENTDATABASE_HPP

#include <string>
#include <sstream>
#include <db.h>
#include "SharedPtr.hpp"
#include "DbWrapper.hpp"
#include "Cursor.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Document;
class DocID;
class NameID;
class Name;
class DictionaryDatabase;
class UpdateContext;
class KeyStash;
class DocumentCursor;
class NsDocumentDatabase;

/**
 * Implements the DocID generator, content and
 * metadata store for a container, and handles
 * indexing of content and metadata.
 */
class DocumentDatabase
{
public:
	typedef SharedPtr<DocumentDatabase> Ptr;

	DocumentDatabase(DB_ENV *env, Transaction *txn,
			 const std::string &name,
			 const ContainerConfig &config,
			 XmlCompression *compression);
	// used for defaulted, read-only access to existing database
	DocumentDatabase(DB_ENV *env, const std::string &name,
			 XmlContainer::ContainerType type,
			 XmlCompression *compression);

	virtual ~DocumentDatabase();

	DbWrapper &getContentDatabase()
	{
		return content_;
	}

	virtual DbWrapper *getNodeDatabase() { return 0; }
	virtual u_int32_t getPageSize() const; // page size of content db
	virtual unsigned long getNumberOfPages() const;
	virtual int deleteID(OperationContext &context, const DocID& oldId);
	virtual int createDocumentCursor(Transaction *txn,
					 ScopedPtr<DocumentCursor> &cursor,
					 u_int32_t flags) const;
	virtual int getContent(OperationContext &context, Document *document,
			       u_int32_t flags) const;
	int getContent(Transaction *txn, DbtOut &key, DbtOut &data,
		       u_int32_t flags) const;
	virtual int updateContentAndIndex(Document &new_document,
					  UpdateContext &context,
					  KeyStash &stash, bool validate,
					  bool updateStats);
	/// Also deallocates the ID
	virtual int removeContentAndIndex(const Document &document,
					  UpdateContext &context,
					  KeyStash &stash, bool updateStats);
        int reindex(const Document &document, OperationContext &oc,
		bool updateStats, bool forDelete);

	virtual int reindex(const IndexSpecification &is, const Document &document,
			    UpdateContext &context, KeyStash &stash, bool updateStats);

	virtual int getAllMetaData(OperationContext &context,
				   DictionaryDatabase *dictionary,
				   Document *document, u_int32_t flags) const;
	virtual int getMetaData(OperationContext &context,
				DictionaryDatabase *dictionary,
				const Name &name, const DocID &did,
				XmlValue::Type &type, DbXmlDbt *metadata,
				u_int32_t flags) const;
	virtual int addMetaData(OperationContext &oc,
				DictionaryDatabase *dictionary,
				Document &document);
	virtual int updateMetaData(OperationContext &oc,
				   DictionaryDatabase *dictionary,
				   Document &document);
	virtual int removeMetaData(OperationContext &oc, const DocID &id,
				   std::vector<NameID> *toRemove);

	virtual void sync() {
		content_.sync();
		secondary_.sync();
	}

	virtual NsDocumentDatabase *getNsDocumentDatabase() {
		return NULL;
	}

	virtual int addContent(Document &document, UpdateContext &context);
	int addContent(Transaction *txn, DbtOut &key, DbXmlDbt *data, u_int32_t flags);
	
	virtual void addContentException(Document &document,
					 UpdateContext &context, bool isDbExc) {}

	virtual void run(Transaction *txn,
			 DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, content_);
		runner.run(txn, secondary_);
	}

	static int dump(DB_ENV *env, const std::string &name,
			XmlContainer::ContainerType type,
			std::ostream *out);
	static int load(DB_ENV *env, const std::string &name,
			XmlContainer::ContainerType type,
			std::istream *in, unsigned long *lineno);
	static int verify(DB_ENV *env, const std::string &name,
			  XmlContainer::ContainerType type,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static void upgrade(const std::string &name,
			    const std::string &tname, Manager &mgr,
			    int old_version, int current_version);

	DB_ENV *getDB_ENV() { return environment_; }

	const std::string &getContainerName() const {
		return name_;
	}
	XmlContainer::ContainerType getContainerType() const {
		return type_;
	}

private:
	void open(Transaction *txn, const ContainerConfig &config);

protected:
	// no need for copy and assignment
	DocumentDatabase(const DocumentDatabase&);
	DocumentDatabase &operator=(const DocumentDatabase&);
	
	DB_ENV *environment_;
	std::string name_;
	XmlContainer::ContainerType type_;
	/**
	   TBD: factor content_ out into a document level
	   storage specific class, DsDocumentDatabase.cpp
	   - jpcs
	 */
	mutable DbWrapper content_;
	SecondaryDatabase secondary_;

	XmlCompression *compressor_;
};

}

#endif
