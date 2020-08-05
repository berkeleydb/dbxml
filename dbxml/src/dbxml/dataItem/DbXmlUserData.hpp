//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLUSERDATA_HPP
#define	__DBXMLUSERDATA_HPP

#include "../query/ImpliedSchemaNode.hpp"

namespace DbXml
{

class DbXmlUserData
{
public:
	DbXmlUserData(XPath2MemoryManager *mm)
		: paths(XQillaAllocator<ImpliedSchemaNode*>(mm)),
		  paths2(XQillaAllocator<ImpliedSchemaNode*>(mm)) {}

	ImpliedSchemaNode::MVector paths;
	ImpliedSchemaNode::MVector paths2;
};

}

#endif
