/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */
 
#ifndef __NORMALTESTCASE_HPP
#define __NORMALTESTCASE_HPP

#include "RepNode.hpp"

/*
 * NormalTestCase emulates a NormalTestCase Scenario for HA testing.
 *
 *  Test steps:
 *   1. Setup 1 master and 2 replicas.
 *
 *   2.  Master put some documents and self-destructs. Then the
 *      replicas would hold a election and elect the new master.
 *
 *   3. The new master continue to put documents until reach the defined max
 *      document number. After sync all the documents, the replica exit.
 *      And the new master wait for the former master comes back.
 *
 *   4. The former master comes back as a replica, syncs up to current master.
 *      After sync all the documents, the former master exit.
 *
 *   5. The new master wait for a while and exit.
 *
 * Verify:
 *   1. Check whether all the nodes have 200 documents.
 *   2. Compare are put into container collectly and verify that it
 *      appears at the replica.
 *
 */
 
class NormalRepNode : public RepNode {
public:
	NormalRepNode(u_int32_t startPolicy, const std::string& envHome,
		      const std::string& host, int port, u_int32_t priority,
		      std::vector<RepNode *>& group, const Config& config)
		: RepNode(startPolicy, envHome, host, port, priority, group, config),
		  docNum_(0)
	{}
	size_t getDocNum() { return docNum_; }
	void masterOperations();
	void clientOperations();

private:
	size_t docNum_;
};


class NormalTestCase : public TestCase {
public:
	NormalTestCase(const Config& config);
	virtual void run();
	virtual bool verify();

private:
	size_t docNum_;
	// It's possible that this scenario simulation would failed if the
	// machine is too slow or there are too many nodes. Increase the
	// weighting then.
	const int weighting_;
	const int timeoutSec_;

	NormalRepNode master0_;
	NormalRepNode client1_;
	NormalRepNode client2_;

	void masterProcess(NormalRepNode& node);
	void clientProcess(NormalRepNode& node);

	void waitForAwhile(int n);
	void yield() {	usleep(100000); }
};

#endif //__NORMALTESTCASE_HPP
