
//See the file LICENSE for redistribution information.

//Copyright (c) 2002,2009 Oracle.  All rights reserved.



package com.sleepycat.dbxml;

import java.io.InputStream;
import java.io.IOException;

import com.sleepycat.dbxml.XmlInputStream;

/**
 * An implementation of InputStream that uses XmlInputStream
 */ 
/* package */ class XmlInputStreamWrap extends java.io.InputStream {
  private XmlInputStream xis_;

  /* package */ XmlInputStreamWrap(XmlInputStream xis) {
    xis_ = xis;
  }

  /* InputStream methods */
  public void close() throws IOException {
    if(xis_ != null)
      xis_.delete();
    xis_ = null;
  }

  public boolean markSupported() {
    return false;
  }

  public int read() throws IOException {
    try {
      byte b[] = new byte[1];
      long n = xis_.readBytes(b, 1);
      if (n == 0)
        return -1;
      return b[0];
    } catch (XmlException e) {
      throw new IOException("XmlException in XmlInputStreamWrap.read()");
    }
  }

  public int read(byte[] b) throws IOException {
    try {
      long ret = xis_.readBytes(b, b.length);
      if (ret == 0)
        return -1;
      return (int) ret;
    } catch (XmlException e) {
      throw new IOException("XmlException in XmlInputStreamWrap.read()");
    }
  }

  public int read(byte[] b, int off, int len) throws IOException {
    try {
      /* this is a bit ugly.  If off is 0, readBytes() works, but
       * if not, there is no way to pass an offset into b, so
       * allocate a temporary array, read, and use System.arraycopy()
       * to move the bytes into the right place.
       */
      long ret = 0;
      if (off == 0) {
        ret = xis_.readBytes(b, len);
      } else {
        byte tb[] = new byte[len];
        int tlen = (int) xis_.readBytes(tb, len);
        if (tlen != 0) 
          System.arraycopy(tb, 0, b, off, tlen);
        ret = (long) tlen;
      }
      if (ret == 0)
        return -1;
      return (int) ret;
    } catch (XmlException e) {
      throw new IOException("XmlException in XmlInputStreamWrap.read()");
    }
  }

  /** use default implementations of:
   * available()  -- returns 0
   * skip -- reads into dummy buffer
   */
}
