/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996,2009 Oracle.  All rights reserved.
 *
 */

#ifndef _DB_LOAD_UTIL_H_
#define	_DB_LOAD_UTIL_H_

#if defined(__cplusplus)
extern "C"
{
#endif

	/*
	 * Reads at most 'len' characters into 'buf' up to the first 'until' characater
	 * (if non-zero).  The terminator (if found) is discarded, and the string is nul
	 * terminated if 'len' > 1.
	 * Returns: zero on success, DB_NOTFOUND if 'until' != 0 and not found, or
	 * EOF on EOF.
	 */
	typedef int (READ_FN)(char *buf, size_t len, char until, void *handle);

	int	__db_rheader __P((DB_ENV *, DB *, DBTYPE *, char **, int *, u_int32_t *,
			      READ_FN *, void *, u_long *));
	int	__db_rconfig __P((DB_ENV *, DB *, char *, char *));
	int	__db_rdbt __P((DB_ENV *, DBT *, DBT *, u_int32_t,
			   READ_FN *, void *in, u_long *));

#define	DB_READ_HASKEYS		0x01
#define	DB_READ_PRINTABLE	0x02
#define	DB_READ_RECNO		0x04
#define	DB_READ_RECNOHEX	0x08
#define	DB_READ_HASDATAEND	0x10

#if defined(__cplusplus)
}
#endif

#endif /* !_DB_LOAD_UTIL_H_ */
