/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id: FunctionIriToUri.cpp 580 2008-08-11 15:49:41Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionIriToUri.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>

const XMLCh FunctionIriToUri::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionIriToUri::minArgs = 1;
const unsigned int FunctionIriToUri::maxArgs = 1;

static const XMLCh HEX_DIGITS[16] = 
{ 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_4, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_6, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_7, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_8, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_B, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_C, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_E, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F
};

static bool isUCSCharOrIPrivate(XMLCh ch)
{
    // TODO: handle non-BMP characters

    //ucschar        = %xA0-D7FF / %xF900-FDCF / %xFDF0-FFEF
    //                 / %x10000-1FFFD / %x20000-2FFFD / %x30000-3FFFD
    //                 / %x40000-4FFFD / %x50000-5FFFD / %x60000-6FFFD
    //                 / %x70000-7FFFD / %x80000-8FFFD / %x90000-9FFFD
    //                 / %xA0000-AFFFD / %xB0000-BFFFD / %xC0000-CFFFD
    //                 / %xD0000-DFFFD / %xE1000-EFFFD

    //iprivate       = %xE000-F8FF / %xF0000-FFFFD / %x100000-10FFFD
    return (ch>0xA0 && ch<0xD7FF) || (ch>0xF900 && ch<0xFDCF) || (ch>0xFDF0 && ch<0xFFEF) || (ch>0xE000 && ch<0xF8FF);
}

/*
  fn:iri-to-uri($uri-part as xs:string?) as xs:string  
 */

FunctionIriToUri::FunctionIriToUri(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
  _src.getStaticType() = StaticType::STRING_TYPE;
}

#if _XERCES_VERSION >= 30000
typedef XMLSize_t stringLen_t;
#else
typedef unsigned int stringLen_t;
#endif

Sequence FunctionIriToUri::createSequence(DynamicContext* context, int flags) const
{
    Sequence uriPart=getParamNumber(1,context)->toSequence(context);
    if(uriPart.isEmpty())
        return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), context->getMemoryManager());

    const XMLCh* source = uriPart.first()->asString(context);
    stringLen_t len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(source);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer outString(len+1, context->getMemoryManager());
    XERCES_CPP_NAMESPACE_QUALIFIER XMLUTF8Transcoder utf8Trans(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgUTF8EncodingString, 10, context->getMemoryManager());
    for(stringLen_t i=0;i<len;i++)
    {
        // If $uri-part contains a character that is invalid in an IRI, such as a space character, the invalid character is 
        // replaced by its percent-encoded form as described in [RFC 3986] before the conversion is performed.
        if(source[i]==XERCES_CPP_NAMESPACE_QUALIFIER chSpace)
        {
            outString.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
            outString.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2);
            outString.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
        }
        else if(isUCSCharOrIPrivate(source[i]))
        {
            XMLByte utf8Str[8];
            stringLen_t charsEaten;
            stringLen_t nLen=utf8Trans.transcodeTo(&source[i], 1, utf8Str, 7, charsEaten, XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder::UnRep_RepChar);
            for(stringLen_t j=0;j<nLen;j++)
            {
                outString.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
                outString.append(HEX_DIGITS[utf8Str[j] >> 4]);
                outString.append(HEX_DIGITS[utf8Str[j] & 0xF]);
            }
        }
        else
            outString.append(source[i]);
    }

    return Sequence(context->getItemFactory()->createString(outString.getRawBuffer(), context), context->getMemoryManager());
}
