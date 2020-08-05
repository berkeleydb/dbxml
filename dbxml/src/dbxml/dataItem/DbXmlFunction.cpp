//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include "DbXmlFunction.hpp"
#include "../QueryContext.hpp"

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

/* http://www.sleepycat.com/2002/dbxml */
const XMLCh DbXmlFunction::XMLChFunctionURI[] = {
	chLatin_h,
	chLatin_t,
	chLatin_t,
	chLatin_p,
	chColon,
	chForwardSlash,
	chForwardSlash,
	chLatin_w,
	chLatin_w,
	chLatin_w,
	chPeriod,
	chLatin_s,
	chLatin_l,
	chLatin_e,
	chLatin_e,
	chLatin_p,
	chLatin_y,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chPeriod,
	chLatin_c,
	chLatin_o,
	chLatin_m,
	chForwardSlash,
	chDigit_2,
	chDigit_0,
	chDigit_0,
	chDigit_2,
	chForwardSlash,
	chLatin_d,
	chLatin_b,
	chLatin_x,
	chLatin_m,
	chLatin_l,
	chNull
};

DbXmlFunction::DbXmlFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo,
	const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: XQFunction(name, argsFrom, argsTo, paramDecl, args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

void DbXmlFunction::getQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name, DynamicContext *context) const
{
	// Convert the string to a qname, thus checking it's syntax
	// and resolving it's prefix in one go
	AnyAtomicType::Ptr qname = context->getItemFactory()->
		createDerivedFromAtomicType(AnyAtomicType::QNAME, getParamNumber(argNum, context)
			->next(context)->asString(context), context);
	uri = ((const ATQNameOrDerived*)qname.get())->getURI();
	name = ((const ATQNameOrDerived*)qname.get())->getName();
}

bool DbXmlFunction::getConstantQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name, DynamicContext *context) const
{
	// argNum is 1-based, not 0-based, but _args is 0-based
	if (!_args[argNum - 1]->isConstant()) {
		return false;
	}

	getQNameArg(argNum, uri, name, context);

	return true;
}
