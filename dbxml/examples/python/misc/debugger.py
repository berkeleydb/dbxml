#
# See the file LICENSE for redistribution information.
#
# Copyright (c) 2004,2009 Oracle. All rights reserved.
#
#
# Debug API
# A very simple Berkeley DB XML program that demonstrates
# the XQuery debug API.
# 
# This program demonstrates how to set a custom debug listener 
# class on the XmlQueryContext, and how to derive a class form 
# XmlDebugListener. The derived class, myDebugListener, simply 
# prints out partial execution information when a query is executed.

from bsddb3.db import *
from dbxml import *

class myDebugListener(XmlDebugListener):
    def __init__(self):
	XmlDebugListener.__init__(self)
        self.depth = 0
        self.outputFormat = "%s: column:line : %s:%s"

    def start(self, frame):
        self.depth += 1
        self.indent()
        print self.outputFormat %("{start}", frame.getQueryColumn(), frame.getQueryLine())

    def end(self, frame):
        self.indent()
        self.depth -= 1
        print self.outputFormat %("{end}", frame.getQueryColumn(), frame.getQueryLine())

    def enter(self, frame):
        self.depth += 1
        self.indent()
        print self.outputFormat %("{enter}", frame.getQueryColumn(), frame.getQueryLine())

    def exit(self, frame):
        self.indent()
        self.depth -= 1
        print self.outputFormat %("{exit}", frame.getQueryColumn(), frame.getQueryLine())

    def error(self, exc, frame):
        print "myDebugListener -- error"
        
    def indent(self):
        i = 0;
        while i < self.depth:
            print "%s" % " ",
            i += 1

    def __del__(self):
        print "myDebugListener -- del"


# utility function
def populateContainer(mgr, cont):
    uc = mgr.createUpdateContext()
    cont.putDocument("doc", "<root><a>1</a><a>2</a><a>3</a><a>4</a></root>", uc)

#
# The main program
#
# No containers are involved, just a simple query
#
try:        
    mgr = XmlManager()

    listener = myDebugListener()
    qc = mgr.createQueryContext()
    qc.setDebugListener(listener)
    # use in-memory container
    cont = mgr.createContainer("")
    cont.addAlias("c")
    populateContainer(mgr, cont)

    # new the query
    query = "for $a in collection('c')/root/a\n where $a>'2'\n return $a";
    results = mgr.query(query, qc)
    for value in results:
        print "value of result: ", value.asString()
except XmlException, inst:
	print "XmlException (", inst.exceptionCode,"): ", inst.what
	if inst.exceptionCode == DATABASE_ERROR:
	    print "Database error code:",inst.dbError
