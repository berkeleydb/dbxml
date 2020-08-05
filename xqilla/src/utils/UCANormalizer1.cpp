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
 * $Id: UCANormalizer1.cpp 475 2008-01-08 18:47:44Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/UCANormalizer.hpp>

static unsigned int de000A0[] = { 0x00020, 0x00000 };
static unsigned int de000A8[] = { 0x00020, 0x00308, 0x00000 };
static unsigned int de000AA[] = { 0x00061, 0x00000 };
static unsigned int de000AF[] = { 0x00020, 0x00304, 0x00000 };
static unsigned int de000B2[] = { 0x00032, 0x00000 };
static unsigned int de000B3[] = { 0x00033, 0x00000 };
static unsigned int de000B4[] = { 0x00020, 0x00301, 0x00000 };
static unsigned int de000B5[] = { 0x003BC, 0x00000 };
static unsigned int de000B8[] = { 0x00020, 0x00327, 0x00000 };
static unsigned int de000B9[] = { 0x00031, 0x00000 };
static unsigned int de000BA[] = { 0x0006F, 0x00000 };
static unsigned int de000BC[] = { 0x00031, 0x02044, 0x00034, 0x00000 };
static unsigned int de000BD[] = { 0x00031, 0x02044, 0x00032, 0x00000 };
static unsigned int de000BE[] = { 0x00033, 0x02044, 0x00034, 0x00000 };
static unsigned int de000C0[] = { 0x00041, 0x00300, 0x00000 };
static unsigned int de000C1[] = { 0x00041, 0x00301, 0x00000 };
static unsigned int de000C2[] = { 0x00041, 0x00302, 0x00000 };
static unsigned int de000C3[] = { 0x00041, 0x00303, 0x00000 };
static unsigned int de000C4[] = { 0x00041, 0x00308, 0x00000 };
static unsigned int de000C5[] = { 0x00041, 0x0030A, 0x00000 };
static unsigned int de000C7[] = { 0x00043, 0x00327, 0x00000 };
static unsigned int de000C8[] = { 0x00045, 0x00300, 0x00000 };
static unsigned int de000C9[] = { 0x00045, 0x00301, 0x00000 };
static unsigned int de000CA[] = { 0x00045, 0x00302, 0x00000 };
static unsigned int de000CB[] = { 0x00045, 0x00308, 0x00000 };
static unsigned int de000CC[] = { 0x00049, 0x00300, 0x00000 };
static unsigned int de000CD[] = { 0x00049, 0x00301, 0x00000 };
static unsigned int de000CE[] = { 0x00049, 0x00302, 0x00000 };
static unsigned int de000CF[] = { 0x00049, 0x00308, 0x00000 };
static unsigned int de000D1[] = { 0x0004E, 0x00303, 0x00000 };
static unsigned int de000D2[] = { 0x0004F, 0x00300, 0x00000 };
static unsigned int de000D3[] = { 0x0004F, 0x00301, 0x00000 };
static unsigned int de000D4[] = { 0x0004F, 0x00302, 0x00000 };
static unsigned int de000D5[] = { 0x0004F, 0x00303, 0x00000 };
static unsigned int de000D6[] = { 0x0004F, 0x00308, 0x00000 };
static unsigned int de000D9[] = { 0x00055, 0x00300, 0x00000 };
static unsigned int de000DA[] = { 0x00055, 0x00301, 0x00000 };
static unsigned int de000DB[] = { 0x00055, 0x00302, 0x00000 };
static unsigned int de000DC[] = { 0x00055, 0x00308, 0x00000 };
static unsigned int de000DD[] = { 0x00059, 0x00301, 0x00000 };
static unsigned int de000E0[] = { 0x00061, 0x00300, 0x00000 };
static unsigned int de000E1[] = { 0x00061, 0x00301, 0x00000 };
static unsigned int de000E2[] = { 0x00061, 0x00302, 0x00000 };
static unsigned int de000E3[] = { 0x00061, 0x00303, 0x00000 };
static unsigned int de000E4[] = { 0x00061, 0x00308, 0x00000 };
static unsigned int de000E5[] = { 0x00061, 0x0030A, 0x00000 };
static unsigned int de000E7[] = { 0x00063, 0x00327, 0x00000 };
static unsigned int de000E8[] = { 0x00065, 0x00300, 0x00000 };
static unsigned int de000E9[] = { 0x00065, 0x00301, 0x00000 };
static unsigned int de000EA[] = { 0x00065, 0x00302, 0x00000 };
static unsigned int de000EB[] = { 0x00065, 0x00308, 0x00000 };
static unsigned int de000EC[] = { 0x00069, 0x00300, 0x00000 };
static unsigned int de000ED[] = { 0x00069, 0x00301, 0x00000 };
static unsigned int de000EE[] = { 0x00069, 0x00302, 0x00000 };
static unsigned int de000EF[] = { 0x00069, 0x00308, 0x00000 };
static unsigned int de000F1[] = { 0x0006E, 0x00303, 0x00000 };
static unsigned int de000F2[] = { 0x0006F, 0x00300, 0x00000 };
static unsigned int de000F3[] = { 0x0006F, 0x00301, 0x00000 };
static unsigned int de000F4[] = { 0x0006F, 0x00302, 0x00000 };
static unsigned int de000F5[] = { 0x0006F, 0x00303, 0x00000 };
static unsigned int de000F6[] = { 0x0006F, 0x00308, 0x00000 };
static unsigned int de000F9[] = { 0x00075, 0x00300, 0x00000 };
static unsigned int de000FA[] = { 0x00075, 0x00301, 0x00000 };
static unsigned int de000FB[] = { 0x00075, 0x00302, 0x00000 };
static unsigned int de000FC[] = { 0x00075, 0x00308, 0x00000 };
static unsigned int de000FD[] = { 0x00079, 0x00301, 0x00000 };
static unsigned int de000FF[] = { 0x00079, 0x00308, 0x00000 };
static unsigned int de00100[] = { 0x00041, 0x00304, 0x00000 };
static unsigned int de00101[] = { 0x00061, 0x00304, 0x00000 };
static unsigned int de00102[] = { 0x00041, 0x00306, 0x00000 };
static unsigned int de00103[] = { 0x00061, 0x00306, 0x00000 };
static unsigned int de00104[] = { 0x00041, 0x00328, 0x00000 };
static unsigned int de00105[] = { 0x00061, 0x00328, 0x00000 };
static unsigned int de00106[] = { 0x00043, 0x00301, 0x00000 };
static unsigned int de00107[] = { 0x00063, 0x00301, 0x00000 };
static unsigned int de00108[] = { 0x00043, 0x00302, 0x00000 };
static unsigned int de00109[] = { 0x00063, 0x00302, 0x00000 };
static unsigned int de0010A[] = { 0x00043, 0x00307, 0x00000 };
static unsigned int de0010B[] = { 0x00063, 0x00307, 0x00000 };
static unsigned int de0010C[] = { 0x00043, 0x0030C, 0x00000 };
static unsigned int de0010D[] = { 0x00063, 0x0030C, 0x00000 };
static unsigned int de0010E[] = { 0x00044, 0x0030C, 0x00000 };
static unsigned int de0010F[] = { 0x00064, 0x0030C, 0x00000 };
static unsigned int de00112[] = { 0x00045, 0x00304, 0x00000 };
static unsigned int de00113[] = { 0x00065, 0x00304, 0x00000 };
static unsigned int de00114[] = { 0x00045, 0x00306, 0x00000 };
static unsigned int de00115[] = { 0x00065, 0x00306, 0x00000 };
static unsigned int de00116[] = { 0x00045, 0x00307, 0x00000 };
static unsigned int de00117[] = { 0x00065, 0x00307, 0x00000 };
static unsigned int de00118[] = { 0x00045, 0x00328, 0x00000 };
static unsigned int de00119[] = { 0x00065, 0x00328, 0x00000 };
static unsigned int de0011A[] = { 0x00045, 0x0030C, 0x00000 };
static unsigned int de0011B[] = { 0x00065, 0x0030C, 0x00000 };
static unsigned int de0011C[] = { 0x00047, 0x00302, 0x00000 };
static unsigned int de0011D[] = { 0x00067, 0x00302, 0x00000 };
static unsigned int de0011E[] = { 0x00047, 0x00306, 0x00000 };
static unsigned int de0011F[] = { 0x00067, 0x00306, 0x00000 };
static unsigned int de00120[] = { 0x00047, 0x00307, 0x00000 };
static unsigned int de00121[] = { 0x00067, 0x00307, 0x00000 };
static unsigned int de00122[] = { 0x00047, 0x00327, 0x00000 };
static unsigned int de00123[] = { 0x00067, 0x00327, 0x00000 };
static unsigned int de00124[] = { 0x00048, 0x00302, 0x00000 };
static unsigned int de00125[] = { 0x00068, 0x00302, 0x00000 };
static unsigned int de00128[] = { 0x00049, 0x00303, 0x00000 };
static unsigned int de00129[] = { 0x00069, 0x00303, 0x00000 };
static unsigned int de0012A[] = { 0x00049, 0x00304, 0x00000 };
static unsigned int de0012B[] = { 0x00069, 0x00304, 0x00000 };
static unsigned int de0012C[] = { 0x00049, 0x00306, 0x00000 };
static unsigned int de0012D[] = { 0x00069, 0x00306, 0x00000 };
static unsigned int de0012E[] = { 0x00049, 0x00328, 0x00000 };
static unsigned int de0012F[] = { 0x00069, 0x00328, 0x00000 };
static unsigned int de00130[] = { 0x00049, 0x00307, 0x00000 };
static unsigned int de00132[] = { 0x00049, 0x0004A, 0x00000 };
static unsigned int de00133[] = { 0x00069, 0x0006A, 0x00000 };
static unsigned int de00134[] = { 0x0004A, 0x00302, 0x00000 };
static unsigned int de00135[] = { 0x0006A, 0x00302, 0x00000 };
static unsigned int de00136[] = { 0x0004B, 0x00327, 0x00000 };
static unsigned int de00137[] = { 0x0006B, 0x00327, 0x00000 };
static unsigned int de00139[] = { 0x0004C, 0x00301, 0x00000 };
static unsigned int de0013A[] = { 0x0006C, 0x00301, 0x00000 };
static unsigned int de0013B[] = { 0x0004C, 0x00327, 0x00000 };
static unsigned int de0013C[] = { 0x0006C, 0x00327, 0x00000 };
static unsigned int de0013D[] = { 0x0004C, 0x0030C, 0x00000 };
static unsigned int de0013E[] = { 0x0006C, 0x0030C, 0x00000 };
static unsigned int de0013F[] = { 0x0004C, 0x000B7, 0x00000 };
static unsigned int de00140[] = { 0x0006C, 0x000B7, 0x00000 };
static unsigned int de00143[] = { 0x0004E, 0x00301, 0x00000 };
static unsigned int de00144[] = { 0x0006E, 0x00301, 0x00000 };
static unsigned int de00145[] = { 0x0004E, 0x00327, 0x00000 };
static unsigned int de00146[] = { 0x0006E, 0x00327, 0x00000 };
static unsigned int de00147[] = { 0x0004E, 0x0030C, 0x00000 };
static unsigned int de00148[] = { 0x0006E, 0x0030C, 0x00000 };
static unsigned int de00149[] = { 0x002BC, 0x0006E, 0x00000 };
static unsigned int de0014C[] = { 0x0004F, 0x00304, 0x00000 };
static unsigned int de0014D[] = { 0x0006F, 0x00304, 0x00000 };
static unsigned int de0014E[] = { 0x0004F, 0x00306, 0x00000 };
static unsigned int de0014F[] = { 0x0006F, 0x00306, 0x00000 };
static unsigned int de00150[] = { 0x0004F, 0x0030B, 0x00000 };
static unsigned int de00151[] = { 0x0006F, 0x0030B, 0x00000 };
static unsigned int de00154[] = { 0x00052, 0x00301, 0x00000 };
static unsigned int de00155[] = { 0x00072, 0x00301, 0x00000 };
static unsigned int de00156[] = { 0x00052, 0x00327, 0x00000 };
static unsigned int de00157[] = { 0x00072, 0x00327, 0x00000 };
static unsigned int de00158[] = { 0x00052, 0x0030C, 0x00000 };
static unsigned int de00159[] = { 0x00072, 0x0030C, 0x00000 };
static unsigned int de0015A[] = { 0x00053, 0x00301, 0x00000 };
static unsigned int de0015B[] = { 0x00073, 0x00301, 0x00000 };
static unsigned int de0015C[] = { 0x00053, 0x00302, 0x00000 };
static unsigned int de0015D[] = { 0x00073, 0x00302, 0x00000 };
static unsigned int de0015E[] = { 0x00053, 0x00327, 0x00000 };
static unsigned int de0015F[] = { 0x00073, 0x00327, 0x00000 };
static unsigned int de00160[] = { 0x00053, 0x0030C, 0x00000 };
static unsigned int de00161[] = { 0x00073, 0x0030C, 0x00000 };
static unsigned int de00162[] = { 0x00054, 0x00327, 0x00000 };
static unsigned int de00163[] = { 0x00074, 0x00327, 0x00000 };
static unsigned int de00164[] = { 0x00054, 0x0030C, 0x00000 };
static unsigned int de00165[] = { 0x00074, 0x0030C, 0x00000 };
static unsigned int de00168[] = { 0x00055, 0x00303, 0x00000 };
static unsigned int de00169[] = { 0x00075, 0x00303, 0x00000 };
static unsigned int de0016A[] = { 0x00055, 0x00304, 0x00000 };
static unsigned int de0016B[] = { 0x00075, 0x00304, 0x00000 };
static unsigned int de0016C[] = { 0x00055, 0x00306, 0x00000 };
static unsigned int de0016D[] = { 0x00075, 0x00306, 0x00000 };
static unsigned int de0016E[] = { 0x00055, 0x0030A, 0x00000 };
static unsigned int de0016F[] = { 0x00075, 0x0030A, 0x00000 };
static unsigned int de00170[] = { 0x00055, 0x0030B, 0x00000 };
static unsigned int de00171[] = { 0x00075, 0x0030B, 0x00000 };
static unsigned int de00172[] = { 0x00055, 0x00328, 0x00000 };
static unsigned int de00173[] = { 0x00075, 0x00328, 0x00000 };
static unsigned int de00174[] = { 0x00057, 0x00302, 0x00000 };
static unsigned int de00175[] = { 0x00077, 0x00302, 0x00000 };
static unsigned int de00176[] = { 0x00059, 0x00302, 0x00000 };
static unsigned int de00177[] = { 0x00079, 0x00302, 0x00000 };
static unsigned int de00178[] = { 0x00059, 0x00308, 0x00000 };
static unsigned int de00179[] = { 0x0005A, 0x00301, 0x00000 };
static unsigned int de0017A[] = { 0x0007A, 0x00301, 0x00000 };
static unsigned int de0017B[] = { 0x0005A, 0x00307, 0x00000 };
static unsigned int de0017C[] = { 0x0007A, 0x00307, 0x00000 };
static unsigned int de0017D[] = { 0x0005A, 0x0030C, 0x00000 };
static unsigned int de0017E[] = { 0x0007A, 0x0030C, 0x00000 };
static unsigned int de0017F[] = { 0x00073, 0x00000 };
static unsigned int de001A0[] = { 0x0004F, 0x0031B, 0x00000 };
static unsigned int de001A1[] = { 0x0006F, 0x0031B, 0x00000 };
static unsigned int de001AF[] = { 0x00055, 0x0031B, 0x00000 };
static unsigned int de001B0[] = { 0x00075, 0x0031B, 0x00000 };
static unsigned int de001C4[] = { 0x00044, 0x0017D, 0x00000 };
static unsigned int de001C5[] = { 0x00044, 0x0017E, 0x00000 };
static unsigned int de001C6[] = { 0x00064, 0x0017E, 0x00000 };
static unsigned int de001C7[] = { 0x0004C, 0x0004A, 0x00000 };
static unsigned int de001C8[] = { 0x0004C, 0x0006A, 0x00000 };
static unsigned int de001C9[] = { 0x0006C, 0x0006A, 0x00000 };
static unsigned int de001CA[] = { 0x0004E, 0x0004A, 0x00000 };
static unsigned int de001CB[] = { 0x0004E, 0x0006A, 0x00000 };
static unsigned int de001CC[] = { 0x0006E, 0x0006A, 0x00000 };
static unsigned int de001CD[] = { 0x00041, 0x0030C, 0x00000 };
static unsigned int de001CE[] = { 0x00061, 0x0030C, 0x00000 };
static unsigned int de001CF[] = { 0x00049, 0x0030C, 0x00000 };
static unsigned int de001D0[] = { 0x00069, 0x0030C, 0x00000 };
static unsigned int de001D1[] = { 0x0004F, 0x0030C, 0x00000 };
static unsigned int de001D2[] = { 0x0006F, 0x0030C, 0x00000 };
static unsigned int de001D3[] = { 0x00055, 0x0030C, 0x00000 };
static unsigned int de001D4[] = { 0x00075, 0x0030C, 0x00000 };
static unsigned int de001D5[] = { 0x000DC, 0x00304, 0x00000 };
static unsigned int de001D6[] = { 0x000FC, 0x00304, 0x00000 };
static unsigned int de001D7[] = { 0x000DC, 0x00301, 0x00000 };
static unsigned int de001D8[] = { 0x000FC, 0x00301, 0x00000 };
static unsigned int de001D9[] = { 0x000DC, 0x0030C, 0x00000 };
static unsigned int de001DA[] = { 0x000FC, 0x0030C, 0x00000 };
static unsigned int de001DB[] = { 0x000DC, 0x00300, 0x00000 };
static unsigned int de001DC[] = { 0x000FC, 0x00300, 0x00000 };
static unsigned int de001DE[] = { 0x000C4, 0x00304, 0x00000 };
static unsigned int de001DF[] = { 0x000E4, 0x00304, 0x00000 };
static unsigned int de001E0[] = { 0x00226, 0x00304, 0x00000 };
static unsigned int de001E1[] = { 0x00227, 0x00304, 0x00000 };
static unsigned int de001E2[] = { 0x000C6, 0x00304, 0x00000 };
static unsigned int de001E3[] = { 0x000E6, 0x00304, 0x00000 };
static unsigned int de001E6[] = { 0x00047, 0x0030C, 0x00000 };
static unsigned int de001E7[] = { 0x00067, 0x0030C, 0x00000 };
static unsigned int de001E8[] = { 0x0004B, 0x0030C, 0x00000 };
static unsigned int de001E9[] = { 0x0006B, 0x0030C, 0x00000 };
static unsigned int de001EA[] = { 0x0004F, 0x00328, 0x00000 };
static unsigned int de001EB[] = { 0x0006F, 0x00328, 0x00000 };
static unsigned int de001EC[] = { 0x001EA, 0x00304, 0x00000 };
static unsigned int de001ED[] = { 0x001EB, 0x00304, 0x00000 };
static unsigned int de001EE[] = { 0x001B7, 0x0030C, 0x00000 };
static unsigned int de001EF[] = { 0x00292, 0x0030C, 0x00000 };
static unsigned int de001F0[] = { 0x0006A, 0x0030C, 0x00000 };
static unsigned int de001F1[] = { 0x00044, 0x0005A, 0x00000 };
static unsigned int de001F2[] = { 0x00044, 0x0007A, 0x00000 };
static unsigned int de001F3[] = { 0x00064, 0x0007A, 0x00000 };
static unsigned int de001F4[] = { 0x00047, 0x00301, 0x00000 };
static unsigned int de001F5[] = { 0x00067, 0x00301, 0x00000 };
static unsigned int de001F8[] = { 0x0004E, 0x00300, 0x00000 };
static unsigned int de001F9[] = { 0x0006E, 0x00300, 0x00000 };
static unsigned int de001FA[] = { 0x000C5, 0x00301, 0x00000 };
static unsigned int de001FB[] = { 0x000E5, 0x00301, 0x00000 };
static unsigned int de001FC[] = { 0x000C6, 0x00301, 0x00000 };
static unsigned int de001FD[] = { 0x000E6, 0x00301, 0x00000 };
static unsigned int de001FE[] = { 0x000D8, 0x00301, 0x00000 };
static unsigned int de001FF[] = { 0x000F8, 0x00301, 0x00000 };
static unsigned int de00200[] = { 0x00041, 0x0030F, 0x00000 };
static unsigned int de00201[] = { 0x00061, 0x0030F, 0x00000 };
static unsigned int de00202[] = { 0x00041, 0x00311, 0x00000 };
static unsigned int de00203[] = { 0x00061, 0x00311, 0x00000 };
static unsigned int de00204[] = { 0x00045, 0x0030F, 0x00000 };
static unsigned int de00205[] = { 0x00065, 0x0030F, 0x00000 };
static unsigned int de00206[] = { 0x00045, 0x00311, 0x00000 };
static unsigned int de00207[] = { 0x00065, 0x00311, 0x00000 };
static unsigned int de00208[] = { 0x00049, 0x0030F, 0x00000 };
static unsigned int de00209[] = { 0x00069, 0x0030F, 0x00000 };
static unsigned int de0020A[] = { 0x00049, 0x00311, 0x00000 };
static unsigned int de0020B[] = { 0x00069, 0x00311, 0x00000 };
static unsigned int de0020C[] = { 0x0004F, 0x0030F, 0x00000 };
static unsigned int de0020D[] = { 0x0006F, 0x0030F, 0x00000 };
static unsigned int de0020E[] = { 0x0004F, 0x00311, 0x00000 };
static unsigned int de0020F[] = { 0x0006F, 0x00311, 0x00000 };
static unsigned int de00210[] = { 0x00052, 0x0030F, 0x00000 };
static unsigned int de00211[] = { 0x00072, 0x0030F, 0x00000 };
static unsigned int de00212[] = { 0x00052, 0x00311, 0x00000 };
static unsigned int de00213[] = { 0x00072, 0x00311, 0x00000 };
static unsigned int de00214[] = { 0x00055, 0x0030F, 0x00000 };
static unsigned int de00215[] = { 0x00075, 0x0030F, 0x00000 };
static unsigned int de00216[] = { 0x00055, 0x00311, 0x00000 };
static unsigned int de00217[] = { 0x00075, 0x00311, 0x00000 };
static unsigned int de00218[] = { 0x00053, 0x00326, 0x00000 };
static unsigned int de00219[] = { 0x00073, 0x00326, 0x00000 };
static unsigned int de0021A[] = { 0x00054, 0x00326, 0x00000 };
static unsigned int de0021B[] = { 0x00074, 0x00326, 0x00000 };
static unsigned int de0021E[] = { 0x00048, 0x0030C, 0x00000 };
static unsigned int de0021F[] = { 0x00068, 0x0030C, 0x00000 };
static unsigned int de00226[] = { 0x00041, 0x00307, 0x00000 };
static unsigned int de00227[] = { 0x00061, 0x00307, 0x00000 };
static unsigned int de00228[] = { 0x00045, 0x00327, 0x00000 };
static unsigned int de00229[] = { 0x00065, 0x00327, 0x00000 };
static unsigned int de0022A[] = { 0x000D6, 0x00304, 0x00000 };
static unsigned int de0022B[] = { 0x000F6, 0x00304, 0x00000 };
static unsigned int de0022C[] = { 0x000D5, 0x00304, 0x00000 };
static unsigned int de0022D[] = { 0x000F5, 0x00304, 0x00000 };
static unsigned int de0022E[] = { 0x0004F, 0x00307, 0x00000 };
static unsigned int de0022F[] = { 0x0006F, 0x00307, 0x00000 };
static unsigned int de00230[] = { 0x0022E, 0x00304, 0x00000 };
static unsigned int de00231[] = { 0x0022F, 0x00304, 0x00000 };
static unsigned int de00232[] = { 0x00059, 0x00304, 0x00000 };
static unsigned int de00233[] = { 0x00079, 0x00304, 0x00000 };
static unsigned int de002B0[] = { 0x00068, 0x00000 };
static unsigned int de002B1[] = { 0x00266, 0x00000 };
static unsigned int de002B2[] = { 0x0006A, 0x00000 };
static unsigned int de002B3[] = { 0x00072, 0x00000 };
static unsigned int de002B4[] = { 0x00279, 0x00000 };
static unsigned int de002B5[] = { 0x0027B, 0x00000 };
static unsigned int de002B6[] = { 0x00281, 0x00000 };
static unsigned int de002B7[] = { 0x00077, 0x00000 };
static unsigned int de002B8[] = { 0x00079, 0x00000 };
static unsigned int de002D8[] = { 0x00020, 0x00306, 0x00000 };
static unsigned int de002D9[] = { 0x00020, 0x00307, 0x00000 };
static unsigned int de002DA[] = { 0x00020, 0x0030A, 0x00000 };
static unsigned int de002DB[] = { 0x00020, 0x00328, 0x00000 };
static unsigned int de002DC[] = { 0x00020, 0x00303, 0x00000 };
static unsigned int de002DD[] = { 0x00020, 0x0030B, 0x00000 };
static unsigned int de002E0[] = { 0x00263, 0x00000 };
static unsigned int de002E1[] = { 0x0006C, 0x00000 };
static unsigned int de002E2[] = { 0x00073, 0x00000 };
static unsigned int de002E3[] = { 0x00078, 0x00000 };
static unsigned int de002E4[] = { 0x00295, 0x00000 };
static unsigned int de00340[] = { 0x00300, 0x00000 };
static unsigned int de00341[] = { 0x00301, 0x00000 };
static unsigned int de00343[] = { 0x00313, 0x00000 };
static unsigned int de00344[] = { 0x00308, 0x00301, 0x00000 };
static unsigned int de00374[] = { 0x002B9, 0x00000 };
static unsigned int de0037A[] = { 0x00020, 0x00345, 0x00000 };
static unsigned int de0037E[] = { 0x0003B, 0x00000 };
static unsigned int de00384[] = { 0x00020, 0x00301, 0x00000 };
static unsigned int de00385[] = { 0x000A8, 0x00301, 0x00000 };
static unsigned int de00386[] = { 0x00391, 0x00301, 0x00000 };
static unsigned int de00387[] = { 0x000B7, 0x00000 };
static unsigned int de00388[] = { 0x00395, 0x00301, 0x00000 };
static unsigned int de00389[] = { 0x00397, 0x00301, 0x00000 };
static unsigned int de0038A[] = { 0x00399, 0x00301, 0x00000 };
static unsigned int de0038C[] = { 0x0039F, 0x00301, 0x00000 };
static unsigned int de0038E[] = { 0x003A5, 0x00301, 0x00000 };
static unsigned int de0038F[] = { 0x003A9, 0x00301, 0x00000 };
static unsigned int de00390[] = { 0x003CA, 0x00301, 0x00000 };
static unsigned int de003AA[] = { 0x00399, 0x00308, 0x00000 };
static unsigned int de003AB[] = { 0x003A5, 0x00308, 0x00000 };
static unsigned int de003AC[] = { 0x003B1, 0x00301, 0x00000 };
static unsigned int de003AD[] = { 0x003B5, 0x00301, 0x00000 };
static unsigned int de003AE[] = { 0x003B7, 0x00301, 0x00000 };
static unsigned int de003AF[] = { 0x003B9, 0x00301, 0x00000 };
static unsigned int de003B0[] = { 0x003CB, 0x00301, 0x00000 };
static unsigned int de003CA[] = { 0x003B9, 0x00308, 0x00000 };
static unsigned int de003CB[] = { 0x003C5, 0x00308, 0x00000 };
static unsigned int de003CC[] = { 0x003BF, 0x00301, 0x00000 };
static unsigned int de003CD[] = { 0x003C5, 0x00301, 0x00000 };
static unsigned int de003CE[] = { 0x003C9, 0x00301, 0x00000 };
static unsigned int de003D0[] = { 0x003B2, 0x00000 };
static unsigned int de003D1[] = { 0x003B8, 0x00000 };
static unsigned int de003D2[] = { 0x003A5, 0x00000 };
static unsigned int de003D3[] = { 0x003D2, 0x00301, 0x00000 };
static unsigned int de003D4[] = { 0x003D2, 0x00308, 0x00000 };
static unsigned int de003D5[] = { 0x003C6, 0x00000 };
static unsigned int de003D6[] = { 0x003C0, 0x00000 };
static unsigned int de003F0[] = { 0x003BA, 0x00000 };
static unsigned int de003F1[] = { 0x003C1, 0x00000 };
static unsigned int de003F2[] = { 0x003C2, 0x00000 };
static unsigned int de003F4[] = { 0x00398, 0x00000 };
static unsigned int de003F5[] = { 0x003B5, 0x00000 };
static unsigned int de003F9[] = { 0x003A3, 0x00000 };
static unsigned int de00400[] = { 0x00415, 0x00300, 0x00000 };
static unsigned int de00401[] = { 0x00415, 0x00308, 0x00000 };
static unsigned int de00403[] = { 0x00413, 0x00301, 0x00000 };
static unsigned int de00407[] = { 0x00406, 0x00308, 0x00000 };
static unsigned int de0040C[] = { 0x0041A, 0x00301, 0x00000 };
static unsigned int de0040D[] = { 0x00418, 0x00300, 0x00000 };
static unsigned int de0040E[] = { 0x00423, 0x00306, 0x00000 };
static unsigned int de00419[] = { 0x00418, 0x00306, 0x00000 };
static unsigned int de00439[] = { 0x00438, 0x00306, 0x00000 };
static unsigned int de00450[] = { 0x00435, 0x00300, 0x00000 };
static unsigned int de00451[] = { 0x00435, 0x00308, 0x00000 };
static unsigned int de00453[] = { 0x00433, 0x00301, 0x00000 };
static unsigned int de00457[] = { 0x00456, 0x00308, 0x00000 };
static unsigned int de0045C[] = { 0x0043A, 0x00301, 0x00000 };
static unsigned int de0045D[] = { 0x00438, 0x00300, 0x00000 };
static unsigned int de0045E[] = { 0x00443, 0x00306, 0x00000 };
static unsigned int de00476[] = { 0x00474, 0x0030F, 0x00000 };
static unsigned int de00477[] = { 0x00475, 0x0030F, 0x00000 };
static unsigned int de004C1[] = { 0x00416, 0x00306, 0x00000 };
static unsigned int de004C2[] = { 0x00436, 0x00306, 0x00000 };
static unsigned int de004D0[] = { 0x00410, 0x00306, 0x00000 };
static unsigned int de004D1[] = { 0x00430, 0x00306, 0x00000 };
static unsigned int de004D2[] = { 0x00410, 0x00308, 0x00000 };
static unsigned int de004D3[] = { 0x00430, 0x00308, 0x00000 };
static unsigned int de004D6[] = { 0x00415, 0x00306, 0x00000 };
static unsigned int de004D7[] = { 0x00435, 0x00306, 0x00000 };
static unsigned int de004DA[] = { 0x004D8, 0x00308, 0x00000 };
static unsigned int de004DB[] = { 0x004D9, 0x00308, 0x00000 };
static unsigned int de004DC[] = { 0x00416, 0x00308, 0x00000 };
static unsigned int de004DD[] = { 0x00436, 0x00308, 0x00000 };
static unsigned int de004DE[] = { 0x00417, 0x00308, 0x00000 };
static unsigned int de004DF[] = { 0x00437, 0x00308, 0x00000 };
static unsigned int de004E2[] = { 0x00418, 0x00304, 0x00000 };
static unsigned int de004E3[] = { 0x00438, 0x00304, 0x00000 };
static unsigned int de004E4[] = { 0x00418, 0x00308, 0x00000 };
static unsigned int de004E5[] = { 0x00438, 0x00308, 0x00000 };
static unsigned int de004E6[] = { 0x0041E, 0x00308, 0x00000 };
static unsigned int de004E7[] = { 0x0043E, 0x00308, 0x00000 };
static unsigned int de004EA[] = { 0x004E8, 0x00308, 0x00000 };
static unsigned int de004EB[] = { 0x004E9, 0x00308, 0x00000 };
static unsigned int de004EC[] = { 0x0042D, 0x00308, 0x00000 };
static unsigned int de004ED[] = { 0x0044D, 0x00308, 0x00000 };
static unsigned int de004EE[] = { 0x00423, 0x00304, 0x00000 };
static unsigned int de004EF[] = { 0x00443, 0x00304, 0x00000 };
static unsigned int de004F0[] = { 0x00423, 0x00308, 0x00000 };
static unsigned int de004F1[] = { 0x00443, 0x00308, 0x00000 };
static unsigned int de004F2[] = { 0x00423, 0x0030B, 0x00000 };
static unsigned int de004F3[] = { 0x00443, 0x0030B, 0x00000 };
static unsigned int de004F4[] = { 0x00427, 0x00308, 0x00000 };
static unsigned int de004F5[] = { 0x00447, 0x00308, 0x00000 };
static unsigned int de004F8[] = { 0x0042B, 0x00308, 0x00000 };
static unsigned int de004F9[] = { 0x0044B, 0x00308, 0x00000 };
static unsigned int de00587[] = { 0x00565, 0x00582, 0x00000 };
static unsigned int de00622[] = { 0x00627, 0x00653, 0x00000 };
static unsigned int de00623[] = { 0x00627, 0x00654, 0x00000 };
static unsigned int de00624[] = { 0x00648, 0x00654, 0x00000 };
static unsigned int de00625[] = { 0x00627, 0x00655, 0x00000 };
static unsigned int de00626[] = { 0x0064A, 0x00654, 0x00000 };
static unsigned int de00675[] = { 0x00627, 0x00674, 0x00000 };
static unsigned int de00676[] = { 0x00648, 0x00674, 0x00000 };
static unsigned int de00677[] = { 0x006C7, 0x00674, 0x00000 };
static unsigned int de00678[] = { 0x0064A, 0x00674, 0x00000 };
static unsigned int de006C0[] = { 0x006D5, 0x00654, 0x00000 };
static unsigned int de006C2[] = { 0x006C1, 0x00654, 0x00000 };
static unsigned int de006D3[] = { 0x006D2, 0x00654, 0x00000 };
static unsigned int de00929[] = { 0x00928, 0x0093C, 0x00000 };
static unsigned int de00931[] = { 0x00930, 0x0093C, 0x00000 };
static unsigned int de00934[] = { 0x00933, 0x0093C, 0x00000 };
static unsigned int de00958[] = { 0x00915, 0x0093C, 0x00000 };
static unsigned int de00959[] = { 0x00916, 0x0093C, 0x00000 };
static unsigned int de0095A[] = { 0x00917, 0x0093C, 0x00000 };
static unsigned int de0095B[] = { 0x0091C, 0x0093C, 0x00000 };
static unsigned int de0095C[] = { 0x00921, 0x0093C, 0x00000 };
static unsigned int de0095D[] = { 0x00922, 0x0093C, 0x00000 };
static unsigned int de0095E[] = { 0x0092B, 0x0093C, 0x00000 };
static unsigned int de0095F[] = { 0x0092F, 0x0093C, 0x00000 };
static unsigned int de009CB[] = { 0x009C7, 0x009BE, 0x00000 };
static unsigned int de009CC[] = { 0x009C7, 0x009D7, 0x00000 };
static unsigned int de009DC[] = { 0x009A1, 0x009BC, 0x00000 };
static unsigned int de009DD[] = { 0x009A2, 0x009BC, 0x00000 };
static unsigned int de009DF[] = { 0x009AF, 0x009BC, 0x00000 };
static unsigned int de00A33[] = { 0x00A32, 0x00A3C, 0x00000 };
static unsigned int de00A36[] = { 0x00A38, 0x00A3C, 0x00000 };
static unsigned int de00A59[] = { 0x00A16, 0x00A3C, 0x00000 };
static unsigned int de00A5A[] = { 0x00A17, 0x00A3C, 0x00000 };
static unsigned int de00A5B[] = { 0x00A1C, 0x00A3C, 0x00000 };
static unsigned int de00A5E[] = { 0x00A2B, 0x00A3C, 0x00000 };
static unsigned int de00B48[] = { 0x00B47, 0x00B56, 0x00000 };
static unsigned int de00B4B[] = { 0x00B47, 0x00B3E, 0x00000 };
static unsigned int de00B4C[] = { 0x00B47, 0x00B57, 0x00000 };
static unsigned int de00B5C[] = { 0x00B21, 0x00B3C, 0x00000 };
static unsigned int de00B5D[] = { 0x00B22, 0x00B3C, 0x00000 };
static unsigned int de00B94[] = { 0x00B92, 0x00BD7, 0x00000 };
static unsigned int de00BCA[] = { 0x00BC6, 0x00BBE, 0x00000 };
static unsigned int de00BCB[] = { 0x00BC7, 0x00BBE, 0x00000 };
static unsigned int de00BCC[] = { 0x00BC6, 0x00BD7, 0x00000 };
static unsigned int de00C48[] = { 0x00C46, 0x00C56, 0x00000 };
static unsigned int de00CC0[] = { 0x00CBF, 0x00CD5, 0x00000 };
static unsigned int de00CC7[] = { 0x00CC6, 0x00CD5, 0x00000 };
static unsigned int de00CC8[] = { 0x00CC6, 0x00CD6, 0x00000 };
static unsigned int de00CCA[] = { 0x00CC6, 0x00CC2, 0x00000 };
static unsigned int de00CCB[] = { 0x00CCA, 0x00CD5, 0x00000 };
static unsigned int de00D4A[] = { 0x00D46, 0x00D3E, 0x00000 };
static unsigned int de00D4B[] = { 0x00D47, 0x00D3E, 0x00000 };
static unsigned int de00D4C[] = { 0x00D46, 0x00D57, 0x00000 };
static unsigned int de00DDA[] = { 0x00DD9, 0x00DCA, 0x00000 };
static unsigned int de00DDC[] = { 0x00DD9, 0x00DCF, 0x00000 };
static unsigned int de00DDD[] = { 0x00DDC, 0x00DCA, 0x00000 };
static unsigned int de00DDE[] = { 0x00DD9, 0x00DDF, 0x00000 };
static unsigned int de00E33[] = { 0x00E4D, 0x00E32, 0x00000 };
static unsigned int de00EB3[] = { 0x00ECD, 0x00EB2, 0x00000 };
static unsigned int de00EDC[] = { 0x00EAB, 0x00E99, 0x00000 };
static unsigned int de00EDD[] = { 0x00EAB, 0x00EA1, 0x00000 };
static unsigned int de00F0C[] = { 0x00F0B, 0x00000 };
static unsigned int de00F43[] = { 0x00F42, 0x00FB7, 0x00000 };
static unsigned int de00F4D[] = { 0x00F4C, 0x00FB7, 0x00000 };
static unsigned int de00F52[] = { 0x00F51, 0x00FB7, 0x00000 };
static unsigned int de00F57[] = { 0x00F56, 0x00FB7, 0x00000 };
static unsigned int de00F5C[] = { 0x00F5B, 0x00FB7, 0x00000 };
static unsigned int de00F69[] = { 0x00F40, 0x00FB5, 0x00000 };
static unsigned int de00F73[] = { 0x00F71, 0x00F72, 0x00000 };
static unsigned int de00F75[] = { 0x00F71, 0x00F74, 0x00000 };
static unsigned int de00F76[] = { 0x00FB2, 0x00F80, 0x00000 };
static unsigned int de00F77[] = { 0x00FB2, 0x00F81, 0x00000 };
static unsigned int de00F78[] = { 0x00FB3, 0x00F80, 0x00000 };
static unsigned int de00F79[] = { 0x00FB3, 0x00F81, 0x00000 };
static unsigned int de00F81[] = { 0x00F71, 0x00F80, 0x00000 };
static unsigned int de00F93[] = { 0x00F92, 0x00FB7, 0x00000 };
static unsigned int de00F9D[] = { 0x00F9C, 0x00FB7, 0x00000 };
static unsigned int de00FA2[] = { 0x00FA1, 0x00FB7, 0x00000 };
static unsigned int de00FA7[] = { 0x00FA6, 0x00FB7, 0x00000 };
static unsigned int de00FAC[] = { 0x00FAB, 0x00FB7, 0x00000 };
static unsigned int de00FB9[] = { 0x00F90, 0x00FB5, 0x00000 };
static unsigned int de01026[] = { 0x01025, 0x0102E, 0x00000 };
static unsigned int de010FC[] = { 0x010DC, 0x00000 };
static unsigned int de01D2C[] = { 0x00041, 0x00000 };
static unsigned int de01D2D[] = { 0x000C6, 0x00000 };
static unsigned int de01D2E[] = { 0x00042, 0x00000 };
static unsigned int de01D30[] = { 0x00044, 0x00000 };
static unsigned int de01D31[] = { 0x00045, 0x00000 };
static unsigned int de01D32[] = { 0x0018E, 0x00000 };
static unsigned int de01D33[] = { 0x00047, 0x00000 };
static unsigned int de01D34[] = { 0x00048, 0x00000 };
static unsigned int de01D35[] = { 0x00049, 0x00000 };
static unsigned int de01D36[] = { 0x0004A, 0x00000 };
static unsigned int de01D37[] = { 0x0004B, 0x00000 };
static unsigned int de01D38[] = { 0x0004C, 0x00000 };
static unsigned int de01D39[] = { 0x0004D, 0x00000 };
static unsigned int de01D3A[] = { 0x0004E, 0x00000 };
static unsigned int de01D3C[] = { 0x0004F, 0x00000 };
static unsigned int de01D3D[] = { 0x00222, 0x00000 };
static unsigned int de01D3E[] = { 0x00050, 0x00000 };
static unsigned int de01D3F[] = { 0x00052, 0x00000 };
static unsigned int de01D40[] = { 0x00054, 0x00000 };
static unsigned int de01D41[] = { 0x00055, 0x00000 };
static unsigned int de01D42[] = { 0x00057, 0x00000 };
static unsigned int de01D43[] = { 0x00061, 0x00000 };
static unsigned int de01D44[] = { 0x00250, 0x00000 };
static unsigned int de01D45[] = { 0x00251, 0x00000 };
static unsigned int de01D46[] = { 0x01D02, 0x00000 };
static unsigned int de01D47[] = { 0x00062, 0x00000 };
static unsigned int de01D48[] = { 0x00064, 0x00000 };
static unsigned int de01D49[] = { 0x00065, 0x00000 };
static unsigned int de01D4A[] = { 0x00259, 0x00000 };
static unsigned int de01D4B[] = { 0x0025B, 0x00000 };
static unsigned int de01D4C[] = { 0x0025C, 0x00000 };
static unsigned int de01D4D[] = { 0x00067, 0x00000 };
static unsigned int de01D4F[] = { 0x0006B, 0x00000 };
static unsigned int de01D50[] = { 0x0006D, 0x00000 };
static unsigned int de01D51[] = { 0x0014B, 0x00000 };
static unsigned int de01D52[] = { 0x0006F, 0x00000 };
static unsigned int de01D53[] = { 0x00254, 0x00000 };
static unsigned int de01D54[] = { 0x01D16, 0x00000 };
static unsigned int de01D55[] = { 0x01D17, 0x00000 };
static unsigned int de01D56[] = { 0x00070, 0x00000 };
static unsigned int de01D57[] = { 0x00074, 0x00000 };
static unsigned int de01D58[] = { 0x00075, 0x00000 };
static unsigned int de01D59[] = { 0x01D1D, 0x00000 };
static unsigned int de01D5A[] = { 0x0026F, 0x00000 };
static unsigned int de01D5B[] = { 0x00076, 0x00000 };
static unsigned int de01D5C[] = { 0x01D25, 0x00000 };
static unsigned int de01D5D[] = { 0x003B2, 0x00000 };
static unsigned int de01D5E[] = { 0x003B3, 0x00000 };
static unsigned int de01D5F[] = { 0x003B4, 0x00000 };
static unsigned int de01D60[] = { 0x003C6, 0x00000 };
static unsigned int de01D61[] = { 0x003C7, 0x00000 };
static unsigned int de01D62[] = { 0x00069, 0x00000 };
static unsigned int de01D63[] = { 0x00072, 0x00000 };
static unsigned int de01D64[] = { 0x00075, 0x00000 };
static unsigned int de01D65[] = { 0x00076, 0x00000 };
static unsigned int de01D66[] = { 0x003B2, 0x00000 };
static unsigned int de01D67[] = { 0x003B3, 0x00000 };
static unsigned int de01D68[] = { 0x003C1, 0x00000 };
static unsigned int de01D69[] = { 0x003C6, 0x00000 };
static unsigned int de01D6A[] = { 0x003C7, 0x00000 };
static unsigned int de01D78[] = { 0x0043D, 0x00000 };
static unsigned int de01D9B[] = { 0x00252, 0x00000 };
static unsigned int de01D9C[] = { 0x00063, 0x00000 };
static unsigned int de01D9D[] = { 0x00255, 0x00000 };
static unsigned int de01D9E[] = { 0x000F0, 0x00000 };
static unsigned int de01D9F[] = { 0x0025C, 0x00000 };
static unsigned int de01DA0[] = { 0x00066, 0x00000 };
static unsigned int de01DA1[] = { 0x0025F, 0x00000 };
static unsigned int de01DA2[] = { 0x00261, 0x00000 };
static unsigned int de01DA3[] = { 0x00265, 0x00000 };
static unsigned int de01DA4[] = { 0x00268, 0x00000 };
static unsigned int de01DA5[] = { 0x00269, 0x00000 };
static unsigned int de01DA6[] = { 0x0026A, 0x00000 };
static unsigned int de01DA7[] = { 0x01D7B, 0x00000 };
static unsigned int de01DA8[] = { 0x0029D, 0x00000 };
static unsigned int de01DA9[] = { 0x0026D, 0x00000 };
static unsigned int de01DAA[] = { 0x01D85, 0x00000 };
static unsigned int de01DAB[] = { 0x0029F, 0x00000 };
static unsigned int de01DAC[] = { 0x00271, 0x00000 };
static unsigned int de01DAD[] = { 0x00270, 0x00000 };
static unsigned int de01DAE[] = { 0x00272, 0x00000 };
static unsigned int de01DAF[] = { 0x00273, 0x00000 };
static unsigned int de01DB0[] = { 0x00274, 0x00000 };
static unsigned int de01DB1[] = { 0x00275, 0x00000 };
static unsigned int de01DB2[] = { 0x00278, 0x00000 };
static unsigned int de01DB3[] = { 0x00282, 0x00000 };
static unsigned int de01DB4[] = { 0x00283, 0x00000 };
static unsigned int de01DB5[] = { 0x001AB, 0x00000 };
static unsigned int de01DB6[] = { 0x00289, 0x00000 };
static unsigned int de01DB7[] = { 0x0028A, 0x00000 };
static unsigned int de01DB8[] = { 0x01D1C, 0x00000 };
static unsigned int de01DB9[] = { 0x0028B, 0x00000 };
static unsigned int de01DBA[] = { 0x0028C, 0x00000 };
static unsigned int de01DBB[] = { 0x0007A, 0x00000 };
static unsigned int de01DBC[] = { 0x00290, 0x00000 };
static unsigned int de01DBD[] = { 0x00291, 0x00000 };
static unsigned int de01DBE[] = { 0x00292, 0x00000 };
static unsigned int de01DBF[] = { 0x003B8, 0x00000 };
static unsigned int de01E00[] = { 0x00041, 0x00325, 0x00000 };
static unsigned int de01E01[] = { 0x00061, 0x00325, 0x00000 };
static unsigned int de01E02[] = { 0x00042, 0x00307, 0x00000 };
static unsigned int de01E03[] = { 0x00062, 0x00307, 0x00000 };
static unsigned int de01E04[] = { 0x00042, 0x00323, 0x00000 };
static unsigned int de01E05[] = { 0x00062, 0x00323, 0x00000 };
static unsigned int de01E06[] = { 0x00042, 0x00331, 0x00000 };
static unsigned int de01E07[] = { 0x00062, 0x00331, 0x00000 };
static unsigned int de01E08[] = { 0x000C7, 0x00301, 0x00000 };
static unsigned int de01E09[] = { 0x000E7, 0x00301, 0x00000 };
static unsigned int de01E0A[] = { 0x00044, 0x00307, 0x00000 };
static unsigned int de01E0B[] = { 0x00064, 0x00307, 0x00000 };
static unsigned int de01E0C[] = { 0x00044, 0x00323, 0x00000 };
static unsigned int de01E0D[] = { 0x00064, 0x00323, 0x00000 };
static unsigned int de01E0E[] = { 0x00044, 0x00331, 0x00000 };
static unsigned int de01E0F[] = { 0x00064, 0x00331, 0x00000 };
static unsigned int de01E10[] = { 0x00044, 0x00327, 0x00000 };
static unsigned int de01E11[] = { 0x00064, 0x00327, 0x00000 };
static unsigned int de01E12[] = { 0x00044, 0x0032D, 0x00000 };
static unsigned int de01E13[] = { 0x00064, 0x0032D, 0x00000 };
static unsigned int de01E14[] = { 0x00112, 0x00300, 0x00000 };
static unsigned int de01E15[] = { 0x00113, 0x00300, 0x00000 };
static unsigned int de01E16[] = { 0x00112, 0x00301, 0x00000 };
static unsigned int de01E17[] = { 0x00113, 0x00301, 0x00000 };
static unsigned int de01E18[] = { 0x00045, 0x0032D, 0x00000 };
static unsigned int de01E19[] = { 0x00065, 0x0032D, 0x00000 };
static unsigned int de01E1A[] = { 0x00045, 0x00330, 0x00000 };
static unsigned int de01E1B[] = { 0x00065, 0x00330, 0x00000 };
static unsigned int de01E1C[] = { 0x00228, 0x00306, 0x00000 };
static unsigned int de01E1D[] = { 0x00229, 0x00306, 0x00000 };
static unsigned int de01E1E[] = { 0x00046, 0x00307, 0x00000 };
static unsigned int de01E1F[] = { 0x00066, 0x00307, 0x00000 };
static unsigned int de01E20[] = { 0x00047, 0x00304, 0x00000 };
static unsigned int de01E21[] = { 0x00067, 0x00304, 0x00000 };
static unsigned int de01E22[] = { 0x00048, 0x00307, 0x00000 };
static unsigned int de01E23[] = { 0x00068, 0x00307, 0x00000 };
static unsigned int de01E24[] = { 0x00048, 0x00323, 0x00000 };
static unsigned int de01E25[] = { 0x00068, 0x00323, 0x00000 };
static unsigned int de01E26[] = { 0x00048, 0x00308, 0x00000 };
static unsigned int de01E27[] = { 0x00068, 0x00308, 0x00000 };
static unsigned int de01E28[] = { 0x00048, 0x00327, 0x00000 };
static unsigned int de01E29[] = { 0x00068, 0x00327, 0x00000 };
static unsigned int de01E2A[] = { 0x00048, 0x0032E, 0x00000 };
static unsigned int de01E2B[] = { 0x00068, 0x0032E, 0x00000 };
static unsigned int de01E2C[] = { 0x00049, 0x00330, 0x00000 };
static unsigned int de01E2D[] = { 0x00069, 0x00330, 0x00000 };
static unsigned int de01E2E[] = { 0x000CF, 0x00301, 0x00000 };
static unsigned int de01E2F[] = { 0x000EF, 0x00301, 0x00000 };
static unsigned int de01E30[] = { 0x0004B, 0x00301, 0x00000 };
static unsigned int de01E31[] = { 0x0006B, 0x00301, 0x00000 };
static unsigned int de01E32[] = { 0x0004B, 0x00323, 0x00000 };
static unsigned int de01E33[] = { 0x0006B, 0x00323, 0x00000 };
static unsigned int de01E34[] = { 0x0004B, 0x00331, 0x00000 };
static unsigned int de01E35[] = { 0x0006B, 0x00331, 0x00000 };
static unsigned int de01E36[] = { 0x0004C, 0x00323, 0x00000 };
static unsigned int de01E37[] = { 0x0006C, 0x00323, 0x00000 };
static unsigned int de01E38[] = { 0x01E36, 0x00304, 0x00000 };
static unsigned int de01E39[] = { 0x01E37, 0x00304, 0x00000 };
static unsigned int de01E3A[] = { 0x0004C, 0x00331, 0x00000 };
static unsigned int de01E3B[] = { 0x0006C, 0x00331, 0x00000 };
static unsigned int de01E3C[] = { 0x0004C, 0x0032D, 0x00000 };
static unsigned int de01E3D[] = { 0x0006C, 0x0032D, 0x00000 };
static unsigned int de01E3E[] = { 0x0004D, 0x00301, 0x00000 };
static unsigned int de01E3F[] = { 0x0006D, 0x00301, 0x00000 };
static unsigned int de01E40[] = { 0x0004D, 0x00307, 0x00000 };
static unsigned int de01E41[] = { 0x0006D, 0x00307, 0x00000 };
static unsigned int de01E42[] = { 0x0004D, 0x00323, 0x00000 };
static unsigned int de01E43[] = { 0x0006D, 0x00323, 0x00000 };
static unsigned int de01E44[] = { 0x0004E, 0x00307, 0x00000 };
static unsigned int de01E45[] = { 0x0006E, 0x00307, 0x00000 };
static unsigned int de01E46[] = { 0x0004E, 0x00323, 0x00000 };
static unsigned int de01E47[] = { 0x0006E, 0x00323, 0x00000 };
static unsigned int de01E48[] = { 0x0004E, 0x00331, 0x00000 };
static unsigned int de01E49[] = { 0x0006E, 0x00331, 0x00000 };
static unsigned int de01E4A[] = { 0x0004E, 0x0032D, 0x00000 };
static unsigned int de01E4B[] = { 0x0006E, 0x0032D, 0x00000 };
static unsigned int de01E4C[] = { 0x000D5, 0x00301, 0x00000 };
static unsigned int de01E4D[] = { 0x000F5, 0x00301, 0x00000 };
static unsigned int de01E4E[] = { 0x000D5, 0x00308, 0x00000 };
static unsigned int de01E4F[] = { 0x000F5, 0x00308, 0x00000 };
static unsigned int de01E50[] = { 0x0014C, 0x00300, 0x00000 };
static unsigned int de01E51[] = { 0x0014D, 0x00300, 0x00000 };
static unsigned int de01E52[] = { 0x0014C, 0x00301, 0x00000 };
static unsigned int de01E53[] = { 0x0014D, 0x00301, 0x00000 };
static unsigned int de01E54[] = { 0x00050, 0x00301, 0x00000 };
static unsigned int de01E55[] = { 0x00070, 0x00301, 0x00000 };
static unsigned int de01E56[] = { 0x00050, 0x00307, 0x00000 };
static unsigned int de01E57[] = { 0x00070, 0x00307, 0x00000 };
static unsigned int de01E58[] = { 0x00052, 0x00307, 0x00000 };
static unsigned int de01E59[] = { 0x00072, 0x00307, 0x00000 };
static unsigned int de01E5A[] = { 0x00052, 0x00323, 0x00000 };
static unsigned int de01E5B[] = { 0x00072, 0x00323, 0x00000 };
static unsigned int de01E5C[] = { 0x01E5A, 0x00304, 0x00000 };
static unsigned int de01E5D[] = { 0x01E5B, 0x00304, 0x00000 };
static unsigned int de01E5E[] = { 0x00052, 0x00331, 0x00000 };
static unsigned int de01E5F[] = { 0x00072, 0x00331, 0x00000 };
static unsigned int de01E60[] = { 0x00053, 0x00307, 0x00000 };
static unsigned int de01E61[] = { 0x00073, 0x00307, 0x00000 };
static unsigned int de01E62[] = { 0x00053, 0x00323, 0x00000 };
static unsigned int de01E63[] = { 0x00073, 0x00323, 0x00000 };
static unsigned int de01E64[] = { 0x0015A, 0x00307, 0x00000 };
static unsigned int de01E65[] = { 0x0015B, 0x00307, 0x00000 };
static unsigned int de01E66[] = { 0x00160, 0x00307, 0x00000 };
static unsigned int de01E67[] = { 0x00161, 0x00307, 0x00000 };
static unsigned int de01E68[] = { 0x01E62, 0x00307, 0x00000 };
static unsigned int de01E69[] = { 0x01E63, 0x00307, 0x00000 };
static unsigned int de01E6A[] = { 0x00054, 0x00307, 0x00000 };
static unsigned int de01E6B[] = { 0x00074, 0x00307, 0x00000 };
static unsigned int de01E6C[] = { 0x00054, 0x00323, 0x00000 };
static unsigned int de01E6D[] = { 0x00074, 0x00323, 0x00000 };
static unsigned int de01E6E[] = { 0x00054, 0x00331, 0x00000 };
static unsigned int de01E6F[] = { 0x00074, 0x00331, 0x00000 };
static unsigned int de01E70[] = { 0x00054, 0x0032D, 0x00000 };
static unsigned int de01E71[] = { 0x00074, 0x0032D, 0x00000 };
static unsigned int de01E72[] = { 0x00055, 0x00324, 0x00000 };
static unsigned int de01E73[] = { 0x00075, 0x00324, 0x00000 };
static unsigned int de01E74[] = { 0x00055, 0x00330, 0x00000 };
static unsigned int de01E75[] = { 0x00075, 0x00330, 0x00000 };
static unsigned int de01E76[] = { 0x00055, 0x0032D, 0x00000 };
static unsigned int de01E77[] = { 0x00075, 0x0032D, 0x00000 };
static unsigned int de01E78[] = { 0x00168, 0x00301, 0x00000 };
static unsigned int de01E79[] = { 0x00169, 0x00301, 0x00000 };
static unsigned int de01E7A[] = { 0x0016A, 0x00308, 0x00000 };
static unsigned int de01E7B[] = { 0x0016B, 0x00308, 0x00000 };
static unsigned int de01E7C[] = { 0x00056, 0x00303, 0x00000 };
static unsigned int de01E7D[] = { 0x00076, 0x00303, 0x00000 };
static unsigned int de01E7E[] = { 0x00056, 0x00323, 0x00000 };
static unsigned int de01E7F[] = { 0x00076, 0x00323, 0x00000 };
static unsigned int de01E80[] = { 0x00057, 0x00300, 0x00000 };
static unsigned int de01E81[] = { 0x00077, 0x00300, 0x00000 };
static unsigned int de01E82[] = { 0x00057, 0x00301, 0x00000 };
static unsigned int de01E83[] = { 0x00077, 0x00301, 0x00000 };
static unsigned int de01E84[] = { 0x00057, 0x00308, 0x00000 };
static unsigned int de01E85[] = { 0x00077, 0x00308, 0x00000 };
static unsigned int de01E86[] = { 0x00057, 0x00307, 0x00000 };
static unsigned int de01E87[] = { 0x00077, 0x00307, 0x00000 };
static unsigned int de01E88[] = { 0x00057, 0x00323, 0x00000 };
static unsigned int de01E89[] = { 0x00077, 0x00323, 0x00000 };
static unsigned int de01E8A[] = { 0x00058, 0x00307, 0x00000 };
static unsigned int de01E8B[] = { 0x00078, 0x00307, 0x00000 };
static unsigned int de01E8C[] = { 0x00058, 0x00308, 0x00000 };
static unsigned int de01E8D[] = { 0x00078, 0x00308, 0x00000 };
static unsigned int de01E8E[] = { 0x00059, 0x00307, 0x00000 };
static unsigned int de01E8F[] = { 0x00079, 0x00307, 0x00000 };
static unsigned int de01E90[] = { 0x0005A, 0x00302, 0x00000 };
static unsigned int de01E91[] = { 0x0007A, 0x00302, 0x00000 };
static unsigned int de01E92[] = { 0x0005A, 0x00323, 0x00000 };
static unsigned int de01E93[] = { 0x0007A, 0x00323, 0x00000 };
static unsigned int de01E94[] = { 0x0005A, 0x00331, 0x00000 };
static unsigned int de01E95[] = { 0x0007A, 0x00331, 0x00000 };
static unsigned int de01E96[] = { 0x00068, 0x00331, 0x00000 };
static unsigned int de01E97[] = { 0x00074, 0x00308, 0x00000 };
static unsigned int de01E98[] = { 0x00077, 0x0030A, 0x00000 };
static unsigned int de01E99[] = { 0x00079, 0x0030A, 0x00000 };
static unsigned int de01E9A[] = { 0x00061, 0x002BE, 0x00000 };
static unsigned int de01E9B[] = { 0x0017F, 0x00307, 0x00000 };
static unsigned int de01EA0[] = { 0x00041, 0x00323, 0x00000 };
static unsigned int de01EA1[] = { 0x00061, 0x00323, 0x00000 };
static unsigned int de01EA2[] = { 0x00041, 0x00309, 0x00000 };
static unsigned int de01EA3[] = { 0x00061, 0x00309, 0x00000 };
static unsigned int de01EA4[] = { 0x000C2, 0x00301, 0x00000 };
static unsigned int de01EA5[] = { 0x000E2, 0x00301, 0x00000 };
static unsigned int de01EA6[] = { 0x000C2, 0x00300, 0x00000 };
static unsigned int de01EA7[] = { 0x000E2, 0x00300, 0x00000 };
static unsigned int de01EA8[] = { 0x000C2, 0x00309, 0x00000 };
static unsigned int de01EA9[] = { 0x000E2, 0x00309, 0x00000 };
static unsigned int de01EAA[] = { 0x000C2, 0x00303, 0x00000 };
static unsigned int de01EAB[] = { 0x000E2, 0x00303, 0x00000 };
static unsigned int de01EAC[] = { 0x01EA0, 0x00302, 0x00000 };
static unsigned int de01EAD[] = { 0x01EA1, 0x00302, 0x00000 };
static unsigned int de01EAE[] = { 0x00102, 0x00301, 0x00000 };
static unsigned int de01EAF[] = { 0x00103, 0x00301, 0x00000 };
static unsigned int de01EB0[] = { 0x00102, 0x00300, 0x00000 };
static unsigned int de01EB1[] = { 0x00103, 0x00300, 0x00000 };
static unsigned int de01EB2[] = { 0x00102, 0x00309, 0x00000 };
static unsigned int de01EB3[] = { 0x00103, 0x00309, 0x00000 };
static unsigned int de01EB4[] = { 0x00102, 0x00303, 0x00000 };
static unsigned int de01EB5[] = { 0x00103, 0x00303, 0x00000 };
static unsigned int de01EB6[] = { 0x01EA0, 0x00306, 0x00000 };
static unsigned int de01EB7[] = { 0x01EA1, 0x00306, 0x00000 };
static unsigned int de01EB8[] = { 0x00045, 0x00323, 0x00000 };
static unsigned int de01EB9[] = { 0x00065, 0x00323, 0x00000 };
static unsigned int de01EBA[] = { 0x00045, 0x00309, 0x00000 };
static unsigned int de01EBB[] = { 0x00065, 0x00309, 0x00000 };
static unsigned int de01EBC[] = { 0x00045, 0x00303, 0x00000 };
static unsigned int de01EBD[] = { 0x00065, 0x00303, 0x00000 };
static unsigned int de01EBE[] = { 0x000CA, 0x00301, 0x00000 };
static unsigned int de01EBF[] = { 0x000EA, 0x00301, 0x00000 };
static unsigned int de01EC0[] = { 0x000CA, 0x00300, 0x00000 };
static unsigned int de01EC1[] = { 0x000EA, 0x00300, 0x00000 };
static unsigned int de01EC2[] = { 0x000CA, 0x00309, 0x00000 };
static unsigned int de01EC3[] = { 0x000EA, 0x00309, 0x00000 };
static unsigned int de01EC4[] = { 0x000CA, 0x00303, 0x00000 };
static unsigned int de01EC5[] = { 0x000EA, 0x00303, 0x00000 };
static unsigned int de01EC6[] = { 0x01EB8, 0x00302, 0x00000 };
static unsigned int de01EC7[] = { 0x01EB9, 0x00302, 0x00000 };
static unsigned int de01EC8[] = { 0x00049, 0x00309, 0x00000 };
static unsigned int de01EC9[] = { 0x00069, 0x00309, 0x00000 };
static unsigned int de01ECA[] = { 0x00049, 0x00323, 0x00000 };
static unsigned int de01ECB[] = { 0x00069, 0x00323, 0x00000 };
static unsigned int de01ECC[] = { 0x0004F, 0x00323, 0x00000 };
static unsigned int de01ECD[] = { 0x0006F, 0x00323, 0x00000 };
static unsigned int de01ECE[] = { 0x0004F, 0x00309, 0x00000 };
static unsigned int de01ECF[] = { 0x0006F, 0x00309, 0x00000 };
static unsigned int de01ED0[] = { 0x000D4, 0x00301, 0x00000 };
static unsigned int de01ED1[] = { 0x000F4, 0x00301, 0x00000 };
static unsigned int de01ED2[] = { 0x000D4, 0x00300, 0x00000 };
static unsigned int de01ED3[] = { 0x000F4, 0x00300, 0x00000 };
static unsigned int de01ED4[] = { 0x000D4, 0x00309, 0x00000 };
static unsigned int de01ED5[] = { 0x000F4, 0x00309, 0x00000 };
static unsigned int de01ED6[] = { 0x000D4, 0x00303, 0x00000 };
static unsigned int de01ED7[] = { 0x000F4, 0x00303, 0x00000 };
static unsigned int de01ED8[] = { 0x01ECC, 0x00302, 0x00000 };
static unsigned int de01ED9[] = { 0x01ECD, 0x00302, 0x00000 };
static unsigned int de01EDA[] = { 0x001A0, 0x00301, 0x00000 };
static unsigned int de01EDB[] = { 0x001A1, 0x00301, 0x00000 };
static unsigned int de01EDC[] = { 0x001A0, 0x00300, 0x00000 };
static unsigned int de01EDD[] = { 0x001A1, 0x00300, 0x00000 };
static unsigned int de01EDE[] = { 0x001A0, 0x00309, 0x00000 };
static unsigned int de01EDF[] = { 0x001A1, 0x00309, 0x00000 };
static unsigned int de01EE0[] = { 0x001A0, 0x00303, 0x00000 };
static unsigned int de01EE1[] = { 0x001A1, 0x00303, 0x00000 };
static unsigned int de01EE2[] = { 0x001A0, 0x00323, 0x00000 };
static unsigned int de01EE3[] = { 0x001A1, 0x00323, 0x00000 };
static unsigned int de01EE4[] = { 0x00055, 0x00323, 0x00000 };
static unsigned int de01EE5[] = { 0x00075, 0x00323, 0x00000 };
static unsigned int de01EE6[] = { 0x00055, 0x00309, 0x00000 };
static unsigned int de01EE7[] = { 0x00075, 0x00309, 0x00000 };
static unsigned int de01EE8[] = { 0x001AF, 0x00301, 0x00000 };
static unsigned int de01EE9[] = { 0x001B0, 0x00301, 0x00000 };
static unsigned int de01EEA[] = { 0x001AF, 0x00300, 0x00000 };
static unsigned int de01EEB[] = { 0x001B0, 0x00300, 0x00000 };
static unsigned int de01EEC[] = { 0x001AF, 0x00309, 0x00000 };
static unsigned int de01EED[] = { 0x001B0, 0x00309, 0x00000 };
static unsigned int de01EEE[] = { 0x001AF, 0x00303, 0x00000 };
static unsigned int de01EEF[] = { 0x001B0, 0x00303, 0x00000 };
static unsigned int de01EF0[] = { 0x001AF, 0x00323, 0x00000 };
static unsigned int de01EF1[] = { 0x001B0, 0x00323, 0x00000 };
static unsigned int de01EF2[] = { 0x00059, 0x00300, 0x00000 };
static unsigned int de01EF3[] = { 0x00079, 0x00300, 0x00000 };
static unsigned int de01EF4[] = { 0x00059, 0x00323, 0x00000 };
static unsigned int de01EF5[] = { 0x00079, 0x00323, 0x00000 };
static unsigned int de01EF6[] = { 0x00059, 0x00309, 0x00000 };
static unsigned int de01EF7[] = { 0x00079, 0x00309, 0x00000 };
static unsigned int de01EF8[] = { 0x00059, 0x00303, 0x00000 };
static unsigned int de01EF9[] = { 0x00079, 0x00303, 0x00000 };
static unsigned int de01F00[] = { 0x003B1, 0x00313, 0x00000 };
static unsigned int de01F01[] = { 0x003B1, 0x00314, 0x00000 };
static unsigned int de01F02[] = { 0x01F00, 0x00300, 0x00000 };
static unsigned int de01F03[] = { 0x01F01, 0x00300, 0x00000 };
static unsigned int de01F04[] = { 0x01F00, 0x00301, 0x00000 };
static unsigned int de01F05[] = { 0x01F01, 0x00301, 0x00000 };
static unsigned int de01F06[] = { 0x01F00, 0x00342, 0x00000 };
static unsigned int de01F07[] = { 0x01F01, 0x00342, 0x00000 };
static unsigned int de01F08[] = { 0x00391, 0x00313, 0x00000 };
static unsigned int de01F09[] = { 0x00391, 0x00314, 0x00000 };
static unsigned int de01F0A[] = { 0x01F08, 0x00300, 0x00000 };
static unsigned int de01F0B[] = { 0x01F09, 0x00300, 0x00000 };
static unsigned int de01F0C[] = { 0x01F08, 0x00301, 0x00000 };
static unsigned int de01F0D[] = { 0x01F09, 0x00301, 0x00000 };
static unsigned int de01F0E[] = { 0x01F08, 0x00342, 0x00000 };
static unsigned int de01F0F[] = { 0x01F09, 0x00342, 0x00000 };
static unsigned int de01F10[] = { 0x003B5, 0x00313, 0x00000 };
static unsigned int de01F11[] = { 0x003B5, 0x00314, 0x00000 };
static unsigned int de01F12[] = { 0x01F10, 0x00300, 0x00000 };
static unsigned int de01F13[] = { 0x01F11, 0x00300, 0x00000 };
static unsigned int de01F14[] = { 0x01F10, 0x00301, 0x00000 };
static unsigned int de01F15[] = { 0x01F11, 0x00301, 0x00000 };
static unsigned int de01F18[] = { 0x00395, 0x00313, 0x00000 };
static unsigned int de01F19[] = { 0x00395, 0x00314, 0x00000 };
static unsigned int de01F1A[] = { 0x01F18, 0x00300, 0x00000 };
static unsigned int de01F1B[] = { 0x01F19, 0x00300, 0x00000 };
static unsigned int de01F1C[] = { 0x01F18, 0x00301, 0x00000 };
static unsigned int de01F1D[] = { 0x01F19, 0x00301, 0x00000 };
static unsigned int de01F20[] = { 0x003B7, 0x00313, 0x00000 };
static unsigned int de01F21[] = { 0x003B7, 0x00314, 0x00000 };
static unsigned int de01F22[] = { 0x01F20, 0x00300, 0x00000 };
static unsigned int de01F23[] = { 0x01F21, 0x00300, 0x00000 };
static unsigned int de01F24[] = { 0x01F20, 0x00301, 0x00000 };
static unsigned int de01F25[] = { 0x01F21, 0x00301, 0x00000 };
static unsigned int de01F26[] = { 0x01F20, 0x00342, 0x00000 };
static unsigned int de01F27[] = { 0x01F21, 0x00342, 0x00000 };
static unsigned int de01F28[] = { 0x00397, 0x00313, 0x00000 };
static unsigned int de01F29[] = { 0x00397, 0x00314, 0x00000 };
static unsigned int de01F2A[] = { 0x01F28, 0x00300, 0x00000 };
static unsigned int de01F2B[] = { 0x01F29, 0x00300, 0x00000 };
static unsigned int de01F2C[] = { 0x01F28, 0x00301, 0x00000 };
static unsigned int de01F2D[] = { 0x01F29, 0x00301, 0x00000 };
static unsigned int de01F2E[] = { 0x01F28, 0x00342, 0x00000 };
static unsigned int de01F2F[] = { 0x01F29, 0x00342, 0x00000 };
static unsigned int de01F30[] = { 0x003B9, 0x00313, 0x00000 };
static unsigned int de01F31[] = { 0x003B9, 0x00314, 0x00000 };
static unsigned int de01F32[] = { 0x01F30, 0x00300, 0x00000 };
static unsigned int de01F33[] = { 0x01F31, 0x00300, 0x00000 };
static unsigned int de01F34[] = { 0x01F30, 0x00301, 0x00000 };
static unsigned int de01F35[] = { 0x01F31, 0x00301, 0x00000 };
static unsigned int de01F36[] = { 0x01F30, 0x00342, 0x00000 };
static unsigned int de01F37[] = { 0x01F31, 0x00342, 0x00000 };
static unsigned int de01F38[] = { 0x00399, 0x00313, 0x00000 };
static unsigned int de01F39[] = { 0x00399, 0x00314, 0x00000 };
static unsigned int de01F3A[] = { 0x01F38, 0x00300, 0x00000 };
static unsigned int de01F3B[] = { 0x01F39, 0x00300, 0x00000 };
static unsigned int de01F3C[] = { 0x01F38, 0x00301, 0x00000 };
static unsigned int de01F3D[] = { 0x01F39, 0x00301, 0x00000 };
static unsigned int de01F3E[] = { 0x01F38, 0x00342, 0x00000 };
static unsigned int de01F3F[] = { 0x01F39, 0x00342, 0x00000 };
static unsigned int de01F40[] = { 0x003BF, 0x00313, 0x00000 };
static unsigned int de01F41[] = { 0x003BF, 0x00314, 0x00000 };
static unsigned int de01F42[] = { 0x01F40, 0x00300, 0x00000 };
static unsigned int de01F43[] = { 0x01F41, 0x00300, 0x00000 };
static unsigned int de01F44[] = { 0x01F40, 0x00301, 0x00000 };
static unsigned int de01F45[] = { 0x01F41, 0x00301, 0x00000 };
static unsigned int de01F48[] = { 0x0039F, 0x00313, 0x00000 };
static unsigned int de01F49[] = { 0x0039F, 0x00314, 0x00000 };
static unsigned int de01F4A[] = { 0x01F48, 0x00300, 0x00000 };
static unsigned int de01F4B[] = { 0x01F49, 0x00300, 0x00000 };
static unsigned int de01F4C[] = { 0x01F48, 0x00301, 0x00000 };
static unsigned int de01F4D[] = { 0x01F49, 0x00301, 0x00000 };
static unsigned int de01F50[] = { 0x003C5, 0x00313, 0x00000 };
static unsigned int de01F51[] = { 0x003C5, 0x00314, 0x00000 };
static unsigned int de01F52[] = { 0x01F50, 0x00300, 0x00000 };
static unsigned int de01F53[] = { 0x01F51, 0x00300, 0x00000 };
static unsigned int de01F54[] = { 0x01F50, 0x00301, 0x00000 };
static unsigned int de01F55[] = { 0x01F51, 0x00301, 0x00000 };
static unsigned int de01F56[] = { 0x01F50, 0x00342, 0x00000 };
static unsigned int de01F57[] = { 0x01F51, 0x00342, 0x00000 };
static unsigned int de01F59[] = { 0x003A5, 0x00314, 0x00000 };
static unsigned int de01F5B[] = { 0x01F59, 0x00300, 0x00000 };
static unsigned int de01F5D[] = { 0x01F59, 0x00301, 0x00000 };
static unsigned int de01F5F[] = { 0x01F59, 0x00342, 0x00000 };
static unsigned int de01F60[] = { 0x003C9, 0x00313, 0x00000 };
static unsigned int de01F61[] = { 0x003C9, 0x00314, 0x00000 };
static unsigned int de01F62[] = { 0x01F60, 0x00300, 0x00000 };
static unsigned int de01F63[] = { 0x01F61, 0x00300, 0x00000 };
static unsigned int de01F64[] = { 0x01F60, 0x00301, 0x00000 };
static unsigned int de01F65[] = { 0x01F61, 0x00301, 0x00000 };
static unsigned int de01F66[] = { 0x01F60, 0x00342, 0x00000 };
static unsigned int de01F67[] = { 0x01F61, 0x00342, 0x00000 };
static unsigned int de01F68[] = { 0x003A9, 0x00313, 0x00000 };
static unsigned int de01F69[] = { 0x003A9, 0x00314, 0x00000 };
static unsigned int de01F6A[] = { 0x01F68, 0x00300, 0x00000 };
static unsigned int de01F6B[] = { 0x01F69, 0x00300, 0x00000 };
static unsigned int de01F6C[] = { 0x01F68, 0x00301, 0x00000 };
static unsigned int de01F6D[] = { 0x01F69, 0x00301, 0x00000 };
static unsigned int de01F6E[] = { 0x01F68, 0x00342, 0x00000 };
static unsigned int de01F6F[] = { 0x01F69, 0x00342, 0x00000 };
static unsigned int de01F70[] = { 0x003B1, 0x00300, 0x00000 };
static unsigned int de01F71[] = { 0x003AC, 0x00000 };
static unsigned int de01F72[] = { 0x003B5, 0x00300, 0x00000 };
static unsigned int de01F73[] = { 0x003AD, 0x00000 };
static unsigned int de01F74[] = { 0x003B7, 0x00300, 0x00000 };
static unsigned int de01F75[] = { 0x003AE, 0x00000 };
static unsigned int de01F76[] = { 0x003B9, 0x00300, 0x00000 };
static unsigned int de01F77[] = { 0x003AF, 0x00000 };
static unsigned int de01F78[] = { 0x003BF, 0x00300, 0x00000 };
static unsigned int de01F79[] = { 0x003CC, 0x00000 };
static unsigned int de01F7A[] = { 0x003C5, 0x00300, 0x00000 };
static unsigned int de01F7B[] = { 0x003CD, 0x00000 };
static unsigned int de01F7C[] = { 0x003C9, 0x00300, 0x00000 };
static unsigned int de01F7D[] = { 0x003CE, 0x00000 };
static unsigned int de01F80[] = { 0x01F00, 0x00345, 0x00000 };
static unsigned int de01F81[] = { 0x01F01, 0x00345, 0x00000 };
static unsigned int de01F82[] = { 0x01F02, 0x00345, 0x00000 };
static unsigned int de01F83[] = { 0x01F03, 0x00345, 0x00000 };
static unsigned int de01F84[] = { 0x01F04, 0x00345, 0x00000 };
static unsigned int de01F85[] = { 0x01F05, 0x00345, 0x00000 };
static unsigned int de01F86[] = { 0x01F06, 0x00345, 0x00000 };
static unsigned int de01F87[] = { 0x01F07, 0x00345, 0x00000 };
static unsigned int de01F88[] = { 0x01F08, 0x00345, 0x00000 };
static unsigned int de01F89[] = { 0x01F09, 0x00345, 0x00000 };
static unsigned int de01F8A[] = { 0x01F0A, 0x00345, 0x00000 };
static unsigned int de01F8B[] = { 0x01F0B, 0x00345, 0x00000 };
static unsigned int de01F8C[] = { 0x01F0C, 0x00345, 0x00000 };
static unsigned int de01F8D[] = { 0x01F0D, 0x00345, 0x00000 };
static unsigned int de01F8E[] = { 0x01F0E, 0x00345, 0x00000 };
static unsigned int de01F8F[] = { 0x01F0F, 0x00345, 0x00000 };
static unsigned int de01F90[] = { 0x01F20, 0x00345, 0x00000 };
static unsigned int de01F91[] = { 0x01F21, 0x00345, 0x00000 };
static unsigned int de01F92[] = { 0x01F22, 0x00345, 0x00000 };
static unsigned int de01F93[] = { 0x01F23, 0x00345, 0x00000 };
static unsigned int de01F94[] = { 0x01F24, 0x00345, 0x00000 };
static unsigned int de01F95[] = { 0x01F25, 0x00345, 0x00000 };
static unsigned int de01F96[] = { 0x01F26, 0x00345, 0x00000 };
static unsigned int de01F97[] = { 0x01F27, 0x00345, 0x00000 };
static unsigned int de01F98[] = { 0x01F28, 0x00345, 0x00000 };
static unsigned int de01F99[] = { 0x01F29, 0x00345, 0x00000 };
static unsigned int de01F9A[] = { 0x01F2A, 0x00345, 0x00000 };
static unsigned int de01F9B[] = { 0x01F2B, 0x00345, 0x00000 };
static unsigned int de01F9C[] = { 0x01F2C, 0x00345, 0x00000 };
static unsigned int de01F9D[] = { 0x01F2D, 0x00345, 0x00000 };
static unsigned int de01F9E[] = { 0x01F2E, 0x00345, 0x00000 };
static unsigned int de01F9F[] = { 0x01F2F, 0x00345, 0x00000 };
static unsigned int de01FA0[] = { 0x01F60, 0x00345, 0x00000 };
static unsigned int de01FA1[] = { 0x01F61, 0x00345, 0x00000 };
static unsigned int de01FA2[] = { 0x01F62, 0x00345, 0x00000 };
static unsigned int de01FA3[] = { 0x01F63, 0x00345, 0x00000 };
static unsigned int de01FA4[] = { 0x01F64, 0x00345, 0x00000 };
static unsigned int de01FA5[] = { 0x01F65, 0x00345, 0x00000 };
static unsigned int de01FA6[] = { 0x01F66, 0x00345, 0x00000 };
static unsigned int de01FA7[] = { 0x01F67, 0x00345, 0x00000 };
static unsigned int de01FA8[] = { 0x01F68, 0x00345, 0x00000 };
static unsigned int de01FA9[] = { 0x01F69, 0x00345, 0x00000 };
static unsigned int de01FAA[] = { 0x01F6A, 0x00345, 0x00000 };
static unsigned int de01FAB[] = { 0x01F6B, 0x00345, 0x00000 };
static unsigned int de01FAC[] = { 0x01F6C, 0x00345, 0x00000 };
static unsigned int de01FAD[] = { 0x01F6D, 0x00345, 0x00000 };
static unsigned int de01FAE[] = { 0x01F6E, 0x00345, 0x00000 };
static unsigned int de01FAF[] = { 0x01F6F, 0x00345, 0x00000 };
static unsigned int de01FB0[] = { 0x003B1, 0x00306, 0x00000 };
static unsigned int de01FB1[] = { 0x003B1, 0x00304, 0x00000 };
static unsigned int de01FB2[] = { 0x01F70, 0x00345, 0x00000 };
static unsigned int de01FB3[] = { 0x003B1, 0x00345, 0x00000 };
static unsigned int de01FB4[] = { 0x003AC, 0x00345, 0x00000 };
static unsigned int de01FB6[] = { 0x003B1, 0x00342, 0x00000 };
static unsigned int de01FB7[] = { 0x01FB6, 0x00345, 0x00000 };
static unsigned int de01FB8[] = { 0x00391, 0x00306, 0x00000 };
static unsigned int de01FB9[] = { 0x00391, 0x00304, 0x00000 };
static unsigned int de01FBA[] = { 0x00391, 0x00300, 0x00000 };
static unsigned int de01FBB[] = { 0x00386, 0x00000 };
static unsigned int de01FBC[] = { 0x00391, 0x00345, 0x00000 };
static unsigned int de01FBD[] = { 0x00020, 0x00313, 0x00000 };
static unsigned int de01FBE[] = { 0x003B9, 0x00000 };
static unsigned int de01FBF[] = { 0x00020, 0x00313, 0x00000 };
static unsigned int de01FC0[] = { 0x00020, 0x00342, 0x00000 };
static unsigned int de01FC1[] = { 0x000A8, 0x00342, 0x00000 };
static unsigned int de01FC2[] = { 0x01F74, 0x00345, 0x00000 };
static unsigned int de01FC3[] = { 0x003B7, 0x00345, 0x00000 };
static unsigned int de01FC4[] = { 0x003AE, 0x00345, 0x00000 };
static unsigned int de01FC6[] = { 0x003B7, 0x00342, 0x00000 };
static unsigned int de01FC7[] = { 0x01FC6, 0x00345, 0x00000 };
static unsigned int de01FC8[] = { 0x00395, 0x00300, 0x00000 };
static unsigned int de01FC9[] = { 0x00388, 0x00000 };
static unsigned int de01FCA[] = { 0x00397, 0x00300, 0x00000 };
static unsigned int de01FCB[] = { 0x00389, 0x00000 };
static unsigned int de01FCC[] = { 0x00397, 0x00345, 0x00000 };
static unsigned int de01FCD[] = { 0x01FBF, 0x00300, 0x00000 };
static unsigned int de01FCE[] = { 0x01FBF, 0x00301, 0x00000 };
static unsigned int de01FCF[] = { 0x01FBF, 0x00342, 0x00000 };
static unsigned int de01FD0[] = { 0x003B9, 0x00306, 0x00000 };
static unsigned int de01FD1[] = { 0x003B9, 0x00304, 0x00000 };
static unsigned int de01FD2[] = { 0x003CA, 0x00300, 0x00000 };
static unsigned int de01FD3[] = { 0x00390, 0x00000 };
static unsigned int de01FD6[] = { 0x003B9, 0x00342, 0x00000 };
static unsigned int de01FD7[] = { 0x003CA, 0x00342, 0x00000 };
static unsigned int de01FD8[] = { 0x00399, 0x00306, 0x00000 };
static unsigned int de01FD9[] = { 0x00399, 0x00304, 0x00000 };
static unsigned int de01FDA[] = { 0x00399, 0x00300, 0x00000 };
static unsigned int de01FDB[] = { 0x0038A, 0x00000 };
static unsigned int de01FDD[] = { 0x01FFE, 0x00300, 0x00000 };
static unsigned int de01FDE[] = { 0x01FFE, 0x00301, 0x00000 };
static unsigned int de01FDF[] = { 0x01FFE, 0x00342, 0x00000 };
static unsigned int de01FE0[] = { 0x003C5, 0x00306, 0x00000 };
static unsigned int de01FE1[] = { 0x003C5, 0x00304, 0x00000 };
static unsigned int de01FE2[] = { 0x003CB, 0x00300, 0x00000 };
static unsigned int de01FE3[] = { 0x003B0, 0x00000 };
static unsigned int de01FE4[] = { 0x003C1, 0x00313, 0x00000 };
static unsigned int de01FE5[] = { 0x003C1, 0x00314, 0x00000 };
static unsigned int de01FE6[] = { 0x003C5, 0x00342, 0x00000 };
static unsigned int de01FE7[] = { 0x003CB, 0x00342, 0x00000 };
static unsigned int de01FE8[] = { 0x003A5, 0x00306, 0x00000 };
static unsigned int de01FE9[] = { 0x003A5, 0x00304, 0x00000 };
static unsigned int de01FEA[] = { 0x003A5, 0x00300, 0x00000 };
static unsigned int de01FEB[] = { 0x0038E, 0x00000 };
static unsigned int de01FEC[] = { 0x003A1, 0x00314, 0x00000 };
static unsigned int de01FED[] = { 0x000A8, 0x00300, 0x00000 };
static unsigned int de01FEE[] = { 0x00385, 0x00000 };
static unsigned int de01FEF[] = { 0x00060, 0x00000 };
static unsigned int de01FF2[] = { 0x01F7C, 0x00345, 0x00000 };
static unsigned int de01FF3[] = { 0x003C9, 0x00345, 0x00000 };
static unsigned int de01FF4[] = { 0x003CE, 0x00345, 0x00000 };
static unsigned int de01FF6[] = { 0x003C9, 0x00342, 0x00000 };
static unsigned int de01FF7[] = { 0x01FF6, 0x00345, 0x00000 };
static unsigned int de01FF8[] = { 0x0039F, 0x00300, 0x00000 };
static unsigned int de01FF9[] = { 0x0038C, 0x00000 };
static unsigned int de01FFA[] = { 0x003A9, 0x00300, 0x00000 };
static unsigned int de01FFB[] = { 0x0038F, 0x00000 };
static unsigned int de01FFC[] = { 0x003A9, 0x00345, 0x00000 };
static unsigned int de01FFD[] = { 0x000B4, 0x00000 };
static unsigned int de01FFE[] = { 0x00020, 0x00314, 0x00000 };
static unsigned int de02000[] = { 0x02002, 0x00000 };
static unsigned int de02001[] = { 0x02003, 0x00000 };
static unsigned int de02002[] = { 0x00020, 0x00000 };
static unsigned int de02003[] = { 0x00020, 0x00000 };
static unsigned int de02004[] = { 0x00020, 0x00000 };
static unsigned int de02005[] = { 0x00020, 0x00000 };
static unsigned int de02006[] = { 0x00020, 0x00000 };
static unsigned int de02007[] = { 0x00020, 0x00000 };
static unsigned int de02008[] = { 0x00020, 0x00000 };
static unsigned int de02009[] = { 0x00020, 0x00000 };
static unsigned int de0200A[] = { 0x00020, 0x00000 };
static unsigned int de02011[] = { 0x02010, 0x00000 };
static unsigned int de02017[] = { 0x00020, 0x00333, 0x00000 };
static unsigned int de02024[] = { 0x0002E, 0x00000 };
static unsigned int de02025[] = { 0x0002E, 0x0002E, 0x00000 };
static unsigned int de02026[] = { 0x0002E, 0x0002E, 0x0002E, 0x00000 };
static unsigned int de0202F[] = { 0x00020, 0x00000 };
static unsigned int de02033[] = { 0x02032, 0x02032, 0x00000 };
static unsigned int de02034[] = { 0x02032, 0x02032, 0x02032, 0x00000 };
static unsigned int de02036[] = { 0x02035, 0x02035, 0x00000 };
static unsigned int de02037[] = { 0x02035, 0x02035, 0x02035, 0x00000 };
static unsigned int de0203C[] = { 0x00021, 0x00021, 0x00000 };
static unsigned int de0203E[] = { 0x00020, 0x00305, 0x00000 };
static unsigned int de02047[] = { 0x0003F, 0x0003F, 0x00000 };
static unsigned int de02048[] = { 0x0003F, 0x00021, 0x00000 };
static unsigned int de02049[] = { 0x00021, 0x0003F, 0x00000 };
static unsigned int de02057[] = { 0x02032, 0x02032, 0x02032, 0x02032, 0x00000 };
static unsigned int de0205F[] = { 0x00020, 0x00000 };
static unsigned int de02070[] = { 0x00030, 0x00000 };
static unsigned int de02071[] = { 0x00069, 0x00000 };
static unsigned int de02074[] = { 0x00034, 0x00000 };
static unsigned int de02075[] = { 0x00035, 0x00000 };
static unsigned int de02076[] = { 0x00036, 0x00000 };
static unsigned int de02077[] = { 0x00037, 0x00000 };
static unsigned int de02078[] = { 0x00038, 0x00000 };
static unsigned int de02079[] = { 0x00039, 0x00000 };
static unsigned int de0207A[] = { 0x0002B, 0x00000 };
static unsigned int de0207B[] = { 0x02212, 0x00000 };
static unsigned int de0207C[] = { 0x0003D, 0x00000 };
static unsigned int de0207D[] = { 0x00028, 0x00000 };
static unsigned int de0207E[] = { 0x00029, 0x00000 };
static unsigned int de0207F[] = { 0x0006E, 0x00000 };
static unsigned int de02080[] = { 0x00030, 0x00000 };
static unsigned int de02081[] = { 0x00031, 0x00000 };
static unsigned int de02082[] = { 0x00032, 0x00000 };
static unsigned int de02083[] = { 0x00033, 0x00000 };
static unsigned int de02084[] = { 0x00034, 0x00000 };
static unsigned int de02085[] = { 0x00035, 0x00000 };
static unsigned int de02086[] = { 0x00036, 0x00000 };
static unsigned int de02087[] = { 0x00037, 0x00000 };
static unsigned int de02088[] = { 0x00038, 0x00000 };
static unsigned int de02089[] = { 0x00039, 0x00000 };
static unsigned int de0208A[] = { 0x0002B, 0x00000 };
static unsigned int de0208B[] = { 0x02212, 0x00000 };
static unsigned int de0208C[] = { 0x0003D, 0x00000 };
static unsigned int de0208D[] = { 0x00028, 0x00000 };
static unsigned int de0208E[] = { 0x00029, 0x00000 };
static unsigned int de02090[] = { 0x00061, 0x00000 };
static unsigned int de02091[] = { 0x00065, 0x00000 };
static unsigned int de02092[] = { 0x0006F, 0x00000 };
static unsigned int de02093[] = { 0x00078, 0x00000 };
static unsigned int de02094[] = { 0x00259, 0x00000 };
static unsigned int de020A8[] = { 0x00052, 0x00073, 0x00000 };
static unsigned int de02100[] = { 0x00061, 0x0002F, 0x00063, 0x00000 };
static unsigned int de02101[] = { 0x00061, 0x0002F, 0x00073, 0x00000 };
static unsigned int de02102[] = { 0x00043, 0x00000 };
static unsigned int de02103[] = { 0x000B0, 0x00043, 0x00000 };
static unsigned int de02105[] = { 0x00063, 0x0002F, 0x0006F, 0x00000 };
static unsigned int de02106[] = { 0x00063, 0x0002F, 0x00075, 0x00000 };
static unsigned int de02107[] = { 0x00190, 0x00000 };
static unsigned int de02109[] = { 0x000B0, 0x00046, 0x00000 };
static unsigned int de0210A[] = { 0x00067, 0x00000 };
static unsigned int de0210B[] = { 0x00048, 0x00000 };
static unsigned int de0210C[] = { 0x00048, 0x00000 };
static unsigned int de0210D[] = { 0x00048, 0x00000 };
static unsigned int de0210E[] = { 0x00068, 0x00000 };
static unsigned int de0210F[] = { 0x00127, 0x00000 };
static unsigned int de02110[] = { 0x00049, 0x00000 };
static unsigned int de02111[] = { 0x00049, 0x00000 };
static unsigned int de02112[] = { 0x0004C, 0x00000 };
static unsigned int de02113[] = { 0x0006C, 0x00000 };
static unsigned int de02115[] = { 0x0004E, 0x00000 };
static unsigned int de02116[] = { 0x0004E, 0x0006F, 0x00000 };
static unsigned int de02119[] = { 0x00050, 0x00000 };
static unsigned int de0211A[] = { 0x00051, 0x00000 };
static unsigned int de0211B[] = { 0x00052, 0x00000 };
static unsigned int de0211C[] = { 0x00052, 0x00000 };
static unsigned int de0211D[] = { 0x00052, 0x00000 };
static unsigned int de02120[] = { 0x00053, 0x0004D, 0x00000 };
static unsigned int de02121[] = { 0x00054, 0x00045, 0x0004C, 0x00000 };
static unsigned int de02122[] = { 0x00054, 0x0004D, 0x00000 };
static unsigned int de02124[] = { 0x0005A, 0x00000 };
static unsigned int de02126[] = { 0x003A9, 0x00000 };
static unsigned int de02128[] = { 0x0005A, 0x00000 };
static unsigned int de0212A[] = { 0x0004B, 0x00000 };
static unsigned int de0212B[] = { 0x000C5, 0x00000 };
static unsigned int de0212C[] = { 0x00042, 0x00000 };
static unsigned int de0212D[] = { 0x00043, 0x00000 };
static unsigned int de0212F[] = { 0x00065, 0x00000 };
static unsigned int de02130[] = { 0x00045, 0x00000 };
static unsigned int de02131[] = { 0x00046, 0x00000 };
static unsigned int de02133[] = { 0x0004D, 0x00000 };
static unsigned int de02134[] = { 0x0006F, 0x00000 };
static unsigned int de02135[] = { 0x005D0, 0x00000 };
static unsigned int de02136[] = { 0x005D1, 0x00000 };
static unsigned int de02137[] = { 0x005D2, 0x00000 };
static unsigned int de02138[] = { 0x005D3, 0x00000 };
static unsigned int de02139[] = { 0x00069, 0x00000 };
static unsigned int de0213B[] = { 0x00046, 0x00041, 0x00058, 0x00000 };
static unsigned int de0213C[] = { 0x003C0, 0x00000 };
static unsigned int de0213D[] = { 0x003B3, 0x00000 };
static unsigned int de0213E[] = { 0x00393, 0x00000 };
static unsigned int de0213F[] = { 0x003A0, 0x00000 };
static unsigned int de02140[] = { 0x02211, 0x00000 };
static unsigned int de02145[] = { 0x00044, 0x00000 };
static unsigned int de02146[] = { 0x00064, 0x00000 };
static unsigned int de02147[] = { 0x00065, 0x00000 };
static unsigned int de02148[] = { 0x00069, 0x00000 };
static unsigned int de02149[] = { 0x0006A, 0x00000 };
static unsigned int de02153[] = { 0x00031, 0x02044, 0x00033, 0x00000 };
static unsigned int de02154[] = { 0x00032, 0x02044, 0x00033, 0x00000 };
static unsigned int de02155[] = { 0x00031, 0x02044, 0x00035, 0x00000 };
static unsigned int de02156[] = { 0x00032, 0x02044, 0x00035, 0x00000 };
static unsigned int de02157[] = { 0x00033, 0x02044, 0x00035, 0x00000 };
static unsigned int de02158[] = { 0x00034, 0x02044, 0x00035, 0x00000 };
static unsigned int de02159[] = { 0x00031, 0x02044, 0x00036, 0x00000 };
static unsigned int de0215A[] = { 0x00035, 0x02044, 0x00036, 0x00000 };
static unsigned int de0215B[] = { 0x00031, 0x02044, 0x00038, 0x00000 };
static unsigned int de0215C[] = { 0x00033, 0x02044, 0x00038, 0x00000 };
static unsigned int de0215D[] = { 0x00035, 0x02044, 0x00038, 0x00000 };
static unsigned int de0215E[] = { 0x00037, 0x02044, 0x00038, 0x00000 };
static unsigned int de0215F[] = { 0x00031, 0x02044, 0x00000 };
static unsigned int de02160[] = { 0x00049, 0x00000 };
static unsigned int de02161[] = { 0x00049, 0x00049, 0x00000 };
static unsigned int de02162[] = { 0x00049, 0x00049, 0x00049, 0x00000 };
static unsigned int de02163[] = { 0x00049, 0x00056, 0x00000 };
static unsigned int de02164[] = { 0x00056, 0x00000 };
static unsigned int de02165[] = { 0x00056, 0x00049, 0x00000 };
static unsigned int de02166[] = { 0x00056, 0x00049, 0x00049, 0x00000 };
static unsigned int de02167[] = { 0x00056, 0x00049, 0x00049, 0x00049, 0x00000 };
static unsigned int de02168[] = { 0x00049, 0x00058, 0x00000 };
static unsigned int de02169[] = { 0x00058, 0x00000 };
static unsigned int de0216A[] = { 0x00058, 0x00049, 0x00000 };
static unsigned int de0216B[] = { 0x00058, 0x00049, 0x00049, 0x00000 };
static unsigned int de0216C[] = { 0x0004C, 0x00000 };
static unsigned int de0216D[] = { 0x00043, 0x00000 };
static unsigned int de0216E[] = { 0x00044, 0x00000 };
static unsigned int de0216F[] = { 0x0004D, 0x00000 };
static unsigned int de02170[] = { 0x00069, 0x00000 };
static unsigned int de02171[] = { 0x00069, 0x00069, 0x00000 };
static unsigned int de02172[] = { 0x00069, 0x00069, 0x00069, 0x00000 };
static unsigned int de02173[] = { 0x00069, 0x00076, 0x00000 };
static unsigned int de02174[] = { 0x00076, 0x00000 };
static unsigned int de02175[] = { 0x00076, 0x00069, 0x00000 };
static unsigned int de02176[] = { 0x00076, 0x00069, 0x00069, 0x00000 };
static unsigned int de02177[] = { 0x00076, 0x00069, 0x00069, 0x00069, 0x00000 };
static unsigned int de02178[] = { 0x00069, 0x00078, 0x00000 };
static unsigned int de02179[] = { 0x00078, 0x00000 };
static unsigned int de0217A[] = { 0x00078, 0x00069, 0x00000 };
static unsigned int de0217B[] = { 0x00078, 0x00069, 0x00069, 0x00000 };
static unsigned int de0217C[] = { 0x0006C, 0x00000 };
static unsigned int de0217D[] = { 0x00063, 0x00000 };
static unsigned int de0217E[] = { 0x00064, 0x00000 };
static unsigned int de0217F[] = { 0x0006D, 0x00000 };
static unsigned int de0219A[] = { 0x02190, 0x00338, 0x00000 };
static unsigned int de0219B[] = { 0x02192, 0x00338, 0x00000 };
static unsigned int de021AE[] = { 0x02194, 0x00338, 0x00000 };
static unsigned int de021CD[] = { 0x021D0, 0x00338, 0x00000 };
static unsigned int de021CE[] = { 0x021D4, 0x00338, 0x00000 };
static unsigned int de021CF[] = { 0x021D2, 0x00338, 0x00000 };
static unsigned int de02204[] = { 0x02203, 0x00338, 0x00000 };
static unsigned int de02209[] = { 0x02208, 0x00338, 0x00000 };
static unsigned int de0220C[] = { 0x0220B, 0x00338, 0x00000 };
static unsigned int de02224[] = { 0x02223, 0x00338, 0x00000 };
static unsigned int de02226[] = { 0x02225, 0x00338, 0x00000 };
static unsigned int de0222C[] = { 0x0222B, 0x0222B, 0x00000 };
static unsigned int de0222D[] = { 0x0222B, 0x0222B, 0x0222B, 0x00000 };
static unsigned int de0222F[] = { 0x0222E, 0x0222E, 0x00000 };
static unsigned int de02230[] = { 0x0222E, 0x0222E, 0x0222E, 0x00000 };
static unsigned int de02241[] = { 0x0223C, 0x00338, 0x00000 };
static unsigned int de02244[] = { 0x02243, 0x00338, 0x00000 };
static unsigned int de02247[] = { 0x02245, 0x00338, 0x00000 };
static unsigned int de02249[] = { 0x02248, 0x00338, 0x00000 };
static unsigned int de02260[] = { 0x0003D, 0x00338, 0x00000 };
static unsigned int de02262[] = { 0x02261, 0x00338, 0x00000 };
static unsigned int de0226D[] = { 0x0224D, 0x00338, 0x00000 };
static unsigned int de0226E[] = { 0x0003C, 0x00338, 0x00000 };
static unsigned int de0226F[] = { 0x0003E, 0x00338, 0x00000 };
static unsigned int de02270[] = { 0x02264, 0x00338, 0x00000 };
static unsigned int de02271[] = { 0x02265, 0x00338, 0x00000 };
static unsigned int de02274[] = { 0x02272, 0x00338, 0x00000 };
static unsigned int de02275[] = { 0x02273, 0x00338, 0x00000 };
static unsigned int de02278[] = { 0x02276, 0x00338, 0x00000 };
static unsigned int de02279[] = { 0x02277, 0x00338, 0x00000 };
static unsigned int de02280[] = { 0x0227A, 0x00338, 0x00000 };
static unsigned int de02281[] = { 0x0227B, 0x00338, 0x00000 };
static unsigned int de02284[] = { 0x02282, 0x00338, 0x00000 };
static unsigned int de02285[] = { 0x02283, 0x00338, 0x00000 };
static unsigned int de02288[] = { 0x02286, 0x00338, 0x00000 };
static unsigned int de02289[] = { 0x02287, 0x00338, 0x00000 };
static unsigned int de022AC[] = { 0x022A2, 0x00338, 0x00000 };
static unsigned int de022AD[] = { 0x022A8, 0x00338, 0x00000 };
static unsigned int de022AE[] = { 0x022A9, 0x00338, 0x00000 };
static unsigned int de022AF[] = { 0x022AB, 0x00338, 0x00000 };
static unsigned int de022E0[] = { 0x0227C, 0x00338, 0x00000 };
static unsigned int de022E1[] = { 0x0227D, 0x00338, 0x00000 };
static unsigned int de022E2[] = { 0x02291, 0x00338, 0x00000 };
static unsigned int de022E3[] = { 0x02292, 0x00338, 0x00000 };
static unsigned int de022EA[] = { 0x022B2, 0x00338, 0x00000 };
static unsigned int de022EB[] = { 0x022B3, 0x00338, 0x00000 };
static unsigned int de022EC[] = { 0x022B4, 0x00338, 0x00000 };
static unsigned int de022ED[] = { 0x022B5, 0x00338, 0x00000 };
static unsigned int de02329[] = { 0x03008, 0x00000 };
static unsigned int de0232A[] = { 0x03009, 0x00000 };
static unsigned int de02460[] = { 0x00031, 0x00000 };
static unsigned int de02461[] = { 0x00032, 0x00000 };
static unsigned int de02462[] = { 0x00033, 0x00000 };
static unsigned int de02463[] = { 0x00034, 0x00000 };
static unsigned int de02464[] = { 0x00035, 0x00000 };
static unsigned int de02465[] = { 0x00036, 0x00000 };
static unsigned int de02466[] = { 0x00037, 0x00000 };
static unsigned int de02467[] = { 0x00038, 0x00000 };
static unsigned int de02468[] = { 0x00039, 0x00000 };
static unsigned int de02469[] = { 0x00031, 0x00030, 0x00000 };
static unsigned int de0246A[] = { 0x00031, 0x00031, 0x00000 };
static unsigned int de0246B[] = { 0x00031, 0x00032, 0x00000 };
static unsigned int de0246C[] = { 0x00031, 0x00033, 0x00000 };
static unsigned int de0246D[] = { 0x00031, 0x00034, 0x00000 };
static unsigned int de0246E[] = { 0x00031, 0x00035, 0x00000 };
static unsigned int de0246F[] = { 0x00031, 0x00036, 0x00000 };
static unsigned int de02470[] = { 0x00031, 0x00037, 0x00000 };
static unsigned int de02471[] = { 0x00031, 0x00038, 0x00000 };
static unsigned int de02472[] = { 0x00031, 0x00039, 0x00000 };
static unsigned int de02473[] = { 0x00032, 0x00030, 0x00000 };
static unsigned int de02474[] = { 0x00028, 0x00031, 0x00029, 0x00000 };
static unsigned int de02475[] = { 0x00028, 0x00032, 0x00029, 0x00000 };
static unsigned int de02476[] = { 0x00028, 0x00033, 0x00029, 0x00000 };
static unsigned int de02477[] = { 0x00028, 0x00034, 0x00029, 0x00000 };
static unsigned int de02478[] = { 0x00028, 0x00035, 0x00029, 0x00000 };
static unsigned int de02479[] = { 0x00028, 0x00036, 0x00029, 0x00000 };
static unsigned int de0247A[] = { 0x00028, 0x00037, 0x00029, 0x00000 };
static unsigned int de0247B[] = { 0x00028, 0x00038, 0x00029, 0x00000 };
static unsigned int de0247C[] = { 0x00028, 0x00039, 0x00029, 0x00000 };
static unsigned int de0247D[] = { 0x00028, 0x00031, 0x00030, 0x00029, 0x00000 };
static unsigned int de0247E[] = { 0x00028, 0x00031, 0x00031, 0x00029, 0x00000 };
static unsigned int de0247F[] = { 0x00028, 0x00031, 0x00032, 0x00029, 0x00000 };
static unsigned int de02480[] = { 0x00028, 0x00031, 0x00033, 0x00029, 0x00000 };
static unsigned int de02481[] = { 0x00028, 0x00031, 0x00034, 0x00029, 0x00000 };
static unsigned int de02482[] = { 0x00028, 0x00031, 0x00035, 0x00029, 0x00000 };
static unsigned int de02483[] = { 0x00028, 0x00031, 0x00036, 0x00029, 0x00000 };
static unsigned int de02484[] = { 0x00028, 0x00031, 0x00037, 0x00029, 0x00000 };
static unsigned int de02485[] = { 0x00028, 0x00031, 0x00038, 0x00029, 0x00000 };
static unsigned int de02486[] = { 0x00028, 0x00031, 0x00039, 0x00029, 0x00000 };
static unsigned int de02487[] = { 0x00028, 0x00032, 0x00030, 0x00029, 0x00000 };
static unsigned int de02488[] = { 0x00031, 0x0002E, 0x00000 };
static unsigned int de02489[] = { 0x00032, 0x0002E, 0x00000 };
static unsigned int de0248A[] = { 0x00033, 0x0002E, 0x00000 };
static unsigned int de0248B[] = { 0x00034, 0x0002E, 0x00000 };
static unsigned int de0248C[] = { 0x00035, 0x0002E, 0x00000 };
static unsigned int de0248D[] = { 0x00036, 0x0002E, 0x00000 };
static unsigned int de0248E[] = { 0x00037, 0x0002E, 0x00000 };
static unsigned int de0248F[] = { 0x00038, 0x0002E, 0x00000 };
static unsigned int de02490[] = { 0x00039, 0x0002E, 0x00000 };
static unsigned int de02491[] = { 0x00031, 0x00030, 0x0002E, 0x00000 };
static unsigned int de02492[] = { 0x00031, 0x00031, 0x0002E, 0x00000 };
static unsigned int de02493[] = { 0x00031, 0x00032, 0x0002E, 0x00000 };
static unsigned int de02494[] = { 0x00031, 0x00033, 0x0002E, 0x00000 };
static unsigned int de02495[] = { 0x00031, 0x00034, 0x0002E, 0x00000 };
static unsigned int de02496[] = { 0x00031, 0x00035, 0x0002E, 0x00000 };
static unsigned int de02497[] = { 0x00031, 0x00036, 0x0002E, 0x00000 };
static unsigned int de02498[] = { 0x00031, 0x00037, 0x0002E, 0x00000 };
static unsigned int de02499[] = { 0x00031, 0x00038, 0x0002E, 0x00000 };
static unsigned int de0249A[] = { 0x00031, 0x00039, 0x0002E, 0x00000 };
static unsigned int de0249B[] = { 0x00032, 0x00030, 0x0002E, 0x00000 };
static unsigned int de0249C[] = { 0x00028, 0x00061, 0x00029, 0x00000 };
static unsigned int de0249D[] = { 0x00028, 0x00062, 0x00029, 0x00000 };
static unsigned int de0249E[] = { 0x00028, 0x00063, 0x00029, 0x00000 };
static unsigned int de0249F[] = { 0x00028, 0x00064, 0x00029, 0x00000 };
static unsigned int de024A0[] = { 0x00028, 0x00065, 0x00029, 0x00000 };
static unsigned int de024A1[] = { 0x00028, 0x00066, 0x00029, 0x00000 };
static unsigned int de024A2[] = { 0x00028, 0x00067, 0x00029, 0x00000 };
static unsigned int de024A3[] = { 0x00028, 0x00068, 0x00029, 0x00000 };
static unsigned int de024A4[] = { 0x00028, 0x00069, 0x00029, 0x00000 };
static unsigned int de024A5[] = { 0x00028, 0x0006A, 0x00029, 0x00000 };
static unsigned int de024A6[] = { 0x00028, 0x0006B, 0x00029, 0x00000 };
static unsigned int de024A7[] = { 0x00028, 0x0006C, 0x00029, 0x00000 };
static unsigned int de024A8[] = { 0x00028, 0x0006D, 0x00029, 0x00000 };
static unsigned int de024A9[] = { 0x00028, 0x0006E, 0x00029, 0x00000 };
static unsigned int de024AA[] = { 0x00028, 0x0006F, 0x00029, 0x00000 };
static unsigned int de024AB[] = { 0x00028, 0x00070, 0x00029, 0x00000 };
static unsigned int de024AC[] = { 0x00028, 0x00071, 0x00029, 0x00000 };
static unsigned int de024AD[] = { 0x00028, 0x00072, 0x00029, 0x00000 };
static unsigned int de024AE[] = { 0x00028, 0x00073, 0x00029, 0x00000 };
static unsigned int de024AF[] = { 0x00028, 0x00074, 0x00029, 0x00000 };
static unsigned int de024B0[] = { 0x00028, 0x00075, 0x00029, 0x00000 };
static unsigned int de024B1[] = { 0x00028, 0x00076, 0x00029, 0x00000 };
static unsigned int de024B2[] = { 0x00028, 0x00077, 0x00029, 0x00000 };
static unsigned int de024B3[] = { 0x00028, 0x00078, 0x00029, 0x00000 };
static unsigned int de024B4[] = { 0x00028, 0x00079, 0x00029, 0x00000 };
static unsigned int de024B5[] = { 0x00028, 0x0007A, 0x00029, 0x00000 };
static unsigned int de024B6[] = { 0x00041, 0x00000 };
static unsigned int de024B7[] = { 0x00042, 0x00000 };
static unsigned int de024B8[] = { 0x00043, 0x00000 };
static unsigned int de024B9[] = { 0x00044, 0x00000 };
static unsigned int de024BA[] = { 0x00045, 0x00000 };
static unsigned int de024BB[] = { 0x00046, 0x00000 };
static unsigned int de024BC[] = { 0x00047, 0x00000 };
static unsigned int de024BD[] = { 0x00048, 0x00000 };
static unsigned int de024BE[] = { 0x00049, 0x00000 };
static unsigned int de024BF[] = { 0x0004A, 0x00000 };
static unsigned int de024C0[] = { 0x0004B, 0x00000 };
static unsigned int de024C1[] = { 0x0004C, 0x00000 };
static unsigned int de024C2[] = { 0x0004D, 0x00000 };
static unsigned int de024C3[] = { 0x0004E, 0x00000 };
static unsigned int de024C4[] = { 0x0004F, 0x00000 };
static unsigned int de024C5[] = { 0x00050, 0x00000 };
static unsigned int de024C6[] = { 0x00051, 0x00000 };
static unsigned int de024C7[] = { 0x00052, 0x00000 };
static unsigned int de024C8[] = { 0x00053, 0x00000 };
static unsigned int de024C9[] = { 0x00054, 0x00000 };
static unsigned int de024CA[] = { 0x00055, 0x00000 };
static unsigned int de024CB[] = { 0x00056, 0x00000 };
static unsigned int de024CC[] = { 0x00057, 0x00000 };
static unsigned int de024CD[] = { 0x00058, 0x00000 };
static unsigned int de024CE[] = { 0x00059, 0x00000 };
static unsigned int de024CF[] = { 0x0005A, 0x00000 };
static unsigned int de024D0[] = { 0x00061, 0x00000 };
static unsigned int de024D1[] = { 0x00062, 0x00000 };
static unsigned int de024D2[] = { 0x00063, 0x00000 };
static unsigned int de024D3[] = { 0x00064, 0x00000 };
static unsigned int de024D4[] = { 0x00065, 0x00000 };
static unsigned int de024D5[] = { 0x00066, 0x00000 };
static unsigned int de024D6[] = { 0x00067, 0x00000 };
static unsigned int de024D7[] = { 0x00068, 0x00000 };
static unsigned int de024D8[] = { 0x00069, 0x00000 };
static unsigned int de024D9[] = { 0x0006A, 0x00000 };
static unsigned int de024DA[] = { 0x0006B, 0x00000 };
static unsigned int de024DB[] = { 0x0006C, 0x00000 };
static unsigned int de024DC[] = { 0x0006D, 0x00000 };
static unsigned int de024DD[] = { 0x0006E, 0x00000 };
static unsigned int de024DE[] = { 0x0006F, 0x00000 };
static unsigned int de024DF[] = { 0x00070, 0x00000 };
static unsigned int de024E0[] = { 0x00071, 0x00000 };
static unsigned int de024E1[] = { 0x00072, 0x00000 };
static unsigned int de024E2[] = { 0x00073, 0x00000 };
static unsigned int de024E3[] = { 0x00074, 0x00000 };
static unsigned int de024E4[] = { 0x00075, 0x00000 };
static unsigned int de024E5[] = { 0x00076, 0x00000 };
static unsigned int de024E6[] = { 0x00077, 0x00000 };
static unsigned int de024E7[] = { 0x00078, 0x00000 };
static unsigned int de024E8[] = { 0x00079, 0x00000 };
static unsigned int de024E9[] = { 0x0007A, 0x00000 };
static unsigned int de024EA[] = { 0x00030, 0x00000 };
static unsigned int de02A0C[] = { 0x0222B, 0x0222B, 0x0222B, 0x0222B, 0x00000 };
static unsigned int de02A74[] = { 0x0003A, 0x0003A, 0x0003D, 0x00000 };
static unsigned int de02A75[] = { 0x0003D, 0x0003D, 0x00000 };
static unsigned int de02A76[] = { 0x0003D, 0x0003D, 0x0003D, 0x00000 };
static unsigned int de02ADC[] = { 0x02ADD, 0x00338, 0x00000 };
static unsigned int de02D6F[] = { 0x02D61, 0x00000 };
static unsigned int de02E9F[] = { 0x06BCD, 0x00000 };
static unsigned int de02EF3[] = { 0x09F9F, 0x00000 };
static unsigned int de02F00[] = { 0x04E00, 0x00000 };
static unsigned int de02F01[] = { 0x04E28, 0x00000 };
static unsigned int de02F02[] = { 0x04E36, 0x00000 };
static unsigned int de02F03[] = { 0x04E3F, 0x00000 };
static unsigned int de02F04[] = { 0x04E59, 0x00000 };
static unsigned int de02F05[] = { 0x04E85, 0x00000 };
static unsigned int de02F06[] = { 0x04E8C, 0x00000 };
static unsigned int de02F07[] = { 0x04EA0, 0x00000 };
static unsigned int de02F08[] = { 0x04EBA, 0x00000 };
static unsigned int de02F09[] = { 0x0513F, 0x00000 };
static unsigned int de02F0A[] = { 0x05165, 0x00000 };
static unsigned int de02F0B[] = { 0x0516B, 0x00000 };
static unsigned int de02F0C[] = { 0x05182, 0x00000 };
static unsigned int de02F0D[] = { 0x05196, 0x00000 };
static unsigned int de02F0E[] = { 0x051AB, 0x00000 };
static unsigned int de02F0F[] = { 0x051E0, 0x00000 };
static unsigned int de02F10[] = { 0x051F5, 0x00000 };
static unsigned int de02F11[] = { 0x05200, 0x00000 };
static unsigned int de02F12[] = { 0x0529B, 0x00000 };
static unsigned int de02F13[] = { 0x052F9, 0x00000 };
static unsigned int de02F14[] = { 0x05315, 0x00000 };
static unsigned int de02F15[] = { 0x0531A, 0x00000 };
static unsigned int de02F16[] = { 0x05338, 0x00000 };
static unsigned int de02F17[] = { 0x05341, 0x00000 };
static unsigned int de02F18[] = { 0x0535C, 0x00000 };
static unsigned int de02F19[] = { 0x05369, 0x00000 };
static unsigned int de02F1A[] = { 0x05382, 0x00000 };
static unsigned int de02F1B[] = { 0x053B6, 0x00000 };
static unsigned int de02F1C[] = { 0x053C8, 0x00000 };
static unsigned int de02F1D[] = { 0x053E3, 0x00000 };
static unsigned int de02F1E[] = { 0x056D7, 0x00000 };
static unsigned int de02F1F[] = { 0x0571F, 0x00000 };
static unsigned int de02F20[] = { 0x058EB, 0x00000 };
static unsigned int de02F21[] = { 0x05902, 0x00000 };
static unsigned int de02F22[] = { 0x0590A, 0x00000 };
static unsigned int de02F23[] = { 0x05915, 0x00000 };
static unsigned int de02F24[] = { 0x05927, 0x00000 };
static unsigned int de02F25[] = { 0x05973, 0x00000 };
static unsigned int de02F26[] = { 0x05B50, 0x00000 };
static unsigned int de02F27[] = { 0x05B80, 0x00000 };
static unsigned int de02F28[] = { 0x05BF8, 0x00000 };
static unsigned int de02F29[] = { 0x05C0F, 0x00000 };
static unsigned int de02F2A[] = { 0x05C22, 0x00000 };
static unsigned int de02F2B[] = { 0x05C38, 0x00000 };
static unsigned int de02F2C[] = { 0x05C6E, 0x00000 };
static unsigned int de02F2D[] = { 0x05C71, 0x00000 };
static unsigned int de02F2E[] = { 0x05DDB, 0x00000 };
static unsigned int de02F2F[] = { 0x05DE5, 0x00000 };
static unsigned int de02F30[] = { 0x05DF1, 0x00000 };
static unsigned int de02F31[] = { 0x05DFE, 0x00000 };
static unsigned int de02F32[] = { 0x05E72, 0x00000 };
static unsigned int de02F33[] = { 0x05E7A, 0x00000 };
static unsigned int de02F34[] = { 0x05E7F, 0x00000 };
static unsigned int de02F35[] = { 0x05EF4, 0x00000 };
static unsigned int de02F36[] = { 0x05EFE, 0x00000 };
static unsigned int de02F37[] = { 0x05F0B, 0x00000 };
static unsigned int de02F38[] = { 0x05F13, 0x00000 };
static unsigned int de02F39[] = { 0x05F50, 0x00000 };
static unsigned int de02F3A[] = { 0x05F61, 0x00000 };
static unsigned int de02F3B[] = { 0x05F73, 0x00000 };
static unsigned int de02F3C[] = { 0x05FC3, 0x00000 };
static unsigned int de02F3D[] = { 0x06208, 0x00000 };
static unsigned int de02F3E[] = { 0x06236, 0x00000 };
static unsigned int de02F3F[] = { 0x0624B, 0x00000 };
static unsigned int de02F40[] = { 0x0652F, 0x00000 };
static unsigned int de02F41[] = { 0x06534, 0x00000 };
static unsigned int de02F42[] = { 0x06587, 0x00000 };
static unsigned int de02F43[] = { 0x06597, 0x00000 };
static unsigned int de02F44[] = { 0x065A4, 0x00000 };
static unsigned int de02F45[] = { 0x065B9, 0x00000 };
static unsigned int de02F46[] = { 0x065E0, 0x00000 };
static unsigned int de02F47[] = { 0x065E5, 0x00000 };
static unsigned int de02F48[] = { 0x066F0, 0x00000 };
static unsigned int de02F49[] = { 0x06708, 0x00000 };
static unsigned int de02F4A[] = { 0x06728, 0x00000 };
static unsigned int de02F4B[] = { 0x06B20, 0x00000 };
static unsigned int de02F4C[] = { 0x06B62, 0x00000 };
static unsigned int de02F4D[] = { 0x06B79, 0x00000 };
static unsigned int de02F4E[] = { 0x06BB3, 0x00000 };
static unsigned int de02F4F[] = { 0x06BCB, 0x00000 };
static unsigned int de02F50[] = { 0x06BD4, 0x00000 };
static unsigned int de02F51[] = { 0x06BDB, 0x00000 };
static unsigned int de02F52[] = { 0x06C0F, 0x00000 };
static unsigned int de02F53[] = { 0x06C14, 0x00000 };
static unsigned int de02F54[] = { 0x06C34, 0x00000 };
static unsigned int de02F55[] = { 0x0706B, 0x00000 };
static unsigned int de02F56[] = { 0x0722A, 0x00000 };
static unsigned int de02F57[] = { 0x07236, 0x00000 };
static unsigned int de02F58[] = { 0x0723B, 0x00000 };
static unsigned int de02F59[] = { 0x0723F, 0x00000 };
static unsigned int de02F5A[] = { 0x07247, 0x00000 };
static unsigned int de02F5B[] = { 0x07259, 0x00000 };
static unsigned int de02F5C[] = { 0x0725B, 0x00000 };
static unsigned int de02F5D[] = { 0x072AC, 0x00000 };
static unsigned int de02F5E[] = { 0x07384, 0x00000 };
static unsigned int de02F5F[] = { 0x07389, 0x00000 };
static unsigned int de02F60[] = { 0x074DC, 0x00000 };
static unsigned int de02F61[] = { 0x074E6, 0x00000 };
static unsigned int de02F62[] = { 0x07518, 0x00000 };
static unsigned int de02F63[] = { 0x0751F, 0x00000 };
static unsigned int de02F64[] = { 0x07528, 0x00000 };
static unsigned int de02F65[] = { 0x07530, 0x00000 };
static unsigned int de02F66[] = { 0x0758B, 0x00000 };
static unsigned int de02F67[] = { 0x07592, 0x00000 };
static unsigned int de02F68[] = { 0x07676, 0x00000 };
static unsigned int de02F69[] = { 0x0767D, 0x00000 };
static unsigned int de02F6A[] = { 0x076AE, 0x00000 };
static unsigned int de02F6B[] = { 0x076BF, 0x00000 };
static unsigned int de02F6C[] = { 0x076EE, 0x00000 };
static unsigned int de02F6D[] = { 0x077DB, 0x00000 };
static unsigned int de02F6E[] = { 0x077E2, 0x00000 };
static unsigned int de02F6F[] = { 0x077F3, 0x00000 };
static unsigned int de02F70[] = { 0x0793A, 0x00000 };
static unsigned int de02F71[] = { 0x079B8, 0x00000 };
static unsigned int de02F72[] = { 0x079BE, 0x00000 };
static unsigned int de02F73[] = { 0x07A74, 0x00000 };
static unsigned int de02F74[] = { 0x07ACB, 0x00000 };
static unsigned int de02F75[] = { 0x07AF9, 0x00000 };
static unsigned int de02F76[] = { 0x07C73, 0x00000 };
static unsigned int de02F77[] = { 0x07CF8, 0x00000 };
static unsigned int de02F78[] = { 0x07F36, 0x00000 };
static unsigned int de02F79[] = { 0x07F51, 0x00000 };
static unsigned int de02F7A[] = { 0x07F8A, 0x00000 };
static unsigned int de02F7B[] = { 0x07FBD, 0x00000 };
static unsigned int de02F7C[] = { 0x08001, 0x00000 };
static unsigned int de02F7D[] = { 0x0800C, 0x00000 };
static unsigned int de02F7E[] = { 0x08012, 0x00000 };
static unsigned int de02F7F[] = { 0x08033, 0x00000 };
static unsigned int de02F80[] = { 0x0807F, 0x00000 };
static unsigned int de02F81[] = { 0x08089, 0x00000 };
static unsigned int de02F82[] = { 0x081E3, 0x00000 };
static unsigned int de02F83[] = { 0x081EA, 0x00000 };
static unsigned int de02F84[] = { 0x081F3, 0x00000 };
static unsigned int de02F85[] = { 0x081FC, 0x00000 };
static unsigned int de02F86[] = { 0x0820C, 0x00000 };
static unsigned int de02F87[] = { 0x0821B, 0x00000 };
static unsigned int de02F88[] = { 0x0821F, 0x00000 };
static unsigned int de02F89[] = { 0x0826E, 0x00000 };
static unsigned int de02F8A[] = { 0x08272, 0x00000 };
static unsigned int de02F8B[] = { 0x08278, 0x00000 };
static unsigned int de02F8C[] = { 0x0864D, 0x00000 };
static unsigned int de02F8D[] = { 0x0866B, 0x00000 };
static unsigned int de02F8E[] = { 0x08840, 0x00000 };
static unsigned int de02F8F[] = { 0x0884C, 0x00000 };
static unsigned int de02F90[] = { 0x08863, 0x00000 };
static unsigned int de02F91[] = { 0x0897E, 0x00000 };
static unsigned int de02F92[] = { 0x0898B, 0x00000 };
static unsigned int de02F93[] = { 0x089D2, 0x00000 };
static unsigned int de02F94[] = { 0x08A00, 0x00000 };
static unsigned int de02F95[] = { 0x08C37, 0x00000 };
static unsigned int de02F96[] = { 0x08C46, 0x00000 };
static unsigned int de02F97[] = { 0x08C55, 0x00000 };
static unsigned int de02F98[] = { 0x08C78, 0x00000 };
static unsigned int de02F99[] = { 0x08C9D, 0x00000 };
static unsigned int de02F9A[] = { 0x08D64, 0x00000 };
static unsigned int de02F9B[] = { 0x08D70, 0x00000 };
static unsigned int de02F9C[] = { 0x08DB3, 0x00000 };
static unsigned int de02F9D[] = { 0x08EAB, 0x00000 };
static unsigned int de02F9E[] = { 0x08ECA, 0x00000 };
static unsigned int de02F9F[] = { 0x08F9B, 0x00000 };
static unsigned int de02FA0[] = { 0x08FB0, 0x00000 };
static unsigned int de02FA1[] = { 0x08FB5, 0x00000 };
static unsigned int de02FA2[] = { 0x09091, 0x00000 };
static unsigned int de02FA3[] = { 0x09149, 0x00000 };
static unsigned int de02FA4[] = { 0x091C6, 0x00000 };
static unsigned int de02FA5[] = { 0x091CC, 0x00000 };
static unsigned int de02FA6[] = { 0x091D1, 0x00000 };
static unsigned int de02FA7[] = { 0x09577, 0x00000 };
static unsigned int de02FA8[] = { 0x09580, 0x00000 };
static unsigned int de02FA9[] = { 0x0961C, 0x00000 };
static unsigned int de02FAA[] = { 0x096B6, 0x00000 };
static unsigned int de02FAB[] = { 0x096B9, 0x00000 };
static unsigned int de02FAC[] = { 0x096E8, 0x00000 };
static unsigned int de02FAD[] = { 0x09751, 0x00000 };
static unsigned int de02FAE[] = { 0x0975E, 0x00000 };
static unsigned int de02FAF[] = { 0x09762, 0x00000 };
static unsigned int de02FB0[] = { 0x09769, 0x00000 };
static unsigned int de02FB1[] = { 0x097CB, 0x00000 };
static unsigned int de02FB2[] = { 0x097ED, 0x00000 };
static unsigned int de02FB3[] = { 0x097F3, 0x00000 };
static unsigned int de02FB4[] = { 0x09801, 0x00000 };
static unsigned int de02FB5[] = { 0x098A8, 0x00000 };
static unsigned int de02FB6[] = { 0x098DB, 0x00000 };
static unsigned int de02FB7[] = { 0x098DF, 0x00000 };
static unsigned int de02FB8[] = { 0x09996, 0x00000 };
static unsigned int de02FB9[] = { 0x09999, 0x00000 };
static unsigned int de02FBA[] = { 0x099AC, 0x00000 };
static unsigned int de02FBB[] = { 0x09AA8, 0x00000 };
static unsigned int de02FBC[] = { 0x09AD8, 0x00000 };
static unsigned int de02FBD[] = { 0x09ADF, 0x00000 };
static unsigned int de02FBE[] = { 0x09B25, 0x00000 };
static unsigned int de02FBF[] = { 0x09B2F, 0x00000 };
static unsigned int de02FC0[] = { 0x09B32, 0x00000 };
static unsigned int de02FC1[] = { 0x09B3C, 0x00000 };
static unsigned int de02FC2[] = { 0x09B5A, 0x00000 };
static unsigned int de02FC3[] = { 0x09CE5, 0x00000 };
static unsigned int de02FC4[] = { 0x09E75, 0x00000 };
static unsigned int de02FC5[] = { 0x09E7F, 0x00000 };
static unsigned int de02FC6[] = { 0x09EA5, 0x00000 };
static unsigned int de02FC7[] = { 0x09EBB, 0x00000 };
static unsigned int de02FC8[] = { 0x09EC3, 0x00000 };
static unsigned int de02FC9[] = { 0x09ECD, 0x00000 };
static unsigned int de02FCA[] = { 0x09ED1, 0x00000 };
static unsigned int de02FCB[] = { 0x09EF9, 0x00000 };
static unsigned int de02FCC[] = { 0x09EFD, 0x00000 };
static unsigned int de02FCD[] = { 0x09F0E, 0x00000 };
static unsigned int de02FCE[] = { 0x09F13, 0x00000 };
static unsigned int de02FCF[] = { 0x09F20, 0x00000 };
static unsigned int de02FD0[] = { 0x09F3B, 0x00000 };
static unsigned int de02FD1[] = { 0x09F4A, 0x00000 };
static unsigned int de02FD2[] = { 0x09F52, 0x00000 };
static unsigned int de02FD3[] = { 0x09F8D, 0x00000 };
static unsigned int de02FD4[] = { 0x09F9C, 0x00000 };
static unsigned int de02FD5[] = { 0x09FA0, 0x00000 };
static unsigned int de03000[] = { 0x00020, 0x00000 };
static unsigned int de03036[] = { 0x03012, 0x00000 };
static unsigned int de03038[] = { 0x05341, 0x00000 };
static unsigned int de03039[] = { 0x05344, 0x00000 };
static unsigned int de0303A[] = { 0x05345, 0x00000 };
static unsigned int de0304C[] = { 0x0304B, 0x03099, 0x00000 };
static unsigned int de0304E[] = { 0x0304D, 0x03099, 0x00000 };
static unsigned int de03050[] = { 0x0304F, 0x03099, 0x00000 };
static unsigned int de03052[] = { 0x03051, 0x03099, 0x00000 };
static unsigned int de03054[] = { 0x03053, 0x03099, 0x00000 };
static unsigned int de03056[] = { 0x03055, 0x03099, 0x00000 };
static unsigned int de03058[] = { 0x03057, 0x03099, 0x00000 };
static unsigned int de0305A[] = { 0x03059, 0x03099, 0x00000 };
static unsigned int de0305C[] = { 0x0305B, 0x03099, 0x00000 };
static unsigned int de0305E[] = { 0x0305D, 0x03099, 0x00000 };
static unsigned int de03060[] = { 0x0305F, 0x03099, 0x00000 };
static unsigned int de03062[] = { 0x03061, 0x03099, 0x00000 };
static unsigned int de03065[] = { 0x03064, 0x03099, 0x00000 };
static unsigned int de03067[] = { 0x03066, 0x03099, 0x00000 };
static unsigned int de03069[] = { 0x03068, 0x03099, 0x00000 };
static unsigned int de03070[] = { 0x0306F, 0x03099, 0x00000 };
static unsigned int de03071[] = { 0x0306F, 0x0309A, 0x00000 };
static unsigned int de03073[] = { 0x03072, 0x03099, 0x00000 };
static unsigned int de03074[] = { 0x03072, 0x0309A, 0x00000 };
static unsigned int de03076[] = { 0x03075, 0x03099, 0x00000 };
static unsigned int de03077[] = { 0x03075, 0x0309A, 0x00000 };
static unsigned int de03079[] = { 0x03078, 0x03099, 0x00000 };
static unsigned int de0307A[] = { 0x03078, 0x0309A, 0x00000 };
static unsigned int de0307C[] = { 0x0307B, 0x03099, 0x00000 };
static unsigned int de0307D[] = { 0x0307B, 0x0309A, 0x00000 };
static unsigned int de03094[] = { 0x03046, 0x03099, 0x00000 };
static unsigned int de0309B[] = { 0x00020, 0x03099, 0x00000 };
static unsigned int de0309C[] = { 0x00020, 0x0309A, 0x00000 };
static unsigned int de0309E[] = { 0x0309D, 0x03099, 0x00000 };
static unsigned int de0309F[] = { 0x03088, 0x0308A, 0x00000 };
static unsigned int de030AC[] = { 0x030AB, 0x03099, 0x00000 };
static unsigned int de030AE[] = { 0x030AD, 0x03099, 0x00000 };
static unsigned int de030B0[] = { 0x030AF, 0x03099, 0x00000 };
static unsigned int de030B2[] = { 0x030B1, 0x03099, 0x00000 };
static unsigned int de030B4[] = { 0x030B3, 0x03099, 0x00000 };
static unsigned int de030B6[] = { 0x030B5, 0x03099, 0x00000 };
static unsigned int de030B8[] = { 0x030B7, 0x03099, 0x00000 };
static unsigned int de030BA[] = { 0x030B9, 0x03099, 0x00000 };
static unsigned int de030BC[] = { 0x030BB, 0x03099, 0x00000 };
static unsigned int de030BE[] = { 0x030BD, 0x03099, 0x00000 };
static unsigned int de030C0[] = { 0x030BF, 0x03099, 0x00000 };
static unsigned int de030C2[] = { 0x030C1, 0x03099, 0x00000 };
static unsigned int de030C5[] = { 0x030C4, 0x03099, 0x00000 };
static unsigned int de030C7[] = { 0x030C6, 0x03099, 0x00000 };
static unsigned int de030C9[] = { 0x030C8, 0x03099, 0x00000 };
static unsigned int de030D0[] = { 0x030CF, 0x03099, 0x00000 };
static unsigned int de030D1[] = { 0x030CF, 0x0309A, 0x00000 };
static unsigned int de030D3[] = { 0x030D2, 0x03099, 0x00000 };
static unsigned int de030D4[] = { 0x030D2, 0x0309A, 0x00000 };
static unsigned int de030D6[] = { 0x030D5, 0x03099, 0x00000 };
static unsigned int de030D7[] = { 0x030D5, 0x0309A, 0x00000 };
static unsigned int de030D9[] = { 0x030D8, 0x03099, 0x00000 };
static unsigned int de030DA[] = { 0x030D8, 0x0309A, 0x00000 };
static unsigned int de030DC[] = { 0x030DB, 0x03099, 0x00000 };
static unsigned int de030DD[] = { 0x030DB, 0x0309A, 0x00000 };
static unsigned int de030F4[] = { 0x030A6, 0x03099, 0x00000 };
static unsigned int de030F7[] = { 0x030EF, 0x03099, 0x00000 };
static unsigned int de030F8[] = { 0x030F0, 0x03099, 0x00000 };
static unsigned int de030F9[] = { 0x030F1, 0x03099, 0x00000 };
static unsigned int de030FA[] = { 0x030F2, 0x03099, 0x00000 };
static unsigned int de030FE[] = { 0x030FD, 0x03099, 0x00000 };
static unsigned int de030FF[] = { 0x030B3, 0x030C8, 0x00000 };
static unsigned int de03131[] = { 0x01100, 0x00000 };
static unsigned int de03132[] = { 0x01101, 0x00000 };
static unsigned int de03133[] = { 0x011AA, 0x00000 };
static unsigned int de03134[] = { 0x01102, 0x00000 };
static unsigned int de03135[] = { 0x011AC, 0x00000 };
static unsigned int de03136[] = { 0x011AD, 0x00000 };
static unsigned int de03137[] = { 0x01103, 0x00000 };
static unsigned int de03138[] = { 0x01104, 0x00000 };
static unsigned int de03139[] = { 0x01105, 0x00000 };
static unsigned int de0313A[] = { 0x011B0, 0x00000 };
static unsigned int de0313B[] = { 0x011B1, 0x00000 };
static unsigned int de0313C[] = { 0x011B2, 0x00000 };
static unsigned int de0313D[] = { 0x011B3, 0x00000 };
static unsigned int de0313E[] = { 0x011B4, 0x00000 };
static unsigned int de0313F[] = { 0x011B5, 0x00000 };
static unsigned int de03140[] = { 0x0111A, 0x00000 };
static unsigned int de03141[] = { 0x01106, 0x00000 };
static unsigned int de03142[] = { 0x01107, 0x00000 };
static unsigned int de03143[] = { 0x01108, 0x00000 };
static unsigned int de03144[] = { 0x01121, 0x00000 };
static unsigned int de03145[] = { 0x01109, 0x00000 };
static unsigned int de03146[] = { 0x0110A, 0x00000 };
static unsigned int de03147[] = { 0x0110B, 0x00000 };
static unsigned int de03148[] = { 0x0110C, 0x00000 };
static unsigned int de03149[] = { 0x0110D, 0x00000 };
static unsigned int de0314A[] = { 0x0110E, 0x00000 };
static unsigned int de0314B[] = { 0x0110F, 0x00000 };
static unsigned int de0314C[] = { 0x01110, 0x00000 };
static unsigned int de0314D[] = { 0x01111, 0x00000 };
static unsigned int de0314E[] = { 0x01112, 0x00000 };
static unsigned int de0314F[] = { 0x01161, 0x00000 };
static unsigned int de03150[] = { 0x01162, 0x00000 };
static unsigned int de03151[] = { 0x01163, 0x00000 };
static unsigned int de03152[] = { 0x01164, 0x00000 };
static unsigned int de03153[] = { 0x01165, 0x00000 };
static unsigned int de03154[] = { 0x01166, 0x00000 };
static unsigned int de03155[] = { 0x01167, 0x00000 };
static unsigned int de03156[] = { 0x01168, 0x00000 };
static unsigned int de03157[] = { 0x01169, 0x00000 };
static unsigned int de03158[] = { 0x0116A, 0x00000 };
static unsigned int de03159[] = { 0x0116B, 0x00000 };
static unsigned int de0315A[] = { 0x0116C, 0x00000 };
static unsigned int de0315B[] = { 0x0116D, 0x00000 };
static unsigned int de0315C[] = { 0x0116E, 0x00000 };
static unsigned int de0315D[] = { 0x0116F, 0x00000 };
static unsigned int de0315E[] = { 0x01170, 0x00000 };
static unsigned int de0315F[] = { 0x01171, 0x00000 };
static unsigned int de03160[] = { 0x01172, 0x00000 };
static unsigned int de03161[] = { 0x01173, 0x00000 };
static unsigned int de03162[] = { 0x01174, 0x00000 };
static unsigned int de03163[] = { 0x01175, 0x00000 };
static unsigned int de03164[] = { 0x01160, 0x00000 };
static unsigned int de03165[] = { 0x01114, 0x00000 };
static unsigned int de03166[] = { 0x01115, 0x00000 };
static unsigned int de03167[] = { 0x011C7, 0x00000 };
static unsigned int de03168[] = { 0x011C8, 0x00000 };
static unsigned int de03169[] = { 0x011CC, 0x00000 };
static unsigned int de0316A[] = { 0x011CE, 0x00000 };
static unsigned int de0316B[] = { 0x011D3, 0x00000 };
static unsigned int de0316C[] = { 0x011D7, 0x00000 };
static unsigned int de0316D[] = { 0x011D9, 0x00000 };
static unsigned int de0316E[] = { 0x0111C, 0x00000 };
static unsigned int de0316F[] = { 0x011DD, 0x00000 };
static unsigned int de03170[] = { 0x011DF, 0x00000 };
static unsigned int de03171[] = { 0x0111D, 0x00000 };
static unsigned int de03172[] = { 0x0111E, 0x00000 };
static unsigned int de03173[] = { 0x01120, 0x00000 };
static unsigned int de03174[] = { 0x01122, 0x00000 };
static unsigned int de03175[] = { 0x01123, 0x00000 };
static unsigned int de03176[] = { 0x01127, 0x00000 };
static unsigned int de03177[] = { 0x01129, 0x00000 };
static unsigned int de03178[] = { 0x0112B, 0x00000 };
static unsigned int de03179[] = { 0x0112C, 0x00000 };
static unsigned int de0317A[] = { 0x0112D, 0x00000 };
static unsigned int de0317B[] = { 0x0112E, 0x00000 };
static unsigned int de0317C[] = { 0x0112F, 0x00000 };
static unsigned int de0317D[] = { 0x01132, 0x00000 };
static unsigned int de0317E[] = { 0x01136, 0x00000 };
static unsigned int de0317F[] = { 0x01140, 0x00000 };
static unsigned int de03180[] = { 0x01147, 0x00000 };
static unsigned int de03181[] = { 0x0114C, 0x00000 };
static unsigned int de03182[] = { 0x011F1, 0x00000 };
static unsigned int de03183[] = { 0x011F2, 0x00000 };
static unsigned int de03184[] = { 0x01157, 0x00000 };
static unsigned int de03185[] = { 0x01158, 0x00000 };
static unsigned int de03186[] = { 0x01159, 0x00000 };
static unsigned int de03187[] = { 0x01184, 0x00000 };
static unsigned int de03188[] = { 0x01185, 0x00000 };
static unsigned int de03189[] = { 0x01188, 0x00000 };
static unsigned int de0318A[] = { 0x01191, 0x00000 };
static unsigned int de0318B[] = { 0x01192, 0x00000 };
static unsigned int de0318C[] = { 0x01194, 0x00000 };
static unsigned int de0318D[] = { 0x0119E, 0x00000 };
static unsigned int de0318E[] = { 0x011A1, 0x00000 };
static unsigned int de03192[] = { 0x04E00, 0x00000 };
static unsigned int de03193[] = { 0x04E8C, 0x00000 };
static unsigned int de03194[] = { 0x04E09, 0x00000 };
static unsigned int de03195[] = { 0x056DB, 0x00000 };
static unsigned int de03196[] = { 0x04E0A, 0x00000 };
static unsigned int de03197[] = { 0x04E2D, 0x00000 };
static unsigned int de03198[] = { 0x04E0B, 0x00000 };
static unsigned int de03199[] = { 0x07532, 0x00000 };
static unsigned int de0319A[] = { 0x04E59, 0x00000 };
static unsigned int de0319B[] = { 0x04E19, 0x00000 };
static unsigned int de0319C[] = { 0x04E01, 0x00000 };
static unsigned int de0319D[] = { 0x05929, 0x00000 };
static unsigned int de0319E[] = { 0x05730, 0x00000 };
static unsigned int de0319F[] = { 0x04EBA, 0x00000 };
static unsigned int de03200[] = { 0x00028, 0x01100, 0x00029, 0x00000 };
static unsigned int de03201[] = { 0x00028, 0x01102, 0x00029, 0x00000 };
static unsigned int de03202[] = { 0x00028, 0x01103, 0x00029, 0x00000 };
static unsigned int de03203[] = { 0x00028, 0x01105, 0x00029, 0x00000 };
static unsigned int de03204[] = { 0x00028, 0x01106, 0x00029, 0x00000 };
static unsigned int de03205[] = { 0x00028, 0x01107, 0x00029, 0x00000 };
static unsigned int de03206[] = { 0x00028, 0x01109, 0x00029, 0x00000 };
static unsigned int de03207[] = { 0x00028, 0x0110B, 0x00029, 0x00000 };
static unsigned int de03208[] = { 0x00028, 0x0110C, 0x00029, 0x00000 };
static unsigned int de03209[] = { 0x00028, 0x0110E, 0x00029, 0x00000 };
static unsigned int de0320A[] = { 0x00028, 0x0110F, 0x00029, 0x00000 };
static unsigned int de0320B[] = { 0x00028, 0x01110, 0x00029, 0x00000 };
static unsigned int de0320C[] = { 0x00028, 0x01111, 0x00029, 0x00000 };
static unsigned int de0320D[] = { 0x00028, 0x01112, 0x00029, 0x00000 };
static unsigned int de0320E[] = { 0x00028, 0x01100, 0x01161, 0x00029, 0x00000 };
static unsigned int de0320F[] = { 0x00028, 0x01102, 0x01161, 0x00029, 0x00000 };
static unsigned int de03210[] = { 0x00028, 0x01103, 0x01161, 0x00029, 0x00000 };
static unsigned int de03211[] = { 0x00028, 0x01105, 0x01161, 0x00029, 0x00000 };
static unsigned int de03212[] = { 0x00028, 0x01106, 0x01161, 0x00029, 0x00000 };
static unsigned int de03213[] = { 0x00028, 0x01107, 0x01161, 0x00029, 0x00000 };
static unsigned int de03214[] = { 0x00028, 0x01109, 0x01161, 0x00029, 0x00000 };
static unsigned int de03215[] = { 0x00028, 0x0110B, 0x01161, 0x00029, 0x00000 };
static unsigned int de03216[] = { 0x00028, 0x0110C, 0x01161, 0x00029, 0x00000 };
static unsigned int de03217[] = { 0x00028, 0x0110E, 0x01161, 0x00029, 0x00000 };
static unsigned int de03218[] = { 0x00028, 0x0110F, 0x01161, 0x00029, 0x00000 };
static unsigned int de03219[] = { 0x00028, 0x01110, 0x01161, 0x00029, 0x00000 };
static unsigned int de0321A[] = { 0x00028, 0x01111, 0x01161, 0x00029, 0x00000 };
static unsigned int de0321B[] = { 0x00028, 0x01112, 0x01161, 0x00029, 0x00000 };
static unsigned int de0321C[] = { 0x00028, 0x0110C, 0x0116E, 0x00029, 0x00000 };
static unsigned int de0321D[] = { 0x00028, 0x0110B, 0x01169, 0x0110C, 0x01165, 0x011AB, 0x00029, 0x00000 };
static unsigned int de0321E[] = { 0x00028, 0x0110B, 0x01169, 0x01112, 0x0116E, 0x00029, 0x00000 };
static unsigned int de03220[] = { 0x00028, 0x04E00, 0x00029, 0x00000 };
static unsigned int de03221[] = { 0x00028, 0x04E8C, 0x00029, 0x00000 };
static unsigned int de03222[] = { 0x00028, 0x04E09, 0x00029, 0x00000 };
static unsigned int de03223[] = { 0x00028, 0x056DB, 0x00029, 0x00000 };
static unsigned int de03224[] = { 0x00028, 0x04E94, 0x00029, 0x00000 };
static unsigned int de03225[] = { 0x00028, 0x0516D, 0x00029, 0x00000 };
static unsigned int de03226[] = { 0x00028, 0x04E03, 0x00029, 0x00000 };
static unsigned int de03227[] = { 0x00028, 0x0516B, 0x00029, 0x00000 };
static unsigned int de03228[] = { 0x00028, 0x04E5D, 0x00029, 0x00000 };
static unsigned int de03229[] = { 0x00028, 0x05341, 0x00029, 0x00000 };
static unsigned int de0322A[] = { 0x00028, 0x06708, 0x00029, 0x00000 };
static unsigned int de0322B[] = { 0x00028, 0x0706B, 0x00029, 0x00000 };
static unsigned int de0322C[] = { 0x00028, 0x06C34, 0x00029, 0x00000 };
static unsigned int de0322D[] = { 0x00028, 0x06728, 0x00029, 0x00000 };
static unsigned int de0322E[] = { 0x00028, 0x091D1, 0x00029, 0x00000 };
static unsigned int de0322F[] = { 0x00028, 0x0571F, 0x00029, 0x00000 };
static unsigned int de03230[] = { 0x00028, 0x065E5, 0x00029, 0x00000 };
static unsigned int de03231[] = { 0x00028, 0x0682A, 0x00029, 0x00000 };
static unsigned int de03232[] = { 0x00028, 0x06709, 0x00029, 0x00000 };
static unsigned int de03233[] = { 0x00028, 0x0793E, 0x00029, 0x00000 };
static unsigned int de03234[] = { 0x00028, 0x0540D, 0x00029, 0x00000 };
static unsigned int de03235[] = { 0x00028, 0x07279, 0x00029, 0x00000 };
static unsigned int de03236[] = { 0x00028, 0x08CA1, 0x00029, 0x00000 };
static unsigned int de03237[] = { 0x00028, 0x0795D, 0x00029, 0x00000 };
static unsigned int de03238[] = { 0x00028, 0x052B4, 0x00029, 0x00000 };
static unsigned int de03239[] = { 0x00028, 0x04EE3, 0x00029, 0x00000 };
static unsigned int de0323A[] = { 0x00028, 0x0547C, 0x00029, 0x00000 };
static unsigned int de0323B[] = { 0x00028, 0x05B66, 0x00029, 0x00000 };
static unsigned int de0323C[] = { 0x00028, 0x076E3, 0x00029, 0x00000 };
static unsigned int de0323D[] = { 0x00028, 0x04F01, 0x00029, 0x00000 };
static unsigned int de0323E[] = { 0x00028, 0x08CC7, 0x00029, 0x00000 };
static unsigned int de0323F[] = { 0x00028, 0x05354, 0x00029, 0x00000 };
static unsigned int de03240[] = { 0x00028, 0x0796D, 0x00029, 0x00000 };
static unsigned int de03241[] = { 0x00028, 0x04F11, 0x00029, 0x00000 };
static unsigned int de03242[] = { 0x00028, 0x081EA, 0x00029, 0x00000 };
static unsigned int de03243[] = { 0x00028, 0x081F3, 0x00029, 0x00000 };
static unsigned int de03250[] = { 0x00050, 0x00054, 0x00045, 0x00000 };
static unsigned int de03251[] = { 0x00032, 0x00031, 0x00000 };
static unsigned int de03252[] = { 0x00032, 0x00032, 0x00000 };
static unsigned int de03253[] = { 0x00032, 0x00033, 0x00000 };
static unsigned int de03254[] = { 0x00032, 0x00034, 0x00000 };
static unsigned int de03255[] = { 0x00032, 0x00035, 0x00000 };
static unsigned int de03256[] = { 0x00032, 0x00036, 0x00000 };
static unsigned int de03257[] = { 0x00032, 0x00037, 0x00000 };
static unsigned int de03258[] = { 0x00032, 0x00038, 0x00000 };
static unsigned int de03259[] = { 0x00032, 0x00039, 0x00000 };
static unsigned int de0325A[] = { 0x00033, 0x00030, 0x00000 };
static unsigned int de0325B[] = { 0x00033, 0x00031, 0x00000 };
static unsigned int de0325C[] = { 0x00033, 0x00032, 0x00000 };
static unsigned int de0325D[] = { 0x00033, 0x00033, 0x00000 };
static unsigned int de0325E[] = { 0x00033, 0x00034, 0x00000 };
static unsigned int de0325F[] = { 0x00033, 0x00035, 0x00000 };
static unsigned int de03260[] = { 0x01100, 0x00000 };
static unsigned int de03261[] = { 0x01102, 0x00000 };
static unsigned int de03262[] = { 0x01103, 0x00000 };
static unsigned int de03263[] = { 0x01105, 0x00000 };
static unsigned int de03264[] = { 0x01106, 0x00000 };
static unsigned int de03265[] = { 0x01107, 0x00000 };
static unsigned int de03266[] = { 0x01109, 0x00000 };
static unsigned int de03267[] = { 0x0110B, 0x00000 };
static unsigned int de03268[] = { 0x0110C, 0x00000 };
static unsigned int de03269[] = { 0x0110E, 0x00000 };
static unsigned int de0326A[] = { 0x0110F, 0x00000 };
static unsigned int de0326B[] = { 0x01110, 0x00000 };
static unsigned int de0326C[] = { 0x01111, 0x00000 };
static unsigned int de0326D[] = { 0x01112, 0x00000 };
static unsigned int de0326E[] = { 0x01100, 0x01161, 0x00000 };
static unsigned int de0326F[] = { 0x01102, 0x01161, 0x00000 };
static unsigned int de03270[] = { 0x01103, 0x01161, 0x00000 };
static unsigned int de03271[] = { 0x01105, 0x01161, 0x00000 };
static unsigned int de03272[] = { 0x01106, 0x01161, 0x00000 };
static unsigned int de03273[] = { 0x01107, 0x01161, 0x00000 };
static unsigned int de03274[] = { 0x01109, 0x01161, 0x00000 };
static unsigned int de03275[] = { 0x0110B, 0x01161, 0x00000 };
static unsigned int de03276[] = { 0x0110C, 0x01161, 0x00000 };
static unsigned int de03277[] = { 0x0110E, 0x01161, 0x00000 };
static unsigned int de03278[] = { 0x0110F, 0x01161, 0x00000 };
static unsigned int de03279[] = { 0x01110, 0x01161, 0x00000 };
static unsigned int de0327A[] = { 0x01111, 0x01161, 0x00000 };
static unsigned int de0327B[] = { 0x01112, 0x01161, 0x00000 };
static unsigned int de0327C[] = { 0x0110E, 0x01161, 0x011B7, 0x01100, 0x01169, 0x00000 };
static unsigned int de0327D[] = { 0x0110C, 0x0116E, 0x0110B, 0x01174, 0x00000 };
static unsigned int de0327E[] = { 0x0110B, 0x0116E, 0x00000 };
static unsigned int de03280[] = { 0x04E00, 0x00000 };
static unsigned int de03281[] = { 0x04E8C, 0x00000 };
static unsigned int de03282[] = { 0x04E09, 0x00000 };
static unsigned int de03283[] = { 0x056DB, 0x00000 };
static unsigned int de03284[] = { 0x04E94, 0x00000 };
static unsigned int de03285[] = { 0x0516D, 0x00000 };
static unsigned int de03286[] = { 0x04E03, 0x00000 };
static unsigned int de03287[] = { 0x0516B, 0x00000 };
static unsigned int de03288[] = { 0x04E5D, 0x00000 };
static unsigned int de03289[] = { 0x05341, 0x00000 };
static unsigned int de0328A[] = { 0x06708, 0x00000 };
static unsigned int de0328B[] = { 0x0706B, 0x00000 };
static unsigned int de0328C[] = { 0x06C34, 0x00000 };
static unsigned int de0328D[] = { 0x06728, 0x00000 };
static unsigned int de0328E[] = { 0x091D1, 0x00000 };
static unsigned int de0328F[] = { 0x0571F, 0x00000 };
static unsigned int de03290[] = { 0x065E5, 0x00000 };
static unsigned int de03291[] = { 0x0682A, 0x00000 };
static unsigned int de03292[] = { 0x06709, 0x00000 };
static unsigned int de03293[] = { 0x0793E, 0x00000 };
static unsigned int de03294[] = { 0x0540D, 0x00000 };
static unsigned int de03295[] = { 0x07279, 0x00000 };
static unsigned int de03296[] = { 0x08CA1, 0x00000 };
static unsigned int de03297[] = { 0x0795D, 0x00000 };
static unsigned int de03298[] = { 0x052B4, 0x00000 };
static unsigned int de03299[] = { 0x079D8, 0x00000 };
static unsigned int de0329A[] = { 0x07537, 0x00000 };
static unsigned int de0329B[] = { 0x05973, 0x00000 };
static unsigned int de0329C[] = { 0x09069, 0x00000 };
static unsigned int de0329D[] = { 0x0512A, 0x00000 };
static unsigned int de0329E[] = { 0x05370, 0x00000 };
static unsigned int de0329F[] = { 0x06CE8, 0x00000 };
static unsigned int de032A0[] = { 0x09805, 0x00000 };
static unsigned int de032A1[] = { 0x04F11, 0x00000 };
static unsigned int de032A2[] = { 0x05199, 0x00000 };
static unsigned int de032A3[] = { 0x06B63, 0x00000 };
static unsigned int de032A4[] = { 0x04E0A, 0x00000 };
static unsigned int de032A5[] = { 0x04E2D, 0x00000 };
static unsigned int de032A6[] = { 0x04E0B, 0x00000 };
static unsigned int de032A7[] = { 0x05DE6, 0x00000 };
static unsigned int de032A8[] = { 0x053F3, 0x00000 };
static unsigned int de032A9[] = { 0x0533B, 0x00000 };
static unsigned int de032AA[] = { 0x05B97, 0x00000 };
static unsigned int de032AB[] = { 0x05B66, 0x00000 };
static unsigned int de032AC[] = { 0x076E3, 0x00000 };
static unsigned int de032AD[] = { 0x04F01, 0x00000 };
static unsigned int de032AE[] = { 0x08CC7, 0x00000 };
static unsigned int de032AF[] = { 0x05354, 0x00000 };
static unsigned int de032B0[] = { 0x0591C, 0x00000 };
static unsigned int de032B1[] = { 0x00033, 0x00036, 0x00000 };
static unsigned int de032B2[] = { 0x00033, 0x00037, 0x00000 };
static unsigned int de032B3[] = { 0x00033, 0x00038, 0x00000 };
static unsigned int de032B4[] = { 0x00033, 0x00039, 0x00000 };
static unsigned int de032B5[] = { 0x00034, 0x00030, 0x00000 };
static unsigned int de032B6[] = { 0x00034, 0x00031, 0x00000 };
static unsigned int de032B7[] = { 0x00034, 0x00032, 0x00000 };
static unsigned int de032B8[] = { 0x00034, 0x00033, 0x00000 };
static unsigned int de032B9[] = { 0x00034, 0x00034, 0x00000 };
static unsigned int de032BA[] = { 0x00034, 0x00035, 0x00000 };
static unsigned int de032BB[] = { 0x00034, 0x00036, 0x00000 };
static unsigned int de032BC[] = { 0x00034, 0x00037, 0x00000 };
static unsigned int de032BD[] = { 0x00034, 0x00038, 0x00000 };
static unsigned int de032BE[] = { 0x00034, 0x00039, 0x00000 };
static unsigned int de032BF[] = { 0x00035, 0x00030, 0x00000 };
static unsigned int de032C0[] = { 0x00031, 0x06708, 0x00000 };
static unsigned int de032C1[] = { 0x00032, 0x06708, 0x00000 };
static unsigned int de032C2[] = { 0x00033, 0x06708, 0x00000 };
static unsigned int de032C3[] = { 0x00034, 0x06708, 0x00000 };
static unsigned int de032C4[] = { 0x00035, 0x06708, 0x00000 };
static unsigned int de032C5[] = { 0x00036, 0x06708, 0x00000 };
static unsigned int de032C6[] = { 0x00037, 0x06708, 0x00000 };
static unsigned int de032C7[] = { 0x00038, 0x06708, 0x00000 };
static unsigned int de032C8[] = { 0x00039, 0x06708, 0x00000 };
static unsigned int de032C9[] = { 0x00031, 0x00030, 0x06708, 0x00000 };
static unsigned int de032CA[] = { 0x00031, 0x00031, 0x06708, 0x00000 };
static unsigned int de032CB[] = { 0x00031, 0x00032, 0x06708, 0x00000 };
static unsigned int de032CC[] = { 0x00048, 0x00067, 0x00000 };
static unsigned int de032CD[] = { 0x00065, 0x00072, 0x00067, 0x00000 };
static unsigned int de032CE[] = { 0x00065, 0x00056, 0x00000 };
static unsigned int de032CF[] = { 0x0004C, 0x00054, 0x00044, 0x00000 };
static unsigned int de032D0[] = { 0x030A2, 0x00000 };
static unsigned int de032D1[] = { 0x030A4, 0x00000 };
static unsigned int de032D2[] = { 0x030A6, 0x00000 };
static unsigned int de032D3[] = { 0x030A8, 0x00000 };
static unsigned int de032D4[] = { 0x030AA, 0x00000 };
static unsigned int de032D5[] = { 0x030AB, 0x00000 };
static unsigned int de032D6[] = { 0x030AD, 0x00000 };
static unsigned int de032D7[] = { 0x030AF, 0x00000 };
static unsigned int de032D8[] = { 0x030B1, 0x00000 };
static unsigned int de032D9[] = { 0x030B3, 0x00000 };
static unsigned int de032DA[] = { 0x030B5, 0x00000 };
static unsigned int de032DB[] = { 0x030B7, 0x00000 };
static unsigned int de032DC[] = { 0x030B9, 0x00000 };
static unsigned int de032DD[] = { 0x030BB, 0x00000 };
static unsigned int de032DE[] = { 0x030BD, 0x00000 };
static unsigned int de032DF[] = { 0x030BF, 0x00000 };
static unsigned int de032E0[] = { 0x030C1, 0x00000 };
static unsigned int de032E1[] = { 0x030C4, 0x00000 };
static unsigned int de032E2[] = { 0x030C6, 0x00000 };
static unsigned int de032E3[] = { 0x030C8, 0x00000 };
static unsigned int de032E4[] = { 0x030CA, 0x00000 };
static unsigned int de032E5[] = { 0x030CB, 0x00000 };
static unsigned int de032E6[] = { 0x030CC, 0x00000 };
static unsigned int de032E7[] = { 0x030CD, 0x00000 };
static unsigned int de032E8[] = { 0x030CE, 0x00000 };
static unsigned int de032E9[] = { 0x030CF, 0x00000 };
static unsigned int de032EA[] = { 0x030D2, 0x00000 };
static unsigned int de032EB[] = { 0x030D5, 0x00000 };
static unsigned int de032EC[] = { 0x030D8, 0x00000 };
static unsigned int de032ED[] = { 0x030DB, 0x00000 };
static unsigned int de032EE[] = { 0x030DE, 0x00000 };
static unsigned int de032EF[] = { 0x030DF, 0x00000 };
static unsigned int de032F0[] = { 0x030E0, 0x00000 };
static unsigned int de032F1[] = { 0x030E1, 0x00000 };
static unsigned int de032F2[] = { 0x030E2, 0x00000 };
static unsigned int de032F3[] = { 0x030E4, 0x00000 };
static unsigned int de032F4[] = { 0x030E6, 0x00000 };
static unsigned int de032F5[] = { 0x030E8, 0x00000 };
static unsigned int de032F6[] = { 0x030E9, 0x00000 };
static unsigned int de032F7[] = { 0x030EA, 0x00000 };
static unsigned int de032F8[] = { 0x030EB, 0x00000 };
static unsigned int de032F9[] = { 0x030EC, 0x00000 };
static unsigned int de032FA[] = { 0x030ED, 0x00000 };
static unsigned int de032FB[] = { 0x030EF, 0x00000 };
static unsigned int de032FC[] = { 0x030F0, 0x00000 };
static unsigned int de032FD[] = { 0x030F1, 0x00000 };
static unsigned int de032FE[] = { 0x030F2, 0x00000 };
static unsigned int de03300[] = { 0x030A2, 0x030D1, 0x030FC, 0x030C8, 0x00000 };
static unsigned int de03301[] = { 0x030A2, 0x030EB, 0x030D5, 0x030A1, 0x00000 };
static unsigned int de03302[] = { 0x030A2, 0x030F3, 0x030DA, 0x030A2, 0x00000 };
static unsigned int de03303[] = { 0x030A2, 0x030FC, 0x030EB, 0x00000 };
static unsigned int de03304[] = { 0x030A4, 0x030CB, 0x030F3, 0x030B0, 0x00000 };
static unsigned int de03305[] = { 0x030A4, 0x030F3, 0x030C1, 0x00000 };
static unsigned int de03306[] = { 0x030A6, 0x030A9, 0x030F3, 0x00000 };
static unsigned int de03307[] = { 0x030A8, 0x030B9, 0x030AF, 0x030FC, 0x030C9, 0x00000 };
static unsigned int de03308[] = { 0x030A8, 0x030FC, 0x030AB, 0x030FC, 0x00000 };
static unsigned int de03309[] = { 0x030AA, 0x030F3, 0x030B9, 0x00000 };
static unsigned int de0330A[] = { 0x030AA, 0x030FC, 0x030E0, 0x00000 };
static unsigned int de0330B[] = { 0x030AB, 0x030A4, 0x030EA, 0x00000 };
static unsigned int de0330C[] = { 0x030AB, 0x030E9, 0x030C3, 0x030C8, 0x00000 };
static unsigned int de0330D[] = { 0x030AB, 0x030ED, 0x030EA, 0x030FC, 0x00000 };
static unsigned int de0330E[] = { 0x030AC, 0x030ED, 0x030F3, 0x00000 };
static unsigned int de0330F[] = { 0x030AC, 0x030F3, 0x030DE, 0x00000 };
static unsigned int de03310[] = { 0x030AE, 0x030AC, 0x00000 };
static unsigned int de03311[] = { 0x030AE, 0x030CB, 0x030FC, 0x00000 };
static unsigned int de03312[] = { 0x030AD, 0x030E5, 0x030EA, 0x030FC, 0x00000 };
static unsigned int de03313[] = { 0x030AE, 0x030EB, 0x030C0, 0x030FC, 0x00000 };
static unsigned int de03314[] = { 0x030AD, 0x030ED, 0x00000 };
static unsigned int de03315[] = { 0x030AD, 0x030ED, 0x030B0, 0x030E9, 0x030E0, 0x00000 };
static unsigned int de03316[] = { 0x030AD, 0x030ED, 0x030E1, 0x030FC, 0x030C8, 0x030EB, 0x00000 };
static unsigned int de03317[] = { 0x030AD, 0x030ED, 0x030EF, 0x030C3, 0x030C8, 0x00000 };
static unsigned int de03318[] = { 0x030B0, 0x030E9, 0x030E0, 0x00000 };
static unsigned int de03319[] = { 0x030B0, 0x030E9, 0x030E0, 0x030C8, 0x030F3, 0x00000 };
static unsigned int de0331A[] = { 0x030AF, 0x030EB, 0x030BC, 0x030A4, 0x030ED, 0x00000 };
static unsigned int de0331B[] = { 0x030AF, 0x030ED, 0x030FC, 0x030CD, 0x00000 };
static unsigned int de0331C[] = { 0x030B1, 0x030FC, 0x030B9, 0x00000 };
static unsigned int de0331D[] = { 0x030B3, 0x030EB, 0x030CA, 0x00000 };
static unsigned int de0331E[] = { 0x030B3, 0x030FC, 0x030DD, 0x00000 };
static unsigned int de0331F[] = { 0x030B5, 0x030A4, 0x030AF, 0x030EB, 0x00000 };
static unsigned int de03320[] = { 0x030B5, 0x030F3, 0x030C1, 0x030FC, 0x030E0, 0x00000 };
static unsigned int de03321[] = { 0x030B7, 0x030EA, 0x030F3, 0x030B0, 0x00000 };
static unsigned int de03322[] = { 0x030BB, 0x030F3, 0x030C1, 0x00000 };
static unsigned int de03323[] = { 0x030BB, 0x030F3, 0x030C8, 0x00000 };
static unsigned int de03324[] = { 0x030C0, 0x030FC, 0x030B9, 0x00000 };
static unsigned int de03325[] = { 0x030C7, 0x030B7, 0x00000 };
static unsigned int de03326[] = { 0x030C9, 0x030EB, 0x00000 };
static unsigned int de03327[] = { 0x030C8, 0x030F3, 0x00000 };
static unsigned int de03328[] = { 0x030CA, 0x030CE, 0x00000 };
static unsigned int de03329[] = { 0x030CE, 0x030C3, 0x030C8, 0x00000 };
static unsigned int de0332A[] = { 0x030CF, 0x030A4, 0x030C4, 0x00000 };
static unsigned int de0332B[] = { 0x030D1, 0x030FC, 0x030BB, 0x030F3, 0x030C8, 0x00000 };
static unsigned int de0332C[] = { 0x030D1, 0x030FC, 0x030C4, 0x00000 };
static unsigned int de0332D[] = { 0x030D0, 0x030FC, 0x030EC, 0x030EB, 0x00000 };
static unsigned int de0332E[] = { 0x030D4, 0x030A2, 0x030B9, 0x030C8, 0x030EB, 0x00000 };
static unsigned int de0332F[] = { 0x030D4, 0x030AF, 0x030EB, 0x00000 };
static unsigned int de03330[] = { 0x030D4, 0x030B3, 0x00000 };
static unsigned int de03331[] = { 0x030D3, 0x030EB, 0x00000 };
static unsigned int de03332[] = { 0x030D5, 0x030A1, 0x030E9, 0x030C3, 0x030C9, 0x00000 };
static unsigned int de03333[] = { 0x030D5, 0x030A3, 0x030FC, 0x030C8, 0x00000 };
static unsigned int de03334[] = { 0x030D6, 0x030C3, 0x030B7, 0x030A7, 0x030EB, 0x00000 };
static unsigned int de03335[] = { 0x030D5, 0x030E9, 0x030F3, 0x00000 };
static unsigned int de03336[] = { 0x030D8, 0x030AF, 0x030BF, 0x030FC, 0x030EB, 0x00000 };
static unsigned int de03337[] = { 0x030DA, 0x030BD, 0x00000 };
static unsigned int de03338[] = { 0x030DA, 0x030CB, 0x030D2, 0x00000 };
static unsigned int de03339[] = { 0x030D8, 0x030EB, 0x030C4, 0x00000 };
static unsigned int de0333A[] = { 0x030DA, 0x030F3, 0x030B9, 0x00000 };
static unsigned int de0333B[] = { 0x030DA, 0x030FC, 0x030B8, 0x00000 };
static unsigned int de0333C[] = { 0x030D9, 0x030FC, 0x030BF, 0x00000 };
static unsigned int de0333D[] = { 0x030DD, 0x030A4, 0x030F3, 0x030C8, 0x00000 };
static unsigned int de0333E[] = { 0x030DC, 0x030EB, 0x030C8, 0x00000 };
static unsigned int de0333F[] = { 0x030DB, 0x030F3, 0x00000 };
static unsigned int de03340[] = { 0x030DD, 0x030F3, 0x030C9, 0x00000 };
static unsigned int de03341[] = { 0x030DB, 0x030FC, 0x030EB, 0x00000 };
static unsigned int de03342[] = { 0x030DB, 0x030FC, 0x030F3, 0x00000 };
static unsigned int de03343[] = { 0x030DE, 0x030A4, 0x030AF, 0x030ED, 0x00000 };
static unsigned int de03344[] = { 0x030DE, 0x030A4, 0x030EB, 0x00000 };
static unsigned int de03345[] = { 0x030DE, 0x030C3, 0x030CF, 0x00000 };
static unsigned int de03346[] = { 0x030DE, 0x030EB, 0x030AF, 0x00000 };
static unsigned int de03347[] = { 0x030DE, 0x030F3, 0x030B7, 0x030E7, 0x030F3, 0x00000 };
static unsigned int de03348[] = { 0x030DF, 0x030AF, 0x030ED, 0x030F3, 0x00000 };
static unsigned int de03349[] = { 0x030DF, 0x030EA, 0x00000 };
static unsigned int de0334A[] = { 0x030DF, 0x030EA, 0x030D0, 0x030FC, 0x030EB, 0x00000 };
static unsigned int de0334B[] = { 0x030E1, 0x030AC, 0x00000 };
static unsigned int de0334C[] = { 0x030E1, 0x030AC, 0x030C8, 0x030F3, 0x00000 };
static unsigned int de0334D[] = { 0x030E1, 0x030FC, 0x030C8, 0x030EB, 0x00000 };
static unsigned int de0334E[] = { 0x030E4, 0x030FC, 0x030C9, 0x00000 };
static unsigned int de0334F[] = { 0x030E4, 0x030FC, 0x030EB, 0x00000 };
static unsigned int de03350[] = { 0x030E6, 0x030A2, 0x030F3, 0x00000 };
static unsigned int de03351[] = { 0x030EA, 0x030C3, 0x030C8, 0x030EB, 0x00000 };
static unsigned int de03352[] = { 0x030EA, 0x030E9, 0x00000 };
static unsigned int de03353[] = { 0x030EB, 0x030D4, 0x030FC, 0x00000 };
static unsigned int de03354[] = { 0x030EB, 0x030FC, 0x030D6, 0x030EB, 0x00000 };
static unsigned int de03355[] = { 0x030EC, 0x030E0, 0x00000 };
static unsigned int de03356[] = { 0x030EC, 0x030F3, 0x030C8, 0x030B2, 0x030F3, 0x00000 };
static unsigned int de03357[] = { 0x030EF, 0x030C3, 0x030C8, 0x00000 };
static unsigned int de03358[] = { 0x00030, 0x070B9, 0x00000 };
static unsigned int de03359[] = { 0x00031, 0x070B9, 0x00000 };
static unsigned int de0335A[] = { 0x00032, 0x070B9, 0x00000 };
static unsigned int de0335B[] = { 0x00033, 0x070B9, 0x00000 };
static unsigned int de0335C[] = { 0x00034, 0x070B9, 0x00000 };
static unsigned int de0335D[] = { 0x00035, 0x070B9, 0x00000 };
static unsigned int de0335E[] = { 0x00036, 0x070B9, 0x00000 };
static unsigned int de0335F[] = { 0x00037, 0x070B9, 0x00000 };
static unsigned int de03360[] = { 0x00038, 0x070B9, 0x00000 };
static unsigned int de03361[] = { 0x00039, 0x070B9, 0x00000 };
static unsigned int de03362[] = { 0x00031, 0x00030, 0x070B9, 0x00000 };
static unsigned int de03363[] = { 0x00031, 0x00031, 0x070B9, 0x00000 };
static unsigned int de03364[] = { 0x00031, 0x00032, 0x070B9, 0x00000 };
static unsigned int de03365[] = { 0x00031, 0x00033, 0x070B9, 0x00000 };
static unsigned int de03366[] = { 0x00031, 0x00034, 0x070B9, 0x00000 };
static unsigned int de03367[] = { 0x00031, 0x00035, 0x070B9, 0x00000 };
static unsigned int de03368[] = { 0x00031, 0x00036, 0x070B9, 0x00000 };
static unsigned int de03369[] = { 0x00031, 0x00037, 0x070B9, 0x00000 };
static unsigned int de0336A[] = { 0x00031, 0x00038, 0x070B9, 0x00000 };
static unsigned int de0336B[] = { 0x00031, 0x00039, 0x070B9, 0x00000 };
static unsigned int de0336C[] = { 0x00032, 0x00030, 0x070B9, 0x00000 };
static unsigned int de0336D[] = { 0x00032, 0x00031, 0x070B9, 0x00000 };
static unsigned int de0336E[] = { 0x00032, 0x00032, 0x070B9, 0x00000 };
static unsigned int de0336F[] = { 0x00032, 0x00033, 0x070B9, 0x00000 };
static unsigned int de03370[] = { 0x00032, 0x00034, 0x070B9, 0x00000 };
static unsigned int de03371[] = { 0x00068, 0x00050, 0x00061, 0x00000 };
static unsigned int de03372[] = { 0x00064, 0x00061, 0x00000 };
static unsigned int de03373[] = { 0x00041, 0x00055, 0x00000 };
static unsigned int de03374[] = { 0x00062, 0x00061, 0x00072, 0x00000 };
static unsigned int de03375[] = { 0x0006F, 0x00056, 0x00000 };
static unsigned int de03376[] = { 0x00070, 0x00063, 0x00000 };
static unsigned int de03377[] = { 0x00064, 0x0006D, 0x00000 };
static unsigned int de03378[] = { 0x00064, 0x0006D, 0x000B2, 0x00000 };
static unsigned int de03379[] = { 0x00064, 0x0006D, 0x000B3, 0x00000 };
static unsigned int de0337A[] = { 0x00049, 0x00055, 0x00000 };
static unsigned int de0337B[] = { 0x05E73, 0x06210, 0x00000 };
static unsigned int de0337C[] = { 0x0662D, 0x0548C, 0x00000 };
static unsigned int de0337D[] = { 0x05927, 0x06B63, 0x00000 };
static unsigned int de0337E[] = { 0x0660E, 0x06CBB, 0x00000 };
static unsigned int de0337F[] = { 0x0682A, 0x05F0F, 0x04F1A, 0x0793E, 0x00000 };
static unsigned int de03380[] = { 0x00070, 0x00041, 0x00000 };
static unsigned int de03381[] = { 0x0006E, 0x00041, 0x00000 };
static unsigned int de03382[] = { 0x003BC, 0x00041, 0x00000 };
static unsigned int de03383[] = { 0x0006D, 0x00041, 0x00000 };
static unsigned int de03384[] = { 0x0006B, 0x00041, 0x00000 };
static unsigned int de03385[] = { 0x0004B, 0x00042, 0x00000 };
static unsigned int de03386[] = { 0x0004D, 0x00042, 0x00000 };
static unsigned int de03387[] = { 0x00047, 0x00042, 0x00000 };
static unsigned int de03388[] = { 0x00063, 0x00061, 0x0006C, 0x00000 };
static unsigned int de03389[] = { 0x0006B, 0x00063, 0x00061, 0x0006C, 0x00000 };
static unsigned int de0338A[] = { 0x00070, 0x00046, 0x00000 };
static unsigned int de0338B[] = { 0x0006E, 0x00046, 0x00000 };
static unsigned int de0338C[] = { 0x003BC, 0x00046, 0x00000 };
static unsigned int de0338D[] = { 0x003BC, 0x00067, 0x00000 };
static unsigned int de0338E[] = { 0x0006D, 0x00067, 0x00000 };
static unsigned int de0338F[] = { 0x0006B, 0x00067, 0x00000 };
static unsigned int de03390[] = { 0x00048, 0x0007A, 0x00000 };
static unsigned int de03391[] = { 0x0006B, 0x00048, 0x0007A, 0x00000 };
static unsigned int de03392[] = { 0x0004D, 0x00048, 0x0007A, 0x00000 };
static unsigned int de03393[] = { 0x00047, 0x00048, 0x0007A, 0x00000 };
static unsigned int de03394[] = { 0x00054, 0x00048, 0x0007A, 0x00000 };
static unsigned int de03395[] = { 0x003BC, 0x02113, 0x00000 };
static unsigned int de03396[] = { 0x0006D, 0x02113, 0x00000 };
static unsigned int de03397[] = { 0x00064, 0x02113, 0x00000 };
static unsigned int de03398[] = { 0x0006B, 0x02113, 0x00000 };
static unsigned int de03399[] = { 0x00066, 0x0006D, 0x00000 };
static unsigned int de0339A[] = { 0x0006E, 0x0006D, 0x00000 };
static unsigned int de0339B[] = { 0x003BC, 0x0006D, 0x00000 };
static unsigned int de0339C[] = { 0x0006D, 0x0006D, 0x00000 };
static unsigned int de0339D[] = { 0x00063, 0x0006D, 0x00000 };
static unsigned int de0339E[] = { 0x0006B, 0x0006D, 0x00000 };
static unsigned int de0339F[] = { 0x0006D, 0x0006D, 0x000B2, 0x00000 };
static unsigned int de033A0[] = { 0x00063, 0x0006D, 0x000B2, 0x00000 };
static unsigned int de033A1[] = { 0x0006D, 0x000B2, 0x00000 };
static unsigned int de033A2[] = { 0x0006B, 0x0006D, 0x000B2, 0x00000 };
static unsigned int de033A3[] = { 0x0006D, 0x0006D, 0x000B3, 0x00000 };
static unsigned int de033A4[] = { 0x00063, 0x0006D, 0x000B3, 0x00000 };
static unsigned int de033A5[] = { 0x0006D, 0x000B3, 0x00000 };
static unsigned int de033A6[] = { 0x0006B, 0x0006D, 0x000B3, 0x00000 };
static unsigned int de033A7[] = { 0x0006D, 0x02215, 0x00073, 0x00000 };
static unsigned int de033A8[] = { 0x0006D, 0x02215, 0x00073, 0x000B2, 0x00000 };
static unsigned int de033A9[] = { 0x00050, 0x00061, 0x00000 };
static unsigned int de033AA[] = { 0x0006B, 0x00050, 0x00061, 0x00000 };
static unsigned int de033AB[] = { 0x0004D, 0x00050, 0x00061, 0x00000 };
static unsigned int de033AC[] = { 0x00047, 0x00050, 0x00061, 0x00000 };
static unsigned int de033AD[] = { 0x00072, 0x00061, 0x00064, 0x00000 };
static unsigned int de033AE[] = { 0x00072, 0x00061, 0x00064, 0x02215, 0x00073, 0x00000 };
static unsigned int de033AF[] = { 0x00072, 0x00061, 0x00064, 0x02215, 0x00073, 0x000B2, 0x00000 };
static unsigned int de033B0[] = { 0x00070, 0x00073, 0x00000 };
static unsigned int de033B1[] = { 0x0006E, 0x00073, 0x00000 };
static unsigned int de033B2[] = { 0x003BC, 0x00073, 0x00000 };
static unsigned int de033B3[] = { 0x0006D, 0x00073, 0x00000 };
static unsigned int de033B4[] = { 0x00070, 0x00056, 0x00000 };
static unsigned int de033B5[] = { 0x0006E, 0x00056, 0x00000 };
static unsigned int de033B6[] = { 0x003BC, 0x00056, 0x00000 };
static unsigned int de033B7[] = { 0x0006D, 0x00056, 0x00000 };
static unsigned int de033B8[] = { 0x0006B, 0x00056, 0x00000 };
static unsigned int de033B9[] = { 0x0004D, 0x00056, 0x00000 };
static unsigned int de033BA[] = { 0x00070, 0x00057, 0x00000 };
static unsigned int de033BB[] = { 0x0006E, 0x00057, 0x00000 };
static unsigned int de033BC[] = { 0x003BC, 0x00057, 0x00000 };
static unsigned int de033BD[] = { 0x0006D, 0x00057, 0x00000 };
static unsigned int de033BE[] = { 0x0006B, 0x00057, 0x00000 };
static unsigned int de033BF[] = { 0x0004D, 0x00057, 0x00000 };
static unsigned int de033C0[] = { 0x0006B, 0x003A9, 0x00000 };
static unsigned int de033C1[] = { 0x0004D, 0x003A9, 0x00000 };
static unsigned int de033C2[] = { 0x00061, 0x0002E, 0x0006D, 0x0002E, 0x00000 };
static unsigned int de033C3[] = { 0x00042, 0x00071, 0x00000 };
static unsigned int de033C4[] = { 0x00063, 0x00063, 0x00000 };
static unsigned int de033C5[] = { 0x00063, 0x00064, 0x00000 };
static unsigned int de033C6[] = { 0x00043, 0x02215, 0x0006B, 0x00067, 0x00000 };
static unsigned int de033C7[] = { 0x00043, 0x0006F, 0x0002E, 0x00000 };
static unsigned int de033C8[] = { 0x00064, 0x00042, 0x00000 };
static unsigned int de033C9[] = { 0x00047, 0x00079, 0x00000 };
static unsigned int de033CA[] = { 0x00068, 0x00061, 0x00000 };
static unsigned int de033CB[] = { 0x00048, 0x00050, 0x00000 };
static unsigned int de033CC[] = { 0x00069, 0x0006E, 0x00000 };
static unsigned int de033CD[] = { 0x0004B, 0x0004B, 0x00000 };
static unsigned int de033CE[] = { 0x0004B, 0x0004D, 0x00000 };
static unsigned int de033CF[] = { 0x0006B, 0x00074, 0x00000 };
static unsigned int de033D0[] = { 0x0006C, 0x0006D, 0x00000 };
static unsigned int de033D1[] = { 0x0006C, 0x0006E, 0x00000 };
static unsigned int de033D2[] = { 0x0006C, 0x0006F, 0x00067, 0x00000 };
static unsigned int de033D3[] = { 0x0006C, 0x00078, 0x00000 };
static unsigned int de033D4[] = { 0x0006D, 0x00062, 0x00000 };
static unsigned int de033D5[] = { 0x0006D, 0x00069, 0x0006C, 0x00000 };
static unsigned int de033D6[] = { 0x0006D, 0x0006F, 0x0006C, 0x00000 };
static unsigned int de033D7[] = { 0x00050, 0x00048, 0x00000 };
static unsigned int de033D8[] = { 0x00070, 0x0002E, 0x0006D, 0x0002E, 0x00000 };
static unsigned int de033D9[] = { 0x00050, 0x00050, 0x0004D, 0x00000 };
static unsigned int de033DA[] = { 0x00050, 0x00052, 0x00000 };
static unsigned int de033DB[] = { 0x00073, 0x00072, 0x00000 };
static unsigned int de033DC[] = { 0x00053, 0x00076, 0x00000 };
static unsigned int de033DD[] = { 0x00057, 0x00062, 0x00000 };
static unsigned int de033DE[] = { 0x00056, 0x02215, 0x0006D, 0x00000 };
static unsigned int de033DF[] = { 0x00041, 0x02215, 0x0006D, 0x00000 };
static unsigned int de033E0[] = { 0x00031, 0x065E5, 0x00000 };
static unsigned int de033E1[] = { 0x00032, 0x065E5, 0x00000 };
static unsigned int de033E2[] = { 0x00033, 0x065E5, 0x00000 };
static unsigned int de033E3[] = { 0x00034, 0x065E5, 0x00000 };
static unsigned int de033E4[] = { 0x00035, 0x065E5, 0x00000 };
static unsigned int de033E5[] = { 0x00036, 0x065E5, 0x00000 };
static unsigned int de033E6[] = { 0x00037, 0x065E5, 0x00000 };
static unsigned int de033E7[] = { 0x00038, 0x065E5, 0x00000 };
static unsigned int de033E8[] = { 0x00039, 0x065E5, 0x00000 };
static unsigned int de033E9[] = { 0x00031, 0x00030, 0x065E5, 0x00000 };
static unsigned int de033EA[] = { 0x00031, 0x00031, 0x065E5, 0x00000 };
static unsigned int de033EB[] = { 0x00031, 0x00032, 0x065E5, 0x00000 };
static unsigned int de033EC[] = { 0x00031, 0x00033, 0x065E5, 0x00000 };
static unsigned int de033ED[] = { 0x00031, 0x00034, 0x065E5, 0x00000 };
static unsigned int de033EE[] = { 0x00031, 0x00035, 0x065E5, 0x00000 };
static unsigned int de033EF[] = { 0x00031, 0x00036, 0x065E5, 0x00000 };
static unsigned int de033F0[] = { 0x00031, 0x00037, 0x065E5, 0x00000 };
static unsigned int de033F1[] = { 0x00031, 0x00038, 0x065E5, 0x00000 };
static unsigned int de033F2[] = { 0x00031, 0x00039, 0x065E5, 0x00000 };
static unsigned int de033F3[] = { 0x00032, 0x00030, 0x065E5, 0x00000 };
static unsigned int de033F4[] = { 0x00032, 0x00031, 0x065E5, 0x00000 };
static unsigned int de033F5[] = { 0x00032, 0x00032, 0x065E5, 0x00000 };
static unsigned int de033F6[] = { 0x00032, 0x00033, 0x065E5, 0x00000 };
static unsigned int de033F7[] = { 0x00032, 0x00034, 0x065E5, 0x00000 };
static unsigned int de033F8[] = { 0x00032, 0x00035, 0x065E5, 0x00000 };
static unsigned int de033F9[] = { 0x00032, 0x00036, 0x065E5, 0x00000 };
static unsigned int de033FA[] = { 0x00032, 0x00037, 0x065E5, 0x00000 };
static unsigned int de033FB[] = { 0x00032, 0x00038, 0x065E5, 0x00000 };
static unsigned int de033FC[] = { 0x00032, 0x00039, 0x065E5, 0x00000 };
static unsigned int de033FD[] = { 0x00033, 0x00030, 0x065E5, 0x00000 };
static unsigned int de033FE[] = { 0x00033, 0x00031, 0x065E5, 0x00000 };
static unsigned int de033FF[] = { 0x00067, 0x00061, 0x0006C, 0x00000 };
static unsigned int de0F900[] = { 0x08C48, 0x00000 };
static unsigned int de0F901[] = { 0x066F4, 0x00000 };
static unsigned int de0F902[] = { 0x08ECA, 0x00000 };
static unsigned int de0F903[] = { 0x08CC8, 0x00000 };
static unsigned int de0F904[] = { 0x06ED1, 0x00000 };
static unsigned int de0F905[] = { 0x04E32, 0x00000 };
static unsigned int de0F906[] = { 0x053E5, 0x00000 };
static unsigned int de0F907[] = { 0x09F9C, 0x00000 };
static unsigned int de0F908[] = { 0x09F9C, 0x00000 };
static unsigned int de0F909[] = { 0x05951, 0x00000 };
static unsigned int de0F90A[] = { 0x091D1, 0x00000 };
static unsigned int de0F90B[] = { 0x05587, 0x00000 };
static unsigned int de0F90C[] = { 0x05948, 0x00000 };
static unsigned int de0F90D[] = { 0x061F6, 0x00000 };
static unsigned int de0F90E[] = { 0x07669, 0x00000 };
static unsigned int de0F90F[] = { 0x07F85, 0x00000 };
static unsigned int de0F910[] = { 0x0863F, 0x00000 };
static unsigned int de0F911[] = { 0x087BA, 0x00000 };
static unsigned int de0F912[] = { 0x088F8, 0x00000 };
static unsigned int de0F913[] = { 0x0908F, 0x00000 };
static unsigned int de0F914[] = { 0x06A02, 0x00000 };
static unsigned int de0F915[] = { 0x06D1B, 0x00000 };
static unsigned int de0F916[] = { 0x070D9, 0x00000 };
static unsigned int de0F917[] = { 0x073DE, 0x00000 };
static unsigned int de0F918[] = { 0x0843D, 0x00000 };
static unsigned int de0F919[] = { 0x0916A, 0x00000 };
static unsigned int de0F91A[] = { 0x099F1, 0x00000 };
static unsigned int de0F91B[] = { 0x04E82, 0x00000 };
static unsigned int de0F91C[] = { 0x05375, 0x00000 };
static unsigned int de0F91D[] = { 0x06B04, 0x00000 };
static unsigned int de0F91E[] = { 0x0721B, 0x00000 };
static unsigned int de0F91F[] = { 0x0862D, 0x00000 };
static unsigned int de0F920[] = { 0x09E1E, 0x00000 };
static unsigned int de0F921[] = { 0x05D50, 0x00000 };
static unsigned int de0F922[] = { 0x06FEB, 0x00000 };
static unsigned int de0F923[] = { 0x085CD, 0x00000 };
static unsigned int de0F924[] = { 0x08964, 0x00000 };
static unsigned int de0F925[] = { 0x062C9, 0x00000 };
static unsigned int de0F926[] = { 0x081D8, 0x00000 };
static unsigned int de0F927[] = { 0x0881F, 0x00000 };
static unsigned int de0F928[] = { 0x05ECA, 0x00000 };
static unsigned int de0F929[] = { 0x06717, 0x00000 };
static unsigned int de0F92A[] = { 0x06D6A, 0x00000 };
static unsigned int de0F92B[] = { 0x072FC, 0x00000 };
static unsigned int de0F92C[] = { 0x090CE, 0x00000 };
static unsigned int de0F92D[] = { 0x04F86, 0x00000 };
static unsigned int de0F92E[] = { 0x051B7, 0x00000 };
static unsigned int de0F92F[] = { 0x052DE, 0x00000 };
static unsigned int de0F930[] = { 0x064C4, 0x00000 };
static unsigned int de0F931[] = { 0x06AD3, 0x00000 };
static unsigned int de0F932[] = { 0x07210, 0x00000 };
static unsigned int de0F933[] = { 0x076E7, 0x00000 };
static unsigned int de0F934[] = { 0x08001, 0x00000 };
static unsigned int de0F935[] = { 0x08606, 0x00000 };
static unsigned int de0F936[] = { 0x0865C, 0x00000 };
static unsigned int de0F937[] = { 0x08DEF, 0x00000 };
static unsigned int de0F938[] = { 0x09732, 0x00000 };
static unsigned int de0F939[] = { 0x09B6F, 0x00000 };
static unsigned int de0F93A[] = { 0x09DFA, 0x00000 };
static unsigned int de0F93B[] = { 0x0788C, 0x00000 };
static unsigned int de0F93C[] = { 0x0797F, 0x00000 };
static unsigned int de0F93D[] = { 0x07DA0, 0x00000 };
static unsigned int de0F93E[] = { 0x083C9, 0x00000 };
static unsigned int de0F93F[] = { 0x09304, 0x00000 };
static unsigned int de0F940[] = { 0x09E7F, 0x00000 };
static unsigned int de0F941[] = { 0x08AD6, 0x00000 };
static unsigned int de0F942[] = { 0x058DF, 0x00000 };
static unsigned int de0F943[] = { 0x05F04, 0x00000 };
static unsigned int de0F944[] = { 0x07C60, 0x00000 };
static unsigned int de0F945[] = { 0x0807E, 0x00000 };
static unsigned int de0F946[] = { 0x07262, 0x00000 };
static unsigned int de0F947[] = { 0x078CA, 0x00000 };
static unsigned int de0F948[] = { 0x08CC2, 0x00000 };
static unsigned int de0F949[] = { 0x096F7, 0x00000 };
static unsigned int de0F94A[] = { 0x058D8, 0x00000 };
static unsigned int de0F94B[] = { 0x05C62, 0x00000 };
static unsigned int de0F94C[] = { 0x06A13, 0x00000 };
static unsigned int de0F94D[] = { 0x06DDA, 0x00000 };
static unsigned int de0F94E[] = { 0x06F0F, 0x00000 };
static unsigned int de0F94F[] = { 0x07D2F, 0x00000 };
static unsigned int de0F950[] = { 0x07E37, 0x00000 };
static unsigned int de0F951[] = { 0x0964B, 0x00000 };
static unsigned int de0F952[] = { 0x052D2, 0x00000 };
static unsigned int de0F953[] = { 0x0808B, 0x00000 };
static unsigned int de0F954[] = { 0x051DC, 0x00000 };
static unsigned int de0F955[] = { 0x051CC, 0x00000 };
static unsigned int de0F956[] = { 0x07A1C, 0x00000 };
static unsigned int de0F957[] = { 0x07DBE, 0x00000 };
static unsigned int de0F958[] = { 0x083F1, 0x00000 };
static unsigned int de0F959[] = { 0x09675, 0x00000 };
static unsigned int de0F95A[] = { 0x08B80, 0x00000 };
static unsigned int de0F95B[] = { 0x062CF, 0x00000 };
static unsigned int de0F95C[] = { 0x06A02, 0x00000 };
static unsigned int de0F95D[] = { 0x08AFE, 0x00000 };
static unsigned int de0F95E[] = { 0x04E39, 0x00000 };
static unsigned int de0F95F[] = { 0x05BE7, 0x00000 };
static unsigned int de0F960[] = { 0x06012, 0x00000 };
static unsigned int de0F961[] = { 0x07387, 0x00000 };
static unsigned int de0F962[] = { 0x07570, 0x00000 };
static unsigned int de0F963[] = { 0x05317, 0x00000 };
static unsigned int de0F964[] = { 0x078FB, 0x00000 };
static unsigned int de0F965[] = { 0x04FBF, 0x00000 };
static unsigned int de0F966[] = { 0x05FA9, 0x00000 };
static unsigned int de0F967[] = { 0x04E0D, 0x00000 };
static unsigned int de0F968[] = { 0x06CCC, 0x00000 };
static unsigned int de0F969[] = { 0x06578, 0x00000 };
static unsigned int de0F96A[] = { 0x07D22, 0x00000 };
static unsigned int de0F96B[] = { 0x053C3, 0x00000 };
static unsigned int de0F96C[] = { 0x0585E, 0x00000 };
static unsigned int de0F96D[] = { 0x07701, 0x00000 };
static unsigned int de0F96E[] = { 0x08449, 0x00000 };
static unsigned int de0F96F[] = { 0x08AAA, 0x00000 };
static unsigned int de0F970[] = { 0x06BBA, 0x00000 };
static unsigned int de0F971[] = { 0x08FB0, 0x00000 };
static unsigned int de0F972[] = { 0x06C88, 0x00000 };
static unsigned int de0F973[] = { 0x062FE, 0x00000 };
static unsigned int de0F974[] = { 0x082E5, 0x00000 };
static unsigned int de0F975[] = { 0x063A0, 0x00000 };
static unsigned int de0F976[] = { 0x07565, 0x00000 };
static unsigned int de0F977[] = { 0x04EAE, 0x00000 };
static unsigned int de0F978[] = { 0x05169, 0x00000 };
static unsigned int de0F979[] = { 0x051C9, 0x00000 };
static unsigned int de0F97A[] = { 0x06881, 0x00000 };
static unsigned int de0F97B[] = { 0x07CE7, 0x00000 };
static unsigned int de0F97C[] = { 0x0826F, 0x00000 };
static unsigned int de0F97D[] = { 0x08AD2, 0x00000 };
static unsigned int de0F97E[] = { 0x091CF, 0x00000 };
static unsigned int de0F97F[] = { 0x052F5, 0x00000 };
static unsigned int de0F980[] = { 0x05442, 0x00000 };
static unsigned int de0F981[] = { 0x05973, 0x00000 };
static unsigned int de0F982[] = { 0x05EEC, 0x00000 };
static unsigned int de0F983[] = { 0x065C5, 0x00000 };
static unsigned int de0F984[] = { 0x06FFE, 0x00000 };
static unsigned int de0F985[] = { 0x0792A, 0x00000 };
static unsigned int de0F986[] = { 0x095AD, 0x00000 };
static unsigned int de0F987[] = { 0x09A6A, 0x00000 };
static unsigned int de0F988[] = { 0x09E97, 0x00000 };
static unsigned int de0F989[] = { 0x09ECE, 0x00000 };
static unsigned int de0F98A[] = { 0x0529B, 0x00000 };
static unsigned int de0F98B[] = { 0x066C6, 0x00000 };
static unsigned int de0F98C[] = { 0x06B77, 0x00000 };
static unsigned int de0F98D[] = { 0x08F62, 0x00000 };
static unsigned int de0F98E[] = { 0x05E74, 0x00000 };
static unsigned int de0F98F[] = { 0x06190, 0x00000 };
static unsigned int de0F990[] = { 0x06200, 0x00000 };
static unsigned int de0F991[] = { 0x0649A, 0x00000 };
static unsigned int de0F992[] = { 0x06F23, 0x00000 };
static unsigned int de0F993[] = { 0x07149, 0x00000 };
static unsigned int de0F994[] = { 0x07489, 0x00000 };
static unsigned int de0F995[] = { 0x079CA, 0x00000 };
static unsigned int de0F996[] = { 0x07DF4, 0x00000 };
static unsigned int de0F997[] = { 0x0806F, 0x00000 };
static unsigned int de0F998[] = { 0x08F26, 0x00000 };
static unsigned int de0F999[] = { 0x084EE, 0x00000 };
static unsigned int de0F99A[] = { 0x09023, 0x00000 };
static unsigned int de0F99B[] = { 0x0934A, 0x00000 };
static unsigned int de0F99C[] = { 0x05217, 0x00000 };
static unsigned int de0F99D[] = { 0x052A3, 0x00000 };
static unsigned int de0F99E[] = { 0x054BD, 0x00000 };
static unsigned int de0F99F[] = { 0x070C8, 0x00000 };
static unsigned int de0F9A0[] = { 0x088C2, 0x00000 };
static unsigned int de0F9A1[] = { 0x08AAA, 0x00000 };
static unsigned int de0F9A2[] = { 0x05EC9, 0x00000 };
static unsigned int de0F9A3[] = { 0x05FF5, 0x00000 };
static unsigned int de0F9A4[] = { 0x0637B, 0x00000 };
static unsigned int de0F9A5[] = { 0x06BAE, 0x00000 };
static unsigned int de0F9A6[] = { 0x07C3E, 0x00000 };
static unsigned int de0F9A7[] = { 0x07375, 0x00000 };
static unsigned int de0F9A8[] = { 0x04EE4, 0x00000 };
static unsigned int de0F9A9[] = { 0x056F9, 0x00000 };
static unsigned int de0F9AA[] = { 0x05BE7, 0x00000 };
static unsigned int de0F9AB[] = { 0x05DBA, 0x00000 };
static unsigned int de0F9AC[] = { 0x0601C, 0x00000 };
static unsigned int de0F9AD[] = { 0x073B2, 0x00000 };
static unsigned int de0F9AE[] = { 0x07469, 0x00000 };
static unsigned int de0F9AF[] = { 0x07F9A, 0x00000 };
static unsigned int de0F9B0[] = { 0x08046, 0x00000 };
static unsigned int de0F9B1[] = { 0x09234, 0x00000 };
static unsigned int de0F9B2[] = { 0x096F6, 0x00000 };
static unsigned int de0F9B3[] = { 0x09748, 0x00000 };
static unsigned int de0F9B4[] = { 0x09818, 0x00000 };
static unsigned int de0F9B5[] = { 0x04F8B, 0x00000 };
static unsigned int de0F9B6[] = { 0x079AE, 0x00000 };
static unsigned int de0F9B7[] = { 0x091B4, 0x00000 };
static unsigned int de0F9B8[] = { 0x096B8, 0x00000 };
static unsigned int de0F9B9[] = { 0x060E1, 0x00000 };
static unsigned int de0F9BA[] = { 0x04E86, 0x00000 };
static unsigned int de0F9BB[] = { 0x050DA, 0x00000 };
static unsigned int de0F9BC[] = { 0x05BEE, 0x00000 };
static unsigned int de0F9BD[] = { 0x05C3F, 0x00000 };
static unsigned int de0F9BE[] = { 0x06599, 0x00000 };
static unsigned int de0F9BF[] = { 0x06A02, 0x00000 };
static unsigned int de0F9C0[] = { 0x071CE, 0x00000 };
static unsigned int de0F9C1[] = { 0x07642, 0x00000 };
static unsigned int de0F9C2[] = { 0x084FC, 0x00000 };
static unsigned int de0F9C3[] = { 0x0907C, 0x00000 };
static unsigned int de0F9C4[] = { 0x09F8D, 0x00000 };
static unsigned int de0F9C5[] = { 0x06688, 0x00000 };
static unsigned int de0F9C6[] = { 0x0962E, 0x00000 };
static unsigned int de0F9C7[] = { 0x05289, 0x00000 };
static unsigned int de0F9C8[] = { 0x0677B, 0x00000 };
static unsigned int de0F9C9[] = { 0x067F3, 0x00000 };
static unsigned int de0F9CA[] = { 0x06D41, 0x00000 };
static unsigned int de0F9CB[] = { 0x06E9C, 0x00000 };
static unsigned int de0F9CC[] = { 0x07409, 0x00000 };
static unsigned int de0F9CD[] = { 0x07559, 0x00000 };
static unsigned int de0F9CE[] = { 0x0786B, 0x00000 };
static unsigned int de0F9CF[] = { 0x07D10, 0x00000 };
static unsigned int de0F9D0[] = { 0x0985E, 0x00000 };
static unsigned int de0F9D1[] = { 0x0516D, 0x00000 };
static unsigned int de0F9D2[] = { 0x0622E, 0x00000 };
static unsigned int de0F9D3[] = { 0x09678, 0x00000 };
static unsigned int de0F9D4[] = { 0x0502B, 0x00000 };
static unsigned int de0F9D5[] = { 0x05D19, 0x00000 };
static unsigned int de0F9D6[] = { 0x06DEA, 0x00000 };
static unsigned int de0F9D7[] = { 0x08F2A, 0x00000 };
static unsigned int de0F9D8[] = { 0x05F8B, 0x00000 };
static unsigned int de0F9D9[] = { 0x06144, 0x00000 };
static unsigned int de0F9DA[] = { 0x06817, 0x00000 };
static unsigned int de0F9DB[] = { 0x07387, 0x00000 };
static unsigned int de0F9DC[] = { 0x09686, 0x00000 };
static unsigned int de0F9DD[] = { 0x05229, 0x00000 };
static unsigned int de0F9DE[] = { 0x0540F, 0x00000 };
static unsigned int de0F9DF[] = { 0x05C65, 0x00000 };
static unsigned int de0F9E0[] = { 0x06613, 0x00000 };
static unsigned int de0F9E1[] = { 0x0674E, 0x00000 };
static unsigned int de0F9E2[] = { 0x068A8, 0x00000 };
static unsigned int de0F9E3[] = { 0x06CE5, 0x00000 };
static unsigned int de0F9E4[] = { 0x07406, 0x00000 };
static unsigned int de0F9E5[] = { 0x075E2, 0x00000 };
static unsigned int de0F9E6[] = { 0x07F79, 0x00000 };
static unsigned int de0F9E7[] = { 0x088CF, 0x00000 };
static unsigned int de0F9E8[] = { 0x088E1, 0x00000 };
static unsigned int de0F9E9[] = { 0x091CC, 0x00000 };
static unsigned int de0F9EA[] = { 0x096E2, 0x00000 };
static unsigned int de0F9EB[] = { 0x0533F, 0x00000 };
static unsigned int de0F9EC[] = { 0x06EBA, 0x00000 };
static unsigned int de0F9ED[] = { 0x0541D, 0x00000 };
static unsigned int de0F9EE[] = { 0x071D0, 0x00000 };
static unsigned int de0F9EF[] = { 0x07498, 0x00000 };
static unsigned int de0F9F0[] = { 0x085FA, 0x00000 };
static unsigned int de0F9F1[] = { 0x096A3, 0x00000 };
static unsigned int de0F9F2[] = { 0x09C57, 0x00000 };
static unsigned int de0F9F3[] = { 0x09E9F, 0x00000 };
static unsigned int de0F9F4[] = { 0x06797, 0x00000 };
static unsigned int de0F9F5[] = { 0x06DCB, 0x00000 };
static unsigned int de0F9F6[] = { 0x081E8, 0x00000 };
static unsigned int de0F9F7[] = { 0x07ACB, 0x00000 };
static unsigned int de0F9F8[] = { 0x07B20, 0x00000 };
static unsigned int de0F9F9[] = { 0x07C92, 0x00000 };
static unsigned int de0F9FA[] = { 0x072C0, 0x00000 };
static unsigned int de0F9FB[] = { 0x07099, 0x00000 };
static unsigned int de0F9FC[] = { 0x08B58, 0x00000 };
static unsigned int de0F9FD[] = { 0x04EC0, 0x00000 };
static unsigned int de0F9FE[] = { 0x08336, 0x00000 };
static unsigned int de0F9FF[] = { 0x0523A, 0x00000 };
static unsigned int de0FA00[] = { 0x05207, 0x00000 };
static unsigned int de0FA01[] = { 0x05EA6, 0x00000 };
static unsigned int de0FA02[] = { 0x062D3, 0x00000 };
static unsigned int de0FA03[] = { 0x07CD6, 0x00000 };
static unsigned int de0FA04[] = { 0x05B85, 0x00000 };
static unsigned int de0FA05[] = { 0x06D1E, 0x00000 };
static unsigned int de0FA06[] = { 0x066B4, 0x00000 };
static unsigned int de0FA07[] = { 0x08F3B, 0x00000 };
static unsigned int de0FA08[] = { 0x0884C, 0x00000 };
static unsigned int de0FA09[] = { 0x0964D, 0x00000 };
static unsigned int de0FA0A[] = { 0x0898B, 0x00000 };
static unsigned int de0FA0B[] = { 0x05ED3, 0x00000 };
static unsigned int de0FA0C[] = { 0x05140, 0x00000 };
static unsigned int de0FA0D[] = { 0x055C0, 0x00000 };
static unsigned int de0FA10[] = { 0x0585A, 0x00000 };
static unsigned int de0FA12[] = { 0x06674, 0x00000 };
static unsigned int de0FA15[] = { 0x051DE, 0x00000 };
static unsigned int de0FA16[] = { 0x0732A, 0x00000 };
static unsigned int de0FA17[] = { 0x076CA, 0x00000 };
static unsigned int de0FA18[] = { 0x0793C, 0x00000 };
static unsigned int de0FA19[] = { 0x0795E, 0x00000 };
static unsigned int de0FA1A[] = { 0x07965, 0x00000 };
static unsigned int de0FA1B[] = { 0x0798F, 0x00000 };
static unsigned int de0FA1C[] = { 0x09756, 0x00000 };
static unsigned int de0FA1D[] = { 0x07CBE, 0x00000 };
static unsigned int de0FA1E[] = { 0x07FBD, 0x00000 };
static unsigned int de0FA20[] = { 0x08612, 0x00000 };
static unsigned int de0FA22[] = { 0x08AF8, 0x00000 };
static unsigned int de0FA25[] = { 0x09038, 0x00000 };
static unsigned int de0FA26[] = { 0x090FD, 0x00000 };
static unsigned int de0FA2A[] = { 0x098EF, 0x00000 };
static unsigned int de0FA2B[] = { 0x098FC, 0x00000 };
static unsigned int de0FA2C[] = { 0x09928, 0x00000 };
static unsigned int de0FA2D[] = { 0x09DB4, 0x00000 };
static unsigned int de0FA30[] = { 0x04FAE, 0x00000 };
static unsigned int de0FA31[] = { 0x050E7, 0x00000 };
static unsigned int de0FA32[] = { 0x0514D, 0x00000 };
static unsigned int de0FA33[] = { 0x052C9, 0x00000 };
static unsigned int de0FA34[] = { 0x052E4, 0x00000 };
static unsigned int de0FA35[] = { 0x05351, 0x00000 };
static unsigned int de0FA36[] = { 0x0559D, 0x00000 };
static unsigned int de0FA37[] = { 0x05606, 0x00000 };
static unsigned int de0FA38[] = { 0x05668, 0x00000 };
static unsigned int de0FA39[] = { 0x05840, 0x00000 };
static unsigned int de0FA3A[] = { 0x058A8, 0x00000 };
static unsigned int de0FA3B[] = { 0x05C64, 0x00000 };
static unsigned int de0FA3C[] = { 0x05C6E, 0x00000 };
static unsigned int de0FA3D[] = { 0x06094, 0x00000 };
static unsigned int de0FA3E[] = { 0x06168, 0x00000 };
static unsigned int de0FA3F[] = { 0x0618E, 0x00000 };
static unsigned int de0FA40[] = { 0x061F2, 0x00000 };
static unsigned int de0FA41[] = { 0x0654F, 0x00000 };
static unsigned int de0FA42[] = { 0x065E2, 0x00000 };
static unsigned int de0FA43[] = { 0x06691, 0x00000 };
static unsigned int de0FA44[] = { 0x06885, 0x00000 };
static unsigned int de0FA45[] = { 0x06D77, 0x00000 };
static unsigned int de0FA46[] = { 0x06E1A, 0x00000 };
static unsigned int de0FA47[] = { 0x06F22, 0x00000 };
static unsigned int de0FA48[] = { 0x0716E, 0x00000 };
static unsigned int de0FA49[] = { 0x0722B, 0x00000 };
static unsigned int de0FA4A[] = { 0x07422, 0x00000 };
static unsigned int de0FA4B[] = { 0x07891, 0x00000 };
static unsigned int de0FA4C[] = { 0x0793E, 0x00000 };
static unsigned int de0FA4D[] = { 0x07949, 0x00000 };
static unsigned int de0FA4E[] = { 0x07948, 0x00000 };
static unsigned int de0FA4F[] = { 0x07950, 0x00000 };
static unsigned int de0FA50[] = { 0x07956, 0x00000 };
static unsigned int de0FA51[] = { 0x0795D, 0x00000 };
static unsigned int de0FA52[] = { 0x0798D, 0x00000 };
static unsigned int de0FA53[] = { 0x0798E, 0x00000 };
static unsigned int de0FA54[] = { 0x07A40, 0x00000 };
static unsigned int de0FA55[] = { 0x07A81, 0x00000 };
static unsigned int de0FA56[] = { 0x07BC0, 0x00000 };
static unsigned int de0FA57[] = { 0x07DF4, 0x00000 };
static unsigned int de0FA58[] = { 0x07E09, 0x00000 };
static unsigned int de0FA59[] = { 0x07E41, 0x00000 };
static unsigned int de0FA5A[] = { 0x07F72, 0x00000 };
static unsigned int de0FA5B[] = { 0x08005, 0x00000 };
static unsigned int de0FA5C[] = { 0x081ED, 0x00000 };
static unsigned int de0FA5D[] = { 0x08279, 0x00000 };
static unsigned int de0FA5E[] = { 0x08279, 0x00000 };
static unsigned int de0FA5F[] = { 0x08457, 0x00000 };
static unsigned int de0FA60[] = { 0x08910, 0x00000 };
static unsigned int de0FA61[] = { 0x08996, 0x00000 };
static unsigned int de0FA62[] = { 0x08B01, 0x00000 };
static unsigned int de0FA63[] = { 0x08B39, 0x00000 };
static unsigned int de0FA64[] = { 0x08CD3, 0x00000 };
static unsigned int de0FA65[] = { 0x08D08, 0x00000 };
static unsigned int de0FA66[] = { 0x08FB6, 0x00000 };
static unsigned int de0FA67[] = { 0x09038, 0x00000 };
static unsigned int de0FA68[] = { 0x096E3, 0x00000 };
static unsigned int de0FA69[] = { 0x097FF, 0x00000 };
static unsigned int de0FA6A[] = { 0x0983B, 0x00000 };
static unsigned int de0FA70[] = { 0x04E26, 0x00000 };
static unsigned int de0FA71[] = { 0x051B5, 0x00000 };
static unsigned int de0FA72[] = { 0x05168, 0x00000 };
static unsigned int de0FA73[] = { 0x04F80, 0x00000 };
static unsigned int de0FA74[] = { 0x05145, 0x00000 };
static unsigned int de0FA75[] = { 0x05180, 0x00000 };
static unsigned int de0FA76[] = { 0x052C7, 0x00000 };
static unsigned int de0FA77[] = { 0x052FA, 0x00000 };
static unsigned int de0FA78[] = { 0x0559D, 0x00000 };
static unsigned int de0FA79[] = { 0x05555, 0x00000 };
static unsigned int de0FA7A[] = { 0x05599, 0x00000 };
static unsigned int de0FA7B[] = { 0x055E2, 0x00000 };
static unsigned int de0FA7C[] = { 0x0585A, 0x00000 };
static unsigned int de0FA7D[] = { 0x058B3, 0x00000 };
static unsigned int de0FA7E[] = { 0x05944, 0x00000 };
static unsigned int de0FA7F[] = { 0x05954, 0x00000 };
static unsigned int de0FA80[] = { 0x05A62, 0x00000 };
static unsigned int de0FA81[] = { 0x05B28, 0x00000 };
static unsigned int de0FA82[] = { 0x05ED2, 0x00000 };
static unsigned int de0FA83[] = { 0x05ED9, 0x00000 };
static unsigned int de0FA84[] = { 0x05F69, 0x00000 };
static unsigned int de0FA85[] = { 0x05FAD, 0x00000 };
static unsigned int de0FA86[] = { 0x060D8, 0x00000 };
static unsigned int de0FA87[] = { 0x0614E, 0x00000 };
static unsigned int de0FA88[] = { 0x06108, 0x00000 };
static unsigned int de0FA89[] = { 0x0618E, 0x00000 };
static unsigned int de0FA8A[] = { 0x06160, 0x00000 };
static unsigned int de0FA8B[] = { 0x061F2, 0x00000 };
static unsigned int de0FA8C[] = { 0x06234, 0x00000 };
static unsigned int de0FA8D[] = { 0x063C4, 0x00000 };
static unsigned int de0FA8E[] = { 0x0641C, 0x00000 };
static unsigned int de0FA8F[] = { 0x06452, 0x00000 };
static unsigned int de0FA90[] = { 0x06556, 0x00000 };
static unsigned int de0FA91[] = { 0x06674, 0x00000 };
static unsigned int de0FA92[] = { 0x06717, 0x00000 };
static unsigned int de0FA93[] = { 0x0671B, 0x00000 };
static unsigned int de0FA94[] = { 0x06756, 0x00000 };
static unsigned int de0FA95[] = { 0x06B79, 0x00000 };
static unsigned int de0FA96[] = { 0x06BBA, 0x00000 };
static unsigned int de0FA97[] = { 0x06D41, 0x00000 };
static unsigned int de0FA98[] = { 0x06EDB, 0x00000 };
static unsigned int de0FA99[] = { 0x06ECB, 0x00000 };
static unsigned int de0FA9A[] = { 0x06F22, 0x00000 };
static unsigned int de0FA9B[] = { 0x0701E, 0x00000 };
static unsigned int de0FA9C[] = { 0x0716E, 0x00000 };
static unsigned int de0FA9D[] = { 0x077A7, 0x00000 };
static unsigned int de0FA9E[] = { 0x07235, 0x00000 };
static unsigned int de0FA9F[] = { 0x072AF, 0x00000 };
static unsigned int de0FAA0[] = { 0x0732A, 0x00000 };
static unsigned int de0FAA1[] = { 0x07471, 0x00000 };
static unsigned int de0FAA2[] = { 0x07506, 0x00000 };
static unsigned int de0FAA3[] = { 0x0753B, 0x00000 };
static unsigned int de0FAA4[] = { 0x0761D, 0x00000 };
static unsigned int de0FAA5[] = { 0x0761F, 0x00000 };
static unsigned int de0FAA6[] = { 0x076CA, 0x00000 };
static unsigned int de0FAA7[] = { 0x076DB, 0x00000 };
static unsigned int de0FAA8[] = { 0x076F4, 0x00000 };
static unsigned int de0FAA9[] = { 0x0774A, 0x00000 };
static unsigned int de0FAAA[] = { 0x07740, 0x00000 };
static unsigned int de0FAAB[] = { 0x078CC, 0x00000 };
static unsigned int de0FAAC[] = { 0x07AB1, 0x00000 };
static unsigned int de0FAAD[] = { 0x07BC0, 0x00000 };
static unsigned int de0FAAE[] = { 0x07C7B, 0x00000 };
static unsigned int de0FAAF[] = { 0x07D5B, 0x00000 };
static unsigned int de0FAB0[] = { 0x07DF4, 0x00000 };
static unsigned int de0FAB1[] = { 0x07F3E, 0x00000 };
static unsigned int de0FAB2[] = { 0x08005, 0x00000 };
static unsigned int de0FAB3[] = { 0x08352, 0x00000 };
static unsigned int de0FAB4[] = { 0x083EF, 0x00000 };
static unsigned int de0FAB5[] = { 0x08779, 0x00000 };
static unsigned int de0FAB6[] = { 0x08941, 0x00000 };
static unsigned int de0FAB7[] = { 0x08986, 0x00000 };
static unsigned int de0FAB8[] = { 0x08996, 0x00000 };
static unsigned int de0FAB9[] = { 0x08ABF, 0x00000 };
static unsigned int de0FABA[] = { 0x08AF8, 0x00000 };
static unsigned int de0FABB[] = { 0x08ACB, 0x00000 };
static unsigned int de0FABC[] = { 0x08B01, 0x00000 };
static unsigned int de0FABD[] = { 0x08AFE, 0x00000 };
static unsigned int de0FABE[] = { 0x08AED, 0x00000 };
static unsigned int de0FABF[] = { 0x08B39, 0x00000 };
static unsigned int de0FAC0[] = { 0x08B8A, 0x00000 };
static unsigned int de0FAC1[] = { 0x08D08, 0x00000 };
static unsigned int de0FAC2[] = { 0x08F38, 0x00000 };
static unsigned int de0FAC3[] = { 0x09072, 0x00000 };
static unsigned int de0FAC4[] = { 0x09199, 0x00000 };
static unsigned int de0FAC5[] = { 0x09276, 0x00000 };
static unsigned int de0FAC6[] = { 0x0967C, 0x00000 };
static unsigned int de0FAC7[] = { 0x096E3, 0x00000 };
static unsigned int de0FAC8[] = { 0x09756, 0x00000 };
static unsigned int de0FAC9[] = { 0x097DB, 0x00000 };
static unsigned int de0FACA[] = { 0x097FF, 0x00000 };
static unsigned int de0FACB[] = { 0x0980B, 0x00000 };
static unsigned int de0FACC[] = { 0x0983B, 0x00000 };
static unsigned int de0FACD[] = { 0x09B12, 0x00000 };
static unsigned int de0FACE[] = { 0x09F9C, 0x00000 };
static unsigned int de0FACF[] = { 0x2284A, 0x00000 };
static unsigned int de0FAD0[] = { 0x22844, 0x00000 };
static unsigned int de0FAD1[] = { 0x233D5, 0x00000 };
static unsigned int de0FAD2[] = { 0x03B9D, 0x00000 };
static unsigned int de0FAD3[] = { 0x04018, 0x00000 };
static unsigned int de0FAD4[] = { 0x04039, 0x00000 };
static unsigned int de0FAD5[] = { 0x25249, 0x00000 };
static unsigned int de0FAD6[] = { 0x25CD0, 0x00000 };
static unsigned int de0FAD7[] = { 0x27ED3, 0x00000 };
static unsigned int de0FAD8[] = { 0x09F43, 0x00000 };
static unsigned int de0FAD9[] = { 0x09F8E, 0x00000 };
static unsigned int de0FB00[] = { 0x00066, 0x00066, 0x00000 };
static unsigned int de0FB01[] = { 0x00066, 0x00069, 0x00000 };
static unsigned int de0FB02[] = { 0x00066, 0x0006C, 0x00000 };
static unsigned int de0FB03[] = { 0x00066, 0x00066, 0x00069, 0x00000 };
static unsigned int de0FB04[] = { 0x00066, 0x00066, 0x0006C, 0x00000 };
static unsigned int de0FB05[] = { 0x0017F, 0x00074, 0x00000 };
static unsigned int de0FB06[] = { 0x00073, 0x00074, 0x00000 };
static unsigned int de0FB13[] = { 0x00574, 0x00576, 0x00000 };
static unsigned int de0FB14[] = { 0x00574, 0x00565, 0x00000 };
static unsigned int de0FB15[] = { 0x00574, 0x0056B, 0x00000 };
static unsigned int de0FB16[] = { 0x0057E, 0x00576, 0x00000 };
static unsigned int de0FB17[] = { 0x00574, 0x0056D, 0x00000 };
static unsigned int de0FB1D[] = { 0x005D9, 0x005B4, 0x00000 };
static unsigned int de0FB1F[] = { 0x005F2, 0x005B7, 0x00000 };
static unsigned int de0FB20[] = { 0x005E2, 0x00000 };
static unsigned int de0FB21[] = { 0x005D0, 0x00000 };
static unsigned int de0FB22[] = { 0x005D3, 0x00000 };
static unsigned int de0FB23[] = { 0x005D4, 0x00000 };
static unsigned int de0FB24[] = { 0x005DB, 0x00000 };
static unsigned int de0FB25[] = { 0x005DC, 0x00000 };
static unsigned int de0FB26[] = { 0x005DD, 0x00000 };
static unsigned int de0FB27[] = { 0x005E8, 0x00000 };
static unsigned int de0FB28[] = { 0x005EA, 0x00000 };
static unsigned int de0FB29[] = { 0x0002B, 0x00000 };
static unsigned int de0FB2A[] = { 0x005E9, 0x005C1, 0x00000 };
static unsigned int de0FB2B[] = { 0x005E9, 0x005C2, 0x00000 };
static unsigned int de0FB2C[] = { 0x0FB49, 0x005C1, 0x00000 };
static unsigned int de0FB2D[] = { 0x0FB49, 0x005C2, 0x00000 };
static unsigned int de0FB2E[] = { 0x005D0, 0x005B7, 0x00000 };
static unsigned int de0FB2F[] = { 0x005D0, 0x005B8, 0x00000 };
static unsigned int de0FB30[] = { 0x005D0, 0x005BC, 0x00000 };
static unsigned int de0FB31[] = { 0x005D1, 0x005BC, 0x00000 };
static unsigned int de0FB32[] = { 0x005D2, 0x005BC, 0x00000 };
static unsigned int de0FB33[] = { 0x005D3, 0x005BC, 0x00000 };
static unsigned int de0FB34[] = { 0x005D4, 0x005BC, 0x00000 };
static unsigned int de0FB35[] = { 0x005D5, 0x005BC, 0x00000 };
static unsigned int de0FB36[] = { 0x005D6, 0x005BC, 0x00000 };
static unsigned int de0FB38[] = { 0x005D8, 0x005BC, 0x00000 };
static unsigned int de0FB39[] = { 0x005D9, 0x005BC, 0x00000 };
static unsigned int de0FB3A[] = { 0x005DA, 0x005BC, 0x00000 };
static unsigned int de0FB3B[] = { 0x005DB, 0x005BC, 0x00000 };
static unsigned int de0FB3C[] = { 0x005DC, 0x005BC, 0x00000 };
static unsigned int de0FB3E[] = { 0x005DE, 0x005BC, 0x00000 };
static unsigned int de0FB40[] = { 0x005E0, 0x005BC, 0x00000 };
static unsigned int de0FB41[] = { 0x005E1, 0x005BC, 0x00000 };
static unsigned int de0FB43[] = { 0x005E3, 0x005BC, 0x00000 };
static unsigned int de0FB44[] = { 0x005E4, 0x005BC, 0x00000 };
static unsigned int de0FB46[] = { 0x005E6, 0x005BC, 0x00000 };
static unsigned int de0FB47[] = { 0x005E7, 0x005BC, 0x00000 };
static unsigned int de0FB48[] = { 0x005E8, 0x005BC, 0x00000 };
static unsigned int de0FB49[] = { 0x005E9, 0x005BC, 0x00000 };
static unsigned int de0FB4A[] = { 0x005EA, 0x005BC, 0x00000 };
static unsigned int de0FB4B[] = { 0x005D5, 0x005B9, 0x00000 };
static unsigned int de0FB4C[] = { 0x005D1, 0x005BF, 0x00000 };
static unsigned int de0FB4D[] = { 0x005DB, 0x005BF, 0x00000 };
static unsigned int de0FB4E[] = { 0x005E4, 0x005BF, 0x00000 };
static unsigned int de0FB4F[] = { 0x005D0, 0x005DC, 0x00000 };
static unsigned int de0FB50[] = { 0x00671, 0x00000 };
static unsigned int de0FB51[] = { 0x00671, 0x00000 };
static unsigned int de0FB52[] = { 0x0067B, 0x00000 };
static unsigned int de0FB53[] = { 0x0067B, 0x00000 };
static unsigned int de0FB54[] = { 0x0067B, 0x00000 };
static unsigned int de0FB55[] = { 0x0067B, 0x00000 };
static unsigned int de0FB56[] = { 0x0067E, 0x00000 };
static unsigned int de0FB57[] = { 0x0067E, 0x00000 };
static unsigned int de0FB58[] = { 0x0067E, 0x00000 };
static unsigned int de0FB59[] = { 0x0067E, 0x00000 };
static unsigned int de0FB5A[] = { 0x00680, 0x00000 };
static unsigned int de0FB5B[] = { 0x00680, 0x00000 };
static unsigned int de0FB5C[] = { 0x00680, 0x00000 };
static unsigned int de0FB5D[] = { 0x00680, 0x00000 };
static unsigned int de0FB5E[] = { 0x0067A, 0x00000 };
static unsigned int de0FB5F[] = { 0x0067A, 0x00000 };
static unsigned int de0FB60[] = { 0x0067A, 0x00000 };
static unsigned int de0FB61[] = { 0x0067A, 0x00000 };
static unsigned int de0FB62[] = { 0x0067F, 0x00000 };
static unsigned int de0FB63[] = { 0x0067F, 0x00000 };
static unsigned int de0FB64[] = { 0x0067F, 0x00000 };
static unsigned int de0FB65[] = { 0x0067F, 0x00000 };
static unsigned int de0FB66[] = { 0x00679, 0x00000 };
static unsigned int de0FB67[] = { 0x00679, 0x00000 };
static unsigned int de0FB68[] = { 0x00679, 0x00000 };
static unsigned int de0FB69[] = { 0x00679, 0x00000 };
static unsigned int de0FB6A[] = { 0x006A4, 0x00000 };
static unsigned int de0FB6B[] = { 0x006A4, 0x00000 };
static unsigned int de0FB6C[] = { 0x006A4, 0x00000 };
static unsigned int de0FB6D[] = { 0x006A4, 0x00000 };
static unsigned int de0FB6E[] = { 0x006A6, 0x00000 };
static unsigned int de0FB6F[] = { 0x006A6, 0x00000 };
static unsigned int de0FB70[] = { 0x006A6, 0x00000 };
static unsigned int de0FB71[] = { 0x006A6, 0x00000 };
static unsigned int de0FB72[] = { 0x00684, 0x00000 };
static unsigned int de0FB73[] = { 0x00684, 0x00000 };
static unsigned int de0FB74[] = { 0x00684, 0x00000 };
static unsigned int de0FB75[] = { 0x00684, 0x00000 };
static unsigned int de0FB76[] = { 0x00683, 0x00000 };
static unsigned int de0FB77[] = { 0x00683, 0x00000 };
static unsigned int de0FB78[] = { 0x00683, 0x00000 };
static unsigned int de0FB79[] = { 0x00683, 0x00000 };
static unsigned int de0FB7A[] = { 0x00686, 0x00000 };
static unsigned int de0FB7B[] = { 0x00686, 0x00000 };
static unsigned int de0FB7C[] = { 0x00686, 0x00000 };
static unsigned int de0FB7D[] = { 0x00686, 0x00000 };
static unsigned int de0FB7E[] = { 0x00687, 0x00000 };
static unsigned int de0FB7F[] = { 0x00687, 0x00000 };
static unsigned int de0FB80[] = { 0x00687, 0x00000 };
static unsigned int de0FB81[] = { 0x00687, 0x00000 };
static unsigned int de0FB82[] = { 0x0068D, 0x00000 };
static unsigned int de0FB83[] = { 0x0068D, 0x00000 };
static unsigned int de0FB84[] = { 0x0068C, 0x00000 };
static unsigned int de0FB85[] = { 0x0068C, 0x00000 };
static unsigned int de0FB86[] = { 0x0068E, 0x00000 };
static unsigned int de0FB87[] = { 0x0068E, 0x00000 };
static unsigned int de0FB88[] = { 0x00688, 0x00000 };
static unsigned int de0FB89[] = { 0x00688, 0x00000 };
static unsigned int de0FB8A[] = { 0x00698, 0x00000 };
static unsigned int de0FB8B[] = { 0x00698, 0x00000 };
static unsigned int de0FB8C[] = { 0x00691, 0x00000 };
static unsigned int de0FB8D[] = { 0x00691, 0x00000 };
static unsigned int de0FB8E[] = { 0x006A9, 0x00000 };
static unsigned int de0FB8F[] = { 0x006A9, 0x00000 };
static unsigned int de0FB90[] = { 0x006A9, 0x00000 };
static unsigned int de0FB91[] = { 0x006A9, 0x00000 };
static unsigned int de0FB92[] = { 0x006AF, 0x00000 };
static unsigned int de0FB93[] = { 0x006AF, 0x00000 };
static unsigned int de0FB94[] = { 0x006AF, 0x00000 };
static unsigned int de0FB95[] = { 0x006AF, 0x00000 };
static unsigned int de0FB96[] = { 0x006B3, 0x00000 };
static unsigned int de0FB97[] = { 0x006B3, 0x00000 };
static unsigned int de0FB98[] = { 0x006B3, 0x00000 };
static unsigned int de0FB99[] = { 0x006B3, 0x00000 };
static unsigned int de0FB9A[] = { 0x006B1, 0x00000 };
static unsigned int de0FB9B[] = { 0x006B1, 0x00000 };
static unsigned int de0FB9C[] = { 0x006B1, 0x00000 };
static unsigned int de0FB9D[] = { 0x006B1, 0x00000 };
static unsigned int de0FB9E[] = { 0x006BA, 0x00000 };
static unsigned int de0FB9F[] = { 0x006BA, 0x00000 };
static unsigned int de0FBA0[] = { 0x006BB, 0x00000 };
static unsigned int de0FBA1[] = { 0x006BB, 0x00000 };
static unsigned int de0FBA2[] = { 0x006BB, 0x00000 };
static unsigned int de0FBA3[] = { 0x006BB, 0x00000 };
static unsigned int de0FBA4[] = { 0x006C0, 0x00000 };
static unsigned int de0FBA5[] = { 0x006C0, 0x00000 };
static unsigned int de0FBA6[] = { 0x006C1, 0x00000 };
static unsigned int de0FBA7[] = { 0x006C1, 0x00000 };
static unsigned int de0FBA8[] = { 0x006C1, 0x00000 };
static unsigned int de0FBA9[] = { 0x006C1, 0x00000 };
static unsigned int de0FBAA[] = { 0x006BE, 0x00000 };
static unsigned int de0FBAB[] = { 0x006BE, 0x00000 };
static unsigned int de0FBAC[] = { 0x006BE, 0x00000 };
static unsigned int de0FBAD[] = { 0x006BE, 0x00000 };
static unsigned int de0FBAE[] = { 0x006D2, 0x00000 };
static unsigned int de0FBAF[] = { 0x006D2, 0x00000 };
static unsigned int de0FBB0[] = { 0x006D3, 0x00000 };
static unsigned int de0FBB1[] = { 0x006D3, 0x00000 };
static unsigned int de0FBD3[] = { 0x006AD, 0x00000 };
static unsigned int de0FBD4[] = { 0x006AD, 0x00000 };
static unsigned int de0FBD5[] = { 0x006AD, 0x00000 };
static unsigned int de0FBD6[] = { 0x006AD, 0x00000 };
static unsigned int de0FBD7[] = { 0x006C7, 0x00000 };
static unsigned int de0FBD8[] = { 0x006C7, 0x00000 };
static unsigned int de0FBD9[] = { 0x006C6, 0x00000 };
static unsigned int de0FBDA[] = { 0x006C6, 0x00000 };
static unsigned int de0FBDB[] = { 0x006C8, 0x00000 };
static unsigned int de0FBDC[] = { 0x006C8, 0x00000 };
static unsigned int de0FBDD[] = { 0x00677, 0x00000 };
static unsigned int de0FBDE[] = { 0x006CB, 0x00000 };
static unsigned int de0FBDF[] = { 0x006CB, 0x00000 };
static unsigned int de0FBE0[] = { 0x006C5, 0x00000 };
static unsigned int de0FBE1[] = { 0x006C5, 0x00000 };
static unsigned int de0FBE2[] = { 0x006C9, 0x00000 };
static unsigned int de0FBE3[] = { 0x006C9, 0x00000 };
static unsigned int de0FBE4[] = { 0x006D0, 0x00000 };
static unsigned int de0FBE5[] = { 0x006D0, 0x00000 };
static unsigned int de0FBE6[] = { 0x006D0, 0x00000 };
static unsigned int de0FBE7[] = { 0x006D0, 0x00000 };
static unsigned int de0FBE8[] = { 0x00649, 0x00000 };
static unsigned int de0FBE9[] = { 0x00649, 0x00000 };
static unsigned int de0FBEA[] = { 0x00626, 0x00627, 0x00000 };
static unsigned int de0FBEB[] = { 0x00626, 0x00627, 0x00000 };
static unsigned int de0FBEC[] = { 0x00626, 0x006D5, 0x00000 };
static unsigned int de0FBED[] = { 0x00626, 0x006D5, 0x00000 };
static unsigned int de0FBEE[] = { 0x00626, 0x00648, 0x00000 };
static unsigned int de0FBEF[] = { 0x00626, 0x00648, 0x00000 };
static unsigned int de0FBF0[] = { 0x00626, 0x006C7, 0x00000 };
static unsigned int de0FBF1[] = { 0x00626, 0x006C7, 0x00000 };
static unsigned int de0FBF2[] = { 0x00626, 0x006C6, 0x00000 };
static unsigned int de0FBF3[] = { 0x00626, 0x006C6, 0x00000 };
static unsigned int de0FBF4[] = { 0x00626, 0x006C8, 0x00000 };
static unsigned int de0FBF5[] = { 0x00626, 0x006C8, 0x00000 };
static unsigned int de0FBF6[] = { 0x00626, 0x006D0, 0x00000 };
static unsigned int de0FBF7[] = { 0x00626, 0x006D0, 0x00000 };
static unsigned int de0FBF8[] = { 0x00626, 0x006D0, 0x00000 };
static unsigned int de0FBF9[] = { 0x00626, 0x00649, 0x00000 };
static unsigned int de0FBFA[] = { 0x00626, 0x00649, 0x00000 };
static unsigned int de0FBFB[] = { 0x00626, 0x00649, 0x00000 };
static unsigned int de0FBFC[] = { 0x006CC, 0x00000 };
static unsigned int de0FBFD[] = { 0x006CC, 0x00000 };
static unsigned int de0FBFE[] = { 0x006CC, 0x00000 };
static unsigned int de0FBFF[] = { 0x006CC, 0x00000 };
static unsigned int de0FC00[] = { 0x00626, 0x0062C, 0x00000 };
static unsigned int de0FC01[] = { 0x00626, 0x0062D, 0x00000 };
static unsigned int de0FC02[] = { 0x00626, 0x00645, 0x00000 };
static unsigned int de0FC03[] = { 0x00626, 0x00649, 0x00000 };
static unsigned int de0FC04[] = { 0x00626, 0x0064A, 0x00000 };
static unsigned int de0FC05[] = { 0x00628, 0x0062C, 0x00000 };
static unsigned int de0FC06[] = { 0x00628, 0x0062D, 0x00000 };
static unsigned int de0FC07[] = { 0x00628, 0x0062E, 0x00000 };
static unsigned int de0FC08[] = { 0x00628, 0x00645, 0x00000 };
static unsigned int de0FC09[] = { 0x00628, 0x00649, 0x00000 };
static unsigned int de0FC0A[] = { 0x00628, 0x0064A, 0x00000 };
static unsigned int de0FC0B[] = { 0x0062A, 0x0062C, 0x00000 };
static unsigned int de0FC0C[] = { 0x0062A, 0x0062D, 0x00000 };
static unsigned int de0FC0D[] = { 0x0062A, 0x0062E, 0x00000 };
static unsigned int de0FC0E[] = { 0x0062A, 0x00645, 0x00000 };
static unsigned int de0FC0F[] = { 0x0062A, 0x00649, 0x00000 };
static unsigned int de0FC10[] = { 0x0062A, 0x0064A, 0x00000 };
static unsigned int de0FC11[] = { 0x0062B, 0x0062C, 0x00000 };
static unsigned int de0FC12[] = { 0x0062B, 0x00645, 0x00000 };
static unsigned int de0FC13[] = { 0x0062B, 0x00649, 0x00000 };
static unsigned int de0FC14[] = { 0x0062B, 0x0064A, 0x00000 };
static unsigned int de0FC15[] = { 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FC16[] = { 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FC17[] = { 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FC18[] = { 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FC19[] = { 0x0062E, 0x0062C, 0x00000 };
static unsigned int de0FC1A[] = { 0x0062E, 0x0062D, 0x00000 };
static unsigned int de0FC1B[] = { 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FC1C[] = { 0x00633, 0x0062C, 0x00000 };
static unsigned int de0FC1D[] = { 0x00633, 0x0062D, 0x00000 };
static unsigned int de0FC1E[] = { 0x00633, 0x0062E, 0x00000 };
static unsigned int de0FC1F[] = { 0x00633, 0x00645, 0x00000 };
static unsigned int de0FC20[] = { 0x00635, 0x0062D, 0x00000 };
static unsigned int de0FC21[] = { 0x00635, 0x00645, 0x00000 };
static unsigned int de0FC22[] = { 0x00636, 0x0062C, 0x00000 };
static unsigned int de0FC23[] = { 0x00636, 0x0062D, 0x00000 };
static unsigned int de0FC24[] = { 0x00636, 0x0062E, 0x00000 };
static unsigned int de0FC25[] = { 0x00636, 0x00645, 0x00000 };
static unsigned int de0FC26[] = { 0x00637, 0x0062D, 0x00000 };
static unsigned int de0FC27[] = { 0x00637, 0x00645, 0x00000 };
static unsigned int de0FC28[] = { 0x00638, 0x00645, 0x00000 };
static unsigned int de0FC29[] = { 0x00639, 0x0062C, 0x00000 };
static unsigned int de0FC2A[] = { 0x00639, 0x00645, 0x00000 };
static unsigned int de0FC2B[] = { 0x0063A, 0x0062C, 0x00000 };
static unsigned int de0FC2C[] = { 0x0063A, 0x00645, 0x00000 };
static unsigned int de0FC2D[] = { 0x00641, 0x0062C, 0x00000 };
static unsigned int de0FC2E[] = { 0x00641, 0x0062D, 0x00000 };
static unsigned int de0FC2F[] = { 0x00641, 0x0062E, 0x00000 };
static unsigned int de0FC30[] = { 0x00641, 0x00645, 0x00000 };
static unsigned int de0FC31[] = { 0x00641, 0x00649, 0x00000 };
static unsigned int de0FC32[] = { 0x00641, 0x0064A, 0x00000 };
static unsigned int de0FC33[] = { 0x00642, 0x0062D, 0x00000 };
static unsigned int de0FC34[] = { 0x00642, 0x00645, 0x00000 };
static unsigned int de0FC35[] = { 0x00642, 0x00649, 0x00000 };
static unsigned int de0FC36[] = { 0x00642, 0x0064A, 0x00000 };
static unsigned int de0FC37[] = { 0x00643, 0x00627, 0x00000 };
static unsigned int de0FC38[] = { 0x00643, 0x0062C, 0x00000 };
static unsigned int de0FC39[] = { 0x00643, 0x0062D, 0x00000 };
static unsigned int de0FC3A[] = { 0x00643, 0x0062E, 0x00000 };
static unsigned int de0FC3B[] = { 0x00643, 0x00644, 0x00000 };
static unsigned int de0FC3C[] = { 0x00643, 0x00645, 0x00000 };
static unsigned int de0FC3D[] = { 0x00643, 0x00649, 0x00000 };
static unsigned int de0FC3E[] = { 0x00643, 0x0064A, 0x00000 };
static unsigned int de0FC3F[] = { 0x00644, 0x0062C, 0x00000 };
static unsigned int de0FC40[] = { 0x00644, 0x0062D, 0x00000 };
static unsigned int de0FC41[] = { 0x00644, 0x0062E, 0x00000 };
static unsigned int de0FC42[] = { 0x00644, 0x00645, 0x00000 };
static unsigned int de0FC43[] = { 0x00644, 0x00649, 0x00000 };
static unsigned int de0FC44[] = { 0x00644, 0x0064A, 0x00000 };
static unsigned int de0FC45[] = { 0x00645, 0x0062C, 0x00000 };
static unsigned int de0FC46[] = { 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FC47[] = { 0x00645, 0x0062E, 0x00000 };
static unsigned int de0FC48[] = { 0x00645, 0x00645, 0x00000 };
static unsigned int de0FC49[] = { 0x00645, 0x00649, 0x00000 };
static unsigned int de0FC4A[] = { 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FC4B[] = { 0x00646, 0x0062C, 0x00000 };
static unsigned int de0FC4C[] = { 0x00646, 0x0062D, 0x00000 };
static unsigned int de0FC4D[] = { 0x00646, 0x0062E, 0x00000 };
static unsigned int de0FC4E[] = { 0x00646, 0x00645, 0x00000 };
static unsigned int de0FC4F[] = { 0x00646, 0x00649, 0x00000 };
static unsigned int de0FC50[] = { 0x00646, 0x0064A, 0x00000 };
static unsigned int de0FC51[] = { 0x00647, 0x0062C, 0x00000 };
static unsigned int de0FC52[] = { 0x00647, 0x00645, 0x00000 };
static unsigned int de0FC53[] = { 0x00647, 0x00649, 0x00000 };
static unsigned int de0FC54[] = { 0x00647, 0x0064A, 0x00000 };
static unsigned int de0FC55[] = { 0x0064A, 0x0062C, 0x00000 };
static unsigned int de0FC56[] = { 0x0064A, 0x0062D, 0x00000 };
static unsigned int de0FC57[] = { 0x0064A, 0x0062E, 0x00000 };
static unsigned int de0FC58[] = { 0x0064A, 0x00645, 0x00000 };
static unsigned int de0FC59[] = { 0x0064A, 0x00649, 0x00000 };
static unsigned int de0FC5A[] = { 0x0064A, 0x0064A, 0x00000 };
static unsigned int de0FC5B[] = { 0x00630, 0x00670, 0x00000 };
static unsigned int de0FC5C[] = { 0x00631, 0x00670, 0x00000 };
static unsigned int de0FC5D[] = { 0x00649, 0x00670, 0x00000 };
static unsigned int de0FC5E[] = { 0x00020, 0x0064C, 0x00651, 0x00000 };
static unsigned int de0FC5F[] = { 0x00020, 0x0064D, 0x00651, 0x00000 };
static unsigned int de0FC60[] = { 0x00020, 0x0064E, 0x00651, 0x00000 };
static unsigned int de0FC61[] = { 0x00020, 0x0064F, 0x00651, 0x00000 };
static unsigned int de0FC62[] = { 0x00020, 0x00650, 0x00651, 0x00000 };
static unsigned int de0FC63[] = { 0x00020, 0x00651, 0x00670, 0x00000 };
static unsigned int de0FC64[] = { 0x00626, 0x00631, 0x00000 };
static unsigned int de0FC65[] = { 0x00626, 0x00632, 0x00000 };
static unsigned int de0FC66[] = { 0x00626, 0x00645, 0x00000 };
static unsigned int de0FC67[] = { 0x00626, 0x00646, 0x00000 };
static unsigned int de0FC68[] = { 0x00626, 0x00649, 0x00000 };
static unsigned int de0FC69[] = { 0x00626, 0x0064A, 0x00000 };
static unsigned int de0FC6A[] = { 0x00628, 0x00631, 0x00000 };
static unsigned int de0FC6B[] = { 0x00628, 0x00632, 0x00000 };
static unsigned int de0FC6C[] = { 0x00628, 0x00645, 0x00000 };
static unsigned int de0FC6D[] = { 0x00628, 0x00646, 0x00000 };
static unsigned int de0FC6E[] = { 0x00628, 0x00649, 0x00000 };
static unsigned int de0FC6F[] = { 0x00628, 0x0064A, 0x00000 };
static unsigned int de0FC70[] = { 0x0062A, 0x00631, 0x00000 };
static unsigned int de0FC71[] = { 0x0062A, 0x00632, 0x00000 };
static unsigned int de0FC72[] = { 0x0062A, 0x00645, 0x00000 };
static unsigned int de0FC73[] = { 0x0062A, 0x00646, 0x00000 };
static unsigned int de0FC74[] = { 0x0062A, 0x00649, 0x00000 };
static unsigned int de0FC75[] = { 0x0062A, 0x0064A, 0x00000 };
static unsigned int de0FC76[] = { 0x0062B, 0x00631, 0x00000 };
static unsigned int de0FC77[] = { 0x0062B, 0x00632, 0x00000 };
static unsigned int de0FC78[] = { 0x0062B, 0x00645, 0x00000 };
static unsigned int de0FC79[] = { 0x0062B, 0x00646, 0x00000 };
static unsigned int de0FC7A[] = { 0x0062B, 0x00649, 0x00000 };
static unsigned int de0FC7B[] = { 0x0062B, 0x0064A, 0x00000 };
static unsigned int de0FC7C[] = { 0x00641, 0x00649, 0x00000 };
static unsigned int de0FC7D[] = { 0x00641, 0x0064A, 0x00000 };
static unsigned int de0FC7E[] = { 0x00642, 0x00649, 0x00000 };
static unsigned int de0FC7F[] = { 0x00642, 0x0064A, 0x00000 };
static unsigned int de0FC80[] = { 0x00643, 0x00627, 0x00000 };
static unsigned int de0FC81[] = { 0x00643, 0x00644, 0x00000 };
static unsigned int de0FC82[] = { 0x00643, 0x00645, 0x00000 };
static unsigned int de0FC83[] = { 0x00643, 0x00649, 0x00000 };
static unsigned int de0FC84[] = { 0x00643, 0x0064A, 0x00000 };
static unsigned int de0FC85[] = { 0x00644, 0x00645, 0x00000 };
static unsigned int de0FC86[] = { 0x00644, 0x00649, 0x00000 };
static unsigned int de0FC87[] = { 0x00644, 0x0064A, 0x00000 };
static unsigned int de0FC88[] = { 0x00645, 0x00627, 0x00000 };
static unsigned int de0FC89[] = { 0x00645, 0x00645, 0x00000 };
static unsigned int de0FC8A[] = { 0x00646, 0x00631, 0x00000 };
static unsigned int de0FC8B[] = { 0x00646, 0x00632, 0x00000 };
static unsigned int de0FC8C[] = { 0x00646, 0x00645, 0x00000 };
static unsigned int de0FC8D[] = { 0x00646, 0x00646, 0x00000 };
static unsigned int de0FC8E[] = { 0x00646, 0x00649, 0x00000 };
static unsigned int de0FC8F[] = { 0x00646, 0x0064A, 0x00000 };
static unsigned int de0FC90[] = { 0x00649, 0x00670, 0x00000 };
static unsigned int de0FC91[] = { 0x0064A, 0x00631, 0x00000 };
static unsigned int de0FC92[] = { 0x0064A, 0x00632, 0x00000 };
static unsigned int de0FC93[] = { 0x0064A, 0x00645, 0x00000 };
static unsigned int de0FC94[] = { 0x0064A, 0x00646, 0x00000 };
static unsigned int de0FC95[] = { 0x0064A, 0x00649, 0x00000 };
static unsigned int de0FC96[] = { 0x0064A, 0x0064A, 0x00000 };
static unsigned int de0FC97[] = { 0x00626, 0x0062C, 0x00000 };
static unsigned int de0FC98[] = { 0x00626, 0x0062D, 0x00000 };
static unsigned int de0FC99[] = { 0x00626, 0x0062E, 0x00000 };
static unsigned int de0FC9A[] = { 0x00626, 0x00645, 0x00000 };
static unsigned int de0FC9B[] = { 0x00626, 0x00647, 0x00000 };
static unsigned int de0FC9C[] = { 0x00628, 0x0062C, 0x00000 };
static unsigned int de0FC9D[] = { 0x00628, 0x0062D, 0x00000 };
static unsigned int de0FC9E[] = { 0x00628, 0x0062E, 0x00000 };
static unsigned int de0FC9F[] = { 0x00628, 0x00645, 0x00000 };
static unsigned int de0FCA0[] = { 0x00628, 0x00647, 0x00000 };
static unsigned int de0FCA1[] = { 0x0062A, 0x0062C, 0x00000 };
static unsigned int de0FCA2[] = { 0x0062A, 0x0062D, 0x00000 };
static unsigned int de0FCA3[] = { 0x0062A, 0x0062E, 0x00000 };
static unsigned int de0FCA4[] = { 0x0062A, 0x00645, 0x00000 };
static unsigned int de0FCA5[] = { 0x0062A, 0x00647, 0x00000 };
static unsigned int de0FCA6[] = { 0x0062B, 0x00645, 0x00000 };
static unsigned int de0FCA7[] = { 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FCA8[] = { 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FCA9[] = { 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FCAA[] = { 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FCAB[] = { 0x0062E, 0x0062C, 0x00000 };
static unsigned int de0FCAC[] = { 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FCAD[] = { 0x00633, 0x0062C, 0x00000 };
static unsigned int de0FCAE[] = { 0x00633, 0x0062D, 0x00000 };
static unsigned int de0FCAF[] = { 0x00633, 0x0062E, 0x00000 };
static unsigned int de0FCB0[] = { 0x00633, 0x00645, 0x00000 };
static unsigned int de0FCB1[] = { 0x00635, 0x0062D, 0x00000 };
static unsigned int de0FCB2[] = { 0x00635, 0x0062E, 0x00000 };
static unsigned int de0FCB3[] = { 0x00635, 0x00645, 0x00000 };
static unsigned int de0FCB4[] = { 0x00636, 0x0062C, 0x00000 };
static unsigned int de0FCB5[] = { 0x00636, 0x0062D, 0x00000 };
static unsigned int de0FCB6[] = { 0x00636, 0x0062E, 0x00000 };
static unsigned int de0FCB7[] = { 0x00636, 0x00645, 0x00000 };
static unsigned int de0FCB8[] = { 0x00637, 0x0062D, 0x00000 };
static unsigned int de0FCB9[] = { 0x00638, 0x00645, 0x00000 };
static unsigned int de0FCBA[] = { 0x00639, 0x0062C, 0x00000 };
static unsigned int de0FCBB[] = { 0x00639, 0x00645, 0x00000 };
static unsigned int de0FCBC[] = { 0x0063A, 0x0062C, 0x00000 };
static unsigned int de0FCBD[] = { 0x0063A, 0x00645, 0x00000 };
static unsigned int de0FCBE[] = { 0x00641, 0x0062C, 0x00000 };
static unsigned int de0FCBF[] = { 0x00641, 0x0062D, 0x00000 };
static unsigned int de0FCC0[] = { 0x00641, 0x0062E, 0x00000 };
static unsigned int de0FCC1[] = { 0x00641, 0x00645, 0x00000 };
static unsigned int de0FCC2[] = { 0x00642, 0x0062D, 0x00000 };
static unsigned int de0FCC3[] = { 0x00642, 0x00645, 0x00000 };
static unsigned int de0FCC4[] = { 0x00643, 0x0062C, 0x00000 };
static unsigned int de0FCC5[] = { 0x00643, 0x0062D, 0x00000 };
static unsigned int de0FCC6[] = { 0x00643, 0x0062E, 0x00000 };
static unsigned int de0FCC7[] = { 0x00643, 0x00644, 0x00000 };
static unsigned int de0FCC8[] = { 0x00643, 0x00645, 0x00000 };
static unsigned int de0FCC9[] = { 0x00644, 0x0062C, 0x00000 };
static unsigned int de0FCCA[] = { 0x00644, 0x0062D, 0x00000 };
static unsigned int de0FCCB[] = { 0x00644, 0x0062E, 0x00000 };
static unsigned int de0FCCC[] = { 0x00644, 0x00645, 0x00000 };
static unsigned int de0FCCD[] = { 0x00644, 0x00647, 0x00000 };
static unsigned int de0FCCE[] = { 0x00645, 0x0062C, 0x00000 };
static unsigned int de0FCCF[] = { 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FCD0[] = { 0x00645, 0x0062E, 0x00000 };
static unsigned int de0FCD1[] = { 0x00645, 0x00645, 0x00000 };
static unsigned int de0FCD2[] = { 0x00646, 0x0062C, 0x00000 };
static unsigned int de0FCD3[] = { 0x00646, 0x0062D, 0x00000 };
static unsigned int de0FCD4[] = { 0x00646, 0x0062E, 0x00000 };
static unsigned int de0FCD5[] = { 0x00646, 0x00645, 0x00000 };
static unsigned int de0FCD6[] = { 0x00646, 0x00647, 0x00000 };
static unsigned int de0FCD7[] = { 0x00647, 0x0062C, 0x00000 };
static unsigned int de0FCD8[] = { 0x00647, 0x00645, 0x00000 };
static unsigned int de0FCD9[] = { 0x00647, 0x00670, 0x00000 };
static unsigned int de0FCDA[] = { 0x0064A, 0x0062C, 0x00000 };
static unsigned int de0FCDB[] = { 0x0064A, 0x0062D, 0x00000 };
static unsigned int de0FCDC[] = { 0x0064A, 0x0062E, 0x00000 };
static unsigned int de0FCDD[] = { 0x0064A, 0x00645, 0x00000 };
static unsigned int de0FCDE[] = { 0x0064A, 0x00647, 0x00000 };
static unsigned int de0FCDF[] = { 0x00626, 0x00645, 0x00000 };
static unsigned int de0FCE0[] = { 0x00626, 0x00647, 0x00000 };
static unsigned int de0FCE1[] = { 0x00628, 0x00645, 0x00000 };
static unsigned int de0FCE2[] = { 0x00628, 0x00647, 0x00000 };
static unsigned int de0FCE3[] = { 0x0062A, 0x00645, 0x00000 };
static unsigned int de0FCE4[] = { 0x0062A, 0x00647, 0x00000 };
static unsigned int de0FCE5[] = { 0x0062B, 0x00645, 0x00000 };
static unsigned int de0FCE6[] = { 0x0062B, 0x00647, 0x00000 };
static unsigned int de0FCE7[] = { 0x00633, 0x00645, 0x00000 };
static unsigned int de0FCE8[] = { 0x00633, 0x00647, 0x00000 };
static unsigned int de0FCE9[] = { 0x00634, 0x00645, 0x00000 };
static unsigned int de0FCEA[] = { 0x00634, 0x00647, 0x00000 };
static unsigned int de0FCEB[] = { 0x00643, 0x00644, 0x00000 };
static unsigned int de0FCEC[] = { 0x00643, 0x00645, 0x00000 };
static unsigned int de0FCED[] = { 0x00644, 0x00645, 0x00000 };
static unsigned int de0FCEE[] = { 0x00646, 0x00645, 0x00000 };
static unsigned int de0FCEF[] = { 0x00646, 0x00647, 0x00000 };
static unsigned int de0FCF0[] = { 0x0064A, 0x00645, 0x00000 };
static unsigned int de0FCF1[] = { 0x0064A, 0x00647, 0x00000 };
static unsigned int de0FCF2[] = { 0x00640, 0x0064E, 0x00651, 0x00000 };
static unsigned int de0FCF3[] = { 0x00640, 0x0064F, 0x00651, 0x00000 };
static unsigned int de0FCF4[] = { 0x00640, 0x00650, 0x00651, 0x00000 };
static unsigned int de0FCF5[] = { 0x00637, 0x00649, 0x00000 };
static unsigned int de0FCF6[] = { 0x00637, 0x0064A, 0x00000 };
static unsigned int de0FCF7[] = { 0x00639, 0x00649, 0x00000 };
static unsigned int de0FCF8[] = { 0x00639, 0x0064A, 0x00000 };
static unsigned int de0FCF9[] = { 0x0063A, 0x00649, 0x00000 };
static unsigned int de0FCFA[] = { 0x0063A, 0x0064A, 0x00000 };
static unsigned int de0FCFB[] = { 0x00633, 0x00649, 0x00000 };
static unsigned int de0FCFC[] = { 0x00633, 0x0064A, 0x00000 };
static unsigned int de0FCFD[] = { 0x00634, 0x00649, 0x00000 };
static unsigned int de0FCFE[] = { 0x00634, 0x0064A, 0x00000 };
static unsigned int de0FCFF[] = { 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FD00[] = { 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FD01[] = { 0x0062C, 0x00649, 0x00000 };
static unsigned int de0FD02[] = { 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FD03[] = { 0x0062E, 0x00649, 0x00000 };
static unsigned int de0FD04[] = { 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FD05[] = { 0x00635, 0x00649, 0x00000 };
static unsigned int de0FD06[] = { 0x00635, 0x0064A, 0x00000 };
static unsigned int de0FD07[] = { 0x00636, 0x00649, 0x00000 };
static unsigned int de0FD08[] = { 0x00636, 0x0064A, 0x00000 };
static unsigned int de0FD09[] = { 0x00634, 0x0062C, 0x00000 };
static unsigned int de0FD0A[] = { 0x00634, 0x0062D, 0x00000 };
static unsigned int de0FD0B[] = { 0x00634, 0x0062E, 0x00000 };
static unsigned int de0FD0C[] = { 0x00634, 0x00645, 0x00000 };
static unsigned int de0FD0D[] = { 0x00634, 0x00631, 0x00000 };
static unsigned int de0FD0E[] = { 0x00633, 0x00631, 0x00000 };
static unsigned int de0FD0F[] = { 0x00635, 0x00631, 0x00000 };
static unsigned int de0FD10[] = { 0x00636, 0x00631, 0x00000 };
static unsigned int de0FD11[] = { 0x00637, 0x00649, 0x00000 };
static unsigned int de0FD12[] = { 0x00637, 0x0064A, 0x00000 };
static unsigned int de0FD13[] = { 0x00639, 0x00649, 0x00000 };
static unsigned int de0FD14[] = { 0x00639, 0x0064A, 0x00000 };
static unsigned int de0FD15[] = { 0x0063A, 0x00649, 0x00000 };
static unsigned int de0FD16[] = { 0x0063A, 0x0064A, 0x00000 };
static unsigned int de0FD17[] = { 0x00633, 0x00649, 0x00000 };
static unsigned int de0FD18[] = { 0x00633, 0x0064A, 0x00000 };
static unsigned int de0FD19[] = { 0x00634, 0x00649, 0x00000 };
static unsigned int de0FD1A[] = { 0x00634, 0x0064A, 0x00000 };
static unsigned int de0FD1B[] = { 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FD1C[] = { 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FD1D[] = { 0x0062C, 0x00649, 0x00000 };
static unsigned int de0FD1E[] = { 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FD1F[] = { 0x0062E, 0x00649, 0x00000 };
static unsigned int de0FD20[] = { 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FD21[] = { 0x00635, 0x00649, 0x00000 };
static unsigned int de0FD22[] = { 0x00635, 0x0064A, 0x00000 };
static unsigned int de0FD23[] = { 0x00636, 0x00649, 0x00000 };
static unsigned int de0FD24[] = { 0x00636, 0x0064A, 0x00000 };
static unsigned int de0FD25[] = { 0x00634, 0x0062C, 0x00000 };
static unsigned int de0FD26[] = { 0x00634, 0x0062D, 0x00000 };
static unsigned int de0FD27[] = { 0x00634, 0x0062E, 0x00000 };
static unsigned int de0FD28[] = { 0x00634, 0x00645, 0x00000 };
static unsigned int de0FD29[] = { 0x00634, 0x00631, 0x00000 };
static unsigned int de0FD2A[] = { 0x00633, 0x00631, 0x00000 };
static unsigned int de0FD2B[] = { 0x00635, 0x00631, 0x00000 };
static unsigned int de0FD2C[] = { 0x00636, 0x00631, 0x00000 };
static unsigned int de0FD2D[] = { 0x00634, 0x0062C, 0x00000 };
static unsigned int de0FD2E[] = { 0x00634, 0x0062D, 0x00000 };
static unsigned int de0FD2F[] = { 0x00634, 0x0062E, 0x00000 };
static unsigned int de0FD30[] = { 0x00634, 0x00645, 0x00000 };
static unsigned int de0FD31[] = { 0x00633, 0x00647, 0x00000 };
static unsigned int de0FD32[] = { 0x00634, 0x00647, 0x00000 };
static unsigned int de0FD33[] = { 0x00637, 0x00645, 0x00000 };
static unsigned int de0FD34[] = { 0x00633, 0x0062C, 0x00000 };
static unsigned int de0FD35[] = { 0x00633, 0x0062D, 0x00000 };
static unsigned int de0FD36[] = { 0x00633, 0x0062E, 0x00000 };
static unsigned int de0FD37[] = { 0x00634, 0x0062C, 0x00000 };
static unsigned int de0FD38[] = { 0x00634, 0x0062D, 0x00000 };
static unsigned int de0FD39[] = { 0x00634, 0x0062E, 0x00000 };
static unsigned int de0FD3A[] = { 0x00637, 0x00645, 0x00000 };
static unsigned int de0FD3B[] = { 0x00638, 0x00645, 0x00000 };
static unsigned int de0FD3C[] = { 0x00627, 0x0064B, 0x00000 };
static unsigned int de0FD3D[] = { 0x00627, 0x0064B, 0x00000 };
static unsigned int de0FD50[] = { 0x0062A, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FD51[] = { 0x0062A, 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FD52[] = { 0x0062A, 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FD53[] = { 0x0062A, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD54[] = { 0x0062A, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD55[] = { 0x0062A, 0x00645, 0x0062C, 0x00000 };
static unsigned int de0FD56[] = { 0x0062A, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD57[] = { 0x0062A, 0x00645, 0x0062E, 0x00000 };
static unsigned int de0FD58[] = { 0x0062C, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD59[] = { 0x0062C, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD5A[] = { 0x0062D, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FD5B[] = { 0x0062D, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FD5C[] = { 0x00633, 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FD5D[] = { 0x00633, 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FD5E[] = { 0x00633, 0x0062C, 0x00649, 0x00000 };
static unsigned int de0FD5F[] = { 0x00633, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD60[] = { 0x00633, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD61[] = { 0x00633, 0x00645, 0x0062C, 0x00000 };
static unsigned int de0FD62[] = { 0x00633, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD63[] = { 0x00633, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD64[] = { 0x00635, 0x0062D, 0x0062D, 0x00000 };
static unsigned int de0FD65[] = { 0x00635, 0x0062D, 0x0062D, 0x00000 };
static unsigned int de0FD66[] = { 0x00635, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD67[] = { 0x00634, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD68[] = { 0x00634, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD69[] = { 0x00634, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FD6A[] = { 0x00634, 0x00645, 0x0062E, 0x00000 };
static unsigned int de0FD6B[] = { 0x00634, 0x00645, 0x0062E, 0x00000 };
static unsigned int de0FD6C[] = { 0x00634, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD6D[] = { 0x00634, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD6E[] = { 0x00636, 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FD6F[] = { 0x00636, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD70[] = { 0x00636, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD71[] = { 0x00637, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD72[] = { 0x00637, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD73[] = { 0x00637, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD74[] = { 0x00637, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FD75[] = { 0x00639, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FD76[] = { 0x00639, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD77[] = { 0x00639, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD78[] = { 0x00639, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FD79[] = { 0x0063A, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD7A[] = { 0x0063A, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FD7B[] = { 0x0063A, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FD7C[] = { 0x00641, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD7D[] = { 0x00641, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD7E[] = { 0x00642, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD7F[] = { 0x00642, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD80[] = { 0x00644, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD81[] = { 0x00644, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FD82[] = { 0x00644, 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FD83[] = { 0x00644, 0x0062C, 0x0062C, 0x00000 };
static unsigned int de0FD84[] = { 0x00644, 0x0062C, 0x0062C, 0x00000 };
static unsigned int de0FD85[] = { 0x00644, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD86[] = { 0x00644, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD87[] = { 0x00644, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD88[] = { 0x00644, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FD89[] = { 0x00645, 0x0062D, 0x0062C, 0x00000 };
static unsigned int de0FD8A[] = { 0x00645, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD8B[] = { 0x00645, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FD8C[] = { 0x00645, 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FD8D[] = { 0x00645, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FD8E[] = { 0x00645, 0x0062E, 0x0062C, 0x00000 };
static unsigned int de0FD8F[] = { 0x00645, 0x0062E, 0x00645, 0x00000 };
static unsigned int de0FD92[] = { 0x00645, 0x0062C, 0x0062E, 0x00000 };
static unsigned int de0FD93[] = { 0x00647, 0x00645, 0x0062C, 0x00000 };
static unsigned int de0FD94[] = { 0x00647, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD95[] = { 0x00646, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FD96[] = { 0x00646, 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FD97[] = { 0x00646, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FD98[] = { 0x00646, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FD99[] = { 0x00646, 0x0062C, 0x00649, 0x00000 };
static unsigned int de0FD9A[] = { 0x00646, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FD9B[] = { 0x00646, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FD9C[] = { 0x0064A, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD9D[] = { 0x0064A, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FD9E[] = { 0x00628, 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FD9F[] = { 0x0062A, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDA0[] = { 0x0062A, 0x0062C, 0x00649, 0x00000 };
static unsigned int de0FDA1[] = { 0x0062A, 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FDA2[] = { 0x0062A, 0x0062E, 0x00649, 0x00000 };
static unsigned int de0FDA3[] = { 0x0062A, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDA4[] = { 0x0062A, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FDA5[] = { 0x0062C, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDA6[] = { 0x0062C, 0x0062D, 0x00649, 0x00000 };
static unsigned int de0FDA7[] = { 0x0062C, 0x00645, 0x00649, 0x00000 };
static unsigned int de0FDA8[] = { 0x00633, 0x0062E, 0x00649, 0x00000 };
static unsigned int de0FDA9[] = { 0x00635, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDAA[] = { 0x00634, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDAB[] = { 0x00636, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDAC[] = { 0x00644, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDAD[] = { 0x00644, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDAE[] = { 0x0064A, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDAF[] = { 0x0064A, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDB0[] = { 0x0064A, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDB1[] = { 0x00645, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDB2[] = { 0x00642, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDB3[] = { 0x00646, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDB4[] = { 0x00642, 0x00645, 0x0062D, 0x00000 };
static unsigned int de0FDB5[] = { 0x00644, 0x0062D, 0x00645, 0x00000 };
static unsigned int de0FDB6[] = { 0x00639, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDB7[] = { 0x00643, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDB8[] = { 0x00646, 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FDB9[] = { 0x00645, 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FDBA[] = { 0x00644, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FDBB[] = { 0x00643, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FDBC[] = { 0x00644, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FDBD[] = { 0x00646, 0x0062C, 0x0062D, 0x00000 };
static unsigned int de0FDBE[] = { 0x0062C, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDBF[] = { 0x0062D, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDC0[] = { 0x00645, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDC1[] = { 0x00641, 0x00645, 0x0064A, 0x00000 };
static unsigned int de0FDC2[] = { 0x00628, 0x0062D, 0x0064A, 0x00000 };
static unsigned int de0FDC3[] = { 0x00643, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FDC4[] = { 0x00639, 0x0062C, 0x00645, 0x00000 };
static unsigned int de0FDC5[] = { 0x00635, 0x00645, 0x00645, 0x00000 };
static unsigned int de0FDC6[] = { 0x00633, 0x0062E, 0x0064A, 0x00000 };
static unsigned int de0FDC7[] = { 0x00646, 0x0062C, 0x0064A, 0x00000 };
static unsigned int de0FDF0[] = { 0x00635, 0x00644, 0x006D2, 0x00000 };
static unsigned int de0FDF1[] = { 0x00642, 0x00644, 0x006D2, 0x00000 };
static unsigned int de0FDF2[] = { 0x00627, 0x00644, 0x00644, 0x00647, 0x00000 };
static unsigned int de0FDF3[] = { 0x00627, 0x00643, 0x00628, 0x00631, 0x00000 };
static unsigned int de0FDF4[] = { 0x00645, 0x0062D, 0x00645, 0x0062F, 0x00000 };
static unsigned int de0FDF5[] = { 0x00635, 0x00644, 0x00639, 0x00645, 0x00000 };
static unsigned int de0FDF6[] = { 0x00631, 0x00633, 0x00648, 0x00644, 0x00000 };
static unsigned int de0FDF7[] = { 0x00639, 0x00644, 0x0064A, 0x00647, 0x00000 };
static unsigned int de0FDF8[] = { 0x00648, 0x00633, 0x00644, 0x00645, 0x00000 };
static unsigned int de0FDF9[] = { 0x00635, 0x00644, 0x00649, 0x00000 };
static unsigned int de0FDFA[] = { 0x00635, 0x00644, 0x00649, 0x00020, 0x00627, 0x00644, 0x00644, 0x00647, 0x00020, 0x00639, 0x00644, 0x0064A, 0x00647, 0x00020, 0x00648, 0x00633, 0x00644, 0x00645, 0x00000 };
static unsigned int de0FDFB[] = { 0x0062C, 0x00644, 0x00020, 0x0062C, 0x00644, 0x00627, 0x00644, 0x00647, 0x00000 };
static unsigned int de0FDFC[] = { 0x00631, 0x006CC, 0x00627, 0x00644, 0x00000 };
static unsigned int de0FE10[] = { 0x0002C, 0x00000 };
static unsigned int de0FE11[] = { 0x03001, 0x00000 };
static unsigned int de0FE12[] = { 0x03002, 0x00000 };
static unsigned int de0FE13[] = { 0x0003A, 0x00000 };
static unsigned int de0FE14[] = { 0x0003B, 0x00000 };
static unsigned int de0FE15[] = { 0x00021, 0x00000 };
static unsigned int de0FE16[] = { 0x0003F, 0x00000 };
static unsigned int de0FE17[] = { 0x03016, 0x00000 };
static unsigned int de0FE18[] = { 0x03017, 0x00000 };
static unsigned int de0FE19[] = { 0x02026, 0x00000 };
static unsigned int de0FE30[] = { 0x02025, 0x00000 };
static unsigned int de0FE31[] = { 0x02014, 0x00000 };
static unsigned int de0FE32[] = { 0x02013, 0x00000 };
static unsigned int de0FE33[] = { 0x0005F, 0x00000 };
static unsigned int de0FE34[] = { 0x0005F, 0x00000 };
static unsigned int de0FE35[] = { 0x00028, 0x00000 };
static unsigned int de0FE36[] = { 0x00029, 0x00000 };
static unsigned int de0FE37[] = { 0x0007B, 0x00000 };
static unsigned int de0FE38[] = { 0x0007D, 0x00000 };
static unsigned int de0FE39[] = { 0x03014, 0x00000 };
static unsigned int de0FE3A[] = { 0x03015, 0x00000 };
static unsigned int de0FE3B[] = { 0x03010, 0x00000 };
static unsigned int de0FE3C[] = { 0x03011, 0x00000 };
static unsigned int de0FE3D[] = { 0x0300A, 0x00000 };
static unsigned int de0FE3E[] = { 0x0300B, 0x00000 };
static unsigned int de0FE3F[] = { 0x03008, 0x00000 };
static unsigned int de0FE40[] = { 0x03009, 0x00000 };
static unsigned int de0FE41[] = { 0x0300C, 0x00000 };
static unsigned int de0FE42[] = { 0x0300D, 0x00000 };
static unsigned int de0FE43[] = { 0x0300E, 0x00000 };
static unsigned int de0FE44[] = { 0x0300F, 0x00000 };
static unsigned int de0FE47[] = { 0x0005B, 0x00000 };
static unsigned int de0FE48[] = { 0x0005D, 0x00000 };
static unsigned int de0FE49[] = { 0x0203E, 0x00000 };
static unsigned int de0FE4A[] = { 0x0203E, 0x00000 };
static unsigned int de0FE4B[] = { 0x0203E, 0x00000 };
static unsigned int de0FE4C[] = { 0x0203E, 0x00000 };
static unsigned int de0FE4D[] = { 0x0005F, 0x00000 };
static unsigned int de0FE4E[] = { 0x0005F, 0x00000 };
static unsigned int de0FE4F[] = { 0x0005F, 0x00000 };
static unsigned int de0FE50[] = { 0x0002C, 0x00000 };
static unsigned int de0FE51[] = { 0x03001, 0x00000 };
static unsigned int de0FE52[] = { 0x0002E, 0x00000 };
static unsigned int de0FE54[] = { 0x0003B, 0x00000 };
static unsigned int de0FE55[] = { 0x0003A, 0x00000 };
static unsigned int de0FE56[] = { 0x0003F, 0x00000 };
static unsigned int de0FE57[] = { 0x00021, 0x00000 };
static unsigned int de0FE58[] = { 0x02014, 0x00000 };
static unsigned int de0FE59[] = { 0x00028, 0x00000 };
static unsigned int de0FE5A[] = { 0x00029, 0x00000 };
static unsigned int de0FE5B[] = { 0x0007B, 0x00000 };
static unsigned int de0FE5C[] = { 0x0007D, 0x00000 };
static unsigned int de0FE5D[] = { 0x03014, 0x00000 };
static unsigned int de0FE5E[] = { 0x03015, 0x00000 };
static unsigned int de0FE5F[] = { 0x00023, 0x00000 };
static unsigned int de0FE60[] = { 0x00026, 0x00000 };
static unsigned int de0FE61[] = { 0x0002A, 0x00000 };
static unsigned int de0FE62[] = { 0x0002B, 0x00000 };
static unsigned int de0FE63[] = { 0x0002D, 0x00000 };
static unsigned int de0FE64[] = { 0x0003C, 0x00000 };
static unsigned int de0FE65[] = { 0x0003E, 0x00000 };
static unsigned int de0FE66[] = { 0x0003D, 0x00000 };
static unsigned int de0FE68[] = { 0x0005C, 0x00000 };
static unsigned int de0FE69[] = { 0x00024, 0x00000 };
static unsigned int de0FE6A[] = { 0x00025, 0x00000 };
static unsigned int de0FE6B[] = { 0x00040, 0x00000 };
static unsigned int de0FE70[] = { 0x00020, 0x0064B, 0x00000 };
static unsigned int de0FE71[] = { 0x00640, 0x0064B, 0x00000 };
static unsigned int de0FE72[] = { 0x00020, 0x0064C, 0x00000 };
static unsigned int de0FE74[] = { 0x00020, 0x0064D, 0x00000 };
static unsigned int de0FE76[] = { 0x00020, 0x0064E, 0x00000 };
static unsigned int de0FE77[] = { 0x00640, 0x0064E, 0x00000 };
static unsigned int de0FE78[] = { 0x00020, 0x0064F, 0x00000 };
static unsigned int de0FE79[] = { 0x00640, 0x0064F, 0x00000 };
static unsigned int de0FE7A[] = { 0x00020, 0x00650, 0x00000 };
static unsigned int de0FE7B[] = { 0x00640, 0x00650, 0x00000 };
static unsigned int de0FE7C[] = { 0x00020, 0x00651, 0x00000 };
static unsigned int de0FE7D[] = { 0x00640, 0x00651, 0x00000 };
static unsigned int de0FE7E[] = { 0x00020, 0x00652, 0x00000 };
static unsigned int de0FE7F[] = { 0x00640, 0x00652, 0x00000 };
static unsigned int de0FE80[] = { 0x00621, 0x00000 };
static unsigned int de0FE81[] = { 0x00622, 0x00000 };
static unsigned int de0FE82[] = { 0x00622, 0x00000 };
static unsigned int de0FE83[] = { 0x00623, 0x00000 };
static unsigned int de0FE84[] = { 0x00623, 0x00000 };
static unsigned int de0FE85[] = { 0x00624, 0x00000 };
static unsigned int de0FE86[] = { 0x00624, 0x00000 };
static unsigned int de0FE87[] = { 0x00625, 0x00000 };
static unsigned int de0FE88[] = { 0x00625, 0x00000 };
static unsigned int de0FE89[] = { 0x00626, 0x00000 };
static unsigned int de0FE8A[] = { 0x00626, 0x00000 };
static unsigned int de0FE8B[] = { 0x00626, 0x00000 };
static unsigned int de0FE8C[] = { 0x00626, 0x00000 };
static unsigned int de0FE8D[] = { 0x00627, 0x00000 };
static unsigned int de0FE8E[] = { 0x00627, 0x00000 };
static unsigned int de0FE8F[] = { 0x00628, 0x00000 };
static unsigned int de0FE90[] = { 0x00628, 0x00000 };
static unsigned int de0FE91[] = { 0x00628, 0x00000 };
static unsigned int de0FE92[] = { 0x00628, 0x00000 };
static unsigned int de0FE93[] = { 0x00629, 0x00000 };
static unsigned int de0FE94[] = { 0x00629, 0x00000 };
static unsigned int de0FE95[] = { 0x0062A, 0x00000 };
static unsigned int de0FE96[] = { 0x0062A, 0x00000 };
static unsigned int de0FE97[] = { 0x0062A, 0x00000 };
static unsigned int de0FE98[] = { 0x0062A, 0x00000 };
static unsigned int de0FE99[] = { 0x0062B, 0x00000 };
static unsigned int de0FE9A[] = { 0x0062B, 0x00000 };
static unsigned int de0FE9B[] = { 0x0062B, 0x00000 };
static unsigned int de0FE9C[] = { 0x0062B, 0x00000 };
static unsigned int de0FE9D[] = { 0x0062C, 0x00000 };
static unsigned int de0FE9E[] = { 0x0062C, 0x00000 };
static unsigned int de0FE9F[] = { 0x0062C, 0x00000 };
static unsigned int de0FEA0[] = { 0x0062C, 0x00000 };
static unsigned int de0FEA1[] = { 0x0062D, 0x00000 };
static unsigned int de0FEA2[] = { 0x0062D, 0x00000 };
static unsigned int de0FEA3[] = { 0x0062D, 0x00000 };
static unsigned int de0FEA4[] = { 0x0062D, 0x00000 };
static unsigned int de0FEA5[] = { 0x0062E, 0x00000 };
static unsigned int de0FEA6[] = { 0x0062E, 0x00000 };
static unsigned int de0FEA7[] = { 0x0062E, 0x00000 };
static unsigned int de0FEA8[] = { 0x0062E, 0x00000 };
static unsigned int de0FEA9[] = { 0x0062F, 0x00000 };
static unsigned int de0FEAA[] = { 0x0062F, 0x00000 };
static unsigned int de0FEAB[] = { 0x00630, 0x00000 };
static unsigned int de0FEAC[] = { 0x00630, 0x00000 };
static unsigned int de0FEAD[] = { 0x00631, 0x00000 };
static unsigned int de0FEAE[] = { 0x00631, 0x00000 };
static unsigned int de0FEAF[] = { 0x00632, 0x00000 };
static unsigned int de0FEB0[] = { 0x00632, 0x00000 };
static unsigned int de0FEB1[] = { 0x00633, 0x00000 };
static unsigned int de0FEB2[] = { 0x00633, 0x00000 };
static unsigned int de0FEB3[] = { 0x00633, 0x00000 };
static unsigned int de0FEB4[] = { 0x00633, 0x00000 };
static unsigned int de0FEB5[] = { 0x00634, 0x00000 };
static unsigned int de0FEB6[] = { 0x00634, 0x00000 };
static unsigned int de0FEB7[] = { 0x00634, 0x00000 };
static unsigned int de0FEB8[] = { 0x00634, 0x00000 };
static unsigned int de0FEB9[] = { 0x00635, 0x00000 };
static unsigned int de0FEBA[] = { 0x00635, 0x00000 };
static unsigned int de0FEBB[] = { 0x00635, 0x00000 };
static unsigned int de0FEBC[] = { 0x00635, 0x00000 };
static unsigned int de0FEBD[] = { 0x00636, 0x00000 };
static unsigned int de0FEBE[] = { 0x00636, 0x00000 };
static unsigned int de0FEBF[] = { 0x00636, 0x00000 };
static unsigned int de0FEC0[] = { 0x00636, 0x00000 };
static unsigned int de0FEC1[] = { 0x00637, 0x00000 };
static unsigned int de0FEC2[] = { 0x00637, 0x00000 };
static unsigned int de0FEC3[] = { 0x00637, 0x00000 };
static unsigned int de0FEC4[] = { 0x00637, 0x00000 };
static unsigned int de0FEC5[] = { 0x00638, 0x00000 };
static unsigned int de0FEC6[] = { 0x00638, 0x00000 };
static unsigned int de0FEC7[] = { 0x00638, 0x00000 };
static unsigned int de0FEC8[] = { 0x00638, 0x00000 };
static unsigned int de0FEC9[] = { 0x00639, 0x00000 };
static unsigned int de0FECA[] = { 0x00639, 0x00000 };
static unsigned int de0FECB[] = { 0x00639, 0x00000 };
static unsigned int de0FECC[] = { 0x00639, 0x00000 };
static unsigned int de0FECD[] = { 0x0063A, 0x00000 };
static unsigned int de0FECE[] = { 0x0063A, 0x00000 };
static unsigned int de0FECF[] = { 0x0063A, 0x00000 };
static unsigned int de0FED0[] = { 0x0063A, 0x00000 };
static unsigned int de0FED1[] = { 0x00641, 0x00000 };
static unsigned int de0FED2[] = { 0x00641, 0x00000 };
static unsigned int de0FED3[] = { 0x00641, 0x00000 };
static unsigned int de0FED4[] = { 0x00641, 0x00000 };
static unsigned int de0FED5[] = { 0x00642, 0x00000 };
static unsigned int de0FED6[] = { 0x00642, 0x00000 };
static unsigned int de0FED7[] = { 0x00642, 0x00000 };
static unsigned int de0FED8[] = { 0x00642, 0x00000 };
static unsigned int de0FED9[] = { 0x00643, 0x00000 };
static unsigned int de0FEDA[] = { 0x00643, 0x00000 };
static unsigned int de0FEDB[] = { 0x00643, 0x00000 };
static unsigned int de0FEDC[] = { 0x00643, 0x00000 };
static unsigned int de0FEDD[] = { 0x00644, 0x00000 };
static unsigned int de0FEDE[] = { 0x00644, 0x00000 };
static unsigned int de0FEDF[] = { 0x00644, 0x00000 };
static unsigned int de0FEE0[] = { 0x00644, 0x00000 };
static unsigned int de0FEE1[] = { 0x00645, 0x00000 };
static unsigned int de0FEE2[] = { 0x00645, 0x00000 };
static unsigned int de0FEE3[] = { 0x00645, 0x00000 };
static unsigned int de0FEE4[] = { 0x00645, 0x00000 };
static unsigned int de0FEE5[] = { 0x00646, 0x00000 };
static unsigned int de0FEE6[] = { 0x00646, 0x00000 };
static unsigned int de0FEE7[] = { 0x00646, 0x00000 };
static unsigned int de0FEE8[] = { 0x00646, 0x00000 };
static unsigned int de0FEE9[] = { 0x00647, 0x00000 };
static unsigned int de0FEEA[] = { 0x00647, 0x00000 };
static unsigned int de0FEEB[] = { 0x00647, 0x00000 };
static unsigned int de0FEEC[] = { 0x00647, 0x00000 };
static unsigned int de0FEED[] = { 0x00648, 0x00000 };
static unsigned int de0FEEE[] = { 0x00648, 0x00000 };
static unsigned int de0FEEF[] = { 0x00649, 0x00000 };
static unsigned int de0FEF0[] = { 0x00649, 0x00000 };
static unsigned int de0FEF1[] = { 0x0064A, 0x00000 };
static unsigned int de0FEF2[] = { 0x0064A, 0x00000 };
static unsigned int de0FEF3[] = { 0x0064A, 0x00000 };
static unsigned int de0FEF4[] = { 0x0064A, 0x00000 };
static unsigned int de0FEF5[] = { 0x00644, 0x00622, 0x00000 };
static unsigned int de0FEF6[] = { 0x00644, 0x00622, 0x00000 };
static unsigned int de0FEF7[] = { 0x00644, 0x00623, 0x00000 };
static unsigned int de0FEF8[] = { 0x00644, 0x00623, 0x00000 };
static unsigned int de0FEF9[] = { 0x00644, 0x00625, 0x00000 };
static unsigned int de0FEFA[] = { 0x00644, 0x00625, 0x00000 };
static unsigned int de0FEFB[] = { 0x00644, 0x00627, 0x00000 };
static unsigned int de0FEFC[] = { 0x00644, 0x00627, 0x00000 };
static unsigned int de0FF01[] = { 0x00021, 0x00000 };
static unsigned int de0FF02[] = { 0x00022, 0x00000 };
static unsigned int de0FF03[] = { 0x00023, 0x00000 };
static unsigned int de0FF04[] = { 0x00024, 0x00000 };
static unsigned int de0FF05[] = { 0x00025, 0x00000 };
static unsigned int de0FF06[] = { 0x00026, 0x00000 };
static unsigned int de0FF07[] = { 0x00027, 0x00000 };
static unsigned int de0FF08[] = { 0x00028, 0x00000 };
static unsigned int de0FF09[] = { 0x00029, 0x00000 };
static unsigned int de0FF0A[] = { 0x0002A, 0x00000 };
static unsigned int de0FF0B[] = { 0x0002B, 0x00000 };
static unsigned int de0FF0C[] = { 0x0002C, 0x00000 };
static unsigned int de0FF0D[] = { 0x0002D, 0x00000 };
static unsigned int de0FF0E[] = { 0x0002E, 0x00000 };
static unsigned int de0FF0F[] = { 0x0002F, 0x00000 };
static unsigned int de0FF10[] = { 0x00030, 0x00000 };
static unsigned int de0FF11[] = { 0x00031, 0x00000 };
static unsigned int de0FF12[] = { 0x00032, 0x00000 };
static unsigned int de0FF13[] = { 0x00033, 0x00000 };
static unsigned int de0FF14[] = { 0x00034, 0x00000 };
static unsigned int de0FF15[] = { 0x00035, 0x00000 };
static unsigned int de0FF16[] = { 0x00036, 0x00000 };
static unsigned int de0FF17[] = { 0x00037, 0x00000 };
static unsigned int de0FF18[] = { 0x00038, 0x00000 };
static unsigned int de0FF19[] = { 0x00039, 0x00000 };
static unsigned int de0FF1A[] = { 0x0003A, 0x00000 };
static unsigned int de0FF1B[] = { 0x0003B, 0x00000 };
static unsigned int de0FF1C[] = { 0x0003C, 0x00000 };
static unsigned int de0FF1D[] = { 0x0003D, 0x00000 };
static unsigned int de0FF1E[] = { 0x0003E, 0x00000 };
static unsigned int de0FF1F[] = { 0x0003F, 0x00000 };
static unsigned int de0FF20[] = { 0x00040, 0x00000 };
static unsigned int de0FF21[] = { 0x00041, 0x00000 };
static unsigned int de0FF22[] = { 0x00042, 0x00000 };
static unsigned int de0FF23[] = { 0x00043, 0x00000 };
static unsigned int de0FF24[] = { 0x00044, 0x00000 };
static unsigned int de0FF25[] = { 0x00045, 0x00000 };
static unsigned int de0FF26[] = { 0x00046, 0x00000 };
static unsigned int de0FF27[] = { 0x00047, 0x00000 };
static unsigned int de0FF28[] = { 0x00048, 0x00000 };
static unsigned int de0FF29[] = { 0x00049, 0x00000 };
static unsigned int de0FF2A[] = { 0x0004A, 0x00000 };
static unsigned int de0FF2B[] = { 0x0004B, 0x00000 };
static unsigned int de0FF2C[] = { 0x0004C, 0x00000 };
static unsigned int de0FF2D[] = { 0x0004D, 0x00000 };
static unsigned int de0FF2E[] = { 0x0004E, 0x00000 };
static unsigned int de0FF2F[] = { 0x0004F, 0x00000 };
static unsigned int de0FF30[] = { 0x00050, 0x00000 };
static unsigned int de0FF31[] = { 0x00051, 0x00000 };
static unsigned int de0FF32[] = { 0x00052, 0x00000 };
static unsigned int de0FF33[] = { 0x00053, 0x00000 };
static unsigned int de0FF34[] = { 0x00054, 0x00000 };
static unsigned int de0FF35[] = { 0x00055, 0x00000 };
static unsigned int de0FF36[] = { 0x00056, 0x00000 };
static unsigned int de0FF37[] = { 0x00057, 0x00000 };
static unsigned int de0FF38[] = { 0x00058, 0x00000 };
static unsigned int de0FF39[] = { 0x00059, 0x00000 };
static unsigned int de0FF3A[] = { 0x0005A, 0x00000 };
static unsigned int de0FF3B[] = { 0x0005B, 0x00000 };
static unsigned int de0FF3C[] = { 0x0005C, 0x00000 };
static unsigned int de0FF3D[] = { 0x0005D, 0x00000 };
static unsigned int de0FF3E[] = { 0x0005E, 0x00000 };
static unsigned int de0FF3F[] = { 0x0005F, 0x00000 };
static unsigned int de0FF40[] = { 0x00060, 0x00000 };
static unsigned int de0FF41[] = { 0x00061, 0x00000 };
static unsigned int de0FF42[] = { 0x00062, 0x00000 };
static unsigned int de0FF43[] = { 0x00063, 0x00000 };
static unsigned int de0FF44[] = { 0x00064, 0x00000 };
static unsigned int de0FF45[] = { 0x00065, 0x00000 };
static unsigned int de0FF46[] = { 0x00066, 0x00000 };
static unsigned int de0FF47[] = { 0x00067, 0x00000 };
static unsigned int de0FF48[] = { 0x00068, 0x00000 };
static unsigned int de0FF49[] = { 0x00069, 0x00000 };
static unsigned int de0FF4A[] = { 0x0006A, 0x00000 };
static unsigned int de0FF4B[] = { 0x0006B, 0x00000 };
static unsigned int de0FF4C[] = { 0x0006C, 0x00000 };
static unsigned int de0FF4D[] = { 0x0006D, 0x00000 };
static unsigned int de0FF4E[] = { 0x0006E, 0x00000 };
static unsigned int de0FF4F[] = { 0x0006F, 0x00000 };
static unsigned int de0FF50[] = { 0x00070, 0x00000 };
static unsigned int de0FF51[] = { 0x00071, 0x00000 };
static unsigned int de0FF52[] = { 0x00072, 0x00000 };
static unsigned int de0FF53[] = { 0x00073, 0x00000 };
static unsigned int de0FF54[] = { 0x00074, 0x00000 };
static unsigned int de0FF55[] = { 0x00075, 0x00000 };
static unsigned int de0FF56[] = { 0x00076, 0x00000 };
static unsigned int de0FF57[] = { 0x00077, 0x00000 };
static unsigned int de0FF58[] = { 0x00078, 0x00000 };
static unsigned int de0FF59[] = { 0x00079, 0x00000 };
static unsigned int de0FF5A[] = { 0x0007A, 0x00000 };
static unsigned int de0FF5B[] = { 0x0007B, 0x00000 };
static unsigned int de0FF5C[] = { 0x0007C, 0x00000 };
static unsigned int de0FF5D[] = { 0x0007D, 0x00000 };
static unsigned int de0FF5E[] = { 0x0007E, 0x00000 };
static unsigned int de0FF5F[] = { 0x02985, 0x00000 };
static unsigned int de0FF60[] = { 0x02986, 0x00000 };
static unsigned int de0FF61[] = { 0x03002, 0x00000 };
static unsigned int de0FF62[] = { 0x0300C, 0x00000 };
static unsigned int de0FF63[] = { 0x0300D, 0x00000 };
static unsigned int de0FF64[] = { 0x03001, 0x00000 };
static unsigned int de0FF65[] = { 0x030FB, 0x00000 };
static unsigned int de0FF66[] = { 0x030F2, 0x00000 };
static unsigned int de0FF67[] = { 0x030A1, 0x00000 };
static unsigned int de0FF68[] = { 0x030A3, 0x00000 };
static unsigned int de0FF69[] = { 0x030A5, 0x00000 };
static unsigned int de0FF6A[] = { 0x030A7, 0x00000 };
static unsigned int de0FF6B[] = { 0x030A9, 0x00000 };
static unsigned int de0FF6C[] = { 0x030E3, 0x00000 };
static unsigned int de0FF6D[] = { 0x030E5, 0x00000 };
static unsigned int de0FF6E[] = { 0x030E7, 0x00000 };
static unsigned int de0FF6F[] = { 0x030C3, 0x00000 };
static unsigned int de0FF70[] = { 0x030FC, 0x00000 };
static unsigned int de0FF71[] = { 0x030A2, 0x00000 };
static unsigned int de0FF72[] = { 0x030A4, 0x00000 };
static unsigned int de0FF73[] = { 0x030A6, 0x00000 };
static unsigned int de0FF74[] = { 0x030A8, 0x00000 };
static unsigned int de0FF75[] = { 0x030AA, 0x00000 };
static unsigned int de0FF76[] = { 0x030AB, 0x00000 };
static unsigned int de0FF77[] = { 0x030AD, 0x00000 };
static unsigned int de0FF78[] = { 0x030AF, 0x00000 };
static unsigned int de0FF79[] = { 0x030B1, 0x00000 };
static unsigned int de0FF7A[] = { 0x030B3, 0x00000 };
static unsigned int de0FF7B[] = { 0x030B5, 0x00000 };
static unsigned int de0FF7C[] = { 0x030B7, 0x00000 };
static unsigned int de0FF7D[] = { 0x030B9, 0x00000 };
static unsigned int de0FF7E[] = { 0x030BB, 0x00000 };
static unsigned int de0FF7F[] = { 0x030BD, 0x00000 };
static unsigned int de0FF80[] = { 0x030BF, 0x00000 };
static unsigned int de0FF81[] = { 0x030C1, 0x00000 };
static unsigned int de0FF82[] = { 0x030C4, 0x00000 };
static unsigned int de0FF83[] = { 0x030C6, 0x00000 };
static unsigned int de0FF84[] = { 0x030C8, 0x00000 };
static unsigned int de0FF85[] = { 0x030CA, 0x00000 };
static unsigned int de0FF86[] = { 0x030CB, 0x00000 };
static unsigned int de0FF87[] = { 0x030CC, 0x00000 };
static unsigned int de0FF88[] = { 0x030CD, 0x00000 };
static unsigned int de0FF89[] = { 0x030CE, 0x00000 };
static unsigned int de0FF8A[] = { 0x030CF, 0x00000 };
static unsigned int de0FF8B[] = { 0x030D2, 0x00000 };
static unsigned int de0FF8C[] = { 0x030D5, 0x00000 };
static unsigned int de0FF8D[] = { 0x030D8, 0x00000 };
static unsigned int de0FF8E[] = { 0x030DB, 0x00000 };
static unsigned int de0FF8F[] = { 0x030DE, 0x00000 };
static unsigned int de0FF90[] = { 0x030DF, 0x00000 };
static unsigned int de0FF91[] = { 0x030E0, 0x00000 };
static unsigned int de0FF92[] = { 0x030E1, 0x00000 };
static unsigned int de0FF93[] = { 0x030E2, 0x00000 };
static unsigned int de0FF94[] = { 0x030E4, 0x00000 };
static unsigned int de0FF95[] = { 0x030E6, 0x00000 };
static unsigned int de0FF96[] = { 0x030E8, 0x00000 };
static unsigned int de0FF97[] = { 0x030E9, 0x00000 };
static unsigned int de0FF98[] = { 0x030EA, 0x00000 };
static unsigned int de0FF99[] = { 0x030EB, 0x00000 };
static unsigned int de0FF9A[] = { 0x030EC, 0x00000 };
static unsigned int de0FF9B[] = { 0x030ED, 0x00000 };
static unsigned int de0FF9C[] = { 0x030EF, 0x00000 };
static unsigned int de0FF9D[] = { 0x030F3, 0x00000 };
static unsigned int de0FF9E[] = { 0x03099, 0x00000 };
static unsigned int de0FF9F[] = { 0x0309A, 0x00000 };
static unsigned int de0FFA0[] = { 0x03164, 0x00000 };
static unsigned int de0FFA1[] = { 0x03131, 0x00000 };
static unsigned int de0FFA2[] = { 0x03132, 0x00000 };
static unsigned int de0FFA3[] = { 0x03133, 0x00000 };
static unsigned int de0FFA4[] = { 0x03134, 0x00000 };
static unsigned int de0FFA5[] = { 0x03135, 0x00000 };
static unsigned int de0FFA6[] = { 0x03136, 0x00000 };
static unsigned int de0FFA7[] = { 0x03137, 0x00000 };
static unsigned int de0FFA8[] = { 0x03138, 0x00000 };
static unsigned int de0FFA9[] = { 0x03139, 0x00000 };
static unsigned int de0FFAA[] = { 0x0313A, 0x00000 };
static unsigned int de0FFAB[] = { 0x0313B, 0x00000 };
static unsigned int de0FFAC[] = { 0x0313C, 0x00000 };
static unsigned int de0FFAD[] = { 0x0313D, 0x00000 };
static unsigned int de0FFAE[] = { 0x0313E, 0x00000 };
static unsigned int de0FFAF[] = { 0x0313F, 0x00000 };
static unsigned int de0FFB0[] = { 0x03140, 0x00000 };
static unsigned int de0FFB1[] = { 0x03141, 0x00000 };
static unsigned int de0FFB2[] = { 0x03142, 0x00000 };
static unsigned int de0FFB3[] = { 0x03143, 0x00000 };
static unsigned int de0FFB4[] = { 0x03144, 0x00000 };
static unsigned int de0FFB5[] = { 0x03145, 0x00000 };
static unsigned int de0FFB6[] = { 0x03146, 0x00000 };
static unsigned int de0FFB7[] = { 0x03147, 0x00000 };
static unsigned int de0FFB8[] = { 0x03148, 0x00000 };
static unsigned int de0FFB9[] = { 0x03149, 0x00000 };
static unsigned int de0FFBA[] = { 0x0314A, 0x00000 };
static unsigned int de0FFBB[] = { 0x0314B, 0x00000 };
static unsigned int de0FFBC[] = { 0x0314C, 0x00000 };
static unsigned int de0FFBD[] = { 0x0314D, 0x00000 };
static unsigned int de0FFBE[] = { 0x0314E, 0x00000 };
static unsigned int de0FFC2[] = { 0x0314F, 0x00000 };
static unsigned int de0FFC3[] = { 0x03150, 0x00000 };
static unsigned int de0FFC4[] = { 0x03151, 0x00000 };
static unsigned int de0FFC5[] = { 0x03152, 0x00000 };
static unsigned int de0FFC6[] = { 0x03153, 0x00000 };
static unsigned int de0FFC7[] = { 0x03154, 0x00000 };
static unsigned int de0FFCA[] = { 0x03155, 0x00000 };
static unsigned int de0FFCB[] = { 0x03156, 0x00000 };
static unsigned int de0FFCC[] = { 0x03157, 0x00000 };
static unsigned int de0FFCD[] = { 0x03158, 0x00000 };
static unsigned int de0FFCE[] = { 0x03159, 0x00000 };
static unsigned int de0FFCF[] = { 0x0315A, 0x00000 };
static unsigned int de0FFD2[] = { 0x0315B, 0x00000 };
static unsigned int de0FFD3[] = { 0x0315C, 0x00000 };
static unsigned int de0FFD4[] = { 0x0315D, 0x00000 };
static unsigned int de0FFD5[] = { 0x0315E, 0x00000 };
static unsigned int de0FFD6[] = { 0x0315F, 0x00000 };
static unsigned int de0FFD7[] = { 0x03160, 0x00000 };
static unsigned int de0FFDA[] = { 0x03161, 0x00000 };
static unsigned int de0FFDB[] = { 0x03162, 0x00000 };
static unsigned int de0FFDC[] = { 0x03163, 0x00000 };
static unsigned int de0FFE0[] = { 0x000A2, 0x00000 };
static unsigned int de0FFE1[] = { 0x000A3, 0x00000 };
static unsigned int de0FFE2[] = { 0x000AC, 0x00000 };
static unsigned int de0FFE3[] = { 0x000AF, 0x00000 };
static unsigned int de0FFE4[] = { 0x000A6, 0x00000 };
static unsigned int de0FFE5[] = { 0x000A5, 0x00000 };
static unsigned int de0FFE6[] = { 0x020A9, 0x00000 };
static unsigned int de0FFE8[] = { 0x02502, 0x00000 };
static unsigned int de0FFE9[] = { 0x02190, 0x00000 };
static unsigned int de0FFEA[] = { 0x02191, 0x00000 };
static unsigned int de0FFEB[] = { 0x02192, 0x00000 };
static unsigned int de0FFEC[] = { 0x02193, 0x00000 };
static unsigned int de0FFED[] = { 0x025A0, 0x00000 };
static unsigned int de0FFEE[] = { 0x025CB, 0x00000 };
static unsigned int de1D15E[] = { 0x1D157, 0x1D165, 0x00000 };
static unsigned int de1D15F[] = { 0x1D158, 0x1D165, 0x00000 };
static unsigned int de1D160[] = { 0x1D15F, 0x1D16E, 0x00000 };
static unsigned int de1D161[] = { 0x1D15F, 0x1D16F, 0x00000 };
static unsigned int de1D162[] = { 0x1D15F, 0x1D170, 0x00000 };
static unsigned int de1D163[] = { 0x1D15F, 0x1D171, 0x00000 };
static unsigned int de1D164[] = { 0x1D15F, 0x1D172, 0x00000 };
static unsigned int de1D1BB[] = { 0x1D1B9, 0x1D165, 0x00000 };
static unsigned int de1D1BC[] = { 0x1D1BA, 0x1D165, 0x00000 };
static unsigned int de1D1BD[] = { 0x1D1BB, 0x1D16E, 0x00000 };
static unsigned int de1D1BE[] = { 0x1D1BC, 0x1D16E, 0x00000 };
static unsigned int de1D1BF[] = { 0x1D1BB, 0x1D16F, 0x00000 };
static unsigned int de1D1C0[] = { 0x1D1BC, 0x1D16F, 0x00000 };
static unsigned int de1D400[] = { 0x00041, 0x00000 };
static unsigned int de1D401[] = { 0x00042, 0x00000 };
static unsigned int de1D402[] = { 0x00043, 0x00000 };
static unsigned int de1D403[] = { 0x00044, 0x00000 };
static unsigned int de1D404[] = { 0x00045, 0x00000 };
static unsigned int de1D405[] = { 0x00046, 0x00000 };
static unsigned int de1D406[] = { 0x00047, 0x00000 };
static unsigned int de1D407[] = { 0x00048, 0x00000 };
static unsigned int de1D408[] = { 0x00049, 0x00000 };
static unsigned int de1D409[] = { 0x0004A, 0x00000 };
static unsigned int de1D40A[] = { 0x0004B, 0x00000 };
static unsigned int de1D40B[] = { 0x0004C, 0x00000 };
static unsigned int de1D40C[] = { 0x0004D, 0x00000 };
static unsigned int de1D40D[] = { 0x0004E, 0x00000 };
static unsigned int de1D40E[] = { 0x0004F, 0x00000 };
static unsigned int de1D40F[] = { 0x00050, 0x00000 };
static unsigned int de1D410[] = { 0x00051, 0x00000 };
static unsigned int de1D411[] = { 0x00052, 0x00000 };
static unsigned int de1D412[] = { 0x00053, 0x00000 };
static unsigned int de1D413[] = { 0x00054, 0x00000 };
static unsigned int de1D414[] = { 0x00055, 0x00000 };
static unsigned int de1D415[] = { 0x00056, 0x00000 };
static unsigned int de1D416[] = { 0x00057, 0x00000 };
static unsigned int de1D417[] = { 0x00058, 0x00000 };
static unsigned int de1D418[] = { 0x00059, 0x00000 };
static unsigned int de1D419[] = { 0x0005A, 0x00000 };
static unsigned int de1D41A[] = { 0x00061, 0x00000 };
static unsigned int de1D41B[] = { 0x00062, 0x00000 };
static unsigned int de1D41C[] = { 0x00063, 0x00000 };
static unsigned int de1D41D[] = { 0x00064, 0x00000 };
static unsigned int de1D41E[] = { 0x00065, 0x00000 };
static unsigned int de1D41F[] = { 0x00066, 0x00000 };
static unsigned int de1D420[] = { 0x00067, 0x00000 };
static unsigned int de1D421[] = { 0x00068, 0x00000 };
static unsigned int de1D422[] = { 0x00069, 0x00000 };
static unsigned int de1D423[] = { 0x0006A, 0x00000 };
static unsigned int de1D424[] = { 0x0006B, 0x00000 };
static unsigned int de1D425[] = { 0x0006C, 0x00000 };
static unsigned int de1D426[] = { 0x0006D, 0x00000 };
static unsigned int de1D427[] = { 0x0006E, 0x00000 };
static unsigned int de1D428[] = { 0x0006F, 0x00000 };
static unsigned int de1D429[] = { 0x00070, 0x00000 };
static unsigned int de1D42A[] = { 0x00071, 0x00000 };
static unsigned int de1D42B[] = { 0x00072, 0x00000 };
static unsigned int de1D42C[] = { 0x00073, 0x00000 };
static unsigned int de1D42D[] = { 0x00074, 0x00000 };
static unsigned int de1D42E[] = { 0x00075, 0x00000 };
static unsigned int de1D42F[] = { 0x00076, 0x00000 };
static unsigned int de1D430[] = { 0x00077, 0x00000 };
static unsigned int de1D431[] = { 0x00078, 0x00000 };
static unsigned int de1D432[] = { 0x00079, 0x00000 };
static unsigned int de1D433[] = { 0x0007A, 0x00000 };
static unsigned int de1D434[] = { 0x00041, 0x00000 };
static unsigned int de1D435[] = { 0x00042, 0x00000 };
static unsigned int de1D436[] = { 0x00043, 0x00000 };
static unsigned int de1D437[] = { 0x00044, 0x00000 };
static unsigned int de1D438[] = { 0x00045, 0x00000 };
static unsigned int de1D439[] = { 0x00046, 0x00000 };
static unsigned int de1D43A[] = { 0x00047, 0x00000 };
static unsigned int de1D43B[] = { 0x00048, 0x00000 };
static unsigned int de1D43C[] = { 0x00049, 0x00000 };
static unsigned int de1D43D[] = { 0x0004A, 0x00000 };
static unsigned int de1D43E[] = { 0x0004B, 0x00000 };
static unsigned int de1D43F[] = { 0x0004C, 0x00000 };
static unsigned int de1D440[] = { 0x0004D, 0x00000 };
static unsigned int de1D441[] = { 0x0004E, 0x00000 };
static unsigned int de1D442[] = { 0x0004F, 0x00000 };
static unsigned int de1D443[] = { 0x00050, 0x00000 };
static unsigned int de1D444[] = { 0x00051, 0x00000 };
static unsigned int de1D445[] = { 0x00052, 0x00000 };
static unsigned int de1D446[] = { 0x00053, 0x00000 };
static unsigned int de1D447[] = { 0x00054, 0x00000 };
static unsigned int de1D448[] = { 0x00055, 0x00000 };
static unsigned int de1D449[] = { 0x00056, 0x00000 };
static unsigned int de1D44A[] = { 0x00057, 0x00000 };
static unsigned int de1D44B[] = { 0x00058, 0x00000 };
static unsigned int de1D44C[] = { 0x00059, 0x00000 };
static unsigned int de1D44D[] = { 0x0005A, 0x00000 };
static unsigned int de1D44E[] = { 0x00061, 0x00000 };
static unsigned int de1D44F[] = { 0x00062, 0x00000 };
static unsigned int de1D450[] = { 0x00063, 0x00000 };
static unsigned int de1D451[] = { 0x00064, 0x00000 };
static unsigned int de1D452[] = { 0x00065, 0x00000 };
static unsigned int de1D453[] = { 0x00066, 0x00000 };
static unsigned int de1D454[] = { 0x00067, 0x00000 };
static unsigned int de1D456[] = { 0x00069, 0x00000 };
static unsigned int de1D457[] = { 0x0006A, 0x00000 };
static unsigned int de1D458[] = { 0x0006B, 0x00000 };
static unsigned int de1D459[] = { 0x0006C, 0x00000 };
static unsigned int de1D45A[] = { 0x0006D, 0x00000 };
static unsigned int de1D45B[] = { 0x0006E, 0x00000 };
static unsigned int de1D45C[] = { 0x0006F, 0x00000 };
static unsigned int de1D45D[] = { 0x00070, 0x00000 };
static unsigned int de1D45E[] = { 0x00071, 0x00000 };
static unsigned int de1D45F[] = { 0x00072, 0x00000 };
static unsigned int de1D460[] = { 0x00073, 0x00000 };
static unsigned int de1D461[] = { 0x00074, 0x00000 };
static unsigned int de1D462[] = { 0x00075, 0x00000 };
static unsigned int de1D463[] = { 0x00076, 0x00000 };
static unsigned int de1D464[] = { 0x00077, 0x00000 };
static unsigned int de1D465[] = { 0x00078, 0x00000 };
static unsigned int de1D466[] = { 0x00079, 0x00000 };
static unsigned int de1D467[] = { 0x0007A, 0x00000 };
static unsigned int de1D468[] = { 0x00041, 0x00000 };
static unsigned int de1D469[] = { 0x00042, 0x00000 };
static unsigned int de1D46A[] = { 0x00043, 0x00000 };
static unsigned int de1D46B[] = { 0x00044, 0x00000 };
static unsigned int de1D46C[] = { 0x00045, 0x00000 };
static unsigned int de1D46D[] = { 0x00046, 0x00000 };
static unsigned int de1D46E[] = { 0x00047, 0x00000 };
static unsigned int de1D46F[] = { 0x00048, 0x00000 };
static unsigned int de1D470[] = { 0x00049, 0x00000 };
static unsigned int de1D471[] = { 0x0004A, 0x00000 };
static unsigned int de1D472[] = { 0x0004B, 0x00000 };
static unsigned int de1D473[] = { 0x0004C, 0x00000 };
static unsigned int de1D474[] = { 0x0004D, 0x00000 };
static unsigned int de1D475[] = { 0x0004E, 0x00000 };
static unsigned int de1D476[] = { 0x0004F, 0x00000 };
static unsigned int de1D477[] = { 0x00050, 0x00000 };
static unsigned int de1D478[] = { 0x00051, 0x00000 };
static unsigned int de1D479[] = { 0x00052, 0x00000 };
static unsigned int de1D47A[] = { 0x00053, 0x00000 };
static unsigned int de1D47B[] = { 0x00054, 0x00000 };
static unsigned int de1D47C[] = { 0x00055, 0x00000 };
static unsigned int de1D47D[] = { 0x00056, 0x00000 };
static unsigned int de1D47E[] = { 0x00057, 0x00000 };
static unsigned int de1D47F[] = { 0x00058, 0x00000 };
static unsigned int de1D480[] = { 0x00059, 0x00000 };
static unsigned int de1D481[] = { 0x0005A, 0x00000 };
static unsigned int de1D482[] = { 0x00061, 0x00000 };
static unsigned int de1D483[] = { 0x00062, 0x00000 };
static unsigned int de1D484[] = { 0x00063, 0x00000 };
static unsigned int de1D485[] = { 0x00064, 0x00000 };
static unsigned int de1D486[] = { 0x00065, 0x00000 };
static unsigned int de1D487[] = { 0x00066, 0x00000 };
static unsigned int de1D488[] = { 0x00067, 0x00000 };
static unsigned int de1D489[] = { 0x00068, 0x00000 };
static unsigned int de1D48A[] = { 0x00069, 0x00000 };
static unsigned int de1D48B[] = { 0x0006A, 0x00000 };
static unsigned int de1D48C[] = { 0x0006B, 0x00000 };
static unsigned int de1D48D[] = { 0x0006C, 0x00000 };
static unsigned int de1D48E[] = { 0x0006D, 0x00000 };
static unsigned int de1D48F[] = { 0x0006E, 0x00000 };
static unsigned int de1D490[] = { 0x0006F, 0x00000 };
static unsigned int de1D491[] = { 0x00070, 0x00000 };
static unsigned int de1D492[] = { 0x00071, 0x00000 };
static unsigned int de1D493[] = { 0x00072, 0x00000 };
static unsigned int de1D494[] = { 0x00073, 0x00000 };
static unsigned int de1D495[] = { 0x00074, 0x00000 };
static unsigned int de1D496[] = { 0x00075, 0x00000 };
static unsigned int de1D497[] = { 0x00076, 0x00000 };
static unsigned int de1D498[] = { 0x00077, 0x00000 };
static unsigned int de1D499[] = { 0x00078, 0x00000 };
static unsigned int de1D49A[] = { 0x00079, 0x00000 };
static unsigned int de1D49B[] = { 0x0007A, 0x00000 };
static unsigned int de1D49C[] = { 0x00041, 0x00000 };
static unsigned int de1D49E[] = { 0x00043, 0x00000 };
static unsigned int de1D49F[] = { 0x00044, 0x00000 };
static unsigned int de1D4A2[] = { 0x00047, 0x00000 };
static unsigned int de1D4A5[] = { 0x0004A, 0x00000 };
static unsigned int de1D4A6[] = { 0x0004B, 0x00000 };
static unsigned int de1D4A9[] = { 0x0004E, 0x00000 };
static unsigned int de1D4AA[] = { 0x0004F, 0x00000 };
static unsigned int de1D4AB[] = { 0x00050, 0x00000 };
static unsigned int de1D4AC[] = { 0x00051, 0x00000 };
static unsigned int de1D4AE[] = { 0x00053, 0x00000 };
static unsigned int de1D4AF[] = { 0x00054, 0x00000 };
static unsigned int de1D4B0[] = { 0x00055, 0x00000 };
static unsigned int de1D4B1[] = { 0x00056, 0x00000 };
static unsigned int de1D4B2[] = { 0x00057, 0x00000 };
static unsigned int de1D4B3[] = { 0x00058, 0x00000 };
static unsigned int de1D4B4[] = { 0x00059, 0x00000 };
static unsigned int de1D4B5[] = { 0x0005A, 0x00000 };
static unsigned int de1D4B6[] = { 0x00061, 0x00000 };
static unsigned int de1D4B7[] = { 0x00062, 0x00000 };
static unsigned int de1D4B8[] = { 0x00063, 0x00000 };
static unsigned int de1D4B9[] = { 0x00064, 0x00000 };
static unsigned int de1D4BB[] = { 0x00066, 0x00000 };
static unsigned int de1D4BD[] = { 0x00068, 0x00000 };
static unsigned int de1D4BE[] = { 0x00069, 0x00000 };
static unsigned int de1D4BF[] = { 0x0006A, 0x00000 };
static unsigned int de1D4C0[] = { 0x0006B, 0x00000 };
static unsigned int de1D4C1[] = { 0x0006C, 0x00000 };
static unsigned int de1D4C2[] = { 0x0006D, 0x00000 };
static unsigned int de1D4C3[] = { 0x0006E, 0x00000 };
static unsigned int de1D4C5[] = { 0x00070, 0x00000 };
static unsigned int de1D4C6[] = { 0x00071, 0x00000 };
static unsigned int de1D4C7[] = { 0x00072, 0x00000 };
static unsigned int de1D4C8[] = { 0x00073, 0x00000 };
static unsigned int de1D4C9[] = { 0x00074, 0x00000 };
static unsigned int de1D4CA[] = { 0x00075, 0x00000 };
static unsigned int de1D4CB[] = { 0x00076, 0x00000 };
static unsigned int de1D4CC[] = { 0x00077, 0x00000 };
static unsigned int de1D4CD[] = { 0x00078, 0x00000 };
static unsigned int de1D4CE[] = { 0x00079, 0x00000 };
static unsigned int de1D4CF[] = { 0x0007A, 0x00000 };
static unsigned int de1D4D0[] = { 0x00041, 0x00000 };
static unsigned int de1D4D1[] = { 0x00042, 0x00000 };
static unsigned int de1D4D2[] = { 0x00043, 0x00000 };
static unsigned int de1D4D3[] = { 0x00044, 0x00000 };
static unsigned int de1D4D4[] = { 0x00045, 0x00000 };
static unsigned int de1D4D5[] = { 0x00046, 0x00000 };
static unsigned int de1D4D6[] = { 0x00047, 0x00000 };
static unsigned int de1D4D7[] = { 0x00048, 0x00000 };
static unsigned int de1D4D8[] = { 0x00049, 0x00000 };
static unsigned int de1D4D9[] = { 0x0004A, 0x00000 };
static unsigned int de1D4DA[] = { 0x0004B, 0x00000 };
static unsigned int de1D4DB[] = { 0x0004C, 0x00000 };
static unsigned int de1D4DC[] = { 0x0004D, 0x00000 };
static unsigned int de1D4DD[] = { 0x0004E, 0x00000 };
static unsigned int de1D4DE[] = { 0x0004F, 0x00000 };
static unsigned int de1D4DF[] = { 0x00050, 0x00000 };
static unsigned int de1D4E0[] = { 0x00051, 0x00000 };
static unsigned int de1D4E1[] = { 0x00052, 0x00000 };
static unsigned int de1D4E2[] = { 0x00053, 0x00000 };
static unsigned int de1D4E3[] = { 0x00054, 0x00000 };
static unsigned int de1D4E4[] = { 0x00055, 0x00000 };
static unsigned int de1D4E5[] = { 0x00056, 0x00000 };
static unsigned int de1D4E6[] = { 0x00057, 0x00000 };
static unsigned int de1D4E7[] = { 0x00058, 0x00000 };
static unsigned int de1D4E8[] = { 0x00059, 0x00000 };
static unsigned int de1D4E9[] = { 0x0005A, 0x00000 };
static unsigned int de1D4EA[] = { 0x00061, 0x00000 };
static unsigned int de1D4EB[] = { 0x00062, 0x00000 };
static unsigned int de1D4EC[] = { 0x00063, 0x00000 };
static unsigned int de1D4ED[] = { 0x00064, 0x00000 };
static unsigned int de1D4EE[] = { 0x00065, 0x00000 };
static unsigned int de1D4EF[] = { 0x00066, 0x00000 };
static unsigned int de1D4F0[] = { 0x00067, 0x00000 };
static unsigned int de1D4F1[] = { 0x00068, 0x00000 };
static unsigned int de1D4F2[] = { 0x00069, 0x00000 };
static unsigned int de1D4F3[] = { 0x0006A, 0x00000 };
static unsigned int de1D4F4[] = { 0x0006B, 0x00000 };
static unsigned int de1D4F5[] = { 0x0006C, 0x00000 };
static unsigned int de1D4F6[] = { 0x0006D, 0x00000 };
static unsigned int de1D4F7[] = { 0x0006E, 0x00000 };
static unsigned int de1D4F8[] = { 0x0006F, 0x00000 };
static unsigned int de1D4F9[] = { 0x00070, 0x00000 };
static unsigned int de1D4FA[] = { 0x00071, 0x00000 };
static unsigned int de1D4FB[] = { 0x00072, 0x00000 };
static unsigned int de1D4FC[] = { 0x00073, 0x00000 };
static unsigned int de1D4FD[] = { 0x00074, 0x00000 };
static unsigned int de1D4FE[] = { 0x00075, 0x00000 };
static unsigned int de1D4FF[] = { 0x00076, 0x00000 };
static unsigned int de1D500[] = { 0x00077, 0x00000 };
static unsigned int de1D501[] = { 0x00078, 0x00000 };
static unsigned int de1D502[] = { 0x00079, 0x00000 };
static unsigned int de1D503[] = { 0x0007A, 0x00000 };
static unsigned int de1D504[] = { 0x00041, 0x00000 };
static unsigned int de1D505[] = { 0x00042, 0x00000 };
static unsigned int de1D507[] = { 0x00044, 0x00000 };
static unsigned int de1D508[] = { 0x00045, 0x00000 };
static unsigned int de1D509[] = { 0x00046, 0x00000 };
static unsigned int de1D50A[] = { 0x00047, 0x00000 };
static unsigned int de1D50D[] = { 0x0004A, 0x00000 };
static unsigned int de1D50E[] = { 0x0004B, 0x00000 };
static unsigned int de1D50F[] = { 0x0004C, 0x00000 };
static unsigned int de1D510[] = { 0x0004D, 0x00000 };
static unsigned int de1D511[] = { 0x0004E, 0x00000 };
static unsigned int de1D512[] = { 0x0004F, 0x00000 };
static unsigned int de1D513[] = { 0x00050, 0x00000 };
static unsigned int de1D514[] = { 0x00051, 0x00000 };
static unsigned int de1D516[] = { 0x00053, 0x00000 };
static unsigned int de1D517[] = { 0x00054, 0x00000 };
static unsigned int de1D518[] = { 0x00055, 0x00000 };
static unsigned int de1D519[] = { 0x00056, 0x00000 };
static unsigned int de1D51A[] = { 0x00057, 0x00000 };
static unsigned int de1D51B[] = { 0x00058, 0x00000 };
static unsigned int de1D51C[] = { 0x00059, 0x00000 };
static unsigned int de1D51E[] = { 0x00061, 0x00000 };
static unsigned int de1D51F[] = { 0x00062, 0x00000 };
static unsigned int de1D520[] = { 0x00063, 0x00000 };
static unsigned int de1D521[] = { 0x00064, 0x00000 };
static unsigned int de1D522[] = { 0x00065, 0x00000 };
static unsigned int de1D523[] = { 0x00066, 0x00000 };
static unsigned int de1D524[] = { 0x00067, 0x00000 };
static unsigned int de1D525[] = { 0x00068, 0x00000 };
static unsigned int de1D526[] = { 0x00069, 0x00000 };
static unsigned int de1D527[] = { 0x0006A, 0x00000 };
static unsigned int de1D528[] = { 0x0006B, 0x00000 };
static unsigned int de1D529[] = { 0x0006C, 0x00000 };
static unsigned int de1D52A[] = { 0x0006D, 0x00000 };
static unsigned int de1D52B[] = { 0x0006E, 0x00000 };
static unsigned int de1D52C[] = { 0x0006F, 0x00000 };
static unsigned int de1D52D[] = { 0x00070, 0x00000 };
static unsigned int de1D52E[] = { 0x00071, 0x00000 };
static unsigned int de1D52F[] = { 0x00072, 0x00000 };
static unsigned int de1D530[] = { 0x00073, 0x00000 };
static unsigned int de1D531[] = { 0x00074, 0x00000 };
static unsigned int de1D532[] = { 0x00075, 0x00000 };
static unsigned int de1D533[] = { 0x00076, 0x00000 };
static unsigned int de1D534[] = { 0x00077, 0x00000 };
static unsigned int de1D535[] = { 0x00078, 0x00000 };
static unsigned int de1D536[] = { 0x00079, 0x00000 };
static unsigned int de1D537[] = { 0x0007A, 0x00000 };
static unsigned int de1D538[] = { 0x00041, 0x00000 };
static unsigned int de1D539[] = { 0x00042, 0x00000 };
static unsigned int de1D53B[] = { 0x00044, 0x00000 };
static unsigned int de1D53C[] = { 0x00045, 0x00000 };
static unsigned int de1D53D[] = { 0x00046, 0x00000 };
static unsigned int de1D53E[] = { 0x00047, 0x00000 };
static unsigned int de1D540[] = { 0x00049, 0x00000 };
static unsigned int de1D541[] = { 0x0004A, 0x00000 };
static unsigned int de1D542[] = { 0x0004B, 0x00000 };
static unsigned int de1D543[] = { 0x0004C, 0x00000 };
static unsigned int de1D544[] = { 0x0004D, 0x00000 };
static unsigned int de1D546[] = { 0x0004F, 0x00000 };
static unsigned int de1D54A[] = { 0x00053, 0x00000 };
static unsigned int de1D54B[] = { 0x00054, 0x00000 };
static unsigned int de1D54C[] = { 0x00055, 0x00000 };
static unsigned int de1D54D[] = { 0x00056, 0x00000 };
static unsigned int de1D54E[] = { 0x00057, 0x00000 };
static unsigned int de1D54F[] = { 0x00058, 0x00000 };
static unsigned int de1D550[] = { 0x00059, 0x00000 };
static unsigned int de1D552[] = { 0x00061, 0x00000 };
static unsigned int de1D553[] = { 0x00062, 0x00000 };
static unsigned int de1D554[] = { 0x00063, 0x00000 };
static unsigned int de1D555[] = { 0x00064, 0x00000 };
static unsigned int de1D556[] = { 0x00065, 0x00000 };
static unsigned int de1D557[] = { 0x00066, 0x00000 };
static unsigned int de1D558[] = { 0x00067, 0x00000 };
static unsigned int de1D559[] = { 0x00068, 0x00000 };
static unsigned int de1D55A[] = { 0x00069, 0x00000 };
static unsigned int de1D55B[] = { 0x0006A, 0x00000 };
static unsigned int de1D55C[] = { 0x0006B, 0x00000 };
static unsigned int de1D55D[] = { 0x0006C, 0x00000 };
static unsigned int de1D55E[] = { 0x0006D, 0x00000 };
static unsigned int de1D55F[] = { 0x0006E, 0x00000 };
static unsigned int de1D560[] = { 0x0006F, 0x00000 };
static unsigned int de1D561[] = { 0x00070, 0x00000 };
static unsigned int de1D562[] = { 0x00071, 0x00000 };
static unsigned int de1D563[] = { 0x00072, 0x00000 };
static unsigned int de1D564[] = { 0x00073, 0x00000 };
static unsigned int de1D565[] = { 0x00074, 0x00000 };
static unsigned int de1D566[] = { 0x00075, 0x00000 };
static unsigned int de1D567[] = { 0x00076, 0x00000 };
static unsigned int de1D568[] = { 0x00077, 0x00000 };
static unsigned int de1D569[] = { 0x00078, 0x00000 };
static unsigned int de1D56A[] = { 0x00079, 0x00000 };
static unsigned int de1D56B[] = { 0x0007A, 0x00000 };
static unsigned int de1D56C[] = { 0x00041, 0x00000 };
static unsigned int de1D56D[] = { 0x00042, 0x00000 };
static unsigned int de1D56E[] = { 0x00043, 0x00000 };
static unsigned int de1D56F[] = { 0x00044, 0x00000 };
static unsigned int de1D570[] = { 0x00045, 0x00000 };
static unsigned int de1D571[] = { 0x00046, 0x00000 };
static unsigned int de1D572[] = { 0x00047, 0x00000 };
static unsigned int de1D573[] = { 0x00048, 0x00000 };
static unsigned int de1D574[] = { 0x00049, 0x00000 };
static unsigned int de1D575[] = { 0x0004A, 0x00000 };
static unsigned int de1D576[] = { 0x0004B, 0x00000 };
static unsigned int de1D577[] = { 0x0004C, 0x00000 };
static unsigned int de1D578[] = { 0x0004D, 0x00000 };
static unsigned int de1D579[] = { 0x0004E, 0x00000 };
static unsigned int de1D57A[] = { 0x0004F, 0x00000 };
static unsigned int de1D57B[] = { 0x00050, 0x00000 };
static unsigned int de1D57C[] = { 0x00051, 0x00000 };
static unsigned int de1D57D[] = { 0x00052, 0x00000 };
static unsigned int de1D57E[] = { 0x00053, 0x00000 };
static unsigned int de1D57F[] = { 0x00054, 0x00000 };
static unsigned int de1D580[] = { 0x00055, 0x00000 };
static unsigned int de1D581[] = { 0x00056, 0x00000 };
static unsigned int de1D582[] = { 0x00057, 0x00000 };
static unsigned int de1D583[] = { 0x00058, 0x00000 };
static unsigned int de1D584[] = { 0x00059, 0x00000 };
static unsigned int de1D585[] = { 0x0005A, 0x00000 };
static unsigned int de1D586[] = { 0x00061, 0x00000 };
static unsigned int de1D587[] = { 0x00062, 0x00000 };
static unsigned int de1D588[] = { 0x00063, 0x00000 };
static unsigned int de1D589[] = { 0x00064, 0x00000 };
static unsigned int de1D58A[] = { 0x00065, 0x00000 };
static unsigned int de1D58B[] = { 0x00066, 0x00000 };
static unsigned int de1D58C[] = { 0x00067, 0x00000 };
static unsigned int de1D58D[] = { 0x00068, 0x00000 };
static unsigned int de1D58E[] = { 0x00069, 0x00000 };
static unsigned int de1D58F[] = { 0x0006A, 0x00000 };
static unsigned int de1D590[] = { 0x0006B, 0x00000 };
static unsigned int de1D591[] = { 0x0006C, 0x00000 };
static unsigned int de1D592[] = { 0x0006D, 0x00000 };
static unsigned int de1D593[] = { 0x0006E, 0x00000 };
static unsigned int de1D594[] = { 0x0006F, 0x00000 };
static unsigned int de1D595[] = { 0x00070, 0x00000 };
static unsigned int de1D596[] = { 0x00071, 0x00000 };
static unsigned int de1D597[] = { 0x00072, 0x00000 };
static unsigned int de1D598[] = { 0x00073, 0x00000 };
static unsigned int de1D599[] = { 0x00074, 0x00000 };
static unsigned int de1D59A[] = { 0x00075, 0x00000 };
static unsigned int de1D59B[] = { 0x00076, 0x00000 };
static unsigned int de1D59C[] = { 0x00077, 0x00000 };
static unsigned int de1D59D[] = { 0x00078, 0x00000 };
static unsigned int de1D59E[] = { 0x00079, 0x00000 };
static unsigned int de1D59F[] = { 0x0007A, 0x00000 };
static unsigned int de1D5A0[] = { 0x00041, 0x00000 };
static unsigned int de1D5A1[] = { 0x00042, 0x00000 };
static unsigned int de1D5A2[] = { 0x00043, 0x00000 };
static unsigned int de1D5A3[] = { 0x00044, 0x00000 };
static unsigned int de1D5A4[] = { 0x00045, 0x00000 };
static unsigned int de1D5A5[] = { 0x00046, 0x00000 };
static unsigned int de1D5A6[] = { 0x00047, 0x00000 };
static unsigned int de1D5A7[] = { 0x00048, 0x00000 };
static unsigned int de1D5A8[] = { 0x00049, 0x00000 };
static unsigned int de1D5A9[] = { 0x0004A, 0x00000 };
static unsigned int de1D5AA[] = { 0x0004B, 0x00000 };
static unsigned int de1D5AB[] = { 0x0004C, 0x00000 };
static unsigned int de1D5AC[] = { 0x0004D, 0x00000 };
static unsigned int de1D5AD[] = { 0x0004E, 0x00000 };
static unsigned int de1D5AE[] = { 0x0004F, 0x00000 };
static unsigned int de1D5AF[] = { 0x00050, 0x00000 };
static unsigned int de1D5B0[] = { 0x00051, 0x00000 };
static unsigned int de1D5B1[] = { 0x00052, 0x00000 };
static unsigned int de1D5B2[] = { 0x00053, 0x00000 };
static unsigned int de1D5B3[] = { 0x00054, 0x00000 };
static unsigned int de1D5B4[] = { 0x00055, 0x00000 };
static unsigned int de1D5B5[] = { 0x00056, 0x00000 };
static unsigned int de1D5B6[] = { 0x00057, 0x00000 };
static unsigned int de1D5B7[] = { 0x00058, 0x00000 };
static unsigned int de1D5B8[] = { 0x00059, 0x00000 };
static unsigned int de1D5B9[] = { 0x0005A, 0x00000 };
static unsigned int de1D5BA[] = { 0x00061, 0x00000 };
static unsigned int de1D5BB[] = { 0x00062, 0x00000 };
static unsigned int de1D5BC[] = { 0x00063, 0x00000 };
static unsigned int de1D5BD[] = { 0x00064, 0x00000 };
static unsigned int de1D5BE[] = { 0x00065, 0x00000 };
static unsigned int de1D5BF[] = { 0x00066, 0x00000 };
static unsigned int de1D5C0[] = { 0x00067, 0x00000 };
static unsigned int de1D5C1[] = { 0x00068, 0x00000 };
static unsigned int de1D5C2[] = { 0x00069, 0x00000 };
static unsigned int de1D5C3[] = { 0x0006A, 0x00000 };
static unsigned int de1D5C4[] = { 0x0006B, 0x00000 };
static unsigned int de1D5C5[] = { 0x0006C, 0x00000 };
static unsigned int de1D5C6[] = { 0x0006D, 0x00000 };
static unsigned int de1D5C7[] = { 0x0006E, 0x00000 };
static unsigned int de1D5C8[] = { 0x0006F, 0x00000 };
static unsigned int de1D5C9[] = { 0x00070, 0x00000 };
static unsigned int de1D5CA[] = { 0x00071, 0x00000 };
static unsigned int de1D5CB[] = { 0x00072, 0x00000 };
static unsigned int de1D5CC[] = { 0x00073, 0x00000 };
static unsigned int de1D5CD[] = { 0x00074, 0x00000 };
static unsigned int de1D5CE[] = { 0x00075, 0x00000 };
static unsigned int de1D5CF[] = { 0x00076, 0x00000 };
static unsigned int de1D5D0[] = { 0x00077, 0x00000 };
static unsigned int de1D5D1[] = { 0x00078, 0x00000 };
static unsigned int de1D5D2[] = { 0x00079, 0x00000 };
static unsigned int de1D5D3[] = { 0x0007A, 0x00000 };
static unsigned int de1D5D4[] = { 0x00041, 0x00000 };
static unsigned int de1D5D5[] = { 0x00042, 0x00000 };
static unsigned int de1D5D6[] = { 0x00043, 0x00000 };
static unsigned int de1D5D7[] = { 0x00044, 0x00000 };
static unsigned int de1D5D8[] = { 0x00045, 0x00000 };
static unsigned int de1D5D9[] = { 0x00046, 0x00000 };
static unsigned int de1D5DA[] = { 0x00047, 0x00000 };
static unsigned int de1D5DB[] = { 0x00048, 0x00000 };
static unsigned int de1D5DC[] = { 0x00049, 0x00000 };
static unsigned int de1D5DD[] = { 0x0004A, 0x00000 };
static unsigned int de1D5DE[] = { 0x0004B, 0x00000 };
static unsigned int de1D5DF[] = { 0x0004C, 0x00000 };
static unsigned int de1D5E0[] = { 0x0004D, 0x00000 };
static unsigned int de1D5E1[] = { 0x0004E, 0x00000 };
static unsigned int de1D5E2[] = { 0x0004F, 0x00000 };
static unsigned int de1D5E3[] = { 0x00050, 0x00000 };
static unsigned int de1D5E4[] = { 0x00051, 0x00000 };
static unsigned int de1D5E5[] = { 0x00052, 0x00000 };
static unsigned int de1D5E6[] = { 0x00053, 0x00000 };
static unsigned int de1D5E7[] = { 0x00054, 0x00000 };
static unsigned int de1D5E8[] = { 0x00055, 0x00000 };
static unsigned int de1D5E9[] = { 0x00056, 0x00000 };
static unsigned int de1D5EA[] = { 0x00057, 0x00000 };
static unsigned int de1D5EB[] = { 0x00058, 0x00000 };
static unsigned int de1D5EC[] = { 0x00059, 0x00000 };
static unsigned int de1D5ED[] = { 0x0005A, 0x00000 };
static unsigned int de1D5EE[] = { 0x00061, 0x00000 };
static unsigned int de1D5EF[] = { 0x00062, 0x00000 };
static unsigned int de1D5F0[] = { 0x00063, 0x00000 };
static unsigned int de1D5F1[] = { 0x00064, 0x00000 };
static unsigned int de1D5F2[] = { 0x00065, 0x00000 };
static unsigned int de1D5F3[] = { 0x00066, 0x00000 };
static unsigned int de1D5F4[] = { 0x00067, 0x00000 };
static unsigned int de1D5F5[] = { 0x00068, 0x00000 };
static unsigned int de1D5F6[] = { 0x00069, 0x00000 };
static unsigned int de1D5F7[] = { 0x0006A, 0x00000 };
static unsigned int de1D5F8[] = { 0x0006B, 0x00000 };
static unsigned int de1D5F9[] = { 0x0006C, 0x00000 };
static unsigned int de1D5FA[] = { 0x0006D, 0x00000 };
static unsigned int de1D5FB[] = { 0x0006E, 0x00000 };
static unsigned int de1D5FC[] = { 0x0006F, 0x00000 };
static unsigned int de1D5FD[] = { 0x00070, 0x00000 };
static unsigned int de1D5FE[] = { 0x00071, 0x00000 };
static unsigned int de1D5FF[] = { 0x00072, 0x00000 };
static unsigned int de1D600[] = { 0x00073, 0x00000 };
static unsigned int de1D601[] = { 0x00074, 0x00000 };
static unsigned int de1D602[] = { 0x00075, 0x00000 };
static unsigned int de1D603[] = { 0x00076, 0x00000 };
static unsigned int de1D604[] = { 0x00077, 0x00000 };
static unsigned int de1D605[] = { 0x00078, 0x00000 };
static unsigned int de1D606[] = { 0x00079, 0x00000 };
static unsigned int de1D607[] = { 0x0007A, 0x00000 };
static unsigned int de1D608[] = { 0x00041, 0x00000 };
static unsigned int de1D609[] = { 0x00042, 0x00000 };
static unsigned int de1D60A[] = { 0x00043, 0x00000 };
static unsigned int de1D60B[] = { 0x00044, 0x00000 };
static unsigned int de1D60C[] = { 0x00045, 0x00000 };
static unsigned int de1D60D[] = { 0x00046, 0x00000 };
static unsigned int de1D60E[] = { 0x00047, 0x00000 };
static unsigned int de1D60F[] = { 0x00048, 0x00000 };
static unsigned int de1D610[] = { 0x00049, 0x00000 };
static unsigned int de1D611[] = { 0x0004A, 0x00000 };
static unsigned int de1D612[] = { 0x0004B, 0x00000 };
static unsigned int de1D613[] = { 0x0004C, 0x00000 };
static unsigned int de1D614[] = { 0x0004D, 0x00000 };
static unsigned int de1D615[] = { 0x0004E, 0x00000 };
static unsigned int de1D616[] = { 0x0004F, 0x00000 };
static unsigned int de1D617[] = { 0x00050, 0x00000 };
static unsigned int de1D618[] = { 0x00051, 0x00000 };
static unsigned int de1D619[] = { 0x00052, 0x00000 };
static unsigned int de1D61A[] = { 0x00053, 0x00000 };
static unsigned int de1D61B[] = { 0x00054, 0x00000 };
static unsigned int de1D61C[] = { 0x00055, 0x00000 };
static unsigned int de1D61D[] = { 0x00056, 0x00000 };
static unsigned int de1D61E[] = { 0x00057, 0x00000 };
static unsigned int de1D61F[] = { 0x00058, 0x00000 };
static unsigned int de1D620[] = { 0x00059, 0x00000 };
static unsigned int de1D621[] = { 0x0005A, 0x00000 };
static unsigned int de1D622[] = { 0x00061, 0x00000 };
static unsigned int de1D623[] = { 0x00062, 0x00000 };
static unsigned int de1D624[] = { 0x00063, 0x00000 };
static unsigned int de1D625[] = { 0x00064, 0x00000 };
static unsigned int de1D626[] = { 0x00065, 0x00000 };
static unsigned int de1D627[] = { 0x00066, 0x00000 };
static unsigned int de1D628[] = { 0x00067, 0x00000 };
static unsigned int de1D629[] = { 0x00068, 0x00000 };
static unsigned int de1D62A[] = { 0x00069, 0x00000 };
static unsigned int de1D62B[] = { 0x0006A, 0x00000 };
static unsigned int de1D62C[] = { 0x0006B, 0x00000 };
static unsigned int de1D62D[] = { 0x0006C, 0x00000 };
static unsigned int de1D62E[] = { 0x0006D, 0x00000 };
static unsigned int de1D62F[] = { 0x0006E, 0x00000 };
static unsigned int de1D630[] = { 0x0006F, 0x00000 };
static unsigned int de1D631[] = { 0x00070, 0x00000 };
static unsigned int de1D632[] = { 0x00071, 0x00000 };
static unsigned int de1D633[] = { 0x00072, 0x00000 };
static unsigned int de1D634[] = { 0x00073, 0x00000 };
static unsigned int de1D635[] = { 0x00074, 0x00000 };
static unsigned int de1D636[] = { 0x00075, 0x00000 };
static unsigned int de1D637[] = { 0x00076, 0x00000 };
static unsigned int de1D638[] = { 0x00077, 0x00000 };
static unsigned int de1D639[] = { 0x00078, 0x00000 };
static unsigned int de1D63A[] = { 0x00079, 0x00000 };
static unsigned int de1D63B[] = { 0x0007A, 0x00000 };
static unsigned int de1D63C[] = { 0x00041, 0x00000 };
static unsigned int de1D63D[] = { 0x00042, 0x00000 };
static unsigned int de1D63E[] = { 0x00043, 0x00000 };
static unsigned int de1D63F[] = { 0x00044, 0x00000 };
static unsigned int de1D640[] = { 0x00045, 0x00000 };
static unsigned int de1D641[] = { 0x00046, 0x00000 };
static unsigned int de1D642[] = { 0x00047, 0x00000 };
static unsigned int de1D643[] = { 0x00048, 0x00000 };
static unsigned int de1D644[] = { 0x00049, 0x00000 };
static unsigned int de1D645[] = { 0x0004A, 0x00000 };
static unsigned int de1D646[] = { 0x0004B, 0x00000 };
static unsigned int de1D647[] = { 0x0004C, 0x00000 };
static unsigned int de1D648[] = { 0x0004D, 0x00000 };
static unsigned int de1D649[] = { 0x0004E, 0x00000 };
static unsigned int de1D64A[] = { 0x0004F, 0x00000 };
static unsigned int de1D64B[] = { 0x00050, 0x00000 };
static unsigned int de1D64C[] = { 0x00051, 0x00000 };
static unsigned int de1D64D[] = { 0x00052, 0x00000 };
static unsigned int de1D64E[] = { 0x00053, 0x00000 };
static unsigned int de1D64F[] = { 0x00054, 0x00000 };
static unsigned int de1D650[] = { 0x00055, 0x00000 };
static unsigned int de1D651[] = { 0x00056, 0x00000 };
static unsigned int de1D652[] = { 0x00057, 0x00000 };
static unsigned int de1D653[] = { 0x00058, 0x00000 };
static unsigned int de1D654[] = { 0x00059, 0x00000 };
static unsigned int de1D655[] = { 0x0005A, 0x00000 };
static unsigned int de1D656[] = { 0x00061, 0x00000 };
static unsigned int de1D657[] = { 0x00062, 0x00000 };
static unsigned int de1D658[] = { 0x00063, 0x00000 };
static unsigned int de1D659[] = { 0x00064, 0x00000 };
static unsigned int de1D65A[] = { 0x00065, 0x00000 };
static unsigned int de1D65B[] = { 0x00066, 0x00000 };
static unsigned int de1D65C[] = { 0x00067, 0x00000 };
static unsigned int de1D65D[] = { 0x00068, 0x00000 };
static unsigned int de1D65E[] = { 0x00069, 0x00000 };
static unsigned int de1D65F[] = { 0x0006A, 0x00000 };
static unsigned int de1D660[] = { 0x0006B, 0x00000 };
static unsigned int de1D661[] = { 0x0006C, 0x00000 };
static unsigned int de1D662[] = { 0x0006D, 0x00000 };
static unsigned int de1D663[] = { 0x0006E, 0x00000 };
static unsigned int de1D664[] = { 0x0006F, 0x00000 };
static unsigned int de1D665[] = { 0x00070, 0x00000 };
static unsigned int de1D666[] = { 0x00071, 0x00000 };
static unsigned int de1D667[] = { 0x00072, 0x00000 };
static unsigned int de1D668[] = { 0x00073, 0x00000 };
static unsigned int de1D669[] = { 0x00074, 0x00000 };
static unsigned int de1D66A[] = { 0x00075, 0x00000 };
static unsigned int de1D66B[] = { 0x00076, 0x00000 };
static unsigned int de1D66C[] = { 0x00077, 0x00000 };
static unsigned int de1D66D[] = { 0x00078, 0x00000 };
static unsigned int de1D66E[] = { 0x00079, 0x00000 };
static unsigned int de1D66F[] = { 0x0007A, 0x00000 };
static unsigned int de1D670[] = { 0x00041, 0x00000 };
static unsigned int de1D671[] = { 0x00042, 0x00000 };
static unsigned int de1D672[] = { 0x00043, 0x00000 };
static unsigned int de1D673[] = { 0x00044, 0x00000 };
static unsigned int de1D674[] = { 0x00045, 0x00000 };
static unsigned int de1D675[] = { 0x00046, 0x00000 };
static unsigned int de1D676[] = { 0x00047, 0x00000 };
static unsigned int de1D677[] = { 0x00048, 0x00000 };
static unsigned int de1D678[] = { 0x00049, 0x00000 };
static unsigned int de1D679[] = { 0x0004A, 0x00000 };
static unsigned int de1D67A[] = { 0x0004B, 0x00000 };
static unsigned int de1D67B[] = { 0x0004C, 0x00000 };
static unsigned int de1D67C[] = { 0x0004D, 0x00000 };
static unsigned int de1D67D[] = { 0x0004E, 0x00000 };
static unsigned int de1D67E[] = { 0x0004F, 0x00000 };
static unsigned int de1D67F[] = { 0x00050, 0x00000 };
static unsigned int de1D680[] = { 0x00051, 0x00000 };
static unsigned int de1D681[] = { 0x00052, 0x00000 };
static unsigned int de1D682[] = { 0x00053, 0x00000 };
static unsigned int de1D683[] = { 0x00054, 0x00000 };
static unsigned int de1D684[] = { 0x00055, 0x00000 };
static unsigned int de1D685[] = { 0x00056, 0x00000 };
static unsigned int de1D686[] = { 0x00057, 0x00000 };
static unsigned int de1D687[] = { 0x00058, 0x00000 };
static unsigned int de1D688[] = { 0x00059, 0x00000 };
static unsigned int de1D689[] = { 0x0005A, 0x00000 };
static unsigned int de1D68A[] = { 0x00061, 0x00000 };
static unsigned int de1D68B[] = { 0x00062, 0x00000 };
static unsigned int de1D68C[] = { 0x00063, 0x00000 };
static unsigned int de1D68D[] = { 0x00064, 0x00000 };
static unsigned int de1D68E[] = { 0x00065, 0x00000 };
static unsigned int de1D68F[] = { 0x00066, 0x00000 };
static unsigned int de1D690[] = { 0x00067, 0x00000 };
static unsigned int de1D691[] = { 0x00068, 0x00000 };
static unsigned int de1D692[] = { 0x00069, 0x00000 };
static unsigned int de1D693[] = { 0x0006A, 0x00000 };
static unsigned int de1D694[] = { 0x0006B, 0x00000 };
static unsigned int de1D695[] = { 0x0006C, 0x00000 };
static unsigned int de1D696[] = { 0x0006D, 0x00000 };
static unsigned int de1D697[] = { 0x0006E, 0x00000 };
static unsigned int de1D698[] = { 0x0006F, 0x00000 };
static unsigned int de1D699[] = { 0x00070, 0x00000 };
static unsigned int de1D69A[] = { 0x00071, 0x00000 };
static unsigned int de1D69B[] = { 0x00072, 0x00000 };
static unsigned int de1D69C[] = { 0x00073, 0x00000 };
static unsigned int de1D69D[] = { 0x00074, 0x00000 };
static unsigned int de1D69E[] = { 0x00075, 0x00000 };
static unsigned int de1D69F[] = { 0x00076, 0x00000 };
static unsigned int de1D6A0[] = { 0x00077, 0x00000 };
static unsigned int de1D6A1[] = { 0x00078, 0x00000 };
static unsigned int de1D6A2[] = { 0x00079, 0x00000 };
static unsigned int de1D6A3[] = { 0x0007A, 0x00000 };
static unsigned int de1D6A4[] = { 0x00131, 0x00000 };
static unsigned int de1D6A5[] = { 0x00237, 0x00000 };
static unsigned int de1D6A8[] = { 0x00391, 0x00000 };
static unsigned int de1D6A9[] = { 0x00392, 0x00000 };
static unsigned int de1D6AA[] = { 0x00393, 0x00000 };
static unsigned int de1D6AB[] = { 0x00394, 0x00000 };
static unsigned int de1D6AC[] = { 0x00395, 0x00000 };
static unsigned int de1D6AD[] = { 0x00396, 0x00000 };
static unsigned int de1D6AE[] = { 0x00397, 0x00000 };
static unsigned int de1D6AF[] = { 0x00398, 0x00000 };
static unsigned int de1D6B0[] = { 0x00399, 0x00000 };
static unsigned int de1D6B1[] = { 0x0039A, 0x00000 };
static unsigned int de1D6B2[] = { 0x0039B, 0x00000 };
static unsigned int de1D6B3[] = { 0x0039C, 0x00000 };
static unsigned int de1D6B4[] = { 0x0039D, 0x00000 };
static unsigned int de1D6B5[] = { 0x0039E, 0x00000 };
static unsigned int de1D6B6[] = { 0x0039F, 0x00000 };
static unsigned int de1D6B7[] = { 0x003A0, 0x00000 };
static unsigned int de1D6B8[] = { 0x003A1, 0x00000 };
static unsigned int de1D6B9[] = { 0x003F4, 0x00000 };
static unsigned int de1D6BA[] = { 0x003A3, 0x00000 };
static unsigned int de1D6BB[] = { 0x003A4, 0x00000 };
static unsigned int de1D6BC[] = { 0x003A5, 0x00000 };
static unsigned int de1D6BD[] = { 0x003A6, 0x00000 };
static unsigned int de1D6BE[] = { 0x003A7, 0x00000 };
static unsigned int de1D6BF[] = { 0x003A8, 0x00000 };
static unsigned int de1D6C0[] = { 0x003A9, 0x00000 };
static unsigned int de1D6C1[] = { 0x02207, 0x00000 };
static unsigned int de1D6C2[] = { 0x003B1, 0x00000 };
static unsigned int de1D6C3[] = { 0x003B2, 0x00000 };
static unsigned int de1D6C4[] = { 0x003B3, 0x00000 };
static unsigned int de1D6C5[] = { 0x003B4, 0x00000 };
static unsigned int de1D6C6[] = { 0x003B5, 0x00000 };
static unsigned int de1D6C7[] = { 0x003B6, 0x00000 };
static unsigned int de1D6C8[] = { 0x003B7, 0x00000 };
static unsigned int de1D6C9[] = { 0x003B8, 0x00000 };
static unsigned int de1D6CA[] = { 0x003B9, 0x00000 };
static unsigned int de1D6CB[] = { 0x003BA, 0x00000 };
static unsigned int de1D6CC[] = { 0x003BB, 0x00000 };
static unsigned int de1D6CD[] = { 0x003BC, 0x00000 };
static unsigned int de1D6CE[] = { 0x003BD, 0x00000 };
static unsigned int de1D6CF[] = { 0x003BE, 0x00000 };
static unsigned int de1D6D0[] = { 0x003BF, 0x00000 };
static unsigned int de1D6D1[] = { 0x003C0, 0x00000 };
static unsigned int de1D6D2[] = { 0x003C1, 0x00000 };
static unsigned int de1D6D3[] = { 0x003C2, 0x00000 };
static unsigned int de1D6D4[] = { 0x003C3, 0x00000 };
static unsigned int de1D6D5[] = { 0x003C4, 0x00000 };
static unsigned int de1D6D6[] = { 0x003C5, 0x00000 };
static unsigned int de1D6D7[] = { 0x003C6, 0x00000 };
static unsigned int de1D6D8[] = { 0x003C7, 0x00000 };
static unsigned int de1D6D9[] = { 0x003C8, 0x00000 };
static unsigned int de1D6DA[] = { 0x003C9, 0x00000 };
static unsigned int de1D6DB[] = { 0x02202, 0x00000 };
static unsigned int de1D6DC[] = { 0x003F5, 0x00000 };
static unsigned int de1D6DD[] = { 0x003D1, 0x00000 };
static unsigned int de1D6DE[] = { 0x003F0, 0x00000 };
static unsigned int de1D6DF[] = { 0x003D5, 0x00000 };
static unsigned int de1D6E0[] = { 0x003F1, 0x00000 };
static unsigned int de1D6E1[] = { 0x003D6, 0x00000 };
static unsigned int de1D6E2[] = { 0x00391, 0x00000 };
static unsigned int de1D6E3[] = { 0x00392, 0x00000 };
static unsigned int de1D6E4[] = { 0x00393, 0x00000 };
static unsigned int de1D6E5[] = { 0x00394, 0x00000 };
static unsigned int de1D6E6[] = { 0x00395, 0x00000 };
static unsigned int de1D6E7[] = { 0x00396, 0x00000 };
static unsigned int de1D6E8[] = { 0x00397, 0x00000 };
static unsigned int de1D6E9[] = { 0x00398, 0x00000 };
static unsigned int de1D6EA[] = { 0x00399, 0x00000 };
static unsigned int de1D6EB[] = { 0x0039A, 0x00000 };
static unsigned int de1D6EC[] = { 0x0039B, 0x00000 };
static unsigned int de1D6ED[] = { 0x0039C, 0x00000 };
static unsigned int de1D6EE[] = { 0x0039D, 0x00000 };
static unsigned int de1D6EF[] = { 0x0039E, 0x00000 };
static unsigned int de1D6F0[] = { 0x0039F, 0x00000 };
static unsigned int de1D6F1[] = { 0x003A0, 0x00000 };
static unsigned int de1D6F2[] = { 0x003A1, 0x00000 };
static unsigned int de1D6F3[] = { 0x003F4, 0x00000 };
static unsigned int de1D6F4[] = { 0x003A3, 0x00000 };
static unsigned int de1D6F5[] = { 0x003A4, 0x00000 };
static unsigned int de1D6F6[] = { 0x003A5, 0x00000 };
static unsigned int de1D6F7[] = { 0x003A6, 0x00000 };
static unsigned int de1D6F8[] = { 0x003A7, 0x00000 };
static unsigned int de1D6F9[] = { 0x003A8, 0x00000 };
static unsigned int de1D6FA[] = { 0x003A9, 0x00000 };
static unsigned int de1D6FB[] = { 0x02207, 0x00000 };
static unsigned int de1D6FC[] = { 0x003B1, 0x00000 };
static unsigned int de1D6FD[] = { 0x003B2, 0x00000 };
static unsigned int de1D6FE[] = { 0x003B3, 0x00000 };
static unsigned int de1D6FF[] = { 0x003B4, 0x00000 };
static unsigned int de1D700[] = { 0x003B5, 0x00000 };
static unsigned int de1D701[] = { 0x003B6, 0x00000 };
static unsigned int de1D702[] = { 0x003B7, 0x00000 };
static unsigned int de1D703[] = { 0x003B8, 0x00000 };
static unsigned int de1D704[] = { 0x003B9, 0x00000 };
static unsigned int de1D705[] = { 0x003BA, 0x00000 };
static unsigned int de1D706[] = { 0x003BB, 0x00000 };
static unsigned int de1D707[] = { 0x003BC, 0x00000 };
static unsigned int de1D708[] = { 0x003BD, 0x00000 };
static unsigned int de1D709[] = { 0x003BE, 0x00000 };
static unsigned int de1D70A[] = { 0x003BF, 0x00000 };
static unsigned int de1D70B[] = { 0x003C0, 0x00000 };
static unsigned int de1D70C[] = { 0x003C1, 0x00000 };
static unsigned int de1D70D[] = { 0x003C2, 0x00000 };
static unsigned int de1D70E[] = { 0x003C3, 0x00000 };
static unsigned int de1D70F[] = { 0x003C4, 0x00000 };
static unsigned int de1D710[] = { 0x003C5, 0x00000 };
static unsigned int de1D711[] = { 0x003C6, 0x00000 };
static unsigned int de1D712[] = { 0x003C7, 0x00000 };
static unsigned int de1D713[] = { 0x003C8, 0x00000 };
static unsigned int de1D714[] = { 0x003C9, 0x00000 };
static unsigned int de1D715[] = { 0x02202, 0x00000 };
static unsigned int de1D716[] = { 0x003F5, 0x00000 };
static unsigned int de1D717[] = { 0x003D1, 0x00000 };
static unsigned int de1D718[] = { 0x003F0, 0x00000 };
static unsigned int de1D719[] = { 0x003D5, 0x00000 };
static unsigned int de1D71A[] = { 0x003F1, 0x00000 };
static unsigned int de1D71B[] = { 0x003D6, 0x00000 };
static unsigned int de1D71C[] = { 0x00391, 0x00000 };
static unsigned int de1D71D[] = { 0x00392, 0x00000 };
static unsigned int de1D71E[] = { 0x00393, 0x00000 };
static unsigned int de1D71F[] = { 0x00394, 0x00000 };
static unsigned int de1D720[] = { 0x00395, 0x00000 };
static unsigned int de1D721[] = { 0x00396, 0x00000 };
static unsigned int de1D722[] = { 0x00397, 0x00000 };
static unsigned int de1D723[] = { 0x00398, 0x00000 };
static unsigned int de1D724[] = { 0x00399, 0x00000 };
static unsigned int de1D725[] = { 0x0039A, 0x00000 };
static unsigned int de1D726[] = { 0x0039B, 0x00000 };
static unsigned int de1D727[] = { 0x0039C, 0x00000 };
static unsigned int de1D728[] = { 0x0039D, 0x00000 };
static unsigned int de1D729[] = { 0x0039E, 0x00000 };
static unsigned int de1D72A[] = { 0x0039F, 0x00000 };
static unsigned int de1D72B[] = { 0x003A0, 0x00000 };
static unsigned int de1D72C[] = { 0x003A1, 0x00000 };
static unsigned int de1D72D[] = { 0x003F4, 0x00000 };
static unsigned int de1D72E[] = { 0x003A3, 0x00000 };
static unsigned int de1D72F[] = { 0x003A4, 0x00000 };
static unsigned int de1D730[] = { 0x003A5, 0x00000 };
static unsigned int de1D731[] = { 0x003A6, 0x00000 };
static unsigned int de1D732[] = { 0x003A7, 0x00000 };
static unsigned int de1D733[] = { 0x003A8, 0x00000 };
static unsigned int de1D734[] = { 0x003A9, 0x00000 };
static unsigned int de1D735[] = { 0x02207, 0x00000 };
static unsigned int de1D736[] = { 0x003B1, 0x00000 };
static unsigned int de1D737[] = { 0x003B2, 0x00000 };
static unsigned int de1D738[] = { 0x003B3, 0x00000 };
static unsigned int de1D739[] = { 0x003B4, 0x00000 };
static unsigned int de1D73A[] = { 0x003B5, 0x00000 };
static unsigned int de1D73B[] = { 0x003B6, 0x00000 };
static unsigned int de1D73C[] = { 0x003B7, 0x00000 };
static unsigned int de1D73D[] = { 0x003B8, 0x00000 };
static unsigned int de1D73E[] = { 0x003B9, 0x00000 };
static unsigned int de1D73F[] = { 0x003BA, 0x00000 };
static unsigned int de1D740[] = { 0x003BB, 0x00000 };
static unsigned int de1D741[] = { 0x003BC, 0x00000 };
static unsigned int de1D742[] = { 0x003BD, 0x00000 };
static unsigned int de1D743[] = { 0x003BE, 0x00000 };
static unsigned int de1D744[] = { 0x003BF, 0x00000 };
static unsigned int de1D745[] = { 0x003C0, 0x00000 };
static unsigned int de1D746[] = { 0x003C1, 0x00000 };
static unsigned int de1D747[] = { 0x003C2, 0x00000 };
static unsigned int de1D748[] = { 0x003C3, 0x00000 };
static unsigned int de1D749[] = { 0x003C4, 0x00000 };
static unsigned int de1D74A[] = { 0x003C5, 0x00000 };
static unsigned int de1D74B[] = { 0x003C6, 0x00000 };
static unsigned int de1D74C[] = { 0x003C7, 0x00000 };
static unsigned int de1D74D[] = { 0x003C8, 0x00000 };
static unsigned int de1D74E[] = { 0x003C9, 0x00000 };
static unsigned int de1D74F[] = { 0x02202, 0x00000 };
static unsigned int de1D750[] = { 0x003F5, 0x00000 };
static unsigned int de1D751[] = { 0x003D1, 0x00000 };
static unsigned int de1D752[] = { 0x003F0, 0x00000 };
static unsigned int de1D753[] = { 0x003D5, 0x00000 };
static unsigned int de1D754[] = { 0x003F1, 0x00000 };
static unsigned int de1D755[] = { 0x003D6, 0x00000 };
static unsigned int de1D756[] = { 0x00391, 0x00000 };
static unsigned int de1D757[] = { 0x00392, 0x00000 };
static unsigned int de1D758[] = { 0x00393, 0x00000 };
static unsigned int de1D759[] = { 0x00394, 0x00000 };
static unsigned int de1D75A[] = { 0x00395, 0x00000 };
static unsigned int de1D75B[] = { 0x00396, 0x00000 };
static unsigned int de1D75C[] = { 0x00397, 0x00000 };
static unsigned int de1D75D[] = { 0x00398, 0x00000 };
static unsigned int de1D75E[] = { 0x00399, 0x00000 };
static unsigned int de1D75F[] = { 0x0039A, 0x00000 };
static unsigned int de1D760[] = { 0x0039B, 0x00000 };
static unsigned int de1D761[] = { 0x0039C, 0x00000 };
static unsigned int de1D762[] = { 0x0039D, 0x00000 };
static unsigned int de1D763[] = { 0x0039E, 0x00000 };
static unsigned int de1D764[] = { 0x0039F, 0x00000 };
static unsigned int de1D765[] = { 0x003A0, 0x00000 };
static unsigned int de1D766[] = { 0x003A1, 0x00000 };
static unsigned int de1D767[] = { 0x003F4, 0x00000 };
static unsigned int de1D768[] = { 0x003A3, 0x00000 };
static unsigned int de1D769[] = { 0x003A4, 0x00000 };
static unsigned int de1D76A[] = { 0x003A5, 0x00000 };
static unsigned int de1D76B[] = { 0x003A6, 0x00000 };
static unsigned int de1D76C[] = { 0x003A7, 0x00000 };
static unsigned int de1D76D[] = { 0x003A8, 0x00000 };
static unsigned int de1D76E[] = { 0x003A9, 0x00000 };
static unsigned int de1D76F[] = { 0x02207, 0x00000 };
static unsigned int de1D770[] = { 0x003B1, 0x00000 };
static unsigned int de1D771[] = { 0x003B2, 0x00000 };
static unsigned int de1D772[] = { 0x003B3, 0x00000 };
static unsigned int de1D773[] = { 0x003B4, 0x00000 };
static unsigned int de1D774[] = { 0x003B5, 0x00000 };
static unsigned int de1D775[] = { 0x003B6, 0x00000 };
static unsigned int de1D776[] = { 0x003B7, 0x00000 };
static unsigned int de1D777[] = { 0x003B8, 0x00000 };
static unsigned int de1D778[] = { 0x003B9, 0x00000 };
static unsigned int de1D779[] = { 0x003BA, 0x00000 };
static unsigned int de1D77A[] = { 0x003BB, 0x00000 };
static unsigned int de1D77B[] = { 0x003BC, 0x00000 };
static unsigned int de1D77C[] = { 0x003BD, 0x00000 };
static unsigned int de1D77D[] = { 0x003BE, 0x00000 };
static unsigned int de1D77E[] = { 0x003BF, 0x00000 };
static unsigned int de1D77F[] = { 0x003C0, 0x00000 };
static unsigned int de1D780[] = { 0x003C1, 0x00000 };
static unsigned int de1D781[] = { 0x003C2, 0x00000 };
static unsigned int de1D782[] = { 0x003C3, 0x00000 };
static unsigned int de1D783[] = { 0x003C4, 0x00000 };
static unsigned int de1D784[] = { 0x003C5, 0x00000 };
static unsigned int de1D785[] = { 0x003C6, 0x00000 };
static unsigned int de1D786[] = { 0x003C7, 0x00000 };
static unsigned int de1D787[] = { 0x003C8, 0x00000 };
static unsigned int de1D788[] = { 0x003C9, 0x00000 };
static unsigned int de1D789[] = { 0x02202, 0x00000 };
static unsigned int de1D78A[] = { 0x003F5, 0x00000 };
static unsigned int de1D78B[] = { 0x003D1, 0x00000 };
static unsigned int de1D78C[] = { 0x003F0, 0x00000 };
static unsigned int de1D78D[] = { 0x003D5, 0x00000 };
static unsigned int de1D78E[] = { 0x003F1, 0x00000 };
static unsigned int de1D78F[] = { 0x003D6, 0x00000 };
static unsigned int de1D790[] = { 0x00391, 0x00000 };
static unsigned int de1D791[] = { 0x00392, 0x00000 };
static unsigned int de1D792[] = { 0x00393, 0x00000 };
static unsigned int de1D793[] = { 0x00394, 0x00000 };
static unsigned int de1D794[] = { 0x00395, 0x00000 };
static unsigned int de1D795[] = { 0x00396, 0x00000 };
static unsigned int de1D796[] = { 0x00397, 0x00000 };
static unsigned int de1D797[] = { 0x00398, 0x00000 };
static unsigned int de1D798[] = { 0x00399, 0x00000 };
static unsigned int de1D799[] = { 0x0039A, 0x00000 };
static unsigned int de1D79A[] = { 0x0039B, 0x00000 };
static unsigned int de1D79B[] = { 0x0039C, 0x00000 };
static unsigned int de1D79C[] = { 0x0039D, 0x00000 };
static unsigned int de1D79D[] = { 0x0039E, 0x00000 };
static unsigned int de1D79E[] = { 0x0039F, 0x00000 };
static unsigned int de1D79F[] = { 0x003A0, 0x00000 };
static unsigned int de1D7A0[] = { 0x003A1, 0x00000 };
static unsigned int de1D7A1[] = { 0x003F4, 0x00000 };
static unsigned int de1D7A2[] = { 0x003A3, 0x00000 };
static unsigned int de1D7A3[] = { 0x003A4, 0x00000 };
static unsigned int de1D7A4[] = { 0x003A5, 0x00000 };
static unsigned int de1D7A5[] = { 0x003A6, 0x00000 };
static unsigned int de1D7A6[] = { 0x003A7, 0x00000 };
static unsigned int de1D7A7[] = { 0x003A8, 0x00000 };
static unsigned int de1D7A8[] = { 0x003A9, 0x00000 };
static unsigned int de1D7A9[] = { 0x02207, 0x00000 };
static unsigned int de1D7AA[] = { 0x003B1, 0x00000 };
static unsigned int de1D7AB[] = { 0x003B2, 0x00000 };
static unsigned int de1D7AC[] = { 0x003B3, 0x00000 };
static unsigned int de1D7AD[] = { 0x003B4, 0x00000 };
static unsigned int de1D7AE[] = { 0x003B5, 0x00000 };
static unsigned int de1D7AF[] = { 0x003B6, 0x00000 };
static unsigned int de1D7B0[] = { 0x003B7, 0x00000 };
static unsigned int de1D7B1[] = { 0x003B8, 0x00000 };
static unsigned int de1D7B2[] = { 0x003B9, 0x00000 };
static unsigned int de1D7B3[] = { 0x003BA, 0x00000 };
static unsigned int de1D7B4[] = { 0x003BB, 0x00000 };
static unsigned int de1D7B5[] = { 0x003BC, 0x00000 };
static unsigned int de1D7B6[] = { 0x003BD, 0x00000 };
static unsigned int de1D7B7[] = { 0x003BE, 0x00000 };
static unsigned int de1D7B8[] = { 0x003BF, 0x00000 };
static unsigned int de1D7B9[] = { 0x003C0, 0x00000 };
static unsigned int de1D7BA[] = { 0x003C1, 0x00000 };
static unsigned int de1D7BB[] = { 0x003C2, 0x00000 };
static unsigned int de1D7BC[] = { 0x003C3, 0x00000 };
static unsigned int de1D7BD[] = { 0x003C4, 0x00000 };
static unsigned int de1D7BE[] = { 0x003C5, 0x00000 };
static unsigned int de1D7BF[] = { 0x003C6, 0x00000 };
static unsigned int de1D7C0[] = { 0x003C7, 0x00000 };
static unsigned int de1D7C1[] = { 0x003C8, 0x00000 };
static unsigned int de1D7C2[] = { 0x003C9, 0x00000 };
static unsigned int de1D7C3[] = { 0x02202, 0x00000 };
static unsigned int de1D7C4[] = { 0x003F5, 0x00000 };
static unsigned int de1D7C5[] = { 0x003D1, 0x00000 };
static unsigned int de1D7C6[] = { 0x003F0, 0x00000 };
static unsigned int de1D7C7[] = { 0x003D5, 0x00000 };
static unsigned int de1D7C8[] = { 0x003F1, 0x00000 };
static unsigned int de1D7C9[] = { 0x003D6, 0x00000 };
static unsigned int de1D7CE[] = { 0x00030, 0x00000 };
static unsigned int de1D7CF[] = { 0x00031, 0x00000 };
static unsigned int de1D7D0[] = { 0x00032, 0x00000 };
static unsigned int de1D7D1[] = { 0x00033, 0x00000 };
static unsigned int de1D7D2[] = { 0x00034, 0x00000 };
static unsigned int de1D7D3[] = { 0x00035, 0x00000 };
static unsigned int de1D7D4[] = { 0x00036, 0x00000 };
static unsigned int de1D7D5[] = { 0x00037, 0x00000 };
static unsigned int de1D7D6[] = { 0x00038, 0x00000 };
static unsigned int de1D7D7[] = { 0x00039, 0x00000 };
static unsigned int de1D7D8[] = { 0x00030, 0x00000 };
static unsigned int de1D7D9[] = { 0x00031, 0x00000 };
static unsigned int de1D7DA[] = { 0x00032, 0x00000 };
static unsigned int de1D7DB[] = { 0x00033, 0x00000 };
static unsigned int de1D7DC[] = { 0x00034, 0x00000 };
static unsigned int de1D7DD[] = { 0x00035, 0x00000 };
static unsigned int de1D7DE[] = { 0x00036, 0x00000 };
static unsigned int de1D7DF[] = { 0x00037, 0x00000 };
static unsigned int de1D7E0[] = { 0x00038, 0x00000 };
static unsigned int de1D7E1[] = { 0x00039, 0x00000 };
static unsigned int de1D7E2[] = { 0x00030, 0x00000 };
static unsigned int de1D7E3[] = { 0x00031, 0x00000 };
static unsigned int de1D7E4[] = { 0x00032, 0x00000 };
static unsigned int de1D7E5[] = { 0x00033, 0x00000 };
static unsigned int de1D7E6[] = { 0x00034, 0x00000 };
static unsigned int de1D7E7[] = { 0x00035, 0x00000 };
static unsigned int de1D7E8[] = { 0x00036, 0x00000 };
static unsigned int de1D7E9[] = { 0x00037, 0x00000 };
static unsigned int de1D7EA[] = { 0x00038, 0x00000 };
static unsigned int de1D7EB[] = { 0x00039, 0x00000 };
static unsigned int de1D7EC[] = { 0x00030, 0x00000 };
static unsigned int de1D7ED[] = { 0x00031, 0x00000 };
static unsigned int de1D7EE[] = { 0x00032, 0x00000 };
static unsigned int de1D7EF[] = { 0x00033, 0x00000 };
static unsigned int de1D7F0[] = { 0x00034, 0x00000 };
static unsigned int de1D7F1[] = { 0x00035, 0x00000 };
static unsigned int de1D7F2[] = { 0x00036, 0x00000 };
static unsigned int de1D7F3[] = { 0x00037, 0x00000 };
static unsigned int de1D7F4[] = { 0x00038, 0x00000 };
static unsigned int de1D7F5[] = { 0x00039, 0x00000 };
static unsigned int de1D7F6[] = { 0x00030, 0x00000 };
static unsigned int de1D7F7[] = { 0x00031, 0x00000 };
static unsigned int de1D7F8[] = { 0x00032, 0x00000 };
static unsigned int de1D7F9[] = { 0x00033, 0x00000 };
static unsigned int de1D7FA[] = { 0x00034, 0x00000 };
static unsigned int de1D7FB[] = { 0x00035, 0x00000 };
static unsigned int de1D7FC[] = { 0x00036, 0x00000 };
static unsigned int de1D7FD[] = { 0x00037, 0x00000 };
static unsigned int de1D7FE[] = { 0x00038, 0x00000 };
static unsigned int de1D7FF[] = { 0x00039, 0x00000 };
static unsigned int de2F800[] = { 0x04E3D, 0x00000 };
static unsigned int de2F801[] = { 0x04E38, 0x00000 };
static unsigned int de2F802[] = { 0x04E41, 0x00000 };
static unsigned int de2F803[] = { 0x20122, 0x00000 };
static unsigned int de2F804[] = { 0x04F60, 0x00000 };
static unsigned int de2F805[] = { 0x04FAE, 0x00000 };
static unsigned int de2F806[] = { 0x04FBB, 0x00000 };
static unsigned int de2F807[] = { 0x05002, 0x00000 };
static unsigned int de2F808[] = { 0x0507A, 0x00000 };
static unsigned int de2F809[] = { 0x05099, 0x00000 };
static unsigned int de2F80A[] = { 0x050E7, 0x00000 };
static unsigned int de2F80B[] = { 0x050CF, 0x00000 };
static unsigned int de2F80C[] = { 0x0349E, 0x00000 };
static unsigned int de2F80D[] = { 0x2063A, 0x00000 };
static unsigned int de2F80E[] = { 0x0514D, 0x00000 };
static unsigned int de2F80F[] = { 0x05154, 0x00000 };
static unsigned int de2F810[] = { 0x05164, 0x00000 };
static unsigned int de2F811[] = { 0x05177, 0x00000 };
static unsigned int de2F812[] = { 0x2051C, 0x00000 };
static unsigned int de2F813[] = { 0x034B9, 0x00000 };
static unsigned int de2F814[] = { 0x05167, 0x00000 };
static unsigned int de2F815[] = { 0x0518D, 0x00000 };
static unsigned int de2F816[] = { 0x2054B, 0x00000 };
static unsigned int de2F817[] = { 0x05197, 0x00000 };
static unsigned int de2F818[] = { 0x051A4, 0x00000 };
static unsigned int de2F819[] = { 0x04ECC, 0x00000 };
static unsigned int de2F81A[] = { 0x051AC, 0x00000 };
static unsigned int de2F81B[] = { 0x051B5, 0x00000 };
static unsigned int de2F81C[] = { 0x291DF, 0x00000 };
static unsigned int de2F81D[] = { 0x051F5, 0x00000 };
static unsigned int de2F81E[] = { 0x05203, 0x00000 };
static unsigned int de2F81F[] = { 0x034DF, 0x00000 };
static unsigned int de2F820[] = { 0x0523B, 0x00000 };
static unsigned int de2F821[] = { 0x05246, 0x00000 };
static unsigned int de2F822[] = { 0x05272, 0x00000 };
static unsigned int de2F823[] = { 0x05277, 0x00000 };
static unsigned int de2F824[] = { 0x03515, 0x00000 };
static unsigned int de2F825[] = { 0x052C7, 0x00000 };
static unsigned int de2F826[] = { 0x052C9, 0x00000 };
static unsigned int de2F827[] = { 0x052E4, 0x00000 };
static unsigned int de2F828[] = { 0x052FA, 0x00000 };
static unsigned int de2F829[] = { 0x05305, 0x00000 };
static unsigned int de2F82A[] = { 0x05306, 0x00000 };
static unsigned int de2F82B[] = { 0x05317, 0x00000 };
static unsigned int de2F82C[] = { 0x05349, 0x00000 };
static unsigned int de2F82D[] = { 0x05351, 0x00000 };
static unsigned int de2F82E[] = { 0x0535A, 0x00000 };
static unsigned int de2F82F[] = { 0x05373, 0x00000 };
static unsigned int de2F830[] = { 0x0537D, 0x00000 };
static unsigned int de2F831[] = { 0x0537F, 0x00000 };
static unsigned int de2F832[] = { 0x0537F, 0x00000 };
static unsigned int de2F833[] = { 0x0537F, 0x00000 };
static unsigned int de2F834[] = { 0x20A2C, 0x00000 };
static unsigned int de2F835[] = { 0x07070, 0x00000 };
static unsigned int de2F836[] = { 0x053CA, 0x00000 };
static unsigned int de2F837[] = { 0x053DF, 0x00000 };
static unsigned int de2F838[] = { 0x20B63, 0x00000 };
static unsigned int de2F839[] = { 0x053EB, 0x00000 };
static unsigned int de2F83A[] = { 0x053F1, 0x00000 };
static unsigned int de2F83B[] = { 0x05406, 0x00000 };
static unsigned int de2F83C[] = { 0x0549E, 0x00000 };
static unsigned int de2F83D[] = { 0x05438, 0x00000 };
static unsigned int de2F83E[] = { 0x05448, 0x00000 };
static unsigned int de2F83F[] = { 0x05468, 0x00000 };
static unsigned int de2F840[] = { 0x054A2, 0x00000 };
static unsigned int de2F841[] = { 0x054F6, 0x00000 };
static unsigned int de2F842[] = { 0x05510, 0x00000 };
static unsigned int de2F843[] = { 0x05553, 0x00000 };
static unsigned int de2F844[] = { 0x05563, 0x00000 };
static unsigned int de2F845[] = { 0x05584, 0x00000 };
static unsigned int de2F846[] = { 0x05584, 0x00000 };
static unsigned int de2F847[] = { 0x05599, 0x00000 };
static unsigned int de2F848[] = { 0x055AB, 0x00000 };
static unsigned int de2F849[] = { 0x055B3, 0x00000 };
static unsigned int de2F84A[] = { 0x055C2, 0x00000 };
static unsigned int de2F84B[] = { 0x05716, 0x00000 };
static unsigned int de2F84C[] = { 0x05606, 0x00000 };
static unsigned int de2F84D[] = { 0x05717, 0x00000 };
static unsigned int de2F84E[] = { 0x05651, 0x00000 };
static unsigned int de2F84F[] = { 0x05674, 0x00000 };
static unsigned int de2F850[] = { 0x05207, 0x00000 };
static unsigned int de2F851[] = { 0x058EE, 0x00000 };
static unsigned int de2F852[] = { 0x057CE, 0x00000 };
static unsigned int de2F853[] = { 0x057F4, 0x00000 };
static unsigned int de2F854[] = { 0x0580D, 0x00000 };
static unsigned int de2F855[] = { 0x0578B, 0x00000 };
static unsigned int de2F856[] = { 0x05832, 0x00000 };
static unsigned int de2F857[] = { 0x05831, 0x00000 };
static unsigned int de2F858[] = { 0x058AC, 0x00000 };
static unsigned int de2F859[] = { 0x214E4, 0x00000 };
static unsigned int de2F85A[] = { 0x058F2, 0x00000 };
static unsigned int de2F85B[] = { 0x058F7, 0x00000 };
static unsigned int de2F85C[] = { 0x05906, 0x00000 };
static unsigned int de2F85D[] = { 0x0591A, 0x00000 };
static unsigned int de2F85E[] = { 0x05922, 0x00000 };
static unsigned int de2F85F[] = { 0x05962, 0x00000 };
static unsigned int de2F860[] = { 0x216A8, 0x00000 };
static unsigned int de2F861[] = { 0x216EA, 0x00000 };
static unsigned int de2F862[] = { 0x059EC, 0x00000 };
static unsigned int de2F863[] = { 0x05A1B, 0x00000 };
static unsigned int de2F864[] = { 0x05A27, 0x00000 };
static unsigned int de2F865[] = { 0x059D8, 0x00000 };
static unsigned int de2F866[] = { 0x05A66, 0x00000 };
static unsigned int de2F867[] = { 0x036EE, 0x00000 };
static unsigned int de2F868[] = { 0x036FC, 0x00000 };
static unsigned int de2F869[] = { 0x05B08, 0x00000 };
static unsigned int de2F86A[] = { 0x05B3E, 0x00000 };
static unsigned int de2F86B[] = { 0x05B3E, 0x00000 };
static unsigned int de2F86C[] = { 0x219C8, 0x00000 };
static unsigned int de2F86D[] = { 0x05BC3, 0x00000 };
static unsigned int de2F86E[] = { 0x05BD8, 0x00000 };
static unsigned int de2F86F[] = { 0x05BE7, 0x00000 };
static unsigned int de2F870[] = { 0x05BF3, 0x00000 };
static unsigned int de2F871[] = { 0x21B18, 0x00000 };
static unsigned int de2F872[] = { 0x05BFF, 0x00000 };
static unsigned int de2F873[] = { 0x05C06, 0x00000 };
static unsigned int de2F874[] = { 0x05F53, 0x00000 };
static unsigned int de2F875[] = { 0x05C22, 0x00000 };
static unsigned int de2F876[] = { 0x03781, 0x00000 };
static unsigned int de2F877[] = { 0x05C60, 0x00000 };
static unsigned int de2F878[] = { 0x05C6E, 0x00000 };
static unsigned int de2F879[] = { 0x05CC0, 0x00000 };
static unsigned int de2F87A[] = { 0x05C8D, 0x00000 };
static unsigned int de2F87B[] = { 0x21DE4, 0x00000 };
static unsigned int de2F87C[] = { 0x05D43, 0x00000 };
static unsigned int de2F87D[] = { 0x21DE6, 0x00000 };
static unsigned int de2F87E[] = { 0x05D6E, 0x00000 };
static unsigned int de2F87F[] = { 0x05D6B, 0x00000 };
static unsigned int de2F880[] = { 0x05D7C, 0x00000 };
static unsigned int de2F881[] = { 0x05DE1, 0x00000 };
static unsigned int de2F882[] = { 0x05DE2, 0x00000 };
static unsigned int de2F883[] = { 0x0382F, 0x00000 };
static unsigned int de2F884[] = { 0x05DFD, 0x00000 };
static unsigned int de2F885[] = { 0x05E28, 0x00000 };
static unsigned int de2F886[] = { 0x05E3D, 0x00000 };
static unsigned int de2F887[] = { 0x05E69, 0x00000 };
static unsigned int de2F888[] = { 0x03862, 0x00000 };
static unsigned int de2F889[] = { 0x22183, 0x00000 };
static unsigned int de2F88A[] = { 0x0387C, 0x00000 };
static unsigned int de2F88B[] = { 0x05EB0, 0x00000 };
static unsigned int de2F88C[] = { 0x05EB3, 0x00000 };
static unsigned int de2F88D[] = { 0x05EB6, 0x00000 };
static unsigned int de2F88E[] = { 0x05ECA, 0x00000 };
static unsigned int de2F88F[] = { 0x2A392, 0x00000 };
static unsigned int de2F890[] = { 0x05EFE, 0x00000 };
static unsigned int de2F891[] = { 0x22331, 0x00000 };
static unsigned int de2F892[] = { 0x22331, 0x00000 };
static unsigned int de2F893[] = { 0x08201, 0x00000 };
static unsigned int de2F894[] = { 0x05F22, 0x00000 };
static unsigned int de2F895[] = { 0x05F22, 0x00000 };
static unsigned int de2F896[] = { 0x038C7, 0x00000 };
static unsigned int de2F897[] = { 0x232B8, 0x00000 };
static unsigned int de2F898[] = { 0x261DA, 0x00000 };
static unsigned int de2F899[] = { 0x05F62, 0x00000 };
static unsigned int de2F89A[] = { 0x05F6B, 0x00000 };
static unsigned int de2F89B[] = { 0x038E3, 0x00000 };
static unsigned int de2F89C[] = { 0x05F9A, 0x00000 };
static unsigned int de2F89D[] = { 0x05FCD, 0x00000 };
static unsigned int de2F89E[] = { 0x05FD7, 0x00000 };
static unsigned int de2F89F[] = { 0x05FF9, 0x00000 };
static unsigned int de2F8A0[] = { 0x06081, 0x00000 };
static unsigned int de2F8A1[] = { 0x0393A, 0x00000 };
static unsigned int de2F8A2[] = { 0x0391C, 0x00000 };
static unsigned int de2F8A3[] = { 0x06094, 0x00000 };
static unsigned int de2F8A4[] = { 0x226D4, 0x00000 };
static unsigned int de2F8A5[] = { 0x060C7, 0x00000 };
static unsigned int de2F8A6[] = { 0x06148, 0x00000 };
static unsigned int de2F8A7[] = { 0x0614C, 0x00000 };
static unsigned int de2F8A8[] = { 0x0614E, 0x00000 };
static unsigned int de2F8A9[] = { 0x0614C, 0x00000 };
static unsigned int de2F8AA[] = { 0x0617A, 0x00000 };
static unsigned int de2F8AB[] = { 0x0618E, 0x00000 };
static unsigned int de2F8AC[] = { 0x061B2, 0x00000 };
static unsigned int de2F8AD[] = { 0x061A4, 0x00000 };
static unsigned int de2F8AE[] = { 0x061AF, 0x00000 };
static unsigned int de2F8AF[] = { 0x061DE, 0x00000 };
static unsigned int de2F8B0[] = { 0x061F2, 0x00000 };
static unsigned int de2F8B1[] = { 0x061F6, 0x00000 };
static unsigned int de2F8B2[] = { 0x06210, 0x00000 };
static unsigned int de2F8B3[] = { 0x0621B, 0x00000 };
static unsigned int de2F8B4[] = { 0x0625D, 0x00000 };
static unsigned int de2F8B5[] = { 0x062B1, 0x00000 };
static unsigned int de2F8B6[] = { 0x062D4, 0x00000 };
static unsigned int de2F8B7[] = { 0x06350, 0x00000 };
static unsigned int de2F8B8[] = { 0x22B0C, 0x00000 };
static unsigned int de2F8B9[] = { 0x0633D, 0x00000 };
static unsigned int de2F8BA[] = { 0x062FC, 0x00000 };
static unsigned int de2F8BB[] = { 0x06368, 0x00000 };
static unsigned int de2F8BC[] = { 0x06383, 0x00000 };
static unsigned int de2F8BD[] = { 0x063E4, 0x00000 };
static unsigned int de2F8BE[] = { 0x22BF1, 0x00000 };
static unsigned int de2F8BF[] = { 0x06422, 0x00000 };
static unsigned int de2F8C0[] = { 0x063C5, 0x00000 };
static unsigned int de2F8C1[] = { 0x063A9, 0x00000 };
static unsigned int de2F8C2[] = { 0x03A2E, 0x00000 };
static unsigned int de2F8C3[] = { 0x06469, 0x00000 };
static unsigned int de2F8C4[] = { 0x0647E, 0x00000 };
static unsigned int de2F8C5[] = { 0x0649D, 0x00000 };
static unsigned int de2F8C6[] = { 0x06477, 0x00000 };
static unsigned int de2F8C7[] = { 0x03A6C, 0x00000 };
static unsigned int de2F8C8[] = { 0x0654F, 0x00000 };
static unsigned int de2F8C9[] = { 0x0656C, 0x00000 };
static unsigned int de2F8CA[] = { 0x2300A, 0x00000 };
static unsigned int de2F8CB[] = { 0x065E3, 0x00000 };
static unsigned int de2F8CC[] = { 0x066F8, 0x00000 };
static unsigned int de2F8CD[] = { 0x06649, 0x00000 };
static unsigned int de2F8CE[] = { 0x03B19, 0x00000 };
static unsigned int de2F8CF[] = { 0x06691, 0x00000 };
static unsigned int de2F8D0[] = { 0x03B08, 0x00000 };
static unsigned int de2F8D1[] = { 0x03AE4, 0x00000 };
static unsigned int de2F8D2[] = { 0x05192, 0x00000 };
static unsigned int de2F8D3[] = { 0x05195, 0x00000 };
static unsigned int de2F8D4[] = { 0x06700, 0x00000 };
static unsigned int de2F8D5[] = { 0x0669C, 0x00000 };
static unsigned int de2F8D6[] = { 0x080AD, 0x00000 };
static unsigned int de2F8D7[] = { 0x043D9, 0x00000 };
static unsigned int de2F8D8[] = { 0x06717, 0x00000 };
static unsigned int de2F8D9[] = { 0x0671B, 0x00000 };
static unsigned int de2F8DA[] = { 0x06721, 0x00000 };
static unsigned int de2F8DB[] = { 0x0675E, 0x00000 };
static unsigned int de2F8DC[] = { 0x06753, 0x00000 };
static unsigned int de2F8DD[] = { 0x233C3, 0x00000 };
static unsigned int de2F8DE[] = { 0x03B49, 0x00000 };
static unsigned int de2F8DF[] = { 0x067FA, 0x00000 };
static unsigned int de2F8E0[] = { 0x06785, 0x00000 };
static unsigned int de2F8E1[] = { 0x06852, 0x00000 };
static unsigned int de2F8E2[] = { 0x06885, 0x00000 };
static unsigned int de2F8E3[] = { 0x2346D, 0x00000 };
static unsigned int de2F8E4[] = { 0x0688E, 0x00000 };
static unsigned int de2F8E5[] = { 0x0681F, 0x00000 };
static unsigned int de2F8E6[] = { 0x06914, 0x00000 };
static unsigned int de2F8E7[] = { 0x03B9D, 0x00000 };
static unsigned int de2F8E8[] = { 0x06942, 0x00000 };
static unsigned int de2F8E9[] = { 0x069A3, 0x00000 };
static unsigned int de2F8EA[] = { 0x069EA, 0x00000 };
static unsigned int de2F8EB[] = { 0x06AA8, 0x00000 };
static unsigned int de2F8EC[] = { 0x236A3, 0x00000 };
static unsigned int de2F8ED[] = { 0x06ADB, 0x00000 };
static unsigned int de2F8EE[] = { 0x03C18, 0x00000 };
static unsigned int de2F8EF[] = { 0x06B21, 0x00000 };
static unsigned int de2F8F0[] = { 0x238A7, 0x00000 };
static unsigned int de2F8F1[] = { 0x06B54, 0x00000 };
static unsigned int de2F8F2[] = { 0x03C4E, 0x00000 };
static unsigned int de2F8F3[] = { 0x06B72, 0x00000 };
static unsigned int de2F8F4[] = { 0x06B9F, 0x00000 };
static unsigned int de2F8F5[] = { 0x06BBA, 0x00000 };
static unsigned int de2F8F6[] = { 0x06BBB, 0x00000 };
static unsigned int de2F8F7[] = { 0x23A8D, 0x00000 };
static unsigned int de2F8F8[] = { 0x21D0B, 0x00000 };
static unsigned int de2F8F9[] = { 0x23AFA, 0x00000 };
static unsigned int de2F8FA[] = { 0x06C4E, 0x00000 };
static unsigned int de2F8FB[] = { 0x23CBC, 0x00000 };
static unsigned int de2F8FC[] = { 0x06CBF, 0x00000 };
static unsigned int de2F8FD[] = { 0x06CCD, 0x00000 };
static unsigned int de2F8FE[] = { 0x06C67, 0x00000 };
static unsigned int de2F8FF[] = { 0x06D16, 0x00000 };
static unsigned int de2F900[] = { 0x06D3E, 0x00000 };
static unsigned int de2F901[] = { 0x06D77, 0x00000 };
static unsigned int de2F902[] = { 0x06D41, 0x00000 };
static unsigned int de2F903[] = { 0x06D69, 0x00000 };
static unsigned int de2F904[] = { 0x06D78, 0x00000 };
static unsigned int de2F905[] = { 0x06D85, 0x00000 };
static unsigned int de2F906[] = { 0x23D1E, 0x00000 };
static unsigned int de2F907[] = { 0x06D34, 0x00000 };
static unsigned int de2F908[] = { 0x06E2F, 0x00000 };
static unsigned int de2F909[] = { 0x06E6E, 0x00000 };
static unsigned int de2F90A[] = { 0x03D33, 0x00000 };
static unsigned int de2F90B[] = { 0x06ECB, 0x00000 };
static unsigned int de2F90C[] = { 0x06EC7, 0x00000 };
static unsigned int de2F90D[] = { 0x23ED1, 0x00000 };
static unsigned int de2F90E[] = { 0x06DF9, 0x00000 };
static unsigned int de2F90F[] = { 0x06F6E, 0x00000 };
static unsigned int de2F910[] = { 0x23F5E, 0x00000 };
static unsigned int de2F911[] = { 0x23F8E, 0x00000 };
static unsigned int de2F912[] = { 0x06FC6, 0x00000 };
static unsigned int de2F913[] = { 0x07039, 0x00000 };
static unsigned int de2F914[] = { 0x0701E, 0x00000 };
static unsigned int de2F915[] = { 0x0701B, 0x00000 };
static unsigned int de2F916[] = { 0x03D96, 0x00000 };
static unsigned int de2F917[] = { 0x0704A, 0x00000 };
static unsigned int de2F918[] = { 0x0707D, 0x00000 };
static unsigned int de2F919[] = { 0x07077, 0x00000 };
static unsigned int de2F91A[] = { 0x070AD, 0x00000 };
static unsigned int de2F91B[] = { 0x20525, 0x00000 };
static unsigned int de2F91C[] = { 0x07145, 0x00000 };
static unsigned int de2F91D[] = { 0x24263, 0x00000 };
static unsigned int de2F91E[] = { 0x0719C, 0x00000 };
static unsigned int de2F91F[] = { 0x243AB, 0x00000 };
static unsigned int de2F920[] = { 0x07228, 0x00000 };
static unsigned int de2F921[] = { 0x07235, 0x00000 };
static unsigned int de2F922[] = { 0x07250, 0x00000 };
static unsigned int de2F923[] = { 0x24608, 0x00000 };
static unsigned int de2F924[] = { 0x07280, 0x00000 };
static unsigned int de2F925[] = { 0x07295, 0x00000 };
static unsigned int de2F926[] = { 0x24735, 0x00000 };
static unsigned int de2F927[] = { 0x24814, 0x00000 };
static unsigned int de2F928[] = { 0x0737A, 0x00000 };
static unsigned int de2F929[] = { 0x0738B, 0x00000 };
static unsigned int de2F92A[] = { 0x03EAC, 0x00000 };
static unsigned int de2F92B[] = { 0x073A5, 0x00000 };
static unsigned int de2F92C[] = { 0x03EB8, 0x00000 };
static unsigned int de2F92D[] = { 0x03EB8, 0x00000 };
static unsigned int de2F92E[] = { 0x07447, 0x00000 };
static unsigned int de2F92F[] = { 0x0745C, 0x00000 };
static unsigned int de2F930[] = { 0x07471, 0x00000 };
static unsigned int de2F931[] = { 0x07485, 0x00000 };
static unsigned int de2F932[] = { 0x074CA, 0x00000 };
static unsigned int de2F933[] = { 0x03F1B, 0x00000 };
static unsigned int de2F934[] = { 0x07524, 0x00000 };
static unsigned int de2F935[] = { 0x24C36, 0x00000 };
static unsigned int de2F936[] = { 0x0753E, 0x00000 };
static unsigned int de2F937[] = { 0x24C92, 0x00000 };
static unsigned int de2F938[] = { 0x07570, 0x00000 };
static unsigned int de2F939[] = { 0x2219F, 0x00000 };
static unsigned int de2F93A[] = { 0x07610, 0x00000 };
static unsigned int de2F93B[] = { 0x24FA1, 0x00000 };
static unsigned int de2F93C[] = { 0x24FB8, 0x00000 };
static unsigned int de2F93D[] = { 0x25044, 0x00000 };
static unsigned int de2F93E[] = { 0x03FFC, 0x00000 };
static unsigned int de2F93F[] = { 0x04008, 0x00000 };
static unsigned int de2F940[] = { 0x076F4, 0x00000 };
static unsigned int de2F941[] = { 0x250F3, 0x00000 };
static unsigned int de2F942[] = { 0x250F2, 0x00000 };
static unsigned int de2F943[] = { 0x25119, 0x00000 };
static unsigned int de2F944[] = { 0x25133, 0x00000 };
static unsigned int de2F945[] = { 0x0771E, 0x00000 };
static unsigned int de2F946[] = { 0x0771F, 0x00000 };
static unsigned int de2F947[] = { 0x0771F, 0x00000 };
static unsigned int de2F948[] = { 0x0774A, 0x00000 };
static unsigned int de2F949[] = { 0x04039, 0x00000 };
static unsigned int de2F94A[] = { 0x0778B, 0x00000 };
static unsigned int de2F94B[] = { 0x04046, 0x00000 };
static unsigned int de2F94C[] = { 0x04096, 0x00000 };
static unsigned int de2F94D[] = { 0x2541D, 0x00000 };
static unsigned int de2F94E[] = { 0x0784E, 0x00000 };
static unsigned int de2F94F[] = { 0x0788C, 0x00000 };
static unsigned int de2F950[] = { 0x078CC, 0x00000 };
static unsigned int de2F951[] = { 0x040E3, 0x00000 };
static unsigned int de2F952[] = { 0x25626, 0x00000 };
static unsigned int de2F953[] = { 0x07956, 0x00000 };
static unsigned int de2F954[] = { 0x2569A, 0x00000 };
static unsigned int de2F955[] = { 0x256C5, 0x00000 };
static unsigned int de2F956[] = { 0x0798F, 0x00000 };
static unsigned int de2F957[] = { 0x079EB, 0x00000 };
static unsigned int de2F958[] = { 0x0412F, 0x00000 };
static unsigned int de2F959[] = { 0x07A40, 0x00000 };
static unsigned int de2F95A[] = { 0x07A4A, 0x00000 };
static unsigned int de2F95B[] = { 0x07A4F, 0x00000 };
static unsigned int de2F95C[] = { 0x2597C, 0x00000 };
static unsigned int de2F95D[] = { 0x25AA7, 0x00000 };
static unsigned int de2F95E[] = { 0x25AA7, 0x00000 };
static unsigned int de2F95F[] = { 0x07AEE, 0x00000 };
static unsigned int de2F960[] = { 0x04202, 0x00000 };
static unsigned int de2F961[] = { 0x25BAB, 0x00000 };
static unsigned int de2F962[] = { 0x07BC6, 0x00000 };
static unsigned int de2F963[] = { 0x07BC9, 0x00000 };
static unsigned int de2F964[] = { 0x04227, 0x00000 };
static unsigned int de2F965[] = { 0x25C80, 0x00000 };
static unsigned int de2F966[] = { 0x07CD2, 0x00000 };
static unsigned int de2F967[] = { 0x042A0, 0x00000 };
static unsigned int de2F968[] = { 0x07CE8, 0x00000 };
static unsigned int de2F969[] = { 0x07CE3, 0x00000 };
static unsigned int de2F96A[] = { 0x07D00, 0x00000 };
static unsigned int de2F96B[] = { 0x25F86, 0x00000 };
static unsigned int de2F96C[] = { 0x07D63, 0x00000 };
static unsigned int de2F96D[] = { 0x04301, 0x00000 };
static unsigned int de2F96E[] = { 0x07DC7, 0x00000 };
static unsigned int de2F96F[] = { 0x07E02, 0x00000 };
static unsigned int de2F970[] = { 0x07E45, 0x00000 };
static unsigned int de2F971[] = { 0x04334, 0x00000 };
static unsigned int de2F972[] = { 0x26228, 0x00000 };
static unsigned int de2F973[] = { 0x26247, 0x00000 };
static unsigned int de2F974[] = { 0x04359, 0x00000 };
static unsigned int de2F975[] = { 0x262D9, 0x00000 };
static unsigned int de2F976[] = { 0x07F7A, 0x00000 };
static unsigned int de2F977[] = { 0x2633E, 0x00000 };
static unsigned int de2F978[] = { 0x07F95, 0x00000 };
static unsigned int de2F979[] = { 0x07FFA, 0x00000 };
static unsigned int de2F97A[] = { 0x08005, 0x00000 };
static unsigned int de2F97B[] = { 0x264DA, 0x00000 };
static unsigned int de2F97C[] = { 0x26523, 0x00000 };
static unsigned int de2F97D[] = { 0x08060, 0x00000 };
static unsigned int de2F97E[] = { 0x265A8, 0x00000 };
static unsigned int de2F97F[] = { 0x08070, 0x00000 };
static unsigned int de2F980[] = { 0x2335F, 0x00000 };
static unsigned int de2F981[] = { 0x043D5, 0x00000 };
static unsigned int de2F982[] = { 0x080B2, 0x00000 };
static unsigned int de2F983[] = { 0x08103, 0x00000 };
static unsigned int de2F984[] = { 0x0440B, 0x00000 };
static unsigned int de2F985[] = { 0x0813E, 0x00000 };
static unsigned int de2F986[] = { 0x05AB5, 0x00000 };
static unsigned int de2F987[] = { 0x267A7, 0x00000 };
static unsigned int de2F988[] = { 0x267B5, 0x00000 };
static unsigned int de2F989[] = { 0x23393, 0x00000 };
static unsigned int de2F98A[] = { 0x2339C, 0x00000 };
static unsigned int de2F98B[] = { 0x08201, 0x00000 };
static unsigned int de2F98C[] = { 0x08204, 0x00000 };
static unsigned int de2F98D[] = { 0x08F9E, 0x00000 };
static unsigned int de2F98E[] = { 0x0446B, 0x00000 };
static unsigned int de2F98F[] = { 0x08291, 0x00000 };
static unsigned int de2F990[] = { 0x0828B, 0x00000 };
static unsigned int de2F991[] = { 0x0829D, 0x00000 };
static unsigned int de2F992[] = { 0x052B3, 0x00000 };
static unsigned int de2F993[] = { 0x082B1, 0x00000 };
static unsigned int de2F994[] = { 0x082B3, 0x00000 };
static unsigned int de2F995[] = { 0x082BD, 0x00000 };
static unsigned int de2F996[] = { 0x082E6, 0x00000 };
static unsigned int de2F997[] = { 0x26B3C, 0x00000 };
static unsigned int de2F998[] = { 0x082E5, 0x00000 };
static unsigned int de2F999[] = { 0x0831D, 0x00000 };
static unsigned int de2F99A[] = { 0x08363, 0x00000 };
static unsigned int de2F99B[] = { 0x083AD, 0x00000 };
static unsigned int de2F99C[] = { 0x08323, 0x00000 };
static unsigned int de2F99D[] = { 0x083BD, 0x00000 };
static unsigned int de2F99E[] = { 0x083E7, 0x00000 };
static unsigned int de2F99F[] = { 0x08457, 0x00000 };
static unsigned int de2F9A0[] = { 0x08353, 0x00000 };
static unsigned int de2F9A1[] = { 0x083CA, 0x00000 };
static unsigned int de2F9A2[] = { 0x083CC, 0x00000 };
static unsigned int de2F9A3[] = { 0x083DC, 0x00000 };
static unsigned int de2F9A4[] = { 0x26C36, 0x00000 };
static unsigned int de2F9A5[] = { 0x26D6B, 0x00000 };
static unsigned int de2F9A6[] = { 0x26CD5, 0x00000 };
static unsigned int de2F9A7[] = { 0x0452B, 0x00000 };
static unsigned int de2F9A8[] = { 0x084F1, 0x00000 };
static unsigned int de2F9A9[] = { 0x084F3, 0x00000 };
static unsigned int de2F9AA[] = { 0x08516, 0x00000 };
static unsigned int de2F9AB[] = { 0x273CA, 0x00000 };
static unsigned int de2F9AC[] = { 0x08564, 0x00000 };
static unsigned int de2F9AD[] = { 0x26F2C, 0x00000 };
static unsigned int de2F9AE[] = { 0x0455D, 0x00000 };
static unsigned int de2F9AF[] = { 0x04561, 0x00000 };
static unsigned int de2F9B0[] = { 0x26FB1, 0x00000 };
static unsigned int de2F9B1[] = { 0x270D2, 0x00000 };
static unsigned int de2F9B2[] = { 0x0456B, 0x00000 };
static unsigned int de2F9B3[] = { 0x08650, 0x00000 };
static unsigned int de2F9B4[] = { 0x0865C, 0x00000 };
static unsigned int de2F9B5[] = { 0x08667, 0x00000 };
static unsigned int de2F9B6[] = { 0x08669, 0x00000 };
static unsigned int de2F9B7[] = { 0x086A9, 0x00000 };
static unsigned int de2F9B8[] = { 0x08688, 0x00000 };
static unsigned int de2F9B9[] = { 0x0870E, 0x00000 };
static unsigned int de2F9BA[] = { 0x086E2, 0x00000 };
static unsigned int de2F9BB[] = { 0x08779, 0x00000 };
static unsigned int de2F9BC[] = { 0x08728, 0x00000 };
static unsigned int de2F9BD[] = { 0x0876B, 0x00000 };
static unsigned int de2F9BE[] = { 0x08786, 0x00000 };
static unsigned int de2F9BF[] = { 0x045D7, 0x00000 };
static unsigned int de2F9C0[] = { 0x087E1, 0x00000 };
static unsigned int de2F9C1[] = { 0x08801, 0x00000 };
static unsigned int de2F9C2[] = { 0x045F9, 0x00000 };
static unsigned int de2F9C3[] = { 0x08860, 0x00000 };
static unsigned int de2F9C4[] = { 0x08863, 0x00000 };
static unsigned int de2F9C5[] = { 0x27667, 0x00000 };
static unsigned int de2F9C6[] = { 0x088D7, 0x00000 };
static unsigned int de2F9C7[] = { 0x088DE, 0x00000 };
static unsigned int de2F9C8[] = { 0x04635, 0x00000 };
static unsigned int de2F9C9[] = { 0x088FA, 0x00000 };
static unsigned int de2F9CA[] = { 0x034BB, 0x00000 };
static unsigned int de2F9CB[] = { 0x278AE, 0x00000 };
static unsigned int de2F9CC[] = { 0x27966, 0x00000 };
static unsigned int de2F9CD[] = { 0x046BE, 0x00000 };
static unsigned int de2F9CE[] = { 0x046C7, 0x00000 };
static unsigned int de2F9CF[] = { 0x08AA0, 0x00000 };
static unsigned int de2F9D0[] = { 0x08AED, 0x00000 };
static unsigned int de2F9D1[] = { 0x08B8A, 0x00000 };
static unsigned int de2F9D2[] = { 0x08C55, 0x00000 };
static unsigned int de2F9D3[] = { 0x27CA8, 0x00000 };
static unsigned int de2F9D4[] = { 0x08CAB, 0x00000 };
static unsigned int de2F9D5[] = { 0x08CC1, 0x00000 };
static unsigned int de2F9D6[] = { 0x08D1B, 0x00000 };
static unsigned int de2F9D7[] = { 0x08D77, 0x00000 };
static unsigned int de2F9D8[] = { 0x27F2F, 0x00000 };
static unsigned int de2F9D9[] = { 0x20804, 0x00000 };
static unsigned int de2F9DA[] = { 0x08DCB, 0x00000 };
static unsigned int de2F9DB[] = { 0x08DBC, 0x00000 };
static unsigned int de2F9DC[] = { 0x08DF0, 0x00000 };
static unsigned int de2F9DD[] = { 0x208DE, 0x00000 };
static unsigned int de2F9DE[] = { 0x08ED4, 0x00000 };
static unsigned int de2F9DF[] = { 0x08F38, 0x00000 };
static unsigned int de2F9E0[] = { 0x285D2, 0x00000 };
static unsigned int de2F9E1[] = { 0x285ED, 0x00000 };
static unsigned int de2F9E2[] = { 0x09094, 0x00000 };
static unsigned int de2F9E3[] = { 0x090F1, 0x00000 };
static unsigned int de2F9E4[] = { 0x09111, 0x00000 };
static unsigned int de2F9E5[] = { 0x2872E, 0x00000 };
static unsigned int de2F9E6[] = { 0x0911B, 0x00000 };
static unsigned int de2F9E7[] = { 0x09238, 0x00000 };
static unsigned int de2F9E8[] = { 0x092D7, 0x00000 };
static unsigned int de2F9E9[] = { 0x092D8, 0x00000 };
static unsigned int de2F9EA[] = { 0x0927C, 0x00000 };
static unsigned int de2F9EB[] = { 0x093F9, 0x00000 };
static unsigned int de2F9EC[] = { 0x09415, 0x00000 };
static unsigned int de2F9ED[] = { 0x28BFA, 0x00000 };
static unsigned int de2F9EE[] = { 0x0958B, 0x00000 };
static unsigned int de2F9EF[] = { 0x04995, 0x00000 };
static unsigned int de2F9F0[] = { 0x095B7, 0x00000 };
static unsigned int de2F9F1[] = { 0x28D77, 0x00000 };
static unsigned int de2F9F2[] = { 0x049E6, 0x00000 };
static unsigned int de2F9F3[] = { 0x096C3, 0x00000 };
static unsigned int de2F9F4[] = { 0x05DB2, 0x00000 };
static unsigned int de2F9F5[] = { 0x09723, 0x00000 };
static unsigned int de2F9F6[] = { 0x29145, 0x00000 };
static unsigned int de2F9F7[] = { 0x2921A, 0x00000 };
static unsigned int de2F9F8[] = { 0x04A6E, 0x00000 };
static unsigned int de2F9F9[] = { 0x04A76, 0x00000 };
static unsigned int de2F9FA[] = { 0x097E0, 0x00000 };
static unsigned int de2F9FB[] = { 0x2940A, 0x00000 };
static unsigned int de2F9FC[] = { 0x04AB2, 0x00000 };
static unsigned int de2F9FD[] = { 0x29496, 0x00000 };
static unsigned int de2F9FE[] = { 0x0980B, 0x00000 };
static unsigned int de2F9FF[] = { 0x0980B, 0x00000 };
static unsigned int de2FA00[] = { 0x09829, 0x00000 };
static unsigned int de2FA01[] = { 0x295B6, 0x00000 };
static unsigned int de2FA02[] = { 0x098E2, 0x00000 };
static unsigned int de2FA03[] = { 0x04B33, 0x00000 };
static unsigned int de2FA04[] = { 0x09929, 0x00000 };
static unsigned int de2FA05[] = { 0x099A7, 0x00000 };
static unsigned int de2FA06[] = { 0x099C2, 0x00000 };
static unsigned int de2FA07[] = { 0x099FE, 0x00000 };
static unsigned int de2FA08[] = { 0x04BCE, 0x00000 };
static unsigned int de2FA09[] = { 0x29B30, 0x00000 };
static unsigned int de2FA0A[] = { 0x09B12, 0x00000 };
static unsigned int de2FA0B[] = { 0x09C40, 0x00000 };
static unsigned int de2FA0C[] = { 0x09CFD, 0x00000 };
static unsigned int de2FA0D[] = { 0x04CCE, 0x00000 };
static unsigned int de2FA0E[] = { 0x04CED, 0x00000 };
static unsigned int de2FA0F[] = { 0x09D67, 0x00000 };
static unsigned int de2FA10[] = { 0x2A0CE, 0x00000 };
static unsigned int de2FA11[] = { 0x04CF8, 0x00000 };
static unsigned int de2FA12[] = { 0x2A105, 0x00000 };
static unsigned int de2FA13[] = { 0x2A20E, 0x00000 };
static unsigned int de2FA14[] = { 0x2A291, 0x00000 };
static unsigned int de2FA15[] = { 0x09EBB, 0x00000 };
static unsigned int de2FA16[] = { 0x04D56, 0x00000 };
static unsigned int de2FA17[] = { 0x09EF9, 0x00000 };
static unsigned int de2FA18[] = { 0x09EFE, 0x00000 };
static unsigned int de2FA19[] = { 0x09F05, 0x00000 };
static unsigned int de2FA1A[] = { 0x09F0F, 0x00000 };
static unsigned int de2FA1B[] = { 0x09F16, 0x00000 };
static unsigned int de2FA1C[] = { 0x09F3B, 0x00000 };
static unsigned int de2FA1D[] = { 0x2A600, 0x00000 };

struct DecomposeDirectValue
{
  bool canonical;
  unsigned int *dest;
};
static unsigned int decomposeDirectTableSize = 2048;
static DecomposeDirectValue decomposeDirectTable[2048] = {
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false, de000A0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false, de000A8 }, { false,       0 }, { false, de000AA }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false, de000AF }, { false,       0 }, { false,       0 }, { false, de000B2 }, { false, de000B3 }, { false, de000B4 }, { false, de000B5 },
  { false,       0 }, { false,       0 }, { false, de000B8 }, { false, de000B9 }, { false, de000BA }, { false,       0 }, { false, de000BC },
  { false, de000BD }, { false, de000BE }, { false,       0 }, {  true, de000C0 }, {  true, de000C1 }, {  true, de000C2 }, {  true, de000C3 },
  {  true, de000C4 }, {  true, de000C5 }, { false,       0 }, {  true, de000C7 }, {  true, de000C8 }, {  true, de000C9 }, {  true, de000CA },
  {  true, de000CB }, {  true, de000CC }, {  true, de000CD }, {  true, de000CE }, {  true, de000CF }, { false,       0 }, {  true, de000D1 },
  {  true, de000D2 }, {  true, de000D3 }, {  true, de000D4 }, {  true, de000D5 }, {  true, de000D6 }, { false,       0 }, { false,       0 },
  {  true, de000D9 }, {  true, de000DA }, {  true, de000DB }, {  true, de000DC }, {  true, de000DD }, { false,       0 }, { false,       0 },
  {  true, de000E0 }, {  true, de000E1 }, {  true, de000E2 }, {  true, de000E3 }, {  true, de000E4 }, {  true, de000E5 }, { false,       0 },
  {  true, de000E7 }, {  true, de000E8 }, {  true, de000E9 }, {  true, de000EA }, {  true, de000EB }, {  true, de000EC }, {  true, de000ED },
  {  true, de000EE }, {  true, de000EF }, { false,       0 }, {  true, de000F1 }, {  true, de000F2 }, {  true, de000F3 }, {  true, de000F4 },
  {  true, de000F5 }, {  true, de000F6 }, { false,       0 }, { false,       0 }, {  true, de000F9 }, {  true, de000FA }, {  true, de000FB },
  {  true, de000FC }, {  true, de000FD }, { false,       0 }, {  true, de000FF }, {  true, de00100 }, {  true, de00101 }, {  true, de00102 },
  {  true, de00103 }, {  true, de00104 }, {  true, de00105 }, {  true, de00106 }, {  true, de00107 }, {  true, de00108 }, {  true, de00109 },
  {  true, de0010A }, {  true, de0010B }, {  true, de0010C }, {  true, de0010D }, {  true, de0010E }, {  true, de0010F }, { false,       0 },
  { false,       0 }, {  true, de00112 }, {  true, de00113 }, {  true, de00114 }, {  true, de00115 }, {  true, de00116 }, {  true, de00117 },
  {  true, de00118 }, {  true, de00119 }, {  true, de0011A }, {  true, de0011B }, {  true, de0011C }, {  true, de0011D }, {  true, de0011E },
  {  true, de0011F }, {  true, de00120 }, {  true, de00121 }, {  true, de00122 }, {  true, de00123 }, {  true, de00124 }, {  true, de00125 },
  { false,       0 }, { false,       0 }, {  true, de00128 }, {  true, de00129 }, {  true, de0012A }, {  true, de0012B }, {  true, de0012C },
  {  true, de0012D }, {  true, de0012E }, {  true, de0012F }, {  true, de00130 }, { false,       0 }, { false, de00132 }, { false, de00133 },
  {  true, de00134 }, {  true, de00135 }, {  true, de00136 }, {  true, de00137 }, { false,       0 }, {  true, de00139 }, {  true, de0013A },
  {  true, de0013B }, {  true, de0013C }, {  true, de0013D }, {  true, de0013E }, { false, de0013F }, { false, de00140 }, { false,       0 },
  { false,       0 }, {  true, de00143 }, {  true, de00144 }, {  true, de00145 }, {  true, de00146 }, {  true, de00147 }, {  true, de00148 },
  { false, de00149 }, { false,       0 }, { false,       0 }, {  true, de0014C }, {  true, de0014D }, {  true, de0014E }, {  true, de0014F },
  {  true, de00150 }, {  true, de00151 }, { false,       0 }, { false,       0 }, {  true, de00154 }, {  true, de00155 }, {  true, de00156 },
  {  true, de00157 }, {  true, de00158 }, {  true, de00159 }, {  true, de0015A }, {  true, de0015B }, {  true, de0015C }, {  true, de0015D },
  {  true, de0015E }, {  true, de0015F }, {  true, de00160 }, {  true, de00161 }, {  true, de00162 }, {  true, de00163 }, {  true, de00164 },
  {  true, de00165 }, { false,       0 }, { false,       0 }, {  true, de00168 }, {  true, de00169 }, {  true, de0016A }, {  true, de0016B },
  {  true, de0016C }, {  true, de0016D }, {  true, de0016E }, {  true, de0016F }, {  true, de00170 }, {  true, de00171 }, {  true, de00172 },
  {  true, de00173 }, {  true, de00174 }, {  true, de00175 }, {  true, de00176 }, {  true, de00177 }, {  true, de00178 }, {  true, de00179 },
  {  true, de0017A }, {  true, de0017B }, {  true, de0017C }, {  true, de0017D }, {  true, de0017E }, { false, de0017F }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, {  true, de001A0 }, {  true, de001A1 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de001AF }, {  true, de001B0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false, de001C4 }, { false, de001C5 }, { false, de001C6 },
  { false, de001C7 }, { false, de001C8 }, { false, de001C9 }, { false, de001CA }, { false, de001CB }, { false, de001CC }, {  true, de001CD },
  {  true, de001CE }, {  true, de001CF }, {  true, de001D0 }, {  true, de001D1 }, {  true, de001D2 }, {  true, de001D3 }, {  true, de001D4 },
  {  true, de001D5 }, {  true, de001D6 }, {  true, de001D7 }, {  true, de001D8 }, {  true, de001D9 }, {  true, de001DA }, {  true, de001DB },
  {  true, de001DC }, { false,       0 }, {  true, de001DE }, {  true, de001DF }, {  true, de001E0 }, {  true, de001E1 }, {  true, de001E2 },
  {  true, de001E3 }, { false,       0 }, { false,       0 }, {  true, de001E6 }, {  true, de001E7 }, {  true, de001E8 }, {  true, de001E9 },
  {  true, de001EA }, {  true, de001EB }, {  true, de001EC }, {  true, de001ED }, {  true, de001EE }, {  true, de001EF }, {  true, de001F0 },
  { false, de001F1 }, { false, de001F2 }, { false, de001F3 }, {  true, de001F4 }, {  true, de001F5 }, { false,       0 }, { false,       0 },
  {  true, de001F8 }, {  true, de001F9 }, {  true, de001FA }, {  true, de001FB }, {  true, de001FC }, {  true, de001FD }, {  true, de001FE },
  {  true, de001FF }, {  true, de00200 }, {  true, de00201 }, {  true, de00202 }, {  true, de00203 }, {  true, de00204 }, {  true, de00205 },
  {  true, de00206 }, {  true, de00207 }, {  true, de00208 }, {  true, de00209 }, {  true, de0020A }, {  true, de0020B }, {  true, de0020C },
  {  true, de0020D }, {  true, de0020E }, {  true, de0020F }, {  true, de00210 }, {  true, de00211 }, {  true, de00212 }, {  true, de00213 },
  {  true, de00214 }, {  true, de00215 }, {  true, de00216 }, {  true, de00217 }, {  true, de00218 }, {  true, de00219 }, {  true, de0021A },
  {  true, de0021B }, { false,       0 }, { false,       0 }, {  true, de0021E }, {  true, de0021F }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00226 }, {  true, de00227 }, {  true, de00228 },
  {  true, de00229 }, {  true, de0022A }, {  true, de0022B }, {  true, de0022C }, {  true, de0022D }, {  true, de0022E }, {  true, de0022F },
  {  true, de00230 }, {  true, de00231 }, {  true, de00232 }, {  true, de00233 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false, de002B0 }, { false, de002B1 }, { false, de002B2 }, { false, de002B3 }, { false, de002B4 },
  { false, de002B5 }, { false, de002B6 }, { false, de002B7 }, { false, de002B8 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false, de002D8 }, { false, de002D9 }, { false, de002DA }, { false, de002DB }, { false, de002DC }, { false, de002DD }, { false,       0 },
  { false,       0 }, { false, de002E0 }, { false, de002E1 }, { false, de002E2 }, { false, de002E3 }, { false, de002E4 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00340 },
  {  true, de00341 }, { false,       0 }, {  true, de00343 }, {  true, de00344 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, {  true, de00374 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false, de0037A }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de0037E }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false, de00384 }, {  true, de00385 }, {  true, de00386 },
  {  true, de00387 }, {  true, de00388 }, {  true, de00389 }, {  true, de0038A }, { false,       0 }, {  true, de0038C }, { false,       0 },
  {  true, de0038E }, {  true, de0038F }, {  true, de00390 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  {  true, de003AA }, {  true, de003AB }, {  true, de003AC }, {  true, de003AD }, {  true, de003AE }, {  true, de003AF }, {  true, de003B0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de003CA }, {  true, de003CB }, {  true, de003CC },
  {  true, de003CD }, {  true, de003CE }, { false,       0 }, { false, de003D0 }, { false, de003D1 }, { false, de003D2 }, {  true, de003D3 },
  {  true, de003D4 }, { false, de003D5 }, { false, de003D6 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false, de003F0 }, { false, de003F1 }, { false, de003F2 }, { false,       0 }, { false, de003F4 }, { false, de003F5 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false, de003F9 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, {  true, de00400 }, {  true, de00401 }, { false,       0 }, {  true, de00403 }, { false,       0 },
  { false,       0 }, { false,       0 }, {  true, de00407 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  {  true, de0040C }, {  true, de0040D }, {  true, de0040E }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00419 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00439 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00450 }, {  true, de00451 },
  { false,       0 }, {  true, de00453 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de00457 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, {  true, de0045C }, {  true, de0045D }, {  true, de0045E }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, {  true, de00476 }, {  true, de00477 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de004C1 },
  {  true, de004C2 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  {  true, de004D0 }, {  true, de004D1 }, {  true, de004D2 }, {  true, de004D3 }, { false,       0 }, { false,       0 }, {  true, de004D6 },
  {  true, de004D7 }, { false,       0 }, { false,       0 }, {  true, de004DA }, {  true, de004DB }, {  true, de004DC }, {  true, de004DD },
  {  true, de004DE }, {  true, de004DF }, { false,       0 }, { false,       0 }, {  true, de004E2 }, {  true, de004E3 }, {  true, de004E4 },
  {  true, de004E5 }, {  true, de004E6 }, {  true, de004E7 }, { false,       0 }, { false,       0 }, {  true, de004EA }, {  true, de004EB },
  {  true, de004EC }, {  true, de004ED }, {  true, de004EE }, {  true, de004EF }, {  true, de004F0 }, {  true, de004F1 }, {  true, de004F2 },
  {  true, de004F3 }, {  true, de004F4 }, {  true, de004F5 }, { false,       0 }, { false,       0 }, {  true, de004F8 }, {  true, de004F9 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false, de00587 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, {  true, de00622 }, {  true, de00623 }, {  true, de00624 }, {  true, de00625 }, {  true, de00626 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false, de00675 }, { false, de00676 }, { false, de00677 }, { false, de00678 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de006C0 },
  { false,       0 }, {  true, de006C2 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, {  true, de006D3 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 },
  { false,       0 }, { false,       0 }, { false,       0 }, { false,       0 }, 
};

struct DecomposeValue
{
  unsigned int ch;
  bool canonical;
  unsigned int *dest;
};
// static unsigned int decomposeTableSize = 5390;
// static DecomposeValue decomposeTable[5390] = {
static unsigned int decomposeTableSize = 4978;
static DecomposeValue decomposeTable[4978] = {
// { 0x000A0, false, de000A0 }, { 0x000A8, false, de000A8 }, { 0x000AA, false, de000AA }, { 0x000AF, false, de000AF },
// { 0x000B2, false, de000B2 }, { 0x000B3, false, de000B3 }, { 0x000B4, false, de000B4 }, { 0x000B5, false, de000B5 },
// { 0x000B8, false, de000B8 }, { 0x000B9, false, de000B9 }, { 0x000BA, false, de000BA }, { 0x000BC, false, de000BC },
// { 0x000BD, false, de000BD }, { 0x000BE, false, de000BE }, { 0x000C0, true, de000C0 }, { 0x000C1, true, de000C1 },
// { 0x000C2, true, de000C2 }, { 0x000C3, true, de000C3 }, { 0x000C4, true, de000C4 }, { 0x000C5, true, de000C5 },
// { 0x000C7, true, de000C7 }, { 0x000C8, true, de000C8 }, { 0x000C9, true, de000C9 }, { 0x000CA, true, de000CA },
// { 0x000CB, true, de000CB }, { 0x000CC, true, de000CC }, { 0x000CD, true, de000CD }, { 0x000CE, true, de000CE },
// { 0x000CF, true, de000CF }, { 0x000D1, true, de000D1 }, { 0x000D2, true, de000D2 }, { 0x000D3, true, de000D3 },
// { 0x000D4, true, de000D4 }, { 0x000D5, true, de000D5 }, { 0x000D6, true, de000D6 }, { 0x000D9, true, de000D9 },
// { 0x000DA, true, de000DA }, { 0x000DB, true, de000DB }, { 0x000DC, true, de000DC }, { 0x000DD, true, de000DD },
// { 0x000E0, true, de000E0 }, { 0x000E1, true, de000E1 }, { 0x000E2, true, de000E2 }, { 0x000E3, true, de000E3 },
// { 0x000E4, true, de000E4 }, { 0x000E5, true, de000E5 }, { 0x000E7, true, de000E7 }, { 0x000E8, true, de000E8 },
// { 0x000E9, true, de000E9 }, { 0x000EA, true, de000EA }, { 0x000EB, true, de000EB }, { 0x000EC, true, de000EC },
// { 0x000ED, true, de000ED }, { 0x000EE, true, de000EE }, { 0x000EF, true, de000EF }, { 0x000F1, true, de000F1 },
// { 0x000F2, true, de000F2 }, { 0x000F3, true, de000F3 }, { 0x000F4, true, de000F4 }, { 0x000F5, true, de000F5 },
// { 0x000F6, true, de000F6 }, { 0x000F9, true, de000F9 }, { 0x000FA, true, de000FA }, { 0x000FB, true, de000FB },
// { 0x000FC, true, de000FC }, { 0x000FD, true, de000FD }, { 0x000FF, true, de000FF }, { 0x00100, true, de00100 },
// { 0x00101, true, de00101 }, { 0x00102, true, de00102 }, { 0x00103, true, de00103 }, { 0x00104, true, de00104 },
// { 0x00105, true, de00105 }, { 0x00106, true, de00106 }, { 0x00107, true, de00107 }, { 0x00108, true, de00108 },
// { 0x00109, true, de00109 }, { 0x0010A, true, de0010A }, { 0x0010B, true, de0010B }, { 0x0010C, true, de0010C },
// { 0x0010D, true, de0010D }, { 0x0010E, true, de0010E }, { 0x0010F, true, de0010F }, { 0x00112, true, de00112 },
// { 0x00113, true, de00113 }, { 0x00114, true, de00114 }, { 0x00115, true, de00115 }, { 0x00116, true, de00116 },
// { 0x00117, true, de00117 }, { 0x00118, true, de00118 }, { 0x00119, true, de00119 }, { 0x0011A, true, de0011A },
// { 0x0011B, true, de0011B }, { 0x0011C, true, de0011C }, { 0x0011D, true, de0011D }, { 0x0011E, true, de0011E },
// { 0x0011F, true, de0011F }, { 0x00120, true, de00120 }, { 0x00121, true, de00121 }, { 0x00122, true, de00122 },
// { 0x00123, true, de00123 }, { 0x00124, true, de00124 }, { 0x00125, true, de00125 }, { 0x00128, true, de00128 },
// { 0x00129, true, de00129 }, { 0x0012A, true, de0012A }, { 0x0012B, true, de0012B }, { 0x0012C, true, de0012C },
// { 0x0012D, true, de0012D }, { 0x0012E, true, de0012E }, { 0x0012F, true, de0012F }, { 0x00130, true, de00130 },
// { 0x00132, false, de00132 }, { 0x00133, false, de00133 }, { 0x00134, true, de00134 }, { 0x00135, true, de00135 },
// { 0x00136, true, de00136 }, { 0x00137, true, de00137 }, { 0x00139, true, de00139 }, { 0x0013A, true, de0013A },
// { 0x0013B, true, de0013B }, { 0x0013C, true, de0013C }, { 0x0013D, true, de0013D }, { 0x0013E, true, de0013E },
// { 0x0013F, false, de0013F }, { 0x00140, false, de00140 }, { 0x00143, true, de00143 }, { 0x00144, true, de00144 },
// { 0x00145, true, de00145 }, { 0x00146, true, de00146 }, { 0x00147, true, de00147 }, { 0x00148, true, de00148 },
// { 0x00149, false, de00149 }, { 0x0014C, true, de0014C }, { 0x0014D, true, de0014D }, { 0x0014E, true, de0014E },
// { 0x0014F, true, de0014F }, { 0x00150, true, de00150 }, { 0x00151, true, de00151 }, { 0x00154, true, de00154 },
// { 0x00155, true, de00155 }, { 0x00156, true, de00156 }, { 0x00157, true, de00157 }, { 0x00158, true, de00158 },
// { 0x00159, true, de00159 }, { 0x0015A, true, de0015A }, { 0x0015B, true, de0015B }, { 0x0015C, true, de0015C },
// { 0x0015D, true, de0015D }, { 0x0015E, true, de0015E }, { 0x0015F, true, de0015F }, { 0x00160, true, de00160 },
// { 0x00161, true, de00161 }, { 0x00162, true, de00162 }, { 0x00163, true, de00163 }, { 0x00164, true, de00164 },
// { 0x00165, true, de00165 }, { 0x00168, true, de00168 }, { 0x00169, true, de00169 }, { 0x0016A, true, de0016A },
// { 0x0016B, true, de0016B }, { 0x0016C, true, de0016C }, { 0x0016D, true, de0016D }, { 0x0016E, true, de0016E },
// { 0x0016F, true, de0016F }, { 0x00170, true, de00170 }, { 0x00171, true, de00171 }, { 0x00172, true, de00172 },
// { 0x00173, true, de00173 }, { 0x00174, true, de00174 }, { 0x00175, true, de00175 }, { 0x00176, true, de00176 },
// { 0x00177, true, de00177 }, { 0x00178, true, de00178 }, { 0x00179, true, de00179 }, { 0x0017A, true, de0017A },
// { 0x0017B, true, de0017B }, { 0x0017C, true, de0017C }, { 0x0017D, true, de0017D }, { 0x0017E, true, de0017E },
// { 0x0017F, false, de0017F }, { 0x001A0, true, de001A0 }, { 0x001A1, true, de001A1 }, { 0x001AF, true, de001AF },
// { 0x001B0, true, de001B0 }, { 0x001C4, false, de001C4 }, { 0x001C5, false, de001C5 }, { 0x001C6, false, de001C6 },
// { 0x001C7, false, de001C7 }, { 0x001C8, false, de001C8 }, { 0x001C9, false, de001C9 }, { 0x001CA, false, de001CA },
// { 0x001CB, false, de001CB }, { 0x001CC, false, de001CC }, { 0x001CD, true, de001CD }, { 0x001CE, true, de001CE },
// { 0x001CF, true, de001CF }, { 0x001D0, true, de001D0 }, { 0x001D1, true, de001D1 }, { 0x001D2, true, de001D2 },
// { 0x001D3, true, de001D3 }, { 0x001D4, true, de001D4 }, { 0x001D5, true, de001D5 }, { 0x001D6, true, de001D6 },
// { 0x001D7, true, de001D7 }, { 0x001D8, true, de001D8 }, { 0x001D9, true, de001D9 }, { 0x001DA, true, de001DA },
// { 0x001DB, true, de001DB }, { 0x001DC, true, de001DC }, { 0x001DE, true, de001DE }, { 0x001DF, true, de001DF },
// { 0x001E0, true, de001E0 }, { 0x001E1, true, de001E1 }, { 0x001E2, true, de001E2 }, { 0x001E3, true, de001E3 },
// { 0x001E6, true, de001E6 }, { 0x001E7, true, de001E7 }, { 0x001E8, true, de001E8 }, { 0x001E9, true, de001E9 },
// { 0x001EA, true, de001EA }, { 0x001EB, true, de001EB }, { 0x001EC, true, de001EC }, { 0x001ED, true, de001ED },
// { 0x001EE, true, de001EE }, { 0x001EF, true, de001EF }, { 0x001F0, true, de001F0 }, { 0x001F1, false, de001F1 },
// { 0x001F2, false, de001F2 }, { 0x001F3, false, de001F3 }, { 0x001F4, true, de001F4 }, { 0x001F5, true, de001F5 },
// { 0x001F8, true, de001F8 }, { 0x001F9, true, de001F9 }, { 0x001FA, true, de001FA }, { 0x001FB, true, de001FB },
// { 0x001FC, true, de001FC }, { 0x001FD, true, de001FD }, { 0x001FE, true, de001FE }, { 0x001FF, true, de001FF },
// { 0x00200, true, de00200 }, { 0x00201, true, de00201 }, { 0x00202, true, de00202 }, { 0x00203, true, de00203 },
// { 0x00204, true, de00204 }, { 0x00205, true, de00205 }, { 0x00206, true, de00206 }, { 0x00207, true, de00207 },
// { 0x00208, true, de00208 }, { 0x00209, true, de00209 }, { 0x0020A, true, de0020A }, { 0x0020B, true, de0020B },
// { 0x0020C, true, de0020C }, { 0x0020D, true, de0020D }, { 0x0020E, true, de0020E }, { 0x0020F, true, de0020F },
// { 0x00210, true, de00210 }, { 0x00211, true, de00211 }, { 0x00212, true, de00212 }, { 0x00213, true, de00213 },
// { 0x00214, true, de00214 }, { 0x00215, true, de00215 }, { 0x00216, true, de00216 }, { 0x00217, true, de00217 },
// { 0x00218, true, de00218 }, { 0x00219, true, de00219 }, { 0x0021A, true, de0021A }, { 0x0021B, true, de0021B },
// { 0x0021E, true, de0021E }, { 0x0021F, true, de0021F }, { 0x00226, true, de00226 }, { 0x00227, true, de00227 },
// { 0x00228, true, de00228 }, { 0x00229, true, de00229 }, { 0x0022A, true, de0022A }, { 0x0022B, true, de0022B },
// { 0x0022C, true, de0022C }, { 0x0022D, true, de0022D }, { 0x0022E, true, de0022E }, { 0x0022F, true, de0022F },
// { 0x00230, true, de00230 }, { 0x00231, true, de00231 }, { 0x00232, true, de00232 }, { 0x00233, true, de00233 },
// { 0x002B0, false, de002B0 }, { 0x002B1, false, de002B1 }, { 0x002B2, false, de002B2 }, { 0x002B3, false, de002B3 },
// { 0x002B4, false, de002B4 }, { 0x002B5, false, de002B5 }, { 0x002B6, false, de002B6 }, { 0x002B7, false, de002B7 },
// { 0x002B8, false, de002B8 }, { 0x002D8, false, de002D8 }, { 0x002D9, false, de002D9 }, { 0x002DA, false, de002DA },
// { 0x002DB, false, de002DB }, { 0x002DC, false, de002DC }, { 0x002DD, false, de002DD }, { 0x002E0, false, de002E0 },
// { 0x002E1, false, de002E1 }, { 0x002E2, false, de002E2 }, { 0x002E3, false, de002E3 }, { 0x002E4, false, de002E4 },
// { 0x00340, true, de00340 }, { 0x00341, true, de00341 }, { 0x00343, true, de00343 }, { 0x00344, true, de00344 },
// { 0x00374, true, de00374 }, { 0x0037A, false, de0037A }, { 0x0037E, true, de0037E }, { 0x00384, false, de00384 },
// { 0x00385, true, de00385 }, { 0x00386, true, de00386 }, { 0x00387, true, de00387 }, { 0x00388, true, de00388 },
// { 0x00389, true, de00389 }, { 0x0038A, true, de0038A }, { 0x0038C, true, de0038C }, { 0x0038E, true, de0038E },
// { 0x0038F, true, de0038F }, { 0x00390, true, de00390 }, { 0x003AA, true, de003AA }, { 0x003AB, true, de003AB },
// { 0x003AC, true, de003AC }, { 0x003AD, true, de003AD }, { 0x003AE, true, de003AE }, { 0x003AF, true, de003AF },
// { 0x003B0, true, de003B0 }, { 0x003CA, true, de003CA }, { 0x003CB, true, de003CB }, { 0x003CC, true, de003CC },
// { 0x003CD, true, de003CD }, { 0x003CE, true, de003CE }, { 0x003D0, false, de003D0 }, { 0x003D1, false, de003D1 },
// { 0x003D2, false, de003D2 }, { 0x003D3, true, de003D3 }, { 0x003D4, true, de003D4 }, { 0x003D5, false, de003D5 },
// { 0x003D6, false, de003D6 }, { 0x003F0, false, de003F0 }, { 0x003F1, false, de003F1 }, { 0x003F2, false, de003F2 },
// { 0x003F4, false, de003F4 }, { 0x003F5, false, de003F5 }, { 0x003F9, false, de003F9 }, { 0x00400, true, de00400 },
// { 0x00401, true, de00401 }, { 0x00403, true, de00403 }, { 0x00407, true, de00407 }, { 0x0040C, true, de0040C },
// { 0x0040D, true, de0040D }, { 0x0040E, true, de0040E }, { 0x00419, true, de00419 }, { 0x00439, true, de00439 },
// { 0x00450, true, de00450 }, { 0x00451, true, de00451 }, { 0x00453, true, de00453 }, { 0x00457, true, de00457 },
// { 0x0045C, true, de0045C }, { 0x0045D, true, de0045D }, { 0x0045E, true, de0045E }, { 0x00476, true, de00476 },
// { 0x00477, true, de00477 }, { 0x004C1, true, de004C1 }, { 0x004C2, true, de004C2 }, { 0x004D0, true, de004D0 },
// { 0x004D1, true, de004D1 }, { 0x004D2, true, de004D2 }, { 0x004D3, true, de004D3 }, { 0x004D6, true, de004D6 },
// { 0x004D7, true, de004D7 }, { 0x004DA, true, de004DA }, { 0x004DB, true, de004DB }, { 0x004DC, true, de004DC },
// { 0x004DD, true, de004DD }, { 0x004DE, true, de004DE }, { 0x004DF, true, de004DF }, { 0x004E2, true, de004E2 },
// { 0x004E3, true, de004E3 }, { 0x004E4, true, de004E4 }, { 0x004E5, true, de004E5 }, { 0x004E6, true, de004E6 },
// { 0x004E7, true, de004E7 }, { 0x004EA, true, de004EA }, { 0x004EB, true, de004EB }, { 0x004EC, true, de004EC },
// { 0x004ED, true, de004ED }, { 0x004EE, true, de004EE }, { 0x004EF, true, de004EF }, { 0x004F0, true, de004F0 },
// { 0x004F1, true, de004F1 }, { 0x004F2, true, de004F2 }, { 0x004F3, true, de004F3 }, { 0x004F4, true, de004F4 },
// { 0x004F5, true, de004F5 }, { 0x004F8, true, de004F8 }, { 0x004F9, true, de004F9 }, { 0x00587, false, de00587 },
// { 0x00622, true, de00622 }, { 0x00623, true, de00623 }, { 0x00624, true, de00624 }, { 0x00625, true, de00625 },
// { 0x00626, true, de00626 }, { 0x00675, false, de00675 }, { 0x00676, false, de00676 }, { 0x00677, false, de00677 },
// { 0x00678, false, de00678 }, { 0x006C0, true, de006C0 }, { 0x006C2, true, de006C2 }, { 0x006D3, true, de006D3 },

{ 0x00929, true, de00929 }, { 0x00931, true, de00931 }, { 0x00934, true, de00934 }, { 0x00958, true, de00958 },
{ 0x00959, true, de00959 }, { 0x0095A, true, de0095A }, { 0x0095B, true, de0095B }, { 0x0095C, true, de0095C },
{ 0x0095D, true, de0095D }, { 0x0095E, true, de0095E }, { 0x0095F, true, de0095F }, { 0x009CB, true, de009CB },
{ 0x009CC, true, de009CC }, { 0x009DC, true, de009DC }, { 0x009DD, true, de009DD }, { 0x009DF, true, de009DF },
{ 0x00A33, true, de00A33 }, { 0x00A36, true, de00A36 }, { 0x00A59, true, de00A59 }, { 0x00A5A, true, de00A5A },
{ 0x00A5B, true, de00A5B }, { 0x00A5E, true, de00A5E }, { 0x00B48, true, de00B48 }, { 0x00B4B, true, de00B4B },
{ 0x00B4C, true, de00B4C }, { 0x00B5C, true, de00B5C }, { 0x00B5D, true, de00B5D }, { 0x00B94, true, de00B94 },
{ 0x00BCA, true, de00BCA }, { 0x00BCB, true, de00BCB }, { 0x00BCC, true, de00BCC }, { 0x00C48, true, de00C48 },
{ 0x00CC0, true, de00CC0 }, { 0x00CC7, true, de00CC7 }, { 0x00CC8, true, de00CC8 }, { 0x00CCA, true, de00CCA },
{ 0x00CCB, true, de00CCB }, { 0x00D4A, true, de00D4A }, { 0x00D4B, true, de00D4B }, { 0x00D4C, true, de00D4C },
{ 0x00DDA, true, de00DDA }, { 0x00DDC, true, de00DDC }, { 0x00DDD, true, de00DDD }, { 0x00DDE, true, de00DDE },
{ 0x00E33, false, de00E33 }, { 0x00EB3, false, de00EB3 }, { 0x00EDC, false, de00EDC }, { 0x00EDD, false, de00EDD },
{ 0x00F0C, false, de00F0C }, { 0x00F43, true, de00F43 }, { 0x00F4D, true, de00F4D }, { 0x00F52, true, de00F52 },
{ 0x00F57, true, de00F57 }, { 0x00F5C, true, de00F5C }, { 0x00F69, true, de00F69 }, { 0x00F73, true, de00F73 },
{ 0x00F75, true, de00F75 }, { 0x00F76, true, de00F76 }, { 0x00F77, false, de00F77 }, { 0x00F78, true, de00F78 },
{ 0x00F79, false, de00F79 }, { 0x00F81, true, de00F81 }, { 0x00F93, true, de00F93 }, { 0x00F9D, true, de00F9D },
{ 0x00FA2, true, de00FA2 }, { 0x00FA7, true, de00FA7 }, { 0x00FAC, true, de00FAC }, { 0x00FB9, true, de00FB9 },
{ 0x01026, true, de01026 }, { 0x010FC, false, de010FC }, { 0x01D2C, false, de01D2C }, { 0x01D2D, false, de01D2D },
{ 0x01D2E, false, de01D2E }, { 0x01D30, false, de01D30 }, { 0x01D31, false, de01D31 }, { 0x01D32, false, de01D32 },
{ 0x01D33, false, de01D33 }, { 0x01D34, false, de01D34 }, { 0x01D35, false, de01D35 }, { 0x01D36, false, de01D36 },
{ 0x01D37, false, de01D37 }, { 0x01D38, false, de01D38 }, { 0x01D39, false, de01D39 }, { 0x01D3A, false, de01D3A },
{ 0x01D3C, false, de01D3C }, { 0x01D3D, false, de01D3D }, { 0x01D3E, false, de01D3E }, { 0x01D3F, false, de01D3F },
{ 0x01D40, false, de01D40 }, { 0x01D41, false, de01D41 }, { 0x01D42, false, de01D42 }, { 0x01D43, false, de01D43 },
{ 0x01D44, false, de01D44 }, { 0x01D45, false, de01D45 }, { 0x01D46, false, de01D46 }, { 0x01D47, false, de01D47 },
{ 0x01D48, false, de01D48 }, { 0x01D49, false, de01D49 }, { 0x01D4A, false, de01D4A }, { 0x01D4B, false, de01D4B },
{ 0x01D4C, false, de01D4C }, { 0x01D4D, false, de01D4D }, { 0x01D4F, false, de01D4F }, { 0x01D50, false, de01D50 },
{ 0x01D51, false, de01D51 }, { 0x01D52, false, de01D52 }, { 0x01D53, false, de01D53 }, { 0x01D54, false, de01D54 },
{ 0x01D55, false, de01D55 }, { 0x01D56, false, de01D56 }, { 0x01D57, false, de01D57 }, { 0x01D58, false, de01D58 },
{ 0x01D59, false, de01D59 }, { 0x01D5A, false, de01D5A }, { 0x01D5B, false, de01D5B }, { 0x01D5C, false, de01D5C },
{ 0x01D5D, false, de01D5D }, { 0x01D5E, false, de01D5E }, { 0x01D5F, false, de01D5F }, { 0x01D60, false, de01D60 },
{ 0x01D61, false, de01D61 }, { 0x01D62, false, de01D62 }, { 0x01D63, false, de01D63 }, { 0x01D64, false, de01D64 },
{ 0x01D65, false, de01D65 }, { 0x01D66, false, de01D66 }, { 0x01D67, false, de01D67 }, { 0x01D68, false, de01D68 },
{ 0x01D69, false, de01D69 }, { 0x01D6A, false, de01D6A }, { 0x01D78, false, de01D78 }, { 0x01D9B, false, de01D9B },
{ 0x01D9C, false, de01D9C }, { 0x01D9D, false, de01D9D }, { 0x01D9E, false, de01D9E }, { 0x01D9F, false, de01D9F },
{ 0x01DA0, false, de01DA0 }, { 0x01DA1, false, de01DA1 }, { 0x01DA2, false, de01DA2 }, { 0x01DA3, false, de01DA3 },
{ 0x01DA4, false, de01DA4 }, { 0x01DA5, false, de01DA5 }, { 0x01DA6, false, de01DA6 }, { 0x01DA7, false, de01DA7 },
{ 0x01DA8, false, de01DA8 }, { 0x01DA9, false, de01DA9 }, { 0x01DAA, false, de01DAA }, { 0x01DAB, false, de01DAB },
{ 0x01DAC, false, de01DAC }, { 0x01DAD, false, de01DAD }, { 0x01DAE, false, de01DAE }, { 0x01DAF, false, de01DAF },
{ 0x01DB0, false, de01DB0 }, { 0x01DB1, false, de01DB1 }, { 0x01DB2, false, de01DB2 }, { 0x01DB3, false, de01DB3 },
{ 0x01DB4, false, de01DB4 }, { 0x01DB5, false, de01DB5 }, { 0x01DB6, false, de01DB6 }, { 0x01DB7, false, de01DB7 },
{ 0x01DB8, false, de01DB8 }, { 0x01DB9, false, de01DB9 }, { 0x01DBA, false, de01DBA }, { 0x01DBB, false, de01DBB },
{ 0x01DBC, false, de01DBC }, { 0x01DBD, false, de01DBD }, { 0x01DBE, false, de01DBE }, { 0x01DBF, false, de01DBF },
{ 0x01E00, true, de01E00 }, { 0x01E01, true, de01E01 }, { 0x01E02, true, de01E02 }, { 0x01E03, true, de01E03 },
{ 0x01E04, true, de01E04 }, { 0x01E05, true, de01E05 }, { 0x01E06, true, de01E06 }, { 0x01E07, true, de01E07 },
{ 0x01E08, true, de01E08 }, { 0x01E09, true, de01E09 }, { 0x01E0A, true, de01E0A }, { 0x01E0B, true, de01E0B },
{ 0x01E0C, true, de01E0C }, { 0x01E0D, true, de01E0D }, { 0x01E0E, true, de01E0E }, { 0x01E0F, true, de01E0F },
{ 0x01E10, true, de01E10 }, { 0x01E11, true, de01E11 }, { 0x01E12, true, de01E12 }, { 0x01E13, true, de01E13 },
{ 0x01E14, true, de01E14 }, { 0x01E15, true, de01E15 }, { 0x01E16, true, de01E16 }, { 0x01E17, true, de01E17 },
{ 0x01E18, true, de01E18 }, { 0x01E19, true, de01E19 }, { 0x01E1A, true, de01E1A }, { 0x01E1B, true, de01E1B },
{ 0x01E1C, true, de01E1C }, { 0x01E1D, true, de01E1D }, { 0x01E1E, true, de01E1E }, { 0x01E1F, true, de01E1F },
{ 0x01E20, true, de01E20 }, { 0x01E21, true, de01E21 }, { 0x01E22, true, de01E22 }, { 0x01E23, true, de01E23 },
{ 0x01E24, true, de01E24 }, { 0x01E25, true, de01E25 }, { 0x01E26, true, de01E26 }, { 0x01E27, true, de01E27 },
{ 0x01E28, true, de01E28 }, { 0x01E29, true, de01E29 }, { 0x01E2A, true, de01E2A }, { 0x01E2B, true, de01E2B },
{ 0x01E2C, true, de01E2C }, { 0x01E2D, true, de01E2D }, { 0x01E2E, true, de01E2E }, { 0x01E2F, true, de01E2F },
{ 0x01E30, true, de01E30 }, { 0x01E31, true, de01E31 }, { 0x01E32, true, de01E32 }, { 0x01E33, true, de01E33 },
{ 0x01E34, true, de01E34 }, { 0x01E35, true, de01E35 }, { 0x01E36, true, de01E36 }, { 0x01E37, true, de01E37 },
{ 0x01E38, true, de01E38 }, { 0x01E39, true, de01E39 }, { 0x01E3A, true, de01E3A }, { 0x01E3B, true, de01E3B },
{ 0x01E3C, true, de01E3C }, { 0x01E3D, true, de01E3D }, { 0x01E3E, true, de01E3E }, { 0x01E3F, true, de01E3F },
{ 0x01E40, true, de01E40 }, { 0x01E41, true, de01E41 }, { 0x01E42, true, de01E42 }, { 0x01E43, true, de01E43 },
{ 0x01E44, true, de01E44 }, { 0x01E45, true, de01E45 }, { 0x01E46, true, de01E46 }, { 0x01E47, true, de01E47 },
{ 0x01E48, true, de01E48 }, { 0x01E49, true, de01E49 }, { 0x01E4A, true, de01E4A }, { 0x01E4B, true, de01E4B },
{ 0x01E4C, true, de01E4C }, { 0x01E4D, true, de01E4D }, { 0x01E4E, true, de01E4E }, { 0x01E4F, true, de01E4F },
{ 0x01E50, true, de01E50 }, { 0x01E51, true, de01E51 }, { 0x01E52, true, de01E52 }, { 0x01E53, true, de01E53 },
{ 0x01E54, true, de01E54 }, { 0x01E55, true, de01E55 }, { 0x01E56, true, de01E56 }, { 0x01E57, true, de01E57 },
{ 0x01E58, true, de01E58 }, { 0x01E59, true, de01E59 }, { 0x01E5A, true, de01E5A }, { 0x01E5B, true, de01E5B },
{ 0x01E5C, true, de01E5C }, { 0x01E5D, true, de01E5D }, { 0x01E5E, true, de01E5E }, { 0x01E5F, true, de01E5F },
{ 0x01E60, true, de01E60 }, { 0x01E61, true, de01E61 }, { 0x01E62, true, de01E62 }, { 0x01E63, true, de01E63 },
{ 0x01E64, true, de01E64 }, { 0x01E65, true, de01E65 }, { 0x01E66, true, de01E66 }, { 0x01E67, true, de01E67 },
{ 0x01E68, true, de01E68 }, { 0x01E69, true, de01E69 }, { 0x01E6A, true, de01E6A }, { 0x01E6B, true, de01E6B },
{ 0x01E6C, true, de01E6C }, { 0x01E6D, true, de01E6D }, { 0x01E6E, true, de01E6E }, { 0x01E6F, true, de01E6F },
{ 0x01E70, true, de01E70 }, { 0x01E71, true, de01E71 }, { 0x01E72, true, de01E72 }, { 0x01E73, true, de01E73 },
{ 0x01E74, true, de01E74 }, { 0x01E75, true, de01E75 }, { 0x01E76, true, de01E76 }, { 0x01E77, true, de01E77 },
{ 0x01E78, true, de01E78 }, { 0x01E79, true, de01E79 }, { 0x01E7A, true, de01E7A }, { 0x01E7B, true, de01E7B },
{ 0x01E7C, true, de01E7C }, { 0x01E7D, true, de01E7D }, { 0x01E7E, true, de01E7E }, { 0x01E7F, true, de01E7F },
{ 0x01E80, true, de01E80 }, { 0x01E81, true, de01E81 }, { 0x01E82, true, de01E82 }, { 0x01E83, true, de01E83 },
{ 0x01E84, true, de01E84 }, { 0x01E85, true, de01E85 }, { 0x01E86, true, de01E86 }, { 0x01E87, true, de01E87 },
{ 0x01E88, true, de01E88 }, { 0x01E89, true, de01E89 }, { 0x01E8A, true, de01E8A }, { 0x01E8B, true, de01E8B },
{ 0x01E8C, true, de01E8C }, { 0x01E8D, true, de01E8D }, { 0x01E8E, true, de01E8E }, { 0x01E8F, true, de01E8F },
{ 0x01E90, true, de01E90 }, { 0x01E91, true, de01E91 }, { 0x01E92, true, de01E92 }, { 0x01E93, true, de01E93 },
{ 0x01E94, true, de01E94 }, { 0x01E95, true, de01E95 }, { 0x01E96, true, de01E96 }, { 0x01E97, true, de01E97 },
{ 0x01E98, true, de01E98 }, { 0x01E99, true, de01E99 }, { 0x01E9A, false, de01E9A }, { 0x01E9B, true, de01E9B },
{ 0x01EA0, true, de01EA0 }, { 0x01EA1, true, de01EA1 }, { 0x01EA2, true, de01EA2 }, { 0x01EA3, true, de01EA3 },
{ 0x01EA4, true, de01EA4 }, { 0x01EA5, true, de01EA5 }, { 0x01EA6, true, de01EA6 }, { 0x01EA7, true, de01EA7 },
{ 0x01EA8, true, de01EA8 }, { 0x01EA9, true, de01EA9 }, { 0x01EAA, true, de01EAA }, { 0x01EAB, true, de01EAB },
{ 0x01EAC, true, de01EAC }, { 0x01EAD, true, de01EAD }, { 0x01EAE, true, de01EAE }, { 0x01EAF, true, de01EAF },
{ 0x01EB0, true, de01EB0 }, { 0x01EB1, true, de01EB1 }, { 0x01EB2, true, de01EB2 }, { 0x01EB3, true, de01EB3 },
{ 0x01EB4, true, de01EB4 }, { 0x01EB5, true, de01EB5 }, { 0x01EB6, true, de01EB6 }, { 0x01EB7, true, de01EB7 },
{ 0x01EB8, true, de01EB8 }, { 0x01EB9, true, de01EB9 }, { 0x01EBA, true, de01EBA }, { 0x01EBB, true, de01EBB },
{ 0x01EBC, true, de01EBC }, { 0x01EBD, true, de01EBD }, { 0x01EBE, true, de01EBE }, { 0x01EBF, true, de01EBF },
{ 0x01EC0, true, de01EC0 }, { 0x01EC1, true, de01EC1 }, { 0x01EC2, true, de01EC2 }, { 0x01EC3, true, de01EC3 },
{ 0x01EC4, true, de01EC4 }, { 0x01EC5, true, de01EC5 }, { 0x01EC6, true, de01EC6 }, { 0x01EC7, true, de01EC7 },
{ 0x01EC8, true, de01EC8 }, { 0x01EC9, true, de01EC9 }, { 0x01ECA, true, de01ECA }, { 0x01ECB, true, de01ECB },
{ 0x01ECC, true, de01ECC }, { 0x01ECD, true, de01ECD }, { 0x01ECE, true, de01ECE }, { 0x01ECF, true, de01ECF },
{ 0x01ED0, true, de01ED0 }, { 0x01ED1, true, de01ED1 }, { 0x01ED2, true, de01ED2 }, { 0x01ED3, true, de01ED3 },
{ 0x01ED4, true, de01ED4 }, { 0x01ED5, true, de01ED5 }, { 0x01ED6, true, de01ED6 }, { 0x01ED7, true, de01ED7 },
{ 0x01ED8, true, de01ED8 }, { 0x01ED9, true, de01ED9 }, { 0x01EDA, true, de01EDA }, { 0x01EDB, true, de01EDB },
{ 0x01EDC, true, de01EDC }, { 0x01EDD, true, de01EDD }, { 0x01EDE, true, de01EDE }, { 0x01EDF, true, de01EDF },
{ 0x01EE0, true, de01EE0 }, { 0x01EE1, true, de01EE1 }, { 0x01EE2, true, de01EE2 }, { 0x01EE3, true, de01EE3 },
{ 0x01EE4, true, de01EE4 }, { 0x01EE5, true, de01EE5 }, { 0x01EE6, true, de01EE6 }, { 0x01EE7, true, de01EE7 },
{ 0x01EE8, true, de01EE8 }, { 0x01EE9, true, de01EE9 }, { 0x01EEA, true, de01EEA }, { 0x01EEB, true, de01EEB },
{ 0x01EEC, true, de01EEC }, { 0x01EED, true, de01EED }, { 0x01EEE, true, de01EEE }, { 0x01EEF, true, de01EEF },
{ 0x01EF0, true, de01EF0 }, { 0x01EF1, true, de01EF1 }, { 0x01EF2, true, de01EF2 }, { 0x01EF3, true, de01EF3 },
{ 0x01EF4, true, de01EF4 }, { 0x01EF5, true, de01EF5 }, { 0x01EF6, true, de01EF6 }, { 0x01EF7, true, de01EF7 },
{ 0x01EF8, true, de01EF8 }, { 0x01EF9, true, de01EF9 }, { 0x01F00, true, de01F00 }, { 0x01F01, true, de01F01 },
{ 0x01F02, true, de01F02 }, { 0x01F03, true, de01F03 }, { 0x01F04, true, de01F04 }, { 0x01F05, true, de01F05 },
{ 0x01F06, true, de01F06 }, { 0x01F07, true, de01F07 }, { 0x01F08, true, de01F08 }, { 0x01F09, true, de01F09 },
{ 0x01F0A, true, de01F0A }, { 0x01F0B, true, de01F0B }, { 0x01F0C, true, de01F0C }, { 0x01F0D, true, de01F0D },
{ 0x01F0E, true, de01F0E }, { 0x01F0F, true, de01F0F }, { 0x01F10, true, de01F10 }, { 0x01F11, true, de01F11 },
{ 0x01F12, true, de01F12 }, { 0x01F13, true, de01F13 }, { 0x01F14, true, de01F14 }, { 0x01F15, true, de01F15 },
{ 0x01F18, true, de01F18 }, { 0x01F19, true, de01F19 }, { 0x01F1A, true, de01F1A }, { 0x01F1B, true, de01F1B },
{ 0x01F1C, true, de01F1C }, { 0x01F1D, true, de01F1D }, { 0x01F20, true, de01F20 }, { 0x01F21, true, de01F21 },
{ 0x01F22, true, de01F22 }, { 0x01F23, true, de01F23 }, { 0x01F24, true, de01F24 }, { 0x01F25, true, de01F25 },
{ 0x01F26, true, de01F26 }, { 0x01F27, true, de01F27 }, { 0x01F28, true, de01F28 }, { 0x01F29, true, de01F29 },
{ 0x01F2A, true, de01F2A }, { 0x01F2B, true, de01F2B }, { 0x01F2C, true, de01F2C }, { 0x01F2D, true, de01F2D },
{ 0x01F2E, true, de01F2E }, { 0x01F2F, true, de01F2F }, { 0x01F30, true, de01F30 }, { 0x01F31, true, de01F31 },
{ 0x01F32, true, de01F32 }, { 0x01F33, true, de01F33 }, { 0x01F34, true, de01F34 }, { 0x01F35, true, de01F35 },
{ 0x01F36, true, de01F36 }, { 0x01F37, true, de01F37 }, { 0x01F38, true, de01F38 }, { 0x01F39, true, de01F39 },
{ 0x01F3A, true, de01F3A }, { 0x01F3B, true, de01F3B }, { 0x01F3C, true, de01F3C }, { 0x01F3D, true, de01F3D },
{ 0x01F3E, true, de01F3E }, { 0x01F3F, true, de01F3F }, { 0x01F40, true, de01F40 }, { 0x01F41, true, de01F41 },
{ 0x01F42, true, de01F42 }, { 0x01F43, true, de01F43 }, { 0x01F44, true, de01F44 }, { 0x01F45, true, de01F45 },
{ 0x01F48, true, de01F48 }, { 0x01F49, true, de01F49 }, { 0x01F4A, true, de01F4A }, { 0x01F4B, true, de01F4B },
{ 0x01F4C, true, de01F4C }, { 0x01F4D, true, de01F4D }, { 0x01F50, true, de01F50 }, { 0x01F51, true, de01F51 },
{ 0x01F52, true, de01F52 }, { 0x01F53, true, de01F53 }, { 0x01F54, true, de01F54 }, { 0x01F55, true, de01F55 },
{ 0x01F56, true, de01F56 }, { 0x01F57, true, de01F57 }, { 0x01F59, true, de01F59 }, { 0x01F5B, true, de01F5B },
{ 0x01F5D, true, de01F5D }, { 0x01F5F, true, de01F5F }, { 0x01F60, true, de01F60 }, { 0x01F61, true, de01F61 },
{ 0x01F62, true, de01F62 }, { 0x01F63, true, de01F63 }, { 0x01F64, true, de01F64 }, { 0x01F65, true, de01F65 },
{ 0x01F66, true, de01F66 }, { 0x01F67, true, de01F67 }, { 0x01F68, true, de01F68 }, { 0x01F69, true, de01F69 },
{ 0x01F6A, true, de01F6A }, { 0x01F6B, true, de01F6B }, { 0x01F6C, true, de01F6C }, { 0x01F6D, true, de01F6D },
{ 0x01F6E, true, de01F6E }, { 0x01F6F, true, de01F6F }, { 0x01F70, true, de01F70 }, { 0x01F71, true, de01F71 },
{ 0x01F72, true, de01F72 }, { 0x01F73, true, de01F73 }, { 0x01F74, true, de01F74 }, { 0x01F75, true, de01F75 },
{ 0x01F76, true, de01F76 }, { 0x01F77, true, de01F77 }, { 0x01F78, true, de01F78 }, { 0x01F79, true, de01F79 },
{ 0x01F7A, true, de01F7A }, { 0x01F7B, true, de01F7B }, { 0x01F7C, true, de01F7C }, { 0x01F7D, true, de01F7D },
{ 0x01F80, true, de01F80 }, { 0x01F81, true, de01F81 }, { 0x01F82, true, de01F82 }, { 0x01F83, true, de01F83 },
{ 0x01F84, true, de01F84 }, { 0x01F85, true, de01F85 }, { 0x01F86, true, de01F86 }, { 0x01F87, true, de01F87 },
{ 0x01F88, true, de01F88 }, { 0x01F89, true, de01F89 }, { 0x01F8A, true, de01F8A }, { 0x01F8B, true, de01F8B },
{ 0x01F8C, true, de01F8C }, { 0x01F8D, true, de01F8D }, { 0x01F8E, true, de01F8E }, { 0x01F8F, true, de01F8F },
{ 0x01F90, true, de01F90 }, { 0x01F91, true, de01F91 }, { 0x01F92, true, de01F92 }, { 0x01F93, true, de01F93 },
{ 0x01F94, true, de01F94 }, { 0x01F95, true, de01F95 }, { 0x01F96, true, de01F96 }, { 0x01F97, true, de01F97 },
{ 0x01F98, true, de01F98 }, { 0x01F99, true, de01F99 }, { 0x01F9A, true, de01F9A }, { 0x01F9B, true, de01F9B },
{ 0x01F9C, true, de01F9C }, { 0x01F9D, true, de01F9D }, { 0x01F9E, true, de01F9E }, { 0x01F9F, true, de01F9F },
{ 0x01FA0, true, de01FA0 }, { 0x01FA1, true, de01FA1 }, { 0x01FA2, true, de01FA2 }, { 0x01FA3, true, de01FA3 },
{ 0x01FA4, true, de01FA4 }, { 0x01FA5, true, de01FA5 }, { 0x01FA6, true, de01FA6 }, { 0x01FA7, true, de01FA7 },
{ 0x01FA8, true, de01FA8 }, { 0x01FA9, true, de01FA9 }, { 0x01FAA, true, de01FAA }, { 0x01FAB, true, de01FAB },
{ 0x01FAC, true, de01FAC }, { 0x01FAD, true, de01FAD }, { 0x01FAE, true, de01FAE }, { 0x01FAF, true, de01FAF },
{ 0x01FB0, true, de01FB0 }, { 0x01FB1, true, de01FB1 }, { 0x01FB2, true, de01FB2 }, { 0x01FB3, true, de01FB3 },
{ 0x01FB4, true, de01FB4 }, { 0x01FB6, true, de01FB6 }, { 0x01FB7, true, de01FB7 }, { 0x01FB8, true, de01FB8 },
{ 0x01FB9, true, de01FB9 }, { 0x01FBA, true, de01FBA }, { 0x01FBB, true, de01FBB }, { 0x01FBC, true, de01FBC },
{ 0x01FBD, false, de01FBD }, { 0x01FBE, true, de01FBE }, { 0x01FBF, false, de01FBF }, { 0x01FC0, false, de01FC0 },
{ 0x01FC1, true, de01FC1 }, { 0x01FC2, true, de01FC2 }, { 0x01FC3, true, de01FC3 }, { 0x01FC4, true, de01FC4 },
{ 0x01FC6, true, de01FC6 }, { 0x01FC7, true, de01FC7 }, { 0x01FC8, true, de01FC8 }, { 0x01FC9, true, de01FC9 },
{ 0x01FCA, true, de01FCA }, { 0x01FCB, true, de01FCB }, { 0x01FCC, true, de01FCC }, { 0x01FCD, true, de01FCD },
{ 0x01FCE, true, de01FCE }, { 0x01FCF, true, de01FCF }, { 0x01FD0, true, de01FD0 }, { 0x01FD1, true, de01FD1 },
{ 0x01FD2, true, de01FD2 }, { 0x01FD3, true, de01FD3 }, { 0x01FD6, true, de01FD6 }, { 0x01FD7, true, de01FD7 },
{ 0x01FD8, true, de01FD8 }, { 0x01FD9, true, de01FD9 }, { 0x01FDA, true, de01FDA }, { 0x01FDB, true, de01FDB },
{ 0x01FDD, true, de01FDD }, { 0x01FDE, true, de01FDE }, { 0x01FDF, true, de01FDF }, { 0x01FE0, true, de01FE0 },
{ 0x01FE1, true, de01FE1 }, { 0x01FE2, true, de01FE2 }, { 0x01FE3, true, de01FE3 }, { 0x01FE4, true, de01FE4 },
{ 0x01FE5, true, de01FE5 }, { 0x01FE6, true, de01FE6 }, { 0x01FE7, true, de01FE7 }, { 0x01FE8, true, de01FE8 },
{ 0x01FE9, true, de01FE9 }, { 0x01FEA, true, de01FEA }, { 0x01FEB, true, de01FEB }, { 0x01FEC, true, de01FEC },
{ 0x01FED, true, de01FED }, { 0x01FEE, true, de01FEE }, { 0x01FEF, true, de01FEF }, { 0x01FF2, true, de01FF2 },
{ 0x01FF3, true, de01FF3 }, { 0x01FF4, true, de01FF4 }, { 0x01FF6, true, de01FF6 }, { 0x01FF7, true, de01FF7 },
{ 0x01FF8, true, de01FF8 }, { 0x01FF9, true, de01FF9 }, { 0x01FFA, true, de01FFA }, { 0x01FFB, true, de01FFB },
{ 0x01FFC, true, de01FFC }, { 0x01FFD, true, de01FFD }, { 0x01FFE, false, de01FFE }, { 0x02000, true, de02000 },
{ 0x02001, true, de02001 }, { 0x02002, false, de02002 }, { 0x02003, false, de02003 }, { 0x02004, false, de02004 },
{ 0x02005, false, de02005 }, { 0x02006, false, de02006 }, { 0x02007, false, de02007 }, { 0x02008, false, de02008 },
{ 0x02009, false, de02009 }, { 0x0200A, false, de0200A }, { 0x02011, false, de02011 }, { 0x02017, false, de02017 },
{ 0x02024, false, de02024 }, { 0x02025, false, de02025 }, { 0x02026, false, de02026 }, { 0x0202F, false, de0202F },
{ 0x02033, false, de02033 }, { 0x02034, false, de02034 }, { 0x02036, false, de02036 }, { 0x02037, false, de02037 },
{ 0x0203C, false, de0203C }, { 0x0203E, false, de0203E }, { 0x02047, false, de02047 }, { 0x02048, false, de02048 },
{ 0x02049, false, de02049 }, { 0x02057, false, de02057 }, { 0x0205F, false, de0205F }, { 0x02070, false, de02070 },
{ 0x02071, false, de02071 }, { 0x02074, false, de02074 }, { 0x02075, false, de02075 }, { 0x02076, false, de02076 },
{ 0x02077, false, de02077 }, { 0x02078, false, de02078 }, { 0x02079, false, de02079 }, { 0x0207A, false, de0207A },
{ 0x0207B, false, de0207B }, { 0x0207C, false, de0207C }, { 0x0207D, false, de0207D }, { 0x0207E, false, de0207E },
{ 0x0207F, false, de0207F }, { 0x02080, false, de02080 }, { 0x02081, false, de02081 }, { 0x02082, false, de02082 },
{ 0x02083, false, de02083 }, { 0x02084, false, de02084 }, { 0x02085, false, de02085 }, { 0x02086, false, de02086 },
{ 0x02087, false, de02087 }, { 0x02088, false, de02088 }, { 0x02089, false, de02089 }, { 0x0208A, false, de0208A },
{ 0x0208B, false, de0208B }, { 0x0208C, false, de0208C }, { 0x0208D, false, de0208D }, { 0x0208E, false, de0208E },
{ 0x02090, false, de02090 }, { 0x02091, false, de02091 }, { 0x02092, false, de02092 }, { 0x02093, false, de02093 },
{ 0x02094, false, de02094 }, { 0x020A8, false, de020A8 }, { 0x02100, false, de02100 }, { 0x02101, false, de02101 },
{ 0x02102, false, de02102 }, { 0x02103, false, de02103 }, { 0x02105, false, de02105 }, { 0x02106, false, de02106 },
{ 0x02107, false, de02107 }, { 0x02109, false, de02109 }, { 0x0210A, false, de0210A }, { 0x0210B, false, de0210B },
{ 0x0210C, false, de0210C }, { 0x0210D, false, de0210D }, { 0x0210E, false, de0210E }, { 0x0210F, false, de0210F },
{ 0x02110, false, de02110 }, { 0x02111, false, de02111 }, { 0x02112, false, de02112 }, { 0x02113, false, de02113 },
{ 0x02115, false, de02115 }, { 0x02116, false, de02116 }, { 0x02119, false, de02119 }, { 0x0211A, false, de0211A },
{ 0x0211B, false, de0211B }, { 0x0211C, false, de0211C }, { 0x0211D, false, de0211D }, { 0x02120, false, de02120 },
{ 0x02121, false, de02121 }, { 0x02122, false, de02122 }, { 0x02124, false, de02124 }, { 0x02126, true, de02126 },
{ 0x02128, false, de02128 }, { 0x0212A, true, de0212A }, { 0x0212B, true, de0212B }, { 0x0212C, false, de0212C },
{ 0x0212D, false, de0212D }, { 0x0212F, false, de0212F }, { 0x02130, false, de02130 }, { 0x02131, false, de02131 },
{ 0x02133, false, de02133 }, { 0x02134, false, de02134 }, { 0x02135, false, de02135 }, { 0x02136, false, de02136 },
{ 0x02137, false, de02137 }, { 0x02138, false, de02138 }, { 0x02139, false, de02139 }, { 0x0213B, false, de0213B },
{ 0x0213C, false, de0213C }, { 0x0213D, false, de0213D }, { 0x0213E, false, de0213E }, { 0x0213F, false, de0213F },
{ 0x02140, false, de02140 }, { 0x02145, false, de02145 }, { 0x02146, false, de02146 }, { 0x02147, false, de02147 },
{ 0x02148, false, de02148 }, { 0x02149, false, de02149 }, { 0x02153, false, de02153 }, { 0x02154, false, de02154 },
{ 0x02155, false, de02155 }, { 0x02156, false, de02156 }, { 0x02157, false, de02157 }, { 0x02158, false, de02158 },
{ 0x02159, false, de02159 }, { 0x0215A, false, de0215A }, { 0x0215B, false, de0215B }, { 0x0215C, false, de0215C },
{ 0x0215D, false, de0215D }, { 0x0215E, false, de0215E }, { 0x0215F, false, de0215F }, { 0x02160, false, de02160 },
{ 0x02161, false, de02161 }, { 0x02162, false, de02162 }, { 0x02163, false, de02163 }, { 0x02164, false, de02164 },
{ 0x02165, false, de02165 }, { 0x02166, false, de02166 }, { 0x02167, false, de02167 }, { 0x02168, false, de02168 },
{ 0x02169, false, de02169 }, { 0x0216A, false, de0216A }, { 0x0216B, false, de0216B }, { 0x0216C, false, de0216C },
{ 0x0216D, false, de0216D }, { 0x0216E, false, de0216E }, { 0x0216F, false, de0216F }, { 0x02170, false, de02170 },
{ 0x02171, false, de02171 }, { 0x02172, false, de02172 }, { 0x02173, false, de02173 }, { 0x02174, false, de02174 },
{ 0x02175, false, de02175 }, { 0x02176, false, de02176 }, { 0x02177, false, de02177 }, { 0x02178, false, de02178 },
{ 0x02179, false, de02179 }, { 0x0217A, false, de0217A }, { 0x0217B, false, de0217B }, { 0x0217C, false, de0217C },
{ 0x0217D, false, de0217D }, { 0x0217E, false, de0217E }, { 0x0217F, false, de0217F }, { 0x0219A, true, de0219A },
{ 0x0219B, true, de0219B }, { 0x021AE, true, de021AE }, { 0x021CD, true, de021CD }, { 0x021CE, true, de021CE },
{ 0x021CF, true, de021CF }, { 0x02204, true, de02204 }, { 0x02209, true, de02209 }, { 0x0220C, true, de0220C },
{ 0x02224, true, de02224 }, { 0x02226, true, de02226 }, { 0x0222C, false, de0222C }, { 0x0222D, false, de0222D },
{ 0x0222F, false, de0222F }, { 0x02230, false, de02230 }, { 0x02241, true, de02241 }, { 0x02244, true, de02244 },
{ 0x02247, true, de02247 }, { 0x02249, true, de02249 }, { 0x02260, true, de02260 }, { 0x02262, true, de02262 },
{ 0x0226D, true, de0226D }, { 0x0226E, true, de0226E }, { 0x0226F, true, de0226F }, { 0x02270, true, de02270 },
{ 0x02271, true, de02271 }, { 0x02274, true, de02274 }, { 0x02275, true, de02275 }, { 0x02278, true, de02278 },
{ 0x02279, true, de02279 }, { 0x02280, true, de02280 }, { 0x02281, true, de02281 }, { 0x02284, true, de02284 },
{ 0x02285, true, de02285 }, { 0x02288, true, de02288 }, { 0x02289, true, de02289 }, { 0x022AC, true, de022AC },
{ 0x022AD, true, de022AD }, { 0x022AE, true, de022AE }, { 0x022AF, true, de022AF }, { 0x022E0, true, de022E0 },
{ 0x022E1, true, de022E1 }, { 0x022E2, true, de022E2 }, { 0x022E3, true, de022E3 }, { 0x022EA, true, de022EA },
{ 0x022EB, true, de022EB }, { 0x022EC, true, de022EC }, { 0x022ED, true, de022ED }, { 0x02329, true, de02329 },
{ 0x0232A, true, de0232A }, { 0x02460, false, de02460 }, { 0x02461, false, de02461 }, { 0x02462, false, de02462 },
{ 0x02463, false, de02463 }, { 0x02464, false, de02464 }, { 0x02465, false, de02465 }, { 0x02466, false, de02466 },
{ 0x02467, false, de02467 }, { 0x02468, false, de02468 }, { 0x02469, false, de02469 }, { 0x0246A, false, de0246A },
{ 0x0246B, false, de0246B }, { 0x0246C, false, de0246C }, { 0x0246D, false, de0246D }, { 0x0246E, false, de0246E },
{ 0x0246F, false, de0246F }, { 0x02470, false, de02470 }, { 0x02471, false, de02471 }, { 0x02472, false, de02472 },
{ 0x02473, false, de02473 }, { 0x02474, false, de02474 }, { 0x02475, false, de02475 }, { 0x02476, false, de02476 },
{ 0x02477, false, de02477 }, { 0x02478, false, de02478 }, { 0x02479, false, de02479 }, { 0x0247A, false, de0247A },
{ 0x0247B, false, de0247B }, { 0x0247C, false, de0247C }, { 0x0247D, false, de0247D }, { 0x0247E, false, de0247E },
{ 0x0247F, false, de0247F }, { 0x02480, false, de02480 }, { 0x02481, false, de02481 }, { 0x02482, false, de02482 },
{ 0x02483, false, de02483 }, { 0x02484, false, de02484 }, { 0x02485, false, de02485 }, { 0x02486, false, de02486 },
{ 0x02487, false, de02487 }, { 0x02488, false, de02488 }, { 0x02489, false, de02489 }, { 0x0248A, false, de0248A },
{ 0x0248B, false, de0248B }, { 0x0248C, false, de0248C }, { 0x0248D, false, de0248D }, { 0x0248E, false, de0248E },
{ 0x0248F, false, de0248F }, { 0x02490, false, de02490 }, { 0x02491, false, de02491 }, { 0x02492, false, de02492 },
{ 0x02493, false, de02493 }, { 0x02494, false, de02494 }, { 0x02495, false, de02495 }, { 0x02496, false, de02496 },
{ 0x02497, false, de02497 }, { 0x02498, false, de02498 }, { 0x02499, false, de02499 }, { 0x0249A, false, de0249A },
{ 0x0249B, false, de0249B }, { 0x0249C, false, de0249C }, { 0x0249D, false, de0249D }, { 0x0249E, false, de0249E },
{ 0x0249F, false, de0249F }, { 0x024A0, false, de024A0 }, { 0x024A1, false, de024A1 }, { 0x024A2, false, de024A2 },
{ 0x024A3, false, de024A3 }, { 0x024A4, false, de024A4 }, { 0x024A5, false, de024A5 }, { 0x024A6, false, de024A6 },
{ 0x024A7, false, de024A7 }, { 0x024A8, false, de024A8 }, { 0x024A9, false, de024A9 }, { 0x024AA, false, de024AA },
{ 0x024AB, false, de024AB }, { 0x024AC, false, de024AC }, { 0x024AD, false, de024AD }, { 0x024AE, false, de024AE },
{ 0x024AF, false, de024AF }, { 0x024B0, false, de024B0 }, { 0x024B1, false, de024B1 }, { 0x024B2, false, de024B2 },
{ 0x024B3, false, de024B3 }, { 0x024B4, false, de024B4 }, { 0x024B5, false, de024B5 }, { 0x024B6, false, de024B6 },
{ 0x024B7, false, de024B7 }, { 0x024B8, false, de024B8 }, { 0x024B9, false, de024B9 }, { 0x024BA, false, de024BA },
{ 0x024BB, false, de024BB }, { 0x024BC, false, de024BC }, { 0x024BD, false, de024BD }, { 0x024BE, false, de024BE },
{ 0x024BF, false, de024BF }, { 0x024C0, false, de024C0 }, { 0x024C1, false, de024C1 }, { 0x024C2, false, de024C2 },
{ 0x024C3, false, de024C3 }, { 0x024C4, false, de024C4 }, { 0x024C5, false, de024C5 }, { 0x024C6, false, de024C6 },
{ 0x024C7, false, de024C7 }, { 0x024C8, false, de024C8 }, { 0x024C9, false, de024C9 }, { 0x024CA, false, de024CA },
{ 0x024CB, false, de024CB }, { 0x024CC, false, de024CC }, { 0x024CD, false, de024CD }, { 0x024CE, false, de024CE },
{ 0x024CF, false, de024CF }, { 0x024D0, false, de024D0 }, { 0x024D1, false, de024D1 }, { 0x024D2, false, de024D2 },
{ 0x024D3, false, de024D3 }, { 0x024D4, false, de024D4 }, { 0x024D5, false, de024D5 }, { 0x024D6, false, de024D6 },
{ 0x024D7, false, de024D7 }, { 0x024D8, false, de024D8 }, { 0x024D9, false, de024D9 }, { 0x024DA, false, de024DA },
{ 0x024DB, false, de024DB }, { 0x024DC, false, de024DC }, { 0x024DD, false, de024DD }, { 0x024DE, false, de024DE },
{ 0x024DF, false, de024DF }, { 0x024E0, false, de024E0 }, { 0x024E1, false, de024E1 }, { 0x024E2, false, de024E2 },
{ 0x024E3, false, de024E3 }, { 0x024E4, false, de024E4 }, { 0x024E5, false, de024E5 }, { 0x024E6, false, de024E6 },
{ 0x024E7, false, de024E7 }, { 0x024E8, false, de024E8 }, { 0x024E9, false, de024E9 }, { 0x024EA, false, de024EA },
{ 0x02A0C, false, de02A0C }, { 0x02A74, false, de02A74 }, { 0x02A75, false, de02A75 }, { 0x02A76, false, de02A76 },
{ 0x02ADC, true, de02ADC }, { 0x02D6F, false, de02D6F }, { 0x02E9F, false, de02E9F }, { 0x02EF3, false, de02EF3 },
{ 0x02F00, false, de02F00 }, { 0x02F01, false, de02F01 }, { 0x02F02, false, de02F02 }, { 0x02F03, false, de02F03 },
{ 0x02F04, false, de02F04 }, { 0x02F05, false, de02F05 }, { 0x02F06, false, de02F06 }, { 0x02F07, false, de02F07 },
{ 0x02F08, false, de02F08 }, { 0x02F09, false, de02F09 }, { 0x02F0A, false, de02F0A }, { 0x02F0B, false, de02F0B },
{ 0x02F0C, false, de02F0C }, { 0x02F0D, false, de02F0D }, { 0x02F0E, false, de02F0E }, { 0x02F0F, false, de02F0F },
{ 0x02F10, false, de02F10 }, { 0x02F11, false, de02F11 }, { 0x02F12, false, de02F12 }, { 0x02F13, false, de02F13 },
{ 0x02F14, false, de02F14 }, { 0x02F15, false, de02F15 }, { 0x02F16, false, de02F16 }, { 0x02F17, false, de02F17 },
{ 0x02F18, false, de02F18 }, { 0x02F19, false, de02F19 }, { 0x02F1A, false, de02F1A }, { 0x02F1B, false, de02F1B },
{ 0x02F1C, false, de02F1C }, { 0x02F1D, false, de02F1D }, { 0x02F1E, false, de02F1E }, { 0x02F1F, false, de02F1F },
{ 0x02F20, false, de02F20 }, { 0x02F21, false, de02F21 }, { 0x02F22, false, de02F22 }, { 0x02F23, false, de02F23 },
{ 0x02F24, false, de02F24 }, { 0x02F25, false, de02F25 }, { 0x02F26, false, de02F26 }, { 0x02F27, false, de02F27 },
{ 0x02F28, false, de02F28 }, { 0x02F29, false, de02F29 }, { 0x02F2A, false, de02F2A }, { 0x02F2B, false, de02F2B },
{ 0x02F2C, false, de02F2C }, { 0x02F2D, false, de02F2D }, { 0x02F2E, false, de02F2E }, { 0x02F2F, false, de02F2F },
{ 0x02F30, false, de02F30 }, { 0x02F31, false, de02F31 }, { 0x02F32, false, de02F32 }, { 0x02F33, false, de02F33 },
{ 0x02F34, false, de02F34 }, { 0x02F35, false, de02F35 }, { 0x02F36, false, de02F36 }, { 0x02F37, false, de02F37 },
{ 0x02F38, false, de02F38 }, { 0x02F39, false, de02F39 }, { 0x02F3A, false, de02F3A }, { 0x02F3B, false, de02F3B },
{ 0x02F3C, false, de02F3C }, { 0x02F3D, false, de02F3D }, { 0x02F3E, false, de02F3E }, { 0x02F3F, false, de02F3F },
{ 0x02F40, false, de02F40 }, { 0x02F41, false, de02F41 }, { 0x02F42, false, de02F42 }, { 0x02F43, false, de02F43 },
{ 0x02F44, false, de02F44 }, { 0x02F45, false, de02F45 }, { 0x02F46, false, de02F46 }, { 0x02F47, false, de02F47 },
{ 0x02F48, false, de02F48 }, { 0x02F49, false, de02F49 }, { 0x02F4A, false, de02F4A }, { 0x02F4B, false, de02F4B },
{ 0x02F4C, false, de02F4C }, { 0x02F4D, false, de02F4D }, { 0x02F4E, false, de02F4E }, { 0x02F4F, false, de02F4F },
{ 0x02F50, false, de02F50 }, { 0x02F51, false, de02F51 }, { 0x02F52, false, de02F52 }, { 0x02F53, false, de02F53 },
{ 0x02F54, false, de02F54 }, { 0x02F55, false, de02F55 }, { 0x02F56, false, de02F56 }, { 0x02F57, false, de02F57 },
{ 0x02F58, false, de02F58 }, { 0x02F59, false, de02F59 }, { 0x02F5A, false, de02F5A }, { 0x02F5B, false, de02F5B },
{ 0x02F5C, false, de02F5C }, { 0x02F5D, false, de02F5D }, { 0x02F5E, false, de02F5E }, { 0x02F5F, false, de02F5F },
{ 0x02F60, false, de02F60 }, { 0x02F61, false, de02F61 }, { 0x02F62, false, de02F62 }, { 0x02F63, false, de02F63 },
{ 0x02F64, false, de02F64 }, { 0x02F65, false, de02F65 }, { 0x02F66, false, de02F66 }, { 0x02F67, false, de02F67 },
{ 0x02F68, false, de02F68 }, { 0x02F69, false, de02F69 }, { 0x02F6A, false, de02F6A }, { 0x02F6B, false, de02F6B },
{ 0x02F6C, false, de02F6C }, { 0x02F6D, false, de02F6D }, { 0x02F6E, false, de02F6E }, { 0x02F6F, false, de02F6F },
{ 0x02F70, false, de02F70 }, { 0x02F71, false, de02F71 }, { 0x02F72, false, de02F72 }, { 0x02F73, false, de02F73 },
{ 0x02F74, false, de02F74 }, { 0x02F75, false, de02F75 }, { 0x02F76, false, de02F76 }, { 0x02F77, false, de02F77 },
{ 0x02F78, false, de02F78 }, { 0x02F79, false, de02F79 }, { 0x02F7A, false, de02F7A }, { 0x02F7B, false, de02F7B },
{ 0x02F7C, false, de02F7C }, { 0x02F7D, false, de02F7D }, { 0x02F7E, false, de02F7E }, { 0x02F7F, false, de02F7F },
{ 0x02F80, false, de02F80 }, { 0x02F81, false, de02F81 }, { 0x02F82, false, de02F82 }, { 0x02F83, false, de02F83 },
{ 0x02F84, false, de02F84 }, { 0x02F85, false, de02F85 }, { 0x02F86, false, de02F86 }, { 0x02F87, false, de02F87 },
{ 0x02F88, false, de02F88 }, { 0x02F89, false, de02F89 }, { 0x02F8A, false, de02F8A }, { 0x02F8B, false, de02F8B },
{ 0x02F8C, false, de02F8C }, { 0x02F8D, false, de02F8D }, { 0x02F8E, false, de02F8E }, { 0x02F8F, false, de02F8F },
{ 0x02F90, false, de02F90 }, { 0x02F91, false, de02F91 }, { 0x02F92, false, de02F92 }, { 0x02F93, false, de02F93 },
{ 0x02F94, false, de02F94 }, { 0x02F95, false, de02F95 }, { 0x02F96, false, de02F96 }, { 0x02F97, false, de02F97 },
{ 0x02F98, false, de02F98 }, { 0x02F99, false, de02F99 }, { 0x02F9A, false, de02F9A }, { 0x02F9B, false, de02F9B },
{ 0x02F9C, false, de02F9C }, { 0x02F9D, false, de02F9D }, { 0x02F9E, false, de02F9E }, { 0x02F9F, false, de02F9F },
{ 0x02FA0, false, de02FA0 }, { 0x02FA1, false, de02FA1 }, { 0x02FA2, false, de02FA2 }, { 0x02FA3, false, de02FA3 },
{ 0x02FA4, false, de02FA4 }, { 0x02FA5, false, de02FA5 }, { 0x02FA6, false, de02FA6 }, { 0x02FA7, false, de02FA7 },
{ 0x02FA8, false, de02FA8 }, { 0x02FA9, false, de02FA9 }, { 0x02FAA, false, de02FAA }, { 0x02FAB, false, de02FAB },
{ 0x02FAC, false, de02FAC }, { 0x02FAD, false, de02FAD }, { 0x02FAE, false, de02FAE }, { 0x02FAF, false, de02FAF },
{ 0x02FB0, false, de02FB0 }, { 0x02FB1, false, de02FB1 }, { 0x02FB2, false, de02FB2 }, { 0x02FB3, false, de02FB3 },
{ 0x02FB4, false, de02FB4 }, { 0x02FB5, false, de02FB5 }, { 0x02FB6, false, de02FB6 }, { 0x02FB7, false, de02FB7 },
{ 0x02FB8, false, de02FB8 }, { 0x02FB9, false, de02FB9 }, { 0x02FBA, false, de02FBA }, { 0x02FBB, false, de02FBB },
{ 0x02FBC, false, de02FBC }, { 0x02FBD, false, de02FBD }, { 0x02FBE, false, de02FBE }, { 0x02FBF, false, de02FBF },
{ 0x02FC0, false, de02FC0 }, { 0x02FC1, false, de02FC1 }, { 0x02FC2, false, de02FC2 }, { 0x02FC3, false, de02FC3 },
{ 0x02FC4, false, de02FC4 }, { 0x02FC5, false, de02FC5 }, { 0x02FC6, false, de02FC6 }, { 0x02FC7, false, de02FC7 },
{ 0x02FC8, false, de02FC8 }, { 0x02FC9, false, de02FC9 }, { 0x02FCA, false, de02FCA }, { 0x02FCB, false, de02FCB },
{ 0x02FCC, false, de02FCC }, { 0x02FCD, false, de02FCD }, { 0x02FCE, false, de02FCE }, { 0x02FCF, false, de02FCF },
{ 0x02FD0, false, de02FD0 }, { 0x02FD1, false, de02FD1 }, { 0x02FD2, false, de02FD2 }, { 0x02FD3, false, de02FD3 },
{ 0x02FD4, false, de02FD4 }, { 0x02FD5, false, de02FD5 }, { 0x03000, false, de03000 }, { 0x03036, false, de03036 },
{ 0x03038, false, de03038 }, { 0x03039, false, de03039 }, { 0x0303A, false, de0303A }, { 0x0304C, true, de0304C },
{ 0x0304E, true, de0304E }, { 0x03050, true, de03050 }, { 0x03052, true, de03052 }, { 0x03054, true, de03054 },
{ 0x03056, true, de03056 }, { 0x03058, true, de03058 }, { 0x0305A, true, de0305A }, { 0x0305C, true, de0305C },
{ 0x0305E, true, de0305E }, { 0x03060, true, de03060 }, { 0x03062, true, de03062 }, { 0x03065, true, de03065 },
{ 0x03067, true, de03067 }, { 0x03069, true, de03069 }, { 0x03070, true, de03070 }, { 0x03071, true, de03071 },
{ 0x03073, true, de03073 }, { 0x03074, true, de03074 }, { 0x03076, true, de03076 }, { 0x03077, true, de03077 },
{ 0x03079, true, de03079 }, { 0x0307A, true, de0307A }, { 0x0307C, true, de0307C }, { 0x0307D, true, de0307D },
{ 0x03094, true, de03094 }, { 0x0309B, false, de0309B }, { 0x0309C, false, de0309C }, { 0x0309E, true, de0309E },
{ 0x0309F, false, de0309F }, { 0x030AC, true, de030AC }, { 0x030AE, true, de030AE }, { 0x030B0, true, de030B0 },
{ 0x030B2, true, de030B2 }, { 0x030B4, true, de030B4 }, { 0x030B6, true, de030B6 }, { 0x030B8, true, de030B8 },
{ 0x030BA, true, de030BA }, { 0x030BC, true, de030BC }, { 0x030BE, true, de030BE }, { 0x030C0, true, de030C0 },
{ 0x030C2, true, de030C2 }, { 0x030C5, true, de030C5 }, { 0x030C7, true, de030C7 }, { 0x030C9, true, de030C9 },
{ 0x030D0, true, de030D0 }, { 0x030D1, true, de030D1 }, { 0x030D3, true, de030D3 }, { 0x030D4, true, de030D4 },
{ 0x030D6, true, de030D6 }, { 0x030D7, true, de030D7 }, { 0x030D9, true, de030D9 }, { 0x030DA, true, de030DA },
{ 0x030DC, true, de030DC }, { 0x030DD, true, de030DD }, { 0x030F4, true, de030F4 }, { 0x030F7, true, de030F7 },
{ 0x030F8, true, de030F8 }, { 0x030F9, true, de030F9 }, { 0x030FA, true, de030FA }, { 0x030FE, true, de030FE },
{ 0x030FF, false, de030FF }, { 0x03131, false, de03131 }, { 0x03132, false, de03132 }, { 0x03133, false, de03133 },
{ 0x03134, false, de03134 }, { 0x03135, false, de03135 }, { 0x03136, false, de03136 }, { 0x03137, false, de03137 },
{ 0x03138, false, de03138 }, { 0x03139, false, de03139 }, { 0x0313A, false, de0313A }, { 0x0313B, false, de0313B },
{ 0x0313C, false, de0313C }, { 0x0313D, false, de0313D }, { 0x0313E, false, de0313E }, { 0x0313F, false, de0313F },
{ 0x03140, false, de03140 }, { 0x03141, false, de03141 }, { 0x03142, false, de03142 }, { 0x03143, false, de03143 },
{ 0x03144, false, de03144 }, { 0x03145, false, de03145 }, { 0x03146, false, de03146 }, { 0x03147, false, de03147 },
{ 0x03148, false, de03148 }, { 0x03149, false, de03149 }, { 0x0314A, false, de0314A }, { 0x0314B, false, de0314B },
{ 0x0314C, false, de0314C }, { 0x0314D, false, de0314D }, { 0x0314E, false, de0314E }, { 0x0314F, false, de0314F },
{ 0x03150, false, de03150 }, { 0x03151, false, de03151 }, { 0x03152, false, de03152 }, { 0x03153, false, de03153 },
{ 0x03154, false, de03154 }, { 0x03155, false, de03155 }, { 0x03156, false, de03156 }, { 0x03157, false, de03157 },
{ 0x03158, false, de03158 }, { 0x03159, false, de03159 }, { 0x0315A, false, de0315A }, { 0x0315B, false, de0315B },
{ 0x0315C, false, de0315C }, { 0x0315D, false, de0315D }, { 0x0315E, false, de0315E }, { 0x0315F, false, de0315F },
{ 0x03160, false, de03160 }, { 0x03161, false, de03161 }, { 0x03162, false, de03162 }, { 0x03163, false, de03163 },
{ 0x03164, false, de03164 }, { 0x03165, false, de03165 }, { 0x03166, false, de03166 }, { 0x03167, false, de03167 },
{ 0x03168, false, de03168 }, { 0x03169, false, de03169 }, { 0x0316A, false, de0316A }, { 0x0316B, false, de0316B },
{ 0x0316C, false, de0316C }, { 0x0316D, false, de0316D }, { 0x0316E, false, de0316E }, { 0x0316F, false, de0316F },
{ 0x03170, false, de03170 }, { 0x03171, false, de03171 }, { 0x03172, false, de03172 }, { 0x03173, false, de03173 },
{ 0x03174, false, de03174 }, { 0x03175, false, de03175 }, { 0x03176, false, de03176 }, { 0x03177, false, de03177 },
{ 0x03178, false, de03178 }, { 0x03179, false, de03179 }, { 0x0317A, false, de0317A }, { 0x0317B, false, de0317B },
{ 0x0317C, false, de0317C }, { 0x0317D, false, de0317D }, { 0x0317E, false, de0317E }, { 0x0317F, false, de0317F },
{ 0x03180, false, de03180 }, { 0x03181, false, de03181 }, { 0x03182, false, de03182 }, { 0x03183, false, de03183 },
{ 0x03184, false, de03184 }, { 0x03185, false, de03185 }, { 0x03186, false, de03186 }, { 0x03187, false, de03187 },
{ 0x03188, false, de03188 }, { 0x03189, false, de03189 }, { 0x0318A, false, de0318A }, { 0x0318B, false, de0318B },
{ 0x0318C, false, de0318C }, { 0x0318D, false, de0318D }, { 0x0318E, false, de0318E }, { 0x03192, false, de03192 },
{ 0x03193, false, de03193 }, { 0x03194, false, de03194 }, { 0x03195, false, de03195 }, { 0x03196, false, de03196 },
{ 0x03197, false, de03197 }, { 0x03198, false, de03198 }, { 0x03199, false, de03199 }, { 0x0319A, false, de0319A },
{ 0x0319B, false, de0319B }, { 0x0319C, false, de0319C }, { 0x0319D, false, de0319D }, { 0x0319E, false, de0319E },
{ 0x0319F, false, de0319F }, { 0x03200, false, de03200 }, { 0x03201, false, de03201 }, { 0x03202, false, de03202 },
{ 0x03203, false, de03203 }, { 0x03204, false, de03204 }, { 0x03205, false, de03205 }, { 0x03206, false, de03206 },
{ 0x03207, false, de03207 }, { 0x03208, false, de03208 }, { 0x03209, false, de03209 }, { 0x0320A, false, de0320A },
{ 0x0320B, false, de0320B }, { 0x0320C, false, de0320C }, { 0x0320D, false, de0320D }, { 0x0320E, false, de0320E },
{ 0x0320F, false, de0320F }, { 0x03210, false, de03210 }, { 0x03211, false, de03211 }, { 0x03212, false, de03212 },
{ 0x03213, false, de03213 }, { 0x03214, false, de03214 }, { 0x03215, false, de03215 }, { 0x03216, false, de03216 },
{ 0x03217, false, de03217 }, { 0x03218, false, de03218 }, { 0x03219, false, de03219 }, { 0x0321A, false, de0321A },
{ 0x0321B, false, de0321B }, { 0x0321C, false, de0321C }, { 0x0321D, false, de0321D }, { 0x0321E, false, de0321E },
{ 0x03220, false, de03220 }, { 0x03221, false, de03221 }, { 0x03222, false, de03222 }, { 0x03223, false, de03223 },
{ 0x03224, false, de03224 }, { 0x03225, false, de03225 }, { 0x03226, false, de03226 }, { 0x03227, false, de03227 },
{ 0x03228, false, de03228 }, { 0x03229, false, de03229 }, { 0x0322A, false, de0322A }, { 0x0322B, false, de0322B },
{ 0x0322C, false, de0322C }, { 0x0322D, false, de0322D }, { 0x0322E, false, de0322E }, { 0x0322F, false, de0322F },
{ 0x03230, false, de03230 }, { 0x03231, false, de03231 }, { 0x03232, false, de03232 }, { 0x03233, false, de03233 },
{ 0x03234, false, de03234 }, { 0x03235, false, de03235 }, { 0x03236, false, de03236 }, { 0x03237, false, de03237 },
{ 0x03238, false, de03238 }, { 0x03239, false, de03239 }, { 0x0323A, false, de0323A }, { 0x0323B, false, de0323B },
{ 0x0323C, false, de0323C }, { 0x0323D, false, de0323D }, { 0x0323E, false, de0323E }, { 0x0323F, false, de0323F },
{ 0x03240, false, de03240 }, { 0x03241, false, de03241 }, { 0x03242, false, de03242 }, { 0x03243, false, de03243 },
{ 0x03250, false, de03250 }, { 0x03251, false, de03251 }, { 0x03252, false, de03252 }, { 0x03253, false, de03253 },
{ 0x03254, false, de03254 }, { 0x03255, false, de03255 }, { 0x03256, false, de03256 }, { 0x03257, false, de03257 },
{ 0x03258, false, de03258 }, { 0x03259, false, de03259 }, { 0x0325A, false, de0325A }, { 0x0325B, false, de0325B },
{ 0x0325C, false, de0325C }, { 0x0325D, false, de0325D }, { 0x0325E, false, de0325E }, { 0x0325F, false, de0325F },
{ 0x03260, false, de03260 }, { 0x03261, false, de03261 }, { 0x03262, false, de03262 }, { 0x03263, false, de03263 },
{ 0x03264, false, de03264 }, { 0x03265, false, de03265 }, { 0x03266, false, de03266 }, { 0x03267, false, de03267 },
{ 0x03268, false, de03268 }, { 0x03269, false, de03269 }, { 0x0326A, false, de0326A }, { 0x0326B, false, de0326B },
{ 0x0326C, false, de0326C }, { 0x0326D, false, de0326D }, { 0x0326E, false, de0326E }, { 0x0326F, false, de0326F },
{ 0x03270, false, de03270 }, { 0x03271, false, de03271 }, { 0x03272, false, de03272 }, { 0x03273, false, de03273 },
{ 0x03274, false, de03274 }, { 0x03275, false, de03275 }, { 0x03276, false, de03276 }, { 0x03277, false, de03277 },
{ 0x03278, false, de03278 }, { 0x03279, false, de03279 }, { 0x0327A, false, de0327A }, { 0x0327B, false, de0327B },
{ 0x0327C, false, de0327C }, { 0x0327D, false, de0327D }, { 0x0327E, false, de0327E }, { 0x03280, false, de03280 },
{ 0x03281, false, de03281 }, { 0x03282, false, de03282 }, { 0x03283, false, de03283 }, { 0x03284, false, de03284 },
{ 0x03285, false, de03285 }, { 0x03286, false, de03286 }, { 0x03287, false, de03287 }, { 0x03288, false, de03288 },
{ 0x03289, false, de03289 }, { 0x0328A, false, de0328A }, { 0x0328B, false, de0328B }, { 0x0328C, false, de0328C },
{ 0x0328D, false, de0328D }, { 0x0328E, false, de0328E }, { 0x0328F, false, de0328F }, { 0x03290, false, de03290 },
{ 0x03291, false, de03291 }, { 0x03292, false, de03292 }, { 0x03293, false, de03293 }, { 0x03294, false, de03294 },
{ 0x03295, false, de03295 }, { 0x03296, false, de03296 }, { 0x03297, false, de03297 }, { 0x03298, false, de03298 },
{ 0x03299, false, de03299 }, { 0x0329A, false, de0329A }, { 0x0329B, false, de0329B }, { 0x0329C, false, de0329C },
{ 0x0329D, false, de0329D }, { 0x0329E, false, de0329E }, { 0x0329F, false, de0329F }, { 0x032A0, false, de032A0 },
{ 0x032A1, false, de032A1 }, { 0x032A2, false, de032A2 }, { 0x032A3, false, de032A3 }, { 0x032A4, false, de032A4 },
{ 0x032A5, false, de032A5 }, { 0x032A6, false, de032A6 }, { 0x032A7, false, de032A7 }, { 0x032A8, false, de032A8 },
{ 0x032A9, false, de032A9 }, { 0x032AA, false, de032AA }, { 0x032AB, false, de032AB }, { 0x032AC, false, de032AC },
{ 0x032AD, false, de032AD }, { 0x032AE, false, de032AE }, { 0x032AF, false, de032AF }, { 0x032B0, false, de032B0 },
{ 0x032B1, false, de032B1 }, { 0x032B2, false, de032B2 }, { 0x032B3, false, de032B3 }, { 0x032B4, false, de032B4 },
{ 0x032B5, false, de032B5 }, { 0x032B6, false, de032B6 }, { 0x032B7, false, de032B7 }, { 0x032B8, false, de032B8 },
{ 0x032B9, false, de032B9 }, { 0x032BA, false, de032BA }, { 0x032BB, false, de032BB }, { 0x032BC, false, de032BC },
{ 0x032BD, false, de032BD }, { 0x032BE, false, de032BE }, { 0x032BF, false, de032BF }, { 0x032C0, false, de032C0 },
{ 0x032C1, false, de032C1 }, { 0x032C2, false, de032C2 }, { 0x032C3, false, de032C3 }, { 0x032C4, false, de032C4 },
{ 0x032C5, false, de032C5 }, { 0x032C6, false, de032C6 }, { 0x032C7, false, de032C7 }, { 0x032C8, false, de032C8 },
{ 0x032C9, false, de032C9 }, { 0x032CA, false, de032CA }, { 0x032CB, false, de032CB }, { 0x032CC, false, de032CC },
{ 0x032CD, false, de032CD }, { 0x032CE, false, de032CE }, { 0x032CF, false, de032CF }, { 0x032D0, false, de032D0 },
{ 0x032D1, false, de032D1 }, { 0x032D2, false, de032D2 }, { 0x032D3, false, de032D3 }, { 0x032D4, false, de032D4 },
{ 0x032D5, false, de032D5 }, { 0x032D6, false, de032D6 }, { 0x032D7, false, de032D7 }, { 0x032D8, false, de032D8 },
{ 0x032D9, false, de032D9 }, { 0x032DA, false, de032DA }, { 0x032DB, false, de032DB }, { 0x032DC, false, de032DC },
{ 0x032DD, false, de032DD }, { 0x032DE, false, de032DE }, { 0x032DF, false, de032DF }, { 0x032E0, false, de032E0 },
{ 0x032E1, false, de032E1 }, { 0x032E2, false, de032E2 }, { 0x032E3, false, de032E3 }, { 0x032E4, false, de032E4 },
{ 0x032E5, false, de032E5 }, { 0x032E6, false, de032E6 }, { 0x032E7, false, de032E7 }, { 0x032E8, false, de032E8 },
{ 0x032E9, false, de032E9 }, { 0x032EA, false, de032EA }, { 0x032EB, false, de032EB }, { 0x032EC, false, de032EC },
{ 0x032ED, false, de032ED }, { 0x032EE, false, de032EE }, { 0x032EF, false, de032EF }, { 0x032F0, false, de032F0 },
{ 0x032F1, false, de032F1 }, { 0x032F2, false, de032F2 }, { 0x032F3, false, de032F3 }, { 0x032F4, false, de032F4 },
{ 0x032F5, false, de032F5 }, { 0x032F6, false, de032F6 }, { 0x032F7, false, de032F7 }, { 0x032F8, false, de032F8 },
{ 0x032F9, false, de032F9 }, { 0x032FA, false, de032FA }, { 0x032FB, false, de032FB }, { 0x032FC, false, de032FC },
{ 0x032FD, false, de032FD }, { 0x032FE, false, de032FE }, { 0x03300, false, de03300 }, { 0x03301, false, de03301 },
{ 0x03302, false, de03302 }, { 0x03303, false, de03303 }, { 0x03304, false, de03304 }, { 0x03305, false, de03305 },
{ 0x03306, false, de03306 }, { 0x03307, false, de03307 }, { 0x03308, false, de03308 }, { 0x03309, false, de03309 },
{ 0x0330A, false, de0330A }, { 0x0330B, false, de0330B }, { 0x0330C, false, de0330C }, { 0x0330D, false, de0330D },
{ 0x0330E, false, de0330E }, { 0x0330F, false, de0330F }, { 0x03310, false, de03310 }, { 0x03311, false, de03311 },
{ 0x03312, false, de03312 }, { 0x03313, false, de03313 }, { 0x03314, false, de03314 }, { 0x03315, false, de03315 },
{ 0x03316, false, de03316 }, { 0x03317, false, de03317 }, { 0x03318, false, de03318 }, { 0x03319, false, de03319 },
{ 0x0331A, false, de0331A }, { 0x0331B, false, de0331B }, { 0x0331C, false, de0331C }, { 0x0331D, false, de0331D },
{ 0x0331E, false, de0331E }, { 0x0331F, false, de0331F }, { 0x03320, false, de03320 }, { 0x03321, false, de03321 },
{ 0x03322, false, de03322 }, { 0x03323, false, de03323 }, { 0x03324, false, de03324 }, { 0x03325, false, de03325 },
{ 0x03326, false, de03326 }, { 0x03327, false, de03327 }, { 0x03328, false, de03328 }, { 0x03329, false, de03329 },
{ 0x0332A, false, de0332A }, { 0x0332B, false, de0332B }, { 0x0332C, false, de0332C }, { 0x0332D, false, de0332D },
{ 0x0332E, false, de0332E }, { 0x0332F, false, de0332F }, { 0x03330, false, de03330 }, { 0x03331, false, de03331 },
{ 0x03332, false, de03332 }, { 0x03333, false, de03333 }, { 0x03334, false, de03334 }, { 0x03335, false, de03335 },
{ 0x03336, false, de03336 }, { 0x03337, false, de03337 }, { 0x03338, false, de03338 }, { 0x03339, false, de03339 },
{ 0x0333A, false, de0333A }, { 0x0333B, false, de0333B }, { 0x0333C, false, de0333C }, { 0x0333D, false, de0333D },
{ 0x0333E, false, de0333E }, { 0x0333F, false, de0333F }, { 0x03340, false, de03340 }, { 0x03341, false, de03341 },
{ 0x03342, false, de03342 }, { 0x03343, false, de03343 }, { 0x03344, false, de03344 }, { 0x03345, false, de03345 },
{ 0x03346, false, de03346 }, { 0x03347, false, de03347 }, { 0x03348, false, de03348 }, { 0x03349, false, de03349 },
{ 0x0334A, false, de0334A }, { 0x0334B, false, de0334B }, { 0x0334C, false, de0334C }, { 0x0334D, false, de0334D },
{ 0x0334E, false, de0334E }, { 0x0334F, false, de0334F }, { 0x03350, false, de03350 }, { 0x03351, false, de03351 },
{ 0x03352, false, de03352 }, { 0x03353, false, de03353 }, { 0x03354, false, de03354 }, { 0x03355, false, de03355 },
{ 0x03356, false, de03356 }, { 0x03357, false, de03357 }, { 0x03358, false, de03358 }, { 0x03359, false, de03359 },
{ 0x0335A, false, de0335A }, { 0x0335B, false, de0335B }, { 0x0335C, false, de0335C }, { 0x0335D, false, de0335D },
{ 0x0335E, false, de0335E }, { 0x0335F, false, de0335F }, { 0x03360, false, de03360 }, { 0x03361, false, de03361 },
{ 0x03362, false, de03362 }, { 0x03363, false, de03363 }, { 0x03364, false, de03364 }, { 0x03365, false, de03365 },
{ 0x03366, false, de03366 }, { 0x03367, false, de03367 }, { 0x03368, false, de03368 }, { 0x03369, false, de03369 },
{ 0x0336A, false, de0336A }, { 0x0336B, false, de0336B }, { 0x0336C, false, de0336C }, { 0x0336D, false, de0336D },
{ 0x0336E, false, de0336E }, { 0x0336F, false, de0336F }, { 0x03370, false, de03370 }, { 0x03371, false, de03371 },
{ 0x03372, false, de03372 }, { 0x03373, false, de03373 }, { 0x03374, false, de03374 }, { 0x03375, false, de03375 },
{ 0x03376, false, de03376 }, { 0x03377, false, de03377 }, { 0x03378, false, de03378 }, { 0x03379, false, de03379 },
{ 0x0337A, false, de0337A }, { 0x0337B, false, de0337B }, { 0x0337C, false, de0337C }, { 0x0337D, false, de0337D },
{ 0x0337E, false, de0337E }, { 0x0337F, false, de0337F }, { 0x03380, false, de03380 }, { 0x03381, false, de03381 },
{ 0x03382, false, de03382 }, { 0x03383, false, de03383 }, { 0x03384, false, de03384 }, { 0x03385, false, de03385 },
{ 0x03386, false, de03386 }, { 0x03387, false, de03387 }, { 0x03388, false, de03388 }, { 0x03389, false, de03389 },
{ 0x0338A, false, de0338A }, { 0x0338B, false, de0338B }, { 0x0338C, false, de0338C }, { 0x0338D, false, de0338D },
{ 0x0338E, false, de0338E }, { 0x0338F, false, de0338F }, { 0x03390, false, de03390 }, { 0x03391, false, de03391 },
{ 0x03392, false, de03392 }, { 0x03393, false, de03393 }, { 0x03394, false, de03394 }, { 0x03395, false, de03395 },
{ 0x03396, false, de03396 }, { 0x03397, false, de03397 }, { 0x03398, false, de03398 }, { 0x03399, false, de03399 },
{ 0x0339A, false, de0339A }, { 0x0339B, false, de0339B }, { 0x0339C, false, de0339C }, { 0x0339D, false, de0339D },
{ 0x0339E, false, de0339E }, { 0x0339F, false, de0339F }, { 0x033A0, false, de033A0 }, { 0x033A1, false, de033A1 },
{ 0x033A2, false, de033A2 }, { 0x033A3, false, de033A3 }, { 0x033A4, false, de033A4 }, { 0x033A5, false, de033A5 },
{ 0x033A6, false, de033A6 }, { 0x033A7, false, de033A7 }, { 0x033A8, false, de033A8 }, { 0x033A9, false, de033A9 },
{ 0x033AA, false, de033AA }, { 0x033AB, false, de033AB }, { 0x033AC, false, de033AC }, { 0x033AD, false, de033AD },
{ 0x033AE, false, de033AE }, { 0x033AF, false, de033AF }, { 0x033B0, false, de033B0 }, { 0x033B1, false, de033B1 },
{ 0x033B2, false, de033B2 }, { 0x033B3, false, de033B3 }, { 0x033B4, false, de033B4 }, { 0x033B5, false, de033B5 },
{ 0x033B6, false, de033B6 }, { 0x033B7, false, de033B7 }, { 0x033B8, false, de033B8 }, { 0x033B9, false, de033B9 },
{ 0x033BA, false, de033BA }, { 0x033BB, false, de033BB }, { 0x033BC, false, de033BC }, { 0x033BD, false, de033BD },
{ 0x033BE, false, de033BE }, { 0x033BF, false, de033BF }, { 0x033C0, false, de033C0 }, { 0x033C1, false, de033C1 },
{ 0x033C2, false, de033C2 }, { 0x033C3, false, de033C3 }, { 0x033C4, false, de033C4 }, { 0x033C5, false, de033C5 },
{ 0x033C6, false, de033C6 }, { 0x033C7, false, de033C7 }, { 0x033C8, false, de033C8 }, { 0x033C9, false, de033C9 },
{ 0x033CA, false, de033CA }, { 0x033CB, false, de033CB }, { 0x033CC, false, de033CC }, { 0x033CD, false, de033CD },
{ 0x033CE, false, de033CE }, { 0x033CF, false, de033CF }, { 0x033D0, false, de033D0 }, { 0x033D1, false, de033D1 },
{ 0x033D2, false, de033D2 }, { 0x033D3, false, de033D3 }, { 0x033D4, false, de033D4 }, { 0x033D5, false, de033D5 },
{ 0x033D6, false, de033D6 }, { 0x033D7, false, de033D7 }, { 0x033D8, false, de033D8 }, { 0x033D9, false, de033D9 },
{ 0x033DA, false, de033DA }, { 0x033DB, false, de033DB }, { 0x033DC, false, de033DC }, { 0x033DD, false, de033DD },
{ 0x033DE, false, de033DE }, { 0x033DF, false, de033DF }, { 0x033E0, false, de033E0 }, { 0x033E1, false, de033E1 },
{ 0x033E2, false, de033E2 }, { 0x033E3, false, de033E3 }, { 0x033E4, false, de033E4 }, { 0x033E5, false, de033E5 },
{ 0x033E6, false, de033E6 }, { 0x033E7, false, de033E7 }, { 0x033E8, false, de033E8 }, { 0x033E9, false, de033E9 },
{ 0x033EA, false, de033EA }, { 0x033EB, false, de033EB }, { 0x033EC, false, de033EC }, { 0x033ED, false, de033ED },
{ 0x033EE, false, de033EE }, { 0x033EF, false, de033EF }, { 0x033F0, false, de033F0 }, { 0x033F1, false, de033F1 },
{ 0x033F2, false, de033F2 }, { 0x033F3, false, de033F3 }, { 0x033F4, false, de033F4 }, { 0x033F5, false, de033F5 },
{ 0x033F6, false, de033F6 }, { 0x033F7, false, de033F7 }, { 0x033F8, false, de033F8 }, { 0x033F9, false, de033F9 },
{ 0x033FA, false, de033FA }, { 0x033FB, false, de033FB }, { 0x033FC, false, de033FC }, { 0x033FD, false, de033FD },
{ 0x033FE, false, de033FE }, { 0x033FF, false, de033FF }, { 0x0F900, true, de0F900 }, { 0x0F901, true, de0F901 },
{ 0x0F902, true, de0F902 }, { 0x0F903, true, de0F903 }, { 0x0F904, true, de0F904 }, { 0x0F905, true, de0F905 },
{ 0x0F906, true, de0F906 }, { 0x0F907, true, de0F907 }, { 0x0F908, true, de0F908 }, { 0x0F909, true, de0F909 },
{ 0x0F90A, true, de0F90A }, { 0x0F90B, true, de0F90B }, { 0x0F90C, true, de0F90C }, { 0x0F90D, true, de0F90D },
{ 0x0F90E, true, de0F90E }, { 0x0F90F, true, de0F90F }, { 0x0F910, true, de0F910 }, { 0x0F911, true, de0F911 },
{ 0x0F912, true, de0F912 }, { 0x0F913, true, de0F913 }, { 0x0F914, true, de0F914 }, { 0x0F915, true, de0F915 },
{ 0x0F916, true, de0F916 }, { 0x0F917, true, de0F917 }, { 0x0F918, true, de0F918 }, { 0x0F919, true, de0F919 },
{ 0x0F91A, true, de0F91A }, { 0x0F91B, true, de0F91B }, { 0x0F91C, true, de0F91C }, { 0x0F91D, true, de0F91D },
{ 0x0F91E, true, de0F91E }, { 0x0F91F, true, de0F91F }, { 0x0F920, true, de0F920 }, { 0x0F921, true, de0F921 },
{ 0x0F922, true, de0F922 }, { 0x0F923, true, de0F923 }, { 0x0F924, true, de0F924 }, { 0x0F925, true, de0F925 },
{ 0x0F926, true, de0F926 }, { 0x0F927, true, de0F927 }, { 0x0F928, true, de0F928 }, { 0x0F929, true, de0F929 },
{ 0x0F92A, true, de0F92A }, { 0x0F92B, true, de0F92B }, { 0x0F92C, true, de0F92C }, { 0x0F92D, true, de0F92D },
{ 0x0F92E, true, de0F92E }, { 0x0F92F, true, de0F92F }, { 0x0F930, true, de0F930 }, { 0x0F931, true, de0F931 },
{ 0x0F932, true, de0F932 }, { 0x0F933, true, de0F933 }, { 0x0F934, true, de0F934 }, { 0x0F935, true, de0F935 },
{ 0x0F936, true, de0F936 }, { 0x0F937, true, de0F937 }, { 0x0F938, true, de0F938 }, { 0x0F939, true, de0F939 },
{ 0x0F93A, true, de0F93A }, { 0x0F93B, true, de0F93B }, { 0x0F93C, true, de0F93C }, { 0x0F93D, true, de0F93D },
{ 0x0F93E, true, de0F93E }, { 0x0F93F, true, de0F93F }, { 0x0F940, true, de0F940 }, { 0x0F941, true, de0F941 },
{ 0x0F942, true, de0F942 }, { 0x0F943, true, de0F943 }, { 0x0F944, true, de0F944 }, { 0x0F945, true, de0F945 },
{ 0x0F946, true, de0F946 }, { 0x0F947, true, de0F947 }, { 0x0F948, true, de0F948 }, { 0x0F949, true, de0F949 },
{ 0x0F94A, true, de0F94A }, { 0x0F94B, true, de0F94B }, { 0x0F94C, true, de0F94C }, { 0x0F94D, true, de0F94D },
{ 0x0F94E, true, de0F94E }, { 0x0F94F, true, de0F94F }, { 0x0F950, true, de0F950 }, { 0x0F951, true, de0F951 },
{ 0x0F952, true, de0F952 }, { 0x0F953, true, de0F953 }, { 0x0F954, true, de0F954 }, { 0x0F955, true, de0F955 },
{ 0x0F956, true, de0F956 }, { 0x0F957, true, de0F957 }, { 0x0F958, true, de0F958 }, { 0x0F959, true, de0F959 },
{ 0x0F95A, true, de0F95A }, { 0x0F95B, true, de0F95B }, { 0x0F95C, true, de0F95C }, { 0x0F95D, true, de0F95D },
{ 0x0F95E, true, de0F95E }, { 0x0F95F, true, de0F95F }, { 0x0F960, true, de0F960 }, { 0x0F961, true, de0F961 },
{ 0x0F962, true, de0F962 }, { 0x0F963, true, de0F963 }, { 0x0F964, true, de0F964 }, { 0x0F965, true, de0F965 },
{ 0x0F966, true, de0F966 }, { 0x0F967, true, de0F967 }, { 0x0F968, true, de0F968 }, { 0x0F969, true, de0F969 },
{ 0x0F96A, true, de0F96A }, { 0x0F96B, true, de0F96B }, { 0x0F96C, true, de0F96C }, { 0x0F96D, true, de0F96D },
{ 0x0F96E, true, de0F96E }, { 0x0F96F, true, de0F96F }, { 0x0F970, true, de0F970 }, { 0x0F971, true, de0F971 },
{ 0x0F972, true, de0F972 }, { 0x0F973, true, de0F973 }, { 0x0F974, true, de0F974 }, { 0x0F975, true, de0F975 },
{ 0x0F976, true, de0F976 }, { 0x0F977, true, de0F977 }, { 0x0F978, true, de0F978 }, { 0x0F979, true, de0F979 },
{ 0x0F97A, true, de0F97A }, { 0x0F97B, true, de0F97B }, { 0x0F97C, true, de0F97C }, { 0x0F97D, true, de0F97D },
{ 0x0F97E, true, de0F97E }, { 0x0F97F, true, de0F97F }, { 0x0F980, true, de0F980 }, { 0x0F981, true, de0F981 },
{ 0x0F982, true, de0F982 }, { 0x0F983, true, de0F983 }, { 0x0F984, true, de0F984 }, { 0x0F985, true, de0F985 },
{ 0x0F986, true, de0F986 }, { 0x0F987, true, de0F987 }, { 0x0F988, true, de0F988 }, { 0x0F989, true, de0F989 },
{ 0x0F98A, true, de0F98A }, { 0x0F98B, true, de0F98B }, { 0x0F98C, true, de0F98C }, { 0x0F98D, true, de0F98D },
{ 0x0F98E, true, de0F98E }, { 0x0F98F, true, de0F98F }, { 0x0F990, true, de0F990 }, { 0x0F991, true, de0F991 },
{ 0x0F992, true, de0F992 }, { 0x0F993, true, de0F993 }, { 0x0F994, true, de0F994 }, { 0x0F995, true, de0F995 },
{ 0x0F996, true, de0F996 }, { 0x0F997, true, de0F997 }, { 0x0F998, true, de0F998 }, { 0x0F999, true, de0F999 },
{ 0x0F99A, true, de0F99A }, { 0x0F99B, true, de0F99B }, { 0x0F99C, true, de0F99C }, { 0x0F99D, true, de0F99D },
{ 0x0F99E, true, de0F99E }, { 0x0F99F, true, de0F99F }, { 0x0F9A0, true, de0F9A0 }, { 0x0F9A1, true, de0F9A1 },
{ 0x0F9A2, true, de0F9A2 }, { 0x0F9A3, true, de0F9A3 }, { 0x0F9A4, true, de0F9A4 }, { 0x0F9A5, true, de0F9A5 },
{ 0x0F9A6, true, de0F9A6 }, { 0x0F9A7, true, de0F9A7 }, { 0x0F9A8, true, de0F9A8 }, { 0x0F9A9, true, de0F9A9 },
{ 0x0F9AA, true, de0F9AA }, { 0x0F9AB, true, de0F9AB }, { 0x0F9AC, true, de0F9AC }, { 0x0F9AD, true, de0F9AD },
{ 0x0F9AE, true, de0F9AE }, { 0x0F9AF, true, de0F9AF }, { 0x0F9B0, true, de0F9B0 }, { 0x0F9B1, true, de0F9B1 },
{ 0x0F9B2, true, de0F9B2 }, { 0x0F9B3, true, de0F9B3 }, { 0x0F9B4, true, de0F9B4 }, { 0x0F9B5, true, de0F9B5 },
{ 0x0F9B6, true, de0F9B6 }, { 0x0F9B7, true, de0F9B7 }, { 0x0F9B8, true, de0F9B8 }, { 0x0F9B9, true, de0F9B9 },
{ 0x0F9BA, true, de0F9BA }, { 0x0F9BB, true, de0F9BB }, { 0x0F9BC, true, de0F9BC }, { 0x0F9BD, true, de0F9BD },
{ 0x0F9BE, true, de0F9BE }, { 0x0F9BF, true, de0F9BF }, { 0x0F9C0, true, de0F9C0 }, { 0x0F9C1, true, de0F9C1 },
{ 0x0F9C2, true, de0F9C2 }, { 0x0F9C3, true, de0F9C3 }, { 0x0F9C4, true, de0F9C4 }, { 0x0F9C5, true, de0F9C5 },
{ 0x0F9C6, true, de0F9C6 }, { 0x0F9C7, true, de0F9C7 }, { 0x0F9C8, true, de0F9C8 }, { 0x0F9C9, true, de0F9C9 },
{ 0x0F9CA, true, de0F9CA }, { 0x0F9CB, true, de0F9CB }, { 0x0F9CC, true, de0F9CC }, { 0x0F9CD, true, de0F9CD },
{ 0x0F9CE, true, de0F9CE }, { 0x0F9CF, true, de0F9CF }, { 0x0F9D0, true, de0F9D0 }, { 0x0F9D1, true, de0F9D1 },
{ 0x0F9D2, true, de0F9D2 }, { 0x0F9D3, true, de0F9D3 }, { 0x0F9D4, true, de0F9D4 }, { 0x0F9D5, true, de0F9D5 },
{ 0x0F9D6, true, de0F9D6 }, { 0x0F9D7, true, de0F9D7 }, { 0x0F9D8, true, de0F9D8 }, { 0x0F9D9, true, de0F9D9 },
{ 0x0F9DA, true, de0F9DA }, { 0x0F9DB, true, de0F9DB }, { 0x0F9DC, true, de0F9DC }, { 0x0F9DD, true, de0F9DD },
{ 0x0F9DE, true, de0F9DE }, { 0x0F9DF, true, de0F9DF }, { 0x0F9E0, true, de0F9E0 }, { 0x0F9E1, true, de0F9E1 },
{ 0x0F9E2, true, de0F9E2 }, { 0x0F9E3, true, de0F9E3 }, { 0x0F9E4, true, de0F9E4 }, { 0x0F9E5, true, de0F9E5 },
{ 0x0F9E6, true, de0F9E6 }, { 0x0F9E7, true, de0F9E7 }, { 0x0F9E8, true, de0F9E8 }, { 0x0F9E9, true, de0F9E9 },
{ 0x0F9EA, true, de0F9EA }, { 0x0F9EB, true, de0F9EB }, { 0x0F9EC, true, de0F9EC }, { 0x0F9ED, true, de0F9ED },
{ 0x0F9EE, true, de0F9EE }, { 0x0F9EF, true, de0F9EF }, { 0x0F9F0, true, de0F9F0 }, { 0x0F9F1, true, de0F9F1 },
{ 0x0F9F2, true, de0F9F2 }, { 0x0F9F3, true, de0F9F3 }, { 0x0F9F4, true, de0F9F4 }, { 0x0F9F5, true, de0F9F5 },
{ 0x0F9F6, true, de0F9F6 }, { 0x0F9F7, true, de0F9F7 }, { 0x0F9F8, true, de0F9F8 }, { 0x0F9F9, true, de0F9F9 },
{ 0x0F9FA, true, de0F9FA }, { 0x0F9FB, true, de0F9FB }, { 0x0F9FC, true, de0F9FC }, { 0x0F9FD, true, de0F9FD },
{ 0x0F9FE, true, de0F9FE }, { 0x0F9FF, true, de0F9FF }, { 0x0FA00, true, de0FA00 }, { 0x0FA01, true, de0FA01 },
{ 0x0FA02, true, de0FA02 }, { 0x0FA03, true, de0FA03 }, { 0x0FA04, true, de0FA04 }, { 0x0FA05, true, de0FA05 },
{ 0x0FA06, true, de0FA06 }, { 0x0FA07, true, de0FA07 }, { 0x0FA08, true, de0FA08 }, { 0x0FA09, true, de0FA09 },
{ 0x0FA0A, true, de0FA0A }, { 0x0FA0B, true, de0FA0B }, { 0x0FA0C, true, de0FA0C }, { 0x0FA0D, true, de0FA0D },
{ 0x0FA10, true, de0FA10 }, { 0x0FA12, true, de0FA12 }, { 0x0FA15, true, de0FA15 }, { 0x0FA16, true, de0FA16 },
{ 0x0FA17, true, de0FA17 }, { 0x0FA18, true, de0FA18 }, { 0x0FA19, true, de0FA19 }, { 0x0FA1A, true, de0FA1A },
{ 0x0FA1B, true, de0FA1B }, { 0x0FA1C, true, de0FA1C }, { 0x0FA1D, true, de0FA1D }, { 0x0FA1E, true, de0FA1E },
{ 0x0FA20, true, de0FA20 }, { 0x0FA22, true, de0FA22 }, { 0x0FA25, true, de0FA25 }, { 0x0FA26, true, de0FA26 },
{ 0x0FA2A, true, de0FA2A }, { 0x0FA2B, true, de0FA2B }, { 0x0FA2C, true, de0FA2C }, { 0x0FA2D, true, de0FA2D },
{ 0x0FA30, true, de0FA30 }, { 0x0FA31, true, de0FA31 }, { 0x0FA32, true, de0FA32 }, { 0x0FA33, true, de0FA33 },
{ 0x0FA34, true, de0FA34 }, { 0x0FA35, true, de0FA35 }, { 0x0FA36, true, de0FA36 }, { 0x0FA37, true, de0FA37 },
{ 0x0FA38, true, de0FA38 }, { 0x0FA39, true, de0FA39 }, { 0x0FA3A, true, de0FA3A }, { 0x0FA3B, true, de0FA3B },
{ 0x0FA3C, true, de0FA3C }, { 0x0FA3D, true, de0FA3D }, { 0x0FA3E, true, de0FA3E }, { 0x0FA3F, true, de0FA3F },
{ 0x0FA40, true, de0FA40 }, { 0x0FA41, true, de0FA41 }, { 0x0FA42, true, de0FA42 }, { 0x0FA43, true, de0FA43 },
{ 0x0FA44, true, de0FA44 }, { 0x0FA45, true, de0FA45 }, { 0x0FA46, true, de0FA46 }, { 0x0FA47, true, de0FA47 },
{ 0x0FA48, true, de0FA48 }, { 0x0FA49, true, de0FA49 }, { 0x0FA4A, true, de0FA4A }, { 0x0FA4B, true, de0FA4B },
{ 0x0FA4C, true, de0FA4C }, { 0x0FA4D, true, de0FA4D }, { 0x0FA4E, true, de0FA4E }, { 0x0FA4F, true, de0FA4F },
{ 0x0FA50, true, de0FA50 }, { 0x0FA51, true, de0FA51 }, { 0x0FA52, true, de0FA52 }, { 0x0FA53, true, de0FA53 },
{ 0x0FA54, true, de0FA54 }, { 0x0FA55, true, de0FA55 }, { 0x0FA56, true, de0FA56 }, { 0x0FA57, true, de0FA57 },
{ 0x0FA58, true, de0FA58 }, { 0x0FA59, true, de0FA59 }, { 0x0FA5A, true, de0FA5A }, { 0x0FA5B, true, de0FA5B },
{ 0x0FA5C, true, de0FA5C }, { 0x0FA5D, true, de0FA5D }, { 0x0FA5E, true, de0FA5E }, { 0x0FA5F, true, de0FA5F },
{ 0x0FA60, true, de0FA60 }, { 0x0FA61, true, de0FA61 }, { 0x0FA62, true, de0FA62 }, { 0x0FA63, true, de0FA63 },
{ 0x0FA64, true, de0FA64 }, { 0x0FA65, true, de0FA65 }, { 0x0FA66, true, de0FA66 }, { 0x0FA67, true, de0FA67 },
{ 0x0FA68, true, de0FA68 }, { 0x0FA69, true, de0FA69 }, { 0x0FA6A, true, de0FA6A }, { 0x0FA70, true, de0FA70 },
{ 0x0FA71, true, de0FA71 }, { 0x0FA72, true, de0FA72 }, { 0x0FA73, true, de0FA73 }, { 0x0FA74, true, de0FA74 },
{ 0x0FA75, true, de0FA75 }, { 0x0FA76, true, de0FA76 }, { 0x0FA77, true, de0FA77 }, { 0x0FA78, true, de0FA78 },
{ 0x0FA79, true, de0FA79 }, { 0x0FA7A, true, de0FA7A }, { 0x0FA7B, true, de0FA7B }, { 0x0FA7C, true, de0FA7C },
{ 0x0FA7D, true, de0FA7D }, { 0x0FA7E, true, de0FA7E }, { 0x0FA7F, true, de0FA7F }, { 0x0FA80, true, de0FA80 },
{ 0x0FA81, true, de0FA81 }, { 0x0FA82, true, de0FA82 }, { 0x0FA83, true, de0FA83 }, { 0x0FA84, true, de0FA84 },
{ 0x0FA85, true, de0FA85 }, { 0x0FA86, true, de0FA86 }, { 0x0FA87, true, de0FA87 }, { 0x0FA88, true, de0FA88 },
{ 0x0FA89, true, de0FA89 }, { 0x0FA8A, true, de0FA8A }, { 0x0FA8B, true, de0FA8B }, { 0x0FA8C, true, de0FA8C },
{ 0x0FA8D, true, de0FA8D }, { 0x0FA8E, true, de0FA8E }, { 0x0FA8F, true, de0FA8F }, { 0x0FA90, true, de0FA90 },
{ 0x0FA91, true, de0FA91 }, { 0x0FA92, true, de0FA92 }, { 0x0FA93, true, de0FA93 }, { 0x0FA94, true, de0FA94 },
{ 0x0FA95, true, de0FA95 }, { 0x0FA96, true, de0FA96 }, { 0x0FA97, true, de0FA97 }, { 0x0FA98, true, de0FA98 },
{ 0x0FA99, true, de0FA99 }, { 0x0FA9A, true, de0FA9A }, { 0x0FA9B, true, de0FA9B }, { 0x0FA9C, true, de0FA9C },
{ 0x0FA9D, true, de0FA9D }, { 0x0FA9E, true, de0FA9E }, { 0x0FA9F, true, de0FA9F }, { 0x0FAA0, true, de0FAA0 },
{ 0x0FAA1, true, de0FAA1 }, { 0x0FAA2, true, de0FAA2 }, { 0x0FAA3, true, de0FAA3 }, { 0x0FAA4, true, de0FAA4 },
{ 0x0FAA5, true, de0FAA5 }, { 0x0FAA6, true, de0FAA6 }, { 0x0FAA7, true, de0FAA7 }, { 0x0FAA8, true, de0FAA8 },
{ 0x0FAA9, true, de0FAA9 }, { 0x0FAAA, true, de0FAAA }, { 0x0FAAB, true, de0FAAB }, { 0x0FAAC, true, de0FAAC },
{ 0x0FAAD, true, de0FAAD }, { 0x0FAAE, true, de0FAAE }, { 0x0FAAF, true, de0FAAF }, { 0x0FAB0, true, de0FAB0 },
{ 0x0FAB1, true, de0FAB1 }, { 0x0FAB2, true, de0FAB2 }, { 0x0FAB3, true, de0FAB3 }, { 0x0FAB4, true, de0FAB4 },
{ 0x0FAB5, true, de0FAB5 }, { 0x0FAB6, true, de0FAB6 }, { 0x0FAB7, true, de0FAB7 }, { 0x0FAB8, true, de0FAB8 },
{ 0x0FAB9, true, de0FAB9 }, { 0x0FABA, true, de0FABA }, { 0x0FABB, true, de0FABB }, { 0x0FABC, true, de0FABC },
{ 0x0FABD, true, de0FABD }, { 0x0FABE, true, de0FABE }, { 0x0FABF, true, de0FABF }, { 0x0FAC0, true, de0FAC0 },
{ 0x0FAC1, true, de0FAC1 }, { 0x0FAC2, true, de0FAC2 }, { 0x0FAC3, true, de0FAC3 }, { 0x0FAC4, true, de0FAC4 },
{ 0x0FAC5, true, de0FAC5 }, { 0x0FAC6, true, de0FAC6 }, { 0x0FAC7, true, de0FAC7 }, { 0x0FAC8, true, de0FAC8 },
{ 0x0FAC9, true, de0FAC9 }, { 0x0FACA, true, de0FACA }, { 0x0FACB, true, de0FACB }, { 0x0FACC, true, de0FACC },
{ 0x0FACD, true, de0FACD }, { 0x0FACE, true, de0FACE }, { 0x0FACF, true, de0FACF }, { 0x0FAD0, true, de0FAD0 },
{ 0x0FAD1, true, de0FAD1 }, { 0x0FAD2, true, de0FAD2 }, { 0x0FAD3, true, de0FAD3 }, { 0x0FAD4, true, de0FAD4 },
{ 0x0FAD5, true, de0FAD5 }, { 0x0FAD6, true, de0FAD6 }, { 0x0FAD7, true, de0FAD7 }, { 0x0FAD8, true, de0FAD8 },
{ 0x0FAD9, true, de0FAD9 }, { 0x0FB00, false, de0FB00 }, { 0x0FB01, false, de0FB01 }, { 0x0FB02, false, de0FB02 },
{ 0x0FB03, false, de0FB03 }, { 0x0FB04, false, de0FB04 }, { 0x0FB05, false, de0FB05 }, { 0x0FB06, false, de0FB06 },
{ 0x0FB13, false, de0FB13 }, { 0x0FB14, false, de0FB14 }, { 0x0FB15, false, de0FB15 }, { 0x0FB16, false, de0FB16 },
{ 0x0FB17, false, de0FB17 }, { 0x0FB1D, true, de0FB1D }, { 0x0FB1F, true, de0FB1F }, { 0x0FB20, false, de0FB20 },
{ 0x0FB21, false, de0FB21 }, { 0x0FB22, false, de0FB22 }, { 0x0FB23, false, de0FB23 }, { 0x0FB24, false, de0FB24 },
{ 0x0FB25, false, de0FB25 }, { 0x0FB26, false, de0FB26 }, { 0x0FB27, false, de0FB27 }, { 0x0FB28, false, de0FB28 },
{ 0x0FB29, false, de0FB29 }, { 0x0FB2A, true, de0FB2A }, { 0x0FB2B, true, de0FB2B }, { 0x0FB2C, true, de0FB2C },
{ 0x0FB2D, true, de0FB2D }, { 0x0FB2E, true, de0FB2E }, { 0x0FB2F, true, de0FB2F }, { 0x0FB30, true, de0FB30 },
{ 0x0FB31, true, de0FB31 }, { 0x0FB32, true, de0FB32 }, { 0x0FB33, true, de0FB33 }, { 0x0FB34, true, de0FB34 },
{ 0x0FB35, true, de0FB35 }, { 0x0FB36, true, de0FB36 }, { 0x0FB38, true, de0FB38 }, { 0x0FB39, true, de0FB39 },
{ 0x0FB3A, true, de0FB3A }, { 0x0FB3B, true, de0FB3B }, { 0x0FB3C, true, de0FB3C }, { 0x0FB3E, true, de0FB3E },
{ 0x0FB40, true, de0FB40 }, { 0x0FB41, true, de0FB41 }, { 0x0FB43, true, de0FB43 }, { 0x0FB44, true, de0FB44 },
{ 0x0FB46, true, de0FB46 }, { 0x0FB47, true, de0FB47 }, { 0x0FB48, true, de0FB48 }, { 0x0FB49, true, de0FB49 },
{ 0x0FB4A, true, de0FB4A }, { 0x0FB4B, true, de0FB4B }, { 0x0FB4C, true, de0FB4C }, { 0x0FB4D, true, de0FB4D },
{ 0x0FB4E, true, de0FB4E }, { 0x0FB4F, false, de0FB4F }, { 0x0FB50, false, de0FB50 }, { 0x0FB51, false, de0FB51 },
{ 0x0FB52, false, de0FB52 }, { 0x0FB53, false, de0FB53 }, { 0x0FB54, false, de0FB54 }, { 0x0FB55, false, de0FB55 },
{ 0x0FB56, false, de0FB56 }, { 0x0FB57, false, de0FB57 }, { 0x0FB58, false, de0FB58 }, { 0x0FB59, false, de0FB59 },
{ 0x0FB5A, false, de0FB5A }, { 0x0FB5B, false, de0FB5B }, { 0x0FB5C, false, de0FB5C }, { 0x0FB5D, false, de0FB5D },
{ 0x0FB5E, false, de0FB5E }, { 0x0FB5F, false, de0FB5F }, { 0x0FB60, false, de0FB60 }, { 0x0FB61, false, de0FB61 },
{ 0x0FB62, false, de0FB62 }, { 0x0FB63, false, de0FB63 }, { 0x0FB64, false, de0FB64 }, { 0x0FB65, false, de0FB65 },
{ 0x0FB66, false, de0FB66 }, { 0x0FB67, false, de0FB67 }, { 0x0FB68, false, de0FB68 }, { 0x0FB69, false, de0FB69 },
{ 0x0FB6A, false, de0FB6A }, { 0x0FB6B, false, de0FB6B }, { 0x0FB6C, false, de0FB6C }, { 0x0FB6D, false, de0FB6D },
{ 0x0FB6E, false, de0FB6E }, { 0x0FB6F, false, de0FB6F }, { 0x0FB70, false, de0FB70 }, { 0x0FB71, false, de0FB71 },
{ 0x0FB72, false, de0FB72 }, { 0x0FB73, false, de0FB73 }, { 0x0FB74, false, de0FB74 }, { 0x0FB75, false, de0FB75 },
{ 0x0FB76, false, de0FB76 }, { 0x0FB77, false, de0FB77 }, { 0x0FB78, false, de0FB78 }, { 0x0FB79, false, de0FB79 },
{ 0x0FB7A, false, de0FB7A }, { 0x0FB7B, false, de0FB7B }, { 0x0FB7C, false, de0FB7C }, { 0x0FB7D, false, de0FB7D },
{ 0x0FB7E, false, de0FB7E }, { 0x0FB7F, false, de0FB7F }, { 0x0FB80, false, de0FB80 }, { 0x0FB81, false, de0FB81 },
{ 0x0FB82, false, de0FB82 }, { 0x0FB83, false, de0FB83 }, { 0x0FB84, false, de0FB84 }, { 0x0FB85, false, de0FB85 },
{ 0x0FB86, false, de0FB86 }, { 0x0FB87, false, de0FB87 }, { 0x0FB88, false, de0FB88 }, { 0x0FB89, false, de0FB89 },
{ 0x0FB8A, false, de0FB8A }, { 0x0FB8B, false, de0FB8B }, { 0x0FB8C, false, de0FB8C }, { 0x0FB8D, false, de0FB8D },
{ 0x0FB8E, false, de0FB8E }, { 0x0FB8F, false, de0FB8F }, { 0x0FB90, false, de0FB90 }, { 0x0FB91, false, de0FB91 },
{ 0x0FB92, false, de0FB92 }, { 0x0FB93, false, de0FB93 }, { 0x0FB94, false, de0FB94 }, { 0x0FB95, false, de0FB95 },
{ 0x0FB96, false, de0FB96 }, { 0x0FB97, false, de0FB97 }, { 0x0FB98, false, de0FB98 }, { 0x0FB99, false, de0FB99 },
{ 0x0FB9A, false, de0FB9A }, { 0x0FB9B, false, de0FB9B }, { 0x0FB9C, false, de0FB9C }, { 0x0FB9D, false, de0FB9D },
{ 0x0FB9E, false, de0FB9E }, { 0x0FB9F, false, de0FB9F }, { 0x0FBA0, false, de0FBA0 }, { 0x0FBA1, false, de0FBA1 },
{ 0x0FBA2, false, de0FBA2 }, { 0x0FBA3, false, de0FBA3 }, { 0x0FBA4, false, de0FBA4 }, { 0x0FBA5, false, de0FBA5 },
{ 0x0FBA6, false, de0FBA6 }, { 0x0FBA7, false, de0FBA7 }, { 0x0FBA8, false, de0FBA8 }, { 0x0FBA9, false, de0FBA9 },
{ 0x0FBAA, false, de0FBAA }, { 0x0FBAB, false, de0FBAB }, { 0x0FBAC, false, de0FBAC }, { 0x0FBAD, false, de0FBAD },
{ 0x0FBAE, false, de0FBAE }, { 0x0FBAF, false, de0FBAF }, { 0x0FBB0, false, de0FBB0 }, { 0x0FBB1, false, de0FBB1 },
{ 0x0FBD3, false, de0FBD3 }, { 0x0FBD4, false, de0FBD4 }, { 0x0FBD5, false, de0FBD5 }, { 0x0FBD6, false, de0FBD6 },
{ 0x0FBD7, false, de0FBD7 }, { 0x0FBD8, false, de0FBD8 }, { 0x0FBD9, false, de0FBD9 }, { 0x0FBDA, false, de0FBDA },
{ 0x0FBDB, false, de0FBDB }, { 0x0FBDC, false, de0FBDC }, { 0x0FBDD, false, de0FBDD }, { 0x0FBDE, false, de0FBDE },
{ 0x0FBDF, false, de0FBDF }, { 0x0FBE0, false, de0FBE0 }, { 0x0FBE1, false, de0FBE1 }, { 0x0FBE2, false, de0FBE2 },
{ 0x0FBE3, false, de0FBE3 }, { 0x0FBE4, false, de0FBE4 }, { 0x0FBE5, false, de0FBE5 }, { 0x0FBE6, false, de0FBE6 },
{ 0x0FBE7, false, de0FBE7 }, { 0x0FBE8, false, de0FBE8 }, { 0x0FBE9, false, de0FBE9 }, { 0x0FBEA, false, de0FBEA },
{ 0x0FBEB, false, de0FBEB }, { 0x0FBEC, false, de0FBEC }, { 0x0FBED, false, de0FBED }, { 0x0FBEE, false, de0FBEE },
{ 0x0FBEF, false, de0FBEF }, { 0x0FBF0, false, de0FBF0 }, { 0x0FBF1, false, de0FBF1 }, { 0x0FBF2, false, de0FBF2 },
{ 0x0FBF3, false, de0FBF3 }, { 0x0FBF4, false, de0FBF4 }, { 0x0FBF5, false, de0FBF5 }, { 0x0FBF6, false, de0FBF6 },
{ 0x0FBF7, false, de0FBF7 }, { 0x0FBF8, false, de0FBF8 }, { 0x0FBF9, false, de0FBF9 }, { 0x0FBFA, false, de0FBFA },
{ 0x0FBFB, false, de0FBFB }, { 0x0FBFC, false, de0FBFC }, { 0x0FBFD, false, de0FBFD }, { 0x0FBFE, false, de0FBFE },
{ 0x0FBFF, false, de0FBFF }, { 0x0FC00, false, de0FC00 }, { 0x0FC01, false, de0FC01 }, { 0x0FC02, false, de0FC02 },
{ 0x0FC03, false, de0FC03 }, { 0x0FC04, false, de0FC04 }, { 0x0FC05, false, de0FC05 }, { 0x0FC06, false, de0FC06 },
{ 0x0FC07, false, de0FC07 }, { 0x0FC08, false, de0FC08 }, { 0x0FC09, false, de0FC09 }, { 0x0FC0A, false, de0FC0A },
{ 0x0FC0B, false, de0FC0B }, { 0x0FC0C, false, de0FC0C }, { 0x0FC0D, false, de0FC0D }, { 0x0FC0E, false, de0FC0E },
{ 0x0FC0F, false, de0FC0F }, { 0x0FC10, false, de0FC10 }, { 0x0FC11, false, de0FC11 }, { 0x0FC12, false, de0FC12 },
{ 0x0FC13, false, de0FC13 }, { 0x0FC14, false, de0FC14 }, { 0x0FC15, false, de0FC15 }, { 0x0FC16, false, de0FC16 },
{ 0x0FC17, false, de0FC17 }, { 0x0FC18, false, de0FC18 }, { 0x0FC19, false, de0FC19 }, { 0x0FC1A, false, de0FC1A },
{ 0x0FC1B, false, de0FC1B }, { 0x0FC1C, false, de0FC1C }, { 0x0FC1D, false, de0FC1D }, { 0x0FC1E, false, de0FC1E },
{ 0x0FC1F, false, de0FC1F }, { 0x0FC20, false, de0FC20 }, { 0x0FC21, false, de0FC21 }, { 0x0FC22, false, de0FC22 },
{ 0x0FC23, false, de0FC23 }, { 0x0FC24, false, de0FC24 }, { 0x0FC25, false, de0FC25 }, { 0x0FC26, false, de0FC26 },
{ 0x0FC27, false, de0FC27 }, { 0x0FC28, false, de0FC28 }, { 0x0FC29, false, de0FC29 }, { 0x0FC2A, false, de0FC2A },
{ 0x0FC2B, false, de0FC2B }, { 0x0FC2C, false, de0FC2C }, { 0x0FC2D, false, de0FC2D }, { 0x0FC2E, false, de0FC2E },
{ 0x0FC2F, false, de0FC2F }, { 0x0FC30, false, de0FC30 }, { 0x0FC31, false, de0FC31 }, { 0x0FC32, false, de0FC32 },
{ 0x0FC33, false, de0FC33 }, { 0x0FC34, false, de0FC34 }, { 0x0FC35, false, de0FC35 }, { 0x0FC36, false, de0FC36 },
{ 0x0FC37, false, de0FC37 }, { 0x0FC38, false, de0FC38 }, { 0x0FC39, false, de0FC39 }, { 0x0FC3A, false, de0FC3A },
{ 0x0FC3B, false, de0FC3B }, { 0x0FC3C, false, de0FC3C }, { 0x0FC3D, false, de0FC3D }, { 0x0FC3E, false, de0FC3E },
{ 0x0FC3F, false, de0FC3F }, { 0x0FC40, false, de0FC40 }, { 0x0FC41, false, de0FC41 }, { 0x0FC42, false, de0FC42 },
{ 0x0FC43, false, de0FC43 }, { 0x0FC44, false, de0FC44 }, { 0x0FC45, false, de0FC45 }, { 0x0FC46, false, de0FC46 },
{ 0x0FC47, false, de0FC47 }, { 0x0FC48, false, de0FC48 }, { 0x0FC49, false, de0FC49 }, { 0x0FC4A, false, de0FC4A },
{ 0x0FC4B, false, de0FC4B }, { 0x0FC4C, false, de0FC4C }, { 0x0FC4D, false, de0FC4D }, { 0x0FC4E, false, de0FC4E },
{ 0x0FC4F, false, de0FC4F }, { 0x0FC50, false, de0FC50 }, { 0x0FC51, false, de0FC51 }, { 0x0FC52, false, de0FC52 },
{ 0x0FC53, false, de0FC53 }, { 0x0FC54, false, de0FC54 }, { 0x0FC55, false, de0FC55 }, { 0x0FC56, false, de0FC56 },
{ 0x0FC57, false, de0FC57 }, { 0x0FC58, false, de0FC58 }, { 0x0FC59, false, de0FC59 }, { 0x0FC5A, false, de0FC5A },
{ 0x0FC5B, false, de0FC5B }, { 0x0FC5C, false, de0FC5C }, { 0x0FC5D, false, de0FC5D }, { 0x0FC5E, false, de0FC5E },
{ 0x0FC5F, false, de0FC5F }, { 0x0FC60, false, de0FC60 }, { 0x0FC61, false, de0FC61 }, { 0x0FC62, false, de0FC62 },
{ 0x0FC63, false, de0FC63 }, { 0x0FC64, false, de0FC64 }, { 0x0FC65, false, de0FC65 }, { 0x0FC66, false, de0FC66 },
{ 0x0FC67, false, de0FC67 }, { 0x0FC68, false, de0FC68 }, { 0x0FC69, false, de0FC69 }, { 0x0FC6A, false, de0FC6A },
{ 0x0FC6B, false, de0FC6B }, { 0x0FC6C, false, de0FC6C }, { 0x0FC6D, false, de0FC6D }, { 0x0FC6E, false, de0FC6E },
{ 0x0FC6F, false, de0FC6F }, { 0x0FC70, false, de0FC70 }, { 0x0FC71, false, de0FC71 }, { 0x0FC72, false, de0FC72 },
{ 0x0FC73, false, de0FC73 }, { 0x0FC74, false, de0FC74 }, { 0x0FC75, false, de0FC75 }, { 0x0FC76, false, de0FC76 },
{ 0x0FC77, false, de0FC77 }, { 0x0FC78, false, de0FC78 }, { 0x0FC79, false, de0FC79 }, { 0x0FC7A, false, de0FC7A },
{ 0x0FC7B, false, de0FC7B }, { 0x0FC7C, false, de0FC7C }, { 0x0FC7D, false, de0FC7D }, { 0x0FC7E, false, de0FC7E },
{ 0x0FC7F, false, de0FC7F }, { 0x0FC80, false, de0FC80 }, { 0x0FC81, false, de0FC81 }, { 0x0FC82, false, de0FC82 },
{ 0x0FC83, false, de0FC83 }, { 0x0FC84, false, de0FC84 }, { 0x0FC85, false, de0FC85 }, { 0x0FC86, false, de0FC86 },
{ 0x0FC87, false, de0FC87 }, { 0x0FC88, false, de0FC88 }, { 0x0FC89, false, de0FC89 }, { 0x0FC8A, false, de0FC8A },
{ 0x0FC8B, false, de0FC8B }, { 0x0FC8C, false, de0FC8C }, { 0x0FC8D, false, de0FC8D }, { 0x0FC8E, false, de0FC8E },
{ 0x0FC8F, false, de0FC8F }, { 0x0FC90, false, de0FC90 }, { 0x0FC91, false, de0FC91 }, { 0x0FC92, false, de0FC92 },
{ 0x0FC93, false, de0FC93 }, { 0x0FC94, false, de0FC94 }, { 0x0FC95, false, de0FC95 }, { 0x0FC96, false, de0FC96 },
{ 0x0FC97, false, de0FC97 }, { 0x0FC98, false, de0FC98 }, { 0x0FC99, false, de0FC99 }, { 0x0FC9A, false, de0FC9A },
{ 0x0FC9B, false, de0FC9B }, { 0x0FC9C, false, de0FC9C }, { 0x0FC9D, false, de0FC9D }, { 0x0FC9E, false, de0FC9E },
{ 0x0FC9F, false, de0FC9F }, { 0x0FCA0, false, de0FCA0 }, { 0x0FCA1, false, de0FCA1 }, { 0x0FCA2, false, de0FCA2 },
{ 0x0FCA3, false, de0FCA3 }, { 0x0FCA4, false, de0FCA4 }, { 0x0FCA5, false, de0FCA5 }, { 0x0FCA6, false, de0FCA6 },
{ 0x0FCA7, false, de0FCA7 }, { 0x0FCA8, false, de0FCA8 }, { 0x0FCA9, false, de0FCA9 }, { 0x0FCAA, false, de0FCAA },
{ 0x0FCAB, false, de0FCAB }, { 0x0FCAC, false, de0FCAC }, { 0x0FCAD, false, de0FCAD }, { 0x0FCAE, false, de0FCAE },
{ 0x0FCAF, false, de0FCAF }, { 0x0FCB0, false, de0FCB0 }, { 0x0FCB1, false, de0FCB1 }, { 0x0FCB2, false, de0FCB2 },
{ 0x0FCB3, false, de0FCB3 }, { 0x0FCB4, false, de0FCB4 }, { 0x0FCB5, false, de0FCB5 }, { 0x0FCB6, false, de0FCB6 },
{ 0x0FCB7, false, de0FCB7 }, { 0x0FCB8, false, de0FCB8 }, { 0x0FCB9, false, de0FCB9 }, { 0x0FCBA, false, de0FCBA },
{ 0x0FCBB, false, de0FCBB }, { 0x0FCBC, false, de0FCBC }, { 0x0FCBD, false, de0FCBD }, { 0x0FCBE, false, de0FCBE },
{ 0x0FCBF, false, de0FCBF }, { 0x0FCC0, false, de0FCC0 }, { 0x0FCC1, false, de0FCC1 }, { 0x0FCC2, false, de0FCC2 },
{ 0x0FCC3, false, de0FCC3 }, { 0x0FCC4, false, de0FCC4 }, { 0x0FCC5, false, de0FCC5 }, { 0x0FCC6, false, de0FCC6 },
{ 0x0FCC7, false, de0FCC7 }, { 0x0FCC8, false, de0FCC8 }, { 0x0FCC9, false, de0FCC9 }, { 0x0FCCA, false, de0FCCA },
{ 0x0FCCB, false, de0FCCB }, { 0x0FCCC, false, de0FCCC }, { 0x0FCCD, false, de0FCCD }, { 0x0FCCE, false, de0FCCE },
{ 0x0FCCF, false, de0FCCF }, { 0x0FCD0, false, de0FCD0 }, { 0x0FCD1, false, de0FCD1 }, { 0x0FCD2, false, de0FCD2 },
{ 0x0FCD3, false, de0FCD3 }, { 0x0FCD4, false, de0FCD4 }, { 0x0FCD5, false, de0FCD5 }, { 0x0FCD6, false, de0FCD6 },
{ 0x0FCD7, false, de0FCD7 }, { 0x0FCD8, false, de0FCD8 }, { 0x0FCD9, false, de0FCD9 }, { 0x0FCDA, false, de0FCDA },
{ 0x0FCDB, false, de0FCDB }, { 0x0FCDC, false, de0FCDC }, { 0x0FCDD, false, de0FCDD }, { 0x0FCDE, false, de0FCDE },
{ 0x0FCDF, false, de0FCDF }, { 0x0FCE0, false, de0FCE0 }, { 0x0FCE1, false, de0FCE1 }, { 0x0FCE2, false, de0FCE2 },
{ 0x0FCE3, false, de0FCE3 }, { 0x0FCE4, false, de0FCE4 }, { 0x0FCE5, false, de0FCE5 }, { 0x0FCE6, false, de0FCE6 },
{ 0x0FCE7, false, de0FCE7 }, { 0x0FCE8, false, de0FCE8 }, { 0x0FCE9, false, de0FCE9 }, { 0x0FCEA, false, de0FCEA },
{ 0x0FCEB, false, de0FCEB }, { 0x0FCEC, false, de0FCEC }, { 0x0FCED, false, de0FCED }, { 0x0FCEE, false, de0FCEE },
{ 0x0FCEF, false, de0FCEF }, { 0x0FCF0, false, de0FCF0 }, { 0x0FCF1, false, de0FCF1 }, { 0x0FCF2, false, de0FCF2 },
{ 0x0FCF3, false, de0FCF3 }, { 0x0FCF4, false, de0FCF4 }, { 0x0FCF5, false, de0FCF5 }, { 0x0FCF6, false, de0FCF6 },
{ 0x0FCF7, false, de0FCF7 }, { 0x0FCF8, false, de0FCF8 }, { 0x0FCF9, false, de0FCF9 }, { 0x0FCFA, false, de0FCFA },
{ 0x0FCFB, false, de0FCFB }, { 0x0FCFC, false, de0FCFC }, { 0x0FCFD, false, de0FCFD }, { 0x0FCFE, false, de0FCFE },
{ 0x0FCFF, false, de0FCFF }, { 0x0FD00, false, de0FD00 }, { 0x0FD01, false, de0FD01 }, { 0x0FD02, false, de0FD02 },
{ 0x0FD03, false, de0FD03 }, { 0x0FD04, false, de0FD04 }, { 0x0FD05, false, de0FD05 }, { 0x0FD06, false, de0FD06 },
{ 0x0FD07, false, de0FD07 }, { 0x0FD08, false, de0FD08 }, { 0x0FD09, false, de0FD09 }, { 0x0FD0A, false, de0FD0A },
{ 0x0FD0B, false, de0FD0B }, { 0x0FD0C, false, de0FD0C }, { 0x0FD0D, false, de0FD0D }, { 0x0FD0E, false, de0FD0E },
{ 0x0FD0F, false, de0FD0F }, { 0x0FD10, false, de0FD10 }, { 0x0FD11, false, de0FD11 }, { 0x0FD12, false, de0FD12 },
{ 0x0FD13, false, de0FD13 }, { 0x0FD14, false, de0FD14 }, { 0x0FD15, false, de0FD15 }, { 0x0FD16, false, de0FD16 },
{ 0x0FD17, false, de0FD17 }, { 0x0FD18, false, de0FD18 }, { 0x0FD19, false, de0FD19 }, { 0x0FD1A, false, de0FD1A },
{ 0x0FD1B, false, de0FD1B }, { 0x0FD1C, false, de0FD1C }, { 0x0FD1D, false, de0FD1D }, { 0x0FD1E, false, de0FD1E },
{ 0x0FD1F, false, de0FD1F }, { 0x0FD20, false, de0FD20 }, { 0x0FD21, false, de0FD21 }, { 0x0FD22, false, de0FD22 },
{ 0x0FD23, false, de0FD23 }, { 0x0FD24, false, de0FD24 }, { 0x0FD25, false, de0FD25 }, { 0x0FD26, false, de0FD26 },
{ 0x0FD27, false, de0FD27 }, { 0x0FD28, false, de0FD28 }, { 0x0FD29, false, de0FD29 }, { 0x0FD2A, false, de0FD2A },
{ 0x0FD2B, false, de0FD2B }, { 0x0FD2C, false, de0FD2C }, { 0x0FD2D, false, de0FD2D }, { 0x0FD2E, false, de0FD2E },
{ 0x0FD2F, false, de0FD2F }, { 0x0FD30, false, de0FD30 }, { 0x0FD31, false, de0FD31 }, { 0x0FD32, false, de0FD32 },
{ 0x0FD33, false, de0FD33 }, { 0x0FD34, false, de0FD34 }, { 0x0FD35, false, de0FD35 }, { 0x0FD36, false, de0FD36 },
{ 0x0FD37, false, de0FD37 }, { 0x0FD38, false, de0FD38 }, { 0x0FD39, false, de0FD39 }, { 0x0FD3A, false, de0FD3A },
{ 0x0FD3B, false, de0FD3B }, { 0x0FD3C, false, de0FD3C }, { 0x0FD3D, false, de0FD3D }, { 0x0FD50, false, de0FD50 },
{ 0x0FD51, false, de0FD51 }, { 0x0FD52, false, de0FD52 }, { 0x0FD53, false, de0FD53 }, { 0x0FD54, false, de0FD54 },
{ 0x0FD55, false, de0FD55 }, { 0x0FD56, false, de0FD56 }, { 0x0FD57, false, de0FD57 }, { 0x0FD58, false, de0FD58 },
{ 0x0FD59, false, de0FD59 }, { 0x0FD5A, false, de0FD5A }, { 0x0FD5B, false, de0FD5B }, { 0x0FD5C, false, de0FD5C },
{ 0x0FD5D, false, de0FD5D }, { 0x0FD5E, false, de0FD5E }, { 0x0FD5F, false, de0FD5F }, { 0x0FD60, false, de0FD60 },
{ 0x0FD61, false, de0FD61 }, { 0x0FD62, false, de0FD62 }, { 0x0FD63, false, de0FD63 }, { 0x0FD64, false, de0FD64 },
{ 0x0FD65, false, de0FD65 }, { 0x0FD66, false, de0FD66 }, { 0x0FD67, false, de0FD67 }, { 0x0FD68, false, de0FD68 },
{ 0x0FD69, false, de0FD69 }, { 0x0FD6A, false, de0FD6A }, { 0x0FD6B, false, de0FD6B }, { 0x0FD6C, false, de0FD6C },
{ 0x0FD6D, false, de0FD6D }, { 0x0FD6E, false, de0FD6E }, { 0x0FD6F, false, de0FD6F }, { 0x0FD70, false, de0FD70 },
{ 0x0FD71, false, de0FD71 }, { 0x0FD72, false, de0FD72 }, { 0x0FD73, false, de0FD73 }, { 0x0FD74, false, de0FD74 },
{ 0x0FD75, false, de0FD75 }, { 0x0FD76, false, de0FD76 }, { 0x0FD77, false, de0FD77 }, { 0x0FD78, false, de0FD78 },
{ 0x0FD79, false, de0FD79 }, { 0x0FD7A, false, de0FD7A }, { 0x0FD7B, false, de0FD7B }, { 0x0FD7C, false, de0FD7C },
{ 0x0FD7D, false, de0FD7D }, { 0x0FD7E, false, de0FD7E }, { 0x0FD7F, false, de0FD7F }, { 0x0FD80, false, de0FD80 },
{ 0x0FD81, false, de0FD81 }, { 0x0FD82, false, de0FD82 }, { 0x0FD83, false, de0FD83 }, { 0x0FD84, false, de0FD84 },
{ 0x0FD85, false, de0FD85 }, { 0x0FD86, false, de0FD86 }, { 0x0FD87, false, de0FD87 }, { 0x0FD88, false, de0FD88 },
{ 0x0FD89, false, de0FD89 }, { 0x0FD8A, false, de0FD8A }, { 0x0FD8B, false, de0FD8B }, { 0x0FD8C, false, de0FD8C },
{ 0x0FD8D, false, de0FD8D }, { 0x0FD8E, false, de0FD8E }, { 0x0FD8F, false, de0FD8F }, { 0x0FD92, false, de0FD92 },
{ 0x0FD93, false, de0FD93 }, { 0x0FD94, false, de0FD94 }, { 0x0FD95, false, de0FD95 }, { 0x0FD96, false, de0FD96 },
{ 0x0FD97, false, de0FD97 }, { 0x0FD98, false, de0FD98 }, { 0x0FD99, false, de0FD99 }, { 0x0FD9A, false, de0FD9A },
{ 0x0FD9B, false, de0FD9B }, { 0x0FD9C, false, de0FD9C }, { 0x0FD9D, false, de0FD9D }, { 0x0FD9E, false, de0FD9E },
{ 0x0FD9F, false, de0FD9F }, { 0x0FDA0, false, de0FDA0 }, { 0x0FDA1, false, de0FDA1 }, { 0x0FDA2, false, de0FDA2 },
{ 0x0FDA3, false, de0FDA3 }, { 0x0FDA4, false, de0FDA4 }, { 0x0FDA5, false, de0FDA5 }, { 0x0FDA6, false, de0FDA6 },
{ 0x0FDA7, false, de0FDA7 }, { 0x0FDA8, false, de0FDA8 }, { 0x0FDA9, false, de0FDA9 }, { 0x0FDAA, false, de0FDAA },
{ 0x0FDAB, false, de0FDAB }, { 0x0FDAC, false, de0FDAC }, { 0x0FDAD, false, de0FDAD }, { 0x0FDAE, false, de0FDAE },
{ 0x0FDAF, false, de0FDAF }, { 0x0FDB0, false, de0FDB0 }, { 0x0FDB1, false, de0FDB1 }, { 0x0FDB2, false, de0FDB2 },
{ 0x0FDB3, false, de0FDB3 }, { 0x0FDB4, false, de0FDB4 }, { 0x0FDB5, false, de0FDB5 }, { 0x0FDB6, false, de0FDB6 },
{ 0x0FDB7, false, de0FDB7 }, { 0x0FDB8, false, de0FDB8 }, { 0x0FDB9, false, de0FDB9 }, { 0x0FDBA, false, de0FDBA },
{ 0x0FDBB, false, de0FDBB }, { 0x0FDBC, false, de0FDBC }, { 0x0FDBD, false, de0FDBD }, { 0x0FDBE, false, de0FDBE },
{ 0x0FDBF, false, de0FDBF }, { 0x0FDC0, false, de0FDC0 }, { 0x0FDC1, false, de0FDC1 }, { 0x0FDC2, false, de0FDC2 },
{ 0x0FDC3, false, de0FDC3 }, { 0x0FDC4, false, de0FDC4 }, { 0x0FDC5, false, de0FDC5 }, { 0x0FDC6, false, de0FDC6 },
{ 0x0FDC7, false, de0FDC7 }, { 0x0FDF0, false, de0FDF0 }, { 0x0FDF1, false, de0FDF1 }, { 0x0FDF2, false, de0FDF2 },
{ 0x0FDF3, false, de0FDF3 }, { 0x0FDF4, false, de0FDF4 }, { 0x0FDF5, false, de0FDF5 }, { 0x0FDF6, false, de0FDF6 },
{ 0x0FDF7, false, de0FDF7 }, { 0x0FDF8, false, de0FDF8 }, { 0x0FDF9, false, de0FDF9 }, { 0x0FDFA, false, de0FDFA },
{ 0x0FDFB, false, de0FDFB }, { 0x0FDFC, false, de0FDFC }, { 0x0FE10, false, de0FE10 }, { 0x0FE11, false, de0FE11 },
{ 0x0FE12, false, de0FE12 }, { 0x0FE13, false, de0FE13 }, { 0x0FE14, false, de0FE14 }, { 0x0FE15, false, de0FE15 },
{ 0x0FE16, false, de0FE16 }, { 0x0FE17, false, de0FE17 }, { 0x0FE18, false, de0FE18 }, { 0x0FE19, false, de0FE19 },
{ 0x0FE30, false, de0FE30 }, { 0x0FE31, false, de0FE31 }, { 0x0FE32, false, de0FE32 }, { 0x0FE33, false, de0FE33 },
{ 0x0FE34, false, de0FE34 }, { 0x0FE35, false, de0FE35 }, { 0x0FE36, false, de0FE36 }, { 0x0FE37, false, de0FE37 },
{ 0x0FE38, false, de0FE38 }, { 0x0FE39, false, de0FE39 }, { 0x0FE3A, false, de0FE3A }, { 0x0FE3B, false, de0FE3B },
{ 0x0FE3C, false, de0FE3C }, { 0x0FE3D, false, de0FE3D }, { 0x0FE3E, false, de0FE3E }, { 0x0FE3F, false, de0FE3F },
{ 0x0FE40, false, de0FE40 }, { 0x0FE41, false, de0FE41 }, { 0x0FE42, false, de0FE42 }, { 0x0FE43, false, de0FE43 },
{ 0x0FE44, false, de0FE44 }, { 0x0FE47, false, de0FE47 }, { 0x0FE48, false, de0FE48 }, { 0x0FE49, false, de0FE49 },
{ 0x0FE4A, false, de0FE4A }, { 0x0FE4B, false, de0FE4B }, { 0x0FE4C, false, de0FE4C }, { 0x0FE4D, false, de0FE4D },
{ 0x0FE4E, false, de0FE4E }, { 0x0FE4F, false, de0FE4F }, { 0x0FE50, false, de0FE50 }, { 0x0FE51, false, de0FE51 },
{ 0x0FE52, false, de0FE52 }, { 0x0FE54, false, de0FE54 }, { 0x0FE55, false, de0FE55 }, { 0x0FE56, false, de0FE56 },
{ 0x0FE57, false, de0FE57 }, { 0x0FE58, false, de0FE58 }, { 0x0FE59, false, de0FE59 }, { 0x0FE5A, false, de0FE5A },
{ 0x0FE5B, false, de0FE5B }, { 0x0FE5C, false, de0FE5C }, { 0x0FE5D, false, de0FE5D }, { 0x0FE5E, false, de0FE5E },
{ 0x0FE5F, false, de0FE5F }, { 0x0FE60, false, de0FE60 }, { 0x0FE61, false, de0FE61 }, { 0x0FE62, false, de0FE62 },
{ 0x0FE63, false, de0FE63 }, { 0x0FE64, false, de0FE64 }, { 0x0FE65, false, de0FE65 }, { 0x0FE66, false, de0FE66 },
{ 0x0FE68, false, de0FE68 }, { 0x0FE69, false, de0FE69 }, { 0x0FE6A, false, de0FE6A }, { 0x0FE6B, false, de0FE6B },
{ 0x0FE70, false, de0FE70 }, { 0x0FE71, false, de0FE71 }, { 0x0FE72, false, de0FE72 }, { 0x0FE74, false, de0FE74 },
{ 0x0FE76, false, de0FE76 }, { 0x0FE77, false, de0FE77 }, { 0x0FE78, false, de0FE78 }, { 0x0FE79, false, de0FE79 },
{ 0x0FE7A, false, de0FE7A }, { 0x0FE7B, false, de0FE7B }, { 0x0FE7C, false, de0FE7C }, { 0x0FE7D, false, de0FE7D },
{ 0x0FE7E, false, de0FE7E }, { 0x0FE7F, false, de0FE7F }, { 0x0FE80, false, de0FE80 }, { 0x0FE81, false, de0FE81 },
{ 0x0FE82, false, de0FE82 }, { 0x0FE83, false, de0FE83 }, { 0x0FE84, false, de0FE84 }, { 0x0FE85, false, de0FE85 },
{ 0x0FE86, false, de0FE86 }, { 0x0FE87, false, de0FE87 }, { 0x0FE88, false, de0FE88 }, { 0x0FE89, false, de0FE89 },
{ 0x0FE8A, false, de0FE8A }, { 0x0FE8B, false, de0FE8B }, { 0x0FE8C, false, de0FE8C }, { 0x0FE8D, false, de0FE8D },
{ 0x0FE8E, false, de0FE8E }, { 0x0FE8F, false, de0FE8F }, { 0x0FE90, false, de0FE90 }, { 0x0FE91, false, de0FE91 },
{ 0x0FE92, false, de0FE92 }, { 0x0FE93, false, de0FE93 }, { 0x0FE94, false, de0FE94 }, { 0x0FE95, false, de0FE95 },
{ 0x0FE96, false, de0FE96 }, { 0x0FE97, false, de0FE97 }, { 0x0FE98, false, de0FE98 }, { 0x0FE99, false, de0FE99 },
{ 0x0FE9A, false, de0FE9A }, { 0x0FE9B, false, de0FE9B }, { 0x0FE9C, false, de0FE9C }, { 0x0FE9D, false, de0FE9D },
{ 0x0FE9E, false, de0FE9E }, { 0x0FE9F, false, de0FE9F }, { 0x0FEA0, false, de0FEA0 }, { 0x0FEA1, false, de0FEA1 },
{ 0x0FEA2, false, de0FEA2 }, { 0x0FEA3, false, de0FEA3 }, { 0x0FEA4, false, de0FEA4 }, { 0x0FEA5, false, de0FEA5 },
{ 0x0FEA6, false, de0FEA6 }, { 0x0FEA7, false, de0FEA7 }, { 0x0FEA8, false, de0FEA8 }, { 0x0FEA9, false, de0FEA9 },
{ 0x0FEAA, false, de0FEAA }, { 0x0FEAB, false, de0FEAB }, { 0x0FEAC, false, de0FEAC }, { 0x0FEAD, false, de0FEAD },
{ 0x0FEAE, false, de0FEAE }, { 0x0FEAF, false, de0FEAF }, { 0x0FEB0, false, de0FEB0 }, { 0x0FEB1, false, de0FEB1 },
{ 0x0FEB2, false, de0FEB2 }, { 0x0FEB3, false, de0FEB3 }, { 0x0FEB4, false, de0FEB4 }, { 0x0FEB5, false, de0FEB5 },
{ 0x0FEB6, false, de0FEB6 }, { 0x0FEB7, false, de0FEB7 }, { 0x0FEB8, false, de0FEB8 }, { 0x0FEB9, false, de0FEB9 },
{ 0x0FEBA, false, de0FEBA }, { 0x0FEBB, false, de0FEBB }, { 0x0FEBC, false, de0FEBC }, { 0x0FEBD, false, de0FEBD },
{ 0x0FEBE, false, de0FEBE }, { 0x0FEBF, false, de0FEBF }, { 0x0FEC0, false, de0FEC0 }, { 0x0FEC1, false, de0FEC1 },
{ 0x0FEC2, false, de0FEC2 }, { 0x0FEC3, false, de0FEC3 }, { 0x0FEC4, false, de0FEC4 }, { 0x0FEC5, false, de0FEC5 },
{ 0x0FEC6, false, de0FEC6 }, { 0x0FEC7, false, de0FEC7 }, { 0x0FEC8, false, de0FEC8 }, { 0x0FEC9, false, de0FEC9 },
{ 0x0FECA, false, de0FECA }, { 0x0FECB, false, de0FECB }, { 0x0FECC, false, de0FECC }, { 0x0FECD, false, de0FECD },
{ 0x0FECE, false, de0FECE }, { 0x0FECF, false, de0FECF }, { 0x0FED0, false, de0FED0 }, { 0x0FED1, false, de0FED1 },
{ 0x0FED2, false, de0FED2 }, { 0x0FED3, false, de0FED3 }, { 0x0FED4, false, de0FED4 }, { 0x0FED5, false, de0FED5 },
{ 0x0FED6, false, de0FED6 }, { 0x0FED7, false, de0FED7 }, { 0x0FED8, false, de0FED8 }, { 0x0FED9, false, de0FED9 },
{ 0x0FEDA, false, de0FEDA }, { 0x0FEDB, false, de0FEDB }, { 0x0FEDC, false, de0FEDC }, { 0x0FEDD, false, de0FEDD },
{ 0x0FEDE, false, de0FEDE }, { 0x0FEDF, false, de0FEDF }, { 0x0FEE0, false, de0FEE0 }, { 0x0FEE1, false, de0FEE1 },
{ 0x0FEE2, false, de0FEE2 }, { 0x0FEE3, false, de0FEE3 }, { 0x0FEE4, false, de0FEE4 }, { 0x0FEE5, false, de0FEE5 },
{ 0x0FEE6, false, de0FEE6 }, { 0x0FEE7, false, de0FEE7 }, { 0x0FEE8, false, de0FEE8 }, { 0x0FEE9, false, de0FEE9 },
{ 0x0FEEA, false, de0FEEA }, { 0x0FEEB, false, de0FEEB }, { 0x0FEEC, false, de0FEEC }, { 0x0FEED, false, de0FEED },
{ 0x0FEEE, false, de0FEEE }, { 0x0FEEF, false, de0FEEF }, { 0x0FEF0, false, de0FEF0 }, { 0x0FEF1, false, de0FEF1 },
{ 0x0FEF2, false, de0FEF2 }, { 0x0FEF3, false, de0FEF3 }, { 0x0FEF4, false, de0FEF4 }, { 0x0FEF5, false, de0FEF5 },
{ 0x0FEF6, false, de0FEF6 }, { 0x0FEF7, false, de0FEF7 }, { 0x0FEF8, false, de0FEF8 }, { 0x0FEF9, false, de0FEF9 },
{ 0x0FEFA, false, de0FEFA }, { 0x0FEFB, false, de0FEFB }, { 0x0FEFC, false, de0FEFC }, { 0x0FF01, false, de0FF01 },
{ 0x0FF02, false, de0FF02 }, { 0x0FF03, false, de0FF03 }, { 0x0FF04, false, de0FF04 }, { 0x0FF05, false, de0FF05 },
{ 0x0FF06, false, de0FF06 }, { 0x0FF07, false, de0FF07 }, { 0x0FF08, false, de0FF08 }, { 0x0FF09, false, de0FF09 },
{ 0x0FF0A, false, de0FF0A }, { 0x0FF0B, false, de0FF0B }, { 0x0FF0C, false, de0FF0C }, { 0x0FF0D, false, de0FF0D },
{ 0x0FF0E, false, de0FF0E }, { 0x0FF0F, false, de0FF0F }, { 0x0FF10, false, de0FF10 }, { 0x0FF11, false, de0FF11 },
{ 0x0FF12, false, de0FF12 }, { 0x0FF13, false, de0FF13 }, { 0x0FF14, false, de0FF14 }, { 0x0FF15, false, de0FF15 },
{ 0x0FF16, false, de0FF16 }, { 0x0FF17, false, de0FF17 }, { 0x0FF18, false, de0FF18 }, { 0x0FF19, false, de0FF19 },
{ 0x0FF1A, false, de0FF1A }, { 0x0FF1B, false, de0FF1B }, { 0x0FF1C, false, de0FF1C }, { 0x0FF1D, false, de0FF1D },
{ 0x0FF1E, false, de0FF1E }, { 0x0FF1F, false, de0FF1F }, { 0x0FF20, false, de0FF20 }, { 0x0FF21, false, de0FF21 },
{ 0x0FF22, false, de0FF22 }, { 0x0FF23, false, de0FF23 }, { 0x0FF24, false, de0FF24 }, { 0x0FF25, false, de0FF25 },
{ 0x0FF26, false, de0FF26 }, { 0x0FF27, false, de0FF27 }, { 0x0FF28, false, de0FF28 }, { 0x0FF29, false, de0FF29 },
{ 0x0FF2A, false, de0FF2A }, { 0x0FF2B, false, de0FF2B }, { 0x0FF2C, false, de0FF2C }, { 0x0FF2D, false, de0FF2D },
{ 0x0FF2E, false, de0FF2E }, { 0x0FF2F, false, de0FF2F }, { 0x0FF30, false, de0FF30 }, { 0x0FF31, false, de0FF31 },
{ 0x0FF32, false, de0FF32 }, { 0x0FF33, false, de0FF33 }, { 0x0FF34, false, de0FF34 }, { 0x0FF35, false, de0FF35 },
{ 0x0FF36, false, de0FF36 }, { 0x0FF37, false, de0FF37 }, { 0x0FF38, false, de0FF38 }, { 0x0FF39, false, de0FF39 },
{ 0x0FF3A, false, de0FF3A }, { 0x0FF3B, false, de0FF3B }, { 0x0FF3C, false, de0FF3C }, { 0x0FF3D, false, de0FF3D },
{ 0x0FF3E, false, de0FF3E }, { 0x0FF3F, false, de0FF3F }, { 0x0FF40, false, de0FF40 }, { 0x0FF41, false, de0FF41 },
{ 0x0FF42, false, de0FF42 }, { 0x0FF43, false, de0FF43 }, { 0x0FF44, false, de0FF44 }, { 0x0FF45, false, de0FF45 },
{ 0x0FF46, false, de0FF46 }, { 0x0FF47, false, de0FF47 }, { 0x0FF48, false, de0FF48 }, { 0x0FF49, false, de0FF49 },
{ 0x0FF4A, false, de0FF4A }, { 0x0FF4B, false, de0FF4B }, { 0x0FF4C, false, de0FF4C }, { 0x0FF4D, false, de0FF4D },
{ 0x0FF4E, false, de0FF4E }, { 0x0FF4F, false, de0FF4F }, { 0x0FF50, false, de0FF50 }, { 0x0FF51, false, de0FF51 },
{ 0x0FF52, false, de0FF52 }, { 0x0FF53, false, de0FF53 }, { 0x0FF54, false, de0FF54 }, { 0x0FF55, false, de0FF55 },
{ 0x0FF56, false, de0FF56 }, { 0x0FF57, false, de0FF57 }, { 0x0FF58, false, de0FF58 }, { 0x0FF59, false, de0FF59 },
{ 0x0FF5A, false, de0FF5A }, { 0x0FF5B, false, de0FF5B }, { 0x0FF5C, false, de0FF5C }, { 0x0FF5D, false, de0FF5D },
{ 0x0FF5E, false, de0FF5E }, { 0x0FF5F, false, de0FF5F }, { 0x0FF60, false, de0FF60 }, { 0x0FF61, false, de0FF61 },
{ 0x0FF62, false, de0FF62 }, { 0x0FF63, false, de0FF63 }, { 0x0FF64, false, de0FF64 }, { 0x0FF65, false, de0FF65 },
{ 0x0FF66, false, de0FF66 }, { 0x0FF67, false, de0FF67 }, { 0x0FF68, false, de0FF68 }, { 0x0FF69, false, de0FF69 },
{ 0x0FF6A, false, de0FF6A }, { 0x0FF6B, false, de0FF6B }, { 0x0FF6C, false, de0FF6C }, { 0x0FF6D, false, de0FF6D },
{ 0x0FF6E, false, de0FF6E }, { 0x0FF6F, false, de0FF6F }, { 0x0FF70, false, de0FF70 }, { 0x0FF71, false, de0FF71 },
{ 0x0FF72, false, de0FF72 }, { 0x0FF73, false, de0FF73 }, { 0x0FF74, false, de0FF74 }, { 0x0FF75, false, de0FF75 },
{ 0x0FF76, false, de0FF76 }, { 0x0FF77, false, de0FF77 }, { 0x0FF78, false, de0FF78 }, { 0x0FF79, false, de0FF79 },
{ 0x0FF7A, false, de0FF7A }, { 0x0FF7B, false, de0FF7B }, { 0x0FF7C, false, de0FF7C }, { 0x0FF7D, false, de0FF7D },
{ 0x0FF7E, false, de0FF7E }, { 0x0FF7F, false, de0FF7F }, { 0x0FF80, false, de0FF80 }, { 0x0FF81, false, de0FF81 },
{ 0x0FF82, false, de0FF82 }, { 0x0FF83, false, de0FF83 }, { 0x0FF84, false, de0FF84 }, { 0x0FF85, false, de0FF85 },
{ 0x0FF86, false, de0FF86 }, { 0x0FF87, false, de0FF87 }, { 0x0FF88, false, de0FF88 }, { 0x0FF89, false, de0FF89 },
{ 0x0FF8A, false, de0FF8A }, { 0x0FF8B, false, de0FF8B }, { 0x0FF8C, false, de0FF8C }, { 0x0FF8D, false, de0FF8D },
{ 0x0FF8E, false, de0FF8E }, { 0x0FF8F, false, de0FF8F }, { 0x0FF90, false, de0FF90 }, { 0x0FF91, false, de0FF91 },
{ 0x0FF92, false, de0FF92 }, { 0x0FF93, false, de0FF93 }, { 0x0FF94, false, de0FF94 }, { 0x0FF95, false, de0FF95 },
{ 0x0FF96, false, de0FF96 }, { 0x0FF97, false, de0FF97 }, { 0x0FF98, false, de0FF98 }, { 0x0FF99, false, de0FF99 },
{ 0x0FF9A, false, de0FF9A }, { 0x0FF9B, false, de0FF9B }, { 0x0FF9C, false, de0FF9C }, { 0x0FF9D, false, de0FF9D },
{ 0x0FF9E, false, de0FF9E }, { 0x0FF9F, false, de0FF9F }, { 0x0FFA0, false, de0FFA0 }, { 0x0FFA1, false, de0FFA1 },
{ 0x0FFA2, false, de0FFA2 }, { 0x0FFA3, false, de0FFA3 }, { 0x0FFA4, false, de0FFA4 }, { 0x0FFA5, false, de0FFA5 },
{ 0x0FFA6, false, de0FFA6 }, { 0x0FFA7, false, de0FFA7 }, { 0x0FFA8, false, de0FFA8 }, { 0x0FFA9, false, de0FFA9 },
{ 0x0FFAA, false, de0FFAA }, { 0x0FFAB, false, de0FFAB }, { 0x0FFAC, false, de0FFAC }, { 0x0FFAD, false, de0FFAD },
{ 0x0FFAE, false, de0FFAE }, { 0x0FFAF, false, de0FFAF }, { 0x0FFB0, false, de0FFB0 }, { 0x0FFB1, false, de0FFB1 },
{ 0x0FFB2, false, de0FFB2 }, { 0x0FFB3, false, de0FFB3 }, { 0x0FFB4, false, de0FFB4 }, { 0x0FFB5, false, de0FFB5 },
{ 0x0FFB6, false, de0FFB6 }, { 0x0FFB7, false, de0FFB7 }, { 0x0FFB8, false, de0FFB8 }, { 0x0FFB9, false, de0FFB9 },
{ 0x0FFBA, false, de0FFBA }, { 0x0FFBB, false, de0FFBB }, { 0x0FFBC, false, de0FFBC }, { 0x0FFBD, false, de0FFBD },
{ 0x0FFBE, false, de0FFBE }, { 0x0FFC2, false, de0FFC2 }, { 0x0FFC3, false, de0FFC3 }, { 0x0FFC4, false, de0FFC4 },
{ 0x0FFC5, false, de0FFC5 }, { 0x0FFC6, false, de0FFC6 }, { 0x0FFC7, false, de0FFC7 }, { 0x0FFCA, false, de0FFCA },
{ 0x0FFCB, false, de0FFCB }, { 0x0FFCC, false, de0FFCC }, { 0x0FFCD, false, de0FFCD }, { 0x0FFCE, false, de0FFCE },
{ 0x0FFCF, false, de0FFCF }, { 0x0FFD2, false, de0FFD2 }, { 0x0FFD3, false, de0FFD3 }, { 0x0FFD4, false, de0FFD4 },
{ 0x0FFD5, false, de0FFD5 }, { 0x0FFD6, false, de0FFD6 }, { 0x0FFD7, false, de0FFD7 }, { 0x0FFDA, false, de0FFDA },
{ 0x0FFDB, false, de0FFDB }, { 0x0FFDC, false, de0FFDC }, { 0x0FFE0, false, de0FFE0 }, { 0x0FFE1, false, de0FFE1 },
{ 0x0FFE2, false, de0FFE2 }, { 0x0FFE3, false, de0FFE3 }, { 0x0FFE4, false, de0FFE4 }, { 0x0FFE5, false, de0FFE5 },
{ 0x0FFE6, false, de0FFE6 }, { 0x0FFE8, false, de0FFE8 }, { 0x0FFE9, false, de0FFE9 }, { 0x0FFEA, false, de0FFEA },
{ 0x0FFEB, false, de0FFEB }, { 0x0FFEC, false, de0FFEC }, { 0x0FFED, false, de0FFED }, { 0x0FFEE, false, de0FFEE },
{ 0x1D15E, true, de1D15E }, { 0x1D15F, true, de1D15F }, { 0x1D160, true, de1D160 }, { 0x1D161, true, de1D161 },
{ 0x1D162, true, de1D162 }, { 0x1D163, true, de1D163 }, { 0x1D164, true, de1D164 }, { 0x1D1BB, true, de1D1BB },
{ 0x1D1BC, true, de1D1BC }, { 0x1D1BD, true, de1D1BD }, { 0x1D1BE, true, de1D1BE }, { 0x1D1BF, true, de1D1BF },
{ 0x1D1C0, true, de1D1C0 }, { 0x1D400, false, de1D400 }, { 0x1D401, false, de1D401 }, { 0x1D402, false, de1D402 },
{ 0x1D403, false, de1D403 }, { 0x1D404, false, de1D404 }, { 0x1D405, false, de1D405 }, { 0x1D406, false, de1D406 },
{ 0x1D407, false, de1D407 }, { 0x1D408, false, de1D408 }, { 0x1D409, false, de1D409 }, { 0x1D40A, false, de1D40A },
{ 0x1D40B, false, de1D40B }, { 0x1D40C, false, de1D40C }, { 0x1D40D, false, de1D40D }, { 0x1D40E, false, de1D40E },
{ 0x1D40F, false, de1D40F }, { 0x1D410, false, de1D410 }, { 0x1D411, false, de1D411 }, { 0x1D412, false, de1D412 },
{ 0x1D413, false, de1D413 }, { 0x1D414, false, de1D414 }, { 0x1D415, false, de1D415 }, { 0x1D416, false, de1D416 },
{ 0x1D417, false, de1D417 }, { 0x1D418, false, de1D418 }, { 0x1D419, false, de1D419 }, { 0x1D41A, false, de1D41A },
{ 0x1D41B, false, de1D41B }, { 0x1D41C, false, de1D41C }, { 0x1D41D, false, de1D41D }, { 0x1D41E, false, de1D41E },
{ 0x1D41F, false, de1D41F }, { 0x1D420, false, de1D420 }, { 0x1D421, false, de1D421 }, { 0x1D422, false, de1D422 },
{ 0x1D423, false, de1D423 }, { 0x1D424, false, de1D424 }, { 0x1D425, false, de1D425 }, { 0x1D426, false, de1D426 },
{ 0x1D427, false, de1D427 }, { 0x1D428, false, de1D428 }, { 0x1D429, false, de1D429 }, { 0x1D42A, false, de1D42A },
{ 0x1D42B, false, de1D42B }, { 0x1D42C, false, de1D42C }, { 0x1D42D, false, de1D42D }, { 0x1D42E, false, de1D42E },
{ 0x1D42F, false, de1D42F }, { 0x1D430, false, de1D430 }, { 0x1D431, false, de1D431 }, { 0x1D432, false, de1D432 },
{ 0x1D433, false, de1D433 }, { 0x1D434, false, de1D434 }, { 0x1D435, false, de1D435 }, { 0x1D436, false, de1D436 },
{ 0x1D437, false, de1D437 }, { 0x1D438, false, de1D438 }, { 0x1D439, false, de1D439 }, { 0x1D43A, false, de1D43A },
{ 0x1D43B, false, de1D43B }, { 0x1D43C, false, de1D43C }, { 0x1D43D, false, de1D43D }, { 0x1D43E, false, de1D43E },
{ 0x1D43F, false, de1D43F }, { 0x1D440, false, de1D440 }, { 0x1D441, false, de1D441 }, { 0x1D442, false, de1D442 },
{ 0x1D443, false, de1D443 }, { 0x1D444, false, de1D444 }, { 0x1D445, false, de1D445 }, { 0x1D446, false, de1D446 },
{ 0x1D447, false, de1D447 }, { 0x1D448, false, de1D448 }, { 0x1D449, false, de1D449 }, { 0x1D44A, false, de1D44A },
{ 0x1D44B, false, de1D44B }, { 0x1D44C, false, de1D44C }, { 0x1D44D, false, de1D44D }, { 0x1D44E, false, de1D44E },
{ 0x1D44F, false, de1D44F }, { 0x1D450, false, de1D450 }, { 0x1D451, false, de1D451 }, { 0x1D452, false, de1D452 },
{ 0x1D453, false, de1D453 }, { 0x1D454, false, de1D454 }, { 0x1D456, false, de1D456 }, { 0x1D457, false, de1D457 },
{ 0x1D458, false, de1D458 }, { 0x1D459, false, de1D459 }, { 0x1D45A, false, de1D45A }, { 0x1D45B, false, de1D45B },
{ 0x1D45C, false, de1D45C }, { 0x1D45D, false, de1D45D }, { 0x1D45E, false, de1D45E }, { 0x1D45F, false, de1D45F },
{ 0x1D460, false, de1D460 }, { 0x1D461, false, de1D461 }, { 0x1D462, false, de1D462 }, { 0x1D463, false, de1D463 },
{ 0x1D464, false, de1D464 }, { 0x1D465, false, de1D465 }, { 0x1D466, false, de1D466 }, { 0x1D467, false, de1D467 },
{ 0x1D468, false, de1D468 }, { 0x1D469, false, de1D469 }, { 0x1D46A, false, de1D46A }, { 0x1D46B, false, de1D46B },
{ 0x1D46C, false, de1D46C }, { 0x1D46D, false, de1D46D }, { 0x1D46E, false, de1D46E }, { 0x1D46F, false, de1D46F },
{ 0x1D470, false, de1D470 }, { 0x1D471, false, de1D471 }, { 0x1D472, false, de1D472 }, { 0x1D473, false, de1D473 },
{ 0x1D474, false, de1D474 }, { 0x1D475, false, de1D475 }, { 0x1D476, false, de1D476 }, { 0x1D477, false, de1D477 },
{ 0x1D478, false, de1D478 }, { 0x1D479, false, de1D479 }, { 0x1D47A, false, de1D47A }, { 0x1D47B, false, de1D47B },
{ 0x1D47C, false, de1D47C }, { 0x1D47D, false, de1D47D }, { 0x1D47E, false, de1D47E }, { 0x1D47F, false, de1D47F },
{ 0x1D480, false, de1D480 }, { 0x1D481, false, de1D481 }, { 0x1D482, false, de1D482 }, { 0x1D483, false, de1D483 },
{ 0x1D484, false, de1D484 }, { 0x1D485, false, de1D485 }, { 0x1D486, false, de1D486 }, { 0x1D487, false, de1D487 },
{ 0x1D488, false, de1D488 }, { 0x1D489, false, de1D489 }, { 0x1D48A, false, de1D48A }, { 0x1D48B, false, de1D48B },
{ 0x1D48C, false, de1D48C }, { 0x1D48D, false, de1D48D }, { 0x1D48E, false, de1D48E }, { 0x1D48F, false, de1D48F },
{ 0x1D490, false, de1D490 }, { 0x1D491, false, de1D491 }, { 0x1D492, false, de1D492 }, { 0x1D493, false, de1D493 },
{ 0x1D494, false, de1D494 }, { 0x1D495, false, de1D495 }, { 0x1D496, false, de1D496 }, { 0x1D497, false, de1D497 },
{ 0x1D498, false, de1D498 }, { 0x1D499, false, de1D499 }, { 0x1D49A, false, de1D49A }, { 0x1D49B, false, de1D49B },
{ 0x1D49C, false, de1D49C }, { 0x1D49E, false, de1D49E }, { 0x1D49F, false, de1D49F }, { 0x1D4A2, false, de1D4A2 },
{ 0x1D4A5, false, de1D4A5 }, { 0x1D4A6, false, de1D4A6 }, { 0x1D4A9, false, de1D4A9 }, { 0x1D4AA, false, de1D4AA },
{ 0x1D4AB, false, de1D4AB }, { 0x1D4AC, false, de1D4AC }, { 0x1D4AE, false, de1D4AE }, { 0x1D4AF, false, de1D4AF },
{ 0x1D4B0, false, de1D4B0 }, { 0x1D4B1, false, de1D4B1 }, { 0x1D4B2, false, de1D4B2 }, { 0x1D4B3, false, de1D4B3 },
{ 0x1D4B4, false, de1D4B4 }, { 0x1D4B5, false, de1D4B5 }, { 0x1D4B6, false, de1D4B6 }, { 0x1D4B7, false, de1D4B7 },
{ 0x1D4B8, false, de1D4B8 }, { 0x1D4B9, false, de1D4B9 }, { 0x1D4BB, false, de1D4BB }, { 0x1D4BD, false, de1D4BD },
{ 0x1D4BE, false, de1D4BE }, { 0x1D4BF, false, de1D4BF }, { 0x1D4C0, false, de1D4C0 }, { 0x1D4C1, false, de1D4C1 },
{ 0x1D4C2, false, de1D4C2 }, { 0x1D4C3, false, de1D4C3 }, { 0x1D4C5, false, de1D4C5 }, { 0x1D4C6, false, de1D4C6 },
{ 0x1D4C7, false, de1D4C7 }, { 0x1D4C8, false, de1D4C8 }, { 0x1D4C9, false, de1D4C9 }, { 0x1D4CA, false, de1D4CA },
{ 0x1D4CB, false, de1D4CB }, { 0x1D4CC, false, de1D4CC }, { 0x1D4CD, false, de1D4CD }, { 0x1D4CE, false, de1D4CE },
{ 0x1D4CF, false, de1D4CF }, { 0x1D4D0, false, de1D4D0 }, { 0x1D4D1, false, de1D4D1 }, { 0x1D4D2, false, de1D4D2 },
{ 0x1D4D3, false, de1D4D3 }, { 0x1D4D4, false, de1D4D4 }, { 0x1D4D5, false, de1D4D5 }, { 0x1D4D6, false, de1D4D6 },
{ 0x1D4D7, false, de1D4D7 }, { 0x1D4D8, false, de1D4D8 }, { 0x1D4D9, false, de1D4D9 }, { 0x1D4DA, false, de1D4DA },
{ 0x1D4DB, false, de1D4DB }, { 0x1D4DC, false, de1D4DC }, { 0x1D4DD, false, de1D4DD }, { 0x1D4DE, false, de1D4DE },
{ 0x1D4DF, false, de1D4DF }, { 0x1D4E0, false, de1D4E0 }, { 0x1D4E1, false, de1D4E1 }, { 0x1D4E2, false, de1D4E2 },
{ 0x1D4E3, false, de1D4E3 }, { 0x1D4E4, false, de1D4E4 }, { 0x1D4E5, false, de1D4E5 }, { 0x1D4E6, false, de1D4E6 },
{ 0x1D4E7, false, de1D4E7 }, { 0x1D4E8, false, de1D4E8 }, { 0x1D4E9, false, de1D4E9 }, { 0x1D4EA, false, de1D4EA },
{ 0x1D4EB, false, de1D4EB }, { 0x1D4EC, false, de1D4EC }, { 0x1D4ED, false, de1D4ED }, { 0x1D4EE, false, de1D4EE },
{ 0x1D4EF, false, de1D4EF }, { 0x1D4F0, false, de1D4F0 }, { 0x1D4F1, false, de1D4F1 }, { 0x1D4F2, false, de1D4F2 },
{ 0x1D4F3, false, de1D4F3 }, { 0x1D4F4, false, de1D4F4 }, { 0x1D4F5, false, de1D4F5 }, { 0x1D4F6, false, de1D4F6 },
{ 0x1D4F7, false, de1D4F7 }, { 0x1D4F8, false, de1D4F8 }, { 0x1D4F9, false, de1D4F9 }, { 0x1D4FA, false, de1D4FA },
{ 0x1D4FB, false, de1D4FB }, { 0x1D4FC, false, de1D4FC }, { 0x1D4FD, false, de1D4FD }, { 0x1D4FE, false, de1D4FE },
{ 0x1D4FF, false, de1D4FF }, { 0x1D500, false, de1D500 }, { 0x1D501, false, de1D501 }, { 0x1D502, false, de1D502 },
{ 0x1D503, false, de1D503 }, { 0x1D504, false, de1D504 }, { 0x1D505, false, de1D505 }, { 0x1D507, false, de1D507 },
{ 0x1D508, false, de1D508 }, { 0x1D509, false, de1D509 }, { 0x1D50A, false, de1D50A }, { 0x1D50D, false, de1D50D },
{ 0x1D50E, false, de1D50E }, { 0x1D50F, false, de1D50F }, { 0x1D510, false, de1D510 }, { 0x1D511, false, de1D511 },
{ 0x1D512, false, de1D512 }, { 0x1D513, false, de1D513 }, { 0x1D514, false, de1D514 }, { 0x1D516, false, de1D516 },
{ 0x1D517, false, de1D517 }, { 0x1D518, false, de1D518 }, { 0x1D519, false, de1D519 }, { 0x1D51A, false, de1D51A },
{ 0x1D51B, false, de1D51B }, { 0x1D51C, false, de1D51C }, { 0x1D51E, false, de1D51E }, { 0x1D51F, false, de1D51F },
{ 0x1D520, false, de1D520 }, { 0x1D521, false, de1D521 }, { 0x1D522, false, de1D522 }, { 0x1D523, false, de1D523 },
{ 0x1D524, false, de1D524 }, { 0x1D525, false, de1D525 }, { 0x1D526, false, de1D526 }, { 0x1D527, false, de1D527 },
{ 0x1D528, false, de1D528 }, { 0x1D529, false, de1D529 }, { 0x1D52A, false, de1D52A }, { 0x1D52B, false, de1D52B },
{ 0x1D52C, false, de1D52C }, { 0x1D52D, false, de1D52D }, { 0x1D52E, false, de1D52E }, { 0x1D52F, false, de1D52F },
{ 0x1D530, false, de1D530 }, { 0x1D531, false, de1D531 }, { 0x1D532, false, de1D532 }, { 0x1D533, false, de1D533 },
{ 0x1D534, false, de1D534 }, { 0x1D535, false, de1D535 }, { 0x1D536, false, de1D536 }, { 0x1D537, false, de1D537 },
{ 0x1D538, false, de1D538 }, { 0x1D539, false, de1D539 }, { 0x1D53B, false, de1D53B }, { 0x1D53C, false, de1D53C },
{ 0x1D53D, false, de1D53D }, { 0x1D53E, false, de1D53E }, { 0x1D540, false, de1D540 }, { 0x1D541, false, de1D541 },
{ 0x1D542, false, de1D542 }, { 0x1D543, false, de1D543 }, { 0x1D544, false, de1D544 }, { 0x1D546, false, de1D546 },
{ 0x1D54A, false, de1D54A }, { 0x1D54B, false, de1D54B }, { 0x1D54C, false, de1D54C }, { 0x1D54D, false, de1D54D },
{ 0x1D54E, false, de1D54E }, { 0x1D54F, false, de1D54F }, { 0x1D550, false, de1D550 }, { 0x1D552, false, de1D552 },
{ 0x1D553, false, de1D553 }, { 0x1D554, false, de1D554 }, { 0x1D555, false, de1D555 }, { 0x1D556, false, de1D556 },
{ 0x1D557, false, de1D557 }, { 0x1D558, false, de1D558 }, { 0x1D559, false, de1D559 }, { 0x1D55A, false, de1D55A },
{ 0x1D55B, false, de1D55B }, { 0x1D55C, false, de1D55C }, { 0x1D55D, false, de1D55D }, { 0x1D55E, false, de1D55E },
{ 0x1D55F, false, de1D55F }, { 0x1D560, false, de1D560 }, { 0x1D561, false, de1D561 }, { 0x1D562, false, de1D562 },
{ 0x1D563, false, de1D563 }, { 0x1D564, false, de1D564 }, { 0x1D565, false, de1D565 }, { 0x1D566, false, de1D566 },
{ 0x1D567, false, de1D567 }, { 0x1D568, false, de1D568 }, { 0x1D569, false, de1D569 }, { 0x1D56A, false, de1D56A },
{ 0x1D56B, false, de1D56B }, { 0x1D56C, false, de1D56C }, { 0x1D56D, false, de1D56D }, { 0x1D56E, false, de1D56E },
{ 0x1D56F, false, de1D56F }, { 0x1D570, false, de1D570 }, { 0x1D571, false, de1D571 }, { 0x1D572, false, de1D572 },
{ 0x1D573, false, de1D573 }, { 0x1D574, false, de1D574 }, { 0x1D575, false, de1D575 }, { 0x1D576, false, de1D576 },
{ 0x1D577, false, de1D577 }, { 0x1D578, false, de1D578 }, { 0x1D579, false, de1D579 }, { 0x1D57A, false, de1D57A },
{ 0x1D57B, false, de1D57B }, { 0x1D57C, false, de1D57C }, { 0x1D57D, false, de1D57D }, { 0x1D57E, false, de1D57E },
{ 0x1D57F, false, de1D57F }, { 0x1D580, false, de1D580 }, { 0x1D581, false, de1D581 }, { 0x1D582, false, de1D582 },
{ 0x1D583, false, de1D583 }, { 0x1D584, false, de1D584 }, { 0x1D585, false, de1D585 }, { 0x1D586, false, de1D586 },
{ 0x1D587, false, de1D587 }, { 0x1D588, false, de1D588 }, { 0x1D589, false, de1D589 }, { 0x1D58A, false, de1D58A },
{ 0x1D58B, false, de1D58B }, { 0x1D58C, false, de1D58C }, { 0x1D58D, false, de1D58D }, { 0x1D58E, false, de1D58E },
{ 0x1D58F, false, de1D58F }, { 0x1D590, false, de1D590 }, { 0x1D591, false, de1D591 }, { 0x1D592, false, de1D592 },
{ 0x1D593, false, de1D593 }, { 0x1D594, false, de1D594 }, { 0x1D595, false, de1D595 }, { 0x1D596, false, de1D596 },
{ 0x1D597, false, de1D597 }, { 0x1D598, false, de1D598 }, { 0x1D599, false, de1D599 }, { 0x1D59A, false, de1D59A },
{ 0x1D59B, false, de1D59B }, { 0x1D59C, false, de1D59C }, { 0x1D59D, false, de1D59D }, { 0x1D59E, false, de1D59E },
{ 0x1D59F, false, de1D59F }, { 0x1D5A0, false, de1D5A0 }, { 0x1D5A1, false, de1D5A1 }, { 0x1D5A2, false, de1D5A2 },
{ 0x1D5A3, false, de1D5A3 }, { 0x1D5A4, false, de1D5A4 }, { 0x1D5A5, false, de1D5A5 }, { 0x1D5A6, false, de1D5A6 },
{ 0x1D5A7, false, de1D5A7 }, { 0x1D5A8, false, de1D5A8 }, { 0x1D5A9, false, de1D5A9 }, { 0x1D5AA, false, de1D5AA },
{ 0x1D5AB, false, de1D5AB }, { 0x1D5AC, false, de1D5AC }, { 0x1D5AD, false, de1D5AD }, { 0x1D5AE, false, de1D5AE },
{ 0x1D5AF, false, de1D5AF }, { 0x1D5B0, false, de1D5B0 }, { 0x1D5B1, false, de1D5B1 }, { 0x1D5B2, false, de1D5B2 },
{ 0x1D5B3, false, de1D5B3 }, { 0x1D5B4, false, de1D5B4 }, { 0x1D5B5, false, de1D5B5 }, { 0x1D5B6, false, de1D5B6 },
{ 0x1D5B7, false, de1D5B7 }, { 0x1D5B8, false, de1D5B8 }, { 0x1D5B9, false, de1D5B9 }, { 0x1D5BA, false, de1D5BA },
{ 0x1D5BB, false, de1D5BB }, { 0x1D5BC, false, de1D5BC }, { 0x1D5BD, false, de1D5BD }, { 0x1D5BE, false, de1D5BE },
{ 0x1D5BF, false, de1D5BF }, { 0x1D5C0, false, de1D5C0 }, { 0x1D5C1, false, de1D5C1 }, { 0x1D5C2, false, de1D5C2 },
{ 0x1D5C3, false, de1D5C3 }, { 0x1D5C4, false, de1D5C4 }, { 0x1D5C5, false, de1D5C5 }, { 0x1D5C6, false, de1D5C6 },
{ 0x1D5C7, false, de1D5C7 }, { 0x1D5C8, false, de1D5C8 }, { 0x1D5C9, false, de1D5C9 }, { 0x1D5CA, false, de1D5CA },
{ 0x1D5CB, false, de1D5CB }, { 0x1D5CC, false, de1D5CC }, { 0x1D5CD, false, de1D5CD }, { 0x1D5CE, false, de1D5CE },
{ 0x1D5CF, false, de1D5CF }, { 0x1D5D0, false, de1D5D0 }, { 0x1D5D1, false, de1D5D1 }, { 0x1D5D2, false, de1D5D2 },
{ 0x1D5D3, false, de1D5D3 }, { 0x1D5D4, false, de1D5D4 }, { 0x1D5D5, false, de1D5D5 }, { 0x1D5D6, false, de1D5D6 },
{ 0x1D5D7, false, de1D5D7 }, { 0x1D5D8, false, de1D5D8 }, { 0x1D5D9, false, de1D5D9 }, { 0x1D5DA, false, de1D5DA },
{ 0x1D5DB, false, de1D5DB }, { 0x1D5DC, false, de1D5DC }, { 0x1D5DD, false, de1D5DD }, { 0x1D5DE, false, de1D5DE },
{ 0x1D5DF, false, de1D5DF }, { 0x1D5E0, false, de1D5E0 }, { 0x1D5E1, false, de1D5E1 }, { 0x1D5E2, false, de1D5E2 },
{ 0x1D5E3, false, de1D5E3 }, { 0x1D5E4, false, de1D5E4 }, { 0x1D5E5, false, de1D5E5 }, { 0x1D5E6, false, de1D5E6 },
{ 0x1D5E7, false, de1D5E7 }, { 0x1D5E8, false, de1D5E8 }, { 0x1D5E9, false, de1D5E9 }, { 0x1D5EA, false, de1D5EA },
{ 0x1D5EB, false, de1D5EB }, { 0x1D5EC, false, de1D5EC }, { 0x1D5ED, false, de1D5ED }, { 0x1D5EE, false, de1D5EE },
{ 0x1D5EF, false, de1D5EF }, { 0x1D5F0, false, de1D5F0 }, { 0x1D5F1, false, de1D5F1 }, { 0x1D5F2, false, de1D5F2 },
{ 0x1D5F3, false, de1D5F3 }, { 0x1D5F4, false, de1D5F4 }, { 0x1D5F5, false, de1D5F5 }, { 0x1D5F6, false, de1D5F6 },
{ 0x1D5F7, false, de1D5F7 }, { 0x1D5F8, false, de1D5F8 }, { 0x1D5F9, false, de1D5F9 }, { 0x1D5FA, false, de1D5FA },
{ 0x1D5FB, false, de1D5FB }, { 0x1D5FC, false, de1D5FC }, { 0x1D5FD, false, de1D5FD }, { 0x1D5FE, false, de1D5FE },
{ 0x1D5FF, false, de1D5FF }, { 0x1D600, false, de1D600 }, { 0x1D601, false, de1D601 }, { 0x1D602, false, de1D602 },
{ 0x1D603, false, de1D603 }, { 0x1D604, false, de1D604 }, { 0x1D605, false, de1D605 }, { 0x1D606, false, de1D606 },
{ 0x1D607, false, de1D607 }, { 0x1D608, false, de1D608 }, { 0x1D609, false, de1D609 }, { 0x1D60A, false, de1D60A },
{ 0x1D60B, false, de1D60B }, { 0x1D60C, false, de1D60C }, { 0x1D60D, false, de1D60D }, { 0x1D60E, false, de1D60E },
{ 0x1D60F, false, de1D60F }, { 0x1D610, false, de1D610 }, { 0x1D611, false, de1D611 }, { 0x1D612, false, de1D612 },
{ 0x1D613, false, de1D613 }, { 0x1D614, false, de1D614 }, { 0x1D615, false, de1D615 }, { 0x1D616, false, de1D616 },
{ 0x1D617, false, de1D617 }, { 0x1D618, false, de1D618 }, { 0x1D619, false, de1D619 }, { 0x1D61A, false, de1D61A },
{ 0x1D61B, false, de1D61B }, { 0x1D61C, false, de1D61C }, { 0x1D61D, false, de1D61D }, { 0x1D61E, false, de1D61E },
{ 0x1D61F, false, de1D61F }, { 0x1D620, false, de1D620 }, { 0x1D621, false, de1D621 }, { 0x1D622, false, de1D622 },
{ 0x1D623, false, de1D623 }, { 0x1D624, false, de1D624 }, { 0x1D625, false, de1D625 }, { 0x1D626, false, de1D626 },
{ 0x1D627, false, de1D627 }, { 0x1D628, false, de1D628 }, { 0x1D629, false, de1D629 }, { 0x1D62A, false, de1D62A },
{ 0x1D62B, false, de1D62B }, { 0x1D62C, false, de1D62C }, { 0x1D62D, false, de1D62D }, { 0x1D62E, false, de1D62E },
{ 0x1D62F, false, de1D62F }, { 0x1D630, false, de1D630 }, { 0x1D631, false, de1D631 }, { 0x1D632, false, de1D632 },
{ 0x1D633, false, de1D633 }, { 0x1D634, false, de1D634 }, { 0x1D635, false, de1D635 }, { 0x1D636, false, de1D636 },
{ 0x1D637, false, de1D637 }, { 0x1D638, false, de1D638 }, { 0x1D639, false, de1D639 }, { 0x1D63A, false, de1D63A },
{ 0x1D63B, false, de1D63B }, { 0x1D63C, false, de1D63C }, { 0x1D63D, false, de1D63D }, { 0x1D63E, false, de1D63E },
{ 0x1D63F, false, de1D63F }, { 0x1D640, false, de1D640 }, { 0x1D641, false, de1D641 }, { 0x1D642, false, de1D642 },
{ 0x1D643, false, de1D643 }, { 0x1D644, false, de1D644 }, { 0x1D645, false, de1D645 }, { 0x1D646, false, de1D646 },
{ 0x1D647, false, de1D647 }, { 0x1D648, false, de1D648 }, { 0x1D649, false, de1D649 }, { 0x1D64A, false, de1D64A },
{ 0x1D64B, false, de1D64B }, { 0x1D64C, false, de1D64C }, { 0x1D64D, false, de1D64D }, { 0x1D64E, false, de1D64E },
{ 0x1D64F, false, de1D64F }, { 0x1D650, false, de1D650 }, { 0x1D651, false, de1D651 }, { 0x1D652, false, de1D652 },
{ 0x1D653, false, de1D653 }, { 0x1D654, false, de1D654 }, { 0x1D655, false, de1D655 }, { 0x1D656, false, de1D656 },
{ 0x1D657, false, de1D657 }, { 0x1D658, false, de1D658 }, { 0x1D659, false, de1D659 }, { 0x1D65A, false, de1D65A },
{ 0x1D65B, false, de1D65B }, { 0x1D65C, false, de1D65C }, { 0x1D65D, false, de1D65D }, { 0x1D65E, false, de1D65E },
{ 0x1D65F, false, de1D65F }, { 0x1D660, false, de1D660 }, { 0x1D661, false, de1D661 }, { 0x1D662, false, de1D662 },
{ 0x1D663, false, de1D663 }, { 0x1D664, false, de1D664 }, { 0x1D665, false, de1D665 }, { 0x1D666, false, de1D666 },
{ 0x1D667, false, de1D667 }, { 0x1D668, false, de1D668 }, { 0x1D669, false, de1D669 }, { 0x1D66A, false, de1D66A },
{ 0x1D66B, false, de1D66B }, { 0x1D66C, false, de1D66C }, { 0x1D66D, false, de1D66D }, { 0x1D66E, false, de1D66E },
{ 0x1D66F, false, de1D66F }, { 0x1D670, false, de1D670 }, { 0x1D671, false, de1D671 }, { 0x1D672, false, de1D672 },
{ 0x1D673, false, de1D673 }, { 0x1D674, false, de1D674 }, { 0x1D675, false, de1D675 }, { 0x1D676, false, de1D676 },
{ 0x1D677, false, de1D677 }, { 0x1D678, false, de1D678 }, { 0x1D679, false, de1D679 }, { 0x1D67A, false, de1D67A },
{ 0x1D67B, false, de1D67B }, { 0x1D67C, false, de1D67C }, { 0x1D67D, false, de1D67D }, { 0x1D67E, false, de1D67E },
{ 0x1D67F, false, de1D67F }, { 0x1D680, false, de1D680 }, { 0x1D681, false, de1D681 }, { 0x1D682, false, de1D682 },
{ 0x1D683, false, de1D683 }, { 0x1D684, false, de1D684 }, { 0x1D685, false, de1D685 }, { 0x1D686, false, de1D686 },
{ 0x1D687, false, de1D687 }, { 0x1D688, false, de1D688 }, { 0x1D689, false, de1D689 }, { 0x1D68A, false, de1D68A },
{ 0x1D68B, false, de1D68B }, { 0x1D68C, false, de1D68C }, { 0x1D68D, false, de1D68D }, { 0x1D68E, false, de1D68E },
{ 0x1D68F, false, de1D68F }, { 0x1D690, false, de1D690 }, { 0x1D691, false, de1D691 }, { 0x1D692, false, de1D692 },
{ 0x1D693, false, de1D693 }, { 0x1D694, false, de1D694 }, { 0x1D695, false, de1D695 }, { 0x1D696, false, de1D696 },
{ 0x1D697, false, de1D697 }, { 0x1D698, false, de1D698 }, { 0x1D699, false, de1D699 }, { 0x1D69A, false, de1D69A },
{ 0x1D69B, false, de1D69B }, { 0x1D69C, false, de1D69C }, { 0x1D69D, false, de1D69D }, { 0x1D69E, false, de1D69E },
{ 0x1D69F, false, de1D69F }, { 0x1D6A0, false, de1D6A0 }, { 0x1D6A1, false, de1D6A1 }, { 0x1D6A2, false, de1D6A2 },
{ 0x1D6A3, false, de1D6A3 }, { 0x1D6A4, false, de1D6A4 }, { 0x1D6A5, false, de1D6A5 }, { 0x1D6A8, false, de1D6A8 },
{ 0x1D6A9, false, de1D6A9 }, { 0x1D6AA, false, de1D6AA }, { 0x1D6AB, false, de1D6AB }, { 0x1D6AC, false, de1D6AC },
{ 0x1D6AD, false, de1D6AD }, { 0x1D6AE, false, de1D6AE }, { 0x1D6AF, false, de1D6AF }, { 0x1D6B0, false, de1D6B0 },
{ 0x1D6B1, false, de1D6B1 }, { 0x1D6B2, false, de1D6B2 }, { 0x1D6B3, false, de1D6B3 }, { 0x1D6B4, false, de1D6B4 },
{ 0x1D6B5, false, de1D6B5 }, { 0x1D6B6, false, de1D6B6 }, { 0x1D6B7, false, de1D6B7 }, { 0x1D6B8, false, de1D6B8 },
{ 0x1D6B9, false, de1D6B9 }, { 0x1D6BA, false, de1D6BA }, { 0x1D6BB, false, de1D6BB }, { 0x1D6BC, false, de1D6BC },
{ 0x1D6BD, false, de1D6BD }, { 0x1D6BE, false, de1D6BE }, { 0x1D6BF, false, de1D6BF }, { 0x1D6C0, false, de1D6C0 },
{ 0x1D6C1, false, de1D6C1 }, { 0x1D6C2, false, de1D6C2 }, { 0x1D6C3, false, de1D6C3 }, { 0x1D6C4, false, de1D6C4 },
{ 0x1D6C5, false, de1D6C5 }, { 0x1D6C6, false, de1D6C6 }, { 0x1D6C7, false, de1D6C7 }, { 0x1D6C8, false, de1D6C8 },
{ 0x1D6C9, false, de1D6C9 }, { 0x1D6CA, false, de1D6CA }, { 0x1D6CB, false, de1D6CB }, { 0x1D6CC, false, de1D6CC },
{ 0x1D6CD, false, de1D6CD }, { 0x1D6CE, false, de1D6CE }, { 0x1D6CF, false, de1D6CF }, { 0x1D6D0, false, de1D6D0 },
{ 0x1D6D1, false, de1D6D1 }, { 0x1D6D2, false, de1D6D2 }, { 0x1D6D3, false, de1D6D3 }, { 0x1D6D4, false, de1D6D4 },
{ 0x1D6D5, false, de1D6D5 }, { 0x1D6D6, false, de1D6D6 }, { 0x1D6D7, false, de1D6D7 }, { 0x1D6D8, false, de1D6D8 },
{ 0x1D6D9, false, de1D6D9 }, { 0x1D6DA, false, de1D6DA }, { 0x1D6DB, false, de1D6DB }, { 0x1D6DC, false, de1D6DC },
{ 0x1D6DD, false, de1D6DD }, { 0x1D6DE, false, de1D6DE }, { 0x1D6DF, false, de1D6DF }, { 0x1D6E0, false, de1D6E0 },
{ 0x1D6E1, false, de1D6E1 }, { 0x1D6E2, false, de1D6E2 }, { 0x1D6E3, false, de1D6E3 }, { 0x1D6E4, false, de1D6E4 },
{ 0x1D6E5, false, de1D6E5 }, { 0x1D6E6, false, de1D6E6 }, { 0x1D6E7, false, de1D6E7 }, { 0x1D6E8, false, de1D6E8 },
{ 0x1D6E9, false, de1D6E9 }, { 0x1D6EA, false, de1D6EA }, { 0x1D6EB, false, de1D6EB }, { 0x1D6EC, false, de1D6EC },
{ 0x1D6ED, false, de1D6ED }, { 0x1D6EE, false, de1D6EE }, { 0x1D6EF, false, de1D6EF }, { 0x1D6F0, false, de1D6F0 },
{ 0x1D6F1, false, de1D6F1 }, { 0x1D6F2, false, de1D6F2 }, { 0x1D6F3, false, de1D6F3 }, { 0x1D6F4, false, de1D6F4 },
{ 0x1D6F5, false, de1D6F5 }, { 0x1D6F6, false, de1D6F6 }, { 0x1D6F7, false, de1D6F7 }, { 0x1D6F8, false, de1D6F8 },
{ 0x1D6F9, false, de1D6F9 }, { 0x1D6FA, false, de1D6FA }, { 0x1D6FB, false, de1D6FB }, { 0x1D6FC, false, de1D6FC },
{ 0x1D6FD, false, de1D6FD }, { 0x1D6FE, false, de1D6FE }, { 0x1D6FF, false, de1D6FF }, { 0x1D700, false, de1D700 },
{ 0x1D701, false, de1D701 }, { 0x1D702, false, de1D702 }, { 0x1D703, false, de1D703 }, { 0x1D704, false, de1D704 },
{ 0x1D705, false, de1D705 }, { 0x1D706, false, de1D706 }, { 0x1D707, false, de1D707 }, { 0x1D708, false, de1D708 },
{ 0x1D709, false, de1D709 }, { 0x1D70A, false, de1D70A }, { 0x1D70B, false, de1D70B }, { 0x1D70C, false, de1D70C },
{ 0x1D70D, false, de1D70D }, { 0x1D70E, false, de1D70E }, { 0x1D70F, false, de1D70F }, { 0x1D710, false, de1D710 },
{ 0x1D711, false, de1D711 }, { 0x1D712, false, de1D712 }, { 0x1D713, false, de1D713 }, { 0x1D714, false, de1D714 },
{ 0x1D715, false, de1D715 }, { 0x1D716, false, de1D716 }, { 0x1D717, false, de1D717 }, { 0x1D718, false, de1D718 },
{ 0x1D719, false, de1D719 }, { 0x1D71A, false, de1D71A }, { 0x1D71B, false, de1D71B }, { 0x1D71C, false, de1D71C },
{ 0x1D71D, false, de1D71D }, { 0x1D71E, false, de1D71E }, { 0x1D71F, false, de1D71F }, { 0x1D720, false, de1D720 },
{ 0x1D721, false, de1D721 }, { 0x1D722, false, de1D722 }, { 0x1D723, false, de1D723 }, { 0x1D724, false, de1D724 },
{ 0x1D725, false, de1D725 }, { 0x1D726, false, de1D726 }, { 0x1D727, false, de1D727 }, { 0x1D728, false, de1D728 },
{ 0x1D729, false, de1D729 }, { 0x1D72A, false, de1D72A }, { 0x1D72B, false, de1D72B }, { 0x1D72C, false, de1D72C },
{ 0x1D72D, false, de1D72D }, { 0x1D72E, false, de1D72E }, { 0x1D72F, false, de1D72F }, { 0x1D730, false, de1D730 },
{ 0x1D731, false, de1D731 }, { 0x1D732, false, de1D732 }, { 0x1D733, false, de1D733 }, { 0x1D734, false, de1D734 },
{ 0x1D735, false, de1D735 }, { 0x1D736, false, de1D736 }, { 0x1D737, false, de1D737 }, { 0x1D738, false, de1D738 },
{ 0x1D739, false, de1D739 }, { 0x1D73A, false, de1D73A }, { 0x1D73B, false, de1D73B }, { 0x1D73C, false, de1D73C },
{ 0x1D73D, false, de1D73D }, { 0x1D73E, false, de1D73E }, { 0x1D73F, false, de1D73F }, { 0x1D740, false, de1D740 },
{ 0x1D741, false, de1D741 }, { 0x1D742, false, de1D742 }, { 0x1D743, false, de1D743 }, { 0x1D744, false, de1D744 },
{ 0x1D745, false, de1D745 }, { 0x1D746, false, de1D746 }, { 0x1D747, false, de1D747 }, { 0x1D748, false, de1D748 },
{ 0x1D749, false, de1D749 }, { 0x1D74A, false, de1D74A }, { 0x1D74B, false, de1D74B }, { 0x1D74C, false, de1D74C },
{ 0x1D74D, false, de1D74D }, { 0x1D74E, false, de1D74E }, { 0x1D74F, false, de1D74F }, { 0x1D750, false, de1D750 },
{ 0x1D751, false, de1D751 }, { 0x1D752, false, de1D752 }, { 0x1D753, false, de1D753 }, { 0x1D754, false, de1D754 },
{ 0x1D755, false, de1D755 }, { 0x1D756, false, de1D756 }, { 0x1D757, false, de1D757 }, { 0x1D758, false, de1D758 },
{ 0x1D759, false, de1D759 }, { 0x1D75A, false, de1D75A }, { 0x1D75B, false, de1D75B }, { 0x1D75C, false, de1D75C },
{ 0x1D75D, false, de1D75D }, { 0x1D75E, false, de1D75E }, { 0x1D75F, false, de1D75F }, { 0x1D760, false, de1D760 },
{ 0x1D761, false, de1D761 }, { 0x1D762, false, de1D762 }, { 0x1D763, false, de1D763 }, { 0x1D764, false, de1D764 },
{ 0x1D765, false, de1D765 }, { 0x1D766, false, de1D766 }, { 0x1D767, false, de1D767 }, { 0x1D768, false, de1D768 },
{ 0x1D769, false, de1D769 }, { 0x1D76A, false, de1D76A }, { 0x1D76B, false, de1D76B }, { 0x1D76C, false, de1D76C },
{ 0x1D76D, false, de1D76D }, { 0x1D76E, false, de1D76E }, { 0x1D76F, false, de1D76F }, { 0x1D770, false, de1D770 },
{ 0x1D771, false, de1D771 }, { 0x1D772, false, de1D772 }, { 0x1D773, false, de1D773 }, { 0x1D774, false, de1D774 },
{ 0x1D775, false, de1D775 }, { 0x1D776, false, de1D776 }, { 0x1D777, false, de1D777 }, { 0x1D778, false, de1D778 },
{ 0x1D779, false, de1D779 }, { 0x1D77A, false, de1D77A }, { 0x1D77B, false, de1D77B }, { 0x1D77C, false, de1D77C },
{ 0x1D77D, false, de1D77D }, { 0x1D77E, false, de1D77E }, { 0x1D77F, false, de1D77F }, { 0x1D780, false, de1D780 },
{ 0x1D781, false, de1D781 }, { 0x1D782, false, de1D782 }, { 0x1D783, false, de1D783 }, { 0x1D784, false, de1D784 },
{ 0x1D785, false, de1D785 }, { 0x1D786, false, de1D786 }, { 0x1D787, false, de1D787 }, { 0x1D788, false, de1D788 },
{ 0x1D789, false, de1D789 }, { 0x1D78A, false, de1D78A }, { 0x1D78B, false, de1D78B }, { 0x1D78C, false, de1D78C },
{ 0x1D78D, false, de1D78D }, { 0x1D78E, false, de1D78E }, { 0x1D78F, false, de1D78F }, { 0x1D790, false, de1D790 },
{ 0x1D791, false, de1D791 }, { 0x1D792, false, de1D792 }, { 0x1D793, false, de1D793 }, { 0x1D794, false, de1D794 },
{ 0x1D795, false, de1D795 }, { 0x1D796, false, de1D796 }, { 0x1D797, false, de1D797 }, { 0x1D798, false, de1D798 },
{ 0x1D799, false, de1D799 }, { 0x1D79A, false, de1D79A }, { 0x1D79B, false, de1D79B }, { 0x1D79C, false, de1D79C },
{ 0x1D79D, false, de1D79D }, { 0x1D79E, false, de1D79E }, { 0x1D79F, false, de1D79F }, { 0x1D7A0, false, de1D7A0 },
{ 0x1D7A1, false, de1D7A1 }, { 0x1D7A2, false, de1D7A2 }, { 0x1D7A3, false, de1D7A3 }, { 0x1D7A4, false, de1D7A4 },
{ 0x1D7A5, false, de1D7A5 }, { 0x1D7A6, false, de1D7A6 }, { 0x1D7A7, false, de1D7A7 }, { 0x1D7A8, false, de1D7A8 },
{ 0x1D7A9, false, de1D7A9 }, { 0x1D7AA, false, de1D7AA }, { 0x1D7AB, false, de1D7AB }, { 0x1D7AC, false, de1D7AC },
{ 0x1D7AD, false, de1D7AD }, { 0x1D7AE, false, de1D7AE }, { 0x1D7AF, false, de1D7AF }, { 0x1D7B0, false, de1D7B0 },
{ 0x1D7B1, false, de1D7B1 }, { 0x1D7B2, false, de1D7B2 }, { 0x1D7B3, false, de1D7B3 }, { 0x1D7B4, false, de1D7B4 },
{ 0x1D7B5, false, de1D7B5 }, { 0x1D7B6, false, de1D7B6 }, { 0x1D7B7, false, de1D7B7 }, { 0x1D7B8, false, de1D7B8 },
{ 0x1D7B9, false, de1D7B9 }, { 0x1D7BA, false, de1D7BA }, { 0x1D7BB, false, de1D7BB }, { 0x1D7BC, false, de1D7BC },
{ 0x1D7BD, false, de1D7BD }, { 0x1D7BE, false, de1D7BE }, { 0x1D7BF, false, de1D7BF }, { 0x1D7C0, false, de1D7C0 },
{ 0x1D7C1, false, de1D7C1 }, { 0x1D7C2, false, de1D7C2 }, { 0x1D7C3, false, de1D7C3 }, { 0x1D7C4, false, de1D7C4 },
{ 0x1D7C5, false, de1D7C5 }, { 0x1D7C6, false, de1D7C6 }, { 0x1D7C7, false, de1D7C7 }, { 0x1D7C8, false, de1D7C8 },
{ 0x1D7C9, false, de1D7C9 }, { 0x1D7CE, false, de1D7CE }, { 0x1D7CF, false, de1D7CF }, { 0x1D7D0, false, de1D7D0 },
{ 0x1D7D1, false, de1D7D1 }, { 0x1D7D2, false, de1D7D2 }, { 0x1D7D3, false, de1D7D3 }, { 0x1D7D4, false, de1D7D4 },
{ 0x1D7D5, false, de1D7D5 }, { 0x1D7D6, false, de1D7D6 }, { 0x1D7D7, false, de1D7D7 }, { 0x1D7D8, false, de1D7D8 },
{ 0x1D7D9, false, de1D7D9 }, { 0x1D7DA, false, de1D7DA }, { 0x1D7DB, false, de1D7DB }, { 0x1D7DC, false, de1D7DC },
{ 0x1D7DD, false, de1D7DD }, { 0x1D7DE, false, de1D7DE }, { 0x1D7DF, false, de1D7DF }, { 0x1D7E0, false, de1D7E0 },
{ 0x1D7E1, false, de1D7E1 }, { 0x1D7E2, false, de1D7E2 }, { 0x1D7E3, false, de1D7E3 }, { 0x1D7E4, false, de1D7E4 },
{ 0x1D7E5, false, de1D7E5 }, { 0x1D7E6, false, de1D7E6 }, { 0x1D7E7, false, de1D7E7 }, { 0x1D7E8, false, de1D7E8 },
{ 0x1D7E9, false, de1D7E9 }, { 0x1D7EA, false, de1D7EA }, { 0x1D7EB, false, de1D7EB }, { 0x1D7EC, false, de1D7EC },
{ 0x1D7ED, false, de1D7ED }, { 0x1D7EE, false, de1D7EE }, { 0x1D7EF, false, de1D7EF }, { 0x1D7F0, false, de1D7F0 },
{ 0x1D7F1, false, de1D7F1 }, { 0x1D7F2, false, de1D7F2 }, { 0x1D7F3, false, de1D7F3 }, { 0x1D7F4, false, de1D7F4 },
{ 0x1D7F5, false, de1D7F5 }, { 0x1D7F6, false, de1D7F6 }, { 0x1D7F7, false, de1D7F7 }, { 0x1D7F8, false, de1D7F8 },
{ 0x1D7F9, false, de1D7F9 }, { 0x1D7FA, false, de1D7FA }, { 0x1D7FB, false, de1D7FB }, { 0x1D7FC, false, de1D7FC },
{ 0x1D7FD, false, de1D7FD }, { 0x1D7FE, false, de1D7FE }, { 0x1D7FF, false, de1D7FF }, { 0x2F800, true, de2F800 },
{ 0x2F801, true, de2F801 }, { 0x2F802, true, de2F802 }, { 0x2F803, true, de2F803 }, { 0x2F804, true, de2F804 },
{ 0x2F805, true, de2F805 }, { 0x2F806, true, de2F806 }, { 0x2F807, true, de2F807 }, { 0x2F808, true, de2F808 },
{ 0x2F809, true, de2F809 }, { 0x2F80A, true, de2F80A }, { 0x2F80B, true, de2F80B }, { 0x2F80C, true, de2F80C },
{ 0x2F80D, true, de2F80D }, { 0x2F80E, true, de2F80E }, { 0x2F80F, true, de2F80F }, { 0x2F810, true, de2F810 },
{ 0x2F811, true, de2F811 }, { 0x2F812, true, de2F812 }, { 0x2F813, true, de2F813 }, { 0x2F814, true, de2F814 },
{ 0x2F815, true, de2F815 }, { 0x2F816, true, de2F816 }, { 0x2F817, true, de2F817 }, { 0x2F818, true, de2F818 },
{ 0x2F819, true, de2F819 }, { 0x2F81A, true, de2F81A }, { 0x2F81B, true, de2F81B }, { 0x2F81C, true, de2F81C },
{ 0x2F81D, true, de2F81D }, { 0x2F81E, true, de2F81E }, { 0x2F81F, true, de2F81F }, { 0x2F820, true, de2F820 },
{ 0x2F821, true, de2F821 }, { 0x2F822, true, de2F822 }, { 0x2F823, true, de2F823 }, { 0x2F824, true, de2F824 },
{ 0x2F825, true, de2F825 }, { 0x2F826, true, de2F826 }, { 0x2F827, true, de2F827 }, { 0x2F828, true, de2F828 },
{ 0x2F829, true, de2F829 }, { 0x2F82A, true, de2F82A }, { 0x2F82B, true, de2F82B }, { 0x2F82C, true, de2F82C },
{ 0x2F82D, true, de2F82D }, { 0x2F82E, true, de2F82E }, { 0x2F82F, true, de2F82F }, { 0x2F830, true, de2F830 },
{ 0x2F831, true, de2F831 }, { 0x2F832, true, de2F832 }, { 0x2F833, true, de2F833 }, { 0x2F834, true, de2F834 },
{ 0x2F835, true, de2F835 }, { 0x2F836, true, de2F836 }, { 0x2F837, true, de2F837 }, { 0x2F838, true, de2F838 },
{ 0x2F839, true, de2F839 }, { 0x2F83A, true, de2F83A }, { 0x2F83B, true, de2F83B }, { 0x2F83C, true, de2F83C },
{ 0x2F83D, true, de2F83D }, { 0x2F83E, true, de2F83E }, { 0x2F83F, true, de2F83F }, { 0x2F840, true, de2F840 },
{ 0x2F841, true, de2F841 }, { 0x2F842, true, de2F842 }, { 0x2F843, true, de2F843 }, { 0x2F844, true, de2F844 },
{ 0x2F845, true, de2F845 }, { 0x2F846, true, de2F846 }, { 0x2F847, true, de2F847 }, { 0x2F848, true, de2F848 },
{ 0x2F849, true, de2F849 }, { 0x2F84A, true, de2F84A }, { 0x2F84B, true, de2F84B }, { 0x2F84C, true, de2F84C },
{ 0x2F84D, true, de2F84D }, { 0x2F84E, true, de2F84E }, { 0x2F84F, true, de2F84F }, { 0x2F850, true, de2F850 },
{ 0x2F851, true, de2F851 }, { 0x2F852, true, de2F852 }, { 0x2F853, true, de2F853 }, { 0x2F854, true, de2F854 },
{ 0x2F855, true, de2F855 }, { 0x2F856, true, de2F856 }, { 0x2F857, true, de2F857 }, { 0x2F858, true, de2F858 },
{ 0x2F859, true, de2F859 }, { 0x2F85A, true, de2F85A }, { 0x2F85B, true, de2F85B }, { 0x2F85C, true, de2F85C },
{ 0x2F85D, true, de2F85D }, { 0x2F85E, true, de2F85E }, { 0x2F85F, true, de2F85F }, { 0x2F860, true, de2F860 },
{ 0x2F861, true, de2F861 }, { 0x2F862, true, de2F862 }, { 0x2F863, true, de2F863 }, { 0x2F864, true, de2F864 },
{ 0x2F865, true, de2F865 }, { 0x2F866, true, de2F866 }, { 0x2F867, true, de2F867 }, { 0x2F868, true, de2F868 },
{ 0x2F869, true, de2F869 }, { 0x2F86A, true, de2F86A }, { 0x2F86B, true, de2F86B }, { 0x2F86C, true, de2F86C },
{ 0x2F86D, true, de2F86D }, { 0x2F86E, true, de2F86E }, { 0x2F86F, true, de2F86F }, { 0x2F870, true, de2F870 },
{ 0x2F871, true, de2F871 }, { 0x2F872, true, de2F872 }, { 0x2F873, true, de2F873 }, { 0x2F874, true, de2F874 },
{ 0x2F875, true, de2F875 }, { 0x2F876, true, de2F876 }, { 0x2F877, true, de2F877 }, { 0x2F878, true, de2F878 },
{ 0x2F879, true, de2F879 }, { 0x2F87A, true, de2F87A }, { 0x2F87B, true, de2F87B }, { 0x2F87C, true, de2F87C },
{ 0x2F87D, true, de2F87D }, { 0x2F87E, true, de2F87E }, { 0x2F87F, true, de2F87F }, { 0x2F880, true, de2F880 },
{ 0x2F881, true, de2F881 }, { 0x2F882, true, de2F882 }, { 0x2F883, true, de2F883 }, { 0x2F884, true, de2F884 },
{ 0x2F885, true, de2F885 }, { 0x2F886, true, de2F886 }, { 0x2F887, true, de2F887 }, { 0x2F888, true, de2F888 },
{ 0x2F889, true, de2F889 }, { 0x2F88A, true, de2F88A }, { 0x2F88B, true, de2F88B }, { 0x2F88C, true, de2F88C },
{ 0x2F88D, true, de2F88D }, { 0x2F88E, true, de2F88E }, { 0x2F88F, true, de2F88F }, { 0x2F890, true, de2F890 },
{ 0x2F891, true, de2F891 }, { 0x2F892, true, de2F892 }, { 0x2F893, true, de2F893 }, { 0x2F894, true, de2F894 },
{ 0x2F895, true, de2F895 }, { 0x2F896, true, de2F896 }, { 0x2F897, true, de2F897 }, { 0x2F898, true, de2F898 },
{ 0x2F899, true, de2F899 }, { 0x2F89A, true, de2F89A }, { 0x2F89B, true, de2F89B }, { 0x2F89C, true, de2F89C },
{ 0x2F89D, true, de2F89D }, { 0x2F89E, true, de2F89E }, { 0x2F89F, true, de2F89F }, { 0x2F8A0, true, de2F8A0 },
{ 0x2F8A1, true, de2F8A1 }, { 0x2F8A2, true, de2F8A2 }, { 0x2F8A3, true, de2F8A3 }, { 0x2F8A4, true, de2F8A4 },
{ 0x2F8A5, true, de2F8A5 }, { 0x2F8A6, true, de2F8A6 }, { 0x2F8A7, true, de2F8A7 }, { 0x2F8A8, true, de2F8A8 },
{ 0x2F8A9, true, de2F8A9 }, { 0x2F8AA, true, de2F8AA }, { 0x2F8AB, true, de2F8AB }, { 0x2F8AC, true, de2F8AC },
{ 0x2F8AD, true, de2F8AD }, { 0x2F8AE, true, de2F8AE }, { 0x2F8AF, true, de2F8AF }, { 0x2F8B0, true, de2F8B0 },
{ 0x2F8B1, true, de2F8B1 }, { 0x2F8B2, true, de2F8B2 }, { 0x2F8B3, true, de2F8B3 }, { 0x2F8B4, true, de2F8B4 },
{ 0x2F8B5, true, de2F8B5 }, { 0x2F8B6, true, de2F8B6 }, { 0x2F8B7, true, de2F8B7 }, { 0x2F8B8, true, de2F8B8 },
{ 0x2F8B9, true, de2F8B9 }, { 0x2F8BA, true, de2F8BA }, { 0x2F8BB, true, de2F8BB }, { 0x2F8BC, true, de2F8BC },
{ 0x2F8BD, true, de2F8BD }, { 0x2F8BE, true, de2F8BE }, { 0x2F8BF, true, de2F8BF }, { 0x2F8C0, true, de2F8C0 },
{ 0x2F8C1, true, de2F8C1 }, { 0x2F8C2, true, de2F8C2 }, { 0x2F8C3, true, de2F8C3 }, { 0x2F8C4, true, de2F8C4 },
{ 0x2F8C5, true, de2F8C5 }, { 0x2F8C6, true, de2F8C6 }, { 0x2F8C7, true, de2F8C7 }, { 0x2F8C8, true, de2F8C8 },
{ 0x2F8C9, true, de2F8C9 }, { 0x2F8CA, true, de2F8CA }, { 0x2F8CB, true, de2F8CB }, { 0x2F8CC, true, de2F8CC },
{ 0x2F8CD, true, de2F8CD }, { 0x2F8CE, true, de2F8CE }, { 0x2F8CF, true, de2F8CF }, { 0x2F8D0, true, de2F8D0 },
{ 0x2F8D1, true, de2F8D1 }, { 0x2F8D2, true, de2F8D2 }, { 0x2F8D3, true, de2F8D3 }, { 0x2F8D4, true, de2F8D4 },
{ 0x2F8D5, true, de2F8D5 }, { 0x2F8D6, true, de2F8D6 }, { 0x2F8D7, true, de2F8D7 }, { 0x2F8D8, true, de2F8D8 },
{ 0x2F8D9, true, de2F8D9 }, { 0x2F8DA, true, de2F8DA }, { 0x2F8DB, true, de2F8DB }, { 0x2F8DC, true, de2F8DC },
{ 0x2F8DD, true, de2F8DD }, { 0x2F8DE, true, de2F8DE }, { 0x2F8DF, true, de2F8DF }, { 0x2F8E0, true, de2F8E0 },
{ 0x2F8E1, true, de2F8E1 }, { 0x2F8E2, true, de2F8E2 }, { 0x2F8E3, true, de2F8E3 }, { 0x2F8E4, true, de2F8E4 },
{ 0x2F8E5, true, de2F8E5 }, { 0x2F8E6, true, de2F8E6 }, { 0x2F8E7, true, de2F8E7 }, { 0x2F8E8, true, de2F8E8 },
{ 0x2F8E9, true, de2F8E9 }, { 0x2F8EA, true, de2F8EA }, { 0x2F8EB, true, de2F8EB }, { 0x2F8EC, true, de2F8EC },
{ 0x2F8ED, true, de2F8ED }, { 0x2F8EE, true, de2F8EE }, { 0x2F8EF, true, de2F8EF }, { 0x2F8F0, true, de2F8F0 },
{ 0x2F8F1, true, de2F8F1 }, { 0x2F8F2, true, de2F8F2 }, { 0x2F8F3, true, de2F8F3 }, { 0x2F8F4, true, de2F8F4 },
{ 0x2F8F5, true, de2F8F5 }, { 0x2F8F6, true, de2F8F6 }, { 0x2F8F7, true, de2F8F7 }, { 0x2F8F8, true, de2F8F8 },
{ 0x2F8F9, true, de2F8F9 }, { 0x2F8FA, true, de2F8FA }, { 0x2F8FB, true, de2F8FB }, { 0x2F8FC, true, de2F8FC },
{ 0x2F8FD, true, de2F8FD }, { 0x2F8FE, true, de2F8FE }, { 0x2F8FF, true, de2F8FF }, { 0x2F900, true, de2F900 },
{ 0x2F901, true, de2F901 }, { 0x2F902, true, de2F902 }, { 0x2F903, true, de2F903 }, { 0x2F904, true, de2F904 },
{ 0x2F905, true, de2F905 }, { 0x2F906, true, de2F906 }, { 0x2F907, true, de2F907 }, { 0x2F908, true, de2F908 },
{ 0x2F909, true, de2F909 }, { 0x2F90A, true, de2F90A }, { 0x2F90B, true, de2F90B }, { 0x2F90C, true, de2F90C },
{ 0x2F90D, true, de2F90D }, { 0x2F90E, true, de2F90E }, { 0x2F90F, true, de2F90F }, { 0x2F910, true, de2F910 },
{ 0x2F911, true, de2F911 }, { 0x2F912, true, de2F912 }, { 0x2F913, true, de2F913 }, { 0x2F914, true, de2F914 },
{ 0x2F915, true, de2F915 }, { 0x2F916, true, de2F916 }, { 0x2F917, true, de2F917 }, { 0x2F918, true, de2F918 },
{ 0x2F919, true, de2F919 }, { 0x2F91A, true, de2F91A }, { 0x2F91B, true, de2F91B }, { 0x2F91C, true, de2F91C },
{ 0x2F91D, true, de2F91D }, { 0x2F91E, true, de2F91E }, { 0x2F91F, true, de2F91F }, { 0x2F920, true, de2F920 },
{ 0x2F921, true, de2F921 }, { 0x2F922, true, de2F922 }, { 0x2F923, true, de2F923 }, { 0x2F924, true, de2F924 },
{ 0x2F925, true, de2F925 }, { 0x2F926, true, de2F926 }, { 0x2F927, true, de2F927 }, { 0x2F928, true, de2F928 },
{ 0x2F929, true, de2F929 }, { 0x2F92A, true, de2F92A }, { 0x2F92B, true, de2F92B }, { 0x2F92C, true, de2F92C },
{ 0x2F92D, true, de2F92D }, { 0x2F92E, true, de2F92E }, { 0x2F92F, true, de2F92F }, { 0x2F930, true, de2F930 },
{ 0x2F931, true, de2F931 }, { 0x2F932, true, de2F932 }, { 0x2F933, true, de2F933 }, { 0x2F934, true, de2F934 },
{ 0x2F935, true, de2F935 }, { 0x2F936, true, de2F936 }, { 0x2F937, true, de2F937 }, { 0x2F938, true, de2F938 },
{ 0x2F939, true, de2F939 }, { 0x2F93A, true, de2F93A }, { 0x2F93B, true, de2F93B }, { 0x2F93C, true, de2F93C },
{ 0x2F93D, true, de2F93D }, { 0x2F93E, true, de2F93E }, { 0x2F93F, true, de2F93F }, { 0x2F940, true, de2F940 },
{ 0x2F941, true, de2F941 }, { 0x2F942, true, de2F942 }, { 0x2F943, true, de2F943 }, { 0x2F944, true, de2F944 },
{ 0x2F945, true, de2F945 }, { 0x2F946, true, de2F946 }, { 0x2F947, true, de2F947 }, { 0x2F948, true, de2F948 },
{ 0x2F949, true, de2F949 }, { 0x2F94A, true, de2F94A }, { 0x2F94B, true, de2F94B }, { 0x2F94C, true, de2F94C },
{ 0x2F94D, true, de2F94D }, { 0x2F94E, true, de2F94E }, { 0x2F94F, true, de2F94F }, { 0x2F950, true, de2F950 },
{ 0x2F951, true, de2F951 }, { 0x2F952, true, de2F952 }, { 0x2F953, true, de2F953 }, { 0x2F954, true, de2F954 },
{ 0x2F955, true, de2F955 }, { 0x2F956, true, de2F956 }, { 0x2F957, true, de2F957 }, { 0x2F958, true, de2F958 },
{ 0x2F959, true, de2F959 }, { 0x2F95A, true, de2F95A }, { 0x2F95B, true, de2F95B }, { 0x2F95C, true, de2F95C },
{ 0x2F95D, true, de2F95D }, { 0x2F95E, true, de2F95E }, { 0x2F95F, true, de2F95F }, { 0x2F960, true, de2F960 },
{ 0x2F961, true, de2F961 }, { 0x2F962, true, de2F962 }, { 0x2F963, true, de2F963 }, { 0x2F964, true, de2F964 },
{ 0x2F965, true, de2F965 }, { 0x2F966, true, de2F966 }, { 0x2F967, true, de2F967 }, { 0x2F968, true, de2F968 },
{ 0x2F969, true, de2F969 }, { 0x2F96A, true, de2F96A }, { 0x2F96B, true, de2F96B }, { 0x2F96C, true, de2F96C },
{ 0x2F96D, true, de2F96D }, { 0x2F96E, true, de2F96E }, { 0x2F96F, true, de2F96F }, { 0x2F970, true, de2F970 },
{ 0x2F971, true, de2F971 }, { 0x2F972, true, de2F972 }, { 0x2F973, true, de2F973 }, { 0x2F974, true, de2F974 },
{ 0x2F975, true, de2F975 }, { 0x2F976, true, de2F976 }, { 0x2F977, true, de2F977 }, { 0x2F978, true, de2F978 },
{ 0x2F979, true, de2F979 }, { 0x2F97A, true, de2F97A }, { 0x2F97B, true, de2F97B }, { 0x2F97C, true, de2F97C },
{ 0x2F97D, true, de2F97D }, { 0x2F97E, true, de2F97E }, { 0x2F97F, true, de2F97F }, { 0x2F980, true, de2F980 },
{ 0x2F981, true, de2F981 }, { 0x2F982, true, de2F982 }, { 0x2F983, true, de2F983 }, { 0x2F984, true, de2F984 },
{ 0x2F985, true, de2F985 }, { 0x2F986, true, de2F986 }, { 0x2F987, true, de2F987 }, { 0x2F988, true, de2F988 },
{ 0x2F989, true, de2F989 }, { 0x2F98A, true, de2F98A }, { 0x2F98B, true, de2F98B }, { 0x2F98C, true, de2F98C },
{ 0x2F98D, true, de2F98D }, { 0x2F98E, true, de2F98E }, { 0x2F98F, true, de2F98F }, { 0x2F990, true, de2F990 },
{ 0x2F991, true, de2F991 }, { 0x2F992, true, de2F992 }, { 0x2F993, true, de2F993 }, { 0x2F994, true, de2F994 },
{ 0x2F995, true, de2F995 }, { 0x2F996, true, de2F996 }, { 0x2F997, true, de2F997 }, { 0x2F998, true, de2F998 },
{ 0x2F999, true, de2F999 }, { 0x2F99A, true, de2F99A }, { 0x2F99B, true, de2F99B }, { 0x2F99C, true, de2F99C },
{ 0x2F99D, true, de2F99D }, { 0x2F99E, true, de2F99E }, { 0x2F99F, true, de2F99F }, { 0x2F9A0, true, de2F9A0 },
{ 0x2F9A1, true, de2F9A1 }, { 0x2F9A2, true, de2F9A2 }, { 0x2F9A3, true, de2F9A3 }, { 0x2F9A4, true, de2F9A4 },
{ 0x2F9A5, true, de2F9A5 }, { 0x2F9A6, true, de2F9A6 }, { 0x2F9A7, true, de2F9A7 }, { 0x2F9A8, true, de2F9A8 },
{ 0x2F9A9, true, de2F9A9 }, { 0x2F9AA, true, de2F9AA }, { 0x2F9AB, true, de2F9AB }, { 0x2F9AC, true, de2F9AC },
{ 0x2F9AD, true, de2F9AD }, { 0x2F9AE, true, de2F9AE }, { 0x2F9AF, true, de2F9AF }, { 0x2F9B0, true, de2F9B0 },
{ 0x2F9B1, true, de2F9B1 }, { 0x2F9B2, true, de2F9B2 }, { 0x2F9B3, true, de2F9B3 }, { 0x2F9B4, true, de2F9B4 },
{ 0x2F9B5, true, de2F9B5 }, { 0x2F9B6, true, de2F9B6 }, { 0x2F9B7, true, de2F9B7 }, { 0x2F9B8, true, de2F9B8 },
{ 0x2F9B9, true, de2F9B9 }, { 0x2F9BA, true, de2F9BA }, { 0x2F9BB, true, de2F9BB }, { 0x2F9BC, true, de2F9BC },
{ 0x2F9BD, true, de2F9BD }, { 0x2F9BE, true, de2F9BE }, { 0x2F9BF, true, de2F9BF }, { 0x2F9C0, true, de2F9C0 },
{ 0x2F9C1, true, de2F9C1 }, { 0x2F9C2, true, de2F9C2 }, { 0x2F9C3, true, de2F9C3 }, { 0x2F9C4, true, de2F9C4 },
{ 0x2F9C5, true, de2F9C5 }, { 0x2F9C6, true, de2F9C6 }, { 0x2F9C7, true, de2F9C7 }, { 0x2F9C8, true, de2F9C8 },
{ 0x2F9C9, true, de2F9C9 }, { 0x2F9CA, true, de2F9CA }, { 0x2F9CB, true, de2F9CB }, { 0x2F9CC, true, de2F9CC },
{ 0x2F9CD, true, de2F9CD }, { 0x2F9CE, true, de2F9CE }, { 0x2F9CF, true, de2F9CF }, { 0x2F9D0, true, de2F9D0 },
{ 0x2F9D1, true, de2F9D1 }, { 0x2F9D2, true, de2F9D2 }, { 0x2F9D3, true, de2F9D3 }, { 0x2F9D4, true, de2F9D4 },
{ 0x2F9D5, true, de2F9D5 }, { 0x2F9D6, true, de2F9D6 }, { 0x2F9D7, true, de2F9D7 }, { 0x2F9D8, true, de2F9D8 },
{ 0x2F9D9, true, de2F9D9 }, { 0x2F9DA, true, de2F9DA }, { 0x2F9DB, true, de2F9DB }, { 0x2F9DC, true, de2F9DC },
{ 0x2F9DD, true, de2F9DD }, { 0x2F9DE, true, de2F9DE }, { 0x2F9DF, true, de2F9DF }, { 0x2F9E0, true, de2F9E0 },
{ 0x2F9E1, true, de2F9E1 }, { 0x2F9E2, true, de2F9E2 }, { 0x2F9E3, true, de2F9E3 }, { 0x2F9E4, true, de2F9E4 },
{ 0x2F9E5, true, de2F9E5 }, { 0x2F9E6, true, de2F9E6 }, { 0x2F9E7, true, de2F9E7 }, { 0x2F9E8, true, de2F9E8 },
{ 0x2F9E9, true, de2F9E9 }, { 0x2F9EA, true, de2F9EA }, { 0x2F9EB, true, de2F9EB }, { 0x2F9EC, true, de2F9EC },
{ 0x2F9ED, true, de2F9ED }, { 0x2F9EE, true, de2F9EE }, { 0x2F9EF, true, de2F9EF }, { 0x2F9F0, true, de2F9F0 },
{ 0x2F9F1, true, de2F9F1 }, { 0x2F9F2, true, de2F9F2 }, { 0x2F9F3, true, de2F9F3 }, { 0x2F9F4, true, de2F9F4 },
{ 0x2F9F5, true, de2F9F5 }, { 0x2F9F6, true, de2F9F6 }, { 0x2F9F7, true, de2F9F7 }, { 0x2F9F8, true, de2F9F8 },
{ 0x2F9F9, true, de2F9F9 }, { 0x2F9FA, true, de2F9FA }, { 0x2F9FB, true, de2F9FB }, { 0x2F9FC, true, de2F9FC },
{ 0x2F9FD, true, de2F9FD }, { 0x2F9FE, true, de2F9FE }, { 0x2F9FF, true, de2F9FF }, { 0x2FA00, true, de2FA00 },
{ 0x2FA01, true, de2FA01 }, { 0x2FA02, true, de2FA02 }, { 0x2FA03, true, de2FA03 }, { 0x2FA04, true, de2FA04 },
{ 0x2FA05, true, de2FA05 }, { 0x2FA06, true, de2FA06 }, { 0x2FA07, true, de2FA07 }, { 0x2FA08, true, de2FA08 },
{ 0x2FA09, true, de2FA09 }, { 0x2FA0A, true, de2FA0A }, { 0x2FA0B, true, de2FA0B }, { 0x2FA0C, true, de2FA0C },
{ 0x2FA0D, true, de2FA0D }, { 0x2FA0E, true, de2FA0E }, { 0x2FA0F, true, de2FA0F }, { 0x2FA10, true, de2FA10 },
{ 0x2FA11, true, de2FA11 }, { 0x2FA12, true, de2FA12 }, { 0x2FA13, true, de2FA13 }, { 0x2FA14, true, de2FA14 },
{ 0x2FA15, true, de2FA15 }, { 0x2FA16, true, de2FA16 }, { 0x2FA17, true, de2FA17 }, { 0x2FA18, true, de2FA18 },
{ 0x2FA19, true, de2FA19 }, { 0x2FA1A, true, de2FA1A }, { 0x2FA1B, true, de2FA1B }, { 0x2FA1C, true, de2FA1C },
{ 0x2FA1D, true, de2FA1D }
};

unsigned int *NormalizeTransform::getDecomposition(unsigned int ch)
{
  if(ch < decomposeDirectTableSize) {
    // Use the direct lookup table
    if(!canonical_ || decomposeDirectTable[ch].canonical) {
      return decomposeDirectTable[ch].dest;
    }
    return 0;
  }

  // Binary search in the decomposeTable table
  unsigned int min = 0;
  unsigned int max = decomposeTableSize;
  unsigned int middle;
  int cmp;

  while(min < max) {
    middle = (max + min) >> 1;

    cmp = decomposeTable[middle].ch - ch;
    if(cmp > 0) max = middle;
    else if(cmp < 0) {
      min = middle + 1;
    }
    else {
      if(!canonical_ || decomposeTable[middle].canonical) {
        return decomposeTable[middle].dest;
      }
      return 0;
    }
  }

  return 0;
}
