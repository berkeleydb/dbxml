#
# See the file LICENSE for redistribution information.
#
# Copyright (c) 2004,2009 Oracle. All rights reserved.
#

# External function
#
# A very simple Berkeley DB XML program that demonstrates
# external function API usage.
#
# This program demonstrates:
#  XmlResolver used to resolve multiple external functions
#  XmlExternalFunction implementations
#  External function results handling
#
# myResolver implements the XmlResolver interface, it is used to
# resolve the external function class myFunction.
#
# myFunction takes no arguments and just returns strings
#
# The myResolver class is responsible for the life cycle of myFunction
#
# The example is a bit noisy but that helps illustrates the order of
# events
#


from bsddb3.db import *
from dbxml import *

class myFunction(XmlExternalFunction):
    def __init__(self):
	XmlExternalFunction.__init__(self)
	print "myFunction constructor"

    def execute(self, txn, mgr, args):
        #print "myFunction -- execute"
        results = mgr.createResults()
        results.add(XmlValue("foo"))
        results.add(XmlValue("bar"))
        return results

    def close(self):
        print "myFunction -- close"
        del self
        
    def __del__(self):
        print "myFunction -- del"

class myResolver(XmlResolver):
    def __init__(self):
	XmlResolver.__init__(self)
        self.fun = myFunction()
        
    def resolveExternalFunction(self, txn, mgr, uri, name, numArgs):
        # verify the number of arguments, uri and name which uniquely
        # identify a function in XQuery
        if numArgs == 0 and uri == "http://my" and name == "foo":
            return self.fun
        else:
            print "myResolver -- could not resolve function"
    
    def __del__(self):
        print "myResolver -- del"

#
# The main program
#
# No containers are involved, just a simple query
#
try:        
    mgr = XmlManager()
    # create and register the resolver
    resolver = myResolver()
    mgr.registerResolver(resolver)

    # need to set the functions namespace/prefix
    qc = mgr.createQueryContext()
    qc.setNamespace("my", "http://my")
    query1 = "declare function my:foo() as xs:string* external; my:foo()"
    results = mgr.query(query1, qc)
    for value in results:
        print "value of result: ", value.asString()
except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
