//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "dbxml/XmlPortability.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <ctime>
#include <iomanip>

#include "TestLogging.hpp"

using namespace std;

// helper stuff in an anonymous namespace
namespace {

  // adds timestamp and type to a message
	string prettify(DbXmlTest::TestLogger::MessageType type, const string& msg)
	{
    static std::map<DbXmlTest::TestLogger::MessageType, string> messageTypes;
    messageTypes[DbXmlTest::TestLogger::Audit] = "AUDIT";
    messageTypes[DbXmlTest::TestLogger::Environment] = "ENVIRONMENT";
    messageTypes[DbXmlTest::TestLogger::Error] = "ERROR";
    messageTypes[DbXmlTest::TestLogger::Information] = "INFORMATION";
    messageTypes[DbXmlTest::TestLogger::Test] = "TEST";
    messageTypes[DbXmlTest::TestLogger::Warning] = "WARNING";
    messageTypes[DbXmlTest::TestLogger::Pending] = "PENDING";

		stringstream ss;
		time_t t(time(0));
		tm *tstruct = localtime(&t);
    // YYYY-MM-DD HH:MM:SS  (understood by the Tcl clock() function)
    unsigned int y = 1900 + tstruct->tm_year;
 		ss << setfill('0') << setw(4) << y << '-'
      << setfill('0') << setw(2) << tstruct->tm_mon + 1 << '-' 
      << setfill('0') << setw(2) << tstruct->tm_mday << ' ' 
      << setfill('0') << setw(2) << tstruct->tm_hour << ':' 
			<< setfill('0') << setw(2) << tstruct->tm_min << ':'
			<< setfill('0') << setw(2) << tstruct->tm_sec << ' '
			<< messageTypes[type] << ' '
			<< msg;
		return ss.str();
	}
}

using namespace DbXmlTest;

/////////////////////////////////////////////////////////////////////
// TestStream methods
TestStream::TestStream( std::ostream* os, const std::string& msg) 
  : os_(os), position_(1), msg_(msg)
{
}

TestStream::TestStream(const TestStream &o) 
  : os_(o.os_), position_(o.position_), msg_(o.msg_)
{
}

TestStream::~TestStream()
{
  ss_ << msg_ << "." << endl;
  *os_ << ss_.str();
  os_->flush();
}

TestStream& TestStream::operator()()
{
	return *this;
}

// deal with '%n' placeholders
TestStream& TestStream::insert(const std::string& str)
{
  if(!os_) return (*this);

  stringstream pattern;
  pattern << "%" << position_;
  string::size_type idx = msg_.find(pattern.str());
  if (idx == string::npos)
	{
    // not found, tag it the end
    msg_ += " ";
    msg_ += str;
  }
  else
	{	
    // replace place holder
    string newMsg(msg_.substr(0, idx));
    newMsg += str;

    // add any remaining string
    string::size_type pos = msg_.find_first_not_of("%0123456789", idx);
    if (pos != string::npos)
		{
      newMsg += msg_.substr(pos, msg_.length());
		}

    position_++;
    msg_ = newMsg;
  }

	return (*this);
}

/////////////////////////////////////////////////////////////////////
// TestLogger methods

TestLogger::TestLogger(const string& file)
{
	string filename(file);
  os_ = new ofstream(filename.c_str(), ios::out | ios::app);

	time_t t(time(0));
	*os_ <<prettify(Audit, "Log opened at ") << asctime(localtime(&t));
}

TestLogger::~TestLogger()
{
	time_t t(time(0));
	*os_ << prettify(Audit, "Log closed at ") << asctime(localtime(&t));
	delete os_;
}

TestStream TestLogger::log(MessageType type, const string &msg)
{
	return TestStream(os_, prettify(type, msg));
}

// verifyLogFile
//
// utility method; reads UTF8 log file, reports warnings and errors to stdout
string DbXmlTest::verifyLogFile(const string &filename)
{
  ostringstream os;
  os << "Verifying log file...";

  // 2004-10-05 ARW analyse the timestamp to discount old test output
  string timeStampTemplate("YYYY-MM-DD HH:MM:SS");

  int warningCount = 0;
  int errorCount = 0;
  bool fileErr = true;

	ifstream is;
	is.open(filename.c_str(), ios::in);
	if(is.is_open())
	{
    fileErr = false;
		while(!is.eof())
		{
			string s;
			getline(is,s);

      if(s.find("WARNING") == (timeStampTemplate.length() + 1)) {
        ++warningCount;
      }
      if(s.find("ERROR") == (timeStampTemplate.length() + 1)) {
        ++errorCount;
      }
		}
	}

  bool oops = (fileErr || warningCount || errorCount);

  if(oops) {
    if(errorCount) {
      os << "\n" << "\t" << errorCount << " ERRORs found";
    }
    if(warningCount) {
      os << "\n" << "\t" << warningCount << " WARNINGs found";
    }
    if(fileErr) {
      os << "No log file?";
    }
  } else {
    os << "OK";
  }
  os << "\n";

  return os.str();
}
