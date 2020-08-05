//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlResults.hpp>
#include "Value.hpp"
#include "Results.hpp"
#include "TypeConversions.hpp"
#include "UTF8.hpp"
#include "SyntaxManager.hpp"
#include "Document.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "Globals.hpp"
#include "ReferenceMinder.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/NsEventReader.hpp"
#include "nodeStore/NsDom.hpp"

#include <sstream>
#include <float.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/items/DatatypeLookup.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

#include <math.h>

using namespace std;
using namespace DbXml;

const std::string Value::NaN_string("NaN");
const std::string Value::PositiveInfinity_string("INF");
const std::string Value::NegativeInfinity_string("-INF");

XmlValue::Type typeFromPrimitive(AnyAtomicType::AtomicObjectType type)
{
	// WARNING: this array depends on the values assigned in
	// the AnyAtomicType enum in AnyAtomicType.hpp.  If those change,
	// this code breaks.  The debug assertions should help detect
	// incompatible changes.
	static XmlValue::Type ourTypes[AnyAtomicType::NumAtomicObjectTypes] =
		{XmlValue::ANY_SIMPLE_TYPE, // 0
		 XmlValue::ANY_URI,
		 XmlValue::BASE_64_BINARY,
		 XmlValue::BOOLEAN,
		 XmlValue::DATE,
		 XmlValue::DATE_TIME,
		 XmlValue::DAY_TIME_DURATION,
		 XmlValue::DECIMAL,
		 XmlValue::DOUBLE,
		 XmlValue::DURATION,
		 XmlValue::FLOAT,
		 XmlValue::G_DAY,
		 XmlValue::G_MONTH,
		 XmlValue::G_MONTH_DAY,
		 XmlValue::G_YEAR,
		 XmlValue::G_YEAR_MONTH,
		 XmlValue::HEX_BINARY,
		 XmlValue::NOTATION,
		 XmlValue::QNAME,
		 XmlValue::STRING,
		 XmlValue::TIME,
		 XmlValue::UNTYPED_ATOMIC,
		 XmlValue::YEAR_MONTH_DURATION};
	
	// a few random assertions to ensure consistency
	DBXML_ASSERT(AnyAtomicType::NumAtomicObjectTypes == 23);
	DBXML_ASSERT(AnyAtomicType::ANY_SIMPLE_TYPE == 0);
	DBXML_ASSERT(AnyAtomicType::G_YEAR == 14);
	DBXML_ASSERT(AnyAtomicType::UNTYPED_ATOMIC == 21);
	DBXML_ASSERT(AnyAtomicType::YEAR_MONTH_DURATION == 22);
	
	if (type < 0 || type >= AnyAtomicType::NumAtomicObjectTypes) {
		std::ostringstream os;
		os << "Cannot convert the primitive type: " << type
		   << " to a supported DB XML type";
		throw XmlException(XmlException::INVALID_VALUE, os.str());
	}
	return ourTypes[type];
}

AnyAtomicType::AtomicObjectType primitiveFromType(XmlValue::Type type)
{
	// This would be more efficient with a simple array, but the type
	// space for XmlValue::Type is sparse.
	switch(type) {
	case XmlValue::ANY_SIMPLE_TYPE: return AnyAtomicType::ANY_SIMPLE_TYPE;
	case XmlValue::ANY_URI: return AnyAtomicType::ANY_URI;
	case XmlValue::BASE_64_BINARY: return AnyAtomicType::BASE_64_BINARY;
	case XmlValue::BOOLEAN: return AnyAtomicType::BOOLEAN;
	case XmlValue::DATE: return AnyAtomicType::DATE;
	case XmlValue::DATE_TIME: return AnyAtomicType::DATE_TIME;
	case XmlValue::DAY_TIME_DURATION: return AnyAtomicType::DAY_TIME_DURATION;
	case XmlValue::DECIMAL: return AnyAtomicType::DECIMAL;
	case XmlValue::DOUBLE: return AnyAtomicType::DOUBLE;
	case XmlValue::DURATION: return AnyAtomicType::DURATION;
	case XmlValue::FLOAT: return AnyAtomicType::FLOAT;
	case XmlValue::G_DAY: return AnyAtomicType::G_DAY;
	case XmlValue::G_MONTH: return AnyAtomicType::G_MONTH;
	case XmlValue::G_MONTH_DAY: return AnyAtomicType::G_MONTH_DAY;
	case XmlValue::G_YEAR: return AnyAtomicType::G_YEAR;
	case XmlValue::G_YEAR_MONTH: return AnyAtomicType::G_YEAR_MONTH;
	case XmlValue::HEX_BINARY: return AnyAtomicType::HEX_BINARY;
	case XmlValue::NOTATION: return AnyAtomicType::NOTATION;
	case XmlValue::QNAME: return AnyAtomicType::QNAME;
	case XmlValue::STRING: return AnyAtomicType::STRING;
	case XmlValue::TIME: return AnyAtomicType::TIME;
	case XmlValue::YEAR_MONTH_DURATION: return AnyAtomicType::YEAR_MONTH_DURATION;
	case XmlValue::UNTYPED_ATOMIC: return AnyAtomicType::UNTYPED_ATOMIC;
	default: break;
	}
	std::ostringstream os;
	os << "Cannot convert the DB XML type " << type
	   << " to a supported primitive type";
	throw XmlException(XmlException::INVALID_VALUE, os.str());
}

// these names match those defined in Xerces-C
const char *stringFromType(XmlValue::Type type)
{
	switch(type) {
	case XmlValue::ANY_SIMPLE_TYPE: return "anySimpleType";
	case XmlValue::ANY_URI: return "anyURI";
	case XmlValue::BASE_64_BINARY: return "base64Binary";
	case XmlValue::BOOLEAN: return "boolean";
	case XmlValue::DATE: return "date";
	case XmlValue::DATE_TIME: return "dateTime";
	case XmlValue::DAY_TIME_DURATION: return "dayTimeDuration";
	case XmlValue::DECIMAL: return "decimal";
	case XmlValue::DOUBLE: return "double";
	case XmlValue::DURATION: return "duration";
	case XmlValue::FLOAT: return "float";
	case XmlValue::G_DAY: return "gDay";
	case XmlValue::G_MONTH: return "gMonth";
	case XmlValue::G_MONTH_DAY: return "gMonthDay";
	case XmlValue::G_YEAR: return "gYear";
	case XmlValue::G_YEAR_MONTH: return "gYearMonth";
	case XmlValue::HEX_BINARY: return "hexBinary";
	case XmlValue::NOTATION: return "NOTATION";
	case XmlValue::QNAME: return "QName";
	case XmlValue::STRING: return "string";
	case XmlValue::TIME: return "time";
	case XmlValue::YEAR_MONTH_DURATION: return "yearMonthDuration";
	case XmlValue::UNTYPED_ATOMIC: return "untypedAtomic";
	default: break;
	}
	return "";
}

Syntax::Type syntaxFromType(XmlValue::Type type)
{
	switch(type) {
	case XmlValue::BASE_64_BINARY: return Syntax::BASE_64_BINARY;
	case XmlValue::BOOLEAN: return Syntax::BOOLEAN;
	case XmlValue::DATE: return Syntax::DATE;
	case XmlValue::DATE_TIME: return Syntax::DATE_TIME;
	case XmlValue::DECIMAL: return Syntax::DECIMAL;
	case XmlValue::DOUBLE: return Syntax::DOUBLE;
	case XmlValue::DURATION: return Syntax::DURATION;
	case XmlValue::FLOAT: return Syntax::FLOAT;
	case XmlValue::G_DAY: return Syntax::DAY;
	case XmlValue::G_MONTH: return Syntax::MONTH;
	case XmlValue::G_MONTH_DAY: return Syntax::MONTH_DAY;
	case XmlValue::G_YEAR: return Syntax::YEAR;
	case XmlValue::G_YEAR_MONTH: return Syntax::YEAR_MONTH;
	case XmlValue::HEX_BINARY: return Syntax::HEX_BINARY;
	case XmlValue::STRING: return Syntax::STRING;
	case XmlValue::TIME: return Syntax::TIME;

	// No syntax for the following
	case XmlValue::ANY_URI:
	case XmlValue::NOTATION:
	case XmlValue::QNAME: return Syntax::STRING;
	default: break;
	}

	return Syntax::NONE;
}

XmlValue::Type typeFromSyntax(Syntax::Type type)
{
	switch(type) {
	case Syntax::BASE_64_BINARY: return XmlValue::BASE_64_BINARY;
	case Syntax::BOOLEAN: return XmlValue::BOOLEAN;
	case Syntax::DATE: return XmlValue::DATE;
	case Syntax::DATE_TIME: return XmlValue::DATE_TIME;
	case Syntax::DECIMAL: return XmlValue::DECIMAL;
	case Syntax::DOUBLE: return XmlValue::DOUBLE;
	case Syntax::DURATION: return XmlValue::DURATION;
	case Syntax::FLOAT: return XmlValue::FLOAT;
	case Syntax::DAY: return XmlValue::G_DAY;
	case Syntax::MONTH: return XmlValue::G_MONTH;
	case Syntax::MONTH_DAY: return XmlValue::G_MONTH_DAY;
	case Syntax::YEAR: return XmlValue::G_YEAR;
	case Syntax::YEAR_MONTH: return XmlValue::G_YEAR_MONTH;
	case Syntax::HEX_BINARY: return XmlValue::HEX_BINARY;
	case Syntax::STRING: return XmlValue::STRING;
	case Syntax::TIME: return XmlValue::TIME;

	case Syntax::DEPRECATED_ANY_URI:
	case Syntax::DEPRECATED_NOTATION:
	case Syntax::DEPRECATED_QNAME:
		DBXML_ASSERT(false);
		break;
	default: break;
	}

	return XmlValue::NONE;
}

Value *Value::create(const AnyAtomicType::Ptr &atom, DynamicContext *context)
{
	std::string typeURI(XMLChToUTF8(atom->getTypeURI()).str());
	std::string typeName(XMLChToUTF8(atom->getTypeName()).str());
	std::string value(XMLChToUTF8(atom->asString(context)).str());
	return new AtomicTypeValue(
		typeFromPrimitive(atom->getPrimitiveTypeIndex()),
		typeURI, typeName, value);
}

Value *Value::create(const Node::Ptr &item, bool lazyDocs)
{
	DbXmlNodeImpl *nodeImpl = (DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	Document *document = (Document*)nodeImpl->getDocument();

	if(!lazyDocs && document) {
		document->setEagerMetaData();
	}

	if(nodeImpl->dmNodeKind() == Node::document_string) {
		DBXML_ASSERT(document);
		// Don't bother creating a copy
		// of the node, as that involves copying the entire tree...
		return new DbXmlNodeValue(document);
	}
	// always return 'live' nodes.
	return new DbXmlNodeValue(nodeImpl, document);
}

Value *Value::create(XmlValue::Type type, const std::string &v, bool validate)
	{
		if (type <= XmlValue::ANY_SIMPLE_TYPE) // NONE OR NODE
			return 0;
		if (type == XmlValue::BINARY)
			return new BinaryValue(v);
		DBXML_ASSERT(type > XmlValue::ANY_SIMPLE_TYPE &&
			     type <= XmlValue::UNTYPED_ATOMIC);
		Value *value = new AtomicTypeValue(type, v);
		if (validate) {
			try {
				value->validate();
			} catch (...) {
				delete value;
				throw; // re-throw
			}
		}
		return value;
}

Value *Value::create(XmlValue::Type type, const XmlData &xd, bool validate)
{
	if (type != XmlValue::BINARY)
		return Value::create(type,
				     std::string((const char *)xd.get_data()),
				     validate);
	return new BinaryValue(xd);
}

Item::Ptr NodeValue::createNode(DynamicContext *context) const
{
	short nodeType = getNodeType();
	DbXmlFactoryImpl *factoryImpl = (DbXmlFactoryImpl*)context->getItemFactory();
	
	switch(nodeType) {
	case nsNodeText: 
	case nsNodeCDATA: 
	case nsNodeComment: {
		UTF8ToXMLCh value(getNodeValue());
		return factoryImpl->createTextNode(nodeType, value.str(), context);
	}
	case nsNodeAttr: {
		UTF8ToXMLCh prefix(getPrefix());
		UTF8ToXMLCh uri(getNamespaceURI());
		UTF8ToXMLCh localName(getLocalName());
		UTF8ToXMLCh value(getNodeValue());
		UTF8ToXMLCh typeName(getTypeName());
		UTF8ToXMLCh typeURI(getTypeURI());
		return factoryImpl->createAttrNode(prefix.str(), uri.str(),
						   localName.str(), value.str(),
						   typeName.str(), typeURI.str(),
						   NULL, NULL, NsNid(), 0, context);
	}
	case nsNodePinst: {
		UTF8ToXMLCh target(getNodeName());
		UTF8ToXMLCh value(getNodeValue());
		return factoryImpl->createPINode(target.str(), value.str(), context);
	}
	case nsNodeDocument: {
		GET_CONFIGURATION(context)->getMinder()->addDocument(asDocument());
		return factoryImpl->createNode(0, asDocument(), context);
	}
	default:
		throw XmlException(XmlException::INVALID_VALUE,
				   "Can't convert XmlValue to Item");
	}
}

// "double check that this node exists and
// is the same as what you expect."  It is intended to handle situations
// where a Value has been deleted out from under an XmlResults object:
//  o document removed or updated in place
//  o node removed
//  o node replaced
// Some situations cannot easily be detected. i.e. node has been
// replaced with another that happens to have the same DID/NID pair.
// That work is TBD -- all this does is verify that the Doc ID and
// Node ID still exist.
//
void NodeValue::validateNode(DbXmlNodeImpl *node, DynamicContext *context)
{
	Document *doc = asDocument();
	// constructed text and attribute nodes will not have documents, and
	// they cannot be removed, so skip them
	if (!doc)
		return;
	DBXML_ASSERT(context);
	IndexEntry ie;
	initIndexEntryFromThis(ie);
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
	try {
		if (!node) {
			NsNodeRef nref = NsFormat::fetchNode(*NsNid::getRootNid(),
							     doc->getID(), *doc->getDocDb(),
							     oc, false);
			if (!nref)
				throw XmlException(DB_NOTFOUND, __FILE__, __LINE__);
		} else {
			bool err = false;
			NsDomNodeRef nsref = ie.fetchNode(doc, oc.txn());
			// (2) the node exists and if an element or attribute,
			// there is *some* entry at the specified index.  If the
			// content *has* been changed and this is coincidence, there's
			// no good/simple/cheap way to verify this.  At least check
			// that the types match for sanity.
			if (nsref) {
				NsDomNodeRef orig = node->getNsDomNode();
				DBXML_ASSERT(orig.get());
				if (orig->getNsNodeType() != nsref->getNsNodeType())
					err = true;
			} else
				err = true;
			if (err)
				throw XmlException(DB_NOTFOUND, __FILE__, __LINE__);
		}
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR &&
		    xe.getDbErrno() == DB_LOCK_DEADLOCK)
			throw; // re-throw deadlocks
		throw XmlException(XmlException::INVALID_VALUE,
				   "An attempt was made to reference a node that no longer exists; the node may be a bound variable or part of a query context");
	}
}

Item::Ptr Value::convertToItem(const Value *v, DynamicContext *context,
			       bool validate)
{
	if(v != 0) {
		switch (v->getType()) {
		case XmlValue::NONE: {
			break;
		}
		case XmlValue::NODE: {
			DbXmlNodeImpl *nodeImpl = ((NodeValue*)v)->getNodeImpl(context, validate);
			if (nodeImpl) {
				// look at possibly merging the Document's cache database into
				// the query's minder to attempt to have only one per container
				if (nodeImpl->getDocument()) {
					Document *d = const_cast<Document*>(nodeImpl->getDocument());
					if (d->getCacheDatabase() && (d->getContainerID() != 0))
						GET_CONFIGURATION(context)->getDbMinder().mergeCacheDatabase(d);
				}
				return nodeImpl;
			}

			const XmlDocument &xdoc = v->asDocument();
			if(!xdoc.isNull())
				GET_CONFIGURATION(context)->getMinder()->addDocument(xdoc);

			return ((NodeValue *)v)->createNode(context);
		}
		case XmlValue::ANY_URI:
		case XmlValue::BASE_64_BINARY:
		case XmlValue::BOOLEAN:
		case XmlValue::DATE:
		case XmlValue::DATE_TIME:
		case XmlValue::DAY_TIME_DURATION:
		case XmlValue::DECIMAL:
		case XmlValue::DOUBLE:
		case XmlValue::DURATION:
		case XmlValue::FLOAT:
		case XmlValue::G_DAY:
		case XmlValue::G_MONTH:
		case XmlValue::G_MONTH_DAY:
		case XmlValue::G_YEAR:
		case XmlValue::G_YEAR_MONTH:
		case XmlValue::HEX_BINARY:
		case XmlValue::NOTATION:
		case XmlValue::QNAME:
		case XmlValue::STRING:
		case XmlValue::TIME:
		case XmlValue::YEAR_MONTH_DURATION:
		case XmlValue::UNTYPED_ATOMIC: {
			// create a simple type atom
			return context->getItemFactory()->
				createDerivedFromAtomicType(
					primitiveFromType(v->getType()),
					context->getMemoryManager()->getPooledString(UTF8ToXMLCh(v->getTypeURI()).str()),
					context->getMemoryManager()->getPooledString(UTF8ToXMLCh(v->getTypeName()).str()),
					UTF8ToXMLCh(v->asString()).str(),
					context);
		}
		default: break;
		}
	}
	return 0;
}

const XmlDocument &Value::asDocument() const
{
	throw XmlException(
		XmlException::INVALID_VALUE, "Can't convert XmlValue to Document");
}

// DbXmlNodeValue

DbXmlNodeValue::DbXmlNodeValue(Document *d)
	: n_(0),
	  d_(d)
{
}

DbXmlNodeValue::DbXmlNodeValue(DbXmlNodeImpl *n, Document *d)
	: n_(n),
	  d_(d)
{
	if(n) n->returnedFromQuery();
}

DbXmlNodeValue::~DbXmlNodeValue()
{
}

string DbXmlNodeValue::getTypeURI() const
{
	if(!n_.isNull())
		return XMLChToUTF8(n_->getTypeURI()).str();
	return "";
}

string DbXmlNodeValue::getTypeName() const
{
	if(!n_.isNull())
		return XMLChToUTF8(n_->getTypeName()).str();
	return "";
}

double DbXmlNodeValue::asNumber() const
{
	return strtod(asString().c_str(), 0);
}

static const XMLCh XMLChUTF8[] = {
	chLatin_U, chLatin_T, chLatin_F, chDash, chDigit_8, chNull
};

std::string DbXmlNodeValue::asString() const
{
	// TBD GMF -- consolidate this with NsWriter?  maybe...
	
	if(n_.isNull() || n_->getNodeType() == nsNodeDocument) {
		std::string content;
		return d_.getContent(content);
	} else if(n_->getNodeType() == nsNodeElement) {
		// Make output writer
		std::string outstr;
		StringNsStream output(outstr);
		NsWriter writer(&output);

		const NsDomNode *n = n_->getNsDomNode().get();
		DBXML_ASSERT(n);
		// Make an event reader
		ScopedPtr<EventReader> reader(
			((Document*)d_)->
			getElementAsReader((NsDomElement*)n));
		
		writer.writeFromReader(*reader);

		return outstr;
	} else if(n_->getNodeType() == nsNodeAttr) {
		std::string str = "{";
		if(n_->getUri() != 0) {
			str.append(XMLChToUTF8(n_->getUri()).str());
		}
		str.append("}");
		str.append(XMLChToUTF8(n_->getLocalName()).str());
		str.append("=\"");
		str.append(XMLChToUTF8(n_->getValue()).str());
		str.append("\"");
		return str;
	} else if(n_->getNodeType() == nsNodeText) {
		return XMLChToUTF8(n_->getValue()).str();
	} else if(n_->getNodeType() == nsNodeComment) {
		std::string str = "<!--";
		str.append(XMLChToUTF8(n_->getValue()).str());
		str.append("-->");
		return str;
	} else if(n_->getNodeType() == nsNodeCDATA) {
		std::string str = "<![CDATA[";
		str.append(XMLChToUTF8(n_->getValue()).str());
		str.append("]]>");
		return str;
	} else if(n_->getNodeType() == nsNodePinst) {
		std::string str = "<?";
		str.append(XMLChToUTF8(n_->getPITarget()).str());
		str.append(" ");
		str.append(XMLChToUTF8(n_->getValue()).str());
		str.append("?>");
		return str;
	} else {
		throw XmlException(XmlException::INVALID_VALUE,
				   "cannot create string for type");
	}
	return "";
}

bool DbXmlNodeValue::asBoolean() const
{

	// jcm: The xpath spec says that an empty sting is false, and a non-empty
	// string is true. But, the Boolean::string() xpath function returns
	// 'true' for true, and 'false' for false.... this is a bit silly, so
	// we diverge from the spec here.
	std::string s(asString());
	return (!s.empty() && s.compare("false") != 0);
}

const XmlDocument &DbXmlNodeValue::asDocument() const
{
	return d_;
}

const Document *DbXmlNodeValue::getDocument() const
{
	return d_;
}

DbXmlNodeImpl *DbXmlNodeValue::getNodeImpl(DynamicContext *context,
					   bool validate)
{
	DbXmlNodeImpl *node = (DbXmlNodeImpl*)n_.get();
	if (validate)
		validateNode(node, context);
	return node;
}

XmlEventReader &DbXmlNodeValue::asEventReader() const
{
	if((n_.isNull() || n_->getNodeType() == nsNodeDocument) ||
	   (n_->getNodeType() == nsNodeElement)) {
		if (n_.isNull()) {
			DBXML_ASSERT(d_);
			return d_.getContentAsEventReader();
		} else {
			// Make an event reader
			NsNid nid = n_->getNodeID();
			const NsDomNode *n = n_->getNsDomNode().get();
			DBXML_ASSERT(n);
			return *(new NsEventReader(
					 *(n->getNsDoc()),
					 NS_EVENT_BULK_BUFSIZE,
					 &nid));

		}
	}
	// The only way to support asEventReader on non-elements is if
	// the XmlEventReader interface were able to iterate through attributes
	// one at a time, rather than using an indexed interface.
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlValue::asEventReader requires an element node");
}

bool DbXmlNodeValue::equals(const Value &v) const
{
	if (v.getType() == XmlValue::NODE) {
		const DbXmlNodeImpl *thisImpl =
			const_cast<DbXmlNodeValue*>(this)->getNodeImpl(NULL, false);
		const DbXmlNodeImpl *otherImpl = ((DbXmlNodeValue&)v).getNodeImpl(NULL, false);
		if (thisImpl && otherImpl) {
			Node::Ptr other(otherImpl);
			if (otherImpl && thisImpl->equals(other))
				return true;
		} else {
			// compare documents
			if (d_ == ((DbXmlNodeValue &)v).d_)
				return true;
		}
	}
	return false;
}

void DbXmlNodeValue::initIndexEntryFromThis(IndexEntry &ie) const
{
	if (getNodeType() != nsNodeDocument) {
		n_->getNodeHandle(ie);
	} else {
		ie.setDocID(((Document&)d_).getID());
		ie.setFormat(IndexEntry::NH_DOCUMENT_FORMAT);
	}
}

string NodeValue::getNodeHandle() const
{
	if (getContainerId() == 0) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Node handles are only available for nodes from a container");
	}
	IndexEntry ie;
	initIndexEntryFromThis(ie);
	return ie.getNodeHandle();
}

// AtomicTypeValue

// static helper methods within file scope
namespace
{
	const std::string s_false("false");
	const std::string s_true("true");

	const double s_nan = sqrt(-2.01);
	const double s_positiveInfinity = HUGE_VAL;
	const double s_negativeInfinity = -s_positiveInfinity;

	// does the double represent an infinite value?
	bool isInfinite(double n)
	{
		return (memcmp(&n, &s_positiveInfinity, sizeof (n)) == 0) ||
			(memcmp(&n, &s_negativeInfinity, sizeof (n)) == 0);
	}

	// does the double represent a NaN?
	bool isNan(double n)
	{
		return memcmp(&n, &s_nan, sizeof(n)) == 0;
	}

	// type validation using the Xerces/XQilla datatype validators
	void validateType(XmlValue::Type type, const std::string &typeURI,
		const std::string &typeName, const std::string &value)
	{
		bool isPrimitive;
		const DatatypeFactory *factory = Globals::getDatatypeLookup()->
			lookupDatatype(UTF8ToXMLCh(typeURI).str(), UTF8ToXMLCh(typeName).str(),
				isPrimitive);

		if(factory == 0) {
			std::ostringstream os;
			os << "Cannot get datatype validator for an XmlValue type of {"
			   << typeURI << "}" << typeName << " (\"" << stringFromType(type) << "\")";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}

		if(factory->getPrimitiveTypeIndex() != primitiveFromType(type)) {
			std::ostringstream os;
			os << "XmlValue enumeration \"" << stringFromType(type) << "\" does not match type name of {"
			   << typeURI << "}" << typeName;
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}

		try {
			if(!factory->checkInstance(UTF8ToXMLCh(value).str(),
				   Globals::defaultMemoryManager)) {
				// InvalidDatatype[Value|Facet]Exception
				std::ostringstream os;
				os << "Error - the value \"" << value <<
					"\" is not valid for type \"" << stringFromType(type) <<
					"\"";
				throw XmlException(XmlException::INVALID_VALUE, os.str());
			}
		}
		catch (XmlException &xe) {
			// will catch transcoding problems
			// InvalidDatatype[Value|Facet]Exception
			std::ostringstream os;
			os << "The value \"" << value <<
				"\" is not valid for type \"" << stringFromType(type) <<
				"\"";
			throw XmlException(XmlException::INVALID_VALUE, os.str());
		}
	}
}

Syntax::Type Value::getSyntaxType(const AnyAtomicType::Ptr &atom)
{
	try {
		return syntaxFromType(typeFromPrimitive(atom->getPrimitiveTypeIndex()));
	}
	catch(...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Unable to obtain syntax type.");
	}
}

AtomicTypeValue::AtomicTypeValue(bool v)
	: Value(XmlValue::BOOLEAN), value_(v ? s_true : s_false)
{
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(double v)
	: Value(XmlValue::DOUBLE), value_("")
{
	if(isNan(v)) {
		value_.assign(NaN_string);
	} else if (isInfinite(v)) {
		if(v < 0) {
			value_.assign(NegativeInfinity_string);
		} else {
			value_.assign(PositiveInfinity_string);
		}
	} else {
		value_ = DbXml::toString(v);
	}
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(const std::string &v)
	: Value(XmlValue::STRING), value_(v)
{
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(const char *v)
	: Value(XmlValue::STRING)
{
	if(!v) {
		throw XmlException(XmlException::INVALID_VALUE,
			"NULL pointer cannot be used as a value");
	}
	value_.assign(v);
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(XmlValue::Type type, const std::string &v)
	: Value(type), value_(v)
{
	setTypeNameFromEnumeration();
}

AtomicTypeValue::AtomicTypeValue(const std::string &typeURI,
		const std::string &typeName, const std::string &v)
	: Value(XmlValue::STRING),
	  typeURI_(typeURI),
	  typeName_(typeName),
	  value_(v)
{
	bool isPrimative; 
	const DatatypeFactory *factory = Globals::getDatatypeLookup()->
		lookupDatatype(UTF8ToXMLCh(typeURI_).str(), 
			       UTF8ToXMLCh(typeName_).str(),
			       isPrimative);

	if(factory == 0) {
		std::ostringstream os;
		os << "Cannot get datatype factory for an XmlValue of type name " << typeName_;
		throw XmlException(XmlException::INVALID_VALUE, os.str());
	}
	
	setType(typeFromPrimitive(factory->getPrimitiveTypeIndex()));
}

AtomicTypeValue::AtomicTypeValue(XmlValue::Type type, const std::string &typeURI,
	const std::string &typeName, const std::string &v)
	: Value(type),
	  typeURI_(typeURI),
	  typeName_(typeName),
	  value_(v)
{
}

void AtomicTypeValue::setTypeNameFromEnumeration()
{
	const DatatypeFactory *factory = Globals::getDatatypeLookup()->
		lookupDatatype(primitiveFromType(getType()));

	if(factory == 0) {
		std::ostringstream os;
		os << "Cannot get datatype validator for an XmlValue type of " << getType();
		throw XmlException(XmlException::INVALID_VALUE, os.str());
	}

	typeURI_ = XMLChToUTF8(factory->getPrimitiveTypeURI()).str();
	typeName_ = XMLChToUTF8(factory->getPrimitiveTypeName()).str();
}

Syntax::Type AtomicTypeValue::convertToSyntaxType(XmlValue::Type valueType)
{
	return syntaxFromType(valueType);
}

XmlValue::Type AtomicTypeValue::convertToValueType(Syntax::Type syntaxType)
{
	return typeFromSyntax(syntaxType);
}

const char *AtomicTypeValue::getTypeString(Syntax::Type syntaxType)
{
	return stringFromType(typeFromSyntax(syntaxType));
}

const char *AtomicTypeValue::getValueTypeString(XmlValue::Type valueType)
{
	return stringFromType(valueType);
}

Syntax::Type AtomicTypeValue::getSyntaxType() const
{
	return syntaxFromType(getType());
}

/// Return as a Number.
double AtomicTypeValue::asNumber() const
{

	switch(getType()) {
	case XmlValue::BOOLEAN: {
		if(value_.compare(s_true) == 0)
			return 1;
		else
			return 0;
		break;
	}
	case XmlValue::FLOAT:
	case XmlValue::DOUBLE:
	case XmlValue::DECIMAL: {
		return strtod(value_.c_str(), 0);
		break;
	}
	case XmlValue::UNTYPED_ATOMIC:
	case XmlValue::STRING: {
		bool numeric = false;
		try {
			numeric = Globals::getDatatypeLookup()->
				lookupDatatype(AnyAtomicType::DOUBLE)->
				checkInstance(UTF8ToXMLCh(value_).str(),
					Globals::defaultMemoryManager);
		}
		catch (XmlException &xe) {
			// will catch transcoding problems
		}

		if(numeric) {
			if (value_.compare(NaN_string)==0)
				return s_nan;
			else if (value_.compare(
					 NegativeInfinity_string) ==0 )
				return s_negativeInfinity;
			else if (value_.compare(
					 PositiveInfinity_string) ==0 )
				return s_positiveInfinity;
			else
				return strtod(value_.c_str(), 0);
		} else {
			return s_nan;
		}
	}
	default:
		throw XmlException(
			XmlException::INVALID_VALUE,
			"The requested type cannot be converted into a number.");
		break;
	}
}

// Return as a string
std::string AtomicTypeValue::asString() const
{
	return value_;

}

/// Return the value as a Boolean.
bool AtomicTypeValue::asBoolean() const
{
	switch(getType()) {
	case XmlValue::BOOLEAN: {
		return (value_.compare(s_true) == 0);
		break;
	}
	case XmlValue::DECIMAL: {
		return (strtod(value_.c_str(), 0) != 0);
		break;
	}
	case XmlValue::FLOAT:
	case XmlValue::DOUBLE: {
		if (value_.compare(NaN_string)==0) {
			return false;
		} else {
			if (value_.compare(NegativeInfinity_string) == 0 ||
			    value_.compare(PositiveInfinity_string) == 0)
				return true;
			else
				return (strtod(value_.c_str(), 0) != 0);
		}
		break;
	}
	case XmlValue::STRING: {
		// jcm: The xpath spec says that an empty sting is false, and
		// a non-empty string is true. But, the Boolean::string()
		// xpath function returns 'true' for true, and 'false' for
		// false.... this is a bit silly, so we diverge
		// from the spec here.
		return (!value_.empty() && value_.compare(s_false) != 0);
		break;
	}
	default:
		throw XmlException(XmlException::INVALID_VALUE,
				   "The requested type cannot be converted into a boolean.");
		break;
	}
}

bool AtomicTypeValue::equals(const Value &v) const
{
	return ((getType() == v.getType()) && (value_.compare(
						       v.asString()) == 0));
}

void AtomicTypeValue::validate() const
{
	validateType(getType(), typeURI_, typeName_, value_);
}

// DbXmlNodeValue

#define MATERIALISE_DOC if (n_.isNull()) {	\
		Document *d = (Document *)d_; \
		n_ = new DbXmlNsDomNode(d->getElement(*NsNid::getRootNid(),0), d, (DbXmlConfiguration *)0); \
	}

// Node name return varies by type of the node
std::string DbXmlNodeValue::getNodeName() const {
	string retval;
	MATERIALISE_DOC;
	if(!n_.isNull()) {
		if (n_->getNsDomNode())
			retval = XMLChToUTF8(n_->getNsDomNode()->
					     getNsNodeName()).str();
		else {
			// rules vary by type
			switch (n_->getNodeType()) {
			case nsNodePinst:
				retval =  XMLChToUTF8(n_->getPITarget()).str();
				break;
			case nsNodeAttr: {
				const xmlch_t *prefix = n_->getPrefix();
				if (prefix) {
					retval = XMLChToUTF8(prefix).str();
					retval += ":";
				}
				retval +=  XMLChToUTF8(n_->getLocalName()).str();
				break;
			}
			case nsNodeText:
				retval = "#text";
				break;
			case nsNodeCDATA:
				retval = "#cdata-section";
				break;
			case nsNodeComment:
				retval = "#comment";
				break;
			default:
				break;
			}
		}
	} else {
		retval = "#document";
	}
	return retval;
}

int DbXmlNodeValue::getContainerId() const {
	return ((Document&)d_).getContainerID();
}

std::string DbXmlNodeValue::getNamespaceURI() const {
	string retval = "";
	if(!n_.isNull()) {
		retval = XMLChToUTF8(n_->getUri()).str();
	}
	return retval;
}

std::string DbXmlNodeValue::getPrefix() const {
	string retval = "";
	if(!n_.isNull()) {
		retval = XMLChToUTF8(n_->getPrefix()).str();
	}
	return retval;
}

std::string DbXmlNodeValue::getLocalName() const {
	string retval = "";
	MATERIALISE_DOC;
	if(!n_.isNull()) {
		if (n_->getNsDomNode())
			retval = XMLChToUTF8(n_->getNsDomNode()->
					   getNsLocalName()).str();
		if (n_->getNodeType() == nsNodeAttr)
			retval = XMLChToUTF8(n_->getLocalName()).str();
	} else {
		retval = "#document";
	}
	return retval;
}

// Node value return varies by type of the node
std::string DbXmlNodeValue::getNodeValue() const {
	MATERIALISE_DOC;
	if(!n_.isNull()) {
		if (n_->getNsDomNode())
			return XMLChToUTF8(n_->getNsDomNode()->
					   getNsNodeValue()).str();
		// rules vary by type
		switch (n_->getNodeType()) {
		case nsNodeAttr:
		case nsNodeText:
		case nsNodeCDATA:
		case nsNodeComment:
		case nsNodePinst:
			return XMLChToUTF8(n_->getValue()).str();
		case nsNodeElement: // shouldn't happen
		default:
			break;
		}
		return "";
	} else {
		return "";
	}
}

// Note: this returns NsNodeType_t -- an enum
short DbXmlNodeValue::getNodeType() const {
	MATERIALISE_DOC;
	if(!n_.isNull()) {
		return n_->getNodeType();
	} else {
		return nsNodeDocument;
	}
}

DbXmlNodeValue *DbXmlNodeValue::makeDbXmlNodeValue(const NsDomNode *node,
						   Document *doc,
						   DbXmlConfiguration *conf)
{
	DbXmlNodeImpl *tnode = new DbXmlNsDomNode(node,
						  doc, conf);
	return new DbXmlNodeValue(tnode, doc);
}
		

XmlValue DbXmlNodeValue::getParentNode() const {
	const NsDomNode *parent = 0;
	if(!n_.isNull() &&
	   (parent = n_->getParentNode()) != 0)
		return makeDbXmlNodeValue(parent, d_);
	else
		return XmlValue();
}

XmlValue DbXmlNodeValue::getFirstChild() const {
	const NsDomNode *child = 0;
	const NsDomNode *thisnode = 0;
	MATERIALISE_DOC;
	if(!n_.isNull() &&
	   (thisnode = n_->getNsDomNode().get()) != 0)
		child = const_cast<NsDomNode*>(thisnode)->
			getNsFirstChild();
	if (child == 0)
		return XmlValue();
	else
		return makeDbXmlNodeValue(child, d_);
}

XmlValue DbXmlNodeValue::getLastChild() const {
	const NsDomNode *child = 0;
	const NsDomNode *thisnode = 0;
	MATERIALISE_DOC;
	if(!n_.isNull() &&
	   (thisnode = n_->getNsDomNode().get()) != 0)
		child = const_cast<NsDomNode*>(thisnode)->
			getNsLastChild();
	if (child == 0)
		return XmlValue();
	else
		return makeDbXmlNodeValue(child, d_);
}

XmlValue DbXmlNodeValue::getPreviousSibling() const {
	const NsDomNode *sibling = 0;
	const NsDomNode *thisnode = 0;
	if(!n_.isNull() &&
	   (thisnode = n_->getNsDomNode().get()) != 0)
		sibling = const_cast<NsDomNode*>(thisnode)->
			getNsPrevSibling();
	if (sibling == 0)
		return XmlValue();
	else
		return makeDbXmlNodeValue(sibling, d_);
}

XmlValue DbXmlNodeValue::getNextSibling() const {
	const NsDomNode *sibling = 0;
	const NsDomNode *thisnode = 0;
	if(!n_.isNull() &&
	   (thisnode = n_->getNsDomNode().get()) != 0)
		sibling = const_cast<NsDomNode*>(thisnode)->
			getNsNextSibling();
	if (sibling == 0)
		return XmlValue();
	else
		return makeDbXmlNodeValue(sibling, d_);
}

XmlResults DbXmlNodeValue::getAttributes() const {
	DBXML_ASSERT(d_);
	XmlManager mgr(((Document&)d_).getManager());
	ValueResults *vr = new ValueResults(mgr);
	if(!n_.isNull() && (n_->getNodeType() == nsNodeElement)) {
		NsDomElement *elem = (NsDomElement*)n_->getNsDomNode().get();
		DBXML_ASSERT(elem);
		int size = elem->getNumAttrs();
		for(int i = 0; i < size; ++i) {
			DbXmlNodeValue *curNode = makeDbXmlNodeValue(
				elem->getNsAttr(i),
				d_);
			vr->add(curNode);
		}
	}
	return vr;
}

XmlValue DbXmlNodeValue::getOwnerElement() const {

	if(!n_.isNull() && n_->getNodeType() ==  nsNodeAttr) {
		const NsDomNode *parentnode = n_->getParentNode();
		if (parentnode)
			return makeDbXmlNodeValue(parentnode, d_);
		else
			return XmlValue();
	}
	throw XmlException(XmlException::INVALID_VALUE,
			   "Node is not an attribute node");

}

BinaryValue::BinaryValue(const XmlData &xd)
	: Value(XmlValue::BINARY)
{
	init(xd.get_data(), xd.get_size());
}

BinaryValue::BinaryValue(const std::string &content)
	: Value(XmlValue::BINARY)
{
	// include the terminting null
	init((void*)content.c_str(), (u_int32_t)(content.size()+1));
}

BinaryValue::~BinaryValue()
{
	if (dbt_.data)
		::free(dbt_.data);
}

void BinaryValue::init(const void *data, size_t size)
{
	if (size && data) {
		void *bin = ::malloc(size);
		if (!bin)
			throw XmlException(XmlException::NO_MEMORY_ERROR,
					   "Cannot allocate memory for XmlValue");
		::memcpy(bin, data, size);
		dbt_.data = bin;
		dbt_.size = (uint32_t)size;
	}
}
	
bool BinaryValue::equals(const Value &v) const
{
	if (v.getType() == XmlValue::BINARY) {
		const void *thisData = dbt_.data;
		const void *otherData = v.asBinary().data;
		u_int32_t thisSize = dbt_.size;
		u_int32_t otherSize = v.asBinary().size;
		if (thisSize == otherSize) {
			if (::memcmp(thisData, otherData, thisSize) == 0)
				return true;
		}
	}
	return false;
}

