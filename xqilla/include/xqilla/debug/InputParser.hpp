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

#ifndef __INPUTPARSER_HPP
#define __INPUTPARSER_HPP

#include <vector>
#include <string>
#include <istream>

#include <xqilla/framework/XQillaExport.hpp>

class XQILLA_API InputParser
{
public:
  typedef std::vector<std::string> Args;

  int parse(std::istream &input, Args &args);

private:
  void reset();
  void whitespace(Args &args);
  void character();
  void escapedCharacter();
  bool inQuotes();
  void quote(Args &args, char c);
  void openCurly();
  void closeCurly();
  void endOfArg(Args &args);
  void addArgument(Args &args, std::string::size_type start, std::string::size_type end);
  void nextLine(std::istream &input, Args &args);

  std::string line_;
  std::string::size_type start_, pos_;
  char quoteChar_;
  int curlyBraceDepth_;
  bool seenNonWS_;
  bool stripQuotes_;
};

#endif
