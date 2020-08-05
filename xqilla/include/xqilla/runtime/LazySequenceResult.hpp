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
 * $Id: LazySequenceResult.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _LAZYSEQUENCERESULT_HPP
#define _LAZYSEQUENCERESULT_HPP

#include <xqilla/runtime/ResultImpl.hpp>

#include <xqilla/runtime/Sequence.hpp>

class XQILLA_API LazySequenceResult : public ResultImpl
{
public:
  LazySequenceResult(const LocationInfo *o, DynamicContext *context);

  /// Get the next item from the iterator
  virtual Item::Ptr next(DynamicContext *context);

  /// Called lazily to get the Sequence to iterate over
  virtual void getResult(Sequence &toFill, DynamicContext *context) const = 0;

  /// Cast operator to a Sequence
  virtual Sequence toSequence(DynamicContext *context = 0);

private:
  bool toInit_;
  Sequence::iterator it_;
  Sequence seq_;
};

#endif
