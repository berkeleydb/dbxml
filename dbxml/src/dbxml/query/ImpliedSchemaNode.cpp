//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ImpliedSchemaNode.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <sstream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

ImpliedSchemaNode::ImpliedSchemaNode(Type type, bool generalComp, Syntax::Type syntaxType, const ASTNode *value,
	MemoryManager *mm)
	: QueryPathNode(0, (QueryPathNode::Type)type, mm),
	  generalComp_(generalComp),
	  syntaxType_(syntaxType),
	  astnode_(value),
	  utf8URIName_(0),
	  qpr_(0)
{
}

ImpliedSchemaNode::ImpliedSchemaNode(const DbXmlNodeTest *nodeTest, Type type, MemoryManager *mm)
	: QueryPathNode(nodeTest, (QueryPathNode::Type)type, mm),
	  generalComp_(false),
	  syntaxType_(Syntax::NONE),
	  astnode_(0),
	  utf8URIName_(0),
	  qpr_(0)
{
}

ImpliedSchemaNode::ImpliedSchemaNode(const XQCastAs *cast, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
	: QueryPathNode(0, (QueryPathNode::Type)CAST, mm),
	  generalComp_(false),
	  syntaxType_(Syntax::NONE),
	  astnode_(cast),
	  utf8URIName_(0),
	  qpr_(0)
{
}

ImpliedSchemaNode::~ImpliedSchemaNode()
{
}

const XQCastAs *ImpliedSchemaNode::getCast() const
{
	return type_ == (QueryPathNode::Type)CAST ? (XQCastAs*)astnode_ : 0;
}

bool ImpliedSchemaNode::isWildcard() const
{
	if(nodeTest_ == 0 || nodeTest_->getItemType()) return true;

	return nodeTest_->getNamespaceWildcard() || nodeTest_->getNameWildcard() || nodeTest_->getTypeWildcard() ||
		(type_ != QueryPathNode::ATTRIBUTE && type_ != QueryPathNode::DESCENDANT_ATTR && type_ != (QueryPathNode::Type)METADATA &&
			nodeTest_->getNodeType() != Node::element_string);
}

bool ImpliedSchemaNode::isSuitableForIndex() const
{
	if(nodeTest_ == 0 || nodeTest_->getItemType()) return false;

	if(nodeTest_->getTypeWildcard() || (type_ != QueryPathNode::ATTRIBUTE && type_ != QueryPathNode::DESCENDANT_ATTR &&
		   nodeTest_->getNodeType() != Node::element_string)) return false;

	return true;
}

ImpliedSchemaNode *ImpliedSchemaNode::getBaseNode()
{
	switch((ImpliedSchemaNode::Type)type_) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::METADATA:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR: {
		return this;
	}
	case ImpliedSchemaNode::CAST:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::NOT_EQUALS:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::PREFIX:
	case ImpliedSchemaNode::SUFFIX:
	case ImpliedSchemaNode::SUBSTRING_CD:
	case ImpliedSchemaNode::SUBSTRING: {
		return ((ImpliedSchemaNode*)parent_)->getBaseNode();
	}
	}
	return 0;
}

void ImpliedSchemaNode::markSubtreeValue()
{
	if(type_ != QueryPathNode::ATTRIBUTE && type_ != QueryPathNode::DESCENDANT_ATTR) {
		appendChild(new (memMgr_) ImpliedSchemaNode(0, DESCENDANT, memMgr_));
	}
}

void ImpliedSchemaNode::markSubtreeResult()
{
	if(type_ != QueryPathNode::ATTRIBUTE && type_ != QueryPathNode::DESCENDANT_ATTR) {
		appendChild(new (memMgr_) ImpliedSchemaNode(0, DESCENDANT_ATTR, memMgr_));
		appendChild(new (memMgr_) ImpliedSchemaNode(0, DESCENDANT, memMgr_));
	}
}

bool ImpliedSchemaNode::equals(const QueryPathNode *oq) const
{
	ImpliedSchemaNode *o = (ImpliedSchemaNode*)oq;
	return type_ == o->type_ && NodeTest::isSubsetOf(nodeTest_, o->nodeTest_)
		&& NodeTest::isSubsetOf(o->nodeTest_, nodeTest_)
		&& generalComp_ == o->generalComp_
		&& syntaxType_ == o->syntaxType_
		&& astnode_ == o->astnode_;
}

bool ImpliedSchemaNode::isSubsetOf(const QueryPathNode *oq) const
{
	ImpliedSchemaNode *node = (ImpliedSchemaNode*)oq;
	return DbXmlNodeTest::isSubsetOf(nodeTest_, node->nodeTest_);
}

ImpliedSchemaNode *ImpliedSchemaNode::copy(MemoryManager *mm) const
{
	if(mm == 0) {
		mm = memMgr_;
	}

	ImpliedSchemaNode *result = new (mm) ImpliedSchemaNode((DbXmlNodeTest*)nodeTest_, (ImpliedSchemaNode::Type)type_, mm);

	ImpliedSchemaNode *child;
	for(child = (ImpliedSchemaNode*)firstChild_; child; child = (ImpliedSchemaNode*)child->nextSibling_) {
		result->appendChild(child->copy(mm));
	}

	return result;
}

static string getIndent(int level)
{
	ostringstream s;

	for(int i = level; i != 0; --i) {
		s << "  ";
	}

	return s.str();
}

static string typeToString(ImpliedSchemaNode::Type type) {
	switch(type) {
	case ImpliedSchemaNode::ATTRIBUTE: {
		return "attribute";
		break;
	}
	case ImpliedSchemaNode::CHILD: {
		return "child";
		break;
	}
	case ImpliedSchemaNode::DESCENDANT: {
		return "descendant";
		break;
	}
	case ImpliedSchemaNode::DESCENDANT_ATTR: {
		return "descendant-attr";
		break;
	}
	case ImpliedSchemaNode::ROOT: {
		return "root";
		break;
	}
	case ImpliedSchemaNode::METADATA: {
		return "metadata";
		break;
	}
	case ImpliedSchemaNode::CAST: {
		return "cast";
		break;
	}
	case ImpliedSchemaNode::EQUALS: {
		return "equals";
		break;
	}
	case ImpliedSchemaNode::NOT_EQUALS: {
		return "not_equals";
		break;
	}
	case ImpliedSchemaNode::GTX: {
		return "gtx";
		break;
	}
	case ImpliedSchemaNode::GTE: {
		return "gte";
		break;
	}
	case ImpliedSchemaNode::LTX: {
		return "ltx";
		break;
	}
	case ImpliedSchemaNode::LTE: {
		return "lte";
		break;
	}
	case ImpliedSchemaNode::PREFIX: {
		return "prefix";
		break;
	}
	case ImpliedSchemaNode::SUFFIX: {
		return "suffix";
		break;
	}
	case ImpliedSchemaNode::SUBSTRING_CD: {
		return "substring_cd";
		break;
	}
	case ImpliedSchemaNode::SUBSTRING: {
		return "substring";
		break;
	}
	}
	return "UNKNOWN";
}

string ImpliedSchemaNode::toString(int level) const
{
	ostringstream s;
	string in(getIndent(level));
	string type(typeToString((ImpliedSchemaNode::Type)type_));

	s << in << "<" << type;

	switch(type_) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::METADATA:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR:
		if(isWildcardURI()) {
			s << " uri=\"*\"";
		} else if(((DbXmlNodeTest*)nodeTest_)->getNodeUri8() == 0) {
			s << " uri=\"\"";
		} else {
			s << " uri=\"" << (char*)((DbXmlNodeTest*)nodeTest_)->getNodeUri8() << "\"";
		}
		if(isWildcardName()) {
			s << " name=\"*\"";
		} else if(((DbXmlNodeTest*)nodeTest_)->getNodeName8() == 0) {
			s << " name=\"\"";
		} else {
			s << " name=\"" << (char*)((DbXmlNodeTest*)nodeTest_)->getNodeName8() << "\"";
		}
		if(isWildcardNodeType()) {
			s << " nodeType=\"*\"";
		}
		break;
	case ImpliedSchemaNode::CAST:
		s << " uri=\"" << XMLChToUTF8(getCast()->getSequenceType()->getTypeURI()).str() << "\"";
		s << " name=\"" << XMLChToUTF8(getCast()->getSequenceType()->getConstrainingType()->getName()).str() << "\"";
		break;
	default:
		break;
	}

	if(firstChild_) {
		s << ">" << endl;

		ImpliedSchemaNode *child = (ImpliedSchemaNode*)firstChild_;
		while(child) {
			s << child->toString(level + 1);
			child = (ImpliedSchemaNode*)child->nextSibling_;
		}

		s << in << "</" << type << ">" << endl;
	}
	else {
		s << "/>" << endl;
	}

	return s.str();
}

string ImpliedSchemaNode::getPath() const
{
	ostringstream s;

	if(parent_) {
		s << ((ImpliedSchemaNode*)parent_)->getPath();
	}

	if(type_ == QueryPathNode::CHILD || type_ == QueryPathNode::ATTRIBUTE || type_ == QueryPathNode::DESCENDANT || type_ == QueryPathNode::DESCENDANT_ATTR) {
		s << "/";
	}

	s << getStepName();

	return s.str();
}

string ImpliedSchemaNode::getStepName() const
{
	ostringstream s;

	switch((ImpliedSchemaNode::Type)type_) {
	case ROOT: {
		s << "root()";
		break;
	}
	case CAST:
	case GTX:
	case GTE:
	case LTX:
	case LTE:
	case EQUALS:
	case NOT_EQUALS:
	case PREFIX:
	case SUFFIX:
	case SUBSTRING_CD:
	case SUBSTRING: {
		switch((ImpliedSchemaNode::Type)type_) {
		case CAST: s << " [cast] "; break;
		case GTX: s << " [gt] "; break;
		case GTE: s << " [ge] "; break;
		case LTX: s << " [lt] "; break;
		case LTE: s << " [le] "; break;
		case EQUALS: s << " [eq] "; break;
		case NOT_EQUALS: s << " [ne] "; break;
		case PREFIX: s << " [prefix] "; break;
		case SUFFIX: s << " [suffix] "; break;
		case SUBSTRING: s << " [substring] "; break;
		case SUBSTRING_CD: s << " [substring_cd] "; break;
		default: break;
		}
		break;
	}
	default: {
		if(type_ == QueryPathNode::ATTRIBUTE) {
			s << "@";
		}
		else if(type_ != QueryPathNode::CHILD) {
			s << typeToString((ImpliedSchemaNode::Type)type_) << "::";
		}

		if(isWildcardNodeType() && type_ != QueryPathNode::ATTRIBUTE && type_ != QueryPathNode::DESCENDANT_ATTR) {
			s << "node()";
		}
		else {
			s << getUriName();
		}
		break;
	}
	}

	return s.str();
}

const char *ImpliedSchemaNode::getURI8() const
{
	if(nodeTest_->getItemType() || nodeTest_->getNamespaceWildcard())
		return 0;
	return (char*)((DbXmlNodeTest*)nodeTest_)->getNodeUri8();
}

const char *ImpliedSchemaNode::getName8() const
{
	if(nodeTest_->getItemType() || nodeTest_->getNameWildcard())
		return 0;
	return (char*)((DbXmlNodeTest*)nodeTest_)->getNodeName8();
}

// uriname must be name:uri, to match
// Name.cpp usage
const char *ImpliedSchemaNode::getUriName() const
{
	if(utf8URIName_ == 0) {
		ostringstream s;

		if(isWildcardURI() && isWildcardName()) {
			s << "*";
		} else {
			if(isWildcardName()) {
				s << "*";
			} else {
				s << (char*)((DbXmlNodeTest*)nodeTest_)->getNodeName8();
			}
			if(isWildcardURI()) {
				s << ":*";
			} else if(((DbXmlNodeTest*)nodeTest_)->getNodeUri8() != 0) {
				s << ":" << (char*)((DbXmlNodeTest*)nodeTest_)->getNodeUri8();
			}
		}

		const_cast<ImpliedSchemaNode*>(this)->utf8URIName_ = XMLString::replicate(s.str().c_str(), memMgr_);
	}
	return utf8URIName_;
}
