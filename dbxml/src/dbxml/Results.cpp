//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "dbxml/XmlEventReaderToWriter.hpp"
#include "Results.hpp"
#include "QueryContext.hpp"
#include "OperationContext.hpp"
#include "Container.hpp"
#include "QueryExpression.hpp"
#include "UTF8.hpp"
#include "Manager.hpp"
#include "HighResTimer.hpp"
#include "Document.hpp"
#include "IndexLookup.hpp"
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "ResultsEventWriter.hpp"
#include "nodeStore/NsDom.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>

using namespace DbXml;
using namespace std;


// big macro to encapsulate exceptions that might
// come from XQilla and Xerces
#define CATCH_XQUERY_EXCEPTIONS \
	catch (QueryInterruptedException &) { \
		throw XmlException(XmlException::OPERATION_INTERRUPTED, \
				   "Query was interrupted by the application");\
	} \
	catch (QueryTimeoutException &) { \
		throw XmlException(XmlException::OPERATION_TIMEOUT, \
				   "Query timed out"); \
	} \
	catch (const XQillaException &e) { \
		throw XmlException(XmlException::QUERY_EVALUATION_ERROR, XMLChToUTF8(e.getString()).str()); \
	} \
	catch(const XQException &e) { \
		throw XmlException(XmlException::QUERY_EVALUATION_ERROR, e); \
	} \
	catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) { \
		throw XmlException(XmlException::QUERY_EVALUATION_ERROR, XMLChToUTF8(e.getMessage()).str()); \
	}


ResultsEventWriter& Results::asEventWriter(void)
{
	if (isLazy()) {
    		throw XmlException(XmlException::INVALID_VALUE,
			"XmlResults::asEventWriter() can not be called on lazy result sets");
	}

	if(writer_ == NULL)
		writer_ = new ResultsEventWriter(this);
	else
		throw XmlException(XmlException::INVALID_VALUE,
			"Only one active XmlEventWriter can be used per XmlResults.");


	return *writer_;
}

Results::~Results()
{
	if (writer_)
		delete writer_;
}

Results* Results::copyResults(void)
{
	if (!isLazy())
		reset();
	
	Results* copy = new ValueResults(mgr_);
	copy->concatResults(*this);

	if (!isLazy())
		reset();

	return copy;
}

XmlEventReader::XmlEventType Results::mapToEtype(short nodeType)
{
	if (nodeType == XmlValue::TEXT_NODE)
		return XmlEventReader::Characters;
	if (nodeType == XmlValue::COMMENT_NODE)
		return XmlEventReader::Comment;
	if (nodeType == XmlValue::CDATA_SECTION_NODE)
		return XmlEventReader::CDATA;
	
	DBXML_ASSERT(false);
	return (XmlEventReader::XmlEventType)0;
}

void Results::concatResults(Results &from)
{
	if (isLazy())
		throw XmlException(XmlException::LAZY_EVALUATION,
			"XmlResults::concatResults() can only be called on eager result sets");

	XmlEventWriter &writer = this->asEventWriter();

	XmlValue value;
	while (from.hasNext()){
		from.next(value);
		if (value.isNode()){//copy node types
			short nodeType = value.getNodeType();
			switch (nodeType){
				case XmlValue::ELEMENT_NODE:
				case XmlValue::DOCUMENT_NODE:{
					XmlEventReader &reader = value.asEventReader();
					XmlEventReaderToWriter r2w(reader, writer, true, false);
					r2w.start();
					break;
				}
				case XmlValue::ATTRIBUTE_NODE:
					writer.writeAttribute((const unsigned char *)value.getLocalName().c_str(),
						(const unsigned char *)value.getPrefix().c_str(),
						(const unsigned char *)value.getNamespaceURI().c_str(),
						(const unsigned char *)value.getNodeValue().c_str(),
						false);
					break;
				case XmlValue::CDATA_SECTION_NODE:
				case XmlValue::TEXT_NODE:
				case XmlValue::COMMENT_NODE:
					writer.writeText(mapToEtype(nodeType), 
						(const unsigned char *)value.getNodeValue().c_str(), 0);
					break;
				case XmlValue::DOCUMENT_TYPE_NODE:
					writer.writeDTD((const unsigned char *)value.getNodeValue().c_str(),0);
					break;
				case XmlValue::PROCESSING_INSTRUCTION_NODE:
					writer.writeProcessingInstruction((const unsigned char *)value.getNodeName().c_str(), 
						(const unsigned char *)value.getNodeValue().c_str()); 
					break;
				default :
					DBXML_ASSERT(false);		
			}
		} else {
			XmlValue newValue(value.getType(), value.asString());
			this->add(newValue);
		}
	}
	writer.close();
}

// Results from Values
ValueResults::ValueResults(XmlManager &mgr, Transaction *txn)
	: Results(mgr, txn), vvi_(0)
{
}

ValueResults::ValueResults(const ValueResults &o, XmlManager &mgr,
			   Transaction *txn)
	: Results(mgr, txn), vv_(o.vv_),
	  vvi_(0)
{
	dbMinder_ = o.dbMinder_;
}

ValueResults::ValueResults(const XmlValue &value, XmlManager &mgr,
			   Transaction *txn)
	: Results(mgr, txn), vvi_(0)
{
	if(!value.isNull())
		vv_.push_back(value);
}

ValueResults::ValueResults(Results *resultsToAdopt, XmlManager &mgr,
			   Transaction *txn)
	: Results(mgr, txn), vvi_(0)
{
	if(resultsToAdopt) {
		XmlResults results(resultsToAdopt);
		init(results);
		dbMinder_ = resultsToAdopt->getDbMinder();
		if((*mgr_).allowAutoOpen())
			containers_ = resultsToAdopt->getContainers();
	}
}

void ValueResults::init(XmlResults &results)
{
	XmlValue value;
	while(results.next(value)) {
		if (value.getType() == XmlValue::BINARY)
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlQueryContext::setVariableValue value cannot be binary");
		vv_.push_back(value);
	}
}

ValueResults::~ValueResults()
{
	delete vvi_;
}

int ValueResults::reset()
{
	delete vvi_;
	vvi_ = 0;
	return 0;
}

size_t ValueResults::size() const
{
	return vv_.size();
}

// NOTE: algorithm keeps vvi_ pointing to "current" entry
int ValueResults::doNext(XmlValue &value, bool isPeek)
{
    if (vv_.size() == 0){
        value = XmlValue();
        return 0;
    }

    if (vvi_ == 0) {
        vvi_ = new XmlValueVector::iterator;
        *vvi_ = vv_.begin();
    } 

    //return the current
    if (*vvi_ != vv_.end()){ 
        value = **vvi_;
    } else {
        value = XmlValue();
    }

    // move if not peek or not after last
    if (!isPeek && (*vvi_ != vv_.end()))
        (*vvi_)++;		
	
    return 0;
}

int ValueResults::next(XmlValue &value)
{
	return doNext(value, false);
}

int ValueResults::peek(XmlValue &value)
{
	return doNext(value, true);
}

bool ValueResults::hasNext()
{
	// Don't create the vector if it's not already done; just use size 
	if(vvi_ == 0)
		return (vv_.size() != 0);
	if(*vvi_ != vv_.end())
		return true;
	return false;
}

int ValueResults::previous(XmlValue &value)
{
	if (hasPrevious()) { // share initialization code, below
		(*vvi_)--;
		value = **vvi_;
	} else {
		value = XmlValue();
	}
	return 0;
}

bool ValueResults::hasPrevious()
{
	if ((vvi_ == 0) ||
	    (*vvi_ == vv_.begin()))
		return false;
	return true;
}

void ValueResults::add(const XmlValue &value)
{
	if(!value.isNull())
		vv_.push_back(value);
	else
		throw XmlException(XmlException::INVALID_VALUE, 
				"A null XmlValue object cannot be added to the result set.");
}

void ValueResults::add(const XmlValueVector &values)
{
	vv_.insert(vv_.end(), values.begin(), values.end());
}

// Lazily evaluated results
void LazyResults::add(const XmlValue &value)
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. add() can only be called for eagerly evaluated result sets.");
}

void LazyResults::add(const XmlValueVector &vv)
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. add() can only be called for eagerly evaluated result sets.");
}

size_t LazyResults::size() const
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. size() can only be called for eagerly evaluated result sets.");
	return 0;
}

// Results from XQ tree, evaluated lazily
LazyDIResults::LazyDIResults(QueryContext *context, Value *contextItem,
			     QueryExpression &expr, Transaction *txn, u_int32_t flags)
	: LazyResults(context->getManager(), txn), context_(context),
	  expr_(&expr),
	  contextItem_(contextItem),
	  lazyDocs_((flags & DBXML_LAZY_DOCS) != 0),
	  qec_(context_, /*debugging*/false),
	  conf_(*context, txn, expr.getCompileInfo(),
		  ((DbXmlFactoryImpl*)((QueryExpression*)expr_)->
			  getDynamicContext()->getItemFactory())->getDatatypeLookup()),
	  xqc_(0),
	  result_(0),
	  nextItem_(0)
{
	// this code ensures that transiently-parsed XmlDocument objects
	// share the same CacheDatabaseMinder implementation as the query
	if (contextItem && (contextItem->getType() == XmlValue::NODE)) {
		XmlDocument &xdoc = (XmlDocument&)contextItem->asDocument();
		CacheDatabaseMinder &minder = ((Document *)xdoc)->getDbMinder();
		if (minder.isNull() && !((Document*)xdoc)->getContainerID()) {
			dbMinder_.findOrAllocate((Manager&)(context->getManager()),
						 ((Document *)xdoc)->getContainerID());
			minder = dbMinder_;
		}else
			dbMinder_ = minder;
		if ((*xdoc).isUninitialized()) (*xdoc).changeContentToNsDom(0);
	}
	oc_.set(txn);
	conf_.setMinder(&evaluationMinder_);
	conf_.setQueryExecutionContext(&qec_);
	conf_.setDbMinder(dbMinder_);

	// Set up document projection
	if((flags & DBXML_DOCUMENT_PROJECTION) != 0) {
		conf_.setProjectionInfo(expr.getProjectionInfo());
		flags &= ~DBXML_DOCUMENT_PROJECTION;
	}

	// Documents are always lazy, until they are returned to the user
	conf_.setFlags(flags | DBXML_LAZY_DOCS);

	int err = reset();
	if(err != 0)
		throw XmlException(err);
}

LazyDIResults::~LazyDIResults()
{
	nextItem_ = 0; // [#14890]
	result_ = 0; // Destruct the Result tree before the context
	if(xqc_ != 0) delete xqc_;
}

int LazyDIResults::next(XmlValue &value)
{
	timer_.start();

	try {
		Item::Ptr item;
		if (nextItem_.notNull()) {
			item = nextItem_;
			nextItem_ = 0;
		} else
			item = result_->next(xqc_);
		if(item.isNull()) {
			value = XmlValue();
		} else {
			if(item->isNode()) {
				value = Value::create(item, lazyDocs_);
			}
			else {
				value = Value::create(item, xqc_);
			}
		}
		// iteration may result in construction of a
		// CacheDatabaseMinderImpl object and this object must
		// share it with the DbXmlConfiguration
		if (dbMinder_.isNull() && !conf_.getDbMinder().isNull())
			dbMinder_ = conf_.getDbMinder();
	}
	CATCH_XQUERY_EXCEPTIONS
	timer_.stop();

	if(value.isNull() && !result_.isNull()) {
		result_ = 0;

		if(Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
			ostringstream s;
			s << "Finished query execution, time taken = "
			  << (timer_.durationInSeconds() * 1000) << "ms";
			((Manager &)((QueryContext &)context_).getManager())
				.log(Log::C_QUERY, Log::L_INFO, s);
		}
	}

	return 0;
}

static void
_throwNotImpl(const char * op)
{
	ostringstream s;
	s << "Operation not supported on Lazy XmlResults: ";
	s << op;
	throw XmlException(XmlException::INVALID_VALUE, s.str().c_str());
}

int LazyDIResults::peek(XmlValue &value)
{
	Item::Ptr item = nextItem_;
	if (!item) {
		try {
			nextItem_ = result_->next(xqc_);
			item = nextItem_;
		}
		CATCH_XQUERY_EXCEPTIONS
	}
	int ret = next(value);
	// need to reset nextItem, so iterator doesn't really move
	nextItem_ = item;
	return ret;
}

int LazyDIResults::previous(XmlValue &)
{
	_throwNotImpl("previous");
	return 0;
}

bool LazyDIResults::hasNext()
{
	try {
		if (nextItem_.isNull())
			nextItem_ = result_->next(xqc_);
		if (nextItem_.isNull())
			return false;
	}
	CATCH_XQUERY_EXCEPTIONS
	return true;
}

bool LazyDIResults::hasPrevious()
{
	_throwNotImpl("hasPrevious");
	return false;
}
	
int LazyDIResults::reset()
{
	((Manager &)((QueryContext &)context_).getManager())
		.log(Log::C_QUERY, Log::L_INFO, "Starting query execution");

	timer_.reset();
	timer_.start();

	((QueryContext&)context_).startQuery();

	try {
		if(xqc_ != 0) delete xqc_;
		// additional try/catch to clean up xqc_ if there is
		// and exception during the constructor
		try {
			xqc_ = new (Globals::defaultMemoryManager)
				XQDynamicContextImpl(
					&conf_,
					((QueryExpression*)expr_)->getDynamicContext(),
					Globals::defaultMemoryManager);
			conf_.populateDynamicContext(xqc_);
			if(contextItem_)
				xqc_->setContextItem(
					Value::convertToItem((Value*)contextItem_,
							     xqc_, true));
			
			result_ = ((QueryExpression*)expr_)->
				getCompiledExpression()->execute(xqc_);
		} catch (...) {
			if (xqc_) {
				delete xqc_;
				xqc_ = 0;
			}
			throw; // rethrow
		}
	}
	CATCH_XQUERY_EXCEPTIONS

	timer_.stop();

	return 0;
}

std::map<int, XmlContainer> LazyDIResults::getContainers() const
{
	return (*expr_).getContainers();
}

static DbWrapper::Operation mapIndexOp(XmlIndexLookup::Operation op)
{
	DbWrapper::Operation ret;
	switch (op) {
	case XmlIndexLookup::EQ:
		ret = DbWrapper::EQUALITY;
		break;
	case XmlIndexLookup::LT:
		ret = DbWrapper::LTX;
		break;
	case XmlIndexLookup::LTE:
		ret = DbWrapper::LTE;
		break;
	case XmlIndexLookup::GT:
		ret = DbWrapper::GTX;
		break;
	case XmlIndexLookup::GTE:
		ret = DbWrapper::GTE;
		break;
	default:
		ret = DbWrapper::EQUALITY;
		break;
	}
	return ret;
}

LazyIndexResults::LazyIndexResults(Container &container,
				   QueryContext *context,
				   Transaction *txn, const Index &index,
				   const IndexLookup &il,
				   u_int32_t flags)
	: LazyResults(context->getManager(), txn),
	  context_(context),
	  conf_(*context, txn, 0),
	  container_(&((Container &)container)),
	  lowOp_(DbWrapper::PREFIX),
	  lowKey_(((Manager&)container.getManager()).getImplicitTimezone()),
	  highOp_(DbWrapper::NONE),
	  highKey_(((Manager&)container.getManager()).getImplicitTimezone()),
	  reverse_((flags & DBXML_REVERSE_ORDER) != 0),
	  docOnly_((flags & DBXML_NO_INDEX_NODES) != 0),
	  cacheDocuments_((flags & DBXML_CACHE_DOCUMENTS) != 0)
{
	// validate that IndexLookup object is self-consistent.
	// It checks value types and validity of range operations.
	// It will throw on invalid input.
	il.validate();
	
	bool isNull = il.getLowBoundValue().isNull();

	Name cname(il.getNodeURI(), il.getNodeName());
	string childUriName = cname.getURIName();
	string parentUriName;
	if (il.hasParent()) {
		Name pname(il.getParentURI(), il.getParentName());
		parentUriName = pname.getURIName();
	}
	container.log(Log::C_QUERY, Log::L_INFO, "Starting index lookup");

	conf_.setFlags(flags & ~(DBXML_REVERSE_ORDER|DBXML_NO_INDEX_NODES|DBXML_CACHE_DOCUMENTS));
	conf_.setDbMinder(dbMinder_);
	OperationContext &oc = conf_.getOperationContext();

	lowKey_.setIndex(index);

	if(!isNull && AtomicTypeValue::convertToSyntaxType(
		   il.getLowBoundValue().getType()) != lowKey_.getSyntaxType()) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Value type does not match index syntax type.");
	}
	
	if(!isNull && lowKey_.getSyntaxType() == Syntax::NONE) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "A value has been specified for an index that does not require one.");
	}

	lowKey_.setIDsFromNames(oc, container_, parentUriName.c_str(),
		childUriName.c_str());
	if(index.getPath() == Index::PATH_EDGE && !il.hasParent()) {
		lowKey_.setNodeLookup(true);
	}

	// operation is prefix unless there is at least one value
	if (!isNull) {
		lowOp_ = mapIndexOp(il.getLowBoundOperation());
		lowKey_.setValue(il.getLowBoundValue());
		if (!il.getHighBoundValue().isNull()) {
			highKey_.setIndex(index);
			highKey_.setIDsFromNames(oc, container_,
				parentUriName.c_str(), childUriName.c_str());
			if(index.getPath() == Index::PATH_EDGE &&
			   !il.hasParent())
				highKey_.setNodeLookup(true);
			highOp_ = mapIndexOp(il.getHighBoundOperation());
			highKey_.setValue(il.getHighBoundValue());
		}
	}
	int err = reset();
	if(err != 0)
		throw XmlException(err);
}

int LazyIndexResults::reset()
{
	SyntaxDatabase *sdb = ((Container*)container_)->getIndexDB(lowKey_.getSyntaxType());
	int err = 0;
	if(sdb) {
		if (highOp_ != DbWrapper::NONE) {
			cursor_.reset(sdb->getIndexDB()->
				createCursor(
					conf_.getTransaction(),
					lowOp_, &lowKey_, highOp_, &highKey_, reverse_));
		} else {
			cursor_.reset(sdb->getIndexDB()->
				createCursor(
					conf_.getTransaction(),
					lowOp_, &lowKey_, reverse_));
		}
		err = cursor_->error();
		if(err == 0)
			err = cursor_->first(ie_);
	}
	return err;
}

int LazyIndexResults::doNext(XmlValue &value, bool isPeek)
{
	// ie_ holds the "next" one, which is the entry to process
	DocID docId = ie_.getDocID();
	if(docId == 0) {
		// done
		value = XmlValue();
		return 0;
	}

	XmlDocument document;
	// try to avoid re-fetching documents if the last result
	// was from the same doc
	if (!value.isNull() && (value.getType() == XmlValue::NODE) &&
	    docId == ((const Document&) value.asDocument()).getID())
		document = value.asDocument();
	else
		docId.fetchDocument(container_, conf_, document,
				    (cacheDocuments_ ? &evaluationMinder_ : 0));

	NsDomNode *node = 0;
	if(!docOnly_ && ie_.isSpecified(IndexEntry::NODE_ID)) {
		node = ie_.fetchNode(document, conf_.getTransaction(),
				     &conf_);
	}

	value = DbXmlNodeValue::makeDbXmlNodeValue(node, document, &conf_);

	int err = 0;
	if(!isPeek)
		err = cursor_->next(ie_);
	return err;
}

int LazyIndexResults::next(XmlValue &value)
{
	return doNext(value, false);
}

int LazyIndexResults::peek(XmlValue &value)
{
	return doNext(value, true);
}

bool LazyIndexResults::hasNext()
{
	if (ie_.getDocID() != 0)
		return true;
	return false;
}

//
// these next 2 are harder to do, and have limited usefulness
//
inline void liOpNotSupp(const char *str)
{
	string s("XmlValue operation not supported for lazy index lookup: ");
	s += str;
	throw XmlException(XmlException::LAZY_EVALUATION, s.c_str());
}

int LazyIndexResults::previous(XmlValue &value)
{
	liOpNotSupp("previous");
	return 0;
}

bool LazyIndexResults::hasPrevious()
{
	liOpNotSupp("hasPrevious");
	return false;
}

