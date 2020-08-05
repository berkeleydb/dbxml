//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLPREDICATE_HPP
#define	__DBXMLPREDICATE_HPP

#include "DbXmlNodeImpl.hpp"
#include "DbXmlASTNode.hpp"
#include "../IndexEntry.hpp"

#include <xqilla/context/VariableStore.hpp>

namespace DbXml
{

class DbXmlPredicate : public DbXmlASTNode
{
public:
	DbXmlPredicate(ASTNode *expr, ASTNode *predicate, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager *mm);

	ASTNode *getExpression() const { return expr_; }
	void setExpression(ASTNode *expr) { expr_ = expr; }

	ASTNode *getPredicate() const { return pred_; }
	void setPredicate(ASTNode *pred) { pred_ = pred; }

	const XMLCh *getURI() const { return uri_; }
	const XMLCh *getName() const { return name_; }

	StaticAnalysis &getVarSRC() { return varSrc_; }

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

protected:
	ASTNode *expr_;
	ASTNode *pred_;

	const XMLCh *uri_;
	const XMLCh *name_;
	StaticAnalysis varSrc_;
};

class VarPredicateResult : public ResultImpl, public VariableStore
{
public:
	VarPredicateResult(const Result &parent, const ASTNode *pred, const XMLCh *uri, const XMLCh *name);

	virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const;
	virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const;

	Item::Ptr next(DynamicContext *context);
	std::string asString(DynamicContext *context, int indent) const { return ""; }

private:
	Result parent_;
	const ASTNode *pred_;
	const XMLCh *uri_;
	const XMLCh *name_;

	Item::Ptr item_;
	const VariableStore *scope_;
};

class DbXmlNodeCheck : public DbXmlASTNode
{
public:
	DbXmlNodeCheck(ASTNode *arg, XPath2MemoryManager* memMgr);

	ASTNode *getArg() const { return arg_; }
	void setArg(ASTNode *a) { arg_ = a; }

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	ASTNode *arg_;
};

class DbXmlLastStepCheck : public DbXmlASTNode
{
public:
	DbXmlLastStepCheck(ASTNode *arg, XPath2MemoryManager* memMgr);

	ASTNode *getArg() const { return arg_; }
	void setArg(ASTNode *a) { arg_ = a; }

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	ASTNode *arg_;
};


}

#endif
