//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

package com.sleepycat.dbxml;

import java.io.InputStream;
import java.io.IOException;

/**
 * An implementation of XmlInputStream that works using an
 * underlying java.io.InputStream
 */
/*package*/ class NativeXmlInputStream extends XmlInputStream {

	private InputStream is_;

	/*package*/ NativeXmlInputStream(InputStream is) {
		is_ = is;
	}

	public void delete() /* no exception */ {
		if(getCPtr(this) != 0 && is_ != null) {
			try {
				is_.close();
			}
			catch(IOException e) {
				// Ignore
			}
		}
		super.delete();
	}

	private long curPos_ = 0;

	public long curPos() throws XmlException {
		return curPos_;
	}

	public long readBytes(byte[] toFill, long maxToRead) throws XmlException {
		try {
			int bytesRead = is_.read(toFill, 0, (int)maxToRead);
			if(bytesRead == -1) return 0;
			curPos_ += bytesRead;
			return bytesRead;
		}
		catch(IOException e) {
			throw new XmlException(XmlException.INTERNAL_ERROR, e.getMessage());
		}
	}

}
