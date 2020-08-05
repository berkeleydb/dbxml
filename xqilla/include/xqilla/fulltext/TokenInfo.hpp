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
 * $Id: TokenInfo.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _TOKENINFO_HPP
#define _TOKENINFO_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>

#include <stdlib.h>

class XQILLA_API TokenInfo : public ReferenceCounted
{
public:
  typedef RefCountPointer<const TokenInfo> Ptr;

  virtual ~TokenInfo() {}

  virtual const XMLCh *getWord() const = 0;
  virtual unsigned int getPosition() const = 0;
  virtual unsigned int getSentence() const = 0;
  virtual unsigned int getParagraph() const = 0;

protected:
  TokenInfo() {}
};

class XQILLA_API TokenStream : public ReferenceCounted
{
public:
  typedef RefCountPointer<TokenStream> Ptr;

  virtual ~TokenStream() {}

  virtual TokenInfo::Ptr next() = 0;

protected:
  TokenStream() {}
};

#endif
