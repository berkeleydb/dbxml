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
 * $Id: VarTypeStoreImpl.cpp 475 2008-01-08 18:47:44Z jpcs $
 */

#include "../../config/xqilla_config.h"
#include <assert.h>
#include "VarTypeStoreImpl.hpp"
#include <xqilla/context/impl/VarHashEntryImpl.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

VarTypeStoreImpl::VarTypeStoreImpl(XPath2MemoryManager* memMgr)
  : _store(memMgr)
{
}

VarTypeStoreImpl::~VarTypeStoreImpl()
{
  /* nothing to do */
}

void VarTypeStoreImpl::clear()
{
  _store.clear();
}

void VarTypeStoreImpl::addLocalScope()
{
  _store.addScope(Scope<const StaticAnalysis*>::LOCAL_SCOPE);
}

void VarTypeStoreImpl::addLogicalBlockScope()
{
  _store.addScope(Scope<const StaticAnalysis*>::LOGICAL_BLOCK_SCOPE);
}

void VarTypeStoreImpl::removeScope()
{
  _store.removeScope();
}

void VarTypeStoreImpl::declareGlobalVar(const XMLCh* namespaceURI,
                                        const XMLCh* name,
                                        const StaticAnalysis &src)
{
  _store.setGlobalVar(namespaceURI, name, &src);
}

void VarTypeStoreImpl::declareVar(const XMLCh* namespaceURI,
                                  const XMLCh* name,
                                  const StaticAnalysis &src)
{
  _store.declareVar(namespaceURI, name, &src);
}

const StaticAnalysis *VarTypeStoreImpl::getVar(const XMLCh* namespaceURI,
                                                        const XMLCh* name) const
{
  VarHashEntry<const StaticAnalysis*>* result = _store.getVar(namespaceURI, name);
  if(result)
    return result->getValue();
  return 0;
}

const StaticAnalysis* VarTypeStoreImpl::getGlobalVar(const XMLCh* namespaceURI,
                                                              const XMLCh* name) const
{
  VarHashEntry<const StaticAnalysis*>* result = _store.getGlobalVar(namespaceURI, name);
  if(result)
    return result->getValue();
  return 0;
}
