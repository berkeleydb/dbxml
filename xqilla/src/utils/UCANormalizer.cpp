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
 * $Id: UCANormalizer.cpp 475 2008-01-08 18:47:44Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/UCANormalizer.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLString.hpp>

#define NO_COMPOSITION 0xFFFFFFFF

using namespace std;
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

void NormalizeTransform::pushChar(unsigned int ch)
{
  if(ch == 0) {
    composeCache();
    dest_->pushChar(ch);
  }
  else {
    getRecursiveDecomposition(ch);
  }
}

void NormalizeTransform::getRecursiveDecomposition(unsigned int ch)
{
  unsigned int *decomp = getDecomposition(ch);
  if(decomp != 0) {
    for(; *decomp != 0; ++decomp)
      getRecursiveDecomposition(*decomp);
  } 
  else if(!decomposeHangul(ch)) {
    unsigned int chClass = getCanonicalCombiningClass(ch);
    if(chClass != 0) {
      if(cache_.empty()) {
        cache_.push_back(ch);
      }
      else {
        // Find the correct position for the combining char
        vector<unsigned int>::iterator begin = cache_.begin();
        vector<unsigned int>::iterator pos = cache_.end();
        while(pos != begin) {
          --pos;
          if(getCanonicalCombiningClass(*pos) <= chClass) {
            ++pos;
            break;
          }
        }
        cache_.insert(pos, ch);
      }
    }
    else {
      composeCache();
      cache_.push_back(ch);
    }
  }
}

// Performs composition on the cached characters if required,
// and sends them to the destination
void NormalizeTransform::composeCache()
{
  if(cache_.empty()) return;

  vector<unsigned int>::iterator starterPos = cache_.begin();
  vector<unsigned int>::iterator end = cache_.end();
  unsigned int starterCh = *starterPos;
  int lastClass = getCanonicalCombiningClass(starterCh);

  if(!compose_ || lastClass != 0) {
    // Unbuffer the chars in the cache
    for(; starterPos != end; ++starterPos) {
      dest_->pushChar(*starterPos);
    }
  }
  else {
    vector<unsigned int>::iterator target = starterPos;
    ++target;

    // Loop on the decomposed characters, combining where possible
    for(vector<unsigned int>::iterator source = target; source != end; ++source) {
      unsigned int ch = *source;

      int chClass = getCanonicalCombiningClass(ch);
      if(lastClass < chClass) {

        unsigned int composite = getComposition(starterCh, ch);
        if(composite == NO_COMPOSITION)
          composite = composeHangul(starterCh, ch);

        if(composite != NO_COMPOSITION) {
          *starterPos = composite;
          starterCh = composite;
          continue;
        }
      }
      else {
        lastClass = chClass;
        *target++ = ch;
      }
    }

    for(starterPos = cache_.begin(); starterPos != target; ++starterPos) {
      dest_->pushChar(*starterPos);
    }
  }

  cache_.clear();
}

// Hangul constants
static const unsigned int SBase = 0xAC00, LBase = 0x1100, VBase = 0x1161, TBase = 0x11A7,
	LCount = 19, VCount = 21, TCount = 28,
	NCount = VCount * TCount,   // 588
	SCount = LCount * NCount;   // 11172

bool NormalizeTransform::decomposeHangul(unsigned int s)
{
  if(s < SBase) return false;

  unsigned int SIndex = s - SBase;
  if(SIndex >= SCount) return false;

  unsigned int l = LBase + SIndex / NCount;
  getRecursiveDecomposition(l);

  unsigned int v = VBase + (SIndex % NCount) / TCount;
  getRecursiveDecomposition(v);

  unsigned int t = TBase + SIndex % TCount;
  if(t != TBase)
    getRecursiveDecomposition(t);

  return true;
}

unsigned int NormalizeTransform::composeHangul(unsigned int first, unsigned int second)
{
  // 1. check to see if two current characters are L and V
  if(first >= LBase && second >= VBase) {
    unsigned int LIndex = first - LBase;
    unsigned int VIndex = second - VBase;
    if(LIndex < LCount && VIndex < VCount) {
      // make syllable of form LV
      return SBase + (LIndex * VCount + VIndex) * TCount;
    }
  }

  // 2. check to see if two current characters are LV and T
  if(first >= SBase && second > TBase) {
    unsigned int SIndex = first - SBase;
    unsigned int TIndex = second - TBase;
    if(SIndex < SCount && (SIndex % TCount) == 0 && TIndex < TCount) {
      // make syllable of form LVT
      return first + TIndex;
    }
  }

  return NO_COMPOSITION;
}

void RemoveDiacriticsTransform::pushChar(unsigned int ch)
{
  if(ch == 0 || !isDiacritic(ch)) {
    dest_->pushChar(ch);
  }
}

void CaseFoldTransform::pushChar(unsigned int ch)
{
  if(ch != 0) {
    unsigned int *value = getCaseFold(ch);
    if(value != 0) {
      while(*value != 0) {
        dest_->pushChar(*value);
        ++value;
      }
      return;
    }
  }

  dest_->pushChar(ch);
}

void LowerCaseTransform::pushChar(unsigned int ch)
{
  if(ch != 0) {
    unsigned int *value = getLowerCase(ch);
    if(value != 0) {
      while(*value != 0) {
        dest_->pushChar(*value);
        ++value;
      }
      return;
    }
  }

  dest_->pushChar(ch);
}

void UpperCaseTransform::pushChar(unsigned int ch)
{
  if(ch != 0) {
    unsigned int *value = getUpperCase(ch);
    if(value != 0) {
      while(*value != 0) {
        dest_->pushChar(*value);
        ++value;
      }
      return;
    }
  }

  dest_->pushChar(ch);
}

void XMLBufferTransform::pushChar(unsigned int ch)
{
  if(!(ch & 0xFFFF0000)) {
    if(ch != 0)
      buffer_.append((XMLCh)ch);
  }
  else {
    assert(ch <= 0x10FFFF);

    // Store the leading surrogate char
    ch -= 0x10000;
    buffer_.append((XMLCh)((ch >> 10) | 0xD800));

    // the trailing char
    buffer_.append((XMLCh)((ch & 0x3FF) | 0xDC00));
  }
}

void StringTransformer::transformUTF16(const XMLCh *source, StringTransform *transform)
{
  while(*source != 0) {
    unsigned int ch = *source;
    ++source;

    if((ch & 0xDC00) == 0xD800) {
	    if(*source == 0) break;
	    ch = ((ch & 0x3FF) << 10) | (*source & 0x3FF);
	    ch += 0x10000;
	    ++source;
    }

    transform->pushChar(ch);
  }
  transform->pushChar(0);
}

void Normalizer::normalizeC(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  NormalizeTransform normalize(true, true, &buf);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::normalizeD(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  NormalizeTransform normalize(true, false, &buf);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::normalizeKC(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  NormalizeTransform normalize(false, true, &buf);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::normalizeKD(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  NormalizeTransform normalize(false, false, &buf);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::removeDiacritics(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  RemoveDiacriticsTransform diacritics(&buf);
  NormalizeTransform normalize(true, false, &diacritics);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::caseFold(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  CaseFoldTransform caseFold(&buf);
  StringTransformer::transformUTF16(source, &caseFold);
}

void Normalizer::caseFoldAndRemoveDiacritics(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  CaseFoldTransform caseFold(&buf);
  RemoveDiacriticsTransform diacritics(&caseFold);
  NormalizeTransform normalize(true, false, &diacritics);
  StringTransformer::transformUTF16(source, &normalize);
}

void Normalizer::lowerCase(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  LowerCaseTransform caseFold(&buf);
  StringTransformer::transformUTF16(source, &caseFold);
}

void Normalizer::upperCase(const XMLCh* source, XMLBuffer &dest)
{
  XMLBufferTransform buf(dest);
  UpperCaseTransform caseFold(&buf);
  StringTransformer::transformUTF16(source, &caseFold);
}
