"""
 * helloWorld is the simplest possible Berkeley DB XML program
 * that does something.
 * This program demonstrates initialization, container creation,
 * document insertion and document retrieval by name.
 *
 * To run the example:
 *     python helloWorld.py
"""
from dbxml import *

def helloWorld():
    containerName = ""
    content = "<hello>Hello World</hello>"
    docName = "doc"
    try:
        # all BDB XML programs require an XmlManager instance
        mgr = XmlManager()
        cont = mgr.createContainer(containerName)
        
        # all Container modification operations need XmlUpdateContext
        uc = mgr.createUpdateContext()
        cont.putDocument(docName, content, uc)
        
        # now, get the document
        doc = cont.getDocument(docName)
        name = doc.getName()
        docContent = doc.getContentAsString()
        
        # print the name and content
        print "Document name: ",name,"\nContent: ",docContent
    except XmlException, inst:
        print inst

if __name__ == "__main__":
    helloWorld()

