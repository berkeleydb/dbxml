//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#include "DbXmlInternal.hpp"
#include <sstream>
#include <dbxml/XmlException.hpp>
#include "Container.hpp"
#include "DocumentDatabase.hpp"
#include "Manager.hpp"
#include "ScopedDbt.hpp"
#include "OperationContext.hpp"
#include "Document.hpp"
#include "Cursor.hpp"
#include "UpdateContext.hpp"
#include "NameID.hpp"
#include "db_utils.h"
#include "nodeStore/NsEvent.hpp"
#include "nodeStore/NsDocument.hpp"

using namespace DbXml;
using namespace std;
static const char *document_name = "document";

DocumentDatabase::DocumentDatabase(DB_ENV *env, Transaction *txn,
				   const std::string &name,
				   const ContainerConfig &config,
				   XmlCompression *compression)
	: environment_(env),
	  name_(name),
	  type_(config.getContainerType()),
	  content_(env, name, "content_", document_name,
		   DEFAULT_CONFIG),
	  secondary_(env, name, document_name, DEFAULT_CONFIG),
	  compressor_(compression)
{
	open(txn, config);
}

DocumentDatabase::DocumentDatabase(DB_ENV *env, const std::string &name,
				   XmlContainer::ContainerType type,
				   XmlCompression *compression)
	: environment_(env),
	  name_(name),
	  type_(type),
	  content_(env, name, "content_", document_name, DEFAULT_CONFIG),
	  secondary_(env, name, document_name, DEFAULT_CONFIG),
	  compressor_(compression)
{
	open(0, DEFAULT_CONFIG);
}

void
DocumentDatabase::open(Transaction *txn, const ContainerConfig &config)
{
	int err = 0;
	try {
		// Open the Db objects
		if (type_ == XmlContainer::WholedocContainer) {
			err = content_.open(txn, DB_BTREE, config);
		}
		if (err == 0)
			err = secondary_.open(txn, /*duplicates*/false,
					      config);
	} catch (XmlException &xe) {
		if (txn)
			txn->abort();
		throw;
	}
	if (err != 0) {
		if (txn)
			txn->abort();
		string msg = name_;
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

DocumentDatabase::~DocumentDatabase()
{
	// nothing to do
	
}

int DocumentDatabase::getContent(OperationContext &context,
				 Document *document, u_int32_t flags) const
{
	DbtOut *data = new DbtOut();
	int err = 0;
	try {
		document->getID().setDbtFromThis(context.key());
		// NOTE: db_.get() does not throw on DB_NOTFOUND, but
		// try/catch anyway...
		err = getContent(context.txn(), context.key(), *data, flags);
	} catch (...) {
		delete data;
		throw; // re-throw
	}
 
	if(err == 0 && (data->size != 0)) 
		document->setContentAsDbt(&data); // Note: consumes data
	else 
		delete data;
	if (err == DB_NOTFOUND)
		err = 0; // allow no-content documents
	return err;
}

int DocumentDatabase::getContent(Transaction *txn, DbtOut &key, DbtOut &data,
			       u_int32_t flags) const
{
	int err = 0;
	// NOTE: db_.get() does not throw on DB_NOTFOUND, but
	// try/catch anyway...
	err = content_.get(txn, &key, &data, flags);
 
	if(err == 0 && (data.size != 0)) {
		if (compressor_) {
			XmlData source(data.data, data.size);
			XmlData dest;
			XmlTransaction xtxn(txn);
			if(!compressor_->decompress(xtxn, source, dest)){
				throw XmlException(XmlException::INTERNAL_ERROR,
						   "Error while tring to decompress your XML document.");
			}
			/* Replace the buffer in data with the one in dest.  Because
			of this, DbtOut and Buffer must both use malloc and free*/
			if (data.data != dest.get_data()) 
				data.setNoCopy((*dest.getBuffer())->donateBuffer(), dest.get_size());
		}
	} 
	return err;
}

u_int32_t DocumentDatabase::getPageSize() const
{
	return content_.getPageSize();
}

unsigned long DocumentDatabase::getNumberOfPages() const
{
	return content_.getNumberOfPages();
}

// Remove the ID -- cannot "remove" an ID.  They are not
// reusable, but implement the call, just in case that ever
// changes.
int DocumentDatabase::deleteID(OperationContext &context, const DocID& oldId)
{
	return 0; // this is a no-op; IDs are never reused
}


int DocumentDatabase::addContent(Document &document, UpdateContext &context)
{
	OperationContext &oc = context.getOperationContext();

	DbXmlDbt *dbt = (DbXmlDbt*) document.getContentAsDbt();
	// Allow no-content documents
	int err = 0;
	if (dbt && (dbt->size != 0)) {
		document.getID().setDbtFromThis(oc.key());
		return addContent(oc.txn(), oc.key(), dbt, 0);
	}
	return err;
}

int DocumentDatabase::addContent(Transaction *txn, DbtOut &key, DbXmlDbt *data, u_int32_t flags)
{
	XmlData dest;
	DbXmlDbt compDbt;
	if(compressor_){
		XmlData source(data->data, data->size);
		XmlTransaction xtxn(txn);
		if(!compressor_->compress(xtxn, source, dest)){
			throw XmlException(XmlException::INTERNAL_ERROR,
					   "Error while tring to compress your XML document.");
		}
		compDbt.data = dest.get_data();
		compDbt.size = (u_int32_t)(dest.get_size());
		data = &compDbt;
	}
	return content_.put(txn, &key, data, flags);
}

int DocumentDatabase::updateContentAndIndex(Document &new_document,
					    UpdateContext &context,
					    KeyStash &stash, bool validate,
					    bool updateStats)
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
	if(err != 0) return err;
	
	// Index the new document
	indexer.indexMetaData(index, new_document, stash, true);
	if(new_document.isContentModified()) {
		// Will need the Dbt later, so get it now.  This also
		// allows the content to be used more than once in case
		// definitiveContent_ is INPUTSTREAM
		(void) new_document.getContentAsDbt();
		// use private method that explicitly assumes Dbt content
		ScopedPtr<NsPushEventSource>
			source(new_document.dbt2events(
				       oc.txn(), /*needsValidation*/validate,
				       indexer.getContainer()->nodesIndexed()));
		if (source.get()) {
			indexer.initIndexContent(index, id,
				source.get(), stash, updateStats,
				true, /*isDelete*/false);
			source.get()->start();
		}
	}

	// Set the modified flags of the old document to the same as the
	// new document, so that when we index, we only generate keys for
	// things that have actually changed.
	MetaData::const_iterator end = new_document.metaDataEnd();
	for(MetaData::const_iterator i = new_document.metaDataBegin();
	    i != end; ++i) {
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

	// Update the content
	if(new_document.isContentModified()) {
		OperationContext &oc = context.getOperationContext();
		id.setDbtFromThis(oc.key());
		DbXmlDbt *dbt = (DbXmlDbt*)new_document.getContentAsDbt();
		err = addContent(oc.txn(), oc.key(),dbt, 0);
	}

	if(err == 0) new_document.setContentModified(false);
	if (resetId)
		new_document.getIDToSet() = 0;
	return err;
}

int DocumentDatabase::removeContentAndIndex(const Document &document,
					    UpdateContext &context,
					    KeyStash &stash,
					    bool updateStats)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	IndexSpecification &index = context.getIndexSpecification();
	index.set(Index::INDEXER_DELETE);

	indexer.indexMetaData(index, document, stash, /*checkModified*/false);
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

	// Delete the content
	deleteID(oc, document.getID()); // a no-op
	document.getID().setDbtFromThis(oc.key());
	int err = content_.del(oc.txn(), &oc.key(), 0);
	if (err == DB_NOTFOUND)
		err = 0; // no-content doc
	
	return err;
}

int DocumentDatabase::reindex(const Document &document, OperationContext &oc,
		bool updateStats, bool forDelete)
{
        XmlManager mgr = document.getManager();
        ScopedContainer sc(mgr, document.getContainerID(), true);
        Container *cont = sc.getContainer();
        UpdateContext uc(mgr);
        Indexer &indexer = uc.getIndexer();
        IndexSpecification is;
        cont->getIndexSpecification(oc.txn(), is);
        indexer.resetContext(cont, &oc);
        KeyStash &ks = uc.getKeyStash();
        ks.reset();
        if (forDelete) is.set(Index::INDEXER_DELETE);
        // Index the document
	indexer.indexMetaData(is, document, ks, /*checkModified*/false);
	ScopedPtr<NsPushEventSource> source(document.
		getContentAsEventSource(oc.txn(), /*needsValidation*/false,
					indexer.getContainer()->nodesIndexed()));
	if (source.get()) {
		indexer.initIndexContent(is, document.getID(), source.get(),
			ks, updateStats, false, /*isDelete*/forDelete);
		source.get()->start();
	}
        ks.updateIndex(oc, cont);
        return 0;
}

int DocumentDatabase::reindex(const IndexSpecification &is, const Document &document,
	UpdateContext &context, KeyStash &stash, bool updateStats)
{
	Indexer &indexer = context.getIndexer();
	OperationContext &oc = context.getOperationContext();

	// Index the document
	indexer.indexMetaData(is, document, stash, /*checkModified*/false);
	ScopedPtr<NsPushEventSource> source(document.
		getContentAsEventSource(oc.txn(), /*needsValidation*/false,
					indexer.getContainer()->nodesIndexed()));
	if (source.get()) {
		indexer.initIndexContent(is, document.getID(), source.get(),
			stash, updateStats, false, /*isDelete*/false);
		source.get()->start();
	}

	return 0;
}

int DocumentDatabase::getAllMetaData(OperationContext &context,
				     DictionaryDatabase *dictionary,
				     Document *document,
				     u_int32_t flags) const
{
	int err = 0;
	u_int32_t orig_flags = flags;
	const DocID &did = document->getID();

	//
	// Read each of the meta-data items from the document secondary
	// database.  Content is no longer considered metadata
	//
	Cursor cursor(const_cast<SecondaryDatabase&>(secondary_),
		context.txn(), CURSOR_READ, "DocumentMetaData", flags);
	orig_flags = flags = DB_CURSOR_GET_MASK(const_cast<SecondaryDatabase&>(secondary_),flags);
	flags |= DB_SET_RANGE;
	bool done = false;
	while (!done) {
		did.setDbtFromThis(context.key());
		DbtIn none;
		none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.
		// if DB_RMW set, don't get locks on this get, wait for a match
		err = cursor.get(context.key(), none, flags & (~DB_RMW));
		if (err == 0) {
			DocID db_did;
			NameID db_nid;
			XmlValue::Type type;
			MetaDatum::decodeKeyDbt(context.key(), db_did,
						db_nid, type);
			if (did == db_did) {
				Name name;
				err = dictionary->lookupNameFromID(context, db_nid, name);
				if(err == 0 && !document->containsMetaData(name)) {
					DbtOut *data = new DbtOut();
					err = cursor.get(context.key(), *data,
							 DB_CURRENT);
					if(err == 0) document->setMetaData(
						name, type, &data,
						/*modified*/false);
					delete data; // If not consumed by setThis..
				}
				flags = orig_flags | DB_NEXT;
			} else {
				err = 0;
				done = true;
			}
		} else if (err == DB_NOTFOUND) {
			err = 0;
			done = true;
		} else {
			done = true;
		}
	}
	return err;
}

int DocumentDatabase::getMetaData(OperationContext &context,
	DictionaryDatabase *dictionary, const Name &name,
	const DocID &did, XmlValue::Type &type, DbXmlDbt *metadata,
	u_int32_t flags) const
{
	NameID nid;
	int err = dictionary->lookupIDFromName(context, name,
					       nid, /*define=*/false);
	if(err == 0) {
		Cursor cursor(const_cast<SecondaryDatabase&>(secondary_),
			context.txn(), CURSOR_READ, "DocumentMetaData", flags);
		u_int32_t origFlags = DB_CURSOR_GET_MASK(const_cast<SecondaryDatabase&>(secondary_),flags);
		MetaDatum::setKeyDbt(did, nid, XmlValue::NONE, context.key());
		DbtIn none;
		none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.
		err = cursor.get(context.key(), none,
				 (flags | DB_SET_RANGE) & ~DB_RMW);
		if (err == 0) {
			DocID db_did;
			NameID db_nid;
			MetaDatum::decodeKeyDbt(context.key(), db_did,
						db_nid, type);
			if(db_did == did && db_nid == nid) {
				err = cursor.get(context.key(), *metadata,
						 origFlags|DB_CURRENT);
			} else {
				return DB_NOTFOUND;
			}
		}
	}

	return err;
}

int DocumentDatabase::addMetaData(OperationContext &oc,
				  DictionaryDatabase *dictionary,
				  Document &document)
{
	int err = 0;
	MetaData::const_iterator end = document.metaDataEnd();
	MetaData::const_iterator i;
	for (i = document.metaDataBegin(); err == 0 && i != end; ++i) {
		NameID nid;
		err = dictionary->lookupIDFromName(oc,
						   (*i)->getName(),
						   nid, /*define=*/true);
		if(err == 0) {
			DbtIn value;
			MetaDatum::setKeyDbt(document.getID(),
					     nid, (*i)->getType(),
					     oc.key());
			(*i)->setValueDbtFromThis(value);
			// could throw on error
			err = secondary_.put(oc.txn(), &oc.key(),
					     &value, 0);
		}
	}
	if(err == 0)
		for(i = document.metaDataBegin(); i != end; ++i)
			(*i)->setModified(false);
	return err;
}

int DocumentDatabase::updateMetaData(OperationContext &oc,
				     DictionaryDatabase *dictionary,
				     Document &document)
{
	int err = 0;
	MetaData::const_iterator end = document.metaDataEnd();
	MetaData::const_iterator i;
	std::vector<NameID> toRemove;
	for(i = document.metaDataBegin(); err == 0 && i != end; ++i) {
		if((*i)->isModified()) {
			NameID nid;
			err = dictionary->lookupIDFromName(oc,
							   (*i)->getName(),
							   nid, /*define=*/true);
			if(err == 0) {
				if ((*i)->isRemoved())
					toRemove.push_back(nid);
				else {
					DbtIn value;
					MetaDatum::setKeyDbt(document.getID(),
							     nid,
							     (*i)->getType(),
							     oc.key());
					(*i)->setValueDbtFromThis(value);
					// could throw on error
					err = secondary_.put(oc.txn(),
							     &oc.key(),
							     &value, 0);
				}
			}
		}
	}
	if (toRemove.size() > 0) {
		err = removeMetaData(oc, document.getID(),
				     &toRemove);
	}
	if(err == 0)
		for(i = document.metaDataBegin(); i != end; ++i)
			(*i)->setModified(false);
	return err;
}

static bool
idInList(const std::vector<NameID> &list, const NameID &id)
{
	std::vector<NameID>::const_iterator it = list.begin();
	while (it != list.end()) {
		if (*it == id)
			return true;
		it++;
	}
	
	return false;
}

// if toRemove is non-null, it specifies a list of Name IDs
// to remove; otherwise remove all metadata for the target document
int DocumentDatabase::removeMetaData(OperationContext &oc,
				     const DocID &id,
				     std::vector<NameID> *toRemove)
{
	Cursor cursor(const_cast<SecondaryDatabase&>(secondary_), oc.txn(),
		      CURSOR_WRITE, "DocumentMetaData_remove");

	DbtIn none;
	none.set_flags(DB_DBT_PARTIAL); // Don't pull back the data.

	id.setDbtFromThis(oc.key());

	DocID db_id;
	int err = cursor.get(oc.key(), none, DB_SET_RANGE);
	try {
		while(err == 0) {
			if (toRemove) {
				NameID nm_id;
				XmlValue::Type type;
				MetaDatum::decodeKeyDbt(oc.key(), db_id, nm_id, type);
				if ((id == db_id) && idInList(*toRemove, nm_id))
					cursor.del(0);
			} else {
				db_id.setThisFromDbt(oc.key());
				if (id == db_id)
					cursor.del(0);
			}
			if (id != db_id) // done with document?
				break;
			err = cursor.get(oc.key(), none, DB_NEXT);
		}
	} catch (...) {
		cursor.close();
		throw;
	}
	if(err == DB_NOTFOUND) {
		err = 0;
	}
	cursor.close();
	return err;
}

int DocumentDatabase::dump(DB_ENV *env, const std::string &name,
			   XmlContainer::ContainerType type,
			   std::ostream *out)
{
	DbWrapper content(env, name, "content_", document_name, DEFAULT_CONFIG);
	SecondaryDatabase secondary(env, name, document_name, DEFAULT_CONFIG);
	int err = 0;
	if (type == XmlContainer::WholedocContainer) {
		err = Container::writeHeader(content.getDatabaseName(), out);
		if(err == 0)
			err = content.dump(out);
	}
	if(err == 0)
		err = Container::writeHeader(secondary.getDatabaseName(), out);
	if(err == 0)
		err = secondary.dump(out);

	return err;
}

int DocumentDatabase::load(DB_ENV *env, const std::string &name,
			   XmlContainer::ContainerType type,
			   std::istream *in, unsigned long *lineno)
{
	DbWrapper content(env, name, "content_", document_name, DEFAULT_CONFIG);
	SecondaryDatabase secondary(env, name, document_name, DEFAULT_CONFIG);

	int err = 0;
	if (type == XmlContainer::WholedocContainer) {
		// Load primary
		err = Container::verifyHeader(content.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		} else {
			err = content.load(in, lineno);
		}
	}

	// Load secondary
	if(err == 0) {
		err = Container::verifyHeader(secondary.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "DocumentDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_CONTAINER, Log::L_ERROR, oss.str().c_str());
		} else {
			err = secondary.load(in, lineno);
		}
	}

	return err;
}

int DocumentDatabase::verify(DB_ENV *env, const std::string &name,
			     XmlContainer::ContainerType type,
			     std::ostream *out, u_int32_t flags)
{
	DbWrapper content(env, name, "content_", document_name, DEFAULT_CONFIG);
	SecondaryDatabase secondary(env, name, document_name, DEFAULT_CONFIG);

	int err = 0;
	int terr = 0;
	if (type == XmlContainer::WholedocContainer) {
		if(flags & DB_SALVAGE)
			terr = Container::writeHeader(content.getDatabaseName(),
						      out);
		err = terr;
		terr = content.verify(out, flags);
		if (terr)
			err = terr;
	}
	if(flags & DB_SALVAGE)
		terr = Container::writeHeader(secondary.getDatabaseName(), out);
	if(terr == 0)
		err = terr;
	terr = secondary.verify(out, flags);
	if(terr == 0)
		err = terr;
	return err;
}

// DocumentDatabaseCursor -- shared with NsDocumentDatabase since
// it iterates over the (shared) metadata database (secondary_)
class DocumentDatabaseCursor : public DocumentCursor
{
public:
	DocumentDatabaseCursor(Transaction *txn, DbWrapper &db, u_int32_t flags)
		: cursor_(db, txn, CURSOR_READ, "DocumentDatabaseCursor", flags),
		  cursorFlags_(DB_CURSOR_GET_MASK(db,flags)),
		  done_(false)
	{
		data_.set_flags(DB_DBT_PARTIAL); // only want keys (for now)
	}
	virtual int first(DocID &id)
	{
		return next(id);
	}
	virtual int next(DocID &id)
	{
		int err = 0;
		if(done_) {
			id = 0;
		} else {
			while((err = cursor_.get(key_, data_, cursorFlags_|DB_NEXT)) == 0) {
				id.setThisFromDbt(key_);
				if(id != last_) {
					last_ = id;
					break;
				}
			}
			if (err == DB_NOTFOUND || err == DB_KEYEMPTY) {
				err = 0;
				done_ = true;
				id = 0;
			}
		}
		return err;
	}
	virtual int seek(DocID &id)
	{
		if (last_ >= id) return next(id);

		id.setDbtFromThis(key_);
		// DB_SET_RANGE does >= get on the key (which is the id)
		int err = cursor_.get(key_, data_, DB_SET_RANGE);
		if(err == DB_NOTFOUND) {
			err = 0;
			done_ = true;
			id = 0;
		} else if(err == 0) {
			last_.setThisFromDbt(key_);
			DBXML_ASSERT(last_ >= id);
			id = last_; // let caller know where it landed
		}
		return err;
	}
private:
	Cursor cursor_;
	u_int32_t cursorFlags_;
	bool done_;
	DocID last_;
	DbtOut key_;
	DbtOut data_;
};

// object is donated to caller
int DocumentDatabase::createDocumentCursor(
	Transaction *txn, ScopedPtr<DocumentCursor> &cursor, u_int32_t flags) const
{
	DocumentDatabaseCursor *newCursor =
		new DocumentDatabaseCursor(txn,
					   const_cast<SecondaryDatabase&>(secondary_),
					   flags);
	cursor.reset(newCursor);
	return 0;
}

// upgrade from 2.1 (version 4) to 2.2 (version 5)
// Modify keys for both document secondary and content (if wholedoc storage)
// secondary key: docId, nameId, type
// content key: docId
// for both:
// 1.  open old DB and create a new database
// 2.  for each record:
//    o unmarshal old format; marshal in new, and put into new DB
static int upgrade22(const std::string &name,
		     const std::string &tname, Manager &mgr)
{
	int err = 0;
	SecondaryDatabase secondary(mgr.getDB_ENV(), name,
				    document_name, DEFAULT_CONFIG);
	err = secondary.open(0, /*duplicates*/false, DEFAULT_CONFIG);
	ContainerConfig config;
	config.setPageSize(secondary.getPageSize());
	SecondaryDatabase newSec(mgr.getDB_ENV(), tname,
				 document_name, config);
	err = newSec.open(0, /*duplicates*/false, CREATE_CONFIG);
	// the key is: docId,nodeId,type
	DbXml::DbXmlDbt key;
	DbXml::DbXmlDbt data;
	Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_READ);
	int ret = 0;
	while ((err == 0) &&
	       (ret = curs.get(key, data, DB_NEXT)) == 0) {
		// decode key using pre-2.2 code
		DocID did;
		NameID nid;
		XmlValue::Type type;
		u_int32_t *p = (u_int32_t *)key.data;
		u_int32_t id1, id2;
		id1 = *p++;
		id2 = *p++;
		if (Manager::isBigendian()) {
			M_32_SWAP(id1);
			M_32_SWAP(id2);  
		}
		did = id1;            
		nid = id2;
		type = (XmlValue::Type) * ((unsigned char*)p);
		// encode using 2.2. code
		DbtOut newKey;
		MetaDatum::setKeyDbt(did, nid, type, newKey);
		err = newSec.put(0, &newKey, &data, 0);
	}
	curs.close();
	secondary.close(0);
	newSec.close(0);
	if (err == 0) {
		DbWrapper content(mgr.getDB_ENV(), name,
				  "content_",
				  document_name, DEFAULT_CONFIG);
		err = content.open(0, DB_BTREE, DEFAULT_CONFIG);
		if (err != 0) {
			// ignore ENOENT
			if (err == ENOENT)
				err = 0;
			return err;
		}
		config.setPageSize(content.getPageSize());
		DbWrapper newContent(mgr.getDB_ENV(), tname,
				     "content_", document_name,
				     config);
		err = newContent.open(0, DB_BTREE,
				      CREATE_CONFIG);
		if (err != 0)
			return err;
		// the key is: docId
		DbXml::DbXmlDbt key;
		DbXml::DbXmlDbt data;
		Cursor curs1(content, (Transaction*)0, DbXml::CURSOR_READ);
		int ret = 0;
		while ((err == 0) &&
		       (ret = curs1.get(key, data, DB_NEXT)) == 0) {
			// decode/encode key, rewrite
			u_int32_t *p = (u_int32_t *)key.data;
			u_int32_t id1;
			id1 = *p;
			if (Manager::isBigendian())
				M_32_SWAP(id1);
			DocID id(id1);
			DbtOut newKey;
			id.setDbtFromThis(newKey);
			err = newContent.put(0, &newKey, &data, 0);
		}
		curs1.close();
		content.close(0);
		newContent.close(0);
	}
	return err;
}

// determine if the name ID needs swapping.
// If the container was created big-endian, then yes.
// This is detected by finding *little-endian* values
// that are very large. Note that this could have issues
// if there is a *HUGE* number of metadata names, which
// is very unlikely
static bool needsSwap(u_int32_t val)
{
	bool be = Manager::isBigendian();
	if (be) {
		if (val <= 0x00010000)
			return true;
	} else {
		if (val >= 0x00010000)
			return true;
	}
	return false;
	
}

// static
void DocumentDatabase::upgrade(const std::string &name,
			       const std::string &tname, Manager &mgr,
			       int old_version, int current_version)
{
	int err = 0;
	DBXML_ASSERT(old_version < current_version);
	// 2.0 -> 2.1 upgrade, if necessary, must be first
	if (old_version == VERSION_20){
		SecondaryDatabase secondary(mgr.getDB_ENV(), name,
					    document_name, DEFAULT_CONFIG);
		err = secondary.open(0, /*duplicates*/false, DEFAULT_CONFIG);
		// byte-swap name ids in document metadata keys.
		// iterate through all records in secondary.
		// the key is: docId,nodeId,type
		DbXmlDbt key;
		DbXmlDbt data;
		Cursor curs(secondary, (Transaction*)0, DbXml::CURSOR_WRITE);
		int ret = 0;
		while ((err == 0) &&
		       (ret = curs.get(key, data, DB_NEXT)) == 0) {
			u_int32_t *p = reinterpret_cast<u_int32_t*>(key.data);
			++p;
			// swap happens on second int, not first
			if (!needsSwap(*p))
				break;
			M_32_SWAP(*p);
			curs.del(0);
			err = curs.put(key, data, DB_KEYFIRST);
		}
	}
	if (old_version < VERSION_22 && err == 0)
		err = upgrade22(name, tname, mgr);
	else if (err == 0) {
		err = DbWrapper::copySecondary(
			mgr.getDB_ENV(),
			name, tname, "secondary_",
			document_name);
		if (err == 0)
			err = DbWrapper::copySecondary(
				mgr.getDB_ENV(),
				name, tname, "content_",
				document_name);
	}
	if (err != 0) {
		char buf[100];
		sprintf(buf, "Unexpected error upgrading Document DB: %d",
			err);
		throw XmlException(XmlException::DATABASE_ERROR, buf);
	}
}
