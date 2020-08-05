
//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2007 Oracle.  All rights reserved.



package com.sleepycat.dbxml;

public class XmlMetaDataIterator {
    private XmlDocument document;
    private int index;

    protected XmlMetaDataIterator(XmlDocument doc){
	document = doc;
	index = -1;
    }

    public XmlMetaDataIterator(XmlMetaDataIterator o) throws XmlException {
	document = o.document;
	index = o.index;
    }

    public void reset() throws XmlException {
	index = -1;
    }

    public XmlMetaData next() throws XmlException {
	index++;
	return document.getMetaData(index);
    }

}
