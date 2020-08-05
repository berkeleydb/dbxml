//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLFUNCTION_HPP
#define	__DBXMLFUNCTION_HPP

#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/ast/XQFunction.hpp>

namespace DbXml
{

class DbXmlFunction : public XQFunction
{
public:
	static const XMLCh XMLChFunctionURI[];

	void getQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name,
			 DynamicContext *context) const;
	bool getConstantQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name,
				 DynamicContext *context) const;
	
protected:
	DbXmlFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo,
		      const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
};
	
template<class TYPE>
class DbXmlFuncFactory : public FuncFactory
{
public:
	DbXmlFuncFactory(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
		: FuncFactory(TYPE::XMLChFunctionURI, TYPE::name, TYPE::minArgs, TYPE::maxArgs, memMgr)
	{
	}
  
	virtual ASTNode *createInstance(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const
	{
		return new (memMgr) TYPE(args, memMgr);
	}
};

}

#endif
