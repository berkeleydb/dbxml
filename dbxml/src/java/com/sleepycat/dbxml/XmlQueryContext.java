//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2007 Oracle.  All rights reserved.

package com.sleepycat.dbxml;

import java.util.*;

public class XmlQueryContext {
    public final static int LiveValues = 0;

    public final static int Eager = 0;
    public final static int Lazy = Eager + 1;

    protected XmlManager mgr;
    protected HashMap<String, XmlValue[]> variables;
    protected HashMap<String, String> namespaces;
    protected String baseURI;
    protected int evaluationType;
    protected String defaultCollection;
    protected int queryInterruptSec;
    protected String defaultCollectionURI;
    protected Interrupter interrupt;
	protected XmlDebugListener debugListener;

    protected XmlQueryContext(int rt, int et) {
	evaluationType = et;
	interrupt = new Interrupter();
    }

    public XmlQueryContext(XmlQueryContext o) throws XmlException {
	baseURI = o.baseURI;
	evaluationType = o.evaluationType;
	defaultCollection = o.defaultCollection;
	queryInterruptSec = o.queryInterruptSec;
	if(o.namespaces != null) namespaces = new HashMap<String, String>(o.namespaces);
	if(o.variables != null)variables = new HashMap<String, XmlValue[]>(o.variables);
	interrupt = new Interrupter();
	mgr = o.mgr;
    }

    /** @deprecated as of 2.4 */
    @Deprecated 
    public void delete(){}

    public void setVariableValue(String name, XmlValue value) throws XmlException {
    if (name == null) throw new NullPointerException();
	if (value.getType() == XmlValue.BINARY)
	    throw (new XmlException(XmlException.INVALID_VALUE, "XmlQueryContext::setVariableValue value cannot be binary"));
	if(variables == null) variables = new HashMap<String,XmlValue[]>();
	XmlValue[] val = {value};
	variables.put(name, val);
    }

    public void setVariableValue(String name, XmlResults value) throws XmlException {
    if (name == null) throw new NullPointerException();
	if(variables == null) variables = new HashMap<String,XmlValue[]>();
	if(value.getEvaluationType() == XmlQueryContext.Lazy){
	    Vector<XmlValue> valueVector = new Vector<XmlValue>();
	    while(value.hasNext()){
		XmlValue val = value.next();
		valueVector.add(val);
	    }
	    XmlValue[] values = valueVector.toArray(new XmlValue[0]);
	    variables.put(name, values);
	    return;
	}
	value.reset();
	XmlValue[] values = new XmlValue[value.size()];
	for(int i = 0; i < values.length; i++)
	    values[i] = value.next();
	variables.put(name, values);
    }
    public XmlValue getVariableValue(String name) throws XmlException { 
	if(variables == null) return new XmlValue();
	XmlValue[] value = variables.get(name);
	if(value == null || value.length == 0)
	    return new XmlValue();
	if(value.length > 1)
	    throw (new XmlException(XmlException.INVALID_VALUE, "Variable has more than one value assigned to it"));
	return value[0];
    }

    public XmlResults getVariableValues(String name) throws XmlException {
	if(variables == null) return null;
	XmlValue[] res = variables.get(name);
	if(res != null){
	    XmlResults result = mgr.createResults();
	    for(int i = 0; i < res.length; i++)
		result.add(res[i]);
	    return result;
	}
	return null;
    }

    public void setNamespace(String prefix, String uri) throws XmlException {
    if (uri == null) throw new NullPointerException();
    if (prefix == null) prefix = "";
	if(namespaces == null) namespaces = new HashMap<String,String>();
	namespaces.put(prefix, uri);
    }

    public String getNamespace(String prefix) throws XmlException {
    if (prefix == null) prefix = "";
	if(prefix.equalsIgnoreCase(dbxml_javaConstants.metaDataNamespace_prefix)) 
	    return dbxml_javaConstants.metaDataNamespace_uri;
	if(namespaces == null) return "";
	String uri = namespaces.get(prefix);
	if(uri == null)
	    return "";
	return uri;
    }

    public void removeNamespace(String prefix) throws XmlException {
	if(namespaces != null)
	    namespaces.remove(prefix);
    }

    public void clearNamespaces() throws XmlException {
	if(namespaces != null)
	    namespaces.clear();
    }

    public void setBaseURI(String baseURI) throws XmlException {
	HelperFunctions.verifyBaseURI(mgr, baseURI);
	this.baseURI = baseURI;
    }

    public String getBaseURI() throws XmlException {
	if(baseURI == null) return "dbxml:/";
	return baseURI;
    }

    public void setReturnType(int type) throws XmlException {
    }

    public int getReturnType() throws XmlException {
	return LiveValues;
    }

    public void setEvaluationType(int type) throws XmlException {
	this.evaluationType = type;
    }

    public int getEvaluationType() throws XmlException {
	return evaluationType;
    }

    public void setDefaultCollection(String uri) throws XmlException {
	defaultCollection = HelperFunctions.verifyDefaultCollection(mgr, uri, baseURI);
	defaultCollectionURI = uri;
    }

    public String getDefaultCollection() throws XmlException {
	if(defaultCollection == null) return "";
	return defaultCollection;
    }

    public void setDebugListener(XmlDebugListener listener) throws XmlException {
		debugListener = listener;
    }

    public XmlDebugListener getDebugListener() throws XmlException {
		return debugListener;
    }

	protected long getDebugListenerCPtr() {
		return XmlDebugListener.getCPtr(debugListener);
	}

    public void interruptQuery() throws XmlException {
	synchronized(interrupt){
	    if(interrupt.cPtr != 0) dbxml_javaJNI.interruptQuery(interrupt.cPtr);
	}
    }

    public void setQueryTimeoutSeconds(int secs) throws XmlException {
	queryInterruptSec = secs;
    }

    public int getQueryTimeoutSeconds() throws XmlException {
	return queryInterruptSec;
    }

    protected static XmlQueryContext copy(XmlQueryContext o) throws XmlException {
	return new XmlQueryContext(o);
    }

    //Used by createCPPXmlQueryContext to translate the object to a C++ XmlQueryContext
    protected long getManagerPtr()
    {
	return XmlManager.getCPtr(mgr);
    }

    /*
     * pack() combines most of the information for XmlQueryContext into an
     * Object array and returns it to the JNI so a C++ XmlQueryContext can be
     * made.  The first spot in the array contains the baseURI, which can be null,
     * the second spot contains the unresolved default collection, which can also
     * be null.  Next all namespace prefixes and uris are added, if any exist.
     * Then a null is added to separate the namespaces and variables.  Next
     * the variables are added.  The name of the variable is added, then
     * the array containing all XmlValue values for that name are added.
     * The format is baseURI, defaultCollection, [namespace prefix,
     * namespace uri, ..] null [variable name, XmlValue array of variable values, ..]
     */
    protected Object[] pack()
    {
	Vector<Object> data = new Vector<Object>();
	data.add(baseURI);
	data.add(defaultCollectionURI);
	if(namespaces != null){
	    Iterator<Map.Entry<String,String> > namespaceIter = namespaces.entrySet().iterator();
	    while(namespaceIter.hasNext()){
		Map.Entry<String,String> entry = namespaceIter.next();
		data.add(entry.getKey());
		data.add(entry.getValue());
	    }
	}
	data.add(null);
	if(variables != null){
	    Iterator<Map.Entry<String,XmlValue[]> > variableIter = variables.entrySet().iterator();
	    while(variableIter.hasNext()){
		Map.Entry<String,XmlValue[]> entry = variableIter.next();
		data.add(entry.getKey());
		XmlValue[] res = entry.getValue();
		data.add(res);
	    }
	}
	return data.toArray();
    }

    protected void setCPtr(long ptr)
    {
	synchronized (interrupt){
	    interrupt.cPtr = ptr;
	}
    }

    class Interrupter{
	public long cPtr;
    }
}
