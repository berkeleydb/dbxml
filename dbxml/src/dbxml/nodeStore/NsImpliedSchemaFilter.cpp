//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NsImpliedSchemaFilter.hpp"
#include "NsXercesTranscoder.hpp"

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/UTF8Str.hpp>

using namespace DbXml;
using namespace std;
XERCES_CPP_NAMESPACE_USE;

NsImpliedSchemaFilter::NsImpliedSchemaFilter(const ISNVector &isns, NsEventHandler16 *next)
	: next_(next),
	  nextT_(0),
	  handlerBase_(0)
{
	stack_.push_back(new StackEntry());
	stack_.back()->matched = true;
	stack_.back()->depthAdded = true;

	ISNVector::const_iterator i = isns.begin();
	for(; i != isns.end(); ++i) {
		stack_.back()->addNode(*i, 0);
	}
}

NsImpliedSchemaFilter::NsImpliedSchemaFilter(const ISNVector &isns, NsEventTranslator *next)
	: next_(next),
	  nextT_(next),
	  handlerBase_(0)
{
	stack_.push_back(new StackEntry());
	stack_.back()->matched = true;
	stack_.back()->depthAdded = true;

	ISNVector::const_iterator i = isns.begin();
	for(; i != isns.end(); ++i) {
		stack_.back()->addNode(*i, 0);
	}
}

NsImpliedSchemaFilter::NsImpliedSchemaFilter(const ISNVector &isns, NsXercesTranscoder *next)
	: next_(next),
	  nextT_(next),
	  handlerBase_(next)
{
	stack_.push_back(new StackEntry());
	stack_.back()->matched = true;
	stack_.back()->depthAdded = true;

	ISNVector::const_iterator i = isns.begin();
	for(; i != isns.end(); ++i) {
		stack_.back()->addNode(*i, 0);
	}
}

NsImpliedSchemaFilter::~NsImpliedSchemaFilter()
{
	for(FilterStack::iterator i = stack_.begin(); i != stack_.end(); ++i)
		delete *i;

	delete next_;
}

void NsImpliedSchemaFilter::startDocument(const xmlch_t *sniffedEncoding)
{
	StackEntry *entry = stack_.back();

	StackEntry *newEntry = new StackEntry();
	stack_.push_back(newEntry);
	for(ISNVector::iterator i = entry->children.begin(); i != entry->children.end(); ++i) {
		switch((*i)->getType()) {
		case ImpliedSchemaNode::ROOT:
			newEntry->matched = true;
			newEntry->addChildren(*i, 0);
			break;
		case ImpliedSchemaNode::DESCENDANT_ATTR:
			newEntry->addNode(*i, 0);
			break;
		case ImpliedSchemaNode::DESCENDANT:
			newEntry->addNode(*i, 0);
			// Fall through
		case ImpliedSchemaNode::CHILD:
			if((*i)->isWildcardNodeType()) {
				newEntry->matched = true;
				newEntry->addChildren(*i, 0);
			}
			break;
		case ImpliedSchemaNode::ATTRIBUTE:
		default: break;
		}
	}

	// Always output the document node
	newEntry->matched = true;
	newEntry->depthAdded = true;
	next_->startDocument(sniffedEncoding);
}

void NsImpliedSchemaFilter::xmlDecl(const xmlch_t *xmlDecl,
	const xmlch_t *encodingStr,
	const xmlch_t *standaloneStr)
{
	if(stack_.back()->matched) next_->xmlDecl(xmlDecl, encodingStr, standaloneStr);
}

void NsImpliedSchemaFilter::docTypeDecl(const xmlch_t *data, size_t len)
{
	if(stack_.back()->matched) next_->docTypeDecl(data, len);
}

void NsImpliedSchemaFilter::endDocument()
{
	StackEntry *entry = stack_.back();

	if(entry->matched)
		next_->endDocument();

	delete entry;
	stack_.pop_back();
}

static inline const XMLCh *emptyToNull(const XMLCh *s)
{
	return (s == 0 || *s == 0) ? 0 : s;
}

void NsImpliedSchemaFilter::updateNodeIDs(FilterStack::reverse_iterator p)
{
	DBXML_ASSERT(handlerBase_);

	// Keep track of node IDs to make sure they are the same as the non-projected document
	for(++p; p != stack_.rend(); ++p) {
		while((*p)->nodeIDCount > 0) {
			handlerBase_->nextId(0);
			--((*p)->nodeIDCount);
		}
		if(!(*p)->depthAdded) {
			handlerBase_->incDepth();
			(*p)->depthAdded = true;
		}
	}
}

void NsImpliedSchemaFilter::checkAncestors(FilterStack::reverse_iterator s)
{
	// The actual parent, if we weren't projecting
	StackEntry *parent = *s;

	// Look for the node this element would be a child of if it were output now
	// (The last ancestor that was matched)
	while(s != stack_.rend() && !(*s)->matched && (*s)->nodeIDCount != 0) ++s;

	DBXML_ASSERT(s != stack_.rend());

	// Is it our real parent?
	while(*s != parent) {
		// Search to see if this pseudo-parent is expecting actual children (not descendants)
		// that match any of the nodes we're looking for right now
		bool found = false;
		ISNVector::iterator i = (*s)->children.begin();
		for(; !found && i != (*s)->children.end(); ++i) {

			if((*i)->getType() == QueryPathNode::CHILD) {

				ISNVector::iterator j = parent->children.begin();
				for(; !found && j != parent->children.end(); ++j) {
					switch((*j)->getType()) {
					case ImpliedSchemaNode::DESCENDANT_ATTR:
						// Could result in the materialization of any element
						found = true;
						break;
					case ImpliedSchemaNode::DESCENDANT:
					case ImpliedSchemaNode::CHILD:
						if(((*i)->isWildcardName() || (*j)->isWildcardName() ||
							   XPath2Utils::equals((*i)->getName(), (*j)->getName())) &&
							((*i)->isWildcardURI() || (*j)->isWildcardURI() ||
								XPath2Utils::equals((*i)->getURI(), (*j)->getURI()))) {
							found = true;
						}
						break;
					case ImpliedSchemaNode::ROOT:
					case ImpliedSchemaNode::ATTRIBUTE:
					default: break;
					}
				}
			}
		}

		if(!found) break;

		// Our pseudo-parent is expecting children that could match nodes in this part
		// of it's sub-tree - better output the next intermediate ancestor to disambiguate
		--s;

		if(handlerBase_) updateNodeIDs(s);

		(*s)->matched = true;
		(*s)->nodeIDCount -= 1;
		(*s)->depthAdded = true;
		next_->startElement((*s)->localName.getRawBuffer(), emptyToNull((*s)->prefix.getRawBuffer()),
			emptyToNull((*s)->uri.getRawBuffer()), 0, 0, /*isEmpty*/false);
	}
}

void NsImpliedSchemaFilter::startElement(const xmlch_t *localName,
	const xmlch_t *prefix,
	const xmlch_t *uri,
	NsEventAttrList16 *attrs,
	const uint32_t attrCount,
	bool isEmpty)
{
	StackEntry *entry = stack_.back();

	bool attrsUsed = false;
	StackEntry *newEntry = new StackEntry();
	stack_.push_back(newEntry);

	ISNVector::iterator i = entry->children.begin();
	for(; i != entry->children.end(); ++i) {
		switch((*i)->getType()) {
		case ImpliedSchemaNode::DESCENDANT_ATTR:
			attrsUsed = newEntry->addNode(*i, attrs) || attrsUsed;
			break;
		case ImpliedSchemaNode::DESCENDANT:
			attrsUsed = newEntry->addNode(*i, attrs) || attrsUsed;
			// Fall through
		case ImpliedSchemaNode::CHILD:
			if(((*i)->isWildcardName() || XPath2Utils::equals((*i)->getName(), localName)) &&
				((*i)->isWildcardURI() || XPath2Utils::equals((*i)->getURI(), uri))) {
				newEntry->matched = true;
				attrsUsed = newEntry->addChildren(*i, attrs) || attrsUsed;
			}
			break;
		case ImpliedSchemaNode::ROOT:
		case ImpliedSchemaNode::ATTRIBUTE:
		default: break;
		}
	}

	// Check for namespace declarations, and output the attributes
	// if there are any
	if(!attrsUsed) {
		for(int i = 0; i < attrs->numAttributes(); ++i) {
			if(XPath2Utils::equals(attrs->uri(i), XMLUni::fgXMLNSURIName)) {
				attrsUsed = true;
				break;
			}
		}
	}

	if(attrsUsed) newEntry->matched = true; // Because attrs need an element to belong to

	if(newEntry->matched) {
		FilterStack::reverse_iterator s = stack_.rbegin();
		++s;
		DBXML_ASSERT(*s == entry);
		checkAncestors(s);

		if(handlerBase_) updateNodeIDs(stack_.rbegin());

		newEntry->depthAdded = true;

		if(attrsUsed) next_->startElement(localName, prefix, uri, attrs, attrCount, isEmpty);
		else next_->startElement(localName, prefix, uri, 0, 0, isEmpty);
	} else {
		newEntry->localName.set(localName);
		newEntry->prefix.set(prefix);
		newEntry->uri.set(uri);

		// Set the count of node IDs that haven't been output to 1, because we
		// haven't output this node
		newEntry->nodeIDCount = 1;
		newEntry->depthAdded = false;
	}

	if(isEmpty) {
		stack_.pop_back();

		if(newEntry->nodeIDCount != 0) {
			// Add the count of unoutput node IDs to our parent's count
			stack_.back()->nodeIDCount += newEntry->nodeIDCount;
		}

		delete newEntry;
	}
}

void NsImpliedSchemaFilter::endElement(const xmlch_t *localName,
	const xmlch_t *prefix,
	const xmlch_t *uri)
{
	StackEntry *entry = stack_.back();

	if(entry->matched) {
		if(handlerBase_) {
			while(entry->nodeIDCount > 0) {
				handlerBase_->nextId(0);
				--(entry->nodeIDCount);
			}
		}
		next_->endElement(localName, prefix, uri);
	} else if(entry->depthAdded) {
		if(handlerBase_) handlerBase_->decDepth();
	}

	stack_.pop_back();
	
	if(entry->nodeIDCount != 0) {
		// Add the count of unoutput node IDs to our parent's count
		stack_.back()->nodeIDCount += entry->nodeIDCount;
	}

	delete entry;
}

void NsImpliedSchemaFilter::characters(const xmlch_t *characters,
	size_t len, bool isCDATA,
	bool needsEscape)
{
	if(stack_.back()->nonElementChildren) {
		checkAncestors(stack_.rbegin());
		next_->characters(characters, len, isCDATA, needsEscape);
	}
}

void NsImpliedSchemaFilter::ignorableWhitespace(const xmlch_t *characters,
	size_t len, bool isCDATA)
{
	if(stack_.back()->nonElementChildren) {
		checkAncestors(stack_.rbegin());
		next_->ignorableWhitespace(characters, len, isCDATA);
	}
}

void NsImpliedSchemaFilter::comment(const xmlch_t *comment, size_t len)
{
	if(stack_.back()->nonElementChildren) {
		checkAncestors(stack_.rbegin());
		next_->comment(comment, len);
	}
}

void NsImpliedSchemaFilter::processingInstruction(const xmlch_t *target,
	const xmlch_t *data)
{
	if(stack_.back()->nonElementChildren) {
		checkAncestors(stack_.rbegin());
		next_->processingInstruction(target, data);
	}
}

bool NsImpliedSchemaFilter::StackEntry::addNode(const ImpliedSchemaNode *isn, NsEventAttrList16 *attrs)
{
	switch(isn->getType()) {
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::CHILD:
		if(isn->isWildcardNodeType())
			nonElementChildren = true;
		// Fall through
	case ImpliedSchemaNode::ROOT:
		children.push_back(isn);
		break;
	case ImpliedSchemaNode::DESCENDANT_ATTR:
		children.push_back(isn);
		// Fall through
	case ImpliedSchemaNode::ATTRIBUTE:
        {
		if(attrs == 0) return false;

		for(int i = 0; i < attrs->numAttributes(); ++i) {
			if((isn->isWildcardName() || XPath2Utils::equals(isn->getName(), attrs->localName(i))) &&
				(isn->isWildcardURI() || XPath2Utils::equals(isn->getURI(), attrs->uri(i)))) {
				return true;
			}
		}
		return false;
        }
	default: break;
	}

	return false;
}

bool NsImpliedSchemaFilter::StackEntry::addChildren(const ImpliedSchemaNode *isn, NsEventAttrList16 *attrs)
{
	bool attrChildren = false;

	const ImpliedSchemaNode *child = (ImpliedSchemaNode*)isn->getFirstChild();
	while(child != 0) {
		attrChildren = addNode(child, attrs) || attrChildren;
		child = (ImpliedSchemaNode*)child->getNextSibling();
	}

	return attrChildren;
}

EventWriter *NsImpliedSchemaFilter::getEventWriter() const
{
	DBXML_ASSERT(nextT_ != 0);
	return nextT_->getEventWriter();
}

void NsImpliedSchemaFilter::setEventWriter(EventWriter *writer)
{
	DBXML_ASSERT(nextT_ != 0);
	nextT_->setEventWriter(writer);
}

