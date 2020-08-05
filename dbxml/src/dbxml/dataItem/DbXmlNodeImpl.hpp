//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNODEIMPL_HPP
#define	__DBXMLNODEIMPL_HPP

#include <set>

#include <dbxml/XmlDocument.hpp>
#include <dbxml/XmlTransaction.hpp>

#include <xqilla/items/Node.hpp>
#include <xqilla/utils/XMLChCompare.hpp>

#include "../IndexEntry.hpp"
#include "../nodeStore/NsTypes.hpp"
#include "../query/NodeInfo.hpp"
#include "../nodeStore/NsUtil.hpp"
#include "../nodeStore/NsNid.hpp"
#include "../nodeStore/NsNode.hpp"

namespace DbXml
{

class NsDomNode;
class NsDomElement;
class NsDomAttr;
class DbXmlConfiguration;
class Value;
class DbXmlNodeTest;
class NsNid;
class EventReader;
class ContainerBase;
	
typedef RefCountJanitor<NsDomNode> NsDomNodeRef;
typedef std::set<std::string> DoneSet;
	
// DbXmlNodeImpl
// Pure virtual base for several types of Node/Items:
//  1. encapsulation of NsDomNode (DbXmlNsDomNode)
//  2. standalone attribute, text and PI nodes (DbXmlAttributeNode,
//	DbXmlTextNode, DbXmlPINode)
//  3. namespace nodes (DbXmlNamespaceNode)

class DbXmlNodeImpl : public Node, public NodeInfo
{
public:
	typedef RefCountPointer<const DbXmlNodeImpl> Ptr;

	/** The "DbXmlNodeImpl" node interface */
	static const XMLCh gDbXml[];

	DbXmlNodeImpl() {}
	virtual ~DbXmlNodeImpl();

	virtual void decrementRefCount() const {
		if(--const_cast<unsigned int&>(_ref_count) == 0)
			delete this;
	}
	/// NodeInfo methods
	virtual int getContainerID() const { return 0; }
	virtual DocID getDocID() const { return 0; }
	virtual const NsNid getNodeID() const { return NsNid(); }
	virtual const NsNid getLastDescendantID() const { return NsNid(); }
	virtual u_int32_t getNodeLevel() const { return 0; }
	virtual u_int32_t getIndex() const { return (u_int32_t)-1; }
	virtual bool isLeadingText() const { return false; }
	/// end NodeInfo

	virtual ContainerBase *getContainer() const { return 0; }
	virtual NsDomNodeRef getNsDomNode() const { return 0; }
	virtual NsDomNode * getParentNode() const { return 0; }
	/// Called if the DbXmlNodeImpl gets put into a NodeValue and returned to outside the query
	virtual void returnedFromQuery() {}
	
	/// Item methods
	virtual bool isNode() const { return true; }
	virtual bool isAtomicValue() const { return false; }
	virtual const XMLCh* asString(
		const DynamicContext* context) const;
	virtual const XMLCh* getTypeURI() const;
	virtual const XMLCh* getTypeName() const;
	virtual void *getInterface(const XMLCh *name) const {
		if(name == gDbXml)
			return (void*)this;
		return 0;
	}
	/// end Item

	/// Node methods
	// implemented in this base
	virtual bool hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName,
				       const DynamicContext* context) const;
	virtual Sequence dmBaseURI(const DynamicContext* context) const;
	virtual Sequence dmDocumentURI(const DynamicContext* context) const;
	virtual const XMLCh* dmNodeKind() const;
	virtual ATQNameOrDerived::Ptr dmTypeName(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmNilled(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmIsId(const DynamicContext* context) const;
	virtual ATBooleanOrDerived::Ptr dmIsIdRefs(const DynamicContext* context) const;
	virtual Sequence dmTypedValue(DynamicContext* context) const;
	virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const;
	virtual bool equals(const Node::Ptr &other) const;
	virtual bool uniqueLessThan(const Node::Ptr &other,
				    const DynamicContext *context) const;
	virtual Node::Ptr root(const DynamicContext* context) const;
	virtual Node::Ptr dmParent(const DynamicContext* context) const {
		return 0;
	}
	virtual Result dmAttributes(const DynamicContext* context,
				    const LocationInfo *location) const
		{ return 0; }
	virtual Result dmNamespaceNodes(const DynamicContext* context,
					const LocationInfo *location) const
		{ return 0; }
	virtual Result dmChildren(const DynamicContext *context,
				  const LocationInfo *location) const
		{ return 0; }

	virtual void generateEvents(
		EventHandler *events,
		const DynamicContext *context,
		bool preserveNS = true,
		bool preserveType = true) const {}
	
	/* these must be implemented by subclasses:
	   virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	   virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
	*/

	/// end Node and Item

	/// Virtual behavior for DbXmlNodeImpl
	// non-virtual class methods
	virtual const XmlDocument *getXmlDocument() const { return 0; }
	virtual const Document *getDocument() const { return 0; }
	// can this instance be modified?
	virtual bool isUpdateAble() const { return false; }
	
	virtual short getNodeType() const = 0;
	virtual Item::Ptr getMetaData(const XMLCh *uri,
				      const XMLCh *name,
				      DynamicContext *context) const {
		return 0;
	}
	virtual void getNodeHandle(IndexEntry &ie) const;
	virtual EventReader *getEventReader(const DynamicContext *) const
		{ return 0; }
	// added for NodeValue implementation and partial NsDom removal
	virtual const XMLCh *getLocalName() const { return 0; }
	virtual const XMLCh *getUri() const { return 0; }
	virtual const XMLCh *getPrefix() const { return 0; }
	virtual const XMLCh *getValue() const { return 0; }
	virtual const XMLCh *getPITarget() const { return 0; }
protected:
	void checkReadOnly() const;
};
	
class DbXmlNsDomNode: public DbXmlNodeImpl
{
public:
	typedef RefCountPointer<const DbXmlNsDomNode> Ptr;

	DbXmlNsDomNode(Document *doc, const DynamicContext *context);
	DbXmlNsDomNode(const NsDomNode *node, Document *doc,
		       const DynamicContext *context);
	DbXmlNsDomNode(const NsDomNode *node, Document *doc,
		       DbXmlConfiguration *conf);
	DbXmlNsDomNode(const IndexEntry::Ptr &ie,
		       const ContainerBase *container,
		       const DynamicContext *context);
	virtual ~DbXmlNsDomNode();

	/// NodeInfo methods
	virtual NodeInfo::Type getType() const;
	virtual int getContainerID() const;
	virtual DocID getDocID() const;
	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;
	/// end NodeInfo

	virtual ContainerBase *getContainer() const;
	virtual NsDomNodeRef getNsDomNode() const;
	virtual NsDomNode * getParentNode() const;
	virtual void returnedFromQuery();
	
	/// Item methods
	virtual bool isNode() const { return true; }
	virtual bool isAtomicValue() const { return false; }
	/// end Item

	// Node methods
	virtual Sequence dmBaseURI(const DynamicContext* context) const;
	virtual Sequence dmDocumentURI(const DynamicContext* context) const;
	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual Result dmAttributes(const DynamicContext* context,
				    const LocationInfo *location) const;
	virtual Result dmNamespaceNodes(const DynamicContext* context,
					const LocationInfo *location) const;
	virtual Result dmChildren(const DynamicContext *context,
				  const LocationInfo *location) const;
	virtual Result getAxisResult(XQStep::Axis axis,
				     const NodeTest *nodeTest,
				     const DynamicContext *context,
				     const LocationInfo *location) const;
	virtual void generateEvents(EventHandler *events, const DynamicContext *context,
		bool preserveNS = true, bool preserveType = true) const;


	/// Virtual behavior for DbXmlNodeImpl
	virtual const XmlDocument *getXmlDocument() const;
	virtual const Document *getDocument() const;
	virtual bool isUpdateAble() const;
	virtual short getNodeType() const;
	virtual Item::Ptr getMetaData(const XMLCh *uri,
				      const XMLCh *name,
				      DynamicContext *context) const;
	virtual const XMLCh *getLocalName() const;
	virtual const XMLCh *getUri() const;
	virtual const XMLCh *getPrefix() const;
	virtual const XMLCh *getValue() const;
	virtual const XMLCh *getPITarget() const;
	virtual EventReader *getEventReader(const DynamicContext *context) const;
private:
	void readerToEventHandler(EventHandler *events, EventReader *reader,
				  bool outputNamespaces) const;
	void getDocumentAsNode();
	Transaction *getTransaction() const { return (Transaction*)txn_; }
protected:
	// Before materialisation
	IndexEntry::Ptr ie_;
	const ContainerBase *container_;
	DbXmlConfiguration *conf_;
	mutable XmlTransaction txn_;

	// After materialisation
	XmlDocument document_;
	NsDomNodeRef node_;
};

/// A (hacky) namespace node, that does as much as we need for XQuery.
/// This may need to be changed if we start supporting XPath 2.
class DbXmlNamespaceNode : public DbXmlNodeImpl
{
public:
	typedef RefCountPointer<const DbXmlNamespaceNode> Ptr;

	DbXmlNamespaceNode(const XMLCh *prefix, const XMLCh *uri,
			   NsDomElement *parent, Document *doc,
			   const DynamicContext *context);
	~DbXmlNamespaceNode() {}

	virtual NodeInfo::Type getType() const {
		return NodeInfo::ATTRIBUTE; // won't be called
	}
	virtual const XMLCh* dmNodeKind() const { return namespace_string; }
	virtual ATQNameOrDerived::Ptr dmTypeName(
		const DynamicContext* context) const { return 0; }
	virtual ATQNameOrDerived::Ptr dmNodeName(
		const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(
		const DynamicContext* context) const;
	virtual Sequence dmTypedValue(DynamicContext* context) const;

	virtual int compare(const Node::Ptr &o) const { return 0; }
	virtual bool lessThan(const Node::Ptr &other,
			      const DynamicContext *context) const { return false; }
	virtual bool equals(const Node::Ptr &other) const { return false; }
	virtual bool uniqueLessThan(const Node::Ptr &other,
				    const DynamicContext *context) const { return false; }

	virtual Result getAxisResult(XQStep::Axis axis,
		const NodeTest *nodeTest,
		const DynamicContext *context,
		const LocationInfo *location) const
	{ return 0; }

	virtual Node::Ptr dmParent(const DynamicContext* context) const;
	virtual const XMLCh* getTypeURI() const { return 0; }
	virtual const XMLCh* getTypeName() const { return 0; }

	virtual const XMLCh *getLocalName() const { return 0; }
	virtual const XMLCh *getUri() const { return uri_; }
	virtual const XMLCh *getPrefix() const { return prefix_; }
	
	virtual short getNodeType() const;
private:
	XmlDocument document_;
	NsString prefix_;
	NsString uri_;
	NsDomNodeRef parent_; // an element
};

/// An attribute node that does not depend on NsDomAttr.
/// It can represent an attribute in a real document or a
/// standalone constructed attribute.  In the former case,
/// it will have a valid XmlDocument, NsFullNid, and index.
class DbXmlAttributeNode : public DbXmlNodeImpl
{
public:
	typedef RefCountPointer<const DbXmlAttributeNode> Ptr;

	DbXmlAttributeNode(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localName,
			   const XMLCh *value, const XMLCh *typeName,
			   const XMLCh *typeURI, const ContainerBase *container,
			   Document *doc, const NsNid &nid, u_int32_t index,
			   const DynamicContext *context);
	DbXmlAttributeNode(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localName,
			   const XMLCh *value, const XMLCh *typeName,
			   const XMLCh *typeURI, const ContainerBase *container,
			   const DocID &did, const NsNid &nid, u_int32_t index);
	~DbXmlAttributeNode() {}

	virtual ContainerBase *getContainer() const;
	virtual int getContainerID() const;
	virtual DocID getDocID() const { return did_; }
	virtual NodeInfo::Type getType() const {
		return NodeInfo::ATTRIBUTE; // won't be called
	}
	virtual void generateEvents(
		EventHandler *events,
		const DynamicContext *context,
		bool preserveNS = true,
		bool preserveType = true) const;

	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;

	virtual void returnedFromQuery();

	virtual Result getAxisResult(XQStep::Axis axis,
		const NodeTest *nodeTest,
		const DynamicContext *context,
		const LocationInfo *location) const;

	virtual NsDomNode *getParentNode() const;
	// comparison functions are not needed -- the default works
	// because this object gets a unique document ID
	virtual Node::Ptr dmParent(const DynamicContext* context) const;

	virtual const XmlDocument *getXmlDocument() const {
		if (doc_.isNull())
			return 0;
		return &doc_;
	}
	virtual const Document *getDocument() const {
		return (Document*)doc_;
	}
	virtual bool isUpdateAble() const;
	virtual const NsNid getNodeID() const { return &nid_; }
	virtual u_int32_t getIndex() const { return index_; }
	virtual const XMLCh* getTypeURI() const;
	virtual const XMLCh* getTypeName() const;
	virtual const XMLCh *getLocalName() const { return localName_; }
	virtual const XMLCh *getUri() const { return uri_; }
	virtual const XMLCh *getPrefix() const { return prefix_; }
	virtual const XMLCh *getValue() const { return value_; }
	virtual short getNodeType() const;

	virtual Sequence dmBaseURI(const DynamicContext* context) const;

private:
	NsString prefix_;
	NsString uri_;
	NsString localName_;
	NsString value_;
	NsString typeName_;
	NsString typeURI_;
	XmlDocument doc_;
	NsFullNid nid_;
	u_int32_t index_;
	const ContainerBase *container_;
	const DocID did_;
	DbXmlConfiguration *conf_;
};


/// A standalone text node, used for text, comments, and PI.
/// This object is purely standalone and has no parent,
/// nor navigational ability.
/// NOTE: CDATA is mapped to type nsNodeText, since they
/// are processed the same.
class DbXmlTextNode : public DbXmlNodeImpl
{
public:
	typedef RefCountPointer<const DbXmlTextNode> Ptr;

	// text, comment ctor
	DbXmlTextNode(short type, const XMLCh *value,
		      const DynamicContext *context);
	DbXmlTextNode(short type, const XMLCh *value, const DocID &did);
	// Processing Instruction ctor
	DbXmlTextNode(const XMLCh *target,
		      const XMLCh *value,
		      const DynamicContext *context);
	DbXmlTextNode(const XMLCh *target,
		      const XMLCh *value,
		      const DocID &did);
	~DbXmlTextNode() {}

	virtual DocID getDocID() const { return did_; }
	virtual NodeInfo::Type getType() const;
	virtual void generateEvents(
		EventHandler *events,
		const DynamicContext *context,
		bool preserveNS = true,
		bool preserveType = true) const;
	virtual Result getAxisResult(XQStep::Axis axis,
				     const NodeTest *nodeTest,
				     const DynamicContext *context,
				     const LocationInfo *location) const;

	virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
	virtual const XMLCh* dmStringValue(const DynamicContext* context) const;

	virtual Node::Ptr dmParent(const DynamicContext* context) const
		{ return 0; }

	virtual short getNodeType() const { return type_; }
	virtual const XMLCh *getPITarget() const { return target_; }
	virtual const XMLCh *getValue() const { return value_; }
private:
	short type_;
	NsString value_;
	NsString target_;
	const DocID did_;
};

class DbXmlAxis : public ResultImpl
{
public:
	DbXmlAxis(const LocationInfo *location,
		  const DbXmlNodeImpl *contextNode,
		  const DbXmlNodeTest *nodeTest);
	Item::Ptr next(DynamicContext *context);

	virtual NsDomNodeRef nextNode(DynamicContext *context) = 0;

protected:
	DbXmlNodeImpl::Ptr nodeObj_;

	NsDomNodeRef contextNode_;
	const DbXmlNodeTest *nodeTest_;
	bool toDo_;
};

class DbXmlAttributeAxis : public DbXmlAxis
{
public:
	DbXmlAttributeAxis(const LocationInfo *location,
			   const DbXmlNodeImpl *contextNode,
			   const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsNodeRef nodeRef_;
	int i_;
};

class DbXmlNamespaceAxis : public ResultImpl
{
public:
	DbXmlNamespaceAxis(const LocationInfo *location,
			   const DbXmlNodeImpl *contextNode,
			   const DbXmlNodeTest *nodeTest);
	Item::Ptr next(DynamicContext *context);
	DbXmlNamespaceNode::Ptr nextNode(DynamicContext *context);
private:
	DbXmlNodeImpl::Ptr nodeObj_;

	NsDomNodeRef contextNode_;
	const DbXmlNodeTest *nodeTest_;

	NsDomNodeRef node_; // element
	NsNodeRef nodeRef_;
	int i_;

	enum {
		CHECK_ELEMENT,
		CHECK_ATTR,
		DO_XML,
		DONE
	} state_;

	DoneSet done_;
};

class DbXmlAttributeOrChildAxis : public DbXmlAxis
{
public:
	DbXmlAttributeOrChildAxis(const LocationInfo *location,
				  const DbXmlNodeImpl *contextNode,
				  const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsNodeRef nodeRef_;
	int i_;
	NsDomNodeRef child_;
};

class DbXmlChildAxis : public DbXmlAxis
{
public:
	DbXmlChildAxis(const LocationInfo *location,
		       const DbXmlNodeImpl *contextNode,
		       const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef child_;
};

class ElementChildAxis : public DbXmlAxis
{
public:
	ElementChildAxis(const LocationInfo *location,
			 const DbXmlNodeImpl *contextNode,
			 const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef child_; // element
};

class DbXmlDescendantAxis : public DbXmlAxis
{
public:
	DbXmlDescendantAxis(const LocationInfo *location,
			    const DbXmlNodeImpl *contextNode,
			    const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef descendant_;
};

class ElementDescendantAxis : public DbXmlAxis
{
public:
	ElementDescendantAxis(const LocationInfo *location,
			      const DbXmlNodeImpl *contextNode,
			      const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef nscontext_; // element
	NsDomNodeRef descendant_; // element
};

class DbXmlDescendantOrSelfAxis : public DbXmlAxis
{
public:
	DbXmlDescendantOrSelfAxis(const LocationInfo *location,
				  const DbXmlNodeImpl *contextNode,
				  const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef descendant_;
};

class ElementDescendantOrSelfAxis : public DbXmlAxis
{
public:
	ElementDescendantOrSelfAxis(const LocationInfo *location,
				    const DbXmlNodeImpl *contextNode,
				    const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef nscontext_; // element
	NsDomNodeRef descendant_; // element
};

class DbXmlParentAxis : public DbXmlAxis
{
public:
	DbXmlParentAxis(const LocationInfo *location,
			const DbXmlNodeImpl *contextNode,
			const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
};

class DbXmlAncestorAxis : public DbXmlAxis
{
public:
	DbXmlAncestorAxis(const LocationInfo *location,
			  const DbXmlNodeImpl *contextNode,
			  const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef ancestor_;
};

class DbXmlAncestorOrSelfAxis : public DbXmlAxis
{
public:
	DbXmlAncestorOrSelfAxis(const LocationInfo *location,
				const DbXmlNodeImpl *contextNode,
				const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef ancestor_;
};

class DbXmlFollowingAxis : public DbXmlAxis
{
public:
	DbXmlFollowingAxis(const LocationInfo *location,
			   const DbXmlNodeImpl *contextNode,
			   const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef node_;
};

class DbXmlFollowingSiblingAxis : public DbXmlAxis
{
public:
	DbXmlFollowingSiblingAxis(const LocationInfo *location,
				  const DbXmlNodeImpl *contextNode,
				  const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef node_;
};

class DbXmlPrecedingAxis : public DbXmlAxis
{
public:
	DbXmlPrecedingAxis(const LocationInfo *location,
			   const DbXmlNodeImpl *contextNode,
			   const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef node_;
	NsDomNodeRef parent_;
};

class DbXmlPrecedingSiblingAxis : public DbXmlAxis
{
public:
	DbXmlPrecedingSiblingAxis(const LocationInfo *location,
				  const DbXmlNodeImpl *contextNode,
				  const DbXmlNodeTest *nodeTest);
	NsDomNodeRef nextNode(DynamicContext *context);
private:
	NsDomNodeRef sibling_;
};

}

#endif
