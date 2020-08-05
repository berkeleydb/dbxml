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
 * $Id$
 */

#include <xqilla/debug/InputParser.hpp>
#include <iostream>
#include <sstream>

using namespace std;

void InputParser::reset()
{
  start_ = 0;
  pos_ = 0;
  quoteChar_ = 0;
  curlyBraceDepth_ = 0;
  seenNonWS_ = false;
  stripQuotes_ = false;
}

int InputParser::parse(istream &input, Args &args)
{
  reset();
  args.clear();

  int lines = 0;

  getline(input, line_);
  ++lines;
  for(; (size_t)pos_ < line_.length(); ++pos_) {
    char c = line_[pos_];
    switch(c) {
    case '\\': {
      if(pos_ == line_.length() - 1) {
        // Escaped newline, get next line of input
        nextLine(input, args);
        ++lines;
      }
      else {
        escapedCharacter();
      }
      break;
    }
    case ' ':
    case '\t':
    case '\n':
    case '\r': {
      whitespace(args);
      break;
    }
    case '\'':
    case '"': {
      quote(args, c);
      break;
    }
    case '{': {
      openCurly();
      break;
    }
    case '}': {
      closeCurly();
      break;
    }
    case '#': {
      if (!inQuotes())
        pos_ = line_.length();
      break; // force end of parse for this line
    }
    default: {
      character();
      break;
    }
    }

    if(inQuotes() && pos_ == line_.length() - 1) {
      // We have an unfinished quote, so get the next line
      nextLine(input, args);
      ++lines;
    }
  }
  endOfArg(args);

  line_ = "";

  return lines;
}

bool InputParser::inQuotes()
{
  return quoteChar_ || curlyBraceDepth_ != 0;
}

void InputParser::nextLine(istream &input, Args &args)
{
  string nextbit;
  getline(input, nextbit);
  line_ += "\n" + nextbit;

  // Counts as whitespace
  whitespace(args);
}

void InputParser::whitespace(Args &args)
{
  if(inQuotes()) {
    // Counts as character
    character();
  }
  else {
    endOfArg(args);
  }
}

void InputParser::endOfArg(Args &args)
{
  if(seenNonWS_) {
    // Whitespace as a separator
    if(stripQuotes_) {
      addArgument(args, start_ + 1, pos_ - 1);
      stripQuotes_ = false;
    }
    else {
      addArgument(args, start_, pos_);
    }
    seenNonWS_ = false;
  }
}

void InputParser::character()
{
  // Significant character
  if(!seenNonWS_) {
    start_ = pos_;
  }
  if(!inQuotes()) {
    stripQuotes_ = false;
  }
  seenNonWS_ = true;
}

void InputParser::escapedCharacter()
{
  character();
  ++pos_;
}

void InputParser::quote(Args &args, char c)
{
  if(!inQuotes()) {
    // Start of quoted passage
    quoteChar_ = c;
    stripQuotes_ = !seenNonWS_;
    character();
  }
  else if(quoteChar_ == c) {
    // End of quoted passage
    character();
    quoteChar_ = 0;
  }
  else {
    // Counts as significant character
    character();
  }
}

void InputParser::openCurly()
{
  if(quoteChar_) {
    // counts as character
    character();
  }
  else {
    if(curlyBraceDepth_ == 0) {
      stripQuotes_ = !seenNonWS_;
    }
    ++curlyBraceDepth_;
    character();
  }
}

void InputParser::closeCurly()
{
  if(curlyBraceDepth_ != 0) {
    character();
    --curlyBraceDepth_;
  }
  else {
    // Counts as character
    character();
  }
}

void InputParser::addArgument(Args &args, string::size_type start, string::size_type end)
{
  // Un-escape the argument
  ostringstream oss;
  for(string::size_type i = start; i < end; ++i) {
    char c = line_[i];
    if(c == '\\') {
      ++i;
      if(i < pos_) {
        c = line_[i];
        switch(c) {
        case 'n': { // escaped newline
          oss.put('\n');
          break;
        }
        case 'r': { // escaped carriage return
          oss.put('\r');
          break;
        }
        case 't': { // escaped tab
          oss.put('\t');
          break;
        }
        default: {
          oss.put(c);
          break;
        }
        }
      }
      else {
        oss.put(c);
      }
    }
    else {
      oss.put(c);
    }
  }

  args.push_back(oss.str());
}

