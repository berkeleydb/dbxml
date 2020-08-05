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
 * $Id: UCANormalizer.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef UCANORMALIZER_HPP
#define UCANORMALIZER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <vector>

///
class XQILLA_API StringTransform
{
public:
  virtual ~StringTransform() {}

  virtual void pushChar(unsigned int ch) = 0;
};

///
class XQILLA_API NormalizeTransform : public StringTransform
{
public:
  NormalizeTransform(bool canonical, bool compose, StringTransform *destination)
    : canonical_(canonical), compose_(compose), dest_(destination) {}

  virtual void pushChar(unsigned int ch);

private:
  void getRecursiveDecomposition(unsigned int ch);
  bool decomposeHangul(unsigned int s);
  unsigned int *getDecomposition(unsigned int ch);

  void composeCache();
  static unsigned int composeHangul(unsigned int first, unsigned int second);
  static unsigned int getComposition(unsigned int first, unsigned int second);

  static unsigned int getCanonicalCombiningClass(unsigned int ch);

  bool canonical_, compose_;
  StringTransform *dest_;
  std::vector<unsigned int> cache_;
};

///
class XQILLA_API RemoveDiacriticsTransform : public StringTransform
{
public:
  RemoveDiacriticsTransform(StringTransform *destination)
    : dest_(destination) {}

  virtual void pushChar(unsigned int ch);

private:
  static bool isDiacritic(unsigned int ch);

  StringTransform *dest_;
};

///
class XQILLA_API CaseFoldTransform : public StringTransform
{
public:
  CaseFoldTransform(StringTransform *destination)
    : dest_(destination) {}

  virtual void pushChar(unsigned int ch);

private:
  static unsigned int *getCaseFold(unsigned int ch);

  StringTransform *dest_;
};

///
class XQILLA_API LowerCaseTransform : public StringTransform
{
public:
  LowerCaseTransform(StringTransform *destination)
    : dest_(destination) {}

  virtual void pushChar(unsigned int ch);

private:
  static unsigned int *getLowerCase(unsigned int ch);

  StringTransform *dest_;
};

///
class XQILLA_API UpperCaseTransform : public StringTransform
{
public:
  UpperCaseTransform(StringTransform *destination)
    : dest_(destination) {}

  virtual void pushChar(unsigned int ch);

private:
  static unsigned int *getUpperCase(unsigned int ch);

  StringTransform *dest_;
};

///
class XQILLA_API XMLBufferTransform : public StringTransform
{
public:
  XMLBufferTransform(XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &buffer)
    : buffer_(buffer) {}

  virtual void pushChar(unsigned int ch);

private:
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &buffer_;
};

///
class XQILLA_API StringTransformer
{
public:
  static void transformUTF16(const XMLCh *source, StringTransform *transform);
};

///
class XQILLA_API Normalizer
{
public:
  static void normalizeC(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void normalizeD(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void normalizeKC(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void normalizeKD(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);

  static void removeDiacritics(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void caseFold(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void caseFoldAndRemoveDiacritics(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);

  static void lowerCase(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
  static void upperCase(const XMLCh* source, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &dest);
};

#endif
