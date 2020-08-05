//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <dbxml/XmlValue.hpp>
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlResolver.hpp>
#include <dbxml/XmlTransaction.hpp>
#include <dbxml/XmlData.hpp>
#include <dbxml/XmlExternalFunction.hpp>
#include "DbXmlURIResolver.hpp"
#include "DbXmlUri.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../Results.hpp"
#include "../Manager.hpp"
#include "../Document.hpp"
#include "../QueryContext.hpp"
#include "../XmlInputStreamWrapper.hpp"
#include "../nodeStore/NsUtil.hpp"

#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/functions/ExternalFunction.hpp>

#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <sstream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static void _throwResolutionError(const std::string &uri, const char *resource)
{
	ostringstream oss;
	oss << "External access not allowed.  Cannot resolve\n\t" << resource << ": " <<
		uri;
	throw XmlException(XmlException::QUERY_EVALUATION_ERROR,
			   oss.str());
}

XmlInputStream *DbXmlSchemeResolver::resolveSchema(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &schemaLocation,
	const std::string &nameSpace) const
{
	DbXmlUri uri(schemaLocation, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		// Try to resolve the schema as if it's an XML document
		// in a container.
		Transaction *t = 0;
		if (txn)
			t = *txn;
		XmlDocument doc(uri.openDocument(mgr, t));
		// Can't use getContentAsInputStream(), as the XmlInputStream
		// returned does not hold a reference to the XmlDocument, and
		// therefore becomes invalid when the XmlDocument goes out of
		// scope - jpcs
		XmlData content(doc.getContent());
		char *copy = new char[content.get_size()];
		memcpy(copy, content.get_data(), content.get_size());
		return mgr.createMemBufInputStream(
                        copy, (uint32_t)content.get_size(),
                        schemaLocation.c_str(), /*adoptBuffer*/true);
	}
	return 0;
}

XmlInputStream *DbXmlSchemeResolver::resolveEntity(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &systemId,
	const std::string &publicId) const
{
	DbXmlUri uri(systemId, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		// An entity will most likely not be XML, so
		// it cannot be document content.  If, in the future, we
		// choose a convention for storing entities such as DTDs (e.g. as metadata
		// or in a separate DB database), this is where the resolution
		// code would go.
		// If the entity is an XML Schema, resolveSchema will be called, which
		// can look inside a container.
	}
	return 0;
}

XmlInputStream *DbXmlSchemeResolver::resolveModule(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &moduleLocation,
	const std::string &nameSpace) const
{
	DbXmlUri uri(moduleLocation, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		// Since a module is (by definition) not an XML document,
		// it cannot be document content.  If, in the future, we
		// choose a convention for storing modules (e.g. as metadata
		// or in a separate DB database), this is where the resolution
		// code would go.
	}
	return 0;
}

ResolverStore::ResolverStore()
	: secure_(false)
{
 	registerResolver(dbxmlResolver_);
}

void ResolverStore::registerResolver(const XmlResolver &resolver)
{
	resolvers_.push_back(&resolver);
}

ResolverStore::const_iterator ResolverStore::begin() const
{
	return resolvers_.begin();
}

ResolverStore::const_iterator ResolverStore::end() const
{
	return resolvers_.end();
}

DbXmlURIResolver::DbXmlURIResolver(Manager &mgr, Transaction *txn)
	: mgr_(mgr),
	  txn_(txn)
{
}

static std::string _createURI(const XMLCh *base, const XMLCh *uri,
			      Manager &mgr)
{
	try {
		if (base) {
			XMLUri baseURI(base);
			XMLUri newURI(&baseURI, uri);
			XMLChToUTF8 ret(newURI.getUriText());
			return ret.str();
		} else {
			XMLUri newURI(uri);
			XMLChToUTF8 ret(newURI.getUriText());
			return ret.str();
		}
	} catch (XMLException &) {
		XMLChToUTF8 tbase(base);
		XMLChToUTF8 turi(uri);
		string msg = "Error constucting URI from base, uri: ";
		msg += tbase.str();
		msg += ", ";
		msg += turi.str();
		mgr.log(Log::C_QUERY, Log::L_INFO, msg);
		return "";
	}
}

bool DbXmlURIResolver::resolveDocument(Sequence &result, const XMLCh* uri,
	DynamicContext* context, const QueryPathNode *projection)
{

	bool retVal = false;
	XmlValue value;
	std::string sysId;
	// Concat base URI (if present) and uri
	sysId = _createURI(context->getBaseURI(), uri, mgr_);
	if (!sysId.empty()) {
		if(resolveDocument(sysId, value)) {
			retVal = true;
			/*Make sure the configuration and the document share a 
			  CacheDatabaseMinder */
			DbXmlConfiguration *conf = GET_CONFIGURATION(context);
			if(conf && value.isNode()){
				CacheDatabaseMinder &cbm = conf->getDbMinder();
				XmlDocument doc = value.asDocument();
				if(cbm.isNull())
					cbm.findOrAllocate((Manager&)(conf->getManager()),
							   ((Document *)doc)->getContainerID());
				(*doc).setDbMinder(cbm);
			}
		} else if(mgr_.getResolverStore().
			  getSecure()) {
			_throwResolutionError(sysId, "document");
		} else {
			// try to find known URI schemes using base
			retVal = resolveDocKnownSchemes(sysId, value, context);
		}
	}
	if(!retVal) {
		XMLBuffer errMsg;
		errMsg.set(X("Error retrieving resource: "));
		errMsg.append(X(sysId.c_str()));
		errMsg.append(X(" [err:FODC0002]"));
		XQThrow2(XMLParseException,X("DbXmlURIResolver::resolveDocument"), errMsg.getRawBuffer());
	} else if(!value.isNull()) {
		result.addItem(Value::convertToItem(value, context, false));
	}
	return retVal;
}

static void convertToSequence(XmlResults &in, Sequence &out, DynamicContext* context)
{
	if(in.isNull()) return;

	XmlValue value;
	while(in.next(value)) {
		out.addItem(Value::convertToItem(value, context, false));
	}
}

bool DbXmlURIResolver::resolveCollection(Sequence &result, const XMLCh* uri,
	DynamicContext* context, const QueryPathNode *projection)
{

	// Concat base URI (if present) and uri
	std::string sysId = _createURI(context->getBaseURI(), uri, mgr_);
	bool retVal = false;
	if (!sysId.empty()) {
		XmlManager mgr(mgr_);
		XmlResults results = new ValueResults(mgr);
		if(resolveCollection(sysId, results)) {
			results.reset();
			convertToSequence(results, result, context);
			result.sortIntoDocumentOrder(context);
			retVal = true;
		} else if(mgr_.getResolverStore().getSecure()) {
			_throwResolutionError(sysId, "collection");
		}
	}

	if(!retVal) {
		XMLBuffer errMsg;
		errMsg.set(X("Error retrieving resource: "));
		errMsg.append(X(sysId.c_str()));
		errMsg.append(X(" [err:FODC0004]"));
		XQThrow2(XMLParseException,X("DbXmlURIResolver::resolveCollection"), errMsg.getRawBuffer());
	}
	return retVal;
}

bool DbXmlURIResolver::resolveDefaultCollection(Sequence &result, DynamicContext* context,
	const QueryPathNode *projection)
{
	// Handled in QueryPlanFunction
	return false;
}

bool DbXmlURIResolver::putDocument(const Node::Ptr &document, const XMLCh *uri, DynamicContext *context)
{
	// Handled by DbXmlUpdateFactory
	return false;
}

bool DbXmlURIResolver::resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri,
	const StaticContext* context)
{
	XmlManager mgr(mgr_);
	XmlResults results = new ValueResults(mgr);
	if(resolveModuleLocation(XMLChToUTF8(nsUri).str(), results)) {
		results.reset();
		XmlValue value;
		while(results.next(value)) {
			result->push_back(context->getMemoryManager()->
				getPooledString(value.asString().c_str()));
		}
		return true;
	}
	return false;
}

class DbXmlExFunc : public ExternalFunction
{
public:
	DbXmlExFunc(const XMLCh *uri, const XMLCh *name, size_t numArgs,
		XmlExternalFunction *impl, Transaction *txn, Manager &mgr, XPath2MemoryManager *mm)
		: ExternalFunction(uri, name, (unsigned int)numArgs, mm),
		  impl_(impl),
		  txn_(txn),
		  mgr_(mgr)
	{
	}

	virtual ~DbXmlExFunc()
	{
		impl_->close();
	}

	virtual Result execute(const Arguments *args, DynamicContext *context) const
	{
		XmlTransaction txn(txn_);
		XmlManager mgr(mgr_);
		DbXmlArgs dbxmlArgs(args, context, getNumberOfArguments());
		XmlResults results = impl_->execute(txn, mgr, dbxmlArgs);

		// Could be lazier - jpcs
		Sequence seq;
		convertToSequence(results, seq, context);
		return seq;
	}

private:
	class DbXmlArgs : public XmlArguments
	{
	public:
		DbXmlArgs(const ExternalFunction::Arguments *in, DynamicContext *context, unsigned int num)
			: in_(in), context_(context), num_(num) {}

		virtual XmlResults getArgument(size_t index) const
		{
			// Could be lazier - jpcs
			XmlResults xmlresult = new ValueResults(GET_CONFIGURATION(context_)->getManager());

			try {
				Result result = in_->getArgument(index, context_);
				Item::Ptr item;
				while((item = result->next(context_)).notNull()) {
					if(item->isNode()) {
						xmlresult.add(Value::create((Node*)item.get(), /*lazyDocs*/true));
						/* The XmlResults needs access to the dbMinder so that
						it can access temporary databases if the nodes are passed
						to Java then need to be recreated in C++ */
						if((*xmlresult).getDbMinder().isNull())
							(*xmlresult).getDbMinder() = GET_CONFIGURATION(context_)->getDbMinder();
					}
					else {
						xmlresult.add(Value::create(item, context_));
					}
				}
			}
			catch(const XQException &e) {
				throw XmlException(XmlException::QUERY_EVALUATION_ERROR, e);
			}

			return xmlresult;
		}

		virtual unsigned int getNumberOfArgs() const { return num_; }

	private:
		const ExternalFunction::Arguments *in_;
		DynamicContext *context_;
		unsigned int num_;
	};


	XmlExternalFunction *impl_;
	Transaction *txn_;
	Manager &mgr_;
};

ExternalFunction *DbXmlURIResolver::resolveExternalFunction(const XMLCh *uri, const XMLCh *name,
	size_t numArgs, const StaticContext* context)
{
	XmlExternalFunction *result = resolveExternalFunction(XMLChToUTF8(uri).str(),
		XMLChToUTF8(name).str(), numArgs);
	if(result == 0) return 0;

	return new DbXmlExFunc(uri, name, numArgs, result, txn_, mgr_, context->getMemoryManager());
}

InputSource* DbXmlURIResolver::resolveEntity(XMLResourceIdentifier *ri)
{
	XmlInputStream *result = 0;
	switch(ri->getResourceIdentifierType()) {
	case XMLResourceIdentifier::SchemaGrammar:
	case XMLResourceIdentifier::SchemaImport:
	case XMLResourceIdentifier::SchemaInclude:
	case XMLResourceIdentifier::SchemaRedefine: {
		result = resolveSchema(XMLChToUTF8(ri->getSchemaLocation()).str(),
				       XMLChToUTF8(ri->getNameSpace()).str());
		break;
	}
	case XMLResourceIdentifier::UnKnown: {
		// XQuery module imports are of type UnKnown
		result = resolveModule(XMLChToUTF8(ri->getSystemId()).str(),
			XMLChToUTF8(ri->getNameSpace()).str());
		if(result == 0) {
			// Preserve old behaviour for compatibility
			// TBD This should be deprecated and taken out at a later date - jpcs
			result = resolveEntity(XMLChToUTF8(ri->getSystemId()).str(),
				XMLChToUTF8(ri->getNameSpace()).str());
		}
		break;
	}
	case XMLResourceIdentifier::ExternalEntity: {
		result = resolveEntity(XMLChToUTF8(ri->getSystemId()).str(),
				       XMLChToUTF8(ri->getPublicId()).str());
		break;
	}
	default: {
		// Stop compiler warnings
		break;
	}
	}

	if(result != 0) {
		XmlInputStreamWrapper *res = new XmlInputStreamWrapper(&result);
		res->setPublicId(ri->getPublicId());
		res->setSystemId(ri->getSystemId());
		return res;
	} else if(mgr_.getResolverStore().getSecure()) {
		_throwResolutionError(
			XMLChToUTF8(ri->getSystemId()).str(),
			"entity");
	}
	return 0;
}

bool DbXmlURIResolver::resolveDocument(const std::string &uri,
				       XmlValue &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));
	
	ResolverStore::const_iterator end = mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it = mgr_.getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveDocument(txn.get(), mgr, uri, result)) {
			return true;
		}
	}
	return false;
}

bool DbXmlURIResolver::resolveCollection(const std::string &uri,
					 XmlResults &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it = mgr_.getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveCollection(txn.get(), mgr, uri, result)) {
			return true;
		}
	}
	return false;
}

bool DbXmlURIResolver::resolveModuleLocation(const std::string &nameSpace,
	XmlResults &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it = mgr_.getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveModuleLocation(txn.get(), mgr, nameSpace, result)) {
			return true;
		}
	}
	return false;
}

XmlExternalFunction *DbXmlURIResolver::resolveExternalFunction(const std::string &uri,
	const std::string &name, size_t numberOfArgs) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlExternalFunction *result = 0;
	ResolverStore::const_iterator end = mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it = mgr_.getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveExternalFunction(txn.get(), mgr, uri, name, numberOfArgs);
		if(result != 0) break;
	}
	return result;
}

XmlInputStream *DbXmlURIResolver::resolveSchema(
	const std::string &schemaLocation, const std::string &nameSpace) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end = mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it = mgr_.getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveSchema(txn.get(), mgr, schemaLocation, nameSpace);
		if(result != 0) break;
	}
	return result;
}

XmlInputStream *DbXmlURIResolver::resolveEntity(
	const std::string &systemId, const std::string &publicId) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end =
		mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it =
		    mgr_.getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveEntity(txn.get(), mgr, systemId,
					      publicId);
		if(result != 0) break;
	}
	return result;
}

XmlInputStream *DbXmlURIResolver::resolveModule(const std::string &location,
	const std::string &nameSpace) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end =
		mgr_.getResolverStore().end();
	XmlManager mgr(mgr_);
	for(ResolverStore::const_iterator it =
		    mgr_.getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveModule(txn.get(), mgr, location,
					      nameSpace);
		if(result != 0) break;
	}
	return result;
}

bool DbXmlURIResolver::resolveDocKnownSchemes(const std::string &uri,
					      XmlValue &value,
					      DynamicContext *context)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	ReferenceMinder *minder = conf->getMinder();

	// Check in the ReferenceMinder for the document first
	UTF8ToXMLCh uri16(uri);
	Document *doc = minder->findDocument(uri16.str());
	if(doc != 0) {
		value = XmlValue(XmlDocument(doc));
		return true;
	}
	XmlDocument xdoc;
	XmlInputStream *is = 0;
	if (uri.find("http:") == 0 || uri.find("file:") == 0) {
		xdoc = mgr_.createDocument();
		is = mgr_.createURLInputStream("", uri);
	}
	if (is) {
		xdoc.setContentAsXmlInputStream(is);
		xdoc.setName(uri);
		Document &tdoc = (Document&)xdoc;
		tdoc.setDocumentURI(uri16.str());
		CacheDatabaseMinder &dbMinder = conf->getDbMinder();
		 // ensure temp DB for container ID 0 exists
		dbMinder.findOrAllocate(mgr_, 0);
		tdoc.setDbMinder(dbMinder);

		// Find the ImpliedSchemaNodes for the document, so we can
		// perform document projection
		ISNVector isns;
		conf->getImpliedSchemaNodes(uri, isns);

		tdoc.changeContentToNsDom(&isns); // parses document and may throw on error
		minder->addDocument(xdoc);
		value = XmlValue(xdoc);
		return true;
	}
	// TBD: others...
	return false;
}
