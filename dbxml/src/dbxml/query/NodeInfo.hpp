//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NODEINFO_HPP
#define	__NODEINFO_HPP

#include "../nodeStore/NsTypes.hpp"
#include "../nodeStore/NsNid.hpp"

namespace DbXml
{

class DocID;

class NodeInfo
{
public:
	enum Type {
		DOCUMENT,
		ELEMENT,
		ATTRIBUTE,
		TEXT,
		COMMENT,
		PI
	};

	virtual ~NodeInfo();

	virtual Type getType() const = 0;

	virtual int getContainerID() const = 0;
	virtual DocID getDocID() const = 0;

	virtual const NsNid getNodeID() const = 0;
	virtual const NsNid getLastDescendantID() const = 0;
	virtual u_int32_t getNodeLevel() const = 0;
	/// Only returns a sensible answer for ATTRIBUTE, TEXT, COMMENT and PI types
	virtual u_int32_t getIndex() const = 0;
	/// Only returns a sensible answer for TEXT, COMMENT and PI types
	virtual bool isLeadingText() const = 0;

	/// Checks that container and document ID are the same
	/// Returns 0 if equal, <0 if a is before b, >0 if a is after b
	static int isSameDocument(const NodeInfo *a, const NodeInfo *b);
	/// Checks that container and document ID are the same
	/// Returns 0 if equal, <0 if a is before b, >0 if a is after b
	static int isSameDocument(const NodeInfo *a, int containerID, const DocID &did);
	/// Checks that container, document ID and node ID are the same
	/// Returns 0 if equal, <0 if a is before b, >0 if a is after b
	static int isSameNID(const NodeInfo *a, const NodeInfo *b);
	/// Checks that container, document ID and node ID are the same
	/// Returns 0 if equal, <0 if a is before b, >0 if a is after b
	static int isSameNID(const NodeInfo *a, int containerID, const DocID &did, const NsNid &nid);
	/// Checks if the given node is a descendant of this node
	/// Returns 0 if it is, <0 if it is too small, >0 if it is too big
	static int isDescendantOf(const NodeInfo *descendant,
		const NodeInfo *ancestor, bool orSelf);
	/// Checks whether the NodeInfo objects point to the same node
	/// Returns 0 if they are the same, <0 if a comes before b, >0 if a comes after b
	static int compare(const NodeInfo *a, const NodeInfo *b);

	/// Displays the NodeInfo on the output stream given
	static void display(const NodeInfo *a, std::ostream &out);
};

}

#endif
