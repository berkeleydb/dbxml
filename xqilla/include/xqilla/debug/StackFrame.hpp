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
 * $Id: MessageListener.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _STACKFRAME_HPP
#define _STACKFRAME_HPP

#include <string>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Item.hpp>

#include <xercesc/util/XercesDefs.hpp>

class ASTNode;
class TupleNode;
class LocationInfo;
class DynamicContext;
class EventHandler;
class Sequence;
class VariableStore;

XERCES_CPP_NAMESPACE_BEGIN
class DOMXPathNSResolver;
XERCES_CPP_NAMESPACE_END

/** 
 * A class that represents an item in a query call stack
 */
class XQILLA_API StackFrame
{
public:
  virtual ~StackFrame() {}

  const LocationInfo *getLocationInfo() const { return location_; }
  const StackFrame *getPreviousFrame() const { return prev_; }

  virtual Sequence query(const XMLCh *query) const;
  virtual void query(const XMLCh *query, EventHandler *events) const;

  virtual const ASTNode *getASTNode() const = 0;
  virtual const TupleNode *getTupleNode() const = 0;
  virtual std::string getQueryPlan() const = 0;

protected:
  StackFrame(const LocationInfo *location, DynamicContext *context);

  const LocationInfo *location_;

  DynamicContext *context_;
  Item::Ptr contextItem_;
  size_t contextPosition_;
  size_t contextSize_;
  const VariableStore *variables_;
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsResolver_;
  const XMLCh *defaultElementNS_;

  const StackFrame *prev_;

private:
  StackFrame(const StackFrame &);
  StackFrame &operator=(const StackFrame&);
};

#endif
