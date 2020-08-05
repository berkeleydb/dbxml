//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// Generates query plans for a specified container query. The container
// already exists and may contain an index specification.
//
// The output allows manual verification of whether the new
// query plans are equal to, or better, than the 1.2 query plans
//

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// System includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Xerces includes

// XQilla includes

// test code includes
#include "../util/TestLogging.hpp"
#include "TestEnvironment.hpp"
#include "QueryPlanTest.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;

namespace {
  void usage(const string &progname, int exitCode)
  {
    cerr << "Usage: " << progname << " [INHERITED OPTIONS] [OPTIONS]" << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "\t-o|--outdir dir\t\tdir for outputs (mandatory)" << endl;
    cerr << "\t-q|--query query\tcontainer query (mandatory)" << endl;
    cerr << "\t-r|--oldresult oqp\told OQP for comparison (optional)" << endl;
    cerr << endl;
    cerr << "Inherited options are those for the calling program." << endl;
    cerr << endl;

    exit(exitCode);
  }

  string getIndent(int indent)
  {
    ostringstream s;
    for(int i = indent; i != 0; --i)
      s << "  ";
    return s.str();
  }
}

QueryPlanTest::QueryPlanTest(TestEnvironment &env, const Iterator<std::string> &args)
  : UnitTest(env, "QueryPlanTest")
{
  TestLogger *log = env_.log();

  // process arguments (need query, output directory)
  string errMsg;
  bool ok(true);
  while(args.hasNext() && ok) {
    string arg(args.next());
    if(!arg.compare(0, 1, "-")) {
      if(!arg.compare(1, 1, "o") || !arg.compare(1, 7, "-outdir")) {
        if(!args.hasNext()) {
          errMsg.append("No output directory argument specified!");
          ok = false;
        } else {
          string s(args.next());
          // ensure we have a trailing slash
          // FIXME x-platform compatability?
          string::size_type pos = s.rfind("/");
          if(pos != s.length()-1) { //maybe on windows?
            pos = s.rfind("\\");
          }
          if (pos != s.length()-1) { //just add a slash
            s.append("/");
          }
          outputDirectory_.assign(s);
        }
      } else if(!arg.compare(1, 1, "q") || !arg.compare(1, 6, "-query")) {
        if(!args.hasNext()) {
          errMsg.append("No query argument specified!");
          ok = false;
        } else {
          string s(args.next());
          query_.assign(s);
        }
      } else if(!arg.compare(1, 1, "r") || !arg.compare(1, 10, "-oldresult")) {
        if(!args.hasNext()) {
          errMsg.append("No OQP specified!");
          ok = false;
        } else {
          string s(args.next());
          oldOQP_.assign(s);
        }
      }
      else {
        // unknown switch - ignore
      }
    }
    else {
      // unknown argument - ignore
    }
  }

  if(!errMsg.empty()) {
    ERROR_MSG(*log, "%1") << errMsg;
  	usage(name_, -1);
    return;
  }

  if(outputDirectory_.empty() || query_.empty()) {
    ERROR_MSG(*log, "No output directory or query specified!");
    return;
  }
  INFO_MSG(*log, "Results will be written to '%1'") << outputDirectory_;
  INFO_MSG(*log, "Executing the query '%1'") << query_;
  INFO_MSG(*log, "Old OQP is '%1'") << oldOQP_;
}

QueryPlanTest::~QueryPlanTest()
{
}

void QueryPlanTest::execute()
{
	Sequence result = env_.query("//base");
	if(result.getLength() > 0) {
		// any individual tests will contain instructions on setting
		// up the container
		for (Sequence::iterator it = result.begin();
		     it != result.end(); it++) {
			if ((*it)->isNode()) {
				// set up the container with indexes, variables
				Sequence res1 = env_.query("//variable");
				env_.addVariables(res1);

				res1 = env_.query("//indexSpecification");
				env_.addIndexSpecifications(res1);

				doTest();
			}
		}
	} else {
		// just do the test, using the container as seen
		doTest();
	}
}

static const u_int32_t INDENT = 1;

// writes the test id, old result, optimized query plan to an XML file
void QueryPlanTest::doTest()
{
  TestLogger *log = env_.log();

  // result documents, eager evaluation
  XmlQueryContext *xqc = env_.context();
  xqc->setEvaluationType(XmlQueryContext::Eager);

  ostringstream oss;
  u_int32_t indent(0);

  // output - root element (with test id)
  oss << "<test";
  oss << " id = \"" << env_.id() << "\"";
  oss << ">" << endl;
  indent += INDENT;

  // output - index specification
  oss << indexSpecification(indent);

  // output - query
  oss << getIndent(indent);
  oss << "<query>" << endl;
  indent += INDENT;
  oss << getIndent(indent) << query_ << endl;
  indent -= INDENT;
  oss << getIndent(indent) << "</query>" << endl;

  // output - old OQP
  if(!oldOQP_.empty()) {
    oss << getIndent(indent);
    oss << "<oldOQP>" << endl;
    indent += INDENT;
    oss << getIndent(indent) << oldOQP_ << endl;
    indent -= INDENT;
    oss << getIndent(indent) << "</oldOQP>" << endl;
  }

  // output - optimized query plan
  oss << optimizedQueryPlan(indent);

  // output - close root
  indent -= INDENT;
  oss << getIndent(indent) << "</test>" << endl;

  // dump to file
  string file(outputDirectory_);
  file.append(env_.id());
  file.append(".xml");
  TEST_MSG(*log, "Writing results to '%1'") << file;

  ofstream out(file.c_str(), std::ios::out);
  out << oss.str();

  TEST_MSG(*log, "Successful completion of test");
}

// gets the index specification on the container
// throws upon error
std::string QueryPlanTest::indexSpecification(u_int32_t indent)
{
  TestLogger *log = env_.log();

  // get index specification on the container
  XmlContainer *xc = env_.container();
  XmlIndexSpecification xis;
  if(env_.transacted()) {
    XmlTransaction txn(env_.createTransaction());
    xis = xc->getIndexSpecification(txn);
    env_.commitTransaction(txn);
  } else {
    xis = xc->getIndexSpecification();
  }

  // get the details
  ostringstream oss;
  string uri, node, index;
  while(xis.next(uri, node, index)) {
    INFO_MSG(*log, "Found index '%1'/'%2'/'%3'") << uri << node << index;

    oss << getIndent(indent);
    oss << "<indexSpecification>" << endl;
    indent += INDENT;

    oss << getIndent(indent) << "<uri>" << uri << "</uri>" << endl;
    oss << getIndent(indent) << "<node>" << node << "</node>" << endl;
    oss << getIndent(indent) << "<index>" << index << "</index>" << endl;

    indent -= INDENT;
    oss << getIndent(indent) << "</indexSpecification>" << endl;
  }

  return oss.str();
}

// gets the optimized query plan
// throws upon error
std::string QueryPlanTest::optimizedQueryPlan(u_int32_t indent)
{
  // use external code to get query expression
  XmlQueryContext *xqc = env_.context();
  XmlContainer *xc = env_.container();
  XmlManager *mgr = env_.db();
  string queryString("collection(" + xc->getName() +
		     ")" + query_);

  XmlQueryExpression xqe = mgr->prepare(queryString, *xqc);

  TestLogger *log = env_.log();
  TEST_MSG(*log, "Got optimized query plan");

  return xqe.getQueryPlan();
}
