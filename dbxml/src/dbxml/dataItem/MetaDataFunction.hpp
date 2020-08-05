//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __METADATAFUNCTION_HPP
#define	__METADATAFUNCTION_HPP

#include "DbXmlFunction.hpp"

#include <xqilla/runtime/SingleResult.hpp>

namespace DbXml
{

class MetaDataFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	MetaDataFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	class MetaDataResult : public SingleResult
	{
	public:
		MetaDataResult(const MetaDataFunction *func);
		Item::Ptr getSingleResult(DynamicContext *context) const;
	private:
		const MetaDataFunction *func_;
	};
};

}

#endif
