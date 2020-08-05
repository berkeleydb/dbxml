//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __UNITTEST_HPP
#define __UNITTEST_HPP

#include <string>

namespace DbXmlTest
{
  class TestEnvironment;

  class UnitTest
  {
  public:
    UnitTest(TestEnvironment &env, const std::string &name);
    virtual ~UnitTest();

    virtual void prepare();
    virtual void execute() = 0;

  protected:

    DbXmlTest::TestEnvironment &env_;
    const std::string name_;

  private:
    UnitTest(const UnitTest &);
    UnitTest &operator=(const UnitTest &);
  };
}

#endif
