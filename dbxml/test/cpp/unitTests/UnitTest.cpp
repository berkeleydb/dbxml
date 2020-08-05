//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "dbxml/DbXml.hpp"

#include "TestEnvironment.hpp"
#include "UnitTest.hpp"

using namespace DbXmlTest;

UnitTest::UnitTest(TestEnvironment &env, const std::string &name)
  : env_(env), name_(name)
{
}

UnitTest::~UnitTest()
{
}

// preparation is optional
void UnitTest::prepare()
{
}
