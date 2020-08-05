//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLURI_HPP
#define	__DBXMLURI_HPP

#include <string>
#include <xercesc/util/XMLUni.hpp>

namespace DbXml
{

class XmlManager;
class Transaction;
class XmlContainer;
class XmlDocument;

/**
 * A class that encapsulates the parsing and understanding
 * of a DbXml scheme uri, as well as local file scheme uri's
 * used as an absolute path to a container
 */
class DbXmlUri
{
public:
	DbXmlUri(const std::string &uri, bool documentUri);
	DbXmlUri(const std::string &baseUri, const std::string &relativeUri, bool documentUri);
	DbXmlUri(const XMLCh *uri, bool documentUri);
	DbXmlUri(const XMLCh *baseUri, const XMLCh *relativeUri, bool documentUri);

	bool isResolved() const { return resolved_; }
	bool isDbXmlScheme() const { return dbxmlScheme_; }

	const std::string &getResolvedUri() const { return resolvedUri_; }
	const std::string &getContainerName() const { return containerName_; }
	const std::string &getDocumentName() const { return documentName_; }

	XmlContainer openContainer(XmlManager &mgr, Transaction *txn) const;
	XmlDocument openDocument(XmlManager &mgr, Transaction *txn) const;

	// validate a Base URI
	static bool isValidBase(const std::string &baseUri);

	// Tries to open a named container for use in a query
	static XmlContainer openContainer(const std::string &containerName, XmlManager &mgr, Transaction *txn);

	// globals
	static const XMLCh dbxmlScheme16[];
	static const char *dbxmlBaseURI;

private:
	void parseUri(const XMLCh *baseUri, const XMLCh *relativeUri, bool documentUri);

	bool dbxmlScheme_, resolved_;
	std::string resolvedUri_;
	std::string containerName_;
	std::string documentName_;
};

}

#endif

