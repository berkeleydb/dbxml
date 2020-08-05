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
 * $Id: UCANormalizer4.cpp 475 2008-01-08 18:47:44Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/UCANormalizer.hpp>

static unsigned int lc00041[] = { 0x00061, 0x00000 };
static unsigned int lc00042[] = { 0x00062, 0x00000 };
static unsigned int lc00043[] = { 0x00063, 0x00000 };
static unsigned int lc00044[] = { 0x00064, 0x00000 };
static unsigned int lc00045[] = { 0x00065, 0x00000 };
static unsigned int lc00046[] = { 0x00066, 0x00000 };
static unsigned int lc00047[] = { 0x00067, 0x00000 };
static unsigned int lc00048[] = { 0x00068, 0x00000 };
static unsigned int lc00049[] = { 0x00069, 0x00000 };
static unsigned int lc0004A[] = { 0x0006A, 0x00000 };
static unsigned int lc0004B[] = { 0x0006B, 0x00000 };
static unsigned int lc0004C[] = { 0x0006C, 0x00000 };
static unsigned int lc0004D[] = { 0x0006D, 0x00000 };
static unsigned int lc0004E[] = { 0x0006E, 0x00000 };
static unsigned int lc0004F[] = { 0x0006F, 0x00000 };
static unsigned int lc00050[] = { 0x00070, 0x00000 };
static unsigned int lc00051[] = { 0x00071, 0x00000 };
static unsigned int lc00052[] = { 0x00072, 0x00000 };
static unsigned int lc00053[] = { 0x00073, 0x00000 };
static unsigned int lc00054[] = { 0x00074, 0x00000 };
static unsigned int lc00055[] = { 0x00075, 0x00000 };
static unsigned int lc00056[] = { 0x00076, 0x00000 };
static unsigned int lc00057[] = { 0x00077, 0x00000 };
static unsigned int lc00058[] = { 0x00078, 0x00000 };
static unsigned int lc00059[] = { 0x00079, 0x00000 };
static unsigned int lc0005A[] = { 0x0007A, 0x00000 };
static unsigned int lc000C0[] = { 0x000E0, 0x00000 };
static unsigned int lc000C1[] = { 0x000E1, 0x00000 };
static unsigned int lc000C2[] = { 0x000E2, 0x00000 };
static unsigned int lc000C3[] = { 0x000E3, 0x00000 };
static unsigned int lc000C4[] = { 0x000E4, 0x00000 };
static unsigned int lc000C5[] = { 0x000E5, 0x00000 };
static unsigned int lc000C6[] = { 0x000E6, 0x00000 };
static unsigned int lc000C7[] = { 0x000E7, 0x00000 };
static unsigned int lc000C8[] = { 0x000E8, 0x00000 };
static unsigned int lc000C9[] = { 0x000E9, 0x00000 };
static unsigned int lc000CA[] = { 0x000EA, 0x00000 };
static unsigned int lc000CB[] = { 0x000EB, 0x00000 };
static unsigned int lc000CC[] = { 0x000EC, 0x00000 };
static unsigned int lc000CD[] = { 0x000ED, 0x00000 };
static unsigned int lc000CE[] = { 0x000EE, 0x00000 };
static unsigned int lc000CF[] = { 0x000EF, 0x00000 };
static unsigned int lc000D0[] = { 0x000F0, 0x00000 };
static unsigned int lc000D1[] = { 0x000F1, 0x00000 };
static unsigned int lc000D2[] = { 0x000F2, 0x00000 };
static unsigned int lc000D3[] = { 0x000F3, 0x00000 };
static unsigned int lc000D4[] = { 0x000F4, 0x00000 };
static unsigned int lc000D5[] = { 0x000F5, 0x00000 };
static unsigned int lc000D6[] = { 0x000F6, 0x00000 };
static unsigned int lc000D8[] = { 0x000F8, 0x00000 };
static unsigned int lc000D9[] = { 0x000F9, 0x00000 };
static unsigned int lc000DA[] = { 0x000FA, 0x00000 };
static unsigned int lc000DB[] = { 0x000FB, 0x00000 };
static unsigned int lc000DC[] = { 0x000FC, 0x00000 };
static unsigned int lc000DD[] = { 0x000FD, 0x00000 };
static unsigned int lc000DE[] = { 0x000FE, 0x00000 };
static unsigned int lc000DF[] = { 0x000DF, 0x00000 };
static unsigned int lc00100[] = { 0x00101, 0x00000 };
static unsigned int lc00102[] = { 0x00103, 0x00000 };
static unsigned int lc00104[] = { 0x00105, 0x00000 };
static unsigned int lc00106[] = { 0x00107, 0x00000 };
static unsigned int lc00108[] = { 0x00109, 0x00000 };
static unsigned int lc0010A[] = { 0x0010B, 0x00000 };
static unsigned int lc0010C[] = { 0x0010D, 0x00000 };
static unsigned int lc0010E[] = { 0x0010F, 0x00000 };
static unsigned int lc00110[] = { 0x00111, 0x00000 };
static unsigned int lc00112[] = { 0x00113, 0x00000 };
static unsigned int lc00114[] = { 0x00115, 0x00000 };
static unsigned int lc00116[] = { 0x00117, 0x00000 };
static unsigned int lc00118[] = { 0x00119, 0x00000 };
static unsigned int lc0011A[] = { 0x0011B, 0x00000 };
static unsigned int lc0011C[] = { 0x0011D, 0x00000 };
static unsigned int lc0011E[] = { 0x0011F, 0x00000 };
static unsigned int lc00120[] = { 0x00121, 0x00000 };
static unsigned int lc00122[] = { 0x00123, 0x00000 };
static unsigned int lc00124[] = { 0x00125, 0x00000 };
static unsigned int lc00126[] = { 0x00127, 0x00000 };
static unsigned int lc00128[] = { 0x00129, 0x00000 };
static unsigned int lc0012A[] = { 0x0012B, 0x00000 };
static unsigned int lc0012C[] = { 0x0012D, 0x00000 };
static unsigned int lc0012E[] = { 0x0012F, 0x00000 };
static unsigned int lc00130[] = { 0x00069, 0x00307, 0x00000 };
static unsigned int lc00132[] = { 0x00133, 0x00000 };
static unsigned int lc00134[] = { 0x00135, 0x00000 };
static unsigned int lc00136[] = { 0x00137, 0x00000 };
static unsigned int lc00139[] = { 0x0013A, 0x00000 };
static unsigned int lc0013B[] = { 0x0013C, 0x00000 };
static unsigned int lc0013D[] = { 0x0013E, 0x00000 };
static unsigned int lc0013F[] = { 0x00140, 0x00000 };
static unsigned int lc00141[] = { 0x00142, 0x00000 };
static unsigned int lc00143[] = { 0x00144, 0x00000 };
static unsigned int lc00145[] = { 0x00146, 0x00000 };
static unsigned int lc00147[] = { 0x00148, 0x00000 };
static unsigned int lc00149[] = { 0x00149, 0x00000 };
static unsigned int lc0014A[] = { 0x0014B, 0x00000 };
static unsigned int lc0014C[] = { 0x0014D, 0x00000 };
static unsigned int lc0014E[] = { 0x0014F, 0x00000 };
static unsigned int lc00150[] = { 0x00151, 0x00000 };
static unsigned int lc00152[] = { 0x00153, 0x00000 };
static unsigned int lc00154[] = { 0x00155, 0x00000 };
static unsigned int lc00156[] = { 0x00157, 0x00000 };
static unsigned int lc00158[] = { 0x00159, 0x00000 };
static unsigned int lc0015A[] = { 0x0015B, 0x00000 };
static unsigned int lc0015C[] = { 0x0015D, 0x00000 };
static unsigned int lc0015E[] = { 0x0015F, 0x00000 };
static unsigned int lc00160[] = { 0x00161, 0x00000 };
static unsigned int lc00162[] = { 0x00163, 0x00000 };
static unsigned int lc00164[] = { 0x00165, 0x00000 };
static unsigned int lc00166[] = { 0x00167, 0x00000 };
static unsigned int lc00168[] = { 0x00169, 0x00000 };
static unsigned int lc0016A[] = { 0x0016B, 0x00000 };
static unsigned int lc0016C[] = { 0x0016D, 0x00000 };
static unsigned int lc0016E[] = { 0x0016F, 0x00000 };
static unsigned int lc00170[] = { 0x00171, 0x00000 };
static unsigned int lc00172[] = { 0x00173, 0x00000 };
static unsigned int lc00174[] = { 0x00175, 0x00000 };
static unsigned int lc00176[] = { 0x00177, 0x00000 };
static unsigned int lc00178[] = { 0x000FF, 0x00000 };
static unsigned int lc00179[] = { 0x0017A, 0x00000 };
static unsigned int lc0017B[] = { 0x0017C, 0x00000 };
static unsigned int lc0017D[] = { 0x0017E, 0x00000 };
static unsigned int lc00181[] = { 0x00253, 0x00000 };
static unsigned int lc00182[] = { 0x00183, 0x00000 };
static unsigned int lc00184[] = { 0x00185, 0x00000 };
static unsigned int lc00186[] = { 0x00254, 0x00000 };
static unsigned int lc00187[] = { 0x00188, 0x00000 };
static unsigned int lc00189[] = { 0x00256, 0x00000 };
static unsigned int lc0018A[] = { 0x00257, 0x00000 };
static unsigned int lc0018B[] = { 0x0018C, 0x00000 };
static unsigned int lc0018E[] = { 0x001DD, 0x00000 };
static unsigned int lc0018F[] = { 0x00259, 0x00000 };
static unsigned int lc00190[] = { 0x0025B, 0x00000 };
static unsigned int lc00191[] = { 0x00192, 0x00000 };
static unsigned int lc00193[] = { 0x00260, 0x00000 };
static unsigned int lc00194[] = { 0x00263, 0x00000 };
static unsigned int lc00196[] = { 0x00269, 0x00000 };
static unsigned int lc00197[] = { 0x00268, 0x00000 };
static unsigned int lc00198[] = { 0x00199, 0x00000 };
static unsigned int lc0019C[] = { 0x0026F, 0x00000 };
static unsigned int lc0019D[] = { 0x00272, 0x00000 };
static unsigned int lc0019F[] = { 0x00275, 0x00000 };
static unsigned int lc001A0[] = { 0x001A1, 0x00000 };
static unsigned int lc001A2[] = { 0x001A3, 0x00000 };
static unsigned int lc001A4[] = { 0x001A5, 0x00000 };
static unsigned int lc001A6[] = { 0x00280, 0x00000 };
static unsigned int lc001A7[] = { 0x001A8, 0x00000 };
static unsigned int lc001A9[] = { 0x00283, 0x00000 };
static unsigned int lc001AC[] = { 0x001AD, 0x00000 };
static unsigned int lc001AE[] = { 0x00288, 0x00000 };
static unsigned int lc001AF[] = { 0x001B0, 0x00000 };
static unsigned int lc001B1[] = { 0x0028A, 0x00000 };
static unsigned int lc001B2[] = { 0x0028B, 0x00000 };
static unsigned int lc001B3[] = { 0x001B4, 0x00000 };
static unsigned int lc001B5[] = { 0x001B6, 0x00000 };
static unsigned int lc001B7[] = { 0x00292, 0x00000 };
static unsigned int lc001B8[] = { 0x001B9, 0x00000 };
static unsigned int lc001BC[] = { 0x001BD, 0x00000 };
static unsigned int lc001C4[] = { 0x001C6, 0x00000 };
static unsigned int lc001C5[] = { 0x001C6, 0x00000 };
static unsigned int lc001C7[] = { 0x001C9, 0x00000 };
static unsigned int lc001C8[] = { 0x001C9, 0x00000 };
static unsigned int lc001CA[] = { 0x001CC, 0x00000 };
static unsigned int lc001CB[] = { 0x001CC, 0x00000 };
static unsigned int lc001CD[] = { 0x001CE, 0x00000 };
static unsigned int lc001CF[] = { 0x001D0, 0x00000 };
static unsigned int lc001D1[] = { 0x001D2, 0x00000 };
static unsigned int lc001D3[] = { 0x001D4, 0x00000 };
static unsigned int lc001D5[] = { 0x001D6, 0x00000 };
static unsigned int lc001D7[] = { 0x001D8, 0x00000 };
static unsigned int lc001D9[] = { 0x001DA, 0x00000 };
static unsigned int lc001DB[] = { 0x001DC, 0x00000 };
static unsigned int lc001DE[] = { 0x001DF, 0x00000 };
static unsigned int lc001E0[] = { 0x001E1, 0x00000 };
static unsigned int lc001E2[] = { 0x001E3, 0x00000 };
static unsigned int lc001E4[] = { 0x001E5, 0x00000 };
static unsigned int lc001E6[] = { 0x001E7, 0x00000 };
static unsigned int lc001E8[] = { 0x001E9, 0x00000 };
static unsigned int lc001EA[] = { 0x001EB, 0x00000 };
static unsigned int lc001EC[] = { 0x001ED, 0x00000 };
static unsigned int lc001EE[] = { 0x001EF, 0x00000 };
static unsigned int lc001F0[] = { 0x001F0, 0x00000 };
static unsigned int lc001F1[] = { 0x001F3, 0x00000 };
static unsigned int lc001F2[] = { 0x001F3, 0x00000 };
static unsigned int lc001F4[] = { 0x001F5, 0x00000 };
static unsigned int lc001F6[] = { 0x00195, 0x00000 };
static unsigned int lc001F7[] = { 0x001BF, 0x00000 };
static unsigned int lc001F8[] = { 0x001F9, 0x00000 };
static unsigned int lc001FA[] = { 0x001FB, 0x00000 };
static unsigned int lc001FC[] = { 0x001FD, 0x00000 };
static unsigned int lc001FE[] = { 0x001FF, 0x00000 };
static unsigned int lc00200[] = { 0x00201, 0x00000 };
static unsigned int lc00202[] = { 0x00203, 0x00000 };
static unsigned int lc00204[] = { 0x00205, 0x00000 };
static unsigned int lc00206[] = { 0x00207, 0x00000 };
static unsigned int lc00208[] = { 0x00209, 0x00000 };
static unsigned int lc0020A[] = { 0x0020B, 0x00000 };
static unsigned int lc0020C[] = { 0x0020D, 0x00000 };
static unsigned int lc0020E[] = { 0x0020F, 0x00000 };
static unsigned int lc00210[] = { 0x00211, 0x00000 };
static unsigned int lc00212[] = { 0x00213, 0x00000 };
static unsigned int lc00214[] = { 0x00215, 0x00000 };
static unsigned int lc00216[] = { 0x00217, 0x00000 };
static unsigned int lc00218[] = { 0x00219, 0x00000 };
static unsigned int lc0021A[] = { 0x0021B, 0x00000 };
static unsigned int lc0021C[] = { 0x0021D, 0x00000 };
static unsigned int lc0021E[] = { 0x0021F, 0x00000 };
static unsigned int lc00220[] = { 0x0019E, 0x00000 };
static unsigned int lc00222[] = { 0x00223, 0x00000 };
static unsigned int lc00224[] = { 0x00225, 0x00000 };
static unsigned int lc00226[] = { 0x00227, 0x00000 };
static unsigned int lc00228[] = { 0x00229, 0x00000 };
static unsigned int lc0022A[] = { 0x0022B, 0x00000 };
static unsigned int lc0022C[] = { 0x0022D, 0x00000 };
static unsigned int lc0022E[] = { 0x0022F, 0x00000 };
static unsigned int lc00230[] = { 0x00231, 0x00000 };
static unsigned int lc00232[] = { 0x00233, 0x00000 };
static unsigned int lc0023A[] = { 0x02C65, 0x00000 };
static unsigned int lc0023B[] = { 0x0023C, 0x00000 };
static unsigned int lc0023D[] = { 0x0019A, 0x00000 };
static unsigned int lc0023E[] = { 0x02C66, 0x00000 };
static unsigned int lc00241[] = { 0x00242, 0x00000 };
static unsigned int lc00243[] = { 0x00180, 0x00000 };
static unsigned int lc00244[] = { 0x00289, 0x00000 };
static unsigned int lc00245[] = { 0x0028C, 0x00000 };
static unsigned int lc00246[] = { 0x00247, 0x00000 };
static unsigned int lc00248[] = { 0x00249, 0x00000 };
static unsigned int lc0024A[] = { 0x0024B, 0x00000 };
static unsigned int lc0024C[] = { 0x0024D, 0x00000 };
static unsigned int lc0024E[] = { 0x0024F, 0x00000 };
static unsigned int lc00386[] = { 0x003AC, 0x00000 };
static unsigned int lc00388[] = { 0x003AD, 0x00000 };
static unsigned int lc00389[] = { 0x003AE, 0x00000 };
static unsigned int lc0038A[] = { 0x003AF, 0x00000 };
static unsigned int lc0038C[] = { 0x003CC, 0x00000 };
static unsigned int lc0038E[] = { 0x003CD, 0x00000 };
static unsigned int lc0038F[] = { 0x003CE, 0x00000 };
static unsigned int lc00390[] = { 0x00390, 0x00000 };
static unsigned int lc00391[] = { 0x003B1, 0x00000 };
static unsigned int lc00392[] = { 0x003B2, 0x00000 };
static unsigned int lc00393[] = { 0x003B3, 0x00000 };
static unsigned int lc00394[] = { 0x003B4, 0x00000 };
static unsigned int lc00395[] = { 0x003B5, 0x00000 };
static unsigned int lc00396[] = { 0x003B6, 0x00000 };
static unsigned int lc00397[] = { 0x003B7, 0x00000 };
static unsigned int lc00398[] = { 0x003B8, 0x00000 };
static unsigned int lc00399[] = { 0x003B9, 0x00000 };
static unsigned int lc0039A[] = { 0x003BA, 0x00000 };
static unsigned int lc0039B[] = { 0x003BB, 0x00000 };
static unsigned int lc0039C[] = { 0x003BC, 0x00000 };
static unsigned int lc0039D[] = { 0x003BD, 0x00000 };
static unsigned int lc0039E[] = { 0x003BE, 0x00000 };
static unsigned int lc0039F[] = { 0x003BF, 0x00000 };
static unsigned int lc003A0[] = { 0x003C0, 0x00000 };
static unsigned int lc003A1[] = { 0x003C1, 0x00000 };
static unsigned int lc003A3[] = { 0x003C3, 0x00000 };
static unsigned int lc003A4[] = { 0x003C4, 0x00000 };
static unsigned int lc003A5[] = { 0x003C5, 0x00000 };
static unsigned int lc003A6[] = { 0x003C6, 0x00000 };
static unsigned int lc003A7[] = { 0x003C7, 0x00000 };
static unsigned int lc003A8[] = { 0x003C8, 0x00000 };
static unsigned int lc003A9[] = { 0x003C9, 0x00000 };
static unsigned int lc003AA[] = { 0x003CA, 0x00000 };
static unsigned int lc003AB[] = { 0x003CB, 0x00000 };
static unsigned int lc003B0[] = { 0x003B0, 0x00000 };
static unsigned int lc003D8[] = { 0x003D9, 0x00000 };
static unsigned int lc003DA[] = { 0x003DB, 0x00000 };
static unsigned int lc003DC[] = { 0x003DD, 0x00000 };
static unsigned int lc003DE[] = { 0x003DF, 0x00000 };
static unsigned int lc003E0[] = { 0x003E1, 0x00000 };
static unsigned int lc003E2[] = { 0x003E3, 0x00000 };
static unsigned int lc003E4[] = { 0x003E5, 0x00000 };
static unsigned int lc003E6[] = { 0x003E7, 0x00000 };
static unsigned int lc003E8[] = { 0x003E9, 0x00000 };
static unsigned int lc003EA[] = { 0x003EB, 0x00000 };
static unsigned int lc003EC[] = { 0x003ED, 0x00000 };
static unsigned int lc003EE[] = { 0x003EF, 0x00000 };
static unsigned int lc003F4[] = { 0x003B8, 0x00000 };
static unsigned int lc003F7[] = { 0x003F8, 0x00000 };
static unsigned int lc003F9[] = { 0x003F2, 0x00000 };
static unsigned int lc003FA[] = { 0x003FB, 0x00000 };
static unsigned int lc003FD[] = { 0x0037B, 0x00000 };
static unsigned int lc003FE[] = { 0x0037C, 0x00000 };
static unsigned int lc003FF[] = { 0x0037D, 0x00000 };
static unsigned int lc00400[] = { 0x00450, 0x00000 };
static unsigned int lc00401[] = { 0x00451, 0x00000 };
static unsigned int lc00402[] = { 0x00452, 0x00000 };
static unsigned int lc00403[] = { 0x00453, 0x00000 };
static unsigned int lc00404[] = { 0x00454, 0x00000 };
static unsigned int lc00405[] = { 0x00455, 0x00000 };
static unsigned int lc00406[] = { 0x00456, 0x00000 };
static unsigned int lc00407[] = { 0x00457, 0x00000 };
static unsigned int lc00408[] = { 0x00458, 0x00000 };
static unsigned int lc00409[] = { 0x00459, 0x00000 };
static unsigned int lc0040A[] = { 0x0045A, 0x00000 };
static unsigned int lc0040B[] = { 0x0045B, 0x00000 };
static unsigned int lc0040C[] = { 0x0045C, 0x00000 };
static unsigned int lc0040D[] = { 0x0045D, 0x00000 };
static unsigned int lc0040E[] = { 0x0045E, 0x00000 };
static unsigned int lc0040F[] = { 0x0045F, 0x00000 };
static unsigned int lc00410[] = { 0x00430, 0x00000 };
static unsigned int lc00411[] = { 0x00431, 0x00000 };
static unsigned int lc00412[] = { 0x00432, 0x00000 };
static unsigned int lc00413[] = { 0x00433, 0x00000 };
static unsigned int lc00414[] = { 0x00434, 0x00000 };
static unsigned int lc00415[] = { 0x00435, 0x00000 };
static unsigned int lc00416[] = { 0x00436, 0x00000 };
static unsigned int lc00417[] = { 0x00437, 0x00000 };
static unsigned int lc00418[] = { 0x00438, 0x00000 };
static unsigned int lc00419[] = { 0x00439, 0x00000 };
static unsigned int lc0041A[] = { 0x0043A, 0x00000 };
static unsigned int lc0041B[] = { 0x0043B, 0x00000 };
static unsigned int lc0041C[] = { 0x0043C, 0x00000 };
static unsigned int lc0041D[] = { 0x0043D, 0x00000 };
static unsigned int lc0041E[] = { 0x0043E, 0x00000 };
static unsigned int lc0041F[] = { 0x0043F, 0x00000 };
static unsigned int lc00420[] = { 0x00440, 0x00000 };
static unsigned int lc00421[] = { 0x00441, 0x00000 };
static unsigned int lc00422[] = { 0x00442, 0x00000 };
static unsigned int lc00423[] = { 0x00443, 0x00000 };
static unsigned int lc00424[] = { 0x00444, 0x00000 };
static unsigned int lc00425[] = { 0x00445, 0x00000 };
static unsigned int lc00426[] = { 0x00446, 0x00000 };
static unsigned int lc00427[] = { 0x00447, 0x00000 };
static unsigned int lc00428[] = { 0x00448, 0x00000 };
static unsigned int lc00429[] = { 0x00449, 0x00000 };
static unsigned int lc0042A[] = { 0x0044A, 0x00000 };
static unsigned int lc0042B[] = { 0x0044B, 0x00000 };
static unsigned int lc0042C[] = { 0x0044C, 0x00000 };
static unsigned int lc0042D[] = { 0x0044D, 0x00000 };
static unsigned int lc0042E[] = { 0x0044E, 0x00000 };
static unsigned int lc0042F[] = { 0x0044F, 0x00000 };
static unsigned int lc00460[] = { 0x00461, 0x00000 };
static unsigned int lc00462[] = { 0x00463, 0x00000 };
static unsigned int lc00464[] = { 0x00465, 0x00000 };
static unsigned int lc00466[] = { 0x00467, 0x00000 };
static unsigned int lc00468[] = { 0x00469, 0x00000 };
static unsigned int lc0046A[] = { 0x0046B, 0x00000 };
static unsigned int lc0046C[] = { 0x0046D, 0x00000 };
static unsigned int lc0046E[] = { 0x0046F, 0x00000 };
static unsigned int lc00470[] = { 0x00471, 0x00000 };
static unsigned int lc00472[] = { 0x00473, 0x00000 };
static unsigned int lc00474[] = { 0x00475, 0x00000 };
static unsigned int lc00476[] = { 0x00477, 0x00000 };
static unsigned int lc00478[] = { 0x00479, 0x00000 };
static unsigned int lc0047A[] = { 0x0047B, 0x00000 };
static unsigned int lc0047C[] = { 0x0047D, 0x00000 };
static unsigned int lc0047E[] = { 0x0047F, 0x00000 };
static unsigned int lc00480[] = { 0x00481, 0x00000 };
static unsigned int lc0048A[] = { 0x0048B, 0x00000 };
static unsigned int lc0048C[] = { 0x0048D, 0x00000 };
static unsigned int lc0048E[] = { 0x0048F, 0x00000 };
static unsigned int lc00490[] = { 0x00491, 0x00000 };
static unsigned int lc00492[] = { 0x00493, 0x00000 };
static unsigned int lc00494[] = { 0x00495, 0x00000 };
static unsigned int lc00496[] = { 0x00497, 0x00000 };
static unsigned int lc00498[] = { 0x00499, 0x00000 };
static unsigned int lc0049A[] = { 0x0049B, 0x00000 };
static unsigned int lc0049C[] = { 0x0049D, 0x00000 };
static unsigned int lc0049E[] = { 0x0049F, 0x00000 };
static unsigned int lc004A0[] = { 0x004A1, 0x00000 };
static unsigned int lc004A2[] = { 0x004A3, 0x00000 };
static unsigned int lc004A4[] = { 0x004A5, 0x00000 };
static unsigned int lc004A6[] = { 0x004A7, 0x00000 };
static unsigned int lc004A8[] = { 0x004A9, 0x00000 };
static unsigned int lc004AA[] = { 0x004AB, 0x00000 };
static unsigned int lc004AC[] = { 0x004AD, 0x00000 };
static unsigned int lc004AE[] = { 0x004AF, 0x00000 };
static unsigned int lc004B0[] = { 0x004B1, 0x00000 };
static unsigned int lc004B2[] = { 0x004B3, 0x00000 };
static unsigned int lc004B4[] = { 0x004B5, 0x00000 };
static unsigned int lc004B6[] = { 0x004B7, 0x00000 };
static unsigned int lc004B8[] = { 0x004B9, 0x00000 };
static unsigned int lc004BA[] = { 0x004BB, 0x00000 };
static unsigned int lc004BC[] = { 0x004BD, 0x00000 };
static unsigned int lc004BE[] = { 0x004BF, 0x00000 };
static unsigned int lc004C0[] = { 0x004CF, 0x00000 };
static unsigned int lc004C1[] = { 0x004C2, 0x00000 };
static unsigned int lc004C3[] = { 0x004C4, 0x00000 };
static unsigned int lc004C5[] = { 0x004C6, 0x00000 };
static unsigned int lc004C7[] = { 0x004C8, 0x00000 };
static unsigned int lc004C9[] = { 0x004CA, 0x00000 };
static unsigned int lc004CB[] = { 0x004CC, 0x00000 };
static unsigned int lc004CD[] = { 0x004CE, 0x00000 };
static unsigned int lc004D0[] = { 0x004D1, 0x00000 };
static unsigned int lc004D2[] = { 0x004D3, 0x00000 };
static unsigned int lc004D4[] = { 0x004D5, 0x00000 };
static unsigned int lc004D6[] = { 0x004D7, 0x00000 };
static unsigned int lc004D8[] = { 0x004D9, 0x00000 };
static unsigned int lc004DA[] = { 0x004DB, 0x00000 };
static unsigned int lc004DC[] = { 0x004DD, 0x00000 };
static unsigned int lc004DE[] = { 0x004DF, 0x00000 };
static unsigned int lc004E0[] = { 0x004E1, 0x00000 };
static unsigned int lc004E2[] = { 0x004E3, 0x00000 };
static unsigned int lc004E4[] = { 0x004E5, 0x00000 };
static unsigned int lc004E6[] = { 0x004E7, 0x00000 };
static unsigned int lc004E8[] = { 0x004E9, 0x00000 };
static unsigned int lc004EA[] = { 0x004EB, 0x00000 };
static unsigned int lc004EC[] = { 0x004ED, 0x00000 };
static unsigned int lc004EE[] = { 0x004EF, 0x00000 };
static unsigned int lc004F0[] = { 0x004F1, 0x00000 };
static unsigned int lc004F2[] = { 0x004F3, 0x00000 };
static unsigned int lc004F4[] = { 0x004F5, 0x00000 };
static unsigned int lc004F6[] = { 0x004F7, 0x00000 };
static unsigned int lc004F8[] = { 0x004F9, 0x00000 };
static unsigned int lc004FA[] = { 0x004FB, 0x00000 };
static unsigned int lc004FC[] = { 0x004FD, 0x00000 };
static unsigned int lc004FE[] = { 0x004FF, 0x00000 };
static unsigned int lc00500[] = { 0x00501, 0x00000 };
static unsigned int lc00502[] = { 0x00503, 0x00000 };
static unsigned int lc00504[] = { 0x00505, 0x00000 };
static unsigned int lc00506[] = { 0x00507, 0x00000 };
static unsigned int lc00508[] = { 0x00509, 0x00000 };
static unsigned int lc0050A[] = { 0x0050B, 0x00000 };
static unsigned int lc0050C[] = { 0x0050D, 0x00000 };
static unsigned int lc0050E[] = { 0x0050F, 0x00000 };
static unsigned int lc00510[] = { 0x00511, 0x00000 };
static unsigned int lc00512[] = { 0x00513, 0x00000 };
static unsigned int lc00531[] = { 0x00561, 0x00000 };
static unsigned int lc00532[] = { 0x00562, 0x00000 };
static unsigned int lc00533[] = { 0x00563, 0x00000 };
static unsigned int lc00534[] = { 0x00564, 0x00000 };
static unsigned int lc00535[] = { 0x00565, 0x00000 };
static unsigned int lc00536[] = { 0x00566, 0x00000 };
static unsigned int lc00537[] = { 0x00567, 0x00000 };
static unsigned int lc00538[] = { 0x00568, 0x00000 };
static unsigned int lc00539[] = { 0x00569, 0x00000 };
static unsigned int lc0053A[] = { 0x0056A, 0x00000 };
static unsigned int lc0053B[] = { 0x0056B, 0x00000 };
static unsigned int lc0053C[] = { 0x0056C, 0x00000 };
static unsigned int lc0053D[] = { 0x0056D, 0x00000 };
static unsigned int lc0053E[] = { 0x0056E, 0x00000 };
static unsigned int lc0053F[] = { 0x0056F, 0x00000 };
static unsigned int lc00540[] = { 0x00570, 0x00000 };
static unsigned int lc00541[] = { 0x00571, 0x00000 };
static unsigned int lc00542[] = { 0x00572, 0x00000 };
static unsigned int lc00543[] = { 0x00573, 0x00000 };
static unsigned int lc00544[] = { 0x00574, 0x00000 };
static unsigned int lc00545[] = { 0x00575, 0x00000 };
static unsigned int lc00546[] = { 0x00576, 0x00000 };
static unsigned int lc00547[] = { 0x00577, 0x00000 };
static unsigned int lc00548[] = { 0x00578, 0x00000 };
static unsigned int lc00549[] = { 0x00579, 0x00000 };
static unsigned int lc0054A[] = { 0x0057A, 0x00000 };
static unsigned int lc0054B[] = { 0x0057B, 0x00000 };
static unsigned int lc0054C[] = { 0x0057C, 0x00000 };
static unsigned int lc0054D[] = { 0x0057D, 0x00000 };
static unsigned int lc0054E[] = { 0x0057E, 0x00000 };
static unsigned int lc0054F[] = { 0x0057F, 0x00000 };
static unsigned int lc00550[] = { 0x00580, 0x00000 };
static unsigned int lc00551[] = { 0x00581, 0x00000 };
static unsigned int lc00552[] = { 0x00582, 0x00000 };
static unsigned int lc00553[] = { 0x00583, 0x00000 };
static unsigned int lc00554[] = { 0x00584, 0x00000 };
static unsigned int lc00555[] = { 0x00585, 0x00000 };
static unsigned int lc00556[] = { 0x00586, 0x00000 };
static unsigned int lc00587[] = { 0x00587, 0x00000 };
static unsigned int lc010A0[] = { 0x02D00, 0x00000 };
static unsigned int lc010A1[] = { 0x02D01, 0x00000 };
static unsigned int lc010A2[] = { 0x02D02, 0x00000 };
static unsigned int lc010A3[] = { 0x02D03, 0x00000 };
static unsigned int lc010A4[] = { 0x02D04, 0x00000 };
static unsigned int lc010A5[] = { 0x02D05, 0x00000 };
static unsigned int lc010A6[] = { 0x02D06, 0x00000 };
static unsigned int lc010A7[] = { 0x02D07, 0x00000 };
static unsigned int lc010A8[] = { 0x02D08, 0x00000 };
static unsigned int lc010A9[] = { 0x02D09, 0x00000 };
static unsigned int lc010AA[] = { 0x02D0A, 0x00000 };
static unsigned int lc010AB[] = { 0x02D0B, 0x00000 };
static unsigned int lc010AC[] = { 0x02D0C, 0x00000 };
static unsigned int lc010AD[] = { 0x02D0D, 0x00000 };
static unsigned int lc010AE[] = { 0x02D0E, 0x00000 };
static unsigned int lc010AF[] = { 0x02D0F, 0x00000 };
static unsigned int lc010B0[] = { 0x02D10, 0x00000 };
static unsigned int lc010B1[] = { 0x02D11, 0x00000 };
static unsigned int lc010B2[] = { 0x02D12, 0x00000 };
static unsigned int lc010B3[] = { 0x02D13, 0x00000 };
static unsigned int lc010B4[] = { 0x02D14, 0x00000 };
static unsigned int lc010B5[] = { 0x02D15, 0x00000 };
static unsigned int lc010B6[] = { 0x02D16, 0x00000 };
static unsigned int lc010B7[] = { 0x02D17, 0x00000 };
static unsigned int lc010B8[] = { 0x02D18, 0x00000 };
static unsigned int lc010B9[] = { 0x02D19, 0x00000 };
static unsigned int lc010BA[] = { 0x02D1A, 0x00000 };
static unsigned int lc010BB[] = { 0x02D1B, 0x00000 };
static unsigned int lc010BC[] = { 0x02D1C, 0x00000 };
static unsigned int lc010BD[] = { 0x02D1D, 0x00000 };
static unsigned int lc010BE[] = { 0x02D1E, 0x00000 };
static unsigned int lc010BF[] = { 0x02D1F, 0x00000 };
static unsigned int lc010C0[] = { 0x02D20, 0x00000 };
static unsigned int lc010C1[] = { 0x02D21, 0x00000 };
static unsigned int lc010C2[] = { 0x02D22, 0x00000 };
static unsigned int lc010C3[] = { 0x02D23, 0x00000 };
static unsigned int lc010C4[] = { 0x02D24, 0x00000 };
static unsigned int lc010C5[] = { 0x02D25, 0x00000 };
static unsigned int lc01E00[] = { 0x01E01, 0x00000 };
static unsigned int lc01E02[] = { 0x01E03, 0x00000 };
static unsigned int lc01E04[] = { 0x01E05, 0x00000 };
static unsigned int lc01E06[] = { 0x01E07, 0x00000 };
static unsigned int lc01E08[] = { 0x01E09, 0x00000 };
static unsigned int lc01E0A[] = { 0x01E0B, 0x00000 };
static unsigned int lc01E0C[] = { 0x01E0D, 0x00000 };
static unsigned int lc01E0E[] = { 0x01E0F, 0x00000 };
static unsigned int lc01E10[] = { 0x01E11, 0x00000 };
static unsigned int lc01E12[] = { 0x01E13, 0x00000 };
static unsigned int lc01E14[] = { 0x01E15, 0x00000 };
static unsigned int lc01E16[] = { 0x01E17, 0x00000 };
static unsigned int lc01E18[] = { 0x01E19, 0x00000 };
static unsigned int lc01E1A[] = { 0x01E1B, 0x00000 };
static unsigned int lc01E1C[] = { 0x01E1D, 0x00000 };
static unsigned int lc01E1E[] = { 0x01E1F, 0x00000 };
static unsigned int lc01E20[] = { 0x01E21, 0x00000 };
static unsigned int lc01E22[] = { 0x01E23, 0x00000 };
static unsigned int lc01E24[] = { 0x01E25, 0x00000 };
static unsigned int lc01E26[] = { 0x01E27, 0x00000 };
static unsigned int lc01E28[] = { 0x01E29, 0x00000 };
static unsigned int lc01E2A[] = { 0x01E2B, 0x00000 };
static unsigned int lc01E2C[] = { 0x01E2D, 0x00000 };
static unsigned int lc01E2E[] = { 0x01E2F, 0x00000 };
static unsigned int lc01E30[] = { 0x01E31, 0x00000 };
static unsigned int lc01E32[] = { 0x01E33, 0x00000 };
static unsigned int lc01E34[] = { 0x01E35, 0x00000 };
static unsigned int lc01E36[] = { 0x01E37, 0x00000 };
static unsigned int lc01E38[] = { 0x01E39, 0x00000 };
static unsigned int lc01E3A[] = { 0x01E3B, 0x00000 };
static unsigned int lc01E3C[] = { 0x01E3D, 0x00000 };
static unsigned int lc01E3E[] = { 0x01E3F, 0x00000 };
static unsigned int lc01E40[] = { 0x01E41, 0x00000 };
static unsigned int lc01E42[] = { 0x01E43, 0x00000 };
static unsigned int lc01E44[] = { 0x01E45, 0x00000 };
static unsigned int lc01E46[] = { 0x01E47, 0x00000 };
static unsigned int lc01E48[] = { 0x01E49, 0x00000 };
static unsigned int lc01E4A[] = { 0x01E4B, 0x00000 };
static unsigned int lc01E4C[] = { 0x01E4D, 0x00000 };
static unsigned int lc01E4E[] = { 0x01E4F, 0x00000 };
static unsigned int lc01E50[] = { 0x01E51, 0x00000 };
static unsigned int lc01E52[] = { 0x01E53, 0x00000 };
static unsigned int lc01E54[] = { 0x01E55, 0x00000 };
static unsigned int lc01E56[] = { 0x01E57, 0x00000 };
static unsigned int lc01E58[] = { 0x01E59, 0x00000 };
static unsigned int lc01E5A[] = { 0x01E5B, 0x00000 };
static unsigned int lc01E5C[] = { 0x01E5D, 0x00000 };
static unsigned int lc01E5E[] = { 0x01E5F, 0x00000 };
static unsigned int lc01E60[] = { 0x01E61, 0x00000 };
static unsigned int lc01E62[] = { 0x01E63, 0x00000 };
static unsigned int lc01E64[] = { 0x01E65, 0x00000 };
static unsigned int lc01E66[] = { 0x01E67, 0x00000 };
static unsigned int lc01E68[] = { 0x01E69, 0x00000 };
static unsigned int lc01E6A[] = { 0x01E6B, 0x00000 };
static unsigned int lc01E6C[] = { 0x01E6D, 0x00000 };
static unsigned int lc01E6E[] = { 0x01E6F, 0x00000 };
static unsigned int lc01E70[] = { 0x01E71, 0x00000 };
static unsigned int lc01E72[] = { 0x01E73, 0x00000 };
static unsigned int lc01E74[] = { 0x01E75, 0x00000 };
static unsigned int lc01E76[] = { 0x01E77, 0x00000 };
static unsigned int lc01E78[] = { 0x01E79, 0x00000 };
static unsigned int lc01E7A[] = { 0x01E7B, 0x00000 };
static unsigned int lc01E7C[] = { 0x01E7D, 0x00000 };
static unsigned int lc01E7E[] = { 0x01E7F, 0x00000 };
static unsigned int lc01E80[] = { 0x01E81, 0x00000 };
static unsigned int lc01E82[] = { 0x01E83, 0x00000 };
static unsigned int lc01E84[] = { 0x01E85, 0x00000 };
static unsigned int lc01E86[] = { 0x01E87, 0x00000 };
static unsigned int lc01E88[] = { 0x01E89, 0x00000 };
static unsigned int lc01E8A[] = { 0x01E8B, 0x00000 };
static unsigned int lc01E8C[] = { 0x01E8D, 0x00000 };
static unsigned int lc01E8E[] = { 0x01E8F, 0x00000 };
static unsigned int lc01E90[] = { 0x01E91, 0x00000 };
static unsigned int lc01E92[] = { 0x01E93, 0x00000 };
static unsigned int lc01E94[] = { 0x01E95, 0x00000 };
static unsigned int lc01E96[] = { 0x01E96, 0x00000 };
static unsigned int lc01E97[] = { 0x01E97, 0x00000 };
static unsigned int lc01E98[] = { 0x01E98, 0x00000 };
static unsigned int lc01E99[] = { 0x01E99, 0x00000 };
static unsigned int lc01E9A[] = { 0x01E9A, 0x00000 };
static unsigned int lc01EA0[] = { 0x01EA1, 0x00000 };
static unsigned int lc01EA2[] = { 0x01EA3, 0x00000 };
static unsigned int lc01EA4[] = { 0x01EA5, 0x00000 };
static unsigned int lc01EA6[] = { 0x01EA7, 0x00000 };
static unsigned int lc01EA8[] = { 0x01EA9, 0x00000 };
static unsigned int lc01EAA[] = { 0x01EAB, 0x00000 };
static unsigned int lc01EAC[] = { 0x01EAD, 0x00000 };
static unsigned int lc01EAE[] = { 0x01EAF, 0x00000 };
static unsigned int lc01EB0[] = { 0x01EB1, 0x00000 };
static unsigned int lc01EB2[] = { 0x01EB3, 0x00000 };
static unsigned int lc01EB4[] = { 0x01EB5, 0x00000 };
static unsigned int lc01EB6[] = { 0x01EB7, 0x00000 };
static unsigned int lc01EB8[] = { 0x01EB9, 0x00000 };
static unsigned int lc01EBA[] = { 0x01EBB, 0x00000 };
static unsigned int lc01EBC[] = { 0x01EBD, 0x00000 };
static unsigned int lc01EBE[] = { 0x01EBF, 0x00000 };
static unsigned int lc01EC0[] = { 0x01EC1, 0x00000 };
static unsigned int lc01EC2[] = { 0x01EC3, 0x00000 };
static unsigned int lc01EC4[] = { 0x01EC5, 0x00000 };
static unsigned int lc01EC6[] = { 0x01EC7, 0x00000 };
static unsigned int lc01EC8[] = { 0x01EC9, 0x00000 };
static unsigned int lc01ECA[] = { 0x01ECB, 0x00000 };
static unsigned int lc01ECC[] = { 0x01ECD, 0x00000 };
static unsigned int lc01ECE[] = { 0x01ECF, 0x00000 };
static unsigned int lc01ED0[] = { 0x01ED1, 0x00000 };
static unsigned int lc01ED2[] = { 0x01ED3, 0x00000 };
static unsigned int lc01ED4[] = { 0x01ED5, 0x00000 };
static unsigned int lc01ED6[] = { 0x01ED7, 0x00000 };
static unsigned int lc01ED8[] = { 0x01ED9, 0x00000 };
static unsigned int lc01EDA[] = { 0x01EDB, 0x00000 };
static unsigned int lc01EDC[] = { 0x01EDD, 0x00000 };
static unsigned int lc01EDE[] = { 0x01EDF, 0x00000 };
static unsigned int lc01EE0[] = { 0x01EE1, 0x00000 };
static unsigned int lc01EE2[] = { 0x01EE3, 0x00000 };
static unsigned int lc01EE4[] = { 0x01EE5, 0x00000 };
static unsigned int lc01EE6[] = { 0x01EE7, 0x00000 };
static unsigned int lc01EE8[] = { 0x01EE9, 0x00000 };
static unsigned int lc01EEA[] = { 0x01EEB, 0x00000 };
static unsigned int lc01EEC[] = { 0x01EED, 0x00000 };
static unsigned int lc01EEE[] = { 0x01EEF, 0x00000 };
static unsigned int lc01EF0[] = { 0x01EF1, 0x00000 };
static unsigned int lc01EF2[] = { 0x01EF3, 0x00000 };
static unsigned int lc01EF4[] = { 0x01EF5, 0x00000 };
static unsigned int lc01EF6[] = { 0x01EF7, 0x00000 };
static unsigned int lc01EF8[] = { 0x01EF9, 0x00000 };
static unsigned int lc01F08[] = { 0x01F00, 0x00000 };
static unsigned int lc01F09[] = { 0x01F01, 0x00000 };
static unsigned int lc01F0A[] = { 0x01F02, 0x00000 };
static unsigned int lc01F0B[] = { 0x01F03, 0x00000 };
static unsigned int lc01F0C[] = { 0x01F04, 0x00000 };
static unsigned int lc01F0D[] = { 0x01F05, 0x00000 };
static unsigned int lc01F0E[] = { 0x01F06, 0x00000 };
static unsigned int lc01F0F[] = { 0x01F07, 0x00000 };
static unsigned int lc01F18[] = { 0x01F10, 0x00000 };
static unsigned int lc01F19[] = { 0x01F11, 0x00000 };
static unsigned int lc01F1A[] = { 0x01F12, 0x00000 };
static unsigned int lc01F1B[] = { 0x01F13, 0x00000 };
static unsigned int lc01F1C[] = { 0x01F14, 0x00000 };
static unsigned int lc01F1D[] = { 0x01F15, 0x00000 };
static unsigned int lc01F28[] = { 0x01F20, 0x00000 };
static unsigned int lc01F29[] = { 0x01F21, 0x00000 };
static unsigned int lc01F2A[] = { 0x01F22, 0x00000 };
static unsigned int lc01F2B[] = { 0x01F23, 0x00000 };
static unsigned int lc01F2C[] = { 0x01F24, 0x00000 };
static unsigned int lc01F2D[] = { 0x01F25, 0x00000 };
static unsigned int lc01F2E[] = { 0x01F26, 0x00000 };
static unsigned int lc01F2F[] = { 0x01F27, 0x00000 };
static unsigned int lc01F38[] = { 0x01F30, 0x00000 };
static unsigned int lc01F39[] = { 0x01F31, 0x00000 };
static unsigned int lc01F3A[] = { 0x01F32, 0x00000 };
static unsigned int lc01F3B[] = { 0x01F33, 0x00000 };
static unsigned int lc01F3C[] = { 0x01F34, 0x00000 };
static unsigned int lc01F3D[] = { 0x01F35, 0x00000 };
static unsigned int lc01F3E[] = { 0x01F36, 0x00000 };
static unsigned int lc01F3F[] = { 0x01F37, 0x00000 };
static unsigned int lc01F48[] = { 0x01F40, 0x00000 };
static unsigned int lc01F49[] = { 0x01F41, 0x00000 };
static unsigned int lc01F4A[] = { 0x01F42, 0x00000 };
static unsigned int lc01F4B[] = { 0x01F43, 0x00000 };
static unsigned int lc01F4C[] = { 0x01F44, 0x00000 };
static unsigned int lc01F4D[] = { 0x01F45, 0x00000 };
static unsigned int lc01F50[] = { 0x01F50, 0x00000 };
static unsigned int lc01F52[] = { 0x01F52, 0x00000 };
static unsigned int lc01F54[] = { 0x01F54, 0x00000 };
static unsigned int lc01F56[] = { 0x01F56, 0x00000 };
static unsigned int lc01F59[] = { 0x01F51, 0x00000 };
static unsigned int lc01F5B[] = { 0x01F53, 0x00000 };
static unsigned int lc01F5D[] = { 0x01F55, 0x00000 };
static unsigned int lc01F5F[] = { 0x01F57, 0x00000 };
static unsigned int lc01F68[] = { 0x01F60, 0x00000 };
static unsigned int lc01F69[] = { 0x01F61, 0x00000 };
static unsigned int lc01F6A[] = { 0x01F62, 0x00000 };
static unsigned int lc01F6B[] = { 0x01F63, 0x00000 };
static unsigned int lc01F6C[] = { 0x01F64, 0x00000 };
static unsigned int lc01F6D[] = { 0x01F65, 0x00000 };
static unsigned int lc01F6E[] = { 0x01F66, 0x00000 };
static unsigned int lc01F6F[] = { 0x01F67, 0x00000 };
static unsigned int lc01F80[] = { 0x01F80, 0x00000 };
static unsigned int lc01F81[] = { 0x01F81, 0x00000 };
static unsigned int lc01F82[] = { 0x01F82, 0x00000 };
static unsigned int lc01F83[] = { 0x01F83, 0x00000 };
static unsigned int lc01F84[] = { 0x01F84, 0x00000 };
static unsigned int lc01F85[] = { 0x01F85, 0x00000 };
static unsigned int lc01F86[] = { 0x01F86, 0x00000 };
static unsigned int lc01F87[] = { 0x01F87, 0x00000 };
static unsigned int lc01F88[] = { 0x01F80, 0x00000 };
static unsigned int lc01F89[] = { 0x01F81, 0x00000 };
static unsigned int lc01F8A[] = { 0x01F82, 0x00000 };
static unsigned int lc01F8B[] = { 0x01F83, 0x00000 };
static unsigned int lc01F8C[] = { 0x01F84, 0x00000 };
static unsigned int lc01F8D[] = { 0x01F85, 0x00000 };
static unsigned int lc01F8E[] = { 0x01F86, 0x00000 };
static unsigned int lc01F8F[] = { 0x01F87, 0x00000 };
static unsigned int lc01F90[] = { 0x01F90, 0x00000 };
static unsigned int lc01F91[] = { 0x01F91, 0x00000 };
static unsigned int lc01F92[] = { 0x01F92, 0x00000 };
static unsigned int lc01F93[] = { 0x01F93, 0x00000 };
static unsigned int lc01F94[] = { 0x01F94, 0x00000 };
static unsigned int lc01F95[] = { 0x01F95, 0x00000 };
static unsigned int lc01F96[] = { 0x01F96, 0x00000 };
static unsigned int lc01F97[] = { 0x01F97, 0x00000 };
static unsigned int lc01F98[] = { 0x01F90, 0x00000 };
static unsigned int lc01F99[] = { 0x01F91, 0x00000 };
static unsigned int lc01F9A[] = { 0x01F92, 0x00000 };
static unsigned int lc01F9B[] = { 0x01F93, 0x00000 };
static unsigned int lc01F9C[] = { 0x01F94, 0x00000 };
static unsigned int lc01F9D[] = { 0x01F95, 0x00000 };
static unsigned int lc01F9E[] = { 0x01F96, 0x00000 };
static unsigned int lc01F9F[] = { 0x01F97, 0x00000 };
static unsigned int lc01FA0[] = { 0x01FA0, 0x00000 };
static unsigned int lc01FA1[] = { 0x01FA1, 0x00000 };
static unsigned int lc01FA2[] = { 0x01FA2, 0x00000 };
static unsigned int lc01FA3[] = { 0x01FA3, 0x00000 };
static unsigned int lc01FA4[] = { 0x01FA4, 0x00000 };
static unsigned int lc01FA5[] = { 0x01FA5, 0x00000 };
static unsigned int lc01FA6[] = { 0x01FA6, 0x00000 };
static unsigned int lc01FA7[] = { 0x01FA7, 0x00000 };
static unsigned int lc01FA8[] = { 0x01FA0, 0x00000 };
static unsigned int lc01FA9[] = { 0x01FA1, 0x00000 };
static unsigned int lc01FAA[] = { 0x01FA2, 0x00000 };
static unsigned int lc01FAB[] = { 0x01FA3, 0x00000 };
static unsigned int lc01FAC[] = { 0x01FA4, 0x00000 };
static unsigned int lc01FAD[] = { 0x01FA5, 0x00000 };
static unsigned int lc01FAE[] = { 0x01FA6, 0x00000 };
static unsigned int lc01FAF[] = { 0x01FA7, 0x00000 };
static unsigned int lc01FB2[] = { 0x01FB2, 0x00000 };
static unsigned int lc01FB3[] = { 0x01FB3, 0x00000 };
static unsigned int lc01FB4[] = { 0x01FB4, 0x00000 };
static unsigned int lc01FB6[] = { 0x01FB6, 0x00000 };
static unsigned int lc01FB7[] = { 0x01FB7, 0x00000 };
static unsigned int lc01FB8[] = { 0x01FB0, 0x00000 };
static unsigned int lc01FB9[] = { 0x01FB1, 0x00000 };
static unsigned int lc01FBA[] = { 0x01F70, 0x00000 };
static unsigned int lc01FBB[] = { 0x01F71, 0x00000 };
static unsigned int lc01FBC[] = { 0x01FB3, 0x00000 };
static unsigned int lc01FC2[] = { 0x01FC2, 0x00000 };
static unsigned int lc01FC3[] = { 0x01FC3, 0x00000 };
static unsigned int lc01FC4[] = { 0x01FC4, 0x00000 };
static unsigned int lc01FC6[] = { 0x01FC6, 0x00000 };
static unsigned int lc01FC7[] = { 0x01FC7, 0x00000 };
static unsigned int lc01FC8[] = { 0x01F72, 0x00000 };
static unsigned int lc01FC9[] = { 0x01F73, 0x00000 };
static unsigned int lc01FCA[] = { 0x01F74, 0x00000 };
static unsigned int lc01FCB[] = { 0x01F75, 0x00000 };
static unsigned int lc01FCC[] = { 0x01FC3, 0x00000 };
static unsigned int lc01FD2[] = { 0x01FD2, 0x00000 };
static unsigned int lc01FD3[] = { 0x01FD3, 0x00000 };
static unsigned int lc01FD6[] = { 0x01FD6, 0x00000 };
static unsigned int lc01FD7[] = { 0x01FD7, 0x00000 };
static unsigned int lc01FD8[] = { 0x01FD0, 0x00000 };
static unsigned int lc01FD9[] = { 0x01FD1, 0x00000 };
static unsigned int lc01FDA[] = { 0x01F76, 0x00000 };
static unsigned int lc01FDB[] = { 0x01F77, 0x00000 };
static unsigned int lc01FE2[] = { 0x01FE2, 0x00000 };
static unsigned int lc01FE3[] = { 0x01FE3, 0x00000 };
static unsigned int lc01FE4[] = { 0x01FE4, 0x00000 };
static unsigned int lc01FE6[] = { 0x01FE6, 0x00000 };
static unsigned int lc01FE7[] = { 0x01FE7, 0x00000 };
static unsigned int lc01FE8[] = { 0x01FE0, 0x00000 };
static unsigned int lc01FE9[] = { 0x01FE1, 0x00000 };
static unsigned int lc01FEA[] = { 0x01F7A, 0x00000 };
static unsigned int lc01FEB[] = { 0x01F7B, 0x00000 };
static unsigned int lc01FEC[] = { 0x01FE5, 0x00000 };
static unsigned int lc01FF2[] = { 0x01FF2, 0x00000 };
static unsigned int lc01FF3[] = { 0x01FF3, 0x00000 };
static unsigned int lc01FF4[] = { 0x01FF4, 0x00000 };
static unsigned int lc01FF6[] = { 0x01FF6, 0x00000 };
static unsigned int lc01FF7[] = { 0x01FF7, 0x00000 };
static unsigned int lc01FF8[] = { 0x01F78, 0x00000 };
static unsigned int lc01FF9[] = { 0x01F79, 0x00000 };
static unsigned int lc01FFA[] = { 0x01F7C, 0x00000 };
static unsigned int lc01FFB[] = { 0x01F7D, 0x00000 };
static unsigned int lc01FFC[] = { 0x01FF3, 0x00000 };
static unsigned int lc02126[] = { 0x003C9, 0x00000 };
static unsigned int lc0212A[] = { 0x0006B, 0x00000 };
static unsigned int lc0212B[] = { 0x000E5, 0x00000 };
static unsigned int lc02132[] = { 0x0214E, 0x00000 };
static unsigned int lc02160[] = { 0x02170, 0x00000 };
static unsigned int lc02161[] = { 0x02171, 0x00000 };
static unsigned int lc02162[] = { 0x02172, 0x00000 };
static unsigned int lc02163[] = { 0x02173, 0x00000 };
static unsigned int lc02164[] = { 0x02174, 0x00000 };
static unsigned int lc02165[] = { 0x02175, 0x00000 };
static unsigned int lc02166[] = { 0x02176, 0x00000 };
static unsigned int lc02167[] = { 0x02177, 0x00000 };
static unsigned int lc02168[] = { 0x02178, 0x00000 };
static unsigned int lc02169[] = { 0x02179, 0x00000 };
static unsigned int lc0216A[] = { 0x0217A, 0x00000 };
static unsigned int lc0216B[] = { 0x0217B, 0x00000 };
static unsigned int lc0216C[] = { 0x0217C, 0x00000 };
static unsigned int lc0216D[] = { 0x0217D, 0x00000 };
static unsigned int lc0216E[] = { 0x0217E, 0x00000 };
static unsigned int lc0216F[] = { 0x0217F, 0x00000 };
static unsigned int lc02183[] = { 0x02184, 0x00000 };
static unsigned int lc024B6[] = { 0x024D0, 0x00000 };
static unsigned int lc024B7[] = { 0x024D1, 0x00000 };
static unsigned int lc024B8[] = { 0x024D2, 0x00000 };
static unsigned int lc024B9[] = { 0x024D3, 0x00000 };
static unsigned int lc024BA[] = { 0x024D4, 0x00000 };
static unsigned int lc024BB[] = { 0x024D5, 0x00000 };
static unsigned int lc024BC[] = { 0x024D6, 0x00000 };
static unsigned int lc024BD[] = { 0x024D7, 0x00000 };
static unsigned int lc024BE[] = { 0x024D8, 0x00000 };
static unsigned int lc024BF[] = { 0x024D9, 0x00000 };
static unsigned int lc024C0[] = { 0x024DA, 0x00000 };
static unsigned int lc024C1[] = { 0x024DB, 0x00000 };
static unsigned int lc024C2[] = { 0x024DC, 0x00000 };
static unsigned int lc024C3[] = { 0x024DD, 0x00000 };
static unsigned int lc024C4[] = { 0x024DE, 0x00000 };
static unsigned int lc024C5[] = { 0x024DF, 0x00000 };
static unsigned int lc024C6[] = { 0x024E0, 0x00000 };
static unsigned int lc024C7[] = { 0x024E1, 0x00000 };
static unsigned int lc024C8[] = { 0x024E2, 0x00000 };
static unsigned int lc024C9[] = { 0x024E3, 0x00000 };
static unsigned int lc024CA[] = { 0x024E4, 0x00000 };
static unsigned int lc024CB[] = { 0x024E5, 0x00000 };
static unsigned int lc024CC[] = { 0x024E6, 0x00000 };
static unsigned int lc024CD[] = { 0x024E7, 0x00000 };
static unsigned int lc024CE[] = { 0x024E8, 0x00000 };
static unsigned int lc024CF[] = { 0x024E9, 0x00000 };
static unsigned int lc02C00[] = { 0x02C30, 0x00000 };
static unsigned int lc02C01[] = { 0x02C31, 0x00000 };
static unsigned int lc02C02[] = { 0x02C32, 0x00000 };
static unsigned int lc02C03[] = { 0x02C33, 0x00000 };
static unsigned int lc02C04[] = { 0x02C34, 0x00000 };
static unsigned int lc02C05[] = { 0x02C35, 0x00000 };
static unsigned int lc02C06[] = { 0x02C36, 0x00000 };
static unsigned int lc02C07[] = { 0x02C37, 0x00000 };
static unsigned int lc02C08[] = { 0x02C38, 0x00000 };
static unsigned int lc02C09[] = { 0x02C39, 0x00000 };
static unsigned int lc02C0A[] = { 0x02C3A, 0x00000 };
static unsigned int lc02C0B[] = { 0x02C3B, 0x00000 };
static unsigned int lc02C0C[] = { 0x02C3C, 0x00000 };
static unsigned int lc02C0D[] = { 0x02C3D, 0x00000 };
static unsigned int lc02C0E[] = { 0x02C3E, 0x00000 };
static unsigned int lc02C0F[] = { 0x02C3F, 0x00000 };
static unsigned int lc02C10[] = { 0x02C40, 0x00000 };
static unsigned int lc02C11[] = { 0x02C41, 0x00000 };
static unsigned int lc02C12[] = { 0x02C42, 0x00000 };
static unsigned int lc02C13[] = { 0x02C43, 0x00000 };
static unsigned int lc02C14[] = { 0x02C44, 0x00000 };
static unsigned int lc02C15[] = { 0x02C45, 0x00000 };
static unsigned int lc02C16[] = { 0x02C46, 0x00000 };
static unsigned int lc02C17[] = { 0x02C47, 0x00000 };
static unsigned int lc02C18[] = { 0x02C48, 0x00000 };
static unsigned int lc02C19[] = { 0x02C49, 0x00000 };
static unsigned int lc02C1A[] = { 0x02C4A, 0x00000 };
static unsigned int lc02C1B[] = { 0x02C4B, 0x00000 };
static unsigned int lc02C1C[] = { 0x02C4C, 0x00000 };
static unsigned int lc02C1D[] = { 0x02C4D, 0x00000 };
static unsigned int lc02C1E[] = { 0x02C4E, 0x00000 };
static unsigned int lc02C1F[] = { 0x02C4F, 0x00000 };
static unsigned int lc02C20[] = { 0x02C50, 0x00000 };
static unsigned int lc02C21[] = { 0x02C51, 0x00000 };
static unsigned int lc02C22[] = { 0x02C52, 0x00000 };
static unsigned int lc02C23[] = { 0x02C53, 0x00000 };
static unsigned int lc02C24[] = { 0x02C54, 0x00000 };
static unsigned int lc02C25[] = { 0x02C55, 0x00000 };
static unsigned int lc02C26[] = { 0x02C56, 0x00000 };
static unsigned int lc02C27[] = { 0x02C57, 0x00000 };
static unsigned int lc02C28[] = { 0x02C58, 0x00000 };
static unsigned int lc02C29[] = { 0x02C59, 0x00000 };
static unsigned int lc02C2A[] = { 0x02C5A, 0x00000 };
static unsigned int lc02C2B[] = { 0x02C5B, 0x00000 };
static unsigned int lc02C2C[] = { 0x02C5C, 0x00000 };
static unsigned int lc02C2D[] = { 0x02C5D, 0x00000 };
static unsigned int lc02C2E[] = { 0x02C5E, 0x00000 };
static unsigned int lc02C60[] = { 0x02C61, 0x00000 };
static unsigned int lc02C62[] = { 0x0026B, 0x00000 };
static unsigned int lc02C63[] = { 0x01D7D, 0x00000 };
static unsigned int lc02C64[] = { 0x0027D, 0x00000 };
static unsigned int lc02C67[] = { 0x02C68, 0x00000 };
static unsigned int lc02C69[] = { 0x02C6A, 0x00000 };
static unsigned int lc02C6B[] = { 0x02C6C, 0x00000 };
static unsigned int lc02C75[] = { 0x02C76, 0x00000 };
static unsigned int lc02C80[] = { 0x02C81, 0x00000 };
static unsigned int lc02C82[] = { 0x02C83, 0x00000 };
static unsigned int lc02C84[] = { 0x02C85, 0x00000 };
static unsigned int lc02C86[] = { 0x02C87, 0x00000 };
static unsigned int lc02C88[] = { 0x02C89, 0x00000 };
static unsigned int lc02C8A[] = { 0x02C8B, 0x00000 };
static unsigned int lc02C8C[] = { 0x02C8D, 0x00000 };
static unsigned int lc02C8E[] = { 0x02C8F, 0x00000 };
static unsigned int lc02C90[] = { 0x02C91, 0x00000 };
static unsigned int lc02C92[] = { 0x02C93, 0x00000 };
static unsigned int lc02C94[] = { 0x02C95, 0x00000 };
static unsigned int lc02C96[] = { 0x02C97, 0x00000 };
static unsigned int lc02C98[] = { 0x02C99, 0x00000 };
static unsigned int lc02C9A[] = { 0x02C9B, 0x00000 };
static unsigned int lc02C9C[] = { 0x02C9D, 0x00000 };
static unsigned int lc02C9E[] = { 0x02C9F, 0x00000 };
static unsigned int lc02CA0[] = { 0x02CA1, 0x00000 };
static unsigned int lc02CA2[] = { 0x02CA3, 0x00000 };
static unsigned int lc02CA4[] = { 0x02CA5, 0x00000 };
static unsigned int lc02CA6[] = { 0x02CA7, 0x00000 };
static unsigned int lc02CA8[] = { 0x02CA9, 0x00000 };
static unsigned int lc02CAA[] = { 0x02CAB, 0x00000 };
static unsigned int lc02CAC[] = { 0x02CAD, 0x00000 };
static unsigned int lc02CAE[] = { 0x02CAF, 0x00000 };
static unsigned int lc02CB0[] = { 0x02CB1, 0x00000 };
static unsigned int lc02CB2[] = { 0x02CB3, 0x00000 };
static unsigned int lc02CB4[] = { 0x02CB5, 0x00000 };
static unsigned int lc02CB6[] = { 0x02CB7, 0x00000 };
static unsigned int lc02CB8[] = { 0x02CB9, 0x00000 };
static unsigned int lc02CBA[] = { 0x02CBB, 0x00000 };
static unsigned int lc02CBC[] = { 0x02CBD, 0x00000 };
static unsigned int lc02CBE[] = { 0x02CBF, 0x00000 };
static unsigned int lc02CC0[] = { 0x02CC1, 0x00000 };
static unsigned int lc02CC2[] = { 0x02CC3, 0x00000 };
static unsigned int lc02CC4[] = { 0x02CC5, 0x00000 };
static unsigned int lc02CC6[] = { 0x02CC7, 0x00000 };
static unsigned int lc02CC8[] = { 0x02CC9, 0x00000 };
static unsigned int lc02CCA[] = { 0x02CCB, 0x00000 };
static unsigned int lc02CCC[] = { 0x02CCD, 0x00000 };
static unsigned int lc02CCE[] = { 0x02CCF, 0x00000 };
static unsigned int lc02CD0[] = { 0x02CD1, 0x00000 };
static unsigned int lc02CD2[] = { 0x02CD3, 0x00000 };
static unsigned int lc02CD4[] = { 0x02CD5, 0x00000 };
static unsigned int lc02CD6[] = { 0x02CD7, 0x00000 };
static unsigned int lc02CD8[] = { 0x02CD9, 0x00000 };
static unsigned int lc02CDA[] = { 0x02CDB, 0x00000 };
static unsigned int lc02CDC[] = { 0x02CDD, 0x00000 };
static unsigned int lc02CDE[] = { 0x02CDF, 0x00000 };
static unsigned int lc02CE0[] = { 0x02CE1, 0x00000 };
static unsigned int lc02CE2[] = { 0x02CE3, 0x00000 };
static unsigned int lc0FB00[] = { 0x0FB00, 0x00000 };
static unsigned int lc0FB01[] = { 0x0FB01, 0x00000 };
static unsigned int lc0FB02[] = { 0x0FB02, 0x00000 };
static unsigned int lc0FB03[] = { 0x0FB03, 0x00000 };
static unsigned int lc0FB04[] = { 0x0FB04, 0x00000 };
static unsigned int lc0FB05[] = { 0x0FB05, 0x00000 };
static unsigned int lc0FB06[] = { 0x0FB06, 0x00000 };
static unsigned int lc0FB13[] = { 0x0FB13, 0x00000 };
static unsigned int lc0FB14[] = { 0x0FB14, 0x00000 };
static unsigned int lc0FB15[] = { 0x0FB15, 0x00000 };
static unsigned int lc0FB16[] = { 0x0FB16, 0x00000 };
static unsigned int lc0FB17[] = { 0x0FB17, 0x00000 };
static unsigned int lc0FF21[] = { 0x0FF41, 0x00000 };
static unsigned int lc0FF22[] = { 0x0FF42, 0x00000 };
static unsigned int lc0FF23[] = { 0x0FF43, 0x00000 };
static unsigned int lc0FF24[] = { 0x0FF44, 0x00000 };
static unsigned int lc0FF25[] = { 0x0FF45, 0x00000 };
static unsigned int lc0FF26[] = { 0x0FF46, 0x00000 };
static unsigned int lc0FF27[] = { 0x0FF47, 0x00000 };
static unsigned int lc0FF28[] = { 0x0FF48, 0x00000 };
static unsigned int lc0FF29[] = { 0x0FF49, 0x00000 };
static unsigned int lc0FF2A[] = { 0x0FF4A, 0x00000 };
static unsigned int lc0FF2B[] = { 0x0FF4B, 0x00000 };
static unsigned int lc0FF2C[] = { 0x0FF4C, 0x00000 };
static unsigned int lc0FF2D[] = { 0x0FF4D, 0x00000 };
static unsigned int lc0FF2E[] = { 0x0FF4E, 0x00000 };
static unsigned int lc0FF2F[] = { 0x0FF4F, 0x00000 };
static unsigned int lc0FF30[] = { 0x0FF50, 0x00000 };
static unsigned int lc0FF31[] = { 0x0FF51, 0x00000 };
static unsigned int lc0FF32[] = { 0x0FF52, 0x00000 };
static unsigned int lc0FF33[] = { 0x0FF53, 0x00000 };
static unsigned int lc0FF34[] = { 0x0FF54, 0x00000 };
static unsigned int lc0FF35[] = { 0x0FF55, 0x00000 };
static unsigned int lc0FF36[] = { 0x0FF56, 0x00000 };
static unsigned int lc0FF37[] = { 0x0FF57, 0x00000 };
static unsigned int lc0FF38[] = { 0x0FF58, 0x00000 };
static unsigned int lc0FF39[] = { 0x0FF59, 0x00000 };
static unsigned int lc0FF3A[] = { 0x0FF5A, 0x00000 };
static unsigned int lc10400[] = { 0x10428, 0x00000 };
static unsigned int lc10401[] = { 0x10429, 0x00000 };
static unsigned int lc10402[] = { 0x1042A, 0x00000 };
static unsigned int lc10403[] = { 0x1042B, 0x00000 };
static unsigned int lc10404[] = { 0x1042C, 0x00000 };
static unsigned int lc10405[] = { 0x1042D, 0x00000 };
static unsigned int lc10406[] = { 0x1042E, 0x00000 };
static unsigned int lc10407[] = { 0x1042F, 0x00000 };
static unsigned int lc10408[] = { 0x10430, 0x00000 };
static unsigned int lc10409[] = { 0x10431, 0x00000 };
static unsigned int lc1040A[] = { 0x10432, 0x00000 };
static unsigned int lc1040B[] = { 0x10433, 0x00000 };
static unsigned int lc1040C[] = { 0x10434, 0x00000 };
static unsigned int lc1040D[] = { 0x10435, 0x00000 };
static unsigned int lc1040E[] = { 0x10436, 0x00000 };
static unsigned int lc1040F[] = { 0x10437, 0x00000 };
static unsigned int lc10410[] = { 0x10438, 0x00000 };
static unsigned int lc10411[] = { 0x10439, 0x00000 };
static unsigned int lc10412[] = { 0x1043A, 0x00000 };
static unsigned int lc10413[] = { 0x1043B, 0x00000 };
static unsigned int lc10414[] = { 0x1043C, 0x00000 };
static unsigned int lc10415[] = { 0x1043D, 0x00000 };
static unsigned int lc10416[] = { 0x1043E, 0x00000 };
static unsigned int lc10417[] = { 0x1043F, 0x00000 };
static unsigned int lc10418[] = { 0x10440, 0x00000 };
static unsigned int lc10419[] = { 0x10441, 0x00000 };
static unsigned int lc1041A[] = { 0x10442, 0x00000 };
static unsigned int lc1041B[] = { 0x10443, 0x00000 };
static unsigned int lc1041C[] = { 0x10444, 0x00000 };
static unsigned int lc1041D[] = { 0x10445, 0x00000 };
static unsigned int lc1041E[] = { 0x10446, 0x00000 };
static unsigned int lc1041F[] = { 0x10447, 0x00000 };
static unsigned int lc10420[] = { 0x10448, 0x00000 };
static unsigned int lc10421[] = { 0x10449, 0x00000 };
static unsigned int lc10422[] = { 0x1044A, 0x00000 };
static unsigned int lc10423[] = { 0x1044B, 0x00000 };
static unsigned int lc10424[] = { 0x1044C, 0x00000 };
static unsigned int lc10425[] = { 0x1044D, 0x00000 };
static unsigned int lc10426[] = { 0x1044E, 0x00000 };
static unsigned int lc10427[] = { 0x1044F, 0x00000 };

static unsigned int lowerCaseDirectTableSize = 2048;
static unsigned int *lowerCaseDirectTable[2048] = {
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, lc00041, lc00042, lc00043, lc00044, lc00045, lc00046, lc00047, lc00048, lc00049, lc0004A, lc0004B, lc0004C, lc0004D, lc0004E, lc0004F,
  lc00050, lc00051, lc00052, lc00053, lc00054, lc00055, lc00056, lc00057, lc00058, lc00059, lc0005A,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  lc000C0, lc000C1, lc000C2, lc000C3, lc000C4, lc000C5, lc000C6, lc000C7, lc000C8, lc000C9, lc000CA, lc000CB, lc000CC, lc000CD, lc000CE, lc000CF,
  lc000D0, lc000D1, lc000D2, lc000D3, lc000D4, lc000D5, lc000D6,       0, lc000D8, lc000D9, lc000DA, lc000DB, lc000DC, lc000DD, lc000DE, lc000DF,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  lc00100,       0, lc00102,       0, lc00104,       0, lc00106,       0, lc00108,       0, lc0010A,       0, lc0010C,       0, lc0010E,       0,
  lc00110,       0, lc00112,       0, lc00114,       0, lc00116,       0, lc00118,       0, lc0011A,       0, lc0011C,       0, lc0011E,       0,
  lc00120,       0, lc00122,       0, lc00124,       0, lc00126,       0, lc00128,       0, lc0012A,       0, lc0012C,       0, lc0012E,       0,
  lc00130,       0, lc00132,       0, lc00134,       0, lc00136,       0,       0, lc00139,       0, lc0013B,       0, lc0013D,       0, lc0013F,
        0, lc00141,       0, lc00143,       0, lc00145,       0, lc00147,       0, lc00149, lc0014A,       0, lc0014C,       0, lc0014E,       0,
  lc00150,       0, lc00152,       0, lc00154,       0, lc00156,       0, lc00158,       0, lc0015A,       0, lc0015C,       0, lc0015E,       0,
  lc00160,       0, lc00162,       0, lc00164,       0, lc00166,       0, lc00168,       0, lc0016A,       0, lc0016C,       0, lc0016E,       0,
  lc00170,       0, lc00172,       0, lc00174,       0, lc00176,       0, lc00178, lc00179,       0, lc0017B,       0, lc0017D,       0,       0,
        0, lc00181, lc00182,       0, lc00184,       0, lc00186, lc00187,       0, lc00189, lc0018A, lc0018B,       0,       0, lc0018E, lc0018F,
  lc00190, lc00191,       0, lc00193, lc00194,       0, lc00196, lc00197, lc00198,       0,       0,       0, lc0019C, lc0019D,       0, lc0019F,
  lc001A0,       0, lc001A2,       0, lc001A4,       0, lc001A6, lc001A7,       0, lc001A9,       0,       0, lc001AC,       0, lc001AE, lc001AF,
        0, lc001B1, lc001B2, lc001B3,       0, lc001B5,       0, lc001B7, lc001B8,       0,       0,       0, lc001BC,       0,       0,       0,
        0,       0,       0,       0, lc001C4, lc001C5,       0, lc001C7, lc001C8,       0, lc001CA, lc001CB,       0, lc001CD,       0, lc001CF,
        0, lc001D1,       0, lc001D3,       0, lc001D5,       0, lc001D7,       0, lc001D9,       0, lc001DB,       0,       0, lc001DE,       0,
  lc001E0,       0, lc001E2,       0, lc001E4,       0, lc001E6,       0, lc001E8,       0, lc001EA,       0, lc001EC,       0, lc001EE,       0,
  lc001F0, lc001F1, lc001F2,       0, lc001F4,       0, lc001F6, lc001F7, lc001F8,       0, lc001FA,       0, lc001FC,       0, lc001FE,       0,
  lc00200,       0, lc00202,       0, lc00204,       0, lc00206,       0, lc00208,       0, lc0020A,       0, lc0020C,       0, lc0020E,       0,
  lc00210,       0, lc00212,       0, lc00214,       0, lc00216,       0, lc00218,       0, lc0021A,       0, lc0021C,       0, lc0021E,       0,
  lc00220,       0, lc00222,       0, lc00224,       0, lc00226,       0, lc00228,       0, lc0022A,       0, lc0022C,       0, lc0022E,       0,
  lc00230,       0, lc00232,       0,       0,       0,       0,       0,       0,       0, lc0023A, lc0023B,       0, lc0023D, lc0023E,       0,
        0, lc00241,       0, lc00243, lc00244, lc00245, lc00246,       0, lc00248,       0, lc0024A,       0, lc0024C,       0, lc0024E,       0,
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
        0,       0,       0,       0,       0,       0, lc00386,       0, lc00388, lc00389, lc0038A,       0, lc0038C,       0, lc0038E, lc0038F,
  lc00390, lc00391, lc00392, lc00393, lc00394, lc00395, lc00396, lc00397, lc00398, lc00399, lc0039A, lc0039B, lc0039C, lc0039D, lc0039E, lc0039F,
  lc003A0, lc003A1,       0, lc003A3, lc003A4, lc003A5, lc003A6, lc003A7, lc003A8, lc003A9, lc003AA, lc003AB,       0,       0,       0,       0,
  lc003B0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0, lc003D8,       0, lc003DA,       0, lc003DC,       0, lc003DE,       0,
  lc003E0,       0, lc003E2,       0, lc003E4,       0, lc003E6,       0, lc003E8,       0, lc003EA,       0, lc003EC,       0, lc003EE,       0,
        0,       0,       0,       0, lc003F4,       0,       0, lc003F7,       0, lc003F9, lc003FA,       0,       0, lc003FD, lc003FE, lc003FF,
  lc00400, lc00401, lc00402, lc00403, lc00404, lc00405, lc00406, lc00407, lc00408, lc00409, lc0040A, lc0040B, lc0040C, lc0040D, lc0040E, lc0040F,
  lc00410, lc00411, lc00412, lc00413, lc00414, lc00415, lc00416, lc00417, lc00418, lc00419, lc0041A, lc0041B, lc0041C, lc0041D, lc0041E, lc0041F,
  lc00420, lc00421, lc00422, lc00423, lc00424, lc00425, lc00426, lc00427, lc00428, lc00429, lc0042A, lc0042B, lc0042C, lc0042D, lc0042E, lc0042F,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  lc00460,       0, lc00462,       0, lc00464,       0, lc00466,       0, lc00468,       0, lc0046A,       0, lc0046C,       0, lc0046E,       0,
  lc00470,       0, lc00472,       0, lc00474,       0, lc00476,       0, lc00478,       0, lc0047A,       0, lc0047C,       0, lc0047E,       0,
  lc00480,       0,       0,       0,       0,       0,       0,       0,       0,       0, lc0048A,       0, lc0048C,       0, lc0048E,       0,
  lc00490,       0, lc00492,       0, lc00494,       0, lc00496,       0, lc00498,       0, lc0049A,       0, lc0049C,       0, lc0049E,       0,
  lc004A0,       0, lc004A2,       0, lc004A4,       0, lc004A6,       0, lc004A8,       0, lc004AA,       0, lc004AC,       0, lc004AE,       0,
  lc004B0,       0, lc004B2,       0, lc004B4,       0, lc004B6,       0, lc004B8,       0, lc004BA,       0, lc004BC,       0, lc004BE,       0,
  lc004C0, lc004C1,       0, lc004C3,       0, lc004C5,       0, lc004C7,       0, lc004C9,       0, lc004CB,       0, lc004CD,       0,       0,
  lc004D0,       0, lc004D2,       0, lc004D4,       0, lc004D6,       0, lc004D8,       0, lc004DA,       0, lc004DC,       0, lc004DE,       0,
  lc004E0,       0, lc004E2,       0, lc004E4,       0, lc004E6,       0, lc004E8,       0, lc004EA,       0, lc004EC,       0, lc004EE,       0,
  lc004F0,       0, lc004F2,       0, lc004F4,       0, lc004F6,       0, lc004F8,       0, lc004FA,       0, lc004FC,       0, lc004FE,       0,
  lc00500,       0, lc00502,       0, lc00504,       0, lc00506,       0, lc00508,       0, lc0050A,       0, lc0050C,       0, lc0050E,       0,
  lc00510,       0, lc00512,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, lc00531, lc00532, lc00533, lc00534, lc00535, lc00536, lc00537, lc00538, lc00539, lc0053A, lc0053B, lc0053C, lc0053D, lc0053E, lc0053F,
  lc00540, lc00541, lc00542, lc00543, lc00544, lc00545, lc00546, lc00547, lc00548, lc00549, lc0054A, lc0054B, lc0054C, lc0054D, lc0054E, lc0054F,
  lc00550, lc00551, lc00552, lc00553, lc00554, lc00555, lc00556,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0, lc00587,       0,       0,       0,       0,       0,       0,       0,       0,
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

struct CaseValue
{
  unsigned int ch;
  unsigned int *dest;
};
// static unsigned int lowerCaseTableSize = 997;
// static CaseValue lowerCaseTable[997] = {
static unsigned int lowerCaseTableSize = 541;
static CaseValue lowerCaseTable[541] = {
// { 0x00041, lc00041 }, { 0x00042, lc00042 }, { 0x00043, lc00043 }, { 0x00044, lc00044 }, { 0x00045, lc00045 }, { 0x00046, lc00046 }, { 0x00047, lc00047 }, { 0x00048, lc00048 },
// { 0x00049, lc00049 }, { 0x0004A, lc0004A }, { 0x0004B, lc0004B }, { 0x0004C, lc0004C }, { 0x0004D, lc0004D }, { 0x0004E, lc0004E }, { 0x0004F, lc0004F }, { 0x00050, lc00050 },
// { 0x00051, lc00051 }, { 0x00052, lc00052 }, { 0x00053, lc00053 }, { 0x00054, lc00054 }, { 0x00055, lc00055 }, { 0x00056, lc00056 }, { 0x00057, lc00057 }, { 0x00058, lc00058 },
// { 0x00059, lc00059 }, { 0x0005A, lc0005A }, { 0x000C0, lc000C0 }, { 0x000C1, lc000C1 }, { 0x000C2, lc000C2 }, { 0x000C3, lc000C3 }, { 0x000C4, lc000C4 }, { 0x000C5, lc000C5 },
// { 0x000C6, lc000C6 }, { 0x000C7, lc000C7 }, { 0x000C8, lc000C8 }, { 0x000C9, lc000C9 }, { 0x000CA, lc000CA }, { 0x000CB, lc000CB }, { 0x000CC, lc000CC }, { 0x000CD, lc000CD },
// { 0x000CE, lc000CE }, { 0x000CF, lc000CF }, { 0x000D0, lc000D0 }, { 0x000D1, lc000D1 }, { 0x000D2, lc000D2 }, { 0x000D3, lc000D3 }, { 0x000D4, lc000D4 }, { 0x000D5, lc000D5 },
// { 0x000D6, lc000D6 }, { 0x000D8, lc000D8 }, { 0x000D9, lc000D9 }, { 0x000DA, lc000DA }, { 0x000DB, lc000DB }, { 0x000DC, lc000DC }, { 0x000DD, lc000DD }, { 0x000DE, lc000DE },
// { 0x000DF, lc000DF }, { 0x00100, lc00100 }, { 0x00102, lc00102 }, { 0x00104, lc00104 }, { 0x00106, lc00106 }, { 0x00108, lc00108 }, { 0x0010A, lc0010A }, { 0x0010C, lc0010C },
// { 0x0010E, lc0010E }, { 0x00110, lc00110 }, { 0x00112, lc00112 }, { 0x00114, lc00114 }, { 0x00116, lc00116 }, { 0x00118, lc00118 }, { 0x0011A, lc0011A }, { 0x0011C, lc0011C },
// { 0x0011E, lc0011E }, { 0x00120, lc00120 }, { 0x00122, lc00122 }, { 0x00124, lc00124 }, { 0x00126, lc00126 }, { 0x00128, lc00128 }, { 0x0012A, lc0012A }, { 0x0012C, lc0012C },
// { 0x0012E, lc0012E }, { 0x00130, lc00130 }, { 0x00132, lc00132 }, { 0x00134, lc00134 }, { 0x00136, lc00136 }, { 0x00139, lc00139 }, { 0x0013B, lc0013B }, { 0x0013D, lc0013D },
// { 0x0013F, lc0013F }, { 0x00141, lc00141 }, { 0x00143, lc00143 }, { 0x00145, lc00145 }, { 0x00147, lc00147 }, { 0x00149, lc00149 }, { 0x0014A, lc0014A }, { 0x0014C, lc0014C },
// { 0x0014E, lc0014E }, { 0x00150, lc00150 }, { 0x00152, lc00152 }, { 0x00154, lc00154 }, { 0x00156, lc00156 }, { 0x00158, lc00158 }, { 0x0015A, lc0015A }, { 0x0015C, lc0015C },
// { 0x0015E, lc0015E }, { 0x00160, lc00160 }, { 0x00162, lc00162 }, { 0x00164, lc00164 }, { 0x00166, lc00166 }, { 0x00168, lc00168 }, { 0x0016A, lc0016A }, { 0x0016C, lc0016C },
// { 0x0016E, lc0016E }, { 0x00170, lc00170 }, { 0x00172, lc00172 }, { 0x00174, lc00174 }, { 0x00176, lc00176 }, { 0x00178, lc00178 }, { 0x00179, lc00179 }, { 0x0017B, lc0017B },
// { 0x0017D, lc0017D }, { 0x00181, lc00181 }, { 0x00182, lc00182 }, { 0x00184, lc00184 }, { 0x00186, lc00186 }, { 0x00187, lc00187 }, { 0x00189, lc00189 }, { 0x0018A, lc0018A },
// { 0x0018B, lc0018B }, { 0x0018E, lc0018E }, { 0x0018F, lc0018F }, { 0x00190, lc00190 }, { 0x00191, lc00191 }, { 0x00193, lc00193 }, { 0x00194, lc00194 }, { 0x00196, lc00196 },
// { 0x00197, lc00197 }, { 0x00198, lc00198 }, { 0x0019C, lc0019C }, { 0x0019D, lc0019D }, { 0x0019F, lc0019F }, { 0x001A0, lc001A0 }, { 0x001A2, lc001A2 }, { 0x001A4, lc001A4 },
// { 0x001A6, lc001A6 }, { 0x001A7, lc001A7 }, { 0x001A9, lc001A9 }, { 0x001AC, lc001AC }, { 0x001AE, lc001AE }, { 0x001AF, lc001AF }, { 0x001B1, lc001B1 }, { 0x001B2, lc001B2 },
// { 0x001B3, lc001B3 }, { 0x001B5, lc001B5 }, { 0x001B7, lc001B7 }, { 0x001B8, lc001B8 }, { 0x001BC, lc001BC }, { 0x001C4, lc001C4 }, { 0x001C5, lc001C5 }, { 0x001C7, lc001C7 },
// { 0x001C8, lc001C8 }, { 0x001CA, lc001CA }, { 0x001CB, lc001CB }, { 0x001CD, lc001CD }, { 0x001CF, lc001CF }, { 0x001D1, lc001D1 }, { 0x001D3, lc001D3 }, { 0x001D5, lc001D5 },
// { 0x001D7, lc001D7 }, { 0x001D9, lc001D9 }, { 0x001DB, lc001DB }, { 0x001DE, lc001DE }, { 0x001E0, lc001E0 }, { 0x001E2, lc001E2 }, { 0x001E4, lc001E4 }, { 0x001E6, lc001E6 },
// { 0x001E8, lc001E8 }, { 0x001EA, lc001EA }, { 0x001EC, lc001EC }, { 0x001EE, lc001EE }, { 0x001F0, lc001F0 }, { 0x001F1, lc001F1 }, { 0x001F2, lc001F2 }, { 0x001F4, lc001F4 },
// { 0x001F6, lc001F6 }, { 0x001F7, lc001F7 }, { 0x001F8, lc001F8 }, { 0x001FA, lc001FA }, { 0x001FC, lc001FC }, { 0x001FE, lc001FE }, { 0x00200, lc00200 }, { 0x00202, lc00202 },
// { 0x00204, lc00204 }, { 0x00206, lc00206 }, { 0x00208, lc00208 }, { 0x0020A, lc0020A }, { 0x0020C, lc0020C }, { 0x0020E, lc0020E }, { 0x00210, lc00210 }, { 0x00212, lc00212 },
// { 0x00214, lc00214 }, { 0x00216, lc00216 }, { 0x00218, lc00218 }, { 0x0021A, lc0021A }, { 0x0021C, lc0021C }, { 0x0021E, lc0021E }, { 0x00220, lc00220 }, { 0x00222, lc00222 },
// { 0x00224, lc00224 }, { 0x00226, lc00226 }, { 0x00228, lc00228 }, { 0x0022A, lc0022A }, { 0x0022C, lc0022C }, { 0x0022E, lc0022E }, { 0x00230, lc00230 }, { 0x00232, lc00232 },
// { 0x0023A, lc0023A }, { 0x0023B, lc0023B }, { 0x0023D, lc0023D }, { 0x0023E, lc0023E }, { 0x00241, lc00241 }, { 0x00243, lc00243 }, { 0x00244, lc00244 }, { 0x00245, lc00245 },
// { 0x00246, lc00246 }, { 0x00248, lc00248 }, { 0x0024A, lc0024A }, { 0x0024C, lc0024C }, { 0x0024E, lc0024E }, { 0x00386, lc00386 }, { 0x00388, lc00388 }, { 0x00389, lc00389 },
// { 0x0038A, lc0038A }, { 0x0038C, lc0038C }, { 0x0038E, lc0038E }, { 0x0038F, lc0038F }, { 0x00390, lc00390 }, { 0x00391, lc00391 }, { 0x00392, lc00392 }, { 0x00393, lc00393 },
// { 0x00394, lc00394 }, { 0x00395, lc00395 }, { 0x00396, lc00396 }, { 0x00397, lc00397 }, { 0x00398, lc00398 }, { 0x00399, lc00399 }, { 0x0039A, lc0039A }, { 0x0039B, lc0039B },
// { 0x0039C, lc0039C }, { 0x0039D, lc0039D }, { 0x0039E, lc0039E }, { 0x0039F, lc0039F }, { 0x003A0, lc003A0 }, { 0x003A1, lc003A1 }, { 0x003A3, lc003A3 }, { 0x003A4, lc003A4 },
// { 0x003A5, lc003A5 }, { 0x003A6, lc003A6 }, { 0x003A7, lc003A7 }, { 0x003A8, lc003A8 }, { 0x003A9, lc003A9 }, { 0x003AA, lc003AA }, { 0x003AB, lc003AB }, { 0x003B0, lc003B0 },
// { 0x003D8, lc003D8 }, { 0x003DA, lc003DA }, { 0x003DC, lc003DC }, { 0x003DE, lc003DE }, { 0x003E0, lc003E0 }, { 0x003E2, lc003E2 }, { 0x003E4, lc003E4 }, { 0x003E6, lc003E6 },
// { 0x003E8, lc003E8 }, { 0x003EA, lc003EA }, { 0x003EC, lc003EC }, { 0x003EE, lc003EE }, { 0x003F4, lc003F4 }, { 0x003F7, lc003F7 }, { 0x003F9, lc003F9 }, { 0x003FA, lc003FA },
// { 0x003FD, lc003FD }, { 0x003FE, lc003FE }, { 0x003FF, lc003FF }, { 0x00400, lc00400 }, { 0x00401, lc00401 }, { 0x00402, lc00402 }, { 0x00403, lc00403 }, { 0x00404, lc00404 },
// { 0x00405, lc00405 }, { 0x00406, lc00406 }, { 0x00407, lc00407 }, { 0x00408, lc00408 }, { 0x00409, lc00409 }, { 0x0040A, lc0040A }, { 0x0040B, lc0040B }, { 0x0040C, lc0040C },
// { 0x0040D, lc0040D }, { 0x0040E, lc0040E }, { 0x0040F, lc0040F }, { 0x00410, lc00410 }, { 0x00411, lc00411 }, { 0x00412, lc00412 }, { 0x00413, lc00413 }, { 0x00414, lc00414 },
// { 0x00415, lc00415 }, { 0x00416, lc00416 }, { 0x00417, lc00417 }, { 0x00418, lc00418 }, { 0x00419, lc00419 }, { 0x0041A, lc0041A }, { 0x0041B, lc0041B }, { 0x0041C, lc0041C },
// { 0x0041D, lc0041D }, { 0x0041E, lc0041E }, { 0x0041F, lc0041F }, { 0x00420, lc00420 }, { 0x00421, lc00421 }, { 0x00422, lc00422 }, { 0x00423, lc00423 }, { 0x00424, lc00424 },
// { 0x00425, lc00425 }, { 0x00426, lc00426 }, { 0x00427, lc00427 }, { 0x00428, lc00428 }, { 0x00429, lc00429 }, { 0x0042A, lc0042A }, { 0x0042B, lc0042B }, { 0x0042C, lc0042C },
// { 0x0042D, lc0042D }, { 0x0042E, lc0042E }, { 0x0042F, lc0042F }, { 0x00460, lc00460 }, { 0x00462, lc00462 }, { 0x00464, lc00464 }, { 0x00466, lc00466 }, { 0x00468, lc00468 },
// { 0x0046A, lc0046A }, { 0x0046C, lc0046C }, { 0x0046E, lc0046E }, { 0x00470, lc00470 }, { 0x00472, lc00472 }, { 0x00474, lc00474 }, { 0x00476, lc00476 }, { 0x00478, lc00478 },
// { 0x0047A, lc0047A }, { 0x0047C, lc0047C }, { 0x0047E, lc0047E }, { 0x00480, lc00480 }, { 0x0048A, lc0048A }, { 0x0048C, lc0048C }, { 0x0048E, lc0048E }, { 0x00490, lc00490 },
// { 0x00492, lc00492 }, { 0x00494, lc00494 }, { 0x00496, lc00496 }, { 0x00498, lc00498 }, { 0x0049A, lc0049A }, { 0x0049C, lc0049C }, { 0x0049E, lc0049E }, { 0x004A0, lc004A0 },
// { 0x004A2, lc004A2 }, { 0x004A4, lc004A4 }, { 0x004A6, lc004A6 }, { 0x004A8, lc004A8 }, { 0x004AA, lc004AA }, { 0x004AC, lc004AC }, { 0x004AE, lc004AE }, { 0x004B0, lc004B0 },
// { 0x004B2, lc004B2 }, { 0x004B4, lc004B4 }, { 0x004B6, lc004B6 }, { 0x004B8, lc004B8 }, { 0x004BA, lc004BA }, { 0x004BC, lc004BC }, { 0x004BE, lc004BE }, { 0x004C0, lc004C0 },
// { 0x004C1, lc004C1 }, { 0x004C3, lc004C3 }, { 0x004C5, lc004C5 }, { 0x004C7, lc004C7 }, { 0x004C9, lc004C9 }, { 0x004CB, lc004CB }, { 0x004CD, lc004CD }, { 0x004D0, lc004D0 },
// { 0x004D2, lc004D2 }, { 0x004D4, lc004D4 }, { 0x004D6, lc004D6 }, { 0x004D8, lc004D8 }, { 0x004DA, lc004DA }, { 0x004DC, lc004DC }, { 0x004DE, lc004DE }, { 0x004E0, lc004E0 },
// { 0x004E2, lc004E2 }, { 0x004E4, lc004E4 }, { 0x004E6, lc004E6 }, { 0x004E8, lc004E8 }, { 0x004EA, lc004EA }, { 0x004EC, lc004EC }, { 0x004EE, lc004EE }, { 0x004F0, lc004F0 },
// { 0x004F2, lc004F2 }, { 0x004F4, lc004F4 }, { 0x004F6, lc004F6 }, { 0x004F8, lc004F8 }, { 0x004FA, lc004FA }, { 0x004FC, lc004FC }, { 0x004FE, lc004FE }, { 0x00500, lc00500 },
// { 0x00502, lc00502 }, { 0x00504, lc00504 }, { 0x00506, lc00506 }, { 0x00508, lc00508 }, { 0x0050A, lc0050A }, { 0x0050C, lc0050C }, { 0x0050E, lc0050E }, { 0x00510, lc00510 },
// { 0x00512, lc00512 }, { 0x00531, lc00531 }, { 0x00532, lc00532 }, { 0x00533, lc00533 }, { 0x00534, lc00534 }, { 0x00535, lc00535 }, { 0x00536, lc00536 }, { 0x00537, lc00537 },
// { 0x00538, lc00538 }, { 0x00539, lc00539 }, { 0x0053A, lc0053A }, { 0x0053B, lc0053B }, { 0x0053C, lc0053C }, { 0x0053D, lc0053D }, { 0x0053E, lc0053E }, { 0x0053F, lc0053F },
// { 0x00540, lc00540 }, { 0x00541, lc00541 }, { 0x00542, lc00542 }, { 0x00543, lc00543 }, { 0x00544, lc00544 }, { 0x00545, lc00545 }, { 0x00546, lc00546 }, { 0x00547, lc00547 },
// { 0x00548, lc00548 }, { 0x00549, lc00549 }, { 0x0054A, lc0054A }, { 0x0054B, lc0054B }, { 0x0054C, lc0054C }, { 0x0054D, lc0054D }, { 0x0054E, lc0054E }, { 0x0054F, lc0054F },
// { 0x00550, lc00550 }, { 0x00551, lc00551 }, { 0x00552, lc00552 }, { 0x00553, lc00553 }, { 0x00554, lc00554 }, { 0x00555, lc00555 }, { 0x00556, lc00556 }, { 0x00587, lc00587 },
{ 0x010A0, lc010A0 }, { 0x010A1, lc010A1 }, { 0x010A2, lc010A2 }, { 0x010A3, lc010A3 }, { 0x010A4, lc010A4 }, { 0x010A5, lc010A5 }, { 0x010A6, lc010A6 }, { 0x010A7, lc010A7 },
{ 0x010A8, lc010A8 }, { 0x010A9, lc010A9 }, { 0x010AA, lc010AA }, { 0x010AB, lc010AB }, { 0x010AC, lc010AC }, { 0x010AD, lc010AD }, { 0x010AE, lc010AE }, { 0x010AF, lc010AF },
{ 0x010B0, lc010B0 }, { 0x010B1, lc010B1 }, { 0x010B2, lc010B2 }, { 0x010B3, lc010B3 }, { 0x010B4, lc010B4 }, { 0x010B5, lc010B5 }, { 0x010B6, lc010B6 }, { 0x010B7, lc010B7 },
{ 0x010B8, lc010B8 }, { 0x010B9, lc010B9 }, { 0x010BA, lc010BA }, { 0x010BB, lc010BB }, { 0x010BC, lc010BC }, { 0x010BD, lc010BD }, { 0x010BE, lc010BE }, { 0x010BF, lc010BF },
{ 0x010C0, lc010C0 }, { 0x010C1, lc010C1 }, { 0x010C2, lc010C2 }, { 0x010C3, lc010C3 }, { 0x010C4, lc010C4 }, { 0x010C5, lc010C5 }, { 0x01E00, lc01E00 }, { 0x01E02, lc01E02 },
{ 0x01E04, lc01E04 }, { 0x01E06, lc01E06 }, { 0x01E08, lc01E08 }, { 0x01E0A, lc01E0A }, { 0x01E0C, lc01E0C }, { 0x01E0E, lc01E0E }, { 0x01E10, lc01E10 }, { 0x01E12, lc01E12 },
{ 0x01E14, lc01E14 }, { 0x01E16, lc01E16 }, { 0x01E18, lc01E18 }, { 0x01E1A, lc01E1A }, { 0x01E1C, lc01E1C }, { 0x01E1E, lc01E1E }, { 0x01E20, lc01E20 }, { 0x01E22, lc01E22 },
{ 0x01E24, lc01E24 }, { 0x01E26, lc01E26 }, { 0x01E28, lc01E28 }, { 0x01E2A, lc01E2A }, { 0x01E2C, lc01E2C }, { 0x01E2E, lc01E2E }, { 0x01E30, lc01E30 }, { 0x01E32, lc01E32 },
{ 0x01E34, lc01E34 }, { 0x01E36, lc01E36 }, { 0x01E38, lc01E38 }, { 0x01E3A, lc01E3A }, { 0x01E3C, lc01E3C }, { 0x01E3E, lc01E3E }, { 0x01E40, lc01E40 }, { 0x01E42, lc01E42 },
{ 0x01E44, lc01E44 }, { 0x01E46, lc01E46 }, { 0x01E48, lc01E48 }, { 0x01E4A, lc01E4A }, { 0x01E4C, lc01E4C }, { 0x01E4E, lc01E4E }, { 0x01E50, lc01E50 }, { 0x01E52, lc01E52 },
{ 0x01E54, lc01E54 }, { 0x01E56, lc01E56 }, { 0x01E58, lc01E58 }, { 0x01E5A, lc01E5A }, { 0x01E5C, lc01E5C }, { 0x01E5E, lc01E5E }, { 0x01E60, lc01E60 }, { 0x01E62, lc01E62 },
{ 0x01E64, lc01E64 }, { 0x01E66, lc01E66 }, { 0x01E68, lc01E68 }, { 0x01E6A, lc01E6A }, { 0x01E6C, lc01E6C }, { 0x01E6E, lc01E6E }, { 0x01E70, lc01E70 }, { 0x01E72, lc01E72 },
{ 0x01E74, lc01E74 }, { 0x01E76, lc01E76 }, { 0x01E78, lc01E78 }, { 0x01E7A, lc01E7A }, { 0x01E7C, lc01E7C }, { 0x01E7E, lc01E7E }, { 0x01E80, lc01E80 }, { 0x01E82, lc01E82 },
{ 0x01E84, lc01E84 }, { 0x01E86, lc01E86 }, { 0x01E88, lc01E88 }, { 0x01E8A, lc01E8A }, { 0x01E8C, lc01E8C }, { 0x01E8E, lc01E8E }, { 0x01E90, lc01E90 }, { 0x01E92, lc01E92 },
{ 0x01E94, lc01E94 }, { 0x01E96, lc01E96 }, { 0x01E97, lc01E97 }, { 0x01E98, lc01E98 }, { 0x01E99, lc01E99 }, { 0x01E9A, lc01E9A }, { 0x01EA0, lc01EA0 }, { 0x01EA2, lc01EA2 },
{ 0x01EA4, lc01EA4 }, { 0x01EA6, lc01EA6 }, { 0x01EA8, lc01EA8 }, { 0x01EAA, lc01EAA }, { 0x01EAC, lc01EAC }, { 0x01EAE, lc01EAE }, { 0x01EB0, lc01EB0 }, { 0x01EB2, lc01EB2 },
{ 0x01EB4, lc01EB4 }, { 0x01EB6, lc01EB6 }, { 0x01EB8, lc01EB8 }, { 0x01EBA, lc01EBA }, { 0x01EBC, lc01EBC }, { 0x01EBE, lc01EBE }, { 0x01EC0, lc01EC0 }, { 0x01EC2, lc01EC2 },
{ 0x01EC4, lc01EC4 }, { 0x01EC6, lc01EC6 }, { 0x01EC8, lc01EC8 }, { 0x01ECA, lc01ECA }, { 0x01ECC, lc01ECC }, { 0x01ECE, lc01ECE }, { 0x01ED0, lc01ED0 }, { 0x01ED2, lc01ED2 },
{ 0x01ED4, lc01ED4 }, { 0x01ED6, lc01ED6 }, { 0x01ED8, lc01ED8 }, { 0x01EDA, lc01EDA }, { 0x01EDC, lc01EDC }, { 0x01EDE, lc01EDE }, { 0x01EE0, lc01EE0 }, { 0x01EE2, lc01EE2 },
{ 0x01EE4, lc01EE4 }, { 0x01EE6, lc01EE6 }, { 0x01EE8, lc01EE8 }, { 0x01EEA, lc01EEA }, { 0x01EEC, lc01EEC }, { 0x01EEE, lc01EEE }, { 0x01EF0, lc01EF0 }, { 0x01EF2, lc01EF2 },
{ 0x01EF4, lc01EF4 }, { 0x01EF6, lc01EF6 }, { 0x01EF8, lc01EF8 }, { 0x01F08, lc01F08 }, { 0x01F09, lc01F09 }, { 0x01F0A, lc01F0A }, { 0x01F0B, lc01F0B }, { 0x01F0C, lc01F0C },
{ 0x01F0D, lc01F0D }, { 0x01F0E, lc01F0E }, { 0x01F0F, lc01F0F }, { 0x01F18, lc01F18 }, { 0x01F19, lc01F19 }, { 0x01F1A, lc01F1A }, { 0x01F1B, lc01F1B }, { 0x01F1C, lc01F1C },
{ 0x01F1D, lc01F1D }, { 0x01F28, lc01F28 }, { 0x01F29, lc01F29 }, { 0x01F2A, lc01F2A }, { 0x01F2B, lc01F2B }, { 0x01F2C, lc01F2C }, { 0x01F2D, lc01F2D }, { 0x01F2E, lc01F2E },
{ 0x01F2F, lc01F2F }, { 0x01F38, lc01F38 }, { 0x01F39, lc01F39 }, { 0x01F3A, lc01F3A }, { 0x01F3B, lc01F3B }, { 0x01F3C, lc01F3C }, { 0x01F3D, lc01F3D }, { 0x01F3E, lc01F3E },
{ 0x01F3F, lc01F3F }, { 0x01F48, lc01F48 }, { 0x01F49, lc01F49 }, { 0x01F4A, lc01F4A }, { 0x01F4B, lc01F4B }, { 0x01F4C, lc01F4C }, { 0x01F4D, lc01F4D }, { 0x01F50, lc01F50 },
{ 0x01F52, lc01F52 }, { 0x01F54, lc01F54 }, { 0x01F56, lc01F56 }, { 0x01F59, lc01F59 }, { 0x01F5B, lc01F5B }, { 0x01F5D, lc01F5D }, { 0x01F5F, lc01F5F }, { 0x01F68, lc01F68 },
{ 0x01F69, lc01F69 }, { 0x01F6A, lc01F6A }, { 0x01F6B, lc01F6B }, { 0x01F6C, lc01F6C }, { 0x01F6D, lc01F6D }, { 0x01F6E, lc01F6E }, { 0x01F6F, lc01F6F }, { 0x01F80, lc01F80 },
{ 0x01F81, lc01F81 }, { 0x01F82, lc01F82 }, { 0x01F83, lc01F83 }, { 0x01F84, lc01F84 }, { 0x01F85, lc01F85 }, { 0x01F86, lc01F86 }, { 0x01F87, lc01F87 }, { 0x01F88, lc01F88 },
{ 0x01F89, lc01F89 }, { 0x01F8A, lc01F8A }, { 0x01F8B, lc01F8B }, { 0x01F8C, lc01F8C }, { 0x01F8D, lc01F8D }, { 0x01F8E, lc01F8E }, { 0x01F8F, lc01F8F }, { 0x01F90, lc01F90 },
{ 0x01F91, lc01F91 }, { 0x01F92, lc01F92 }, { 0x01F93, lc01F93 }, { 0x01F94, lc01F94 }, { 0x01F95, lc01F95 }, { 0x01F96, lc01F96 }, { 0x01F97, lc01F97 }, { 0x01F98, lc01F98 },
{ 0x01F99, lc01F99 }, { 0x01F9A, lc01F9A }, { 0x01F9B, lc01F9B }, { 0x01F9C, lc01F9C }, { 0x01F9D, lc01F9D }, { 0x01F9E, lc01F9E }, { 0x01F9F, lc01F9F }, { 0x01FA0, lc01FA0 },
{ 0x01FA1, lc01FA1 }, { 0x01FA2, lc01FA2 }, { 0x01FA3, lc01FA3 }, { 0x01FA4, lc01FA4 }, { 0x01FA5, lc01FA5 }, { 0x01FA6, lc01FA6 }, { 0x01FA7, lc01FA7 }, { 0x01FA8, lc01FA8 },
{ 0x01FA9, lc01FA9 }, { 0x01FAA, lc01FAA }, { 0x01FAB, lc01FAB }, { 0x01FAC, lc01FAC }, { 0x01FAD, lc01FAD }, { 0x01FAE, lc01FAE }, { 0x01FAF, lc01FAF }, { 0x01FB2, lc01FB2 },
{ 0x01FB3, lc01FB3 }, { 0x01FB4, lc01FB4 }, { 0x01FB6, lc01FB6 }, { 0x01FB7, lc01FB7 }, { 0x01FB8, lc01FB8 }, { 0x01FB9, lc01FB9 }, { 0x01FBA, lc01FBA }, { 0x01FBB, lc01FBB },
{ 0x01FBC, lc01FBC }, { 0x01FC2, lc01FC2 }, { 0x01FC3, lc01FC3 }, { 0x01FC4, lc01FC4 }, { 0x01FC6, lc01FC6 }, { 0x01FC7, lc01FC7 }, { 0x01FC8, lc01FC8 }, { 0x01FC9, lc01FC9 },
{ 0x01FCA, lc01FCA }, { 0x01FCB, lc01FCB }, { 0x01FCC, lc01FCC }, { 0x01FD2, lc01FD2 }, { 0x01FD3, lc01FD3 }, { 0x01FD6, lc01FD6 }, { 0x01FD7, lc01FD7 }, { 0x01FD8, lc01FD8 },
{ 0x01FD9, lc01FD9 }, { 0x01FDA, lc01FDA }, { 0x01FDB, lc01FDB }, { 0x01FE2, lc01FE2 }, { 0x01FE3, lc01FE3 }, { 0x01FE4, lc01FE4 }, { 0x01FE6, lc01FE6 }, { 0x01FE7, lc01FE7 },
{ 0x01FE8, lc01FE8 }, { 0x01FE9, lc01FE9 }, { 0x01FEA, lc01FEA }, { 0x01FEB, lc01FEB }, { 0x01FEC, lc01FEC }, { 0x01FF2, lc01FF2 }, { 0x01FF3, lc01FF3 }, { 0x01FF4, lc01FF4 },
{ 0x01FF6, lc01FF6 }, { 0x01FF7, lc01FF7 }, { 0x01FF8, lc01FF8 }, { 0x01FF9, lc01FF9 }, { 0x01FFA, lc01FFA }, { 0x01FFB, lc01FFB }, { 0x01FFC, lc01FFC }, { 0x02126, lc02126 },
{ 0x0212A, lc0212A }, { 0x0212B, lc0212B }, { 0x02132, lc02132 }, { 0x02160, lc02160 }, { 0x02161, lc02161 }, { 0x02162, lc02162 }, { 0x02163, lc02163 }, { 0x02164, lc02164 },
{ 0x02165, lc02165 }, { 0x02166, lc02166 }, { 0x02167, lc02167 }, { 0x02168, lc02168 }, { 0x02169, lc02169 }, { 0x0216A, lc0216A }, { 0x0216B, lc0216B }, { 0x0216C, lc0216C },
{ 0x0216D, lc0216D }, { 0x0216E, lc0216E }, { 0x0216F, lc0216F }, { 0x02183, lc02183 }, { 0x024B6, lc024B6 }, { 0x024B7, lc024B7 }, { 0x024B8, lc024B8 }, { 0x024B9, lc024B9 },
{ 0x024BA, lc024BA }, { 0x024BB, lc024BB }, { 0x024BC, lc024BC }, { 0x024BD, lc024BD }, { 0x024BE, lc024BE }, { 0x024BF, lc024BF }, { 0x024C0, lc024C0 }, { 0x024C1, lc024C1 },
{ 0x024C2, lc024C2 }, { 0x024C3, lc024C3 }, { 0x024C4, lc024C4 }, { 0x024C5, lc024C5 }, { 0x024C6, lc024C6 }, { 0x024C7, lc024C7 }, { 0x024C8, lc024C8 }, { 0x024C9, lc024C9 },
{ 0x024CA, lc024CA }, { 0x024CB, lc024CB }, { 0x024CC, lc024CC }, { 0x024CD, lc024CD }, { 0x024CE, lc024CE }, { 0x024CF, lc024CF }, { 0x02C00, lc02C00 }, { 0x02C01, lc02C01 },
{ 0x02C02, lc02C02 }, { 0x02C03, lc02C03 }, { 0x02C04, lc02C04 }, { 0x02C05, lc02C05 }, { 0x02C06, lc02C06 }, { 0x02C07, lc02C07 }, { 0x02C08, lc02C08 }, { 0x02C09, lc02C09 },
{ 0x02C0A, lc02C0A }, { 0x02C0B, lc02C0B }, { 0x02C0C, lc02C0C }, { 0x02C0D, lc02C0D }, { 0x02C0E, lc02C0E }, { 0x02C0F, lc02C0F }, { 0x02C10, lc02C10 }, { 0x02C11, lc02C11 },
{ 0x02C12, lc02C12 }, { 0x02C13, lc02C13 }, { 0x02C14, lc02C14 }, { 0x02C15, lc02C15 }, { 0x02C16, lc02C16 }, { 0x02C17, lc02C17 }, { 0x02C18, lc02C18 }, { 0x02C19, lc02C19 },
{ 0x02C1A, lc02C1A }, { 0x02C1B, lc02C1B }, { 0x02C1C, lc02C1C }, { 0x02C1D, lc02C1D }, { 0x02C1E, lc02C1E }, { 0x02C1F, lc02C1F }, { 0x02C20, lc02C20 }, { 0x02C21, lc02C21 },
{ 0x02C22, lc02C22 }, { 0x02C23, lc02C23 }, { 0x02C24, lc02C24 }, { 0x02C25, lc02C25 }, { 0x02C26, lc02C26 }, { 0x02C27, lc02C27 }, { 0x02C28, lc02C28 }, { 0x02C29, lc02C29 },
{ 0x02C2A, lc02C2A }, { 0x02C2B, lc02C2B }, { 0x02C2C, lc02C2C }, { 0x02C2D, lc02C2D }, { 0x02C2E, lc02C2E }, { 0x02C60, lc02C60 }, { 0x02C62, lc02C62 }, { 0x02C63, lc02C63 },
{ 0x02C64, lc02C64 }, { 0x02C67, lc02C67 }, { 0x02C69, lc02C69 }, { 0x02C6B, lc02C6B }, { 0x02C75, lc02C75 }, { 0x02C80, lc02C80 }, { 0x02C82, lc02C82 }, { 0x02C84, lc02C84 },
{ 0x02C86, lc02C86 }, { 0x02C88, lc02C88 }, { 0x02C8A, lc02C8A }, { 0x02C8C, lc02C8C }, { 0x02C8E, lc02C8E }, { 0x02C90, lc02C90 }, { 0x02C92, lc02C92 }, { 0x02C94, lc02C94 },
{ 0x02C96, lc02C96 }, { 0x02C98, lc02C98 }, { 0x02C9A, lc02C9A }, { 0x02C9C, lc02C9C }, { 0x02C9E, lc02C9E }, { 0x02CA0, lc02CA0 }, { 0x02CA2, lc02CA2 }, { 0x02CA4, lc02CA4 },
{ 0x02CA6, lc02CA6 }, { 0x02CA8, lc02CA8 }, { 0x02CAA, lc02CAA }, { 0x02CAC, lc02CAC }, { 0x02CAE, lc02CAE }, { 0x02CB0, lc02CB0 }, { 0x02CB2, lc02CB2 }, { 0x02CB4, lc02CB4 },
{ 0x02CB6, lc02CB6 }, { 0x02CB8, lc02CB8 }, { 0x02CBA, lc02CBA }, { 0x02CBC, lc02CBC }, { 0x02CBE, lc02CBE }, { 0x02CC0, lc02CC0 }, { 0x02CC2, lc02CC2 }, { 0x02CC4, lc02CC4 },
{ 0x02CC6, lc02CC6 }, { 0x02CC8, lc02CC8 }, { 0x02CCA, lc02CCA }, { 0x02CCC, lc02CCC }, { 0x02CCE, lc02CCE }, { 0x02CD0, lc02CD0 }, { 0x02CD2, lc02CD2 }, { 0x02CD4, lc02CD4 },
{ 0x02CD6, lc02CD6 }, { 0x02CD8, lc02CD8 }, { 0x02CDA, lc02CDA }, { 0x02CDC, lc02CDC }, { 0x02CDE, lc02CDE }, { 0x02CE0, lc02CE0 }, { 0x02CE2, lc02CE2 }, { 0x0FB00, lc0FB00 },
{ 0x0FB01, lc0FB01 }, { 0x0FB02, lc0FB02 }, { 0x0FB03, lc0FB03 }, { 0x0FB04, lc0FB04 }, { 0x0FB05, lc0FB05 }, { 0x0FB06, lc0FB06 }, { 0x0FB13, lc0FB13 }, { 0x0FB14, lc0FB14 },
{ 0x0FB15, lc0FB15 }, { 0x0FB16, lc0FB16 }, { 0x0FB17, lc0FB17 }, { 0x0FF21, lc0FF21 }, { 0x0FF22, lc0FF22 }, { 0x0FF23, lc0FF23 }, { 0x0FF24, lc0FF24 }, { 0x0FF25, lc0FF25 },
{ 0x0FF26, lc0FF26 }, { 0x0FF27, lc0FF27 }, { 0x0FF28, lc0FF28 }, { 0x0FF29, lc0FF29 }, { 0x0FF2A, lc0FF2A }, { 0x0FF2B, lc0FF2B }, { 0x0FF2C, lc0FF2C }, { 0x0FF2D, lc0FF2D },
{ 0x0FF2E, lc0FF2E }, { 0x0FF2F, lc0FF2F }, { 0x0FF30, lc0FF30 }, { 0x0FF31, lc0FF31 }, { 0x0FF32, lc0FF32 }, { 0x0FF33, lc0FF33 }, { 0x0FF34, lc0FF34 }, { 0x0FF35, lc0FF35 },
{ 0x0FF36, lc0FF36 }, { 0x0FF37, lc0FF37 }, { 0x0FF38, lc0FF38 }, { 0x0FF39, lc0FF39 }, { 0x0FF3A, lc0FF3A }, { 0x10400, lc10400 }, { 0x10401, lc10401 }, { 0x10402, lc10402 },
{ 0x10403, lc10403 }, { 0x10404, lc10404 }, { 0x10405, lc10405 }, { 0x10406, lc10406 }, { 0x10407, lc10407 }, { 0x10408, lc10408 }, { 0x10409, lc10409 }, { 0x1040A, lc1040A },
{ 0x1040B, lc1040B }, { 0x1040C, lc1040C }, { 0x1040D, lc1040D }, { 0x1040E, lc1040E }, { 0x1040F, lc1040F }, { 0x10410, lc10410 }, { 0x10411, lc10411 }, { 0x10412, lc10412 },
{ 0x10413, lc10413 }, { 0x10414, lc10414 }, { 0x10415, lc10415 }, { 0x10416, lc10416 }, { 0x10417, lc10417 }, { 0x10418, lc10418 }, { 0x10419, lc10419 }, { 0x1041A, lc1041A },
{ 0x1041B, lc1041B }, { 0x1041C, lc1041C }, { 0x1041D, lc1041D }, { 0x1041E, lc1041E }, { 0x1041F, lc1041F }, { 0x10420, lc10420 }, { 0x10421, lc10421 }, { 0x10422, lc10422 },
{ 0x10423, lc10423 }, { 0x10424, lc10424 }, { 0x10425, lc10425 }, { 0x10426, lc10426 }, { 0x10427, lc10427 }
};

unsigned int *LowerCaseTransform::getLowerCase(unsigned int ch)
{
  if(ch < lowerCaseDirectTableSize) {
    return lowerCaseDirectTable[ch];
  }

  // Binary search in the lowerCaseTable table
  unsigned int min = 0;
  unsigned int max = lowerCaseTableSize;
  unsigned int middle;
  int cmp;

  while(min < max) {
    middle = (max + min) >> 1;

    cmp = lowerCaseTable[middle].ch - ch;
    if(cmp > 0) max = middle;
    else if(cmp < 0) {
      min = middle + 1;
    }
    else {
      return lowerCaseTable[middle].dest;
    }
  }

  return 0;
}

static unsigned int uc00061[] = { 0x00041, 0x00000 };
static unsigned int uc00062[] = { 0x00042, 0x00000 };
static unsigned int uc00063[] = { 0x00043, 0x00000 };
static unsigned int uc00064[] = { 0x00044, 0x00000 };
static unsigned int uc00065[] = { 0x00045, 0x00000 };
static unsigned int uc00066[] = { 0x00046, 0x00000 };
static unsigned int uc00067[] = { 0x00047, 0x00000 };
static unsigned int uc00068[] = { 0x00048, 0x00000 };
static unsigned int uc00069[] = { 0x00049, 0x00000 };
static unsigned int uc0006A[] = { 0x0004A, 0x00000 };
static unsigned int uc0006B[] = { 0x0004B, 0x00000 };
static unsigned int uc0006C[] = { 0x0004C, 0x00000 };
static unsigned int uc0006D[] = { 0x0004D, 0x00000 };
static unsigned int uc0006E[] = { 0x0004E, 0x00000 };
static unsigned int uc0006F[] = { 0x0004F, 0x00000 };
static unsigned int uc00070[] = { 0x00050, 0x00000 };
static unsigned int uc00071[] = { 0x00051, 0x00000 };
static unsigned int uc00072[] = { 0x00052, 0x00000 };
static unsigned int uc00073[] = { 0x00053, 0x00000 };
static unsigned int uc00074[] = { 0x00054, 0x00000 };
static unsigned int uc00075[] = { 0x00055, 0x00000 };
static unsigned int uc00076[] = { 0x00056, 0x00000 };
static unsigned int uc00077[] = { 0x00057, 0x00000 };
static unsigned int uc00078[] = { 0x00058, 0x00000 };
static unsigned int uc00079[] = { 0x00059, 0x00000 };
static unsigned int uc0007A[] = { 0x0005A, 0x00000 };
static unsigned int uc000B5[] = { 0x0039C, 0x00000 };
static unsigned int uc000DF[] = { 0x00053, 0x00053, 0x00000 };
static unsigned int uc000E0[] = { 0x000C0, 0x00000 };
static unsigned int uc000E1[] = { 0x000C1, 0x00000 };
static unsigned int uc000E2[] = { 0x000C2, 0x00000 };
static unsigned int uc000E3[] = { 0x000C3, 0x00000 };
static unsigned int uc000E4[] = { 0x000C4, 0x00000 };
static unsigned int uc000E5[] = { 0x000C5, 0x00000 };
static unsigned int uc000E6[] = { 0x000C6, 0x00000 };
static unsigned int uc000E7[] = { 0x000C7, 0x00000 };
static unsigned int uc000E8[] = { 0x000C8, 0x00000 };
static unsigned int uc000E9[] = { 0x000C9, 0x00000 };
static unsigned int uc000EA[] = { 0x000CA, 0x00000 };
static unsigned int uc000EB[] = { 0x000CB, 0x00000 };
static unsigned int uc000EC[] = { 0x000CC, 0x00000 };
static unsigned int uc000ED[] = { 0x000CD, 0x00000 };
static unsigned int uc000EE[] = { 0x000CE, 0x00000 };
static unsigned int uc000EF[] = { 0x000CF, 0x00000 };
static unsigned int uc000F0[] = { 0x000D0, 0x00000 };
static unsigned int uc000F1[] = { 0x000D1, 0x00000 };
static unsigned int uc000F2[] = { 0x000D2, 0x00000 };
static unsigned int uc000F3[] = { 0x000D3, 0x00000 };
static unsigned int uc000F4[] = { 0x000D4, 0x00000 };
static unsigned int uc000F5[] = { 0x000D5, 0x00000 };
static unsigned int uc000F6[] = { 0x000D6, 0x00000 };
static unsigned int uc000F8[] = { 0x000D8, 0x00000 };
static unsigned int uc000F9[] = { 0x000D9, 0x00000 };
static unsigned int uc000FA[] = { 0x000DA, 0x00000 };
static unsigned int uc000FB[] = { 0x000DB, 0x00000 };
static unsigned int uc000FC[] = { 0x000DC, 0x00000 };
static unsigned int uc000FD[] = { 0x000DD, 0x00000 };
static unsigned int uc000FE[] = { 0x000DE, 0x00000 };
static unsigned int uc000FF[] = { 0x00178, 0x00000 };
static unsigned int uc00101[] = { 0x00100, 0x00000 };
static unsigned int uc00103[] = { 0x00102, 0x00000 };
static unsigned int uc00105[] = { 0x00104, 0x00000 };
static unsigned int uc00107[] = { 0x00106, 0x00000 };
static unsigned int uc00109[] = { 0x00108, 0x00000 };
static unsigned int uc0010B[] = { 0x0010A, 0x00000 };
static unsigned int uc0010D[] = { 0x0010C, 0x00000 };
static unsigned int uc0010F[] = { 0x0010E, 0x00000 };
static unsigned int uc00111[] = { 0x00110, 0x00000 };
static unsigned int uc00113[] = { 0x00112, 0x00000 };
static unsigned int uc00115[] = { 0x00114, 0x00000 };
static unsigned int uc00117[] = { 0x00116, 0x00000 };
static unsigned int uc00119[] = { 0x00118, 0x00000 };
static unsigned int uc0011B[] = { 0x0011A, 0x00000 };
static unsigned int uc0011D[] = { 0x0011C, 0x00000 };
static unsigned int uc0011F[] = { 0x0011E, 0x00000 };
static unsigned int uc00121[] = { 0x00120, 0x00000 };
static unsigned int uc00123[] = { 0x00122, 0x00000 };
static unsigned int uc00125[] = { 0x00124, 0x00000 };
static unsigned int uc00127[] = { 0x00126, 0x00000 };
static unsigned int uc00129[] = { 0x00128, 0x00000 };
static unsigned int uc0012B[] = { 0x0012A, 0x00000 };
static unsigned int uc0012D[] = { 0x0012C, 0x00000 };
static unsigned int uc0012F[] = { 0x0012E, 0x00000 };
static unsigned int uc00130[] = { 0x00130, 0x00000 };
static unsigned int uc00131[] = { 0x00049, 0x00000 };
static unsigned int uc00133[] = { 0x00132, 0x00000 };
static unsigned int uc00135[] = { 0x00134, 0x00000 };
static unsigned int uc00137[] = { 0x00136, 0x00000 };
static unsigned int uc0013A[] = { 0x00139, 0x00000 };
static unsigned int uc0013C[] = { 0x0013B, 0x00000 };
static unsigned int uc0013E[] = { 0x0013D, 0x00000 };
static unsigned int uc00140[] = { 0x0013F, 0x00000 };
static unsigned int uc00142[] = { 0x00141, 0x00000 };
static unsigned int uc00144[] = { 0x00143, 0x00000 };
static unsigned int uc00146[] = { 0x00145, 0x00000 };
static unsigned int uc00148[] = { 0x00147, 0x00000 };
static unsigned int uc00149[] = { 0x002BC, 0x0004E, 0x00000 };
static unsigned int uc0014B[] = { 0x0014A, 0x00000 };
static unsigned int uc0014D[] = { 0x0014C, 0x00000 };
static unsigned int uc0014F[] = { 0x0014E, 0x00000 };
static unsigned int uc00151[] = { 0x00150, 0x00000 };
static unsigned int uc00153[] = { 0x00152, 0x00000 };
static unsigned int uc00155[] = { 0x00154, 0x00000 };
static unsigned int uc00157[] = { 0x00156, 0x00000 };
static unsigned int uc00159[] = { 0x00158, 0x00000 };
static unsigned int uc0015B[] = { 0x0015A, 0x00000 };
static unsigned int uc0015D[] = { 0x0015C, 0x00000 };
static unsigned int uc0015F[] = { 0x0015E, 0x00000 };
static unsigned int uc00161[] = { 0x00160, 0x00000 };
static unsigned int uc00163[] = { 0x00162, 0x00000 };
static unsigned int uc00165[] = { 0x00164, 0x00000 };
static unsigned int uc00167[] = { 0x00166, 0x00000 };
static unsigned int uc00169[] = { 0x00168, 0x00000 };
static unsigned int uc0016B[] = { 0x0016A, 0x00000 };
static unsigned int uc0016D[] = { 0x0016C, 0x00000 };
static unsigned int uc0016F[] = { 0x0016E, 0x00000 };
static unsigned int uc00171[] = { 0x00170, 0x00000 };
static unsigned int uc00173[] = { 0x00172, 0x00000 };
static unsigned int uc00175[] = { 0x00174, 0x00000 };
static unsigned int uc00177[] = { 0x00176, 0x00000 };
static unsigned int uc0017A[] = { 0x00179, 0x00000 };
static unsigned int uc0017C[] = { 0x0017B, 0x00000 };
static unsigned int uc0017E[] = { 0x0017D, 0x00000 };
static unsigned int uc0017F[] = { 0x00053, 0x00000 };
static unsigned int uc00180[] = { 0x00243, 0x00000 };
static unsigned int uc00183[] = { 0x00182, 0x00000 };
static unsigned int uc00185[] = { 0x00184, 0x00000 };
static unsigned int uc00188[] = { 0x00187, 0x00000 };
static unsigned int uc0018C[] = { 0x0018B, 0x00000 };
static unsigned int uc00192[] = { 0x00191, 0x00000 };
static unsigned int uc00195[] = { 0x001F6, 0x00000 };
static unsigned int uc00199[] = { 0x00198, 0x00000 };
static unsigned int uc0019A[] = { 0x0023D, 0x00000 };
static unsigned int uc0019E[] = { 0x00220, 0x00000 };
static unsigned int uc001A1[] = { 0x001A0, 0x00000 };
static unsigned int uc001A3[] = { 0x001A2, 0x00000 };
static unsigned int uc001A5[] = { 0x001A4, 0x00000 };
static unsigned int uc001A8[] = { 0x001A7, 0x00000 };
static unsigned int uc001AD[] = { 0x001AC, 0x00000 };
static unsigned int uc001B0[] = { 0x001AF, 0x00000 };
static unsigned int uc001B4[] = { 0x001B3, 0x00000 };
static unsigned int uc001B6[] = { 0x001B5, 0x00000 };
static unsigned int uc001B9[] = { 0x001B8, 0x00000 };
static unsigned int uc001BD[] = { 0x001BC, 0x00000 };
static unsigned int uc001BF[] = { 0x001F7, 0x00000 };
static unsigned int uc001C5[] = { 0x001C4, 0x00000 };
static unsigned int uc001C6[] = { 0x001C4, 0x00000 };
static unsigned int uc001C8[] = { 0x001C7, 0x00000 };
static unsigned int uc001C9[] = { 0x001C7, 0x00000 };
static unsigned int uc001CB[] = { 0x001CA, 0x00000 };
static unsigned int uc001CC[] = { 0x001CA, 0x00000 };
static unsigned int uc001CE[] = { 0x001CD, 0x00000 };
static unsigned int uc001D0[] = { 0x001CF, 0x00000 };
static unsigned int uc001D2[] = { 0x001D1, 0x00000 };
static unsigned int uc001D4[] = { 0x001D3, 0x00000 };
static unsigned int uc001D6[] = { 0x001D5, 0x00000 };
static unsigned int uc001D8[] = { 0x001D7, 0x00000 };
static unsigned int uc001DA[] = { 0x001D9, 0x00000 };
static unsigned int uc001DC[] = { 0x001DB, 0x00000 };
static unsigned int uc001DD[] = { 0x0018E, 0x00000 };
static unsigned int uc001DF[] = { 0x001DE, 0x00000 };
static unsigned int uc001E1[] = { 0x001E0, 0x00000 };
static unsigned int uc001E3[] = { 0x001E2, 0x00000 };
static unsigned int uc001E5[] = { 0x001E4, 0x00000 };
static unsigned int uc001E7[] = { 0x001E6, 0x00000 };
static unsigned int uc001E9[] = { 0x001E8, 0x00000 };
static unsigned int uc001EB[] = { 0x001EA, 0x00000 };
static unsigned int uc001ED[] = { 0x001EC, 0x00000 };
static unsigned int uc001EF[] = { 0x001EE, 0x00000 };
static unsigned int uc001F0[] = { 0x0004A, 0x0030C, 0x00000 };
static unsigned int uc001F2[] = { 0x001F1, 0x00000 };
static unsigned int uc001F3[] = { 0x001F1, 0x00000 };
static unsigned int uc001F5[] = { 0x001F4, 0x00000 };
static unsigned int uc001F9[] = { 0x001F8, 0x00000 };
static unsigned int uc001FB[] = { 0x001FA, 0x00000 };
static unsigned int uc001FD[] = { 0x001FC, 0x00000 };
static unsigned int uc001FF[] = { 0x001FE, 0x00000 };
static unsigned int uc00201[] = { 0x00200, 0x00000 };
static unsigned int uc00203[] = { 0x00202, 0x00000 };
static unsigned int uc00205[] = { 0x00204, 0x00000 };
static unsigned int uc00207[] = { 0x00206, 0x00000 };
static unsigned int uc00209[] = { 0x00208, 0x00000 };
static unsigned int uc0020B[] = { 0x0020A, 0x00000 };
static unsigned int uc0020D[] = { 0x0020C, 0x00000 };
static unsigned int uc0020F[] = { 0x0020E, 0x00000 };
static unsigned int uc00211[] = { 0x00210, 0x00000 };
static unsigned int uc00213[] = { 0x00212, 0x00000 };
static unsigned int uc00215[] = { 0x00214, 0x00000 };
static unsigned int uc00217[] = { 0x00216, 0x00000 };
static unsigned int uc00219[] = { 0x00218, 0x00000 };
static unsigned int uc0021B[] = { 0x0021A, 0x00000 };
static unsigned int uc0021D[] = { 0x0021C, 0x00000 };
static unsigned int uc0021F[] = { 0x0021E, 0x00000 };
static unsigned int uc00223[] = { 0x00222, 0x00000 };
static unsigned int uc00225[] = { 0x00224, 0x00000 };
static unsigned int uc00227[] = { 0x00226, 0x00000 };
static unsigned int uc00229[] = { 0x00228, 0x00000 };
static unsigned int uc0022B[] = { 0x0022A, 0x00000 };
static unsigned int uc0022D[] = { 0x0022C, 0x00000 };
static unsigned int uc0022F[] = { 0x0022E, 0x00000 };
static unsigned int uc00231[] = { 0x00230, 0x00000 };
static unsigned int uc00233[] = { 0x00232, 0x00000 };
static unsigned int uc0023C[] = { 0x0023B, 0x00000 };
static unsigned int uc00242[] = { 0x00241, 0x00000 };
static unsigned int uc00247[] = { 0x00246, 0x00000 };
static unsigned int uc00249[] = { 0x00248, 0x00000 };
static unsigned int uc0024B[] = { 0x0024A, 0x00000 };
static unsigned int uc0024D[] = { 0x0024C, 0x00000 };
static unsigned int uc0024F[] = { 0x0024E, 0x00000 };
static unsigned int uc00253[] = { 0x00181, 0x00000 };
static unsigned int uc00254[] = { 0x00186, 0x00000 };
static unsigned int uc00256[] = { 0x00189, 0x00000 };
static unsigned int uc00257[] = { 0x0018A, 0x00000 };
static unsigned int uc00259[] = { 0x0018F, 0x00000 };
static unsigned int uc0025B[] = { 0x00190, 0x00000 };
static unsigned int uc00260[] = { 0x00193, 0x00000 };
static unsigned int uc00263[] = { 0x00194, 0x00000 };
static unsigned int uc00268[] = { 0x00197, 0x00000 };
static unsigned int uc00269[] = { 0x00196, 0x00000 };
static unsigned int uc0026B[] = { 0x02C62, 0x00000 };
static unsigned int uc0026F[] = { 0x0019C, 0x00000 };
static unsigned int uc00272[] = { 0x0019D, 0x00000 };
static unsigned int uc00275[] = { 0x0019F, 0x00000 };
static unsigned int uc0027D[] = { 0x02C64, 0x00000 };
static unsigned int uc00280[] = { 0x001A6, 0x00000 };
static unsigned int uc00283[] = { 0x001A9, 0x00000 };
static unsigned int uc00288[] = { 0x001AE, 0x00000 };
static unsigned int uc00289[] = { 0x00244, 0x00000 };
static unsigned int uc0028A[] = { 0x001B1, 0x00000 };
static unsigned int uc0028B[] = { 0x001B2, 0x00000 };
static unsigned int uc0028C[] = { 0x00245, 0x00000 };
static unsigned int uc00292[] = { 0x001B7, 0x00000 };
static unsigned int uc00345[] = { 0x00399, 0x00000 };
static unsigned int uc0037B[] = { 0x003FD, 0x00000 };
static unsigned int uc0037C[] = { 0x003FE, 0x00000 };
static unsigned int uc0037D[] = { 0x003FF, 0x00000 };
static unsigned int uc00390[] = { 0x00399, 0x00308, 0x00301, 0x00000 };
static unsigned int uc003AC[] = { 0x00386, 0x00000 };
static unsigned int uc003AD[] = { 0x00388, 0x00000 };
static unsigned int uc003AE[] = { 0x00389, 0x00000 };
static unsigned int uc003AF[] = { 0x0038A, 0x00000 };
static unsigned int uc003B0[] = { 0x003A5, 0x00308, 0x00301, 0x00000 };
static unsigned int uc003B1[] = { 0x00391, 0x00000 };
static unsigned int uc003B2[] = { 0x00392, 0x00000 };
static unsigned int uc003B3[] = { 0x00393, 0x00000 };
static unsigned int uc003B4[] = { 0x00394, 0x00000 };
static unsigned int uc003B5[] = { 0x00395, 0x00000 };
static unsigned int uc003B6[] = { 0x00396, 0x00000 };
static unsigned int uc003B7[] = { 0x00397, 0x00000 };
static unsigned int uc003B8[] = { 0x00398, 0x00000 };
static unsigned int uc003B9[] = { 0x00399, 0x00000 };
static unsigned int uc003BA[] = { 0x0039A, 0x00000 };
static unsigned int uc003BB[] = { 0x0039B, 0x00000 };
static unsigned int uc003BC[] = { 0x0039C, 0x00000 };
static unsigned int uc003BD[] = { 0x0039D, 0x00000 };
static unsigned int uc003BE[] = { 0x0039E, 0x00000 };
static unsigned int uc003BF[] = { 0x0039F, 0x00000 };
static unsigned int uc003C0[] = { 0x003A0, 0x00000 };
static unsigned int uc003C1[] = { 0x003A1, 0x00000 };
static unsigned int uc003C2[] = { 0x003A3, 0x00000 };
static unsigned int uc003C3[] = { 0x003A3, 0x00000 };
static unsigned int uc003C4[] = { 0x003A4, 0x00000 };
static unsigned int uc003C5[] = { 0x003A5, 0x00000 };
static unsigned int uc003C6[] = { 0x003A6, 0x00000 };
static unsigned int uc003C7[] = { 0x003A7, 0x00000 };
static unsigned int uc003C8[] = { 0x003A8, 0x00000 };
static unsigned int uc003C9[] = { 0x003A9, 0x00000 };
static unsigned int uc003CA[] = { 0x003AA, 0x00000 };
static unsigned int uc003CB[] = { 0x003AB, 0x00000 };
static unsigned int uc003CC[] = { 0x0038C, 0x00000 };
static unsigned int uc003CD[] = { 0x0038E, 0x00000 };
static unsigned int uc003CE[] = { 0x0038F, 0x00000 };
static unsigned int uc003D0[] = { 0x00392, 0x00000 };
static unsigned int uc003D1[] = { 0x00398, 0x00000 };
static unsigned int uc003D5[] = { 0x003A6, 0x00000 };
static unsigned int uc003D6[] = { 0x003A0, 0x00000 };
static unsigned int uc003D9[] = { 0x003D8, 0x00000 };
static unsigned int uc003DB[] = { 0x003DA, 0x00000 };
static unsigned int uc003DD[] = { 0x003DC, 0x00000 };
static unsigned int uc003DF[] = { 0x003DE, 0x00000 };
static unsigned int uc003E1[] = { 0x003E0, 0x00000 };
static unsigned int uc003E3[] = { 0x003E2, 0x00000 };
static unsigned int uc003E5[] = { 0x003E4, 0x00000 };
static unsigned int uc003E7[] = { 0x003E6, 0x00000 };
static unsigned int uc003E9[] = { 0x003E8, 0x00000 };
static unsigned int uc003EB[] = { 0x003EA, 0x00000 };
static unsigned int uc003ED[] = { 0x003EC, 0x00000 };
static unsigned int uc003EF[] = { 0x003EE, 0x00000 };
static unsigned int uc003F0[] = { 0x0039A, 0x00000 };
static unsigned int uc003F1[] = { 0x003A1, 0x00000 };
static unsigned int uc003F2[] = { 0x003F9, 0x00000 };
static unsigned int uc003F5[] = { 0x00395, 0x00000 };
static unsigned int uc003F8[] = { 0x003F7, 0x00000 };
static unsigned int uc003FB[] = { 0x003FA, 0x00000 };
static unsigned int uc00430[] = { 0x00410, 0x00000 };
static unsigned int uc00431[] = { 0x00411, 0x00000 };
static unsigned int uc00432[] = { 0x00412, 0x00000 };
static unsigned int uc00433[] = { 0x00413, 0x00000 };
static unsigned int uc00434[] = { 0x00414, 0x00000 };
static unsigned int uc00435[] = { 0x00415, 0x00000 };
static unsigned int uc00436[] = { 0x00416, 0x00000 };
static unsigned int uc00437[] = { 0x00417, 0x00000 };
static unsigned int uc00438[] = { 0x00418, 0x00000 };
static unsigned int uc00439[] = { 0x00419, 0x00000 };
static unsigned int uc0043A[] = { 0x0041A, 0x00000 };
static unsigned int uc0043B[] = { 0x0041B, 0x00000 };
static unsigned int uc0043C[] = { 0x0041C, 0x00000 };
static unsigned int uc0043D[] = { 0x0041D, 0x00000 };
static unsigned int uc0043E[] = { 0x0041E, 0x00000 };
static unsigned int uc0043F[] = { 0x0041F, 0x00000 };
static unsigned int uc00440[] = { 0x00420, 0x00000 };
static unsigned int uc00441[] = { 0x00421, 0x00000 };
static unsigned int uc00442[] = { 0x00422, 0x00000 };
static unsigned int uc00443[] = { 0x00423, 0x00000 };
static unsigned int uc00444[] = { 0x00424, 0x00000 };
static unsigned int uc00445[] = { 0x00425, 0x00000 };
static unsigned int uc00446[] = { 0x00426, 0x00000 };
static unsigned int uc00447[] = { 0x00427, 0x00000 };
static unsigned int uc00448[] = { 0x00428, 0x00000 };
static unsigned int uc00449[] = { 0x00429, 0x00000 };
static unsigned int uc0044A[] = { 0x0042A, 0x00000 };
static unsigned int uc0044B[] = { 0x0042B, 0x00000 };
static unsigned int uc0044C[] = { 0x0042C, 0x00000 };
static unsigned int uc0044D[] = { 0x0042D, 0x00000 };
static unsigned int uc0044E[] = { 0x0042E, 0x00000 };
static unsigned int uc0044F[] = { 0x0042F, 0x00000 };
static unsigned int uc00450[] = { 0x00400, 0x00000 };
static unsigned int uc00451[] = { 0x00401, 0x00000 };
static unsigned int uc00452[] = { 0x00402, 0x00000 };
static unsigned int uc00453[] = { 0x00403, 0x00000 };
static unsigned int uc00454[] = { 0x00404, 0x00000 };
static unsigned int uc00455[] = { 0x00405, 0x00000 };
static unsigned int uc00456[] = { 0x00406, 0x00000 };
static unsigned int uc00457[] = { 0x00407, 0x00000 };
static unsigned int uc00458[] = { 0x00408, 0x00000 };
static unsigned int uc00459[] = { 0x00409, 0x00000 };
static unsigned int uc0045A[] = { 0x0040A, 0x00000 };
static unsigned int uc0045B[] = { 0x0040B, 0x00000 };
static unsigned int uc0045C[] = { 0x0040C, 0x00000 };
static unsigned int uc0045D[] = { 0x0040D, 0x00000 };
static unsigned int uc0045E[] = { 0x0040E, 0x00000 };
static unsigned int uc0045F[] = { 0x0040F, 0x00000 };
static unsigned int uc00461[] = { 0x00460, 0x00000 };
static unsigned int uc00463[] = { 0x00462, 0x00000 };
static unsigned int uc00465[] = { 0x00464, 0x00000 };
static unsigned int uc00467[] = { 0x00466, 0x00000 };
static unsigned int uc00469[] = { 0x00468, 0x00000 };
static unsigned int uc0046B[] = { 0x0046A, 0x00000 };
static unsigned int uc0046D[] = { 0x0046C, 0x00000 };
static unsigned int uc0046F[] = { 0x0046E, 0x00000 };
static unsigned int uc00471[] = { 0x00470, 0x00000 };
static unsigned int uc00473[] = { 0x00472, 0x00000 };
static unsigned int uc00475[] = { 0x00474, 0x00000 };
static unsigned int uc00477[] = { 0x00476, 0x00000 };
static unsigned int uc00479[] = { 0x00478, 0x00000 };
static unsigned int uc0047B[] = { 0x0047A, 0x00000 };
static unsigned int uc0047D[] = { 0x0047C, 0x00000 };
static unsigned int uc0047F[] = { 0x0047E, 0x00000 };
static unsigned int uc00481[] = { 0x00480, 0x00000 };
static unsigned int uc0048B[] = { 0x0048A, 0x00000 };
static unsigned int uc0048D[] = { 0x0048C, 0x00000 };
static unsigned int uc0048F[] = { 0x0048E, 0x00000 };
static unsigned int uc00491[] = { 0x00490, 0x00000 };
static unsigned int uc00493[] = { 0x00492, 0x00000 };
static unsigned int uc00495[] = { 0x00494, 0x00000 };
static unsigned int uc00497[] = { 0x00496, 0x00000 };
static unsigned int uc00499[] = { 0x00498, 0x00000 };
static unsigned int uc0049B[] = { 0x0049A, 0x00000 };
static unsigned int uc0049D[] = { 0x0049C, 0x00000 };
static unsigned int uc0049F[] = { 0x0049E, 0x00000 };
static unsigned int uc004A1[] = { 0x004A0, 0x00000 };
static unsigned int uc004A3[] = { 0x004A2, 0x00000 };
static unsigned int uc004A5[] = { 0x004A4, 0x00000 };
static unsigned int uc004A7[] = { 0x004A6, 0x00000 };
static unsigned int uc004A9[] = { 0x004A8, 0x00000 };
static unsigned int uc004AB[] = { 0x004AA, 0x00000 };
static unsigned int uc004AD[] = { 0x004AC, 0x00000 };
static unsigned int uc004AF[] = { 0x004AE, 0x00000 };
static unsigned int uc004B1[] = { 0x004B0, 0x00000 };
static unsigned int uc004B3[] = { 0x004B2, 0x00000 };
static unsigned int uc004B5[] = { 0x004B4, 0x00000 };
static unsigned int uc004B7[] = { 0x004B6, 0x00000 };
static unsigned int uc004B9[] = { 0x004B8, 0x00000 };
static unsigned int uc004BB[] = { 0x004BA, 0x00000 };
static unsigned int uc004BD[] = { 0x004BC, 0x00000 };
static unsigned int uc004BF[] = { 0x004BE, 0x00000 };
static unsigned int uc004C2[] = { 0x004C1, 0x00000 };
static unsigned int uc004C4[] = { 0x004C3, 0x00000 };
static unsigned int uc004C6[] = { 0x004C5, 0x00000 };
static unsigned int uc004C8[] = { 0x004C7, 0x00000 };
static unsigned int uc004CA[] = { 0x004C9, 0x00000 };
static unsigned int uc004CC[] = { 0x004CB, 0x00000 };
static unsigned int uc004CE[] = { 0x004CD, 0x00000 };
static unsigned int uc004CF[] = { 0x004C0, 0x00000 };
static unsigned int uc004D1[] = { 0x004D0, 0x00000 };
static unsigned int uc004D3[] = { 0x004D2, 0x00000 };
static unsigned int uc004D5[] = { 0x004D4, 0x00000 };
static unsigned int uc004D7[] = { 0x004D6, 0x00000 };
static unsigned int uc004D9[] = { 0x004D8, 0x00000 };
static unsigned int uc004DB[] = { 0x004DA, 0x00000 };
static unsigned int uc004DD[] = { 0x004DC, 0x00000 };
static unsigned int uc004DF[] = { 0x004DE, 0x00000 };
static unsigned int uc004E1[] = { 0x004E0, 0x00000 };
static unsigned int uc004E3[] = { 0x004E2, 0x00000 };
static unsigned int uc004E5[] = { 0x004E4, 0x00000 };
static unsigned int uc004E7[] = { 0x004E6, 0x00000 };
static unsigned int uc004E9[] = { 0x004E8, 0x00000 };
static unsigned int uc004EB[] = { 0x004EA, 0x00000 };
static unsigned int uc004ED[] = { 0x004EC, 0x00000 };
static unsigned int uc004EF[] = { 0x004EE, 0x00000 };
static unsigned int uc004F1[] = { 0x004F0, 0x00000 };
static unsigned int uc004F3[] = { 0x004F2, 0x00000 };
static unsigned int uc004F5[] = { 0x004F4, 0x00000 };
static unsigned int uc004F7[] = { 0x004F6, 0x00000 };
static unsigned int uc004F9[] = { 0x004F8, 0x00000 };
static unsigned int uc004FB[] = { 0x004FA, 0x00000 };
static unsigned int uc004FD[] = { 0x004FC, 0x00000 };
static unsigned int uc004FF[] = { 0x004FE, 0x00000 };
static unsigned int uc00501[] = { 0x00500, 0x00000 };
static unsigned int uc00503[] = { 0x00502, 0x00000 };
static unsigned int uc00505[] = { 0x00504, 0x00000 };
static unsigned int uc00507[] = { 0x00506, 0x00000 };
static unsigned int uc00509[] = { 0x00508, 0x00000 };
static unsigned int uc0050B[] = { 0x0050A, 0x00000 };
static unsigned int uc0050D[] = { 0x0050C, 0x00000 };
static unsigned int uc0050F[] = { 0x0050E, 0x00000 };
static unsigned int uc00511[] = { 0x00510, 0x00000 };
static unsigned int uc00513[] = { 0x00512, 0x00000 };
static unsigned int uc00561[] = { 0x00531, 0x00000 };
static unsigned int uc00562[] = { 0x00532, 0x00000 };
static unsigned int uc00563[] = { 0x00533, 0x00000 };
static unsigned int uc00564[] = { 0x00534, 0x00000 };
static unsigned int uc00565[] = { 0x00535, 0x00000 };
static unsigned int uc00566[] = { 0x00536, 0x00000 };
static unsigned int uc00567[] = { 0x00537, 0x00000 };
static unsigned int uc00568[] = { 0x00538, 0x00000 };
static unsigned int uc00569[] = { 0x00539, 0x00000 };
static unsigned int uc0056A[] = { 0x0053A, 0x00000 };
static unsigned int uc0056B[] = { 0x0053B, 0x00000 };
static unsigned int uc0056C[] = { 0x0053C, 0x00000 };
static unsigned int uc0056D[] = { 0x0053D, 0x00000 };
static unsigned int uc0056E[] = { 0x0053E, 0x00000 };
static unsigned int uc0056F[] = { 0x0053F, 0x00000 };
static unsigned int uc00570[] = { 0x00540, 0x00000 };
static unsigned int uc00571[] = { 0x00541, 0x00000 };
static unsigned int uc00572[] = { 0x00542, 0x00000 };
static unsigned int uc00573[] = { 0x00543, 0x00000 };
static unsigned int uc00574[] = { 0x00544, 0x00000 };
static unsigned int uc00575[] = { 0x00545, 0x00000 };
static unsigned int uc00576[] = { 0x00546, 0x00000 };
static unsigned int uc00577[] = { 0x00547, 0x00000 };
static unsigned int uc00578[] = { 0x00548, 0x00000 };
static unsigned int uc00579[] = { 0x00549, 0x00000 };
static unsigned int uc0057A[] = { 0x0054A, 0x00000 };
static unsigned int uc0057B[] = { 0x0054B, 0x00000 };
static unsigned int uc0057C[] = { 0x0054C, 0x00000 };
static unsigned int uc0057D[] = { 0x0054D, 0x00000 };
static unsigned int uc0057E[] = { 0x0054E, 0x00000 };
static unsigned int uc0057F[] = { 0x0054F, 0x00000 };
static unsigned int uc00580[] = { 0x00550, 0x00000 };
static unsigned int uc00581[] = { 0x00551, 0x00000 };
static unsigned int uc00582[] = { 0x00552, 0x00000 };
static unsigned int uc00583[] = { 0x00553, 0x00000 };
static unsigned int uc00584[] = { 0x00554, 0x00000 };
static unsigned int uc00585[] = { 0x00555, 0x00000 };
static unsigned int uc00586[] = { 0x00556, 0x00000 };
static unsigned int uc00587[] = { 0x00535, 0x00552, 0x00000 };
static unsigned int uc01D7D[] = { 0x02C63, 0x00000 };
static unsigned int uc01E01[] = { 0x01E00, 0x00000 };
static unsigned int uc01E03[] = { 0x01E02, 0x00000 };
static unsigned int uc01E05[] = { 0x01E04, 0x00000 };
static unsigned int uc01E07[] = { 0x01E06, 0x00000 };
static unsigned int uc01E09[] = { 0x01E08, 0x00000 };
static unsigned int uc01E0B[] = { 0x01E0A, 0x00000 };
static unsigned int uc01E0D[] = { 0x01E0C, 0x00000 };
static unsigned int uc01E0F[] = { 0x01E0E, 0x00000 };
static unsigned int uc01E11[] = { 0x01E10, 0x00000 };
static unsigned int uc01E13[] = { 0x01E12, 0x00000 };
static unsigned int uc01E15[] = { 0x01E14, 0x00000 };
static unsigned int uc01E17[] = { 0x01E16, 0x00000 };
static unsigned int uc01E19[] = { 0x01E18, 0x00000 };
static unsigned int uc01E1B[] = { 0x01E1A, 0x00000 };
static unsigned int uc01E1D[] = { 0x01E1C, 0x00000 };
static unsigned int uc01E1F[] = { 0x01E1E, 0x00000 };
static unsigned int uc01E21[] = { 0x01E20, 0x00000 };
static unsigned int uc01E23[] = { 0x01E22, 0x00000 };
static unsigned int uc01E25[] = { 0x01E24, 0x00000 };
static unsigned int uc01E27[] = { 0x01E26, 0x00000 };
static unsigned int uc01E29[] = { 0x01E28, 0x00000 };
static unsigned int uc01E2B[] = { 0x01E2A, 0x00000 };
static unsigned int uc01E2D[] = { 0x01E2C, 0x00000 };
static unsigned int uc01E2F[] = { 0x01E2E, 0x00000 };
static unsigned int uc01E31[] = { 0x01E30, 0x00000 };
static unsigned int uc01E33[] = { 0x01E32, 0x00000 };
static unsigned int uc01E35[] = { 0x01E34, 0x00000 };
static unsigned int uc01E37[] = { 0x01E36, 0x00000 };
static unsigned int uc01E39[] = { 0x01E38, 0x00000 };
static unsigned int uc01E3B[] = { 0x01E3A, 0x00000 };
static unsigned int uc01E3D[] = { 0x01E3C, 0x00000 };
static unsigned int uc01E3F[] = { 0x01E3E, 0x00000 };
static unsigned int uc01E41[] = { 0x01E40, 0x00000 };
static unsigned int uc01E43[] = { 0x01E42, 0x00000 };
static unsigned int uc01E45[] = { 0x01E44, 0x00000 };
static unsigned int uc01E47[] = { 0x01E46, 0x00000 };
static unsigned int uc01E49[] = { 0x01E48, 0x00000 };
static unsigned int uc01E4B[] = { 0x01E4A, 0x00000 };
static unsigned int uc01E4D[] = { 0x01E4C, 0x00000 };
static unsigned int uc01E4F[] = { 0x01E4E, 0x00000 };
static unsigned int uc01E51[] = { 0x01E50, 0x00000 };
static unsigned int uc01E53[] = { 0x01E52, 0x00000 };
static unsigned int uc01E55[] = { 0x01E54, 0x00000 };
static unsigned int uc01E57[] = { 0x01E56, 0x00000 };
static unsigned int uc01E59[] = { 0x01E58, 0x00000 };
static unsigned int uc01E5B[] = { 0x01E5A, 0x00000 };
static unsigned int uc01E5D[] = { 0x01E5C, 0x00000 };
static unsigned int uc01E5F[] = { 0x01E5E, 0x00000 };
static unsigned int uc01E61[] = { 0x01E60, 0x00000 };
static unsigned int uc01E63[] = { 0x01E62, 0x00000 };
static unsigned int uc01E65[] = { 0x01E64, 0x00000 };
static unsigned int uc01E67[] = { 0x01E66, 0x00000 };
static unsigned int uc01E69[] = { 0x01E68, 0x00000 };
static unsigned int uc01E6B[] = { 0x01E6A, 0x00000 };
static unsigned int uc01E6D[] = { 0x01E6C, 0x00000 };
static unsigned int uc01E6F[] = { 0x01E6E, 0x00000 };
static unsigned int uc01E71[] = { 0x01E70, 0x00000 };
static unsigned int uc01E73[] = { 0x01E72, 0x00000 };
static unsigned int uc01E75[] = { 0x01E74, 0x00000 };
static unsigned int uc01E77[] = { 0x01E76, 0x00000 };
static unsigned int uc01E79[] = { 0x01E78, 0x00000 };
static unsigned int uc01E7B[] = { 0x01E7A, 0x00000 };
static unsigned int uc01E7D[] = { 0x01E7C, 0x00000 };
static unsigned int uc01E7F[] = { 0x01E7E, 0x00000 };
static unsigned int uc01E81[] = { 0x01E80, 0x00000 };
static unsigned int uc01E83[] = { 0x01E82, 0x00000 };
static unsigned int uc01E85[] = { 0x01E84, 0x00000 };
static unsigned int uc01E87[] = { 0x01E86, 0x00000 };
static unsigned int uc01E89[] = { 0x01E88, 0x00000 };
static unsigned int uc01E8B[] = { 0x01E8A, 0x00000 };
static unsigned int uc01E8D[] = { 0x01E8C, 0x00000 };
static unsigned int uc01E8F[] = { 0x01E8E, 0x00000 };
static unsigned int uc01E91[] = { 0x01E90, 0x00000 };
static unsigned int uc01E93[] = { 0x01E92, 0x00000 };
static unsigned int uc01E95[] = { 0x01E94, 0x00000 };
static unsigned int uc01E96[] = { 0x00048, 0x00331, 0x00000 };
static unsigned int uc01E97[] = { 0x00054, 0x00308, 0x00000 };
static unsigned int uc01E98[] = { 0x00057, 0x0030A, 0x00000 };
static unsigned int uc01E99[] = { 0x00059, 0x0030A, 0x00000 };
static unsigned int uc01E9A[] = { 0x00041, 0x002BE, 0x00000 };
static unsigned int uc01E9B[] = { 0x01E60, 0x00000 };
static unsigned int uc01EA1[] = { 0x01EA0, 0x00000 };
static unsigned int uc01EA3[] = { 0x01EA2, 0x00000 };
static unsigned int uc01EA5[] = { 0x01EA4, 0x00000 };
static unsigned int uc01EA7[] = { 0x01EA6, 0x00000 };
static unsigned int uc01EA9[] = { 0x01EA8, 0x00000 };
static unsigned int uc01EAB[] = { 0x01EAA, 0x00000 };
static unsigned int uc01EAD[] = { 0x01EAC, 0x00000 };
static unsigned int uc01EAF[] = { 0x01EAE, 0x00000 };
static unsigned int uc01EB1[] = { 0x01EB0, 0x00000 };
static unsigned int uc01EB3[] = { 0x01EB2, 0x00000 };
static unsigned int uc01EB5[] = { 0x01EB4, 0x00000 };
static unsigned int uc01EB7[] = { 0x01EB6, 0x00000 };
static unsigned int uc01EB9[] = { 0x01EB8, 0x00000 };
static unsigned int uc01EBB[] = { 0x01EBA, 0x00000 };
static unsigned int uc01EBD[] = { 0x01EBC, 0x00000 };
static unsigned int uc01EBF[] = { 0x01EBE, 0x00000 };
static unsigned int uc01EC1[] = { 0x01EC0, 0x00000 };
static unsigned int uc01EC3[] = { 0x01EC2, 0x00000 };
static unsigned int uc01EC5[] = { 0x01EC4, 0x00000 };
static unsigned int uc01EC7[] = { 0x01EC6, 0x00000 };
static unsigned int uc01EC9[] = { 0x01EC8, 0x00000 };
static unsigned int uc01ECB[] = { 0x01ECA, 0x00000 };
static unsigned int uc01ECD[] = { 0x01ECC, 0x00000 };
static unsigned int uc01ECF[] = { 0x01ECE, 0x00000 };
static unsigned int uc01ED1[] = { 0x01ED0, 0x00000 };
static unsigned int uc01ED3[] = { 0x01ED2, 0x00000 };
static unsigned int uc01ED5[] = { 0x01ED4, 0x00000 };
static unsigned int uc01ED7[] = { 0x01ED6, 0x00000 };
static unsigned int uc01ED9[] = { 0x01ED8, 0x00000 };
static unsigned int uc01EDB[] = { 0x01EDA, 0x00000 };
static unsigned int uc01EDD[] = { 0x01EDC, 0x00000 };
static unsigned int uc01EDF[] = { 0x01EDE, 0x00000 };
static unsigned int uc01EE1[] = { 0x01EE0, 0x00000 };
static unsigned int uc01EE3[] = { 0x01EE2, 0x00000 };
static unsigned int uc01EE5[] = { 0x01EE4, 0x00000 };
static unsigned int uc01EE7[] = { 0x01EE6, 0x00000 };
static unsigned int uc01EE9[] = { 0x01EE8, 0x00000 };
static unsigned int uc01EEB[] = { 0x01EEA, 0x00000 };
static unsigned int uc01EED[] = { 0x01EEC, 0x00000 };
static unsigned int uc01EEF[] = { 0x01EEE, 0x00000 };
static unsigned int uc01EF1[] = { 0x01EF0, 0x00000 };
static unsigned int uc01EF3[] = { 0x01EF2, 0x00000 };
static unsigned int uc01EF5[] = { 0x01EF4, 0x00000 };
static unsigned int uc01EF7[] = { 0x01EF6, 0x00000 };
static unsigned int uc01EF9[] = { 0x01EF8, 0x00000 };
static unsigned int uc01F00[] = { 0x01F08, 0x00000 };
static unsigned int uc01F01[] = { 0x01F09, 0x00000 };
static unsigned int uc01F02[] = { 0x01F0A, 0x00000 };
static unsigned int uc01F03[] = { 0x01F0B, 0x00000 };
static unsigned int uc01F04[] = { 0x01F0C, 0x00000 };
static unsigned int uc01F05[] = { 0x01F0D, 0x00000 };
static unsigned int uc01F06[] = { 0x01F0E, 0x00000 };
static unsigned int uc01F07[] = { 0x01F0F, 0x00000 };
static unsigned int uc01F10[] = { 0x01F18, 0x00000 };
static unsigned int uc01F11[] = { 0x01F19, 0x00000 };
static unsigned int uc01F12[] = { 0x01F1A, 0x00000 };
static unsigned int uc01F13[] = { 0x01F1B, 0x00000 };
static unsigned int uc01F14[] = { 0x01F1C, 0x00000 };
static unsigned int uc01F15[] = { 0x01F1D, 0x00000 };
static unsigned int uc01F20[] = { 0x01F28, 0x00000 };
static unsigned int uc01F21[] = { 0x01F29, 0x00000 };
static unsigned int uc01F22[] = { 0x01F2A, 0x00000 };
static unsigned int uc01F23[] = { 0x01F2B, 0x00000 };
static unsigned int uc01F24[] = { 0x01F2C, 0x00000 };
static unsigned int uc01F25[] = { 0x01F2D, 0x00000 };
static unsigned int uc01F26[] = { 0x01F2E, 0x00000 };
static unsigned int uc01F27[] = { 0x01F2F, 0x00000 };
static unsigned int uc01F30[] = { 0x01F38, 0x00000 };
static unsigned int uc01F31[] = { 0x01F39, 0x00000 };
static unsigned int uc01F32[] = { 0x01F3A, 0x00000 };
static unsigned int uc01F33[] = { 0x01F3B, 0x00000 };
static unsigned int uc01F34[] = { 0x01F3C, 0x00000 };
static unsigned int uc01F35[] = { 0x01F3D, 0x00000 };
static unsigned int uc01F36[] = { 0x01F3E, 0x00000 };
static unsigned int uc01F37[] = { 0x01F3F, 0x00000 };
static unsigned int uc01F40[] = { 0x01F48, 0x00000 };
static unsigned int uc01F41[] = { 0x01F49, 0x00000 };
static unsigned int uc01F42[] = { 0x01F4A, 0x00000 };
static unsigned int uc01F43[] = { 0x01F4B, 0x00000 };
static unsigned int uc01F44[] = { 0x01F4C, 0x00000 };
static unsigned int uc01F45[] = { 0x01F4D, 0x00000 };
static unsigned int uc01F50[] = { 0x003A5, 0x00313, 0x00000 };
static unsigned int uc01F51[] = { 0x01F59, 0x00000 };
static unsigned int uc01F52[] = { 0x003A5, 0x00313, 0x00300, 0x00000 };
static unsigned int uc01F53[] = { 0x01F5B, 0x00000 };
static unsigned int uc01F54[] = { 0x003A5, 0x00313, 0x00301, 0x00000 };
static unsigned int uc01F55[] = { 0x01F5D, 0x00000 };
static unsigned int uc01F56[] = { 0x003A5, 0x00313, 0x00342, 0x00000 };
static unsigned int uc01F57[] = { 0x01F5F, 0x00000 };
static unsigned int uc01F60[] = { 0x01F68, 0x00000 };
static unsigned int uc01F61[] = { 0x01F69, 0x00000 };
static unsigned int uc01F62[] = { 0x01F6A, 0x00000 };
static unsigned int uc01F63[] = { 0x01F6B, 0x00000 };
static unsigned int uc01F64[] = { 0x01F6C, 0x00000 };
static unsigned int uc01F65[] = { 0x01F6D, 0x00000 };
static unsigned int uc01F66[] = { 0x01F6E, 0x00000 };
static unsigned int uc01F67[] = { 0x01F6F, 0x00000 };
static unsigned int uc01F70[] = { 0x01FBA, 0x00000 };
static unsigned int uc01F71[] = { 0x01FBB, 0x00000 };
static unsigned int uc01F72[] = { 0x01FC8, 0x00000 };
static unsigned int uc01F73[] = { 0x01FC9, 0x00000 };
static unsigned int uc01F74[] = { 0x01FCA, 0x00000 };
static unsigned int uc01F75[] = { 0x01FCB, 0x00000 };
static unsigned int uc01F76[] = { 0x01FDA, 0x00000 };
static unsigned int uc01F77[] = { 0x01FDB, 0x00000 };
static unsigned int uc01F78[] = { 0x01FF8, 0x00000 };
static unsigned int uc01F79[] = { 0x01FF9, 0x00000 };
static unsigned int uc01F7A[] = { 0x01FEA, 0x00000 };
static unsigned int uc01F7B[] = { 0x01FEB, 0x00000 };
static unsigned int uc01F7C[] = { 0x01FFA, 0x00000 };
static unsigned int uc01F7D[] = { 0x01FFB, 0x00000 };
static unsigned int uc01F80[] = { 0x01F08, 0x00399, 0x00000 };
static unsigned int uc01F81[] = { 0x01F09, 0x00399, 0x00000 };
static unsigned int uc01F82[] = { 0x01F0A, 0x00399, 0x00000 };
static unsigned int uc01F83[] = { 0x01F0B, 0x00399, 0x00000 };
static unsigned int uc01F84[] = { 0x01F0C, 0x00399, 0x00000 };
static unsigned int uc01F85[] = { 0x01F0D, 0x00399, 0x00000 };
static unsigned int uc01F86[] = { 0x01F0E, 0x00399, 0x00000 };
static unsigned int uc01F87[] = { 0x01F0F, 0x00399, 0x00000 };
static unsigned int uc01F88[] = { 0x01F08, 0x00399, 0x00000 };
static unsigned int uc01F89[] = { 0x01F09, 0x00399, 0x00000 };
static unsigned int uc01F8A[] = { 0x01F0A, 0x00399, 0x00000 };
static unsigned int uc01F8B[] = { 0x01F0B, 0x00399, 0x00000 };
static unsigned int uc01F8C[] = { 0x01F0C, 0x00399, 0x00000 };
static unsigned int uc01F8D[] = { 0x01F0D, 0x00399, 0x00000 };
static unsigned int uc01F8E[] = { 0x01F0E, 0x00399, 0x00000 };
static unsigned int uc01F8F[] = { 0x01F0F, 0x00399, 0x00000 };
static unsigned int uc01F90[] = { 0x01F28, 0x00399, 0x00000 };
static unsigned int uc01F91[] = { 0x01F29, 0x00399, 0x00000 };
static unsigned int uc01F92[] = { 0x01F2A, 0x00399, 0x00000 };
static unsigned int uc01F93[] = { 0x01F2B, 0x00399, 0x00000 };
static unsigned int uc01F94[] = { 0x01F2C, 0x00399, 0x00000 };
static unsigned int uc01F95[] = { 0x01F2D, 0x00399, 0x00000 };
static unsigned int uc01F96[] = { 0x01F2E, 0x00399, 0x00000 };
static unsigned int uc01F97[] = { 0x01F2F, 0x00399, 0x00000 };
static unsigned int uc01F98[] = { 0x01F28, 0x00399, 0x00000 };
static unsigned int uc01F99[] = { 0x01F29, 0x00399, 0x00000 };
static unsigned int uc01F9A[] = { 0x01F2A, 0x00399, 0x00000 };
static unsigned int uc01F9B[] = { 0x01F2B, 0x00399, 0x00000 };
static unsigned int uc01F9C[] = { 0x01F2C, 0x00399, 0x00000 };
static unsigned int uc01F9D[] = { 0x01F2D, 0x00399, 0x00000 };
static unsigned int uc01F9E[] = { 0x01F2E, 0x00399, 0x00000 };
static unsigned int uc01F9F[] = { 0x01F2F, 0x00399, 0x00000 };
static unsigned int uc01FA0[] = { 0x01F68, 0x00399, 0x00000 };
static unsigned int uc01FA1[] = { 0x01F69, 0x00399, 0x00000 };
static unsigned int uc01FA2[] = { 0x01F6A, 0x00399, 0x00000 };
static unsigned int uc01FA3[] = { 0x01F6B, 0x00399, 0x00000 };
static unsigned int uc01FA4[] = { 0x01F6C, 0x00399, 0x00000 };
static unsigned int uc01FA5[] = { 0x01F6D, 0x00399, 0x00000 };
static unsigned int uc01FA6[] = { 0x01F6E, 0x00399, 0x00000 };
static unsigned int uc01FA7[] = { 0x01F6F, 0x00399, 0x00000 };
static unsigned int uc01FA8[] = { 0x01F68, 0x00399, 0x00000 };
static unsigned int uc01FA9[] = { 0x01F69, 0x00399, 0x00000 };
static unsigned int uc01FAA[] = { 0x01F6A, 0x00399, 0x00000 };
static unsigned int uc01FAB[] = { 0x01F6B, 0x00399, 0x00000 };
static unsigned int uc01FAC[] = { 0x01F6C, 0x00399, 0x00000 };
static unsigned int uc01FAD[] = { 0x01F6D, 0x00399, 0x00000 };
static unsigned int uc01FAE[] = { 0x01F6E, 0x00399, 0x00000 };
static unsigned int uc01FAF[] = { 0x01F6F, 0x00399, 0x00000 };
static unsigned int uc01FB0[] = { 0x01FB8, 0x00000 };
static unsigned int uc01FB1[] = { 0x01FB9, 0x00000 };
static unsigned int uc01FB2[] = { 0x01FBA, 0x00399, 0x00000 };
static unsigned int uc01FB3[] = { 0x00391, 0x00399, 0x00000 };
static unsigned int uc01FB4[] = { 0x00386, 0x00399, 0x00000 };
static unsigned int uc01FB6[] = { 0x00391, 0x00342, 0x00000 };
static unsigned int uc01FB7[] = { 0x00391, 0x00342, 0x00399, 0x00000 };
static unsigned int uc01FBC[] = { 0x00391, 0x00399, 0x00000 };
static unsigned int uc01FBE[] = { 0x00399, 0x00000 };
static unsigned int uc01FC2[] = { 0x01FCA, 0x00399, 0x00000 };
static unsigned int uc01FC3[] = { 0x00397, 0x00399, 0x00000 };
static unsigned int uc01FC4[] = { 0x00389, 0x00399, 0x00000 };
static unsigned int uc01FC6[] = { 0x00397, 0x00342, 0x00000 };
static unsigned int uc01FC7[] = { 0x00397, 0x00342, 0x00399, 0x00000 };
static unsigned int uc01FCC[] = { 0x00397, 0x00399, 0x00000 };
static unsigned int uc01FD0[] = { 0x01FD8, 0x00000 };
static unsigned int uc01FD1[] = { 0x01FD9, 0x00000 };
static unsigned int uc01FD2[] = { 0x00399, 0x00308, 0x00300, 0x00000 };
static unsigned int uc01FD3[] = { 0x00399, 0x00308, 0x00301, 0x00000 };
static unsigned int uc01FD6[] = { 0x00399, 0x00342, 0x00000 };
static unsigned int uc01FD7[] = { 0x00399, 0x00308, 0x00342, 0x00000 };
static unsigned int uc01FE0[] = { 0x01FE8, 0x00000 };
static unsigned int uc01FE1[] = { 0x01FE9, 0x00000 };
static unsigned int uc01FE2[] = { 0x003A5, 0x00308, 0x00300, 0x00000 };
static unsigned int uc01FE3[] = { 0x003A5, 0x00308, 0x00301, 0x00000 };
static unsigned int uc01FE4[] = { 0x003A1, 0x00313, 0x00000 };
static unsigned int uc01FE5[] = { 0x01FEC, 0x00000 };
static unsigned int uc01FE6[] = { 0x003A5, 0x00342, 0x00000 };
static unsigned int uc01FE7[] = { 0x003A5, 0x00308, 0x00342, 0x00000 };
static unsigned int uc01FF2[] = { 0x01FFA, 0x00399, 0x00000 };
static unsigned int uc01FF3[] = { 0x003A9, 0x00399, 0x00000 };
static unsigned int uc01FF4[] = { 0x0038F, 0x00399, 0x00000 };
static unsigned int uc01FF6[] = { 0x003A9, 0x00342, 0x00000 };
static unsigned int uc01FF7[] = { 0x003A9, 0x00342, 0x00399, 0x00000 };
static unsigned int uc01FFC[] = { 0x003A9, 0x00399, 0x00000 };
static unsigned int uc0214E[] = { 0x02132, 0x00000 };
static unsigned int uc02170[] = { 0x02160, 0x00000 };
static unsigned int uc02171[] = { 0x02161, 0x00000 };
static unsigned int uc02172[] = { 0x02162, 0x00000 };
static unsigned int uc02173[] = { 0x02163, 0x00000 };
static unsigned int uc02174[] = { 0x02164, 0x00000 };
static unsigned int uc02175[] = { 0x02165, 0x00000 };
static unsigned int uc02176[] = { 0x02166, 0x00000 };
static unsigned int uc02177[] = { 0x02167, 0x00000 };
static unsigned int uc02178[] = { 0x02168, 0x00000 };
static unsigned int uc02179[] = { 0x02169, 0x00000 };
static unsigned int uc0217A[] = { 0x0216A, 0x00000 };
static unsigned int uc0217B[] = { 0x0216B, 0x00000 };
static unsigned int uc0217C[] = { 0x0216C, 0x00000 };
static unsigned int uc0217D[] = { 0x0216D, 0x00000 };
static unsigned int uc0217E[] = { 0x0216E, 0x00000 };
static unsigned int uc0217F[] = { 0x0216F, 0x00000 };
static unsigned int uc02184[] = { 0x02183, 0x00000 };
static unsigned int uc024D0[] = { 0x024B6, 0x00000 };
static unsigned int uc024D1[] = { 0x024B7, 0x00000 };
static unsigned int uc024D2[] = { 0x024B8, 0x00000 };
static unsigned int uc024D3[] = { 0x024B9, 0x00000 };
static unsigned int uc024D4[] = { 0x024BA, 0x00000 };
static unsigned int uc024D5[] = { 0x024BB, 0x00000 };
static unsigned int uc024D6[] = { 0x024BC, 0x00000 };
static unsigned int uc024D7[] = { 0x024BD, 0x00000 };
static unsigned int uc024D8[] = { 0x024BE, 0x00000 };
static unsigned int uc024D9[] = { 0x024BF, 0x00000 };
static unsigned int uc024DA[] = { 0x024C0, 0x00000 };
static unsigned int uc024DB[] = { 0x024C1, 0x00000 };
static unsigned int uc024DC[] = { 0x024C2, 0x00000 };
static unsigned int uc024DD[] = { 0x024C3, 0x00000 };
static unsigned int uc024DE[] = { 0x024C4, 0x00000 };
static unsigned int uc024DF[] = { 0x024C5, 0x00000 };
static unsigned int uc024E0[] = { 0x024C6, 0x00000 };
static unsigned int uc024E1[] = { 0x024C7, 0x00000 };
static unsigned int uc024E2[] = { 0x024C8, 0x00000 };
static unsigned int uc024E3[] = { 0x024C9, 0x00000 };
static unsigned int uc024E4[] = { 0x024CA, 0x00000 };
static unsigned int uc024E5[] = { 0x024CB, 0x00000 };
static unsigned int uc024E6[] = { 0x024CC, 0x00000 };
static unsigned int uc024E7[] = { 0x024CD, 0x00000 };
static unsigned int uc024E8[] = { 0x024CE, 0x00000 };
static unsigned int uc024E9[] = { 0x024CF, 0x00000 };
static unsigned int uc02C30[] = { 0x02C00, 0x00000 };
static unsigned int uc02C31[] = { 0x02C01, 0x00000 };
static unsigned int uc02C32[] = { 0x02C02, 0x00000 };
static unsigned int uc02C33[] = { 0x02C03, 0x00000 };
static unsigned int uc02C34[] = { 0x02C04, 0x00000 };
static unsigned int uc02C35[] = { 0x02C05, 0x00000 };
static unsigned int uc02C36[] = { 0x02C06, 0x00000 };
static unsigned int uc02C37[] = { 0x02C07, 0x00000 };
static unsigned int uc02C38[] = { 0x02C08, 0x00000 };
static unsigned int uc02C39[] = { 0x02C09, 0x00000 };
static unsigned int uc02C3A[] = { 0x02C0A, 0x00000 };
static unsigned int uc02C3B[] = { 0x02C0B, 0x00000 };
static unsigned int uc02C3C[] = { 0x02C0C, 0x00000 };
static unsigned int uc02C3D[] = { 0x02C0D, 0x00000 };
static unsigned int uc02C3E[] = { 0x02C0E, 0x00000 };
static unsigned int uc02C3F[] = { 0x02C0F, 0x00000 };
static unsigned int uc02C40[] = { 0x02C10, 0x00000 };
static unsigned int uc02C41[] = { 0x02C11, 0x00000 };
static unsigned int uc02C42[] = { 0x02C12, 0x00000 };
static unsigned int uc02C43[] = { 0x02C13, 0x00000 };
static unsigned int uc02C44[] = { 0x02C14, 0x00000 };
static unsigned int uc02C45[] = { 0x02C15, 0x00000 };
static unsigned int uc02C46[] = { 0x02C16, 0x00000 };
static unsigned int uc02C47[] = { 0x02C17, 0x00000 };
static unsigned int uc02C48[] = { 0x02C18, 0x00000 };
static unsigned int uc02C49[] = { 0x02C19, 0x00000 };
static unsigned int uc02C4A[] = { 0x02C1A, 0x00000 };
static unsigned int uc02C4B[] = { 0x02C1B, 0x00000 };
static unsigned int uc02C4C[] = { 0x02C1C, 0x00000 };
static unsigned int uc02C4D[] = { 0x02C1D, 0x00000 };
static unsigned int uc02C4E[] = { 0x02C1E, 0x00000 };
static unsigned int uc02C4F[] = { 0x02C1F, 0x00000 };
static unsigned int uc02C50[] = { 0x02C20, 0x00000 };
static unsigned int uc02C51[] = { 0x02C21, 0x00000 };
static unsigned int uc02C52[] = { 0x02C22, 0x00000 };
static unsigned int uc02C53[] = { 0x02C23, 0x00000 };
static unsigned int uc02C54[] = { 0x02C24, 0x00000 };
static unsigned int uc02C55[] = { 0x02C25, 0x00000 };
static unsigned int uc02C56[] = { 0x02C26, 0x00000 };
static unsigned int uc02C57[] = { 0x02C27, 0x00000 };
static unsigned int uc02C58[] = { 0x02C28, 0x00000 };
static unsigned int uc02C59[] = { 0x02C29, 0x00000 };
static unsigned int uc02C5A[] = { 0x02C2A, 0x00000 };
static unsigned int uc02C5B[] = { 0x02C2B, 0x00000 };
static unsigned int uc02C5C[] = { 0x02C2C, 0x00000 };
static unsigned int uc02C5D[] = { 0x02C2D, 0x00000 };
static unsigned int uc02C5E[] = { 0x02C2E, 0x00000 };
static unsigned int uc02C61[] = { 0x02C60, 0x00000 };
static unsigned int uc02C65[] = { 0x0023A, 0x00000 };
static unsigned int uc02C66[] = { 0x0023E, 0x00000 };
static unsigned int uc02C68[] = { 0x02C67, 0x00000 };
static unsigned int uc02C6A[] = { 0x02C69, 0x00000 };
static unsigned int uc02C6C[] = { 0x02C6B, 0x00000 };
static unsigned int uc02C76[] = { 0x02C75, 0x00000 };
static unsigned int uc02C81[] = { 0x02C80, 0x00000 };
static unsigned int uc02C83[] = { 0x02C82, 0x00000 };
static unsigned int uc02C85[] = { 0x02C84, 0x00000 };
static unsigned int uc02C87[] = { 0x02C86, 0x00000 };
static unsigned int uc02C89[] = { 0x02C88, 0x00000 };
static unsigned int uc02C8B[] = { 0x02C8A, 0x00000 };
static unsigned int uc02C8D[] = { 0x02C8C, 0x00000 };
static unsigned int uc02C8F[] = { 0x02C8E, 0x00000 };
static unsigned int uc02C91[] = { 0x02C90, 0x00000 };
static unsigned int uc02C93[] = { 0x02C92, 0x00000 };
static unsigned int uc02C95[] = { 0x02C94, 0x00000 };
static unsigned int uc02C97[] = { 0x02C96, 0x00000 };
static unsigned int uc02C99[] = { 0x02C98, 0x00000 };
static unsigned int uc02C9B[] = { 0x02C9A, 0x00000 };
static unsigned int uc02C9D[] = { 0x02C9C, 0x00000 };
static unsigned int uc02C9F[] = { 0x02C9E, 0x00000 };
static unsigned int uc02CA1[] = { 0x02CA0, 0x00000 };
static unsigned int uc02CA3[] = { 0x02CA2, 0x00000 };
static unsigned int uc02CA5[] = { 0x02CA4, 0x00000 };
static unsigned int uc02CA7[] = { 0x02CA6, 0x00000 };
static unsigned int uc02CA9[] = { 0x02CA8, 0x00000 };
static unsigned int uc02CAB[] = { 0x02CAA, 0x00000 };
static unsigned int uc02CAD[] = { 0x02CAC, 0x00000 };
static unsigned int uc02CAF[] = { 0x02CAE, 0x00000 };
static unsigned int uc02CB1[] = { 0x02CB0, 0x00000 };
static unsigned int uc02CB3[] = { 0x02CB2, 0x00000 };
static unsigned int uc02CB5[] = { 0x02CB4, 0x00000 };
static unsigned int uc02CB7[] = { 0x02CB6, 0x00000 };
static unsigned int uc02CB9[] = { 0x02CB8, 0x00000 };
static unsigned int uc02CBB[] = { 0x02CBA, 0x00000 };
static unsigned int uc02CBD[] = { 0x02CBC, 0x00000 };
static unsigned int uc02CBF[] = { 0x02CBE, 0x00000 };
static unsigned int uc02CC1[] = { 0x02CC0, 0x00000 };
static unsigned int uc02CC3[] = { 0x02CC2, 0x00000 };
static unsigned int uc02CC5[] = { 0x02CC4, 0x00000 };
static unsigned int uc02CC7[] = { 0x02CC6, 0x00000 };
static unsigned int uc02CC9[] = { 0x02CC8, 0x00000 };
static unsigned int uc02CCB[] = { 0x02CCA, 0x00000 };
static unsigned int uc02CCD[] = { 0x02CCC, 0x00000 };
static unsigned int uc02CCF[] = { 0x02CCE, 0x00000 };
static unsigned int uc02CD1[] = { 0x02CD0, 0x00000 };
static unsigned int uc02CD3[] = { 0x02CD2, 0x00000 };
static unsigned int uc02CD5[] = { 0x02CD4, 0x00000 };
static unsigned int uc02CD7[] = { 0x02CD6, 0x00000 };
static unsigned int uc02CD9[] = { 0x02CD8, 0x00000 };
static unsigned int uc02CDB[] = { 0x02CDA, 0x00000 };
static unsigned int uc02CDD[] = { 0x02CDC, 0x00000 };
static unsigned int uc02CDF[] = { 0x02CDE, 0x00000 };
static unsigned int uc02CE1[] = { 0x02CE0, 0x00000 };
static unsigned int uc02CE3[] = { 0x02CE2, 0x00000 };
static unsigned int uc02D00[] = { 0x010A0, 0x00000 };
static unsigned int uc02D01[] = { 0x010A1, 0x00000 };
static unsigned int uc02D02[] = { 0x010A2, 0x00000 };
static unsigned int uc02D03[] = { 0x010A3, 0x00000 };
static unsigned int uc02D04[] = { 0x010A4, 0x00000 };
static unsigned int uc02D05[] = { 0x010A5, 0x00000 };
static unsigned int uc02D06[] = { 0x010A6, 0x00000 };
static unsigned int uc02D07[] = { 0x010A7, 0x00000 };
static unsigned int uc02D08[] = { 0x010A8, 0x00000 };
static unsigned int uc02D09[] = { 0x010A9, 0x00000 };
static unsigned int uc02D0A[] = { 0x010AA, 0x00000 };
static unsigned int uc02D0B[] = { 0x010AB, 0x00000 };
static unsigned int uc02D0C[] = { 0x010AC, 0x00000 };
static unsigned int uc02D0D[] = { 0x010AD, 0x00000 };
static unsigned int uc02D0E[] = { 0x010AE, 0x00000 };
static unsigned int uc02D0F[] = { 0x010AF, 0x00000 };
static unsigned int uc02D10[] = { 0x010B0, 0x00000 };
static unsigned int uc02D11[] = { 0x010B1, 0x00000 };
static unsigned int uc02D12[] = { 0x010B2, 0x00000 };
static unsigned int uc02D13[] = { 0x010B3, 0x00000 };
static unsigned int uc02D14[] = { 0x010B4, 0x00000 };
static unsigned int uc02D15[] = { 0x010B5, 0x00000 };
static unsigned int uc02D16[] = { 0x010B6, 0x00000 };
static unsigned int uc02D17[] = { 0x010B7, 0x00000 };
static unsigned int uc02D18[] = { 0x010B8, 0x00000 };
static unsigned int uc02D19[] = { 0x010B9, 0x00000 };
static unsigned int uc02D1A[] = { 0x010BA, 0x00000 };
static unsigned int uc02D1B[] = { 0x010BB, 0x00000 };
static unsigned int uc02D1C[] = { 0x010BC, 0x00000 };
static unsigned int uc02D1D[] = { 0x010BD, 0x00000 };
static unsigned int uc02D1E[] = { 0x010BE, 0x00000 };
static unsigned int uc02D1F[] = { 0x010BF, 0x00000 };
static unsigned int uc02D20[] = { 0x010C0, 0x00000 };
static unsigned int uc02D21[] = { 0x010C1, 0x00000 };
static unsigned int uc02D22[] = { 0x010C2, 0x00000 };
static unsigned int uc02D23[] = { 0x010C3, 0x00000 };
static unsigned int uc02D24[] = { 0x010C4, 0x00000 };
static unsigned int uc02D25[] = { 0x010C5, 0x00000 };
static unsigned int uc0FB00[] = { 0x00046, 0x00046, 0x00000 };
static unsigned int uc0FB01[] = { 0x00046, 0x00049, 0x00000 };
static unsigned int uc0FB02[] = { 0x00046, 0x0004C, 0x00000 };
static unsigned int uc0FB03[] = { 0x00046, 0x00046, 0x00049, 0x00000 };
static unsigned int uc0FB04[] = { 0x00046, 0x00046, 0x0004C, 0x00000 };
static unsigned int uc0FB05[] = { 0x00053, 0x00054, 0x00000 };
static unsigned int uc0FB06[] = { 0x00053, 0x00054, 0x00000 };
static unsigned int uc0FB13[] = { 0x00544, 0x00546, 0x00000 };
static unsigned int uc0FB14[] = { 0x00544, 0x00535, 0x00000 };
static unsigned int uc0FB15[] = { 0x00544, 0x0053B, 0x00000 };
static unsigned int uc0FB16[] = { 0x0054E, 0x00546, 0x00000 };
static unsigned int uc0FB17[] = { 0x00544, 0x0053D, 0x00000 };
static unsigned int uc0FF41[] = { 0x0FF21, 0x00000 };
static unsigned int uc0FF42[] = { 0x0FF22, 0x00000 };
static unsigned int uc0FF43[] = { 0x0FF23, 0x00000 };
static unsigned int uc0FF44[] = { 0x0FF24, 0x00000 };
static unsigned int uc0FF45[] = { 0x0FF25, 0x00000 };
static unsigned int uc0FF46[] = { 0x0FF26, 0x00000 };
static unsigned int uc0FF47[] = { 0x0FF27, 0x00000 };
static unsigned int uc0FF48[] = { 0x0FF28, 0x00000 };
static unsigned int uc0FF49[] = { 0x0FF29, 0x00000 };
static unsigned int uc0FF4A[] = { 0x0FF2A, 0x00000 };
static unsigned int uc0FF4B[] = { 0x0FF2B, 0x00000 };
static unsigned int uc0FF4C[] = { 0x0FF2C, 0x00000 };
static unsigned int uc0FF4D[] = { 0x0FF2D, 0x00000 };
static unsigned int uc0FF4E[] = { 0x0FF2E, 0x00000 };
static unsigned int uc0FF4F[] = { 0x0FF2F, 0x00000 };
static unsigned int uc0FF50[] = { 0x0FF30, 0x00000 };
static unsigned int uc0FF51[] = { 0x0FF31, 0x00000 };
static unsigned int uc0FF52[] = { 0x0FF32, 0x00000 };
static unsigned int uc0FF53[] = { 0x0FF33, 0x00000 };
static unsigned int uc0FF54[] = { 0x0FF34, 0x00000 };
static unsigned int uc0FF55[] = { 0x0FF35, 0x00000 };
static unsigned int uc0FF56[] = { 0x0FF36, 0x00000 };
static unsigned int uc0FF57[] = { 0x0FF37, 0x00000 };
static unsigned int uc0FF58[] = { 0x0FF38, 0x00000 };
static unsigned int uc0FF59[] = { 0x0FF39, 0x00000 };
static unsigned int uc0FF5A[] = { 0x0FF3A, 0x00000 };
static unsigned int uc10428[] = { 0x10400, 0x00000 };
static unsigned int uc10429[] = { 0x10401, 0x00000 };
static unsigned int uc1042A[] = { 0x10402, 0x00000 };
static unsigned int uc1042B[] = { 0x10403, 0x00000 };
static unsigned int uc1042C[] = { 0x10404, 0x00000 };
static unsigned int uc1042D[] = { 0x10405, 0x00000 };
static unsigned int uc1042E[] = { 0x10406, 0x00000 };
static unsigned int uc1042F[] = { 0x10407, 0x00000 };
static unsigned int uc10430[] = { 0x10408, 0x00000 };
static unsigned int uc10431[] = { 0x10409, 0x00000 };
static unsigned int uc10432[] = { 0x1040A, 0x00000 };
static unsigned int uc10433[] = { 0x1040B, 0x00000 };
static unsigned int uc10434[] = { 0x1040C, 0x00000 };
static unsigned int uc10435[] = { 0x1040D, 0x00000 };
static unsigned int uc10436[] = { 0x1040E, 0x00000 };
static unsigned int uc10437[] = { 0x1040F, 0x00000 };
static unsigned int uc10438[] = { 0x10410, 0x00000 };
static unsigned int uc10439[] = { 0x10411, 0x00000 };
static unsigned int uc1043A[] = { 0x10412, 0x00000 };
static unsigned int uc1043B[] = { 0x10413, 0x00000 };
static unsigned int uc1043C[] = { 0x10414, 0x00000 };
static unsigned int uc1043D[] = { 0x10415, 0x00000 };
static unsigned int uc1043E[] = { 0x10416, 0x00000 };
static unsigned int uc1043F[] = { 0x10417, 0x00000 };
static unsigned int uc10440[] = { 0x10418, 0x00000 };
static unsigned int uc10441[] = { 0x10419, 0x00000 };
static unsigned int uc10442[] = { 0x1041A, 0x00000 };
static unsigned int uc10443[] = { 0x1041B, 0x00000 };
static unsigned int uc10444[] = { 0x1041C, 0x00000 };
static unsigned int uc10445[] = { 0x1041D, 0x00000 };
static unsigned int uc10446[] = { 0x1041E, 0x00000 };
static unsigned int uc10447[] = { 0x1041F, 0x00000 };
static unsigned int uc10448[] = { 0x10420, 0x00000 };
static unsigned int uc10449[] = { 0x10421, 0x00000 };
static unsigned int uc1044A[] = { 0x10422, 0x00000 };
static unsigned int uc1044B[] = { 0x10423, 0x00000 };
static unsigned int uc1044C[] = { 0x10424, 0x00000 };
static unsigned int uc1044D[] = { 0x10425, 0x00000 };
static unsigned int uc1044E[] = { 0x10426, 0x00000 };
static unsigned int uc1044F[] = { 0x10427, 0x00000 };

static unsigned int upperCaseDirectTableSize = 2048;
static unsigned int *upperCaseDirectTable[2048] = {
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, uc00061, uc00062, uc00063, uc00064, uc00065, uc00066, uc00067, uc00068, uc00069, uc0006A, uc0006B, uc0006C, uc0006D, uc0006E, uc0006F,
  uc00070, uc00071, uc00072, uc00073, uc00074, uc00075, uc00076, uc00077, uc00078, uc00079, uc0007A,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0, uc000B5,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0, uc000DF,
  uc000E0, uc000E1, uc000E2, uc000E3, uc000E4, uc000E5, uc000E6, uc000E7, uc000E8, uc000E9, uc000EA, uc000EB, uc000EC, uc000ED, uc000EE, uc000EF,
  uc000F0, uc000F1, uc000F2, uc000F3, uc000F4, uc000F5, uc000F6,       0, uc000F8, uc000F9, uc000FA, uc000FB, uc000FC, uc000FD, uc000FE, uc000FF,
        0, uc00101,       0, uc00103,       0, uc00105,       0, uc00107,       0, uc00109,       0, uc0010B,       0, uc0010D,       0, uc0010F,
        0, uc00111,       0, uc00113,       0, uc00115,       0, uc00117,       0, uc00119,       0, uc0011B,       0, uc0011D,       0, uc0011F,
        0, uc00121,       0, uc00123,       0, uc00125,       0, uc00127,       0, uc00129,       0, uc0012B,       0, uc0012D,       0, uc0012F,
  uc00130, uc00131,       0, uc00133,       0, uc00135,       0, uc00137,       0,       0, uc0013A,       0, uc0013C,       0, uc0013E,       0,
  uc00140,       0, uc00142,       0, uc00144,       0, uc00146,       0, uc00148, uc00149,       0, uc0014B,       0, uc0014D,       0, uc0014F,
        0, uc00151,       0, uc00153,       0, uc00155,       0, uc00157,       0, uc00159,       0, uc0015B,       0, uc0015D,       0, uc0015F,
        0, uc00161,       0, uc00163,       0, uc00165,       0, uc00167,       0, uc00169,       0, uc0016B,       0, uc0016D,       0, uc0016F,
        0, uc00171,       0, uc00173,       0, uc00175,       0, uc00177,       0,       0, uc0017A,       0, uc0017C,       0, uc0017E, uc0017F,
  uc00180,       0,       0, uc00183,       0, uc00185,       0,       0, uc00188,       0,       0,       0, uc0018C,       0,       0,       0,
        0,       0, uc00192,       0,       0, uc00195,       0,       0,       0, uc00199, uc0019A,       0,       0,       0, uc0019E,       0,
        0, uc001A1,       0, uc001A3,       0, uc001A5,       0,       0, uc001A8,       0,       0,       0,       0, uc001AD,       0,       0,
  uc001B0,       0,       0,       0, uc001B4,       0, uc001B6,       0,       0, uc001B9,       0,       0,       0, uc001BD,       0, uc001BF,
        0,       0,       0,       0,       0, uc001C5, uc001C6,       0, uc001C8, uc001C9,       0, uc001CB, uc001CC,       0, uc001CE,       0,
  uc001D0,       0, uc001D2,       0, uc001D4,       0, uc001D6,       0, uc001D8,       0, uc001DA,       0, uc001DC, uc001DD,       0, uc001DF,
        0, uc001E1,       0, uc001E3,       0, uc001E5,       0, uc001E7,       0, uc001E9,       0, uc001EB,       0, uc001ED,       0, uc001EF,
  uc001F0,       0, uc001F2, uc001F3,       0, uc001F5,       0,       0,       0, uc001F9,       0, uc001FB,       0, uc001FD,       0, uc001FF,
        0, uc00201,       0, uc00203,       0, uc00205,       0, uc00207,       0, uc00209,       0, uc0020B,       0, uc0020D,       0, uc0020F,
        0, uc00211,       0, uc00213,       0, uc00215,       0, uc00217,       0, uc00219,       0, uc0021B,       0, uc0021D,       0, uc0021F,
        0,       0,       0, uc00223,       0, uc00225,       0, uc00227,       0, uc00229,       0, uc0022B,       0, uc0022D,       0, uc0022F,
        0, uc00231,       0, uc00233,       0,       0,       0,       0,       0,       0,       0,       0, uc0023C,       0,       0,       0,
        0,       0, uc00242,       0,       0,       0,       0, uc00247,       0, uc00249,       0, uc0024B,       0, uc0024D,       0, uc0024F,
        0,       0,       0, uc00253, uc00254,       0, uc00256, uc00257,       0, uc00259,       0, uc0025B,       0,       0,       0,       0,
  uc00260,       0,       0, uc00263,       0,       0,       0,       0, uc00268, uc00269,       0, uc0026B,       0,       0,       0, uc0026F,
        0,       0, uc00272,       0,       0, uc00275,       0,       0,       0,       0,       0,       0,       0, uc0027D,       0,       0,
  uc00280,       0,       0, uc00283,       0,       0,       0,       0, uc00288, uc00289, uc0028A, uc0028B, uc0028C,       0,       0,       0,
        0,       0, uc00292,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
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
        0,       0,       0,       0,       0, uc00345,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0, uc0037B, uc0037C, uc0037D,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  uc00390,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0, uc003AC, uc003AD, uc003AE, uc003AF,
  uc003B0, uc003B1, uc003B2, uc003B3, uc003B4, uc003B5, uc003B6, uc003B7, uc003B8, uc003B9, uc003BA, uc003BB, uc003BC, uc003BD, uc003BE, uc003BF,
  uc003C0, uc003C1, uc003C2, uc003C3, uc003C4, uc003C5, uc003C6, uc003C7, uc003C8, uc003C9, uc003CA, uc003CB, uc003CC, uc003CD, uc003CE,       0,
  uc003D0, uc003D1,       0,       0,       0, uc003D5, uc003D6,       0,       0, uc003D9,       0, uc003DB,       0, uc003DD,       0, uc003DF,
        0, uc003E1,       0, uc003E3,       0, uc003E5,       0, uc003E7,       0, uc003E9,       0, uc003EB,       0, uc003ED,       0, uc003EF,
  uc003F0, uc003F1, uc003F2,       0,       0, uc003F5,       0,       0, uc003F8,       0,       0, uc003FB,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
  uc00430, uc00431, uc00432, uc00433, uc00434, uc00435, uc00436, uc00437, uc00438, uc00439, uc0043A, uc0043B, uc0043C, uc0043D, uc0043E, uc0043F,
  uc00440, uc00441, uc00442, uc00443, uc00444, uc00445, uc00446, uc00447, uc00448, uc00449, uc0044A, uc0044B, uc0044C, uc0044D, uc0044E, uc0044F,
  uc00450, uc00451, uc00452, uc00453, uc00454, uc00455, uc00456, uc00457, uc00458, uc00459, uc0045A, uc0045B, uc0045C, uc0045D, uc0045E, uc0045F,
        0, uc00461,       0, uc00463,       0, uc00465,       0, uc00467,       0, uc00469,       0, uc0046B,       0, uc0046D,       0, uc0046F,
        0, uc00471,       0, uc00473,       0, uc00475,       0, uc00477,       0, uc00479,       0, uc0047B,       0, uc0047D,       0, uc0047F,
        0, uc00481,       0,       0,       0,       0,       0,       0,       0,       0,       0, uc0048B,       0, uc0048D,       0, uc0048F,
        0, uc00491,       0, uc00493,       0, uc00495,       0, uc00497,       0, uc00499,       0, uc0049B,       0, uc0049D,       0, uc0049F,
        0, uc004A1,       0, uc004A3,       0, uc004A5,       0, uc004A7,       0, uc004A9,       0, uc004AB,       0, uc004AD,       0, uc004AF,
        0, uc004B1,       0, uc004B3,       0, uc004B5,       0, uc004B7,       0, uc004B9,       0, uc004BB,       0, uc004BD,       0, uc004BF,
        0,       0, uc004C2,       0, uc004C4,       0, uc004C6,       0, uc004C8,       0, uc004CA,       0, uc004CC,       0, uc004CE, uc004CF,
        0, uc004D1,       0, uc004D3,       0, uc004D5,       0, uc004D7,       0, uc004D9,       0, uc004DB,       0, uc004DD,       0, uc004DF,
        0, uc004E1,       0, uc004E3,       0, uc004E5,       0, uc004E7,       0, uc004E9,       0, uc004EB,       0, uc004ED,       0, uc004EF,
        0, uc004F1,       0, uc004F3,       0, uc004F5,       0, uc004F7,       0, uc004F9,       0, uc004FB,       0, uc004FD,       0, uc004FF,
        0, uc00501,       0, uc00503,       0, uc00505,       0, uc00507,       0, uc00509,       0, uc0050B,       0, uc0050D,       0, uc0050F,
        0, uc00511,       0, uc00513,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
        0, uc00561, uc00562, uc00563, uc00564, uc00565, uc00566, uc00567, uc00568, uc00569, uc0056A, uc0056B, uc0056C, uc0056D, uc0056E, uc0056F,
  uc00570, uc00571, uc00572, uc00573, uc00574, uc00575, uc00576, uc00577, uc00578, uc00579, uc0057A, uc0057B, uc0057C, uc0057D, uc0057E, uc0057F,
  uc00580, uc00581, uc00582, uc00583, uc00584, uc00585, uc00586, uc00587,       0,       0,       0,       0,       0,       0,       0,       0,
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

// static unsigned int upperCaseTableSize = 1007;
// static CaseValue upperCaseTable[1007] = {
static unsigned int upperCaseTableSize = 540;
static CaseValue upperCaseTable[540] = {
// { 0x00061, uc00061 }, { 0x00062, uc00062 }, { 0x00063, uc00063 }, { 0x00064, uc00064 }, { 0x00065, uc00065 }, { 0x00066, uc00066 }, { 0x00067, uc00067 }, { 0x00068, uc00068 },
// { 0x00069, uc00069 }, { 0x0006A, uc0006A }, { 0x0006B, uc0006B }, { 0x0006C, uc0006C }, { 0x0006D, uc0006D }, { 0x0006E, uc0006E }, { 0x0006F, uc0006F }, { 0x00070, uc00070 },
// { 0x00071, uc00071 }, { 0x00072, uc00072 }, { 0x00073, uc00073 }, { 0x00074, uc00074 }, { 0x00075, uc00075 }, { 0x00076, uc00076 }, { 0x00077, uc00077 }, { 0x00078, uc00078 },
// { 0x00079, uc00079 }, { 0x0007A, uc0007A }, { 0x000B5, uc000B5 }, { 0x000DF, uc000DF }, { 0x000E0, uc000E0 }, { 0x000E1, uc000E1 }, { 0x000E2, uc000E2 }, { 0x000E3, uc000E3 },
// { 0x000E4, uc000E4 }, { 0x000E5, uc000E5 }, { 0x000E6, uc000E6 }, { 0x000E7, uc000E7 }, { 0x000E8, uc000E8 }, { 0x000E9, uc000E9 }, { 0x000EA, uc000EA }, { 0x000EB, uc000EB },
// { 0x000EC, uc000EC }, { 0x000ED, uc000ED }, { 0x000EE, uc000EE }, { 0x000EF, uc000EF }, { 0x000F0, uc000F0 }, { 0x000F1, uc000F1 }, { 0x000F2, uc000F2 }, { 0x000F3, uc000F3 },
// { 0x000F4, uc000F4 }, { 0x000F5, uc000F5 }, { 0x000F6, uc000F6 }, { 0x000F8, uc000F8 }, { 0x000F9, uc000F9 }, { 0x000FA, uc000FA }, { 0x000FB, uc000FB }, { 0x000FC, uc000FC },
// { 0x000FD, uc000FD }, { 0x000FE, uc000FE }, { 0x000FF, uc000FF }, { 0x00101, uc00101 }, { 0x00103, uc00103 }, { 0x00105, uc00105 }, { 0x00107, uc00107 }, { 0x00109, uc00109 },
// { 0x0010B, uc0010B }, { 0x0010D, uc0010D }, { 0x0010F, uc0010F }, { 0x00111, uc00111 }, { 0x00113, uc00113 }, { 0x00115, uc00115 }, { 0x00117, uc00117 }, { 0x00119, uc00119 },
// { 0x0011B, uc0011B }, { 0x0011D, uc0011D }, { 0x0011F, uc0011F }, { 0x00121, uc00121 }, { 0x00123, uc00123 }, { 0x00125, uc00125 }, { 0x00127, uc00127 }, { 0x00129, uc00129 },
// { 0x0012B, uc0012B }, { 0x0012D, uc0012D }, { 0x0012F, uc0012F }, { 0x00130, uc00130 }, { 0x00131, uc00131 }, { 0x00133, uc00133 }, { 0x00135, uc00135 }, { 0x00137, uc00137 },
// { 0x0013A, uc0013A }, { 0x0013C, uc0013C }, { 0x0013E, uc0013E }, { 0x00140, uc00140 }, { 0x00142, uc00142 }, { 0x00144, uc00144 }, { 0x00146, uc00146 }, { 0x00148, uc00148 },
// { 0x00149, uc00149 }, { 0x0014B, uc0014B }, { 0x0014D, uc0014D }, { 0x0014F, uc0014F }, { 0x00151, uc00151 }, { 0x00153, uc00153 }, { 0x00155, uc00155 }, { 0x00157, uc00157 },
// { 0x00159, uc00159 }, { 0x0015B, uc0015B }, { 0x0015D, uc0015D }, { 0x0015F, uc0015F }, { 0x00161, uc00161 }, { 0x00163, uc00163 }, { 0x00165, uc00165 }, { 0x00167, uc00167 },
// { 0x00169, uc00169 }, { 0x0016B, uc0016B }, { 0x0016D, uc0016D }, { 0x0016F, uc0016F }, { 0x00171, uc00171 }, { 0x00173, uc00173 }, { 0x00175, uc00175 }, { 0x00177, uc00177 },
// { 0x0017A, uc0017A }, { 0x0017C, uc0017C }, { 0x0017E, uc0017E }, { 0x0017F, uc0017F }, { 0x00180, uc00180 }, { 0x00183, uc00183 }, { 0x00185, uc00185 }, { 0x00188, uc00188 },
// { 0x0018C, uc0018C }, { 0x00192, uc00192 }, { 0x00195, uc00195 }, { 0x00199, uc00199 }, { 0x0019A, uc0019A }, { 0x0019E, uc0019E }, { 0x001A1, uc001A1 }, { 0x001A3, uc001A3 },
// { 0x001A5, uc001A5 }, { 0x001A8, uc001A8 }, { 0x001AD, uc001AD }, { 0x001B0, uc001B0 }, { 0x001B4, uc001B4 }, { 0x001B6, uc001B6 }, { 0x001B9, uc001B9 }, { 0x001BD, uc001BD },
// { 0x001BF, uc001BF }, { 0x001C5, uc001C5 }, { 0x001C6, uc001C6 }, { 0x001C8, uc001C8 }, { 0x001C9, uc001C9 }, { 0x001CB, uc001CB }, { 0x001CC, uc001CC }, { 0x001CE, uc001CE },
// { 0x001D0, uc001D0 }, { 0x001D2, uc001D2 }, { 0x001D4, uc001D4 }, { 0x001D6, uc001D6 }, { 0x001D8, uc001D8 }, { 0x001DA, uc001DA }, { 0x001DC, uc001DC }, { 0x001DD, uc001DD },
// { 0x001DF, uc001DF }, { 0x001E1, uc001E1 }, { 0x001E3, uc001E3 }, { 0x001E5, uc001E5 }, { 0x001E7, uc001E7 }, { 0x001E9, uc001E9 }, { 0x001EB, uc001EB }, { 0x001ED, uc001ED },
// { 0x001EF, uc001EF }, { 0x001F0, uc001F0 }, { 0x001F2, uc001F2 }, { 0x001F3, uc001F3 }, { 0x001F5, uc001F5 }, { 0x001F9, uc001F9 }, { 0x001FB, uc001FB }, { 0x001FD, uc001FD },
// { 0x001FF, uc001FF }, { 0x00201, uc00201 }, { 0x00203, uc00203 }, { 0x00205, uc00205 }, { 0x00207, uc00207 }, { 0x00209, uc00209 }, { 0x0020B, uc0020B }, { 0x0020D, uc0020D },
// { 0x0020F, uc0020F }, { 0x00211, uc00211 }, { 0x00213, uc00213 }, { 0x00215, uc00215 }, { 0x00217, uc00217 }, { 0x00219, uc00219 }, { 0x0021B, uc0021B }, { 0x0021D, uc0021D },
// { 0x0021F, uc0021F }, { 0x00223, uc00223 }, { 0x00225, uc00225 }, { 0x00227, uc00227 }, { 0x00229, uc00229 }, { 0x0022B, uc0022B }, { 0x0022D, uc0022D }, { 0x0022F, uc0022F },
// { 0x00231, uc00231 }, { 0x00233, uc00233 }, { 0x0023C, uc0023C }, { 0x00242, uc00242 }, { 0x00247, uc00247 }, { 0x00249, uc00249 }, { 0x0024B, uc0024B }, { 0x0024D, uc0024D },
// { 0x0024F, uc0024F }, { 0x00253, uc00253 }, { 0x00254, uc00254 }, { 0x00256, uc00256 }, { 0x00257, uc00257 }, { 0x00259, uc00259 }, { 0x0025B, uc0025B }, { 0x00260, uc00260 },
// { 0x00263, uc00263 }, { 0x00268, uc00268 }, { 0x00269, uc00269 }, { 0x0026B, uc0026B }, { 0x0026F, uc0026F }, { 0x00272, uc00272 }, { 0x00275, uc00275 }, { 0x0027D, uc0027D },
// { 0x00280, uc00280 }, { 0x00283, uc00283 }, { 0x00288, uc00288 }, { 0x00289, uc00289 }, { 0x0028A, uc0028A }, { 0x0028B, uc0028B }, { 0x0028C, uc0028C }, { 0x00292, uc00292 },
// { 0x00345, uc00345 }, { 0x0037B, uc0037B }, { 0x0037C, uc0037C }, { 0x0037D, uc0037D }, { 0x00390, uc00390 }, { 0x003AC, uc003AC }, { 0x003AD, uc003AD }, { 0x003AE, uc003AE },
// { 0x003AF, uc003AF }, { 0x003B0, uc003B0 }, { 0x003B1, uc003B1 }, { 0x003B2, uc003B2 }, { 0x003B3, uc003B3 }, { 0x003B4, uc003B4 }, { 0x003B5, uc003B5 }, { 0x003B6, uc003B6 },
// { 0x003B7, uc003B7 }, { 0x003B8, uc003B8 }, { 0x003B9, uc003B9 }, { 0x003BA, uc003BA }, { 0x003BB, uc003BB }, { 0x003BC, uc003BC }, { 0x003BD, uc003BD }, { 0x003BE, uc003BE },
// { 0x003BF, uc003BF }, { 0x003C0, uc003C0 }, { 0x003C1, uc003C1 }, { 0x003C2, uc003C2 }, { 0x003C3, uc003C3 }, { 0x003C4, uc003C4 }, { 0x003C5, uc003C5 }, { 0x003C6, uc003C6 },
// { 0x003C7, uc003C7 }, { 0x003C8, uc003C8 }, { 0x003C9, uc003C9 }, { 0x003CA, uc003CA }, { 0x003CB, uc003CB }, { 0x003CC, uc003CC }, { 0x003CD, uc003CD }, { 0x003CE, uc003CE },
// { 0x003D0, uc003D0 }, { 0x003D1, uc003D1 }, { 0x003D5, uc003D5 }, { 0x003D6, uc003D6 }, { 0x003D9, uc003D9 }, { 0x003DB, uc003DB }, { 0x003DD, uc003DD }, { 0x003DF, uc003DF },
// { 0x003E1, uc003E1 }, { 0x003E3, uc003E3 }, { 0x003E5, uc003E5 }, { 0x003E7, uc003E7 }, { 0x003E9, uc003E9 }, { 0x003EB, uc003EB }, { 0x003ED, uc003ED }, { 0x003EF, uc003EF },
// { 0x003F0, uc003F0 }, { 0x003F1, uc003F1 }, { 0x003F2, uc003F2 }, { 0x003F5, uc003F5 }, { 0x003F8, uc003F8 }, { 0x003FB, uc003FB }, { 0x00430, uc00430 }, { 0x00431, uc00431 },
// { 0x00432, uc00432 }, { 0x00433, uc00433 }, { 0x00434, uc00434 }, { 0x00435, uc00435 }, { 0x00436, uc00436 }, { 0x00437, uc00437 }, { 0x00438, uc00438 }, { 0x00439, uc00439 },
// { 0x0043A, uc0043A }, { 0x0043B, uc0043B }, { 0x0043C, uc0043C }, { 0x0043D, uc0043D }, { 0x0043E, uc0043E }, { 0x0043F, uc0043F }, { 0x00440, uc00440 }, { 0x00441, uc00441 },
// { 0x00442, uc00442 }, { 0x00443, uc00443 }, { 0x00444, uc00444 }, { 0x00445, uc00445 }, { 0x00446, uc00446 }, { 0x00447, uc00447 }, { 0x00448, uc00448 }, { 0x00449, uc00449 },
// { 0x0044A, uc0044A }, { 0x0044B, uc0044B }, { 0x0044C, uc0044C }, { 0x0044D, uc0044D }, { 0x0044E, uc0044E }, { 0x0044F, uc0044F }, { 0x00450, uc00450 }, { 0x00451, uc00451 },
// { 0x00452, uc00452 }, { 0x00453, uc00453 }, { 0x00454, uc00454 }, { 0x00455, uc00455 }, { 0x00456, uc00456 }, { 0x00457, uc00457 }, { 0x00458, uc00458 }, { 0x00459, uc00459 },
// { 0x0045A, uc0045A }, { 0x0045B, uc0045B }, { 0x0045C, uc0045C }, { 0x0045D, uc0045D }, { 0x0045E, uc0045E }, { 0x0045F, uc0045F }, { 0x00461, uc00461 }, { 0x00463, uc00463 },
// { 0x00465, uc00465 }, { 0x00467, uc00467 }, { 0x00469, uc00469 }, { 0x0046B, uc0046B }, { 0x0046D, uc0046D }, { 0x0046F, uc0046F }, { 0x00471, uc00471 }, { 0x00473, uc00473 },
// { 0x00475, uc00475 }, { 0x00477, uc00477 }, { 0x00479, uc00479 }, { 0x0047B, uc0047B }, { 0x0047D, uc0047D }, { 0x0047F, uc0047F }, { 0x00481, uc00481 }, { 0x0048B, uc0048B },
// { 0x0048D, uc0048D }, { 0x0048F, uc0048F }, { 0x00491, uc00491 }, { 0x00493, uc00493 }, { 0x00495, uc00495 }, { 0x00497, uc00497 }, { 0x00499, uc00499 }, { 0x0049B, uc0049B },
// { 0x0049D, uc0049D }, { 0x0049F, uc0049F }, { 0x004A1, uc004A1 }, { 0x004A3, uc004A3 }, { 0x004A5, uc004A5 }, { 0x004A7, uc004A7 }, { 0x004A9, uc004A9 }, { 0x004AB, uc004AB },
// { 0x004AD, uc004AD }, { 0x004AF, uc004AF }, { 0x004B1, uc004B1 }, { 0x004B3, uc004B3 }, { 0x004B5, uc004B5 }, { 0x004B7, uc004B7 }, { 0x004B9, uc004B9 }, { 0x004BB, uc004BB },
// { 0x004BD, uc004BD }, { 0x004BF, uc004BF }, { 0x004C2, uc004C2 }, { 0x004C4, uc004C4 }, { 0x004C6, uc004C6 }, { 0x004C8, uc004C8 }, { 0x004CA, uc004CA }, { 0x004CC, uc004CC },
// { 0x004CE, uc004CE }, { 0x004CF, uc004CF }, { 0x004D1, uc004D1 }, { 0x004D3, uc004D3 }, { 0x004D5, uc004D5 }, { 0x004D7, uc004D7 }, { 0x004D9, uc004D9 }, { 0x004DB, uc004DB },
// { 0x004DD, uc004DD }, { 0x004DF, uc004DF }, { 0x004E1, uc004E1 }, { 0x004E3, uc004E3 }, { 0x004E5, uc004E5 }, { 0x004E7, uc004E7 }, { 0x004E9, uc004E9 }, { 0x004EB, uc004EB },
// { 0x004ED, uc004ED }, { 0x004EF, uc004EF }, { 0x004F1, uc004F1 }, { 0x004F3, uc004F3 }, { 0x004F5, uc004F5 }, { 0x004F7, uc004F7 }, { 0x004F9, uc004F9 }, { 0x004FB, uc004FB },
// { 0x004FD, uc004FD }, { 0x004FF, uc004FF }, { 0x00501, uc00501 }, { 0x00503, uc00503 }, { 0x00505, uc00505 }, { 0x00507, uc00507 }, { 0x00509, uc00509 }, { 0x0050B, uc0050B },
// { 0x0050D, uc0050D }, { 0x0050F, uc0050F }, { 0x00511, uc00511 }, { 0x00513, uc00513 }, { 0x00561, uc00561 }, { 0x00562, uc00562 }, { 0x00563, uc00563 }, { 0x00564, uc00564 },
// { 0x00565, uc00565 }, { 0x00566, uc00566 }, { 0x00567, uc00567 }, { 0x00568, uc00568 }, { 0x00569, uc00569 }, { 0x0056A, uc0056A }, { 0x0056B, uc0056B }, { 0x0056C, uc0056C },
// { 0x0056D, uc0056D }, { 0x0056E, uc0056E }, { 0x0056F, uc0056F }, { 0x00570, uc00570 }, { 0x00571, uc00571 }, { 0x00572, uc00572 }, { 0x00573, uc00573 }, { 0x00574, uc00574 },
// { 0x00575, uc00575 }, { 0x00576, uc00576 }, { 0x00577, uc00577 }, { 0x00578, uc00578 }, { 0x00579, uc00579 }, { 0x0057A, uc0057A }, { 0x0057B, uc0057B }, { 0x0057C, uc0057C },
// { 0x0057D, uc0057D }, { 0x0057E, uc0057E }, { 0x0057F, uc0057F }, { 0x00580, uc00580 }, { 0x00581, uc00581 }, { 0x00582, uc00582 }, { 0x00583, uc00583 }, { 0x00584, uc00584 },
// { 0x00585, uc00585 }, { 0x00586, uc00586 }, { 0x00587, uc00587 },
                                                                  { 0x01D7D, uc01D7D }, { 0x01E01, uc01E01 }, { 0x01E03, uc01E03 }, { 0x01E05, uc01E05 }, { 0x01E07, uc01E07 },
{ 0x01E09, uc01E09 }, { 0x01E0B, uc01E0B }, { 0x01E0D, uc01E0D }, { 0x01E0F, uc01E0F }, { 0x01E11, uc01E11 }, { 0x01E13, uc01E13 }, { 0x01E15, uc01E15 }, { 0x01E17, uc01E17 },
{ 0x01E19, uc01E19 }, { 0x01E1B, uc01E1B }, { 0x01E1D, uc01E1D }, { 0x01E1F, uc01E1F }, { 0x01E21, uc01E21 }, { 0x01E23, uc01E23 }, { 0x01E25, uc01E25 }, { 0x01E27, uc01E27 },
{ 0x01E29, uc01E29 }, { 0x01E2B, uc01E2B }, { 0x01E2D, uc01E2D }, { 0x01E2F, uc01E2F }, { 0x01E31, uc01E31 }, { 0x01E33, uc01E33 }, { 0x01E35, uc01E35 }, { 0x01E37, uc01E37 },
{ 0x01E39, uc01E39 }, { 0x01E3B, uc01E3B }, { 0x01E3D, uc01E3D }, { 0x01E3F, uc01E3F }, { 0x01E41, uc01E41 }, { 0x01E43, uc01E43 }, { 0x01E45, uc01E45 }, { 0x01E47, uc01E47 },
{ 0x01E49, uc01E49 }, { 0x01E4B, uc01E4B }, { 0x01E4D, uc01E4D }, { 0x01E4F, uc01E4F }, { 0x01E51, uc01E51 }, { 0x01E53, uc01E53 }, { 0x01E55, uc01E55 }, { 0x01E57, uc01E57 },
{ 0x01E59, uc01E59 }, { 0x01E5B, uc01E5B }, { 0x01E5D, uc01E5D }, { 0x01E5F, uc01E5F }, { 0x01E61, uc01E61 }, { 0x01E63, uc01E63 }, { 0x01E65, uc01E65 }, { 0x01E67, uc01E67 },
{ 0x01E69, uc01E69 }, { 0x01E6B, uc01E6B }, { 0x01E6D, uc01E6D }, { 0x01E6F, uc01E6F }, { 0x01E71, uc01E71 }, { 0x01E73, uc01E73 }, { 0x01E75, uc01E75 }, { 0x01E77, uc01E77 },
{ 0x01E79, uc01E79 }, { 0x01E7B, uc01E7B }, { 0x01E7D, uc01E7D }, { 0x01E7F, uc01E7F }, { 0x01E81, uc01E81 }, { 0x01E83, uc01E83 }, { 0x01E85, uc01E85 }, { 0x01E87, uc01E87 },
{ 0x01E89, uc01E89 }, { 0x01E8B, uc01E8B }, { 0x01E8D, uc01E8D }, { 0x01E8F, uc01E8F }, { 0x01E91, uc01E91 }, { 0x01E93, uc01E93 }, { 0x01E95, uc01E95 }, { 0x01E96, uc01E96 },
{ 0x01E97, uc01E97 }, { 0x01E98, uc01E98 }, { 0x01E99, uc01E99 }, { 0x01E9A, uc01E9A }, { 0x01E9B, uc01E9B }, { 0x01EA1, uc01EA1 }, { 0x01EA3, uc01EA3 }, { 0x01EA5, uc01EA5 },
{ 0x01EA7, uc01EA7 }, { 0x01EA9, uc01EA9 }, { 0x01EAB, uc01EAB }, { 0x01EAD, uc01EAD }, { 0x01EAF, uc01EAF }, { 0x01EB1, uc01EB1 }, { 0x01EB3, uc01EB3 }, { 0x01EB5, uc01EB5 },
{ 0x01EB7, uc01EB7 }, { 0x01EB9, uc01EB9 }, { 0x01EBB, uc01EBB }, { 0x01EBD, uc01EBD }, { 0x01EBF, uc01EBF }, { 0x01EC1, uc01EC1 }, { 0x01EC3, uc01EC3 }, { 0x01EC5, uc01EC5 },
{ 0x01EC7, uc01EC7 }, { 0x01EC9, uc01EC9 }, { 0x01ECB, uc01ECB }, { 0x01ECD, uc01ECD }, { 0x01ECF, uc01ECF }, { 0x01ED1, uc01ED1 }, { 0x01ED3, uc01ED3 }, { 0x01ED5, uc01ED5 },
{ 0x01ED7, uc01ED7 }, { 0x01ED9, uc01ED9 }, { 0x01EDB, uc01EDB }, { 0x01EDD, uc01EDD }, { 0x01EDF, uc01EDF }, { 0x01EE1, uc01EE1 }, { 0x01EE3, uc01EE3 }, { 0x01EE5, uc01EE5 },
{ 0x01EE7, uc01EE7 }, { 0x01EE9, uc01EE9 }, { 0x01EEB, uc01EEB }, { 0x01EED, uc01EED }, { 0x01EEF, uc01EEF }, { 0x01EF1, uc01EF1 }, { 0x01EF3, uc01EF3 }, { 0x01EF5, uc01EF5 },
{ 0x01EF7, uc01EF7 }, { 0x01EF9, uc01EF9 }, { 0x01F00, uc01F00 }, { 0x01F01, uc01F01 }, { 0x01F02, uc01F02 }, { 0x01F03, uc01F03 }, { 0x01F04, uc01F04 }, { 0x01F05, uc01F05 },
{ 0x01F06, uc01F06 }, { 0x01F07, uc01F07 }, { 0x01F10, uc01F10 }, { 0x01F11, uc01F11 }, { 0x01F12, uc01F12 }, { 0x01F13, uc01F13 }, { 0x01F14, uc01F14 }, { 0x01F15, uc01F15 },
{ 0x01F20, uc01F20 }, { 0x01F21, uc01F21 }, { 0x01F22, uc01F22 }, { 0x01F23, uc01F23 }, { 0x01F24, uc01F24 }, { 0x01F25, uc01F25 }, { 0x01F26, uc01F26 }, { 0x01F27, uc01F27 },
{ 0x01F30, uc01F30 }, { 0x01F31, uc01F31 }, { 0x01F32, uc01F32 }, { 0x01F33, uc01F33 }, { 0x01F34, uc01F34 }, { 0x01F35, uc01F35 }, { 0x01F36, uc01F36 }, { 0x01F37, uc01F37 },
{ 0x01F40, uc01F40 }, { 0x01F41, uc01F41 }, { 0x01F42, uc01F42 }, { 0x01F43, uc01F43 }, { 0x01F44, uc01F44 }, { 0x01F45, uc01F45 }, { 0x01F50, uc01F50 }, { 0x01F51, uc01F51 },
{ 0x01F52, uc01F52 }, { 0x01F53, uc01F53 }, { 0x01F54, uc01F54 }, { 0x01F55, uc01F55 }, { 0x01F56, uc01F56 }, { 0x01F57, uc01F57 }, { 0x01F60, uc01F60 }, { 0x01F61, uc01F61 },
{ 0x01F62, uc01F62 }, { 0x01F63, uc01F63 }, { 0x01F64, uc01F64 }, { 0x01F65, uc01F65 }, { 0x01F66, uc01F66 }, { 0x01F67, uc01F67 }, { 0x01F70, uc01F70 }, { 0x01F71, uc01F71 },
{ 0x01F72, uc01F72 }, { 0x01F73, uc01F73 }, { 0x01F74, uc01F74 }, { 0x01F75, uc01F75 }, { 0x01F76, uc01F76 }, { 0x01F77, uc01F77 }, { 0x01F78, uc01F78 }, { 0x01F79, uc01F79 },
{ 0x01F7A, uc01F7A }, { 0x01F7B, uc01F7B }, { 0x01F7C, uc01F7C }, { 0x01F7D, uc01F7D }, { 0x01F80, uc01F80 }, { 0x01F81, uc01F81 }, { 0x01F82, uc01F82 }, { 0x01F83, uc01F83 },
{ 0x01F84, uc01F84 }, { 0x01F85, uc01F85 }, { 0x01F86, uc01F86 }, { 0x01F87, uc01F87 }, { 0x01F88, uc01F88 }, { 0x01F89, uc01F89 }, { 0x01F8A, uc01F8A }, { 0x01F8B, uc01F8B },
{ 0x01F8C, uc01F8C }, { 0x01F8D, uc01F8D }, { 0x01F8E, uc01F8E }, { 0x01F8F, uc01F8F }, { 0x01F90, uc01F90 }, { 0x01F91, uc01F91 }, { 0x01F92, uc01F92 }, { 0x01F93, uc01F93 },
{ 0x01F94, uc01F94 }, { 0x01F95, uc01F95 }, { 0x01F96, uc01F96 }, { 0x01F97, uc01F97 }, { 0x01F98, uc01F98 }, { 0x01F99, uc01F99 }, { 0x01F9A, uc01F9A }, { 0x01F9B, uc01F9B },
{ 0x01F9C, uc01F9C }, { 0x01F9D, uc01F9D }, { 0x01F9E, uc01F9E }, { 0x01F9F, uc01F9F }, { 0x01FA0, uc01FA0 }, { 0x01FA1, uc01FA1 }, { 0x01FA2, uc01FA2 }, { 0x01FA3, uc01FA3 },
{ 0x01FA4, uc01FA4 }, { 0x01FA5, uc01FA5 }, { 0x01FA6, uc01FA6 }, { 0x01FA7, uc01FA7 }, { 0x01FA8, uc01FA8 }, { 0x01FA9, uc01FA9 }, { 0x01FAA, uc01FAA }, { 0x01FAB, uc01FAB },
{ 0x01FAC, uc01FAC }, { 0x01FAD, uc01FAD }, { 0x01FAE, uc01FAE }, { 0x01FAF, uc01FAF }, { 0x01FB0, uc01FB0 }, { 0x01FB1, uc01FB1 }, { 0x01FB2, uc01FB2 }, { 0x01FB3, uc01FB3 },
{ 0x01FB4, uc01FB4 }, { 0x01FB6, uc01FB6 }, { 0x01FB7, uc01FB7 }, { 0x01FBC, uc01FBC }, { 0x01FBE, uc01FBE }, { 0x01FC2, uc01FC2 }, { 0x01FC3, uc01FC3 }, { 0x01FC4, uc01FC4 },
{ 0x01FC6, uc01FC6 }, { 0x01FC7, uc01FC7 }, { 0x01FCC, uc01FCC }, { 0x01FD0, uc01FD0 }, { 0x01FD1, uc01FD1 }, { 0x01FD2, uc01FD2 }, { 0x01FD3, uc01FD3 }, { 0x01FD6, uc01FD6 },
{ 0x01FD7, uc01FD7 }, { 0x01FE0, uc01FE0 }, { 0x01FE1, uc01FE1 }, { 0x01FE2, uc01FE2 }, { 0x01FE3, uc01FE3 }, { 0x01FE4, uc01FE4 }, { 0x01FE5, uc01FE5 }, { 0x01FE6, uc01FE6 },
{ 0x01FE7, uc01FE7 }, { 0x01FF2, uc01FF2 }, { 0x01FF3, uc01FF3 }, { 0x01FF4, uc01FF4 }, { 0x01FF6, uc01FF6 }, { 0x01FF7, uc01FF7 }, { 0x01FFC, uc01FFC }, { 0x0214E, uc0214E },
{ 0x02170, uc02170 }, { 0x02171, uc02171 }, { 0x02172, uc02172 }, { 0x02173, uc02173 }, { 0x02174, uc02174 }, { 0x02175, uc02175 }, { 0x02176, uc02176 }, { 0x02177, uc02177 },
{ 0x02178, uc02178 }, { 0x02179, uc02179 }, { 0x0217A, uc0217A }, { 0x0217B, uc0217B }, { 0x0217C, uc0217C }, { 0x0217D, uc0217D }, { 0x0217E, uc0217E }, { 0x0217F, uc0217F },
{ 0x02184, uc02184 }, { 0x024D0, uc024D0 }, { 0x024D1, uc024D1 }, { 0x024D2, uc024D2 }, { 0x024D3, uc024D3 }, { 0x024D4, uc024D4 }, { 0x024D5, uc024D5 }, { 0x024D6, uc024D6 },
{ 0x024D7, uc024D7 }, { 0x024D8, uc024D8 }, { 0x024D9, uc024D9 }, { 0x024DA, uc024DA }, { 0x024DB, uc024DB }, { 0x024DC, uc024DC }, { 0x024DD, uc024DD }, { 0x024DE, uc024DE },
{ 0x024DF, uc024DF }, { 0x024E0, uc024E0 }, { 0x024E1, uc024E1 }, { 0x024E2, uc024E2 }, { 0x024E3, uc024E3 }, { 0x024E4, uc024E4 }, { 0x024E5, uc024E5 }, { 0x024E6, uc024E6 },
{ 0x024E7, uc024E7 }, { 0x024E8, uc024E8 }, { 0x024E9, uc024E9 }, { 0x02C30, uc02C30 }, { 0x02C31, uc02C31 }, { 0x02C32, uc02C32 }, { 0x02C33, uc02C33 }, { 0x02C34, uc02C34 },
{ 0x02C35, uc02C35 }, { 0x02C36, uc02C36 }, { 0x02C37, uc02C37 }, { 0x02C38, uc02C38 }, { 0x02C39, uc02C39 }, { 0x02C3A, uc02C3A }, { 0x02C3B, uc02C3B }, { 0x02C3C, uc02C3C },
{ 0x02C3D, uc02C3D }, { 0x02C3E, uc02C3E }, { 0x02C3F, uc02C3F }, { 0x02C40, uc02C40 }, { 0x02C41, uc02C41 }, { 0x02C42, uc02C42 }, { 0x02C43, uc02C43 }, { 0x02C44, uc02C44 },
{ 0x02C45, uc02C45 }, { 0x02C46, uc02C46 }, { 0x02C47, uc02C47 }, { 0x02C48, uc02C48 }, { 0x02C49, uc02C49 }, { 0x02C4A, uc02C4A }, { 0x02C4B, uc02C4B }, { 0x02C4C, uc02C4C },
{ 0x02C4D, uc02C4D }, { 0x02C4E, uc02C4E }, { 0x02C4F, uc02C4F }, { 0x02C50, uc02C50 }, { 0x02C51, uc02C51 }, { 0x02C52, uc02C52 }, { 0x02C53, uc02C53 }, { 0x02C54, uc02C54 },
{ 0x02C55, uc02C55 }, { 0x02C56, uc02C56 }, { 0x02C57, uc02C57 }, { 0x02C58, uc02C58 }, { 0x02C59, uc02C59 }, { 0x02C5A, uc02C5A }, { 0x02C5B, uc02C5B }, { 0x02C5C, uc02C5C },
{ 0x02C5D, uc02C5D }, { 0x02C5E, uc02C5E }, { 0x02C61, uc02C61 }, { 0x02C65, uc02C65 }, { 0x02C66, uc02C66 }, { 0x02C68, uc02C68 }, { 0x02C6A, uc02C6A }, { 0x02C6C, uc02C6C },
{ 0x02C76, uc02C76 }, { 0x02C81, uc02C81 }, { 0x02C83, uc02C83 }, { 0x02C85, uc02C85 }, { 0x02C87, uc02C87 }, { 0x02C89, uc02C89 }, { 0x02C8B, uc02C8B }, { 0x02C8D, uc02C8D },
{ 0x02C8F, uc02C8F }, { 0x02C91, uc02C91 }, { 0x02C93, uc02C93 }, { 0x02C95, uc02C95 }, { 0x02C97, uc02C97 }, { 0x02C99, uc02C99 }, { 0x02C9B, uc02C9B }, { 0x02C9D, uc02C9D },
{ 0x02C9F, uc02C9F }, { 0x02CA1, uc02CA1 }, { 0x02CA3, uc02CA3 }, { 0x02CA5, uc02CA5 }, { 0x02CA7, uc02CA7 }, { 0x02CA9, uc02CA9 }, { 0x02CAB, uc02CAB }, { 0x02CAD, uc02CAD },
{ 0x02CAF, uc02CAF }, { 0x02CB1, uc02CB1 }, { 0x02CB3, uc02CB3 }, { 0x02CB5, uc02CB5 }, { 0x02CB7, uc02CB7 }, { 0x02CB9, uc02CB9 }, { 0x02CBB, uc02CBB }, { 0x02CBD, uc02CBD },
{ 0x02CBF, uc02CBF }, { 0x02CC1, uc02CC1 }, { 0x02CC3, uc02CC3 }, { 0x02CC5, uc02CC5 }, { 0x02CC7, uc02CC7 }, { 0x02CC9, uc02CC9 }, { 0x02CCB, uc02CCB }, { 0x02CCD, uc02CCD },
{ 0x02CCF, uc02CCF }, { 0x02CD1, uc02CD1 }, { 0x02CD3, uc02CD3 }, { 0x02CD5, uc02CD5 }, { 0x02CD7, uc02CD7 }, { 0x02CD9, uc02CD9 }, { 0x02CDB, uc02CDB }, { 0x02CDD, uc02CDD },
{ 0x02CDF, uc02CDF }, { 0x02CE1, uc02CE1 }, { 0x02CE3, uc02CE3 }, { 0x02D00, uc02D00 }, { 0x02D01, uc02D01 }, { 0x02D02, uc02D02 }, { 0x02D03, uc02D03 }, { 0x02D04, uc02D04 },
{ 0x02D05, uc02D05 }, { 0x02D06, uc02D06 }, { 0x02D07, uc02D07 }, { 0x02D08, uc02D08 }, { 0x02D09, uc02D09 }, { 0x02D0A, uc02D0A }, { 0x02D0B, uc02D0B }, { 0x02D0C, uc02D0C },
{ 0x02D0D, uc02D0D }, { 0x02D0E, uc02D0E }, { 0x02D0F, uc02D0F }, { 0x02D10, uc02D10 }, { 0x02D11, uc02D11 }, { 0x02D12, uc02D12 }, { 0x02D13, uc02D13 }, { 0x02D14, uc02D14 },
{ 0x02D15, uc02D15 }, { 0x02D16, uc02D16 }, { 0x02D17, uc02D17 }, { 0x02D18, uc02D18 }, { 0x02D19, uc02D19 }, { 0x02D1A, uc02D1A }, { 0x02D1B, uc02D1B }, { 0x02D1C, uc02D1C },
{ 0x02D1D, uc02D1D }, { 0x02D1E, uc02D1E }, { 0x02D1F, uc02D1F }, { 0x02D20, uc02D20 }, { 0x02D21, uc02D21 }, { 0x02D22, uc02D22 }, { 0x02D23, uc02D23 }, { 0x02D24, uc02D24 },
{ 0x02D25, uc02D25 }, { 0x0FB00, uc0FB00 }, { 0x0FB01, uc0FB01 }, { 0x0FB02, uc0FB02 }, { 0x0FB03, uc0FB03 }, { 0x0FB04, uc0FB04 }, { 0x0FB05, uc0FB05 }, { 0x0FB06, uc0FB06 },
{ 0x0FB13, uc0FB13 }, { 0x0FB14, uc0FB14 }, { 0x0FB15, uc0FB15 }, { 0x0FB16, uc0FB16 }, { 0x0FB17, uc0FB17 }, { 0x0FF41, uc0FF41 }, { 0x0FF42, uc0FF42 }, { 0x0FF43, uc0FF43 },
{ 0x0FF44, uc0FF44 }, { 0x0FF45, uc0FF45 }, { 0x0FF46, uc0FF46 }, { 0x0FF47, uc0FF47 }, { 0x0FF48, uc0FF48 }, { 0x0FF49, uc0FF49 }, { 0x0FF4A, uc0FF4A }, { 0x0FF4B, uc0FF4B },
{ 0x0FF4C, uc0FF4C }, { 0x0FF4D, uc0FF4D }, { 0x0FF4E, uc0FF4E }, { 0x0FF4F, uc0FF4F }, { 0x0FF50, uc0FF50 }, { 0x0FF51, uc0FF51 }, { 0x0FF52, uc0FF52 }, { 0x0FF53, uc0FF53 },
{ 0x0FF54, uc0FF54 }, { 0x0FF55, uc0FF55 }, { 0x0FF56, uc0FF56 }, { 0x0FF57, uc0FF57 }, { 0x0FF58, uc0FF58 }, { 0x0FF59, uc0FF59 }, { 0x0FF5A, uc0FF5A }, { 0x10428, uc10428 },
{ 0x10429, uc10429 }, { 0x1042A, uc1042A }, { 0x1042B, uc1042B }, { 0x1042C, uc1042C }, { 0x1042D, uc1042D }, { 0x1042E, uc1042E }, { 0x1042F, uc1042F }, { 0x10430, uc10430 },
{ 0x10431, uc10431 }, { 0x10432, uc10432 }, { 0x10433, uc10433 }, { 0x10434, uc10434 }, { 0x10435, uc10435 }, { 0x10436, uc10436 }, { 0x10437, uc10437 }, { 0x10438, uc10438 },
{ 0x10439, uc10439 }, { 0x1043A, uc1043A }, { 0x1043B, uc1043B }, { 0x1043C, uc1043C }, { 0x1043D, uc1043D }, { 0x1043E, uc1043E }, { 0x1043F, uc1043F }, { 0x10440, uc10440 },
{ 0x10441, uc10441 }, { 0x10442, uc10442 }, { 0x10443, uc10443 }, { 0x10444, uc10444 }, { 0x10445, uc10445 }, { 0x10446, uc10446 }, { 0x10447, uc10447 }, { 0x10448, uc10448 },
{ 0x10449, uc10449 }, { 0x1044A, uc1044A }, { 0x1044B, uc1044B }, { 0x1044C, uc1044C }, { 0x1044D, uc1044D }, { 0x1044E, uc1044E }, { 0x1044F, uc1044F }, 
};

unsigned int *UpperCaseTransform::getUpperCase(unsigned int ch)
{
  if(ch < upperCaseDirectTableSize) {
    return upperCaseDirectTable[ch];
  }

  // Binary search in the upperCaseTable table
  unsigned int min = 0;
  unsigned int max = upperCaseTableSize;
  unsigned int middle;
  int cmp;

  while(min < max) {
    middle = (max + min) >> 1;

    cmp = upperCaseTable[middle].ch - ch;
    if(cmp > 0) max = middle;
    else if(cmp < 0) {
      min = middle + 1;
    }
    else {
      return upperCaseTable[middle].dest;
    }
  }

  return 0;
}
