//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NodeIterator.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlFactoryImpl.hpp"
#include "../nodeStore/NsNode.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

NodeIterator::NodeIterator(const LocationInfo *location)
	: location_(location)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IndexEntryIterator::IndexEntryIterator(ContainerBase *cont, const LocationInfo *location)
	: NodeIterator(location),
	  cont_(cont),
	  ie_(new IndexEntry)
{
}

NodeIterator::Type IndexEntryIterator::getType() const
{
	if(ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX)) {
		return ATTRIBUTE;
	} else if(ie_->isSpecified(IndexEntry::TEXT_INDEX)) {
		DBXML_ASSERT(false);
// 		return TEXT;
	} else if(ie_->isSpecified(IndexEntry::COMMENT_INDEX)) {
		DBXML_ASSERT(false);
// 		return COMMENT;
	} else if(ie_->isSpecified(IndexEntry::PI_INDEX)) {
		DBXML_ASSERT(false);
// 		return PROCESSING_INSTRUCTION;
	} else if(ie_->isSpecified(IndexEntry::NODE_ID)) {
		return ELEMENT;
	}

	return DOCUMENT;
}


int IndexEntryIterator::getContainerID() const
{
	return cont_->getContainerID();
}

DocID IndexEntryIterator::getDocID() const
{
	return ie_->getDocID();
}


const NsNid IndexEntryIterator::getNodeID() const
{
	if(ie_->isSpecified(IndexEntry::NODE_ID))
		return ie_->getNodeID();
	return *NsNid::getRootNid();
}

const NsNid IndexEntryIterator::getLastDescendantID() const
{
	return ie_->getLastDescendant();
}

u_int32_t IndexEntryIterator::getNodeLevel() const
{
	return ie_->getNodeLevel();
}

u_int32_t IndexEntryIterator::getIndex() const
{
	return ie_->getIndex();
}


bool IndexEntryIterator::isLeadingText() const
{
	return false;
}

DbXmlNodeImpl::Ptr IndexEntryIterator::asDbXmlNode(DynamicContext *context)
{
	// allow subclasses to assign memory for current item
	// to the IndexEntry
	adoptMemory(); 
	DbXmlNodeImpl::Ptr result =
		(DbXmlNodeImpl*)((DbXmlFactoryImpl*)context->
				 getItemFactory())->createNode(ie_, cont_, context, false).get();

	// Make sure the IndexEntry we gave to the DbXmlNodeImpl doesn't
	// get reset
	ie_ = new IndexEntry;

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ProxyIterator::ProxyIterator(const LocationInfo *location)
	: NodeIterator(location),
	  result_(0)
{
}

NodeIterator::Type ProxyIterator::getType() const
{
	return result_->getType();
}


int ProxyIterator::getContainerID() const
{
	return result_->getContainerID();
}

DocID ProxyIterator::getDocID() const
{
	return result_->getDocID();
}


const NsNid ProxyIterator::getNodeID() const
{
	return result_->getNodeID();
}

const NsNid ProxyIterator::getLastDescendantID() const
{
	return result_->getLastDescendantID();
}

u_int32_t ProxyIterator::getNodeLevel() const
{
	return result_->getNodeLevel();
}

u_int32_t ProxyIterator::getIndex() const
{
	return result_->getIndex();
}

bool ProxyIterator::isLeadingText() const
{
	return result_->isLeadingText();
}

DbXmlNodeImpl::Ptr ProxyIterator::asDbXmlNode(DynamicContext *context)
{
	return result_->asDbXmlNode(context);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

NameFilter::NameFilter(NamedNodeIterator *arg, int32_t uriIndex, const xmlbyte_t *name, const LocationInfo *location)
	: ProxyIterator(location),
	  arg_(arg),
	  uriIndex_(uriIndex),
	  name_(name)
{
	result_ = arg;
}

NameFilter::~NameFilter()
{
	delete arg_;
}

bool NameFilter::next(DynamicContext *context)
{
	while(arg_->next(context)) {
		if((uriIndex_ == 0 || arg_->getNodeURIIndex() == uriIndex_) &&
			(name_ == 0 || NsUtil::nsStringEqual(arg_->getNodeName(), name_)))
			return true;
	}
	return false;
}

bool NameFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!arg_->seek(container, did, nid, context)) return false;
	while((uriIndex_ != 0 && arg_->getNodeURIIndex() != uriIndex_) ||
		(name_ != 0 && !NsUtil::nsStringEqual(arg_->getNodeName(), name_))) {
		if(!arg_->next(context)) return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NsNodeIterator::NsNodeIterator(ContainerBase *cont, const LocationInfo *location)
	: NamedNodeIterator(location),
	  container_(cont)
{
	if(cont != 0) rawNode_.setContainerID(cont->getContainerID());
}

NsNodeIterator::~NsNodeIterator()
{
}

int NsNodeIterator::getContainerID() const
{
	return container_->getContainerID();
}

DocID NsNodeIterator::getDocID() const
{
	return rawNode_.getDocID();
}

const NsNid NsNodeIterator::getNodeID() const
{
	return rawNode_.getNodeID();
}

const NsNid NsNodeIterator::getLastDescendantID() const
{
	return rawNode_.getLastDescendantID();
}

u_int32_t NsNodeIterator::getNodeLevel() const
{
	return rawNode_.getNodeLevel();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NodeInfo::Type DbXmlNodeIterator::getType() const
{
	return node_->getType();
}

int DbXmlNodeIterator::getContainerID() const
{
	return node_->getContainerID();
}

DocID DbXmlNodeIterator::getDocID() const
{
	return node_->getDocID();
}

const NsNid DbXmlNodeIterator::getNodeID() const
{
	return node_->getNodeID();
}

const NsNid DbXmlNodeIterator::getLastDescendantID() const
{
	return node_->getLastDescendantID();
}

u_int32_t DbXmlNodeIterator::getNodeLevel() const
{
	return node_->getNodeLevel();
}

u_int32_t DbXmlNodeIterator::getIndex() const
{
	return node_->getIndex();
}

bool DbXmlNodeIterator::isLeadingText() const
{
	return node_->isLeadingText();
}

DbXmlNodeImpl::Ptr DbXmlNodeIterator::asDbXmlNode(DynamicContext *context)
{
	return node_;
}

