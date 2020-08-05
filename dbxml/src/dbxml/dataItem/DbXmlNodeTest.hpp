//
//

#ifndef __DBXMLNODETEST_HPP
#define	__DBXMLNODETEST_HPP

#include "../nodeStore/NsTypes.hpp"

#include <xqilla/axis/NodeTest.hpp>

namespace DbXml
{

class DbXmlNodeTest : public NodeTest
{
public:
	virtual ~DbXmlNodeTest();
	
	/** The DB XML NodeTest interface */
	static const XMLCh gDbXml[];

	DbXmlNodeTest(const XMLCh *nodeType);
	DbXmlNodeTest(const XMLCh *nodeType, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager *mm);
	DbXmlNodeTest(const NodeTest *other, XPath2MemoryManager *memMgr);

	virtual void *getInterface(const XMLCh *name) const;

	const xmlbyte_t* getNodeName8() const { return name8_; }
	const xmlbyte_t* getNodeUri8() const { return uri8_; }

protected:
	const xmlbyte_t *name8_;
	const xmlbyte_t *uri8_;
};

}

#endif
