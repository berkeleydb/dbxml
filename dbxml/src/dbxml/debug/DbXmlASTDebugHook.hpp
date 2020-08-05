//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLASTDEBUGHOOK_HPP
#define	__DBXMLASTDEBUGHOOK_HPP

#include <xqilla/debug/ASTDebugHook.hpp>

namespace DbXml
{

class DbXmlASTDebugHook : public ASTDebugHook
{
public:
  DbXmlASTDebugHook(ASTNode *astNode, XPath2MemoryManager *mm)
	  : ASTDebugHook(astNode, mm) {}

  virtual Result createResult(DynamicContext *context, int flags) const;
  virtual Result iterateResult(const Result &contextItems, DynamicContext *context) const;
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                                             bool preserveNS, bool preserveType) const;
  virtual PendingUpdateList createUpdateList(DynamicContext *context) const;
};


}

#endif
