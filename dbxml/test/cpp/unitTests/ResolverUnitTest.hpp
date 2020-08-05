//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __RESOLVERUNITTEST_HPP
#define __RESOLVERUNITTEST_HPP

#include <string>

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

namespace DbXmlTest
{
  class ResolverUnitTest : public UnitTest
  {
  public:
    ResolverUnitTest(TestEnvironment &env, const Iterator<std::string> &args = EMPTY(std::string));
    virtual ~ResolverUnitTest();

    virtual void execute();

  private:
    ResolverUnitTest(const ResolverUnitTest &);
    ResolverUnitTest &operator=(const ResolverUnitTest &);

  };
}

#endif
