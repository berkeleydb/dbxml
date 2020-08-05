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
 * $Id: DefaultTokenStore.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _DEFAULTTOKENSTORE_HPP
#define _DEFAULTTOKENSTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/fulltext/TokenStore.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

#include <vector>

class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API DefaultTokenStore : public TokenStore
{
public:
  typedef std::vector<TokenInfo::Ptr> Tokens;

  class TokenEntryStream : public TokenStream
  {
  public:
    TokenEntryStream(const Tokens &tokens)
      : it_(tokens.begin()), end_(tokens.end()) {}

    virtual TokenInfo::Ptr next()
    {
      if(it_ != end_)
        return *it_++;
      return 0;
    }

  private:
    Tokens::const_iterator it_;
    Tokens::const_iterator end_;
  };

  DefaultTokenStore(const Node::Ptr &node, const Tokenizer *tokenizer,
                    DynamicContext *context);
  DefaultTokenStore(XPath2MemoryManager *mm);

  virtual TokenStream::Ptr findTokens(const XMLCh *searchString) const;

  virtual unsigned int getStartTokenPosition() const { return 0; }
  virtual unsigned int getEndTokenPosition() const { return numTokens_ - 1; }

private:
  class TokenEntry
  {
  public:
    void addToken(const TokenInfo::Ptr &token)
    {
      tokens_.push_back(token);
    }
    TokenStream::Ptr getTokenStream() const
    {
      return new TokenEntryStream(tokens_);
    }

  private:
    Tokens tokens_;
  };

private:
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<TokenEntry> tokens_;
  unsigned int numTokens_;
  XPath2MemoryManager *mm_;

  mutable XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer_;
};

#endif
