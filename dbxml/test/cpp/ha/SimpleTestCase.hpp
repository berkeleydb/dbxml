/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */
 
#ifndef __SIMPLETESTCASE_HPP
#define __SIMPLETESTCASE_HPP

#include "RepNode.hpp"

/*
 *
 * Simple testCase emulates a simple scenario for HA testing.
 *
 * Test steps:
 *   1. Start out with just a master. Do putDocument to add some documents into
 *      the database.
 *
 *   2. Add a replica, and connect it to the master.  That should cause it
 *      to "sync up" with the master.
 *
 *   3. Add some number(100) of documents at the master. After putting done,
 *      wait for client sync done and kill both master and client.
 *
 * Verify:
 *   1. Check whether both master and client has 200 documents.
 *   2. Compare   are put into container collectly and verify that it
 *      appears at the replica.
 */

class SimpleRepNode : public RepNode {
public:
	SimpleRepNode(u_int32_t startPolicy, const std::string& envHome,
		      const std::string& host, int port, u_int32_t priority,
		      std::vector<RepNode *>& group, const Config& config)
		: RepNode(startPolicy, envHome, host, port, priority, group, config)
	{}

	void putDocuments(size_t num);
	size_t getDocNum();

private:
	void yield() { usleep(10000); }
};

class SimpleTestCase : public TestCase {
public:
	SimpleTestCase(const Config& config);
	virtual void run();
	virtual bool verify();

private:
	size_t documentNum_;
	size_t totalDocumentNum_;
	SimpleRepNode master_;
	SimpleRepNode client_;

	void yield() {	usleep(10000); }
};

#endif //__SIMPLETESTCASE_HPP
