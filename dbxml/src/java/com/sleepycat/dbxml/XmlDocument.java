//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2007 Oracle.  All rights reserved.


package com.sleepycat.dbxml;

import java.io.InputStream;
import java.util.Vector;

public class XmlDocument {
    protected Vector<XmlMetaData> metaData;
    protected long docID;
    protected int cid;
    protected Content content;
    /*
     * State information for the document, such as its transaction or temporary
     * database, is stored either in an XmlResults or in an XmlManager with an
     * optional XmlTransaction. The XmlResults is set if the document is created
     * from an XmlResults or XmlValue, the XmlManager is set if the document is
     * created from an XmlContainer, XmlManager, or XmlValue.
     */
    protected XmlManager manager;
    protected XmlTransaction txn;
    protected XmlResults results;

    protected XmlDocument() {
	docID = 0;
	cid = 0;
    }

    public XmlDocument(XmlDocument o) throws XmlException {
	docID = o.docID;
	cid = o.cid;
	if(o.metaData != null)
	    metaData = new Vector<XmlMetaData>(o.metaData);
	if(o.content != null)
	    content = new Content(o.content);
	manager = o.manager;
	txn = o.txn;
	results = o.results;
    }

    /** @deprecated as of 2.4 */
    @Deprecated 
    public void delete() {}

    public void setContentAsEventReader(XmlEventReader reader)
    throws XmlException {
    if (reader == null) throw new NullPointerException();
	initContent();
	content.setContentAsEventReader(reader);
    }

    public InputStream getContentAsInputStream() throws XmlException {
	return new XmlInputStreamWrap(getContentAsXmlInputStream());
    }

    public void setContentAsXmlInputStream(XmlInputStream adopted)
    throws XmlException {
    if (adopted == null) throw new NullPointerException();
	initContent();
	content.setContentAsXmlInputStream(adopted);
    }

    public boolean getMetaData(String uri, String name, XmlValue value)
    throws XmlException {
    if (uri == null) uri = "";
    if (name == null) name = "";
	initMetaData();
	for (int i = 0; i < metaData.size(); i++) {
	    XmlMetaData data = metaData.get(i);
	    if (uri.matches(data.get_uri()) && name.matches(data.get_name())
		    && !data.get_removed()) {
		XmlValue.setValue(value, data.get_value());
		return true;
	    }
	}
	XmlValue returned = HelperFunctions.getMetaData(uri, name, this);
	if(!returned.isNull()){
	    XmlValue.setValue(value, returned);
	    return true;
	}
	return false;
    }

    public void setName(String name) throws XmlException {
	String newName = name;
	if(newName == null)
	    newName = "";
	XmlValue value = new XmlValue(newName);
	setMetaData(dbxml_javaConstants.metaDataNamespace_uri,
		dbxml_javaConstants.metaDataName_name, value);
    }

    public String getName() throws XmlException {
	XmlValue value = new XmlValue();
	boolean exists = getMetaData(dbxml_javaConstants.metaDataNamespace_uri,
		dbxml_javaConstants.metaDataName_name, value);
	if (exists)
	    return value.asString();
	return "";
    }

    public void setContent(String content) throws XmlException {
	initContent();
	if (content == null) content = "";
	this.content.setContent(content);
    }

    public void setContent(byte[] content) throws XmlException {
	initContent();
	if (content == null) content = new byte[0];
	this.content.setContent(content);
    }

    public XmlInputStream getContentAsXmlInputStream() throws XmlException {
	initContent();
	if (content.isStream()){
	    XmlInputStream ins = content.getXmlInputStream();
	    content.stream = null;
	    content.type = NONE;
	    return ins;
	} else
	    return HelperFunctions.getContentAsXmlInputStream(this);
    }

    public XmlEventReader getContentAsEventReader() throws XmlException {
	initContent();
	if (content.isReader()){
	    XmlEventReader xer = content.getEventReader();
	    content.reader = null;
	    content.type = NONE;
	    return xer;
	} else
	    return HelperFunctions.getContentAsEventReader(this);
    }

    public void getContentAsEventWriter(XmlEventWriter writer)
    throws XmlException {
	initContent();
	try{
	    HelperFunctions.getContentAsEventWriter(this, writer);
	}catch(XmlException e){
	    writer.release();
	    throw e;
	}
	writer.release();
    }

    public void fetchAllData() 
    throws XmlException {
	materializeMetaData();
	getContent();
    }

    public void setMetaData(String uri, String name, XmlValue value)
    throws XmlException {
    if (value == null) throw new NullPointerException();
	addMetaData(uri, name, value, true, false);
    }

    public void removeMetaData(String uri, String name) throws XmlException {
	XmlValue temp = new XmlValue("x");
	addMetaData(uri, name, temp, false, true);
    }

    public void setMetaData(String uri, String name, byte[] value)
    throws XmlException {
	if (value == null)
		value = new byte[0];
	XmlValue data = new XmlValue(XmlValue.BINARY, value);
	addMetaData(uri, name, data, true, false);
    }

    public String getContentAsString() throws XmlException {
	initContent();
	if (content.isDBT())
	    return content.getString();
	else if (!content.hasContent() && docID == 0)
	    return "";
	else{
	    String results = HelperFunctions.getContentAsString(this);
	    if (isConstructed())
	    	content.setContent(results);
	    return results;
	}
    }

    public byte[] getContent() throws XmlException {
	initContent();
	if (content.isDBT())
	    return content.getDBT();
	else if (!content.hasContent() && docID == 0){
	    byte [] empty = {};
	    return empty;
	}else{
	    byte [] results = dbxml_javaJNI.getContent(this);
	    if (isConstructed())
	    	content.setContent(results);
	    return results;
	}
    }

    public byte[] getMetaData(String uri, String name) throws XmlException {
	XmlValue data = new XmlValue();
	if(getMetaData(uri, name, data)){
	    if(data.isBinary())
		return data.asBinary();
	    return data.asString().getBytes();
	}
	return null;
    }

    public XmlMetaDataIterator getMetaDataIterator() throws XmlException {
	materializeMetaData();
	return new XmlMetaDataIterator(this);
    }

    public boolean equals(XmlDocument other) throws XmlException {
	if(other == null)
	    return false;
	if(docID != 0 && docID == other.docID && cid == other.cid)
	    return true;
	else if(docID == 0 && other.docID == 0 && content == content 
		&& metaData == other.metaData)
	    return true;
	return false;
    }
    
    public XmlResults getResults() 
    {
    	return results;
    }

    //The rest of this class is for internal use.
    protected boolean isConstructed()
    {
    	if(results == null && docID == 0)
    		return true;
    	return false;
    }
    
    /* If both modified and removed are set to false then the meta data is 
     * only being added if it does not already exist.
     */
    protected void addMetaData(String uri, String name, XmlValue value, boolean modified, boolean removed)
    throws XmlException {
    if (uri == null) uri = "";
    if (name == null)  name = "";
	initMetaData();
	XmlMetaData data = null;
	for (int i = 0; i < metaData.size(); i++) {
	    data = metaData.get(i);
	    if (uri.matches(data.get_uri()) && name.matches(data.get_name())){
		if(modified || removed){
		    data.set_modified(modified);
		    data.set_removed(removed);
		    data.set_value(value);
		}
		return;
	    }
	}
	data = new XmlMetaData(uri, name, value);
	if(modified || removed){
	    data.set_modified(modified);
	    data.set_removed(removed);
	}
	metaData.add(data);
    }
    
    protected void addMetaData(String[] uri, String[] name, XmlValue[] value, boolean[] modified, boolean[] removed)
    throws XmlException {
    	if (uri == null) return;
    	for(int i = 0; i < uri.length; i++)
    		addMetaData(uri[i], name[i], value[i], modified[i], removed[i]);
    }

    protected XmlMetaData getMetaData(int index) {
	initMetaData();
	if(index >= metaData.size())
	    return null;
	return metaData.get(index);
    }

    protected void materializeMetaData() 
    throws XmlException {
	initMetaData();
	if(docID != 0 && cid != 0){
	    dbxml_javaJNI.materializeMetaData(this);
	}
    }

    protected XmlDocument(long documentId, int containerId, byte[] dbt) {
	metaData = new Vector<XmlMetaData>();
	docID = documentId;
	cid = containerId;
	if(dbt != null) {
		initContent();
		content.dbt = dbt;
		content.type = DBT;
	}
    }

    protected void copy(XmlDocument o) throws XmlException {
	docID = o.docID;
	cid = o.cid;
	if(content != null)
	    content.clear();
	if(o.content != null)
	    content = new Content(o.content);
	else
	    content = null;
	if(metaData != null)
	    metaData.clear();
	if(o.metaData != null)
	    metaData = new Vector<XmlMetaData>(o.metaData);
	else
	    metaData = null;
	results = o.results;
	manager = o.manager;
	txn = o.txn;
    }

    protected void copyID(XmlDocument o) throws XmlException {
	docID = o.docID;
	cid = o.cid;
	results = null;
	manager = null;
	txn = null;
	if(content != null)
	    content.clear();
	if(metaData != null)
	    metaData.clear();
    }

    protected void clearID() throws XmlException{
	docID = 0;
	cid = 0;
    }

    protected void clearContent() throws XmlException{
	if(content != null)
	    content.clear();
    }

    protected long getEventReaderPtr() {
	initContent();
	long ptr = XmlEventReader.getCPtr(content.getEventReader());
	content.reader.release();
	content.reader = null;
	content.type = NONE;
	return ptr;
    }

    protected long getInputStreamPtr() {
	initContent();
	long ptr = XmlInputStream.getCPtr(content.getXmlInputStream());
	content.stream = null;
	content.type = NONE;
	return ptr;
    }

    protected void setManager(XmlManager mgr)
    {
	manager = mgr;
    }

    protected void setTransaction(XmlTransaction transaction)
    {
	txn = transaction;
    }

    protected void setResults(XmlResults result)
    {
	results = result;
    }
    
    /*
     * Pack the meta data into an object array so it can be
     * easily passed between C++ and Java.  Only pass meta
     * data that is either removed or modifed.  All other meta
     * data can be gotten from a database.
     */
    private Object[] packMetaData()
    {
    	Object[] data = null;
    	if (metaData != null){
    		data = new Object[metaData.size()*4];
    		for(int i = 0; i < metaData.size(); i++)
    		{
    			XmlMetaData xmd = metaData.get(i);
    			if(xmd.modified || xmd.removed) {
    				data[i*4] = xmd.uri;
    				data[i*4+1] = xmd.name;
    				data[i*4+2] = xmd.value;
    				boolean[] state = new boolean[2];
    				state[0] = xmd.modified;
    				state[1] = xmd.removed;
    				data[i*4+3] = state;
    			}
    		}
    	}
    	return data;
    }
    
    /*
     * Combines the data of an XmlDocument so it can be passed easily between
     * C++ and Java.  The makeup of the Object[] are as follows:
     * 0 - long[] docID, content pointer, XmlResults pointer, XmlManager pointer, 
     * XmlTransaction pointer
     * 1 - int[] cid, content type
     * 2 - byte[] dbt
     * 3 - MetaData
     * 
     * Some values will be null or non-existent if they are not relevant to 
     * the current document object.
     */
    protected Object[] pack()
    {
    	Object[] data = new Object[4];
    	int contentType;
    	if (content == null)
    		contentType = NONE;
    	else
    		contentType = content.type;
    	long[] state = new long[5];
    	state[0] = docID;
    	if (contentType == READER)
    		state[1] = getEventReaderPtr();
    	else if (contentType == INPUTSTREAM)
    		state[1] = getInputStreamPtr();
    	state[2] = XmlResults.getCPtr(results);
    	state[3] = XmlManager.getCPtr(manager);
    	state[4] = XmlTransaction.getCPtr(txn);
    	data[0] = state;
    	int[] cidNType = new int[2];
    	cidNType[0] = cid;
    	cidNType[1] = contentType;
    	data[1] = cidNType;
    	if (contentType == DBT)
    		data[2] = content.dbt;
    	data[3] = packMetaData();
    	return data;
    }
    
    private void initContent()
    {
	if(content == null)
	    content = new Content();
    }
    
    private void initMetaData()
    {
	if(metaData == null)
	    metaData = new Vector<XmlMetaData>();
    }

    class Content {
	public XmlEventReader reader;
	public XmlInputStream stream;
	public byte[] dbt;
	public int type;
	public static final String UTF8 = "UTF-8";

	public Content() {
	    type = NONE;
	}

	public Content(Content c) {
	    type = c.type;
	    reader = c.reader;
	    stream = c.stream;
	    dbt = c.dbt;
	}

	protected void clear() throws XmlException {
	    if (reader != null) {
		reader.close();
		reader = null;
	    }
	    if (stream != null) {
		stream.delete();
		stream = null;
	    }
	    if (dbt != null)
		dbt = null;
	    type = NONE;
	}

	public boolean hasContent() {
	    return type != NONE;
	}

	public boolean isStream() {
	    return type == INPUTSTREAM;
	}

	public boolean isReader() {
	    return type == READER;
	}

	public boolean isDBT() {
	    return type == DBT;
	}

	public void setContentAsEventReader(XmlEventReader rdr)
	throws XmlException {
	    clear();
	    type = READER;
	    reader = rdr;
	}

	public void setContentAsXmlInputStream(XmlInputStream adopted)
	throws XmlException {
	    clear();
	    type = INPUTSTREAM;
	    stream = adopted;
	}

	public void setContent(String content) throws XmlException {
	    clear();
	    type = DBT;
	    try {
		dbt = content.getBytes(UTF8);
	    } catch (Exception e) {} //never called
	}

	public void setContent(byte[] content) throws XmlException {
	    clear();
	    type = DBT;
	    dbt = content;
	}

	public XmlEventReader getEventReader(){
	    return reader;
	}

	public XmlInputStream getXmlInputStream(){
	    return stream;
	}

	public byte[] getDBT() {
	    return dbt;
	}

	public String getString() {
	    String content = null;
	    try {
		content = new String(dbt, UTF8);
	    } catch (Exception e) {} //never throws
	    return content;
	}
    }

    public final static int NONE = 0;
    public final static int DBT = 1;
    public final static int INPUTSTREAM = 2;
    public final static int DOM = 4;
    public final static int READER = 8;
    public final static int VALIDATE = 1;
    public final static int DONT_VALIDATE = 2;
    public final static int WF_ONLY = 3;
}
