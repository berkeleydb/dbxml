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

#include <db.h>
#include "db_utils.h"
#include "common_utils.h" // for signal handling
#include <string.h>
#include <cstdlib>

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
		      "usage: dbxml_load [-V]",
		      "[-f file] [-h home] [-P password] xml_container");
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
db_init(DB_ENV *dbenv, char *home, int *is_privatep)
{
	u_int32_t flags;
	int ret;

	*is_privatep = 0;
	/* We may be loading into a live environment.  Try and join. */
	flags = DB_USE_ENVIRON;
	if (dbenv->open(dbenv, home, flags, 0) == 0)
		return (0);

	/*
	 * We're trying to load a container.
	 *
	 * An environment is required because we may be trying to look at
	 * databases in directories other than the current one.  We could
	 * avoid using an environment iff the -h option wasn't specified,
	 * but that seems like more work than it's worth.
	 *
	 * No environment exists (or, at least no environment that includes
	 * an mpool region exists).  Create one, but make it private so that
	 * no files are actually created.
	 */
	LF_SET(DB_CREATE | DB_PRIVATE | DB_INIT_MPOOL);
	*is_privatep = 1;
	if ((ret = dbenv->open(dbenv, home, flags, 0)) == 0)
		return (0);

	/* An environment is required. */
	dbenv->err(dbenv, ret, "DB_ENV->open");
	return (1);
}

int main(int argc, char *argv[])
{
	const char *progname = "dbxml_load";
	DB_ENV *dbenv;
	XmlManager *xmlDb;
	char *home, *passwd;
	unsigned long lineno = 0;
	int ch, exitval, is_private, ret;

	if ((ret = version_check(progname)) != 0)
		return (ret);

	dbenv = NULL;
	xmlDb = NULL;
	exitval = is_private = 0;
	passwd = home = NULL;

	while ((ch = getopt(argc, argv, "f:h:P:V")) != EOF)
		switch (ch) {
		case 'f':
			if (freopen(optarg, "r", stdin) == NULL) {
				fprintf(stderr, "%s: %s: reopen: %s\n",
					progname, optarg, strerror(errno));
				return (EXIT_FAILURE);
			}
			break;
		case 'h':
			home = optarg;
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
	if (passwd != NULL && (ret = dbenv->set_encrypt(dbenv,
				     passwd, DB_ENCRYPT_AES)) != 0) {
		dbenv->err(dbenv, ret, "set_passwd");
		goto err;
	}

	/* Initialize the environment. */
	if ((ret = db_init(dbenv, home, &is_private)) != 0) {
		dbenv->err(dbenv, ret, "db_init");
		goto err;
	}

	xmlDb = new XmlManager(dbenv);

	try {
		XmlUpdateContext uc = xmlDb->createUpdateContext();
		xmlDb->loadContainer(argv[0], &cin, &lineno, uc);
	} catch (XmlException &e) {
		dbenv->errx(dbenv, "load %s: %s", argv[0], e.what());
		goto err;
	}
	
	if (0) {
err:
		exitval = 1;
	}
	if (xmlDb)
		delete xmlDb;
	if ((ret = dbenv->close(dbenv, 0)) != 0) {
		exitval = 1;
		fprintf(stderr,
			"%s: dbenv->close: %s\n", progname, db_strerror(ret));
	}
	return (exitval == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

