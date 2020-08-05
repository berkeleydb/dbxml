//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __TESTLOGGING_HPP
#define __TESTLOGGING_HPP

#include <string>
#include <ostream>
#include <sstream>

#include "Transcoding.hpp"

namespace DbXmlTest
{
  // TestStream
  class TestStream
  {
  public:
    TestStream(std::ostream* sink, const std::string& msg);
    TestStream(const TestStream &o);
    ~TestStream();

    TestStream& operator()();

    TestStream& insert(const std::string& str);

  private:
    TestStream& operator=(const TestStream &o);

    std::stringstream ss_;
    std::ostream* os_;
    int position_;
    std::string msg_;
  };

  template<typename T>
  TestStream& operator<<(TestStream& stream, const T& t)
  {
    std::stringstream ss;
    ss << t;
    return stream.insert(ss.str());
  }
  template<char*>
  TestStream& operator<<(TestStream& stream, const char* str)
  {
    return stream.insert(str);
  }

  // TestLogger 
  class TestLogger
  {
  public:
    enum MessageType
      {
        Audit,
        Environment,
        Error,
        Information,
        Test,
        Warning,
	Pending
      };

    explicit TestLogger(const std::string& file);
    ~TestLogger();

    TestStream log(MessageType type, const std::string &msg);

  private:
    std::ostream *os_;
  };

  std::string verifyLogFile(const std::string &file);
} 

// macros that enable clients to use the logger
#define AUDIT_MSG(x, m)  (x).log(DbXmlTest::TestLogger::Audit, m)()
#define ENV_MSG(x, m)    (x).log(DbXmlTest::TestLogger::Environment, m)()
#define ERROR_MSG(x, m)  (x).log(DbXmlTest::TestLogger::Error, m)()
#define INFO_MSG(x, m)   (x).log(DbXmlTest::TestLogger::Information, m)()
#define TEST_MSG(x, m)   (x).log(DbXmlTest::TestLogger::Test, m)()
#define WARNING_MSG(x, m)(x).log(DbXmlTest::TestLogger::Warning, m)()
#define PENDING_MSG(x, m)(x).log(DbXmlTest::TestLogger::Pending, m)()

#endif
