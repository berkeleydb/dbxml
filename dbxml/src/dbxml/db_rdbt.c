/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996,2009 Oracle.  All rights reserved.
 *
 */

#ifndef lint
static const char copyright[] =
    "Copyright (c) 1996,2009 Oracle.  All rights reserved.\n";
#endif
#ifdef _MSC_VER
#pragma warning( disable : 4996 ) // warning: deprecation of strdup() in VS8
#endif

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#include <db.h>
#include "db_rdbt.h"
#include "db_utils.h"

#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 2)
extern int __db_getlong __P((DB_ENV *, const char *, char *, long, long, long *));
extern int __db_getulong __P((DB_ENV *, const char *, char *, u_long, u_long, u_long *));
#define	DB_GETLONG_HANDLE dbenv
#else
extern int __db_getlong __P((DB *, const char *, char *, long, long, long *));
extern int __db_getulong __P((DB *, const char *, char *, u_long, u_long, u_long *));
#define	DB_GETLONG_HANDLE dbp
#endif

static int	dbt_rdump
__P((DB_ENV *, DBT *, u_int32_t, READ_FN *, void *, u_long *));
static int	dbt_rprint
__P((DB_ENV *, DBT *, u_int32_t, READ_FN *, void *, u_long *));
static int	dbt_rrecno
__P((DB_ENV *, DBT *, u_int32_t, READ_FN *, void *, u_long *));
static int	convprintable __P((DB_ENV *, char *, char **));
static int	digitize __P((DB_ENV *, int, int *));
static void	badend __P((DB_ENV *));
static void	badnum __P((DB_ENV *));

#define	FLAG(name, value, keyword, flag)				\
	if (strcmp(name, keyword) == 0) {				\
		switch (*value) {					\
		case '1':						\
			if ((ret = dbp->set_flags(dbp, flag)) != 0) {	\
				dbp->err(dbp, ret, "set_flags: %s",	\
				    name);			\
				return (1);				\
			}						\
			break;						\
		case '0':						\
			break;						\
		default:						\
			badnum(dbenv);					\
			return (1);					\
		}							\
		return (0);						\
	}

#define	NUMBER(name, value, keyword, func)				\
	if (strcmp(name, keyword) == 0) {				\
		if (__db_getlong(DB_GETLONG_HANDLE,					\
		    NULL, value, 1, LONG_MAX, &val) != 0)		\
			return (1);					\
		if ((ret = dbp->func(dbp, val)) != 0)			\
			goto nameerr;					\
		return (0);						\
	}
#define	STRING(name, value, keyword, func)				\
	if (strcmp(name, keyword) == 0) {				\
		if ((ret = dbp->func(dbp, value[0])) != 0)		\
			goto nameerr;					\
		return (0);						\
	}

/*
 * __db_rconfig --
 *	Handle command-line configuration options.
 */
int
__db_rconfig(dbenv, dbp, name, value)
DB_ENV *dbenv;
DB *dbp;
char *name, *value;
{
	long val;
	int ret;

#ifdef notyet

	NUMBER(name, value, "bt_maxkey", set_bt_maxkey);
#endif

	NUMBER(name, value, "bt_minkey", set_bt_minkey);
	NUMBER(name, value, "db_lorder", set_lorder);
	NUMBER(name, value, "db_pagesize", set_pagesize);
#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 2)

	FLAG(name, value, "chksum", DB_CHKSUM);
#else

	FLAG(name, value, "chksum", DB_CHKSUM_SHA1);
#endif

	FLAG(name, value, "duplicates", DB_DUP);
	FLAG(name, value, "dupsort", DB_DUPSORT);
	NUMBER(name, value, "extentsize", set_q_extentsize);
	NUMBER(name, value, "h_ffactor", set_h_ffactor);
	NUMBER(name, value, "h_nelem", set_h_nelem);
	NUMBER(name, value, "re_len", set_re_len);
	STRING(name, value, "re_pad", set_re_pad);
	FLAG(name, value, "recnum", DB_RECNUM);
	FLAG(name, value, "renumber", DB_RENUMBER);

	dbp->errx(dbp,
		  "unknown configuration keyword \"%s\"", name);
	return (EINVAL);

nameerr:
	dbp->err(dbp, ret, "%s=%s", name, value);
	return (EINVAL);
}

/*
 * __db_rheader --
 *	Read the header message.
 */
int
__db_rheader(dbenv, dbp, dbtypep, subdbp, version, flagsp, rf, in, lineno)
DB_ENV *dbenv;
DB *dbp;
DBTYPE *dbtypep;
char **subdbp;
int *version;
u_int32_t *flagsp;
READ_FN *rf;
void *in;
u_long *lineno;
{
	int first, hdr, linelen, buflen, ret, start;
	char ch, *hdrbuf, *buf, *name, *p, *value;

  ret = 0;
	*dbtypep = DB_UNKNOWN;
	*flagsp = 0;
	*subdbp = hdrbuf = name = p = NULL;

	/*
	 * We start with a smallish buffer;  most headers are small.
	 * We may need to realloc it for a large subdatabase name.
	 */
	buflen = 4096;
	hdr = 0;
	if ((buf = (char *)malloc(buflen)) == NULL) {
memerr:
		dbp->errx(dbp, "could not allocate buffer %d", buflen);
		return (1);
	}
	hdrbuf = buf;

	start = 0;
	for (first = 1;; first = 0) {
		++*lineno;

		/* Read a line, which may be of arbitrary length, into buf. */
		linelen = 0;
		buf = &hdrbuf[start];
		if (hdr == 0) {
			for (;;) {
				if ((ret = (*rf)(&ch, 1, 0, in)) != 0) {
					if (!first && ret == EOF)
						goto badfmt;
					break;
				}

				if (ch == '\n')
					break;

				buf[linelen++] = ch;

				/* If the buffer is too small, double it. */
				if (linelen + start == buflen) {
					hdrbuf = (char *)realloc(hdrbuf,
								 buflen *= 2);
					if (hdrbuf == NULL)
						goto memerr;
					buf = &hdrbuf[start];
				}
			}
			if (ret == EOF)
				break;
			buf[linelen++] = '\0';
		} else
			linelen = (int) strlen(buf) + 1;
		start += linelen;

		if (name != NULL) {
			if (p != NULL)
				*p = '=';
			free(name);
			name = NULL;
		}
		/* If we don't see the expected information, it's an error. */
		if ((name = strdup(buf)) == NULL)
			goto memerr;
		if ((p = strchr(name, '=')) == NULL)
			goto badfmt;
		*p++ = '\0';

		value = p--;

		if (name[0] == '\0' || value[0] == '\0')
			goto badfmt;

		if (strcmp(name, "HEADER") == 0)
			break;
		if (strcmp(name, "VERSION") == 0) {
			/*
			 * Version 1 didn't have a "VERSION" header line.  We
			 * only support versions 1, 2, and 3 of the dump format.
			 */
			*version = atoi(value);

			if (*version > 3) {
				dbp->errx(dbp,
					  "line %lu: VERSION %d is unsupported",
					  *lineno, *version);
				goto err;
			}
			continue;
		}
		if (strcmp(name, "format") == 0) {
			if (strcmp(value, "bytevalue") == 0) {
				*flagsp &= ~DB_READ_PRINTABLE;
				continue;
			}
			if (strcmp(value, "print") == 0) {
				*flagsp |= DB_READ_PRINTABLE;
				continue;
			}
			goto badfmt;
		}
		if (strcmp(name, "type") == 0) {
			if (strcmp(value, "btree") == 0) {
				*dbtypep = DB_BTREE;
				continue;
			}
			if (strcmp(value, "hash") == 0) {
				*dbtypep = DB_HASH;
				continue;
			}
			if (strcmp(value, "recno") == 0) {
				*dbtypep = DB_RECNO;
				continue;
			}
			if (strcmp(value, "queue") == 0) {
				*dbtypep = DB_QUEUE;
				continue;
			}
			dbp->errx(dbp, "line %lu: unknown type", *lineno);
			goto err;
		}
		if (strcmp(name, "database") == 0 ||
		    strcmp(name, "subdatabase") == 0) {
			if ((ret = convprintable(dbenv, value, subdbp)) != 0) {
				dbp->err(dbp, ret, "error reading db name");
				goto err;
			}
			continue;
		}
		if (strcmp(name, "keys") == 0) {
			if (strcmp(value, "1") == 0)
				*flagsp |= DB_READ_HASKEYS;
			else if (strcmp(value, "0") == 0)
				*flagsp &= ~DB_READ_HASKEYS;
			else {
				ret = EINVAL;
				badnum(dbenv);
				goto err;
			}
			continue;
		}

		if ((ret = __db_rconfig(dbenv, dbp, name, value)) != 0) {
			dbp->errx(dbp, "unknown input-file header "
				  "configuration keyword \"%s\"", name);
			goto err;
		}
	}

	if (*version > 1)
		*flagsp |= DB_READ_HASDATAEND;

	/*
	 * Recno keys have only been printed in hexadecimal starting
	 * with db_dump format version 3 (DB 3.2).
	 */
	if (*version >= 3 && !(*flagsp & DB_READ_PRINTABLE))
		*flagsp |= DB_READ_RECNOHEX;

	if (*dbtypep == DB_RECNO || *dbtypep == DB_QUEUE)
		*flagsp |= DB_READ_RECNO;
	else
		*flagsp &= ~DB_READ_RECNO;

	if (0) {
badfmt:
		dbp->errx(dbp, "line %lu: unexpected format", *lineno);
		ret = 1;
	}
err:
	if (name != NULL)
		free(name);
	if (hdrbuf != NULL)
		free(hdrbuf);
	return (ret);
}

/*
 * __db_rdbt --
 *	Read DBT record from a dump.
 */
int
__db_rdbt(dbenv, keyp, datap, flags, rf, in, lineno)
DB_ENV *dbenv;
DBT *keyp, *datap;
u_int32_t flags;
READ_FN *rf;
void *in;
u_long *lineno;
{
	int ret;

	if (!LF_ISSET(DB_READ_HASKEYS))
		if (LF_ISSET(DB_READ_PRINTABLE))
			ret = dbt_rprint(dbenv, datap, flags, rf, in, lineno);
		else
			ret = dbt_rdump(dbenv, datap, flags, rf, in, lineno);
	else if (LF_ISSET(DB_READ_PRINTABLE)) {
		if (LF_ISSET(DB_READ_RECNO)) {
			if ((ret = dbt_rrecno(dbenv, keyp, flags,
					      rf, in, lineno)) != 0)
				goto err;
		} else if ((ret = dbt_rprint(dbenv, keyp, flags,
					     rf, in, lineno)) != 0)
			goto err;
		if ((ret = dbt_rprint(dbenv, datap, flags,
				      rf, in, lineno)) == EOF)
			goto odd_count;
	} else {
		if (LF_ISSET(DB_READ_RECNO)) {
			if ((ret = dbt_rrecno(dbenv, keyp, flags,
					      rf, in, lineno)) != 0)
				goto err;
		} else if ((ret = dbt_rdump(dbenv, keyp, flags,
					    rf, in, lineno)) != 0)
			goto err;
		if ((ret = dbt_rdump(dbenv, datap, flags,
				     rf, in, lineno)) == EOF)
odd_count:
			dbenv->errx(dbenv,
				    "odd number of key/data pairs");
	}

err:
	return (ret);
}

/*
 * dbt_rprint --
 *	Read a printable line into a DBT structure.
 */
int
dbt_rprint(dbenv, dbtp, flags, rf, in, lineno)
DB_ENV *dbenv;
DBT *dbtp;
u_int32_t flags;
READ_FN *rf;
void *in;
u_long *lineno;
{
	u_int32_t len;
	u_int8_t *p;
	int e, escape, first, ret;
	char c1, c2, buf[32];

	++*lineno;

	first = 1;
	e = escape = 0;
	p = dbtp->data;
	len = 0;
	while ((ret = (*rf)(&c1, 1, 0, in)) == 0 && c1 != '\n') {
		if (first) {
			first = 0;
			if (LF_ISSET(DB_READ_HASDATAEND)) {
				if (c1 != ' ') {
					buf[0] = c1;
					if ((*rf)(buf + 1, sizeof(buf) - 1,
						  '\n', in) != 0 ||
					    strcmp(buf, "DATA=END") != 0) {
						badend(dbenv);
						return (EINVAL);
					}
					return (EOF);
				}
				continue;
			}
		}
		if (escape) {
			if (c1 != '\\') {
				if ((*rf)(&c2, 1, 0, in) != 0) {
					badend(dbenv);
					return (EINVAL);
				}
				c1 = digitize(dbenv,
					      c1, &e) << 4 | digitize(dbenv, c2, &e);
				if (e)
					return (e);
			}
			escape = 0;
		} else
			if (c1 == '\\') {
				escape = 1;
				continue;
			}
		if (len >= dbtp->ulen - 10) {
			dbtp->ulen *= 2;
			if ((dbtp->data =
				 (void *)realloc(dbtp->data, dbtp->ulen)) == NULL) {
				dbenv->err(dbenv, ENOMEM, NULL);
				return (ENOMEM);
			}
			p = (u_int8_t *)dbtp->data + len;
		}
		++len;
		*p++ = c1;
	}

	if (ret == EOF && len != 0)
		badend(dbenv);

	dbtp->size = len;
	return (ret);
}

/*
 * dbt_rdump --
 *	Read a byte dump line into a DBT structure.
 */
int
dbt_rdump(dbenv, dbtp, flags, rf, in, lineno)
DB_ENV *dbenv;
DBT *dbtp;
u_int32_t flags;
READ_FN *rf;
void *in;
u_long *lineno;
{
	u_int32_t len;
	u_int8_t *p;
	int e, first, ret;
	char c1, c2, buf[32];

	++*lineno;

	first = 1;
	e = 0;
	p = dbtp->data;
	len = 0;
	while ((ret = (*rf)(&c1, 1, 0, in)) == 0 && c1 != '\n') {
		if (first) {
			first = 0;
			if (LF_ISSET(DB_READ_HASDATAEND)) {
				if (c1 != ' ') {
					buf[0] = c1;
					if ((*rf)(buf + 1, sizeof(buf) - 1,
						  '\n', in) != 0 ||
					    strcmp(buf, "DATA=END") != 0) {
						badend(dbenv);
						return (EINVAL);
					}
					return (EOF);
				}
				continue;
			}
		}
		if ((*rf)(&c2, 1, 0, in) != 0) {
			badend(dbenv);
			return (EINVAL);
		}
		if (len >= dbtp->ulen - 10) {
			dbtp->ulen *= 2;
			if ((dbtp->data =
				 (void *)realloc(dbtp->data, dbtp->ulen)) == NULL) {
				dbenv->err(dbenv, ENOMEM, NULL);
				return (ENOMEM);
			}
			p = (u_int8_t *)dbtp->data + len;
		}
		++len;
		*p++ = digitize(dbenv, c1, &e) << 4 | digitize(dbenv, c2, &e);
		if (e)
			return (e);
	}

	if (ret == EOF && len != 0)
		badend(dbenv);

	dbtp->size = len;
	return (ret);
}

/*
 * dbt_rrecno --
 *	Read a record number dump line into a DBT structure.
 */
int
dbt_rrecno(dbenv, dbtp, flags, rf, in, lineno)
DB_ENV *dbenv;
DBT *dbtp;
u_int32_t flags;
READ_FN *rf;
void *in;
u_long *lineno;
{
	int ret;
	char buf[32], *p, *q;
	u_long db_recno;

	ret = 0;
	++*lineno;

	if ((*rf)(buf, sizeof(buf), '\n', in) == EOF ||
	    strcmp(buf, "DATA=END") == 0)
		return (EOF);

	if (buf[0] != ' ')
		goto bad;

	/*
	 * If we're expecting a hex key, do an in-place conversion
	 * of hex to straight ASCII before calling __db_getulong().
	 */
	if (LF_ISSET(DB_READ_RECNOHEX)) {
		for (p = q = buf + 1; *q != '\0' && *q != '\n';) {
			/*
			 * 0-9 in hex are 0x30-0x39, so this is easy.
			 * We should alternate between 3's and [0-9], and
			 * if the [0-9] are something unexpected,
			 * __db_getulong will fail, so we only need to catch
			 * end-of-string conditions.
			 */
			if (*q++ != '3')
				goto bad;
			if (*q == '\n' || *q == '\0')
				goto bad;
			*p++ = *q++;
		}
		*p = '\0';
	}

	if (__db_getulong(NULL,
			  "dbt_rrecno", buf + 1, 0, 0, &db_recno)) {
bad:
		badend(dbenv);
		return (EINVAL);
	}

	*(db_recno_t *)dbtp->data = (db_recno_t)db_recno;
	dbtp->size = sizeof(db_recno_t);
	return (ret);
}

/*
 * convprintable --
 *	Convert a printable-encoded string into a newly allocated string.
 *
 * In an ideal world, this would probably share code with dbt_rprint, but
 * that's set up to read character-by-character (to avoid large memory
 * allocations that aren't likely to be a problem here), and this has fewer
 * special cases to deal with.
 *
 * Note that despite the printable encoding, the char * interface to this
 * function (which is, not coincidentally, also used for database naming)
 * means that outstr cannot contain any nuls.
 */
int
convprintable(dbenv, instr, outstrp)
DB_ENV *dbenv;
char *instr, **outstrp;
{
	char c, *outstr;
	int e;

	/*
	 * Just malloc a string big enough for the whole input string;
	 * the output string will be smaller (or of equal length).
	 */
	if ((outstr = (char *)malloc(strlen(instr))) == NULL)
		return (ENOMEM);

	*outstrp = outstr;
	e = 0;
	for ( ; *instr != '\0'; instr++)
		if (*instr == '\\') {
			if (*++instr == '\\') {
				*outstr++ = '\\';
				continue;
			}
			c = digitize(dbenv, *instr, &e) << 4;
			c |= digitize(dbenv, *++instr, &e);
			if (e)
				return (e);

			*outstr++ = c;
		} else
			*outstr++ = *instr;
	*outstr = '\0';

	return (0);
}

/*
 * digitize --
 *	Convert a character to an integer.
 */
int
digitize(dbenv, c, errorp)
DB_ENV *dbenv;
int c, *errorp;
{
	switch (c) {			/* Don't depend on ASCII ordering. */
	case '0':
		return (0);
	case '1':
		return (1);
	case '2':
		return (2);
	case '3':
		return (3);
	case '4':
		return (4);
	case '5':
		return (5);
	case '6':
		return (6);
	case '7':
		return (7);
	case '8':
		return (8);
	case '9':
		return (9);
	case 'a':
		return (10);
	case 'b':
		return (11);
	case 'c':
		return (12);
	case 'd':
		return (13);
	case 'e':
		return (14);
	case 'f':
		return (15);
	}

	dbenv->errx(dbenv, "unexpected hexadecimal value '%c'", c);
	*errorp = EINVAL;

	return (0);
}

/*
 * badnum --
 *	Display the bad number message.
 */
void
badnum(dbenv)
DB_ENV *dbenv;
{
	dbenv->errx(dbenv,
		    "boolean name=value pairs require a value of 0 or 1");
}

/*
 * badend --
 *	Display the bad end to input message.
 */
void
badend(dbenv)
DB_ENV *dbenv;
{
	dbenv->errx(dbenv, "unexpected end of input data or key/data pair");
}
