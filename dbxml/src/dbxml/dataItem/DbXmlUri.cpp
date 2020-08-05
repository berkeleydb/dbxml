//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "DbXmlUri.hpp"
#include "../UTF8.hpp"
#include "../Manager.hpp"
#include "../OperationContext.hpp"
#include "../nodeStore/NsUtil.hpp"
#include <dbxml/XmlContainer.hpp>
#include <dbxml/XmlTransaction.hpp>
#include <dbxml/XmlException.hpp>

#include <xercesc/util/XMLUri.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlUri::dbxmlScheme16[] = { 'd', 'b', 'x', 'm', 'l', 0 };

const char *DbXmlUri::dbxmlBaseURI = "dbxml:/";

DbXmlUri::DbXmlUri(const std::string &uri, bool documentUri)
	: dbxmlScheme_(false),
	  resolved_(false)
{
	parseUri(0, UTF8ToXMLCh(uri).str(), documentUri);
}

DbXmlUri::DbXmlUri(const XMLCh *uri, bool documentUri)
	: dbxmlScheme_(false),
	  resolved_(false)
{
	parseUri(0, uri, documentUri);
}

DbXmlUri::DbXmlUri(const std::string &baseUri, const std::string &relativeUri,
		   bool documentUri)
	: dbxmlScheme_(false),
	  resolved_(false)
{
	parseUri(UTF8ToXMLCh(baseUri).str(), UTF8ToXMLCh(relativeUri).str(),
		 documentUri);
}

DbXmlUri::DbXmlUri(const XMLCh *baseUri, const XMLCh *relativeUri,
		   bool documentUri)
	: dbxmlScheme_(false),
	  resolved_(false)
{
	parseUri(baseUri, relativeUri, documentUri);
}

// throws if it doesn't exist
XmlContainer DbXmlUri::openContainer(XmlManager &mgr, Transaction *txn) const
{
	return openContainer(containerName_, mgr, txn);
}

// throws if it doesn't exist
XmlContainer DbXmlUri::openContainer(const string &containerName, XmlManager &mgr, Transaction *txn)
{
	Manager &mgrImpl = (Manager &)mgr;

	if (mgrImpl.allowAutoOpen()) {
		if(txn) {
			XmlTransaction childTxn = txn->createChild();
			XmlContainer container(
				mgrImpl.openContainer(
					containerName, childTxn,
					mgrImpl.getDefaultContainerConfig()));
			childTxn.commit();
			return container;
		} else {
			return mgrImpl.openContainer(
				containerName, 0,
				mgrImpl.getDefaultContainerConfig());
		}
	} else {
		// container must be open
		XmlContainer cont =
			mgrImpl.getOpenContainer(containerName);
		if (!cont.isNull())
			return cont;
		std::string msg = "Cannot resolve container: ";
		msg += containerName;
		msg += ".  Container not open and auto-open is not enabled.";
		msg += "  Container may not exist.";
		throw XmlException(
			XmlException::CONTAINER_CLOSED, msg);
	}
}

XmlDocument DbXmlUri::openDocument(XmlManager &mgr, Transaction *txn) const
{
	XmlContainer cont(openContainer(mgr, txn));
	XmlDocument doc;

	OperationContext oc(txn);
	((Container&)cont).getDocument(oc, documentName_, doc, DBXML_LAZY_DOCS);

	return doc;
}

//
// Old style dbxml URIs
//
//     dbxml:/absolute/path/to/container/document
//     dbxml:relative/path/to/container/document
//     dbxml:C:/windows/container/document
//     dbxml:container_alias/document
//
//   With base URI
//
//     dbxml: + /absolute/path/to/container/document = dbxml:/absolute/path/to/container/document
//     dbxml: + relative/path/to/container/document = dbxml:relative/path/to/container/document
//     dbxml: + C:/windows/container/document = dbxml:C:/windows/container/document
//
// --------------------------------------------
//
// New style dbxml URIs
//
//     dbxml:/container_alias/document
//     dbxml://xxx/container_alias/document (NB xxx is the URL authority, and is ignored)
//     dbxml:///container_alias/document
//     dbxml:////absolute/path/to/container/document
//     dbxml:/C:/windows/container/document
//
//   With base URI
//
//     dbxml:/ + container_alias/document = dbxml:/container_alias/document
//     dbxml:/container_alias + container_alias2/document2 = dbxml:/container_alias2/document2
//     dbxml:/container_alias/document + /container_alias2/document2 = dbxml:/container_alias2/document2
//     dbxml:///container_alias/document + /container_alias2/document2 = dbxml:///container_alias2/document2
//     dbxml://xxx/container_alias/document + /container_alias2/document2 = dbxml://xxx/container_alias2/document2
//     dbxml:/container_alias/document + document2 = dbxml:/container_alias/document2
//
//   Backwards compatibility mappings
//
//     dbxml:container_alias/document -> dbxml:/container_alias/document
//
void DbXmlUri::parseUri(const XMLCh *baseUri, const XMLCh *relativeUri,
			bool documentUri)
{
	try {
		XMLUri uri;
		if(baseUri != 0 && *baseUri != 0) {
			XMLUri base(baseUri);
			uri = XMLUri(&base, relativeUri);
		} else {
			uri = XMLUri(relativeUri);
		}

		resolvedUri_ = XMLChToUTF8(uri.getUriText()).str();
		resolved_ = true;

		if(!NsUtil::nsStringEqual(dbxmlScheme16, uri.getScheme())) {
			return;
		}

		const XMLCh *path = uri.getPath();
		size_t pathLen = NsUtil::nsStringLen(path);

		// Remove a leading slash
		if(*path == '/') {
			++path;
			--pathLen;
		}

		// Remove a trailing slash
		if(pathLen && (*(path + pathLen - 1) == '/')) {
			--pathLen;
		}

		if(documentUri && pathLen) {
			// Search backwards for the last slash
			const XMLCh *doc = path + pathLen - 1;
			while(doc > path && *doc != '/')
				--doc;

			if(doc == path) {
				// No document specified
				return;
			}

			size_t docLen = pathLen - (doc - path);
			documentName_ = XMLChToUTF8(doc + 1, docLen - 1).str();

			pathLen = doc - path;
		}

		if(pathLen != 0) {
			containerName_ = XMLChToUTF8(path, pathLen).str();
		}

		dbxmlScheme_ = true;
	}
	catch(...)  {
		// don't throw; it's just that the URI isn't valid.
	}
}

// validate a Base URI.  Just needs to be:
//   scheme:
// Special URI characters, such as '/' '?' and '#'
// cannot precede the colon.  Scheme must be
// more than 1 character.
// static
bool DbXmlUri::isValidBase(const std::string &base)
{
	try {
		XMLUri uri(UTF8ToXMLCh(base).str());
		return true;
	}
	catch(...) {
	}
	return false;
}
