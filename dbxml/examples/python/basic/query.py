"""
 * Query is a very simple Berkeley DB XML program
 * that performs a query and handles results.
 * It demonstrates initialization, container creation,
 * document insertion, query creation and execution,
 * use of a variable in a query and context, and
 * results handling.
 *
 * To run the example:
 *     python query.py
"""
from dbxml import *

def initialize(cname):
    # all BDB XML programs require an XmlManager instance
    mgr = XmlManager()
    
    # remove the container if it exists
    if mgr.existsContainer(cname) != 0:
        mgr.removeContainer(cname)

    # create a default container type
    cont = mgr.createContainer(cname)
    return cont

def query(mgr, cont):
    content = "<people><person><name>joe</name></person><person><name>mary</name></person></people>"
    docName = "people"
    queryString = "collection('%s')/people/person[name=$name]"%cont.getName()
    try:
        # all modification operations need XmlUpdateContext
        uc = mgr.createUpdateContext()
        cont.putDocument(docName, content, uc)
        
        # querying requires an XmlQueryContext
        qc = mgr.createQueryContext()

        # add a variable to the context, as required by the query
        qc.setVariableValue("name", XmlValue("mary"))

        # note: these two calls could be replaced by a shortcut
        # mgr.query(queryString, qc), but under the covers,
        # this is what is happening.
        expr = mgr.prepare(queryString, qc)
        res = expr.execute(qc)
	    
        # note use of XmlQueryExpression.getQuery() and
        # XmlResults.size()
        print "The query '%s'\n\t returned %d result(s)"%(expr.getQuery(),res.size())
	    
        # process results -- just print them.  XmlResults
        # are a Python iterator
	for value in res:
	    print "Result: %s"%value.asString()

    except XmlException, inst:
        print inst

import sys
if __name__ == "__main__":
    try:
        containerName = "people.dbxml"
        cont = initialize(containerName)
    except XmlException, xe:
        print xe
        sys.exit()

    query(cont.getManager(), cont)

