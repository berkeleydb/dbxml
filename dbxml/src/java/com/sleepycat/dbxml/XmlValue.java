
//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2009 Oracle.  All rights reserved.



package com.sleepycat.dbxml;

import java.util.*;

import com.sleepycat.dbxml.XmlDocument.Content;

public class XmlValue {
    protected Value value;
    protected int valueType;

    public XmlValue()
    {
	valueType = NONE;
	value = new Value();
    }

    public XmlValue(XmlValue other) throws XmlException
    {
	valueType = other.getType();
	Class<? extends Value> valueClass = other.value.getClass();
	String simpleName = valueClass.getSimpleName();
	if(simpleName.matches("Value")){
	    value = new Value();
	}else if(simpleName.matches("StringValue")){
	    value = new StringValue((StringValue)other.value);
	}else if(simpleName.matches("NodeValue")){
	    value = new NodeValue((NodeValue)other.value);
	}else if(simpleName.matches("BinaryValue")){
	    value = new BinaryValue((BinaryValue)other.value);
	}else
	    throw (new XmlException(XmlException.INVALID_VALUE, "Invalid XmlValue object."));
    }

    public XmlValue(String v) throws XmlException
    {
	valueType = STRING;
	value = new StringValue(v);
    }

    public XmlValue(double v) throws XmlException
    {
	valueType = DOUBLE;
	String doubleString;
	try{
	    doubleString = Double.toString(v);
	}catch(Exception e){
	    doubleString = Double.toString(Double.NaN);
	}
	value = new StringValue(doubleString);
    }

    public XmlValue(boolean v) throws XmlException
    {
	valueType = BOOLEAN;
	String boolString;
	try{
	    boolString = Boolean.toString(v);
	}catch(Exception e){
	    boolString = Boolean.toString(false);
	}
	value = new StringValue(boolString);
    }

    public XmlValue(XmlDocument document) throws XmlException
    {
    Content con = document.content;
    document.content = null; //Prevents the content from being consumed
	XmlValue xmlvalue = HelperFunctions.createDocumentValue(document);
	document.content = con;
	valueType = xmlvalue.getType();
	value = new NodeValue((NodeValue)xmlvalue.value);
	((NodeValue)value).setDocument(document);
	((NodeValue)value).setResult(document.results);
	((NodeValue)value).setManager(document.manager);
	((NodeValue)value).setTransaction(document.txn);
    }

    public XmlValue(int type, String v) throws XmlException
    {
	if (type <= ANY_SIMPLE_TYPE){
	    valueType = NONE;
	    value = new Value();
	}else{
	    HelperFunctions.validateString(type, v);
	    valueType = type;
	    value = new StringValue(v);
	}
    }

    public XmlValue(String typeURI, String typeName, String v) throws XmlException
    {
	value = new StringValue(typeURI, typeName, v);
    }

    public XmlValue(int type, byte[] dbt) throws XmlException
    {
	if (type <= ANY_SIMPLE_TYPE){
	    valueType = NONE;
	    value = new Value();
	}else{
	    valueType = type;
	    if(valueType != BINARY){
		String strValue = new String(dbt);
		HelperFunctions.validateString(valueType, strValue);
		value = new StringValue(strValue);
	    }else{
		value = new BinaryValue(dbt);
	    }
	}
    }
	
    /** @deprecated as of 2.4 */
    @Deprecated 
    public void delete(){}
	
    public int getType() throws XmlException
    {
	return valueType;
    }

    public String getTypeURI() throws XmlException
    {
	return value.getTypeURI();
    }

    public String getTypeName() throws XmlException
    {
	return value.getTypeName();
    }

    public boolean isNull() throws XmlException
    {
	return valueType == NONE;
    }

    public boolean isType(int type) throws XmlException
    {
	return this.valueType == type;
    }

    public boolean isNumber() throws XmlException 
    {
	return isType(DECIMAL) || isType(DOUBLE) || isType(FLOAT);
    }

    public boolean isString()  throws XmlException
    {
	return isType(STRING);
    }

    public boolean isBoolean() throws XmlException 
    {
	return isType(BOOLEAN);
    }

    public boolean isBinary() throws XmlException 
    {
	return isType(BINARY);
    }

    public boolean isNode() throws XmlException
    {
	return isType(NODE);
    }

    public double asNumber() throws XmlException
    {
	return value.asNumber();
    }

    public String asString() throws XmlException
    {
	return value.asString();
    }

    public boolean asBoolean() throws XmlException
    {
	return value.asBoolean();
    }

    public byte[] asBinary() throws XmlException { 
	return value.asBinary(); 
    }
    
    public boolean equals(XmlValue value) throws XmlException
    {
	return (valueType == value.getType() && this.value.equals(value.value));
    }
    
    public static void setValue(XmlValue to, XmlValue from) throws XmlException
    {
	to.setValue(from);
    }

    //Functions for XmlValue of type NODE
    public XmlDocument asDocument() throws XmlException
    {
	return value.asDocument();
    }

    public XmlEventReader asEventReader() throws XmlException
    {
	return value.asEventReader();
    }

    public String getNodeHandle() throws XmlException
    {
	return value.getNodeHandle();
    }

    public String getNodeName() throws XmlException
    {
	return value.getNodeName();
    }

    public String getNodeValue() throws XmlException
    {
	return value.getNodeValue();
    }

    public String getNamespaceURI() throws XmlException
    {
	return value.getNamespaceURI();
    }

    public String getPrefix() throws XmlException
    {
	return value.getPrefix();
    }

    public String getLocalName() throws XmlException
    {
	return value.getLocalName();
    }

    public short getNodeType() throws XmlException
    {
	return value.getNodeType();
    }

    public XmlValue getParentNode() throws XmlException
    {
	return value.getParentNode();
    }

    public XmlValue getFirstChild() throws XmlException
    {
	return value.getFirstChild();
    }

    public XmlValue getLastChild() throws XmlException
    {
	return value.getLastChild();
    }

    public XmlValue getPreviousSibling() throws XmlException
    {
	return value.getPreviousSibling();
    }

    public XmlValue getNextSibling() throws XmlException
    {
	return value.getNextSibling();
    }

    public XmlResults getAttributes() throws XmlException
    {
	return value.getAttributes();
    }

    public XmlValue getOwnerElement() throws XmlException
    {
	return value.getOwnerElement();
    }
    
    public XmlResults getResults()
    {
	if(valueType == NODE)
	    return ((NodeValue)value).results;
	else
	    return null;
    }
    
    //Functions for internal use only 
    protected void setResult(XmlResults result)
    {
	if(valueType == NODE)
	    ((NodeValue)value).setResult(result);
    }
  
    protected long getReturnResultPtr()
    {
	if(valueType == NODE)
	    return ((NodeValue)value).getReturnResultPtr();
	return 0;
    }
  
    protected XmlResults getResult()throws XmlException
    {
	if(valueType == NODE)
	    return ((NodeValue)value).getResult();
	else
	    return null;
    }
  
    protected long getResultPtr()throws XmlException
    {
	if(valueType == NODE)
	    return ((NodeValue)value).getResultPtr();
	else
	    return 0;
    }
  
    protected void setManager(XmlManager mgr)
    {
	if(valueType == NODE)
	    ((NodeValue)value).setManager(mgr);
    }
  
    protected void setTransaction(XmlTransaction txn)
    {
	if(valueType == NODE)
	    ((NodeValue)value).setTransaction(txn);
    }
    
    protected void setDocument(XmlDocument doc)
    {
	if(valueType == NODE)
	    ((NodeValue)value).setDocument(doc);
    }
    
    /*
     * Used to condense the data into a form that is easier to pass
     * from Java to C++.  The content goes as follows:
     * 0 - int[] type, node type
     * 1 - String string value, or byte[] node ID or binary value
     * 2 - XmlDocument for a node, or String type name
     * 3 - long[] XmlResult, XmlManager, XmlTransaction, return XmlResult pointers
     * for a node, String type uri
     */
    protected Object[] pack()
    {
    	Object[] data = null;
    	Class<? extends Value> valueClass = value.getClass();
    	String simpleName = valueClass.getSimpleName();
    	int[] type = new int[1];
    	type[0] = valueType;
    	if(simpleName.matches("Value")){
    		data = new Object[1];
    		data[0] = type;
    	    return data; //All values are null
    	}else if(simpleName.matches("StringValue")){
    	    StringValue svalue = (StringValue)value;
    	    /*
    	     * If the type name and uri are specified pass them back,
    	     * otherwise only return the string value
    	     */
    	    if (svalue.typeName == null && svalue.typeUri == null) {
    	    	data = new Object[2];
    	    } else {
    	    	data = new Object[4];
    	    	data[2] = svalue.typeName;
        	    data[3] = svalue.typeUri;
    	    }
    	    data[0] = type;
    	    data[1] = svalue.value;
    	}else if(simpleName.matches("NodeValue")){
    		/*
    		 * Store the node value's ID, node type, 
    		 * the XmlDocument (which can be null), state information in the form
    		 * of the pointers to the XmlResult, XmlManager, and XmlTransaction, 
    		 * and a reference to this object.
    		 */
    		NodeValue nvalue = (NodeValue)value;
    		data = new Object[4];
    		type = new int[2];
    		type[0] = valueType;
    		type[1] = nvalue.nodeType;
    		data[0] = type;
    		data[1] = nvalue.ID;
    		data[2] = nvalue.document;
    		long[] state = new long[4];
    		state[0] = nvalue.getResultPtr();
    		state[1] = nvalue.getManagerPtr();
    		state[2] = nvalue.getTransactionPtr();
    		state[3] = nvalue.getReturnResultPtr();
    		data[3] = state;
    	}else if(simpleName.matches("BinaryValue")){
    		/*
    		 * For binary values store the byte array and the
    		 * reference to this object.
    		 */
    	    data = new Object[3];
    	    data[0] = type;
    	    BinaryValue bvalue = (BinaryValue)value;
    	    data[1] = bvalue.dbt;
    	}
    	return data;
    }
    
    /*
     * The Object array has the same format as pack except the value type
     * is not stored in it anymore.
     */
    protected XmlValue(int type, Object[] data) 
    {
    	valueType = type;
    	if (valueType == NONE)
    		value = new Value();
    	else if (valueType == NODE) 
    		value = new NodeValue(data);
    	else if (data.length == 2)
    		value = new BinaryValue(data);
    	else
    		value = new StringValue(data);
    }

    private void setValue(XmlValue from) throws XmlException
    {
	valueType = from.getType();
	Class<? extends Value> valueClass = from.value.getClass();
	String simpleName = valueClass.getSimpleName();
	if(simpleName.matches("Value")){
	    value = new Value();
	}else if(simpleName.matches("StringValue")){
	    value = new StringValue((StringValue)from.value);
	}else if(simpleName.matches("NodeValue")){
	    value = new NodeValue((NodeValue)from.value);
	}else if(simpleName.matches("BinaryValue")){
	    value = new BinaryValue((BinaryValue)from.value);
	}else
	    throw (new XmlException(XmlException.INVALID_VALUE, "Invalid XmlValue object."));
    }
	
    //Value is the class the represents the empty XmlValue
    class Value{
		
	public Value(){}
		
	public boolean equals(Value value) throws XmlException
	{
	    return true;
	}
		
	public String getTypeURI() throws XmlException
	{
	    return "";
	}
		
	public String getTypeName() throws XmlException
	{
	    return "";
	}
		
	public double asNumber() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to Number"));
	}
		
	public String asString() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to String"));
	}
		
	public boolean asBoolean() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to Boolean"));
	}
		
	public byte[] asBinary() throws XmlException { 
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to Binary")); 
	}
		
	public XmlDocument asDocument() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to Document"));
	}
		
	public XmlEventReader asEventReader() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Can't convert XmlValue to XmlEventReader"));
	}
		
	public String getNodeHandle() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public String getNodeName() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public String getNodeValue() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public String getNamespaceURI() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public String getPrefix() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public String getLocalName() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public short getNodeType() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getParentNode() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getFirstChild() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getLastChild() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getPreviousSibling() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getNextSibling() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlResults getAttributes() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
	public XmlValue getOwnerElement() throws XmlException
	{
	    throw (new XmlException(XmlException.INVALID_VALUE, "Cannot convert XmlValue to Node"));
	}
		
    }
    //	StringValue holds all atomic values except for binary values
    class StringValue extends Value
    {
		
	protected String value; 
	protected String typeName;
	protected String typeUri;
		
	public StringValue(){}
		
	public StringValue(StringValue other) throws XmlException
	{
	    value = other.asString();
	    typeName = other.typeName;
	    typeUri = other.typeUri;
	}
	
	public StringValue(Object[] data)
	{
		value = (String)data[0];
		if(data.length == 3) {
			typeName = (String)data[1];
			typeUri = (String)data[2];
		}
	}
		
	public boolean equals(Value value) throws XmlException
	{
	    return (this.value.equals(value.asString())
	    		&& getTypeName().equals(value.getTypeName())
	    		&& getTypeURI().equals(value.getTypeURI()));
	}
		
	public StringValue(String v)
	{
		if (v == null) v = "";
	    value = v;
	}   

	public StringValue(String URI, String name, String v) throws XmlException
	{
		if (v == null) v = "";
	    value = v;
	    if (URI == null)  URI = "";
	    typeUri = URI;
	    if (name == null) name = "";
	    typeName = name;
	    valueType = HelperFunctions.getType(URI, name, v);	    
	} 
		
	public String asString() throws XmlException
	{
	    return value;
	}
		
	public boolean asBoolean() throws XmlException
	{
	    try{
		if(valueType == DOUBLE || valueType == DECIMAL || valueType == FLOAT){
		    double d = Double.parseDouble(value);
		    if(d == 0)
			return false;
		    else
			return true;
		}
		else if(valueType == STRING){
		    if(value.length() == 0 || value.compareToIgnoreCase("false") == 0)
			return false;
		    else
			return true;
		}else if(valueType == BOOLEAN){
		    return Boolean.parseBoolean(value);
		}else{
		    throw new XmlException(XmlException.INVALID_VALUE, 
					   "The requested type cannot be converted into a boolean.");
		}
	    }catch(Exception e){
		throw new XmlException(XmlException.INVALID_VALUE, 
				       "The requested type cannot be converted into a boolean.");
	    }
	}
		
	public double asNumber() throws XmlException
	{
	    try{
		if(valueType == BOOLEAN){
		    if(Boolean.parseBoolean(value))
			return 1;
		    else
			return 0;
		}else if(valueType == STRING || valueType == UNTYPED_ATOMIC){
		    try{
			return Double.parseDouble(value);
		    }catch(Exception e){
			return Double.NaN;
		    }
		}else if(valueType == FLOAT || valueType == DOUBLE || valueType == DECIMAL){
		    return Double.parseDouble(value);
		}else{
		    throw new XmlException(XmlException.INVALID_VALUE, 
					   "The requested type cannot be converted into a number.");
		}
	    }catch(Exception e){
		throw new XmlException(XmlException.INVALID_VALUE, 
				       "The requested type cannot be converted into a number.");
	    }
	}
		
	public String getTypeURI() throws XmlException
	{
		if (typeUri != null) return typeUri;
	    return HelperFunctions.getAtomicTypeURI(valueType, value);
	}
		
	public String getTypeName() throws XmlException
	{
		if (typeName != null) return typeName;
	    return HelperFunctions.getAtomicTypeName(valueType, value);
	}
		
    }
	
    //BinaryValue holds all binary atomic values
    class BinaryValue extends Value
    {
		
	private byte[] dbt;

	public BinaryValue(byte[] newDBT)
	{
		if (newDBT == null) newDBT = new byte[0];
	    dbt = newDBT;
	}
		
	public BinaryValue(BinaryValue other) throws XmlException
	{
	    dbt = other.asBinary();
	}
	
	public BinaryValue(Object [] data)
	{
		dbt = (byte[])data[0];
	}
		
	public byte[] asBinary() throws XmlException
	{
	    return dbt;
	}
		
	public boolean equals(Value value) throws XmlException
	{
	    byte[] otherDBT = value.asBinary();
	    return Arrays.equals(dbt, otherDBT);
	}
		
    }
    //NodeValue holds all node and document values for XmlValue
    class NodeValue extends Value
    {
	/* NodeValue is a very complex class because of the state that has
	 * to be preserved between calls to the C++.  The nodes can depend
	 * on transactions and temporary databases, and where these are stored
	 * depends on how the XmlValue is created.  The state is preserved either
	 * in an XmlResults, an XmlDocument, or an XmlManager and XmlTransaction.
	 */
	//All NodeValues have a node type and an ID
	protected short nodeType;
	protected byte[] ID;

	//All NodeValues created by an XmlResults has the results value set
	protected XmlResults results;

	//All NodeValues created by the constructor XmlValue(XmlDocument)
	//has the document value set.
	protected XmlDocument document;

	//All NodeValues created by XmlContainer.getNode and XmlIndexLookup have
	//the manager set, and optionally the transaction.
	protected XmlManager manager;
	protected XmlTransaction transaction;

	/* resultsPtr is used to deal with the case where
	 * a node is created from an XmlResults set, then that node is added
	 * to another XmlResults set using XmlResults.add.  When that node is
	 * retrieved from the second XmlResults it needs to point back to its
	 * original XmlResults set, because that set has access to any transactions
	 * or temporary databases needed to get the node's value.
	 */
	protected long resultsPtr;

	/* tempResults is used to store an XmlResults created from either the
	 * XmlDocument or the XmlManager.  This XmlResults is used to construct
	 * a RawNodeValue object in C++ for a function call, and is deleted
	 * when the function returns.
	 */
	protected XmlResults tempResults;

	public NodeValue(short nodeType, byte[] ID)
	{
	    this.nodeType = nodeType;
	    this.ID = ID;
	}
	
	/*
	 * Data consist of:
	 * 0 - type and node type
	 * 1 - ID
	 * 2 - The XmlDocument if it exists
	 * 3 - The pointer for returnResults if it exists
	 */
	public NodeValue(Object[] data)
	{
		int[] type = (int[])data[0];
		nodeType = (short)type[0];
		ID = (byte[])data[1];
		if (data[2] != null)
			document = (XmlDocument)data[2];
		if (data[3] != null) {
			long[] state = (long[])data[3];
			resultsPtr = state[3];
		}
	}
		
	public NodeValue(NodeValue other) 
	{
	    this.nodeType = other.nodeType;
	    this.ID = other.ID;
	    this.results = other.results;
	    this.document = other.document;
	    this.manager = other.manager;
	    this.transaction = other.transaction;
	}
		
	protected void setResult(XmlResults result)
	{
	    results = result;
	    if(document != null)
		document.setResults(result);
	}
		
	protected XmlResults getResult()throws XmlException
	{
	    if(results != null)
		{
		    if(XmlResults.getCPtr(results) == 0)
			throw new XmlException(XmlException.INVALID_VALUE, 
					       "The results set for this value has been closed, cannot access value.");
		}else if(manager != null){
		if(XmlManager.getCPtr(manager) == 0)
		    throw new XmlException(XmlException.INVALID_VALUE, 
					   "The manager has been closed, cannot access value.");
		if(transaction != null)
		    tempResults = HelperFunctions.createResults(transaction, manager);
		else
		    tempResults = manager.createResults();
		return tempResults;
	    }else{
		throw new XmlException(XmlException.INTERNAL_ERROR, 
				       "The XmlValue does not have an XmlResult or XmlManager.");
	    }
	    return results;
	}
		
	protected long getResultPtr()
	{
	    return XmlResults.getCPtr(results);
	}
		
	protected long getReturnResultPtr()
	{
	    return resultsPtr;
	}
		
	protected long getManagerPtr()
	{
	    return XmlManager.getCPtr(manager);
	}
		
	protected long getTransactionPtr()
	{
	    return XmlTransaction.getCPtr(transaction);
	}
		
	protected void setResultFromPtr(long ptr)
	{
	    resultsPtr = ptr;
	}
		
	protected void setDocument(XmlDocument doc)
	{
	    document = doc;
	}
		
	protected void setManager(XmlManager mgr)
	{
	    manager = mgr;
	    if(document != null)
		document.setManager(mgr);
	}
		
	protected void setTransaction(XmlTransaction txn)
	{
	    transaction = txn;
	    if(document != null)
		document.setTransaction(txn);
	}
		
	private void setInternals(NodeValue node)
	{
	    node.results = results;
	    node.manager = manager;
	    node.document = document;
	    node.transaction = transaction;
	}
	
	//[#16041] Navigation does not work with XmlValues created from
	//constructed XmlDocuments
	private void checkConstructedDocument() throws XmlException
	{
	    if(document != null && document.isConstructed())
		throw (new XmlException(XmlException.INVALID_VALUE, 
					"Navigation functions are not valid for XmlValues created from constructed documents."));
	}
		
	public String getTypeURI() throws XmlException
	{
	    String string = HelperFunctions.getTypeURI(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}

	public String getTypeName() throws XmlException
	{
	    String string = HelperFunctions.getTypeName(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}
		
	public double asNumber() throws XmlException
	{
	    if(document != null){
		XmlValue string = new XmlValue(document.getContentAsString());
		return string.asNumber();
	    }
	    XmlValue string = new XmlValue(HelperFunctions.asString(ID, getResult()));
	    if(tempResults != null)
		tempResults.delete();
	    return string.asNumber();
	}
		
	public String asString() throws XmlException
	{
	    if(document != null)
		return document.getContentAsString();
	    String string = HelperFunctions.asString(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}
		
	public boolean asBoolean() throws XmlException
	{
	    if(document != null){
		XmlValue string = new XmlValue(document.getContentAsString());
		return string.asBoolean();
	    }
	    XmlValue string = new XmlValue(HelperFunctions.asString(ID, getResult()));
	    if(tempResults != null)
		tempResults.delete();
	    return string.asBoolean();
	}
		
	public XmlDocument asDocument() throws XmlException
	{
	    if(document != null)
		return document;
	    XmlDocument doc = HelperFunctions.asDocument(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if(doc == null) return doc;
	    doc.setResults(results);
	    doc.setManager(manager);
	    doc.setTransaction(transaction);
	    return doc;
	}
		
	public XmlEventReader asEventReader() throws XmlException
	{
	    if(document != null)
		return document.getContentAsEventReader();
	    XmlEventReader event = HelperFunctions.asEventReader(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return event;
	}
		
	public String getNodeHandle() throws XmlException
	{
	    String handle = HelperFunctions.getNodeHandle(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return handle;
	}
		
	public boolean equals(XmlValue value) throws XmlException
	{
	    NodeValue node = (NodeValue)value.value;
	    if(nodeType == node.nodeType && ID.length == node.ID.length){
		for(int i = 0; i < ID.length; i++){
		    if(ID[i] != node.ID[i])
			return false;
		}
		return true;
	    }
	    return false;
	}
		
	public String getNodeName() throws XmlException
	{
	    String name = HelperFunctions.getNodeName(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return name;
	}
		
	public String getNodeValue() throws XmlException
	{
	    if(document != null)
		return document.getContentAsString();
	    String value = HelperFunctions.getNodeValue(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return value;
	}
		
	public String getNamespaceURI() throws XmlException
	{
	    String string = HelperFunctions.getNamespaceURI(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}
		
	public String getPrefix() throws XmlException
	{
	    String string = HelperFunctions.getPrefix(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}
		
	public String getLocalName() throws XmlException
	{
	    String string = HelperFunctions.getLocalName(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    return string;
	}
		
	public short getNodeType() throws XmlException
	{
	    return nodeType;
	}
		
	public XmlValue getParentNode() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getParentNode(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
	public XmlValue getFirstChild() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getFirstChild(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
	public XmlValue getLastChild() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getLastChild(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
	public XmlValue getPreviousSibling() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getPreviousSibling(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
	public XmlValue getNextSibling() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getNextSibling(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
	public XmlResults getAttributes() throws XmlException
	{
	    checkConstructedDocument();
	    XmlResults xmlresults = HelperFunctions.getAttributes(ID, getResult());
	    if(tempResults != null)
		xmlresults.resultsMapPut(new Long(XmlResults.getCPtr(tempResults)), tempResults);
	    else{
		XmlResults origResults = getResult();
		if (origResults == results)
		    origResults = new XmlResults(results);
		xmlresults.resultsMapPut(new Long(XmlResults.getCPtr(origResults)), origResults);
	    }
	    
	    return xmlresults;
	}
		
	public XmlValue getOwnerElement() throws XmlException
	{
	    checkConstructedDocument();
	    XmlValue node = HelperFunctions.getOwnerElement(ID, getResult());
	    if(tempResults != null)
		tempResults.delete();
	    if (node.isNode())
		setInternals((NodeValue)node.value);
	    return node;
	}
		
    }

    public final static int ELEMENT_NODE = 1;
    public final static int ATTRIBUTE_NODE = 2;
    public final static int TEXT_NODE = 3;
    public final static int CDATA_SECTION_NODE = 4;
    public final static int ENTITY_REFERENCE_NODE = 5;
    public final static int ENTITY_NODE = 6;
    public final static int PROCESSING_INSTRUCTION_NODE = 7;
    public final static int COMMENT_NODE = 8;
    public final static int DOCUMENT_NODE = 9;
    public final static int DOCUMENT_TYPE_NODE = 10;
    public final static int DOCUMENT_FRAGMENT_NODE = 11;
    public final static int NOTATION_NODE = 12;

    public final static int NONE = 0;
    public final static int NODE = 3;
    public final static int ANY_SIMPLE_TYPE = 10;
    public final static int ANY_URI = 11;
    public final static int BASE_64_BINARY = 12;
    public final static int BOOLEAN = 13;
    public final static int DATE = 14;
    public final static int DATE_TIME = 15;
    public final static int DAY_TIME_DURATION = 16;
    public final static int DECIMAL = 17;
    public final static int DOUBLE = 18;
    public final static int DURATION = 19;
    public final static int FLOAT = 20;
    public final static int G_DAY = 21;
    public final static int G_MONTH = 22;
    public final static int G_MONTH_DAY = 23;
    public final static int G_YEAR = 24;
    public final static int G_YEAR_MONTH = 25;
    public final static int HEX_BINARY = 26;
    public final static int NOTATION = 27;
    public final static int QNAME = 28;
    public final static int STRING = 29;
    public final static int TIME = 30;
    public final static int YEAR_MONTH_DURATION = 31;
    public final static int UNTYPED_ATOMIC = 32;
    public final static int BINARY = 40;

}
