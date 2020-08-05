//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NodeInfo.hpp"
#include "../DocID.hpp"
#include "../nodeStore/NsNid.hpp"
#include <iostream>

using namespace DbXml;
using namespace std;

NodeInfo::~NodeInfo()
{
}

/* static */
int NodeInfo::isSameDocument(const NodeInfo *a, const NodeInfo *b)
{
	int aContainer = a->getContainerID();
	int bContainer = b->getContainerID();
	if(aContainer < bContainer) return -1;
	if(aContainer > bContainer) return +1;

	DocID aDocID = a->getDocID();
	DocID bDocID = b->getDocID();
	if(aDocID < bDocID) return -1;
	if(aDocID > bDocID) return +1;
	return 0;
}

/* static */
int NodeInfo::isSameDocument(const NodeInfo *a, int bContainer, const DocID &bDocID)
{
	int aContainer = a->getContainerID();
	if(aContainer < bContainer) return -1;
	if(aContainer > bContainer) return +1;

	DocID aDocID = a->getDocID();
	if(aDocID < bDocID) return -1;
	if(aDocID > bDocID) return +1;
	return 0;
}

/* static */
int NodeInfo::isSameNID(const NodeInfo *a, const NodeInfo *b)
{
	int aContainer = a->getContainerID();
	int bContainer = b->getContainerID();
	if(aContainer < bContainer) return -1;
	if(aContainer > bContainer) return +1;

	DocID aDocID = a->getDocID();
	DocID bDocID = b->getDocID();
	if(aDocID < bDocID) return -1;
	if(aDocID > bDocID) return +1;

	// Compare starting NID
	return a->getNodeID().compareNids(b->getNodeID());
}

/* static */
int NodeInfo::isSameNID(const NodeInfo *a, int bContainer, const DocID &bDocID, const NsNid &bNid)
{
	int aContainer = a->getContainerID();
	if(aContainer < bContainer) return -1;
	if(aContainer > bContainer) return +1;

	DocID aDocID = a->getDocID();
	if(aDocID < bDocID) return -1;
	if(aDocID > bDocID) return +1;

	// Compare starting NID
	return a->getNodeID().compareNids(bNid);
}

/* static */
int NodeInfo::isDescendantOf(const NodeInfo *descendant,
	const NodeInfo *ancestor, bool orSelf)
{
	int descContainer = descendant->getContainerID();
	int anContainer = ancestor->getContainerID();
	if(descContainer < anContainer) return -1;
	if(descContainer > anContainer) return +1;

	DocID descDocID = descendant->getDocID();
	DocID anDocID = ancestor->getDocID();
	if(descDocID < anDocID) return -1;
	if(descDocID > anDocID) return +1;

	// Every node is a decendant of the document node
	if(descendant->getType() == DOCUMENT) {
		return (orSelf && ancestor->getType() == DOCUMENT) ? 0 : -1;
	}
	if(ancestor->getType() == DOCUMENT) {
		return 0;
	}

	const NsNid descNID = descendant->getNodeID();
	const NsNid ancNID = ancestor->getNodeID();

	// Compare starting NID
	int cmp = descNID.compareNids(ancNID);

	switch(ancestor->getType()) {
	case DOCUMENT: // Dealt with above
	case ELEMENT: {
		switch(descendant->getType()) {
		case DOCUMENT: // Dealt with above
		case ELEMENT: {
			if(cmp < 0) return -1;
			if(cmp == 0) return orSelf ? 0 : -1;
			break;
		}
		case ATTRIBUTE: {
			if(cmp <= 0) return cmp;
			break;
		}
		default: {
			if(cmp < 0) {
				if(descendant->isLeadingText()) return -1;

				const NsNid ld = descendant->getLastDescendantID();
				if(ld.isNull()) return -1;

				// Compare ending NID
				return (ld.compareNids(ancNID) < 0) ? -1 : +1;
			}
			if(cmp == 0) return descendant->isLeadingText() ? -1 : 0;
			break;
		}
		}

		const NsNid ld = ancestor->getLastDescendantID();
		if(ld.isNull()) return +1;

		// Compare ending NID
		return (descNID.compareNids(ld) <= 0) ? 0 : +1;
	}
	case ATTRIBUTE: {
		switch(descendant->getType()) {
		case DOCUMENT: // Dealt with above
		case ELEMENT: {
			if(cmp == 0) return -1;
			return cmp;
		}
		case ATTRIBUTE: {
			if(cmp == 0) {
				cmp = (int)descendant->getIndex() - (int)ancestor->getIndex();
				if(cmp == 0) return orSelf ? 0 : -1;
			}
			return cmp;
		}
		default: {
			if(cmp < 0) {
				if(descendant->isLeadingText()) return -1;

				const NsNid ld = descendant->getLastDescendantID();
				if(ld.isNull()) return -1;

				// Compare ending NID
				return (ld.compareNids(ancNID) < 0) ? -1 : +1;
			}
			if(cmp == 0) return descendant->isLeadingText() ? -1 : +1;

			return +1;
		}
		}
	}
	default: {
		switch(descendant->getType()) {
		case DOCUMENT: // Dealt with above
		case ELEMENT:
		case ATTRIBUTE: {
			if(cmp < 0) return -1;
			if(cmp == 0) return ancestor->isLeadingText() ? +1 : -1;

			if(ancestor->isLeadingText()) return +1;

			const NsNid ld = ancestor->getLastDescendantID();
			if(ld.isNull()) return -1;

			// Compare ending NID
			return (descNID.compareNids(ld) <= 0) ? -1 : +1;
		}
		default: {
			if(cmp == 0) {
				cmp = (int)descendant->getIndex() - (int)ancestor->getIndex();
				if(cmp == 0) return orSelf ? 0 : -1;
				return cmp;
			}

			if(cmp > 0) {
				if(ancestor->isLeadingText()) return cmp;

				const NsNid ld = ancestor->getLastDescendantID();
				if(ld.isNull()) return cmp;

				return (descNID.compareNids(ld) <= 0) ? -1 : +1;
			} else {
				if(descendant->isLeadingText()) return cmp;

				const NsNid ld = descendant->getLastDescendantID();
				if(ld.isNull()) return cmp;

				return (ld.compareNids(ancNID) < 0) ? -1 : +1;
			}
		}
		}
	}
	}

	return 0;
}

static int compareElementAndText(int ret, const NodeInfo *element, const NodeInfo *text)
{
	if(ret == 0) return text->isLeadingText() ? +1 : -1;

	// Element is before text's owner
	if(ret < 0) return ret;

	// Text is leading text, and element is after text's owner
	if(text->isLeadingText()) return +1;

	const NsNid ld = text->getLastDescendantID();
	if(!ld.isNull()) ret = element->getNodeID().compareNids(ld);

	// Element is after text's owner's subtree
	if(ret > 0) return ret;

	// Element is in text's owner's subtree, so element must be smaller
	// than text
	return -1;
}

/* static */
int NodeInfo::compare(const NodeInfo *a, const NodeInfo *b)
{
	int aContainer = a->getContainerID();
	int bContainer = b->getContainerID();
	if(aContainer < bContainer) return -1;
	if(aContainer > bContainer) return +1;

	DocID aDocID = a->getDocID();
	DocID bDocID = b->getDocID();
	if(aDocID < bDocID) return -1;
	if(aDocID > bDocID) return +1;

	// Now we know it's in the same document
	Type aType = a->getType();
	Type bType = b->getType();

	// The document root is always less than any other nodes in the tree
	if(aType == DOCUMENT) {
		return bType == DOCUMENT ? 0 : -1;
	}
	if(bType == DOCUMENT) {
		return +1;
	}

	// compare node ids
	const NsNid aNid = a->getNodeID();
	const NsNid bNid = b->getNodeID();

// 	// Are the nodes stand alone constructed ones?
// 	// TBD Remove this when it is no longer necessary - jpcs
// 	if(aNid.isNull()) {
// 		return bNid.isNull() ? (long)(a - b) : +1;
// 	}
// 	if(bNid.isNull()) {
// 		return -1;
// 	}

	int ret = aNid.compareNids(bNid);

	switch(aType) {
	case DOCUMENT: break; // Dealt with above
	case ELEMENT:
		switch(bType) {
		case DOCUMENT: break; // Dealt with above
		case ELEMENT: return ret;
		case ATTRIBUTE: return ret == 0 ? -1 : ret;
		default: return compareElementAndText(ret, a, b);
		}
		break;
	case ATTRIBUTE:
		switch(bType) {
		case DOCUMENT: break; // Dealt with above
		case ELEMENT: return ret == 0 ? +1 : ret;
		case ATTRIBUTE: return ret == 0 ? (int)a->getIndex() - (int)b->getIndex() : ret;
		default: return compareElementAndText(ret, a, b);
		}
		break;
	default:
		switch(bType) {
		case ELEMENT:
		case ATTRIBUTE: return -compareElementAndText(-ret, b, a);
		default:
			if(ret == 0) return (int)a->getIndex() - (int)b->getIndex();

			if(ret > 0) {
				// B's owner is before A's owner
				if(b->isLeadingText()) return ret;

				// Is A in B's owner's subtree?
				const NsNid ld = b->getLastDescendantID();
				if(ld.isNull()) return ret;

				ret = aNid.compareNids(ld);
				return ret == 0 ? -1 : ret;
			} else {
				// A's owner is before B's owner
				if(a->isLeadingText()) return ret;

				// Is B in A's owner's subtree?
				const NsNid ld = a->getLastDescendantID();
				if(ld.isNull()) return ret;

				ret = ld.compareNids(bNid);
				return ret == 0 ? +1 : ret;
			}
		}
		break;
	}

	return 0;
}

/* static */
void NodeInfo::display(const NodeInfo *a, std::ostream &out)
{
	if(a == 0) {
		out << "null";
		return;
	}

	switch(a->getType()) {
	case DOCUMENT: out << "document("; break;
	case ELEMENT: out << "element("; break;
	case ATTRIBUTE: out << "attribute("; break;
	case TEXT: out << "text("; break;
	case COMMENT: out << "comment("; break;
	case PI: out << "pi("; break;
	}

	out << a->getContainerID() << ",";
	out << a->getDocID().asString().c_str();

	if(a->getType() != DOCUMENT) {
		out << ",";
		a->getNodeID().displayNid(out); out << ",";
		a->getLastDescendantID().displayNid(out); out << ",";
		out << a->getNodeLevel();
	}

	switch(a->getType()) {
	case ATTRIBUTE:
		out << "," << a->getIndex();
		break;
	case TEXT:
	case COMMENT:
	case PI:
		out << "," << a->getIndex() << (a->isLeadingText() ? ",leading" : "");
		break;
	default: break;
	}

	out << ")";
}
