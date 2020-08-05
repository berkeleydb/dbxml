//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "CollectionQP.hpp"
#include "SequentialScanQP.hpp"
#include "ASTToQueryPlan.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../QueryContext.hpp"
#include "../OperationContext.hpp"
#include "../ReferenceMinder.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../dataItem/DbXmlUri.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../nodeStore/NsUtil.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

#include <xercesc/util/XMLUri.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

CollectionQP::CollectionQP(ASTNode *arg, ImpliedSchemaNode *isn, DynamicContext *context, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(COLLECTION, flags, mm),
	  QueryPlanRoot(0),
	  arg_(arg),
	  isn_(isn)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);

	if(arg_ == 0 || arg_->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);

		DbXmlUri uri(context->getBaseURI(), currentUri, /*documentUri*/false);
		if(uri.isDbXmlScheme()) {
			try {
				XmlContainer containerWrapper = uri.openContainer(
					conf->getManager(),
					conf->getTransaction());
				container_ = (Container*)containerWrapper;
				conf->getMinder()->addContainer((Container*)containerWrapper);

				if(container_->getContainer() != 0 && container_->getContainer()->isWholedocContainer()) {
					conf->addImpliedSchemaNode(container_->getContainerID(), isn);
				}
			}
			catch(XmlException &e) {
				e.setLocationInfo(this);
				throw;
			}
		}
	} else {
		// Turn document projection off everywhere - it's not safe to project
		// documents if we can't identify everywhere a document will be used at
		// compile time.
		conf->overrideProjection();
	}
}

CollectionQP::CollectionQP(ASTNode *arg, ImpliedSchemaNode *isn, ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(COLLECTION, flags, mm),
	  QueryPlanRoot(cont),
	  arg_(arg),
	  isn_(isn)
{
}

NodeIterator *CollectionQP::createNodeIterator(DynamicContext *context) const
{
	return new CollectionIterator(this);
}

QueryPlan *CollectionQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	CollectionQP *result = new (mm) CollectionQP(arg_, isn_, container_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void CollectionQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *CollectionQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	if(arg_ != 0) {
		arg_ = arg_->staticTyping(context, styper);
		_src.add(arg_->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType(StaticType::NODE_TYPE, 0, StaticType::UNLIMITED);
	_src.availableCollectionsUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	return this;
}

void CollectionQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(arg_ != 0) {
		_src.add(arg_->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType(StaticType::NODE_TYPE, 0, StaticType::UNLIMITED);
	_src.availableCollectionsUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);
}

QueryPlan *CollectionQP::optimize(OptimizationContext &opt)
{
	if(opt.getQueryPlanOptimizer() && arg_ != 0) {
		arg_ = opt.getQueryPlanOptimizer()->optimize(arg_);
	}

	if(container_ == 0) return this;

	XPath2MemoryManager *mm = opt.getMemoryManager();

	QueryPlan *result = new (mm) SequentialScanQP(isn_, flags_, mm);
	result->setLocationInfo(this);

	logTransformation(opt.getLog(), result);

	OptimizationContext newOpt(opt.getPhase(), opt.getContext(), opt.getQueryPlanOptimizer(), container_);
	return result->optimize(newOpt);
}

const XMLCh *CollectionQP::getUriArg(DynamicContext *context) const
{
	const XMLCh *currentUri = 0;

	Item::Ptr arg;
	if(arg_ != 0) {
		arg = arg_->createResult(context)->next(context);
	}

	if(arg.isNull()) {
		// Use the default collection URI
		const std::string &defCol = GET_CONFIGURATION(context)->getDefaultCollection();
		if(!defCol.empty()) {
			currentUri = context->getMemoryManager()
				->getPooledString(UTF8ToXMLCh(defCol).str());
		} else {
			XQThrow(FunctionException,
				X("CollectionQP::getUriArg"),
				X("The default collection has not been set [err:FODC0002]"));
		}
	}
	else {
		// Get the uri argument
		currentUri = arg->asString(context);
	}

	if(currentUri == 0) {
		XQThrow(FunctionException,
			X("CollectionQP::getUriArg"),
			X("Error retrieving resource [err:FODC0002]"));
	}

	if(!XMLUri::isValidURI(true, currentUri)) {
		XQThrow(FunctionException,
			X("CollectionQP::getUriArg"),
			X("Invalid URI format [err:FODC0002]"));
	}

	return currentUri;
}

void CollectionQP::findQueryPlanRoots(QPRSet &qprset) const
{
	qprset.insert(this);
}

Cost CollectionQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// TBD we need some way to estimate this - jpcs
	return Cost(5, 1);
}

bool CollectionQP::isSubsetOf(const QueryPlan *o) const
{
	// TBD Can we do better here? - jpcs
	return false;
}

string CollectionQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	if(arg_ != 0) {
		s << in << "<CollectionQP>" << endl;
		s << DbXmlPrintAST::print(arg_, context, indent + INDENT);
		s << in << "</CollectionQP>" << endl;
	} else {
		s << in << "<CollectionQP/>" << endl;
	}

	return s.str();
}

string CollectionQP::toString(bool brief) const
{
	ostringstream s;

	s << "COL";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

CollectionIterator::~CollectionIterator()
{
	delete result_;
}

void CollectionIterator::init(DynamicContext *context)
{
	toDo_ = false;

	const XMLCh* currentUri = qp_->getUriArg(context);

	DbXmlUri uri(context->getBaseURI(), currentUri, /*documentUri*/false);
	if(uri.isDbXmlScheme()) {
		DbXmlConfiguration *conf = GET_CONFIGURATION(context);

		try {
			cont_ = uri.openContainer(conf->getManager(),
						  conf->getTransaction());
		}
		catch(XmlException &e) {
			e.setLocationInfo(location_);
			throw;
		}

		// Perform a presence lookup on the built-in node-metadata-equality-string
		// index for the document name
		Container *container = (Container*)cont_;
		result_ = container->createDocumentIterator(
			context, location_);
	} else {
		// If it's not a DB XML URI, use the default behaviour
		result_ = new ASTToQueryPlanIterator(context->resolveCollection(currentUri, location_), location_);
	}
}

bool CollectionIterator::next(DynamicContext *context)
{
	if(toDo_) init(context);
	return result_->next(context);
}

bool CollectionIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(toDo_) init(context);
	return result_->seek(container, did, nid, context);
}

