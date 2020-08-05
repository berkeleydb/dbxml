//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "UTF8.hpp"

#include <db.h>

#include <string>
#include <sstream>

#include <xqilla/ast/LocationInfo.hpp>
#include <xqilla/exceptions/XQException.hpp>

using namespace DbXml;

//
// jcm - Note that the member variable description is not a std::string
// because in some cases when the XmlException object is deleted an exception
// is thrown by the MSVC6 debug heap. Using strdup and free seems to work ok.
//

XmlException::XmlException(ExceptionCode ec, const std::string &description, const char *file, int line)
	: exceptionCode_(ec),
	  dberr_(0),
	  description_(::strdup(description.c_str())),
	  qFile_(0),
	  qLine_(0),
	  qCol_(0),
	  file_(file),
	  line_(line),
	  text_(0)
{
	describe();
}

XmlException::XmlException(ExceptionCode ec, const char *description, const char *file, int line)
	: exceptionCode_(ec),
	  dberr_(0),
	  description_(::strdup(description)),
	  qFile_(0),
	  qLine_(0),
	  qCol_(0),
	  file_(file),
	  line_(line),
	  text_(0)
{
	describe();
}

XmlException::XmlException(ExceptionCode ec, const XQException &e, const char *file, int line)
	: exceptionCode_(ec),
	  dberr_(0),
	  description_(::strdup(XMLChToUTF8(e.getError()).str())),
	  qFile_(0),
	  qLine_(e.getXQueryLine()),
	  qCol_(e.getXQueryColumn()),
	  file_(file),
	  line_(line),
	  text_(0)
{
	if(e.getXQueryFile() != 0)
		qFile_ = ::strdup(XMLChToUTF8(e.getXQueryFile()).str());

	describe();
}

XmlException::XmlException(int dberr, const char *file, int line)
	: exceptionCode_(DATABASE_ERROR),
	  dberr_(dberr),
	  description_(::strdup(db_strerror(dberr))),
	  qFile_(0),
	  qLine_(0),
	  qCol_(0),
	  file_(file),
	  line_(line),
	  text_(0)
{
	describe();
}

XmlException::XmlException(const XmlException &that)
	: std::exception(that),
	  exceptionCode_(that.exceptionCode_),
	  dberr_(that.dberr_),
	  description_(::strdup(that.description_)),
	  qFile_(that.qFile_ ? ::strdup(that.qFile_) : 0),
	  qLine_(that.qLine_),
	  qCol_(that.qCol_),
	  file_(that.file_),
	  line_(that.line_),
	  text_(::strdup(that.text_))
{}

XmlException::~XmlException() throw()
{
	file_ = 0;
	::free(description_);
	if(qFile_ != 0)
		::free(qFile_);
	if(text_ != 0)
		::free(text_);
}

void XmlException::setLocationInfo(const LocationInfo *info)
{
	if(info->getFile() != 0)
		qFile_ = ::strdup(XMLChToUTF8(info->getFile()).str());
	qLine_ = info->getLine();
	qCol_ = info->getColumn();
}

void XmlException::describe()
{
	std::ostringstream s;
	s << "Error: ";
	s << description_;
	if(qFile_ != 0 || qLine_ != 0) {
		s << ", ";
		if(qFile_ != 0)
			s << qFile_;
		else s << "<query>";
		if(qLine_ != 0) {
			s << ":" << qLine_;
			if(qCol_ != 0)
				s << ":" << qCol_;
		}
	}
	if (file_ != 0) {
		s << " File: ";
		s << file_;

		if (line_ != 0) {
			s << " Line: ";
			s << line_;
		}
	}
	text_ = ::strdup(s.str().c_str());
}

const char *XmlException::what() const throw()
{
	return text_;
}
