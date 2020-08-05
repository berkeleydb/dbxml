package dbxmltest;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlInputStream;

public class TestInputStream extends XmlInputStream {

    private static String docString = "<?xml version=\"1.0\" "
	+ "encoding=\"UTF-8\"?><old:a_node xmlns:old=\""
	+ "http://dbxmltest.test/test\" atr1=\"test\" atr2=\"test2\""
	+ "><b_node/><c_node>Other text</c_node><d_node/>"
	+ "</old:a_node>";

    private ByteArrayInputStream st;
    private long curPos_ = 0;

    public TestInputStream() {
	st = new ByteArrayInputStream(docString.getBytes());
    }

    public void delete() {
	try {
	    st.close();
	} catch (IOException e) {
	    e.printStackTrace();
	}
    }

    public long curPos() throws XmlException {
	return curPos_;
    }

    public long readBytes(byte[] toFill, long maxToRead) throws XmlException {

	int bytesRead = st.read(toFill, 0, (int) maxToRead);
	if (bytesRead == -1)
	    return 0;
	curPos_ += bytesRead;
	return bytesRead;
    }

}
