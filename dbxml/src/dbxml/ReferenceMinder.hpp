//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __REFERENCEMINDER_HPP
#define	__REFERENCEMINDER_HPP

#include "DbXmlInternal.hpp"
#include <dbxml/XmlContainer.hpp>

#include "DocID.hpp"

#include <xercesc/util/XMLUri.hpp>

#include <map>

namespace DbXml
{

class Document;

struct DocMapKey
{
	DocMapKey(int cid, DocID i)
		: containerID(cid), id(i) {}

	bool operator<(const DocMapKey &o) const;

	const int containerID;
	DocID id;
};

class ReferenceMinder
{
public:
	ReferenceMinder() {}
	~ReferenceMinder();

	void addContainer(Container *container);
	void removeContainer(int cid);

	void addDocument(Document *doc);
	void removeDocument(Document *doc);
	/// Returns 0 if none found
	Document *findDocument(const XMLCh *uri);
	/// Returns 0 if none found
	Document *findDocument(int cid, DocID id);

	void resetMinder();

	std::map<int, XmlContainer> getContainers() const;
private:
	ReferenceMinder(const ReferenceMinder&);
	ReferenceMinder &operator=(const ReferenceMinder&);

	struct xmlchCompare
	{
		bool operator()(const XMLCh *s1, const XMLCh *s2) const;
	};

	typedef std::map<int, XmlContainer> ContainerMap;
	typedef std::map<DocMapKey, Document*> ID2DocumentMap;
	typedef std::map<const XMLCh*, Document*, xmlchCompare> URI2DocumentMap;

	ContainerMap containers_;
	ID2DocumentMap ids2documents_;
	URI2DocumentMap uris2documents_;
};

}

#endif
