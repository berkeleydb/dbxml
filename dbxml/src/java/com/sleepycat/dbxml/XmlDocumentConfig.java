//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
//

package com.sleepycat.dbxml;
import com.sleepycat.db.LockMode;
import com.sleepycat.db.XmlHelper;
import com.sleepycat.db.DatabaseException;

public class XmlDocumentConfig implements Cloneable {

    public static final XmlDocumentConfig DEFAULT = new XmlDocumentConfig();

    public boolean getLazyDocs() {
	return lazyDocs;
    }

    public XmlDocumentConfig setLazyDocs(final boolean value) {
	this.lazyDocs = value;
	return this;
    }

    public boolean getDocumentProjection() {
	return documentProjection;
    }

    public XmlDocumentConfig setDocumentProjection(final boolean value) {
	this.documentProjection = value;
	return this;
    }

    public boolean getAutoCommit() {
	return autoCommit;
    }

    public XmlDocumentConfig setAutoCommit(final boolean value) {
	this.autoCommit = value;
	return this;
    }

    public boolean getReverseOrder() {
	return reverseOrder;
    }

    public XmlDocumentConfig setReverseOrder(final boolean value) {
	this.reverseOrder = value;
	return this;
    }

    public boolean getCacheDocuments() {
	return cacheDocuments;
    }

    public XmlDocumentConfig setCacheDocuments(final boolean value) {
	this.cacheDocuments = value;
	return this;
    }

    public boolean getDocumentsOnly() {
	return documentsOnly;
    }

    public XmlDocumentConfig setDocumentsOnly(final boolean value) {
	this.documentsOnly = value;
	return this;
    }

    public boolean getWellFormedOnly() {
	return wellFormedOnly;
    }

    public XmlDocumentConfig setWellFormedOnly(final boolean value) {
	this.wellFormedOnly = value;
	return this;
    }

    public boolean getGenerateName() {
	return genName;
    }

    public XmlDocumentConfig setGenerateName(final boolean value) {
	this.genName = value;
	return this;
    }

    // com.sleepycat.db.LockMode encapsulates one of:
    // DB_DIRTY_READ/DB_READ_UNCOMMITTED, 
    // DB_DEGREE_2/DB_READ_COMMITTED, DB_RMW
    public LockMode getLockMode() {
	return lockMode;
    }

    public XmlDocumentConfig setLockMode(LockMode lmode) {
	this.lockMode = lmode;
	return this;
    }

    /* package */
    int makeFlags() {
	int flags = 0;
	if (lazyDocs)
	    flags |= dbxml_javaConstants.DBXML_LAZY_DOCS;
	if (documentProjection)
	    flags |= dbxml_javaConstants.DBXML_DOCUMENT_PROJECTION;
	if (!autoCommit)
	    flags |= dbxml_javaConstants.DBXML_NO_AUTO_COMMIT;
	if (genName)
	    flags |= dbxml_javaConstants.DBXML_GEN_NAME;
	if (reverseOrder)
	    flags |= dbxml_javaConstants.DBXML_REVERSE_ORDER;
	if (cacheDocuments)
	    flags |= dbxml_javaConstants.DBXML_CACHE_DOCUMENTS;
	if (documentsOnly)
	    flags |= dbxml_javaConstants.DBXML_NO_INDEX_NODES;
	if (wellFormedOnly)
	    flags |= dbxml_javaConstants.DBXML_WELL_FORMED_ONLY;
	if (lockMode != null) {
	    try {
		flags |= XmlHelper.getFlag(lockMode);
	    } catch (DatabaseException e) {}
	}
	return flags;
    }

    /* Flags */
    private boolean lazyDocs = false;
    private boolean documentProjection = false;
    private boolean genName = false;
    private LockMode lockMode = null;
    private boolean wellFormedOnly = false;
    /* for XmlIndexLookup.execute() */
    private boolean reverseOrder = false;
    private boolean cacheDocuments = false;
    private boolean documentsOnly = false;
    private boolean autoCommit = true;
}
