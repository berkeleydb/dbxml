//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLDEBUGLISTENER_HPP
#define __XMLDEBUGLISTENER_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"

namespace DbXml
{

class DBXML_EXPORT XmlStackFrame
{
public:
	virtual ~XmlStackFrame() {}

	virtual const char *getQueryFile() const = 0;
	virtual int getQueryLine() const = 0;
	virtual int getQueryColumn() const = 0;

	virtual XmlResults query(const std::string &queryString) const = 0;
	virtual std::string getQueryPlan() const = 0;

	virtual const XmlStackFrame *getPreviousStackFrame() const = 0;
};

class DBXML_EXPORT XmlDebugListener
{
public:
	virtual ~XmlDebugListener() {}

	virtual void start(const XmlStackFrame *stack) {}
	virtual void end(const XmlStackFrame *stack) {}
	virtual void enter(const XmlStackFrame *stack) {}
	virtual void exit(const XmlStackFrame *stack) {}
	virtual void error(const XmlException &error, const XmlStackFrame *stack) { throw error; }

protected:
	XmlDebugListener() {}
private:
	XmlDebugListener(const XmlDebugListener &);
	XmlDebugListener &operator=(const XmlDebugListener &);
};

}

#endif

