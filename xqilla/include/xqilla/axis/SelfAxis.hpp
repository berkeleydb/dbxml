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
 * $Id: SelfAxis.hpp 548 2008-06-09 19:55:12Z jpcs $
 */

#ifndef _SELFAXIS_HPP
#define _SELFAXIS_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/items/Node.hpp>

class XQILLA_API SelfAxis : public ResultImpl
{
public:
  SelfAxis(const LocationInfo *info, Node::Ptr contextNode);
  Item::Ptr next(DynamicContext *context);
private:
  bool toDo_;
  Node::Ptr contextNode_;
};

#endif

