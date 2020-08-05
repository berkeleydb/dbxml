//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#include "ReferenceMinder.hpp"
#include "Document.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xqilla/utils/XPath2Utils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

ReferenceMinder::~ReferenceMinder()
{
	resetMinder();
}

void ReferenceMinder::addContainer(Container *container)
{
	containers_.insert(ContainerMap::value_type(
				   container->getContainerID(),
				   container));
}

void ReferenceMinder::removeContainer(int cid)
{
	ContainerMap::iterator i = containers_.find(cid);
	if(i != containers_.end()) containers_.erase(i);
}

void ReferenceMinder::addDocument(Document *doc)
{
	if(doc->getContainerID()) {
		DocMapKey key(doc->getContainerID(), doc->getID());
		if(ids2documents_.insert(ID2DocumentMap::value_type(key, doc)).second) {
			doc->addReferenceMinder(this);
		}
	} else if (doc->getDocumentURI()) {
		// don't insert documents without a URI; they are
		// constructed
		if(uris2documents_.insert(URI2DocumentMap::value_type(doc->getDocumentURI(), doc)).second) {
			doc->addReferenceMinder(this);
		}
	}
}

void ReferenceMinder::removeDocument(Document *doc)
{
	if(doc->getContainerID()) {
		ids2documents_.erase(DocMapKey(doc->getContainerID(),
					       doc->getID()));
	} else {
		uris2documents_.erase(doc->getDocumentURI());
	}
}

Document *ReferenceMinder::findDocument(const XMLCh *uri)
{
	URI2DocumentMap::iterator i = uris2documents_.find(uri);
	if(i != uris2documents_.end()) {
		return i->second;
	}
	return 0;
}

Document *ReferenceMinder::findDocument(int cid, DocID id)
{
	ID2DocumentMap::iterator i = ids2documents_.find(DocMapKey(cid, id));
	if(i != ids2documents_.end()) {
		return i->second;
	}
	return 0;
}

void ReferenceMinder::resetMinder()
{
	// Let the Document objects know that we don't hold a
	// reference to them anymore
	ID2DocumentMap::iterator i = ids2documents_.begin();
	for(; i != ids2documents_.end(); ++i) {
		i->second->removeReferenceMinder(this);
	}
	URI2DocumentMap::iterator j = uris2documents_.begin();
	for(; j != uris2documents_.end(); ++j) {
		j->second->removeReferenceMinder(this);
	}

	containers_.clear();
	ids2documents_.clear();
	uris2documents_.clear();
}

std::map<int, XmlContainer> ReferenceMinder::getContainers() const
{
	return containers_;
}

bool DocMapKey::operator<(const DocMapKey &o) const
{
	if(id < o.id) return true;
	if(id > o.id) return false;
	return containerID < o.containerID;
}

bool ReferenceMinder::xmlchCompare::operator()(const XMLCh *s1, const XMLCh *s2) const
{
	return XPath2Utils::compare(s1, s2) < 0;
}
