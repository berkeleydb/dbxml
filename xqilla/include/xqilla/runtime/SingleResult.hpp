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
 * $Id: SingleResult.hpp 593 2008-08-14 21:21:17Z jpcs $
 */

#ifndef _SINGLERESULT_HPP
#define _SINGLERESULT_HPP

#include <xqilla/runtime/ResultImpl.hpp>

/** A reference counting wrapper for the lazily evaluated query result */
class XQILLA_API SingleResult : public ResultImpl
{
public:
  SingleResult(const LocationInfo *o);

  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context);

  virtual Item::Ptr getSingleResult(DynamicContext *context) const = 0;
};

#endif
