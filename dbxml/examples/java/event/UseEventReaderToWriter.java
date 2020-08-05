/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 */

package event;

import event.EventReaderToWriter;

import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlEventWriter;
import com.sleepycat.dbxml.XmlManager;
import com.sleepycat.dbxml.XmlContainer;
import com.sleepycat.dbxml.XmlDocument;
import com.sleepycat.dbxml.XmlUpdateContext;

/**
 * UseEventReaderToWriter is simple example of using the EventReaderToWriter
 * example code.  It creates an in-memory container, adds a document, and
 * copies that document to a new name using EventReaderToWriter.
 * <p>
 * To run this program:
 * <pre>
 * java event.UseEventReaderToWriter
 * </pre>
 */
class UseEventReaderToWriter
{
    public static void main(String args[]) throws Throwable {
	String containerName = "";
	String docName1 = "doc1";
	String docName2 = "doc2";
	String content = "<root><a a='val'>a text</a><b/><c><d nm='joe'/></c></root>";
	try {
	    // create container and put a document in
	    XmlManager mgr = new XmlManager();
	    XmlContainer cont = mgr.createContainer(containerName);
	    cont.putDocument(docName1, content);

	    // now copy the document
	    XmlDocument doc = cont.getDocument(docName1);
	    XmlDocument newDoc = mgr.createDocument();
	    newDoc.setName(docName2);

	    // get an XmlEventWriter to create the new content
	    XmlEventWriter writer = cont.putDocumentAsEventWriter(newDoc);

	    // get XmlEventReader to read existing document
	    XmlEventReader reader = doc.getContentAsEventReader();

	    // copy
	    EventReaderToWriter r2w = new EventReaderToWriter(reader, writer);
	    r2w.start();

	    // clean up (EventReaderToWriter does not close its arguments)
	    writer.close();
	    reader.close();
	    
	    // verify the copy
	    XmlDocument copyDoc = cont.getDocument(docName2);
	    String newContent = copyDoc.getContentAsString();
	    System.out.println("Original:\n\t" + content);
	    System.out.println("New:\n\t" + newContent);

	    // clean up
	    cont.delete();
	    mgr.delete();
	} catch (XmlException e) {
	    System.out.println("Exception: " + e.toString());
	}
	catch (Throwable t) {}
    }
}

