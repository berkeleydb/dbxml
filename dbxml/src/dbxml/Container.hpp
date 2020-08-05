//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#ifndef __CONTAINER_HPP
#define	__CONTAINER_HPP

#include "ContainerBase.hpp"
#include "ConfigurationDatabase.hpp"
#include "DictionaryDatabase.hpp"
#include "DocumentDatabase.hpp"
#include "SyntaxDatabase.hpp"
#include <string>
#include <sstream>
#include <dbxml/XmlManager.hpp>
#include "ScopedPtr.hpp"
#include "DocID.hpp"
#include "IndexSpecification.hpp"
#include "DbWrapper.hpp"
#include "ReferenceCounted.hpp"
#include "Syntax.hpp"
#include "Log.hpp"
#include "KeyStatistics.hpp"
#include "StructuralStatsDatabase.hpp"
#include <dbxml/XmlTransaction.hpp>
#include <dbxml/XmlCompression.hpp>
#include "ContainerConfig.hpp"

namespace DbXml
{

class DbtIn;
class UpdateContext;
class Results;
class Document;
class Transaction;
class Statistics;
class IndexDbNotify;
class NsPushEventSource;
class NsEventWriter;
class Functor;
class Manager;
class NodeIterator;
class NamedNodeIterator;
	
// container format versions
// NOTE: version 6 is only incompatible with
// version 5 in node storage format (for now; index
// layout may change). Rather than special case node
// vs doc, neither will open
enum ContainerVersion {
	VERSION_121 = 2,
	VERSION_20 = 3,
	VERSION_21 = 4,
	VERSION_22 = 5,
	VERSION_23 = 6,
	CURRENT_VERSION = VERSION_23
};
	
/**
 * Container encapsulates all the Berkeley DB databases that constitute
 * an XmlContainer. Container provides methods that implement the
 * functionality provided through the XmlContainer interface.
 */
class Container : public ContainerBase
{
public:
	static const unsigned int version;   ///< format version of container

	/** @name Construction */
	Container(Manager &mgr, const std::string &name,
		  Transaction *txn, const ContainerConfig &config,
		  bool doVersionCheck);
	~Container();

	// ReferenceCountedProtected -- control destruction of
	// the object
	virtual void release();
	
	/** @name Simple Accessors */
	const std::string &getName() const { return name_; }

	XmlContainer::ContainerType getContainerType() const {
 		return containerConfig_.getContainerType();
  	}
  	void setContainerType(XmlContainer::ContainerType type) {
		containerConfig_.setContainerType(type);
  	}
  	bool isNodeContainer() const {
 		return (containerConfig_.getContainerType() == XmlContainer::NodeContainer);
  	}
  	bool isWholedocContainer() const {
 		return (containerConfig_.getContainerType() == XmlContainer::WholedocContainer);
  	}
	void setIndexNodes(Transaction *txn, bool indexNodes);

	void setContainerID(int id) { id_ = id; }

	
	bool getDoValidation() const {
		return containerConfig_.getAllowValidation();
  	}
  
  	u_int32_t getPageSize() const {
 		return containerConfig_.getPageSize();
  	}

 	const ContainerConfig &getContainerConfig() const {
 		return containerConfig_;
  	}  

	unsigned int getIndexVersion() const
	{
		return indexVersion_;
	}

	void setUseCDB() { usingCDB_ = true; }

	// ContainerBase implementation

	// GMF TBD: see if this method can be avoided
	Container *getContainer() { return this; }

	bool nodesIndexed() const {
		return indexNodes_;
	}
	int getContainerID() const { return id_; }

	void getIndexSpecification(Transaction *txn,
				   IndexSpecification &index);

	virtual void checkReadOnly() const; // throws if read-only

	virtual Cost getDocumentSSCost(OperationContext &oc, StructuralStatsCache &cache);
	virtual Cost getIndexCost(OperationContext &oc,
		DbWrapper::Operation op1, const Key &key1,
		DbWrapper::Operation op2 = DbWrapper::NONE,
		const Key &key2 = Key(0));
	virtual Cost getAttributeSSCost(OperationContext &oc, StructuralStatsCache &cache,
		const char *childUriName);
	virtual Cost getElementSSCost(OperationContext &oc, StructuralStatsCache &cache,
		const char *childUriName);
	virtual StructuralStats getStructuralStats(OperationContext &oc,
		const NameID &id1, const NameID &id2) const;

	double getPercentage(OperationContext &oc,
		     DbWrapper::Operation op1, const Key &key1,
		     DbWrapper::Operation op2 = DbWrapper::NONE,
		     const Key &key2 = Key(0));

	NodeIterator *createDocumentIterator(
		DynamicContext *context, const LocationInfo *location,
		const char *docName = 0, size_t docNameLen = 0) const;
	NodeIterator *createIndexIterator(
		Syntax::Type type, DynamicContext *context,
		bool documentIndex, const LocationInfo *l,
		DbWrapper::Operation op1, const Key &key1,
		DbWrapper::Operation op2 = DbWrapper::NONE,
		const Key &key2 = Key(0)) const;
	NamedNodeIterator *createAttributeIterator(
		DynamicContext *context, const LocationInfo *location,
		const NameID &nsUriID) const;
	NamedNodeIterator *createElementIterator(
		DynamicContext *context, const LocationInfo *location) const;

	DictionaryDatabase *getDictionaryDatabase() const {
		return dictionary_.get();
	}
	DbWrapper *getDbWrapper(DynamicContext *context = 0) {
		DbWrapper *ret = getDocumentDB()->getNodeDatabase();
		if (!ret)
			ret = ContainerBase::getDbWrapper(context);
		return ret;
	}
	
	// end ContainerBase

	XmlManager &getXmlManager() {
		return xmlMgr_;
	}
	
	void sync(); // flush database cache to disk
	
	/** @name Container Operations */
	int openInternal(Transaction *txn, const ContainerConfig &config,
			 bool doVersionCheck);
	int getDocument(OperationContext &context, const std::string &name,
			XmlDocument &document, u_int32_t flags) const;
	int getDocument(OperationContext &context, const DocID &id,
			XmlDocument &document, u_int32_t flags) const;
	int addDocument(Transaction *txn, Document &document,
			UpdateContext &context, u_int32_t flags);
	int addDocumentInternal(Transaction *txn, Document &document,
				UpdateContext &context, u_int32_t flags);
	int addDocumentAsEventReader(Transaction *txn, Document &document,
				     UpdateContext &context, u_int32_t flags);
	NsEventWriter &addDocumentAsEventWriter(
		Transaction *txn, Document &document,
		UpdateContext &context, u_int32_t flags);
	NsPushEventSource *prepareAddDocument(
		Transaction *txn, Document &document,
		UpdateContext &context, u_int32_t flags, bool createEvents = true);
	int indexAddDocument(NsPushEventSource *events, Document &document,
			     UpdateContext &context);
	int completeAddDocument(Document &document, UpdateContext &context);

	int deleteDocument(Transaction *txn, const std::string &name,
				   UpdateContext &context);
	int deleteDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int deleteDocumentInternal(Transaction *txn, Document &document,
				   UpdateContext &context);
	int updateDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int updateDocumentInternal(Transaction *txn, Document &document,
				   UpdateContext &context, bool validate);
	int setIndexSpecification(Transaction *txn,
				  const XmlIndexSpecification &index,
				  UpdateContext &context);
	int setIndexSpecificationInternal(Transaction *txn,
					  const IndexSpecification &index,
					  UpdateContext &context);
	Results *lookupIndex(Transaction *txn,
			     XmlQueryContext &context,
			     const IndexLookup &il,
			     u_int32_t flags);

	Statistics *lookupStatistics(Transaction *txn,
				     Name *child,
				     Name *parent,
				     const std::string &indexStr,
				     const XmlValue &value);

	/** @name KeyStatistics utility operations */
	KeyStatistics getKeyStatistics(Transaction *txn, const Index &index,
				       const char *child, const char *parent,
				       const XmlValue &value);

	KeyStatistics getKeyStatistics(Transaction *txn, const Key &key);
	
	/** @name Configuration Database Accessors */
	ConfigurationDatabase *getConfigurationDB() {
		return configuration_.get();
	}
	const ConfigurationDatabase *getConfigurationDB() const {
		return configuration_.get();
	}

	/** @name Document Database Accessors */
	DocumentDatabase *getDocumentDB() { return documentDb_.get(); }
	const DocumentDatabase *getDocumentDB() const {
		return documentDb_.get();
	}

	/** @name Index Databases Accessors */
	SyntaxDatabase *getIndexDB(Syntax::Type type, Transaction *txn = 0,
				   bool toWrite = false);
	const SyntaxDatabase *getIndexDB(Syntax::Type type) const;

	/** @name Structural Statistics Database Accessors */
	StructuralStatsDatabase *getStructuralStatsDB() { return stats_.get(); }
	void displayStructuralStats(OperationContext &context, std::ostream &out)
	{
		if(stats_) stats_->display(context, out, dictionary_.get());
	}

	/** @name Administration methods */
	
	bool addAlias(const std::string &alias);
	bool removeAlias(const std::string &alias);

	// close allows explicit shut down of databases, and not count
	// on lazy references or garbage collection.  It's only
	// supported for non-C++ languages.
	void close();
	
	// @{
	static void dump(Manager &mgr, const std::string &name,
			 std::ostream *out);
	static void load(Manager &mgr, const std::string &name,
			 std::istream *in, unsigned long *lineno,
			 UpdateContext &context);
	static void verify(Manager &mgr, const std::string &name,
			   std::ostream *out, u_int32_t flags);
	
	static int writeHeader(const std::string &name, std::ostream *out);
	static int verifyHeader(const std::string &name, std::istream *in);

	// upgrade containers
	static void upgradeContainer(const std::string &name,
				     Manager &mgr,
				     UpdateContext &context);
	// reindex container
	static void reindexContainer(Transaction *txn,
				     Manager &mgr,
				     const std::string &name,
				     UpdateContext &context,
				     const ContainerConfig &flags);
	// truncate container
	static void truncateContainer(Transaction *txn,
				      Manager &mgr,
				      const std::string &name,
				      UpdateContext &context);

	// compact container
	static void compactContainer(Transaction *txn,
				     Manager &mgr,
				     const std::string &name,
				     UpdateContext &context);
	
	void upgrade(unsigned int saved_version, unsigned int current_version,
		     UpdateContext &uc);
	//@}
	
	void dumpStructuralStatistics(XmlTransaction &txn, std::ostream &out);
		
	static unsigned int checkContainer(const std::string &name, DB_ENV *dbenv);

	// not static -- run the method on all dbs in the container.
	// obviously, methods must be ok for open databases.
	void runOnAllDatabases(Transaction *txn,
			       DbWrapper::DbFunctionRunner &runner,
			       bool skipCoreDbs);
	// reindex based on auto-indexes
	int doAutoIndex(IndexSpecification *autoIs,
			IndexSpecification &is, OperationContext &oc);
	std::string disk(Transaction *txn);

protected:
	// VC6 needs this to be protected, rather than private
	// re-index entire container, based on IS.  Used for
	// addition of indexes, including container loading
	int reindex(Transaction *txn, const IndexSpecification &is,
		    bool updateStats);
private:
	// no need for copy and assignment
	Container(const Container&);
	void operator=(const Container &);
	
	int ensureDocName(OperationContext &context, Document &document,
			  u_int32_t flags) const;
	int getDocumentID(OperationContext &context,
			  const std::string &name, DocID &id) const;
	// remove the indexes in the IS
	int removeIndexes(Transaction *txn, const IndexSpecification &is,
			  UpdateContext &context);
	int removeIndexes(OperationContext &oc, const IndexVector &iv,
			  UpdateContext &context, bool isDefault);
	
	// removes, then re-creates indexes; used for upgrade.
	// Accepts flags DBXML_STATISTICS and DBXML_NO_STATISTICS
	void reloadIndexes(Transaction *txn, UpdateContext &uc, const ContainerConfig &flags);
	void closeIndexes(int idx = -1);
	void openIndexDbs(Transaction *txn, const ContainerConfig &origFlags);
	void logDocumentOperation(const Document &doc, const char *msg);

	Transaction *autoTransact(Transaction *txn,
				  TransactionGuard &txnGuard,
				  bool checkReadonly = true) const;
private:
	XmlManager xmlMgr_;

	bool indexNodes_;
	unsigned int indexVersion_;
	bool doValidation_;
	int id_;
	bool usingTxns_;
	bool usingCDB_; // ignored if not using DB 4.5 or greater
	bool readOnly_;

	IndexDbNotify *indexDbNotify_;

	ConfigurationDatabase::Ptr configuration_;
	DictionaryDatabase::Ptr dictionary_;
	DocumentDatabase::Ptr documentDb_;
	SyntaxDatabase::Vector indexes_;
	StructuralStatsDatabase::Ptr stats_;

	ContainerConfig containerConfig_;
	XmlCompression *compression_;

	friend class XmlManager;
	friend class IndexDbNotify;
};

}

#endif

