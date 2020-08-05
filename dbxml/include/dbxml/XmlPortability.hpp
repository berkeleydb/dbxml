//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLPORTABILITY_HPP
#define	__XMLPORTABILITY_HPP

#if defined(_DBXML_COMPRESSION)
#define DBXML_COMPRESSION 1
#else
#undef DBXML_COMPRESSION
#endif

// Keep compilers quiet about unused parameters
#define	UNUSED(x) static_cast<void>(&(x))

#if defined(_MSC_VER)
// versions check:
// we don't support Visual C++ prior to version 6:
#if (_MSC_VER<1200)
#error "Compiler not supported"
#endif // _MSC_VER<1200

// For Visual Studio 8 and above...
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

// MSVC requires explicit exportation of symbols accessed in a DLL
// If we are building the library, export symbols
// If we are linking against the library, import symbols
// Static library users should define DBXML_EXPORT to nothing

#if !defined(DBXML_EXPORT)
  #if defined(DBXML_CREATE_DLL)
    #define DBXML_EXPORT __declspec(dllexport)
    #define DBXML_EXPORT_TEMPLATE template class DBXML_EXPORT
  #else
    #define DBXML_EXPORT __declspec(dllimport)
    #define DBXML_EXPORT_TEMPLATE extern template class DBXML_EXPORT
  #endif
#endif

#else // !_MSC_VER

// Building on a non-Windows platform.
#define	DBXML_EXPORT
#define	DBXML_EXPORT_TEMPLATE template class

#endif // !_MSC_VER

#endif // __XMLPORTABILITY_HPP
