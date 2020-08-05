"""
 * Query is a very simple Berkeley DB XML program
 * that performs a query and handles results transactionally.
 * It demonstrates transactional initialization container creation,
 * transactional document insertion, query creation and execution,
 * use of a variable in a query and context, and
 * transactional results handling.
 *
 * To run the example:
 *     python queryTxn.py
"""
from dbxml import *
from bsddb3.db import *

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
    except XmlException, xe:
        print xe
        sys.exit()
    return mgr

def createContainer(mgr, containerName, flags):
    """create/open a node container"""
    if mgr.existsContainer(containerName) != 0:
        mgr.removeContainer(containerName)
    try:
        return mgr.openContainer(containerName,
                                 flags|DB_CREATE,
                                 XmlContainer.NodeContainer)
    except XmlException, ex:
        print ex
        sys.exit()

def queryTxn(mgr, cont):
    content = "<people><person><name>joe</name></person><person><name>mary</name></person></people>"
    docName = "people"
    queryString = "collection('%s')/people/person[name=$name]"%cont.getName()
    try:
        # all modification operations need XmlUpdateContext
        uc = mgr.createUpdateContext()

        txn = mgr.createTransaction()
        try:
            docName = cont.putDocument(txn, docName, content, uc, DBXML_GEN_NAME)
            txn.commit()
        except XmlException, ex:
            print ex
            txn.abort()

        # querying requires an XmlQueryContext
        qc = mgr.createQueryContext()

        # add a variable to the context, as required by the query
        qc.setVariableValue("name", XmlValue("mary"))

        # create a new transaction for the query
        txn = mgr.createTransaction()

        try:
            # note: these two calls could be replaced by a shortcut
            # mgr.query(txn, queryString, qc), but under the covers,
            # this is what is happening.  Prepared queries can be shared
            # and reused
            expr = mgr.prepare(txn, queryString, qc)
            res = expr.execute(txn, qc)
	    
            # note use of XmlQueryExpression.getQuery() and
            # XmlResults.size()
            print "The query '%s'\n\t returned %d result(s)"%(expr.getQuery(),res.size())
	    
            # process results -- just print them.  XmlResults
            # are a Python iterator
            for value in res:
                print "Result: %s"%value.asString()
            txn.commit()
        except XmlException, xe:
            print xe
            txn.abort()
    except XmlException, inst:
        print inst

def usage():
    print "queryTxn.py [-h home_directory]"

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
    containerName = "people.dbxml"
    # initialize...
    mgr = createEnvironment(home)
    # create/open a transactional container
    cont = createContainer(mgr, containerName, 
                           DBXML_TRANSACTIONAL)

    # run the guts of the program...
    queryTxn(mgr, cont)

if __name__ == "__main__":
    main()
