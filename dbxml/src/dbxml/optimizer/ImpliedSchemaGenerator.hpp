//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __IMPLIEDSCHEMAGENERATOR_HPP
#define	__IMPLIEDSCHEMAGENERATOR_HPP

#include <dbxml/XmlQueryContext.hpp>
#include "NodeVisitingOptimizer.hpp"
#include "../query/ImpliedSchemaNode.hpp"
#include "../dataItem/Join.hpp"

#include <xqilla/context/impl/VariableStoreTemplate.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>
#include <xqilla/optimizer/QueryPathTreeGenerator.hpp>

namespace DbXml
{

class DbXmlDocAvailable;
class DbXmlNodeTest;

/**
 * Generates ImpliedSchemaNode trees of the paths
 * in the documents that will be navigated. Decorates
 * the AST with the ImpliedSchemaNode objects that apply
 * at each stage.
 */
class ImpliedSchemaGenerator : public ASTVisitorExtender<QueryPathTreeGenerator>
{
public:
	ImpliedSchemaGenerator(DynamicContext *xpc, Optimizer *parent = 0);

	const ImpliedSchemaNode::Vector & getRoots() const { return roots_; }

private:
	virtual void resetInternal();

	virtual QueryPathNode *createQueryPathNode(const NodeTest *nodeTest, QueryPathNode::Type type);
	virtual NodeTest *createNodeTest(const XMLCh *nodeType, const XMLCh *uri, const XMLCh *name);

	virtual ASTNode *optimizeStep(XQStep *item);
	virtual ASTNode *optimizeFunction(XQFunction *item);
	virtual ASTNode *optimizeOperator(XQOperator *item);
	virtual ASTNode *optimizeDOMConstructor(XQDOMConstructor *item);
	virtual ASTNode *optimizeCastAs(XQCastAs *item);
	virtual ASTNode *optimizeAtomize(XQAtomize *item);

	ASTVISITOREXTENDER_AST_METHODS()

	void generateComparison(ImpliedSchemaNode::Type type, bool generalComp,
		VectorOfASTNodes &args, PathResult &result, ASTNode *item);
	void generateSubstring(ImpliedSchemaNode::Type type, VectorOfASTNodes &args,
		PathResult &result, ASTNode *item);
	void generateLookup(ASTNode *item, QueryPlanRoot *qpr, ImpliedSchemaNode::Type type,
		const char *child, const char *parent, PathResult &result);

	ImpliedSchemaNode::Vector roots_;
};

/// Adds document projection schemas so that any node that a DLS+ index points to is present
class ProjectionSchemaAdjuster : public NodeVisitingOptimizer
{
public:
	ProjectionSchemaAdjuster(DynamicContext *context, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), context_(context) {}

	const ImpliedSchemaNode::Vector & getRoots() const { return roots_; }

protected:
	virtual void resetInternal() {}

	virtual QueryPlan *optimizePresence(PresenceQP *item);
	virtual QueryPlan *optimizeValue(ValueQP *item);
	virtual QueryPlan *optimizeRange(RangeQP *item);

	void addSchemaForIndex(PresenceQP *item);

	DynamicContext *context_;

	ImpliedSchemaNode::Vector roots_;
};

}

#endif
