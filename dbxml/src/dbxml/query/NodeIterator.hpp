//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NODEITERATOR_HPP
#define	__NODEITERATOR_HPP

#include "NodeInfo.hpp"
#include "../IndexEntry.hpp"
#include "../dataItem/DbXmlNodeImpl.hpp"
#include "../nodeStore/NsRawNode.hpp"

class DynamicContext;
class LocationInfo;

namespace DbXml
{

class ContainerBase;
	
class NodeIterator : public NodeInfo
{
public:
	NodeIterator(const LocationInfo *location);

	const LocationInfo *getLocation() const { return location_; }

	virtual bool next(DynamicContext *context) = 0;
	virtual bool seek(int containerID, const DocID &did, const NsNid &nid, DynamicContext *context) = 0;
	bool seek(const NodeIterator *toSeek, DynamicContext *context)
	{
		return seek(toSeek->getContainerID(), toSeek->getDocID(), toSeek->getNodeID(), context);
	}

	/**
	 * To be used only when sending nodes back into XQilla code.
	 * Calling this method means that you cannot call any of the
	 * other methods until you call next() or seek().
	 */
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context) = 0;

protected:
	const LocationInfo *location_;
};

/**
 * Utility base class that works by getting derived classes
 * to fill in it's IndexEntry.
 */
class IndexEntryIterator : public NodeIterator
{
public:
	IndexEntryIterator(ContainerBase *cont, const LocationInfo *location);

	virtual Type getType() const;

	virtual int getContainerID() const;
	virtual DocID getDocID() const;

	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;

	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);

	// introduced in this class -- asks implementation to ensure
	// the IndexEntry owns memory referenced by ids
	virtual void adoptMemory() {}
protected:
	ContainerBase *cont_;
	IndexEntry::Ptr ie_;
};

/**
 * Utility base class that works by proxying informational calls
 * to a predetermined other NodeIterator
 */
class ProxyIterator : public NodeIterator
{
public:
	ProxyIterator(const LocationInfo *location);

	virtual Type getType() const;

	virtual int getContainerID() const;
	virtual DocID getDocID() const;

	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;

	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);

protected:
	NodeIterator *result_;
};

/**
 * A NodeIterator that provides access to the node's name
 */
class NamedNodeIterator : public NodeIterator
{
public:
	NamedNodeIterator(const LocationInfo *location)
		: NodeIterator(location) {}

	virtual int32_t getNodeURIIndex() = 0;
	virtual const xmlbyte_t *getNodeName() = 0;
};

/**
 * Filters a NamedNodeIterator for nodes with a specific name
 */
class NameFilter : public ProxyIterator
{
public:
	NameFilter(NamedNodeIterator *arg, int32_t uriIndex, const xmlbyte_t *name, const LocationInfo *location);
	~NameFilter();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int containerID, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	NamedNodeIterator *arg_;
	int32_t uriIndex_;
	const xmlbyte_t *name_;
};

/**
 * Utility base class that works by getting the derived class
 * to fill in it's DocID and NsNode member variables.
 */
class NsNodeIterator : public NamedNodeIterator
{
public:
	NsNodeIterator(ContainerBase *cont, const LocationInfo *location);
	~NsNodeIterator();

	virtual int getContainerID() const;
	virtual DocID getDocID() const;

	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;

protected:
	ContainerBase *container_;
	mutable NsRawNode rawNode_;
};

/**
 * Utility base class that works by getting derived classes
 * to fill in it's DbXmlNodeImpl.
 */
class DbXmlNodeIterator : public NodeIterator
{
public:
	DbXmlNodeIterator(const LocationInfo *location)
		: NodeIterator(location) {}

	virtual Type getType() const;

	virtual int getContainerID() const;
	virtual DocID getDocID() const;

	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;

	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);

protected:
	DbXmlNodeImpl::Ptr node_;
};

}

#endif
