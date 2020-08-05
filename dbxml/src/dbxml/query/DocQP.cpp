//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DocQP.hpp"
#include "EmptyQP.hpp"
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
#include <xqilla/exceptions/DynamicErrorException.hpp>

#include <xercesc/util/XMLUri.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

DocQP::DocQP(ASTNode *arg, ImpliedSchemaNode *isn, DynamicContext *context, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(DOC, flags, mm),
	  QueryPlanRoot(0),
	  arg_(arg),
	  isn_(isn),
	  documentName_(0)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);

	if(arg_->isConstant()) {
		const XMLCh* currentUri = getUriArg(context);
		if(currentUri != 0) {
			DbXmlUri uri(context->getBaseURI(), currentUri, /*documentUri*/true);
			if(uri.isDbXmlScheme()) {
				if(uri.getDocumentName() == "") {
					ostringstream oss;
					oss << "The URI '" << uri.getResolvedUri() <<
						"' does not specifiy a document";
					throw XmlException(XmlException::DOCUMENT_NOT_FOUND, oss.str());
				}

				documentName_ = XMLString::replicate(uri.getDocumentName().c_str(), mm);

				try {
					XmlContainer containerWrapper =
						uri.openContainer(conf->getManager(),
								  conf->getTransaction());
					container_ = (Container*)containerWrapper;
					conf->getMinder()->addContainer((Container*)containerWrapper);

					if(container_->getContainer() != 0 && container_->getContainer()->isWholedocContainer()) {
						conf->addImpliedSchemaNode(container_->getContainerID(), documentName_, isn);
					}
				}
				catch(XmlException &e) {
					e.setLocationInfo(this);
					throw;
				}
			} else if(uri.isResolved()) {
				conf->addImpliedSchemaNode(uri.getResolvedUri(), isn);
			}
		}
	} else {
		// Turn document projection off everywhere - it's not safe to project
		// documents if we can't identify everywhere a document will be used at
		// compile time.
		conf->overrideProjection();
	}
}

DocQP::DocQP(ASTNode *arg, ImpliedSchemaNode *isn, ContainerBase *cont, const char *docName, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(DOC, flags, mm),
	  QueryPlanRoot(cont),
	  arg_(arg),
	  isn_(isn),
	  documentName_(docName)
{
}

NodeIterator *DocQP::createNodeIterator(DynamicContext *context) const
{
	return new DocIterator(this);
}

QueryPlan *DocQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DocQP *result = new (mm) DocQP(arg_, isn_, container_, documentName_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void DocQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *DocQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	if(arg_ != 0) {
		arg_ = arg_->staticTyping(context, styper);
		_src.add(arg_->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, 1);
	_src.availableDocumentsUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::ONENODE);

	return this;
}

void DocQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(arg_ != 0) {
		_src.add(arg_->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, 1);
	_src.availableDocumentsUsed(true);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::ONENODE);
}

QueryPlan *DocQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	if(opt.getQueryPlanOptimizer() && arg_ != 0) {
		arg_ = opt.getQueryPlanOptimizer()->optimize(arg_);
	}

	if(container_ == 0) return this;

	QueryPlan *result = new (mm) ValueQP(ImpliedSchemaNode::METADATA, 0, DbXml::metaDataName_uri_name, /*documentIndex*/false, Syntax::STRING,
		DbWrapper::EQUALITY, /*generalComp*/false, documentName_, NsUtil::nsStringLen((unsigned char*)documentName_), isn_, flags_, mm);
	result->setLocationInfo(this);

	result = new (mm) DocExistsCheckQP(result, documentName_, 0, mm);
	result->setLocationInfo(this);

	logTransformation(opt.getLog(), result);

	OptimizationContext newOpt(opt.getPhase(), opt.getContext(), opt.getQueryPlanOptimizer(), container_);
	return result->optimize(newOpt);
}

const XMLCh *DocQP::getUriArg(DynamicContext *context) const
{
	Item::Ptr arg = arg_->createResult(context)->next(context);
	if(arg.isNull()) return 0;

	// Get the uri argument
	const XMLCh *currentUri = arg->asString(context);

	if(currentUri == 0) {
		XQThrow(FunctionException,
			X("DocQP::getUriArg"),
			X("Error retrieving resource [err:FODC0002]"));
	}

	if(!XMLUri::isValidURI(true, currentUri)) {
		XQThrow(FunctionException,
			X("DocQP::getUriArg"),
			X("Invalid URI format [err:FODC0005]"));
	}

	return currentUri;
}

void DocQP::findQueryPlanRoots(QPRSet &qprset) const
{
	qprset.insert(this);
}

Cost DocQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// TBD we need some way to estimate this - jpcs
	return Cost(1, 1);
}

bool DocQP::isSubsetOf(const QueryPlan *o) const
{
	// TBD Can we do better here? - jpcs
	return false;
}

string DocQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<DocQP>" << endl;
	s << DbXmlPrintAST::print(arg_, context, indent + INDENT);
	s << in << "</DocQP>" << endl;

	return s.str();
}

string DocQP::toString(bool brief) const
{
	ostringstream s;

	s << "DOC";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

DocIterator::~DocIterator()
{
	delete result_;
}

void DocIterator::init(DynamicContext *context)
{
	toDo_ = false;

	const XMLCh* currentUri = qp_->getUriArg(context);

	DbXmlUri uri(context->getBaseURI(), currentUri, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		if(uri.getDocumentName() == "") {
			ostringstream oss;
			oss << "The URI '" << uri.getResolvedUri() <<
				"' does not specifiy a document";
			throw XmlException(XmlException::DOCUMENT_NOT_FOUND, oss.str());
		}

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
			context, location_, uri.getDocumentName().c_str(),
			uri.getDocumentName().length());
		documentName_ = uri.getDocumentName();
		result_ = new DocExistsCheck(result_, documentName_.c_str(), location_);
	} else {
		// If it's not a DB XML URI, use the default behaviour
		if(currentUri != 0)
			result_ = new ASTToQueryPlanIterator(context->resolveDocument(currentUri, location_), location_);
		else result_ = new EmptyIterator(location_);
	}
}

bool DocIterator::next(DynamicContext *context)
{
	if(toDo_) init(context);
	return result_->next(context);
}

bool DocIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(toDo_) init(context);
	return result_->seek(container, did, nid, context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DocExistsCheckQP::DocExistsCheckQP(QueryPlan *arg, const char *documentName, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(DOC_EXISTS, arg, flags, mm),
	  documentName_(documentName)
{
}

NodeIterator *DocExistsCheckQP::createNodeIterator(DynamicContext *context) const
{
	return new DocExistsCheck(arg_->createNodeIterator(context), documentName_, this);
}

QueryPlan *DocExistsCheckQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DocExistsCheckQP *result = new (mm) DocExistsCheckQP(arg_->copy(mm), documentName_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void DocExistsCheckQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *DocExistsCheckQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());

	return this;
}

void DocExistsCheckQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
}

QueryPlan *DocExistsCheckQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	return this;
}

void DocExistsCheckQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		DocExistsCheckQP *result = new (mm) DocExistsCheckQP(*it, documentName_, flags_, mm);
		result->setLocationInfo(this);

		combinations.push_back(result);
	}
}

bool DocExistsCheckQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == DOC_EXISTS) {
		DocExistsCheckQP *vf = (DocExistsCheckQP*)o;
		return arg_->isSubsetOf(vf->arg_);
	}

	return arg_->isSubsetOf(o);
}

string DocExistsCheckQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<DocExistsCheckQP>" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << in << "</DocExistsCheckQP>" << endl;

	return s.str();
}

string DocExistsCheckQP::toString(bool brief) const
{
	ostringstream s;

	s << "DE(";
	s << arg_->toString(brief);
	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DocExistsCheck::DocExistsCheck(NodeIterator *parent, const char *documentName, const LocationInfo *location)
	: ProxyIterator(location),
	  documentName_(documentName),
	  seenOne_(false)
{
	result_ = parent;
}

DocExistsCheck::~DocExistsCheck()
{
	delete result_;
}

bool DocExistsCheck::next(DynamicContext *context)
{
	if(!result_->next(context)) {
		if(!seenOne_) {
			XMLBuffer errMsg;
			errMsg.set(X("Document does not exist: "));
			errMsg.append(X(documentName_));
			errMsg.append(X(" [err:FODC0002]"));
			XQThrow3(DynamicErrorException,X("DocExistsCheck::next"), errMsg.getRawBuffer(), location_);
		}
		return false;
	}
	seenOne_ = true;
	return true;
}

bool DocExistsCheck::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!seenOne_) return next(context);
	return result_->seek(container, did, nid, context);
}

