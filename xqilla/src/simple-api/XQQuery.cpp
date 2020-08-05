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
 * $Id: XQQuery.cpp 797 2009-09-24 15:00:23Z jpcs $
 */

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/events/EventHandler.hpp>
#include <xqilla/optimizer/StaticTyper.hpp>
#include <xqilla/functions/XQillaFunction.hpp>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLResourceIdentifier.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

#include <sstream>
#include <list>

using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQQuery::XQQuery(const XMLCh* queryText, DynamicContext *context, bool contextOwned, MemoryManager *memMgr)
  : m_memMgr(memMgr),
    m_context(context),
    m_contextOwned(contextOwned),
    m_query(NULL),
    m_bIsLibraryModule(false),
    m_szTargetNamespace(NULL),
    m_szQueryText(m_context->getMemoryManager()->getPooledString(queryText)),
    m_szCurrentFile(NULL),
    m_userDefFns(XQillaAllocator<XQUserFunction*>(memMgr)),
    m_userDefVars(XQillaAllocator<XQGlobalVariable*>(memMgr)),
    m_importedModules(XQillaAllocator<XQQuery*>(memMgr))
{
}

XQQuery::~XQQuery()
{
  for(ImportedModules::iterator it = m_importedModules.begin();
      it != m_importedModules.end(); ++it) {
    delete *it;
  }
  if(m_contextOwned)
    delete m_context;
}

DynamicContext *XQQuery::createDynamicContext(MemoryManager *memMgr) const
{
  return m_context->createDynamicContext(memMgr);
}

Result XQQuery::execute(DynamicContext* context) const
{
  return new QueryResult(this);
}

Result XQQuery::execute(const Item::Ptr &contextItem, DynamicContext *context) const
{
  context->setContextItem(contextItem);
  return execute(context);
}

Result XQQuery::execute(const XMLCh *templateQName, DynamicContext *context) const
{
  Item::Ptr value = context->getItemFactory()->createUntypedAtomic(templateQName, context);
  context->setExternalVariable(XQillaFunction::XMLChFunctionURI, X("name"), value);
  return execute(context);
}

void XQQuery::executeProlog(DynamicContext *context) const
{
  try {
    // Execute the imported modules
    for(ImportedModules::const_iterator modIt = m_importedModules.begin();
        modIt != m_importedModules.end(); ++modIt) {

      // Derive the module's execution context from it's static context
      AutoDelete<DynamicContext> moduleCtx(context->createModuleDynamicContext((*modIt)->getStaticContext(),
                                                                               context->getMemoryManager()));
      (*modIt)->executeProlog(moduleCtx);

      // Copy the module's imported variables into our context
      for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
          varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
        Result value = moduleCtx->getGlobalVariableStore()->
          getVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName());
        context->setExternalVariable((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(),
                                     value->toSequence(context));
      }
    }

    // define global variables
    for(GlobalVariables::const_iterator it3 = m_userDefVars.begin(); it3 != m_userDefVars.end(); ++it3)
      (*it3)->execute(context);
  }
  catch(XQException& e) {
    if(e.getXQueryLine() == 0) {
      e.setXQueryPosition(m_szCurrentFile, 1, 1);
    }
    throw e;
  }
}

void XQQuery::execute(EventHandler *events, DynamicContext* context) const
{
  executeProlog(context);

  if(m_query != NULL) {
    try {
      // execute the query body
      m_query->generateAndTailCall(events, context, true, true);
      events->endEvent();
    }
    catch(XQException& e) {
      if(e.getXQueryLine() == 0) {
        e.setXQueryPosition(m_szCurrentFile, 1, 1);
      }
      throw e;
    }
  }
}

void XQQuery::execute(EventHandler *events, const Item::Ptr &contextItem, DynamicContext *context) const
{
  context->setContextItem(contextItem);
  execute(events, context);
}

void XQQuery::execute(EventHandler *events, const XMLCh *templateQName, DynamicContext *context) const
{
  Item::Ptr value = context->getItemFactory()->createUntypedAtomic(templateQName, context);
  context->setExternalVariable(XQillaFunction::XMLChFunctionURI, X("name"), value);
  execute(events, context);
}

void XQQuery::staticResolution()
{
  StaticContext *context = m_context;

  // Run staticResolutionStage1 on the user defined functions,
  // which gives them the static type they were defined with
  UserFunctions::iterator i;
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
    (*i)->staticResolutionStage1(context);

    if(getIsLibraryModule() && !(*i)->isTemplate() && !XERCES_CPP_NAMESPACE::XMLString::equals((*i)->getURI(), getModuleTargetNamespace()))
      XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), X("Every function in a module must be in the module namespace [err:XQST0048]."), *i);

    if((*i)->isTemplate()) {
      context->addTemplate(*i);
    }
    else if((*i)->getName()) {
      context->addCustomFunction(*i);
    }
  }

  // Define types for the imported variables
  for(ImportedModules::const_iterator modIt = m_importedModules.begin();
      modIt != m_importedModules.end(); ++modIt) {
    for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
        varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
      context->getVariableTypeStore()->
        declareGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(),
                         (*varIt)->getStaticAnalysis());
    }
  }

  // Run staticResolution on the global variables
  if(!m_userDefVars.empty()) {
    GlobalVariables::iterator itVar, itVar2;
    // declare all the global variables with a special StaticAnalysis, in order to recognize 'variable is defined later' errors 
    // instead of more generic 'variable not found'
    // In order to catch references to not yet defined variables (but when no recursion happens) we also create a scope where we undefine
    // the rest of the variables (once we enter in a function call, the scope will disappear and the forward references to the global variables 
    // will happear)
    StaticAnalysis forwardRef(context->getMemoryManager());
    forwardRef.setProperties(StaticAnalysis::FORWARDREF);
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      const XMLCh* varName=(*itVar)->getVariableName();
      const XMLCh* prefix=XPath2NSUtils::getPrefix(varName, context->getMemoryManager());
      const XMLCh* uri=NULL;
      if(prefix && *prefix)
        uri = context->getUriBoundToPrefix(prefix, *itVar);
      const XMLCh* name= XPath2NSUtils::getLocalName(varName);
      context->getVariableTypeStore()->declareGlobalVar(uri, name, forwardRef);
    }
    StaticAnalysis forwardRef2(context->getMemoryManager());
    forwardRef2.setProperties(StaticAnalysis::UNDEFINEDVAR);
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      context->getVariableTypeStore()->addLogicalBlockScope();
      for(itVar2 = itVar; itVar2 != m_userDefVars.end(); ++itVar2) {
          const XMLCh* varName=(*itVar2)->getVariableName();
          const XMLCh* prefix=XPath2NSUtils::getPrefix(varName, context->getMemoryManager());
          const XMLCh* uri=NULL;
          if(prefix && *prefix)
            uri = context->getUriBoundToPrefix(prefix, *itVar2);
          const XMLCh* name= XPath2NSUtils::getLocalName(varName);
          context->getVariableTypeStore()->declareVar(uri,name,forwardRef2);
      }
      (*itVar)->staticResolution(context);
      context->getVariableTypeStore()->removeScope();
      if(getIsLibraryModule() && !XERCES_CPP_NAMESPACE::XMLString::equals((*itVar)->getVariableURI(),
		 getModuleTargetNamespace()))
        XQThrow3(StaticErrorException,X("XQQuery::staticResolution"),
		X("Every global variable in a module must be in the module namespace [err:XQST0048]."), *itVar);
    }
    // check for duplicate variable declarations
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) 
    {
      for (GlobalVariables::iterator it2 = itVar+1; it2 != m_userDefVars.end(); ++it2) 
      {
        if(XPath2Utils::equals((*itVar)->getVariableURI(), (*it2)->getVariableURI()) &&
           XPath2Utils::equals((*itVar)->getVariableLocalName(), (*it2)->getVariableLocalName()))
        {
          XMLBuffer errMsg(1023, context->getMemoryManager());
          errMsg.set(X("A variable with name {"));
            errMsg.append((*itVar)->getVariableURI());
          errMsg.append(X("}"));
            errMsg.append((*itVar)->getVariableLocalName());
          errMsg.append(X(" has already been declared [err:XQST0049]"));
          XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), errMsg.getRawBuffer(), *itVar);
        }
      }
      for(ImportedModules::const_iterator modIt = m_importedModules.begin();
          modIt != m_importedModules.end(); ++modIt) {
        for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
            varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
          if(XPath2Utils::equals((*itVar)->getVariableURI(), (*varIt)->getVariableURI()) &&
             XPath2Utils::equals((*itVar)->getVariableLocalName(), (*varIt)->getVariableLocalName())) {
            XMLBuffer errMsg(1023, context->getMemoryManager());
            errMsg.set(X("A variable with name {"));
            errMsg.append((*itVar)->getVariableURI());
            errMsg.append(X("}"));
            errMsg.append((*itVar)->getVariableLocalName());
            errMsg.append(X(" has already been imported from a module [err:XQST0049]"));
            XQThrow3(StaticErrorException,X("XQQuery::staticResolution"), errMsg.getRawBuffer(), *varIt);
          }
        }
      }
    }
  }

  // Run staticResolutionStage2 on the user defined functions,
  // which statically resolves their function bodies
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
    (*i)->staticResolutionStage2(context);
  }

  // Run static resolution on the query body
  if(m_query) m_query = m_query->staticResolution(context);
}

void XQQuery::staticTyping(StaticTyper *styper)
{
  StaticContext *context = m_context;

  StaticTyper defaultTyper;
  if(styper == 0) styper = &defaultTyper;

  VariableTypeStore* varStore = context->getVariableTypeStore();

  // Static type the imported modules (again)
  ImportedModules::const_iterator modIt;
  for(modIt = m_importedModules.begin(); modIt != m_importedModules.end(); ++modIt) {
    (*modIt)->staticTyping(styper);
  }  

  // Define types for the imported variables
  for(modIt = m_importedModules.begin(); modIt != m_importedModules.end(); ++modIt) {
    for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
        varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
      varStore->declareGlobalVar((*varIt)->getVariableURI(), (*varIt)->getVariableLocalName(),
                                 (*varIt)->getStaticAnalysis());
    }
  }

  // Run staticTyping on the global variables
  if(!m_userDefVars.empty()) {
    // declare all the global variables with a special StaticAnalysis, in order to recognize 'variable is defined
    // later' errors instead of more generic 'variable not found'. In order to catch references to not yet defined
    // variables (but when no recursion happens) we also create a scope where we undefine the rest of the variables (once
    // we enter in a function call, the scope will disappear and the forward references to the global variables will
    // appear).
    StaticAnalysis forwardRef(context->getMemoryManager());
    forwardRef.setProperties(StaticAnalysis::FORWARDREF);

    StaticAnalysis undefinedVar(context->getMemoryManager());
    undefinedVar.setProperties(StaticAnalysis::UNDEFINEDVAR);

    GlobalVariables::iterator itVar, itVar2;
    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      varStore->declareGlobalVar((*itVar)->getVariableURI(), (*itVar)->getVariableLocalName(),
                                 forwardRef);
    }

    for(itVar = m_userDefVars.begin(); itVar != m_userDefVars.end(); ++itVar) {
      varStore->addLogicalBlockScope();
      for(itVar2 = itVar; itVar2 != m_userDefVars.end(); ++itVar2) {
        varStore->declareVar((*itVar2)->getVariableURI(), (*itVar2)->getVariableLocalName(),
                             undefinedVar);
      }
      (*itVar)->staticTyping(context, styper);
      varStore->removeScope();
    }
  }

  // Run staticTyping on the user defined functions,
  // which calculates a better type for them
  UserFunctions::iterator i, j;
  for(i = m_userDefFns.begin(); i != m_userDefFns.end(); ++i) {
    for(j = m_userDefFns.begin(); j != m_userDefFns.end(); ++j) {
      (*j)->resetStaticTypingOnce();
    }

    (*i)->staticTypingOnce(context, styper);
  }

  // Run staticTyping on the query body
  if(m_query) m_query = m_query->staticTyping(context, styper);
}

std::string XQQuery::getQueryPlan() const
{
  return PrintAST::print(this, m_context);
}

ASTNode* XQQuery::getQueryBody() const
{
  return m_query;
}

void XQQuery::setQueryBody(ASTNode* query)
{
  m_query=query;
}

void XQQuery::addFunction(XQUserFunction* fnDef)
{
  m_userDefFns.push_back(fnDef);
}

void XQQuery::addVariable(XQGlobalVariable* varDef)
{
  m_userDefVars.push_back(varDef);
}

void XQQuery::setIsLibraryModule(bool bIsModule/*=true*/)
{
  m_bIsLibraryModule=bIsModule;
}

bool XQQuery::getIsLibraryModule() const
{
  return m_bIsLibraryModule;
}

void XQQuery::setModuleTargetNamespace(const XMLCh* uri)
{
  m_szTargetNamespace=uri;
}

const XMLCh* XQQuery::getModuleTargetNamespace() const
{
  return m_szTargetNamespace;
}

class LoopDetector : public XMLEntityResolver
{
public:
  LoopDetector(XMLEntityResolver* pParent, const XMLCh* myModuleURI, const LocationInfo *location) 
  {
    m_pParentResolver = pParent;
    m_PreviousModuleNamespace = myModuleURI;
    m_location = location;
  }

  virtual InputSource *resolveEntity(XMLResourceIdentifier* resourceIdentifier)
  {
    if(resourceIdentifier->getResourceIdentifierType() == XMLResourceIdentifier::UnKnown &&
       XPath2Utils::equals(resourceIdentifier->getNameSpace(), m_PreviousModuleNamespace)) {
      XMLBuffer buf;
      buf.set(X("The graph of module imports contains a cycle for namespace '"));
      buf.append(resourceIdentifier->getNameSpace());
      buf.append(X("' [err:XQST0073]"));
      XQThrow3(StaticErrorException, X("LoopDetector::resolveEntity"), buf.getRawBuffer(), m_location);
    }
    if(m_pParentResolver)
      return m_pParentResolver->resolveEntity(resourceIdentifier);
    return NULL;
  }

protected:
  XMLEntityResolver* m_pParentResolver;
  const XMLCh* m_PreviousModuleNamespace;
  const LocationInfo *m_location;
};

void XQQuery::importModule(const XMLCh* szUri, VectorOfStrings* locations, StaticContext* context, const LocationInfo *location)
{
  for(ImportedModules::iterator modIt = m_importedModules.begin();
      modIt != m_importedModules.end(); ++modIt) {
    if(XPath2Utils::equals((*modIt)->getModuleTargetNamespace(),szUri)) {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("Module for namespace '"));
      buf.append(szUri);
      buf.append(X("' has already been imported [err:XQST0047]"));
      XQThrow3(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer(), location);
    }
  }
  if(locations==NULL)
    locations=context->resolveModuleURI(szUri);
  if(locations==NULL || locations->empty()) {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Cannot locate module for namespace "));
    buf.append(szUri);
    buf.append(X(" without the 'at <location>' keyword [err:XQST0059]"));
    XQThrow3(StaticErrorException,X("XQQuery::ImportModule"), buf.getRawBuffer(), location);
  }

  bool bFound=false;
  for(VectorOfStrings::iterator it=locations->begin();it!=locations->end();++it) {
    InputSource* srcToUse = 0;
    if (context->getDocumentCache()->getXMLEntityResolver()){
      XMLResourceIdentifier resourceIdentifier(XMLResourceIdentifier::UnKnown,
                                               *it, szUri, XMLUni::fgZeroLenString, 
                                               context->getBaseURI());
      srcToUse = context->getDocumentCache()->getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
    }

    if(srcToUse==0) {
      try {
        XMLURL urlTmp(context->getBaseURI(), *it);
        if (urlTmp.isRelative()) {
          throw MalformedURLException(__FILE__, __LINE__, XMLExcepts::URL_NoProtocolPresent);
        }
        srcToUse = new URLInputSource(urlTmp);
      }
      catch(const MalformedURLException&) {
        // It's not a URL, so let's assume it's a local file name.
        const XMLCh* baseUri=context->getBaseURI();
        if(baseUri && baseUri[0]) {
          XMLCh* tmpBuf = XMLPlatformUtils::weavePaths(baseUri, *it);
          srcToUse = new LocalFileInputSource(tmpBuf);
          XMLPlatformUtils::fgMemoryManager->deallocate(tmpBuf);
        }
        else {
          srcToUse = new LocalFileInputSource(*it);
        }
      }
    }
    Janitor<InputSource> janIS(srcToUse);

    AutoDelete<DynamicContext> ctxGuard(context->createModuleContext());
    DynamicContext *moduleCtx = ctxGuard.get();

    moduleCtx->setBaseURI(srcToUse->getSystemId());
    LoopDetector loopDetector(context->getXMLEntityResolver(), szUri, location);
    moduleCtx->setXMLEntityResolver(&loopDetector);

    AutoDelete<XQQuery> pParsedQuery(XQilla::parse(*srcToUse, ctxGuard.adopt(), XQilla::NO_OPTIMIZATION));

    if(!pParsedQuery->getIsLibraryModule()) {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("The module at "));
      buf.append(srcToUse->getSystemId());
      buf.append(X(" is not a module"));
      XQThrow3(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer(), location);
    }
    if(!XERCES_CPP_NAMESPACE::XMLString::equals(szUri,pParsedQuery->getModuleTargetNamespace())) {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("The module at "));
      buf.append(srcToUse->getSystemId());
      buf.append(X(" specifies a different namespace [err:XQST0059]"));
      XQThrow3(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer(), location);
    }
    // now move the variable declarations and the function definitions into my context
    for(UserFunctions::iterator itFn = pParsedQuery->m_userDefFns.begin(); itFn != pParsedQuery->m_userDefFns.end(); ++itFn) {
      (*itFn)->setModuleDocumentCache(const_cast<DocumentCache*>(moduleCtx->getDocumentCache()));
      if((*itFn)->isTemplate()) {
        context->addTemplate(*itFn);
      }
      else if((*itFn)->getName()) {
        context->addCustomFunction(*itFn);
      }
    }
    for(GlobalVariables::iterator itVar = pParsedQuery->m_userDefVars.begin(); itVar != pParsedQuery->m_userDefVars.end(); ++itVar) {
      for(ImportedModules::const_iterator modIt = m_importedModules.begin();
          modIt != m_importedModules.end(); ++modIt) {
        for(GlobalVariables::const_iterator varIt = (*modIt)->m_userDefVars.begin();
            varIt != (*modIt)->m_userDefVars.end(); ++varIt) {
          if(XPath2Utils::equals((*varIt)->getVariableURI(), (*itVar)->getVariableURI()) &&
             XPath2Utils::equals((*varIt)->getVariableLocalName(), (*itVar)->getVariableLocalName())) {
            XMLBuffer buf(1023,context->getMemoryManager());
            buf.set(X("An imported variable {"));
            buf.append((*itVar)->getVariableURI());
            buf.append(X("}"));
            buf.append((*itVar)->getVariableLocalName());
            buf.append(X(" conflicts with an already defined global variable [err:XQST0049]."));
            XQThrow3(StaticErrorException, X("XQQuery::ImportModule"), buf.getRawBuffer(), *varIt);
          }
        }
      }
    }

    moduleCtx->setXMLEntityResolver(context->getXMLEntityResolver());
    m_importedModules.push_back(pParsedQuery.adopt());

    bFound=true;
  }
  if(!bFound)
  {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Cannot locate the module for namespace \""));
    buf.append(szUri);
    buf.append(X("\" [err:XQST0059]"));
    XQThrow3(StaticErrorException,X("XQQuery::ImportModule"), buf.getRawBuffer(), location);
  }
}

const XMLCh* XQQuery::getFile() const
{
	return m_szCurrentFile;
}

void XQQuery::setFile(const XMLCh* file)
{
	m_szCurrentFile=m_context->getMemoryManager()->getPooledString(file);
}

const XMLCh* XQQuery::getQueryText() const
{
    return m_szQueryText;
}

XQQuery::QueryResult::QueryResult(const XQQuery *query)
  : ResultImpl(query->getQueryBody()),
    _query(query)
{
}

Item::Ptr XQQuery::QueryResult::nextOrTail(Result &tail, DynamicContext *context)
{
  _query->executeProlog(context);

  if(_query->getQueryBody() != NULL) {
    // No closure needed here
    tail = _query->getQueryBody()->createResult(context);
  }
  else {
    tail = 0;
  }
  return 0;
}

