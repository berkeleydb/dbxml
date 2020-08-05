//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANTEST_HPP
#define __QUERYPLANTEST_HPP

#include <string>

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

namespace DbXmlTest
{
  class QueryPlanTest : public UnitTest
  {
  public:
    QueryPlanTest(TestEnvironment &env, const Iterator<std::string> &args = EMPTY(std::string));
    virtual ~QueryPlanTest();

    virtual void execute();

  private:
    std::string optimizedQueryPlan(u_int32_t indent = 0);
    std::string indexSpecification(u_int32_t indent = 0);

    QueryPlanTest(const QueryPlanTest &);
    QueryPlanTest &operator=(const QueryPlanTest &);
 
    void doTest();

    std::string query_;
    std::string outputDirectory_;
    std::string oldOQP_;
  };

}

#endif
