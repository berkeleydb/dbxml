//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <fstream>
#include <string>
#include <cstdlib>

#include "DefaultShell.hpp"
#include "dbxml/DbXml.hpp"
#include "Environment.hpp"
#include "common_utils.h"

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

using namespace DbXml;
using namespace std;
bool verboseErrors = false;

static void errcall(const DB_ENV *dbenv, const char *errpfx, const char *msg)
{
	if (verboseErrors) {
		if(errpfx) {
			cerr << errpfx << ": ";
		}
		cerr << msg << endl;
	}
}

static void usage(const string &progname, int exitCode)
{
	string::size_type pos = progname.rfind('/');
	if(pos == string::npos) {
		pos = progname.rfind('\\');
	}
	if(pos == string::npos) {
		pos = 0;
	}
	else {
		++pos;
	}

	cerr << "Usage: " << progname.substr(pos) << " [OPTIONS]" << endl << endl;
	cerr << "Options:" << endl;
	cerr << "-c\t\tcreate a new environment in the directory specified by the -h option." << endl;
	cerr << "-h <home>\tuse specified directory as a Berkeley DB environment. (default: .)" << endl;
	cerr << "-P <password>\tspecify database password" << endl;
	cerr << "-s <script>\trun the script in non-interactive mode.\n\t\t Multiple scripts are allowed" << endl;
	cerr << "-t\t\ttransaction mode -- transactions can be used, and are required \n\t\tfor writes" << endl;
	cerr << "-V\t\tprint software version" << endl;
	cerr << "-v\t\tverbose. Using the switch a second time increases verbosity" << endl;
	cerr << "-x\t\tsecure mode -- disallow access to the local file system and \n\t\tthe internet" << endl;
	cerr << "-z <size>\tIf an environment is created, set the cache size to <size> Mb (default: 64)" << endl;
	cerr << "-?\t\thelp -- print this message" << endl;
	cerr << endl;
	exit(exitCode);
}

int main(int argc, char **argv)
{
	// Deal with command line arguments
	const char *path2DbEnv = 0;
	u_int32_t envFlags = (DB_CREATE|DB_PRIVATE|DB_INIT_MPOOL);
	u_int32_t txnEnvFlags =	(DB_INIT_TXN|DB_INIT_LOCK|DB_INIT_LOG);
	u_int32_t dbxmlFlags = DBXML_ALLOW_EXTERNAL_ACCESS;
	vector<string> scripts;
	int verbose = 0;
	bool transactionMode = false;
	bool dbPrivate = false;
	bool envCreate = false;
	const char *progName = argv[0];
	const char *password = 0;
	int cacheSize = 64;
	int ch;
	int ret = 0;

	while ((ch = getopt(argc, argv, "?h:hs:tvxVP:cz:")) != -1) {
		switch (ch) {
		case 'h': {
			path2DbEnv = optarg;
			break;
		}
		case 'z': {
			cacheSize = atoi(optarg);
			break;
		}
		case 'c': {
			envFlags &= ~DB_PRIVATE;
			envCreate = true;
			break;
		}
		case 'x': {
			dbxmlFlags &= ~DBXML_ALLOW_EXTERNAL_ACCESS;
			break;
		}
		case 't': {
			transactionMode = true;
			envFlags |= txnEnvFlags;
			break;
		}
		case 's': {
			scripts.push_back(optarg);
			break;
		}
		case 'v': {
			++verbose;
			break;
		}
		case 'V': {
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", db_version(NULL, NULL, NULL));
			exit(0);
		}
		case 'P': {
			password = optarg;
			break;
		}
		case '?':
		default: {
			usage(progName, 0);
			break;
		}
		}
	}

	// Turn on logging if extra verbose is specified
	if(verbose > 1) {
		setLogLevel(LEVEL_ALL, true);
		setLogCategory(CATEGORY_ALL, true);
		setLogCategory(CATEGORY_NODESTORE, verbose > 2);
		verboseErrors = true;
	}

	SigBlock sb; // block signals, resend at end of scope
	try {
		// Create a DB environment, and XmlManager
		DB_ENV *dbenv;
		int dberr = 0;
		dberr = db_env_create(&dbenv, 0);
		if (dberr) {
			cout << "Error creating environment: " << dberr << endl;
			exit(-1);
		}
		if (password)
			dbenv->set_encrypt(dbenv, password, DB_ENCRYPT_AES);
		dbenv->set_errcall(dbenv, errcall);
		dbenv->set_cachesize(dbenv, 0, cacheSize * 1024 * 1024, 1);
		dbenv->set_lk_max_lockers(dbenv, 10000);
		dbenv->set_lk_max_locks(dbenv, 10000);
		dbenv->set_lk_max_objects(dbenv, 10000);
		if (!dbPrivate) {
			dbenv->set_lk_detect(dbenv, DB_LOCK_DEFAULT);
			if (verbose && !envCreate) {
				cout <<
					"Attempting to join environment: "
				     << (path2DbEnv ? path2DbEnv : ".")
				     << endl;
			}
			dberr = dbenv->open(dbenv, path2DbEnv, DB_USE_ENVIRON, 0);
			if (dberr != 0) {
				if (dberr == DB_VERSION_MISMATCH) {
					cerr << "Error opening environment "
					     << (path2DbEnv ? path2DbEnv : ".")						
					     << ": " << "environment version mismatch" << endl;
					exit(-1);
				}
				if (verbose) {
					if(envCreate) {
						cerr << "Creating environment: "
						     << (path2DbEnv ? path2DbEnv : ".")
						     << endl;
					} else {
						cerr << "Unable to join environment "
						     << (path2DbEnv ? path2DbEnv : ".")
						     << ", creating a DB_PRIVATE environment" << endl;
					}
				}
				dberr = dbenv->open(dbenv, path2DbEnv,
						    envFlags, 0);
			} else {
				cout <<	"Joined existing environment"
				     << endl;
				u_int32_t eflags = 0;
				dbenv->get_open_flags(dbenv, &eflags);
				if (eflags & DB_INIT_TXN)
					transactionMode = true;
				else {
					if (verbose && (transactionMode == true))
						cout << "Joined a non-transactional environment, turning off transaction mode" << endl;
					transactionMode = false;
				}
			}
		} else {
			dberr = dbenv->open(dbenv, path2DbEnv,
					    envFlags, 0);
		}
		if (dberr != 0) {
			cerr << "Error opening environment "
			     << (path2DbEnv ? path2DbEnv : ".")
			     << ", error is " << dberr << endl;
			exit(-1);
		}
		XmlManager db(dbenv, dbxmlFlags|DBXML_ADOPT_DBENV);

		// Create the environment
		Environment env(db, sb);
		env.transactions() = transactionMode;

		// Create the Shell object
		DefaultShell shell;

		// Run scripts, if specified
		if(!scripts.empty()) {
			env.interactive() = false;
			env.verbose() = (verbose != 0);

			for(vector<string>::iterator i = scripts.begin();
			    i != scripts.end() && !env.quit(); ++i) {
				ifstream scriptFile(i->c_str(), ios::in);
				if(!scriptFile) {
					cerr << progName << ": cannot open script file: " << *i << endl;
				} else {
					env.streamName() = *i;
					env.lineNo() = 0;
					shell.mainLoop(scriptFile, env);
					scriptFile.close();
				}
			}
		}

		// Perform the queries
		if(!env.quit()) {
			env.interactive() = true;
			env.verbose() = true;
			env.streamName() = "stdin";
			env.lineNo() = 0;

			do {
				shell.mainLoop(cin, env);
				if(env.sigBlock().isInterrupted())
					env.sigBlock().reset();
			} while(!env.quit() && !cin.eof());
		}
	}
	catch(exception &e) {
		cerr << progName << ": error at lowest level: " << e.what() << endl;
		ret = 1;
	}
	catch(...) {
		cerr << progName << ": error at lowest level: " << endl;
		ret = 1;
	}
	return ret;
}
