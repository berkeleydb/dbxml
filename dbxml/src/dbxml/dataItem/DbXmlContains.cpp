//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlContains.hpp"

#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/UCANormalizer.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlContainsFunction::name[] = {
	chLatin_c, chLatin_o, chLatin_n, 
	chLatin_t, chLatin_a, chLatin_i, 
	chLatin_n, chLatin_s, chNull 
};
const unsigned int DbXmlContainsFunction::minArgs = 2;
const unsigned int DbXmlContainsFunction::maxArgs = 2;

DbXmlContainsFunction::DbXmlContainsFunction(const VectorOfASTNodes &args, XPath2MemoryManager *memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string?, string?", args, memMgr)
{
}

ASTNode *DbXmlContainsFunction::staticResolution(StaticContext *context)
{
	return resolveArguments(context);
}

ASTNode *DbXmlContainsFunction::staticTypingImpl(StaticContext *context)
{
	_src.clear();

	_src.getStaticType() = StaticType::BOOLEAN_TYPE;
	return calculateSRCForArguments(context);
}

Sequence DbXmlContainsFunction::createSequence(DynamicContext *context, int flags) const
{
	Item::Ptr item1 = getParamNumber(1,context)->next(context);
	Item::Ptr item2 = getParamNumber(2,context)->next(context);

	const XMLCh *str1 = item1.isNull() ? XMLUni::fgZeroLenString : item1->asString(context);
	const XMLCh *str2 = item2.isNull() ? XMLUni::fgZeroLenString : item2->asString(context);

	bool result;
	if(str2 == 0 || *str2 == 0) result = true;
	else if(str1 == 0 || *str1 == 0) result = false;
	else {
		XMLBuffer buf1, buf2;
		Normalizer::caseFoldAndRemoveDiacritics(str1, buf1);
		Normalizer::caseFoldAndRemoveDiacritics(str2, buf2);

		result = XMLString::patternMatch(buf1.getRawBuffer(), buf2.getRawBuffer()) > -1;
	}

	return Sequence(context->getItemFactory()->createBoolean(result, context), context->getMemoryManager());
}

