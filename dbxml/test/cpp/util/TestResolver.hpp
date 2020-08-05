//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// Simple resolve that allows a directory path to be set for resolving system
// entities. Written initially to replace test 9.1.9 (loading of a document 
// containing a DTD in the local directory)

#ifndef __TESTRESOLVER_HPP
#define __TESTRESOLVER_HPP

#include <string>

#include "dbxml/DbXml.hpp"

namespace DbXmlTest {

  class TestResolver : public DbXml::XmlResolver
  {
  public:
    TestResolver();
    virtual ~TestResolver();

    // overrides from XmlResolver
    virtual DbXml::XmlInputStream *resolveEntity(DbXml::XmlTransaction *txn, DbXml::XmlManager &mgr,
						 const std::string &systemId,
						 const std::string &publicId) const;

    // access to root directory for system entities
    void setDir(const std::string &dir);
    std::string getDir() const;

  private:
    TestResolver(const TestResolver &);
    TestResolver &operator=(const TestResolver &);

    std::string dir_;
  };
}

#endif

