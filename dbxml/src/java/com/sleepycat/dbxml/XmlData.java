/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.39
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.sleepycat.dbxml;

public class XmlData {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected XmlData(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(XmlData obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void delete() {}

	public byte[] get_data()
	{
		return dbxml_javaJNI.get_data(swigCPtr);
	}

  public int get_size() throws XmlException { return dbxml_javaJNI.XmlData_get_size(swigCPtr, this); }

  public void set_size(int size) throws XmlException {
    dbxml_javaJNI.XmlData_set_size(swigCPtr, this, size);
  }

  public void reserve(int size) throws XmlException {
    dbxml_javaJNI.XmlData_reserve(swigCPtr, this, size);
  }

  public int getReservedSize() throws XmlException { return dbxml_javaJNI.XmlData_getReservedSize(swigCPtr, this); }

  public void adoptBuffer(XmlData src) throws XmlException {
    dbxml_javaJNI.XmlData_adoptBuffer(swigCPtr, this, XmlData.getCPtr(src), src);
  }

  public void append(byte[] data) throws XmlException {
    dbxml_javaJNI.XmlData_append(swigCPtr, this, data);
  }

  public void set(byte[] data) throws XmlException {
    dbxml_javaJNI.XmlData_set(swigCPtr, this, data);
  }

}
