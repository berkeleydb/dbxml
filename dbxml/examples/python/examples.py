#!/usr/bin/env python
# As of Python 2.3, there is a built-in module for Berkeley DB,
# called bsddb.  The next uncommented line assumes that module is being used.
# Note the use of XmlException.  There are, as of BDB XML 2.3, a number
# of exception classes derived from XmlException.  See them defined
# in README.exceptions in dbxml/src/python.
# 
from bsddb3.db import *
from dbxml import *

book_content = r"<book><title>Knowledge Discovery in Databases.</title></book>"
book_content_ns = r"<book xmlns:books='http://foo.bar.com/books.dtd'><books:title>Knowledge Discovery in Databases.</books:title></book>"
book_name = r"book1"
book_name_ns = r"book1_ns"

def remove_database():
    """Removes the databases so we can start with a fresh environment for
    each test."""
    import os, glob
    databases = ["test", "test1", "test2"]
    for database in databases:
        files = glob.glob(database + "*.dbxml")
        files += glob.glob("__db*")
        files += glob.glob("log.*")
        for file in files:
            os.remove(file)

def example01():
    """Create an XmlManager/XmlContainer, add a document, get the document,
    display the content of the document.

    >>> remove_database()
    >>> example01()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """
    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    try:
	container = mgr.createContainer("test.dbxml")
	container.putDocument(book_name, book_content, uc)
	document = container.getDocument(book_name)
	s = document.getContent()
	print document.getName(), "=", s
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    
def example02():
    """Create an XmlManager/XmlContainer, add a document, query the container
    for the document, iterate over the result set displaying the
    values returned.

    >>> remove_database()
    >>> example02()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    try:
	container = mgr.createContainer("test.dbxml")
	container.putDocument(book_name, book_content, uc)
	qc = mgr.createQueryContext()
	results = mgr.query("collection('test.dbxml')/book", qc)
	results.reset()
	for value in results:
	    document = value.asDocument()
	    print document.getName(), "=", value.asString()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError

def example03():
    """Create an XmlContainer, add a document that includes a
    namespace definition, create a query context, define a
    namespace prefix to URI mapping, query the container
    for the document within a context, iterate over the
    result set displaying the values returned.

    >>> remove_database()
    >>> example03()
    book1_ns = <book xmlns:books="http://foo.bar.com/books.dtd"><books:title>Knowledge Discovery in Databases.</books:title></book>
    """

    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    try:
	container = mgr.createContainer("test.dbxml")
	container.putDocument(book_name_ns, book_content_ns, uc)
	qc = mgr.createQueryContext()
	qc.setNamespace("books2", "http://foo.bar.com/books.dtd")
	results = mgr.query("collection('test.dbxml')/*[books2:title='Knowledge Discovery in Databases.']", qc)
	results.reset()
	for value in results:
	    document = value.asDocument()
	    print document.getName(), "=", value.asString()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError

def example04():
    """Create an XmlContainer, define an equality string index
    for booktitle elements, add a document, query the container
    for the document, iterate over the result set displaying
    the values returned.

    Note that this example assumes that the 'test' container
    does not already exist. If it does exist then the
    addIndex method will throw an exception to complain
    that the container isn't empty.

    Python note: This exception is not passed on to Python, which
    will abort the interpreter.

    >>> remove_database()
    >>> example04()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    try:
	container = mgr.createContainer("test.dbxml")
	container.addIndex("", "title", "node-element-equality-string", uc)
	container.putDocument(book_name, book_content, uc)
	qc = mgr.createQueryContext()
	results = mgr.query("collection('test.dbxml')//*[title='Knowledge Discovery in Databases.']", qc)
	for value in results:
	    document = value.asDocument()
	    print document.getName(), "=", value.asString()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
		
def example05():
    """
    Create an XmlContainer, define an equality string index
    for booktitle elements, add a document, create a query
    context, define a variable binding, query the container
    for the document within a context referencing the variable
    defined, iterate over the result set displaying the values
    returned.

    Note that this example assumes that the 'test' container
    does not already exist. If it does exist then the
    addIndex method will throw an exception to complain
    that the container isn't empty.

    Python note: This exception is not passed on to Python, which
    will abort the interpreter.

    >>> remove_database()
    >>> example05()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    try:
	container = mgr.createContainer("test.dbxml")
	container.addIndex("", "title", "node-element-equality-string", uc)
	container.putDocument(book_name, book_content, uc)
	qc = mgr.createQueryContext()
	results = mgr.query("collection('test.dbxml')//*[title='Knowledge Discovery in Databases.']", qc)
	qc.setVariableValue("title", XmlValue("Knowledge Discovery in Databases."))
	results = mgr.query("collection('test.dbxml')//*[title=$title]", qc)
	for value in results:
	    document = value.asDocument()
	    print document.getName(), "=", value.asString()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError

def example06():
    """Create an XML Container, rename that container, delete the
    container, and repeat.

    >>> remove_database()
    >>> example06()
    """

    mgr = XmlManager()
    try:
	for i in range(2):
	    container = mgr.createContainer("test1.dbxml")
	    del container
	    mgr.renameContainer("test1.dbxml", "test2.dbxml")
	    container = mgr.openContainer("test2.dbxml")
	    del container
	    mgr.removeContainer("test2.dbxml")
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError

def example07():
    """Create an use a transactional XML Container:
    add a document, get the document, display the content of
    the document.  Requires using DBEnv.

    >>> remove_database()
    >>> example07()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    environment = DBEnv()
    environment.open(None, DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN, 0)
    try:
	mgr = XmlManager(environment, 0)
	uc = mgr.createUpdateContext()
	container = mgr.createContainer("test.dbxml", DBXML_TRANSACTIONAL)
	xtxn = mgr.createTransaction()
	container.putDocument(xtxn, book_name, book_content, uc)
	xtxn.commit()
	document = container.getDocument(book_name)
	s = document.getContent()
	print document.getName(), "=", s
	del uc
	del document
	del container
	#container.close()
	mgr.removeContainer("test.dbxml")
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    environment.close(0)

def example08():
    """Create an XML Container within a Berkeley DB environment,
    add a document, get the document, display the content of
    the document.

    >>> remove_database()
    >>> example08()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    environment = DBEnv()
    environment.open(None, DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN, 0)
    try:
	mgr = XmlManager(environment, 0)
	uc = mgr.createUpdateContext()
	container = mgr.createContainer("test.dbxml")
	container.putDocument(book_name, book_content, uc)
	document = container.getDocument(book_name)
	s = document.getContent()
	print document.getName(), "=", s
	del uc
	del document
	del container
	mgr.removeContainer("test.dbxml")
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    environment.close(0)

def example09():
    """Create an XML Container within a Berkeley DB environment,
    then within a Berkeley DB transaction, add a document,
    get the document, display the content of the document.
    Use a Berkeley DB transaction

    >>> remove_database()
    >>> example09()
    book1 = <book><title>Knowledge Discovery in Databases.</title></book>
    """

    environment = DBEnv()
    environment.open(None, DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN, 0)
    try:
	mgr = XmlManager(environment, 0)
	uc = mgr.createUpdateContext()
	container = mgr.createContainer("test.dbxml", DBXML_TRANSACTIONAL)
	xtxn = mgr.createTransaction();
	document = mgr.createDocument()
	document.setContent(book_content)
	document.setName(book_name)
	container.putDocument(xtxn, document, uc)
	document = container.getDocument(xtxn, book_name)
	s = document.getContent()
	print document.getName(), "=", s
	xtxn.commit()
	del document  # holds ref on container
	del container # prevents removal
	del uc
	mgr.removeContainer("test.dbxml")
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    environment.close(0)

def example10():
    """ Create two XML Containers within a Berkeley DB environment,
    then within a Berkeley DB transaction add a document to
    each container.

    >>> remove_database()
    >>> example10()
    """

    environment = DBEnv()
    environment.open(None, DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN, 0)
    try:
	mgr = XmlManager(environment, 0)
	uc = mgr.createUpdateContext()
	txn = environment.txn_begin()
	xtxn = mgr.createTransaction(txn)
	container1 = mgr.createContainer(xtxn, "test.dbxml")
	container2 = mgr.createContainer(xtxn, "test2.dbxml")
	container1.putDocument(xtxn, book_name, book_content, uc)
	container2.putDocument(xtxn, book_name, book_content, uc)
	txn.commit(0)
	del uc	
	del container1
	del container2
	del mgr
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    environment.close(0)

def example11():
    """Demonstrate the definition of an index, and the iteration over set set
    of indexes specified

    >>> remove_database()
    >>> example11()
    Indexes present:
      http://www.sleepycat.com/2002/dbxml:name unique-node-metadata-equality-string
      :title node-element-equality-date edge-element-equality-string
    """

    mgr = XmlManager()
    try:
	container = mgr.createContainer("test.dbxml")
	uc = mgr.createUpdateContext()
	""" addIndex uri, name, indexType updateContext"""
	# the following (commented) addIndex call is equivalent to the
	# 2 that follow it.
    #container.addIndex("", "title", "node-element-equality-date, edge-element-equality-string", uc)
	container.addIndex("", "title", XmlIndexSpecification.NODE_ELEMENT|
			   XmlIndexSpecification.PATH_NODE|
			   XmlIndexSpecification.KEY_EQUALITY, XmlValue.DATE, uc)
	container.addIndex("", "title", XmlIndexSpecification.NODE_ELEMENT|
			   XmlIndexSpecification.PATH_EDGE|
			   XmlIndexSpecification.KEY_EQUALITY, XmlValue.STRING, uc)
	# this is another way to add indexes..
	#    ispec = container.getIndexSpecification()
	#    ispec.addIndex("", "title", XmlIndexSpecification.NODE_ELEMENT|
	#		XmlIndexSpecification.PATH_NODE|XmlIndexSpecification.KEY_EQUALITY,
	#		XmlValue.DATE);
	#    container.setIndexSpecification(ispec, uc)
	n = 0
	print "Indexes present:"
	for index in container.getIndexSpecification():
	    print "  %s:%s %s" % (index.get_uri(), index.get_name(), index.get_index())
	container.putDocument("foo", "<title>1957-11-03</title>", uc)
	qc = mgr.createQueryContext()
	
	val = XmlValue(XmlValue.DATE, "1957-11-03")
	res = container.lookupIndex(qc, "", "title", "node-element-equality-date",
					val)
	# Shows how to look at a query plan
        #print res.size()
        #qe = mgr.prepare("collection('test.dbxml')/title[.<xs:date('1957-11-03')]", qc)
        #print qe.getQueryPlan()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
    

def example12():
    """Use WholedocContainer by setting default container type on XmlManager
    >>> remove_database()
    >>> example12()
    document:  <book><title text='\\"'>content's  got &quot; dquot</title></book>
    """
    content = r"<book><title text='\"'>content's  got &quot; dquot</title></book>"
    mgr = XmlManager()
    uc = mgr.createUpdateContext()
    mgr.setDefaultContainerType(XmlContainer.WholedocContainer)
    try:
	container = mgr.createContainer("test.dbxml")
	container.putDocument(book_name, content, uc)
	doc = container.getDocument(book_name)
	print "document: ", doc.getContent()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError


# Test example of an overloade XmlResolver instance in Python
# Used in example 13
class myResolver(XmlResolver):
    def __init__(self):
	XmlResolver.__init__(self)
	print "XmlResolver constructor"
    def resolveDocument(self, txn, mgr, uri, result):
	print "In resolveDocument, uri: %s" % (uri)
	content=r"<?xml version='1.0' ?><root xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://xxx mysch.xsd'><a>yyz</a></root>"
	doc = mgr.createDocument()
	doc.setContent(content)
	newval = XmlValue(doc)
	result.setValue(result, newval)
	return 1
    def resolveCollection(self, txn, mgr, uri, result):
	print "In resolveCollection, uri: %s" % (uri)
	return 0
    def resolveSchema(self, txn, mgr, location, nameSpace):
	print "In resolveSchema, location: %s" % (location)
	return 0
    def resolveEntity(self, txn, mgr, systemId, publicId):
	print "In resolveEntity, systemId: %s" % (systemId)
	dtd =r"<?xml version='1.0' encoding='utf-8' ?><!ELEMENT root (a)><!ELEMENT a (#PCDATA)>"
	dlen = len(dtd)
	istr = mgr.createMemBufInputStream(dtd, dlen, "dtd")
	return istr

    def __del__(self):
	XmlResolver.__del__(self)

def example13():
    """Example of creating an XmlResolver instance in Python
    >>> remove_database()
    >>> example13()
    XmlResolver constructor
    In resolveEntity, systemId: mydtd.dtd
    In resolveDocument, uri: myscheme:/docname
    value of result:  <a>yyz</a>
    """
    
    resolver = myResolver()
    try:
	mgr = XmlManager(DBXML_ALLOW_EXTERNAL_ACCESS)
	mgr.registerResolver(resolver)
	uc = mgr.createUpdateContext()
	container = mgr.createContainer("test.dbxml", DBXML_ALLOW_VALIDATION,
					XmlContainer.NodeContainer)
	container.putDocument("foo0", r"<?xml version='1.0' ?><!DOCTYPE root SYSTEM 'mydtd.dtd'><root><a>yyz</a></root>", uc);
	container.putDocument("foo", "<root/>", uc);
	container.putDocument(book_name, book_content, uc)
	
	
	# query to test the resolver
	qc = mgr.createQueryContext()
	qc.setBaseURI("myscheme:/")
	results = mgr.query("doc('docname')/root/a", qc)
	for value in results:
	    document = value.asDocument()
	    print "value of result: ", value.asString()
    except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError

NUMBER_OF_EXAMPLES = 13

def do_example(number):
    print "Running example %d." % number
    globals()["example%02d" % number]()
    
if __name__ == "__main__":
    import sys
    number = sys.argv[-1]
    if number == "test":
        import doctest, examples
        doctest.testmod(examples)
        sys.exit()
        
    try:
        number = int(number)
    except ValueError:
        print "Usage: ./examples.py <example_number>|test"
        sys.exit()

    if number < 1 or number > NUMBER_OF_EXAMPLES:
        print "Usage: ./examples.py <example_number>|test"
        print "Example number out of range (1-%d)" % NUMBER_OF_EXAMPLES
        sys.exit()

    remove_database()
    do_example(number)
