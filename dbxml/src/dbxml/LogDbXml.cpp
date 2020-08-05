//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/DbXmlFwd.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "Log.hpp"

#include <db.h>

#include <iostream>
#include <cstring>
#include <sstream>

using namespace DbXml;
using namespace std;

static unsigned int globalDebugLevels = Log::L_ERROR;
static unsigned int globalDebugCategories = Log::C_ALL;

Log::~Log()
{
}

void Log::setLogLevel(ImplLogLevel level, bool enabled)
{
	if (enabled && globalDebugLevels == L_ALL)
		globalDebugLevels = L_NONE;
	
	if (level == L_NONE) {
		globalDebugLevels = level;
	} else {
		if (enabled) {
			globalDebugLevels |= level;
		} else {
			globalDebugLevels &= ~level;
		}
	}
}

void Log::setLogCategory(ImplLogCategory category, bool enabled)
{
	if (enabled && globalDebugCategories == C_ALL)
		globalDebugCategories = C_NONE;

	if (category == C_NONE) {
		globalDebugCategories = category;
	} else {
		if (enabled) {
			globalDebugCategories |= category;
		} else {
			globalDebugCategories &= ~category;
		}
	}
}

static const char *categoryName(Log::ImplLogCategory category)
{
	switch (category) {
	case Log::C_NONE:
		return "None      ";
	case Log::C_INDEXER:
		return "Indexer   ";
	case Log::C_QUERY:
		return "Query     ";
	case Log::C_OPTIMIZER:
		return "Optimizer ";
	case Log::C_DICTIONARY:
		return "Dictionary";
	case Log::C_CONTAINER:
		return "Container ";
	case Log::C_NODESTORE:
		return "Nodestore ";
	case Log::C_MANAGER:
		return "Manager   ";
	case Log::C_ALL:
		return "All       ";
	}
	return "None";
}

bool Log::isLogEnabled(ImplLogCategory category, ImplLogLevel level)
{
	return (globalDebugLevels&level && globalDebugCategories&category);
}

void Log::log(DB_ENV *environment, ImplLogCategory category, ImplLogLevel level, const char *context, const char *message)
{
	if (isLogEnabled(category, level)) {
		if (environment == 0) {
			std::cerr
			<< categoryName(category)
			<< " - "
			<< (context == 0 ? "none" : context)
			<< " - "
			<< message
			<< std::endl;
		} else {
			// db aborts if you try to log a message longer than an internal buffer,
			// so we must be very careful not to trip that abort. In db.4.1.24 the buffer
			// is 2048 bytes.
			//
			const char *p1 = categoryName(category);
			const char *p2 = (context == 0 ? "none" : context);
			size_t l = strlen(message);
			size_t r = 2048 - (strlen(p1) + 3) - (strlen(p2) + 3);
			if (l > r) {
				((char *)message)[r - 4] = '.';
				((char *)message)[r - 3] = '.';
				((char *)message)[r - 2] = '.';
				((char *)message)[r - 1] = '\0';
			}
			environment->errx(environment, "%s - %s - %s", categoryName(category),
					  (context == 0 ? "none" : context), message);
		}
	}
}

void Log::log(DB_ENV *environment, ImplLogCategory category, ImplLogLevel level, const char *message)
{
	if (isLogEnabled(category, level)) {
		if (environment == 0) {
			std::cerr
			<< categoryName(category)
			<< " - "
			<< message
			<< std::endl;
		} else {
			// db aborts if you try to log a message longer than an internal buffer,
			// so we must be very careful not to trip that abort. In db.4.1.24 the buffer
			// is 2048 bytes.
			//
			const char *p1 = categoryName(category);
			size_t l = strlen(message);
			size_t r = 2048 - (strlen(p1) + 3);
			if (l > r) {
				((char *)message)[r - 4] = '.';
				((char *)message)[r - 3] = '.';
				((char *)message)[r - 2] = '.';
				((char *)message)[r - 1] = '\0';
			}
			environment->errx(environment, "%s - %s", categoryName(category), message);
		}
	}
}

string flagsAsText(const Log::FlagInfo *flag_info, u_int32_t flags)
{
	if(flags == 0) return "none";

	bool needsSeparator = false;
	ostringstream oss;

	while(flag_info->name != 0) {
		if(flags & flag_info->value) {
			if(needsSeparator) oss << "|";
			oss << flag_info->name;
			flags &= ~flag_info->value;
			needsSeparator = true;
		}
		++flag_info;
	}

	if(flags) {
		if(needsSeparator) oss << "|";
		oss << flags;
	}

	return oss.str();
}

void Log::checkFlags(const char *function,
		     const FlagInfo *flag_info, unsigned int flags,
		     unsigned int mask)
{
	if (flags&~mask) {
		ostringstream s;
		s	<< "Flags check failed for "
			<< function
			<< ". Expected some combination of '"
			<< flagsAsText(flag_info, mask)
			<< "', but was passed '"
			<< flagsAsText(flag_info, flags)
			<< "'.";
		throw XmlException(XmlException::INVALID_VALUE, s.str());
	}
	if (flag_info == open_container_flag_info) {
		if (flags & DBXML_INDEX_NODES && flags & DBXML_NO_INDEX_NODES)
			throw XmlException(
				XmlException::INVALID_VALUE,
				"Flags check failure: cannot specify both DBXML_INDEX_NODES and DBXML_NO_INDEX_NODES");
	}
}

const Log::FlagInfo Log::open_container_flag_info[] = {
	{"DBXML_INDEX_NODES", DBXML_INDEX_NODES},
	{"DBXML_NO_INDEX_NODES", DBXML_NO_INDEX_NODES},
	{"DBXML_TRANSACTIONAL", DBXML_TRANSACTIONAL},
	{"DB_READ_UNCOMMITTED", DB_READ_UNCOMMITTED},
	{"DB_NOMMAP", DB_NOMMAP},
	{"DB_THREAD", DB_THREAD},
	{"DBXML_CHKSUM", DBXML_CHKSUM},
	{"DBXML_ENCRYPT", DBXML_ENCRYPT},
	{"DB_CREATE", DB_CREATE},
	{"DB_EXCL", DB_EXCL},
	{"DB_RDONLY", DB_RDONLY},
	{"DBXML_ALLOW_VALIDATION", DBXML_ALLOW_VALIDATION},
	{"DB_TXN_NOT_DURABLE", DB_TXN_NOT_DURABLE},
	{"DB_MULTIVERSION", DB_MULTIVERSION},
	{"DBXML_STATISTICS", DBXML_STATISTICS},
	{"DBXML_NO_STATISTICS", DBXML_NO_STATISTICS},
	{0, 0}
};

const Log::FlagInfo Log::construct_manager_flag_info[] = {
	{"DBXML_ADOPT_DBENV", DBXML_ADOPT_DBENV},
	{"DBXML_ALLOW_EXTERNAL_ACCESS", DBXML_ALLOW_EXTERNAL_ACCESS},
	{"DBXML_ALLOW_AUTO_OPEN", DBXML_ALLOW_AUTO_OPEN},
	{0, 0}
};

const Log::FlagInfo Log::misc_flag_info[] = {
	{"DB_SALVAGE", DB_SALVAGE},
	{"DB_AGGRESSIVE", DB_AGGRESSIVE},
	{"DB_READ_UNCOMMITTED", DB_READ_UNCOMMITTED},
	{"DB_RMW", DB_RMW},
	{"DB_READ_COMMITTED", DB_READ_COMMITTED},
	{"DBXML_LAZY_DOCS", DBXML_LAZY_DOCS},
	{"DB_TXN_NOSYNC", DB_TXN_NOSYNC},
	{"DB_TXN_NOWAIT", DB_TXN_NOWAIT},
	{"DB_TXN_SNAPSHOT", DB_TXN_SNAPSHOT},
	{"DBXML_GEN_NAME", DBXML_GEN_NAME},
	{"DBXML_REVERSE_ORDER", DBXML_REVERSE_ORDER},
	{"DBXML_INDEX_VALUES", DBXML_INDEX_VALUES},
	{"DBXML_NO_INDEX_NODES", DBXML_NO_INDEX_NODES},
	{"DBXML_CACHE_DOCUMENTS", DBXML_CACHE_DOCUMENTS},
	{"DBXML_WELL_FORMED_ONLY", DBXML_WELL_FORMED_ONLY},
	{"DBXML_DOCUMENT_PROJECTION", DBXML_DOCUMENT_PROJECTION},
	{0, 0}
};
