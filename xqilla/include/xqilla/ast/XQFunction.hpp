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
 * $Id: XQFunction.hpp 639 2008-09-01 13:22:54Z jpcs $
 */

#ifndef _XQFUNCTION_HPP
#define _XQFUNCTION_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/util/XMLUniDefs.hpp>      // because every implementation will use these to define the function name
#include <vector>

class SequenceType;

class XQILLA_API XQFunction : public ASTNodeImpl
{

public:
  XQFunction(const XMLCh* name, size_t argsFrom, size_t argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  const XMLCh* getFunctionURI()const;
  const XMLCh* getFunctionName()const;
  const XMLCh* getFunctionSignature() const;
  const VectorOfASTNodes &getArguments() const;
  
  static const XMLCh XMLChFunctionURI[];

  /** Wraps the arguments in the appropriate conversion functions,
      calls static resolution on them, and constant folds if
      this function's StaticAnalysis is not used */
  ASTNode *resolveArguments(StaticContext *context, bool checkTimezone = false, bool numericFunction = false);
  ASTNode *calculateSRCForArguments(StaticContext *context, bool checkTimezone = false, bool numericFunction = false);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual Sequence createSequence(DynamicContext* context, int flags=0) const;

  /** casts the expression given as a parameter into the appropriate type and returns the guaranteed correctly typed objects in a sequence  */
  virtual Result getParamNumber(size_t number, DynamicContext* context, int flags=0) const;

  /** returns the number of parameters passed into the function */
  size_t getNumArgs() const;

protected:

  /** Helper method, produces a single ATStringOrDerived from the output of getParamNumber(). If getParamNumber() returns anything other than a single string, an exception is thrown. */
  static const size_t UNLIMITED;

  //parse the supplied string of comma separated arguments into vector of SequenceTypes
  void parseParamDecl(const char* paramString, XPath2MemoryManager *mm);

  const XMLCh *_fName, *_fURI, *_signature;
  const size_t _nArgsFrom, _nArgsTo;

  std::vector<SequenceType*, XQillaAllocator<SequenceType*> > _paramDecl;

  VectorOfASTNodes _args; // The real store for arguments 

};

#endif
