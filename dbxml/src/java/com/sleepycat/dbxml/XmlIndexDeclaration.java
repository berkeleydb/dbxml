//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

package com.sleepycat.dbxml;

public class XmlIndexDeclaration {
    public String index;
    public String name;
    public String uri;


    public XmlIndexDeclaration(String uri, String name, String index) {
        this.uri = uri;
        this.name = name;
        this.index = index;
    }
}
