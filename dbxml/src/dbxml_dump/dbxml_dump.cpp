//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#ifdef _MSC_VER
#pragma warning( disable : 4996 ) // warning: deprecation of strdup() in VS8
#endif

#include "dbxml_config.h"

#include <dbxml/DbXml.hpp>
#include <iostream>
#include <cerrno>
#include "common_utils.h"
#include <string.h>
#include <cstdlib>

#include <db.h>

#define	MEGABYTE	1048576

#ifndef NO_SYSTEM_INCLUDES
#endif

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

using namespace std;
using namespace DbXml;

/*
 * usage --
 *	Display the usage message.
 */
int
usage()
{
	(void)fprintf(stderr, "%s\n\t%s\n",
		      "usage: dbxml_dump [-rRV]",
		      "[-f output] [-h home] [-P password] xml_container");
	return (EXIT_FAILURE);
}

int
version_check(const char *progname)
{
	int v_major, v_minor, v_patch;

	/* Make sure we're loaded with the right version of DB XML. */
	(void)dbxml_version(&v_major, &v_minor, &v_patch);
	if (v_major != DBXML_VERSION_MAJOR ||
	    v_minor != DBXML_VERSION_MINOR ||
	    v_patch != DBXML_VERSION_PATCH) {
		fprintf(stderr,
			"%s: version %d.%d.%d doesn't match library version %d.%d.%d\n",
			progname, DBXML_VERSION_MAJOR, DBXML_VERSION_MINOR,
			DBXML_VERSION_PATCH, v_major, v_minor, v_patch);
		return (EXIT_FAILURE);
	}
	return (0);
}

/*
 * db_init --
 *	Initialize the environment.
 */
int
db_init(DB_ENV *dbenv, char *home, int is_salvage, u_int32_t cache, int *is_privatep)
{
	int ret;

	/*
	 * Try and use the underlying environment when opening a database.
	 * We wish to use the buffer pool so our information is as up-to-date
	 * as possible, even if the mpool cache hasn't been flushed.
	 *
	 * If we are not doing a salvage, we wish to use the DB_JOINENV flag;
	 * if a locking system is present, this will let us use it and be
	 * safe to run concurrently with other threads of control.  (We never
	 * need to use transactions explicitly, as we're read-only.)  Note
	 * that in CDB, too, this will configure our environment
	 * appropriately, and our cursors will (correctly) do locking as CDB
	 * read cursors.
	 *
	 * If we are doing a salvage, the verification code will protest
	 * if we initialize transactions, logging, or locking;  do an
	 * explicit DB_INIT_MPOOL to try to join any existing environment
	 * before we create our own.
	 */
	*is_privatep = 0;
	if (dbenv->open(dbenv, home,
			DB_USE_ENVIRON | (is_salvage ? DB_INIT_MPOOL : DB_JOINENV), 0) == 0)
		return (0);

	/*
	 * An environment is required because we may be trying to look at
	 * databases in directories other than the current one.  We could
	 * avoid using an environment iff the -h option wasn't specified,
	 * but that seems like more work than it's worth.
	 *
	 * No environment exists (or, at least no environment that includes
	 * an mpool region exists).  Create one, but make it private so that
	 * no files are actually created.
	 */
	*is_privatep = 1;
	if ((ret = dbenv->set_cachesize(dbenv, 0, cache, 1)) == 0 &&
	    (ret = dbenv->open(dbenv, home,
			       DB_CREATE | DB_INIT_MPOOL | DB_PRIVATE | DB_USE_ENVIRON, 0)) == 0)
		return (0);

	/* An environment is required. */
	dbenv->err(dbenv, ret, "open");
	return (1);
}

int
main(int argc, char *const* argv)
{
	const char *progname = "dbxml_dump";
	DB_ENV *dbenv;
	XmlManager *xmlDb;
	u_int32_t cache;
	int ch, exitval, is_private, keyflag, nflag, ret, Rflag, rflag;
	char *home, *passwd;

	if ((ret = version_check(progname)) != 0)
		return (ret);

	dbenv = NULL;
	xmlDb = NULL;
	exitval = nflag = rflag = Rflag = 0;
	keyflag = 0;
	cache = MEGABYTE;
	is_private = 0;
	home = passwd = NULL;

	while ((ch = getopt(argc, argv, "f:h:NP:rRV")) != EOF)
		switch (ch) {
		case 'f':
			if (freopen(optarg, "wb", stdout) == NULL) {
				fprintf(stderr, "%s: %s: reopen: %s\n",
					progname, optarg, strerror(errno));
				return (EXIT_FAILURE);
			}
			break;
		case 'h':
			home = optarg;
			break;
		case 'N':
			nflag = 1;
			break;
		case 'P':
			passwd = strdup(optarg);
			memset(optarg, 0, strlen(optarg));
			if (passwd == NULL) {
				fprintf(stderr, "%s: strdup: %s\n",
					progname, strerror(errno));
				return (EXIT_FAILURE);
			}
			break;
		case 'R':
			Rflag = 1;
			/* DB_AGGRESSIVE requires DB_SALVAGE */
			/* FALLTHROUGH */
		case 'r':
			rflag = 1;
			break;
		case 'V':
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", db_version(NULL, NULL, NULL));
			return (EXIT_SUCCESS);
		case '?':
		default:
			return (usage());
		}
	argc -= optind;
	argv += optind;

	if (argc != 1)
		return (usage());

	/* Handle possible interruptions. */
	SigBlock sb;

	/*
	 * Create an environment object and initialize it for error
	 * reporting.
	 */
	if ((ret = db_env_create(&dbenv, 0)) != 0) {
		fprintf(stderr,
			"%s: db_env_create: %s\n", progname, db_strerror(ret));
		goto err;
	}

	dbenv->set_errfile(dbenv, stderr);
	dbenv->set_errpfx(dbenv, progname);
	if (nflag) {
		if ((ret = dbenv->set_flags(dbenv, DB_NOLOCKING, 1)) != 0) {
			dbenv->err(dbenv, ret, "set_flags: DB_NOLOCKING");
			goto err;
		}
		if ((ret = dbenv->set_flags(dbenv, DB_NOPANIC, 1)) != 0) {
			dbenv->err(dbenv, ret, "set_flags: DB_NOPANIC");
			goto err;
		}
	}
	if (passwd != NULL && (ret = dbenv->set_encrypt(dbenv,
				     passwd, DB_ENCRYPT_AES)) != 0) {
		dbenv->err(dbenv, ret, "set_passwd");
		goto err;
	}

	/* Initialize the environment. */
	if ((ret = db_init(dbenv, home, rflag, cache, &is_private)) != 0) {
		dbenv->err(dbenv, ret, "db_init");
		goto err;
	}

	xmlDb = new XmlManager(dbenv);
	
	if (rflag) {
		try {
			xmlDb->verifyContainer(argv[0], &cout, DB_SALVAGE | (Rflag ? DB_AGGRESSIVE : 0));
		} catch (XmlException &e) {
			dbenv->errx(dbenv, "verify %s: %s", argv[0], e.what());
			goto err;
		}
		goto done;
	}
	
	try {
		xmlDb->dumpContainer(argv[0], &cout);
	} catch (XmlException &e) {
		dbenv->errx(dbenv, "dump %s: %s", argv[0], e.what());
		goto err;
	}
	
	if (0) {
	err:
		exitval = 1;
	}
done:
	if (xmlDb)
		delete xmlDb;
		
	if ((ret = dbenv->close(dbenv, 0)) != 0) {
		exitval = 1;
		fprintf(stderr,
			"%s: dbenv->close: %s\n", progname, db_strerror(ret));
	}
	
	return (exitval == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
