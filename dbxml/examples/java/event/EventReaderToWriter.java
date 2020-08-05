/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 */

package event;

import com.sleepycat.dbxml.XmlException;
import com.sleepycat.dbxml.XmlEventReader;
import com.sleepycat.dbxml.XmlEventWriter;

/**
 * EventReaderToWriter is a Java class that implements, purely in Java, 
 * the same function that is performed by the XmlEventReaderToWriter 
 * class inside BDB XML.
 * <p>
 * This is useful as a template for code that writes events to other
 * systems or, for example, creates a DOM tree from an XmlEventReader
 * helloWorld is the simplest possible Berkeley DB XML program
 * that does something.
 * <p>
 * To use the class:
 * <pre>
 * // acquire XmlEventReader and XmlEventWriter objects, reader, writer
 * EventReaderToWriter r2w = new EventReaderToWriter(reader, writer);
 * r2w.start();
 * // close XmlEventReader and XmlEventWriter objects
 * reader.close();
 * writer.close();
 * </pre>
 */
class EventReaderToWriter
{
    private XmlEventReader reader;
    private XmlEventWriter writer;

    /**
       Construct an EventReaderToWriter instance from the parameters.
       This class does not close the parameters; that is the responsibility
       of the caller.
       @param reader The XmlEventReader from which events are read.
       @param writer The XmlEventWriter} to which events are written.
    */
    public EventReaderToWriter(XmlEventReader reader,
			       XmlEventWriter writer)
    {
	this.reader = reader;
	this.writer = writer;
    }
    /**
       Perform the operation.  This method will read events from the reader, writing
       them to the writer, until there are no further events available.
    */
    public void start() throws XmlException {
	while (reader.hasNext())
	    doEvent(writer);
    }

    /* internal method to get the next event */
    boolean nextEvent(XmlEventWriter writer)
	throws XmlException {
	if (reader.hasNext()) {
	    doEvent(writer);
	    return true;
	}
	return false;
    }

    /* the guts of the class, handling events */
    void doEvent(XmlEventWriter writer) throws XmlException {
	int etype = reader.next();
	switch (etype) {
	case XmlEventReader.StartElement:
	{
	    int nattrs = reader.getAttributeCount();
	    writer.writeStartElement(reader.getLocalName(),
				     reader.getPrefix(),
				     reader.getNamespaceURI(),
				     nattrs,
				     reader.isEmptyElement());
	    for (int i = 0; i < nattrs; i++) {
		writer.writeAttribute(reader.getAttributeLocalName(i),
				      reader.getAttributePrefix(i),
				      reader.getAttributeNamespaceURI(i),
				      reader.getAttributeValue(i),
				      reader.isAttributeSpecified(i));
	    }
	    break;
	}
	case XmlEventReader.EndElement:
	{
	    writer.writeEndElement(reader.getLocalName(),
				   reader.getPrefix(),
				   reader.getNamespaceURI());
	break;
	}
	case XmlEventReader.CDATA:
	case XmlEventReader.Characters:
	case XmlEventReader.Comment:
	case XmlEventReader.Whitespace:
	{
		String chars = reader.getValue();
		writer.writeText(etype, chars);
		break;
	}
	case XmlEventReader.StartDocument:
	{
	    String sa = null;
	    if (reader.standaloneSet()) {
		if (reader.isStandalone())
		    sa = "yes";
		else
		    sa = "no";
	    }
	    writer.writeStartDocument(reader.getVersion(),
				      reader.getEncoding(),
				      sa);
	    break;
	}
	case XmlEventReader.EndDocument:
	{
		writer.writeEndDocument();
		break;
	}
	case XmlEventReader.DTD:
	{
	    writer.writeDTD(reader.getValue());
	    break;
	}
	case XmlEventReader.ProcessingInstruction:
	{
		String target = reader.getLocalName();
		String data = reader.getValue();
		writer.writeProcessingInstruction(target, data);
		break;
	}
	case XmlEventReader.StartEntityReference:
	{
		String chars = reader.getValue();
		writer.writeStartEntity(chars, reader.getExpandEntities());
		break;
	}
	case XmlEventReader.EndEntityReference:
	{
		String chars = reader.getValue();
		writer.writeEndEntity(chars);
		break;
	}
	default:
		throw new XmlException(XmlException.EVENT_ERROR,
				       "EventReaderToWriter.doEvent() unknown event type");
	}
    }
}

