//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"

#include "LookupIndexFunction.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlConfiguration.hpp"
#include "DbXmlUserData.hpp"
#include "../QueryContext.hpp"
#include "../UTF8.hpp"
#include "../OperationContext.hpp"
#include "../Container.hpp"
#include "../ReferenceMinder.hpp"
#include "../query/QueryExecutionContext.hpp"
#include "../query/QueryPlan.hpp"
#include "../query/DecisionPointQP.hpp"
#include "../query/QueryPlanToAST.hpp"
#include "../nodeStore/NsUtil.hpp"

#include <dbxml/XmlDocument.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh LookupIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupIndexFunction::minArgs = 2;
const unsigned int LookupIndexFunction::maxArgs = 3;

// Args are: containerName, node QName, parent QName

LookupIndexFunction::LookupIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, string, string", args, memMgr),
	  QueryPlanRoot(0),
	  childURIName_(0),
	  parentURIName_(0)
{
	_fURI = XMLChFunctionURI;
}

LookupIndexFunction::LookupIndexFunction(const XMLCh* fname, unsigned int argsFrom, unsigned int argsTo,
	const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(fname, argsFrom, argsTo, paramDecl, args, memMgr),
	  QueryPlanRoot(0),
	  childURIName_(0),
	  parentURIName_(0)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* LookupIndexFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* LookupIndexFunction::staticTypingImpl(StaticContext* context)
{
	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	calculateSRCForArguments(context);

	if(context) {
		AutoDelete<DynamicContext> dContext(context->createDynamicContext());
		dContext->setMemoryManager(context->getMemoryManager());

		container_ = getContainerArg(dContext, /*lookup*/false);
		childURIName_ = getURINameArg(2, dContext, /*lookup*/false);
		if(_args.size() == 3) {
			parentURIName_ = getURINameArg(3, dContext, /*lookup*/false);
		}
	}

	return this;
}

ContainerBase *LookupIndexFunction::getContainerArg(DynamicContext *context, bool lookup) const
{
	if(container_ != 0) return container_;

	if(!_args[0]->isConstant() && !lookup) return 0;

	DbXmlConfiguration *conf = GET_CONFIGURATION(context);

	Item::Ptr containerName = getParamNumber(1, context)->next(context);

	try {
		XmlContainer container = DbXmlUri::openContainer(XMLChToUTF8(containerName->asString(context)).str(),
			conf->getManager(), conf->getTransaction());

		Container *tcont = (Container*)container;
		conf->getMinder()->addContainer(tcont);
		return tcont;
	}
	catch(XmlException &e) {
		e.setLocationInfo(this);
		throw;
	}

	// Never reached
	return 0;
}

const char *LookupIndexFunction::getURINameArg(unsigned int argNum, DynamicContext *context, bool lookup) const
{
	if(!_args[argNum - 1]->isConstant() && !lookup) return 0;

	const XMLCh *uri, *name;
	if(lookup) {
		getQNameArg(argNum, uri, name, context);
	} else if(!getConstantQNameArg(argNum, uri, name, context)) {
		return 0;
	}

	Name cname(XMLChToUTF8(uri).str(), XMLChToUTF8(name).str());
	XPath2MemoryManager *mm = context->getMemoryManager();
	size_t len = cname.getURIName().size() + 1;
	if (len == 0)
		return 0;
	char *ret = (char *)mm->allocate(len);
	memcpy(ret, cname.getURIName().c_str(), len);
	return (const char*)ret;
}

QueryPlan *LookupIndexFunction::createQueryPlan(DynamicContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	const char *parent = 0;
	if(_args.size() == 3) {
		parent = parentURIName_;
		if(parent == 0) {
			parent = getURINameArg(3, context, lookup);
			if(parent == 0) return 0;
		}
	}

	ContainerBase *container = getContainerArg(context, lookup);
	if(container == 0) return 0;

	DbXmlUserData *ud = (DbXmlUserData*)getUserData();
	DBXML_ASSERT(ud != 0);
	DBXML_ASSERT(!ud->paths.empty());

	// Search for the ImpliedSchemaNode to use
	ImpliedSchemaNode *isn = ud->paths[0];
	ImpliedSchemaNode::MVector::iterator i = ud->paths.begin();
	for(; i != ud->paths.end(); ++i) {
		if((*i)->getType() == QueryPathNode::CHILD ||
			(*i)->getType() == QueryPathNode::DESCENDANT) {
			isn = *i;
			break;
		}
	}

	PresenceQP *result = new (mm) PresenceQP(ImpliedSchemaNode::CHILD, parent, child, /*documentIndex*/false, isn, 0, mm);
	result->setLocationInfo(this);

	OptimizationContext opt(OptimizationContext::RESOLVE_INDEXES, context, 0, container);
	return result->simpleLookupOptimize(opt);
}

Result LookupIndexFunction::createResult(DynamicContext* context, int flags) const
{
	return new LookupIndexFunctionResult(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const XMLCh LookupAttributeIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_a,
	chLatin_t,
	chLatin_t,
	chLatin_r,
	chLatin_i,
	chLatin_b,
	chLatin_u,
	chLatin_t,
	chLatin_e,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupAttributeIndexFunction::minArgs = 2;
const unsigned int LookupAttributeIndexFunction::maxArgs = 3;

// Args are: containerName, node QName, parent QName

LookupAttributeIndexFunction::LookupAttributeIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: LookupIndexFunction(name, minArgs, maxArgs, "string, string, string", args, memMgr)
{
}

ASTNode* LookupAttributeIndexFunction::staticTypingImpl(StaticContext* context)
{
	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType() = StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	calculateSRCForArguments(context);

	if(context) {
		AutoDelete<DynamicContext> dContext(context->createDynamicContext());
		dContext->setMemoryManager(context->getMemoryManager());

		container_ = getContainerArg(dContext, /*lookup*/false);
		childURIName_ = getURINameArg(2, dContext, /*lookup*/false);
		if(_args.size() == 3) {
			parentURIName_ = getURINameArg(3, dContext, /*lookup*/false);
		}
	}

	return this;
}

QueryPlan *LookupAttributeIndexFunction::createQueryPlan(DynamicContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	const char *parent = 0;
	if(_args.size() == 3) {
		parent = parentURIName_;
		if(parent == 0) {
			parent = getURINameArg(3, context, lookup);
			if(parent == 0) return 0;
		}
	}

	ContainerBase *container = getContainerArg(context, lookup);
	if(container == 0) return 0;

	DbXmlUserData *ud = (DbXmlUserData*)getUserData();
	DBXML_ASSERT(ud != 0);
	DBXML_ASSERT(!ud->paths.empty());

	// Search for the ImpliedSchemaNode to use
	ImpliedSchemaNode *isn = ud->paths[0];
	ImpliedSchemaNode::MVector::iterator i = ud->paths.begin();
	for(; i != ud->paths.end(); ++i) {
		if((*i)->getType() == QueryPathNode::ATTRIBUTE) {
			isn = *i;
			break;
		}
	}

	PresenceQP *result = new (mm) PresenceQP(ImpliedSchemaNode::ATTRIBUTE, parent, child, /*documentIndex*/false, isn, 0, mm);
	result->setLocationInfo(this);

	OptimizationContext opt(OptimizationContext::RESOLVE_INDEXES, context, 0, container);
	return result->simpleLookupOptimize(opt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const XMLCh LookupMetaDataIndexFunction::name[] = {
	chLatin_l,
	chLatin_o,
	chLatin_o,
	chLatin_k,
	chLatin_u,
	chLatin_p,
	chDash,
	chLatin_m,
	chLatin_e,
	chLatin_t,
	chLatin_a,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chDash,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_x,
	chNull
};

const unsigned int LookupMetaDataIndexFunction::minArgs = 2;
const unsigned int LookupMetaDataIndexFunction::maxArgs = 2;

// Args are: containerName, name QName

LookupMetaDataIndexFunction::LookupMetaDataIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: LookupIndexFunction(name, minArgs, maxArgs, "string, string", args, memMgr)
{
}

ASTNode* LookupMetaDataIndexFunction::staticTypingImpl(StaticContext* context)
{
	_src.clear();

	_src.availableCollectionsUsed(true);
	_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	calculateSRCForArguments(context);

	if(context) {
		AutoDelete<DynamicContext> dContext(context->createDynamicContext());
		dContext->setMemoryManager(context->getMemoryManager());

		container_ = getContainerArg(dContext, /*lookup*/false);
		childURIName_ = getURINameArg(2, dContext, /*lookup*/false);
	}

	return this;
}

QueryPlan *LookupMetaDataIndexFunction::createQueryPlan(DynamicContext *context, bool lookup) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	const char *child = childURIName_;
	if(child == 0) {
		child = getURINameArg(2, context, lookup);
		if(child == 0) return 0;
	}

	ContainerBase *container = getContainerArg(context, lookup);
	if(container == 0) return 0;

	DbXmlUserData *ud = (DbXmlUserData*)getUserData();
	DBXML_ASSERT(ud != 0);
	DBXML_ASSERT(ud->paths.size() == 1);

	PresenceQP *result = new (mm) PresenceQP(ImpliedSchemaNode::METADATA, 0, child, /*documentIndex*/false, ud->paths[0], 0, mm);
	result->setLocationInfo(this);

	OptimizationContext opt(OptimizationContext::RESOLVE_INDEXES, context, 0, container);
	return result->simpleLookupOptimize(opt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Item::Ptr LookupIndexFunction::LookupIndexFunctionResult::next(DynamicContext *context)
{
	if(result_.isNull()) {
		qp_.set(func_->createQueryPlan(context, /*lookup*/true));
		result_ = new QueryPlanToASTResult(qp_->createNodeIterator(context), this);
	}
	return result_->next(context);
}

