//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLCONTAINER_HPP
#define	__XMLCONTAINER_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include "XmlValue.hpp"
#include "XmlIndexSpecification.hpp"

#include <string>
#include <db.h>

namespace DbXml
{

class Container;
class XmlManager;
class XmlEventReader;
class XmlEventWriter;
	
class DBXML_EXPORT XmlContainer
{
public:
	enum ContainerType {
		WholedocContainer,///< container stores documents intact
		NodeContainer     ///< container stores documents in pieces
	};

	/** @name For Reference Counting */
	XmlContainer(); // default constructor, for convenience
	XmlContainer(const XmlContainer &o);
	XmlContainer &operator=(const XmlContainer &o);
	~XmlContainer();
	bool isNull() const { return container_ == 0; }

	/** @name Misc. Methods */
	void sync();
	bool addAlias(const std::string &alias);
	bool removeAlias(const std::string &alias);

	/** @name Getter Methods */
	const std::string &getName() const;
	ContainerType getContainerType() const;
	bool getIndexNodes() const;
	u_int32_t getPageSize() const;
	XmlContainerConfig getFlags() const;
	XmlContainerConfig getContainerConfig() const;  
	XmlManager &getManager();

	/** @name Index Specification Methods */
	XmlIndexSpecification getIndexSpecification() const;
	void setIndexSpecification(const XmlIndexSpecification &index,
				   XmlUpdateContext &context);

	void addIndex(const std::string &uri, const std::string &name,
		      const std::string &index, XmlUpdateContext &context);
	void addIndex(const std::string &uri, const std::string &name,
		      XmlIndexSpecification::Type indexType,
		      XmlValue::Type syntaxType, XmlUpdateContext &context);
	void deleteIndex(const std::string &uri, const std::string &name,
			 const std::string &index, XmlUpdateContext &context);
	void replaceIndex(const std::string &uri, const std::string &name,
			  const std::string &index, XmlUpdateContext &context);

	void addDefaultIndex(const std::string &index, XmlUpdateContext &context);
	void deleteDefaultIndex(const std::string &index, XmlUpdateContext &context);
	void replaceDefaultIndex(const std::string &index, XmlUpdateContext &context);

	void setAutoIndexing(bool value, XmlUpdateContext &context);
	bool getAutoIndexing() const;

	/** @name Index Specification Methods (Transacted) */
	/**
	 * Valid flags:
	 * DB_RMW
	 */
	XmlIndexSpecification getIndexSpecification(XmlTransaction &txn,
						    u_int32_t flags = 0) const;

	void setIndexSpecification(XmlTransaction &txn,
				   const XmlIndexSpecification &index,
				   XmlUpdateContext &context);

	void addIndex(XmlTransaction &txn, const std::string &uri,
		      const std::string &name, const std::string &index,
		      XmlUpdateContext &context);
	void addIndex(XmlTransaction &txn,
		      const std::string &uri, const std::string &name,
		      XmlIndexSpecification::Type indexType,
		      XmlValue::Type syntaxType, XmlUpdateContext &context);
	void deleteIndex(XmlTransaction &txn, const std::string &uri,
			 const std::string &name, const std::string &index,
			 XmlUpdateContext &context);
	void replaceIndex(XmlTransaction &txn, const std::string &uri,
			  const std::string &name, const std::string &index,
			  XmlUpdateContext &context);

	void addDefaultIndex(XmlTransaction &txn, const std::string &index,
			     XmlUpdateContext &context);
	void deleteDefaultIndex(XmlTransaction &txn, const std::string &index,
				XmlUpdateContext &context);
	void replaceDefaultIndex(XmlTransaction &txn, const std::string &index,
				 XmlUpdateContext &context);

	void setAutoIndexing(XmlTransaction &txn, bool value, XmlUpdateContext &context);
	bool getAutoIndexing(XmlTransaction &txn) const;

	/** @name Document Retrieval Methods */

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlDocument getDocument(const std::string &name, u_int32_t flags = 0);

	/** @name Document Retrieval Methods (Transacted) */

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlDocument getDocument(XmlTransaction &txn, const std::string &name,
				u_int32_t flags = 0);

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlValue getNode(const std::string &nodeHandle, u_int32_t flags = 0);

	/** @name Node Retrieval Methods (Transacted) */

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlValue getNode(XmlTransaction &txn,
			 const std::string &nodeHandle,
			 u_int32_t flags = 0);

	/** @name Document Altering Methods */

	/// Valid flags: DBXML_GEN_NAME
	void putDocument(XmlDocument &document, XmlUpdateContext &context,
			 u_int32_t flags = 0);

	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(const std::string &name,
				XmlInputStream *adopted_input,
				XmlUpdateContext &context, u_int32_t flags = 0);
	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(const std::string &name,
				const std::string &contents,
				XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(const std::string &name,
				XmlEventReader &reader,
				XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns an XmlEventWriter used to create content.
	 * The close() method *must* be called on this object to
	 * complete the operation.
	 *
	 * Valid flags: DBXML_GEN_NAME
	 */
	XmlEventWriter &putDocumentAsEventWriter(XmlDocument &document,
						 XmlUpdateContext &context,
						 u_int32_t flags = 0);

	void deleteDocument(const std::string &name,
			    XmlUpdateContext &context);

	void deleteDocument(XmlDocument &document,
			    XmlUpdateContext &context);

	void updateDocument(XmlDocument &document,
			    XmlUpdateContext &context);

	/** @name Document Altering Methods (Transacted) */

	/// Valid flags: DBXML_GEN_NAME
	void putDocument(XmlTransaction &txn, XmlDocument &document,
			 XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(XmlTransaction &txn, const std::string &name,
				const std::string &contents,
				XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(XmlTransaction &txn, const std::string &name,
				XmlInputStream *adopted_input,
				XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns the name used to store the document
	 * Valid flags: DBXML_GEN_NAME
	 */
	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				XmlEventReader &reader,
				XmlUpdateContext &context, u_int32_t flags = 0);

	/**
	 * Returns an XmlEventWriter used to create content.
	 * The close() method *must* be called on this object to
	 * complete the operation.
	 *
	 * Valid flags: DBXML_GEN_NAME
	 */
	XmlEventWriter &putDocumentAsEventWriter(XmlTransaction &txn,
						 XmlDocument &document,
						 XmlUpdateContext &context,
						 u_int32_t flags = 0);

	void deleteDocument(XmlTransaction &txn, const std::string &name,
			    XmlUpdateContext &context);

	void deleteDocument(XmlTransaction &txn, XmlDocument &document,
			    XmlUpdateContext &context);

	void updateDocument(XmlTransaction &txn, XmlDocument &document,
			    XmlUpdateContext &context);

	/** @name Convenience methods for document */
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 *  DBXML_LAZY_DOCS is defaulted ON, if flags are 0
	 */
	XmlResults getAllDocuments(u_int32_t flags);
	XmlResults getAllDocuments(XmlTransaction &txn,
				   u_int32_t flags);
	size_t getNumDocuments();
	size_t getNumDocuments(XmlTransaction &txn);

	/** @name Statistics Lookup Methods */
	XmlStatistics lookupStatistics(const std::string &uri,
				       const std::string &name,
				       const std::string &index,
				       const XmlValue &value = XmlValue());
	XmlStatistics lookupStatistics(const std::string &uri,
				       const std::string &name,
				       const std::string &parent_uri,
				       const std::string &parent_name,
				       const std::string &index,
				       const XmlValue &value = XmlValue());

	/** @name Statistics Lookup Methods (Transacted) */
	XmlStatistics lookupStatistics(XmlTransaction &txn,
				       const std::string &uri,
				       const std::string &name,
				       const std::string &index,
				       const XmlValue &value = XmlValue());
	XmlStatistics lookupStatistics(XmlTransaction &txn,
				       const std::string &uri,
				       const std::string &name,
				       const std::string &parent_uri,
				       const std::string &parent_name,
				       const std::string &index,
				       const XmlValue &value = XmlValue());

	/** @name Deprecated index lookup methods */
	
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 */
	XmlResults lookupIndex(XmlQueryContext &context,
			       const std::string &uri,
			       const std::string &name,
			       const std::string &index,
			       const XmlValue &value = XmlValue(),
			       u_int32_t flags = 0);
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 */
	XmlResults lookupIndex(XmlQueryContext &context,
			       const std::string &uri,
			       const std::string &name,
			       const std::string &parent_uri,
			       const std::string &parent_name,
			       const std::string &index,
			       const XmlValue &value = XmlValue(),
			       u_int32_t flags = 0);

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 */
	XmlResults lookupIndex(XmlTransaction &txn,
			       XmlQueryContext &context,
			       const std::string &uri,
			       const std::string &name,
			       const std::string &index,
			       const XmlValue &value = XmlValue(),
			       u_int32_t flags = 0);
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_REVERSE_ORDER
	 */
	XmlResults lookupIndex(XmlTransaction &txn,
			       XmlQueryContext &context,
			       const std::string &uri,
			       const std::string &name,
			       const std::string &parent_uri,
			       const std::string &parent_name,
			       const std::string &index,
			       const XmlValue &value = XmlValue(),
			       u_int32_t flags = 0);

	/** @name Private Methods (for internal use) */
	// @{

	XmlContainer(Container *container);
	operator Container *();
	operator Container &();

	void resetIndexUseStatistics() const;
	void logIndexUseStatistics() const;
	void close(); // not for public use
	//@}

private:
	class Container *container_;
};

}

#endif

