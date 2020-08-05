package dbxmltest;

import com.sleepycat.dbxml.XmlCompression;
import com.sleepycat.dbxml.XmlData;
import com.sleepycat.dbxml.XmlTransaction;
import com.sleepycat.dbxml.XmlException;

/*
 *  A customized compression class that is used by CompressionTest.
 */
public class TestCompression extends XmlCompression {

    public TestCompression() {}

    public void delete()
    {
	super.delete();
    }

    public boolean compress(XmlTransaction txn, XmlData source, XmlData dest)
    {
	try {
	    byte[] src = source.get_data();
	    byte[] des = new byte[src.length];
	    for (int i = 0; i < src.length; i++)
	    {
	    	des[src.length-(i+1)] = src[i];
	    }
	    dest.set(des);
	} catch (XmlException e) {
	    return false;
	}
	return true;
    }

    public boolean decompress(XmlTransaction txn, XmlData source, XmlData dest)
    {
	try {
		byte[] src = source.get_data();
	    byte[] des = new byte[src.length];
	    for (int i = 0; i < src.length; i++)
	    {
	    	des[src.length-(i+1)] = src[i];
	    }
	    dest.set(des);
	} catch (XmlException e) {
	    return false;
	}
	return true;
    }

}
