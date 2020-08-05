
//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2007 Oracle.  All rights reserved.



package com.sleepycat.dbxml;

public class XmlMetaData {
    protected String name;
    protected XmlValue value;
    protected String uri;
    protected boolean modified;
    protected boolean removed;

    protected XmlMetaData(String uri, String name, XmlValue value) throws XmlException{
	if(value.isNull() || value.isNode())
	    throw (new XmlException(XmlException.INVALID_VALUE, "Metadata cannot be a node or null value."));
	this.name = name;
	this.value = value;
	this.uri = uri;
	modified = false;
	removed = false;
    }

    public String get_uri() throws XmlException {
	return uri;
    }

    public String get_name() throws XmlException {
	return name;
    }

    public XmlValue get_value() throws XmlException { 
	return value;
    }
	
    protected void set_modified(boolean mod) {
	modified = mod;
    }
	
    protected void set_removed(boolean rem) {
	removed = rem;
    }
    protected boolean get_modified() {
	return modified;
    }
  
    protected boolean get_removed() {
	return removed;
    }
  
    protected void set_value(XmlValue data) throws XmlException { 
	if(value.isNull() || value.isNode())
	    throw (new XmlException(XmlException.INVALID_VALUE, "Metadata cannot be a node or null value."));
	value = data;
    }
	
}
