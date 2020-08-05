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
 * $Id: ResultBufferImpl.hpp 531 2008-04-10 23:23:07Z jpcs $
 */

#ifndef _RESULTBUFFERIMPL_HPP
#define _RESULTBUFFERIMPL_HPP

#include <vector>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>

/** A buffer for lazily evaluated query results. This object is not thread safe */
class XQILLA_API ResultBufferImpl : public ReferenceCounted
{
public:
  typedef RefCountPointer<ResultBufferImpl> Ptr;

  static const unsigned int UNLIMITED_COUNT;

  /**
   * Do not allocate this object from a memory manager!
   * It must be created using the heap.
   */
  ResultBufferImpl(const Result &result, unsigned int readCount = UNLIMITED_COUNT);
  ResultBufferImpl(const Item::Ptr &item, unsigned int readCount = UNLIMITED_COUNT);
  ~ResultBufferImpl();

  /// Creates a result that returns the same items as the one used to construct this buffer
  Result createResult();

  unsigned int getMaxReadCount() const { return _maxReadCount; }
  void increaseMaxReadCount(unsigned int readCount);
  unsigned int incrementReadCount() { return ++_readCount; }

private:
  ResultBufferImpl(const ResultBufferImpl &);
  ResultBufferImpl &operator=(const ResultBufferImpl &);

  Item::Ptr item(unsigned int index, DynamicContext *context);

  friend class BufferedResult;

  typedef std::vector<Item::Ptr> ItemList;

  unsigned int _refCount;
  Result _result;
  ItemList _items;

  unsigned int _readCount;
  unsigned int _maxReadCount;
};

class BufferedResult : public ResultImpl
{
public:
  BufferedResult(ResultBufferImpl *impl);
  Item::Ptr next(DynamicContext *context);
  ResultBufferImpl *toResultBuffer(unsigned int readCount);
  std::string asString(DynamicContext *context, int indent) const;
private:
  ResultBufferImpl::Ptr _impl;
  unsigned int _pos;
};


#endif
