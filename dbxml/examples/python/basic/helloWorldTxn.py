"""
 * HelloWorldTxn is a very simple Berkeley DB XML program
 * that demonstrates transactional initialization and 
 * container creation, transactional document insertion and 
 * retrieval by name.
 *
 * The program also factors out initialization tasks that are
 * typically performed one time at the beginning of an application.
 * These include opening of a DBEnv, creation of XmlManager and
 * opening XmlContainer objects.
 *
 * After running this program, you will notice a number of files
 * in the environment directory:
 *   __db.* files are the BDB environment, including cache
 *   log.* files are BDB log files for transactions
 * The actual BDB XML container is not present, since it's only
 * created in-memory, and will disappear at the end of the program.
 * To run the example:
 *   python helloWorldTxn.py [-h environmentDirectory]

"""
from bsddb3.db import *
from dbxml import *

def createEnvironment(home):
    """Create DBEnv and initialize XmlManager"""
    try:
        environment = DBEnv()
        # initialize DBEnv for transactions
        environment.open(home, DB_RECOVER|DB_CREATE|DB_INIT_LOCK|
                         DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN, 0)
    except DBError, exc:
        print exc
        sys.exit()
    try:
        mgr = XmlManager(environment, 0)
    except XmlException, se:
        print xe
        sys.exit()
    return mgr

def createContainer(mgr, containerName, flags):
    """create/open a node container"""
    try:
        return mgr.openContainer(containerName,
                                 flags|DB_CREATE,
                                 XmlContainer.NodeContainer)
    except XmlException, ex:
        print ex
        sys.exit()

def helloWorldTxn(mgr, cont):
    """The guts of the example.  Note that because Python will GC the
    BDB XML objects upon function exit, explicit deletion is not 
    required.  Transactions need to be handled specially in the 
    event of exceptions."""
    docName = "doc"
    content = "<hello>Hello World</hello>"

    try:
        # all Container modification operations need XmlUpdateContext
        uc = mgr.createUpdateContext()
        # create XmlTransaction for the operation
        txn = mgr.createTransaction()

        # use the DBXML_GEN_NAME flag to make sure this
        # succeeds by creating a new, unique name
        # Use a try/except block to allow the transaction to
        # be aborted in the proper scope upon error
        try:
            docName = cont.putDocument(txn, docName, 
                                       content, uc) #, DBXML_GEN_NAME)
            txn.commit()
        except XmlException, ex:
            print ex
            txn.abort()

        # now, get the document in a new transaction
        txn = mgr.createTransaction()
        doc = cont.getDocument(txn, docName)
        name = doc.getName()
        docContent = doc.getContentAsString()
        txn.commit() # done with data
        
        # print the name and content
        print "Document name: ",name,"\nContent: ",docContent
    except XmlException, inst:
        print inst
        if txn:
            txn.abort()


def usage():
    print "helloWorldTxn.py [-h home_directory]"

# "main"
import sys
import getopt
def main():
    home = "."
    # parse command line options
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:")
    except getopt.error, msg:
        print msg
        usage()
        sys.exit(2)
    for o, a in opts:
        if o == "-h":
            home = a

    # some configuration...
    containerName = ""
    # initialize...
    mgr = createEnvironment(home)
    # create/open a transactional container
    cont = createContainer(mgr, containerName, 
                           DBXML_TRANSACTIONAL)

    # run the guts of the program...
    helloWorldTxn(mgr, cont)
if __name__ == "__main__":
    main()
