/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2009 Oracle.  All rights reserved.
 *
 */

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h> // for mkdir

#include <errno.h>
#include "RepNode.hpp"

using namespace DbXml;
using namespace std;

RepNode::RepNode(u_int32_t startPolicy, const string& envHome,
	const string& host, int port, u_int32_t priority,
	vector<RepNode *>& group, const Config& config)
	: startPolicy_(startPolicy),
	  envHome_(envHome),
	  host_(host),
	  port_(port),
	  priority_(priority),
	  group_(group),
	  config_(config),
	  containerName_("DbXmlHATest.dbxml"),
	  dbenv_(NULL),
	  mgr_(NULL),
	  cont_(NULL),
	  envFlags_(DB_CREATE | DB_RECOVER | DB_INIT_LOCK | DB_INIT_LOG |
	  	    DB_INIT_MPOOL | DB_INIT_TXN | DB_INIT_REP | DB_THREAD),
	  cacheSize_(10 * 1024 * 1024)
{
	id_ = group_.size();
	group_.push_back(this);
}

RepNode::~RepNode()
{
	stop();
	// erase the nodes in group vector.
	for (size_t i = 0; i < group_.size(); i++) {
		if (group_[i] == this) {
			group_.erase(group_.begin()+i);
			break;
		}
	}
}

bool RepNode::equal(const RepNode& node)
{
	if (id_ == node.id_)
		return true;
	return false;
}

void RepNode::clearDbEnv()
{
	// Clear the existing environment. Transaction is needless in this part.
	if (access(envHome_.c_str(), 0) < 0) {
		mkdir(envHome_.c_str(), 0755);
	} else {
		// Remove Container
		{
			DB_ENV* dbenv;
			createDbEnv(&dbenv, 0);
			dbenv->open(dbenv, envHome_.c_str(), envFlags_, 0);

			XmlManager mgr(dbenv, DBXML_ADOPT_DBENV);
			try {
				mgr.removeContainer(containerName_);
			} catch (...) {}
		}

		// Remove Environment
		DB_ENV* dbEnvRemove;
		createDbEnv(&dbEnvRemove, 0);
		dbEnvRemove->remove(dbEnvRemove, envHome_.c_str(), DB_FORCE);
	}
}

void RepNode::openContainer()
{
	// reopening a container is forbidden.
	if (!cont_.isNull())
		throwException("Attempt to re-open a container.");

	XmlContainerConfig conf;

	// wait for being elected as a Master or start up done(as a client).
	while (!(isMaster() || appData_.isStartupDone()))
		yield();

	if (isMaster())
		conf.setAllowCreate(true);

	XmlTransaction txn;
	while (true) {
		try {
			txn = mgr_->createTransaction();
			cont_ = mgr_->openContainer(txn, containerName_, conf);
			txn.commit();
			return;
		/*
		 * One thing to watch out for is a case where the databases
		 * you are trying to open do not yet exist. This can happen
		 * for replicas where the databases are being opened
		 * read-only. If this happens, ENOENT is returned by the
		 * open() call.
		 */
		} catch (XmlException &xe) {
			ostringstream o;
			XmlException::ExceptionCode ec = xe.getExceptionCode();
			if (ec == XmlException::DATABASE_ERROR) {
				int dbErr = xe.getDbErrno();
				if (dbErr != ENOENT && dbErr != DB_REP_LOCKOUT) {
					o << envHome_ << ":" << xe.what() << endl
					  << "DB_ERRNO:" << dbErr << endl;
					throwException(o.str());
				}
			} else if (ec != XmlException::CONTAINER_NOT_FOUND) {
				o << envHome_ << ":" << xe.what() << endl
				  << "ExceptionCode :" << ec << endl;
				throwException(o.str());
			}
			yield();
		}
	}
}

void RepNode::start()
{
	// Restart is forbidden.
	if (dbenv_ != NULL)
		throwException("Attempt to restart a running node.");

	appData_.reset();
	init();
	mgr_ = new XmlManager(dbenv_, DBXML_ADOPT_DBENV);
	openContainer();
}

void RepNode::start(u_int32_t startPolicy)
{
	startPolicy_ = startPolicy;
	start();
}

void RepNode::stop()
{
	/*
	 * We have used the DB_TXN_NOSYNC environment flag for
	 * improved performance without the usual sacrifice of
	 * transactional durability, as discussed in the
	 * "Transactional guarantees" page of the Reference
	 * Guide: if one replication site crashes, we can
	 * expect the data to exist at another site.  However,
	 * in case we shut down all sites gracefully, we push
	 * out the end of the log here so that the most
	 * recent transactions don't mysteriously disappear.
	 */
	dbenv_->log_flush(dbenv_, NULL);

	cont_ = NULL;
	if (mgr_ != NULL) {
		delete mgr_;
		mgr_ = 0;
	}

	// dbenv is deleted by XmlManager
	dbenv_ = NULL;
}

void RepNode::init()
{
	// If the node is running, just do nothing.
	if (dbenv_ != NULL)
		return;
	/*
	 * Now we open our environment handle and set the APP_DATA structure
	 * to it's app_private member.
	 */
	createDbEnv(&dbenv_, 0);
	dbenv_->app_private = &appData_;
	/*
	 * Configure the environment handle. Here we configure
	 * asynchronous transactional commits for performance reasons.
	 */
	if (config_.verbose_)
		dbenv_->set_errfile(dbenv_, stderr);
	else
		dbenv_->set_errfile(dbenv_, NULL);
	dbenv_->set_errpfx(dbenv_, envHome_.c_str());
	dbenv_->set_event_notify(dbenv_, eventCallback);
	dbenv_->repmgr_set_ack_policy(dbenv_, DB_REPMGR_ACKS_QUORUM);

	/*
	 * Configure the local address. This is the local hostname and
	 * port that this replication participant will use to receive
	 * incoming replication messages. Note that this can be performed
	 * only once for the application. It is required.
	 */
	dbenv_->repmgr_set_local_site(dbenv_, host_.c_str(), port_, 0);
	
	/*
	* Add a site to the list of replication environments known to
	* this application.
	*/
	for (size_t i = 0; i < group_.size(); i++) {
		const RepNode& node = *group_[i];
		if (!equal(node)) {
			dbenv_->repmgr_add_remote_site(dbenv_,
						       node.host_.c_str(),
						       node.port_, NULL, 0);
		}
	}
	/*
	 * Identify the number of sites in the replication group. This is
	 * necessary so that elections and permanent message handling can
	 * be performed correctly.
	 */
	if (group_.size() > 0)
		dbenv_->rep_set_nsites(dbenv_, group_.size());

	/*
	 * Set this application's priority. This is used for elections.
	 *
	 * Set this number to a positive integer, or 0 if you do not want
	 * this site to be able to become a master.
	 */
	dbenv_->rep_set_priority(dbenv_, priority_);

	/*
	 * We can now open our environment, although we're not ready to
	 * begin replicating.  However, we want to have a dbenv around
	 * so that we can send it into any of our message handlers.
	 */
	dbenv_->set_cachesize(dbenv_, 0, cacheSize_, 0);
	dbenv_->set_flags(dbenv_, DB_TXN_NOSYNC, 1);

	// Setup the verbose flag if it's required.
	if (config_.verbose_)
		dbenv_->set_verbose(dbenv_, DB_VERB_REPLICATION, 1);

	// Open dbenv and start the replication framework such that it uses 3 threads.
	dbenv_->open(dbenv_, envHome_.c_str(), envFlags_, 0);
	dbenv_->repmgr_start(dbenv_, 3, startPolicy_);
}

XmlContainer& RepNode::getContainer()
{
	if (dbenv_ == NULL && mgr_ == NULL && cont_.isNull()) {
		// The DbEnv, Manager and Container should be created/open;
		createDbEnv(&dbenv_, 0);
		dbenv_->set_errfile(dbenv_, stderr);
		dbenv_->set_errpfx(dbenv_, envHome_.c_str());
		dbenv_->set_cachesize(dbenv_, 0, cacheSize_, 0);
		dbenv_->open(dbenv_, envHome_.c_str(), envFlags_, 0);

		mgr_ = new XmlManager(dbenv_, DBXML_ADOPT_DBENV);
		cont_ = mgr_->openContainer(containerName_);
	}
	return cont_;
}

void RepNode::eventCallback(DB_ENV* dbenv, u_int32_t which, void *info)
{
	AppData *app = (AppData *)dbenv->app_private;
	switch (which) {
	case DB_EVENT_REP_MASTER:
		app->setMaster(true);
		break;

	case DB_EVENT_REP_CLIENT:
		app->setMaster(false);
		break;

	case DB_EVENT_REP_STARTUPDONE:
		app->setStartupDone(true);
		break;

	// Don't care about this one, for now.
	case DB_EVENT_REP_ELECTED:
	case DB_EVENT_REP_NEWMASTER:
	case DB_EVENT_REP_PERM_FAILED:
		break;

	default:
		dbenv->errx(dbenv, "ignoring event %d", which);
	}
}

void RepNode::createDbEnv(DB_ENV** pDbEnv, u_int32_t flag)
{
	int ret = db_env_create(pDbEnv, flag);
	if (ret != 0) {
		string o = string("Environment open failed: ") +
			   db_strerror(ret);
		throwException(o);
	}
}

void RepNode::yield()
{
	usleep(10000);
}

void RepNode::throwIfUnExpected(XmlException &xe)
{
	if (xe.getExceptionCode() != XmlException::DATABASE_ERROR ||
	    (xe.getDbErrno() != DB_LOCK_DEADLOCK &&
	     xe.getDbErrno() != DB_REP_HANDLE_DEAD &&
	     xe.getDbErrno() != DB_REP_LOCKOUT)) {

		ostringstream o;
		o << envHome_ << ":" << xe.what() << endl;
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR)
			o << "DB_ERRNO:" << xe.getDbErrno() << endl;

		printMsg(o);
		throw xe;
	}
}

void RepNode::throwException(const string& msg)
{
	string what = envHome_ + ":" + msg;
	throwException(what);
}

OutputMutex RepNode::printMutex = OutputMutex();
void RepNode::printMsg(const std::string& str)
{
	printMutex.lock();
	cout << envHome_ << ": " << str << endl;
	printMutex.unlock();
}

void RepNode::printMsg(ostringstream& oss)
{
	printMsg(oss.str());
	oss.str("");		// reset stringstream
}
