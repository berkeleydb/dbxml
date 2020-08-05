//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLTUPLEDEBUGHOOK_HPP
#define	__DBXMLTUPLEDEBUGHOOK_HPP

#include <xqilla/debug/TupleDebugHook.hpp>

namespace DbXml
{

class DbXmlTupleDebugHook : public TupleDebugHook
{
public:
  DbXmlTupleDebugHook(TupleNode *parent, XPath2MemoryManager *mm)
	  : TupleDebugHook(parent, mm) {}

  virtual TupleResult::Ptr createResult(DynamicContext* context) const;
};


}

#endif
