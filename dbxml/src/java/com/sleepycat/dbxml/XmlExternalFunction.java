//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2007 Oracle.  All rights reserved.

package com.sleepycat.dbxml;

public class XmlExternalFunction extends XmlExternalFunctionBase {
  protected XmlExternalFunction() {
	  super();
  }

  public XmlResults execute(XmlTransaction txn, XmlManager mgr, XmlArguments args) throws XmlException {
	  return super.execute(txn, mgr, args);
  }

  public void close() throws XmlException {
	  delete();
  }
}
