/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2007,2009 Oracle.  All rights reserved.
 *
 */

package dbxmltest;

import java.io.ByteArrayInputStream;

import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlResolver;
import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlValue;
import com.sleepycat.dbxml.XmlExternalFunction;

public class TestResolver extends XmlResolver {
    private String content;
    private String name;
    private XmlExternalFunction exFunc;
    private XmlDocument doc = null;

    public TestResolver() {
	content = "";
	name = "";
	exFunc = null;
    }
    
    public void delete() {
    	try {
    		if (exFunc != null) { 
    			exFunc.close();
    			exFunc.delete();
    		}
    	} catch (XmlException e) {}
    	super.delete();
    }

    public TestResolver(XmlExternalFunction f) {
	content = "";
	name = "";
	exFunc = f;
    }

    public TestResolver(String documentName, String documentContent) {
	content = documentContent;
	name = documentName;
	exFunc = null;
    }

    public boolean resolveDocument(XmlTransaction txn, XmlManager mgr,
				   String uri, XmlValue reslt) throws XmlException {
	doc = mgr.createDocument();
	doc.setName(name);
	doc.setContent(content);
	XmlValue val = new XmlValue(doc);
	XmlValue.setValue(reslt, val);
	return true;
    }

    public boolean resolveCollection(XmlTransaction txn, XmlManager mgr,
				     String uri, XmlResults result) throws XmlException {
	doc = mgr.createDocument();
	doc.setName(name);
	doc.setContent(content);
	XmlValue val = new XmlValue(doc);
	result.add(val);
	return true;
    }

    public XmlInputStream resolveEntity(XmlTransaction txn, XmlManager mgr,
					String systemId, String publicId) throws XmlException {
	String str = "<!ELEMENT root (#PCDATA)><!ENTITY stuff \"mystuff\">";

	ByteArrayInputStream is = new ByteArrayInputStream(str.getBytes());
	XmlInputStream input = mgr.createInputStream(is);
	return input;
    }

    public XmlInputStream resolveModule(XmlTransaction txn, XmlManager mgr,
					String moduleLocation, String namespace) throws XmlException {
	String str = "module namespace test = 'http://test.module';\n\ndeclare function test:function() as xs:string {\n 'module' \n};\n";
	ByteArrayInputStream is = new ByteArrayInputStream(str.getBytes());
	XmlInputStream input = mgr.createInputStream(is);
	return input;
    }

    public boolean resolveModuleLocation(XmlTransaction txn, XmlManager mgr,
					 String nameSpace, XmlResults result) throws XmlException {
	XmlValue val = new XmlValue("http://dbxmltest.test/test");
	result.add(val);
	return true;
    }

    public XmlInputStream resolveSchema(XmlTransaction txn, XmlManager mgr,
					String schemaLocation, String nameSpace) throws XmlException {

	String str = "<?xml version=\"1.0\"?><xs:schema xmlns:xs=\""
	    + "http://www.w3.org/2001/XMLSchema\" targetNamespa"
	    + "ce='http://fake.uri.schema' xmlns=\"http://www.w3s"
	    + "chools.com\" ><xs:element name=\"root\"></xs:eleme"
	    + "nt></xs:schema>";

	ByteArrayInputStream is = new ByteArrayInputStream(str.getBytes());
	XmlInputStream input = mgr.createInputStream(is);
	return input;
    }

    public XmlExternalFunction resolveExternalFunction(XmlTransaction txn, XmlManager mgr,
	String uri, String name, int numberOfArgs) throws XmlException {
	return exFunc;
    }
}
