//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLASTNODE_HPP
#define	__DBXMLASTNODE_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

namespace DbXml
{

class DbXmlASTNode : public ASTNodeImpl
{
public:
	enum whichType {
		NODE_CHECK = ASTNode::DIRECT_NAME + 20,
		LAST_STEP_CHECK,
		DBXML_PREDICATE,
		QP_TO_AST
	};

	DbXmlASTNode(whichType type, XPath2MemoryManager* memMgr)
		: ASTNodeImpl((ASTNode::whichType)type, memMgr)
	{
	}

	virtual ~DbXmlASTNode() {}

	/// This has been called by the time any DbXmlASTNode
	/// objects are created, and won't be called again.
	virtual ASTNode *staticResolution(StaticContext *context)
	{
		return this;
	}
};

}

#endif
