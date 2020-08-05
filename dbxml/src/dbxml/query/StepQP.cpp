//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "StepQP.hpp"
#include "QueryExecutionContext.hpp"
#include "StructuralJoinQP.hpp"
#include "ValueFilterQP.hpp"
#include "SequentialScanQP.hpp"
#include "ExceptQP.hpp"
#include "BufferQP.hpp"
#include "ContextNodeQP.hpp"
#include "VariableQP.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/DbXmlFactoryImpl.hpp"
#include "../nodeStore/NsDoc.hpp"
#include "../nodeStore/NsDom.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQNav.hpp>

#include <sstream>
#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

StepQP::StepQP(QueryPlan *arg, Join::Type join, DbXmlNodeTest *nodeTest, ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(STEP, flags, mm),
	  paths_(XQillaAllocator<ImpliedSchemaNode*>(mm)),
	  container_(cont),
	  arg_(arg),
	  joinType_(join),
	  nodeTest_(nodeTest),
	  needsSort_(true),
	  cost_(),
	  costSet_(false)
{
	if(container_ == 0) container_ = findContainer(arg);
}

void StepQP::addPaths(const ImpliedSchemaNode::MVector &paths)
{
	paths_.insert(paths_.end(), paths.begin(), paths.end());
}

void StepQP::addPaths(const ImpliedSchemaNode::Vector &paths)
{
	paths_.insert(paths_.end(), paths.begin(), paths.end());
}

NodeIterator *StepQP::createNodeIterator(DynamicContext *context) const
{
	if(needsSort_) {
		return new SortingStepIterator(arg_->createNodeIterator(context), this);
	}

	switch(joinType_) {
	case Join::ATTRIBUTE_OR_CHILD:
	case Join::CHILD: {
		if(nodeTest_ != 0 && nodeTest_->getItemType() == 0 && (nodeTest_->getHasChildren() ||
			   (!nodeTest_->getTypeWildcard() && nodeTest_->getNodeType() == Node::element_string))) {
			return new ElementChildAxisIterator(arg_->createNodeIterator(context), nodeTest_, this);
		}
		break;
	}
	default: break;
	}

	return new StepIterator(arg_->createNodeIterator(context), this);
}

QueryPlan *StepQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	StepQP *result = new (mm) StepQP(arg_->copy(mm), joinType_, nodeTest_, container_, flags_, mm);
	result->addPaths(paths_);
	result->needsSort_ = needsSort_;
	result->cost_ = cost_;
	result->costSet_ = costSet_;
	result->_src.copy(_src);
	result->setLocationInfo(this);
	return result;
}

void StepQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *StepQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	arg_ = arg_->staticTyping(context, styper);
	staticTypingImpl(context);
	return this;
}

void StepQP::staticTypingLite(StaticContext *context)
{
	arg_->staticTypingLite(context);
	staticTypingImpl(context);
}

void StepQP::staticTypingImpl(StaticContext *context)
{
	costSet_ = false;
	_src.clear();

	_src.add(arg_->getStaticAnalysis());

	_src.setProperties(XQNav::combineProperties(arg_->getStaticAnalysis().getProperties(),
				   Join::getJoinTypeProperties(joinType_)));

	if((_src.getProperties() & StaticAnalysis::DOCORDER) == 0) {
		needsSort_ = true;
		_src.setProperties(_src.getProperties() | StaticAnalysis::DOCORDER);
	} else {
		needsSort_ = false;
	}

	if(nodeTest_ != 0) {
		bool isExact;
		nodeTest_->getStaticType(_src.getStaticType(), context, isExact, this);
	} else {
		_src.getStaticType() = StaticType::NODE_TYPE;
	}
	_src.getStaticType().multiply(0, StaticType::UNLIMITED);

	switch(joinType_) {
	case Join::SELF:
		_src.getStaticType().typeNodeIntersect(arg_->getStaticAnalysis().getStaticType());
		break;
	case Join::ATTRIBUTE:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED));
		break;
	case Join::NAMESPACE:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::NAMESPACE_TYPE, 0, StaticType::UNLIMITED));
		break;
	case Join::ATTRIBUTE_OR_CHILD:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
							       StaticType::COMMENT_TYPE | StaticType::ATTRIBUTE_TYPE), 0, StaticType::UNLIMITED));
		break;
	case Join::CHILD:
	case Join::DESCENDANT:
	case Join::FOLLOWING:
	case Join::FOLLOWING_SIBLING:
	case Join::PRECEDING:
	case Join::PRECEDING_SIBLING:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::StaticTypeFlags(StaticType::ELEMENT_TYPE | StaticType::TEXT_TYPE | StaticType::PI_TYPE |
							       StaticType::COMMENT_TYPE), 0, StaticType::UNLIMITED));
		break;
	case Join::ANCESTOR:
	case Join::PARENT:
	case Join::PARENT_C:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::StaticTypeFlags(StaticType::DOCUMENT_TYPE | StaticType::ELEMENT_TYPE), 0, StaticType::UNLIMITED));
		break;
	case Join::PARENT_A:
		_src.getStaticType().typeNodeIntersect(StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED));
		break;
	case Join::DESCENDANT_OR_SELF:
	case Join::ANCESTOR_OR_SELF:
	case Join::NONE:
		break;
	}
}

QueryPlan *StepQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	costSet_ = false;
	return this;
}

static NameID getNameIDFromNodeTest(ContainerBase *container, const DbXmlNodeTest *nodeTest, OperationContext &oc)
{
	NameID id;
	if(container != 0 && nodeTest != 0 && nodeTest->getItemType() == 0 &&
		!nodeTest->getNamespaceWildcard() && !nodeTest->getNameWildcard()) {
		Name name((const char*)nodeTest->getNodeUri8(), (const char*)nodeTest->getNodeName8());
		if (!container->lookupID(oc, name, id))
			id = NS_NOURI;
	}

	return id;
}

void StepQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		StepQP *result = new (mm) StepQP(*it, joinType_, nodeTest_, container_, flags_, mm);
		result->addPaths(paths_);
		result->needsSort_ = needsSort_;
		result->_src.copy(_src);
		result->setLocationInfo(this);

		combinations.push_back(result);
	}
}

static bool nodeTestSubsetOf(const QueryPlan *qp, const DbXmlNodeTest *nodeTest)
{
	switch(qp->getType()) {
	case QueryPlan::STEP:
		return DbXmlNodeTest::isSubsetOf(((StepQP*)qp)->getNodeTest(), nodeTest);
	case QueryPlan::VARIABLE:
		return DbXmlNodeTest::isSubsetOf(((VariableQP*)qp)->getNodeTest(), nodeTest);
	case QueryPlan::CONTEXT_NODE:
		return DbXmlNodeTest::isSubsetOf(((ContextNodeQP*)qp)->getNodeTest(), nodeTest);
	case QueryPlan::PRESENCE:
	case QueryPlan::VALUE:
	case QueryPlan::RANGE:
		return DbXmlNodeTest::isSubsetOf(StepQP::findNodeTest(((PresenceQP*)qp)->getImpliedSchemaNode()), nodeTest);
	case QueryPlan::SEQUENTIAL_SCAN:
		return DbXmlNodeTest::isSubsetOf(StepQP::findNodeTest(((SequentialScanQP*)qp)->getImpliedSchemaNode()), nodeTest);

	case QueryPlan::UNION: {
		const OperationQP::Vector &args = ((OperationQP*)qp)->getArgs();
		for(OperationQP::Vector::const_iterator i = args.begin(); i != args.end(); ++i) {
			if(!nodeTestSubsetOf(*i, nodeTest)) return false;
		}
		return true;
	}
	case QueryPlan::INTERSECT: {
		const OperationQP::Vector &args = ((OperationQP*)qp)->getArgs();
		for(OperationQP::Vector::const_iterator i = args.begin(); i != args.end(); ++i) {
			if(nodeTestSubsetOf(*i, nodeTest)) return true;
		}
		return true;
	}

	case QueryPlan::EXCEPT:
		return nodeTestSubsetOf(((ExceptQP*)qp)->getLeftArg(), nodeTest);

	case QueryPlan::VALUE_FILTER:
	case QueryPlan::PREDICATE_FILTER:
	case QueryPlan::NODE_PREDICATE_FILTER:
	case QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER:
	case QueryPlan::NUMERIC_PREDICATE_FILTER:
	case QueryPlan::LEVEL_FILTER:
	case QueryPlan::DEBUG_HOOK:
	case QueryPlan::DOC_EXISTS:
		return nodeTestSubsetOf(((FilterQP*)qp)->getArg(), nodeTest);

	case QueryPlan::DESCENDANT:
	case QueryPlan::DESCENDANT_OR_SELF:
	case QueryPlan::ANCESTOR:
	case QueryPlan::ANCESTOR_OR_SELF:
	case QueryPlan::ATTRIBUTE:
	case QueryPlan::CHILD:
	case QueryPlan::ATTRIBUTE_OR_CHILD:
	case QueryPlan::PARENT:
	case QueryPlan::PARENT_OF_ATTRIBUTE:
	case QueryPlan::PARENT_OF_CHILD:
		return nodeTestSubsetOf(((StructuralJoinQP*)qp)->getRightArg(), nodeTest);
	default:
		break;
	}

	return false;
}

void StepQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	if(joinType_ == Join::SELF && nodeTestSubsetOf(arg_, nodeTest_)) {
		alternatives.push_back(arg_);
		return;
	}

	alternatives.push_back(this);
}

static StructuralStats calculateStructuralStats(ContainerBase *container, const QueryPlan *qp,
	OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor)
{
	switch(qp->getType()) {
	case QueryPlan::UNION:
	case QueryPlan::INTERSECT: {
		StructuralStats stats;
		OperationQP::Vector &args = const_cast<OperationQP::Vector&>(((OperationQP*)qp)->getArgs());
		for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
			stats.add(calculateStructuralStats(container, *i, oc, cache, nameID, ancestor));
		}
		return stats;
	}
	case QueryPlan::EXCEPT:
		return calculateStructuralStats(container, ((ExceptQP*)qp)->getLeftArg(), oc, cache, nameID, ancestor);
	case QueryPlan::VALUE_FILTER:
	case QueryPlan::PREDICATE_FILTER:
	case QueryPlan::NODE_PREDICATE_FILTER:
	case QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER:
	case QueryPlan::NUMERIC_PREDICATE_FILTER:
	case QueryPlan::LEVEL_FILTER:
	case QueryPlan::DEBUG_HOOK:
	case QueryPlan::DOC_EXISTS:
		return calculateStructuralStats(container, ((FilterQP*)qp)->getArg(), oc, cache, nameID, ancestor);
	case QueryPlan::STEP:
		return ((StepQP*)qp)->getStructuralStats(oc, cache, nameID, ancestor);
	case QueryPlan::DESCENDANT:
	case QueryPlan::DESCENDANT_OR_SELF:
	case QueryPlan::ANCESTOR:
	case QueryPlan::ANCESTOR_OR_SELF:
	case QueryPlan::ATTRIBUTE:
	case QueryPlan::CHILD:
	case QueryPlan::ATTRIBUTE_OR_CHILD:
	case QueryPlan::PARENT:
	case QueryPlan::PARENT_OF_ATTRIBUTE:
	case QueryPlan::PARENT_OF_CHILD:
		return calculateStructuralStats(container, ((StructuralJoinQP*)qp)->getRightArg(), oc, cache, nameID, ancestor);
	case QueryPlan::PRESENCE:
	case QueryPlan::VALUE:
	case QueryPlan::RANGE:
		return ((PresenceQP*)qp)->getStructuralStats(oc, cache, nameID, ancestor);
	case QueryPlan::SEQUENTIAL_SCAN:
		return ((SequentialScanQP*)qp)->getStructuralStats(oc, cache, nameID, ancestor);
	case QueryPlan::EMPTY:
		return StructuralStats();
	case QueryPlan::BUFFER:
		return calculateStructuralStats(container, ((BufferQP*)qp)->getArg(), oc, cache, nameID, ancestor);
	case QueryPlan::BUFFER_REF:
		return calculateStructuralStats(container, ((BufferReferenceQP*)qp)->getBuffer()->getParent(), oc, cache, nameID, ancestor);
	case QueryPlan::CONTEXT_NODE: {
		ContextNodeQP *ci = (ContextNodeQP*)qp;
		if(container == 0) container = ci->getContainerBase();
		return StepQP::getStructuralStats(container, ci->getNodeTest(), oc, cache, nameID, ancestor);
	}
	case QueryPlan::VARIABLE: {
		VariableQP *var = (VariableQP*)qp;
		if(container == 0) container = var->getContainerBase();
		return StepQP::getStructuralStats(container, var->getNodeTest(), oc, cache, nameID, ancestor);
	}

	case QueryPlan::COLLECTION:
	case QueryPlan::DOC:
	case QueryPlan::AST_TO_QP:
	case QueryPlan::DECISION_POINT:
	case QueryPlan::DECISION_POINT_END:
	case QueryPlan::PATHS: break;
	}

	return StepQP::getStructuralStats(container, 0, oc, cache, nameID, ancestor);
}

Cost StepQP::getStepCost(ContainerBase *container, const QueryPlan *arg, Join::Type join, OperationContext &oc,
	StructuralStatsCache &cache, const NameID &descendantID)
{
	switch(join) {
	case Join::NAMESPACE:
	case Join::ATTRIBUTE:
	case Join::SELF:
	case Join::PARENT_A: {
		// TBD save attribute counts? - jpcs
		return Cost(1, 1);
	}
	case Join::ATTRIBUTE_OR_CHILD: {
		Cost cost = getStepCost(container, arg, Join::ATTRIBUTE, oc, cache, descendantID);
		cost.unionOp(getStepCost(container, arg, Join::CHILD, oc, cache, descendantID));
		return cost;
	}

	case Join::CHILD: {
		StructuralStats stats = calculateStructuralStats(container, arg, oc, cache, descendantID, /*ancestor*/false);
		return Cost((double)stats.sumNumberOfChildren_ / (double)stats.numberOfNodes_,
			(double)stats.sumChildSize_ / (double)stats.numberOfNodes_);
	}
	case Join::FOLLOWING_SIBLING:
	case Join::PRECEDING_SIBLING: {
		Cost cost = getStepCost(container, arg, Join::CHILD, oc, cache, descendantID);
		cost.keys /= 2;
		cost.pagesOverhead = cost.pagesOverhead / 2;
		return cost;
	}

	case Join::DESCENDANT: {
		StructuralStats stats = calculateStructuralStats(container, arg, oc, cache, descendantID, /*ancestor*/false);
		return Cost((double)stats.sumNumberOfDescendants_ / (double)stats.numberOfNodes_,
			(double)stats.sumDescendantSize_ / (double)stats.numberOfNodes_);
	}
	case Join::DESCENDANT_OR_SELF: {
		Cost cost = getStepCost(container, arg, Join::DESCENDANT, oc, cache, descendantID);
		cost.unionOp(getStepCost(container, arg, Join::SELF, oc, cache, descendantID));
		return cost;
	}
	case Join::FOLLOWING:
	case Join::PRECEDING: {
		// An arbitrary way to guess
		Cost cost = getStepCost(container, arg, Join::DESCENDANT, oc, cache, descendantID);
		cost.keys *= 2;
		cost.pagesOverhead *= 2;
		return cost;
	}
	case Join::ANCESTOR: {
		// TBD average node level - jpcs
		StructuralStats stats = calculateStructuralStats(container, arg, oc, cache, descendantID, /*ancestor*/true);
		return Cost((double)stats.numberOfNodes_ / (double)stats.sumNumberOfDescendants_,
			(double)stats.sumSize_ * 3 / (double)stats.numberOfNodes_);
	}
	case Join::ANCESTOR_OR_SELF:  {
		Cost cost = getStepCost(container, arg, Join::ANCESTOR, oc, cache, descendantID);
		cost.unionOp(getStepCost(container, arg, Join::SELF, oc, cache, descendantID));
		return cost;
	}

	case Join::PARENT:
	case Join::PARENT_C: {
		StructuralStats stats = calculateStructuralStats(container, arg, oc, cache, descendantID, /*ancestor*/true);
		return Cost((double)stats.numberOfNodes_ / (double)stats.sumNumberOfChildren_,
			(double)stats.sumSize_ / (double)stats.numberOfNodes_);
	}

	case Join::NONE: break;
	}
	return Cost();
}

Cost StepQP::cost(ContainerBase *container, const QueryPlan *arg, const Cost &argCost, Join::Type join,
	const DbXmlNodeTest *nodeTest, OperationContext &oc, StructuralStatsCache &cache)
{
	if(container == 0) container = findContainer(arg);

	NameID id = getNameIDFromNodeTest(container, nodeTest, oc);
	Cost stepCost = getStepCost(container, arg, join, oc, cache, id);

	Cost cost;
	cost.keys = stepCost.keys * argCost.keys;
	cost.pagesOverhead = (stepCost.totalPages() * argCost.keys) + argCost.totalPages();

	if(container != 0 && id != 0) {
		StructuralStats stats = cache.get(container, oc, id, 0);
		if(stats.numberOfNodes_ < cost.keys) cost.keys = (double) stats.numberOfNodes_;
	}

	return cost;
}

Cost StepQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(!costSet_) {
		cost_ = cost(container_, arg_, arg_->cost(context, qec), joinType_, nodeTest_, context,
			GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache());

		if(needsSort_) {
			// Add an amount to cover the sorting
			cost_.pagesOverhead += cost_.keys;
		}

		costSet_ = true;
	}

	return cost_;
}

StructuralStats StepQP::getStructuralStats(ContainerBase *container, const DbXmlNodeTest *nodeTest, OperationContext &oc,
	StructuralStatsCache &cache, const NameID &nameID, bool ancestor)
{
	if(container == 0) return StructuralStats(nodeTest != 0 && nodeTest->getItemType() == 0 &&
		!nodeTest->getNamespaceWildcard() && !nodeTest->getNameWildcard(), nameID != 0);

	if(ancestor)
		return cache.get(container, oc, nameID, getNameIDFromNodeTest(container, nodeTest, oc));
	return cache.get(container, oc, getNameIDFromNodeTest(container, nodeTest, oc), nameID);
}

StructuralStats StepQP::getStructuralStats(OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor) const
{
	return getStructuralStats(container_, nodeTest_, oc, cache, nameID, ancestor);
}

void StepQP::findQueryPlanRoots(QPRSet &qprset) const
{
	for(ImpliedSchemaNode::MVector::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		qprset.insert(((ImpliedSchemaNode*)(*it)->getRoot())->getQueryPlanRoot());
	}
	arg_->findQueryPlanRoots(qprset);
}

bool StepQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == STEP) {
		StepQP *st = (StepQP*)o;
		return st->joinType_ == joinType_ && DbXmlNodeTest::isSubsetOf(nodeTest_, st->nodeTest_) &&
			arg_->isSubsetOf(st->arg_);
	}
	return false;
}

string StepQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<StepQP";
	s << " axis=\"" << DbXmlPrintAST::getJoinTypeName(joinType_) << "\"";
  
	SequenceType::ItemType *type = 0;
	if(nodeTest_ != 0) {
		type = nodeTest_->getItemType();
		if(type == 0) {
			s << DbXmlPrintAST::printNodeTestAttrs(nodeTest_);
		}
	}
	s << ">" << endl;
	if(type != 0) {
		s << in << "  <ItemType";
		XMLBuffer buf;
		type->toBuffer(buf);
		s << " type=\"" << XMLChToUTF8(buf.getRawBuffer()).str() << "\"";
		s << "/>" << endl;
	}
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << in << "</StepQP>" << endl;

	return s.str();
}

string StepQP::toString(bool brief) const
{
	return toString(joinType_, nodeTest_, arg_, brief);
}

string StepQP::toString(Join::Type joinType, const DbXmlNodeTest *nodeTest, const QueryPlan *arg, bool brief)
{
	ostringstream s;

	s << "step(";
	s << DbXmlPrintAST::getJoinTypeName(joinType) << "::";

	if(nodeTest != 0 && nodeTest->getItemType() == 0) {
		if(!nodeTest->getTypeWildcard()) {
			s << XMLChToUTF8(nodeTest->getNodeType()).str() << "(";
		}

		if(nodeTest->getNamespaceWildcard()) {
			if(!nodeTest->getNameWildcard()) {
				s << "*:";
			}
		} else {
			if(nodeTest->getNodePrefix() != 0) {
				s << XMLChToUTF8(nodeTest->getNodePrefix()).str() << ":";
			}
			if(nodeTest->getNodeUri() != 0) {
				s << "{" << XMLChToUTF8(nodeTest->getNodeUri()).str() << "}";
			}
		}
		if(nodeTest->getNameWildcard()) {
			s << "*";
		} else if(nodeTest->getNodeName() != 0) {
			s << XMLChToUTF8(nodeTest->getNodeName()).str();
		}

		if(!nodeTest->getTypeWildcard()) {
			s << ")";
		}

		s << ",";
	} else {
		s << "*,";
	}

	s << arg->toString(brief);

	s << ")";

	return s.str();
}

ContainerBase *StepQP::findContainer(const QueryPlan *qp)
{
	switch(qp->getType()) {
	case QueryPlan::PRESENCE:
	case QueryPlan::VALUE:
	case QueryPlan::RANGE:
		return ((PresenceQP*)qp)->getContainerBase();
	case QueryPlan::SEQUENTIAL_SCAN:
		return ((SequentialScanQP*)qp)->getContainerBase();
	case QueryPlan::EXCEPT:
		return findContainer(((ExceptQP*)qp)->getLeftArg());
	case QueryPlan::VALUE_FILTER:
	case QueryPlan::PREDICATE_FILTER:
	case QueryPlan::NODE_PREDICATE_FILTER:
	case QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER:
	case QueryPlan::NUMERIC_PREDICATE_FILTER:
	case QueryPlan::LEVEL_FILTER:
	case QueryPlan::DEBUG_HOOK:
	case QueryPlan::DOC_EXISTS:
		return findContainer(((FilterQP*)qp)->getArg());
	case QueryPlan::STEP:
		if(((StepQP*)qp)->getContainerBase()) return ((StepQP*)qp)->getContainerBase();
		return findContainer(((StepQP*)qp)->getArg());
	case QueryPlan::CONTEXT_NODE:
		return ((ContextNodeQP*)qp)->getContainerBase();
	case QueryPlan::VARIABLE:
		return ((VariableQP*)qp)->getContainerBase();
	case QueryPlan::DESCENDANT:
	case QueryPlan::DESCENDANT_OR_SELF:
	case QueryPlan::ANCESTOR:
	case QueryPlan::ANCESTOR_OR_SELF:
	case QueryPlan::ATTRIBUTE:
	case QueryPlan::CHILD:
	case QueryPlan::ATTRIBUTE_OR_CHILD:
	case QueryPlan::PARENT:
	case QueryPlan::PARENT_OF_ATTRIBUTE:
	case QueryPlan::PARENT_OF_CHILD:
		return findContainer(((StructuralJoinQP*)qp)->getRightArg());
	default: break;
	}
	return 0;
}

class FindNodeTest : public FilterSkipper
{
public:
	DbXmlNodeTest *run(const QueryPlan *qp)
	{
		nodeTest_ = 0;
		skip(const_cast<QueryPlan*>(qp));
		return nodeTest_;
	}

	virtual QueryPlan *doWork(QueryPlan *qp)
	{
		switch(qp->getType()) {
		case QueryPlan::SEQUENTIAL_SCAN:
			nodeTest_ = StepQP::findNodeTest(((SequentialScanQP*)qp)->getImpliedSchemaNode());
			break;
		case QueryPlan::RANGE:
		case QueryPlan::PRESENCE:
		case QueryPlan::VALUE:
			nodeTest_ = StepQP::findNodeTest(((PresenceQP*)qp)->getImpliedSchemaNode());
			break;
		case QueryPlan::STEP:
			nodeTest_ = ((StepQP*)qp)->getNodeTest();
			break;
		case QueryPlan::CONTEXT_NODE:
			nodeTest_ = ((ContextNodeQP*)qp)->getNodeTest();
			break;
		case QueryPlan::VARIABLE:
			nodeTest_ = ((VariableQP*)qp)->getNodeTest();
			break;
		default:
			break;
		}

		return qp;
	}

	DbXmlNodeTest *nodeTest_;
};

DbXmlNodeTest *StepQP::findNodeTest(const QueryPlan *lookup)
{
	return FindNodeTest().run(lookup);
}

DbXmlNodeTest *StepQP::findNodeTest(const ImpliedSchemaNode *isn)
{
	while(isn != 0) {
		switch(isn->getType()) {
		case ImpliedSchemaNode::ROOT:
		case ImpliedSchemaNode::DESCENDANT:
		case ImpliedSchemaNode::ATTRIBUTE:
		case ImpliedSchemaNode::CHILD:
		case ImpliedSchemaNode::METADATA:
			return (DbXmlNodeTest*)isn->getNodeTest();
		default:
			isn = (ImpliedSchemaNode*)isn->getParent();
			break;
		}
	}
	return 0;
}

QueryPlan *StepQP::createStep(QueryPlan *arg, Join::Type joinType, QueryPlan *lookup, u_int32_t flags,
	const LocationInfo *location, XPath2MemoryManager *mm)
{
	ImpliedSchemaNode *isn = 0, *isn2 = 0;
	ContainerBase *cont = 0;

	switch(lookup->getType()) {
	case SEQUENTIAL_SCAN:
		isn = ((SequentialScanQP*)lookup)->getImpliedSchemaNode();
		cont = ((SequentialScanQP*)lookup)->getContainerBase();
		break;
	case RANGE:
		isn2 = ((RangeQP*)lookup)->getImpliedSchemaNode2();
		// Fall through
	case PRESENCE:
	case VALUE:
		isn = ((PresenceQP*)lookup)->getImpliedSchemaNode();
		cont = ((PresenceQP*)lookup)->getContainerBase();
		break;
	default:
		return 0;
	}

	return createStep(arg, joinType, isn, isn2, cont, flags, location, mm);
}

QueryPlan *StepQP::createStep(QueryPlan *arg, Join::Type joinType, ImpliedSchemaNode *isn, ImpliedSchemaNode *isn2,
	ContainerBase *cont, u_int32_t flags, const LocationInfo *location, XPath2MemoryManager *mm)
{
	QueryPlan *result = 0;

	switch((ImpliedSchemaNode::Type)isn->getType()) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::METADATA:
		result = new (mm) StepQP(arg, joinType, (DbXmlNodeTest*)isn->getNodeTest(), cont, flags, mm);
		break;
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::NOT_EQUALS:
	case ImpliedSchemaNode::PREFIX:
		result = new (mm) ValueFilterQP(createStep(arg, joinType, (ImpliedSchemaNode*)isn->getParent(), 0, cont, flags, location, mm), isn, 0, mm);
		break;
	case ImpliedSchemaNode::CAST:
		// Skip the cast
		result = createStep(arg, joinType, (ImpliedSchemaNode*)isn->getParent(), 0, cont, flags, location, mm);
		break;
	case ImpliedSchemaNode::SUFFIX:
	case ImpliedSchemaNode::SUBSTRING:
	case ImpliedSchemaNode::SUBSTRING_CD:
		// A substring value filter is always added, so we don't
		// need another one.
		result = createStep(arg, joinType, (ImpliedSchemaNode*)isn->getParent(), 0, cont, flags, location, mm);
		break;
	case ImpliedSchemaNode::DESCENDANT_ATTR:
		DBXML_ASSERT(false);
		break;
	}

	result->setLocationInfo(location);

	if(isn2 != 0) {
		result = new (mm) ValueFilterQP(result, isn2, 0, mm);
		result->setLocationInfo(location);
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

StepIterator::StepIterator(NodeIterator *parent, const StepQP *qp)
	: DbXmlNodeIterator(qp),
	  parent_(parent),
	  qp_(qp),
	  result_(0)
{
}

StepIterator::~StepIterator()
{
	delete parent_;
}

bool StepIterator::next(DynamicContext *context)
{
	while((node_ = (DbXmlNodeImpl*)result_->next(context).get()).isNull()) {
		if(!parent_->next(context)) return false;

		result_ = parent_->asDbXmlNode(context)->getAxisResult((XQStep::Axis)qp_->getJoinType(),
			qp_->getNodeTest(), context, location_);
	}
	return true;
}

bool StepIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	node_ = (DbXmlNodeImpl*)result_->next(context).get();

	if(node_.notNull()) {
		// This is only safe because the result_ iterator only
		// ever returns nodes from the same document
		if(NodeInfo::isSameDocument(node_, container, did) >= 0) {
			return true;
		}
	}

	if(!parent_->seek(container, did, *NsNid::getRootNid(), context)) return false;
	result_ = parent_->asDbXmlNode(context)->getAxisResult((XQStep::Axis)qp_->getJoinType(),
		qp_->getNodeTest(), context, location_);

	return next(context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SortingStepIterator::SortingStepIterator(NodeIterator *parent, const StepQP *qp)
	: DbXmlNodeIterator(qp),
	  parent_(parent),
	  toDo_(true),
	  qp_(qp),
	  sorted_(0)
{
	DBXML_ASSERT(parent_ != 0);
}

SortingStepIterator::~SortingStepIterator()
{
	delete parent_;
}

bool SortingStepIterator::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;
		if(!parent_->next(context)) return false;
	}

	while((node_ = (DbXmlNodeImpl*)sorted_->next(context).get()).isNull()) {
		if(parent_ == 0) return false;

		// TBD Replace this with a temporary BTree - jpcs
		Sequence seq(context->getMemoryManager());
		while(true) {
			node_ = parent_->asDbXmlNode(context);

			Result result = node_->getAxisResult((XQStep::Axis)qp_->getJoinType(),
				qp_->getNodeTest(), context, location_);
			Item::Ptr item;
			while((item = result->next(context)).notNull()) {
				seq.addItem(item);
			}

			if(!parent_->next(context)) {
				delete parent_;
				parent_ = 0;
				break;
			}
			if(NodeInfo::isSameDocument(node_, parent_) != 0) break;
		}
		
		seq.sortIntoDocumentOrder(context);
		sorted_ = seq;
	}
	return true;
}

bool SortingStepIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;
		if(!parent_->seek(container, did, *NsNid::getRootNid(), context)) return false;
	} else {
		node_ = (DbXmlNodeImpl*)sorted_->next(context).get();
		if(node_.notNull()) {
			// This is only safe because the result_ iterator only
			// ever returns nodes from the same document
			if(NodeInfo::isSameDocument(node_, container, did) >= 0) {
				return true;
			}
		}
		sorted_ = 0;

		if(parent_ == 0) return false;

		if(NodeInfo::isSameDocument(parent_, container, did) < 0) {
			if(!parent_->seek(container, did, *NsNid::getRootNid(), context)) return false;
		}
	}

	return next(context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ElementChildAxisIterator::ElementChildAxisIterator(NodeIterator *parent, const DbXmlNodeTest *nodeTest, const LocationInfo *location)
	: NsNodeIterator(0, location),
	  parent_(parent),
	  nodeTest_(nodeTest),
	  uriIndex_(NS_NOURI),
	  nodeObj_(0),
	  cursor_(0)
{
	nextNid_.clear();
}

ElementChildAxisIterator::~ElementChildAxisIterator()
{
	if (cursor_)
		delete cursor_;
	nextNid_.freeNid();
	delete parent_;
}

bool ElementChildAxisIterator::next(DynamicContext *context)
{
	while(nextChild(context)) {
		if(rawNode_.hasNext()) {
			// use last descendant to find "next" element.  It's
			// the one just after the last descendant
			nextNid_.copyNid(rawNode_.getLastDescendantID().getBytes(), rawNode_.getLastDescendantID().getLen());
		} else {
			nextNid_.freeNid();
		}

		if((nodeTest_->getNamespaceWildcard() || rawNode_.getURIIndex() == uriIndex_) &&
			(nodeTest_->getNameWildcard() || NsUtil::nsStringEqual(rawNode_.getNodeName(), nodeTest_->getNodeName8()))) {
			// The node passes the name test
			return true;
		}
		context->testInterrupt();
	}

	return false;
}

bool ElementChildAxisIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	// TBD Could be better - jpcs

	if(nodeObj_.notNull() &&
	   (container_->getContainerID() < container ||
	    (container_->getContainerID() == container &&
	     (((Document&)doc_).getID() < did ||
	      (((Document&)doc_).getID() == did &&
	       nid.compareNids(nodeObj_->getLastDescendantID()) > 0))))) {
		// Force a move to the next node from parent_
		doc_ = (Document*)0;
		delete cursor_;
		cursor_ = 0;
	}

	return next(context);
}

NodeInfo::Type ElementChildAxisIterator::getType() const
{
	return ELEMENT;
}

int32_t ElementChildAxisIterator::getNodeURIIndex()
{
	DBXML_ASSERT(false);
	return -1;
}

const xmlbyte_t *ElementChildAxisIterator::getNodeName()
{
	DBXML_ASSERT(false);
	return 0;
}

u_int32_t ElementChildAxisIterator::getIndex() const
{
	DBXML_ASSERT(false);
	return (u_int32_t)-1;
}

bool ElementChildAxisIterator::isLeadingText() const
{
	DBXML_ASSERT(false);
	return false;
}

DbXmlNodeImpl::Ptr ElementChildAxisIterator::asDbXmlNode(DynamicContext *context)
{
	//XmlDocument doc = *nodeObj_->getXmlDocument();
	DocID did = rawNode_.getDocID();
	return ElementSSIterator::getDbXmlNodeImpl(doc_, did, key_, data_, context);
//	NsDomElement *element = ElementSSIterator::getNsDomElement(doc_, did, key_, data_, context);
//	return (DbXmlNodeImpl*)((DbXmlFactoryImpl*)context->getItemFactory())->createNode(element, doc_, context).get();
}

#define NO_NEXT_ERROR(err, docid, nodeid) \
		if((err) != 0) { \
			ostringstream oss; \
			oss << "Could not fetch next DOM element for doc id: "; \
			oss << (docid).asString(); \
			oss << ", nid: "; \
			(nodeid).displayNid(oss); \
			throw XmlException(XmlException::INTERNAL_ERROR, oss.str()); \
		}


bool ElementChildAxisIterator::nextChild(DynamicContext *context)
{
	if(!doc_.isNull() && rawNode_.hasNext()) {
		const DocID &did = ((Document&)doc_).getID();
		if(rawNode_.hasElemChild()) {
			// Marshal a key that comes directly after the given node ID
			NsFormat::marshalNextNodeKey(did, &nextNid_, key_);

			int err = cursor_->get(key_, data_, DB_SET_RANGE);
			NO_NEXT_ERROR(err, did, nextNid_);
		} else {
			int err = cursor_->get(key_, data_, DB_NEXT);
			NO_NEXT_ERROR(err, did, nextNid_);
		}

		rawNode_.setNode(key_, data_);
		return true;
	}

	while(parent_->next(context)) {
		// Check to see if parent_ has any element children
		// TBD GMF -- maybe add "hasChildElements()" to NodeInfo?
		if(parent_->getType() == DOCUMENT ||
		   parent_->getNodeID().compareNids(parent_->getLastDescendantID()) != 0) {
			// Hold a reference to the last nodeObj_, because cursor_ may still refer to a database owned by it
			DbXmlNodeImpl::Ptr oldNode = nodeObj_;

			nodeObj_ = parent_->asDbXmlNode(context);
			// get hold of the DbWrapper for this document
			NsDomNode *nsNode = nodeObj_->getNsDomNode().get();
			if (!nsNode) // no-content document
				continue;
			NsDoc *nsDoc = nsNode->getNsDoc();
			const XmlDocument *xdoc = nodeObj_->getXmlDocument();
			DBXML_ASSERT(xdoc);
			if(cursor_ == 0 || doc_ != *xdoc) {
				// changing documents
				if (cursor_)
					delete cursor_;
				doc_ = *xdoc; // may release resources (should)
				container_ = nodeObj_->getContainer();
				rawNode_.setContainerID(container_->getContainerID());
				// create a new cursor
				cursor_ = new Cursor(*nsDoc->getDocDb(),
						     nsDoc->getTxn(),
						     CURSOR_READ, 0);
				if(cursor_->error() != 0)
					throw XmlException(cursor_->error(), __FILE__, __LINE__);
				// Re-lookup the URI index - it could be different for different containers
				if(!nodeTest_->getNamespaceWildcard() && nodeTest_->getNodeUri8() != 0) {
					NameID uriId;
					OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
					if(container_->lookupID(oc, (const char*)nodeTest_->getNodeUri8(),
						   NsUtil::nsStringLen((const unsigned char*)nodeTest_->getNodeUri8()), uriId))
						uriIndex_ = uriId.raw();
					else uriIndex_ = 0;
				}
			}

			// Marshal a key that comes directly after the given node ID
			NsFormat::marshalNextNodeKey(((Document&)doc_).getID(),
						     nodeObj_->getNodeID(), key_);

			int err = cursor_->get(key_, data_, DB_SET_RANGE);

			// If the parent is a document node, it could be that it doesn't have a document element,
			// or that document projection has removed it. In those cases we only find out now, in
			// which case we have to move on to the next parent.
			if(err != DB_NOTFOUND || parent_->getType() != DOCUMENT) {
				NO_NEXT_ERROR(err, ((Document&)doc_).getID(),
					nodeObj_->getNodeID());

				rawNode_.setNode(key_, data_);
				return true;
			}
		}
	}

	return false;
}
