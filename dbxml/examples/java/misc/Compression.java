/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * Compression API
 *
 * A very simple Berkeley DB XML program that demonstrates
 * compression API usage.
 *
 * This program demonstrates:
 *  Registration of a custom compression class in XmlManager
 *  Compression configuration for XmlContainer
 *  Turn off default compression
 *  Custom compression class implementation
 *
 *  MyCompression is an subclass of XmlCompression, it demonstrates
 *  compression using JDK's ZLIB compression support. It is a very 
 *  simple implementation and not intended to be particularly efficient.
 *  
 *  Compression only works with whole document containers.
 *  User compression must always be registered with the manager in
 *  order to open/create the container.
 *  
 */

package misc;

import com.sleepycat.dbxml.*;

/* 
 * MyCompression implements the XmlCompression interface: required
 * methods are compress() and decompress(). The two methods are required 
 * to return true for success and false for error. This class 
 * must be thread-safe in multi-thread environment. 
 * 
 */
class MyCompression extends XmlCompression {

    public boolean compress(XmlTransaction txn, XmlData source, XmlData dest) {

	try {
	    // Get the data to compress
	    byte[] src = source.get_data();

	    // Use JDK's ZLIB compress 
	    java.util.zip.Deflater compressor = 
		new java.util.zip.Deflater();
	    compressor.setInput(src);
	    compressor.finish();

	    java.io.ByteArrayOutputStream bos = 
		new java.io.ByteArrayOutputStream(src.length);

	    // Compress the data
	    byte[] buf = new byte[1024];
	    while (!compressor.finished()) {
		int count = compressor.deflate(buf);
		bos.write(buf, 0, count);
	    }
	    bos.close();

	    byte[] data = bos.toByteArray();

	    // Set the compressed data
	    dest.set(data);

	} catch (Exception e) {
	    // If any exception, return false
	    return false;
	}

	// Successful return true
	return true;
    }

    public boolean decompress(XmlTransaction txn, XmlData source, XmlData dest) {
		
        try {
	    // Get the data to decompress
	    byte[] src = source.get_data();

	    // Use JDK's ZLIB decompress
	    java.util.zip.Inflater decompressor = 
		new java.util.zip.Inflater();
	    decompressor.setInput(src);

	    java.io.ByteArrayOutputStream bos = 
		new java.io.ByteArrayOutputStream(src.length);

	    // Decompress the data
	    byte[] buf = new byte[1024];
	    while (!decompressor.finished()) {
		int count = decompressor.inflate(buf);
		bos.write(buf, 0, count);
	    }
	    bos.close();

	    byte[] data = bos.toByteArray();

	    // Set the decompressed data
	    dest.set(data);

	} catch (Exception e) {
	    // If any exception, return false
	    return false;
	}

	// Successful return true
	return true;
    }
}

public class Compression {

    public static void main(String[] args) throws Throwable {

	String containerName1 = "compressionContainer1.dbxml";
	String containerName2 = "compressionContainer2.dbxml";
	String docName = "doc1.xml";
	String content = "<root><a></a></root>";
	String compressionName = "myCompression";
		
	try {

	    XmlManager mgr = new XmlManager();
			
	    // Start with fresh containers
	    if (mgr.existsContainer(containerName1) != 0)
		mgr.removeContainer(containerName1);

	    if (mgr.existsContainer(containerName2) != 0)
		mgr.removeContainer(containerName2);
			
	    // Turn off the default compression			 
            XmlContainerConfig containerConfig = new XmlContainerConfig();
	    // Set the container type WholedocContainer 
	    // and turn off default compression
	    containerConfig.setContainerType(XmlContainer.WholedocContainer);
	    containerConfig.setCompression(XmlContainerConfig.NO_COMPRESSSION);
			
	    // Create container
	    XmlContainer cont = mgr.createContainer(containerName1, 
						    containerConfig);
			
	    XmlDocument doc = mgr.createDocument();
	    doc.setName(docName);
	    doc.setContent(content);
	    cont.putDocument(doc);
			
	    cont.delete();
			
	    // Open the container 
	    cont = mgr.openContainer(containerName1);
			
	    // Verify the content
	    System.out.println("Content of the document: " 
			       + cont.getDocument(docName).getContentAsString());
			
	    cont.delete();
			
			
	    // Following code demonstrates: 
	    //   compression with user's compression class
			 			
	    // Register user's compression object into XmlManager
	    MyCompression compressor = new MyCompression();
	    mgr.registerCompression(compressionName, compressor);

	    containerConfig = new XmlContainerConfig();
	    containerConfig.setContainerType(XmlContainer.WholedocContainer);
			
	    // Set XmlContainerConfig custom compression
	    containerConfig.setCompression(compressionName);
			
	    cont = mgr.createContainer(containerName2, containerConfig);
			
	    // Put a document in
	    doc = mgr.createDocument();
	    doc.setName(docName);
	    doc.setContent(content);
	    cont.putDocument(doc);

	    cont.delete();
			
	    // Open the container  
	    cont = mgr.openContainer(containerName2);
			
	    // Get the content
	    System.out.println("Content of the document: "
			       + cont.getDocument(docName).getContentAsString());
			
	    // Clean up
	    compressor.delete();
	    cont.delete();
	    mgr.delete();
			   
	} catch (XmlException e) {
	    System.out.println("Exception: " + e.getMessage());
	}
    }

}
