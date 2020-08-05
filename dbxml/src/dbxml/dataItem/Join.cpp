//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "Join.hpp"
#include "DbXmlNodeImpl.hpp"

#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

unsigned int Join::getJoinTypeProperties(Type join)
{
	unsigned int properties = 0;
	// properties depend on the axis of the step
	switch (join) {
	case SELF:
		properties |= StaticAnalysis::ONENODE | StaticAnalysis::SELF;
		// Fall through
	case ATTRIBUTE_OR_CHILD:
	case CHILD:
	case ATTRIBUTE:
	case NAMESPACE:
		properties |= StaticAnalysis::SUBTREE | StaticAnalysis::PEER |
			StaticAnalysis::DOCORDER;
		break;
	case DESCENDANT:
	case DESCENDANT_OR_SELF:
		properties |= StaticAnalysis::SUBTREE | StaticAnalysis::DOCORDER;
		break;
	case FOLLOWING_SIBLING:
		properties |= StaticAnalysis::DOCORDER;
		// Fall through
	case PRECEDING_SIBLING:
		properties |= StaticAnalysis::PEER;
		break;
	case PARENT_A:
	case PARENT_C:
	case PARENT:
		properties |= StaticAnalysis::PEER | StaticAnalysis::ONENODE |
			StaticAnalysis::DOCORDER;
		break;
	case FOLLOWING:
		properties |= StaticAnalysis::DOCORDER;
		break;
	case ANCESTOR:
	case ANCESTOR_OR_SELF:
	case PRECEDING:
	case NONE:
		break;
	}

	properties |= StaticAnalysis::GROUPED | StaticAnalysis::SAMEDOC;
	return properties;
}

Join::Type Join::inverse(Type join)
{
  switch(join) {
  case Join::ANCESTOR:           return Join::DESCENDANT;
  case Join::DESCENDANT:         return Join::ANCESTOR;

  case Join::ANCESTOR_OR_SELF:   return Join::DESCENDANT_OR_SELF;
  case Join::DESCENDANT_OR_SELF: return Join::ANCESTOR_OR_SELF;

  case Join::PARENT:             return Join::ATTRIBUTE_OR_CHILD;
  case Join::ATTRIBUTE_OR_CHILD: return Join::PARENT;

  case Join::PRECEDING:          return Join::FOLLOWING;
  case Join::FOLLOWING:          return Join::PRECEDING;

  case Join::PRECEDING_SIBLING:  return Join::FOLLOWING_SIBLING;
  case Join::FOLLOWING_SIBLING:  return Join::PRECEDING_SIBLING;

  case Join::CHILD:              return Join::PARENT_C;
  case Join::PARENT_C:           return Join::CHILD;

  case Join::ATTRIBUTE:          return Join::PARENT_A;
  case Join::PARENT_A:           return Join::ATTRIBUTE;

  case Join::SELF:               return Join::SELF;

  case Join::NONE:
  case Join::NAMESPACE:
	  break;
  }

  return Join::NONE;
}
