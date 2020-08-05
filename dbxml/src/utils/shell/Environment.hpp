//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ENVIRONMENT_HPP
#define	__ENVIRONMENT_HPP

#include "Shell.hpp"
#include "dbxml/DbXml.hpp"
#include "common_utils.h"
#include <vector>
#include <string>


class Environment : private DbXml::SignalListener
{
public:
	Environment(DbXml::XmlManager &db, DbXml::SigBlock &sigblock);
	virtual ~Environment();

	virtual DbXml::SigBlock &sigBlock();

	virtual DbXml::XmlManager &db();
	virtual DbXml::XmlTransaction *&txn();
	virtual DbXml::XmlQueryContext &context();
	virtual DbXml::XmlUpdateContext &uc();
	virtual DbXml::XmlContainer *&container();
	virtual DbXml::XmlQueryExpression *&query();
	virtual DbXml::XmlResults *&results();

	virtual std::string &streamName();
	virtual int &lineNo();

	virtual bool &documentProjection();
	virtual bool &interactive();
	virtual bool &ignoreErrors();
	virtual bool &transactions();
	virtual bool &debugging();
	virtual bool &verbose();
	virtual bool &quit();

	virtual const Shell *&shell();

	void testContainer();
	void deleteContainer();

	void testQuery();
	void deleteQuery();

	void testResults();
	void deleteResults();

	void testTransaction();
	void commitTransaction();
	void abortTransaction();
	DbXml::XmlTransaction childTransaction();

private:
	Environment(const Environment &);
	Environment &operator=(const Environment &);

	virtual void signal(int signo);

	DbXml::SigBlock &sigBlock_;

	DbXml::XmlManager db_;
	DbXml::XmlTransaction *txn_;
	DbXml::XmlQueryContext context_;
	DbXml::XmlUpdateContext uc_;
	DbXml::XmlContainer *container_;
	DbXml::XmlQueryExpression *query_;
	DbXml::XmlResults *results_;

	std::string streamName_;
	int lineNo_;

	bool documentProjection_;
	bool interactive_;
	bool ignoreErrors_;
	bool transactions_;
	bool debugging_;
	bool verbose_;
	bool quit_;

	const Shell *shell_;
};

#endif
