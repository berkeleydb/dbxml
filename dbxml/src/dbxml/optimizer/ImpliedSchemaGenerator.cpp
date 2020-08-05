//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ImpliedSchemaGenerator.hpp"
#include "../QueryContext.hpp"
#include "../Value.hpp"
#include "../ContainerBase.hpp"
#include "../Manager.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/MetaDataFunction.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../dataItem/DbXmlUserData.hpp"
#include "../dataItem/LookupIndexFunction.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/NodeHandleFunction.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Range.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Union.hpp>

#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/functions/FunctionBaseURI.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/functions/FunctionCount.hpp>
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/functions/FunctionDeepEqual.hpp>
#include <xqilla/functions/FunctionDistinctValues.hpp>
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocument.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionEndsWith.hpp>
#include <xqilla/functions/FunctionExactlyOne.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionInScopePrefixes.hpp>
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>
#include <xqilla/functions/FunctionNilled.hpp>
#include <xqilla/functions/FunctionId.hpp>
#include <xqilla/functions/FunctionIdref.hpp>
#include <xqilla/functions/FunctionInsertBefore.hpp>
#include <xqilla/functions/FunctionLang.hpp>
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/functions/FunctionLocalname.hpp>
#include <xqilla/functions/FunctionMax.hpp>
#include <xqilla/functions/FunctionMin.hpp>
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/functions/FunctionNamespaceUri.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/functions/FunctionOneOrMore.hpp>
#include <xqilla/functions/FunctionParseXML.hpp>
#include <xqilla/functions/FunctionParseJSON.hpp>
#include <xqilla/functions/FunctionPosition.hpp>
#include <xqilla/functions/FunctionRemove.hpp>
#include <xqilla/functions/FunctionReverse.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionStartsWith.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionStringLength.hpp>
#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/functions/FunctionTrace.hpp>
#include <xqilla/functions/FunctionUnordered.hpp>
#include <xqilla/functions/FunctionZeroOrOne.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static const XMLCh XMLChDot[] = { chColon, chColon, chLatin_d, chLatin_o, chLatin_t, chNull };

ImpliedSchemaGenerator::ImpliedSchemaGenerator(DynamicContext *xpc, Optimizer *parent)
	: ASTVisitorExtender<QueryPathTreeGenerator>(xpc, parent)
{
	resetInternal();
}

QueryPathNode *ImpliedSchemaGenerator::createQueryPathNode(const NodeTest *nodeTest, QueryPathNode::Type type)
{
  return new (mm_) ImpliedSchemaNode((const DbXmlNodeTest*)nodeTest, (ImpliedSchemaNode::Type)type, mm_);
}

NodeTest *ImpliedSchemaGenerator::createNodeTest(const XMLCh *nodeType, const XMLCh *uri, const XMLCh *name)
{
  return new (mm_) DbXmlNodeTest(nodeType, uri, name, mm_);
}

void ImpliedSchemaGenerator::resetInternal()
{
	roots_.clear();
	ASTVisitorExtender<QueryPathTreeGenerator>::resetInternal();
}

static inline void addPaths(ASTNode *item, const QueryPathNode::Vector &paths, XPath2MemoryManager *mm)
{
	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	if(ud == 0) {
		ud = new (mm) DbXmlUserData(mm);
		item->setUserData(ud);
	}

	bool found;
	ImpliedSchemaNode::MVector::iterator it2;

	QueryPathNode::Vector::const_iterator it = paths.begin();
	for(; it != paths.end(); ++it) {
		found = false;
		for(it2 = ud->paths.begin(); it2 != ud->paths.end(); ++it2) {
			if(*it2 == *it) found = true;
		} 
		if(!found)
			ud->paths.push_back((ImpliedSchemaNode*)*it);
	}
}

static inline void addPaths2(ASTNode *item, const QueryPathNode::Vector &paths, XPath2MemoryManager *mm)
{
	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	if(ud == 0) {
		ud = new (mm) DbXmlUserData(mm);
		item->setUserData(ud);
	}

	bool found;
	ImpliedSchemaNode::MVector::iterator it2;

	QueryPathNode::Vector::const_iterator it = paths.begin();
	for(; it != paths.end(); ++it) {
		found = false;
		for(it2 = ud->paths2.begin(); it2 != ud->paths2.end(); ++it2) {
			if(*it2 == *it) found = true;
		} 
		if(!found)
			ud->paths2.push_back((ImpliedSchemaNode*)*it);
	}
}

ASTNode *ImpliedSchemaGenerator::optimizeStep(XQStep *item)
{
	QueryPathTreeGenerator::optimizeStep(item);
	addPaths(item, results_.back().returnPaths, mm_);
	return item;
}

ASTNode *ImpliedSchemaGenerator::optimizeFunction(XQFunction *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	if(uri == DbXmlFunction::XMLChFunctionURI) {
		PathResult result;

		// dbxml:metadata()
		if(name == MetaDataFunction::name) {
			MetaDataFunction *mdFunc = (MetaDataFunction*)item;

			generate(args[0]).markSubtreeValue();

			PathResult targets;
			if(args.size() == 1) {
				const PathResult &currentContext = getCurrentContext();
				targets = currentContext;
			}
			else {
				targets = generate(args[1]);
			}

			const XMLCh *uri, *name;
			if(mdFunc->getConstantQNameArg(1, uri, name, context_)) {

				for(QueryPathNode::Vector::const_iterator it = targets.returnPaths.begin();
				    it != targets.returnPaths.end(); ++it) {
					DbXmlNodeTest *nt = new (mm_)
						DbXmlNodeTest(Node::document_string, uri, name, mm_);

					ImpliedSchemaNode *newNode = new (mm_)
						ImpliedSchemaNode(nt, ImpliedSchemaNode::METADATA,
							mm_);
					result.join((ImpliedSchemaNode*)(*it)->appendChild(newNode));
				}

				addPaths(item, result.returnPaths, mm_);
			}
		}

		else if(name == LookupIndexFunction::name) {
			LookupIndexFunction *fun = (LookupIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				generate(*i).markSubtreeValue();
			}

			generateLookup(fun, fun, ImpliedSchemaNode::CHILD, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}

		else if(name == LookupAttributeIndexFunction::name) {
			LookupAttributeIndexFunction *fun = (LookupAttributeIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				generate(*i).markSubtreeValue();
			}

			generateLookup(fun, fun, ImpliedSchemaNode::ATTRIBUTE, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}

		else if(name == LookupMetaDataIndexFunction::name) {
			LookupMetaDataIndexFunction *fun = (LookupMetaDataIndexFunction*)item;

			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				generate(*i).markSubtreeValue();
			}

			generateLookup(fun, fun, ImpliedSchemaNode::METADATA, fun->getChildURIName(),
				fun->getParentURIName(), result);
		}

		else if(name == DbXmlContainsFunction::name) {
			generateSubstring(ImpliedSchemaNode::SUBSTRING_CD, args, result, item);
		}

		else if(name == HandleToNodeFunction::name) {
			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				generate(*i).markSubtreeValue();
			}

			createAnyNodeResult(result);
		}

		else {
			// We don't know about this function, assume it will only need the descendants of it's node arguments
			for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
				generate(*i).markSubtreeResult();
			}
		}

		push(result);
		return item;
	}
	else if(uri == XQFunction::XMLChFunctionURI) {
		// These generate substring index lookups
		if(name == FunctionStartsWith::name) {
			PathResult result;
			generateSubstring(ImpliedSchemaNode::PREFIX, args, result, item);
			push(result);
			return item;
		}
		else if(name == FunctionEndsWith::name) {
			PathResult result;
			generateSubstring(ImpliedSchemaNode::SUFFIX, args, result, item);
			push(result);
			return item;
		}
		else if(name == FunctionContains::name) {
			PathResult result;
			generateSubstring(ImpliedSchemaNode::SUBSTRING, args, result, item);
			push(result);
			return item;
		}
	}

	QueryPathTreeGenerator::optimizeFunction(item);

	if(uri == XQFunction::XMLChFunctionURI) {
		if(name == FunctionDoc::name ||
			name == FunctionDocument::name ||
			name == FunctionCollection::name ||
			name == FunctionRoot::name) {
			addPaths(item, results_.back().returnPaths, mm_);
		}
	}
	else if(uri == XQillaFunction::XMLChFunctionURI) {
		if(name == FunctionParseXML::name ||
			name == FunctionParseJSON::name) {
			addPaths(item, results_.back().returnPaths, mm_);
		}
	}

	return item;
}

void ImpliedSchemaGenerator::generateLookup(ASTNode *item, QueryPlanRoot *qpr, ImpliedSchemaNode::Type type,
	const char *child, const char *parent, PathResult &result)
{
	DbXmlUserData *ud = (DbXmlUserData*)item->getUserData();
	if(ud != 0) {
                for (ImpliedSchemaNode::MVector::const_iterator i = ud->paths.begin();
                    i != ud->paths.end(); ++i) {
                        (void) result.returnPaths.insert(
                                result.returnPaths.end(), *i);
                }
		return;
	}

	XPath2MemoryManager *mm = mm_;

	DbXmlNodeTest *nt = new (mm) DbXmlNodeTest(Node::document_string);
        ImpliedSchemaNode *root = new (mm) ImpliedSchemaNode(nt, ImpliedSchemaNode::ROOT, mm);
	roots_.push_back(root);
        root->setQueryPlanRoot(qpr);

	const ContainerBase *container = qpr->getContainerBase();

	if(type == ImpliedSchemaNode::METADATA || container == 0 || !container->nodesIndexed()) {
		result.join(root);
	}

	if(type != ImpliedSchemaNode::METADATA && (container == 0 || container->nodesIndexed())) {
		ImpliedSchemaNode *pisn = 0;
		if(parent == 0 || *parent == 0) {
			if(type == ImpliedSchemaNode::ATTRIBUTE) {
				DbXmlNodeTest *nt = new (mm_) DbXmlNodeTest(Node::element_string);
				pisn = new (mm) ImpliedSchemaNode(nt, ImpliedSchemaNode::DESCENDANT, mm);
				pisn = (ImpliedSchemaNode*)root->appendChild(pisn);
			} else {
				pisn = root;
				type = ImpliedSchemaNode::DESCENDANT;
			}
		} else {
			Name pname(parent);
			const char *puri = pname.hasURI() ? pname.getURI() : 0;
			DbXmlNodeTest *nt = new (mm_) DbXmlNodeTest(Node::element_string,
				mm->getPooledString(puri), mm->getPooledString(pname.getName()), mm_);
			pisn = new (mm) ImpliedSchemaNode(nt, ImpliedSchemaNode::DESCENDANT, mm);
			pisn = (ImpliedSchemaNode*)root->appendChild(pisn);
		}

		const XMLCh *node_type = Node::element_string;
		if(type == ImpliedSchemaNode::ATTRIBUTE) {
			node_type = Node::attribute_string;
		}

		ImpliedSchemaNode *cisn = 0;
		if(child == 0 || *child == 0) {
			DbXmlNodeTest *nt = new (mm_) DbXmlNodeTest(node_type);
			cisn = new (mm) ImpliedSchemaNode(nt, type, mm);
		} else {
			Name cname(child);
			const char *curi = cname.hasURI() ? cname.getURI() : 0;
			DbXmlNodeTest *nt = new (mm_) DbXmlNodeTest(node_type,
				mm->getPooledString(curi), mm->getPooledString(cname.getName()), mm_);
			cisn = new (mm) ImpliedSchemaNode(nt, type, mm);
		}
		result.join((ImpliedSchemaNode*)pisn->appendChild(cisn));
	}

	addPaths(item, result.returnPaths, mm_);
}

static StaticType generalCompTypeConversion(const StaticType &arg0_type, const StaticType &arg1_type)
{
	StaticType result = arg0_type;

	StaticType target = arg1_type;
	if(target.getMax() != 0)
		target.setCardinality(1, 1);
	target.substitute(StaticType::NUMERIC_TYPE, StaticType::DOUBLE_TYPE);
	target.substitute(StaticType::UNTYPED_ATOMIC_TYPE, StaticType::STRING_TYPE);

	result.substitute(StaticType::UNTYPED_ATOMIC_TYPE, target);
	return result;
}

static StaticType valueCompTypeConversion(const StaticType &arg0_type)
{
	StaticType result = arg0_type;
	result.substitute(StaticType::UNTYPED_ATOMIC_TYPE, StaticType::STRING_TYPE);
	return result;
}

static XmlValue::Type staticTypeToValueType(const StaticType &sType)
{
	if(sType.isType(StaticType::NODE_TYPE)) return XmlValue::NODE;
	if(sType.isType(StaticType::ANY_SIMPLE_TYPE)) return XmlValue::ANY_SIMPLE_TYPE;
	if(sType.isType(StaticType::ANY_URI_TYPE)) return XmlValue::ANY_URI;
	if(sType.isType(StaticType::BASE_64_BINARY_TYPE)) return XmlValue::BASE_64_BINARY;
	if(sType.isType(StaticType::BOOLEAN_TYPE)) return XmlValue::BOOLEAN;
	if(sType.isType(StaticType::DATE_TYPE)) return XmlValue::DATE;
	if(sType.isType(StaticType::DATE_TIME_TYPE)) return XmlValue::DATE_TIME;
	if(sType.isType(StaticType::DAY_TIME_DURATION_TYPE)) return XmlValue::DAY_TIME_DURATION;
	if(sType.isType(StaticType::DECIMAL_TYPE)) return XmlValue::DECIMAL;
	if(sType.isType(StaticType::DOUBLE_TYPE)) return XmlValue::DOUBLE;
	if(sType.isType(StaticType::DURATION_TYPE)) return XmlValue::DURATION;
	if(sType.isType(StaticType::FLOAT_TYPE)) return XmlValue::FLOAT;
	if(sType.isType(StaticType::G_DAY_TYPE)) return XmlValue::G_DAY;
	if(sType.isType(StaticType::G_MONTH_TYPE)) return XmlValue::G_MONTH;
	if(sType.isType(StaticType::G_MONTH_DAY_TYPE)) return XmlValue::G_MONTH_DAY;
	if(sType.isType(StaticType::G_YEAR_TYPE)) return XmlValue::G_YEAR;
	if(sType.isType(StaticType::G_YEAR_MONTH_TYPE)) return XmlValue::G_YEAR_MONTH;
	if(sType.isType(StaticType::HEX_BINARY_TYPE)) return XmlValue::HEX_BINARY;
	if(sType.isType(StaticType::NOTATION_TYPE)) return XmlValue::NOTATION;
	if(sType.isType(StaticType::QNAME_TYPE)) return XmlValue::QNAME;
	if(sType.isType(StaticType::STRING_TYPE)) return XmlValue::STRING;
	if(sType.isType(StaticType::TIME_TYPE)) return XmlValue::TIME;
	if(sType.isType(StaticType::UNTYPED_ATOMIC_TYPE)) return XmlValue::UNTYPED_ATOMIC;
	if(sType.isType(StaticType::YEAR_MONTH_DURATION_TYPE)) return XmlValue::YEAR_MONTH_DURATION;
	return XmlValue::NONE;
}

static Syntax::Type getComparisonType(const StaticType &arg0_type, const StaticType &arg1_type)
{
	XmlValue::Type type0 = staticTypeToValueType(arg0_type);
	XmlValue::Type type1 = staticTypeToValueType(arg1_type);

	if(type0 == type1) return AtomicTypeValue::convertToSyntaxType(type0);

	// Numeric type promotion
	if(type0 == XmlValue::DOUBLE &&
		(type1 == XmlValue::FLOAT || type1 == XmlValue::DECIMAL)) {
		return Syntax::DOUBLE;
	}
	if(type1 == XmlValue::DOUBLE &&
		(type0 == XmlValue::FLOAT || type0 == XmlValue::DECIMAL)) {
		return Syntax::DOUBLE;
	}
	if(type0 == XmlValue::FLOAT && type1 == XmlValue::DECIMAL) {
		return Syntax::FLOAT;
	}
	if(type1 == XmlValue::FLOAT && type0 == XmlValue::DECIMAL) {
		return Syntax::FLOAT;
	}

	// anyURI type promotion
	if(type0 == XmlValue::ANY_URI && type1 == XmlValue::STRING) {
		return Syntax::STRING;
	}
	if(type1 == XmlValue::ANY_URI && type0 == XmlValue::STRING) {
		return Syntax::STRING;
	}

	return Syntax::NONE;
}

ASTNode *ImpliedSchemaGenerator::optimizeOperator(XQOperator *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	const XMLCh *name = item->getOperatorName();

	PathResult result;

	if(name == Equals::name) {
		generateComparison(ImpliedSchemaNode::EQUALS, /*generateComp*/false, args, result, item);
	}

	else if(name == NotEquals::name) {
		generateComparison(ImpliedSchemaNode::NOT_EQUALS, /*generateComp*/false, args, result, item);
	}

	else if(name == LessThan::name) {
		generateComparison(ImpliedSchemaNode::LTX, /*generateComp*/false, args, result, item);
	}

	else if(name == LessThanEqual::name) {
		generateComparison(ImpliedSchemaNode::LTE, /*generateComp*/false, args, result, item);
	}

	else if(name == GreaterThan::name) {
		generateComparison(ImpliedSchemaNode::GTX, /*generateComp*/false, args, result, item);
	}

	else if(name == GreaterThanEqual::name) {
		generateComparison(ImpliedSchemaNode::GTE, /*generateComp*/false, args, result, item);
	}

	else if(name == GeneralComp::name) {
		switch(((const GeneralComp *)item)->getOperation()) {
		case GeneralComp::EQUAL:
			generateComparison(ImpliedSchemaNode::EQUALS, /*generateComp*/true, args, result, item);
			break;
		case GeneralComp::NOT_EQUAL:
			generateComparison(ImpliedSchemaNode::NOT_EQUALS, /*generateComp*/true, args, result, item);
			break;
		case GeneralComp::LESS_THAN:
			generateComparison(ImpliedSchemaNode::LTX, /*generateComp*/true, args, result, item);
			break;
		case GeneralComp::LESS_THAN_EQUAL:
			generateComparison(ImpliedSchemaNode::LTE, /*generateComp*/true, args, result, item);
			break;
		case GeneralComp::GREATER_THAN:
			generateComparison(ImpliedSchemaNode::GTX, /*generateComp*/true, args, result, item);
			break;
		case GeneralComp::GREATER_THAN_EQUAL:
			generateComparison(ImpliedSchemaNode::GTE, /*generateComp*/true, args, result, item);
			break;
		}
	}

	else {
		return QueryPathTreeGenerator::optimizeOperator(item);
	}

	push(result);
	return item;
}

static ImpliedSchemaNode::Type oppositeOperation(ImpliedSchemaNode::Type type)
{
	switch(type) {
	case ImpliedSchemaNode::LTX: {
		return ImpliedSchemaNode::GTX;
	}
	case ImpliedSchemaNode::LTE: {
		return ImpliedSchemaNode::GTE;
	}
	case ImpliedSchemaNode::GTX: {
		return ImpliedSchemaNode::LTX;
	}
	case ImpliedSchemaNode::GTE: {
		return ImpliedSchemaNode::LTE;
	}
	case ImpliedSchemaNode::EQUALS: {
		return ImpliedSchemaNode::EQUALS;
	}
	case ImpliedSchemaNode::NOT_EQUALS: {
		return ImpliedSchemaNode::NOT_EQUALS;
	}
	default: break;
	}
	return (ImpliedSchemaNode::Type)-1;
}

void ImpliedSchemaGenerator::generateComparison(ImpliedSchemaNode::Type type, bool generalComp,
	VectorOfASTNodes &args, PathResult &result, ASTNode *item)
{
	ASTNode *argA = args[0];
	ASTNode *argB = args[1];

	PathResult retA = generate(argA);
	PathResult retB = generate(argB);

	StaticType arg0_type = argA->getStaticAnalysis().getStaticType();
	StaticType arg1_type = argB->getStaticAnalysis().getStaticType();

	Syntax::Type syntaxType;
	if(generalComp) {
		syntaxType = getComparisonType(generalCompTypeConversion(arg0_type, arg1_type),
			generalCompTypeConversion(arg1_type, arg0_type));
	}
	else {
		syntaxType = getComparisonType(valueCompTypeConversion(arg0_type),
			valueCompTypeConversion(arg1_type));
	}

	XPath2MemoryManager *mm = mm_;
	QueryPathNode::Vector::iterator it;
	for(it = retA.returnPaths.begin(); it != retA.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			Syntax::Type st = syntaxType;
			if((*it)->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA)
				st = AtomicTypeValue::convertToSyntaxType(staticTypeToValueType(arg1_type));

			ImpliedSchemaNode *newChild = (ImpliedSchemaNode*)(*it)->appendChild(new (mm)
				ImpliedSchemaNode(type, generalComp, st, argB, mm));
			result.join(newChild);
		} else (*it)->markSubtreeValue();
	}

	addPaths(item, result.returnPaths, mm_);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();

	for(it = retB.returnPaths.begin(); it != retB.returnPaths.end(); ++it) {
		if(type != (ImpliedSchemaNode::Type)-1) {
			Syntax::Type st = syntaxType;
			if((*it)->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA)
				st = AtomicTypeValue::convertToSyntaxType(staticTypeToValueType(arg0_type));

			ImpliedSchemaNode *newChild = (ImpliedSchemaNode*)(*it)->appendChild(new (mm)
				ImpliedSchemaNode(oppositeOperation(type), generalComp,
					st, argA, mm));
			result.join(newChild);
		} else (*it)->markSubtreeValue();
	}

	addPaths2(item, result.returnPaths, mm_);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();
}

void ImpliedSchemaGenerator::generateSubstring(ImpliedSchemaNode::Type type, VectorOfASTNodes &args, PathResult &result,
	ASTNode *item)
{
	if(args.size() > 2) {
		// TBD case/diacritic insensitive collation? - jpcs

		// Mark the collation argument
		generate(args[2]).markSubtreeValue();
	}

	PathResult ret = generate(args[0]);
	generate(args[1]).markSubtreeValue();

	XPath2MemoryManager *mm = mm_;
	for(QueryPathNode::Vector::iterator it = ret.returnPaths.begin(); it != ret.returnPaths.end(); ++it) {
		ImpliedSchemaNode *newChild = (ImpliedSchemaNode*)(*it)->appendChild(new (mm)
			ImpliedSchemaNode(type, /*generalComp*/false, Syntax::STRING, args[1], mm));
		result.join(newChild);
	}
	addPaths(item, result.returnPaths, mm_);

	// We don't want to return any ImpliedSchemaNodes with a type
	// other than child, attribute, or descendant.
	result.returnPaths.clear();
}

ASTNode *ImpliedSchemaGenerator::optimizeDOMConstructor(XQDOMConstructor *item)
{
	QueryPathTreeGenerator::optimizeDOMConstructor(item);
	addPaths(item, results_.back().returnPaths, mm_);
	return item;
}

ASTNode *ImpliedSchemaGenerator::optimizeCastAs(XQCastAs *item)
{
	PathResult result;

	PathResult argResult = generate(const_cast<ASTNode*>(item->getExpression()));

	for(QueryPathNode::Vector::iterator it = argResult.returnPaths.begin();
	    it != argResult.returnPaths.end(); ++it) {
		ImpliedSchemaNode *newNode = new (mm_) ImpliedSchemaNode(item, mm_);
		result.join((*it)->appendChild(newNode));
	}

	push(result);
	return item;
}

ASTNode *ImpliedSchemaGenerator::optimizeAtomize(XQAtomize *item)
{
	// We treat atomize as if it returns nodes, so that we can add information
	// on the comparison operations in the query
	PathResult ret = generate(const_cast<ASTNode *>(item->getExpression()));
	ret.markSubtreeValue();
	push(ret);
	return item;
}

#define UNCHANGED(name) \
ASTNode *ImpliedSchemaGenerator::optimize ## name (name *item) \
{ \
  return ASTVisitorExtender<QueryPathTreeGenerator>::optimize ## name (item); \
}

UNCHANGED(DbXmlPredicate)
UNCHANGED(QueryPlanToAST)
UNCHANGED(DbXmlNodeCheck)
UNCHANGED(DbXmlLastStepCheck)

////////////////////////////////////////////////////////////////////////////////////////////////////

void ProjectionSchemaAdjuster::addSchemaForIndex(PresenceQP *item)
{
	XPath2MemoryManager *mm = context_->getMemoryManager();
	DbXmlConfiguration *conf = GET_CONFIGURATION(context_);
	ContainerBase *container = item->getContainerBase();

	// We need to add a document projection schema for and DLS+ index lookups,
	// to make sure that the node it points to is still in the projected tree
	if(container != 0 && container->getContainer() != 0 &&
		container->getContainer()->isWholedocContainer() && container->nodesIndexed() &&
		!item->isDocumentIndex() && item->getNodeType() != ImpliedSchemaNode::METADATA) {

		DbXmlNodeTest *nt = new (mm) DbXmlNodeTest(Node::document_string);
		ImpliedSchemaNode *root = new (mm) ImpliedSchemaNode(nt, ImpliedSchemaNode::ROOT, mm);
		roots_.push_back(root);

		Name cname(item->getChildName());
		const char *curi = cname.hasURI() ? cname.getURI() : 0;

		switch(item->getNodeType()) {
		case ImpliedSchemaNode::ATTRIBUTE: {
			ImpliedSchemaNode::Type type = ImpliedSchemaNode::DESCENDANT_ATTR;
			ImpliedSchemaNode *pisn = root;

			if(item->isParentSet()) {
				Name pname(item->getParentName());
				const char *puri = pname.hasURI() ? pname.getURI() : 0;
				DbXmlNodeTest *nt2 = new (mm) DbXmlNodeTest(Node::element_string,
					mm->getPooledString(puri), mm->getPooledString(pname.getName()), mm);
				pisn = new (mm) ImpliedSchemaNode(nt2, ImpliedSchemaNode::DESCENDANT, mm);
				pisn = (ImpliedSchemaNode*)root->appendChild(pisn);
				type = ImpliedSchemaNode::ATTRIBUTE;
			}

			DbXmlNodeTest *nt = new (mm) DbXmlNodeTest(Node::attribute_string,
				mm->getPooledString(curi), mm->getPooledString(cname.getName()), mm);
			pisn->appendChild(new (mm) ImpliedSchemaNode(nt, type, mm));
			break;
		}
		case ImpliedSchemaNode::CHILD:
		case ImpliedSchemaNode::DESCENDANT: {
			DbXmlNodeTest *nt = new (mm) DbXmlNodeTest(Node::element_string,
				mm->getPooledString(curi), mm->getPooledString(cname.getName()), mm);
			root->appendChild(new (mm) ImpliedSchemaNode(nt, ImpliedSchemaNode::DESCENDANT, mm));
			break;
		}
		default: break;
		}

		conf->addImpliedSchemaNode(container->getContainerID(), root);
	}
}

QueryPlan *ProjectionSchemaAdjuster::optimizePresence(PresenceQP *item)
{
	addSchemaForIndex(item);

	return item;
}

QueryPlan *ProjectionSchemaAdjuster::optimizeValue(ValueQP *item)
{
	addSchemaForIndex(item);

	QPValue &value = const_cast<QPValue&>(item->getValue());
	if(value.getASTNode() != 0) {
		value.setASTNode(optimize(const_cast<ASTNode*>(value.getASTNode())));
	}
	return item;
}

QueryPlan *ProjectionSchemaAdjuster::optimizeRange(RangeQP *item)
{
	addSchemaForIndex(item);

	QPValue &value = const_cast<QPValue&>(item->getValue());
	if(value.getASTNode() != 0) {
		value.setASTNode(optimize(const_cast<ASTNode*>(value.getASTNode())));
	}
	QPValue &value2 = const_cast<QPValue&>(item->getValue2());
	if(value2.getASTNode() != 0) {
		value2.setASTNode(optimize(const_cast<ASTNode*>(value2.getASTNode())));
	}
	return item;
}

