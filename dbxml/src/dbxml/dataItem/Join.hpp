//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __JOIN_HPP_78390
#define	__JOIN_HPP_78390

#include <xqilla/ast/XQStep.hpp>

namespace DbXml
{

class Join
{
public:
	enum Type {
		ANCESTOR = XQStep::ANCESTOR,
		ANCESTOR_OR_SELF = XQStep::ANCESTOR_OR_SELF,
		ATTRIBUTE = XQStep::ATTRIBUTE,
		CHILD = XQStep::CHILD,
		DESCENDANT = XQStep::DESCENDANT,
		DESCENDANT_OR_SELF = XQStep::DESCENDANT_OR_SELF,
		FOLLOWING = XQStep::FOLLOWING,
		FOLLOWING_SIBLING = XQStep::FOLLOWING_SIBLING,
		NAMESPACE = XQStep::NAMESPACE,
		PARENT = XQStep::PARENT,
		PRECEDING = XQStep::PRECEDING,
		PRECEDING_SIBLING = XQStep::PRECEDING_SIBLING,
		SELF = XQStep::SELF,

		PARENT_A, // Parent of an attribute
		PARENT_C, // Parent of a child (not an attribute)
		ATTRIBUTE_OR_CHILD, // Attributes and children of a node

		NONE // No join
	};

	static unsigned int getJoinTypeProperties(Type join);
	static Type inverse(Type join);
};

}

#endif
