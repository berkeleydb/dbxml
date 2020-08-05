//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "dbxml/XmlContainer.hpp"
#include "UpdateContext.hpp"
#include "Container.hpp"

using namespace DbXml;

XmlUpdateContext::XmlUpdateContext()
	: updateContext_(0)
{
}

XmlUpdateContext::XmlUpdateContext(UpdateContext *uc)
	: updateContext_(uc)
{
	if (updateContext_)
		updateContext_->acquire();
}

XmlUpdateContext::~XmlUpdateContext()
{
	if (updateContext_)
		updateContext_->release();
}

XmlUpdateContext::XmlUpdateContext(const XmlUpdateContext &o)
	: updateContext_(o.updateContext_)
{
	if (updateContext_ != 0)
		updateContext_->acquire();
}

XmlUpdateContext &XmlUpdateContext::operator=(const XmlUpdateContext &o)
{
	if (this != &o && updateContext_ != o.updateContext_) {
		if (updateContext_ != 0)
			updateContext_->release();
		updateContext_ = o.updateContext_;
		if (updateContext_ != 0)
			updateContext_->acquire();
	}
	return *this;
}
