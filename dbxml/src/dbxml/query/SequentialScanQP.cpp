//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "SequentialScanQP.hpp"
#include "StepQP.hpp"
#include "EmptyQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../ContainerBase.hpp"
#include "../Document.hpp"
#include "../OperationContext.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../QueryContext.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/DbXmlFactoryImpl.hpp"
#include "../nodeStore/NsFormat.hpp"
#include "../nodeStore/NsDom.hpp"
// For DLS iterators
#include "../Manager.hpp"
#include "../CacheDatabaseMinder.hpp"
#include "../nodeStore/NsEvent.hpp"
#include "../nodeStore/NsXercesTranscoder.hpp"
#include "../nodeStore/NsSAX2Reader.hpp"
#include "../nodeStore/NsConstants.hpp"
#include "../MemBufInputStream.hpp"
#include "../UTF8.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/util/XMLString.hpp>

#include <float.h>
#include <sstream>
#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

static inline bool char_equals(const char *a, const char *b) {
	return XMLString::equals(a, b);
}

SequentialScanQP::SequentialScanQP(ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(SEQUENTIAL_SCAN, flags, mm),
	  nodeType_(ImpliedSchemaNode::CHILD),
	  isn_(isn),
	  container_(0)
{
	switch(isn->getType()) {
	case ImpliedSchemaNode::ATTRIBUTE: nodeType_ = ImpliedSchemaNode::ATTRIBUTE; break;
	case ImpliedSchemaNode::ROOT: nodeType_ = ImpliedSchemaNode::METADATA; break;
	case ImpliedSchemaNode::METADATA:
		isn_ = 0; // We don't need the ImpliedSchemaNode for a conversion from a METADATA index
		nodeType_ = ImpliedSchemaNode::METADATA;
		break;
	default: break;
	}
}

SequentialScanQP::SequentialScanQP(ImpliedSchemaNode::Type type, ImpliedSchemaNode *isn,
	ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(SEQUENTIAL_SCAN, flags, mm),
	  nodeType_(type),
	  isn_(isn),
	  container_(cont)
{
	// If type is METADATA, we must either have no ImpliedSchemaNode, or it must be a wildcard
	DBXML_ASSERT(type != ImpliedSchemaNode::METADATA || isn == 0 || isn->isWildcard());
}

const char *SequentialScanQP::getChildName() const
{
	return isn_ != 0 && !isn_->isWildcard() ? isn_->getUriName() : 0;
}

QueryPlan *SequentialScanQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	staticTypingLite(context);
	return this;
}

void SequentialScanQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType() = StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	} else {
		_src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	}
}

QueryPlan *SequentialScanQP::optimize(OptimizationContext &opt)
{
	if(opt.getContainerBase() != 0) {
		container_ = opt.getContainerBase();
	}
	if(container_ != 0) {
		if(nameid_ == 0) {
			if(nodeType_ == ImpliedSchemaNode::METADATA) {
				// If we are METADATA, nodeid_ is the ID for dbxml:name
				container_->lookupID(opt.getOperationContext(),
					DbXml::metaDataName_uri_name,
					NsUtil::nsStringLen((const unsigned char*)DbXml::metaDataName_uri_name),
					nameid_);
			}
			else if(isn_ != 0 && !isn_->isWildcardURI()) {
				// If we are filtering on a name, nameid_ is the ID for the URI
				const char *uri = isn_->getURI8();
				if (uri == NULL)
					nameid_ = NS_NOURI;
				else
					container_->lookupID(opt.getOperationContext(),
							     uri, NsUtil::nsStringLen((const unsigned char*)uri), nameid_);
			}
			// lookup NameID for the xmlns URI in this container
			container_->lookupID(opt.getOperationContext(),
					     _xmlnsUri8,
					     NsUtil::nsStringLen((const unsigned char *)_xmlnsUri8),
					     nsUriID_);
		}
	}
	return this;
}

NodeIterator *SequentialScanQP::createNodeIterator(DynamicContext *context) const
{
	DBXML_ASSERT(container_->getContainerID() != 0);

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		return container_->createDocumentIterator(context, this);
	} else {
		NameID uriID = nameid_;
		if(uriID == 0 && isn_ && !isn_->isWildcardURI()) {
			// The URI wasn't in the dictionary when we prepared the query,
			// so we need to look it up now.
			const char *uri = isn_->getURI8();
			OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
			if(!container_->lookupID(oc, uri, NsUtil::nsStringLen((const unsigned char*)uri), uriID)) {
				// If the URI isn't in the dictionary, there are no elements or
				// attributes in the container with that namespace URI at the moment.
				return new EmptyIterator(this);
			}
		}

		NamedNodeIterator *result;
		if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
			result = container_->createAttributeIterator(context,
								     this,
								     nsUriID_);
		} else {
			result = container_->createElementIterator(context,
								   this);
		}

		if(isn_ != 0 && (!isn_->isWildcardURI() || !isn_->isWildcardName())) {
			return new NameFilter(
				result, uriID.raw(),
				(const xmlbyte_t *)isn_->getName8(), this);
		}
		return result;
	}
}

Cost SequentialScanQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(/*cost_.pages == 0 && */container_ != 0) {
		if(nodeType_ == ImpliedSchemaNode::METADATA) {
			cost_ = container_->getDocumentSSCost(context,
				GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache());
		} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
			cost_ = container_->getAttributeSSCost(context,
				GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache(), getChildName());
		} else {
			cost_ = container_->getElementSSCost(context,
				GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache(), getChildName());
		}
	}
	return cost_;
}

StructuralStats SequentialScanQP::getStructuralStats(ContainerBase *container, const char *childUriName,
	OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor)
{
	if(container == 0) return StructuralStats(childUriName != 0, nameID != 0);

	NameID id;
	if(childUriName == 0) {
		id = container->getNIDForRoot();
	} else {
		container->lookupID(oc, childUriName, ::strlen(childUriName), id);
	}

	if(ancestor)
		return cache.get(container, oc, nameID, id);
	return cache.get(container, oc, id, nameID);
}

StructuralStats SequentialScanQP::getStructuralStats(OperationContext &oc, StructuralStatsCache &cache,
	const NameID &nameID, bool ancestor) const
{
	return getStructuralStats(container_, getChildName(), oc, cache, nameID, ancestor);
}

bool SequentialScanQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::SEQUENTIAL_SCAN) {
		SequentialScanQP *ss = (SequentialScanQP*)o;

		if(ss->getNodeType() == nodeType_ &&
			char_equals(getChildName(), ss->getChildName()) &&
			container_ == ss->container_) {
			return true;
		}
	} else if(o->getType() == QueryPlan::PRESENCE) {
		PresenceQP *p = (PresenceQP*)o;

		if(p->getNodeType() == nodeType_ && !p->isParentSet() &&
			char_equals(getChildName(), p->getChildName()) &&
			container_ == p->getContainerBase()) {
			return true;
		}
	}
	return false;
}

void SequentialScanQP::findQueryPlanRoots(QPRSet &qprset) const
{
	if(isn_) qprset.insert(((ImpliedSchemaNode*)isn_->getRoot())->getQueryPlanRoot());
}

QueryPlan *SequentialScanQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	SequentialScanQP *result = new (mm) SequentialScanQP(nodeType_, isn_, container_, flags_, mm);
	result->nameid_ = nameid_;
	result->nsUriID_ = nsUriID_;
	result->cost_ = cost_;
	result->setLocationInfo(this);
	return result;
}

void SequentialScanQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

string SequentialScanQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<SequentialScanQP";
 	if(container_ != 0) {
		s << " container=\"" << container_->getName() << "\"";
	}
	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << " nodeType=\"attribute\"";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << " nodeType=\"document\"";
	}
	else  {
		s << " nodeType=\"element\"";
	}
	if(isn_ != 0 && (!isn_->isWildcardURI() || !isn_->isWildcardName())) {
		s << " name=\"" << isn_->getUriName() << "\"";
	}
	s << "/>" << endl;

	return s.str();
}

string SequentialScanQP::toString(bool brief) const
{
	ostringstream s;

	s << "SS(";

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "document(";
	}

	if(isn_ != 0 && !isn_->isWildcardName()) {
		s << isn_->getUriName();
	} else {
		s << "*";
	}

	if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << ")";
	}
	s  << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define THROW_XMLEXCEPTION(err) { \
  XmlException ex((err)); \
  ex.setLocationInfo(location_); \
  throw ex; \
}

ElementSSIterator::ElementSSIterator(
	DbWrapper *docDb, ContainerBase *container,
	DynamicContext *context, const LocationInfo *location)
	: NsNodeIterator(container, location)
{
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
	int err = open(docDb, oc.txn());
	if (err != 0) throw XmlException(err);
	if(cursor_.error() != 0) throw XmlException(cursor_.error());
	cursor_.setCursorName("ElementSSIterator");
}

ElementSSIterator::ElementSSIterator(
	ContainerBase *container,
	const LocationInfo *location)
	: NsNodeIterator(container, location)
{}

int ElementSSIterator::open(DbWrapper *db, Transaction *txn)
{
	return cursor_.open(*db, txn, CURSOR_READ, 0);
}

bool ElementSSIterator::unmarshal(DbtOut &key, DbtOut &data)
{
	while (true) {
		// Check for the metadata node or the document node and skip it
		if (NsRawNode::isRootOrMetaData(key)) {
			int err = cursor_.get(key, data, DB_NEXT);
			if(err == DB_NOTFOUND) return false;
			if(err != 0) THROW_XMLEXCEPTION(err);
			continue;
		}
		rawNode_.setNode(key, data);

		break;
	}
	return true;
}

static NsNode *createNsNode(DbtOut &key, DbtOut &data, DocID &did)
{
	// Because the DbtOut will stick around until we've
	// finished, we can set copyStrings to false, meaning that
	// the NsNode strings point directly into the node buffer.

	NsFullNid nid;
	nid.clear();
	NsNode *node = 0;
	
	unsigned char *keyp = (unsigned char *)key.data;
	unsigned char *datap = (unsigned char *)data.data;

	const NsFormat &fmt = NsFormat::getFormat((int)(*datap));

	// Unmarshal the key to get the node ID
	fmt.unmarshalNodeKey(did, &nid,
			     keyp, /*copyStrings*/false);


	DBXML_ASSERT(!nid.isMetaDataNid());
	// Unmarshal the data
	node = fmt.unmarshalNodeData(datap,
				     /*adoptBuffer*/false);

	// Copy the NsNid into the NsNode (without allocating)
	NsFullNid *id = node->getFullNid();
	id->setLen(nid.getLen(), /*alloced*/false);
	if(nid.getLen() > NsFullNid::nidStaticSize()) {
		id->setPtr(nid.getBytesForCopy());
	} else {
		// id's len must be set for this to work
		memcpy(id->getBytesForCopy(), nid.getBytes(), nid.getLen());
	}
	return node;
}

bool ElementSSIterator::next(DynamicContext *context)
{
	int err = cursor_.get(key_, data_, DB_NEXT);

	if(err == DB_NOTFOUND) return false;
	if(err != 0) THROW_XMLEXCEPTION(err);

	return unmarshal(key_, data_);
}

bool ElementSSIterator::seek(int container, const DocID &did,
			     const NsNid &nid, DynamicContext *context)
{
	if(container_->getContainerID() < container) return false;

	// seek() must always move the cursor forwards. Therefore
	// we check to see if the seek target is before or equal to
	// DB_NEXT, and if it is we return it.
	int err = cursor_.get(tmp_, data_, DB_NEXT);

	if(err == DB_NOTFOUND) return false;
	if(err != 0) THROW_XMLEXCEPTION(err);

	NsFormat::marshalNodeKey(did, nid, key_);

	if(container_->getContainerID() == container &&
	   lexicographical_bt_compare(0, &tmp_, &key_) < 0) {
		err = cursor_.get(key_, data_, DB_SET_RANGE);

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		return unmarshal(key_, data_);
	} else {
		key_.set(tmp_.data,
			 tmp_.size);
		return unmarshal(key_, data_);
	}
}

NodeInfo::Type ElementSSIterator::getType() const
{
	return ELEMENT;
}

int32_t ElementSSIterator::getNodeURIIndex()
{
	return rawNode_.getURIIndex();
}

const xmlbyte_t *ElementSSIterator::getNodeName()
{
	return rawNode_.getNodeName();
}

u_int32_t ElementSSIterator::getIndex() const
{
	DBXML_ASSERT(false);
	return (u_int32_t)-1;
}

bool ElementSSIterator::isLeadingText() const
{
	DBXML_ASSERT(false);
	return false;
}

DbXmlNodeImpl::Ptr ElementSSIterator::getDbXmlNodeImpl(
	XmlDocument &doc, DocID &did,
	DbtOut &key, DbtOut &data, DynamicContext *context)
{
	NsNode *node = createNsNode(key, data, did);
	// Adopt the memory from the data_ DbtOut
	node->setMemory((unsigned char*)data.adopt_data());

	// Copy the NID (the memory belongs to one of our DbtOut objects)
	NsFullNid *id = node->getFullNid();
	uint32_t len = id->getLen();
	if(len > NsFullNid::nidStaticSize()) {
		const xmlbyte_t *ptr = id->getBytes();
		id->freeNid();
		id->copyNid(ptr, len);
	}

	NsDomElement *element = ((Document&)doc).getElement(node);
	return (DbXmlNodeImpl*)((DbXmlFactoryImpl*)context->
		getItemFactory())->createNode(element, doc, context).get();
}

DbXmlNodeImpl::Ptr ElementSSIterator::getDbXmlNodeImpl(XmlDocument &doc,
						       DynamicContext *context)
{
	DocID did = rawNode_.getDocID();

	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	did.fetchDocument(container_, *conf, doc, conf->getMinder());

	DbXmlNodeImpl::Ptr ret = getDbXmlNodeImpl(doc, did, key_, data_, context);
	rawNode_.clear(); // no longer valid
	return ret;
}

DbXmlNodeImpl::Ptr ElementSSIterator::asDbXmlNode(DynamicContext *context)
{
	XmlDocument doc;
	return getDbXmlNodeImpl(doc, context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// skip namespace decls in attributes.
//   Side effect: modifies uri and index
static const unsigned char * skipNsDecls(
	NsRawNode &rawNode,
	const unsigned char *current,
	u_int32_t &index, nsAttr_t *attr,
	const NameID &nsUriID)
{
	DBXML_ASSERT(nsUriID.raw() != (nameId_t)NS_NOURI);
	if (current)
		++index;
	while (index < rawNode.getNumAttrs()) {
		current = rawNode.getNextAttr(current, attr, index);
		// skip namespace decls
		if (attr->a_uri != (int32_t)nsUriID.raw())
			return current;
		index++;
	}
	return 0;
}

AttributeSSIterator::AttributeSSIterator(
	DbWrapper *docDb, ContainerBase *container,
	DynamicContext *context, const LocationInfo *location,
	const NameID &nsUriID)
	: ElementSSIterator(docDb, container, context, location),
	  index_(0), nsUriID_(nsUriID), current_(0)
{
	
}

bool AttributeSSIterator::next(DynamicContext *context)
{
	if(!rawNode_.isNull()) {
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;
	}

	while(true) {
		int err = cursor_.get(key_, data_, DB_NEXT);

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		if(!unmarshal(key_, data_)) return false;

		index_ = 0;
		current_ = 0;
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;
	}

	return false;
}

bool AttributeSSIterator::seek(int container, const DocID &did,
			       const NsNid &nid, DynamicContext *context)
{
	if(container_->getContainerID() < container) return false;

	if(!rawNode_.isNull()) {
		if(container_->getContainerID() == container &&
		   rawNode_.getDocID() == did &&
		   rawNode_.getNodeID().compareNids(nid) == 0) {
			if ((current_ = skipNsDecls(rawNode_, current_,
						    index_, &attr_, nsUriID_)) != 0)
				return true;
			
		}
	}

	// seek() must always move the cursor forwards. Therefore
	// we check to see if the seek target is before or equal to
	// DB_NEXT, and if it is we return it.
	int err = cursor_.get(tmp_, data_, DB_NEXT);

	if(err == DB_NOTFOUND) return false;
	if(err != 0) THROW_XMLEXCEPTION(err);

	NsFormat::marshalNodeKey(did, nid, key_);

	if(container_->getContainerID() == container &&
	   lexicographical_bt_compare(0, &tmp_, &key_) < 0) {
		err = cursor_.get(key_, data_, DB_SET_RANGE);

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		if(!unmarshal(key_, data_)) return false;
	} else {
		key_.set(tmp_.data,
			 tmp_.size);
		if(!unmarshal(key_, data_)) return false;
	}

	while(true) {
		index_ = 0;
		current_ = 0;
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;

		err = cursor_.get(key_, data_, DB_NEXT);

		if(err == DB_NOTFOUND) return false;
		if(err != 0) THROW_XMLEXCEPTION(err);

		if(!unmarshal(key_, data_)) return false;
	}

	return true;
}

NodeInfo::Type AttributeSSIterator::getType() const
{
	return ATTRIBUTE;
}

int32_t AttributeSSIterator::getNodeURIIndex()
{
	return attr_.a_uri;
}

const xmlbyte_t *AttributeSSIterator::getNodeName()
{
	return current_;
}

u_int32_t AttributeSSIterator::getIndex() const
{
	return index_;
}

bool AttributeSSIterator::isLeadingText() const
{
	DBXML_ASSERT(false);
	return false;
}

static Node::Ptr createAttributeNode(
	const nsAttr_t &attr,
	ContainerBase *container,
	Document *doc,
	const NsNid &nid,
	u_int32_t index,
	DynamicContext *context)
{
	UTF8ToXMLCh prefix;
	UTF8ToXMLCh uri;
	UTF8ToXMLCh name((const char *)attr.a_name.n_text.t_chars);
	UTF8ToXMLCh value((const char *)attr.a_value);
	// fetch/transcode attr info
	// use dictionary to lookup uri and prefix if set
	if (attr.a_uri != NS_NOURI) {
		const char *utf8_pfx = 0;
		const char *utf8_uri = 0;
		DictionaryDatabase *ddb = container->getDictionaryDatabase();
		DBXML_ASSERT(ddb);
		OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
		int err = ddb->lookupStringNameFromID(oc, (NameID)attr.a_uri, &utf8_uri);
		DBXML_ASSERT(err == 0);
		uri.set(utf8_uri);
		if (attr.a_name.n_prefix != NS_NOPREFIX) {
			int err = ddb->lookupStringNameFromID(oc,
							      (NameID)attr.a_name.n_prefix,
							      &utf8_pfx);
			DBXML_ASSERT(err == 0);
			prefix.set(utf8_pfx);
		}
	}
	return ((DbXmlFactoryImpl*)context->
		getItemFactory())->createAttrNode(
			prefix.str(),
			uri.str(),
			name.str(),
			value.str(),
			0, 0, // type
			container, doc, nid, index,
			context);
}

DbXmlNodeImpl::Ptr AttributeSSIterator::asDbXmlNode(DynamicContext *context)
{
	// need to fetch document
	XmlDocument doc;
	DocID did = rawNode_.getDocID();
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	did.fetchDocument(container_, *conf, doc, conf->getMinder());
	return (DbXmlNodeImpl*)
		createAttributeNode(attr_, container_, doc,
				    rawNode_.getNodeID(),
				    index_,
				    context).get();
}

////////////////////////////////////////////////////////////////////
// DLS*SSIterator

// sequential scan iterators that walk a wholedoc (DLS) container,
// materializing documents into a temporary DB as necessary

DLSElementSSIterator::DLSElementSSIterator(
	DocumentDatabase *ddb,
	DictionaryDatabase *dict, ContainerBase *container,
	DynamicContext *context, const LocationInfo *location)
	: ElementSSIterator(container, location),
	  ddb_(ddb),
	  content_(&ddb->getContentDatabase()),
	  dict_(dict),
	  useSet_(false)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	Transaction *txn = conf->getTransaction();
	CacheDatabaseMinder &dbMinder = conf->getDbMinder();
	
	Manager &mgr = (Manager&)container->getManager();
	docdb_ = dbMinder.findOrAllocate(mgr, container->getContainerID());
	ddb->createDocumentCursor(txn, docCursor_, 0);
	docCursor_->first(curDid_);
	// open the temporary node storage db cursor
	open(((CacheDatabase*)docdb_)->getDb(),
	     NULL /* no txn for temp db cursor */);
	int err = materializeDoc(context);
	if (err == DB_NOTFOUND)
		nextDoc(context);
	else
		resetCursor(*NsNid::getRootNid());

	cursor_.setCursorName("DLSElementSSIterator");
}

DLSElementSSIterator::~DLSElementSSIterator()
{
}

int DLSElementSSIterator::materializeDoc(DynamicContext *context)
{
	if (curDid_ == 0)
		return 0; // end of container
	// is this document already in the cache?
	if (CacheDatabaseHandle::docExists(((CacheDatabase*)docdb_)->getDb(),
					   curDid_))
		return 0;
	Transaction *txn = GET_CONFIGURATION(context)->getTransaction();
	curDid_.setDbtFromThis(docKey_);
	int err = ddb_->getContent(txn, docKey_, docData_, 0);
	if (err != 0) {
		if (err == DB_NOTFOUND)
			return err;
		throw XmlException(err);
	}
	XmlInputStream *is = new MemBufInputStream(
		(const char *)docData_.data,
		docData_.size,
		"", false);
	NsPushEventSource *pushev = 0;
	NsParserEventSource *pes = 0;
	NsXercesTranscoder *nxt = 0;
	try {
		nxt = new NsXercesTranscoder(
			txn, // need txn for dictionary access, even if temp DB
			((CacheDatabase*)docdb_)->getDb(),
			dict_,
			curDid_, 0 /* cid */, 0 /*flags*/);
		
		pes = new NsParserEventSource(
			container_->getManager(),
			txn, // txn is ok here -- it's for resolver
			NsParserEventSource::NS_PARSER_WELL_FORMED,
			&is);
		
		// objects are donated to, and deleted by, pushev
		pushev = new
			NsPushEventSourceTranslator(pes, nxt);
		pushev->start();
	} catch (...) {
		// cleanup and re-throw
		if (pes)
			delete pes;
		if (nxt)
			delete nxt;
		throw;
	}
	if (pushev)
		delete pushev;

	if (is)
		delete is;
	return 0;
}

bool DLSElementSSIterator::next(DynamicContext *context)
{
	int err = 0;
	while (true) {
		while (true) {
			if ((err = cursor_.get(key_, data_, getCursorFlag())) == 0)
				break;
			if (err == DB_NOTFOUND) {
				if (!nextDoc(context))
					return false;
			} else {
				THROW_XMLEXCEPTION(err);
			}
		}

		if(unmarshal(key_, data_) && curDid_ == rawNode_.getDocID())
			return true;
		
		// next was into the next document in the temp container but
		// it may have skipped a not-yet-materialized doc so make
		// sure
		if (!nextDoc(context))
			return false;
	}
	DBXML_ASSERT(false); // can't get here
}

void DLSElementSSIterator::resetCursor(const NsNid &nid)
{
	NsFormat::marshalNodeKey(curDid_, nid, key_);
	useSet_ = true;
}

bool DLSElementSSIterator::nextDoc(DynamicContext *context)
{
	int err;
	while(true) {
		// materialize next doc and try again
		docCursor_->next(curDid_);
		if(curDid_ == 0) return false;
		err = materializeDoc(context);
		if (err == 0) {
			resetCursor(*NsNid::getRootNid());
			// don't fetch/unmarshal yet -- let next() take care of that
			return true;
		}
	}

	return false;
}

bool DLSElementSSIterator::seek(int container, const DocID &did,
				const NsNid &nid, DynamicContext *context)
{
	if(container_->getContainerID() < container) return false;

	if(container_->getContainerID() > container || curDid_ > did)
		return next(context);

	int err;
	while (true) {
		if(curDid_ < did) {
			// Seek the correct document in the container
			curDid_ = did;
			docCursor_->seek(curDid_);
			if (curDid_ == 0) return false;
			err = materializeDoc(context);
			if (err == 0) {
				DBXML_ASSERT(did == curDid_);
				resetCursor(nid);
				// Seek the correct node in the document
				err = cursor_.get(key_, data_, getCursorFlag());
			}
			if (err == DB_NOTFOUND) {
				// probably a no-content doc, try next one
				if (!nextDoc(context))
					return false;
				continue;
			}
			if (err != 0)
				THROW_XMLEXCEPTION(err);
		} else {
			// seek() must always move the cursor forwards. Therefore
			// we check to see if the seek target is before or equal to
			// DB_NEXT, and if it is we return it.
			int err = cursor_.get(tmp_, data_, getCursorFlag());
			
			if (err == DB_NOTFOUND) {
				if (!nextDoc(context))
					return false;
				continue;
			} else if (err != 0)
				THROW_XMLEXCEPTION(err);
			NsFormat::marshalNodeKey(did, nid, key_);
			if(lexicographical_bt_compare(0, &tmp_, &key_) < 0) {
				// Seek the correct node in the document
				err = cursor_.get(key_, data_, DB_SET_RANGE);
				if(err == DB_NOTFOUND) {
					if (!nextDoc(context))
						return false;
					continue;
				} else if (err != 0)
					THROW_XMLEXCEPTION(err);
			} else {
				key_.set(tmp_.data,
					 tmp_.size);
			}
		}

		if(unmarshal(key_, data_) && curDid_ == rawNode_.getDocID())
			return true;
		
		if (!nextDoc(context))
			return false;
	}
}

// DLSAttributeSSIterator -- needs to replicate mechanism
// of AttributeSSIterator, since it's not a super class.
DLSAttributeSSIterator::DLSAttributeSSIterator(
	DocumentDatabase *ddb,
	DictionaryDatabase *dict, ContainerBase *container,
	DynamicContext *context, const LocationInfo *location,
	const NameID &nsUriID)
	: DLSElementSSIterator(ddb, dict, container, context, location),
	  index_(0), nsUriID_(nsUriID), current_(0)
{
}

bool DLSAttributeSSIterator::next(DynamicContext *context)
{
	if(!rawNode_.isNull()) {
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;
	}
	while (DLSElementSSIterator::next(context)) {
		index_ = 0;
		current_ = 0;
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;
	}
	return false;
}

bool DLSAttributeSSIterator::seek(int container, const DocID &did,
				  const NsNid &nid, DynamicContext *context)
{
	if(container_->getContainerID() < container) return false;

	if(!rawNode_.isNull()) {
		if(container_->getContainerID() == container &&
		   rawNode_.getDocID() == did &&
		   rawNode_.getNodeID().compareNids(nid) == 0) {
			if ((current_ = skipNsDecls(rawNode_, current_,
						    index_, &attr_, nsUriID_)) != 0)
				return true;
		}
	}

	if (!DLSElementSSIterator::seek(container, did, nid, context))
		return false;

	while(true) {
		index_ = 0;
		current_ = 0;
		if ((current_ = skipNsDecls(rawNode_, current_,
					    index_, &attr_, nsUriID_)) != 0)
			return true;
		if (!DLSElementSSIterator::next(context))
			return false;
	}

	return false;
}

NodeInfo::Type DLSAttributeSSIterator::getType() const
{
	return ATTRIBUTE;
}

int32_t DLSAttributeSSIterator::getNodeURIIndex()
{
	return attr_.a_uri;
}

const xmlbyte_t *DLSAttributeSSIterator::getNodeName()
{
	return current_;
}

u_int32_t DLSAttributeSSIterator::getIndex() const
{
	return index_;
}

bool DLSAttributeSSIterator::isLeadingText() const
{
	DBXML_ASSERT(false);
	return false;
}

DbXmlNodeImpl::Ptr DLSAttributeSSIterator::asDbXmlNode(DynamicContext *context)
{
	// need to fetch document
	XmlDocument doc;
	DocID did = rawNode_.getDocID();
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	did.fetchDocument(container_, *conf, doc, conf->getMinder());
	return (DbXmlNodeImpl*)
		createAttributeNode(attr_, container_, doc,
				    rawNode_.getNodeID(),
				    index_,
				    context).get();
}
