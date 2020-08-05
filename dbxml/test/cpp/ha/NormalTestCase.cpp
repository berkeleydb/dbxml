/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */

#include <errno.h>
#include <cstdlib>

#include "NormalTestCase.hpp"
using namespace DbXml;
using namespace std;

static const size_t documentNum = 100;
static const size_t maxDocumentNum = 300;

NormalTestCase::NormalTestCase(const Config& config)
	: TestCase("Normal Scenario", config),
	  docNum_(0),
	  weighting_(2),
	  timeoutSec_(120),
	  master0_(DB_REP_MASTER, "master", "localhost", 6010, 100, group_, config_),
	  client1_(DB_REP_CLIENT, "client1", "localhost", 6011, 80, group_, config_),
	  client2_(DB_REP_CLIENT, "client2", "localhost", 6012, 60, group_, config_)
{
	// Clear the existing environment and database if any.
	for (size_t i = 0; i < group_.size(); i++)
		group_[i]->clearDbEnv();
}

void NormalTestCase::clientProcess(NormalRepNode& node)
{
	node.printMsg("Start...");
	node.start(DB_REP_CLIENT);
	node.printMsg("Done.");

	bool state = node.isMaster();
	string name = node.getEnvHome();

	while (true) {
		if (state != node.isMaster()) {
			state = node.isMaster();
			if(node.isMaster())
				node.printMsg("raise as master.");
		}

		size_t docNum = node.getDocNum();
		if (node.isMaster()) {
			if (docNum < maxDocumentNum) {
				node.masterOperations();
			} else {
				waitForAwhile(10);
				break; // the exit point
			}
		} else {
			if (docNum < maxDocumentNum) {
				node.clientOperations();
			} else {
				waitForAwhile(10);
				break; // the exit point
			}
		}

		yield();
	}

	// exit.
	node.printMsg("exit...");
	node.stop();
}

void NormalTestCase::masterProcess(NormalRepNode &node)
{
	// Master init and input some documents into container;
	node.printMsg("Start...");
	node.start();
	node.printMsg("Done.");

	// Wait for clients setup.
	waitForAwhile(10);

	// Input some documents then self-destructs.
	int retry = 1000;
	for (; retry; retry--) {
		node.masterOperations();
		break;
	}
	if (!retry)
		node.throwException("masterOperations() retry limit exceeded.");

	waitForAwhile(5);
	node.printMsg("exit...");
	node.stop();

	// Wait for a while and comes back as a replica
	waitForAwhile(10);
	return clientProcess(node);
}

void NormalTestCase::run()
{
	vector<pid_t> pids;
	pids.reserve(group_.size());
	pids.resize(group_.size());

	// fork a process for each node running.
	size_t i;
	try {
		for (i= 0; i < pids.size(); i++) {
			if ((pids[i] = fork()) == 0) {
				alarm(timeoutSec_);
				NormalRepNode& node = *(NormalRepNode *)group_[i];
				if (node.getStartPolicy() == DB_REP_MASTER) {
					masterProcess(node);
				} else {
					clientProcess(node);
				}
				exit(0);
			} else
			        waitForAwhile(1);
		}
	} catch (XmlException &xe) {
		group_[i]->printMsg(xe.what());
		exit(1);
	}
	for (size_t i = 0; i < pids.size(); i++)
		waitpid(pids[i], NULL, 0);
}

bool NormalTestCase::verify()
{
	// Verify process would deal with nodes one by one, and the rep_mgr is
	// not working now. so Transaction is needless here.

	// Verify the document nums.
	bool passed = true;
	for (size_t i = 0; i < group_.size(); i++) {
		size_t docNum = group_[i]->getContainer().getNumDocuments();
		if (docNum != maxDocumentNum) {
			cerr << "Error: doc numbers doesn't match:" << endl
			     << "Doc number of Node" << i << " != " << docNum
			     << endl;
			passed = false;
		}
	}

	if (!passed)
		return passed;

	// Compare documents' content one by one.
	XmlResults res1 = group_[0]->getContainer().getAllDocuments(0);

	for (size_t i = 1; i < group_.size(); i++) {
		XmlResults res2 = group_[i]->getContainer().getAllDocuments(0);
		while (true) {
			XmlValue val1, val2;
			res1.next(val1);
			res2.next(val2);

			if (val1.isNull())
				break;

			if (val1.asString() != val2.asString()) {
				cerr << "Error: " << val1.asString() << " != "
				     << val2.asString() << endl;
				passed = false;
				return false;
			}
		}
	}
	return passed;
}

void NormalTestCase::waitForAwhile(int n)
{
	sleep(n * weighting_);
}

void NormalRepNode::masterOperations()
{
	XmlManager& mgr = getManager();
	XmlContainer& cont = getContainer();
	XmlUpdateContext uc = mgr.createUpdateContext();
	int num = documentNum;

	XmlTransaction txn = mgr.createTransaction();
	try {
		ostringstream o;
		printMsg("is putting document.");
		for (int i = 0; i < num && (docNum_+i) < maxDocumentNum; i++) {
			ostringstream o;
			o << "<node>"
			  << "<name>" << getEnvHome() << "</name>"
			  << "<doc_id>" << i << "</doc_id>"
			  << "</node>";

			// Put documents
			cont.putDocument(txn, "",  o.str(), uc, DBXML_GEN_NAME);
		}
		docNum_ = cont.getNumDocuments(txn);
		txn.commit();
		printMsg("put document done.");
	} catch (XmlException &xe) {
		txn.abort();
		// Deal with permission denied exception
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR &&
		    xe.getDbErrno() == EACCES) {
			yield();
		} else
			throwIfUnExpected(xe);
	}
}

void NormalRepNode::clientOperations()
{
	XmlTransaction txn = getManager().createTransaction();
	try {
		// Save the new node state.
		docNum_ = getContainer().getNumDocuments(txn);
		txn.commit();
	} catch (XmlException &xe) {
		txn.abort();
		throwIfUnExpected(xe);
	}
}
