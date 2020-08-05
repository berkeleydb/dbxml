/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */

#include <iostream>
#include <string>
#include <sstream>

#include "SimpleTestCase.hpp"

using namespace DbXml;
using namespace std;

SimpleTestCase::SimpleTestCase(const Config& config)
	: TestCase("Simple Scenario", config),
	  documentNum_(100),
	  totalDocumentNum_(0),
	  master_(DB_REP_MASTER, "master", "localhost", 5001, 200, group_, config_),
	  client_(DB_REP_CLIENT, "client", "localhost", 5002, 190, group_, config_)
{
	// Clear existing DbEnv and Container.
	master_.clearDbEnv();
	client_.clearDbEnv();
}

void SimpleTestCase::run()
{
	ostringstream o;
	// Master: Init, create or open container
	master_.printMsg("is startting... ");
	master_.start();
	master_.printMsg("startup done.");

	// Master: insert some documents
	o << "is putting " << documentNum_ << " documents...";
	master_.printMsg(o);
	master_.putDocuments(documentNum_);
	master_.printMsg("put document done.");

	// Client : Init and wait for "start-up done"
	client_.printMsg("is startting... ");
	client_.start();
	client_.printMsg("startup done.");
	o << "has sync " << client_.getDocNum() << " documents";
	client_.printMsg(o);

	// Master: continue insering some documents.
	o << "is putting " << documentNum_ << " documents...";
	master_.printMsg(o);
	master_.putDocuments(documentNum_);
	totalDocumentNum_ = master_.getDocNum();
	master_.printMsg("put document done.");

	// Wait for sync done.
	size_t docNum = 0;
	while (docNum < totalDocumentNum_) {
		docNum = client_.getDocNum();
		yield();
	}
	o << "has sync " << client_.getDocNum() << " documents" << endl;
	client_.printMsg(o);

	// Exit running.
	client_.stop();
	master_.stop();
}

bool SimpleTestCase::verify()
{
	// Verify process would deal with nodes one by one, and the rep_mgr is
	// not working now. so Transaction is needless here.

	// Verify the document nums.
	size_t masterDocNum = master_.getContainer().getNumDocuments();
	size_t clientDocNum = client_.getContainer().getNumDocuments();
	if (masterDocNum != totalDocumentNum_) {
		cerr << "Error: master doc numbers !=" << totalDocumentNum_ << endl;
		cerr << "Verify failed." << endl;
		return false;
	}
	if (clientDocNum != totalDocumentNum_) {
		cerr << "Error: client doc numbers !=" << totalDocumentNum_ << endl;
		cerr << "Verify failed." << endl;
		return false;
	}

	// Compare documents' content one by one.
	XmlResults res1 = master_.getContainer().getAllDocuments(0);
	XmlResults res2 = client_.getContainer().getAllDocuments(0);

	bool passed = true;
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
		}
	}
	return passed;
}

// Put number of documents into container.
void SimpleRepNode::putDocuments(size_t num)
{
	XmlManager& mgr = getManager();
	XmlContainer& cont = getContainer();
	XmlUpdateContext uc = mgr.createUpdateContext();

	for(int retry = 1000; retry; retry--) {
		XmlTransaction txn = mgr.createTransaction();

		try {
		ostringstream o;
		printMsg("is putting document.");
		for (size_t i = 0; i < num; i++) {
			ostringstream o;
			o << "<node>"
			  << "<name>" <<  getEnvHome() << "</name>"
			  << "<doc_id>" << i << "</doc_id>"
			  << "</node>";

			// Put documents
			cont.putDocument(txn, "",  o.str(), uc, DBXML_GEN_NAME);
		}
		txn.commit();
		printMsg("put document done.");
		return;
		} catch (XmlException &xe) {
			txn.abort();
			throwIfUnExpected(xe);
			yield();
		}
	}
	throwException("putDocuments() retry limit exceeded.");
}

size_t SimpleRepNode::getDocNum()
{
	for(int retry = 1000; retry; retry--) {
		XmlTransaction txn = getManager().createTransaction();

		try {

		size_t ret = getContainer().getNumDocuments(txn);
		txn.commit();
		return ret;

		} catch (XmlException &xe) {
			txn.abort();
			throwIfUnExpected(xe);
			yield();
		}
	}
	throwException("getDocNum() retry limit exceeded.");
	return 0;
}
