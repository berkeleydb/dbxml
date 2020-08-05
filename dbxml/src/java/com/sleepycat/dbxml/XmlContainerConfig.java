
//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2009 Oracle.  All rights reserved.




package com.sleepycat.dbxml;
import com.sleepycat.db.DatabaseConfig;
import com.sleepycat.db.internal.DbConstants;

//Adds configuration for:
//node indexing
//Use DatabaseConfig for:
//checksum, encryption, transactional configuration

public class XmlContainerConfig extends DatabaseConfig {

    public static final XmlContainerConfig DEFAULT = new XmlContainerConfig();
    public static final String DEFAULT_COMPRESSSION = "DEFAULT";
    public static final String NO_COMPRESSSION = "NONE";
    public static final int UseDefault = 0;
    public static final int On = 1;
    public static final int Off = 2;

    public XmlContainerConfig() {
	setMode(0);
    }

    public String getCompression() {
	return compression;
    }

    public XmlContainerConfig setCompression(String compressionName) {
	compression = compressionName;
	return this;
    }

    /** @deprecated use getIndexNodesFlag  */
    @Deprecated 
    public boolean getIndexNodes() {
	if (indexNodesFlag == Off)
	    return false;
	else if (indexNodesFlag == On)
	    return true;
	if (nodeContainer == XmlContainer.NodeContainer)
	    return true;
	else
	    return false;
    }

    public int getIndexNodesFlag() {
	return indexNodesFlag;
    }

    /** @deprecated use setIndexNodes(int) */
    @Deprecated 
    public XmlContainerConfig setIndexNodes(boolean value) {
	if(value)
	    indexNodesFlag = On;
	else
	    indexNodesFlag = Off;
	return this;
    }

    public XmlContainerConfig setIndexNodes(int value) {
	indexNodesFlag = value;
	return this;
    }

    /** @deprecated use getStatisticsEnabledFlag  */
    @Deprecated 
    public boolean getStatisticsEnabled() {
	if(statisticsFlag == Off)
	    return false;
	else
	    return true;
    }

    public int getStatisticsEnabledFlag() {
	return statisticsFlag;
    }

    /** @deprecated use setStatisticsEnabled(int) */
    @Deprecated 
    public XmlContainerConfig setStatisticsEnabled(boolean value) {
	if(value)
	    statisticsFlag = On;
	else
	    statisticsFlag = Off;
	return this;
    }

    public XmlContainerConfig setStatisticsEnabled(int value) {
	statisticsFlag = value;
	return this;
    }

    /** @deprecated use getContainerType 
     */
    @Deprecated 
    public boolean getNodeContainer() {
	if (nodeContainer == XmlContainer.NodeContainer)
	    return true;
	return false;
    }

    public int getContainerType() {
	return nodeContainer;
    }

    /** @deprecated use setContainerType
     */
    @Deprecated public XmlContainerConfig setNodeContainer(boolean value) {
	if (value)
	    nodeContainer = XmlContainer.NodeContainer;
	else
	    nodeContainer = XmlContainer.WholedocContainer;
	return this;
    }

    public XmlContainerConfig setContainerType(int value) {
	nodeContainer = value;
	return this;
    }

    public boolean getAllowValidation() {
	return allowValidation;
    }

    public XmlContainerConfig setAllowValidation(boolean value) {
	this.allowValidation = value;
	return this;
    }

    public void setSequenceIncrement(int incr) {
	sequenceIncr = incr;
    }

    public int getSequenceIncrement() {
	return sequenceIncr;
    }

    /* package */
    
    protected XmlContainerConfig(int[] data)
    {
    	setMode(data[0]);
    	setContainerType(data[1]);
    	setXmlFlags(data[2]);
    	setDbOpenFlags(data[3]);
    	setPageSize(data[4]);
    	setSequenceIncrement(data[5]);
    	setDbSetFlags(data[6]);
    	setSeqFlags(data[7]);
    }
    
    /* packs the XmlContainerConfig data into an array so
     * it can be easily passed to the C++.
     */
    protected int [] pack()
    {
	int [] data = new int[8];

	data[0] = (int)getMode(); //mode
	data[1] = nodeContainer; //container type
	data[2] = getXmlFlags(); //Flags beginning in DBXML_
	data[3] = getDbOpenFlags(); //Flags used to open a database
	data[4] = getPageSize(); //Page size
	data[5] = sequenceIncr; //Sequence increment
	data[6] = getDbSetFlags(); //Flags used for db->set_flags
	data[7] = getSeqFlags(); //Flags used for opening a sequence database
	return data;           
    }

    protected void setThreaded(boolean value)
    {
	isThreaded = value;
    }
    
    private void setXmlFlags(int xmlFlags)
    {
    	if ((xmlFlags & dbxml_javaConstants.DBXML_INDEX_NODES) != 0)
    		indexNodesFlag = On;
    	else if ((xmlFlags & dbxml_javaConstants.DBXML_NO_INDEX_NODES) != 0)
    		indexNodesFlag = Off;
    	else
    		indexNodesFlag = UseDefault;
    	if ((xmlFlags & dbxml_javaConstants.DBXML_STATISTICS) != 0)
    		statisticsFlag = On;
    	else if ((xmlFlags & dbxml_javaConstants.DBXML_NO_STATISTICS) != 0)
    		statisticsFlag = Off;
    	else
    		statisticsFlag = UseDefault;
    	setTransactional((xmlFlags & dbxml_javaConstants.DBXML_TRANSACTIONAL) != 0);
    	setAllowValidation((xmlFlags & dbxml_javaConstants.DBXML_ALLOW_VALIDATION) != 0);
    	setChecksum((xmlFlags & dbxml_javaConstants.DBXML_CHKSUM) != 0);
    	if ((xmlFlags & dbxml_javaConstants.DBXML_ENCRYPT) != 0)
    		setEncrypted("");
    	else
    		setEncrypted(null);
    }
    
    private void setDbOpenFlags(int dbFlags)
    {
    	setAllowCreate((dbFlags & DbConstants.DB_CREATE) != 0);
    	setExclusiveCreate((dbFlags & DbConstants.DB_EXCL) != 0);
    	setNoMMap((dbFlags & DbConstants.DB_NOMMAP) != 0);
    	setReadOnly((dbFlags & DbConstants.DB_RDONLY) != 0);
    	setMultiversion((dbFlags & DbConstants.DB_MULTIVERSION) != 0);
    	setReadUncommitted((dbFlags & DbConstants.DB_READ_UNCOMMITTED) != 0);
    	isThreaded = (dbFlags & DbConstants.DB_THREAD) != 0;
    }
    
    private void setDbSetFlags(int dbFlags)
    {
    	if ((dbFlags & dbxml_javaConstants.DBXML_ENCRYPT) != 0)
    		setEncrypted("");
    	else
    		setEncrypted(null);
    	setChecksum((dbFlags & dbxml_javaConstants.DBXML_CHKSUM) != 0);
    	setTransactionNotDurable((dbFlags & DbConstants.DB_TXN_NOT_DURABLE) != 0);
    }
    
    private void setSeqFlags(int dbFlags)
    {
    	setAllowCreate((dbFlags & DbConstants.DB_CREATE) != 0);
    	setExclusiveCreate((dbFlags & DbConstants.DB_EXCL) != 0);
    	isThreaded = (dbFlags & DbConstants.DB_THREAD) != 0;
    }

    private int getXmlFlags() {
	int flags = 0;
	// if indexNodesFlag isn't set, use default based on container type
	if (indexNodesFlag == On)
	    flags |= dbxml_javaConstants.DBXML_INDEX_NODES;
	else if (indexNodesFlag == Off)
	    flags |= dbxml_javaConstants.DBXML_NO_INDEX_NODES;
	if (statisticsFlag == On)
	    flags |= dbxml_javaConstants.DBXML_STATISTICS;
	else if (statisticsFlag == Off)
	    flags |= dbxml_javaConstants.DBXML_NO_STATISTICS;
	// Translations from DB
	if (getTransactional())
	    flags |= dbxml_javaConstants.DBXML_TRANSACTIONAL;
	if (getAllowValidation())
	    flags |= dbxml_javaConstants.DBXML_ALLOW_VALIDATION;
	if (getChecksum())
	    flags |= dbxml_javaConstants.DBXML_CHKSUM;
	if (getEncrypted())
	    flags |= dbxml_javaConstants.DBXML_ENCRYPT;
	return flags;
    }
    private int getDbOpenFlags() {
	int flags = 0;
	if (getAllowCreate())
	    flags |= DbConstants.DB_CREATE;
	if (getExclusiveCreate())
	    flags |= DbConstants.DB_EXCL;
	if (getNoMMap())
	    flags |= DbConstants.DB_NOMMAP;
	if (getReadOnly())
	    flags |= DbConstants.DB_RDONLY;
	if (getMultiversion())
	    flags |= DbConstants.DB_MULTIVERSION;
	if (getReadUncommitted())
	    flags |= DbConstants.DB_READ_UNCOMMITTED;
	if (isThreaded)
	    flags |= DbConstants.DB_THREAD;
	return flags;
    }
    
    private int getDbSetFlags() {
    	int flags = 0;
    	if (getChecksum())
    	    flags |= dbxml_javaConstants.DBXML_CHKSUM;
    	if (getEncrypted())
    	    flags |= dbxml_javaConstants.DBXML_ENCRYPT;
    	if (getTransactionNotDurable())
    	    flags |= DbConstants.DB_TXN_NOT_DURABLE;
    	return flags;
    }
    
    private int getSeqFlags() {
    	int flags = 0;
    	if (getAllowCreate())
    	    flags |= DbConstants.DB_CREATE;
    	if (getExclusiveCreate())
    	    flags |= DbConstants.DB_EXCL;
    	if (isThreaded)
    	    flags |= DbConstants.DB_THREAD;
    	return flags;
        }

    private String compression = DEFAULT_COMPRESSSION; 
    /* Flags */
    private int indexNodesFlag = UseDefault;
    private int statisticsFlag = UseDefault;
    private boolean isThreaded = false;
    private int nodeContainer = XmlContainer.NodeContainer;
    private boolean allowValidation = false;
    private int sequenceIncr = 5;
}
