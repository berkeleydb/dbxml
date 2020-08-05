//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlNodeTest.hpp"
#include "../nodeStore/NsUtil.hpp"
#include "../Manager.hpp"

#include <xercesc/util/XMLUniDefs.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlNodeTest::gDbXml[] =
{
	chLatin_D, chLatin_b,
	chLatin_X, chLatin_m,
	chLatin_l,
	chNull
};

DbXmlNodeTest::DbXmlNodeTest(const XMLCh *nodeType)
        : NodeTest(nodeType),
	  name8_(0),
          uri8_(0)
{
}

DbXmlNodeTest::DbXmlNodeTest(const XMLCh *nodeType, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager *mm)
        : NodeTest(nodeType, uri, name),
	  name8_(0),
          uri8_(0)
{
	NsDonator donator(_name, _uri, mm);
	name8_ = donator.getStr();
	uri8_ = donator.getStr2();
}

DbXmlNodeTest::DbXmlNodeTest(const NodeTest *other, XPath2MemoryManager *mm)
	: NodeTest(other),
	  name8_(0),
	  uri8_(0)
{
	NsDonator donator(_name, _uri, mm);
	name8_ = donator.getStr();
	uri8_ = donator.getStr2();
}

DbXmlNodeTest::~DbXmlNodeTest()
{
	if (name8_)
		NsUtil::deallocate((void*)name8_);
	if (uri8_)
		NsUtil::deallocate((void*)uri8_);
}

void *DbXmlNodeTest::getInterface(const XMLCh *name) const
{
	if(name == gDbXml) {
		return (void*)this;
	}
	return 0;
}

