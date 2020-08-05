//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

package com.sleepycat.dbxml;
/* Phasing this class out.  The XmlManager is stored so that a new
 * XmlUpdateContext object can be created on the C++ side. 
 */
public class XmlUpdateContext {
    protected XmlManager manager;
	
    protected static long getCPtr(XmlUpdateContext obj) {
	if(obj == null)
	    obj.toString();  //force a null pointer exception
	return XmlManager.getCPtr(obj.manager);
    }
	
    protected XmlUpdateContext(XmlManager mgr){
	manager = mgr;
    }
}
