//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "../Container.hpp"
#include "QueryPlanGenerator.hpp"
#include "../dataItem/DbXmlPredicate.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/LookupIndexFunction.hpp"
#include "../dataItem/DbXmlUserData.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/MetaDataFunction.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../query/ASTToQueryPlan.hpp"
#include "../query/QueryPlanToAST.hpp"
#include "../query/StructuralJoinQP.hpp"
#include "../query/IntersectQP.hpp"
#include "../query/PredicateFilterQP.hpp"
#include "../query/NodePredicateFilterQP.hpp"
#include "../query/DecisionPointQP.hpp"
#include "../query/UnionQP.hpp"
#include "../query/ExceptQP.hpp"
#include "../query/ContextNodeQP.hpp"
#include "../query/CollectionQP.hpp"
#include "../query/DocQP.hpp"
#include "../query/BufferQP.hpp"
#include "../query/StepQP.hpp"
#include "../query/VariableQP.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQMap.hpp>
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>

#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/Union.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/GeneralComp.hpp>

#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/functions/FunctionStartsWith.hpp>
#include <xqilla/functions/FunctionEndsWith.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionRoot.hpp>

using namespace DbXml;
using namespace std;

class AutoStackLevelReset
{
public:
	AutoStackLevelReset(vector<bool> &vec, bool val = false)
		: vec_(vec)
	{
		vec_.push_back(val);
	}

	~AutoStackLevelReset()
	{
		vec_.pop_back();
	}

private:
	vector<bool> &vec_;
};

class AutoStackTopReset
{
public:
	AutoStackTopReset(vector<bool> &vec, bool val = true)
		: vec_(vec), oldVal_(vec.back())
	{
		vec.back() = val;
	}

	~AutoStackTopReset()
	{
		vec_.back() = oldVal_;
	}

private:
	vector<bool> &vec_;
	bool oldVal_;
};

ASTNode *QueryPlanGenerator::toASTNode(QueryPlan *qp, DecisionPointSource *&dps, XPath2MemoryManager *mm)
{
	if(qp->getType() == QueryPlan::AST_TO_QP) {
		return ((ASTToQueryPlan*)qp)->getASTNode();
	}

	QueryPlan *dp = new (mm) DecisionPointQP(qp, dps, 0, mm);
	dp->setLocationInfo(qp);
	dps = 0;

	ASTNode *result = new (mm) QueryPlanToAST(dp, xpc_, mm);
	result->setLocationInfo(qp);
	return result;
}

QueryPlan *QueryPlanGenerator::toQueryPlan(ASTNode *ast, DecisionPointSource *&dps, XPath2MemoryManager *mm)
{
	if(ast->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST) {
		QueryPlan *result = ((QueryPlanToAST*)ast)->getQueryPlan();

		if(result->getType() == QueryPlan::DECISION_POINT) {
			DecisionPointQP *dp = (DecisionPointQP*)result;
			dps = dp->getSource();
			return dp->getArg();
		} else {
			dps = new (mm) QueryPlanDPSource(result, mm);

			QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
			dpe->setLocationInfo(ast);

			return dpe;
		}
	}

	ASTNode *check = new (mm) DbXmlNodeCheck(ast, mm);
	check->setLocationInfo(ast);
	ASTNode *docOrder = new (mm) XQDocumentOrder(check, mm);
	docOrder->setLocationInfo(ast);
        QueryPlan *result = new (mm) ASTToQueryPlan(docOrder, 0, mm);
	result->setLocationInfo(ast);

	dps = new (mm) QueryPlanDPSource(result, mm);

	QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
	dpe->setLocationInfo(ast);

	return dpe;
}

QueryPlan *QueryPlanGenerator::getContext(QueryPlan *context, DecisionPointSource *&dps, const LocationInfo *location,
	XPath2MemoryManager *mm)
{
	if(context != 0) return context;

	QueryPlan *ci = new (mm) ContextNodeQP(0, 0, 0, mm);
	ci->setLocationInfo(location);

	dps = new (mm) QueryPlanDPSource(ci, mm);

	QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
	dpe->setLocationInfo(location);

	return dpe;
}

QueryPlanGenerator::QueryPlanGenerator(DynamicContext *xpc, Optimizer *parent)
	: NodeVisitingOptimizer(parent),
	  xpc_(xpc)
{
}

ASTNode *QueryPlanGenerator::optimize(ASTNode *item)
{
	if(item->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST)
		return item;

	DecisionPointSource *dps = 0;
	GenerateResult res = generate(item, 0, dps, /*addDocOrder*/false);
	if(res.qp != 0) {
		return toASTNode(res.qp, dps, xpc_->getMemoryManager());
	}
	return res.ast;
}

ASTNode *QueryPlanGenerator::optimizePredicate(XQPredicate *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));

	{
		AutoStackTopReset reset(ancestors_);
		item->setPredicate(optimize(const_cast<ASTNode *>(item->getPredicate())));
	}

	return item;
}

ASTNode *QueryPlanGenerator::optimizeOperator(XQOperator *item)
{
	const XMLCh *name = item->getOperatorName();

	if(name == And::name || name == Or::name) {
		AutoStackTopReset reset(ancestors_);
		VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
		for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
			*i = optimize(*i);
		}
		return item;
	}

	return NodeVisitingOptimizer::optimizeOperator(item);
}

ASTNode *QueryPlanGenerator::optimizeIf(XQIf *item)
{
	{
		AutoStackTopReset reset(ancestors_);
		item->setTest(optimize(const_cast<ASTNode *>(item->getTest())));
	}

	item->setWhenTrue(optimize(const_cast<ASTNode *>(item->getWhenTrue())));
	item->setWhenFalse(optimize(const_cast<ASTNode *>(item->getWhenFalse())));
	return item;
}

ASTNode *QueryPlanGenerator::optimizeEffectiveBooleanValue(XQEffectiveBooleanValue *item)
{
	{
		AutoStackTopReset reset(ancestors_);
		item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	}
	return item;
}

TupleNode *QueryPlanGenerator::optimizeWhereTuple(WhereTuple *item)
{
	item->setParent(optimizeTupleNode(const_cast<TupleNode*>(item->getParent())));

	// Try to combine the WhereTuple with a previous ForTuple as a predicate
	ForTuple *found = item->getParent()->getType() == TupleNode::FOR ? (ForTuple*)item->getParent() : 0;

	if(found && found->getPosName() == 0 && 
		found->getExpression()->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE) &&
		(found->getExpression()->getStaticAnalysis().getProperties() & StaticAnalysis::DOCORDER) != 0) {

		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		DecisionPointSource *dps = 0;
		QueryPlan *expr = generateOrWrap(found->getExpression(), 0, dps);

		{
			AutoStackTopReset reset(ancestors_);
			generatePred(item->getExpression(), expr, QName(found->getVarURI(), found->getVarName()));
		}

		found->setExpression(toASTNode(expr, dps, mm));

		return found;
	}

	{
		AutoStackTopReset reset(ancestors_);
		item->setExpression(optimize(item->getExpression()));
	}

	return item;
}

ASTNode *QueryPlanGenerator::optimizeQuantified(XQQuantified *item)
{
	item->setParent(optimizeTupleNode(const_cast<TupleNode*>(item->getParent())));

	{
		AutoStackTopReset reset(ancestors_);
		item->setExpression(optimize(item->getExpression()));
	}

	return item;
}

bool QueryPlanGenerator::insideEBV() const
{
	return ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generate(ASTNode *item, QueryPlan *context, DecisionPointSource *&dps, bool addDocOrder)
{
	AutoStackLevelReset reset(ancestors_);

	switch(item->getType()) {
	case ASTNode::NAVIGATION: {
		return generateNav((XQNav*)item, context, dps);
	}
	case ASTNode::STEP: {
		return generateStep((XQStep*)item, context, dps);
	}
	case ASTNode::PREDICATE: {
		return generatePredicate((XQPredicate*)item, context, dps, addDocOrder);
	}
	case ASTNode::DOCUMENT_ORDER: {
		return generateDocumentOrder((XQDocumentOrder*)item, context, dps);
	}
	case ASTNode::OPERATOR: {
		return generateOperator((XQOperator*)item, context, dps);
	}
	case ASTNode::CONTEXT_ITEM: {
		return generateContextItem((XQContextItem*)item, context, dps);
	}
	case ASTNode::FUNCTION: {
		return generateFunction((XQFunction*)item, context, dps);
	}
	case ASTNode::VARIABLE: {
		return generateVariable((XQVariable*)item, context, dps);
	}
	default: break;
	}
	return NodeVisitingOptimizer::optimize(item);
}

QueryPlan *QueryPlanGenerator::generateOrWrap(ASTNode *item, QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	GenerateResult result = generate(item, context, dps);
	if(result.qp != 0) return result.qp;

	if(context != 0) {
		result.ast = new (mm) XQMap(toASTNode(context, dps, mm), result.ast, 0, 0, mm);
		result.ast->setLocationInfo(item);
		result.ast->staticTypingImpl(0);
	}

	return toQueryPlan(result.ast, dps, mm);
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateNav(XQNav *item, QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	XQNav::Steps &args = const_cast<XQNav::Steps &>(item->getSteps());

	XQNav::Steps::iterator i = args.begin();
	XQNav::Steps::iterator end = args.end();
	ASTNode *step = i->step;
	for(++i; i != end; ++i) {
		context = generateOrWrap(step, context, dps);
		step = i->step;
	}

	// The last step is allowed to return either atomic items or nodes
	GenerateResult result = generate(step, context, dps);
	if(result.qp != 0) return result.qp;

	if(context != 0) {
		result.ast = new (mm) XQMap(toASTNode(context, dps, mm), result.ast, 0, 0, mm);
		result.ast->setLocationInfo(item);
		result.ast->staticTypingImpl(0);
	}

	if(item->getStaticAnalysis().getStaticType().containsType(
                   (StaticType::StaticTypeFlags(StaticType::ANY_ATOMIC_TYPE | StaticType::FUNCTION_TYPE)))) {
		result.ast = new (mm) DbXmlLastStepCheck(result.ast, mm);
		result.ast->setLocationInfo(item);
		return result;
	}

	return toQueryPlan(result.ast, dps, mm);
}

void QueryPlanGenerator::generatePred(ASTNode *pred, QueryPlan *&expr, const QName &contextName)
{
	expr = reverseJoin(reverse(pred, ReverseResult(), contextName), expr, pred);
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generatePredicate(XQPredicate *item, QueryPlan *context, DecisionPointSource *&dps, bool addDocOrder)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	// Look for predicates that we can rewrite in reverse.
	// We can do this if the predicate is not (potentially) a single
	// numeric constant, and it doesn't use the context size or context
	// position (as this rewrite messes up the context position and size).

	ASTNode *exprAST = const_cast<ASTNode*>(item->getExpression());
	ASTNode *predAST = const_cast<ASTNode*>(item->getPredicate());

	if(!exprAST->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE) ||
		(!addDocOrder && (exprAST->getStaticAnalysis().getProperties() & StaticAnalysis::DOCORDER) == 0)) {
		return NodeVisitingOptimizer::optimize(item);
	}

	// Deal with positional predicates
	if(predAST->getStaticAnalysis().getStaticType().containsType(StaticType::NUMERIC_TYPE) ||
		predAST->getStaticAnalysis().isContextPositionUsed() ||
		predAST->getStaticAnalysis().isContextSizeUsed()) {

		DecisionPointSource *exprDPS = 0;
		GenerateResult res = generate(exprAST, 0, exprDPS);

		QueryPlan *expr = res.qp;
		if(res.ast != 0) {
			expr = toQueryPlan(res.ast, exprDPS, mm);
		}

		ASTNode *pred = optimize(optimize(predAST));

		// Reverse axis predicates need special handling, since we reorder them as we generate
		// the new query plan.
		QueryPlan *result = new (mm) NumericPredicateFilterQP(expr, pred, item->getReverse(), 0, mm);
		result->setLocationInfo(item);

		return toASTNode(result, exprDPS, mm);
	}

	QueryPlan *expr = generateOrWrap(exprAST, context, dps);

	{
		AutoStackTopReset reset(ancestors_);
		generatePred(predAST, expr, QName());
	}

	return expr;
}

// right except (left join right)
QueryPlan *QueryPlanGenerator::createNegativeJoin(Join::Type joinType, QueryPlan *left, QueryPlan *right, const LocationInfo *location,
	XPath2MemoryManager *mm)
{
	if(joinType == Join::SELF) {
		QueryPlan *result = new (mm) ExceptQP(right, left, 0, mm);
		result->setLocationInfo(location);
		return result;
	}

	BufferQP *buffer = new (mm) BufferQP(right, 0, GET_CONFIGURATION(xpc_)->allocateBufferID(), 0, mm);
	buffer->setLocationInfo(location);

	right = new (mm) BufferReferenceQP(buffer, 0, mm);
	right->setLocationInfo(location);

	QueryPlan *result = StructuralJoinQP::createJoin(joinType, left, right, 0, location, mm);

	right = new (mm) BufferReferenceQP(buffer, 0, mm);
	right->setLocationInfo(location);

	result = new (mm) ExceptQP(right, result, 0, mm);
	result->setLocationInfo(location);

	buffer->setArg(result);
	return buffer;
}

static inline bool isSuitableForIndex(const ImpliedSchemaNode::MVector &paths)
{
	for(ImpliedSchemaNode::MVector::const_iterator i = paths.begin(); i != paths.end(); ++i)
		if(!(*i)->isSuitableForIndex()) return false;
	return true;
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateStep(XQStep *item, QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	DBXML_ASSERT(ud != 0);

	// Don't generate QueryPlan for wildcard lookups - they'll end up as sequential scans
	// which can't handle them properly
	// TBD handle these in SequentialScanQP - jpcs
	if(!ud->paths.empty() && isSuitableForIndex(ud->paths)) {
		switch(item->getAxis()) {
		case XQStep::PARENT:
		case XQStep::DESCENDANT_OR_SELF:
		case XQStep::SELF:
		case XQStep::ANCESTOR:
		case XQStep::ANCESTOR_OR_SELF:
		case XQStep::ATTRIBUTE:
		case XQStep::CHILD:
		case XQStep::DESCENDANT: {
			PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
			pathsqp->setLocationInfo(item);
			return StructuralJoinQP::createJoin((Join::Type)item->getAxis(), getContext(context, dps, item, mm), pathsqp, 0, item, mm);
		}
		case XQStep::FOLLOWING:
		case XQStep::PRECEDING:
		case XQStep::PRECEDING_SIBLING:
		case XQStep::FOLLOWING_SIBLING: {
			// TBD implement joins for these axes - jpcs
			break;
		}
		case XQStep::NAMESPACE:
		default: {
			DBXML_ASSERT(false);
			// Do nothing
			break;
		}
		}
	}

	StepQP *result = new (mm) StepQP(getContext(context, dps, item, mm),
		(Join::Type)item->getAxis(), (DbXmlNodeTest*)item->getNodeTest(), 0, 0, mm);
	result->setLocationInfo(item);
	result->addPaths(ud->paths);
	return result;
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateDocumentOrder(XQDocumentOrder *item, QueryPlan *context, DecisionPointSource *&dps)
{
	GenerateResult result = generate(const_cast<ASTNode*>(item->getExpression()), context, dps);

	if(result.ast != 0) {
		// Add the XQDocumentOrder back if the result is an ASTNode
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		result.ast = new (mm) XQDocumentOrder(result.ast, mm);
		result.ast->setLocationInfo(item);
	}

	return result;
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateContextItem(XQContextItem *item, QueryPlan *context, DecisionPointSource *&dps)
{
	if(item->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE)) {
		return getContext(context, dps, item, xpc_->getMemoryManager());
	}
	return NodeVisitingOptimizer::optimize(item);
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateVariable(XQVariable *item, QueryPlan *context, DecisionPointSource *&dps)
{
	if(item->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE) &&
		(item->getStaticAnalysis().getProperties() & StaticAnalysis::DOCORDER) != 0) {
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		// Find a DbXmlNodeTest from the user data paths
		DbXmlNodeTest *nodeTest = 0;
		DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
		if(ud != 0) {
			ImpliedSchemaNode::MVector::iterator i = ud->paths.begin();
			if(i != ud->paths.end()) {
				nodeTest = StepQP::findNodeTest(*i);
				for(++i; i != ud->paths.end(); ++i) {
					DbXmlNodeTest *tmp = StepQP::findNodeTest(*i);
					if(DbXmlNodeTest::isSubsetOf(nodeTest, tmp))
						nodeTest = tmp;
					else if(!DbXmlNodeTest::isSubsetOf(tmp, nodeTest)) {
						// There's no single DbXmlNodeTest that covers
						// everything this variable will return
						nodeTest = 0;
						break;
					}
				}
			}
		}

		QueryPlan *var = new (mm) VariableQP(item->getPrefix(), item->getURI(), item->getName(), 0, nodeTest, 0, mm);
		var->setLocationInfo(item);
		const_cast<StaticAnalysis&>(var->getStaticAnalysis())
			.copy(item->getStaticAnalysis());

		dps = new (mm) QueryPlanDPSource(var, mm);

		QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
		dpe->setLocationInfo(item);

		return dpe;
	}
	return NodeVisitingOptimizer::optimize(item);
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateFunction(XQFunction *item, QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());

	if(uri == XQFunction::XMLChFunctionURI) {
		if(name == FunctionCollection::name) {
			DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
			DBXML_ASSERT(ud != 0);
			DBXML_ASSERT(ud->paths.size() == 1);

			ASTNode *arg = args.size() == 0 ? 0 : optimize(args[0]);
			CollectionQP *col = new (mm) CollectionQP(arg, ud->paths[0], xpc_, 0, mm);
			col->setLocationInfo(item);
			
			ud->paths[0]->setQueryPlanRoot(col);

			dps = new (mm) QueryPlanDPSource(col, mm);

			QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
			dpe->setLocationInfo(item);

			return dpe;
		}

		else if(name == FunctionDoc::name) {
			DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
			DBXML_ASSERT(ud != 0);
			DBXML_ASSERT(ud->paths.size() == 1);

			DocQP *doc = new (mm) DocQP(optimize(args[0]), (ImpliedSchemaNode*)ud->paths[0]->getRoot(), xpc_, 0, mm);
			doc->setLocationInfo(item);
			
			((ImpliedSchemaNode*)ud->paths[0]->getRoot())->setQueryPlanRoot(doc);

			dps = new (mm) QueryPlanDPSource(doc, mm);

			QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
			dpe->setLocationInfo(item);

			return dpe;
		}

		// TBD What do we do about trees without a document node at their root? - jpcs
// 		else if(name == FunctionRoot::name) {
// 			DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
// 			DBXML_ASSERT(ud != 0);
// 			DBXML_ASSERT(!ud->paths.empty());

// 			PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
// 			pathsqp->setLocationInfo(item);
// 			return StructuralJoinQP::createJoin(Join::ANCESTOR_OR_SELF, getContext(context, dps, item, mm), pathsqp, 0, item, mm);
// 		}

		else if(name == FunctionContains::name ||
			name == FunctionStartsWith::name ||
			name == FunctionEndsWith::name) {

			DecisionPointSource *newdps = 0;
			GenerateResult res = generateContains(item, 0, newdps);
			if(res.qp != 0) {
				// Add an effective boolean value object
				res.ast = new (mm) XQEffectiveBooleanValue(toASTNode(res.qp, newdps, mm), mm);
				res.ast->setLocationInfo(item);
				res.qp = 0;
			}
			return res;
		}

	} else if(uri == DbXmlFunction::XMLChFunctionURI) {
		if(name == DbXmlContainsFunction::name) {

			DecisionPointSource *newdps = 0;
			GenerateResult res = generateContains(item, 0, newdps);
			if(res.qp != 0) {
				// Add an effective boolean value object
				res.ast = new (mm) XQEffectiveBooleanValue(toASTNode(res.qp, newdps, mm), mm);
				res.ast->setLocationInfo(item);
				res.qp = 0;
			}
			return res;
		}

		else if(name == LookupIndexFunction::name ||
			name == LookupAttributeIndexFunction::name ||
			name == LookupMetaDataIndexFunction::name) {
			LookupIndexFunction *fun = (LookupIndexFunction*)item;

			DbXmlConfiguration *conf = GET_CONFIGURATION(xpc_);
			ContainerBase *container = fun->getContainerBase();

			if(container != 0) {
				if(container->getContainer() != 0 && container->getContainer()->isWholedocContainer()) {
				
					// Add the implied schema to the store, to use for document projection
					DbXmlUserData *ud = (DbXmlUserData*)fun->getUserData();
					DBXML_ASSERT(ud != 0);
					DBXML_ASSERT(!ud->paths.empty());

					conf->addImpliedSchemaNode(container->getContainerID(), (ImpliedSchemaNode*)ud->paths[0]->getRoot());
				}
			} else {
				// Turn document projection off everywhere - it's not safe to project
				// documents if we can't identify everywhere a document will be used at
				// compile time.
				conf->overrideProjection();
			}


			QueryPlan *result = fun->createQueryPlan(xpc_, /*lookup*/false);
			if(result != 0) {
				dps = new (mm) QueryPlanDPSource(result, mm);

				QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
				dpe->setLocationInfo(item);

				return dpe;
			}
		}
	}

	return NodeVisitingOptimizer::optimize(item);
}

QueryPlan *QueryPlanGenerator::generateOrWrap(const VectorOfASTNodes &args, QueryPlan *resultQP, OperationQP::Vector &resultArgs,
	QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	// We need to buffer the context, in case more than one argument needs to use it
	BufferQP *contextBuffer = 0;
	if(context != 0) {
		if(dps != 0) {
			context = new (mm) DecisionPointQP(context, dps, 0, mm);
			context->setLocationInfo(resultQP);
			dps = 0;
		}
		contextBuffer = new (mm) BufferQP(context, resultQP,
			GET_CONFIGURATION(xpc_)->allocateBufferID(), 0, mm);
		contextBuffer->setLocationInfo(resultQP);
	}

	for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
		QueryPlan *argContext = 0;
		DecisionPointSource *argDPS = 0;
		if(contextBuffer != 0) {
			argContext = new (mm) BufferReferenceQP(contextBuffer, 0, mm);
			argContext->setLocationInfo(*i);

			argDPS = new (mm) QueryPlanDPSource(argContext, mm);

			argContext = new (mm) DecisionPointEndQP((QueryPlanDPSource*)argDPS, 0, mm);
			argContext->setLocationInfo(*i);
		}

		QueryPlan *arg = generateOrWrap(*i, argContext, argDPS);
		if(argDPS != 0) {
			arg = new (mm) DecisionPointQP(arg, argDPS, 0, mm);
			arg->setLocationInfo(*i);
			argDPS = 0;
		}

		resultArgs.push_back(arg);
	}

	if(contextBuffer == 0) return resultQP;
	return contextBuffer;
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateOperator(XQOperator *item, QueryPlan *context, DecisionPointSource *&dps)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const XMLCh *name = item->getOperatorName();
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());

	if(insideEBV() && (name == Equals::name || name == LessThan::name || name == LessThanEqual::name ||
			name == GreaterThan::name || name == GreaterThanEqual::name || name == NotEquals::name)) {
		DecisionPointSource *newdps = 0;
		GenerateResult res = generateComparison(item, 0, newdps, name == NotEquals::name);
		if(res.qp != 0) {
			// Add an effective boolean value object
			res.ast = new (mm) XQEffectiveBooleanValue(toASTNode(res.qp, newdps, mm), mm);
			res.ast->setLocationInfo(item);
			res.qp = 0;
		}
		return res;
	}

	else if(name == GeneralComp::name) {
		DecisionPointSource *newdps = 0;
		GenerateResult res = generateComparison(item, 0, newdps, ((const GeneralComp *)item)->getOperation()
			== GeneralComp::NOT_EQUAL);
		if(res.qp != 0) {
			// Add an effective boolean value object
			res.ast = new (mm) XQEffectiveBooleanValue(toASTNode(res.qp, newdps, mm), mm);
			res.ast->setLocationInfo(item);
			res.qp = 0;
		}
		return res;
	}

	else if(item->getOperatorName() == Union::name) {
		UnionQP *unionOp = new (mm) UnionQP(0, mm);
		unionOp->setLocationInfo(item);

		QueryPlan *result = generateOrWrap(args, unionOp, const_cast<OperationQP::Vector&>(unionOp->getArgs()),
			context, dps);

		// Add a DecisionPointEndQP here anyhow - we could be getting nodes from different
		// containers because of the union
		dps = new (mm) QueryPlanDPSource(result, mm);

		QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
		dpe->setLocationInfo(item);

		return dpe;
	}

	else if(item->getOperatorName() == Intersect::name) {
		IntersectQP *intersectOp = new (mm) IntersectQP(0, mm);
		intersectOp->setLocationInfo(item);

		QueryPlan *result = generateOrWrap(args, intersectOp, const_cast<OperationQP::Vector&>(intersectOp->getArgs()),
			context, dps);

		// Add a DecisionPointEndQP here anyhow - we could be getting nodes from different
		// containers because of the intersect
		dps = new (mm) QueryPlanDPSource(result, mm);

		QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
		dpe->setLocationInfo(item);

		return dpe;
	}

	else if(item->getOperatorName() == Except::name) {
		ExceptQP *exceptOp = new (mm) ExceptQP(0, 0, 0, mm);
		exceptOp->setLocationInfo(item);

		OperationQP::Vector results = OperationQP::Vector(XQillaAllocator<QueryPlan*>(mm));
		QueryPlan *result = generateOrWrap(args, exceptOp, results, context, dps);

		exceptOp->setLeftArg(results[0]);
		exceptOp->setRightArg(results[1]);

		// Add a DecisionPointEndQP here anyhow - we could be getting nodes from different
		// containers because of the intersect
		dps = new (mm) QueryPlanDPSource(result, mm);

		QueryPlan *dpe = new (mm) DecisionPointEndQP((QueryPlanDPSource*)dps, 0, mm);
		dpe->setLocationInfo(item);

		return dpe;
	}

	return NodeVisitingOptimizer::optimize(item);
}

static ASTNode *findNodeProducer(ASTNode *arg, StaticType::StaticTypeFlags nodeTypes, XQCastAs *&cast)
{
	cast = 0;
	while(true) {
		switch(arg->getType()) {
		case ASTNode::TREAT_AS: {
			arg = const_cast<ASTNode*>(((XQTreatAs*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_ANY_URI: {
			arg = const_cast<ASTNode*>(((XQPromoteAnyURI*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_NUMERIC: {
			arg = const_cast<ASTNode*>(((XQPromoteNumeric*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_UNTYPED: {
			arg = const_cast<ASTNode*>(((XQPromoteUntyped*)arg)->getExpression());
			break;
		}
		case ASTNode::ATOMIZE: {
			arg = const_cast<ASTNode*>(((XQAtomize*)arg)->getExpression());
			break;
		}
		case ASTNode::CAST_AS: {
			if(cast != 0) {
				// Can't handle more than one cast
				cast = 0;
				return 0;
			}
			cast = (XQCastAs*)arg;
			arg = const_cast<ASTNode*>(((XQCastAs*)arg)->getExpression());
			break;
		}
		default: {
			if(arg->getStaticAnalysis().getStaticType().isType(nodeTypes))
				return arg;
			return 0;
		}
		}
	}

	return 0;
}

static MetaDataFunction *findMetaDataFunction(ASTNode *arg, XQCastAs *&cast)
{
	cast = 0;
	while(true) {
		switch(arg->getType()) {
		case ASTNode::TREAT_AS: {
			arg = const_cast<ASTNode*>(((XQTreatAs*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_ANY_URI: {
			arg = const_cast<ASTNode*>(((XQPromoteAnyURI*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_NUMERIC: {
			arg = const_cast<ASTNode*>(((XQPromoteNumeric*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_UNTYPED: {
			arg = const_cast<ASTNode*>(((XQPromoteUntyped*)arg)->getExpression());
			break;
		}
		case ASTNode::ATOMIZE: {
			arg = const_cast<ASTNode*>(((XQAtomize*)arg)->getExpression());
			break;
		}
		case ASTNode::CAST_AS: {
			if(cast != 0) {
				// Can't handle more than one cast
				cast = 0;
				return 0;
			}
			cast = (XQCastAs*)arg;
			arg = const_cast<ASTNode*>(((XQCastAs*)arg)->getExpression());
			break;
		}
		case ASTNode::FUNCTION: {
			if(((XQFunction*)arg)->getFunctionName() == MetaDataFunction::name &&
				((XQFunction*)arg)->getFunctionURI() == DbXmlFunction::XMLChFunctionURI) {
				return (MetaDataFunction*)arg;
			}
			return 0;
		}
		default: {
			return 0;
		}
		}
	}

	return 0;
}

static inline void swapASTNode(ASTNode *&a, ASTNode *&b)
{
	ASTNode *tmp = a;
	a = b;
	b = tmp;
}

static inline void swapCast(XQCastAs *&a, XQCastAs *&b)
{
	XQCastAs *tmp = a;
	a = b;
	b = tmp;
}

static inline void swapPaths(ImpliedSchemaNode::MVector *&a, ImpliedSchemaNode::MVector *&b)
{
	ImpliedSchemaNode::MVector *tmp = a;
	a = b;
	b = tmp;
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateComparison(XQOperator *item, QueryPlan *context,
	DecisionPointSource *&dps, bool negate)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	ASTNode *arg0 = item->getArgument(0);
	ASTNode *arg1 = item->getArgument(1);

	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	ImpliedSchemaNode::MVector *paths0 = &ud->paths;
	ImpliedSchemaNode::MVector *paths1 = &ud->paths2;

	XQCastAs *arg0Cast, *arg1Cast;
	ASTNode *arg0Nodes = findNodeProducer(arg0, (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg0Cast);
	ASTNode *arg1Nodes = findNodeProducer(arg1, (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg1Cast);

	if(arg1Nodes != 0 && !paths1->empty() && (context == 0 || !arg0->getStaticAnalysis().areContextFlagsUsed())) {
		if(arg0Nodes == 0 || paths0->empty() || (context != 0 && arg1->getStaticAnalysis().areContextFlagsUsed())) {
			// Swap the arguments, and the operation
			swapASTNode(arg0, arg1);
			swapASTNode(arg0Nodes, arg1Nodes);
			swapCast(arg0Cast, arg1Cast);
			swapPaths(paths0, paths1);
		} else {
			// TBD Perform a value join if both arguments are nodes - jpcs
		}
	}

	if(arg0Nodes != 0 && !paths0->empty() && (context == 0 || !arg1->getStaticAnalysis().areContextFlagsUsed())) {
		GenerateResult result = generate(arg0Nodes, context, dps);
		if(result.qp != 0) {
			PathsQP *pathsqp = new (mm) PathsQP(*paths0, mm);
			pathsqp->setLocationInfo(item);

			// Optimise the ASTNode values in the paths
			for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
				if((*it)->getASTNode() != 0) {
					(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
				}
			}

			if(negate) {
				return createNegativeJoin(Join::SELF, pathsqp, result.qp, item, mm);
			} else {
				return StructuralJoinQP::createJoin(Join::SELF, pathsqp, result.qp, 0, item, mm);
			}
		}
	}

	MetaDataFunction *mdFunc = 0;
	if(arg0->getType() == ASTNode::FUNCTION &&
		((XQFunction*)arg0)->getFunctionName() == MetaDataFunction::name &&
		((XQFunction*)arg0)->getFunctionURI() == DbXmlFunction::XMLChFunctionURI) {
		mdFunc = (MetaDataFunction*)arg0;
		paths0 = &ud->paths;
	} else if(arg1->getType() == ASTNode::FUNCTION &&
		((XQFunction*)arg1)->getFunctionName() == MetaDataFunction::name &&
		((XQFunction*)arg1)->getFunctionURI() == DbXmlFunction::XMLChFunctionURI) {
		mdFunc = (MetaDataFunction*)arg1;
		paths0 = &ud->paths2;
	}

	if(mdFunc != 0 && !paths0->empty()) {
		GenerateResult result((QueryPlan*)0);
		if(mdFunc->getArguments().size() > 1) {
			XQCastAs *cast;
			ASTNode *nodeArg = findNodeProducer(mdFunc->getArguments()[1], StaticType::NODE_TYPE, cast);
			if(nodeArg != 0 && cast == 0)
				result = generate(nodeArg, context, dps);
		} else {
			result = getContext(context, dps, item, mm);
		}

		if(result.qp != 0) {
			PathsQP *pathsqp = new (mm) PathsQP(*paths0, mm);
			pathsqp->setLocationInfo(item);

			// Optimise the ASTNode values in the paths
			for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
				if((*it)->getASTNode() != 0) {
					(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
				}
			}

			if(negate) {
				return createNegativeJoin(Join::DESCENDANT_OR_SELF, pathsqp, result.qp, item, mm);
			} else {
				return StructuralJoinQP::createJoin(Join::DESCENDANT_OR_SELF, pathsqp, result.qp, 0, item, mm);
			}
		}
	}

	return NodeVisitingOptimizer::optimize(item);
}

QueryPlanGenerator::GenerateResult QueryPlanGenerator::generateContains(XQFunction *item, QueryPlan *context, DecisionPointSource *&dps)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes&>(item->getArguments());

	// Make sure there is no collation argument
	if(args.size() == 2) {
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		XQCastAs *arg0Cast;
		ASTNode *arg0 = findNodeProducer(args[0], (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg0Cast);
		ASTNode *arg1 = args[1];
		DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();

		if(arg0 != 0 && arg0Cast == 0 && !ud->paths.empty() &&
			!arg1->getStaticAnalysis().areContextFlagsUsed()) {

			GenerateResult result = generate(arg0, context, dps);
			if(result.qp != 0) {
				PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
				pathsqp->setLocationInfo(item);

				// Optimise the ASTNode values in the paths
				for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
					if((*it)->getASTNode() != 0) {
						(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
					}
				}

				return StructuralJoinQP::createJoin(Join::SELF, pathsqp, getContext(result.qp, dps, item, mm), 0, item, mm);
			}
		}

		arg0 = args[0];
		if(arg0->getType() == ASTNode::FUNCTION &&
			((XQFunction*)arg0)->getFunctionName() == MetaDataFunction::name &&
			((XQFunction*)arg0)->getFunctionURI() == DbXmlFunction::XMLChFunctionURI &&
			!ud->paths.empty()) {
			MetaDataFunction *mdFunc = (MetaDataFunction*)arg0;

			GenerateResult result((QueryPlan*)0);
			if(mdFunc->getArguments().size() > 1) {
				XQCastAs *cast;
				ASTNode *nodeArg = findNodeProducer(mdFunc->getArguments()[1], StaticType::NODE_TYPE, cast);
				if(nodeArg != 0 && cast == 0)
					result = generate(nodeArg, context, dps);
			} else {
				result = getContext(context, dps, item, mm);
			}

			if(result.qp != 0) {
				PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
				pathsqp->setLocationInfo(item);

				// Optimise the ASTNode values in the paths
				for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
					if((*it)->getASTNode() != 0) {
						(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
					}
				}

				return StructuralJoinQP::createJoin(Join::DESCENDANT_OR_SELF, pathsqp, result.qp, 0, item, mm);
			}
		}
	}

	return NodeVisitingOptimizer::optimize(item);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverse(ASTNode *item, const ReverseResult &context, const QName &contextName)
{
	switch(item->getType()) {
	case ASTNode::NAVIGATION: {
		XQNav::Steps &args = const_cast<XQNav::Steps &>(((XQNav*)item)->getSteps());
		return reverseNav(args.begin(), args.end(), context, contextName);
	}
	case ASTNode::STEP: {
		return reverseStep((XQStep*)item, context, contextName);
	}
	case ASTNode::PREDICATE: {
		return reversePredicate((XQPredicate*)item, context, contextName);
	}
	case ASTNode::DOCUMENT_ORDER: {
		return reverseDocumentOrder((XQDocumentOrder*)item, context, contextName);
	}
	case ASTNode::OPERATOR: {
		return reverseOperator((XQOperator*)item, context, contextName);
	}
	case ASTNode::FUNCTION: {
		return reverseFunction((XQFunction*)item, context, contextName);
	}
	case ASTNode::EBV: {
		return reverseEffectiveBooleanValue((XQEffectiveBooleanValue*)item, context, contextName);
	}
	case ASTNode::CONTEXT_ITEM: {
		return reverseContextItem((XQContextItem*)item, context, contextName);
	}
	case ASTNode::VARIABLE: {
		return reverseVariable((XQVariable*)item, context, contextName);
	}
	case ASTNode::QUANTIFIED: {
		return reverseQuantified((XQQuantified*)item, context, contextName);
	}
	default: break;
	}
	return ReverseResult(reverseJoin(context, item, item), contextName);
}

QueryPlan *QueryPlanGenerator::reverseJoin(const ReverseResult &context, QueryPlan *qp, const LocationInfo *location)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(!context.subResults.empty()) {
		if(context.negate) {
			for(vector<ReverseResult>::const_iterator i = context.subResults.begin(); i != context.subResults.end(); ++i) {
				qp = reverseJoin(*i, qp, location);
			}
			return qp;
		} else {
			OperationQP *op = new (mm) UnionQP(0, mm);
			op->setLocationInfo(location);

			BufferQP *buffer = new (mm) BufferQP(qp, 0, GET_CONFIGURATION(xpc_)->allocateBufferID(), 0, mm);
			buffer->setLocationInfo(location);

			for(vector<ReverseResult>::const_iterator i = context.subResults.begin(); i != context.subResults.end(); ++i) {
				BufferReferenceQP *ref = new (mm) BufferReferenceQP(buffer, 0, mm);
				ref->setLocationInfo(location);

				op->addArg(reverseJoin(*i, ref, location));
			}

			buffer->setArg(op);
			return buffer;
		}
	} else if(context.qp != 0) {
		if(context.uses(context.name, xpc_)) {
			// If the context variable is still used, we perform the join with a variable
			// lookup and add a NodePredicateFilterQP object to keep the variable binding

			ASTNode *var;
			if(context.name.name == 0) var = new (mm) XQContextItem(mm);
			else var = new (mm) XQVariable(context.name.uri, context.name.name, mm);
			var->setLocationInfo(location);

			StaticAnalysis &varSrc = const_cast<StaticAnalysis&>(var->getStaticAnalysis());
			qp->staticTypingLite(xpc_);
			varSrc.getStaticType() = qp->getStaticAnalysis().getStaticType();
			varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
				StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
				StaticAnalysis::ONENODE | StaticAnalysis::SELF);
			if(context.name.name) varSrc.variableUsed(context.name.uri, context.name.name);
			else varSrc.contextItemUsed(true);

			DecisionPointSource *var_dps = 0;
			QueryPlan *var_qp = toQueryPlan(optimize(var), var_dps, mm);

			QueryPlan *join;
			if(context.negate) {
				join = createNegativeJoin(context.joinType, context.qp, var_qp, location, mm);
			} else {
				join = StructuralJoinQP::createJoin(context.joinType, context.qp, var_qp, 0, location, mm);
			}

			QueryPlan *result = new (mm) PredicateFilterQP(qp, toASTNode(join, var_dps, mm), context.name.uri, context.name.name, 0, mm);
			result->setLocationInfo(location);
			return result;
		}

		if(context.negate) {
			return createNegativeJoin(context.joinType, context.qp, qp, location, mm);
		} else {
			return StructuralJoinQP::createJoin(context.joinType, context.qp, qp, 0, location, mm);
		}
	} else if(context.ast != 0) {
		// The context has not been reversed
		QueryPlan *result = new (mm) PredicateFilterQP(qp, optimize(context.ast), context.name.uri, context.name.name, 0, mm);
		result->setLocationInfo(location);
		return result;
	}

	return qp;
}

ASTNode *QueryPlanGenerator::reverseJoin(const ReverseResult &context, ASTNode *ast, const LocationInfo *location)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(!context.subResults.empty()) {
		if(context.negate) {
			for(vector<ReverseResult>::const_iterator i = context.subResults.begin(); i != context.subResults.end(); ++i) {
				ast = reverseJoin(*i, ast, location);
			}
			return ast;
		} else {
			const XMLCh *tmpVar = GET_CONFIGURATION(xpc_)->allocateTempVarName(mm);

			Or *orOp = new (mm) Or(VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm)), mm);
			orOp->setLocationInfo(location);

			for(vector<ReverseResult>::const_iterator i = context.subResults.begin(); i != context.subResults.end(); ++i) {
				ASTNode *var = new (mm) XQVariable(0, tmpVar, mm);
				var->setLocationInfo(location);

				StaticAnalysis &varSrc = const_cast<StaticAnalysis&>(var->getStaticAnalysis());
				varSrc.getStaticType() = ast->getStaticAnalysis().getStaticType();
				varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
					StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
					StaticAnalysis::ONENODE | StaticAnalysis::SELF);
				varSrc.variableUsed(0, tmpVar);

				ASTNode *arg = new XQEffectiveBooleanValue(reverseJoin(*i, var, location), mm);
				arg->setLocationInfo(location);
				orOp->addArgument(arg);
			}

			DecisionPointSource *dps = 0;
			QueryPlan *qp = toQueryPlan(optimize(ast), dps, mm);

			QueryPlan *result = new (mm) PredicateFilterQP(qp, orOp, 0, tmpVar, 0, mm);
			result->setLocationInfo(location);

			return toASTNode(result, dps, mm);
		}
	} else if(context.qp != 0) {
		if(context.uses(context.name, xpc_)) {
			// If the context variable is still used, we perform the join with a variable
			// lookup and add a PredicateFilterQP object to keep the variable binding

			ASTNode *var;
			if(context.name.name == 0) var = new (mm) XQContextItem(mm);
			else var = new (mm) XQVariable(context.name.uri, context.name.name, mm);
			var->setLocationInfo(location);

			StaticAnalysis &varSrc = const_cast<StaticAnalysis&>(var->getStaticAnalysis());
			varSrc.getStaticType() = ast->getStaticAnalysis().getStaticType();
			varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
				StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
				StaticAnalysis::ONENODE | StaticAnalysis::SELF);
			if(context.name.name) varSrc.variableUsed(context.name.uri, context.name.name);
			else varSrc.contextItemUsed(true);

			DecisionPointSource *var_dps = 0;
			QueryPlan *var_qp = toQueryPlan(optimize(var), var_dps, mm);

			QueryPlan *join;
			if(context.negate) {
				join = createNegativeJoin(context.joinType, context.qp, var_qp, location, mm);
			} else {
				join = StructuralJoinQP::createJoin(context.joinType, context.qp, var_qp, 0, location, mm);
			}

			DecisionPointSource *dps = 0;
			QueryPlan *qp = toQueryPlan(optimize(ast), dps, mm);

			QueryPlan *result = new (mm) PredicateFilterQP(qp, toASTNode(join, var_dps, mm), context.name.uri, context.name.name, 0, mm);
			result->setLocationInfo(location);

			return toASTNode(result, dps, mm);
		}

		// The context has been reversed, so we need to join
		// it to this step
		DecisionPointSource *dps = 0;
		QueryPlan *qp = toQueryPlan(optimize(ast), dps, mm);

		QueryPlan *join;
		if(context.negate) {
			join = createNegativeJoin(context.joinType, context.qp, qp, location, mm);
		} else {
			join = StructuralJoinQP::createJoin(context.joinType, context.qp, qp, 0, location, mm);
		}

		return toASTNode(join, dps, mm);
	} else if(context.ast != 0) {
		if(ast->getStaticAnalysis().getStaticType().containsType(StaticType::ANY_ATOMIC_TYPE)) {
			ASTNode *result = new (mm) DbXmlPredicate(ast, optimize(context.ast), context.name.uri, context.name.name, mm);
			result->setLocationInfo(location);
			return result;
		} else {
			DecisionPointSource *dps = 0;
			QueryPlan *qp = toQueryPlan(optimize(ast), dps, mm);

			QueryPlan *result = new (mm) PredicateFilterQP(qp, optimize(context.ast), context.name.uri, context.name.name, 0, mm);
			result->setLocationInfo(location);

			return toASTNode(result, dps, mm);
		}
	}

	return optimize(ast);
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseNav(XQNav::Steps::iterator i, XQNav::Steps::iterator end, const ReverseResult &context, const QName &contextName)
{
	if(i == end) return context;
	return reverse(i->step, reverseNav(i + 1, end, context, QName()), contextName);
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseStep(XQStep *item, const ReverseResult &context, const QName &contextName)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(contextName.name != 0) return ReverseResult(reverseJoin(context, item, item), contextName);

	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	DBXML_ASSERT(ud != 0);

	// Don't generate QueryPlan for wildcard lookups - they'll end up as sequential scans
	// which can't handle them properly
	// TBD handle these in SequentialScanQP - jpcs
	if(!ud->paths.empty() && isSuitableForIndex(ud->paths)) {
		switch(item->getAxis()) {
		case XQStep::PARENT:
		case XQStep::DESCENDANT_OR_SELF:
		case XQStep::SELF:
		case XQStep::ANCESTOR:
		case XQStep::ANCESTOR_OR_SELF:
		case XQStep::ATTRIBUTE:
		case XQStep::CHILD:
		case XQStep::DESCENDANT: {
			PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
			pathsqp->setLocationInfo(item);
			return ReverseResult(reverseJoin(context, pathsqp, item),
				Join::inverse((Join::Type)item->getAxis()), contextName);
		}
		case XQStep::FOLLOWING:
		case XQStep::PRECEDING:
		case XQStep::PRECEDING_SIBLING:
		case XQStep::FOLLOWING_SIBLING: {
			// TBD implement joins for these axes - jpcs
			break;
		}
		case XQStep::NAMESPACE:
		default: {
			DBXML_ASSERT(false);
			// Do nothing
			break;
		}
		}
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reversePredicate(XQPredicate *item, const ReverseResult &context, const QName &contextName)
{
	// Look for predicates that we can change into DbXmlFilter steps.
	// We can do this if the predicate is not (potentially) a single
	// numeric constant, and it doesn't use the context size or context
	// position (as this rewrite messes up the context position and size).

	ASTNode *exprAST = const_cast<ASTNode*>(item->getExpression());
	ASTNode *predAST = const_cast<ASTNode*>(item->getPredicate());

	if(predAST->getStaticAnalysis().getStaticType().containsType(StaticType::NUMERIC_TYPE) ||
		predAST->getStaticAnalysis().isContextPositionUsed() ||
		predAST->getStaticAnalysis().isContextSizeUsed() ||
		!exprAST->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE)) {
		return ReverseResult(reverseJoin(context, item, item), contextName);
	}

	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	ReverseResult expr = reverse(exprAST, context, contextName);

	DecisionPointSource *dps = 0;
	QueryPlan *exprQP = expr.qp;

	if(!expr.subResults.empty()) {
		return ReverseResult(reverseJoin(context, item, item), contextName);
	} else if(expr.ast != 0) {
		exprQP = toQueryPlan(expr.ast, dps, mm);
	} else if(expr.qp == 0) {
		// This is a special case for expression like this:
		// collection()/doc[.[@type="foo"]]
		return reverse(predAST, ReverseResult(), QName());
	}

	{
		AutoStackTopReset reset(ancestors_);
		generatePred(predAST, exprQP, QName());
	}

	if(expr.ast != 0) {
		expr.ast = toASTNode(exprQP, dps, mm);
	} else {
		expr.qp = exprQP;
	}

	return expr;
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseDocumentOrder(XQDocumentOrder *item, const ReverseResult &context, const QName &contextName)
{
	ReverseResult result = reverse(const_cast<ASTNode*>(item->getExpression()), context, contextName);

	if(result.ast != 0) {
		// Add the XQDocumentOrder back if the reverse has failed
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		result.ast = new (mm) XQDocumentOrder(result.ast, mm);
		result.ast->setLocationInfo(item);
	}

	return result;
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseContextItem(XQContextItem *item, const ReverseResult &context, const QName &contextName)
{
	if(contextName.name != 0) return ReverseResult(reverseJoin(context, item, item), contextName);

	ReverseResult result(context);
	result.name = contextName;
	return result;
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseVariable(XQVariable *item, const ReverseResult &context, const QName &contextName)
{
	if(context.ast != 0 || !XPath2Utils::equals(item->getName(), contextName.name) ||
		!XPath2Utils::equals(item->getURI(), contextName.uri))
		return ReverseResult(reverseJoin(context, item, item), contextName);

	ReverseResult result(context);
	result.name = contextName;
	return result;
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseOperator(XQOperator *item, const ReverseResult &context, const QName &contextName)
{
	// TBD union, intersect and except using buffer? - jpcs

	const XMLCh *name = item->getOperatorName();

	if(name == Or::name) {
		ReverseResult result;
		for(unsigned int index = 0; index < item->getNumArgs(); ++index) {
			result.subResults.push_back(reverse(item->getArgument(index), context, contextName));
		}
		return result;
	}

	else if(name == And::name) {
		ReverseResult result;
		result.negate = true;
		for(unsigned int index = 0; index < item->getNumArgs(); ++index) {
			result.subResults.push_back(reverse(item->getArgument(index), context, contextName));
		}
		return result;
	}

	else if(name == NotEquals::name) {
		ReverseResult result = reverseComparison(item, context, contextName);
		if(result.qp != 0) {
			result.inverse(xpc_->getMemoryManager(), item);
		}
		return result;
	}

	else if(name == Equals::name || name == LessThan::name || name == LessThanEqual::name ||
		name == GreaterThan::name || name == GreaterThanEqual::name) {
		return reverseComparison(item, context, contextName);
	}

	else if(name == GeneralComp::name) {
		ReverseResult result = reverseComparison(item, context, contextName);
		if(result.qp && ((const GeneralComp *)item)->getOperation() == GeneralComp::NOT_EQUAL) {
			result.inverse(xpc_->getMemoryManager(), item);
		}
		return result;
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseEffectiveBooleanValue(XQEffectiveBooleanValue *item, const ReverseResult &context, const QName &contextName)
{
	ReverseResult expr = reverse(const_cast<ASTNode*>(item->getExpression()), context, contextName);

	if(expr.ast != 0) {
		item->setExpression(expr.ast);
		expr.ast = item;
	}

	return expr;
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseFunction(XQFunction *item, const ReverseResult &context, const QName &contextName)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	if(uri == XQFunction::XMLChFunctionURI) {

		if(name == FunctionNot::name || name == FunctionEmpty::name) {
			ReverseResult expr = reverse(args[0], context, contextName);

			if(expr.ast != 0) {
				args[0] = expr.ast;
				expr.ast = item;
			} else {
				expr.inverse(xpc_->getMemoryManager(), item);
			}

			return expr;
		}
		else if(name == FunctionExists::name || name == FunctionBoolean::name) {
			ReverseResult expr = reverse(args[0], context, contextName);

			if(expr.ast != 0) {
				args[0] = expr.ast;
				expr.ast = item;
			}

			return expr;
		}
		else if(name == FunctionContains::name ||
			name == FunctionStartsWith::name ||
			name == FunctionEndsWith::name) {
			return reverseContains(item, context, contextName);
		}
	} else if(uri == DbXmlFunction::XMLChFunctionURI) {
		if(name == DbXmlContainsFunction::name) {
			return reverseContains(item, context, contextName);
		}
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

bool QueryPlanGenerator::ReverseResult::uses(const QName &name, StaticContext *context) const
{
	if(!subResults.empty()) {
		for(vector<ReverseResult>::const_iterator i = subResults.begin(); i != subResults.end(); ++i) {
			if(i->uses(name, context)) return true;
		}
		return false;
	}
	if(qp != 0) {
		qp->staticTypingLite(context);

		if(name.name == 0)
			return qp->getStaticAnalysis().areContextFlagsUsed();
		return qp->getStaticAnalysis().isVariableUsed(name.uri, name.name);
	}
	if(ast != 0) {
		if(name.name == 0)
			return ast->getStaticAnalysis().areContextFlagsUsed();
		return ast->getStaticAnalysis().isVariableUsed(name.uri, name.name);
	}
	return false;
}

static bool usesContext(ASTNode *arg, const QueryPlanGenerator::QName &contextName)
{
	if(contextName.name == 0)
		return arg->getStaticAnalysis().areContextFlagsUsed();

	return arg->getStaticAnalysis().isVariableUsed(contextName.uri, contextName.name);
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseComparison(XQOperator *item, const ReverseResult &context, const QName &contextName)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	ASTNode *arg0 = item->getArgument(0);
	ASTNode *arg1 = item->getArgument(1);

	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	ImpliedSchemaNode::MVector *paths0 = &ud->paths;
	ImpliedSchemaNode::MVector *paths1 = &ud->paths2;

	XQCastAs *arg0Cast, *arg1Cast;
	ASTNode *arg0Nodes = findNodeProducer(arg0, (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg0Cast);
	ASTNode *arg1Nodes = findNodeProducer(arg1, (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg1Cast);

	if(arg1Nodes != 0 && !paths1->empty()) {
		if(arg0Nodes == 0 || paths0->empty() || (usesContext(arg1, contextName) && !usesContext(arg0, contextName))) {
			// Swap the arguments, and the operation
			swapASTNode(arg0, arg1);
			swapASTNode(arg0Nodes, arg1Nodes);
			swapCast(arg0Cast, arg1Cast);
			swapPaths(paths0, paths1);
		} else {
			// TBD Perform a value join if both arguments are nodes - jpcs
		}
	}

	if(arg0Nodes != 0 && !paths0->empty()) {
		PathsQP *pathsqp = new (mm) PathsQP(*paths0, mm);
		pathsqp->setLocationInfo(item);

		// Optimise the ASTNode values in the paths
		for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
			if((*it)->getASTNode() != 0) {
				(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
			}
		}

		ReverseResult newContext(reverseJoin(context, pathsqp, item), Join::SELF, QName());

		ReverseResult expr = reverse(arg0Nodes, newContext, contextName);

		if(expr.qp != 0) return expr;
	}

	MetaDataFunction *mdFunc = findMetaDataFunction(arg0, arg0Cast);
	if(mdFunc != 0) {
		paths0 = &ud->paths;
	} else {
		mdFunc = findMetaDataFunction(arg1, arg0Cast);
		paths0 = &ud->paths2;
	}

	if(mdFunc != 0 && !paths0->empty()) {
		PathsQP *pathsqp = new (mm) PathsQP(*paths0, mm);
		pathsqp->setLocationInfo(item);

		// Optimise the ASTNode values in the paths
		for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
			if((*it)->getASTNode() != 0) {
				(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
			}
		}

		ReverseResult newContext(reverseJoin(context, pathsqp, item), Join::DESCENDANT_OR_SELF, QName());

		ReverseResult expr;
		if(mdFunc->getArguments().size() > 1) {
			XQCastAs *cast;
			ASTNode *nodeArg = findNodeProducer(mdFunc->getArguments()[1], StaticType::NODE_TYPE, cast);
			if(nodeArg != 0 && cast == 0)
				expr = reverse(nodeArg, newContext, contextName);
		} else if(contextName.name == 0) {
				expr = newContext;
		}

		if(expr.qp != 0) return expr;
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

static inline bool noCastOrString(const XQCastAs *cast)
{
	return cast == 0 || (cast->getTypeIndex() == AnyAtomicType::STRING && cast->getIsPrimitive());
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseContains(XQFunction *item, const ReverseResult &context, const QName &contextName)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	VectorOfASTNodes &args = const_cast<VectorOfASTNodes&>(item->getArguments());

	// Make sure there is no collation argument
	if(args.size() == 2) {
		DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();

		XQCastAs *arg0Cast;
		ASTNode *arg0 = findNodeProducer(args[0], (StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE)), arg0Cast);

		if(arg0 != 0 && noCastOrString(arg0Cast) && !ud->paths.empty()) {
			PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
			pathsqp->setLocationInfo(item);

			// Optimise the ASTNode values in the paths
			for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
				if((*it)->getASTNode() != 0) {
					(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
				}
			}

			ReverseResult newContext(reverseJoin(context, pathsqp, item), Join::SELF, QName());

			ReverseResult expr = reverse(arg0, newContext, contextName);

			if(expr.qp != 0) return expr;
		}

		MetaDataFunction *mdFunc = findMetaDataFunction(args[0], arg0Cast);
		if(mdFunc != 0 && noCastOrString(arg0Cast) && !ud->paths.empty()) {
			PathsQP *pathsqp = new (mm) PathsQP(ud->paths, mm);
			pathsqp->setLocationInfo(item);

			// Optimise the ASTNode values in the paths
			for(PathsQP::Paths::const_iterator it = pathsqp->getPaths().begin(); it != pathsqp->getPaths().end(); ++it) {
				if((*it)->getASTNode() != 0) {
					(*it)->setASTNode(optimize(const_cast<ASTNode*>((*it)->getASTNode())));
				}
			}

			ReverseResult newContext(reverseJoin(context, pathsqp, item), Join::DESCENDANT_OR_SELF, QName());

			ReverseResult expr;
			if(mdFunc->getArguments().size() > 1) {
				XQCastAs *cast;
				ASTNode *nodeArg = findNodeProducer(mdFunc->getArguments()[1], StaticType::NODE_TYPE, cast);
				if(nodeArg != 0 && noCastOrString(cast))
					expr = reverse(nodeArg, newContext, contextName);
			} else if(contextName.name == 0) {
				expr = newContext;
			}

			if(expr.qp != 0) return expr;
		}
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

ForTuple *QueryPlanGenerator::reverseQuantifiedForTuple(ForTuple *item, ReverseResult &context, const QName &contextName)
{
	// We don't care if the expression is in document order, since it's fine to reorder it here
	if(item->getExpression()->getStaticAnalysis().getStaticType().containsType(StaticType::ANY_ATOMIC_TYPE) ||
		context.uses(QName(item->getVarURI(), item->getVarName()), xpc_)) {
		return item;
	}

	if(item->getParent()->getType() != TupleNode::FOR) {
		context = reverse(item->getExpression(), context, contextName);
		return 0;
	}

	ForTuple *found = (ForTuple*)item->getParent();

	context = reverse(item->getExpression(), context, QName(found->getVarURI(), found->getVarName()));
	return reverseQuantifiedForTuple(found, context, contextName);
}

void QueryPlanGenerator::ReverseResult::inverse(XPath2MemoryManager *mm, const LocationInfo *location)
{
	if(!subResults.empty()) {
		negate = !negate;
		for(vector<ReverseResult>::iterator i = subResults.begin(); i != subResults.end(); ++i)
			i->inverse(mm, location);
	} else if(ast != 0) {
		VectorOfASTNodes nargs = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
		nargs.push_back(ast);
		ast = new (mm) FunctionNot(nargs, mm);
		ast->setLocationInfo(location);
	} else if(qp != 0) {
		negate = !negate;
	}
}

QueryPlanGenerator::ReverseResult QueryPlanGenerator::reverseQuantified(XQQuantified *item, const ReverseResult &context, const QName &contextName)
{
	//
	// We use the equivalency:
	//
	// every $a in a() satisfies b() <equivalent to> not(some $a in a() satisfies not(b()))
	//

	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	DBXML_ASSERT(item->getParent()->getType() == TupleNode::FOR);
	ForTuple *found = (ForTuple*)item->getParent();

	if(found->getExpression()->getStaticAnalysis().getStaticType().containsType(StaticType::ANY_ATOMIC_TYPE)) {
		return ReverseResult(reverseJoin(context, item, item), contextName);
	}

	ReverseResult result = reverse(item->getExpression(), context, QName(found->getVarURI(), found->getVarName()));
	if(item->getQuantifierType() == XQQuantified::EVERY) result.inverse(mm, item);

	ForTuple *tuple = reverseQuantifiedForTuple(found, result, contextName);

	if(item->getQuantifierType() == XQQuantified::EVERY) {
		result.inverse(mm, item);

		// If the quantifier is "every", we generate QueryPlan objects as if it were a
		// "some" quantifier intersected with an "every" quantifier - this is because
		// "some" is always true when "every" is true, and the "some" construction is
		// often more easily optimised.
		if(result.qp != 0) {
			ReverseResult someResult = reverse(item->getExpression(), context, QName(found->getVarURI(), found->getVarName()));

			ForTuple *tuple2 = reverseQuantifiedForTuple(found, someResult, contextName);
			DBXML_ASSERT(tuple == tuple2);
			DBXML_ASSERT(someResult.qp != 0);

			result.joinType = Join::SELF;
			someResult.qp = reverseJoin(result, someResult.qp, item);

			result = someResult;
		}
	}

	if(tuple == 0) return result;

	item->setParent(tuple);

	if(!result.subResults.empty() && !result.negate) {
		Or *orOp = new (mm) Or(VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm)), mm);
		orOp->setLocationInfo(item);

		for(vector<ReverseResult>::const_iterator i = result.subResults.begin(); i != result.subResults.end(); ++i) {
			ASTNode *var = new (mm) XQVariable(tuple->getVarURI(), tuple->getVarName(), mm);
			var->setLocationInfo(tuple);

			StaticAnalysis &varSrc = const_cast<StaticAnalysis&>(var->getStaticAnalysis());
			varSrc.getStaticType() = tuple->getExpression()->getStaticAnalysis().getStaticType();
			varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
				StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
				StaticAnalysis::ONENODE | StaticAnalysis::SELF);
			varSrc.variableUsed(tuple->getVarURI(), tuple->getVarName());

			ASTNode *arg = new XQEffectiveBooleanValue(reverseJoin(*i, var, item), mm);
			arg->setLocationInfo(item);
			orOp->addArgument(arg);
		}

		item->setExpression(orOp);
	} else if(result.ast != 0 && !result.negate && result.name.uri == tuple->getVarURI() && result.name.name == tuple->getVarName()) {
		// Add an effective boolean value object
		result.ast = new (mm) XQEffectiveBooleanValue(result.ast, mm);
		result.ast->setLocationInfo(item);
		item->setExpression(result.ast);
	} else {
		ASTNode *var = new (mm) XQVariable(tuple->getVarURI(), tuple->getVarName(), mm);
		var->setLocationInfo(tuple);
		const_cast<StaticAnalysis&>(var->getStaticAnalysis()).copy(tuple->getVarSRC());

		StaticAnalysis &varSrc = const_cast<StaticAnalysis&>(var->getStaticAnalysis());
		varSrc.getStaticType() = tuple->getExpression()->getStaticAnalysis().getStaticType();
		varSrc.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
			StaticAnalysis::ONENODE | StaticAnalysis::SELF);
		varSrc.variableUsed(tuple->getVarURI(), tuple->getVarName());

		// Add an effective boolean value object
		ASTNode *expr = new (mm) XQEffectiveBooleanValue(reverseJoin(result, var, item), mm);
		expr->setLocationInfo(item);
		item->setExpression(expr);
	}

	return ReverseResult(reverseJoin(context, item, item), contextName);
}

