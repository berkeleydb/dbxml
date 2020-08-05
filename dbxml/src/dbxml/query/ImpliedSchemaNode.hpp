//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __IMPLIEDSCHEMANODE_HPP
#define	__IMPLIEDSCHEMANODE_HPP

#include <vector>
#include <string>

#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/optimizer/QueryPathNode.hpp>

#include "Syntax.hpp"

class ASTNode;
class XQCastAs;

namespace DbXml
{

class QueryPlanRoot;
class DbXmlNodeTest;

class ImpliedSchemaNode : public QueryPathNode
{
public:
	typedef enum {
		ATTRIBUTE = QueryPathNode::ATTRIBUTE,
		CHILD = QueryPathNode::CHILD,
		DESCENDANT = QueryPathNode::DESCENDANT,
		DESCENDANT_ATTR = QueryPathNode::DESCENDANT_ATTR,
		ROOT = QueryPathNode::ROOT,
		METADATA,
		CAST,
		EQUALS,
		NOT_EQUALS,
		LTX,
		LTE,
		GTX,
		GTE,
		PREFIX,
		SUBSTRING,
		SUBSTRING_CD, ///< Case/diacritic insensitive
		SUFFIX
	} Type;

	typedef std::vector<ImpliedSchemaNode*> Vector;
	typedef std::vector<ImpliedSchemaNode*, XQillaAllocator<ImpliedSchemaNode*> > MVector;


	ImpliedSchemaNode(Type type, bool generalComp, Syntax::Type syntaxType, const ASTNode *value,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ImpliedSchemaNode(const DbXmlNodeTest *nodeTest, Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ImpliedSchemaNode(const XQCastAs *cast, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	virtual ~ImpliedSchemaNode();

	const QueryPlanRoot *getQueryPlanRoot() const { return qpr_; }
	void setQueryPlanRoot(QueryPlanRoot *qpr) { qpr_ = qpr; }

	virtual bool isWildcard() const;
	bool isSuitableForIndex() const;

	const XQCastAs *getCast() const;

	bool getGeneralComp() const { return generalComp_; }
	void setGeneralComp(bool v) { generalComp_ = v; }
	Syntax::Type getSyntaxType() const { return syntaxType_; }
	void setSyntaxType(Syntax::Type st) { syntaxType_ = st; }
	const ASTNode *getASTNode() const { return astnode_; }
	void setASTNode(const ASTNode *value) { astnode_ = value; }

	ImpliedSchemaNode *getBaseNode();

	virtual void markSubtreeValue();
	virtual void markSubtreeResult();

	std::string toString(int level = 0) const;
	std::string getPath() const;
	std::string getStepName() const;

	const char *getURI8() const;
	const char *getName8() const;
	/// Lazily calculated
	const char *getUriName() const;

	virtual bool equals(const QueryPathNode *o) const;
	virtual bool isSubsetOf(const QueryPathNode *node) const;

	/// Performs a deep copy of the node and it's children
	ImpliedSchemaNode *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;

private:
	bool generalComp_;
	Syntax::Type syntaxType_;
	const ASTNode *astnode_;

	char *utf8URIName_;

	QueryPlanRoot *qpr_;
};

}

#endif
