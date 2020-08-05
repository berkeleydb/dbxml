//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlUpdateFactory.hpp"

#include <xqilla/update/PendingUpdateList.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include "DbXmlConfiguration.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlUri.hpp"
#include <dbxml/XmlManager.hpp>
#include <dbxml/XmlUpdateContext.hpp>
#include "../Document.hpp"
#include "../UpdateContext.hpp"
#include "../nodeStore/NsUpdate.hpp"
#include "../nodeStore/NsDom.hpp"
#include "../UTF8.hpp"

using namespace DbXml;
using namespace std;

#define  DBXML_DONT_DELETE_DOCUMENTS 1

// NOTE: updates on documents that may have come from disk originally (vs
// a container) will *not* be applied or written back to the disk.
// TBD: If/when scripting extensions are implemented, such updates will need
// to be at least applied

//
// "next" is NsDomNode * because the navigational methods used
// to calculate it in callers are available via NsDom and not via
// DbXmlNodeImpl, which uses axis iterators.  They could be added
// if ever deemed necessary and/or cleaner
//
void DbXmlUpdateFactory::applyInserts(
	const PendingUpdate &update,
	const DbXmlNodeImpl *parent,
	const NsDomNode *next,
	DynamicContext *context,
        bool firstOrAfter)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	XmlManager &mgr = conf->getManager();
	OperationContext &oc = conf->getOperationContext();
	Document *document = const_cast<Document*>(parent->getDocument());
	
	Result children = update.getValue();
	Item::Ptr item;
	while((item = children->next(context)).notNull()) {
		const DbXmlNodeImpl *child = (const DbXmlNodeImpl*)item->
			getInterface(DbXmlNodeImpl::gDbXml);
		switch(child->getNodeType()) {
		case nsNodeElement:
		{
			update_.insertElement(*child, *parent,
					      next /* next */,
					      mgr, *document, oc, 
                                              context, firstOrAfter);
			break;
		}
		case nsNodeText:
		case nsNodeCDATA:
		case nsNodePinst:
		case nsNodeComment:
		{
			update_.insertText(*child, *parent, next,
					   *document, oc, context);
			break;
		}
		default:
			throw XmlException(XmlException::INVALID_VALUE,
					   "Cannot insert a node that is not element or text");
		}
	}
}

void DbXmlUpdateFactory::applyPut(const PendingUpdate &update, DynamicContext *context)
{
	DbXmlUri uri(update.getValue().first()->
		     asString(context), true);
	if (uri.isDbXmlScheme()) {
		const DbXmlNodeImpl *content =
			(const DbXmlNodeImpl*)update.getTarget().get();
		string cname = uri.getContainerName();
		string docname = uri.getDocumentName();
		DbXmlConfiguration *conf = GET_CONFIGURATION(context);
		XmlManager &mgr = conf->getManager();
		XmlContainer cont = ((Manager&)mgr).getOpenContainer(cname);
		if (cont.isNull()) {
			string msg = "Target container for fn:put -- ";
			msg += cname;
			msg += " -- must be open";
			throw XmlException(XmlException::INVALID_VALUE,
					   msg);
		}
		OperationContext &oc = conf->getOperationContext();
		XmlDocument doc = mgr.createDocument();
		doc.setName(docname);
		XmlEventReader *reader =
			(XmlEventReader*)content->getEventReader(context);
		DBXML_ASSERT(reader);
		doc.setContentAsEventReader(*reader);
		XmlUpdateContext uc = mgr.createUpdateContext();
		// use internal interface to avoid additional transaction
		int err = ((Container &)cont).addDocumentInternal(oc.txn(), doc, uc, 0);
		if (err != 0)
			throw XmlException(err);
	}
}

void DbXmlUpdateFactory::applyInsertInto(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *parent = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!parent->isUpdateAble())
		return;
	// 0 for next implies append
	applyInserts(update, parent, 0, context, false);
}

// do the work of attribute insertion -- shared by applyInsertAttributes
// and applyReplaceAttributes
void DbXmlUpdateFactory::insertAttributes(const PendingUpdate &update,
					  const DbXmlNodeImpl *parent,
					  DynamicContext *context)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	OperationContext &oc = conf->getOperationContext();
	Document *document = const_cast<Document*>(parent->getDocument());
	
	vector<const DbXmlNodeImpl *> nodeVec;
	Result children = update.getValue();
	Item::Ptr item;
	while((item = children->next(context)).notNull()) {
		const DbXmlNodeImpl *attr = (const DbXmlNodeImpl*)item->
			getInterface(DbXmlNodeImpl::gDbXml);
		nodeVec.push_back(attr);
	}	
	update_.insertAttributes(nodeVec, *parent, *document, oc, context);
}

void DbXmlUpdateFactory::applyInsertAttributes(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *parent = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!parent->isUpdateAble())
		return;

	insertAttributes(update, parent, context);
}

void DbXmlUpdateFactory::applyReplaceValue(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *target = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!target->isUpdateAble())
		return;

	// This is slow, but effective...
	// Create a new node with the replaced value and
	//    treat this like replaceNode
	// This is ok because text and attribute nodes have no identity
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getItemFactory();
	Node::Ptr newNode;
	const XMLCh *value = update.getValue().first()->asString(context);
	switch (target->getNodeType()) {
	case nsNodeAttr: {
		newNode = factory->
			createAttrNode(target->getPrefix(),
				       target->getUri(),
				       target->getLocalName(),
				       value,
				       0, 0, // type name and URI
				       0, 0, NsNid(), 0, // cont, doc, nid, index
				       context);
		break;
	}
	case nsNodeText:
	case nsNodeComment:
	case nsNodeCDATA: {
		newNode = factory->
			createTextNode(target->getNodeType(), value, context);
		break;
	}
	case nsNodePinst: {
		newNode = factory->
			createPINode(target->getPITarget(), value, context);
		break;
	}
	default:
		DBXML_ASSERT(false);
		break;
	}

	// now, replace...
	Sequence seq(newNode);
	PendingUpdate pu(PendingUpdate::REPLACE_NODE,
			 update.getTarget(),
			 seq,
			 &update);
	if (target->getNodeType() == nsNodeAttr)
		applyReplaceAttribute(pu, context);
	else
		applyReplaceNode(pu, context);
}

void DbXmlUpdateFactory::renameAttribute(const PendingUpdate &update,
					 ATQNameOrDerived *qname,
					 DynamicContext *context)
{
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getItemFactory();
	// create new attribute, using new name, old value
	Node::Ptr newNode = factory->createAttrNode(
		(qname->getURI() ? qname->getPrefix() : 0),
		qname->getURI(),
		qname->getName(),
		((const DbXmlNodeImpl*)update.getTarget().get())->getValue(),
		0, 0, // type name and URI
		0, 0, NsNid(), 0, // cont, doc, nid, index
		context);
	// now, replace...
	Sequence seq(newNode);
	PendingUpdate pu(PendingUpdate::REPLACE_NODE,
			 update.getTarget(),
			 seq,
			 &update);
	applyReplaceAttribute(pu, context);
}

void DbXmlUpdateFactory::renamePI(const PendingUpdate &update,
					 const XMLCh *name,
					 DynamicContext *context)
{
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getItemFactory();
	// create new attribute, using new name, old value
	Node::Ptr newNode = factory->createPINode(
		name, 
		((const DbXmlNodeImpl*)update.getTarget().get())->getValue(),
		context);
	// now, replace...
	Sequence seq(newNode);
	PendingUpdate pu(PendingUpdate::REPLACE_NODE,
			 update.getTarget(),
			 seq,
			 &update);
	applyReplaceNode(pu, context);
}

void DbXmlUpdateFactory::applyRename(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!node->isUpdateAble())
		return;
	ATQNameOrDerived *qname = (ATQNameOrDerived*)update.getValue().first().get();

	// Retrieve fresh node from database using RMW

	switch(node->getType()) {
	case NodeInfo::ELEMENT: {
		DbXmlConfiguration *conf = GET_CONFIGURATION(context);
		OperationContext &oc = conf->getOperationContext();
		Document *document = const_cast<Document*>(node->getDocument());
		DBXML_ASSERT(document);
		update_.renameElement(*node, qname, *document, oc, context);
		// Remove index entries under old name

		// Put prefix and URI in the dictionary

		// Update the prefix, URI, and name
		// Update NS_NAMEPREFIX, NS_HASURI flags

		// Add index entries for new name
			
		break;
	}
	case NodeInfo::ATTRIBUTE: {
		renameAttribute(update, qname, context);

		// Remove index entries under old name

		// Put prefix and URI in the dictionary

		// Update the prefix, URI, and name
		// Update NS_ATTR_PREFIX, NS_ATTR_URI flags

		// Add index entries for new name
			
		break;
	}
	case NodeInfo::PI: {
		// no indexes on PI
		renamePI(update, qname->getName(), context);
		break;
	}
	default: DBXML_ASSERT(false); break;
	}
}

void DbXmlUpdateFactory::applyDelete(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)update.getTarget().get();

	forDeletion_.insert(nodeImpl);
}

void DbXmlUpdateFactory::applyInsertBefore(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *next = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!next->isUpdateAble())
		return;
	Node::Ptr parent = next->dmParent(context);
	NsDomNodeRef nextRef = next->getNsDomNode();
	applyInserts(update,
		     (const DbXmlNodeImpl *)parent->getInterface(DbXmlNodeImpl::gDbXml),
		     nextRef.get(),
		     context,
                     false);
}

void DbXmlUpdateFactory::applyInsertAfter(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *prev = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!prev->isUpdateAble())
		return;
	Node::Ptr parent = prev->dmParent(context);
	// in order to preserve order for multiple inserts, insertAfter must turn
	// into insertBefore
	NsDomNodeRef prevRef = prev->getNsDomNode();
       NsDomNodeRef nextRef = prevRef->getNsNextSibling();
       if (!nextRef.get()) {
              DbXmlConfiguration *conf = GET_CONFIGURATION(context);
              prevRef->refreshNode(conf->getOperationContext(), true);
              nextRef = prevRef->getNsNextSibling();
       }
	applyInserts(update,
		     (const DbXmlNodeImpl *)parent->getInterface(DbXmlNodeImpl::gDbXml),
		     nextRef.get(),
		     context,
                     true);
}

void DbXmlUpdateFactory::applyInsertAsFirst(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *parent = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!parent->isUpdateAble())
		return;
	NsDomNodeRef parentRef = parent->getNsDomNode();
	NsDomNodeRef nextRef = parentRef->getNsFirstChild();
       if (!nextRef.get()) {
              DbXmlConfiguration *conf = GET_CONFIGURATION(context);
              parentRef->refreshNode(conf->getOperationContext(), true);
              nextRef = parentRef->getNsFirstChild();
       }
	applyInserts(update, parent, nextRef.get(), context, true);
}

void DbXmlUpdateFactory::applyInsertAsLast(const PendingUpdate &update, DynamicContext *context)
{
	// NOTE: this is the same as applyInsertInto, which
	// is implemented as append
	const DbXmlNodeImpl *parent = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!parent->isUpdateAble())
		return;
	// 0 for next implies append
	applyInserts(update, parent, 0, context, false);
}

void DbXmlUpdateFactory::applyReplaceNode(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *target = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!target->isUpdateAble())
		return;
	Node::Ptr parent = target->dmParent(context);
	// insert all new nodes *before* the target, then
	// mark target for deletion
	NsDomNodeRef targetRef = target->getNsDomNode();
	applyInserts(update,
		     (const DbXmlNodeImpl *)parent->getInterface(DbXmlNodeImpl::gDbXml),
		     targetRef.get(),
		     context, false);
	forDeletion_.insert(target);
}

void DbXmlUpdateFactory::applyReplaceAttribute(const PendingUpdate &update, DynamicContext *context)
{
	// Replace via insert (at end), then mark target for removal.
	// This will not maintain order and results in rewriting the
	// parent node twice.
	// NOTE: this changes the index identity of the attribute (its value
	// in node indexes).  This needs to be accounted for in reindexing
	const DbXmlNodeImpl *target = (const DbXmlNodeImpl*)update.getTarget().get();
	if (!target->isUpdateAble())
		return;
	Node::Ptr parent = target->dmParent(context);
	insertAttributes(update,
			 (const DbXmlNodeImpl *)parent->
			 getInterface(DbXmlNodeImpl::gDbXml),
			 context);
	forDeletion_.insert(target);
}

void DbXmlUpdateFactory::applyReplaceElementContent(const PendingUpdate &update, DynamicContext *context)
{
	const DbXmlNodeImpl *target = (const DbXmlNodeImpl*)update.getTarget().get();
	// TBD: this check is commented out... need to re-check why.
//	if (!target->isUpdateAble())
//		return;

	// use child axis to create nodes to mark for delete
	DbXmlChildAxis children(0, target, 0);
	Item::Ptr item;
	while((item = children.next(context)).notNull()) {
		const DbXmlNodeImpl *child = (const DbXmlNodeImpl*)item->
			getInterface(DbXmlNodeImpl::gDbXml);
		forDeletion_.insert(child);
	}	

	// insert new content
	const XMLCh *value = update.getValue().first()->asString(context);
	if(value != 0 && *value != 0) {
		// create text node
		DbXmlConfiguration *conf = GET_CONFIGURATION(context);
		OperationContext &oc = conf->getOperationContext();
		Document *document = const_cast<Document*>(target->getDocument());
		Node::Ptr content = ((DbXmlFactoryImpl*)context->getItemFactory())->
			createTextNode(nsNodeText, value, context);
		update_.insertText(*(const DbXmlNodeImpl*)content->
				   getInterface(DbXmlNodeImpl::gDbXml),
				   *target, 0,
				   *document, oc, context);
	}
}

// NOTE: Because removal of elements can cause text content to
//  change node ownership, thereby making it impossible to locate
//  the proper nodes, it's necessary to either:
//  1.  always remove nodes in document order (will remove leading
//      text first) or
//  2.  perform deletions in 2 passes -- first non-element nodes, then
//      element nodes
// As of release 2.4, (1) is true because of the way the list is created
//
void DbXmlUpdateFactory::completeUpdate(DynamicContext *context)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	XmlManager &mgr = conf->getManager();
	OperationContext &oc = conf->getOperationContext();
	
	for(NodeSet::iterator i = forDeletion_.begin();
	    i != forDeletion_.end(); ++i) {
		const DbXmlNodeImpl *node = i->get();
		try {
			// see if the node is update-able (i.e. persistent)
			if (!node->isUpdateAble())
				continue;
			Document *document = const_cast<Document*>(node->getDocument());
			DBXML_ASSERT(document);
			
			switch(node->getType()) {
			case NodeInfo::DOCUMENT: {
#ifdef DBXML_DONT_DELETE_DOCUMENTS
				throw XmlException(XmlException::QUERY_EVALUATION_ERROR,
						   "Cannot delete a document node [err:XUDY0020]");
#else
				update_.removeDocument(*node,
						       *(node->getContainer()->getContainer()),
						       mgr, oc);
#endif
				break;
			}
			case NodeInfo::ELEMENT: {
				update_.removeElement(*node, *document, oc, context);
				break;
			}
			case NodeInfo::ATTRIBUTE: {
				update_.removeAttribute(*node, *document, oc, context);
				break;
			}
			case NodeInfo::TEXT:
			case NodeInfo::COMMENT:
			case NodeInfo::PI: {
				update_.removeText(*node, *document, oc, context);
				break;
			}
			}
		} catch (XmlException &xe) {
			// ignore DB_NOTFOUND in this path -- nodes may
			// have already been deleted via a parent tree
			if ((xe.getExceptionCode() ==
			     XmlException::DATABASE_ERROR) &&
			    (xe.getDbErrno() == DB_NOTFOUND))
				continue;
			throw;
		}
	}
	update_.completeUpdate(mgr, oc, context);
}

bool DbXmlUpdateFactory::NodeSetCompare::operator()(const DbXmlNodeImpl::Ptr &first, const DbXmlNodeImpl::Ptr &second) const
{
	return NodeInfo::compare(first.get(), second.get()) < 0;
}

// perform BDB XML-specific validity checks on the pending updates.  This allows
// early detection of issues such as multiple root elements.  The tricky part is
// detecting the difference between a legitimate insertion into an empty document
// and one that adds a root element.  This requires looking at the document node
// to see if has existing child elements.
//

// DocClass counts document elements inserted into and deleted from
// documents.
class DocClass {
public:
	DocClass(Document *doc, int count = 0, bool empty = false)
		: doc_(doc), count_(count + (empty ? 0 : 1)), deletedRootElem_(false) {}
	bool operator<(const DocClass &other) const {
		return (doc_->getID() < other.doc_->getID());
	}
	void setRootElementDeleted() { deletedRootElem_ = true; }
	bool getRootElementDeleted() const { return deletedRootElem_; }
	void incrCount(int n) { count_ += n; }
	// subtract out the deleted document element if it was done
	// Keeping this separate from count handles the multiple-delete of the
	// same node case
	int getNumRootElements() const { return (count_ - (deletedRootElem_ ? 1 : 0)); }
	Document *doc_;
	int count_;
	bool deletedRootElem_;
};

struct dsetcompare {
	bool operator()(const DocClass &d1, const DocClass &d2) const {
		return (d1 < d2);
	}
};

static bool isEmptyDoc(const DbXmlNodeImpl *nodeImpl)
{
	const NsNid nid = nodeImpl->getLastDescendantID();
	return (nid == *NsNid::getRootNid());
}

// This method has 2 functions:
// 1.  count number of elements in the child list
// 2.  ensure that text children are either comments or PI
// or if text that it is all whitespace.
// CDATA cannot be inserted (XQilla will not allow it).
static int numElements(const PendingUpdate &update,
		       DynamicContext *context)
{
	int num = 0;
	Result children = update.getValue();
	Item::Ptr item;
	while((item = children->next(context)).notNull()) {
		const DbXmlNodeImpl *child = (const DbXmlNodeImpl*)item->
			getInterface(DbXmlNodeImpl::gDbXml);
		if (child) {
			switch (child->getNodeType()) {
			case nsNodeElement:
				++num;
				break;
			case nsNodeComment:
			case nsNodePinst:
				break; // OK
			case nsNodeText: {
				XMLChToUTF8 s(child->getValue());
				if (!NsUtil::isWhitespace(s.str())) {
					throw XmlException(
						XmlException::QUERY_EVALUATION_ERROR,
						"Cannot insert non-whitespace text node as a child of the document node");
				}
				break;
			}
			default: DBXML_ASSERT(false); break;
			}
		}
	}
	return num;
}

typedef set<DocClass, dsetcompare> DocSet;

void DbXmlUpdateFactory::checkUpdates(const PendingUpdateList &pul, DynamicContext *context, DocumentCache::ValidationMode valMode)
{
	DocSet docMap;
	PendingUpdateList::const_iterator i;
	for (i = pul.begin(); i != pul.end(); ++i) {
		switch (i->getType()) {
		case PendingUpdate::PUDELETE: {
			const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)i->getTarget().get();
			u_int32_t level = nodeImpl->getNodeLevel();
			// level 1 is children of the document node
			if (level == 1
			    && (nodeImpl->getType() == NodeInfo::ELEMENT)
			    && nodeImpl->isUpdateAble()) {
				Document *document = const_cast<Document*>(nodeImpl->getDocument());
				DBXML_ASSERT(document);
				DocSet::iterator it = docMap.find(DocClass(document));
				if (it == docMap.end()) {
					DocClass dc(document);
					dc.setRootElementDeleted();
					docMap.insert(dc);
				} else {
					const_cast<DocClass&>(*it).setRootElementDeleted();
				}
			}
			break;
		}
		case PendingUpdate::INSERT_INTO:
		case PendingUpdate::INSERT_INTO_AS_FIRST:
		case PendingUpdate::INSERT_INTO_AS_LAST: {
			const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)i->getTarget().get();
			u_int32_t level = nodeImpl->getNodeLevel();
			// level 0 is document node -- inserting a potential
			// document/root element
			if (level == 0 &&
			    nodeImpl->isUpdateAble()) {
				int nElems = numElements(*i, context);
				if (nElems != 0) {
					Document *document = const_cast<Document*>(nodeImpl->getDocument());
					DBXML_ASSERT(document);
					DocSet::iterator it = docMap.find(DocClass(document));
					if (it == docMap.end()) {
						docMap.insert(DocClass(document, nElems, isEmptyDoc(nodeImpl)));
					} else {
						const_cast<DocClass&>(*it).incrCount(nElems);
					}
				}
			}
			break;
		}
		case PendingUpdate::INSERT_BEFORE:
		case PendingUpdate::INSERT_AFTER: {
			const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)i->getTarget().get();
			u_int32_t level = nodeImpl->getNodeLevel();
			// level 1 is a child of the document node
			if (level == 1 &&
			    nodeImpl->isUpdateAble()) {
				int nElems = numElements(*i, context);
				if (nElems != 0) {
					Document *document = const_cast<Document*>(nodeImpl->getDocument());
					DBXML_ASSERT(document);
					DocSet::iterator it = docMap.find(DocClass(document));
					if (it == docMap.end()) {
						docMap.insert(DocClass(document, nElems));
					} else {
						const_cast<DocClass&>(*it).incrCount(nElems);
					}
				}
			}
			break;
		}
		case PendingUpdate::REPLACE_NODE: {
			const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)i->getTarget().get();
			u_int32_t level = nodeImpl->getNodeLevel();
			// level 1 is children of the document node
			if (level == 1
			    && (nodeImpl->getType() == NodeInfo::ELEMENT)
			    && nodeImpl->isUpdateAble()) {
				// replacing document element.  If replacing 1 for 1, no-op
				// if replacing with more or 0 elements, count
				int nElems = numElements(*i, context);
				if (nElems != 1) {
					Document *document = const_cast<Document*>(nodeImpl->getDocument());
					DBXML_ASSERT(document);
					DocSet::iterator it = docMap.find(DocClass(document));
					if (it == docMap.end()) {
						DocClass dc(document);
						if (nElems == 0)
							dc.setRootElementDeleted();
						else
							dc.incrCount(nElems);
						docMap.insert(dc);
					} else {
						if (nElems == 0)
							const_cast<DocClass&>(*it).setRootElementDeleted();
						else
							const_cast<DocClass&>(*it).incrCount(nElems);
					}
				}
			}
			break;
		}
			
		default:
			break;
		}
	}
	// perform sanity checks.
	for (DocSet::const_iterator it = docMap.begin(); it != docMap.end(); it++) {
		const DocClass &dc = *it;
		int count = dc.getNumRootElements();
		if (count > 1) {
 			throw XmlException(XmlException::QUERY_EVALUATION_ERROR,
					   "Cannot perform an update that creates a persistent document with more than one document element");
		} else if (count < 1) {
			throw XmlException(XmlException::QUERY_EVALUATION_ERROR,
					   "Cannot perform an update that creates a persistent document with no document element");
		}
	}
}
