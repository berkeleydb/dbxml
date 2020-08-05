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
 * $Id: ResultBuffer.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/runtime/ResultBuffer.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/context/DynamicContext.hpp>

ResultBuffer::ResultBuffer(const Result &result, unsigned int readCount)
  : _impl(const_cast<ResultImpl*>(result.get())->toResultBuffer(readCount))
{
}

ResultBuffer::ResultBuffer(const Item::Ptr &item, unsigned int readCount)
  : _impl(new ResultBufferImpl(item, readCount))
{
}

ResultBuffer::ResultBuffer(ResultBufferImpl *impl)
  : _impl(impl)
{
}

Result ResultBuffer::createResult()
{
  if(_impl.isNull()) return 0;

  Result result = _impl->createResult();

  if(_impl->getMaxReadCount() != ResultBufferImpl::UNLIMITED_COUNT &&
     _impl->incrementReadCount() >= _impl->getMaxReadCount()) {
    // We've reached the maximum read count, so noone
    // else will want to read from this ResultBuffer
    _impl = 0;
  }

  return result;
}
