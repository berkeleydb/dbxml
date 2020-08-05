//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "QueryExecutionContext.hpp"
#include "ContainerBase.hpp"
#include <float.h>

using namespace DbXml;
using namespace std;

QueryExecutionContext::QueryExecutionContext(QueryContext &context, bool debugging)
	: container_(0),
	  context(context),
	  dc_(0)
{}

QueryExecutionContext::~QueryExecutionContext()
{}

Container *QueryExecutionContext::getContainer()
{
	if(container_ == 0) return 0;

	Container *container = const_cast<ContainerBase*>(container_)->
		getContainer();
	DBXML_ASSERT(container);
	return container;
}
