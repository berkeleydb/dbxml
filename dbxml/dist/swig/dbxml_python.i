/* Python integration added by Michael Droettboom */

%module(directors="1") "dbxml"

// Enable the "director" feature for allowing Java
// implementations of virtual behavior for 2 classes,
// XmlInputStream and XmlResolver
%feature("director") XmlInputStream;
%feature("director") XmlResolver;
%feature("director") XmlExternalFunction;
%feature("director") XmlDebugListener;

// Make sure that a DB_ENV passed (python obj) gets an
// extra held reference from XmlManager
%pythonappend XmlManager::XmlManager(DB_ENV *dbEnv, u_int32_t flags) %{
        if (len(args) > 1): self.env = args[0]
%}

// Put a reference on XmlManager in XmlContainer objects as they are
// create.  SWIG appears to require a pythonappend directive for 
// *every* overload to make this work
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name) %{
        val.mgr = self
%}
%pythonappend XmlManager::openContainer(const std::string &name) %{
        val.mgr = self
%}
%pythonappend XmlManager::openContainer(const std::string &name,u_int32_t flags) %{
        val.mgr = self
%}
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags) %{
        val.mgr = self
%}
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type) %{
        val.mgr = self
%}
%pythonappend XmlManager::openContainer(const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type) %{
        val.mgr = self
%}

// now createContainer
%pythonappend XmlManager::createContainer(XmlTransaction &txn, const std::string &name) %{
        val.mgr = self
%}
%pythonappend XmlManager::createContainer(const std::string &name) %{
        val.mgr = self
%}
%pythonappend XmlManager::createContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type=XmlContainer::NodeContainer, int mode=0) %{
        val.mgr = self
%}
%pythonappend XmlManager::createContainer(const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type=XmlContainer::NodeContainer, int mode=0) %{
        val.mgr = self
%}

// TBD -- see if there's a better way to extract
// info from exception
%{
static void throwPyUserException() {
	PyObject *type = 0;
	PyObject *value = 0;
	PyObject *traceback = 0;
	PyErr_Fetch(&type, &value, &traceback);
	if (value) {
		char buf[1024];
		PyObject *str = PyObject_Str(value);
		Py_XINCREF(type);
		PyErr_Clear();
		PyOS_snprintf(buf, sizeof(buf), "Error from Python user code: %s", PyString_AsString(str));
		Py_DECREF(str);
		//PyErr_Print();
		throw XmlException(XmlException::INTERNAL_ERROR,
				   buf);
	}
}

// create the appropriate exception object
static void makeXmlException(const XmlException &xe)
{
	const char *ename = NULL;
	switch (xe.getExceptionCode()) {
	case XmlException::INTERNAL_ERROR:
		ename = "XmlInternalError"; break;
	case XmlException::CONTAINER_OPEN:
		ename = "XmlContainerOpen"; break;
	case XmlException::CONTAINER_CLOSED:
		ename = "XmlContainerClosed"; break;
	case XmlException::CONTAINER_EXISTS:
		ename = "XmlContainerExists"; break;
	case XmlException::CONTAINER_NOT_FOUND:
		ename = "XmlContainerNotFound"; break;
	case XmlException::NULL_POINTER:
		ename = "XmlNullPointer"; break;
	case XmlException::INDEXER_PARSER_ERROR:
		ename = "XmlParserError"; break;
	case XmlException::DATABASE_ERROR:
		ename = "XmlDatabaseError"; break;
	case XmlException::QUERY_PARSER_ERROR:
		ename = "XmlQueryParserError"; break;
	case XmlException::QUERY_EVALUATION_ERROR:
		ename = "XmlQueryEvaluationError"; break;
	case XmlException::LAZY_EVALUATION:
		ename = "XmlLazyEvaluation"; break;
	case XmlException::UNKNOWN_INDEX:
		ename = "XmlUnknownIndex"; break;
	case XmlException::DOCUMENT_NOT_FOUND:
		ename = "XmlDocumentNotFound"; break;
	case XmlException::INVALID_VALUE:
		ename = "XmlInvalidValue"; break;
	case XmlException::VERSION_MISMATCH:
		ename = "XmlVersionMismatch"; break;
	case XmlException::TRANSACTION_ERROR:
		ename = "XmlTransactionError"; break;
	case XmlException::UNIQUE_ERROR:
		ename = "XmlUniqueError"; break;
	case XmlException::NO_MEMORY_ERROR:
		ename = "XmlNoMemoryError"; break;
	case XmlException::EVENT_ERROR:
		ename = "XmlEventError"; break;
	case XmlException::OPERATION_INTERRUPTED:
		ename = "XmlOperationInterrupted"; break;
	case XmlException::OPERATION_TIMEOUT:
		ename = "XmlOperationTimeout"; break;
	default:
		ename = "XmlException";
	}
	if (ename != NULL) {
		PyObject *dbxmlMod = PyImport_ImportModule("dbxml");
		
		// set the value to an object with the code and text
		XmlException::ExceptionCode ec = xe.getExceptionCode();
		const char *what = xe.what();
		int dberr = xe.getDbErrno();
		int qline = xe.getQueryLine();
		int qcol = xe.getQueryColumn();

		// construct an exception object
		PyObject *errClass = PyObject_GetAttrString(dbxmlMod, ename);
		if (!errClass) {
			std::string msg = "Couldn't get BDB XML exception class: ";
			msg += ename;
			PyErr_SetString(PyExc_RuntimeError, msg.c_str());
			return;
		}
		PyObject *args = NULL;
		if (ec == XmlException::DATABASE_ERROR)
			args = Py_BuildValue( "(si)", what,dberr);
		else if ((ec == XmlException::QUERY_PARSER_ERROR) ||
			 (ec == XmlException::QUERY_EVALUATION_ERROR))
			args = Py_BuildValue( "(sii)", what,qline,qcol);
		else
			args = Py_BuildValue( "(s)", what);
		PyObject *errReturn = PyObject_CallObject(errClass, args);
		if (!errReturn) {
			std::string msg = "Couldn't instantiate BDB XML exception: ";
			msg += ename;
			PyErr_SetString(PyExc_RuntimeError, msg.c_str());
			return;
		}
		
		// set the actual error/exception object
		PyObject *etype = PyObject_Type(errReturn);
		PyErr_SetObject(etype, errReturn);
		Py_DECREF(args);
		Py_DECREF(errReturn);
		Py_DECREF(etype);
		Py_DECREF(errClass);
		Py_DECREF(dbxmlMod);
	}
}
%}

// if a director call fails (in python), throw
// an XmlException -- most of these calls originate
// in BDB XML proper, not Python.
%feature("director:except") {
	if ($error != NULL) {
		throwPyUserException();
	}
}

// Encapsulate release/acquire of global interpreter lock in
// an exception-safe class
// GMF: Added this for possible use, but it appears that the
// SWIG macros used with the -threads directive work well enough.
// Leave it for now, just in case it comes in handy.
%{
#ifdef SWIG_PYTHON_USE_GIL
class dbxml_releaseGIL {
public:
	dbxml_releaseGIL() {
		_save = PyEval_SaveThread();
	}
	~dbxml_releaseGIL() {
		PyEval_RestoreThread(_save);
	}
	PyThreadState *_save;
};
#else
class dbxml_releaseGIL {
public:
	dbxml_releaseGIL() {}
};
#endif
%}

%exception {
	try {
		$action
	} catch (XmlException &e) {
		SWIG_PYTHON_THREAD_END_ALLOW;
		makeXmlException(e);
		return NULL;
	}
}

// Director calls don't have SWIG_PYTHON_THREAD_START_ALLOW,
// and it doesn't appear that it's possible to change
// %exception on a per-class basis, so enumerate the virtual methods
%exception XmlInputStream::curPos {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlInputStream::readBytes {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveDocument {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveCollection {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveSchema {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveEntity {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveModule {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveModuleLocation {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveExternalFunction {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlExternalFunction::execute {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlExternalFunction::close {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlDebugListener::start {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlDebugListener::end {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlDebugListener::enter {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlDebugListener::exit {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlDebugListener::error {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}


%define PYTHON_ITERATOR(f)
%exception f {
	try {
		$action
		if (!result) {
			SWIG_PYTHON_THREAD_END_ALLOW;
			PyErr_SetString(PyExc_StopIteration,
					"End of results.");
			return NULL;
		}
	} catch (XmlException &e) {
		SWIG_PYTHON_THREAD_END_ALLOW;
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}
%enddef

PYTHON_ITERATOR(XmlResults::next)
PYTHON_ITERATOR(XmlIndexSpecification::next)
PYTHON_ITERATOR(XmlMetaDataIterator::next)

%typemap(in) const unsigned char * {
  char *ret;
  if ($input == Py_None)
     ret = 0;
  else {
      ret = PyString_AsString($input);
      if (ret && (*ret == '\0')) ret = 0;
  }
  $1 = (unsigned char *)ret;
}

%typemap(out) const unsigned char * {
  if ($1)
    $result = PyString_FromString((const char*)$1);
  else
    $result = Py_None;
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_STRING) const unsigned char * {
  $1 = PyString_Check($input) ? 1 : 0;
}

%typemap(in) XmlData *, XmlData & (XmlData xml_data) {
	xml_data.set((const void*)PyString_AsString($input), 
                     PyString_Size($input));
	$1 = &xml_data;
}

%typemap(in) const XmlData *, const XmlData & (XmlData xml_data) {
	xml_data.set((const void*)PyString_AsString($input), 
                     PyString_Size($input));
	$1 = &xml_data;
}

%typemap(out) XmlData *, XmlData & {
	if ($1) {
		$result =
			PyString_FromStringAndSize((char *)$1->get_data(),
						   $1->get_size());
		delete $1;
	} else
	    $result = Py_None;
}

%typemap(out) const XmlData *, const XmlData & {
	if ($1) {
		$result =
			PyString_FromStringAndSize((char *)$1->get_data(),
						   $1->get_size());
		delete $1;
	} else
	    $result = Py_None;
}

%{
#if defined(HAVE_BSDDB)
#include "Modules/bsddb.h"
#else
/* Copied from bsddb3 source. This can break if bsddb3 changes.
   E.g. they changed in bsddb3-4.6.x so this code
   *requires* that release at least.  Preferably use the
   conditional above and include bsddb.h directly */

struct behaviourFlags {
    unsigned int getReturnsNone : 1;
    unsigned int cursorSetReturnsNone : 1;
};

typedef struct {
    PyObject_HEAD
    DB_ENV*     db_env;
    u_int32_t   flags;             /* saved flags from open() */
    int         closed;
    struct behaviourFlags moduleFlags;
    struct DBObject *children_dbs;
    struct DBTxnObject *children_txns;
    PyObject        *in_weakreflist; /* List of weak references */
} DBEnvObject;

typedef struct DBTxnObject {
    PyObject_HEAD
    DB_TXN*         txn;
    PyObject        *env;
    struct DBTxnObject *parent_txn;
    struct DBTxnObject **sibling_prev_p;
    struct DBTxnObject *sibling_next;
    struct DBTxnObject *children_txns;
    struct DBObject *children_dbs;
    struct DBSequenceObject *children_sequences;
    struct DBCursorObject *children_cursors;
    PyObject        *in_weakreflist; /* List of weak references */
} DBTxnObject;
#endif
%}

%typemap(in) DB_ENV* {
  $1 = ($input == Py_None) ? 
    NULL : ((DBEnvObject *)$input)->db_env;
}

%typemap(in) DB_TXN* {
  $1 = ($input == Py_None) ?
    NULL : ((DBTxnObject *)$input)->txn;
}

/* 
 * gmf: in order to support function overloads that use DB_TXN,
 * which is the subject of a typemap(in), above, a typemap(typecheck)
 * is required; otherwise, the generated code fails.  This is a general
 * rule for SWIG and overloads.  This check always succeeds.  Precedence is
 * set high (low).
 */
%typemap(typecheck, precedence=2000) DB_TXN * {
 /* This would be a typecheck for a DB_TXN *; just succeed */
 $1 = 1; 
}

%typemap(typecheck, precedence=2000) DB_ENV * {
 /* This would be a typecheck for a DB_ENV *; just succeed */
 $1 = 1; 
}

%init %{
	static const int dbMajorMin=4;
	static const int dbMinorMin=3;
	int major, minor, patch, dmajor, dminor, dpatch;
	(void) db_version(&major, &minor, &patch);
	if (major < dbMajorMin || minor < dbMinorMin) {
		char buf[512];
		PyOS_snprintf(buf, sizeof(buf), "Version mismatch: Berkeley DB XML %d.%d.%d requires Berkeley DB \n%d.%d or higher; version %d.%d.%d was found\n",
			      DBXML_VERSION_MAJOR, DBXML_VERSION_MINOR,
			      DBXML_VERSION_PATCH,
			      dbMajorMin, dbMinorMin, major, minor, patch);
		PyErr_SetString(PyExc_RuntimeError, buf);
		return;
	}
	(void) DbXml::dbxml_db_version(&dmajor, &dminor, &dpatch);
	// match major and minor numbers
	// TBD: warn if patch level is different
	if (major != dmajor || minor != dminor) {
		char buf[1024];
		PyOS_snprintf(buf, sizeof(buf), "Mismatched Berkeley DB Libraries.  Python linked with %d.%d.%d, and BDB XML is linked with %d.%d.%d\n",
			      major, minor, patch, dmajor, dminor, dpatch);
		PyErr_SetString(PyExc_RuntimeError, buf);
		return;
	}

        // construct XmlException constants as globals
#define MAKE_EX_CONSTANT(n) PyDict_SetItemString(d, #n, PyInt_FromLong(XmlException::n))
	MAKE_EX_CONSTANT(INTERNAL_ERROR);
	MAKE_EX_CONSTANT(CONTAINER_OPEN);
	MAKE_EX_CONSTANT(CONTAINER_CLOSED);
	MAKE_EX_CONSTANT(NULL_POINTER);
	MAKE_EX_CONSTANT(INDEXER_PARSER_ERROR);
	MAKE_EX_CONSTANT(DATABASE_ERROR);
	MAKE_EX_CONSTANT(QUERY_PARSER_ERROR);
	MAKE_EX_CONSTANT(QUERY_EVALUATION_ERROR);
	MAKE_EX_CONSTANT(LAZY_EVALUATION);
	MAKE_EX_CONSTANT(DOCUMENT_NOT_FOUND);
	MAKE_EX_CONSTANT(CONTAINER_EXISTS);
	MAKE_EX_CONSTANT(UNKNOWN_INDEX);
	MAKE_EX_CONSTANT(INVALID_VALUE);
	MAKE_EX_CONSTANT(VERSION_MISMATCH);
	MAKE_EX_CONSTANT(EVENT_ERROR);
	MAKE_EX_CONSTANT(CONTAINER_NOT_FOUND);
	MAKE_EX_CONSTANT(TRANSACTION_ERROR);
	MAKE_EX_CONSTANT(UNIQUE_ERROR);
	MAKE_EX_CONSTANT(NO_MEMORY_ERROR);
	MAKE_EX_CONSTANT(OPERATION_INTERRUPTED);
	MAKE_EX_CONSTANT(OPERATION_TIMEOUT);
%}

%pythoncode %{
class XmlException(Exception):
    """Base class for BDB XML exceptions.  It should never be called directly, and
    if it is, it's an unknown error
    Attributes:
        what -- the exception message
    """
    def __init__(self, msg):
        self.exceptionCode = INTERNAL_ERROR
        self.what = "Unknown exception thrown: ",msg
    def __str__(self):
        return "XmlException %d, %s"%(self.exceptionCode,self.what)
    def getexceptionCode(self):
        return self.exceptionCode
    def what(self):
        return self.what

class XmlDatabaseError(XmlException):
    """Raised when Berkeley DB returns an error
    Attributes:
        dbError -- the Berkeley DB errno
    """
    def __init__(self, msg, dberr):
        self.exceptionCode = DATABASE_ERROR
        self.what = msg
        self.dbError = dberr
    def __str__(self):
        return "XmlDatabaseError %d, %s"%(self.dbError,self.what)
    def getDbErrno(self):
        return self.dbError

class XmlContainerClosed(XmlException):
    """Raised when an operation requires an open container and it is closed.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = CONTAINER_CLOSED
        self.what = msg

class XmlContainerExists(XmlException):
    """Raised when an operation requires that a container does not exist
    and it exists.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = CONTAINER_EXISTS
        self.what = msg

class XmlContainerOpen(XmlException):
    """Raised when an operation requires a closed container and it is open.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = CONTAINER_OPEN
        self.what = msg

class XmlDocumentNotFound(XmlException):
    """Raised when a specified document is not found in a container.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = DOCUMENT_NOT_FOUND
        self.what = msg

class XmlInvalidValue(XmlException):
    """Raised when a invalid value or parameter is specified.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = INVALID_VALUE
        self.what = msg

class XmlParserError(XmlException):
    """Raised when there is an XML parser error while parsing a document.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = XML_PARSER_ERROR
        self.what = msg

class XmlInternalError(XmlException):
    """Raised when there is an internal error in the BDB XML library.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = INTERNAL_ERROR
        self.what = msg

class XmlVersionMismatch(XmlException):
    """Raised when there is version mismatch between the library and a container.
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = VERSION_MISMATCH
        self.what = msg

class XmlLazyEvaluation(XmlException):
    """Raised when an operation that requires an eager result set is
    called on a lazily evaluated result set.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = LAZY_EVALUATION
        self.what = msg

class XmlUnknownIndex(XmlException):
    """Raised when an operation is called on an unknown index in a container.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = UNKNOWN_INDEX
        self.what = msg

class XmlQueryEvaluationError(XmlException):
    """Raised when there is a runtime error during query evaluation
    
    Attributes: see XmlException
    queryLine -- line of the query that caused the exception, if available
    queryColumn -- column of the query that caused the exception, if available
    
    """
    def __init__(self, msg, line, column):
        self.exceptionCode = QUERY_EVALUATION_ERROR
        self.what = msg
        self.queryLine = line
        self.queryColumn = column
    def __str__(self):
        return "XmlQueryEvaluationError %s, line %d, column %d"%(self.what,self.queryLine,self.queryColumn)
    def getqueryLine(self):
        return self.queryLine
    def getqueryColumn(self):
        return self.queryColumn

class XmlQueryParserError(XmlException):
    """Raised when there is an error parsing a query expression
    
    Attributes: see XmlException
    queryLine -- line of the query that caused the exception, if available
    queryColumn -- column of the query that caused the exception, if available
    """
    def __init__(self, msg, line, column):
        self.exceptionCode = QUERY_PARSER_ERROR
        self.what = msg
        self.queryLine = line
        self.queryColumn = column
    def __str__(self):
        return "XmlQueryParserError %s, line %d, column %d"%(self.what,self.queryLine,self.queryColumn)
    def getqueryLine(self):
        return self.queryLine
    def getqueryColumn(self):
        return self.queryColumn

class XmlEventError(XmlException):
    """Raised when there is an error during an XmlEventReader or 
    XmlEventWriter call.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = EVENT_ERROR
        self.what = msg

class XmlContainerNotFound(XmlException):
    """Raised when a required container is not found.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = CONTAINER_NOT_FOUND
        self.what = msg

class XmlUniqueError(XmlException):
    """Raised when there is an attempt to insert a duplicate of a
    uniquely indexed item such as a document name.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = UNIQUE_ERROR
        self.what = msg

class XmlNoMemoryError(XmlException):
    """Raised when an operation runs out of memory.
    TBD -- should this be a Python MemoryError?
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = NO_MEMORY_ERROR
        self.what = msg

class XmlNullPointer(XmlException):
    """Raised when an operation is attempted on a null or uninitialized object.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = NULL_POINTER
        self.what = msg

class XmlTransactionError(XmlException):
    """Raised when there is a transaction-related error.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = TRANSACTION_ERROR
        self.what = msg

class XmlOperationInterrupted(XmlException):
    """Raised when a query operation is interrupted and does not complete.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = OPERATION_INTERRUPTED
        self.what = msg

class XmlOperationTimeout(XmlException):
    """Raised when a query operation times out.
    
    Attributes: see XmlException
    """
    def __init__(self, msg):
        self.exceptionCode = OPERATION_TIMEOUT
        self.what = msg


INTERNAL_ERROR = _dbxml.INTERNAL_ERROR
CONTAINER_OPEN = _dbxml.CONTAINER_OPEN
CONTAINER_CLOSED = _dbxml.CONTAINER_CLOSED
NULL_POINTER = _dbxml.NULL_POINTER
XML_PARSER_ERROR = _dbxml.INDEXER_PARSER_ERROR
DATABASE_ERROR = _dbxml.DATABASE_ERROR
QUERY_PARSER_ERROR = _dbxml.QUERY_PARSER_ERROR
QUERY_EVALUATION_ERROR = _dbxml.QUERY_EVALUATION_ERROR
LAZY_EVALUATION = _dbxml.LAZY_EVALUATION
DOCUMENT_NOT_FOUND = _dbxml.DOCUMENT_NOT_FOUND
CONTAINER_EXISTS = _dbxml.CONTAINER_EXISTS
UNKNOWN_INDEX = _dbxml.UNKNOWN_INDEX
INVALID_VALUE = _dbxml.INVALID_VALUE
VERSION_MISMATCH = _dbxml.VERSION_MISMATCH
EVENT_ERROR = _dbxml.EVENT_ERROR
CONTAINER_NOT_FOUND = _dbxml.CONTAINER_NOT_FOUND
TRANSACTION_ERROR = _dbxml.TRANSACTION_ERROR
UNIQUE_ERROR = _dbxml.UNIQUE_ERROR
NO_MEMORY_ERROR = _dbxml.NO_MEMORY_ERROR
OPERATION_INTERRUPTED = _dbxml.OPERATION_INTERRUPTED
OPERATION_TIMEOUT = _dbxml.OPERATION_TIMEOUT
%}
