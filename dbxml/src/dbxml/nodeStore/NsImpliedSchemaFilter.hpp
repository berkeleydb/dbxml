//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSIMPLIEDSCHEMAFILTER_HPP
#define __DBXMLNSIMPLIEDSCHEMAFILTER_HPP

#include <vector>

#include "NsEvent.hpp"
#include "../query/ImpliedSchemaNode.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xercesc/framework/XMLBuffer.hpp>

namespace DbXml {

class NsHandlerBase;
class NsXercesTranscoder;

//
// NsImpliedSchemaFilter
//
// Filters NsEventHandler16 events according to which ones are required
// by the given ImpliedSchemaNode schema
//	
class NsImpliedSchemaFilter : public NsEventTranslator {
public:
	NsImpliedSchemaFilter(const ISNVector &isns, NsEventHandler16 *next);
	NsImpliedSchemaFilter(const ISNVector &isns, NsEventTranslator *next);
	NsImpliedSchemaFilter(const ISNVector &isns, NsXercesTranscoder *next);
	virtual ~NsImpliedSchemaFilter();

	virtual void startDocument(const xmlch_t *sniffedEncoding);
	virtual void xmlDecl(const xmlch_t *xmlDecl,
		const xmlch_t *encodingStr,
		const xmlch_t *standaloneStr);
	virtual void endDocument();
	virtual void startElement(const xmlch_t *localName,
		const xmlch_t *prefix,
		const xmlch_t *uri,
		NsEventAttrList16 *attrs,
		const uint32_t attrCount,
		bool isEmpty);
	virtual void endElement(const xmlch_t *localName,
		const xmlch_t *prefix,
		const xmlch_t *uri);
	virtual void characters(const xmlch_t *characters,
		size_t len, bool isCDATA,
		bool needsEscape);
	virtual void ignorableWhitespace(const xmlch_t *characters,
		size_t len, bool isCDATA);
	virtual void comment(const xmlch_t *comment, size_t len);
	virtual void processingInstruction(const xmlch_t *target,
		const xmlch_t *data);
	virtual void docTypeDecl(const xmlch_t *data, size_t len);


	virtual EventWriter *getEventWriter() const;
	virtual void setEventWriter(EventWriter *writer);

private:
	NsImpliedSchemaFilter(const NsImpliedSchemaFilter &);
	NsImpliedSchemaFilter &operator=(const NsImpliedSchemaFilter &);

	struct StackEntry {
		StackEntry() : matched(false), nodeIDCount(0), depthAdded(false), nonElementChildren(false), children() {}

		bool addNode(const ImpliedSchemaNode *isn, NsEventAttrList16 *attrs);
		bool addChildren(const ImpliedSchemaNode *isn, NsEventAttrList16 *attrs);

		XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer localName;
		XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer prefix;
		XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer uri;

		bool matched;
		int nodeIDCount;
		bool depthAdded;

		bool nonElementChildren;
		ISNVector children;
	};

	typedef std::vector<StackEntry*> FilterStack;

	void updateNodeIDs(FilterStack::reverse_iterator p);
	void checkAncestors(FilterStack::reverse_iterator s);

	FilterStack stack_;
	NsEventHandler16 *next_;
	NsEventTranslator *nextT_;
	NsHandlerBase *handlerBase_;
};

}

#endif
