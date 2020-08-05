//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
// IMPORTANT: the enumerations here must match those in 
// include/dbxml/XmlException.hpp
//

package com.sleepycat.dbxml;
import com.sleepycat.db.DatabaseException;

/**
 *  The XmlException class represents an error condition that has
 *  occurred within the Berkeley DB XML system. The system throws an
 *  XmlException if an API method call results in an error condition.
 *  The XmlException class exposes the following methods:</p> <hr
 *  size=1 noshade> <h3>Description: XmlException.toString</h3> <hr
 *  size=1 noshade> <h3>Description: XmlException.getErrorCode</h3>
 *  <hr size=1 noshade> <h3>Description: XmlException.getDbError</h3>
 */
public class XmlException extends com.sleepycat.db.DatabaseException {
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_CLOSED = 2;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_EXISTS = 12;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_OPEN = 1;
    /**
     *
     *</ul>
     *
     */
    public final static int DATABASE_ERROR = 5;
    /**
     *
     *</ul>
     *
     */
    public final static int DOCUMENT_NOT_FOUND = 11;
    /**
     *
     *</ul>
     *
     */
    public final static int INDEXER_PARSER_ERROR = 4;
    /**
     *
     *</ul>
     *
     */
    public final static int INTERNAL_ERROR = 0;
    /**
     *
     *</ul>
     *
     */
    public final static int INVALID_VALUE = 14;
    /**
     *
     *</ul>
     *
     */
    public final static int NULL_POINTER = 3;
    /**
     *
     *</ul>
     *
     */
    public final static int VERSION_MISMATCH = 15;
    /**
     *
     *</ul>
     *
     */
    public final static int LAZY_EVALUATION = 10;
    /**
     *
     *</ul>
     *
     */
    public final static int UNKNOWN_INDEX = 13;
    /**
     *
     *</ul>
     *
     */
    public final static int QUERY_EVALUATION_ERROR = 8;
    /**
     *
     *</ul>
     *
     */
    public final static int QUERY_PARSER_ERROR = 6;
    /**
     *
     *</ul>
     *
     */
    public final static int EVENT_ERROR = 16;
    /**
     *
     *</ul>
     *
     */
    public final static int CONTAINER_NOT_FOUND = 17;
    /**
     *
     *</ul>
     *
     */
    public final static int TRANSACTION_ERROR = 18;
    /**
     *
     *</ul>
     *
     */
    public final static int UNIQUE_ERROR = 19;
    /**
     *
     *</ul>
     *
     */
    public final static int NO_MEMORY_ERROR = 20;
    /**
     *
     *</ul>
     *
     */
    public final static int OPERATION_TIMEOUT = 21;
    /**
     *
     *</ul>
     *
     */
    public final static int OPERATION_INTERRUPTED = 22;

    final static String[] xml_strerror = {
            "INTERNAL_ERROR",
            "CONTAINER_OPEN",
            "CONTAINER_CLOSED",
            "NULL_POINTER",
            "INDEXER_PARSER_ERROR",
            "DATABASE_ERROR",
            "QUERY_PARSER_ERROR",
            "UNUSED1",
            "QUERY_EVALUATION_ERROR",
            "UNUSED2",
            "LAZY_EVALUATION",
            "DOCUMENT_NOT_FOUND",
            "CONTAINER_EXISTS",
            "UNKNOWN_INDEX",
            "INVALID_VALUE",
            "VERSION_MISMATCH",
            "EVENT_ERROR",
            "CONTAINER_NOT_FOUND",
            "TRANSACTION_ERROR",
            "UNIQUE_ERROR",
            "NO_MEMORY_ERROR",
            "OPERATION_TIMEOUT",
            "OPERATION_INTERRUPTED"
    };
    private DatabaseException dbexc;

    private int errcode;
    private int qLine;
    private int qColumn;


    public XmlException(int errcode, String description) {
        super(description, 0);
        this.errcode = errcode;
	this.dbexc = dbexc;
	this.qLine = 0;
	this.qColumn = 0;
    }

    public XmlException(int errcode, String description, 
			DatabaseException dbexc, int dberr,
			int queryLine, int queryColumn) {
        super(description, dberr);
        this.errcode = errcode;
	this.dbexc = dbexc;
	this.qLine = queryLine;
	this.qColumn = queryColumn;
    }

    /* this is mostly for compatibility with pre-2.3 */
    public XmlException(int errcode, String description, 
			DatabaseException dbexc, int dberr) {
        super(description, dberr);
        this.errcode = errcode;
	this.dbexc = dbexc;
	this.qLine = 0;
	this.qColumn = 0;
    }

    /**
     * @return    The getDatabaseException() method retrieves a Berkeley DB
     *  DatabaseException object associated with this exception if
     *  one is present.  If this is a pure BDB XML exception, it returns
     *  null.</p>
     */
    public DatabaseException getDatabaseException() {
        return dbexc;
    }

    /**
     * @return    The getErrorCode() method returns the exception
     *      code.</p>
     */
    public int getErrorCode() {
        return errcode;
    }


    public String toString() {
        return super.toString() + ", errcode = " + xml_strerror[errcode];
    }
 
   /**
     * @return    The getDbError() method is deprecated.
     *  It returns the Berkeley DB error number assoicated with the
     *  exception if the ExceptionCode is DATABASE_ERROR.  If
     *  returns 0 if the ExceptionCode is not DATABASE_ERROR.</p>
     */
    public int getDbError() {
	if (dbexc != null)
	    return dbexc.getErrno();
        return 0;
    }

    /**
     * @return    The getQueryLine() method returns the line
     *  of the query expression that contains the error causing
     *  the exception, in the case of QUERY_PARSER_ERROR.</p>
     */
    public int getQueryLine() {
        return qLine;
    }

    /**
     * @return    The getQueryColumn() method returns the column
     *  within the line of the query expression that contains the error causing
     *  the exception, in the case of QUERY_PARSER_ERROR.</p>
     */
    public int getQueryColumn() {
        return qColumn;
    }

}
