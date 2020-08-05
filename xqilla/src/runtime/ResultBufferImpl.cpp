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
 * $Id: ResultBufferImpl.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/runtime/ResultBufferImpl.hpp>

const unsigned int ResultBufferImpl::UNLIMITED_COUNT = (unsigned int)-1;

ResultBufferImpl::ResultBufferImpl(const Result &result, unsigned int readCount)
  : _refCount(0),
    _result(result),
    _readCount(0),
    _maxReadCount(readCount)
{
  // Do nothing
}

ResultBufferImpl::ResultBufferImpl(const Item::Ptr &item, unsigned int readCount)
  : _refCount(0),
    _result(0),
    _readCount(0),
    _maxReadCount(readCount)
{
  _items.push_back(item);
}

ResultBufferImpl::~ResultBufferImpl()
{
  // Do nothing
}

void ResultBufferImpl::increaseMaxReadCount(unsigned int readCount)
{
  if(_maxReadCount == UNLIMITED_COUNT || readCount == UNLIMITED_COUNT)
    _maxReadCount = UNLIMITED_COUNT;
  else _maxReadCount += readCount;
}

Result ResultBufferImpl::createResult()
{
  return new BufferedResult(this);
}

Item::Ptr ResultBufferImpl::item(unsigned int index, DynamicContext *context)
{
  while(!_result.isNull() && index >= _items.size()) {
    const Item::Ptr item = _result->next(context);
    if(item.isNull()) {
      _result = 0;
    }
    else {
      _items.push_back(item);
    }
  }

  if(index >= _items.size()) {
    return 0;
  }
  else {
    return _items[index];
  }
}

BufferedResult::BufferedResult(ResultBufferImpl *impl)
  : ResultImpl(0),
    _impl(impl),
    _pos(0)
{
}

Item::Ptr BufferedResult::next(DynamicContext *context)
{
  return _impl->item(_pos++, context);
}

ResultBufferImpl *BufferedResult::toResultBuffer(unsigned int readCount)
{
  _impl->increaseMaxReadCount(readCount);
  return _impl;
}

std::string BufferedResult::asString(DynamicContext *context, int indent) const
{
  return "bufferedresult";
}
