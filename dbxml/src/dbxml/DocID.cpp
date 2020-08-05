//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <sstream>

#include "DbXmlInternal.hpp"
#include "DocID.hpp"
#include "ScopedDbt.hpp"
#include "Manager.hpp"
#include "ReferenceMinder.hpp"
#include "QueryContext.hpp"
#include "Document.hpp"
#include "nodeStore/NsFormat.hpp"
#include "TypeConversions.hpp"
#include "dataItem/DbXmlConfiguration.hpp"

#include "db_utils.h"

using namespace DbXml;
using namespace std;

void DocID::setThisFromDbt(const DbXmlDbt &dbt)
{
	unmarshal(dbt.data);
}

void DocID::setDbtFromThis(DbtOut &dbt) const
{
	char buf[9];
	u_int32_t len = marshal(buf);
	dbt.set(buf, len); // copies the data
}

u_int32_t DocID::unmarshal(const void *buf)
{
	return NsFormat::unmarshalInt64((const xmlbyte_t *) buf, &id_);
}

u_int32_t DocID::marshal(void *buf) const
{
	return NsFormat::marshalInt64((xmlbyte_t *) buf, id_);
}

u_int32_t DocID::marshalSize() const
{
	return NsFormat::countInt64(id_);
}

std::string DocID::asString() const
{
	return DbXml::toString(id_);
}

void DocID::asChars(char *buf, int buflen) const
{
	DbXml::toChars(id_, buf, (size_t)buflen);
}

void DocID::fetchDocument(const ContainerBase *container, DbXmlConfiguration &conf,
			  XmlDocument &doc, ReferenceMinder *minder) const
{
	fetchDocument(container, conf.getOperationContext(),
		      conf.getFlags(), doc, minder);

	if(container->getContainerID() == 0 && doc.isNull()) {
		// Just make a new XmlDocument
		XmlManager &mgr = conf.getManager();
		doc = mgr.createDocument();

		// get the temp DB for construction (container id 0)
		CacheDatabase *cdb = conf.getDbMinder().findOrAllocate((Manager&)mgr, 0);

		// Tell the document to use this database, and that it's
		// content is "NsDom"
		// TBD GMF: at some point, Document and NsDocument objects will not
		// be required by DbXmlNodeImpl, so creation/init of Document
		// objects is not required.
		Document &tdoc = (Document&)doc;
		tdoc.setDbMinder(conf.getDbMinder());
		tdoc.setContentAsNsDom(*this, cdb);

		if(minder != 0) minder->addDocument(doc);
	}

	// tell the document to use the shared databases in
	// the DbXmlConfiguration's CacheDatabaseMinder
	// If necessary, initialize the configuration's minder
	// TBD GMF -- figure out how to avoid using Container * here
	Container *cont = const_cast<ContainerBase*>(container)->getContainer();
	if (cont != 0 && !cont->isNodeContainer()) {
		Manager &mgr = (Manager&)cont->getManager();
		CacheDatabaseMinder &dbminder = conf.getDbMinder();
		dbminder.init(mgr);
		((Document&)doc).setDbMinder(dbminder);
	}
}

void DocID::fetchDocument(const ContainerBase *container, OperationContext &oc,
	u_int32_t flags, XmlDocument &doc, ReferenceMinder *minder) const
{
	// Look up the document in the cache
	if(minder != 0) {
		doc = minder->findDocument(container->getContainerID(), *this);
	}

	if(doc.isNull()) {
		// TBD GMF -- figure out how to avoid using Container * here
		Container *cont = const_cast<ContainerBase*>(container)->getContainer();
		if(cont == 0) return;

		// Fetch the document from the container
		int err = cont->getDocument(oc, *this, doc, flags);
		// The document should be found
		if(err != 0) {
			container->log(Log::C_QUERY, Log::L_ERROR,
				"Invalid index values found during query (document not found)");
			throw XmlException(XmlException::INTERNAL_ERROR, "Invalid indexes");
		}
		if(minder != 0) {
			// Store the document in the cache
			minder->addDocument(doc);
		}
	}
}

int DocID::compareMarshaled(const unsigned char *&p1,
			    const unsigned char *&p2)
{
	// Sort based on marshaled integer length first
	// NOTE: countMarshaledInt knows about 64-bit encodings
	int count = NsFormat::countMarshaledInt(p1);
	int res = count - NsFormat::countMarshaledInt(p2);
	if(res != 0) return res;

	// Marshaled integers can be sorted using normal
	// lexicographical comparison if they are the same
	// length.
	while(count && (res = (int)*p1 - (int)*p2) == 0) {
		++p1; ++p2; --count;
	}
	return res;
}

