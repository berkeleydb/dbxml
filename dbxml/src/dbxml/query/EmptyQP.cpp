//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "EmptyQP.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

NodeIterator *EmptyQP::createNodeIterator(DynamicContext *context) const
{
	return new EmptyIterator(this);
}

bool EmptyQP::isSubsetOf(const QueryPlan *o) const
{
	return true;
}

QueryPlan *EmptyQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	QueryPlan *result = new (mm) EmptyQP(flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void EmptyQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

string EmptyQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<EmptyQP/>" << endl;

	return s.str();
}

string EmptyQP::toString(bool brief) const
{
	ostringstream s;

	s << "E";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

EmptyIterator::EmptyIterator(const LocationInfo *location)
	: NodeIterator(location)
{
}

bool EmptyIterator::next(DynamicContext *context)
{
	return false;
}

bool EmptyIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	return false;
}

NodeIterator::Type EmptyIterator::getType() const
{
	return (Type)-1;
}

int EmptyIterator::getContainerID() const
{
	return 0;
}

DocID EmptyIterator::getDocID() const
{
	return DocID();
}

const NsNid EmptyIterator::getNodeID() const
{
	return NsNid();
}

const NsNid EmptyIterator::getLastDescendantID() const
{
	return NsNid();
}

u_int32_t EmptyIterator::getNodeLevel() const
{
	return (u_int32_t)-1;
}

u_int32_t EmptyIterator::getIndex() const
{
	return (u_int32_t)-1;
}

bool EmptyIterator::isLeadingText() const
{
	return false;
}

DbXmlNodeImpl::Ptr EmptyIterator::asDbXmlNode(DynamicContext *context)
{
	return 0;
}

