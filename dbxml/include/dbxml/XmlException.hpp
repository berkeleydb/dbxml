//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLEXCEPTION_HPP
#define	__XMLEXCEPTION_HPP

#include "XmlPortability.hpp"
#include <exception>
#include <string>

/*
 * IMPORTANT: the file, XmlException.java, is matched to
 * this file, and is maintained by hand.  Any changes to
 * the enumerations here must be mirrored there.
 */

class XQException;
class LocationInfo;

namespace DbXml
{

class DBXML_EXPORT XmlException : public std::exception
{
public:
	/// Error codes
	enum ExceptionCode
	{
		INTERNAL_ERROR,  ///< An internal error occured.
		CONTAINER_OPEN,  ///< The container is open.
		CONTAINER_CLOSED,  ///< The container is closed.
		NULL_POINTER,     ///< null pointer exception
		INDEXER_PARSER_ERROR,  ///< XML Indexer could not parse a document.
		DATABASE_ERROR,  ///< Berkeley DB reported a database problem.
		QUERY_PARSER_ERROR,  ///< The query parser was unable to parse the expression.
		UNUSED1_ERROR,  ///< Unused
		QUERY_EVALUATION_ERROR,  ///< The query evaluator was unable to execute the expression.
		UNUSED2_ERROR,  ///< Unused
		LAZY_EVALUATION,  ///< XmlResults is lazily evaluated.
		DOCUMENT_NOT_FOUND,  ///< The specified document could not be found
		CONTAINER_EXISTS,  ///< The container already exists.
		UNKNOWN_INDEX,  ///< The indexing strategy name is unknown.
		INVALID_VALUE, ///< An invalid parameter was passed.
		VERSION_MISMATCH, ///< The container version and the dbxml library version are not compatible.
		EVENT_ERROR, ///< Error using the event reader
		CONTAINER_NOT_FOUND, ///< The specified container could not be found
		TRANSACTION_ERROR, ///< An XmlTransaction has already been committed or aborted
		UNIQUE_ERROR, ///< A uniqueness constraint has been violated
		NO_MEMORY_ERROR, ///< Unable to allocate memory
		OPERATION_TIMEOUT, ///< An operation timed out
		OPERATION_INTERRUPTED ///< An operation was explicitly interrupted
	};
	
	XmlException(ExceptionCode err, const std::string &description, const char *file = 0, int line = 0);
	XmlException(ExceptionCode err, const char *description, const char *file = 0, int line = 0);
	XmlException(int dberr, const char *file = 0, int line = 0);
	XmlException(const XmlException &);
	virtual ~XmlException() throw();
	virtual const char *what() const throw();
	/// Get the the error code for this exception.
	ExceptionCode getExceptionCode() const
	{
		return exceptionCode_;
	}
	/// Get the Berkeley DB errno for the DATABASE_ERROR.
	int getDbErrno() const
	{
		return dberr_;
	}

	const char *getQueryFile() const { return qFile_; }
	int getQueryLine() const { return qLine_; }
	int getQueryColumn() const { return qCol_; }

	/** @name Private Methods (for internal use) */
	// @{

	XmlException(ExceptionCode err, const XQException &e, const char *file = 0, int line = 0);
	void setLocationInfo(const LocationInfo *info);

	//@}

private:
	XmlException &operator = (const XmlException &);
	void describe();

	ExceptionCode exceptionCode_;
	int dberr_; // The Berkeley DB errno
	char *description_;

	char *qFile_;
	int qLine_, qCol_;
	
	const char *file_; // The file where the bad thing happened.
	int line_; // The line where the bad thing happened.

	char *text_;
};

}

#endif

