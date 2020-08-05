/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2009 Oracle.  All rights reserved.
 *
 */

#ifndef __DB_UTILS_HPP
#define	__DB_UTILS_HPP

/*
 * Handy macros from db_swap.h:
 */
#define	M_64_SWAP(a) {							\
	u_int64_t _tmp;							\
	_tmp = (u_int64_t)a;						\
	((u_int8_t *)&a)[0] = ((u_int8_t *)&_tmp)[7];			\
	((u_int8_t *)&a)[1] = ((u_int8_t *)&_tmp)[6];			\
	((u_int8_t *)&a)[2] = ((u_int8_t *)&_tmp)[5];			\
	((u_int8_t *)&a)[3] = ((u_int8_t *)&_tmp)[4];			\
	((u_int8_t *)&a)[4] = ((u_int8_t *)&_tmp)[3];			\
	((u_int8_t *)&a)[5] = ((u_int8_t *)&_tmp)[2];			\
	((u_int8_t *)&a)[6] = ((u_int8_t *)&_tmp)[1];			\
	((u_int8_t *)&a)[7] = ((u_int8_t *)&_tmp)[0];			\
}

#define	M_32_SWAP(a) {							\
	u_int32_t _tmp;							\
	_tmp = a;							\
	((u_int8_t *)&a)[0] = ((u_int8_t *)&_tmp)[3];			\
	((u_int8_t *)&a)[1] = ((u_int8_t *)&_tmp)[2];			\
	((u_int8_t *)&a)[2] = ((u_int8_t *)&_tmp)[1];			\
	((u_int8_t *)&a)[3] = ((u_int8_t *)&_tmp)[0];			\
}

#define M_16_SWAP(a) {                                                  \
        u_int16_t _tmp;                                                 \
        _tmp = (u_int16_t)a;                                            \
        ((u_int8_t *)&a)[0] = ((u_int8_t *)&_tmp)[1];                   \
        ((u_int8_t *)&a)[1] = ((u_int8_t *)&_tmp)[0];                   \
}

/* Flag helpers */
#define	LF_SET(f)		((flags) |= (f))
#define	LF_CLR(f)		((flags) &= ~(f))
#define	LF_ISSET(f)	((flags) & (f))

#endif
