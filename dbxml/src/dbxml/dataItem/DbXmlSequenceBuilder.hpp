//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLSEQUENCEBUILDER_HPP
#define	__DBXMLSEQUENCEBUILDER_HPP

#include <dbxml/XmlDocument.hpp>

#include <xqilla/events/SequenceBuilder.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include "../nodeStore/NsNid.hpp"

namespace DbXml
{

class NsEventWriter;
	
class DbXmlSequenceBuilder : public SequenceBuilder
{
public:
	DbXmlSequenceBuilder(const DynamicContext *context);
	virtual ~DbXmlSequenceBuilder();

	virtual void startDocumentEvent(const XMLCh *documentURI, const XMLCh *encoding);
	virtual void endDocumentEvent();
	virtual void startElementEvent(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname);
	virtual void endElementEvent(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname,
		const XMLCh *typeURI, const XMLCh *typeName);
	virtual void piEvent(const XMLCh *target, const XMLCh *value);
	virtual void textEvent(const XMLCh *value);
	virtual void textEvent(const XMLCh *chars, unsigned int length);
	virtual void commentEvent(const XMLCh *value);
	virtual void attributeEvent(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname, const XMLCh *value,
		const XMLCh *typeURI, const XMLCh *typeName);
	virtual void namespaceEvent(const XMLCh *prefix, const XMLCh *uri);
	virtual void atomicItemEvent(const XMLCh *value, const XMLCh *typeURI, const XMLCh *typeName);
	virtual void endEvent();

	virtual Sequence getSequence() const { return seq_; }
private:
	void addNodeToSequence();
private:
	const DynamicContext *context_;
	XmlDocument xmlDoc_;
	int doingSequence_;
	NsEventWriter *writer_;
	NsFullNid rootNid_;
	Sequence seq_;
	bool nextIsRoot_;
};

}

#endif
