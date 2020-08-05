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
 * $Id: UCANormalizer3.cpp 475 2008-01-08 18:47:44Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/UCANormalizer.hpp>

static const unsigned int diacriticDirectTableSize = 64;
static const unsigned long diacriticDirectTable[64] = {
  0x00000000, 0x00000000, 0x40000000, 0x00000001, 0x00000000, 0x01908100, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xE0FF7FFF, 0x04300007, 0x00000030, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000078, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x02000000, 0x00000000, 0xFFFE0000, 0xBBFFFFFB, 0x00000016, 0x00000000,
  0x00000000, 0x00000000, 0x0187F800, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00001C61,
  0x00000000, 0xFFFF0000, 0x000007FF, 0x00000000, 0x00000000, 0x0001FFC0, 0x00000000, 0x00000000  
};

// static const  unsigned int diacriticTableSize = 482;
// static const unsigned int diacriticTable[482] = {
static const  unsigned int diacriticTableSize = 190;
static const unsigned int diacriticTable[190] = {
//   0x0005E, 0x00060, 0x000A8, 0x000AF, 0x000B4, 0x000B7, 0x000B8, 0x002B0, 0x002B1, 0x002B2, 0x002B3, 0x002B4, 0x002B5, 0x002B6, 0x002B7,
//   0x002B8, 0x002B9, 0x002BA, 0x002BB, 0x002BC, 0x002BD, 0x002BE, 0x002BF, 0x002C0, 0x002C1, 0x002C2, 0x002C3, 0x002C4, 0x002C5, 0x002C6,
//   0x002C7, 0x002C8, 0x002C9, 0x002CA, 0x002CB, 0x002CC, 0x002CD, 0x002CE, 0x002CF, 0x002D0, 0x002D1, 0x002D2, 0x002D3, 0x002D4, 0x002D5,
//   0x002D6, 0x002D7, 0x002D8, 0x002D9, 0x002DA, 0x002DB, 0x002DC, 0x002DD, 0x002DE, 0x002DF, 0x002E0, 0x002E1, 0x002E2, 0x002E3, 0x002E4,
//   0x002E5, 0x002E6, 0x002E7, 0x002E8, 0x002E9, 0x002EA, 0x002EB, 0x002EC, 0x002ED, 0x002EE, 0x002EF, 0x002F0, 0x002F1, 0x002F2, 0x002F3,
//   0x002F4, 0x002F5, 0x002F6, 0x002F7, 0x002F8, 0x002F9, 0x002FA, 0x002FB, 0x002FC, 0x002FD, 0x002FE, 0x002FF, 0x00300, 0x00301, 0x00302,
//   0x00303, 0x00304, 0x00305, 0x00306, 0x00307, 0x00308, 0x00309, 0x0030A, 0x0030B, 0x0030C, 0x0030D, 0x0030E, 0x0030F, 0x00310, 0x00311,
//   0x00312, 0x00313, 0x00314, 0x00315, 0x00316, 0x00317, 0x00318, 0x00319, 0x0031A, 0x0031B, 0x0031C, 0x0031D, 0x0031E, 0x0031F, 0x00320,
//   0x00321, 0x00322, 0x00323, 0x00324, 0x00325, 0x00326, 0x00327, 0x00328, 0x00329, 0x0032A, 0x0032B, 0x0032C, 0x0032D, 0x0032E, 0x0032F,
//   0x00330, 0x00331, 0x00332, 0x00333, 0x00334, 0x00335, 0x00336, 0x00337, 0x00338, 0x00339, 0x0033A, 0x0033B, 0x0033C, 0x0033D, 0x0033E,
//   0x0033F, 0x00340, 0x00341, 0x00342, 0x00343, 0x00344, 0x00345, 0x00346, 0x00347, 0x00348, 0x00349, 0x0034A, 0x0034B, 0x0034C, 0x0034D,
//   0x0034E, 0x00350, 0x00351, 0x00352, 0x00353, 0x00354, 0x00355, 0x00356, 0x00357, 0x0035D, 0x0035E, 0x0035F, 0x00360, 0x00361, 0x00362,
//   0x00374, 0x00375, 0x0037A, 0x00384, 0x00385, 0x00483, 0x00484, 0x00485, 0x00486, 0x00559, 0x00591, 0x00592, 0x00593, 0x00594, 0x00595,
//   0x00596, 0x00597, 0x00598, 0x00599, 0x0059A, 0x0059B, 0x0059C, 0x0059D, 0x0059E, 0x0059F, 0x005A0, 0x005A1, 0x005A3, 0x005A4, 0x005A5,
//   0x005A6, 0x005A7, 0x005A8, 0x005A9, 0x005AA, 0x005AB, 0x005AC, 0x005AD, 0x005AE, 0x005AF, 0x005B0, 0x005B1, 0x005B2, 0x005B3, 0x005B4,
//   0x005B5, 0x005B6, 0x005B7, 0x005B8, 0x005B9, 0x005BB, 0x005BC, 0x005BD, 0x005BF, 0x005C1, 0x005C2, 0x005C4, 0x0064B, 0x0064C, 0x0064D,
//   0x0064E, 0x0064F, 0x00650, 0x00651, 0x00652, 0x00657, 0x00658, 0x006DF, 0x006E0, 0x006E5, 0x006E6, 0x006EA, 0x006EB, 0x006EC, 0x00730,
//   0x00731, 0x00732, 0x00733, 0x00734, 0x00735, 0x00736, 0x00737, 0x00738, 0x00739, 0x0073A, 0x0073B, 0x0073C, 0x0073D, 0x0073E, 0x0073F,
//   0x00740, 0x00741, 0x00742, 0x00743, 0x00744, 0x00745, 0x00746, 0x00747, 0x00748, 0x00749, 0x0074A, 0x007A6, 0x007A7, 0x007A8, 0x007A9,
//   0x007AA, 0x007AB, 0x007AC, 0x007AD, 0x007AE, 0x007AF, 0x007B0,

  0x0093C, 0x0094D, 0x00951, 0x00952, 0x00953, 0x00954, 0x009BC, 0x009CD,
  0x00A3C, 0x00A4D, 0x00ABC, 0x00ACD, 0x00B3C, 0x00B4D, 0x00BCD, 0x00C4D, 0x00CBC, 0x00CCD, 0x00D4D, 0x00DCA, 0x00E47, 0x00E48, 0x00E49,
  0x00E4A, 0x00E4B, 0x00E4C, 0x00E4E, 0x00EC8, 0x00EC9, 0x00ECA, 0x00ECB, 0x00ECC, 0x00F18, 0x00F19, 0x00F35, 0x00F37, 0x00F39, 0x00F3E,
  0x00F3F, 0x00F82, 0x00F83, 0x00F84, 0x00F86, 0x00F87, 0x00FC6, 0x01037, 0x01039, 0x017C9, 0x017CA, 0x017CB, 0x017CC, 0x017CD, 0x017CE,
  0x017CF, 0x017D0, 0x017D1, 0x017D2, 0x017D3, 0x017DD, 0x01939, 0x0193A, 0x0193B, 0x01D2C, 0x01D2D, 0x01D2E, 0x01D2F, 0x01D30, 0x01D31,
  0x01D32, 0x01D33, 0x01D34, 0x01D35, 0x01D36, 0x01D37, 0x01D38, 0x01D39, 0x01D3A, 0x01D3B, 0x01D3C, 0x01D3D, 0x01D3E, 0x01D3F, 0x01D40,
  0x01D41, 0x01D42, 0x01D43, 0x01D44, 0x01D45, 0x01D46, 0x01D47, 0x01D48, 0x01D49, 0x01D4A, 0x01D4B, 0x01D4C, 0x01D4D, 0x01D4E, 0x01D4F,
  0x01D50, 0x01D51, 0x01D52, 0x01D53, 0x01D54, 0x01D55, 0x01D56, 0x01D57, 0x01D58, 0x01D59, 0x01D5A, 0x01D5B, 0x01D5C, 0x01D5D, 0x01D5E,
  0x01D5F, 0x01D60, 0x01D61, 0x01D62, 0x01D63, 0x01D64, 0x01D65, 0x01D66, 0x01D67, 0x01D68, 0x01D69, 0x01D6A, 0x01FBD, 0x01FBF, 0x01FC0,
  0x01FC1, 0x01FCD, 0x01FCE, 0x01FCF, 0x01FDD, 0x01FDE, 0x01FDF, 0x01FED, 0x01FEE, 0x01FEF, 0x01FFD, 0x01FFE, 0x0302A, 0x0302B, 0x0302C,
  0x0302D, 0x0302E, 0x0302F, 0x03099, 0x0309A, 0x0309B, 0x0309C, 0x030FC, 0x0FB1E, 0x0FE20, 0x0FE21, 0x0FE22, 0x0FE23, 0x0FF3E, 0x0FF40,
  0x0FF70, 0x0FF9E, 0x0FF9F, 0x0FFE3, 0x1D167, 0x1D168, 0x1D169, 0x1D16D, 0x1D16E, 0x1D16F, 0x1D170, 0x1D171, 0x1D172, 0x1D17B, 0x1D17C,
  0x1D17D, 0x1D17E, 0x1D17F, 0x1D180, 0x1D181, 0x1D182, 0x1D185, 0x1D186, 0x1D187, 0x1D188, 0x1D189, 0x1D18A, 0x1D18B, 0x1D1AA, 0x1D1AB,
  0x1D1AC, 0x1D1AD
};

bool RemoveDiacriticsTransform::isDiacritic(unsigned int ch)
{
  if(ch < (diacriticDirectTableSize * 32)) {
    return (diacriticDirectTable[ch / 32] & (1 << (ch % 32))) != 0;
  }

  // Binary search in the g_diacriticTable table
  unsigned int min = 0;
  unsigned int max = diacriticTableSize;
  unsigned int middle;
  int cmp;

  while(min < max) {
    middle = (max + min) >> 1;

    cmp = diacriticTable[middle] - ch;
    if(cmp > 0) max = middle;
    else if(cmp < 0) {
      min = middle + 1;
    }
    else {
      return true;
    }
  }

  return false;
}

static unsigned int cf00041[] = { 0x00061, 0x00000 };
static unsigned int cf00042[] = { 0x00062, 0x00000 };
static unsigned int cf00043[] = { 0x00063, 0x00000 };
static unsigned int cf00044[] = { 0x00064, 0x00000 };
static unsigned int cf00045[] = { 0x00065, 0x00000 };
static unsigned int cf00046[] = { 0x00066, 0x00000 };
static unsigned int cf00047[] = { 0x00067, 0x00000 };
static unsigned int cf00048[] = { 0x00068, 0x00000 };
static unsigned int cf00049[] = { 0x00069, 0x00000 };
static unsigned int cf0004A[] = { 0x0006A, 0x00000 };
static unsigned int cf0004B[] = { 0x0006B, 0x00000 };
static unsigned int cf0004C[] = { 0x0006C, 0x00000 };
static unsigned int cf0004D[] = { 0x0006D, 0x00000 };
static unsigned int cf0004E[] = { 0x0006E, 0x00000 };
static unsigned int cf0004F[] = { 0x0006F, 0x00000 };
static unsigned int cf00050[] = { 0x00070, 0x00000 };
static unsigned int cf00051[] = { 0x00071, 0x00000 };
static unsigned int cf00052[] = { 0x00072, 0x00000 };
static unsigned int cf00053[] = { 0x00073, 0x00000 };
static unsigned int cf00054[] = { 0x00074, 0x00000 };
static unsigned int cf00055[] = { 0x00075, 0x00000 };
static unsigned int cf00056[] = { 0x00076, 0x00000 };
static unsigned int cf00057[] = { 0x00077, 0x00000 };
static unsigned int cf00058[] = { 0x00078, 0x00000 };
static unsigned int cf00059[] = { 0x00079, 0x00000 };
static unsigned int cf0005A[] = { 0x0007A, 0x00000 };
static unsigned int cf000B5[] = { 0x003BC, 0x00000 };
static unsigned int cf000C0[] = { 0x000E0, 0x00000 };
static unsigned int cf000C1[] = { 0x000E1, 0x00000 };
static unsigned int cf000C2[] = { 0x000E2, 0x00000 };
static unsigned int cf000C3[] = { 0x000E3, 0x00000 };
static unsigned int cf000C4[] = { 0x000E4, 0x00000 };
static unsigned int cf000C5[] = { 0x000E5, 0x00000 };
static unsigned int cf000C6[] = { 0x000E6, 0x00000 };
static unsigned int cf000C7[] = { 0x000E7, 0x00000 };
static unsigned int cf000C8[] = { 0x000E8, 0x00000 };
static unsigned int cf000C9[] = { 0x000E9, 0x00000 };
static unsigned int cf000CA[] = { 0x000EA, 0x00000 };
static unsigned int cf000CB[] = { 0x000EB, 0x00000 };
static unsigned int cf000CC[] = { 0x000EC, 0x00000 };
static unsigned int cf000CD[] = { 0x000ED, 0x00000 };
static unsigned int cf000CE[] = { 0x000EE, 0x00000 };
static unsigned int cf000CF[] = { 0x000EF, 0x00000 };
static unsigned int cf000D0[] = { 0x000F0, 0x00000 };
static unsigned int cf000D1[] = { 0x000F1, 0x00000 };
static unsigned int cf000D2[] = { 0x000F2, 0x00000 };
static unsigned int cf000D3[] = { 0x000F3, 0x00000 };
static unsigned int cf000D4[] = { 0x000F4, 0x00000 };
static unsigned int cf000D5[] = { 0x000F5, 0x00000 };
static unsigned int cf000D6[] = { 0x000F6, 0x00000 };
static unsigned int cf000D8[] = { 0x000F8, 0x00000 };
static unsigned int cf000D9[] = { 0x000F9, 0x00000 };
static unsigned int cf000DA[] = { 0x000FA, 0x00000 };
static unsigned int cf000DB[] = { 0x000FB, 0x00000 };
static unsigned int cf000DC[] = { 0x000FC, 0x00000 };
static unsigned int cf000DD[] = { 0x000FD, 0x00000 };
static unsigned int cf000DE[] = { 0x000FE, 0x00000 };
static unsigned int cf000DF[] = { 0x00073, 0x00073, 0x00000 };
static unsigned int cf00100[] = { 0x00101, 0x00000 };
static unsigned int cf00102[] = { 0x00103, 0x00000 };
static unsigned int cf00104[] = { 0x00105, 0x00000 };
static unsigned int cf00106[] = { 0x00107, 0x00000 };
static unsigned int cf00108[] = { 0x00109, 0x00000 };
static unsigned int cf0010A[] = { 0x0010B, 0x00000 };
static unsigned int cf0010C[] = { 0x0010D, 0x00000 };
static unsigned int cf0010E[] = { 0x0010F, 0x00000 };
static unsigned int cf00110[] = { 0x00111, 0x00000 };
static unsigned int cf00112[] = { 0x00113, 0x00000 };
static unsigned int cf00114[] = { 0x00115, 0x00000 };
static unsigned int cf00116[] = { 0x00117, 0x00000 };
static unsigned int cf00118[] = { 0x00119, 0x00000 };
static unsigned int cf0011A[] = { 0x0011B, 0x00000 };
static unsigned int cf0011C[] = { 0x0011D, 0x00000 };
static unsigned int cf0011E[] = { 0x0011F, 0x00000 };
static unsigned int cf00120[] = { 0x00121, 0x00000 };
static unsigned int cf00122[] = { 0x00123, 0x00000 };
static unsigned int cf00124[] = { 0x00125, 0x00000 };
static unsigned int cf00126[] = { 0x00127, 0x00000 };
static unsigned int cf00128[] = { 0x00129, 0x00000 };
static unsigned int cf0012A[] = { 0x0012B, 0x00000 };
static unsigned int cf0012C[] = { 0x0012D, 0x00000 };
static unsigned int cf0012E[] = { 0x0012F, 0x00000 };
static unsigned int cf00130[] = { 0x00069, 0x00307, 0x00000 };
static unsigned int cf00132[] = { 0x00133, 0x00000 };
static unsigned int cf00134[] = { 0x00135, 0x00000 };
static unsigned int cf00136[] = { 0x00137, 0x00000 };
static unsigned int cf00139[] = { 0x0013A, 0x00000 };
static unsigned int cf0013B[] = { 0x0013C, 0x00000 };
static unsigned int cf0013D[] = { 0x0013E, 0x00000 };
static unsigned int cf0013F[] = { 0x00140, 0x00000 };
static unsigned int cf00141[] = { 0x00142, 0x00000 };
static unsigned int cf00143[] = { 0x00144, 0x00000 };
static unsigned int cf00145[] = { 0x00146, 0x00000 };
static unsigned int cf00147[] = { 0x00148, 0x00000 };
static unsigned int cf00149[] = { 0x002BC, 0x0006E, 0x00000 };
static unsigned int cf0014A[] = { 0x0014B, 0x00000 };
static unsigned int cf0014C[] = { 0x0014D, 0x00000 };
static unsigned int cf0014E[] = { 0x0014F, 0x00000 };
static unsigned int cf00150[] = { 0x00151, 0x00000 };
static unsigned int cf00152[] = { 0x00153, 0x00000 };
static unsigned int cf00154[] = { 0x00155, 0x00000 };
static unsigned int cf00156[] = { 0x00157, 0x00000 };
static unsigned int cf00158[] = { 0x00159, 0x00000 };
static unsigned int cf0015A[] = { 0x0015B, 0x00000 };
static unsigned int cf0015C[] = { 0x0015D, 0x00000 };
static unsigned int cf0015E[] = { 0x0015F, 0x00000 };
static unsigned int cf00160[] = { 0x00161, 0x00000 };
static unsigned int cf00162[] = { 0x00163, 0x00000 };
static unsigned int cf00164[] = { 0x00165, 0x00000 };
static unsigned int cf00166[] = { 0x00167, 0x00000 };
static unsigned int cf00168[] = { 0x00169, 0x00000 };
static unsigned int cf0016A[] = { 0x0016B, 0x00000 };
static unsigned int cf0016C[] = { 0x0016D, 0x00000 };
static unsigned int cf0016E[] = { 0x0016F, 0x00000 };
static unsigned int cf00170[] = { 0x00171, 0x00000 };
static unsigned int cf00172[] = { 0x00173, 0x00000 };
static unsigned int cf00174[] = { 0x00175, 0x00000 };
static unsigned int cf00176[] = { 0x00177, 0x00000 };
static unsigned int cf00178[] = { 0x000FF, 0x00000 };
static unsigned int cf00179[] = { 0x0017A, 0x00000 };
static unsigned int cf0017B[] = { 0x0017C, 0x00000 };
static unsigned int cf0017D[] = { 0x0017E, 0x00000 };
static unsigned int cf0017F[] = { 0x00073, 0x00000 };
static unsigned int cf00181[] = { 0x00253, 0x00000 };
static unsigned int cf00182[] = { 0x00183, 0x00000 };
static unsigned int cf00184[] = { 0x00185, 0x00000 };
static unsigned int cf00186[] = { 0x00254, 0x00000 };
static unsigned int cf00187[] = { 0x00188, 0x00000 };
static unsigned int cf00189[] = { 0x00256, 0x00000 };
static unsigned int cf0018A[] = { 0x00257, 0x00000 };
static unsigned int cf0018B[] = { 0x0018C, 0x00000 };
static unsigned int cf0018E[] = { 0x001DD, 0x00000 };
static unsigned int cf0018F[] = { 0x00259, 0x00000 };
static unsigned int cf00190[] = { 0x0025B, 0x00000 };
static unsigned int cf00191[] = { 0x00192, 0x00000 };
static unsigned int cf00193[] = { 0x00260, 0x00000 };
static unsigned int cf00194[] = { 0x00263, 0x00000 };
static unsigned int cf00196[] = { 0x00269, 0x00000 };
static unsigned int cf00197[] = { 0x00268, 0x00000 };
static unsigned int cf00198[] = { 0x00199, 0x00000 };
static unsigned int cf0019C[] = { 0x0026F, 0x00000 };
static unsigned int cf0019D[] = { 0x00272, 0x00000 };
static unsigned int cf0019F[] = { 0x00275, 0x00000 };
static unsigned int cf001A0[] = { 0x001A1, 0x00000 };
static unsigned int cf001A2[] = { 0x001A3, 0x00000 };
static unsigned int cf001A4[] = { 0x001A5, 0x00000 };
static unsigned int cf001A6[] = { 0x00280, 0x00000 };
static unsigned int cf001A7[] = { 0x001A8, 0x00000 };
static unsigned int cf001A9[] = { 0x00283, 0x00000 };
static unsigned int cf001AC[] = { 0x001AD, 0x00000 };
static unsigned int cf001AE[] = { 0x00288, 0x00000 };
static unsigned int cf001AF[] = { 0x001B0, 0x00000 };
static unsigned int cf001B1[] = { 0x0028A, 0x00000 };
static unsigned int cf001B2[] = { 0x0028B, 0x00000 };
static unsigned int cf001B3[] = { 0x001B4, 0x00000 };
static unsigned int cf001B5[] = { 0x001B6, 0x00000 };
static unsigned int cf001B7[] = { 0x00292, 0x00000 };
static unsigned int cf001B8[] = { 0x001B9, 0x00000 };
static unsigned int cf001BC[] = { 0x001BD, 0x00000 };
static unsigned int cf001C4[] = { 0x001C6, 0x00000 };
static unsigned int cf001C5[] = { 0x001C6, 0x00000 };
static unsigned int cf001C7[] = { 0x001C9, 0x00000 };
static unsigned int cf001C8[] = { 0x001C9, 0x00000 };
static unsigned int cf001CA[] = { 0x001CC, 0x00000 };
static unsigned int cf001CB[] = { 0x001CC, 0x00000 };
static unsigned int cf001CD[] = { 0x001CE, 0x00000 };
static unsigned int cf001CF[] = { 0x001D0, 0x00000 };
static unsigned int cf001D1[] = { 0x001D2, 0x00000 };
static unsigned int cf001D3[] = { 0x001D4, 0x00000 };
static unsigned int cf001D5[] = { 0x001D6, 0x00000 };
static unsigned int cf001D7[] = { 0x001D8, 0x00000 };
static unsigned int cf001D9[] = { 0x001DA, 0x00000 };
static unsigned int cf001DB[] = { 0x001DC, 0x00000 };
static unsigned int cf001DE[] = { 0x001DF, 0x00000 };
static unsigned int cf001E0[] = { 0x001E1, 0x00000 };
static unsigned int cf001E2[] = { 0x001E3, 0x00000 };
static unsigned int cf001E4[] = { 0x001E5, 0x00000 };
static unsigned int cf001E6[] = { 0x001E7, 0x00000 };
static unsigned int cf001E8[] = { 0x001E9, 0x00000 };
static unsigned int cf001EA[] = { 0x001EB, 0x00000 };
static unsigned int cf001EC[] = { 0x001ED, 0x00000 };
static unsigned int cf001EE[] = { 0x001EF, 0x00000 };
static unsigned int cf001F0[] = { 0x0006A, 0x0030C, 0x00000 };
static unsigned int cf001F1[] = { 0x001F3, 0x00000 };
static unsigned int cf001F2[] = { 0x001F3, 0x00000 };
static unsigned int cf001F4[] = { 0x001F5, 0x00000 };
static unsigned int cf001F6[] = { 0x00195, 0x00000 };
static unsigned int cf001F7[] = { 0x001BF, 0x00000 };
static unsigned int cf001F8[] = { 0x001F9, 0x00000 };
static unsigned int cf001FA[] = { 0x001FB, 0x00000 };
static unsigned int cf001FC[] = { 0x001FD, 0x00000 };
static unsigned int cf001FE[] = { 0x001FF, 0x00000 };
static unsigned int cf00200[] = { 0x00201, 0x00000 };
static unsigned int cf00202[] = { 0x00203, 0x00000 };
static unsigned int cf00204[] = { 0x00205, 0x00000 };
static unsigned int cf00206[] = { 0x00207, 0x00000 };
static unsigned int cf00208[] = { 0x00209, 0x00000 };
static unsigned int cf0020A[] = { 0x0020B, 0x00000 };
static unsigned int cf0020C[] = { 0x0020D, 0x00000 };
static unsigned int cf0020E[] = { 0x0020F, 0x00000 };
static unsigned int cf00210[] = { 0x00211, 0x00000 };
static unsigned int cf00212[] = { 0x00213, 0x00000 };
static unsigned int cf00214[] = { 0x00215, 0x00000 };
static unsigned int cf00216[] = { 0x00217, 0x00000 };
static unsigned int cf00218[] = { 0x00219, 0x00000 };
static unsigned int cf0021A[] = { 0x0021B, 0x00000 };
static unsigned int cf0021C[] = { 0x0021D, 0x00000 };
static unsigned int cf0021E[] = { 0x0021F, 0x00000 };
static unsigned int cf00220[] = { 0x0019E, 0x00000 };
static unsigned int cf00222[] = { 0x00223, 0x00000 };
static unsigned int cf00224[] = { 0x00225, 0x00000 };
static unsigned int cf00226[] = { 0x00227, 0x00000 };
static unsigned int cf00228[] = { 0x00229, 0x00000 };
static unsigned int cf0022A[] = { 0x0022B, 0x00000 };
static unsigned int cf0022C[] = { 0x0022D, 0x00000 };
static unsigned int cf0022E[] = { 0x0022F, 0x00000 };
static unsigned int cf00230[] = { 0x00231, 0x00000 };
static unsigned int cf00232[] = { 0x00233, 0x00000 };
static unsigned int cf0023B[] = { 0x0023C, 0x00000 };
static unsigned int cf0023D[] = { 0x0019A, 0x00000 };
static unsigned int cf00241[] = { 0x00294, 0x00000 };
static unsigned int cf00345[] = { 0x003B9, 0x00000 };
static unsigned int cf00386[] = { 0x003AC, 0x00000 };
static unsigned int cf00388[] = { 0x003AD, 0x00000 };
static unsigned int cf00389[] = { 0x003AE, 0x00000 };
static unsigned int cf0038A[] = { 0x003AF, 0x00000 };
static unsigned int cf0038C[] = { 0x003CC, 0x00000 };
static unsigned int cf0038E[] = { 0x003CD, 0x00000 };
static unsigned int cf0038F[] = { 0x003CE, 0x00000 };
static unsigned int cf00390[] = { 0x003B9, 0x00308, 0x00301, 0x00000 };
static unsigned int cf00391[] = { 0x003B1, 0x00000 };
static unsigned int cf00392[] = { 0x003B2, 0x00000 };
static unsigned int cf00393[] = { 0x003B3, 0x00000 };
static unsigned int cf00394[] = { 0x003B4, 0x00000 };
static unsigned int cf00395[] = { 0x003B5, 0x00000 };
static unsigned int cf00396[] = { 0x003B6, 0x00000 };
static unsigned int cf00397[] = { 0x003B7, 0x00000 };
static unsigned int cf00398[] = { 0x003B8, 0x00000 };
static unsigned int cf00399[] = { 0x003B9, 0x00000 };
static unsigned int cf0039A[] = { 0x003BA, 0x00000 };
static unsigned int cf0039B[] = { 0x003BB, 0x00000 };
static unsigned int cf0039C[] = { 0x003BC, 0x00000 };
static unsigned int cf0039D[] = { 0x003BD, 0x00000 };
static unsigned int cf0039E[] = { 0x003BE, 0x00000 };
static unsigned int cf0039F[] = { 0x003BF, 0x00000 };
static unsigned int cf003A0[] = { 0x003C0, 0x00000 };
static unsigned int cf003A1[] = { 0x003C1, 0x00000 };
static unsigned int cf003A3[] = { 0x003C3, 0x00000 };
static unsigned int cf003A4[] = { 0x003C4, 0x00000 };
static unsigned int cf003A5[] = { 0x003C5, 0x00000 };
static unsigned int cf003A6[] = { 0x003C6, 0x00000 };
static unsigned int cf003A7[] = { 0x003C7, 0x00000 };
static unsigned int cf003A8[] = { 0x003C8, 0x00000 };
static unsigned int cf003A9[] = { 0x003C9, 0x00000 };
static unsigned int cf003AA[] = { 0x003CA, 0x00000 };
static unsigned int cf003AB[] = { 0x003CB, 0x00000 };
static unsigned int cf003B0[] = { 0x003C5, 0x00308, 0x00301, 0x00000 };
static unsigned int cf003C2[] = { 0x003C3, 0x00000 };
static unsigned int cf003D0[] = { 0x003B2, 0x00000 };
static unsigned int cf003D1[] = { 0x003B8, 0x00000 };
static unsigned int cf003D5[] = { 0x003C6, 0x00000 };
static unsigned int cf003D6[] = { 0x003C0, 0x00000 };
static unsigned int cf003D8[] = { 0x003D9, 0x00000 };
static unsigned int cf003DA[] = { 0x003DB, 0x00000 };
static unsigned int cf003DC[] = { 0x003DD, 0x00000 };
static unsigned int cf003DE[] = { 0x003DF, 0x00000 };
static unsigned int cf003E0[] = { 0x003E1, 0x00000 };
static unsigned int cf003E2[] = { 0x003E3, 0x00000 };
static unsigned int cf003E4[] = { 0x003E5, 0x00000 };
static unsigned int cf003E6[] = { 0x003E7, 0x00000 };
static unsigned int cf003E8[] = { 0x003E9, 0x00000 };
static unsigned int cf003EA[] = { 0x003EB, 0x00000 };
static unsigned int cf003EC[] = { 0x003ED, 0x00000 };
static unsigned int cf003EE[] = { 0x003EF, 0x00000 };
static unsigned int cf003F0[] = { 0x003BA, 0x00000 };
static unsigned int cf003F1[] = { 0x003C1, 0x00000 };
static unsigned int cf003F4[] = { 0x003B8, 0x00000 };
static unsigned int cf003F5[] = { 0x003B5, 0x00000 };
static unsigned int cf003F7[] = { 0x003F8, 0x00000 };
static unsigned int cf003F9[] = { 0x003F2, 0x00000 };
static unsigned int cf003FA[] = { 0x003FB, 0x00000 };
static unsigned int cf00400[] = { 0x00450, 0x00000 };
static unsigned int cf00401[] = { 0x00451, 0x00000 };
static unsigned int cf00402[] = { 0x00452, 0x00000 };
static unsigned int cf00403[] = { 0x00453, 0x00000 };
static unsigned int cf00404[] = { 0x00454, 0x00000 };
static unsigned int cf00405[] = { 0x00455, 0x00000 };
static unsigned int cf00406[] = { 0x00456, 0x00000 };
static unsigned int cf00407[] = { 0x00457, 0x00000 };
static unsigned int cf00408[] = { 0x00458, 0x00000 };
static unsigned int cf00409[] = { 0x00459, 0x00000 };
static unsigned int cf0040A[] = { 0x0045A, 0x00000 };
static unsigned int cf0040B[] = { 0x0045B, 0x00000 };
static unsigned int cf0040C[] = { 0x0045C, 0x00000 };
static unsigned int cf0040D[] = { 0x0045D, 0x00000 };
static unsigned int cf0040E[] = { 0x0045E, 0x00000 };
static unsigned int cf0040F[] = { 0x0045F, 0x00000 };
static unsigned int cf00410[] = { 0x00430, 0x00000 };
static unsigned int cf00411[] = { 0x00431, 0x00000 };
static unsigned int cf00412[] = { 0x00432, 0x00000 };
static unsigned int cf00413[] = { 0x00433, 0x00000 };
static unsigned int cf00414[] = { 0x00434, 0x00000 };
static unsigned int cf00415[] = { 0x00435, 0x00000 };
static unsigned int cf00416[] = { 0x00436, 0x00000 };
static unsigned int cf00417[] = { 0x00437, 0x00000 };
static unsigned int cf00418[] = { 0x00438, 0x00000 };
static unsigned int cf00419[] = { 0x00439, 0x00000 };
static unsigned int cf0041A[] = { 0x0043A, 0x00000 };
static unsigned int cf0041B[] = { 0x0043B, 0x00000 };
static unsigned int cf0041C[] = { 0x0043C, 0x00000 };
static unsigned int cf0041D[] = { 0x0043D, 0x00000 };
static unsigned int cf0041E[] = { 0x0043E, 0x00000 };
static unsigned int cf0041F[] = { 0x0043F, 0x00000 };
static unsigned int cf00420[] = { 0x00440, 0x00000 };
static unsigned int cf00421[] = { 0x00441, 0x00000 };
static unsigned int cf00422[] = { 0x00442, 0x00000 };
static unsigned int cf00423[] = { 0x00443, 0x00000 };
static unsigned int cf00424[] = { 0x00444, 0x00000 };
static unsigned int cf00425[] = { 0x00445, 0x00000 };
static unsigned int cf00426[] = { 0x00446, 0x00000 };
static unsigned int cf00427[] = { 0x00447, 0x00000 };
static unsigned int cf00428[] = { 0x00448, 0x00000 };
static unsigned int cf00429[] = { 0x00449, 0x00000 };
static unsigned int cf0042A[] = { 0x0044A, 0x00000 };
static unsigned int cf0042B[] = { 0x0044B, 0x00000 };
static unsigned int cf0042C[] = { 0x0044C, 0x00000 };
static unsigned int cf0042D[] = { 0x0044D, 0x00000 };
static unsigned int cf0042E[] = { 0x0044E, 0x00000 };
static unsigned int cf0042F[] = { 0x0044F, 0x00000 };
static unsigned int cf00460[] = { 0x00461, 0x00000 };
static unsigned int cf00462[] = { 0x00463, 0x00000 };
static unsigned int cf00464[] = { 0x00465, 0x00000 };
static unsigned int cf00466[] = { 0x00467, 0x00000 };
static unsigned int cf00468[] = { 0x00469, 0x00000 };
static unsigned int cf0046A[] = { 0x0046B, 0x00000 };
static unsigned int cf0046C[] = { 0x0046D, 0x00000 };
static unsigned int cf0046E[] = { 0x0046F, 0x00000 };
static unsigned int cf00470[] = { 0x00471, 0x00000 };
static unsigned int cf00472[] = { 0x00473, 0x00000 };
static unsigned int cf00474[] = { 0x00475, 0x00000 };
static unsigned int cf00476[] = { 0x00477, 0x00000 };
static unsigned int cf00478[] = { 0x00479, 0x00000 };
static unsigned int cf0047A[] = { 0x0047B, 0x00000 };
static unsigned int cf0047C[] = { 0x0047D, 0x00000 };
static unsigned int cf0047E[] = { 0x0047F, 0x00000 };
static unsigned int cf00480[] = { 0x00481, 0x00000 };
static unsigned int cf0048A[] = { 0x0048B, 0x00000 };
static unsigned int cf0048C[] = { 0x0048D, 0x00000 };
static unsigned int cf0048E[] = { 0x0048F, 0x00000 };
static unsigned int cf00490[] = { 0x00491, 0x00000 };
static unsigned int cf00492[] = { 0x00493, 0x00000 };
static unsigned int cf00494[] = { 0x00495, 0x00000 };
static unsigned int cf00496[] = { 0x00497, 0x00000 };
static unsigned int cf00498[] = { 0x00499, 0x00000 };
static unsigned int cf0049A[] = { 0x0049B, 0x00000 };
static unsigned int cf0049C[] = { 0x0049D, 0x00000 };
static unsigned int cf0049E[] = { 0x0049F, 0x00000 };
static unsigned int cf004A0[] = { 0x004A1, 0x00000 };
static unsigned int cf004A2[] = { 0x004A3, 0x00000 };
static unsigned int cf004A4[] = { 0x004A5, 0x00000 };
static unsigned int cf004A6[] = { 0x004A7, 0x00000 };
static unsigned int cf004A8[] = { 0x004A9, 0x00000 };
static unsigned int cf004AA[] = { 0x004AB, 0x00000 };
static unsigned int cf004AC[] = { 0x004AD, 0x00000 };
static unsigned int cf004AE[] = { 0x004AF, 0x00000 };
static unsigned int cf004B0[] = { 0x004B1, 0x00000 };
static unsigned int cf004B2[] = { 0x004B3, 0x00000 };
static unsigned int cf004B4[] = { 0x004B5, 0x00000 };
static unsigned int cf004B6[] = { 0x004B7, 0x00000 };
static unsigned int cf004B8[] = { 0x004B9, 0x00000 };
static unsigned int cf004BA[] = { 0x004BB, 0x00000 };
static unsigned int cf004BC[] = { 0x004BD, 0x00000 };
static unsigned int cf004BE[] = { 0x004BF, 0x00000 };
static unsigned int cf004C1[] = { 0x004C2, 0x00000 };
static unsigned int cf004C3[] = { 0x004C4, 0x00000 };
static unsigned int cf004C5[] = { 0x004C6, 0x00000 };
static unsigned int cf004C7[] = { 0x004C8, 0x00000 };
static unsigned int cf004C9[] = { 0x004CA, 0x00000 };
static unsigned int cf004CB[] = { 0x004CC, 0x00000 };
static unsigned int cf004CD[] = { 0x004CE, 0x00000 };
static unsigned int cf004D0[] = { 0x004D1, 0x00000 };
static unsigned int cf004D2[] = { 0x004D3, 0x00000 };
static unsigned int cf004D4[] = { 0x004D5, 0x00000 };
static unsigned int cf004D6[] = { 0x004D7, 0x00000 };
static unsigned int cf004D8[] = { 0x004D9, 0x00000 };
static unsigned int cf004DA[] = { 0x004DB, 0x00000 };
static unsigned int cf004DC[] = { 0x004DD, 0x00000 };
static unsigned int cf004DE[] = { 0x004DF, 0x00000 };
static unsigned int cf004E0[] = { 0x004E1, 0x00000 };
static unsigned int cf004E2[] = { 0x004E3, 0x00000 };
static unsigned int cf004E4[] = { 0x004E5, 0x00000 };
static unsigned int cf004E6[] = { 0x004E7, 0x00000 };
static unsigned int cf004E8[] = { 0x004E9, 0x00000 };
static unsigned int cf004EA[] = { 0x004EB, 0x00000 };
static unsigned int cf004EC[] = { 0x004ED, 0x00000 };
static unsigned int cf004EE[] = { 0x004EF, 0x00000 };
static unsigned int cf004F0[] = { 0x004F1, 0x00000 };
static unsigned int cf004F2[] = { 0x004F3, 0x00000 };
static unsigned int cf004F4[] = { 0x004F5, 0x00000 };
static unsigned int cf004F6[] = { 0x004F7, 0x00000 };
static unsigned int cf004F8[] = { 0x004F9, 0x00000 };
static unsigned int cf00500[] = { 0x00501, 0x00000 };
static unsigned int cf00502[] = { 0x00503, 0x00000 };
static unsigned int cf00504[] = { 0x00505, 0x00000 };
static unsigned int cf00506[] = { 0x00507, 0x00000 };
static unsigned int cf00508[] = { 0x00509, 0x00000 };
static unsigned int cf0050A[] = { 0x0050B, 0x00000 };
static unsigned int cf0050C[] = { 0x0050D, 0x00000 };
static unsigned int cf0050E[] = { 0x0050F, 0x00000 };
static unsigned int cf00531[] = { 0x00561, 0x00000 };
static unsigned int cf00532[] = { 0x00562, 0x00000 };
static unsigned int cf00533[] = { 0x00563, 0x00000 };
static unsigned int cf00534[] = { 0x00564, 0x00000 };
static unsigned int cf00535[] = { 0x00565, 0x00000 };
static unsigned int cf00536[] = { 0x00566, 0x00000 };
static unsigned int cf00537[] = { 0x00567, 0x00000 };
static unsigned int cf00538[] = { 0x00568, 0x00000 };
static unsigned int cf00539[] = { 0x00569, 0x00000 };
static unsigned int cf0053A[] = { 0x0056A, 0x00000 };
static unsigned int cf0053B[] = { 0x0056B, 0x00000 };
static unsigned int cf0053C[] = { 0x0056C, 0x00000 };
static unsigned int cf0053D[] = { 0x0056D, 0x00000 };
static unsigned int cf0053E[] = { 0x0056E, 0x00000 };
static unsigned int cf0053F[] = { 0x0056F, 0x00000 };
static unsigned int cf00540[] = { 0x00570, 0x00000 };
static unsigned int cf00541[] = { 0x00571, 0x00000 };
static unsigned int cf00542[] = { 0x00572, 0x00000 };
static unsigned int cf00543[] = { 0x00573, 0x00000 };
static unsigned int cf00544[] = { 0x00574, 0x00000 };
static unsigned int cf00545[] = { 0x00575, 0x00000 };
static unsigned int cf00546[] = { 0x00576, 0x00000 };
static unsigned int cf00547[] = { 0x00577, 0x00000 };
static unsigned int cf00548[] = { 0x00578, 0x00000 };
static unsigned int cf00549[] = { 0x00579, 0x00000 };
static unsigned int cf0054A[] = { 0x0057A, 0x00000 };
static unsigned int cf0054B[] = { 0x0057B, 0x00000 };
static unsigned int cf0054C[] = { 0x0057C, 0x00000 };
static unsigned int cf0054D[] = { 0x0057D, 0x00000 };
static unsigned int cf0054E[] = { 0x0057E, 0x00000 };
static unsigned int cf0054F[] = { 0x0057F, 0x00000 };
static unsigned int cf00550[] = { 0x00580, 0x00000 };
static unsigned int cf00551[] = { 0x00581, 0x00000 };
static unsigned int cf00552[] = { 0x00582, 0x00000 };
static unsigned int cf00553[] = { 0x00583, 0x00000 };
static unsigned int cf00554[] = { 0x00584, 0x00000 };
static unsigned int cf00555[] = { 0x00585, 0x00000 };
static unsigned int cf00556[] = { 0x00586, 0x00000 };
static unsigned int cf00587[] = { 0x00565, 0x00582, 0x00000 };
static unsigned int cf010A0[] = { 0x02D00, 0x00000 };
static unsigned int cf010A1[] = { 0x02D01, 0x00000 };
static unsigned int cf010A2[] = { 0x02D02, 0x00000 };
static unsigned int cf010A3[] = { 0x02D03, 0x00000 };
static unsigned int cf010A4[] = { 0x02D04, 0x00000 };
static unsigned int cf010A5[] = { 0x02D05, 0x00000 };
static unsigned int cf010A6[] = { 0x02D06, 0x00000 };
static unsigned int cf010A7[] = { 0x02D07, 0x00000 };
static unsigned int cf010A8[] = { 0x02D08, 0x00000 };
static unsigned int cf010A9[] = { 0x02D09, 0x00000 };
static unsigned int cf010AA[] = { 0x02D0A, 0x00000 };
static unsigned int cf010AB[] = { 0x02D0B, 0x00000 };
static unsigned int cf010AC[] = { 0x02D0C, 0x00000 };
static unsigned int cf010AD[] = { 0x02D0D, 0x00000 };
static unsigned int cf010AE[] = { 0x02D0E, 0x00000 };
static unsigned int cf010AF[] = { 0x02D0F, 0x00000 };
static unsigned int cf010B0[] = { 0x02D10, 0x00000 };
static unsigned int cf010B1[] = { 0x02D11, 0x00000 };
static unsigned int cf010B2[] = { 0x02D12, 0x00000 };
static unsigned int cf010B3[] = { 0x02D13, 0x00000 };
static unsigned int cf010B4[] = { 0x02D14, 0x00000 };
static unsigned int cf010B5[] = { 0x02D15, 0x00000 };
static unsigned int cf010B6[] = { 0x02D16, 0x00000 };
static unsigned int cf010B7[] = { 0x02D17, 0x00000 };
static unsigned int cf010B8[] = { 0x02D18, 0x00000 };
static unsigned int cf010B9[] = { 0x02D19, 0x00000 };
static unsigned int cf010BA[] = { 0x02D1A, 0x00000 };
static unsigned int cf010BB[] = { 0x02D1B, 0x00000 };
static unsigned int cf010BC[] = { 0x02D1C, 0x00000 };
static unsigned int cf010BD[] = { 0x02D1D, 0x00000 };
static unsigned int cf010BE[] = { 0x02D1E, 0x00000 };
static unsigned int cf010BF[] = { 0x02D1F, 0x00000 };
static unsigned int cf010C0[] = { 0x02D20, 0x00000 };
static unsigned int cf010C1[] = { 0x02D21, 0x00000 };
static unsigned int cf010C2[] = { 0x02D22, 0x00000 };
static unsigned int cf010C3[] = { 0x02D23, 0x00000 };
static unsigned int cf010C4[] = { 0x02D24, 0x00000 };
static unsigned int cf010C5[] = { 0x02D25, 0x00000 };
static unsigned int cf01E00[] = { 0x01E01, 0x00000 };
static unsigned int cf01E02[] = { 0x01E03, 0x00000 };
static unsigned int cf01E04[] = { 0x01E05, 0x00000 };
static unsigned int cf01E06[] = { 0x01E07, 0x00000 };
static unsigned int cf01E08[] = { 0x01E09, 0x00000 };
static unsigned int cf01E0A[] = { 0x01E0B, 0x00000 };
static unsigned int cf01E0C[] = { 0x01E0D, 0x00000 };
static unsigned int cf01E0E[] = { 0x01E0F, 0x00000 };
static unsigned int cf01E10[] = { 0x01E11, 0x00000 };
static unsigned int cf01E12[] = { 0x01E13, 0x00000 };
static unsigned int cf01E14[] = { 0x01E15, 0x00000 };
static unsigned int cf01E16[] = { 0x01E17, 0x00000 };
static unsigned int cf01E18[] = { 0x01E19, 0x00000 };
static unsigned int cf01E1A[] = { 0x01E1B, 0x00000 };
static unsigned int cf01E1C[] = { 0x01E1D, 0x00000 };
static unsigned int cf01E1E[] = { 0x01E1F, 0x00000 };
static unsigned int cf01E20[] = { 0x01E21, 0x00000 };
static unsigned int cf01E22[] = { 0x01E23, 0x00000 };
static unsigned int cf01E24[] = { 0x01E25, 0x00000 };
static unsigned int cf01E26[] = { 0x01E27, 0x00000 };
static unsigned int cf01E28[] = { 0x01E29, 0x00000 };
static unsigned int cf01E2A[] = { 0x01E2B, 0x00000 };
static unsigned int cf01E2C[] = { 0x01E2D, 0x00000 };
static unsigned int cf01E2E[] = { 0x01E2F, 0x00000 };
static unsigned int cf01E30[] = { 0x01E31, 0x00000 };
static unsigned int cf01E32[] = { 0x01E33, 0x00000 };
static unsigned int cf01E34[] = { 0x01E35, 0x00000 };
static unsigned int cf01E36[] = { 0x01E37, 0x00000 };
static unsigned int cf01E38[] = { 0x01E39, 0x00000 };
static unsigned int cf01E3A[] = { 0x01E3B, 0x00000 };
static unsigned int cf01E3C[] = { 0x01E3D, 0x00000 };
static unsigned int cf01E3E[] = { 0x01E3F, 0x00000 };
static unsigned int cf01E40[] = { 0x01E41, 0x00000 };
static unsigned int cf01E42[] = { 0x01E43, 0x00000 };
static unsigned int cf01E44[] = { 0x01E45, 0x00000 };
static unsigned int cf01E46[] = { 0x01E47, 0x00000 };
static unsigned int cf01E48[] = { 0x01E49, 0x00000 };
static unsigned int cf01E4A[] = { 0x01E4B, 0x00000 };
static unsigned int cf01E4C[] = { 0x01E4D, 0x00000 };
static unsigned int cf01E4E[] = { 0x01E4F, 0x00000 };
static unsigned int cf01E50[] = { 0x01E51, 0x00000 };
static unsigned int cf01E52[] = { 0x01E53, 0x00000 };
static unsigned int cf01E54[] = { 0x01E55, 0x00000 };
static unsigned int cf01E56[] = { 0x01E57, 0x00000 };
static unsigned int cf01E58[] = { 0x01E59, 0x00000 };
static unsigned int cf01E5A[] = { 0x01E5B, 0x00000 };
static unsigned int cf01E5C[] = { 0x01E5D, 0x00000 };
static unsigned int cf01E5E[] = { 0x01E5F, 0x00000 };
static unsigned int cf01E60[] = { 0x01E61, 0x00000 };
static unsigned int cf01E62[] = { 0x01E63, 0x00000 };
static unsigned int cf01E64[] = { 0x01E65, 0x00000 };
static unsigned int cf01E66[] = { 0x01E67, 0x00000 };
static unsigned int cf01E68[] = { 0x01E69, 0x00000 };
static unsigned int cf01E6A[] = { 0x01E6B, 0x00000 };
static unsigned int cf01E6C[] = { 0x01E6D, 0x00000 };
static unsigned int cf01E6E[] = { 0x01E6F, 0x00000 };
static unsigned int cf01E70[] = { 0x01E71, 0x00000 };
static unsigned int cf01E72[] = { 0x01E73, 0x00000 };
static unsigned int cf01E74[] = { 0x01E75, 0x00000 };
static unsigned int cf01E76[] = { 0x01E77, 0x00000 };
static unsigned int cf01E78[] = { 0x01E79, 0x00000 };
static unsigned int cf01E7A[] = { 0x01E7B, 0x00000 };
static unsigned int cf01E7C[] = { 0x01E7D, 0x00000 };
static unsigned int cf01E7E[] = { 0x01E7F, 0x00000 };
static unsigned int cf01E80[] = { 0x01E81, 0x00000 };
static unsigned int cf01E82[] = { 0x01E83, 0x00000 };
static unsigned int cf01E84[] = { 0x01E85, 0x00000 };
static unsigned int cf01E86[] = { 0x01E87, 0x00000 };
static unsigned int cf01E88[] = { 0x01E89, 0x00000 };
static unsigned int cf01E8A[] = { 0x01E8B, 0x00000 };
static unsigned int cf01E8C[] = { 0x01E8D, 0x00000 };
static unsigned int cf01E8E[] = { 0x01E8F, 0x00000 };
static unsigned int cf01E90[] = { 0x01E91, 0x00000 };
static unsigned int cf01E92[] = { 0x01E93, 0x00000 };
static unsigned int cf01E94[] = { 0x01E95, 0x00000 };
static unsigned int cf01E96[] = { 0x00068, 0x00331, 0x00000 };
static unsigned int cf01E97[] = { 0x00074, 0x00308, 0x00000 };
static unsigned int cf01E98[] = { 0x00077, 0x0030A, 0x00000 };
static unsigned int cf01E99[] = { 0x00079, 0x0030A, 0x00000 };
static unsigned int cf01E9A[] = { 0x00061, 0x002BE, 0x00000 };
static unsigned int cf01E9B[] = { 0x01E61, 0x00000 };
static unsigned int cf01EA0[] = { 0x01EA1, 0x00000 };
static unsigned int cf01EA2[] = { 0x01EA3, 0x00000 };
static unsigned int cf01EA4[] = { 0x01EA5, 0x00000 };
static unsigned int cf01EA6[] = { 0x01EA7, 0x00000 };
static unsigned int cf01EA8[] = { 0x01EA9, 0x00000 };
static unsigned int cf01EAA[] = { 0x01EAB, 0x00000 };
static unsigned int cf01EAC[] = { 0x01EAD, 0x00000 };
static unsigned int cf01EAE[] = { 0x01EAF, 0x00000 };
static unsigned int cf01EB0[] = { 0x01EB1, 0x00000 };
static unsigned int cf01EB2[] = { 0x01EB3, 0x00000 };
static unsigned int cf01EB4[] = { 0x01EB5, 0x00000 };
static unsigned int cf01EB6[] = { 0x01EB7, 0x00000 };
static unsigned int cf01EB8[] = { 0x01EB9, 0x00000 };
static unsigned int cf01EBA[] = { 0x01EBB, 0x00000 };
static unsigned int cf01EBC[] = { 0x01EBD, 0x00000 };
static unsigned int cf01EBE[] = { 0x01EBF, 0x00000 };
static unsigned int cf01EC0[] = { 0x01EC1, 0x00000 };
static unsigned int cf01EC2[] = { 0x01EC3, 0x00000 };
static unsigned int cf01EC4[] = { 0x01EC5, 0x00000 };
static unsigned int cf01EC6[] = { 0x01EC7, 0x00000 };
static unsigned int cf01EC8[] = { 0x01EC9, 0x00000 };
static unsigned int cf01ECA[] = { 0x01ECB, 0x00000 };
static unsigned int cf01ECC[] = { 0x01ECD, 0x00000 };
static unsigned int cf01ECE[] = { 0x01ECF, 0x00000 };
static unsigned int cf01ED0[] = { 0x01ED1, 0x00000 };
static unsigned int cf01ED2[] = { 0x01ED3, 0x00000 };
static unsigned int cf01ED4[] = { 0x01ED5, 0x00000 };
static unsigned int cf01ED6[] = { 0x01ED7, 0x00000 };
static unsigned int cf01ED8[] = { 0x01ED9, 0x00000 };
static unsigned int cf01EDA[] = { 0x01EDB, 0x00000 };
static unsigned int cf01EDC[] = { 0x01EDD, 0x00000 };
static unsigned int cf01EDE[] = { 0x01EDF, 0x00000 };
static unsigned int cf01EE0[] = { 0x01EE1, 0x00000 };
static unsigned int cf01EE2[] = { 0x01EE3, 0x00000 };
static unsigned int cf01EE4[] = { 0x01EE5, 0x00000 };
static unsigned int cf01EE6[] = { 0x01EE7, 0x00000 };
static unsigned int cf01EE8[] = { 0x01EE9, 0x00000 };
static unsigned int cf01EEA[] = { 0x01EEB, 0x00000 };
static unsigned int cf01EEC[] = { 0x01EED, 0x00000 };
static unsigned int cf01EEE[] = { 0x01EEF, 0x00000 };
static unsigned int cf01EF0[] = { 0x01EF1, 0x00000 };
static unsigned int cf01EF2[] = { 0x01EF3, 0x00000 };
static unsigned int cf01EF4[] = { 0x01EF5, 0x00000 };
static unsigned int cf01EF6[] = { 0x01EF7, 0x00000 };
static unsigned int cf01EF8[] = { 0x01EF9, 0x00000 };
static unsigned int cf01F08[] = { 0x01F00, 0x00000 };
static unsigned int cf01F09[] = { 0x01F01, 0x00000 };
static unsigned int cf01F0A[] = { 0x01F02, 0x00000 };
static unsigned int cf01F0B[] = { 0x01F03, 0x00000 };
static unsigned int cf01F0C[] = { 0x01F04, 0x00000 };
static unsigned int cf01F0D[] = { 0x01F05, 0x00000 };
static unsigned int cf01F0E[] = { 0x01F06, 0x00000 };
static unsigned int cf01F0F[] = { 0x01F07, 0x00000 };
static unsigned int cf01F18[] = { 0x01F10, 0x00000 };
static unsigned int cf01F19[] = { 0x01F11, 0x00000 };
static unsigned int cf01F1A[] = { 0x01F12, 0x00000 };
static unsigned int cf01F1B[] = { 0x01F13, 0x00000 };
static unsigned int cf01F1C[] = { 0x01F14, 0x00000 };
static unsigned int cf01F1D[] = { 0x01F15, 0x00000 };
static unsigned int cf01F28[] = { 0x01F20, 0x00000 };
static unsigned int cf01F29[] = { 0x01F21, 0x00000 };
static unsigned int cf01F2A[] = { 0x01F22, 0x00000 };
static unsigned int cf01F2B[] = { 0x01F23, 0x00000 };
static unsigned int cf01F2C[] = { 0x01F24, 0x00000 };
static unsigned int cf01F2D[] = { 0x01F25, 0x00000 };
static unsigned int cf01F2E[] = { 0x01F26, 0x00000 };
static unsigned int cf01F2F[] = { 0x01F27, 0x00000 };
static unsigned int cf01F38[] = { 0x01F30, 0x00000 };
static unsigned int cf01F39[] = { 0x01F31, 0x00000 };
static unsigned int cf01F3A[] = { 0x01F32, 0x00000 };
static unsigned int cf01F3B[] = { 0x01F33, 0x00000 };
static unsigned int cf01F3C[] = { 0x01F34, 0x00000 };
static unsigned int cf01F3D[] = { 0x01F35, 0x00000 };
static unsigned int cf01F3E[] = { 0x01F36, 0x00000 };
static unsigned int cf01F3F[] = { 0x01F37, 0x00000 };
static unsigned int cf01F48[] = { 0x01F40, 0x00000 };
static unsigned int cf01F49[] = { 0x01F41, 0x00000 };
static unsigned int cf01F4A[] = { 0x01F42, 0x00000 };
static unsigned int cf01F4B[] = { 0x01F43, 0x00000 };
static unsigned int cf01F4C[] = { 0x01F44, 0x00000 };
static unsigned int cf01F4D[] = { 0x01F45, 0x00000 };
static unsigned int cf01F50[] = { 0x003C5, 0x00313, 0x00000 };
static unsigned int cf01F52[] = { 0x003C5, 0x00313, 0x00300, 0x00000 };
static unsigned int cf01F54[] = { 0x003C5, 0x00313, 0x00301, 0x00000 };
static unsigned int cf01F56[] = { 0x003C5, 0x00313, 0x00342, 0x00000 };
static unsigned int cf01F59[] = { 0x01F51, 0x00000 };
static unsigned int cf01F5B[] = { 0x01F53, 0x00000 };
static unsigned int cf01F5D[] = { 0x01F55, 0x00000 };
static unsigned int cf01F5F[] = { 0x01F57, 0x00000 };
static unsigned int cf01F68[] = { 0x01F60, 0x00000 };
static unsigned int cf01F69[] = { 0x01F61, 0x00000 };
static unsigned int cf01F6A[] = { 0x01F62, 0x00000 };
static unsigned int cf01F6B[] = { 0x01F63, 0x00000 };
static unsigned int cf01F6C[] = { 0x01F64, 0x00000 };
static unsigned int cf01F6D[] = { 0x01F65, 0x00000 };
static unsigned int cf01F6E[] = { 0x01F66, 0x00000 };
static unsigned int cf01F6F[] = { 0x01F67, 0x00000 };
static unsigned int cf01F80[] = { 0x01F00, 0x003B9, 0x00000 };
static unsigned int cf01F81[] = { 0x01F01, 0x003B9, 0x00000 };
static unsigned int cf01F82[] = { 0x01F02, 0x003B9, 0x00000 };
static unsigned int cf01F83[] = { 0x01F03, 0x003B9, 0x00000 };
static unsigned int cf01F84[] = { 0x01F04, 0x003B9, 0x00000 };
static unsigned int cf01F85[] = { 0x01F05, 0x003B9, 0x00000 };
static unsigned int cf01F86[] = { 0x01F06, 0x003B9, 0x00000 };
static unsigned int cf01F87[] = { 0x01F07, 0x003B9, 0x00000 };
static unsigned int cf01F88[] = { 0x01F00, 0x003B9, 0x00000 };
static unsigned int cf01F89[] = { 0x01F01, 0x003B9, 0x00000 };
static unsigned int cf01F8A[] = { 0x01F02, 0x003B9, 0x00000 };
static unsigned int cf01F8B[] = { 0x01F03, 0x003B9, 0x00000 };
static unsigned int cf01F8C[] = { 0x01F04, 0x003B9, 0x00000 };
static unsigned int cf01F8D[] = { 0x01F05, 0x003B9, 0x00000 };
static unsigned int cf01F8E[] = { 0x01F06, 0x003B9, 0x00000 };
static unsigned int cf01F8F[] = { 0x01F07, 0x003B9, 0x00000 };
static unsigned int cf01F90[] = { 0x01F20, 0x003B9, 0x00000 };
static unsigned int cf01F91[] = { 0x01F21, 0x003B9, 0x00000 };
static unsigned int cf01F92[] = { 0x01F22, 0x003B9, 0x00000 };
static unsigned int cf01F93[] = { 0x01F23, 0x003B9, 0x00000 };
static unsigned int cf01F94[] = { 0x01F24, 0x003B9, 0x00000 };
static unsigned int cf01F95[] = { 0x01F25, 0x003B9, 0x00000 };
static unsigned int cf01F96[] = { 0x01F26, 0x003B9, 0x00000 };
static unsigned int cf01F97[] = { 0x01F27, 0x003B9, 0x00000 };
static unsigned int cf01F98[] = { 0x01F20, 0x003B9, 0x00000 };
static unsigned int cf01F99[] = { 0x01F21, 0x003B9, 0x00000 };
static unsigned int cf01F9A[] = { 0x01F22, 0x003B9, 0x00000 };
static unsigned int cf01F9B[] = { 0x01F23, 0x003B9, 0x00000 };
static unsigned int cf01F9C[] = { 0x01F24, 0x003B9, 0x00000 };
static unsigned int cf01F9D[] = { 0x01F25, 0x003B9, 0x00000 };
static unsigned int cf01F9E[] = { 0x01F26, 0x003B9, 0x00000 };
static unsigned int cf01F9F[] = { 0x01F27, 0x003B9, 0x00000 };
static unsigned int cf01FA0[] = { 0x01F60, 0x003B9, 0x00000 };
static unsigned int cf01FA1[] = { 0x01F61, 0x003B9, 0x00000 };
static unsigned int cf01FA2[] = { 0x01F62, 0x003B9, 0x00000 };
static unsigned int cf01FA3[] = { 0x01F63, 0x003B9, 0x00000 };
static unsigned int cf01FA4[] = { 0x01F64, 0x003B9, 0x00000 };
static unsigned int cf01FA5[] = { 0x01F65, 0x003B9, 0x00000 };
static unsigned int cf01FA6[] = { 0x01F66, 0x003B9, 0x00000 };
static unsigned int cf01FA7[] = { 0x01F67, 0x003B9, 0x00000 };
static unsigned int cf01FA8[] = { 0x01F60, 0x003B9, 0x00000 };
static unsigned int cf01FA9[] = { 0x01F61, 0x003B9, 0x00000 };
static unsigned int cf01FAA[] = { 0x01F62, 0x003B9, 0x00000 };
static unsigned int cf01FAB[] = { 0x01F63, 0x003B9, 0x00000 };
static unsigned int cf01FAC[] = { 0x01F64, 0x003B9, 0x00000 };
static unsigned int cf01FAD[] = { 0x01F65, 0x003B9, 0x00000 };
static unsigned int cf01FAE[] = { 0x01F66, 0x003B9, 0x00000 };
static unsigned int cf01FAF[] = { 0x01F67, 0x003B9, 0x00000 };
static unsigned int cf01FB2[] = { 0x01F70, 0x003B9, 0x00000 };
static unsigned int cf01FB3[] = { 0x003B1, 0x003B9, 0x00000 };
static unsigned int cf01FB4[] = { 0x003AC, 0x003B9, 0x00000 };
static unsigned int cf01FB6[] = { 0x003B1, 0x00342, 0x00000 };
static unsigned int cf01FB7[] = { 0x003B1, 0x00342, 0x003B9, 0x00000 };
static unsigned int cf01FB8[] = { 0x01FB0, 0x00000 };
static unsigned int cf01FB9[] = { 0x01FB1, 0x00000 };
static unsigned int cf01FBA[] = { 0x01F70, 0x00000 };
static unsigned int cf01FBB[] = { 0x01F71, 0x00000 };
static unsigned int cf01FBC[] = { 0x003B1, 0x003B9, 0x00000 };
static unsigned int cf01FBE[] = { 0x003B9, 0x00000 };
static unsigned int cf01FC2[] = { 0x01F74, 0x003B9, 0x00000 };
static unsigned int cf01FC3[] = { 0x003B7, 0x003B9, 0x00000 };
static unsigned int cf01FC4[] = { 0x003AE, 0x003B9, 0x00000 };
static unsigned int cf01FC6[] = { 0x003B7, 0x00342, 0x00000 };
static unsigned int cf01FC7[] = { 0x003B7, 0x00342, 0x003B9, 0x00000 };
static unsigned int cf01FC8[] = { 0x01F72, 0x00000 };
static unsigned int cf01FC9[] = { 0x01F73, 0x00000 };
static unsigned int cf01FCA[] = { 0x01F74, 0x00000 };
static unsigned int cf01FCB[] = { 0x01F75, 0x00000 };
static unsigned int cf01FCC[] = { 0x003B7, 0x003B9, 0x00000 };
static unsigned int cf01FD2[] = { 0x003B9, 0x00308, 0x00300, 0x00000 };
static unsigned int cf01FD3[] = { 0x003B9, 0x00308, 0x00301, 0x00000 };
static unsigned int cf01FD6[] = { 0x003B9, 0x00342, 0x00000 };
static unsigned int cf01FD7[] = { 0x003B9, 0x00308, 0x00342, 0x00000 };
static unsigned int cf01FD8[] = { 0x01FD0, 0x00000 };
static unsigned int cf01FD9[] = { 0x01FD1, 0x00000 };
static unsigned int cf01FDA[] = { 0x01F76, 0x00000 };
static unsigned int cf01FDB[] = { 0x01F77, 0x00000 };
static unsigned int cf01FE2[] = { 0x003C5, 0x00308, 0x00300, 0x00000 };
static unsigned int cf01FE3[] = { 0x003C5, 0x00308, 0x00301, 0x00000 };
static unsigned int cf01FE4[] = { 0x003C1, 0x00313, 0x00000 };
static unsigned int cf01FE6[] = { 0x003C5, 0x00342, 0x00000 };
static unsigned int cf01FE7[] = { 0x003C5, 0x00308, 0x00342, 0x00000 };
static unsigned int cf01FE8[] = { 0x01FE0, 0x00000 };
static unsigned int cf01FE9[] = { 0x01FE1, 0x00000 };
static unsigned int cf01FEA[] = { 0x01F7A, 0x00000 };
static unsigned int cf01FEB[] = { 0x01F7B, 0x00000 };
static unsigned int cf01FEC[] = { 0x01FE5, 0x00000 };
static unsigned int cf01FF2[] = { 0x01F7C, 0x003B9, 0x00000 };
static unsigned int cf01FF3[] = { 0x003C9, 0x003B9, 0x00000 };
static unsigned int cf01FF4[] = { 0x003CE, 0x003B9, 0x00000 };
static unsigned int cf01FF6[] = { 0x003C9, 0x00342, 0x00000 };
static unsigned int cf01FF7[] = { 0x003C9, 0x00342, 0x003B9, 0x00000 };
static unsigned int cf01FF8[] = { 0x01F78, 0x00000 };
static unsigned int cf01FF9[] = { 0x01F79, 0x00000 };
static unsigned int cf01FFA[] = { 0x01F7C, 0x00000 };
static unsigned int cf01FFB[] = { 0x01F7D, 0x00000 };
static unsigned int cf01FFC[] = { 0x003C9, 0x003B9, 0x00000 };
static unsigned int cf02126[] = { 0x003C9, 0x00000 };
static unsigned int cf0212A[] = { 0x0006B, 0x00000 };
static unsigned int cf0212B[] = { 0x000E5, 0x00000 };
static unsigned int cf02160[] = { 0x02170, 0x00000 };
static unsigned int cf02161[] = { 0x02171, 0x00000 };
static unsigned int cf02162[] = { 0x02172, 0x00000 };
static unsigned int cf02163[] = { 0x02173, 0x00000 };
static unsigned int cf02164[] = { 0x02174, 0x00000 };
static unsigned int cf02165[] = { 0x02175, 0x00000 };
static unsigned int cf02166[] = { 0x02176, 0x00000 };
static unsigned int cf02167[] = { 0x02177, 0x00000 };
static unsigned int cf02168[] = { 0x02178, 0x00000 };
static unsigned int cf02169[] = { 0x02179, 0x00000 };
static unsigned int cf0216A[] = { 0x0217A, 0x00000 };
static unsigned int cf0216B[] = { 0x0217B, 0x00000 };
static unsigned int cf0216C[] = { 0x0217C, 0x00000 };
static unsigned int cf0216D[] = { 0x0217D, 0x00000 };
static unsigned int cf0216E[] = { 0x0217E, 0x00000 };
static unsigned int cf0216F[] = { 0x0217F, 0x00000 };
static unsigned int cf024B6[] = { 0x024D0, 0x00000 };
static unsigned int cf024B7[] = { 0x024D1, 0x00000 };
static unsigned int cf024B8[] = { 0x024D2, 0x00000 };
static unsigned int cf024B9[] = { 0x024D3, 0x00000 };
static unsigned int cf024BA[] = { 0x024D4, 0x00000 };
static unsigned int cf024BB[] = { 0x024D5, 0x00000 };
static unsigned int cf024BC[] = { 0x024D6, 0x00000 };
static unsigned int cf024BD[] = { 0x024D7, 0x00000 };
static unsigned int cf024BE[] = { 0x024D8, 0x00000 };
static unsigned int cf024BF[] = { 0x024D9, 0x00000 };
static unsigned int cf024C0[] = { 0x024DA, 0x00000 };
static unsigned int cf024C1[] = { 0x024DB, 0x00000 };
static unsigned int cf024C2[] = { 0x024DC, 0x00000 };
static unsigned int cf024C3[] = { 0x024DD, 0x00000 };
static unsigned int cf024C4[] = { 0x024DE, 0x00000 };
static unsigned int cf024C5[] = { 0x024DF, 0x00000 };
static unsigned int cf024C6[] = { 0x024E0, 0x00000 };
static unsigned int cf024C7[] = { 0x024E1, 0x00000 };
static unsigned int cf024C8[] = { 0x024E2, 0x00000 };
static unsigned int cf024C9[] = { 0x024E3, 0x00000 };
static unsigned int cf024CA[] = { 0x024E4, 0x00000 };
static unsigned int cf024CB[] = { 0x024E5, 0x00000 };
static unsigned int cf024CC[] = { 0x024E6, 0x00000 };
static unsigned int cf024CD[] = { 0x024E7, 0x00000 };
static unsigned int cf024CE[] = { 0x024E8, 0x00000 };
static unsigned int cf024CF[] = { 0x024E9, 0x00000 };
static unsigned int cf02C00[] = { 0x02C30, 0x00000 };
static unsigned int cf02C01[] = { 0x02C31, 0x00000 };
static unsigned int cf02C02[] = { 0x02C32, 0x00000 };
static unsigned int cf02C03[] = { 0x02C33, 0x00000 };
static unsigned int cf02C04[] = { 0x02C34, 0x00000 };
static unsigned int cf02C05[] = { 0x02C35, 0x00000 };
static unsigned int cf02C06[] = { 0x02C36, 0x00000 };
static unsigned int cf02C07[] = { 0x02C37, 0x00000 };
static unsigned int cf02C08[] = { 0x02C38, 0x00000 };
static unsigned int cf02C09[] = { 0x02C39, 0x00000 };
static unsigned int cf02C0A[] = { 0x02C3A, 0x00000 };
static unsigned int cf02C0B[] = { 0x02C3B, 0x00000 };
static unsigned int cf02C0C[] = { 0x02C3C, 0x00000 };
static unsigned int cf02C0D[] = { 0x02C3D, 0x00000 };
static unsigned int cf02C0E[] = { 0x02C3E, 0x00000 };
static unsigned int cf02C0F[] = { 0x02C3F, 0x00000 };
static unsigned int cf02C10[] = { 0x02C40, 0x00000 };
static unsigned int cf02C11[] = { 0x02C41, 0x00000 };
static unsigned int cf02C12[] = { 0x02C42, 0x00000 };
static unsigned int cf02C13[] = { 0x02C43, 0x00000 };
static unsigned int cf02C14[] = { 0x02C44, 0x00000 };
static unsigned int cf02C15[] = { 0x02C45, 0x00000 };
static unsigned int cf02C16[] = { 0x02C46, 0x00000 };
static unsigned int cf02C17[] = { 0x02C47, 0x00000 };
static unsigned int cf02C18[] = { 0x02C48, 0x00000 };
static unsigned int cf02C19[] = { 0x02C49, 0x00000 };
static unsigned int cf02C1A[] = { 0x02C4A, 0x00000 };
static unsigned int cf02C1B[] = { 0x02C4B, 0x00000 };
static unsigned int cf02C1C[] = { 0x02C4C, 0x00000 };
static unsigned int cf02C1D[] = { 0x02C4D, 0x00000 };
static unsigned int cf02C1E[] = { 0x02C4E, 0x00000 };
static unsigned int cf02C1F[] = { 0x02C4F, 0x00000 };
static unsigned int cf02C20[] = { 0x02C50, 0x00000 };
static unsigned int cf02C21[] = { 0x02C51, 0x00000 };
static unsigned int cf02C22[] = { 0x02C52, 0x00000 };
static unsigned int cf02C23[] = { 0x02C53, 0x00000 };
static unsigned int cf02C24[] = { 0x02C54, 0x00000 };
static unsigned int cf02C25[] = { 0x02C55, 0x00000 };
static unsigned int cf02C26[] = { 0x02C56, 0x00000 };
static unsigned int cf02C27[] = { 0x02C57, 0x00000 };
static unsigned int cf02C28[] = { 0x02C58, 0x00000 };
static unsigned int cf02C29[] = { 0x02C59, 0x00000 };
static unsigned int cf02C2A[] = { 0x02C5A, 0x00000 };
static unsigned int cf02C2B[] = { 0x02C5B, 0x00000 };
static unsigned int cf02C2C[] = { 0x02C5C, 0x00000 };
static unsigned int cf02C2D[] = { 0x02C5D, 0x00000 };
static unsigned int cf02C2E[] = { 0x02C5E, 0x00000 };
static unsigned int cf02C80[] = { 0x02C81, 0x00000 };
static unsigned int cf02C82[] = { 0x02C83, 0x00000 };
static unsigned int cf02C84[] = { 0x02C85, 0x00000 };
static unsigned int cf02C86[] = { 0x02C87, 0x00000 };
static unsigned int cf02C88[] = { 0x02C89, 0x00000 };
static unsigned int cf02C8A[] = { 0x02C8B, 0x00000 };
static unsigned int cf02C8C[] = { 0x02C8D, 0x00000 };
static unsigned int cf02C8E[] = { 0x02C8F, 0x00000 };
static unsigned int cf02C90[] = { 0x02C91, 0x00000 };
static unsigned int cf02C92[] = { 0x02C93, 0x00000 };
static unsigned int cf02C94[] = { 0x02C95, 0x00000 };
static unsigned int cf02C96[] = { 0x02C97, 0x00000 };
static unsigned int cf02C98[] = { 0x02C99, 0x00000 };
static unsigned int cf02C9A[] = { 0x02C9B, 0x00000 };
static unsigned int cf02C9C[] = { 0x02C9D, 0x00000 };
static unsigned int cf02C9E[] = { 0x02C9F, 0x00000 };
static unsigned int cf02CA0[] = { 0x02CA1, 0x00000 };
static unsigned int cf02CA2[] = { 0x02CA3, 0x00000 };
static unsigned int cf02CA4[] = { 0x02CA5, 0x00000 };
static unsigned int cf02CA6[] = { 0x02CA7, 0x00000 };
static unsigned int cf02CA8[] = { 0x02CA9, 0x00000 };
static unsigned int cf02CAA[] = { 0x02CAB, 0x00000 };
static unsigned int cf02CAC[] = { 0x02CAD, 0x00000 };
static unsigned int cf02CAE[] = { 0x02CAF, 0x00000 };
static unsigned int cf02CB0[] = { 0x02CB1, 0x00000 };
static unsigned int cf02CB2[] = { 0x02CB3, 0x00000 };
static unsigned int cf02CB4[] = { 0x02CB5, 0x00000 };
static unsigned int cf02CB6[] = { 0x02CB7, 0x00000 };
static unsigned int cf02CB8[] = { 0x02CB9, 0x00000 };
static unsigned int cf02CBA[] = { 0x02CBB, 0x00000 };
static unsigned int cf02CBC[] = { 0x02CBD, 0x00000 };
static unsigned int cf02CBE[] = { 0x02CBF, 0x00000 };
static unsigned int cf02CC0[] = { 0x02CC1, 0x00000 };
static unsigned int cf02CC2[] = { 0x02CC3, 0x00000 };
static unsigned int cf02CC4[] = { 0x02CC5, 0x00000 };
static unsigned int cf02CC6[] = { 0x02CC7, 0x00000 };
static unsigned int cf02CC8[] = { 0x02CC9, 0x00000 };
static unsigned int cf02CCA[] = { 0x02CCB, 0x00000 };
static unsigned int cf02CCC[] = { 0x02CCD, 0x00000 };
static unsigned int cf02CCE[] = { 0x02CCF, 0x00000 };
static unsigned int cf02CD0[] = { 0x02CD1, 0x00000 };
static unsigned int cf02CD2[] = { 0x02CD3, 0x00000 };
static unsigned int cf02CD4[] = { 0x02CD5, 0x00000 };
static unsigned int cf02CD6[] = { 0x02CD7, 0x00000 };
static unsigned int cf02CD8[] = { 0x02CD9, 0x00000 };
static unsigned int cf02CDA[] = { 0x02CDB, 0x00000 };
static unsigned int cf02CDC[] = { 0x02CDD, 0x00000 };
static unsigned int cf02CDE[] = { 0x02CDF, 0x00000 };
static unsigned int cf02CE0[] = { 0x02CE1, 0x00000 };
static unsigned int cf02CE2[] = { 0x02CE3, 0x00000 };
static unsigned int cf0FB00[] = { 0x00066, 0x00066, 0x00000 };
static unsigned int cf0FB01[] = { 0x00066, 0x00069, 0x00000 };
static unsigned int cf0FB02[] = { 0x00066, 0x0006C, 0x00000 };
static unsigned int cf0FB03[] = { 0x00066, 0x00066, 0x00069, 0x00000 };
static unsigned int cf0FB04[] = { 0x00066, 0x00066, 0x0006C, 0x00000 };
static unsigned int cf0FB05[] = { 0x00073, 0x00074, 0x00000 };
static unsigned int cf0FB06[] = { 0x00073, 0x00074, 0x00000 };
static unsigned int cf0FB13[] = { 0x00574, 0x00576, 0x00000 };
static unsigned int cf0FB14[] = { 0x00574, 0x00565, 0x00000 };
static unsigned int cf0FB15[] = { 0x00574, 0x0056B, 0x00000 };
static unsigned int cf0FB16[] = { 0x0057E, 0x00576, 0x00000 };
static unsigned int cf0FB17[] = { 0x00574, 0x0056D, 0x00000 };
static unsigned int cf0FF21[] = { 0x0FF41, 0x00000 };
static unsigned int cf0FF22[] = { 0x0FF42, 0x00000 };
static unsigned int cf0FF23[] = { 0x0FF43, 0x00000 };
static unsigned int cf0FF24[] = { 0x0FF44, 0x00000 };
static unsigned int cf0FF25[] = { 0x0FF45, 0x00000 };
static unsigned int cf0FF26[] = { 0x0FF46, 0x00000 };
static unsigned int cf0FF27[] = { 0x0FF47, 0x00000 };
static unsigned int cf0FF28[] = { 0x0FF48, 0x00000 };
static unsigned int cf0FF29[] = { 0x0FF49, 0x00000 };
static unsigned int cf0FF2A[] = { 0x0FF4A, 0x00000 };
static unsigned int cf0FF2B[] = { 0x0FF4B, 0x00000 };
static unsigned int cf0FF2C[] = { 0x0FF4C, 0x00000 };
static unsigned int cf0FF2D[] = { 0x0FF4D, 0x00000 };
static unsigned int cf0FF2E[] = { 0x0FF4E, 0x00000 };
static unsigned int cf0FF2F[] = { 0x0FF4F, 0x00000 };
static unsigned int cf0FF30[] = { 0x0FF50, 0x00000 };
static unsigned int cf0FF31[] = { 0x0FF51, 0x00000 };
static unsigned int cf0FF32[] = { 0x0FF52, 0x00000 };
static unsigned int cf0FF33[] = { 0x0FF53, 0x00000 };
static unsigned int cf0FF34[] = { 0x0FF54, 0x00000 };
static unsigned int cf0FF35[] = { 0x0FF55, 0x00000 };
static unsigned int cf0FF36[] = { 0x0FF56, 0x00000 };
static unsigned int cf0FF37[] = { 0x0FF57, 0x00000 };
static unsigned int cf0FF38[] = { 0x0FF58, 0x00000 };
static unsigned int cf0FF39[] = { 0x0FF59, 0x00000 };
static unsigned int cf0FF3A[] = { 0x0FF5A, 0x00000 };
static unsigned int cf10400[] = { 0x10428, 0x00000 };
static unsigned int cf10401[] = { 0x10429, 0x00000 };
static unsigned int cf10402[] = { 0x1042A, 0x00000 };
static unsigned int cf10403[] = { 0x1042B, 0x00000 };
static unsigned int cf10404[] = { 0x1042C, 0x00000 };
static unsigned int cf10405[] = { 0x1042D, 0x00000 };
static unsigned int cf10406[] = { 0x1042E, 0x00000 };
static unsigned int cf10407[] = { 0x1042F, 0x00000 };
static unsigned int cf10408[] = { 0x10430, 0x00000 };
static unsigned int cf10409[] = { 0x10431, 0x00000 };
static unsigned int cf1040A[] = { 0x10432, 0x00000 };
static unsigned int cf1040B[] = { 0x10433, 0x00000 };
static unsigned int cf1040C[] = { 0x10434, 0x00000 };
static unsigned int cf1040D[] = { 0x10435, 0x00000 };
static unsigned int cf1040E[] = { 0x10436, 0x00000 };
static unsigned int cf1040F[] = { 0x10437, 0x00000 };
static unsigned int cf10410[] = { 0x10438, 0x00000 };
static unsigned int cf10411[] = { 0x10439, 0x00000 };
static unsigned int cf10412[] = { 0x1043A, 0x00000 };
static unsigned int cf10413[] = { 0x1043B, 0x00000 };
static unsigned int cf10414[] = { 0x1043C, 0x00000 };
static unsigned int cf10415[] = { 0x1043D, 0x00000 };
static unsigned int cf10416[] = { 0x1043E, 0x00000 };
static unsigned int cf10417[] = { 0x1043F, 0x00000 };
static unsigned int cf10418[] = { 0x10440, 0x00000 };
static unsigned int cf10419[] = { 0x10441, 0x00000 };
static unsigned int cf1041A[] = { 0x10442, 0x00000 };
static unsigned int cf1041B[] = { 0x10443, 0x00000 };
static unsigned int cf1041C[] = { 0x10444, 0x00000 };
static unsigned int cf1041D[] = { 0x10445, 0x00000 };
static unsigned int cf1041E[] = { 0x10446, 0x00000 };
static unsigned int cf1041F[] = { 0x10447, 0x00000 };
static unsigned int cf10420[] = { 0x10448, 0x00000 };
static unsigned int cf10421[] = { 0x10449, 0x00000 };
static unsigned int cf10422[] = { 0x1044A, 0x00000 };
static unsigned int cf10423[] = { 0x1044B, 0x00000 };
static unsigned int cf10424[] = { 0x1044C, 0x00000 };
static unsigned int cf10425[] = { 0x1044D, 0x00000 };
static unsigned int cf10426[] = { 0x1044E, 0x00000 };
static unsigned int cf10427[] = { 0x1044F, 0x00000 };

static unsigned int caseFoldDirectTableSize = 2048;
static unsigned int *caseFoldDirectTable[2048] = {
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, cf00041, cf00042, cf00043, cf00044, cf00045, cf00046, cf00047, cf00048, cf00049, cf0004A, cf0004B, cf0004C, cf0004D, cf0004E, cf0004F,
  cf00050, cf00051, cf00052, cf00053, cf00054, cf00055, cf00056, cf00057, cf00058, cf00059, cf0005A,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0, cf000B5,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  cf000C0, cf000C1, cf000C2, cf000C3, cf000C4, cf000C5, cf000C6, cf000C7, cf000C8, cf000C9, cf000CA, cf000CB, cf000CC, cf000CD, cf000CE, cf000CF,
  cf000D0, cf000D1, cf000D2, cf000D3, cf000D4, cf000D5, cf000D6,       0, cf000D8, cf000D9, cf000DA, cf000DB, cf000DC, cf000DD, cf000DE, cf000DF,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  cf00100,       0, cf00102,       0, cf00104,       0, cf00106,       0, cf00108,       0, cf0010A,       0, cf0010C,       0, cf0010E,       0,
  cf00110,       0, cf00112,       0, cf00114,       0, cf00116,       0, cf00118,       0, cf0011A,       0, cf0011C,       0, cf0011E,       0,
  cf00120,       0, cf00122,       0, cf00124,       0, cf00126,       0, cf00128,       0, cf0012A,       0, cf0012C,       0, cf0012E,       0,
  cf00130,       0, cf00132,       0, cf00134,       0, cf00136,       0,       0, cf00139,       0, cf0013B,       0, cf0013D,       0, cf0013F,
        0, cf00141,       0, cf00143,       0, cf00145,       0, cf00147,       0, cf00149, cf0014A,       0, cf0014C,       0, cf0014E,       0,
  cf00150,       0, cf00152,       0, cf00154,       0, cf00156,       0, cf00158,       0, cf0015A,       0, cf0015C,       0, cf0015E,       0,
  cf00160,       0, cf00162,       0, cf00164,       0, cf00166,       0, cf00168,       0, cf0016A,       0, cf0016C,       0, cf0016E,       0,
  cf00170,       0, cf00172,       0, cf00174,       0, cf00176,       0, cf00178, cf00179,       0, cf0017B,       0, cf0017D,       0, cf0017F,
        0, cf00181, cf00182,       0, cf00184,       0, cf00186, cf00187,       0, cf00189, cf0018A, cf0018B,       0,       0, cf0018E, cf0018F,
  cf00190, cf00191,       0, cf00193, cf00194,       0, cf00196, cf00197, cf00198,       0,       0,       0, cf0019C, cf0019D,       0, cf0019F,
  cf001A0,       0, cf001A2,       0, cf001A4,       0, cf001A6, cf001A7,       0, cf001A9,       0,       0, cf001AC,       0, cf001AE, cf001AF,
        0, cf001B1, cf001B2, cf001B3,       0, cf001B5,       0, cf001B7, cf001B8,       0,       0,       0, cf001BC,       0,       0,       0,
        0,       0,       0,       0, cf001C4, cf001C5,       0, cf001C7, cf001C8,       0, cf001CA, cf001CB,       0, cf001CD,       0, cf001CF,
        0, cf001D1,       0, cf001D3,       0, cf001D5,       0, cf001D7,       0, cf001D9,       0, cf001DB,       0,       0, cf001DE,       0,
  cf001E0,       0, cf001E2,       0, cf001E4,       0, cf001E6,       0, cf001E8,       0, cf001EA,       0, cf001EC,       0, cf001EE,       0,
  cf001F0, cf001F1, cf001F2,       0, cf001F4,       0, cf001F6, cf001F7, cf001F8,       0, cf001FA,       0, cf001FC,       0, cf001FE,       0,
  cf00200,       0, cf00202,       0, cf00204,       0, cf00206,       0, cf00208,       0, cf0020A,       0, cf0020C,       0, cf0020E,       0,
  cf00210,       0, cf00212,       0, cf00214,       0, cf00216,       0, cf00218,       0, cf0021A,       0, cf0021C,       0, cf0021E,       0,
  cf00220,       0, cf00222,       0, cf00224,       0, cf00226,       0, cf00228,       0, cf0022A,       0, cf0022C,       0, cf0022E,       0,
  cf00230,       0, cf00232,       0,       0,       0,       0,       0,       0,       0,       0, cf0023B,       0, cf0023D,       0,       0,
        0, cf00241,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0, cf00345,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0, cf00386,       0, cf00388, cf00389, cf0038A,       0, cf0038C,       0, cf0038E, cf0038F,
  cf00390, cf00391, cf00392, cf00393, cf00394, cf00395, cf00396, cf00397, cf00398, cf00399, cf0039A, cf0039B, cf0039C, cf0039D, cf0039E, cf0039F,
  cf003A0, cf003A1,       0, cf003A3, cf003A4, cf003A5, cf003A6, cf003A7, cf003A8, cf003A9, cf003AA, cf003AB,       0,       0,       0,       0,
  cf003B0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0, cf003C2,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  cf003D0, cf003D1,       0,       0,       0, cf003D5, cf003D6,       0, cf003D8,       0, cf003DA,       0, cf003DC,       0, cf003DE,       0,
  cf003E0,       0, cf003E2,       0, cf003E4,       0, cf003E6,       0, cf003E8,       0, cf003EA,       0, cf003EC,       0, cf003EE,       0,
  cf003F0, cf003F1,       0,       0, cf003F4, cf003F5,       0, cf003F7,       0, cf003F9, cf003FA,       0,       0,       0,       0,       0,
  cf00400, cf00401, cf00402, cf00403, cf00404, cf00405, cf00406, cf00407, cf00408, cf00409, cf0040A, cf0040B, cf0040C, cf0040D, cf0040E, cf0040F,
  cf00410, cf00411, cf00412, cf00413, cf00414, cf00415, cf00416, cf00417, cf00418, cf00419, cf0041A, cf0041B, cf0041C, cf0041D, cf0041E, cf0041F,
  cf00420, cf00421, cf00422, cf00423, cf00424, cf00425, cf00426, cf00427, cf00428, cf00429, cf0042A, cf0042B, cf0042C, cf0042D, cf0042E, cf0042F,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  cf00460,       0, cf00462,       0, cf00464,       0, cf00466,       0, cf00468,       0, cf0046A,       0, cf0046C,       0, cf0046E,       0,
  cf00470,       0, cf00472,       0, cf00474,       0, cf00476,       0, cf00478,       0, cf0047A,       0, cf0047C,       0, cf0047E,       0,
  cf00480,       0,       0,       0,       0,       0,       0,       0,       0,       0, cf0048A,       0, cf0048C,       0, cf0048E,       0,
  cf00490,       0, cf00492,       0, cf00494,       0, cf00496,       0, cf00498,       0, cf0049A,       0, cf0049C,       0, cf0049E,       0,
  cf004A0,       0, cf004A2,       0, cf004A4,       0, cf004A6,       0, cf004A8,       0, cf004AA,       0, cf004AC,       0, cf004AE,       0,
  cf004B0,       0, cf004B2,       0, cf004B4,       0, cf004B6,       0, cf004B8,       0, cf004BA,       0, cf004BC,       0, cf004BE,       0,
        0, cf004C1,       0, cf004C3,       0, cf004C5,       0, cf004C7,       0, cf004C9,       0, cf004CB,       0, cf004CD,       0,       0,
  cf004D0,       0, cf004D2,       0, cf004D4,       0, cf004D6,       0, cf004D8,       0, cf004DA,       0, cf004DC,       0, cf004DE,       0,
  cf004E0,       0, cf004E2,       0, cf004E4,       0, cf004E6,       0, cf004E8,       0, cf004EA,       0, cf004EC,       0, cf004EE,       0,
  cf004F0,       0, cf004F2,       0, cf004F4,       0, cf004F6,       0, cf004F8,       0,       0,       0,       0,       0,       0,       0,
  cf00500,       0, cf00502,       0, cf00504,       0, cf00506,       0, cf00508,       0, cf0050A,       0, cf0050C,       0, cf0050E,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, cf00531, cf00532, cf00533, cf00534, cf00535, cf00536, cf00537, cf00538, cf00539, cf0053A, cf0053B, cf0053C, cf0053D, cf0053E, cf0053F,
  cf00540, cf00541, cf00542, cf00543, cf00544, cf00545, cf00546, cf00547, cf00548, cf00549, cf0054A, cf0054B, cf0054C, cf0054D, cf0054E, cf0054F,
  cf00550, cf00551, cf00552, cf00553, cf00554, cf00555, cf00556,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0, cf00587,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0
};

struct CaseFoldValue
{
  unsigned int ch;
  unsigned int *dest;
};
// static unsigned int caseFoldTableSize = 981;
// static CaseFoldValue caseFoldTable[981] = {
static unsigned int caseFoldTableSize = 533;
static CaseFoldValue caseFoldTable[533] = {
// { 0x00041, cf00041 }, { 0x00042, cf00042 }, { 0x00043, cf00043 }, { 0x00044, cf00044 },
// { 0x00045, cf00045 }, { 0x00046, cf00046 }, { 0x00047, cf00047 }, { 0x00048, cf00048 },
// { 0x00049, cf00049 }, { 0x0004A, cf0004A }, { 0x0004B, cf0004B }, { 0x0004C, cf0004C },
// { 0x0004D, cf0004D }, { 0x0004E, cf0004E }, { 0x0004F, cf0004F }, { 0x00050, cf00050 },
// { 0x00051, cf00051 }, { 0x00052, cf00052 }, { 0x00053, cf00053 }, { 0x00054, cf00054 },
// { 0x00055, cf00055 }, { 0x00056, cf00056 }, { 0x00057, cf00057 }, { 0x00058, cf00058 },
// { 0x00059, cf00059 }, { 0x0005A, cf0005A }, { 0x000B5, cf000B5 }, { 0x000C0, cf000C0 },
// { 0x000C1, cf000C1 }, { 0x000C2, cf000C2 }, { 0x000C3, cf000C3 }, { 0x000C4, cf000C4 },
// { 0x000C5, cf000C5 }, { 0x000C6, cf000C6 }, { 0x000C7, cf000C7 }, { 0x000C8, cf000C8 },
// { 0x000C9, cf000C9 }, { 0x000CA, cf000CA }, { 0x000CB, cf000CB }, { 0x000CC, cf000CC },
// { 0x000CD, cf000CD }, { 0x000CE, cf000CE }, { 0x000CF, cf000CF }, { 0x000D0, cf000D0 },
// { 0x000D1, cf000D1 }, { 0x000D2, cf000D2 }, { 0x000D3, cf000D3 }, { 0x000D4, cf000D4 },
// { 0x000D5, cf000D5 }, { 0x000D6, cf000D6 }, { 0x000D8, cf000D8 }, { 0x000D9, cf000D9 },
// { 0x000DA, cf000DA }, { 0x000DB, cf000DB }, { 0x000DC, cf000DC }, { 0x000DD, cf000DD },
// { 0x000DE, cf000DE }, { 0x000DF, cf000DF }, { 0x00100, cf00100 }, { 0x00102, cf00102 },
// { 0x00104, cf00104 }, { 0x00106, cf00106 }, { 0x00108, cf00108 }, { 0x0010A, cf0010A },
// { 0x0010C, cf0010C }, { 0x0010E, cf0010E }, { 0x00110, cf00110 }, { 0x00112, cf00112 },
// { 0x00114, cf00114 }, { 0x00116, cf00116 }, { 0x00118, cf00118 }, { 0x0011A, cf0011A },
// { 0x0011C, cf0011C }, { 0x0011E, cf0011E }, { 0x00120, cf00120 }, { 0x00122, cf00122 },
// { 0x00124, cf00124 }, { 0x00126, cf00126 }, { 0x00128, cf00128 }, { 0x0012A, cf0012A },
// { 0x0012C, cf0012C }, { 0x0012E, cf0012E }, { 0x00130, cf00130 }, { 0x00132, cf00132 },
// { 0x00134, cf00134 }, { 0x00136, cf00136 }, { 0x00139, cf00139 }, { 0x0013B, cf0013B },
// { 0x0013D, cf0013D }, { 0x0013F, cf0013F }, { 0x00141, cf00141 }, { 0x00143, cf00143 },
// { 0x00145, cf00145 }, { 0x00147, cf00147 }, { 0x00149, cf00149 }, { 0x0014A, cf0014A },
// { 0x0014C, cf0014C }, { 0x0014E, cf0014E }, { 0x00150, cf00150 }, { 0x00152, cf00152 },
// { 0x00154, cf00154 }, { 0x00156, cf00156 }, { 0x00158, cf00158 }, { 0x0015A, cf0015A },
// { 0x0015C, cf0015C }, { 0x0015E, cf0015E }, { 0x00160, cf00160 }, { 0x00162, cf00162 },
// { 0x00164, cf00164 }, { 0x00166, cf00166 }, { 0x00168, cf00168 }, { 0x0016A, cf0016A },
// { 0x0016C, cf0016C }, { 0x0016E, cf0016E }, { 0x00170, cf00170 }, { 0x00172, cf00172 },
// { 0x00174, cf00174 }, { 0x00176, cf00176 }, { 0x00178, cf00178 }, { 0x00179, cf00179 },
// { 0x0017B, cf0017B }, { 0x0017D, cf0017D }, { 0x0017F, cf0017F }, { 0x00181, cf00181 },
// { 0x00182, cf00182 }, { 0x00184, cf00184 }, { 0x00186, cf00186 }, { 0x00187, cf00187 },
// { 0x00189, cf00189 }, { 0x0018A, cf0018A }, { 0x0018B, cf0018B }, { 0x0018E, cf0018E },
// { 0x0018F, cf0018F }, { 0x00190, cf00190 }, { 0x00191, cf00191 }, { 0x00193, cf00193 },
// { 0x00194, cf00194 }, { 0x00196, cf00196 }, { 0x00197, cf00197 }, { 0x00198, cf00198 },
// { 0x0019C, cf0019C }, { 0x0019D, cf0019D }, { 0x0019F, cf0019F }, { 0x001A0, cf001A0 },
// { 0x001A2, cf001A2 }, { 0x001A4, cf001A4 }, { 0x001A6, cf001A6 }, { 0x001A7, cf001A7 },
// { 0x001A9, cf001A9 }, { 0x001AC, cf001AC }, { 0x001AE, cf001AE }, { 0x001AF, cf001AF },
// { 0x001B1, cf001B1 }, { 0x001B2, cf001B2 }, { 0x001B3, cf001B3 }, { 0x001B5, cf001B5 },
// { 0x001B7, cf001B7 }, { 0x001B8, cf001B8 }, { 0x001BC, cf001BC }, { 0x001C4, cf001C4 },
// { 0x001C5, cf001C5 }, { 0x001C7, cf001C7 }, { 0x001C8, cf001C8 }, { 0x001CA, cf001CA },
// { 0x001CB, cf001CB }, { 0x001CD, cf001CD }, { 0x001CF, cf001CF }, { 0x001D1, cf001D1 },
// { 0x001D3, cf001D3 }, { 0x001D5, cf001D5 }, { 0x001D7, cf001D7 }, { 0x001D9, cf001D9 },
// { 0x001DB, cf001DB }, { 0x001DE, cf001DE }, { 0x001E0, cf001E0 }, { 0x001E2, cf001E2 },
// { 0x001E4, cf001E4 }, { 0x001E6, cf001E6 }, { 0x001E8, cf001E8 }, { 0x001EA, cf001EA },
// { 0x001EC, cf001EC }, { 0x001EE, cf001EE }, { 0x001F0, cf001F0 }, { 0x001F1, cf001F1 },
// { 0x001F2, cf001F2 }, { 0x001F4, cf001F4 }, { 0x001F6, cf001F6 }, { 0x001F7, cf001F7 },
// { 0x001F8, cf001F8 }, { 0x001FA, cf001FA }, { 0x001FC, cf001FC }, { 0x001FE, cf001FE },
// { 0x00200, cf00200 }, { 0x00202, cf00202 }, { 0x00204, cf00204 }, { 0x00206, cf00206 },
// { 0x00208, cf00208 }, { 0x0020A, cf0020A }, { 0x0020C, cf0020C }, { 0x0020E, cf0020E },
// { 0x00210, cf00210 }, { 0x00212, cf00212 }, { 0x00214, cf00214 }, { 0x00216, cf00216 },
// { 0x00218, cf00218 }, { 0x0021A, cf0021A }, { 0x0021C, cf0021C }, { 0x0021E, cf0021E },
// { 0x00220, cf00220 }, { 0x00222, cf00222 }, { 0x00224, cf00224 }, { 0x00226, cf00226 },
// { 0x00228, cf00228 }, { 0x0022A, cf0022A }, { 0x0022C, cf0022C }, { 0x0022E, cf0022E },
// { 0x00230, cf00230 }, { 0x00232, cf00232 }, { 0x0023B, cf0023B }, { 0x0023D, cf0023D },
// { 0x00241, cf00241 }, { 0x00345, cf00345 }, { 0x00386, cf00386 }, { 0x00388, cf00388 },
// { 0x00389, cf00389 }, { 0x0038A, cf0038A }, { 0x0038C, cf0038C }, { 0x0038E, cf0038E },
// { 0x0038F, cf0038F }, { 0x00390, cf00390 }, { 0x00391, cf00391 }, { 0x00392, cf00392 },
// { 0x00393, cf00393 }, { 0x00394, cf00394 }, { 0x00395, cf00395 }, { 0x00396, cf00396 },
// { 0x00397, cf00397 }, { 0x00398, cf00398 }, { 0x00399, cf00399 }, { 0x0039A, cf0039A },
// { 0x0039B, cf0039B }, { 0x0039C, cf0039C }, { 0x0039D, cf0039D }, { 0x0039E, cf0039E },
// { 0x0039F, cf0039F }, { 0x003A0, cf003A0 }, { 0x003A1, cf003A1 }, { 0x003A3, cf003A3 },
// { 0x003A4, cf003A4 }, { 0x003A5, cf003A5 }, { 0x003A6, cf003A6 }, { 0x003A7, cf003A7 },
// { 0x003A8, cf003A8 }, { 0x003A9, cf003A9 }, { 0x003AA, cf003AA }, { 0x003AB, cf003AB },
// { 0x003B0, cf003B0 }, { 0x003C2, cf003C2 }, { 0x003D0, cf003D0 }, { 0x003D1, cf003D1 },
// { 0x003D5, cf003D5 }, { 0x003D6, cf003D6 }, { 0x003D8, cf003D8 }, { 0x003DA, cf003DA },
// { 0x003DC, cf003DC }, { 0x003DE, cf003DE }, { 0x003E0, cf003E0 }, { 0x003E2, cf003E2 },
// { 0x003E4, cf003E4 }, { 0x003E6, cf003E6 }, { 0x003E8, cf003E8 }, { 0x003EA, cf003EA },
// { 0x003EC, cf003EC }, { 0x003EE, cf003EE }, { 0x003F0, cf003F0 }, { 0x003F1, cf003F1 },
// { 0x003F4, cf003F4 }, { 0x003F5, cf003F5 }, { 0x003F7, cf003F7 }, { 0x003F9, cf003F9 },
// { 0x003FA, cf003FA }, { 0x00400, cf00400 }, { 0x00401, cf00401 }, { 0x00402, cf00402 },
// { 0x00403, cf00403 }, { 0x00404, cf00404 }, { 0x00405, cf00405 }, { 0x00406, cf00406 },
// { 0x00407, cf00407 }, { 0x00408, cf00408 }, { 0x00409, cf00409 }, { 0x0040A, cf0040A },
// { 0x0040B, cf0040B }, { 0x0040C, cf0040C }, { 0x0040D, cf0040D }, { 0x0040E, cf0040E },
// { 0x0040F, cf0040F }, { 0x00410, cf00410 }, { 0x00411, cf00411 }, { 0x00412, cf00412 },
// { 0x00413, cf00413 }, { 0x00414, cf00414 }, { 0x00415, cf00415 }, { 0x00416, cf00416 },
// { 0x00417, cf00417 }, { 0x00418, cf00418 }, { 0x00419, cf00419 }, { 0x0041A, cf0041A },
// { 0x0041B, cf0041B }, { 0x0041C, cf0041C }, { 0x0041D, cf0041D }, { 0x0041E, cf0041E },
// { 0x0041F, cf0041F }, { 0x00420, cf00420 }, { 0x00421, cf00421 }, { 0x00422, cf00422 },
// { 0x00423, cf00423 }, { 0x00424, cf00424 }, { 0x00425, cf00425 }, { 0x00426, cf00426 },
// { 0x00427, cf00427 }, { 0x00428, cf00428 }, { 0x00429, cf00429 }, { 0x0042A, cf0042A },
// { 0x0042B, cf0042B }, { 0x0042C, cf0042C }, { 0x0042D, cf0042D }, { 0x0042E, cf0042E },
// { 0x0042F, cf0042F }, { 0x00460, cf00460 }, { 0x00462, cf00462 }, { 0x00464, cf00464 },
// { 0x00466, cf00466 }, { 0x00468, cf00468 }, { 0x0046A, cf0046A }, { 0x0046C, cf0046C },
// { 0x0046E, cf0046E }, { 0x00470, cf00470 }, { 0x00472, cf00472 }, { 0x00474, cf00474 },
// { 0x00476, cf00476 }, { 0x00478, cf00478 }, { 0x0047A, cf0047A }, { 0x0047C, cf0047C },
// { 0x0047E, cf0047E }, { 0x00480, cf00480 }, { 0x0048A, cf0048A }, { 0x0048C, cf0048C },
// { 0x0048E, cf0048E }, { 0x00490, cf00490 }, { 0x00492, cf00492 }, { 0x00494, cf00494 },
// { 0x00496, cf00496 }, { 0x00498, cf00498 }, { 0x0049A, cf0049A }, { 0x0049C, cf0049C },
// { 0x0049E, cf0049E }, { 0x004A0, cf004A0 }, { 0x004A2, cf004A2 }, { 0x004A4, cf004A4 },
// { 0x004A6, cf004A6 }, { 0x004A8, cf004A8 }, { 0x004AA, cf004AA }, { 0x004AC, cf004AC },
// { 0x004AE, cf004AE }, { 0x004B0, cf004B0 }, { 0x004B2, cf004B2 }, { 0x004B4, cf004B4 },
// { 0x004B6, cf004B6 }, { 0x004B8, cf004B8 }, { 0x004BA, cf004BA }, { 0x004BC, cf004BC },
// { 0x004BE, cf004BE }, { 0x004C1, cf004C1 }, { 0x004C3, cf004C3 }, { 0x004C5, cf004C5 },
// { 0x004C7, cf004C7 }, { 0x004C9, cf004C9 }, { 0x004CB, cf004CB }, { 0x004CD, cf004CD },
// { 0x004D0, cf004D0 }, { 0x004D2, cf004D2 }, { 0x004D4, cf004D4 }, { 0x004D6, cf004D6 },
// { 0x004D8, cf004D8 }, { 0x004DA, cf004DA }, { 0x004DC, cf004DC }, { 0x004DE, cf004DE },
// { 0x004E0, cf004E0 }, { 0x004E2, cf004E2 }, { 0x004E4, cf004E4 }, { 0x004E6, cf004E6 },
// { 0x004E8, cf004E8 }, { 0x004EA, cf004EA }, { 0x004EC, cf004EC }, { 0x004EE, cf004EE },
// { 0x004F0, cf004F0 }, { 0x004F2, cf004F2 }, { 0x004F4, cf004F4 }, { 0x004F6, cf004F6 },
// { 0x004F8, cf004F8 }, { 0x00500, cf00500 }, { 0x00502, cf00502 }, { 0x00504, cf00504 },
// { 0x00506, cf00506 }, { 0x00508, cf00508 }, { 0x0050A, cf0050A }, { 0x0050C, cf0050C },
// { 0x0050E, cf0050E }, { 0x00531, cf00531 }, { 0x00532, cf00532 }, { 0x00533, cf00533 },
// { 0x00534, cf00534 }, { 0x00535, cf00535 }, { 0x00536, cf00536 }, { 0x00537, cf00537 },
// { 0x00538, cf00538 }, { 0x00539, cf00539 }, { 0x0053A, cf0053A }, { 0x0053B, cf0053B },
// { 0x0053C, cf0053C }, { 0x0053D, cf0053D }, { 0x0053E, cf0053E }, { 0x0053F, cf0053F },
// { 0x00540, cf00540 }, { 0x00541, cf00541 }, { 0x00542, cf00542 }, { 0x00543, cf00543 },
// { 0x00544, cf00544 }, { 0x00545, cf00545 }, { 0x00546, cf00546 }, { 0x00547, cf00547 },
// { 0x00548, cf00548 }, { 0x00549, cf00549 }, { 0x0054A, cf0054A }, { 0x0054B, cf0054B },
// { 0x0054C, cf0054C }, { 0x0054D, cf0054D }, { 0x0054E, cf0054E }, { 0x0054F, cf0054F },
// { 0x00550, cf00550 }, { 0x00551, cf00551 }, { 0x00552, cf00552 }, { 0x00553, cf00553 },
// { 0x00554, cf00554 }, { 0x00555, cf00555 }, { 0x00556, cf00556 }, { 0x00587, cf00587 },

{ 0x010A0, cf010A0 }, { 0x010A1, cf010A1 }, { 0x010A2, cf010A2 }, { 0x010A3, cf010A3 },
{ 0x010A4, cf010A4 }, { 0x010A5, cf010A5 }, { 0x010A6, cf010A6 }, { 0x010A7, cf010A7 },
{ 0x010A8, cf010A8 }, { 0x010A9, cf010A9 }, { 0x010AA, cf010AA }, { 0x010AB, cf010AB },
{ 0x010AC, cf010AC }, { 0x010AD, cf010AD }, { 0x010AE, cf010AE }, { 0x010AF, cf010AF },
{ 0x010B0, cf010B0 }, { 0x010B1, cf010B1 }, { 0x010B2, cf010B2 }, { 0x010B3, cf010B3 },
{ 0x010B4, cf010B4 }, { 0x010B5, cf010B5 }, { 0x010B6, cf010B6 }, { 0x010B7, cf010B7 },
{ 0x010B8, cf010B8 }, { 0x010B9, cf010B9 }, { 0x010BA, cf010BA }, { 0x010BB, cf010BB },
{ 0x010BC, cf010BC }, { 0x010BD, cf010BD }, { 0x010BE, cf010BE }, { 0x010BF, cf010BF },
{ 0x010C0, cf010C0 }, { 0x010C1, cf010C1 }, { 0x010C2, cf010C2 }, { 0x010C3, cf010C3 },
{ 0x010C4, cf010C4 }, { 0x010C5, cf010C5 }, { 0x01E00, cf01E00 }, { 0x01E02, cf01E02 },
{ 0x01E04, cf01E04 }, { 0x01E06, cf01E06 }, { 0x01E08, cf01E08 }, { 0x01E0A, cf01E0A },
{ 0x01E0C, cf01E0C }, { 0x01E0E, cf01E0E }, { 0x01E10, cf01E10 }, { 0x01E12, cf01E12 },
{ 0x01E14, cf01E14 }, { 0x01E16, cf01E16 }, { 0x01E18, cf01E18 }, { 0x01E1A, cf01E1A },
{ 0x01E1C, cf01E1C }, { 0x01E1E, cf01E1E }, { 0x01E20, cf01E20 }, { 0x01E22, cf01E22 },
{ 0x01E24, cf01E24 }, { 0x01E26, cf01E26 }, { 0x01E28, cf01E28 }, { 0x01E2A, cf01E2A },
{ 0x01E2C, cf01E2C }, { 0x01E2E, cf01E2E }, { 0x01E30, cf01E30 }, { 0x01E32, cf01E32 },
{ 0x01E34, cf01E34 }, { 0x01E36, cf01E36 }, { 0x01E38, cf01E38 }, { 0x01E3A, cf01E3A },
{ 0x01E3C, cf01E3C }, { 0x01E3E, cf01E3E }, { 0x01E40, cf01E40 }, { 0x01E42, cf01E42 },
{ 0x01E44, cf01E44 }, { 0x01E46, cf01E46 }, { 0x01E48, cf01E48 }, { 0x01E4A, cf01E4A },
{ 0x01E4C, cf01E4C }, { 0x01E4E, cf01E4E }, { 0x01E50, cf01E50 }, { 0x01E52, cf01E52 },
{ 0x01E54, cf01E54 }, { 0x01E56, cf01E56 }, { 0x01E58, cf01E58 }, { 0x01E5A, cf01E5A },
{ 0x01E5C, cf01E5C }, { 0x01E5E, cf01E5E }, { 0x01E60, cf01E60 }, { 0x01E62, cf01E62 },
{ 0x01E64, cf01E64 }, { 0x01E66, cf01E66 }, { 0x01E68, cf01E68 }, { 0x01E6A, cf01E6A },
{ 0x01E6C, cf01E6C }, { 0x01E6E, cf01E6E }, { 0x01E70, cf01E70 }, { 0x01E72, cf01E72 },
{ 0x01E74, cf01E74 }, { 0x01E76, cf01E76 }, { 0x01E78, cf01E78 }, { 0x01E7A, cf01E7A },
{ 0x01E7C, cf01E7C }, { 0x01E7E, cf01E7E }, { 0x01E80, cf01E80 }, { 0x01E82, cf01E82 },
{ 0x01E84, cf01E84 }, { 0x01E86, cf01E86 }, { 0x01E88, cf01E88 }, { 0x01E8A, cf01E8A },
{ 0x01E8C, cf01E8C }, { 0x01E8E, cf01E8E }, { 0x01E90, cf01E90 }, { 0x01E92, cf01E92 },
{ 0x01E94, cf01E94 }, { 0x01E96, cf01E96 }, { 0x01E97, cf01E97 }, { 0x01E98, cf01E98 },
{ 0x01E99, cf01E99 }, { 0x01E9A, cf01E9A }, { 0x01E9B, cf01E9B }, { 0x01EA0, cf01EA0 },
{ 0x01EA2, cf01EA2 }, { 0x01EA4, cf01EA4 }, { 0x01EA6, cf01EA6 }, { 0x01EA8, cf01EA8 },
{ 0x01EAA, cf01EAA }, { 0x01EAC, cf01EAC }, { 0x01EAE, cf01EAE }, { 0x01EB0, cf01EB0 },
{ 0x01EB2, cf01EB2 }, { 0x01EB4, cf01EB4 }, { 0x01EB6, cf01EB6 }, { 0x01EB8, cf01EB8 },
{ 0x01EBA, cf01EBA }, { 0x01EBC, cf01EBC }, { 0x01EBE, cf01EBE }, { 0x01EC0, cf01EC0 },
{ 0x01EC2, cf01EC2 }, { 0x01EC4, cf01EC4 }, { 0x01EC6, cf01EC6 }, { 0x01EC8, cf01EC8 },
{ 0x01ECA, cf01ECA }, { 0x01ECC, cf01ECC }, { 0x01ECE, cf01ECE }, { 0x01ED0, cf01ED0 },
{ 0x01ED2, cf01ED2 }, { 0x01ED4, cf01ED4 }, { 0x01ED6, cf01ED6 }, { 0x01ED8, cf01ED8 },
{ 0x01EDA, cf01EDA }, { 0x01EDC, cf01EDC }, { 0x01EDE, cf01EDE }, { 0x01EE0, cf01EE0 },
{ 0x01EE2, cf01EE2 }, { 0x01EE4, cf01EE4 }, { 0x01EE6, cf01EE6 }, { 0x01EE8, cf01EE8 },
{ 0x01EEA, cf01EEA }, { 0x01EEC, cf01EEC }, { 0x01EEE, cf01EEE }, { 0x01EF0, cf01EF0 },
{ 0x01EF2, cf01EF2 }, { 0x01EF4, cf01EF4 }, { 0x01EF6, cf01EF6 }, { 0x01EF8, cf01EF8 },
{ 0x01F08, cf01F08 }, { 0x01F09, cf01F09 }, { 0x01F0A, cf01F0A }, { 0x01F0B, cf01F0B },
{ 0x01F0C, cf01F0C }, { 0x01F0D, cf01F0D }, { 0x01F0E, cf01F0E }, { 0x01F0F, cf01F0F },
{ 0x01F18, cf01F18 }, { 0x01F19, cf01F19 }, { 0x01F1A, cf01F1A }, { 0x01F1B, cf01F1B },
{ 0x01F1C, cf01F1C }, { 0x01F1D, cf01F1D }, { 0x01F28, cf01F28 }, { 0x01F29, cf01F29 },
{ 0x01F2A, cf01F2A }, { 0x01F2B, cf01F2B }, { 0x01F2C, cf01F2C }, { 0x01F2D, cf01F2D },
{ 0x01F2E, cf01F2E }, { 0x01F2F, cf01F2F }, { 0x01F38, cf01F38 }, { 0x01F39, cf01F39 },
{ 0x01F3A, cf01F3A }, { 0x01F3B, cf01F3B }, { 0x01F3C, cf01F3C }, { 0x01F3D, cf01F3D },
{ 0x01F3E, cf01F3E }, { 0x01F3F, cf01F3F }, { 0x01F48, cf01F48 }, { 0x01F49, cf01F49 },
{ 0x01F4A, cf01F4A }, { 0x01F4B, cf01F4B }, { 0x01F4C, cf01F4C }, { 0x01F4D, cf01F4D },
{ 0x01F50, cf01F50 }, { 0x01F52, cf01F52 }, { 0x01F54, cf01F54 }, { 0x01F56, cf01F56 },
{ 0x01F59, cf01F59 }, { 0x01F5B, cf01F5B }, { 0x01F5D, cf01F5D }, { 0x01F5F, cf01F5F },
{ 0x01F68, cf01F68 }, { 0x01F69, cf01F69 }, { 0x01F6A, cf01F6A }, { 0x01F6B, cf01F6B },
{ 0x01F6C, cf01F6C }, { 0x01F6D, cf01F6D }, { 0x01F6E, cf01F6E }, { 0x01F6F, cf01F6F },
{ 0x01F80, cf01F80 }, { 0x01F81, cf01F81 }, { 0x01F82, cf01F82 }, { 0x01F83, cf01F83 },
{ 0x01F84, cf01F84 }, { 0x01F85, cf01F85 }, { 0x01F86, cf01F86 }, { 0x01F87, cf01F87 },
{ 0x01F88, cf01F88 }, { 0x01F89, cf01F89 }, { 0x01F8A, cf01F8A }, { 0x01F8B, cf01F8B },
{ 0x01F8C, cf01F8C }, { 0x01F8D, cf01F8D }, { 0x01F8E, cf01F8E }, { 0x01F8F, cf01F8F },
{ 0x01F90, cf01F90 }, { 0x01F91, cf01F91 }, { 0x01F92, cf01F92 }, { 0x01F93, cf01F93 },
{ 0x01F94, cf01F94 }, { 0x01F95, cf01F95 }, { 0x01F96, cf01F96 }, { 0x01F97, cf01F97 },
{ 0x01F98, cf01F98 }, { 0x01F99, cf01F99 }, { 0x01F9A, cf01F9A }, { 0x01F9B, cf01F9B },
{ 0x01F9C, cf01F9C }, { 0x01F9D, cf01F9D }, { 0x01F9E, cf01F9E }, { 0x01F9F, cf01F9F },
{ 0x01FA0, cf01FA0 }, { 0x01FA1, cf01FA1 }, { 0x01FA2, cf01FA2 }, { 0x01FA3, cf01FA3 },
{ 0x01FA4, cf01FA4 }, { 0x01FA5, cf01FA5 }, { 0x01FA6, cf01FA6 }, { 0x01FA7, cf01FA7 },
{ 0x01FA8, cf01FA8 }, { 0x01FA9, cf01FA9 }, { 0x01FAA, cf01FAA }, { 0x01FAB, cf01FAB },
{ 0x01FAC, cf01FAC }, { 0x01FAD, cf01FAD }, { 0x01FAE, cf01FAE }, { 0x01FAF, cf01FAF },
{ 0x01FB2, cf01FB2 }, { 0x01FB3, cf01FB3 }, { 0x01FB4, cf01FB4 }, { 0x01FB6, cf01FB6 },
{ 0x01FB7, cf01FB7 }, { 0x01FB8, cf01FB8 }, { 0x01FB9, cf01FB9 }, { 0x01FBA, cf01FBA },
{ 0x01FBB, cf01FBB }, { 0x01FBC, cf01FBC }, { 0x01FBE, cf01FBE }, { 0x01FC2, cf01FC2 },
{ 0x01FC3, cf01FC3 }, { 0x01FC4, cf01FC4 }, { 0x01FC6, cf01FC6 }, { 0x01FC7, cf01FC7 },
{ 0x01FC8, cf01FC8 }, { 0x01FC9, cf01FC9 }, { 0x01FCA, cf01FCA }, { 0x01FCB, cf01FCB },
{ 0x01FCC, cf01FCC }, { 0x01FD2, cf01FD2 }, { 0x01FD3, cf01FD3 }, { 0x01FD6, cf01FD6 },
{ 0x01FD7, cf01FD7 }, { 0x01FD8, cf01FD8 }, { 0x01FD9, cf01FD9 }, { 0x01FDA, cf01FDA },
{ 0x01FDB, cf01FDB }, { 0x01FE2, cf01FE2 }, { 0x01FE3, cf01FE3 }, { 0x01FE4, cf01FE4 },
{ 0x01FE6, cf01FE6 }, { 0x01FE7, cf01FE7 }, { 0x01FE8, cf01FE8 }, { 0x01FE9, cf01FE9 },
{ 0x01FEA, cf01FEA }, { 0x01FEB, cf01FEB }, { 0x01FEC, cf01FEC }, { 0x01FF2, cf01FF2 },
{ 0x01FF3, cf01FF3 }, { 0x01FF4, cf01FF4 }, { 0x01FF6, cf01FF6 }, { 0x01FF7, cf01FF7 },
{ 0x01FF8, cf01FF8 }, { 0x01FF9, cf01FF9 }, { 0x01FFA, cf01FFA }, { 0x01FFB, cf01FFB },
{ 0x01FFC, cf01FFC }, { 0x02126, cf02126 }, { 0x0212A, cf0212A }, { 0x0212B, cf0212B },
{ 0x02160, cf02160 }, { 0x02161, cf02161 }, { 0x02162, cf02162 }, { 0x02163, cf02163 },
{ 0x02164, cf02164 }, { 0x02165, cf02165 }, { 0x02166, cf02166 }, { 0x02167, cf02167 },
{ 0x02168, cf02168 }, { 0x02169, cf02169 }, { 0x0216A, cf0216A }, { 0x0216B, cf0216B },
{ 0x0216C, cf0216C }, { 0x0216D, cf0216D }, { 0x0216E, cf0216E }, { 0x0216F, cf0216F },
{ 0x024B6, cf024B6 }, { 0x024B7, cf024B7 }, { 0x024B8, cf024B8 }, { 0x024B9, cf024B9 },
{ 0x024BA, cf024BA }, { 0x024BB, cf024BB }, { 0x024BC, cf024BC }, { 0x024BD, cf024BD },
{ 0x024BE, cf024BE }, { 0x024BF, cf024BF }, { 0x024C0, cf024C0 }, { 0x024C1, cf024C1 },
{ 0x024C2, cf024C2 }, { 0x024C3, cf024C3 }, { 0x024C4, cf024C4 }, { 0x024C5, cf024C5 },
{ 0x024C6, cf024C6 }, { 0x024C7, cf024C7 }, { 0x024C8, cf024C8 }, { 0x024C9, cf024C9 },
{ 0x024CA, cf024CA }, { 0x024CB, cf024CB }, { 0x024CC, cf024CC }, { 0x024CD, cf024CD },
{ 0x024CE, cf024CE }, { 0x024CF, cf024CF }, { 0x02C00, cf02C00 }, { 0x02C01, cf02C01 },
{ 0x02C02, cf02C02 }, { 0x02C03, cf02C03 }, { 0x02C04, cf02C04 }, { 0x02C05, cf02C05 },
{ 0x02C06, cf02C06 }, { 0x02C07, cf02C07 }, { 0x02C08, cf02C08 }, { 0x02C09, cf02C09 },
{ 0x02C0A, cf02C0A }, { 0x02C0B, cf02C0B }, { 0x02C0C, cf02C0C }, { 0x02C0D, cf02C0D },
{ 0x02C0E, cf02C0E }, { 0x02C0F, cf02C0F }, { 0x02C10, cf02C10 }, { 0x02C11, cf02C11 },
{ 0x02C12, cf02C12 }, { 0x02C13, cf02C13 }, { 0x02C14, cf02C14 }, { 0x02C15, cf02C15 },
{ 0x02C16, cf02C16 }, { 0x02C17, cf02C17 }, { 0x02C18, cf02C18 }, { 0x02C19, cf02C19 },
{ 0x02C1A, cf02C1A }, { 0x02C1B, cf02C1B }, { 0x02C1C, cf02C1C }, { 0x02C1D, cf02C1D },
{ 0x02C1E, cf02C1E }, { 0x02C1F, cf02C1F }, { 0x02C20, cf02C20 }, { 0x02C21, cf02C21 },
{ 0x02C22, cf02C22 }, { 0x02C23, cf02C23 }, { 0x02C24, cf02C24 }, { 0x02C25, cf02C25 },
{ 0x02C26, cf02C26 }, { 0x02C27, cf02C27 }, { 0x02C28, cf02C28 }, { 0x02C29, cf02C29 },
{ 0x02C2A, cf02C2A }, { 0x02C2B, cf02C2B }, { 0x02C2C, cf02C2C }, { 0x02C2D, cf02C2D },
{ 0x02C2E, cf02C2E }, { 0x02C80, cf02C80 }, { 0x02C82, cf02C82 }, { 0x02C84, cf02C84 },
{ 0x02C86, cf02C86 }, { 0x02C88, cf02C88 }, { 0x02C8A, cf02C8A }, { 0x02C8C, cf02C8C },
{ 0x02C8E, cf02C8E }, { 0x02C90, cf02C90 }, { 0x02C92, cf02C92 }, { 0x02C94, cf02C94 },
{ 0x02C96, cf02C96 }, { 0x02C98, cf02C98 }, { 0x02C9A, cf02C9A }, { 0x02C9C, cf02C9C },
{ 0x02C9E, cf02C9E }, { 0x02CA0, cf02CA0 }, { 0x02CA2, cf02CA2 }, { 0x02CA4, cf02CA4 },
{ 0x02CA6, cf02CA6 }, { 0x02CA8, cf02CA8 }, { 0x02CAA, cf02CAA }, { 0x02CAC, cf02CAC },
{ 0x02CAE, cf02CAE }, { 0x02CB0, cf02CB0 }, { 0x02CB2, cf02CB2 }, { 0x02CB4, cf02CB4 },
{ 0x02CB6, cf02CB6 }, { 0x02CB8, cf02CB8 }, { 0x02CBA, cf02CBA }, { 0x02CBC, cf02CBC },
{ 0x02CBE, cf02CBE }, { 0x02CC0, cf02CC0 }, { 0x02CC2, cf02CC2 }, { 0x02CC4, cf02CC4 },
{ 0x02CC6, cf02CC6 }, { 0x02CC8, cf02CC8 }, { 0x02CCA, cf02CCA }, { 0x02CCC, cf02CCC },
{ 0x02CCE, cf02CCE }, { 0x02CD0, cf02CD0 }, { 0x02CD2, cf02CD2 }, { 0x02CD4, cf02CD4 },
{ 0x02CD6, cf02CD6 }, { 0x02CD8, cf02CD8 }, { 0x02CDA, cf02CDA }, { 0x02CDC, cf02CDC },
{ 0x02CDE, cf02CDE }, { 0x02CE0, cf02CE0 }, { 0x02CE2, cf02CE2 }, { 0x0FB00, cf0FB00 },
{ 0x0FB01, cf0FB01 }, { 0x0FB02, cf0FB02 }, { 0x0FB03, cf0FB03 }, { 0x0FB04, cf0FB04 },
{ 0x0FB05, cf0FB05 }, { 0x0FB06, cf0FB06 }, { 0x0FB13, cf0FB13 }, { 0x0FB14, cf0FB14 },
{ 0x0FB15, cf0FB15 }, { 0x0FB16, cf0FB16 }, { 0x0FB17, cf0FB17 }, { 0x0FF21, cf0FF21 },
{ 0x0FF22, cf0FF22 }, { 0x0FF23, cf0FF23 }, { 0x0FF24, cf0FF24 }, { 0x0FF25, cf0FF25 },
{ 0x0FF26, cf0FF26 }, { 0x0FF27, cf0FF27 }, { 0x0FF28, cf0FF28 }, { 0x0FF29, cf0FF29 },
{ 0x0FF2A, cf0FF2A }, { 0x0FF2B, cf0FF2B }, { 0x0FF2C, cf0FF2C }, { 0x0FF2D, cf0FF2D },
{ 0x0FF2E, cf0FF2E }, { 0x0FF2F, cf0FF2F }, { 0x0FF30, cf0FF30 }, { 0x0FF31, cf0FF31 },
{ 0x0FF32, cf0FF32 }, { 0x0FF33, cf0FF33 }, { 0x0FF34, cf0FF34 }, { 0x0FF35, cf0FF35 },
{ 0x0FF36, cf0FF36 }, { 0x0FF37, cf0FF37 }, { 0x0FF38, cf0FF38 }, { 0x0FF39, cf0FF39 },
{ 0x0FF3A, cf0FF3A }, { 0x10400, cf10400 }, { 0x10401, cf10401 }, { 0x10402, cf10402 },
{ 0x10403, cf10403 }, { 0x10404, cf10404 }, { 0x10405, cf10405 }, { 0x10406, cf10406 },
{ 0x10407, cf10407 }, { 0x10408, cf10408 }, { 0x10409, cf10409 }, { 0x1040A, cf1040A },
{ 0x1040B, cf1040B }, { 0x1040C, cf1040C }, { 0x1040D, cf1040D }, { 0x1040E, cf1040E },
{ 0x1040F, cf1040F }, { 0x10410, cf10410 }, { 0x10411, cf10411 }, { 0x10412, cf10412 },
{ 0x10413, cf10413 }, { 0x10414, cf10414 }, { 0x10415, cf10415 }, { 0x10416, cf10416 },
{ 0x10417, cf10417 }, { 0x10418, cf10418 }, { 0x10419, cf10419 }, { 0x1041A, cf1041A },
{ 0x1041B, cf1041B }, { 0x1041C, cf1041C }, { 0x1041D, cf1041D }, { 0x1041E, cf1041E },
{ 0x1041F, cf1041F }, { 0x10420, cf10420 }, { 0x10421, cf10421 }, { 0x10422, cf10422 },
{ 0x10423, cf10423 }, { 0x10424, cf10424 }, { 0x10425, cf10425 }, { 0x10426, cf10426 },
{ 0x10427, cf10427 }
};

unsigned int *CaseFoldTransform::getCaseFold(unsigned int ch)
{
  if(ch < caseFoldDirectTableSize) {
    return caseFoldDirectTable[ch];
  }

  // Binary search in the g_caseFoldTable table
  unsigned int min = 0;
  unsigned int max = caseFoldTableSize;
  unsigned int middle;
  int cmp;

  while(min < max) {
    middle = (max + min) >> 1;

    cmp = caseFoldTable[middle].ch - ch;
    if(cmp > 0) max = middle;
    else if(cmp < 0) {
      min = middle + 1;
    }
    else {
      return caseFoldTable[middle].dest;
    }
  }

  return 0;
}
