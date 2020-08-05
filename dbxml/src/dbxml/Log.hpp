//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __LOG_HPP
#define	__LOG_HPP

#include <iostream>
#include <string>

namespace DbXml
{

/**
 * A class that defines static logging and flag checking methods,
 * and can also be implemented by classes wishing to provide the
 * pure virtual logging and flag checking helper methods.
 */
class Log
{
public:
	virtual ~Log();

	enum ImplLogLevel {
		L_NONE =    0x00000000,  ///< No debug LLs are enabled.
		L_DEBUG =   0x00000001,  ///< Program execution tracing messages.
		L_INFO =    0x00000002,  ///< Informational, just for interest.
		L_WARNING = 0x00000004,  ///< For warnings, bad things but recoverable.
		L_ERROR =   0x00000008,  ///< For errors that can't be recovered from.
		L_ALL =     0xFFFFFFFF   ///< All debug LLs are enabled.
	};

	enum ImplLogCategory {
		C_NONE =       0x00000000,
		C_INDEXER =    0x00000001,
		C_QUERY =      0x00000002,
		C_OPTIMIZER =  0x00000004,
		C_DICTIONARY = 0x00000008,
		C_CONTAINER =  0x00000010,
		C_NODESTORE =  0x00000020,
		C_MANAGER =    0x00000040,
		C_ALL =        0xFFFFFFFF
	};

	/// Information on a specific flag
	struct FlagInfo {
		const char *name;
		unsigned int value;
	};

	/** @name Virtual Methods */
	///@{

	virtual void log(ImplLogCategory c, ImplLogLevel l,
			 const std::ostringstream &s) const = 0;
	virtual void log(ImplLogCategory c, ImplLogLevel l,
			 const std::string &s) const = 0;
	virtual void checkFlags(const FlagInfo *flag_info, const char *function,
				unsigned int flags, unsigned int mask) const = 0;

	///@}

	/** @name Logging */
	///@{

	static void setLogLevel(ImplLogLevel level, bool enabled);
	static void setLogCategory(ImplLogCategory category, bool enabled);
	static bool isLogEnabled(ImplLogCategory category, ImplLogLevel level);

	static void log(DB_ENV *environment, ImplLogCategory category,
			ImplLogLevel level, const char *context,
			const char *message);
	static void log(DB_ENV *environment, ImplLogCategory category,
			ImplLogLevel level, const char *message);

	//@}

	/** @name Flag Checking */
	///@{

	// Numbers in square brackets donate flags with the same value

	/*
	 * [2]DBXML_INDEX_NODES, DB_READ_UNCOMMITTED
	 * DB_NOMMAP, [3]DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, [1]DB_CREATE,
	 * [4]DB_EXCL, DB_RDONLY, DBXML_TRANSACTIONAL, DBXML_ALLOW_VALIDATION
	 */
	static const FlagInfo open_container_flag_info[];

	/*
	 * [1]DBXML_ADOPT_DBENV, [2]DBXML_ALLOW_EXTERNAL_ACCESS,
	 * [3]DBXML_ALLOW_AUTO_OPEN
	 */
	static const FlagInfo construct_manager_flag_info[];

	/*
	 * [3]DB_SALVAGE, [1]DB_AGGRESSIVE
	 * DB_READ_UNCOMMITTED, DB_RMW, DBXML_LAZY_DOCS, DB_READ_COMMITTED
	 * DB_TXN_NOSYNC, DB_TXN_SNAPSHOT, [4]DB_TXN_NOWAIT, DBXML_GEN_NAME
	 */
	static const FlagInfo misc_flag_info[];

	// The flag_info array's last element has a name and value equaling 0
	static void checkFlags(const char *function,
			       const FlagInfo *flag_info,
			       unsigned int flags,
			       unsigned int mask);

	///@}
};

}

#endif

