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
 * $Id: SequenceResult.cpp 548 2008-06-09 19:55:12Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>
#include <xqilla/runtime/SequenceResult.hpp>

SequenceResult::SequenceResult(const LocationInfo *o, const Sequence &seq)
  : ResultImpl(o),
    seq_(seq)
{
  it_ = seq_.begin();
}

Item::Ptr SequenceResult::next(DynamicContext *context)
{
  if(it_ == seq_.end()) {
    return 0;
  }
  else {
    return *(it_++);
  }
}

Sequence SequenceResult::toSequence(DynamicContext *context)
{
  return seq_;
}

