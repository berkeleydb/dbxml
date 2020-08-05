//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ValueFilterQP.hpp"
#include "StepQP.hpp"
#include "QueryExecutionContext.hpp"
#include "StructuralJoinQP.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

ValueFilterQP::ValueFilterQP(QueryPlan *arg, ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(VALUE_FILTER, arg, flags, mm),
	  isn_(isn),
	  collation_(0)
{
}

NodeIterator *ValueFilterQP::createNodeIterator(DynamicContext *context) const
{
        switch((ImpliedSchemaNode::Type)isn_->getType()) {
        case ImpliedSchemaNode::GTX:
        case ImpliedSchemaNode::GTE:
        case ImpliedSchemaNode::LTX:
        case ImpliedSchemaNode::LTE:
        case ImpliedSchemaNode::EQUALS:
        case ImpliedSchemaNode::NOT_EQUALS:
		if(isn_->getGeneralComp()) {
			ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
			if(isnp != 0 && (isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA ||
				   isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::CAST))
				return new MetaDataGeneralCompareFilter(arg_->createNodeIterator(context), isn_, collation_, this);
			else return new GeneralCompareFilter(arg_->createNodeIterator(context), isn_, collation_, this);
		}
		else return new ValueCompareFilter(arg_->createNodeIterator(context), isn_, collation_, this);
        case ImpliedSchemaNode::PREFIX:
		return new StartsWithFilter(arg_->createNodeIterator(context), isn_, this);
        case ImpliedSchemaNode::SUFFIX:
		return new EndsWithFilter(arg_->createNodeIterator(context), isn_, this);
        case ImpliedSchemaNode::SUBSTRING:
		return new ContainsFilter(arg_->createNodeIterator(context), isn_, this);
        case ImpliedSchemaNode::SUBSTRING_CD:
		return new ContainsCDFilter(arg_->createNodeIterator(context), isn_, this);
        default: break;
        }

	DBXML_ASSERT(false);
        return 0;
}

QueryPlan *ValueFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ValueFilterQP *result = new (mm) ValueFilterQP(arg_->copy(mm), isn_, flags_, mm);
	result->setLocationInfo(this);
	result->collation_ = collation_;
	return result;
}

void ValueFilterQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *ValueFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	isn_->setASTNode(const_cast<ASTNode*>(isn_->getASTNode())->staticTyping(context, styper));
	_src.add(isn_->getASTNode()->getStaticAnalysis());

	collation_ = context->getDefaultCollation(this);

	return this;
}

void ValueFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	_src.add(isn_->getASTNode()->getStaticAnalysis());

	collation_ = context->getDefaultCollation(this);
}

QueryPlan *ValueFilterQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	if(opt.getQueryPlanOptimizer())
		isn_->setASTNode(opt.getQueryPlanOptimizer()->optimize(const_cast<ASTNode*>(isn_->getASTNode())));

	return this;
}

void ValueFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		ValueFilterQP *result = new (mm) ValueFilterQP(*it, isn_, flags_, mm);
		result->setLocationInfo(this);
		result->collation_ = collation_;

		combinations.push_back(result);
	}
}

Cost ValueFilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost = StepQP::cost(0, arg_, arg_->cost(context, qec), Join::DESCENDANT_OR_SELF, 0, context,
		GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache());

	// Take a token key away from the result, because it is likely to
	// return less nodes than the argument - we just don't know how many less.
	if(cost.keys > 1) cost.keys -= 1;

	return cost;
}

bool ValueFilterQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == VALUE_FILTER) {
		ValueFilterQP *vf = (ValueFilterQP*)o;
		return isn_->equals(vf->isn_) && arg_->isSubsetOf(vf->arg_);
	}

	return arg_->isSubsetOf(o);
}

string ValueFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<ValueFilterQP comparison=\"";

	switch((ImpliedSchemaNode::Type)isn_->getType()) {
	case ImpliedSchemaNode::GTX: s << "gt"; break;
	case ImpliedSchemaNode::GTE: s << "gte"; break;
	case ImpliedSchemaNode::LTX: s << "lt"; break;
	case ImpliedSchemaNode::LTE: s << "lte"; break;
	case ImpliedSchemaNode::EQUALS: s << "eq"; break;
	case ImpliedSchemaNode::NOT_EQUALS: s << "ne"; break;
	case ImpliedSchemaNode::PREFIX: s << "prefix"; break;
	case ImpliedSchemaNode::SUFFIX: s << "suffix"; break;
	case ImpliedSchemaNode::SUBSTRING: s << "substring"; break;
	case ImpliedSchemaNode::SUBSTRING_CD: s << "substring_cd"; break;
	default: break;
	}

	if(isn_->getGeneralComp())
		s << "\" general=\"true";

	ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
	if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::CAST) {
		s << "\" cast=\"{" << XMLChToUTF8(isnp->getCast()->getSequenceType()->getTypeURI()).str() << "}"
		  << XMLChToUTF8(isnp->getCast()->getSequenceType()->getConstrainingType()->getName()).str();
		isnp = (ImpliedSchemaNode*)isnp->getParent();
	}

	if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
		s << "\" metadata=\"" << isnp->getUriName();
	}

	s << "\">" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << DbXmlPrintAST::print(isn_->getASTNode(), context, indent + INDENT);
	s << in << "</ValueFilterQP>" << endl;

	return s.str();
}

string ValueFilterQP::toString(bool brief) const
{
	ostringstream s;

	s << "VF(";

	s << arg_->toString(brief) << ",";

	ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
	if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
		s << "metadata::" << isnp->getUriName() << ",";
	}

	switch((ImpliedSchemaNode::Type)isn_->getType()) {
	case ImpliedSchemaNode::GTX: s << ">"; break;
	case ImpliedSchemaNode::GTE: s << ">="; break;
	case ImpliedSchemaNode::LTX: s << "<"; break;
	case ImpliedSchemaNode::LTE: s << "<="; break;
	case ImpliedSchemaNode::EQUALS: s << "="; break;
	case ImpliedSchemaNode::NOT_EQUALS: s << "!="; break;
	case ImpliedSchemaNode::PREFIX: s << "prefix"; break;
	case ImpliedSchemaNode::SUFFIX: s << "suffix"; break;
	case ImpliedSchemaNode::SUBSTRING: s << "substring"; break;
	case ImpliedSchemaNode::SUBSTRING_CD: s << "substring_cd"; break;
	default: break;
	}

	s << ",'" << "[to be calculated]" << "')";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ValueFilter::ValueFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  isn_(isn)
{
}

ValueFilter::~ValueFilter()
{
	delete parent_;
}

bool ValueFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool ValueFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ValueCompareFilter::ValueCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation* collation,
	const LocationInfo *location)
	: ValueFilter(parent, isn, location),
	  collation_(collation)
{
}

bool ValueCompareFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		// Value comparisons are intended for comparing single values. The result of a value comparison is
		// defined by applying the following rules, in order:

		AnyAtomicType::Ptr context_first;

		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			context_first = ((DbXmlNodeImpl*)node_.get())->getMetaData(isnp->getURI(), isnp->getName(), context);
			if(context_first.isNull()) {
				if(!parent_->next(context)) return false;
				continue;
			}
		} else {
			// TBD Get the node's value without calling asDbXmlNode() - jpcs
			const XMLCh *context_result = node_->dmStringValue(context);

			try {
				if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::CAST) {
					context_first = isnp->getCast()->cast(context_result, context);
				} else {
					context_first = context->getItemFactory()->createString(context_result, context);
				}
			}
			catch(XQException &) {
				XQThrow3(XPath2TypeCastException, X("ValueCompareFilter::next"),
					X("Invalid lexical value [err:FORG0001]"), location_);
			}
		}

		// 1. Atomization is applied to each operand. If the result, called an atomized operand, does not contain
		//    exactly one atomic value, a type error is raised.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		Result arg_result = isn_->getASTNode()->createResult(context);

		// 2. If the atomized operand is an empty sequence, the result of the value comparison is an empty
		//    sequence, and the implementation need not evaluate the other operand or apply the operator. However,
		//    an implementation may choose to evaluate the other operand in order to determine whether it raises an
		//    error.
		AnyAtomicType::Ptr arg_first = (const AnyAtomicType*)arg_result->next(context).get();
		if(arg_first.isNull()) {
			if(!parent_->next(context)) return false;
			continue;
		}

		// 3. If the atomized operand is a sequence of length greater than one, a type error is raised
		//    [err:XPTY0004].
		Item::Ptr arg_second = arg_result->next(context);
		if(arg_second.notNull()) {
			XQThrow3(XPath2TypeCastException,X("ValueCompareFilter::next"),
				X("A parameter of the operator is not a single atomic value [err:XPTY0004]"),
				location_);
		}

		// 4. Any atomized operand that has the dynamic type xdt:untypedAtomic is cast to the type xs:string.
		if(arg_first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
			arg_first = arg_first->castAs(AnyAtomicType::STRING, context);
		}

		bool result = false;
		switch((ImpliedSchemaNode::Type)isn_->getType()) {
		case ImpliedSchemaNode::EQUALS:
			result = Equals::equals(context_first,arg_first,collation_,context,location_);
			break;
		case ImpliedSchemaNode::NOT_EQUALS:
			// NOT_EQUALS is actually modelled internally as the inverse of !=, which is this case is =
			result = Equals::equals(context_first,arg_first,collation_,context,location_);
			break;
		case ImpliedSchemaNode::LTX:
			result = LessThan::less_than(context_first,arg_first,collation_,context,location_);
			break;
		case ImpliedSchemaNode::LTE:
			result = LessThanEqual::less_than_equal(context_first,arg_first,collation_,context,location_);
			break;
		case ImpliedSchemaNode::GTX:
			result = GreaterThan::greater_than(context_first,arg_first,collation_,context,location_);
			break;
		case ImpliedSchemaNode::GTE:
			result = GreaterThanEqual::greater_than_equal(context_first,arg_first,collation_,context,location_);
			break;
		default: DBXML_ASSERT(0);
		}

		if(result) return true;

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

GeneralCompareFilter::GeneralCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation* collation,
	const LocationInfo *location)
	: ValueFilter(parent, isn, location),
	  collation_(collation)
{
}

bool GeneralCompareFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		// Atomization is applied to each operand of a general comparison.
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		const XMLCh *context_result = node_->dmStringValue(context);
		Result arg_result = isn_->getASTNode()->createResult(context);

		// The result of the comparison is true if and only if there is a pair of atomic values, 
		// one belonging to the result of atomization of the first operand and the other belonging 
		// to the result of atomization of the second operand, that have the required magnitude relationship.
		// Otherwise the result of the general comparison is false.

		AnyAtomicType::Ptr arg_item;
		while((arg_item  = (const AnyAtomicType::Ptr)arg_result->next(context)) != NULLRCP) {

			AnyAtomicType::Ptr toComp = 0;
			try {
				if(arg_item->isNumericValue()) {
					toComp = context->getItemFactory()->
						createDouble(context_result, context);
				}
				else if(arg_item->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
					toComp = context->getItemFactory()->
						createString(context_result, context);
					arg_item = context->getItemFactory()->
						createString(arg_item->asString(context), context);
				}
				else {
					toComp = context->getItemFactory()->
						createDerivedFromAtomicType(arg_item->getTypeURI(),
							arg_item->getTypeName(), context_result, context);
				}
			}
			catch(XQException &) {
				XQThrow3(XPath2TypeCastException, X("DbXmlCompare::GeneralCompareResult::next"),
					X("Invalid lexical value [err:FORG0001]"), location_);
			}

			bool result = false;
			switch((ImpliedSchemaNode::Type)isn_->getType()) {
			case ImpliedSchemaNode::EQUALS:
				result = Equals::equals(toComp,arg_item,collation_,context,location_);
				if(result) return true;
				break;
			case ImpliedSchemaNode::NOT_EQUALS:
				// NOT_EQUALS is actually modelled internally as the inverse of !=, which is this case is
				// every $a, $b satisfies $a = $b
				result = Equals::equals(toComp,arg_item,collation_,context,location_);
				if(!result) goto next_parent;
				break;
			case ImpliedSchemaNode::LTX:
				result = LessThan::less_than(toComp,arg_item,collation_,context,location_);
				if(result) return true;
				break;
			case ImpliedSchemaNode::LTE:
				result = LessThanEqual::less_than_equal(toComp,arg_item,collation_,context,location_);
				if(result) return true;
				break;
			case ImpliedSchemaNode::GTX:
				result = GreaterThan::greater_than(toComp,arg_item,collation_,context,location_);
				if(result) return true;
				break;
			case ImpliedSchemaNode::GTE:
				result = GreaterThanEqual::greater_than_equal(toComp,arg_item,collation_,context,location_);
				if(result) return true;
				break;
			default: DBXML_ASSERT(0);
			}
		}

		if(isn_->getType() == (QueryPathNode::Type)ImpliedSchemaNode::NOT_EQUALS) return true;

next_parent:
		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MetaDataGeneralCompareFilter::MetaDataGeneralCompareFilter(NodeIterator *parent, ImpliedSchemaNode *isn, Collation* collation,
	const LocationInfo *location)
	: ValueFilter(parent, isn, location),
	  collation_(collation)
{
}

static GeneralComp::ComparisonOperation getCompOp(ImpliedSchemaNode::Type type)
{
	switch(type) {
	case ImpliedSchemaNode::EQUALS:
		return GeneralComp::EQUAL;
	case ImpliedSchemaNode::NOT_EQUALS:
		return GeneralComp::EQUAL;
	case ImpliedSchemaNode::LTX:
		return GeneralComp::LESS_THAN;
	case ImpliedSchemaNode::LTE:
		return GeneralComp::LESS_THAN_EQUAL;
	case ImpliedSchemaNode::GTX:
		return GeneralComp::GREATER_THAN;
	case ImpliedSchemaNode::GTE:
		return GeneralComp::GREATER_THAN_EQUAL;
	default: break;
	}
	return (GeneralComp::ComparisonOperation)-1;
}

bool MetaDataGeneralCompareFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		// TBD metadata comparisons - jpcs
		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		DBXML_ASSERT(isnp != 0);

		const XQCastAs *cast = 0;
		if(isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::CAST) {
			cast = isnp->getCast();
			isnp = (ImpliedSchemaNode*)isnp->getParent();
		}

		AnyAtomicType::Ptr node_item(0);
		if(isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			node_item = (AnyAtomicType*)node_->getMetaData(isnp->getURI(), isnp->getName(), context).get();
			if(cast != 0 && node_item.notNull()) {
				node_item = cast->cast(node_item, context);
			}
		}
		// TBD We assume that DB XML doesn't support typed XML trees here - jpcs
		else if(cast != 0) {
			node_item = cast->cast(node_->dmStringValue(context), context);
		}
		else {
			node_item = context->getItemFactory()->createUntypedAtomic(node_->dmStringValue(context), context);
		}

		if(node_item.notNull()) {
			// Atomization is applied to each operand of a general comparison.
			Result arg_result = isn_->getASTNode()->createResult(context);

			// The result of the comparison is true if and only if there is a pair of atomic values, 
			// one belonging to the result of atomization of the first operand and the other belonging 
			// to the result of atomization of the second operand, that have the required magnitude relationship.
			// Otherwise the result of the general comparison is false.

			AnyAtomicType::Ptr arg_item;
			while((arg_item  = (AnyAtomicType*)arg_result->next(context).get()).notNull()) {
				if(GeneralComp::compare(getCompOp((ImpliedSchemaNode::Type)isn_->getType()), node_item, arg_item, collation_,
					   /*xpath1compat*/context, false, location_)) return true;
			}
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ContainsFilter::ContainsFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location)
	: ValueFilter(parent, isn, location)
{
}

bool ContainsFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		Item::Ptr arg_first = isn_->getASTNode()->createResult(context)->
			next(context);
		if(arg_first.isNull()) return true;

		const XMLCh* pattern = arg_first->asString(context);
		if(pattern == 0 || *pattern == 0)
			return true;

		const XMLCh *str = 0;

		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			Item::Ptr md = ((DbXmlNodeImpl*)node_.get())->getMetaData(isnp->getURI(), isnp->getName(), context);
			if(md.notNull()) str = md->asString(context);
		} else {
			str = node_->dmStringValue(context);
		}

		if(str != 0 && *str != 0 && XMLString::patternMatch(str, pattern) > -1)
			return true;

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ContainsCDFilter::ContainsCDFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location)
	: ValueFilter(parent, isn, location)
{
}

bool ContainsCDFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		Item::Ptr arg_first = isn_->getASTNode()->createResult(context)->
			next(context);
		if(arg_first.isNull()) return true;

		const XMLCh* pattern = arg_first->asString(context);
		if(pattern == 0 || *pattern == 0) return true;

		const XMLCh *str = 0;

		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			Item::Ptr md = ((DbXmlNodeImpl*)node_.get())->getMetaData(isnp->getURI(), isnp->getName(), context);
			if(md.notNull()) str = md->asString(context);
		} else {
			str = node_->dmStringValue(context);
		}

		if(str != 0 && *str != 0) {
			XMLBuffer buf1, buf2;
			Normalizer::caseFoldAndRemoveDiacritics(str, buf1);
			Normalizer::caseFoldAndRemoveDiacritics(pattern, buf2);

			if(XMLString::patternMatch(buf1.getRawBuffer(), buf2.getRawBuffer()) > -1)
				return true;
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

StartsWithFilter::StartsWithFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location)
	: ValueFilter(parent, isn, location)
{
}

bool StartsWithFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		Item::Ptr arg_first = isn_->getASTNode()->createResult(context)->
			next(context);
		if(arg_first.isNull()) return true;

		const XMLCh* find = arg_first->asString(context);
		if(find == 0 || *find == 0)
			return true;

		const XMLCh *source = 0;

		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			Item::Ptr md = ((DbXmlNodeImpl*)node_.get())->getMetaData(isnp->getURI(), isnp->getName(), context);
			if(md.notNull()) source = md->asString(context);
		} else {
			source = node_->dmStringValue(context);
		}

		if(source != 0 && *source != 0) {
			while(*source == *find) {
				if(*source == 0)
					return true;
				++source;
				++find;
			}

			if(*find == 0) return true;
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

EndsWithFilter::EndsWithFilter(NodeIterator *parent, ImpliedSchemaNode *isn, const LocationInfo *location)
	: ValueFilter(parent, isn, location)
{
}

bool EndsWithFilter::doNext(DynamicContext *context)
{
	while(true) {
		node_ = parent_->asDbXmlNode(context);

		Item::Ptr arg_first = isn_->getASTNode()->createResult(context)->
			next(context);
		if(arg_first.isNull()) return true;

		const XMLCh* find = arg_first->asString(context);
		if(find == 0 || *find == 0)
			return true;
		
		const XMLCh *source = 0;

		ImpliedSchemaNode *isnp = (ImpliedSchemaNode*)isn_->getParent();
		if(isnp != 0 && isnp->getType() == (QueryPathNode::Type)ImpliedSchemaNode::METADATA) {
			// Lookup the metadata
			Item::Ptr md = ((DbXmlNodeImpl*)node_.get())->getMetaData(isnp->getURI(), isnp->getName(), context);
			if(md.notNull()) source = md->asString(context);
		} else {
			source = node_->dmStringValue(context);
		}

		if(source != 0 && *source != 0) {
			source += XMLString::stringLen(source) - XMLString::stringLen(find);

			while(*source == *find) {
				if(*source == 0)
					return true;
				++source;
				++find;
			}
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

