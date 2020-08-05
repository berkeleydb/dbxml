//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include <dbxml/XmlException.hpp>
#include "../Container.hpp"
#include "NsDocumentDatabase.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsWriter.hpp"
#include "NsEventReader.hpp"
#include "NsEventWriter.hpp"
#include "EventReaderToWriter.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "NsUpgrade.hpp"
#include "../ScopedDbt.hpp"
#include "../OperationContext.hpp"
#include "../Document.hpp"
#include "../Cursor.hpp"
#include "../UpdateContext.hpp"
#include "../KeyStash.hpp"
#include "../Manager.hpp"
#include "../Globals.hpp"
#include "../db_utils.h"
#include "../ContainerConfig.hpp"

using namespace DbXml;
using namespace std;

static const char *nodestorage_name = "nodestorage";

NsDocumentDatabase::NsDocumentDatabase(
	DB_ENV*env, Transaction *txn, const std::string &name,
	const ContainerConfig &config, XmlCompression *compression)
	: DocumentDatabase(env, txn, name, config, compression),
	  dbIsOwned_(true)
{
	int err = 0;
	try {
		nodeStorage_ = new DbWrapper(
			env, name, "node_", nodestorage_name,
			DEFAULT_CONFIG);
		if (!nodeStorage_)
			throw XmlException(XmlException::NO_MEMORY_ERROR,
					   "Error opening container");
		// Open the Db object
		DB *dbp = nodeStorage_->getDb();
		dbp->set_bt_compare(dbp, lexicographical_bt_compare);
		err = nodeStorage_->open(txn, DB_BTREE, config);
	}
	catch (XmlException &xe) {
		if (txn)
			txn->abort();
		throw;
	}
	if (err != 0) {
		if (txn)
			txn->abort();
		string msg = name;
		if (err == EEXIST) {
			msg += ": container exists";
			throw XmlException(
				XmlException::CONTAINER_EXISTS, msg);
		} else if (err == ENOENT) {
			msg += ": container file not found, or not a container";
			throw XmlException(XmlException::CONTAINER_NOT_FOUND,
					   msg);
		}
		throw XmlException(err);
	}
}

// this is a private method, only used for read-only access for upgrade
NsDocumentDatabase::NsDocumentDatabase(
	DB_ENV *env, const std::string &name, DbWrapper *db)
	: DocumentDatabase(env, name, XmlContainer::NodeContainer, 0),
	  nodeStorage_(db), // use DbWrapper that's passed in
	  dbIsOwned_(false)
{
	// nothing to do
}

NsDocumentDatabase::~NsDocumentDatabase()
{
	if (dbIsOwned_)
		delete nodeStorage_;
}

int NsDocumentDatabase::getContent(OperationContext &context,
				   Document *document, u_int32_t flags) const
{
	// Make output writer
	BufferNsStream output;
	NsWriter writer(&output);

	// Make an event reader and serialize from it
	NsEventReader reader(context.txn(),
			     const_cast<NsDocumentDatabase*>(this)->
			         getNodeDatabase(),
			     document->getDictionaryDB(),
			     document->getID(),
			     document->getContainerID(), flags, NS_EVENT_BULK_BUFSIZE);

	writer.writeFromReader(reader);

	// donate memory from buffer to avoid copy into DbtOut
	DbtOut *data = new DbtOut();
	data->setNoCopy(output.buffer.donateBuffer(),
			output.buffer.getOccupancy());
	
	// pass setOnly arg -- this tells Document to not change
	// state, other than to set the contentDbt_
	document->setContentAsDbt(&data, true); // Note: consumes data

	return 0;
}

int NsDocumentDatabase::deleteAllNodes(OperationContext &context,
	const DocID &did)
{
	int err = NsFormat::deleteAllNodes(*nodeStorage_, context, did);
	if (err)
		throw XmlException(err);
	return 0;
}

void NsDocumentDatabase::addContentException(Document &document,
					      UpdateContext &context,
					      bool isDbExc)
{
	if (!isDbExc) {
		deleteAllNodes(context.getOperationContext(), document.getID());
	}
}


int NsDocumentDatabase::updateContentAndIndex(
	Document &new_document, UpdateContext &context,
	KeyStash &stash, bool validate, bool updateStats)
{
	OperationContext &oc = context.getOperationContext();
	DocID id = new_document.getID();
	Indexer &indexer = context.getIndexer();
	IndexSpecification &index = context.getIndexSpecification();
	int err = 0;
	bool resetId = false;

	// Add DB_RMW if transacted to reduce deadlocks
	u_int32_t flags =  ((oc.txn() && !content_.isCDBEnv()) ? DB_RMW : 0);
	// Check to see if the old document exists, first
	// If ID is non-zero, let's trust it.  If not, get by name.
	// Retrieve the old document
	XmlDocument old_document;

	if (id == 0) {
		// will throw if name is bad or doc doesn't exist
		err = indexer.getContainer()->getDocument(
			oc, new_document.getName(), old_document, flags);
		if (err == 0) {
			id = ((Document&)old_document).getID();
			new_document.getIDToSet() = id;
			resetId = true;
		}

	} else {
		err = indexer.getContainer()->getDocument(
			oc, id, old_document, flags);
	}

	if (err != 0) return err;

	// Set the modified flags of the old document to the same as the
	// new document, so that when we index, we only generate keys for
	// things that have actually changed.
	MetaData::const_iterator end = new_document.metaDataEnd();
	for(MetaData::const_iterator i = new_document.metaDataBegin(); i != end; ++i) {
		if((*i)->isModified()) {
			const MetaDatum *md = ((Document&)old_document)
				.getMetaDataPtr((*i)->getName());
			if(md != 0) const_cast<MetaDatum*>(md)->setModified(true);
		}
	}

	// Remove the index keys for the old document
	IndexSpecification delete_index(index);
	delete_index.set(Index::INDEXER_DELETE);
	indexer.indexMetaData(delete_index, old_document, stash, true);
	if(new_document.isContentModified()) {
		ScopedPtr<NsPushEventSource>
			source(((Document&)old_document).
			       getContentAsEventSource(
				       oc.txn(),
				       /*needsValidation*/false,
				       indexer.getContainer()->nodesIndexed()));
		if (source.get()) {
			indexer.initIndexContent(delete_index, id,
						 source.get(), stash, updateStats,
						 false, /*isDelete*/true);
			source.get()->start();
		}
	}

	//
	// This routine has arranged this way because the old document
	// content has to be removed before we add the new content.
	// Otherwise, we have no way of removing the
	// old content... - jpcs
	//
	
	// Remove the old content, but only if it's being replaced
	if (new_document.isContentModified()) {
		err = deleteAllNodes(oc, id);
		if(err != 0) {
			return err;
		}
	}
	
	// Index the new document, and add the content, metadata first
	indexer.indexMetaData(index, new_document, stash, true);
	
	// If the there are no content indexes, the parse still
	// needs to happen for unparsed content (DBT or INPUTSTREAM)
	if (new_document.isContentModified() &&
	    ((new_document.getDefinitiveContent() == Document::DBT) ||
	     (new_document.getDefinitiveContent() == Document::INPUTSTREAM))) {
		try {
			// deleted by Document::getContentAsEventSource()...
			ScopedPtr<NsPushEventSource>
				source(new_document.
				       getContentAsEventSource(
					       oc.txn(),
					       /*needsValidation*/validate,
					       indexer.getContainer()->
					       nodesIndexed()));
			if (source.get()) {
				indexer.initIndexContent(
					index, id,
					source.get(), stash, updateStats,
					false, /*isDelete*/false);
				source.get()->start();
			}
		}
		catch(XmlException &xe) {
			// Don't clean up a database exception
			if(xe.getExceptionCode() != XmlException::DATABASE_ERROR) {
				// Remove the content
				deleteAllNodes(oc, id);
			}
			throw;
		}
		catch(...) {
			// Remove the content
			deleteAllNodes(oc, id);
			throw;
		}
	}

	if(err == 0) new_document.setContentModified(false);

	if (resetId)
		new_document.getIDToSet() = 0;

	return err;
}

int NsDocumentDatabase::removeContentAndIndex(
	const Document &document, UpdateContext &context,
	KeyStash &stash, bool updateStats)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	IndexSpecification &index = context.getIndexSpecification();
	index.set(Index::INDEXER_DELETE);

	indexer.indexMetaData(index, document, stash, /*checkModified*/false);

	// put this in its own block to destroy the event source in a
	// timely manner.  It holds a cursor to the document.
	ScopedPtr<NsPushEventSource>
		source(document.getContentAsEventSource(
			       oc.txn(), /*needsValidation*/false,
			       indexer.getContainer()->nodesIndexed()));
	if (source.get()) {
		indexer.initIndexContent(index, document.getID(),
			source.get(), stash, updateStats,
			false, /*isDelete*/true);
		source.get()->start();
	}

	// Remove the ID -- cannot "remove" an ID.  They are not
	// reusable, but make the call, just in case that ever
	// changes.
	int err = deleteID(oc, document.getID());
	if(err != 0) return err;

	// Remove the content
	err = deleteAllNodes(oc, document.getID());
	
	return err;
}

int NsDocumentDatabase::dump(DB_ENV *env, const std::string &name,
			     std::ostream *out)
{
	int err = DocumentDatabase::dump(env, name,
					 XmlContainer::NodeContainer, out);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_",
				      nodestorage_name, DEFAULT_CONFIG);
		err = Container::writeHeader(nodeStorage.getDatabaseName(),
					     out);
		if(err == 0) err = nodeStorage.dump(out);
	}

	return err;
}

int NsDocumentDatabase::load(DB_ENV *env, const std::string &name,
			     std::istream *in, unsigned long *lineno)
{
	int err = DocumentDatabase::load(env, name,
					 XmlContainer::NodeContainer,
					 in, lineno);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_",
				      nodestorage_name, DEFAULT_CONFIG);
		DB *dbp = nodeStorage.getDb();
		dbp->set_bt_compare(dbp, lexicographical_bt_compare);

		err = Container::verifyHeader(
			nodeStorage.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "NsDocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		}
		else {
			err = nodeStorage.load(in, lineno);
		}
	}

	return err;
}

int NsDocumentDatabase::verify(DB_ENV *env, const std::string &name,
			       std::ostream *out, u_int32_t flags)
{
	int terr = 0;
	int err = DocumentDatabase::verify(env, name,
					   XmlContainer::NodeContainer,
					   out, flags);

	if(err == 0) {
		DbWrapper nodeStorage(env, name, "node_", nodestorage_name, DEFAULT_CONFIG);

		if(flags & DB_SALVAGE)
			terr = Container::writeHeader(
				nodeStorage.getDatabaseName(), out);
		err = nodeStorage.verify(out, flags);
		if (terr && !err)
			err = terr;
	}

	return err;
}

// upgrade from 2.0 (version 3) or 2.1 (version 4) to 2.2 (version 5)
// Modify all keys for node storage database
// content key: docId
// 1.  open old DB and create a new database
// 2.  for each record:
//    o unmarshal old format; marshal in new, and put into new DB
// 3.  remove old DB
// 4.  rename new DB to correct name
// static
void NsDocumentDatabase::upgrade(const std::string &name,
				 const std::string &tname, Manager &mgr,
				 int old_version, int current_version)
{
	DBXML_ASSERT(old_version < current_version);
	int err = 0;
	if (old_version < VERSION_22) {
		// do 2.2 upgrade
		DbWrapper oldDb(mgr.getDB_ENV(), name, "node_",
				nodestorage_name, DEFAULT_CONFIG);
		oldDb.getDb()->set_dup_compare(oldDb.getDb(),nsCompareNodes);
		oldDb.getDb()->set_flags(oldDb.getDb(),DB_DUP|DB_DUPSORT);
		err = oldDb.open(0, DB_BTREE, DEFAULT_CONFIG);
		ContainerConfig config(DEFAULT_CONFIG);
		config.setPageSize(oldDb.getPageSize());
		DbWrapper newDb(mgr.getDB_ENV(), name,
				"newns_", nodestorage_name,
				config);
		newDb.getDb()->set_dup_compare(newDb.getDb(), nsCompareNodes);
		newDb.getDb()->set_flags(newDb.getDb(), DB_DUP|DB_DUPSORT);
		err = newDb.open(0, DB_BTREE,
				 CREATE_CONFIG);
		DbXmlDbt key;
		DbXmlDbt data;
		Cursor curs(oldDb, (Transaction*)0, DbXml::CURSOR_READ);
		int ret = 0;
		while ((err == 0) &&
		       (ret = curs.get(key, data, DB_NEXT)) == 0) {
			// decode/encode key, rewrite
			u_int32_t *p = (u_int32_t *)key.data;
			u_int32_t id1;
			id1 = *p;
			if (Manager::isBigendian())
				M_32_SWAP(id1);
			DocID id(id1);
			DbtOut newKey;
			id.setDbtFromThis(newKey);
			err = newDb.put(0, &newKey, &data, 0);
		}
		curs.close();
		newDb.close(0);
		oldDb.close(0);
		if (err == 0) {
			std::string dbname("node_");
			std::string tname("newns_");
			dbname += nodestorage_name;
			tname += nodestorage_name;
			// remove old db, rename new one
			err = mgr.getDB_ENV()->dbremove(mgr.getDB_ENV(),
							0, name.c_str(), dbname.c_str(), 0);
			if (err == 0)
				err = mgr.getDB_ENV()->dbrename(
					mgr.getDB_ENV(),
					0, name.c_str(), tname.c_str(),
					dbname.c_str(), 0);
		}
	}
	if (err == 0) {
		if (old_version < VERSION_23) {
			upgrade23(name, tname, mgr);
		} else {
			// TBD: at some point (next real upgrade)
			// it may be necessary to copy
			// the node storage database
		}
	}
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error upgrading NsDocument DB");
}

//
// 2.2->2.3 upgrade
// This looks a lot like the function above, but with a more
// complex interior:
// 1.  open old db (using old dup compare, if different)
// 2.  create new db (using new dup compare, if different)
// 3.  move all old docs into new database (this is the tricky part)
// 4.  remove old database
// 5.  rename new db to old name
// 6.  In caller: run compaction to squeeze out unused space, after reindexing
static void copyAllDocs(NsDocumentDatabase &oldDb,
			NsDocumentDatabase  &newDb, Manager &mgr);

void NsDocumentDatabase::upgrade23(const std::string &name,
				   const std::string &tname,
				   Manager &mgr)
{
	DbWrapper oldDb(mgr.getDB_ENV(), name, "node_",
			      nodestorage_name, DEFAULT_CONFIG);
	oldDb.getDb()->set_dup_compare(oldDb.getDb(), nsCompareNodes);
	oldDb.getDb()->set_flags(oldDb.getDb(), DB_DUP|DB_DUPSORT);
	int err = oldDb.open(0, DB_BTREE, DEFAULT_CONFIG);

	NsDocumentDatabase oldNs(mgr.getDB_ENV(), name, &oldDb);

	ContainerConfig config(DEFAULT_CONFIG);
	config.setPageSize(oldDb.getPageSize());
	DbWrapper newDb(mgr.getDB_ENV(), tname,
			"node_", nodestorage_name,
			config);

	newDb.getDb()->set_bt_compare(newDb.getDb(), lexicographical_bt_compare);
	err = newDb.open(0, DB_BTREE, CREATE_CONFIG);
	if (err != 0)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"upgrade23: unexpected error upgrading NsDocument DB");

	NsDocumentDatabase newNs(mgr.getDB_ENV(), tname, &newDb);
	//
	// copy all old docs to new db
	//
	copyAllDocs(oldNs, newNs, mgr);
	
	newDb.close(0);
	oldDb.close(0);
}

static
void copyAllDocs(NsDocumentDatabase &oldDb,
		 NsDocumentDatabase &newDb, Manager &mgr)
{
	ScopedPtr<DocumentCursor> cursor;
	oldDb.createDocumentCursor(0, cursor, 0);
	DocID did;
	int count = 0;
	DB_ENV *env = mgr.getDB_ENV();
	
	// 2.3 node storage databases require a dictionary, which
	// *must* be already upgraded.  Default everything, no txn.
	DictionaryDatabase ddb(mgr.getDB_ENV(), 0, newDb.getContainerName(),
			       DEFAULT_CONFIG);

	int err = cursor->first(did);
	DBXML_ASSERT(err == 0 || did == 0);
	while (did != 0) {
		NsUpgradeReader reader(*oldDb.getNodeDatabase(), did);
		NsEventWriter writer(newDb.getNodeDatabase(), &ddb, did);
		EventReaderToWriter r2w(reader, writer, false, false);
		r2w.start();
		cursor->next(did);
		if (++count % 1000 == 0) {
			char buf[100];
			sprintf(buf, "Upgrade progress: copied %d documents\n", count);
			Log::log(env, Log::C_CONTAINER, Log::L_INFO, buf);
		}
	}
	char buf[100];
	sprintf(buf, "Upgrade: copied %d documents\n", count);
	Log::log(env, Log::C_CONTAINER, Log::L_INFO, buf);
}
