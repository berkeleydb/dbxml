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
 * $Id: FunctionLookup.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/functions/ExternalFunction.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

XERCES_CPP_NAMESPACE_USE

#define SECONDARY_KEY(func)(((func)->getMinArgs() << 16) | (func)->getMaxArgs())

FunctionLookup *FunctionLookup::g_globalFunctionTable = 0;
XPath2MemoryManager *FunctionLookup::g_memMgr = 0;

FunctionLookup::FunctionLookup(XPath2MemoryManager* memMgr)
  : _funcTable(197, false, memMgr),
    _exFuncTable(7, false, memMgr)
{
}

FunctionLookup::~FunctionLookup()
{
}

void FunctionLookup::insertFunction(FuncFactory *func)
{
  size_t secondaryKey = SECONDARY_KEY(func);

  // Use similar algorithm to lookup in order to detect overlaps
  // in argument numbers
  RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
  //
  // Walk the matches for the primary key (name) looking for overlaps:
  //   ensure func->max < min OR func->min > max
  //
  iterator.setPrimaryKey(func->getURINameHash());
  while(iterator.hasMoreElements())
    {
      FuncFactory *entry= &(iterator.nextElement());
      if ((func->getMaxArgs() < entry->getMinArgs()) ||
          (func->getMinArgs() > entry->getMaxArgs()))
        continue;
      // overlap -- throw exception
      XMLBuffer buf;
      buf.set(X("Multiple functions have the same expanded QName and number of arguments {"));
      buf.append(func->getURI());
      buf.append(X("}"));
      buf.append(func->getName());
      buf.append(X("/"));
      XMLCh szInt[10];
      XMLString::binToText((unsigned int)secondaryKey,szInt,9,10);
      buf.append(szInt);
      buf.append(X(" [err:XQST0034]."));
      XQThrow2(StaticErrorException,X("FunctionLookup::insertFunction"), buf.getRawBuffer());
    }
  // Ok to add function
  _funcTable.put((void*)func->getURINameHash(), (int)secondaryKey, func);
}

void FunctionLookup::removeFunction(FuncFactory *func)
{
  size_t secondaryKey = SECONDARY_KEY(func);
  _funcTable.removeKey((void*)func->getURINameHash(), (int)secondaryKey);
}

ASTNode* FunctionLookup::lookUpFunction(const XMLCh* URI, const XMLCh* fname,
                                        const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const
{
  if (this != g_globalFunctionTable) {
    ASTNode *ret = g_globalFunctionTable->lookUpFunction(
                                                         URI, fname, args, memMgr);
    if (ret)
      return ret;
  }

  RefHash2KeysTableOfEnumerator<FuncFactory> iterator(const_cast<RefHash2KeysTableOf< FuncFactory >* >(&_funcTable));
  //
  // Walk the matches for the primary key (name) looking for matches
  // based on allowable parameters
  //
  XMLBuffer key;
  key.set(fname);
  key.append(URI);
  iterator.setPrimaryKey(key.getRawBuffer());
  size_t nargs = args.size();
  while(iterator.hasMoreElements()) {
    FuncFactory *entry= &(iterator.nextElement());
    if (entry->getMinArgs() <= nargs &&
        entry->getMaxArgs() >= nargs)
      return entry->createInstance(args, memMgr);
  }
  return NULL;
}

//
// external functions are hashed on name+uri (primary) and numargs (secondary)
//
void FunctionLookup::insertExternalFunction(const ExternalFunction *func)
{
  size_t secondaryKey = func->getNumberOfArguments();
  _exFuncTable.put((void*)func->getURINameHash(), (int)secondaryKey, func);
}

const ExternalFunction *FunctionLookup::lookUpExternalFunction(
  const XMLCh* URI, const XMLCh* fname, size_t numArgs) const
{
  size_t secondaryKey = numArgs;
  XMLBuffer key;
  key.set(fname);
  key.append(URI);
  return _exFuncTable.get(key.getRawBuffer(), (int)secondaryKey);
}

void FunctionLookup::copyExternalFunctionsTo(DynamicContext *context) const
{
  RefHash2KeysTableOfEnumerator<const ExternalFunction> en(const_cast<RefHash2KeysTableOf<const ExternalFunction>*>(&_exFuncTable));
  while(en.hasMoreElements()) {
    context->addExternalFunction(&en.nextElement());
  }
}

/*
 * Global initialization and access
 */
static void initGlobalTable(FunctionLookup *t, MemoryManager *memMgr);

// static
void FunctionLookup::initialize()
{
  /* global table is allocated via the memory manager, so
     no need to delete it at this time
     if (g_globalFunctionTable)
     delete g_globalFunctionTable;
  */
  if (g_memMgr)
    delete g_memMgr;
  g_memMgr = new XPath2MemoryManagerImpl();
  g_globalFunctionTable = new (g_memMgr) FunctionLookup(g_memMgr);
  initGlobalTable(g_globalFunctionTable, g_memMgr);
}

// static
void FunctionLookup::terminate()
{
  if (g_memMgr) {
    delete g_memMgr;
    g_memMgr = 0;
    g_globalFunctionTable = 0;
  }
  /* no need to delete this -- memMgr cleans up
     if (g_globalFunctionTable) {
     delete g_globalFunctionTable;
     g_globalFunctionTable = 0;
     }
  */
}

// static
void FunctionLookup::insertGlobalFunction(FuncFactory *func)
{
  g_globalFunctionTable->insertFunction(func);
}

// static
void FunctionLookup::insertGlobalExternalFunction(const ExternalFunction *func)
{
  g_globalFunctionTable->insertExternalFunction(func);
}

// static
ASTNode* FunctionLookup::lookUpGlobalFunction(
                                              const XMLCh* URI, const XMLCh* fname,
                                              const VectorOfASTNodes &args,
                                              XPath2MemoryManager* memMgr,
                                              const FunctionLookup *contextTable)
{
  ASTNode *ast = g_globalFunctionTable->lookUpFunction(
                                                       URI, fname, args, memMgr);
  if (!ast && contextTable)
    ast = contextTable->lookUpFunction(
                                       URI, fname, args, memMgr);
  return ast;
}

// static
const ExternalFunction *FunctionLookup::lookUpGlobalExternalFunction(
  const XMLCh* URI, const XMLCh* fname, size_t numArgs,
  const FunctionLookup *contextTable)
{
  const ExternalFunction *ef =
    g_globalFunctionTable->lookUpExternalFunction(
      URI, fname, numArgs);
  if (!ef && contextTable)
    ef = contextTable->lookUpExternalFunction(
      URI, fname, numArgs);
  return ef;
}

#include "FuncFactoryTemplate.hpp"
#include <xqilla/functions/FunctionAbs.hpp>
#include <xqilla/functions/FunctionAvg.hpp>
#include <xqilla/functions/FunctionBaseURI.hpp>
#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/functions/FunctionCeiling.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionCompare.hpp>
#include <xqilla/functions/FunctionCodepointEqual.hpp>
#include <xqilla/functions/FunctionConcat.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/functions/FunctionCount.hpp>
#include <xqilla/functions/FunctionCurrentDate.hpp>
#include <xqilla/functions/FunctionCurrentDateTime.hpp>
#include <xqilla/functions/FunctionCurrentTime.hpp>
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/functions/FunctionDateTime.hpp>
#include <xqilla/functions/FunctionDeepEqual.hpp>
#include <xqilla/functions/FunctionDefaultCollation.hpp>
#include <xqilla/functions/FunctionDistinctValues.hpp>
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocument.hpp>
#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/functions/FunctionDocumentURI.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionEncodeForUri.hpp>
#include <xqilla/functions/FunctionEndsWith.hpp>
#include <xqilla/functions/FunctionError.hpp>
#include <xqilla/functions/FunctionEscapeHtmlUri.hpp>
#include <xqilla/functions/FunctionExactlyOne.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionQName.hpp>
#include <xqilla/functions/FunctionFalse.hpp>
#include <xqilla/functions/FunctionFloor.hpp>
#include <xqilla/functions/FunctionInScopePrefixes.hpp>
#include <xqilla/functions/FunctionPrefixFromQName.hpp>
#include <xqilla/functions/FunctionLocalNameFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIFromQName.hpp>
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>
#include <xqilla/functions/FunctionId.hpp>
#include <xqilla/functions/FunctionIdref.hpp>
#include <xqilla/functions/FunctionImplicitTimezone.hpp>
#include <xqilla/functions/FunctionIndexOf.hpp>
#include <xqilla/functions/FunctionInsertBefore.hpp>
#include <xqilla/functions/FunctionIriToUri.hpp>
#include <xqilla/functions/FunctionLang.hpp>
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/functions/FunctionLocalname.hpp>
#include <xqilla/functions/FunctionLowerCase.hpp>
#include <xqilla/functions/FunctionMatches.hpp>
#include <xqilla/functions/FunctionMax.hpp>
#include <xqilla/functions/FunctionMin.hpp>
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/functions/FunctionNamespaceUri.hpp>
#include <xqilla/functions/FunctionNilled.hpp>
#include <xqilla/functions/FunctionNodeName.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/functions/FunctionNormalizeUnicode.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/functions/FunctionOneOrMore.hpp>
#include <xqilla/functions/FunctionPosition.hpp>
#include <xqilla/functions/FunctionRemove.hpp>
#include <xqilla/functions/FunctionReplace.hpp>
#include <xqilla/functions/FunctionResolveQName.hpp>
#include <xqilla/functions/FunctionResolveURI.hpp>
#include <xqilla/functions/FunctionReverse.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionRound.hpp>
#include <xqilla/functions/FunctionRoundHalfToEven.hpp>
#include <xqilla/functions/FunctionStartsWith.hpp>
#include <xqilla/functions/FunctionStaticBaseURI.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionStringJoin.hpp>
#include <xqilla/functions/FunctionStringLength.hpp>
#include <xqilla/functions/FunctionStringToCodepoints.hpp>
#include <xqilla/functions/FunctionCodepointsToString.hpp>
#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/functions/FunctionSubstring.hpp>
#include <xqilla/functions/FunctionSubstringAfter.hpp>
#include <xqilla/functions/FunctionSubstringBefore.hpp>
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/functions/FunctionTokenize.hpp>
#include <xqilla/functions/FunctionTrace.hpp>
#include <xqilla/functions/FunctionTranslate.hpp>
#include <xqilla/functions/FunctionTrue.hpp>
#include <xqilla/functions/FunctionUnordered.hpp>
#include <xqilla/functions/FunctionUpperCase.hpp>
#include <xqilla/functions/FunctionZeroOrOne.hpp>
#include <xqilla/functions/FunctionYearsFromDuration.hpp>
#include <xqilla/functions/FunctionMonthsFromDuration.hpp>
#include <xqilla/functions/FunctionDaysFromDuration.hpp>
#include <xqilla/functions/FunctionHoursFromDuration.hpp>
#include <xqilla/functions/FunctionMinutesFromDuration.hpp>
#include <xqilla/functions/FunctionSecondsFromDuration.hpp>
#include <xqilla/functions/FunctionYearFromDateTime.hpp>
#include <xqilla/functions/FunctionMonthFromDateTime.hpp>
#include <xqilla/functions/FunctionDayFromDateTime.hpp>
#include <xqilla/functions/FunctionHoursFromDateTime.hpp>
#include <xqilla/functions/FunctionMinutesFromDateTime.hpp>
#include <xqilla/functions/FunctionSecondsFromDateTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromDateTime.hpp>
#include <xqilla/functions/FunctionYearFromDate.hpp>
#include <xqilla/functions/FunctionMonthFromDate.hpp>
#include <xqilla/functions/FunctionDayFromDate.hpp>
#include <xqilla/functions/FunctionTimezoneFromDate.hpp>
#include <xqilla/functions/FunctionHoursFromTime.hpp>
#include <xqilla/functions/FunctionMinutesFromTime.hpp>
#include <xqilla/functions/FunctionSecondsFromTime.hpp>
#include <xqilla/functions/FunctionTimezoneFromTime.hpp>
#include <xqilla/functions/FunctionAdjustDateTimeToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustDateToTimezone.hpp>
#include <xqilla/functions/FunctionAdjustTimeToTimezone.hpp>
// Updates
#include <xqilla/update/FunctionPut.hpp>
// XSLT 2.0 functions
#include <xqilla/functions/FunctionUnparsedText.hpp>
#include <xqilla/functions/FunctionRegexGroup.hpp>
// XQilla extension functions
#include <xqilla/functions/FunctionParseXML.hpp>
#include <xqilla/functions/FunctionParseJSON.hpp>
#include <xqilla/functions/FunctionSerializeJSON.hpp>
#include <xqilla/functions/FunctionParseHTML.hpp>
#include <xqilla/functions/FunctionTime.hpp>
#include <xqilla/functions/FunctionAnalyzeString.hpp>
// Higher Order Functions extension functions
#include <xqilla/functions/FunctionPartialApply.hpp>
#include <xqilla/functions/FunctionFunctionArity.hpp>
#include <xqilla/functions/FunctionFunctionName.hpp>

void FunctionLookup::insertUpdateFunctions(XPath2MemoryManager *memMgr)
{
  // Update functions
  //   fn:put
  insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPut>(memMgr));
}

static void initGlobalTable(FunctionLookup *t, MemoryManager *memMgr)
{
  // From the XPath2 Function & Operators list

  // Accessors:
  //   fn:node-name
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNodeName>(memMgr));
  //   fn:nilled
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNilled>(memMgr));
  //   fn:string
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionString>(memMgr));
  //   fn:data
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionData>(memMgr));
  //   fn:base-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionBaseURI>(memMgr));
  //   fn:document-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDocumentURI>(memMgr));


  // Debug Functions:
  //   fn:error
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionError>(memMgr));
  //   fn:trace
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTrace>(memMgr));

  // Special Constructor Functions:
  //   fn:dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDateTime>(memMgr));

  // Functions on numeric values:
  //   fn:abs
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAbs>(memMgr));
  //   fn:ceiling
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCeiling>(memMgr));
  //   fn:floor
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFloor>(memMgr));
  //   fn:round
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRound>(memMgr));
  //   fn:round-half-to-even
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRoundHalfToEven>(memMgr));

  // Functions on strings
  //   fn:codepoints-to-string
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCodepointsToString>(memMgr));
  //   fn:string-to-codepoints
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringToCodepoints>(memMgr));
  //   fn:compare
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCompare>(memMgr));
  //   fn:codepoint-equal
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCodepointEqual>(memMgr));
  //   fn:concat
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionConcat>(memMgr));
  //   fn:string-join
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringJoin>(memMgr));
  //   fn:substring
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstring>(memMgr));
  //   fn:string-length
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStringLength>(memMgr));
  //   fn:normalize-space
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNormalizeSpace>(memMgr));
  //   fn:normalize-unicode
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNormalizeUnicode>(memMgr));
  //   fn:upper-case
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionUpperCase>(memMgr));
  //   fn:lower-case
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLowerCase>(memMgr));
  //   fn:translate
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTranslate>(memMgr));
  //   fn:encode-for-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEncodeForUri>(memMgr));
  //   fn:iri-to-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIriToUri>(memMgr));
  //   fn:escape-html-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEscapeHtmlUri>(memMgr));
  //   fn:contains
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionContains>(memMgr));
  //   fn:starts-with
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStartsWith>(memMgr));
  //   fn:ends-with
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEndsWith>(memMgr));
  //   fn:substring-before
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstringBefore>(memMgr));
  //   fn:substring-after
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubstringAfter>(memMgr));

  //   fn:matches
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMatches>(memMgr));
  //   fn:replace
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionReplace>(memMgr));
  //   fn:tokenize
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTokenize>(memMgr));

  // Functions on boolean values
  //   fn:true
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTrue>(memMgr));
  //   fn:false
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFalse>(memMgr));
  //   fn:not
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNot>(memMgr));

  // Functions on date values
  //   fn:years-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearsFromDuration>(memMgr));
  //   fn:months-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthsFromDuration>(memMgr));
  //   fn:days-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDaysFromDuration>(memMgr));
  //   fn:hours-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromDuration>(memMgr));
  //   fn:minutes-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromDuration>(memMgr));
  //   fn:seconds-from-duration
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromDuration>(memMgr));
  //   fn:year-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearFromDateTime>(memMgr));
  //   fn:month-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthFromDateTime>(memMgr));
  //   fn:day-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDayFromDateTime>(memMgr));
  //   fn:hours-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromDateTime>(memMgr));
  //   fn:minutes-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromDateTime>(memMgr));
  //   fn:seconds-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromDateTime>(memMgr));
  //   fn:timezone-from-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromDateTime>(memMgr));
  //   fn:year-from-date
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionYearFromDate>(memMgr));
  //   fn:month-from-date
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMonthFromDate>(memMgr));
  //   fn:day-from-date
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDayFromDate>(memMgr));
  //   fn:timezone-from-date
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromDate>(memMgr));
  //   fn:hours-from-time
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionHoursFromTime>(memMgr));
  //   fn:minutes-from-time
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMinutesFromTime>(memMgr));
  //   fn:seconds-from-time
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSecondsFromTime>(memMgr));
  //   fn:timezone-from-time
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTimezoneFromTime>(memMgr));
  //   fn:adjust-dateTime-to-timezone
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustDateTimeToTimezone>(memMgr));
  //   fn:adjust-date-to-timezone
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustDateToTimezone>(memMgr));
  //   fn:adjust-time-to-timezone
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAdjustTimeToTimezone>(memMgr));

  // Functions on QName values
  //   fn:resolve-QName
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionResolveQName>(memMgr));
  //   fn:QName
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionQName>(memMgr));
  //   fn:prefix-from-QName
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPrefixFromQName>(memMgr));
  //   fn:local-name-from-QName
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLocalNameFromQName>(memMgr));
  //   fn:namespace-uri-from-QName
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceURIFromQName>(memMgr));
  //   fn:namespace-uri-for-prefix
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceURIForPrefix>(memMgr));
  //   fn:in-scope-prefixes
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionInScopePrefixes>(memMgr));

  // Functions on anyURI values
  //   fn:resolve-URI
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionResolveURI>(memMgr));

  // Functions on nodes
  //   fn:name
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionName>(memMgr));
  //   fn:local-name
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLocalname>(memMgr));
  //   fn:namespace-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNamespaceUri>(memMgr));
  //   fn:number
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionNumber>(memMgr));
  //   fn:lang
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLang>(memMgr));
  //   fn:root
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRoot>(memMgr));

  // Functions on sequences
  //   fn::zero-or-one
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionZeroOrOne>(memMgr));
  //   fn::one-or-more
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionOneOrMore>(memMgr));
  //   fn::exactly-one
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionExactlyOne>(memMgr));
  //   fn:boolean
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionBoolean>(memMgr));
  //   fn:index-of
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIndexOf>(memMgr));
  //   fn:empty
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionEmpty>(memMgr));
  //   fn:exists
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionExists>(memMgr));
  //   fn:distinct-values
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDistinctValues>(memMgr));
  //   fn:insert-before
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionInsertBefore>(memMgr));
  //   fn:remove
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRemove>(memMgr));
  //   fn:reverse
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionReverse>(memMgr));
  //   fn:subsequence
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSubsequence>(memMgr));
  //   fn:unordered
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionUnordered>(memMgr));
  
  //   fn:deep-equal
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDeepEqual>(memMgr));

  //   fn:count
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCount>(memMgr));
  //   fn:avg
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAvg>(memMgr));
  //   fn:max
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMax>(memMgr));
  //   fn:min
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionMin>(memMgr));
  //   fn:sum
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSum>(memMgr));

  //   fn:id
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionId>(memMgr));
  //   fn:idref
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionIdref>(memMgr));
  //   fn:doc
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDoc>(memMgr));
  //   fn:doc-available
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDocAvailable>(memMgr));
  //   fn:collection
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCollection>(memMgr));
  
  // Context functions
  //   fn:position
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPosition>(memMgr));
  //   fn:last
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionLast>(memMgr));
  //   fn:current-dateTime
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentDateTime>(memMgr));
  //   fn:current-date
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentDate>(memMgr));
  //   fn:current-time
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionCurrentTime>(memMgr));
  //   fn:implicit-timezone
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionImplicitTimezone>(memMgr));
  //   fn:default-collation
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDefaultCollation>(memMgr));
  //   fn:static-base-uri
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionStaticBaseURI>(memMgr));

  // XSLT 2.0 functions
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionUnparsedText>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionRegexGroup>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionDocument>(memMgr));

  // XQilla extension functions
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionParseXML>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionParseJSON>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionSerializeJSON>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionTime>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionAnalyzeString>(memMgr));

#ifdef HAVE_LIBTIDY
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionParseHTML>(memMgr));
#endif

  // Higher Order Functions extension functions
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionPartialApply>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFunctionArity>(memMgr));
  t->insertFunction(new (memMgr) FuncFactoryTemplate<FunctionFunctionName>(memMgr));
}
