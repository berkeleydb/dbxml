%include "exception.i"
%include "typemaps.i"

%{
#include "db.h"
#include "dbxml/DbXml.hpp"
#include <errno.h>
#include <fstream>

/* compat w/pre-4.4 */
#ifndef DB_READ_COMMITTED
#define DB_READ_COMMITTED DB_DEGREE_2
#define DB_READ_UNCOMMITTED DB_DIRTY_READ
#endif

/* If these are not defined, there are no equivalents; just make things compile */
#ifndef DB_TXN_SNAPSHOT
#define DB_TXN_SNAPSHOT -1
#define DB_MULTIVERSION -1
#endif
%}

#if !defined(SWIGJAVA)
%{
using namespace DbXml;
%}
#endif

#if defined(SWIGJAVA)
%include "std_string.i"
%include "dbxml_java.i"
#elif defined(SWIGPYTHON)
%include "std_string.i"
%include "dbxml_python.i"
#elif defined(SWIGTCL8)
%include "std_string.i"
%include "dbxml_tcl.i"
#elif defined(SWIGPHP4)
%include "std_string.i"
%include "dbxml_php4.i"
#else
#error "Unknown SWIG target language"
#endif

%{

class XmlIndexDeclaration {
public:
	XmlIndexDeclaration() {}
	XmlIndexDeclaration(const std::string &uri, const std::string &name, const std::string &index) : uri(uri), name(name), index(index) {}
#ifndef SWIGJAVA	
	const std::string &get_uri() const { return uri; }
	const std::string &get_name() const { return name; }
	const std::string &get_index() const { return index; }
#endif
	std::string uri, name, index;
};

class XmlMetaData {
public:
	XmlMetaData() {}
	XmlMetaData(const std::string &uri, const std::string &name, const
		    XmlValue &value) : uri(uri), name(name), value(value) {}
	
	const std::string &get_uri() const { return uri; }
	const std::string &get_name() const { return name; }
	const XmlValue &get_value() const { return value; }
	std::string uri, name;
	XmlValue value;
};

%}

// DBXML_USEOVERLOADS -- defined when a language supports overloaded
// functions.  If defined, the "OVERLOAD_NAME" macro is a no-op
#if defined(SWIGJAVA) || defined(SWIGPYTHON) || defined(SWIGCSHARP) || defined(SWIGTCL8)
#define DBXML_USEOVERLOADS
#endif

#if defined(SWIGJAVA) || defined(SWIGPYTHON)
#define DBXML_DIRECTOR_CLASSES
#endif

 //typedef unsigned int u_int32_t;
typedef int int32_t;

class XmlManager;
#ifndef SWIGJAVA
class XmlDocument;
class XmlContainerConfig;
#endif
class XmlContainer;
class XmlIndexSpecification;
class XmlIndexLookup;
class XmlInputStream;
class XmlResults;
class XmlUpdateContext;
#ifndef SWIGJAVA
class XmlQueryContext;
class XmlValue;
#endif
class XmlQueryExpression;
class XmlTransaction;
#ifndef SWIGJAVA
class XmlMetaDataIterator;
#endif
class XmlStatistics;
class XmlEventReader;
class XmlEventWriter;
class XmlEventReaderToWriter;
#if defined(DBXML_DIRECTOR_CLASSES)
class XmlResolver;
class XmlExternalFunction;
#ifndef SWIGPYTHON
class XmlCompression;
#endif
class XmlArguments;
class XmlDebugListener;
class XmlStackFrame;
#endif


#if defined(DBXML_USEOVERLOADS)
#define OVERLOAD_NAME(n)
#else
#define OVERLOAD_NAME(n) %rename(n)
#endif

// SWIG will generate a class per enumeration by default

#ifndef SWIGJAVA
// For Java, this is done differently
enum {
	DB_CREATE, 
	DB_READ_UNCOMMITTED, 
	DB_DIRTY_READ,
	DB_EXCL, 
	DB_NOMMAP, 
	DB_RDONLY, 
	DB_THREAD,
	DB_READ_COMMITTED, 
	DB_DEGREE_2,
	DB_INIT_LOCK, 
	DB_INIT_LOG, 
	DB_INIT_MPOOL, 
	DB_INIT_TXN,
	DB_SALVAGE, 
	DB_AGGRESSIVE,
    	DB_TXN_SNAPSHOT,
	DB_MULTIVERSION
};

#endif


#ifdef SWIGJAVA
%javaconst(1);
#endif

// This directive will turn all instances of u_int32_t in
// this file into int for the purposes of typemaps and typing.
// For various reaons, as of SWIG 1.3.29, this works better,
// and should not cause any actual failures anywhere.
%apply int {u_int32_t};

//
// see DbXmlFwd.hpp for these enums
//
// Global flags
enum {
	DBXML_ADOPT_DBENV           = 0x00000001,
	DBXML_ALLOW_EXTERNAL_ACCESS = 0x00000002,
	DBXML_ALLOW_AUTO_OPEN       = 0x00000004,
	DBXML_ALLOW_VALIDATION      = 0x00100000,
	DBXML_TRANSACTIONAL         = 0x00200000,
	DBXML_CHKSUM                = 0x00400000,
	DBXML_ENCRYPT               = 0x00800000,
	DBXML_INDEX_NODES           = 0x01000000,
	DBXML_NO_INDEX_NODES        = 0x00010000,
	DBXML_STATISTICS            = 0x02000000,
	DBXML_NO_STATISTICS         = 0x04000000,
	DBXML_REVERSE_ORDER         = 0x00100000,
	DBXML_INDEX_VALUES          = 0x00200000,
	DBXML_CACHE_DOCUMENTS       = 0x00400000,
	DBXML_LAZY_DOCS             = 0x00800000,
	DBXML_DOCUMENT_PROJECTION   = 0x80000000,
	DBXML_NO_AUTO_COMMIT        = 0x00010000,
	DBXML_WELL_FORMED_ONLY      = 0x01000000,
	DBXML_GEN_NAME              = 0x02000000
};

// LogLevel
enum {
    LEVEL_NONE = 0x00000000,  ///< No debug levels are enabled.
    LEVEL_DEBUG = 0x00000001,  ///< Program execution tracing messages.
    LEVEL_INFO = 0x00000002,  ///< Informational, just for interest.
    LEVEL_WARNING = 0x00000004,  ///< For warnings, bad things but recoverable.
    LEVEL_ERROR = 0x00000008,  ///< For errors that can't be recovered from.
    LEVEL_ALL = 0xFFFFFFFF  ///< All debug levels are enabled.
};

// LogCategory
enum {
    CATEGORY_NONE = 0x00000000,
    CATEGORY_INDEXER = 0x00000001,
    CATEGORY_QUERY = 0x00000002,
    CATEGORY_OPTIMIZER = 0x00000004,
    CATEGORY_DICTIONARY = 0x00000008,
    CATEGORY_CONTAINER = 0x00000010,
    CATEGORY_NODESTORE = 0x00000020,
    CATEGORY_MANAGER =    0x00000040,
    CATEGORY_ALL = 0xFFFFFFFF
};

//
// These strings are reproduced from src/dbxml/XmlNamespace.cpp.
// Using them as direct constants is simpler (from a SWIG point of view)
// than trying to reference their C++ values on demand.  This means
// maintaining them together, if they change.
//
%constant const char *metaDataNamespace_uri =  "http://www.sleepycat.com/2002/dbxml";
%constant const char *metaDataNamespace_prefix = "dbxml";
%constant const char *metaDataName_name = "name";
%constant const char *metaDataName_root = "root";

#ifdef SWIGTCL8
/* Put these at global scope for swig */
%typemap(in, numinputs = 0) int *majorp, int *minorp, int *patchp %{ $1 = NULL; %}
const char *dbxml_version(int *majorp, int *minorp, int *patchp);
void setLogLevel(enum LogLevel level, bool enabled) {
		DbXml::setLogLevel(level, enabled)
}
void setLogCategory(enum LogCategory category, bool enabled) {
		DbXml::setLogCategory(category, enabled)	
}
#endif /* SWIGTCL8 */

/*
 * All the methods that return pointers to allocated memory.
 * Required so the memory is freed when the objects are deleted.
 */
%newobject XmlManager::createContainer(const std::string &);
%newobject XmlManager::createContainer(const std::string &, XmlContainerConfig);
%newobject XmlManager::createContainer(const std::string &, // name
				       XmlContainerConfig, //flags
				       enum XmlContainer::ContainerType,
				       int mode=0);      // mode
%newobject XmlManager::createContainer(XmlTransaction &, const std::string &);
%newobject XmlManager::createContainer(XmlTransaction &, const std::string &, XmlContainerConfig);
%newobject XmlManager::createContainer(XmlTransaction &, const std::string &,
				       XmlContainerConfig,  // flags
				       enum XmlContainer::ContainerType,
				       int mode=0);       // mode
%newobject XmlManager::openContainer(const std::string &);
%newobject XmlManager::openContainer(const std::string &, XmlContainerConfig);
%newobject XmlManager::openContainer(XmlTransaction &, const std::string &);
%newobject XmlManager::openContainer(XmlTransaction &, const std::string &,
				     XmlContainerConfig);
%newobject XmlManager::openContainer(XmlTransaction &, const std::string &,
				     XmlContainerConfig, 
				     enum XmlContainer::ContainerType,
				     int mode = 0);
%newobject XmlManager::openContainer(const std::string &,
				     XmlContainerConfig, 
				     enum XmlContainer::ContainerType,
				     int mode = 0);
%newobject XmlManager::createDocument();
//For backwards compatibility with u_int32_t flags
%newobject XmlManager::createContainer(const std::string &, // name
				       u_int32_t, //flags
				       enum XmlContainer::ContainerType=
				       XmlContainer::NodeContainer,
				       int mode=0);      // mode
%newobject XmlManager::createContainer(XmlTransaction &, const std::string &,
				       u_int32_t,  // flags
				       enum XmlContainer::ContainerType=
				       XmlContainer::NodeContainer,
				       int mode=0);       // mode
%newobject XmlManager::openContainer(const std::string &, u_int32_t);
%newobject XmlManager::openContainer(XmlTransaction &, const std::string &,
				     u_int32_t);
%newobject XmlManager::openContainer(XmlTransaction &, const std::string &,
				     u_int32_t, 
				     enum XmlContainer::ContainerType,
				     int mode = 0);
%newobject XmlManager::openContainer(const std::string &,
				     u_int32_t, 
				     enum XmlContainer::ContainerType,
				     int mode = 0);

%newobject XmlManager::createQueryContext(enum XmlQueryContext::ReturnType,
					  enum XmlQueryContext::EvaluationType);
%newobject XmlManager::createQueryContext(enum XmlQueryContext::ReturnType);
%newobject XmlManager::createQueryContext();

%newobject XmlManager::createUpdateContext();

%newobject XmlManager::prepare(const std::string &, XmlQueryContext &);
%newobject XmlManager::prepare(XmlTransaction &, const std::string &,
				XmlQueryContext &);
%newobject XmlManager::query(const std::string &, XmlQueryContext &, u_int32_t);
%newobject XmlManager::query(XmlTransaction &, const std::string &,
			      XmlQueryContext &, u_int32_t);
%newobject XmlManager::query(const std::string &, XmlQueryContext &);
%newobject XmlManager::query(XmlTransaction &, const std::string &,
			      XmlQueryContext &);
%newobject XmlManager::createResults();


%newobject XmlManager::createTransaction(DB_TXN *);
#ifndef SWIGJAVA
%newobject XmlManager::createTransaction();
%newobject XmlManager::createTransaction(u_int32_t);
%newobject XmlTransaction::createChild(u_int32_t);
%newobject XmlTransaction::createChild();
#endif

%newobject XmlContainer::getIndexSpecification() const;
%newobject XmlContainer::getIndexSpecification(XmlTransaction&) const;
%newobject XmlContainer::getIndexSpecification(XmlTransaction&, 
					       u_int32_t flags) const;
#ifdef SWIGJAVA
%newobject XmlManager::createIndexLookupInternal(
#else
%newobject XmlManager::createIndexLookup(
#endif
	XmlContainer &cont,
	const std::string &uri, 
	const std::string &name,
	const std::string &index,
	const XmlValue &value = XmlValue(),
	enum XmlIndexLookup::Operation op = XmlIndexLookup::NONE);

%newobject XmlIndexLookup::execute(
	XmlQueryContext &ctx, u_int32_t flags = 0) const;
%newobject XmlIndexLookup::execute(
	XmlTransaction &, XmlQueryContext &ctx, u_int32_t flags = 0) const;
	
#ifndef SWIGJAVA
%newobject XmlContainer::getDocument(const std::string&);
%newobject XmlContainer::getDocument(XmlTransaction&, const std::string&);
#endif
%newobject XmlContainer::getDocument(const std::string&, u_int32_t);
%newobject XmlContainer::getDocument(XmlTransaction&, const std::string&,
				     u_int32_t);
#ifndef SWIGJAVA
%newobject XmlContainer::getNode(const std::string&, u_int32_t);
%newobject XmlContainer::getNode(XmlTransaction&, const std::string&);
%newobject XmlContainer::getNode(const std::string&);
%newobject XmlContainer::getNode(XmlTransaction&, const std::string&,
				 u_int32_t);
#endif
%newobject XmlContainer::getAllDocuments(u_int32_t);
%newobject XmlContainer::getAllDocuments(XmlTransaction&, u_int32_t);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &,
				     const XmlValue &, u_int32_t);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &,
				     const XmlValue &);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const XmlValue &, u_int32_t);
%newobject XmlContainer::lookupIndex(XmlQueryContext &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const XmlValue &);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &,
				     u_int32_t);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &,
				     u_int32_t);
%newobject XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &);
%newobject XmlContainer::lookupStatistics(const std::string &,
					  const std::string &, const std::string &);
%newobject XmlContainer::lookupStatistics(const std::string &,
					  const std::string &, const std::string &,
					  const XmlValue &);
%newobject XmlContainer::lookupStatistics(const std::string &,
					  const std::string &, const std::string &,
					  const std::string &, const std::string &);
%newobject XmlContainer::lookupStatistics(const std::string &,
					  const std::string &, const std::string &,
					  const std::string &, const std::string &,
					  const XmlValue &);
%newobject XmlContainer::lookupStatistics(XmlTransaction &,
				     const std::string &, const std::string &,
				     const std::string &);
%newobject XmlContainer::lookupStatistics(XmlTransaction &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &);
%newobject XmlContainer::lookupStatistics(XmlTransaction &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &);
%newobject XmlContainer::lookupStatistics(XmlTransaction &,
				     const std::string &, const std::string &,
				     const std::string &, const std::string &,
				     const std::string &, const XmlValue &);
#ifndef SWIGJAVA
%newobject XmlDocument::getContent() const;
%newobject XmlDocument::getMetaDataIterator() const;
%newobject XmlDocument::getContentAsXmlInputStream() const;
#endif
%newobject XmlQueryContext::getVariableValue(const std::string&) const;

%newobject XmlQueryExpression::execute(XmlQueryContext &, u_int32_t) const;
%newobject XmlQueryExpression::execute(XmlQueryContext &) const;
%newobject XmlQueryExpression::execute(const XmlValue &, XmlQueryContext &,
				       u_int32_t) const;
%newobject XmlQueryExpression::execute(const XmlValue &, XmlQueryContext &) const;
%newobject XmlQueryExpression::execute(XmlTransaction &, XmlQueryContext &,
				       u_int32_t) const;
%newobject XmlQueryExpression::execute(XmlTransaction &, XmlQueryContext &) const;
%newobject XmlQueryExpression::execute(XmlTransaction &, const XmlValue &,
				       XmlQueryContext &, u_int32_t) const;
%newobject XmlQueryExpression::execute(XmlTransaction &, const XmlValue &,
				       XmlQueryContext &) const;


#if defined(SWIGPYTHON) || defined(SWIGPHP4)
%newobject XmlResults::next();
%newobject XmlResults::previous();
%newobject XmlResults::peek();
#endif

#ifdef SWIGTCL8
%newobject XmlValue::asDocument(const XmlQueryContext*) const;
#endif

#ifndef SWIGJAVA
%newobject XmlValue::getParentNode() const;
%newobject XmlValue::getFirstChild() const;
%newobject XmlValue::getLastChild() const;
%newobject XmlValue::getPreviousSibling() const;
%newobject XmlValue::getNextSibling() const;
%newobject XmlValue::getAttributes() const;
%newobject XmlValue::getOwnerElement() const;
%newobject XmlValue::asBinary() const;
%newobject XmlValue::loadNodeHandle() const;
#endif
%newobject XmlIndexSpecification::find(const std::string&, const std::string&);
%newobject XmlIndexSpecification::next();
#ifndef SWIGJAVA
%newobject XmlMetaDataIterator::next();
#endif
#if defined(DBXML_DIRECTOR_CLASSES)
%newobject XmlResolver::resolveSchema(XmlTransaction*, XmlManager&, const std::string&, const std::string&) const;
%newobject XmlResolver::resolveEntity(XmlTransaction*, XmlManager&, const std::string&, const std::string&) const;
%newobject XmlResolver::resolveModule(XmlTransaction*, XmlManager&, const std::string&, const std::string&) const;
%newobject XmlResolver::resolveExternalFunction(XmlTransaction *, XmlManager &, const std::string &, const std::string &, size_t numberOfArgs) const;
%newobject XmlStackFrame::query(const std::string &) const;
#endif

#ifdef SWIGJAVA
%nodefault XmlManager;
#endif

class XmlManager
{
public:

#ifndef SWIGJAVA
	XmlManager();
#endif	
#ifndef SWIGJAVA
OVERLOAD_NAME(XmlManagerWithFlags)
	XmlManager(u_int32_t flags);
#endif
OVERLOAD_NAME(XmlManagerFromEnv)	
	XmlManager(DB_ENV *dbEnv, u_int32_t flags);
	~XmlManager();

#ifndef SWIGJAVA
	u_int32_t getFlags() const;
	XmlContainerConfig getDefaultContainerConfig();
	void setDefaultContainerConfig(XmlContainerConfig &config);
	void setDefaultContainerType(enum XmlContainer::ContainerType type);
	enum XmlContainer::ContainerType getDefaultContainerType() const;
	void setDefaultPageSize(u_int32_t pageSize);
	u_int32_t getDefaultPageSize() const;
	void setDefaultSequenceIncrement(u_int32_t incr);
	u_int32_t getDefaultSequenceIncrement() const;
#endif

	const std::string &getHome() const;

#if defined(DBXML_DIRECTOR_CLASSES)
#ifndef SWIGJAVA
	void registerResolver(const XmlResolver &resolver);
#ifndef SWIGPYTHON
	void registerCompression(const char *name, XmlCompression &compression);
#endif
#endif
#endif

	int getImplicitTimezone() const;
	void setImplicitTimezone(int tz);

	int existsContainer(const std::string &name);
	void removeContainer(const std::string &name);
OVERLOAD_NAME(removeContainerWithTxn)
	void removeContainer(XmlTransaction &txn, const std::string &name);
	void renameContainer(const std::string &oldName,
			     const std::string &newName);
OVERLOAD_NAME(renameContainerWithTxn)
	void renameContainer(XmlTransaction &txn, const std::string &oldName,
			     const std::string &newName);
#ifndef SWIGJAVA
	void upgradeContainer(const std::string &name, XmlUpdateContext &uc);
	OVERLOAD_NAME(compactContainerWithConfig)
	void compactContainer(const std::string &name, XmlUpdateContext &uc,
			      XmlContainerConfig flags);
	OVERLOAD_NAME(compactContainerWithTxnConfig)
	void compactContainer(XmlTransaction &txn, const std::string &name,
			      XmlUpdateContext &uc, XmlContainerConfig flags);
	OVERLOAD_NAME(truncateContainerWithConfig)
	void truncateContainer(const std::string &name, XmlUpdateContext &uc,
			       XmlContainerConfig flags);
	OVERLOAD_NAME(truncateContainerWithTxnConfig)
	void truncateContainer(XmlTransaction &txn, const std::string &name,
			       XmlUpdateContext &uc, XmlContainerConfig flags);
	OVERLOAD_NAME(reindexContainerWithConfig)
	void reindexContainer(const std::string &name, XmlUpdateContext &uc,
			      XmlContainerConfig flags);
	OVERLOAD_NAME(reindexContainerWithTxnConfig)
	void reindexContainer(XmlTransaction &txn, const std::string &name,
			      XmlUpdateContext &uc, XmlContainerConfig flags);
			      
//Begin functions for backwards compatability with u_int32_t flags
%extend {
	void setDefaultContainerFlags(u_int32_t flags)
	{
		self->setDefaultContainerFlags(flags);
	}
	u_int32_t getDefaultContainerFlags() const
	{
		return self->getDefaultContainerFlags();
	}
	void compactContainer(const std::string &name, XmlUpdateContext &uc,
			      u_int32_t flags = 0)
	{
		self->compactContainer(name, uc, flags);
	}
	void compactContainer(XmlTransaction &txn, const std::string &name,
			      XmlUpdateContext &uc, u_int32_t flags = 0)
	{
		self->compactContainer(txn, name, uc, flags);
	}
	void truncateContainer(const std::string &name, XmlUpdateContext &uc,
			       u_int32_t flags = 0)
	{
		self->truncateContainer(name, uc, flags);
	}
	void truncateContainer(XmlTransaction &txn, const std::string &name,
			       XmlUpdateContext &uc, u_int32_t flags = 0)
	{
		self->truncateContainer(txn, name, uc, flags);
	}
	void reindexContainer(const std::string &name, XmlUpdateContext &uc,
			      u_int32_t flags = 0)
	{
		self->reindexContainer(name, uc, flags);
	}
	void reindexContainer(XmlTransaction &txn, const std::string &name,
			      XmlUpdateContext &uc, u_int32_t flags = 0)
	{
		self->reindexContainer(name, uc, flags);
	}
}
//end functions for backwards compatability with u_int32_t flags
#endif
	%extend {

	XmlInputStream* createStdInInputStream() const {
		return self->createStdInInputStream();
	}

	XmlInputStream* createLocalFileInputStream(
		const std::string &filename) const {
		return self->createLocalFileInputStream(filename);
	}

	// The following method does not work correctly with Java
	// because the JNI layer copies the Java String, then
	// releases it upon return, leaving an invalid buffer.
	// It appears to work for Python
	// 
	XmlInputStream* createMemBufInputStream(
		const char *bytes, const unsigned int count,
		const char *id, const bool adopt = false) const {
		return self->createMemBufInputStream(bytes,
						     count,
						     id, adopt);
	}
	// This form *always* adopts the buffer, copied or not
	// It is supplied for use by Java, primarily
OVERLOAD_NAME(createMemBufInputStreamCopy)	
	XmlInputStream* createMemBufInputStream(
		const char *bytes, const unsigned int count,
		const bool copyBuffer) const {
		return self->createMemBufInputStream(bytes,
						     count,
						     copyBuffer);
	}

OVERLOAD_NAME(createURLInputStreamWithPublicID)
	XmlInputStream* createURLInputStream(
		const std::string &baseId, 
		const std::string &systemId, 
		const std::string &publicId) const {
		return self->createURLInputStream(baseId, systemId, publicId);
	}
	
	XmlInputStream* createURLInputStream(
		const std::string &baseId, 
		const std::string &systemId) const {
		return self->createURLInputStream(baseId, systemId);
	}

#ifndef SWIGJAVA // these are implemented in pure java
	XmlContainer *createContainer(const std::string &name) {
		return new XmlContainer(self->createContainer(name));
	}
OVERLOAD_NAME(createContainerWithTxn)
	XmlContainer *createContainer(
		XmlTransaction &txn, const std::string &name) {
		return new XmlContainer(self->createContainer(txn, name));
	}
OVERLOAD_NAME(openContainerWithTxn)
	XmlContainer *openContainer(XmlTransaction &txn,
				     const std::string &name) {
		return new XmlContainer(self->openContainer(txn, name));
	}
	XmlContainer *openContainer(const std::string &name) {
		return new XmlContainer(self->openContainer(name));
	}
	
OVERLOAD_NAME(createContainerWithConfig)
	XmlContainer *createContainer(
		const std::string &name, XmlContainerConfig config) {
		return new XmlContainer(
			self->createContainer(name, config));
	 }

OVERLOAD_NAME(createContainerWithTxnConfig)
	XmlContainer *createContainer(
		XmlTransaction &txn, const std::string &name, XmlContainerConfig config) {
		return new XmlContainer(self->createContainer(txn, name, config));
        }


OVERLOAD_NAME(createContainerWithConfigType)
	XmlContainer *createContainer(
		const std::string &name, XmlContainerConfig flags,
		enum XmlContainer::ContainerType type,
		int mode = 0) {
		return new XmlContainer(
			self->createContainer(name, flags, type, mode));
	 }

OVERLOAD_NAME(createContainerWithTxnConfigType)
	XmlContainer *createContainer(
		XmlTransaction &txn, const std::string &name, XmlContainerConfig flags,
		enum XmlContainer::ContainerType type,
		int mode = 0) {
		return new XmlContainer(self->createContainer(txn, name, flags,
							      type, mode));
        }

OVERLOAD_NAME(openContainerWithConfig)
	XmlContainer *openContainer(const std::string &name, XmlContainerConfig flags) {
		return new XmlContainer(self->openContainer(name, flags));
        }
OVERLOAD_NAME(openContainerWithTxnAndConfig)
	XmlContainer *openContainer(XmlTransaction &txn,
				     const std::string &name, XmlContainerConfig flags) {
		return new XmlContainer(self->openContainer(txn, name, flags));
        }
OVERLOAD_NAME(openContainerWithTypeConfig)
	XmlContainer *openContainer(const std::string &name,
				    XmlContainerConfig flags, 
				    enum XmlContainer::ContainerType type,
				    int mode = 0) {
	    return new XmlContainer(self->openContainer(name,
						    flags, type, mode));
        }
OVERLOAD_NAME(openContainerWithTxnTypeConfig)
	XmlContainer *openContainer(XmlTransaction &txn,
				    const std::string &name,
				    XmlContainerConfig flags, 
				    enum XmlContainer::ContainerType type,
				    int mode = 0) {
	    return new XmlContainer(self->openContainer(txn, name,
						    flags, type, mode));
        }
//These functions are defined for backwards compatibility with u_int32_t flags

OVERLOAD_NAME(createContainerWithFlags)
	XmlContainer *createContainer(
		const std::string &name, u_int32_t flags,
		enum XmlContainer::ContainerType type =
		XmlContainer::NodeContainer,
		int mode = 0) {
		return new XmlContainer(
			self->createContainer(name, flags, type, mode));
	 }

OVERLOAD_NAME(createContainerWithTxnAndFlags)
	XmlContainer *createContainer(
		XmlTransaction &txn, const std::string &name, u_int32_t flags,
		enum XmlContainer::ContainerType type =
		XmlContainer::NodeContainer,
		int mode = 0) {
		return new XmlContainer(self->createContainer(txn, name, flags,
							      type, mode));
        }

OVERLOAD_NAME(openContainerWithFlags)
	XmlContainer *openContainer(const std::string &name, u_int32_t flags) {
		XmlContainerConfig config(flags);
		config.setContainerType(self->getDefaultContainerType());
		config.setMode(self->getDefaultContainerConfig().getMode());
		config.setPageSize(self->getDefaultPageSize());
		config.setSequenceIncrement(self->getDefaultSequenceIncrement());
		return new XmlContainer(self->openContainer(name, config));
        }
OVERLOAD_NAME(openContainerWithTxnAndFlags)
	XmlContainer *openContainer(XmlTransaction &txn,
				     const std::string &name, u_int32_t flags) {
		XmlContainerConfig config(flags);
		config.setContainerType(self->getDefaultContainerType());
		config.setMode(self->getDefaultContainerConfig().getMode());
		config.setPageSize(self->getDefaultPageSize());
		config.setSequenceIncrement(self->getDefaultSequenceIncrement());
		return new XmlContainer(self->openContainer(txn, name, config));
        }
OVERLOAD_NAME(openContainerWithType)
	XmlContainer *openContainer(const std::string &name,
				    u_int32_t flags, 
				    enum XmlContainer::ContainerType type,
				    int mode = 0) {
	    return new XmlContainer(self->openContainer(name,
						    flags, type, mode));
        }
OVERLOAD_NAME(openContainerWithTxnAndType)
	XmlContainer *openContainer(XmlTransaction &txn,
				    const std::string &name,
				    u_int32_t flags, 
				    enum XmlContainer::ContainerType type,
				    int mode = 0) {
	    return new XmlContainer(self->openContainer(txn, name,
						    flags, type, mode));
        }
#endif //end backwards compatible functions for u_int32_t flags
}
%extend {
	void dumpContainer(const std::string &name, const char *filename) {
		std::ofstream out(filename);
		self->dumpContainer(name, &out);
		out.close();
	}
#ifndef SWIGJAVA
	void loadContainer(const std::string &name, const char *filename,
			   XmlUpdateContext &uc) {
		std::ifstream in(filename);
		unsigned long lineno = 0;
		self->loadContainer(name, &in, &lineno, uc);
		in.close();
	}
#endif
	void verifyContainer(const std::string &name, const char *filename,
			     u_int32_t flags = 0) {
		std::ofstream out;
		if (flags & DB_SALVAGE)
			out.open(filename);
		self->verifyContainer(name, &out, flags);
		if (flags & DB_SALVAGE)
			out.close();
	}
#ifndef SWIGJAVA
	XmlDocument *createDocument() {
		return new XmlDocument(self->createDocument());
	}
	XmlQueryContext *createQueryContext(
		enum XmlQueryContext::ReturnType rt,
		enum XmlQueryContext::EvaluationType et) {
		return new XmlQueryContext(self->createQueryContext(rt, et));
	}
OVERLOAD_NAME(createQueryContextDefaultAll)
	XmlQueryContext *createQueryContext() {
		return new XmlQueryContext(self->createQueryContext(
			XmlQueryContext::LiveValues, XmlQueryContext::Eager));
	}

OVERLOAD_NAME(createQueryContextDefaultEvalType)
	XmlQueryContext *createQueryContext(
		enum XmlQueryContext::ReturnType rt) {
		return new XmlQueryContext(self->createQueryContext(rt, XmlQueryContext::Eager));
	}
	XmlUpdateContext *createUpdateContext() {
		return new XmlUpdateContext(self->createUpdateContext());
	}
#endif
	XmlQueryExpression *prepare(const std::string &query,
				    XmlQueryContext &context) {
		return new XmlQueryExpression(self->prepare(query, context));
	}
OVERLOAD_NAME(prepareWithTxn)
	 XmlQueryExpression *prepare(XmlTransaction &txn,
				     const std::string &query,
				     XmlQueryContext &context){
		return new XmlQueryExpression(self->prepare(
						      txn,query, context));
	}
    	  XmlResults *query(const std::string &query, XmlQueryContext &context,
		  	  u_int32_t flags) {
		return new XmlResults(self->query(query, context, flags));
	}
OVERLOAD_NAME(queryWithTxn)
	XmlResults *query(XmlTransaction &txn, const std::string &query,
			  XmlQueryContext &context, u_int32_t flags) {
		return new XmlResults(self->query(txn, query, context, flags));
	}

OVERLOAD_NAME(queryDefaultFlags)
	XmlResults *query(const std::string &query, XmlQueryContext &context) {
		return new XmlResults(self->query(query, context, 0));
	}
OVERLOAD_NAME(queryWithTxnDefaultFlags)
	XmlResults *query(XmlTransaction &txn, const std::string &query,
			  XmlQueryContext &context) {
		return new XmlResults(self->query(txn, query, context, 0));
	}


	XmlResults *createResults() {
		return new XmlResults(self->createResults());
	}


OVERLOAD_NAME(createTransactionFromDbTxn)
	XmlTransaction *createTransaction(DB_TXN *toAdopt) {
		return new XmlTransaction(self->createTransaction(toAdopt));
        }
#ifndef SWIGJAVA 
	XmlTransaction *createTransaction(u_int32_t flags) {
		return new XmlTransaction(self->createTransaction(flags));
	}
	
OVERLOAD_NAME(createTransactionDefaultFlags)
	XmlTransaction *createTransaction() {
		return new XmlTransaction(self->createTransaction((u_int32_t)0));
        }
#endif		
#ifdef SWIGJAVA
          XmlIndexLookup *createIndexLookupInternal(
#else		
        XmlIndexLookup *createIndexLookup(
#endif
		XmlContainer &cont,
		const std::string &uri, const std::string &name,
		const std::string &index,
		const XmlValue &value = XmlValue(),
		enum XmlIndexLookup::Operation op = XmlIndexLookup::NONE) {
		return new XmlIndexLookup(self->createIndexLookup(cont, uri,name,
								  index, value,
								  op));
	}

#ifndef SWIGTCL8
// These methods are at global scope (namespace DbXml)
// in C++ and in Tcl.
	static void setLogLevel(enum LogLevel level, bool enabled) {
		DbXml::setLogLevel(level, enabled);
	}

	static void setLogCategory(enum LogCategory category, bool enabled) {
		DbXml::setLogCategory(category, enabled);
	}
	static int get_version_major() {
		int major;
		(void)dbxml_version(&major, NULL, NULL);
		return major;
	}

	static int get_version_minor() {
		int minor;
		(void)dbxml_version(NULL, &minor, NULL);
		return minor;
	}

	static int get_version_patch() {
		int patch;
		(void)dbxml_version(NULL, NULL, &patch);
		return patch;
	}

	static const char *get_version_string() {
		return dbxml_version(NULL, NULL, NULL);
	}
#endif /* !SWIGTCL8 */
} /* extend */
};

class XmlIndexLookup
{
public:
	enum // Operation
	{
		NONE,
		EQ,
		GT,
		GTE,
		LT,
		LTE
	};
#ifndef SWIGJAVA
	XmlIndexLookup();
	XmlIndexLookup(const XmlIndexLookup &o);
#endif
	~XmlIndexLookup();
	bool isNull() const;
	const std::string &getIndex() const;
	void setIndex(const std::string &index);

	const std::string &getNodeURI() const;
	const std::string &getNodeName() const;
	void setNode(const std::string &uri, const std::string &name);

	const std::string &getParentURI() const;
	const std::string &getParentName() const;
	void setParent(const std::string &uri, const std::string &name);
#ifndef SWIGJAVA
	const XmlValue &getLowBoundValue() const;
#endif
	enum XmlIndexLookup::Operation getLowBoundOperation() const;
#ifdef SWIGJAVA
%rename(setLowBoundInternal) setLowBound;
	void setLowBound(const XmlValue &value,
			 enum XmlIndexLookup::Operation op);
#else
	void setLowBound(const XmlValue &value,
			 enum XmlIndexLookup::Operation op);
#endif
#ifndef SWIGJAVA
	const XmlValue &getHighBoundValue() const;
#endif
	enum XmlIndexLookup::Operation getHighBoundOperation() const;
#ifdef SWIGJAVA
%rename(setHighBoundInternal) setHighBound;
	void setHighBound(const XmlValue &value,
			  enum XmlIndexLookup::Operation op);
#else
	void setHighBound(const XmlValue &value,
			  enum XmlIndexLookup::Operation op);
#endif
#ifndef SWIGJAVA	  
	const XmlContainer &getContainer() const;
	void setContainer(XmlContainer &container);
#else
	%rename(setContainerInternal) setContainer;
	void setContainer(XmlContainer &container);
#endif
%extend {
	XmlResults *execute(XmlQueryContext &context,
			    u_int32_t flags = 0) const {
		return new XmlResults(self->execute(context, flags));
	}
	OVERLOAD_NAME(executeWithTxn)
	XmlResults *execute(XmlTransaction &txn, XmlQueryContext &context,
			   u_int32_t flags = 0) const {
		return new XmlResults(self->execute(txn, context, flags));
	}
}

};

#ifdef SWIGJAVA
%ignore putDocument(const std::string &name, XmlEventReader &reader, XmlUpdateContext &context);
%ignore putDocument(XmlTransaction &txn, const std::string &name, XmlEventReader &reader, XmlUpdateContext &context);
#endif

class XmlContainer
{
public:
	enum // ContainerType
	{
		WholedocContainer,
		NodeContainer
	};
#ifndef SWIGJAVA
	XmlContainer();
#endif
	XmlContainer(const XmlContainer &o);
	~XmlContainer();

	void sync();
#ifndef SWIGJAVA
	void close();
	u_int32_t getFlags() const;
#endif SWIGJAVA

	bool addAlias(const std::string &alias);
	bool removeAlias(const std::string &alias);
#ifndef SWIGJAVA	
	XmlManager &getManager();
#endif
	const std::string &getName() const;
	enum XmlContainer::ContainerType getContainerType() const;
	bool getIndexNodes() const;
	u_int32_t getPageSize() const;

	bool getAutoIndexing() const;
OVERLOAD_NAME(getAutoIndexingWithTxn)
	bool getAutoIndexing(XmlTransaction &txn) const;

#ifndef SWIGJAVA
	XmlContainerConfig getContainerConfig() const;
	void setAutoIndexing(bool value, XmlUpdateContext &uc);

	void setIndexSpecification(const XmlIndexSpecification &index,
				   XmlUpdateContext &uc);
	void addIndex(const std::string &uri, const std::string &name,
		      const std::string &index, XmlUpdateContext &uc);
OVERLOAD_NAME(addIndexWithTypes)
	void addIndex(const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type indexType,
		      enum XmlValue::Type syntaxType, 
		      XmlUpdateContext &uc);
	void deleteIndex(const std::string &uri, const std::string &name,
			 const std::string &index, XmlUpdateContext &uc);
	void replaceIndex(const std::string &uri, const std::string &name,
			  const std::string &index, XmlUpdateContext &uc);

	void addDefaultIndex(const std::string &index, XmlUpdateContext &uc);
	void deleteDefaultIndex(const std::string &index, XmlUpdateContext &uc);
	void replaceDefaultIndex(const std::string &index, XmlUpdateContext &uc);

OVERLOAD_NAME(setAutoIndexingWithTxn)
	void setAutoIndexing(XmlTransaction &txn, bool value, XmlUpdateContext &uc);

OVERLOAD_NAME(setIndexSpecificationWithTxn)
	void setIndexSpecification(XmlTransaction &txn,
				   const XmlIndexSpecification &index,
				   XmlUpdateContext &uc);
OVERLOAD_NAME(addIndexWithTxn)
	void addIndex(XmlTransaction &txn, const std::string &uri,
		      const std::string &name, const std::string &index,
		      XmlUpdateContext &uc);
OVERLOAD_NAME(addIndexWithTypesAndTxn)
	void addIndex(XmlTransaction &txn,
		      const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type indexType,
		      enum XmlValue::Type syntaxType, 
		      XmlUpdateContext &uc);
OVERLOAD_NAME(deleteIndexWithTxn)
	void deleteIndex(XmlTransaction &txn, const std::string &uri,
			 const std::string &name, const std::string &index,
			 XmlUpdateContext &uc);
OVERLOAD_NAME(replaceIndexWithTxn)
	void replaceIndex(XmlTransaction &txn, const std::string &uri,
			  const std::string &name, const std::string &index,
			  XmlUpdateContext &uc);

OVERLOAD_NAME(addDefaultIndexWithTxn)
	void addDefaultIndex(XmlTransaction &txn, const std::string &index,
			     XmlUpdateContext &uc);
OVERLOAD_NAME(deleteDefaultIndexWithTxn)
	void deleteDefaultIndex(XmlTransaction &txn, const std::string &index,
				XmlUpdateContext &uc);
OVERLOAD_NAME(replaceDefaultIndexWithTxn)
	void replaceDefaultIndex(XmlTransaction &txn, const std::string &index,
				 XmlUpdateContext &uc);

	void putDocument(XmlDocument &document, XmlUpdateContext &context,
			 u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentAsString)
	std::string putDocument(const std::string &name, const std::string &contents,
				XmlUpdateContext &context, u_int32_t flags = 0);
OVERLOAD_NAME(putDocumentAsEventReader)
        std::string putDocument(const std::string &name, XmlEventReader &reader,
				XmlUpdateContext &context, u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentAsEventWriter)	
        XmlEventWriter &putDocumentAsEventWriter(XmlDocument &document,
						 XmlUpdateContext &context,
						 u_int32_t flags = 0);
	void deleteDocument(XmlDocument &document,
			    XmlUpdateContext &context);

OVERLOAD_NAME(deleteDocumentByName)
	void deleteDocument(const std::string &name,
			    XmlUpdateContext &context);

	void updateDocument(XmlDocument &document,
			    XmlUpdateContext &context);

OVERLOAD_NAME(putDocumentAsInputSource)
	std::string putDocument(const std::string &name,
				XmlInputStream *input,
				XmlUpdateContext &context,
				u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentAsInputSourceWithTxn)
	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				XmlInputStream *input,
				XmlUpdateContext &context,
				u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentWithTxn)
	void putDocument(XmlTransaction &txn, XmlDocument &document,
			 XmlUpdateContext &context, u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentAsStringWithTxn)
	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				const std::string &contents,
				XmlUpdateContext &context,
				u_int32_t flags = 0);
OVERLOAD_NAME(putDocumentAsEventReaderWithTxn)
	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				XmlEventReader &reader,
				XmlUpdateContext &context,
				u_int32_t flags = 0);

OVERLOAD_NAME(putDocumentAsEventWriterWithTxn)	
        XmlEventWriter &putDocumentAsEventWriter(XmlTransaction &txn,
						 XmlDocument &document,
						 XmlUpdateContext &context,
						 u_int32_t flags = 0);
OVERLOAD_NAME(deleteDocumentWithTxn)
	void deleteDocument(XmlTransaction &txn, XmlDocument &document,
			    XmlUpdateContext &context);

OVERLOAD_NAME(deleteDocumentByNameWithTxn)
	void deleteDocument(XmlTransaction &txn, const std::string &name,
			    XmlUpdateContext &context);

OVERLOAD_NAME(updateDocumentWithTxn)
	void updateDocument(XmlTransaction &txn, XmlDocument &document,
			    XmlUpdateContext &context);
#endif /* SWIGJAVA */
	size_t getNumDocuments();
OVERLOAD_NAME(getNumDocumentWithTxn)
	size_t getNumDocuments(XmlTransaction &txn);

%extend {
	XmlIndexSpecification *getIndexSpecification() const {
		return new XmlIndexSpecification(self->getIndexSpecification());
	}
OVERLOAD_NAME(getIndexSpecificationWithTxn)
	XmlIndexSpecification *getIndexSpecification(
		XmlTransaction &txn) const {
		 return new XmlIndexSpecification(
			 self->getIndexSpecification(txn));
        }
OVERLOAD_NAME(getIndexSpecificationWithFlagsAndTxn)
	XmlIndexSpecification *getIndexSpecification(
		XmlTransaction &txn, u_int32_t flags) const {
		 return new XmlIndexSpecification(
			 self->getIndexSpecification(txn, flags));
	 }
#ifndef SWIGJAVA
	XmlDocument *getDocument(const std::string &name) {
		return new XmlDocument(self->getDocument(name, 0));
	}
OVERLOAD_NAME(getDocumentWithTxn)
	XmlDocument *getDocument(XmlTransaction &txn, const std::string &name)
	{
		return new XmlDocument(self->getDocument(txn, name, 0));
	}
	XmlValue *getNode(const std::string &name) {
		return new XmlValue(self->getNode(name, 0));
	}
OVERLOAD_NAME(getNodeWithTxn)
	XmlValue *getNode(XmlTransaction &txn, const std::string &name)
	{
		return new XmlValue(self->getNode(txn, name, 0));
	}
OVERLOAD_NAME(getNodeWithFlags)
	XmlValue *getNode(const std::string &name, u_int32_t flags) {
		return new XmlValue(self->getNode(name, flags));
	}
OVERLOAD_NAME(getNodeWithFlagsAndTxn)
	XmlValue *getNode(XmlTransaction &txn, const std::string &name,
				 u_int32_t flags) {
		return new XmlValue(self->getNode(txn, name, flags));
	}
	
OVERLOAD_NAME(getDocumentWithFlags)
	XmlDocument *getDocument(const std::string &name, u_int32_t flags) {
		return new XmlDocument(self->getDocument(name, flags));
	}
OVERLOAD_NAME(getDocumentWithFlagsAndTxn)
	XmlDocument *getDocument(XmlTransaction &txn, const std::string &name,
				 u_int32_t flags) {
		return new XmlDocument(self->getDocument(txn, name, flags));
	}

#endif

	XmlResults *getAllDocuments(u_int32_t flags) {
		   return new XmlResults(self->getAllDocuments(flags));
	}
OVERLOAD_NAME(getAllDocumentWithTxn)
	XmlResults *getAllDocuments(XmlTransaction &txn, u_int32_t flags) {
		   return new XmlResults(self->getAllDocuments(txn, flags));
	}

	XmlResults *lookupIndex(XmlQueryContext &context, const std::string &uri,
				const std::string &name, const std::string &index,
				const XmlValue &value = XmlValue(),
				u_int32_t flags = 0) {
		return new XmlResults(
			self->lookupIndex(context, uri, name, index,
					  value, flags));
	}
OVERLOAD_NAME(lookupEdgeIndex)
	XmlResults *lookupIndex(XmlQueryContext &context, const std::string &uri,
				const std::string &name,
				const std::string &parent_uri,
				const std::string &parent_name,
				const std::string &index,
				const XmlValue &value = XmlValue(),
				u_int32_t flags = 0) {
		return new XmlResults(self->lookupIndex(context, uri, name,
							parent_uri, parent_name,
							index, value, flags));
	}
OVERLOAD_NAME(lookupIndexWithTxn)
        XmlResults *lookupIndex(XmlTransaction &txn, XmlQueryContext &context,
				const std::string &uri, const std::string &name,
				const std::string &index,
				const XmlValue &value = XmlValue(),
				u_int32_t flags = 0) {
                return new XmlResults(self->lookupIndex(txn, context, uri, name,
							index, value, flags));
        }
OVERLOAD_NAME(lookupEdgeIndexWithTxn)
	XmlResults *lookupIndex(XmlTransaction &txn, XmlQueryContext &context,
				const std::string &uri, const std::string &name,
				const std::string &parent_uri,
				const std::string &parent_name,
				const std::string &index,
				const XmlValue &value = XmlValue(),
				u_int32_t flags = 0) {
	        return new XmlResults(self->lookupIndex(txn, context, uri, name,
							parent_uri, parent_name,
							index, value, flags));
	}
	XmlStatistics *lookupStatistics(const std::string &uri,
					const std::string &name,
					const std::string &index,
					const XmlValue &value = XmlValue()) {
		return new XmlStatistics(self->lookupStatistics(uri, name, index,
								value));
	}
OVERLOAD_NAME(lookupEdgeStatistics)
	XmlStatistics *lookupStatistics(const std::string &uri,
					const std::string &name,
					const std::string &parent_uri,
					const std::string &parent_name,
					const std::string &index,
					const XmlValue &value = XmlValue()) {
		return new XmlStatistics(self->lookupStatistics(uri, name,
								parent_uri,
								parent_name,
								index, value));
	}

OVERLOAD_NAME(lookupStatisticsWithTxn)
	XmlStatistics *lookupStatistics(XmlTransaction &txn,
					const std::string &uri,
					const std::string &name,
					const std::string &index,
					const XmlValue &value = XmlValue()) {
		return new XmlStatistics(self->lookupStatistics(txn, uri, name,
								index, value));
	}

OVERLOAD_NAME(lookupEdgeStatisticsWithTxn)
        XmlStatistics *lookupStatistics(XmlTransaction &txn,
					const std::string &uri,
					const std::string &name,
					const std::string &parent_uri,
					const std::string &parent_name,
					const std::string &index,
					const XmlValue &value = XmlValue()) {
		return new XmlStatistics(self->lookupStatistics(txn, uri, name,
								parent_uri,
								parent_name,
								index, value));
	}
} /* %extend */
};
#ifndef SWIGJAVA
class XmlDocument
{
public:

	XmlDocument();

	XmlDocument(const XmlDocument &o);
	~XmlDocument();

	void setName(const std::string &name);
	std::string getName() const;

#if !defined(SWIGTCL8) && !defined(SWIGPYTHON) && !defined(SWIGPHP4)
	/* 
	 * don't trust conversions with non utf-8 encodings 
	 * may be able to enable with some extra code/test
	 * force use of XmlData-based methods
	 */
	void setContent(const std::string &content);
OVERLOAD_NAME(setContentWithXmlData)
#endif
	void setContent(const XmlData &content);

	// input stream is owned by caller
	XmlInputStream *getContentAsXmlInputStream() const;
	// input stream is donated to callee

	void setContentAsXmlInputStream(XmlInputStream *adopted);

	XmlEventReader &getContentAsEventReader() const;
	void setContentAsEventReader(XmlEventReader &reader);
	void getContentAsEventWriter(XmlEventWriter &writer);
	
	void fetchAllData();
	void setMetaData(const std::string &uri,
			 const std::string &name, const XmlValue &value);
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlValue &value);
	void removeMetaData(const std::string &uri, const std::string &name);	
OVERLOAD_NAME(setMetaDataWithXmlData)
	void setMetaData(const std::string &uri,
			 const std::string &name, const XmlData &value);	
OVERLOAD_NAME(getMetaDataAsXmlData)
	bool getMetaData(const std::string &uri, const std::string &name,
			 XmlData &value);	
%extend {
	std::string getContentAsString() const {
		std::string s;
		return self->getContent(s);
	}
	const XmlData *getContent() const {
		return new XmlData(self->getContent());
	}
	const XmlData *getMetaData(const std::string &uri, const std::string &name) {
		XmlData data;
		bool ret = self->getMetaData(uri, name, data);
		if (ret)
			return new XmlData(data);
		return 0;
	}
	XmlMetaDataIterator *getMetaDataIterator() const {
		return new XmlMetaDataIterator(self->getMetaDataIterator());
	}
	bool equals(const XmlDocument &other) const {
		return (*self == other);
	}
}
};

class XmlMetaDataIterator
{
public:
	XmlMetaDataIterator(const XmlMetaDataIterator &o);
	~XmlMetaDataIterator();

	void reset();

%extend {
	XmlMetaData *next() {
		XmlMetaData *idecl = new XmlMetaData;
		if (self->next(idecl->uri, idecl->name, idecl->value))
			return idecl;
		else
			delete idecl;
		return NULL;
	}
}
#if defined(SWIGPYTHON)
%pythoncode %{
	def __iter__(self): return self
%}
#endif

};

// A SWIG-only class to wrap the results of the
// XmlMetaDataIterator
class XmlMetaData {
public:
	~XmlMetaData();

	const std::string &get_uri() const;
	const std::string &get_name() const;
	const XmlValue &get_value() const;
};

class XmlQueryContext
{
public:
	enum // ReturnType
	{
		LiveValues
	};
	
	enum // EvaluationType
	{
		Eager,
		Lazy
	};
	
	/// Constructor.
	XmlQueryContext();
	XmlQueryContext(const XmlQueryContext &o);
	~XmlQueryContext();

	void setNamespace(const std::string &prefix, const std::string &uri);
	std::string getNamespace(const std::string &prefix);
	void removeNamespace(const std::string &prefix);
	void clearNamespaces(void);
	void setVariableValue(const std::string &name, const XmlValue &value);
	void setVariableValue(const std::string &name, XmlResults &value);
	void setBaseURI(const std::string &baseURI);
	std::string getBaseURI() const;
	void setReturnType(enum XmlQueryContext::ReturnType type);
	enum XmlQueryContext::ReturnType getReturnType() const;
	void setEvaluationType(
		enum XmlQueryContext::EvaluationType type);
	enum XmlQueryContext::EvaluationType getEvaluationType() const;
	void setDefaultCollection(const std::string &uri);
	std::string getDefaultCollection() const;
	void interruptQuery();
	void setQueryTimeoutSeconds(u_int32_t secs);
	u_int32_t getQueryTimeoutSeconds() const;
#if defined(DBXML_DIRECTOR_CLASSES)
	XmlDebugListener *getDebugListener() const;
	void setDebugListener(XmlDebugListener *listener);
#endif

%extend {
	XmlValue *getVariableValue(const std::string &name) const {
		XmlValue *value = new XmlValue;
		try {
			if (self->getVariableValue(name, *value))
				return value;
			else {
				delete value;
				return NULL;
			}
		}
		catch(...) {
			delete value;
			throw;
		}
	}

	XmlResults *getVariableValues(const std::string &name) const {
		XmlResults *res = new XmlResults();
		try {
			if (self->getVariableValue(name, *res))
				return res;
			else {
				delete res;
				return NULL;
			}
		}
		catch(...) {
			delete res;
			throw;
		}
	}
#ifdef SWIGTCL8
	const std::string get(const char *name) const {
		XmlValue value;
		return self->getVariableValue(name, value) ? value.asString() : "";
	}
	
	void set(const std::string &name, const std::string &value) {
		XmlValue xval(value);
		self->setVariableValue(name, xval);
	}
	
	void setDebugVariable(const std::string &var) {
		self->setVariableValue("dbxml:debug", var);
	}
#endif /* SWIGTCL8 */
} /* %extend */
};
#endif //SWIGJAVA
class XmlResults
{
public:
#ifndef SWIGJAVA
	XmlResults();
#endif
	~XmlResults();
#ifndef SWIGJAVA
	XmlResults(const XmlResults &results);
#endif
	bool hasNext();
	bool hasPrevious();
	bool isNull() const;
#if defined(SWIGPYTHON) || defined(SWIGPHP4)
%extend {
	XmlValue *next() {
		XmlValue *value = new XmlValue;
		if (self->next(*value))
			return value;
		else {
			delete value;
			return NULL;
		}
	}
	XmlValue *previous() {
		XmlValue *value = new XmlValue;
		if (self->previous(*value))
			return value;
		else {
			delete value;
			return NULL;
		}
	}
	XmlValue *peek() {
		XmlValue *value = new XmlValue;
		if (self->peek(*value))
			return value;
		else {
			delete value;
			return NULL;
		}
	}
} /* %extend */
#elif defined(SWIGJAVA)
#else
	bool next(XmlValue &value);
	bool previous(XmlValue &value);
	bool peek(XmlValue &value);
#endif
	enum XmlQueryContext::EvaluationType getEvaluationType() const;
#ifndef SWIGJAVA
OVERLOAD_NAME(nextDocument)
	bool next(XmlDocument &document);
OVERLOAD_NAME(previousDocument)
	bool previous(XmlDocument &document);
OVERLOAD_NAME(peekDocument)
	bool peek(XmlDocument &document);
#endif
#if defined(SWIGPYTHON)
%pythoncode %{
	def __iter__(self): return self
%}
#endif
#ifndef SWIGJAVA
	void reset();
	size_t size() const;
	void add(const XmlValue &value);
#else
	size_t size() const;
%rename(resetInternal) reset;
	void reset();
#endif
#ifndef SWIGJAVA
	XmlEventWriter &asEventWriter();
#endif
	XmlResults copyResults();
	void concatResults(XmlResults &from);

};
#ifndef SWIGJAVA
class XmlUpdateContext
{
public:
	XmlUpdateContext();
	XmlUpdateContext(const XmlUpdateContext &o);
	virtual ~XmlUpdateContext();
};


class XmlValue
{
public:
	//NOTE: these enumerations MUST match those in XmlValue.hpp
	//The values should be copied to xml/src/java/com/sleepycat/dbxml/XmlValue.java
	enum  //NodeType
	{
		ELEMENT_NODE = 1,
		ATTRIBUTE_NODE = 2,
		TEXT_NODE = 3,
		CDATA_SECTION_NODE = 4,
		ENTITY_REFERENCE_NODE = 5,
		ENTITY_NODE = 6,
		PROCESSING_INSTRUCTION_NODE = 7,
		COMMENT_NODE = 8,
		DOCUMENT_NODE = 9,
		DOCUMENT_TYPE_NODE = 10,
		DOCUMENT_FRAGMENT_NODE = 11,
		NOTATION_NODE = 12
	};

	enum // Type
	{
		NONE               = 0,
		NODE               = 3,

		/// abstract type (separates type ids for DB XML and
		/// XML Schema atomic types)
		ANY_SIMPLE_TYPE    = 10,

		ANY_URI            = 11,
		BASE_64_BINARY     = 12,
		BOOLEAN            = 13,
		DATE               = 14,
		DATE_TIME          = 15,
		/// not a built-in primitive type
		DAY_TIME_DURATION  = 16,
		DECIMAL            = 17,
		DOUBLE             = 18,
		DURATION           = 19,
		FLOAT              = 20,
		G_DAY              = 21,
		G_MONTH            = 22,
		G_MONTH_DAY        = 23,
		G_YEAR             = 24,
		G_YEAR_MONTH       = 25,
		HEX_BINARY         = 26,
		NOTATION           = 27,
		QNAME              = 28,
		STRING             = 29,
		TIME               = 30,
		/// not a built-in primitive type
		YEAR_MONTH_DURATION= 31,

		/// untyped atomic data
		UNTYPED_ATOMIC     = 32,
		BINARY		   = 40
	};
	XmlValue();
	XmlValue(const XmlValue &other);
OVERLOAD_NAME(XmlValueFromString)
	XmlValue(const std::string &v);
OVERLOAD_NAME(XmlValueFromDouble)
	XmlValue(double v);
#ifndef SWIGTCL8
	// Not included in Tcl because there is no way to distinguish between
	// a boolean and a number.
OVERLOAD_NAME(XmlValueFromBool)
	XmlValue(bool v);
#endif
OVERLOAD_NAME(XmlValueFromDocument)
	XmlValue(const XmlDocument &document);
OVERLOAD_NAME(XmlValueTypedFromString)
	XmlValue(enum XmlValue::Type type, const std::string &v);
OVERLOAD_NAME(XmlValueURINameFromString)
	XmlValue(const std::string &typeURI, const std::string &typeName, 
	const std::string &v);
OVERLOAD_NAME(XmlValueTypedFromXmlData)
	XmlValue(enum XmlValue::Type type, const XmlData &dbt);

	~XmlValue();
	enum XmlValue::Type getType() const;
	std::string getTypeURI() const;
	std::string getTypeName() const;
	bool isNull() const;
	bool isType(enum XmlValue::Type type) const;

	bool isNumber() const;
	bool isString() const;
	bool isBoolean() const;
	bool isBinary() const;
	bool isNode() const;

	double asNumber() const;
	std::string asString() const;
	bool asBoolean() const;
%extend {
	XmlData * asBinary() const {
		XmlData data = self->asBinary();
		return new XmlData(data);
	}
}
#ifndef SWIGTCL8
	XmlDocument asDocument() const;
#else
%extend {
	XmlDocument *asDocument() const {
		return new XmlDocument(self->asDocument());
	}
}
#endif
	XmlEventReader &asEventReader() const;
	std::string getNodeHandle() const;
	bool equals(const XmlValue &value) const;

	std::string getNodeName() const;
	std::string getNodeValue() const;
	std::string getNamespaceURI() const;
	std::string getPrefix() const;
	std::string getLocalName() const;
	short getNodeType() const;

%extend {
	XmlValue *getParentNode() const {
		return new XmlValue(self->getParentNode());
	}
	XmlValue *getFirstChild() const {
		return new XmlValue(self->getFirstChild());
	}
	XmlValue *getLastChild() const {
		return new XmlValue(self->getLastChild());
	}
	XmlValue *getPreviousSibling() const {
		return new XmlValue(self->getPreviousSibling());
	}
	XmlValue *getNextSibling() const {
		return new XmlValue(self->getNextSibling());
	}
	XmlResults *getAttributes() const {
		return new XmlResults(self->getAttributes());
	}
	XmlValue *getOwnerElement() const {
		return new XmlValue(self->getOwnerElement());
	}

	// allows scripted languages to emulate operator=
	static void setValue(XmlValue &to, const XmlValue &from) {
		to = from;
	}
}
};
#endif /* SWIGJAVA */

class XmlIndexSpecification {
public:
	enum //Type 
	{
		UNIQUE_OFF = 0x00000000,
		UNIQUE_ON = 0x10000000,

		PATH_NONE = 0x00000000,
		PATH_NODE = 0x01000000,
		PATH_EDGE = 0x02000000,

		NODE_NONE = 0x00000000,
		NODE_ELEMENT = 0x00010000,
		NODE_ATTRIBUTE = 0x00020000,
		NODE_METADATA = 0x00030000,

		KEY_NONE = 0x00000000,
		KEY_PRESENCE = 0x00000100,
		KEY_EQUALITY = 0x00000200,
		KEY_SUBSTRING = 0x00000300
	};
#ifndef SWIGJAVA
	XmlIndexSpecification();
#endif
	virtual ~XmlIndexSpecification();

	void addIndex(const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type type,
		      enum XmlValue::Type syntax);
OVERLOAD_NAME(addIndexAsString)
	void addIndex(const std::string &uri, const std::string &name,
		      const std::string &index);
	void deleteIndex(const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type type,
		      enum XmlValue::Type syntax);
OVERLOAD_NAME(deleteIndexAsString)
	void deleteIndex(const std::string &uri, const std::string &name,
			 const std::string &index);
	void replaceIndex(const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type type,
		      enum XmlValue::Type syntax);
OVERLOAD_NAME(replaceIndexAsString)	
	void replaceIndex(const std::string &uri, const std::string &name,
			  const std::string &index);

	void addDefaultIndex(enum XmlIndexSpecification::Type type,
			     enum XmlValue::Type syntax);
OVERLOAD_NAME(addDefaultIndexAsString)
	void addDefaultIndex(const std::string &index);
	void deleteDefaultIndex(enum XmlIndexSpecification::Type type,
				enum XmlValue::Type syntax);
OVERLOAD_NAME(deleteDefaultIndexAsString)
	void deleteDefaultIndex(const std::string &index);
	void replaceDefaultIndex(enum XmlIndexSpecification::Type type,
				 enum XmlValue::Type syntax);
OVERLOAD_NAME(replaceDefaultIndexAsString)	
	void replaceDefaultIndex(const std::string &index);

	void reset();

	bool getAutoIndexing() const;
	void setAutoIndexing(bool value);
%extend {
	XmlIndexDeclaration *find(const std::string &uri,
				  const std::string &name) {
		XmlIndexDeclaration *idecl =
			new XmlIndexDeclaration(uri, name, "");
		if (self->find(idecl->uri, idecl->name, idecl->index))
			return idecl;
		else {
			delete idecl;
			return NULL;
		}
	}
	
	XmlIndexDeclaration *next() {
		XmlIndexDeclaration *idecl = new XmlIndexDeclaration;
		if (self->next(idecl->uri, idecl->name, idecl->index))
			return idecl;
		else {
			delete idecl;
			return NULL;
		}
	}
}
	std::string getDefaultIndex() const;

	static enum XmlValue::Type getValueType(const std::string &index);

#if defined(SWIGPYTHON)
%pythoncode %{
	def __iter__(self): return self
%}
#endif
};

#ifndef SWIGJAVA
class XmlIndexDeclaration {
public:
	~XmlIndexDeclaration();

	const std::string &get_uri() const;
	const std::string &get_name() const;
	const std::string &get_index() const;
};
#endif

class XmlQueryExpression
{
public:
#ifndef SWIGJAVA
	XmlQueryExpression();
#endif
	XmlQueryExpression(const XmlQueryExpression &queryExpression);
	~XmlQueryExpression();

	const std::string & getQuery() const;
	std::string getQueryPlan() const;
	bool isUpdateExpression() const;

%extend {
	XmlResults *execute(XmlQueryContext &context,
			    u_int32_t flags = 0) const {
		return new XmlResults(self->execute(context, flags));
	}
OVERLOAD_NAME(executeWithContextItem)
	 XmlResults *execute(const XmlValue &contextItem,
			     XmlQueryContext &context,
			     u_int32_t flags = 0) const {
		return new XmlResults(
			self->execute(contextItem, context, flags));
	}
OVERLOAD_NAME(executeWithTxn)
	 XmlResults *execute(XmlTransaction &txn, XmlQueryContext &context,
			     u_int32_t flags = 0) const {
		return new XmlResults(self->execute(txn, context, flags));
	}
OVERLOAD_NAME(executeWithContextItemAndTxn)
	 XmlResults *execute(XmlTransaction &txn, const XmlValue &contextItem,
			     XmlQueryContext &context,
			     u_int32_t flags = 0) const {
		return new XmlResults(self->execute(txn, contextItem,
						    context, flags));
	}
}
};

class XmlInputStream
{
#if defined(SWIGJAVA)
protected:
	XmlInputStream();
#endif
public:
	virtual ~XmlInputStream();
	// SWIG needs to know about pure virtuals; otherwise,
	// a reference will be generated.
	virtual unsigned int curPos() const = 0;
	virtual unsigned int readBytes(char *toFill,
				       const unsigned int maxToRead) = 0;
%extend {
	// allow wrapped languages to free C++ memory in the event
	// object is not donated to putDocument().
	void freeMemory() {
		delete self;
	}
}
};

#if defined(DBXML_DIRECTOR_CLASSES)

#if defined(SWIGJAVA)
%apply const std::string & {std::string &};
#endif

class XmlResolver
{
protected:
	XmlResolver();
public:
	virtual ~XmlResolver();
	virtual bool resolveDocument(XmlTransaction *txn,XmlManager &mgr,
				     const std::string &uri,
				     XmlValue &reslt) const;
	virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
				       const std::string &uri,
				       XmlResults &reslt) const;
	virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
					      const std::string &schemaLocation,
					      const std::string &nameSpace) const;
	virtual XmlInputStream *resolveEntity(XmlTransaction *txn, XmlManager &mgr,
					      const std::string &systemId,
					      const std::string &publicId) const;

	virtual bool resolveModuleLocation(
		XmlTransaction *txn, XmlManager &mgr,
		const std::string &nameSpace, XmlResults &reslt) const;
	virtual XmlInputStream *resolveModule(XmlTransaction *txn, XmlManager &mgr,
		const std::string &moduleLocation, const std::string &nameSpace) const;
#ifndef(SWIGJAVA)
	virtual XmlExternalFunction *resolveExternalFunction(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, const std::string &name, size_t numberOfArgs) const;
#endif
};

class XmlArguments
{
public:
	virtual ~XmlArguments() {}
	virtual XmlResults getArgument(size_t index) const = 0;
	virtual unsigned int getNumberOfArgs() const = 0;
};

class XmlExternalFunction
{
#if defined(SWIGJAVA) || defined(SWIGPYTHON)
protected:
	XmlExternalFunction() {}
#endif
public:
	virtual ~XmlExternalFunction() {}

	virtual XmlResults execute(XmlTransaction &txn, XmlManager &mgr,
		const XmlArguments &args) const = 0;
	virtual void close() = 0;
};

class XmlStackFrame
{
public:
	virtual const char *getQueryFile() const = 0;
	virtual int getQueryLine() const = 0;
	virtual int getQueryColumn() const = 0;

	virtual XmlResults query(const std::string &queryString) const = 0;
	virtual std::string getQueryPlan() const = 0;

	virtual const XmlStackFrame *getPreviousStackFrame() const = 0;
};

class XmlDebugListener
{
#if defined(SWIGJAVA) || defined(SWIGPYTHON)
protected:
	XmlDebugListener();
#endif
public:
	virtual ~XmlDebugListener();

	virtual void start(const XmlStackFrame *stack);
	virtual void end(const XmlStackFrame *stack);
	virtual void enter(const XmlStackFrame *stack);
	virtual void exit(const XmlStackFrame *stack);
	virtual void error(const XmlException &error, const XmlStackFrame *stack);
};

// For now, no compression API in Python
#if defined(SWIGJAVA)
class XmlCompression
{
#if defined(SWIGJAVA) || defined(SWIGPYTHON)
protected:
	XmlCompression();
#endif
public:
	virtual ~XmlCompression();
	virtual bool compress(XmlTransaction &txn, const XmlData &source, XmlData &dest) = 0;
	virtual bool decompress(XmlTransaction &txn, const XmlData &source, XmlData &dest) = 0;
};

#endif
#endif

#ifndef SWIGJAVA
class XmlContainerConfig
{
public:
	enum //ConfigState
	{
		On,
		Off,
		UseDefault
	};
	XmlContainerConfig();
	XmlContainerConfig(u_int32_t flags);
	~XmlContainerConfig();
	XmlContainerConfig(const XmlContainerConfig &o);
	
	int getMode() const;
	void setMode(int mode);

	enum XmlContainer::ContainerType getContainerType() const;
	void setContainerType(enum XmlContainer::ContainerType type);
	
	u_int32_t getPageSize() const;
	void setPageSize(u_int32_t pageSize);

	u_int32_t getSequenceIncrement() const;
	void setSequenceIncrement(u_int32_t incr);

	void setIndexNodes(enum XmlContainerConfig::ConfigState state);
	enum XmlContainerConfig::ConfigState getIndexNodes() const;

	void setChecksum(bool value);
	bool getChecksum() const;

	void setEncrypted(bool value);
	bool getEncrypted() const;

	void setAllowValidation(bool value);
	bool getAllowValidation() const;

	void setStatistics(enum XmlContainerConfig::ConfigState state);
	enum XmlContainerConfig::ConfigState getStatistics() const;

	void setTransactional(bool value);
	bool getTransactional() const;

	void setAllowCreate(bool value);
	bool getAllowCreate() const;

	void setExclusiveCreate(bool value);
	bool getExclusiveCreate() const;

	void setNoMMap(bool value);
	bool getNoMMap() const;

	void setReadOnly(bool value);
	bool getReadOnly() const;

	void setMultiversion(bool value);
	bool getMultiversion() const;

	void setReadUncommitted(bool value);
	bool getReadUncommitted() const;

	void setThreaded(bool value);
	bool getThreaded() const;

	void setTransactionNotDurable(bool value);
	bool getTransactionNotDurable() const;
#if defined(DBXML_DIRECTOR_CLASSES)
	void setCompressionName(const char *name);
	const char *getCompressionName() const;
#endif
};
#endif /*SWIGJAVA*/

class XmlTransaction
{
public:
	~XmlTransaction();
#ifndef SWIGJAVA
	XmlTransaction();
	XmlTransaction(const XmlTransaction &);

	void abort();
	void commit(u_int32_t flags);
	
%extend {
	XmlTransaction *createChild(u_int32_t flags = 0) {
		return new XmlTransaction(self->createChild(flags));
	}
OVERLOAD_NAME(commitDefaultFlags)
	void commit() {
	  self->commit(0);
	}
}

	DB_TXN *getDB_TXN();
#endif // !SWIGJAVA	
};

class XmlStatistics
{
public:
	XmlStatistics(const XmlStatistics&);
	~XmlStatistics();

	double getNumberOfIndexedKeys() const;
	double getNumberOfUniqueKeys() const;
	double getSumKeyValueSize() const;
};

#ifdef SWIGJAVA
%rename(closeInternal) XmlEventReader::close;
#endif

class XmlEventReader
{
protected:
	XmlEventReader();
public:
	enum { // XmlEventType
		StartElement,
		EndElement,
		Characters,
		CDATA,
		Comment,
		Whitespace,
		StartDocument,
		EndDocument,
		StartEntityReference,
		EndEntityReference,
		ProcessingInstruction,
		DTD
	};
	~XmlEventReader();
	virtual void close() = 0;

	void setReportEntityInfo(bool value);
	bool getReportEntityInfo() const;
	void setExpandEntities(bool value);
	bool getExpandEntities() const;

	enum XmlEventReader::XmlEventType next();
	enum XmlEventReader::XmlEventType nextTag();

	bool hasNext() const;
	enum XmlEventReader::XmlEventType getEventType() const;

	const unsigned char *getNamespaceURI() const;
	const unsigned char *getLocalName() const;
	const unsigned char *getPrefix() const;
// avoid the by-reference length parameter for non-C++, instead
// add an explicit call
%extend {
	const unsigned char *getValue() const {
		size_t len;
		return self->getValue(len);
	}
}
#ifndef SWIGJAVA
// byte length is not useful for Java.  It may not
// be for other languages -- need to check
%extend {
	size_t getValueLength() const {
		size_t len;
		(void)self->getValue(len);
		return len;
	}
}
#endif

	int getAttributeCount() const;
	bool isAttributeSpecified(int index) const;
	const unsigned char *getAttributeLocalName(int index) const;
	const unsigned char *getAttributeNamespaceURI(int index) const;
	const unsigned char *getAttributePrefix(int index) const;
	const unsigned char *getAttributeValue(int index) const;

	const unsigned char *getEncoding() const;
	const unsigned char *getVersion() const;
	const unsigned char *getSystemId() const;
	bool isStandalone() const;
	bool standaloneSet() const;
	bool encodingSet() const;
	
	bool hasEntityEscapeInfo() const;
	bool needsEntityEscape(int index = 0) const;
	bool hasEmptyElementInfo() const;
	bool isEmptyElement() const;
	bool isWhiteSpace() const;
};

#ifdef SWIGJAVA
%rename(closeInternal) XmlEventWriter::close;
#endif

class XmlEventWriter
{
protected:
	XmlEventWriter();
public:
	~XmlEventWriter();
	virtual void close() = 0;

	void writeAttribute(const unsigned char *localName,
			    const unsigned char *prefix,
			    const unsigned char *uri,
			    const unsigned char *value,
			    bool isSpecified);
	// If length parameter is 0, the library will perform
	// the counting.  This should be done if string length
	// is != to the UTF-8 byte count (e.g. multi-byte chars)
	void writeText(enum XmlEventReader::XmlEventType type,
		       const unsigned char *text,
		       size_t length);
	void writeDTD(const unsigned char *dtd, size_t length);

// There is a problem with the Tcl overload here...
#if defined(DBXML_USEOVERLOADS) && !defined(SWIGTCL8)
	// Add no-length overloads
%extend {	
	void writeText(enum XmlEventReader::XmlEventType type,
		       const unsigned char *text) {
		self->writeText(type, text, 0);
	}
	void writeDTD(const unsigned char *dtd) {
		self->writeDTD(dtd, 0);
	}
}	
#endif
	void writeProcessingInstruction(const unsigned char *target,
					const unsigned char *data);
	
	void writeStartElement(const unsigned char *localName,
			       const unsigned char *prefix,
			       const unsigned char *uri,
			       int numAttributes,
			       bool isEmpty);
	void writeEndElement(const unsigned char *localName,
			     const unsigned char *prefix,
			     const unsigned char *uri);
	
	void writeStartDocument(const unsigned char *version,
				const unsigned char *encoding,
				const unsigned char *standalone);
	void writeEndDocument();
	
	void writeStartEntity(const unsigned char *name,
			      bool expandedInfoFollows);
	void writeEndEntity(const unsigned char *name);
};

class XmlEventReaderToWriter
{
public:
#ifndef SWIGJAVA
	XmlEventReaderToWriter(XmlEventReader &reader,
			       XmlEventWriter &writer,
			       bool ownsReader);
#endif
	XmlEventReaderToWriter(XmlEventReader &reader,
			       XmlEventWriter &writer,
			       bool ownsReader, bool ownsWriter);
	~XmlEventReaderToWriter();

	void start();
};
