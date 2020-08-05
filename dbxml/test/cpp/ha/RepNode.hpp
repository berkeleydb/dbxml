/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */

#ifndef __REPNODE_HPP
#define __REPNODE_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <db.h>
#include <dbxml/DbXml.hpp>

/*
 * Use config objects to manager the config flags.
 */
class Config {
public:
	Config()
		: verbose_(false)
	{}

	bool verbose_;
};

class OutputMutex;

/*
 * RepNode is a node object. TestCases could define a couple of rep nodes for 
 * test. TestCases could let a node start running by calling RepNode::start() 
 * and stop it by calling Repnode::stop().
 */
class RepNode {

/*
 * AppData is a class for eventCallback function(). It contains the states which
 * are can be changed asynchronous by rep manager.
 */
class AppData {
public:
	AppData() {
		reset();
	}
	// reset method is provided for node start()/stop().
	void reset() {
		isMaster_ = false;
		isStartupDone_ = false;
	}
	bool isMaster()			{ return isMaster_; }
	bool isStartupDone()		{ return isStartupDone_; }

	// Note: setMaster() and setStartupDone() only can be called by
	// RepNode::eventCallback() to update node state.
	void setMaster(bool value)	{ isMaster_ = value; }
	void setStartupDone(bool value)	{ isStartupDone_ = value; }

private:
	bool isMaster_;
	bool isStartupDone_;
};

public:
	RepNode(u_int32_t startPolicy, const std::string& envHome,
		const std::string& host, int port, u_int32_t priority,
		std::vector<RepNode *>& group, const Config& config);
	virtual ~RepNode();

	// Util methods
	bool equal(const RepNode& node);
	void clearDbEnv();
	void throwIfUnExpected(DbXml::XmlException &xe);
	void throwException(const std::string& msg);

	// Get members/states
	const std::string& getEnvHome()		{ return envHome_; }
	DbXml::XmlManager& getManager()		{ return *mgr_; }
	u_int32_t getStartPolicy()		{ return startPolicy_; }
	bool isMaster()				{ return appData_.isMaster(); }
	size_t getGroupSize()			{ return group_.size(); }
	u_int32_t getID() const			{ return id_; }
	DbXml::XmlContainer& getContainer();

	// Running manager
	void start();
	void start(u_int32_t startPolicy);
	void stop();
	
	// Output-related
	void printMsg(const std::string& str);
	void printMsg(std::ostringstream& oss);

protected:
	void yield();

private:
	// Node property
	u_int32_t startPolicy_;
	std::string envHome_;
	std::string host_;
	int port_;
	u_int32_t priority_;
	std::vector<RepNode *>& group_;	// Reference of nodes vector
	const Config& config_;		// Running config
	u_int32_t id_;			// Unique id in the group.

	// DbXml-related members
	std::string containerName_;
	DB_ENV *dbenv_;
	DbXml::XmlManager *mgr_;
	DbXml::XmlContainer cont_;
	u_int32_t envFlags_;
	u_int32_t cacheSize_;

	// Callback-related members
	AppData appData_;
	static void eventCallback(DB_ENV* dbenv, u_int32_t which, void *info);

	// Private util functions
	void openContainer();
	void init();		// Init the DbEnv
	void createDbEnv(DB_ENV** pDbEnv, u_int32_t flag);

	// print mutex
	static OutputMutex printMutex;
};

/*
 * TestCase Framework. The framework provide some common test steps and
 * keep the group of nodes.
 */
class TestCase {
public:
	TestCase(std::string testName, const Config& config)
		: testName_(testName),
		  config_(config)
	{}
	virtual ~TestCase() {}
	
	virtual void run() = 0;
	virtual bool verify() = 0;

	std::string getName() { return testName_; }
	
	std::vector<RepNode *> group_;

private:
	std::string testName_;
protected:
	const Config& config_;
};

/*
 * OutputMutex is a light class to manager a mutex for ordinal output in
 * multi-proccess program.
 */
class OutputMutex {
public:
	OutputMutex() {
		lock_.l_type = F_WRLCK;
		lock_.l_start = 0;
		lock_.l_whence = SEEK_SET;
		lock_.l_len = 0;

		unlock_.l_type = F_UNLCK;
		unlock_.l_start = 0;
		unlock_.l_whence = SEEK_SET;
		unlock_.l_len = 0;

		fd = open("OutputMutex.lock", O_CREAT | O_TRUNC | O_RDWR, 0666);
	}

	~OutputMutex() {
		close(fd);
	}

	void lock() {
		while(fcntl(fd, F_SETLKW, &lock_))
			yield();
	}

	void unlock() {
		while(fcntl(fd, F_SETLKW, &unlock_))
			yield();
	}

private:
	// This locking may or may not work on NFS-mounted file systems.
	struct flock lock_, unlock_;
	int fd;
	void yield() {	usleep(10000); }
};

#endif //__REPNODE_HPP
