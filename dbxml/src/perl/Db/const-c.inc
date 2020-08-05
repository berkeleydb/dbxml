#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_10 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_DUP" "Db::DB_RMW" "Db::DB_SET" */
  /* Offset 9 gives the best switch position.  */
  switch (name[9]) {
  case 'P':
    if (memEQ(name, "Db::DB_DU", 9)) {
    /*                        P     */
      {
        	int tmp = DB_DUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_SE", 9)) {
    /*                        T     */
      {
        	int tmp = DB_SET;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_RM", 9)) {
    /*                        W     */
      {
        	int tmp = DB_RMW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_11 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_EXCL" "Db::DB_HASH" "Db::DB_LAST" "Db::DB_NEXT" "Db::DB_PREV" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'E':
    if (memEQ(name, "Db::DB_EXCL", 11)) {
    /*                      ^          */
      {
        	int tmp = DB_EXCL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_HASH", 11)) {
    /*                      ^          */
      {
        	int tmp = DB_HASH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LAST", 11)) {
    /*                      ^          */
      {
        	int tmp = DB_LAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NEXT", 11)) {
    /*                      ^          */
      {
        	int tmp = DB_NEXT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PREV", 11)) {
    /*                      ^          */
      {
        	int tmp = DB_PREV;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_12 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AFTER" "Db::DB_BTREE" "Db::DB_FIRST" "Db::DB_FLUSH" "Db::DB_FORCE"
     "Db::DB_QUEUE" "Db::DB_RECNO" "Db::TXN_SYNC" */
  /* Offset 8 gives the best switch position.  */
  switch (name[8]) {
  case 'E':
    if (memEQ(name, "Db::DB_RECNO", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_AFTER", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_AFTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_FIRST", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_FIRST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_FLUSH", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_FLUSH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_FORCE", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_FORCE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::TXN_SYNC", 12)) {
    /*                       ^          */
      {
        	int tmp = TXN_SYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_BTREE", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_BTREE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_QUEUE", 12)) {
    /*                       ^          */
      {
        	int tmp = DB_QUEUE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_13 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_APPEND" "Db::DB_BEFORE" "Db::DB_CHKSUM" "Db::DB_CREATE"
     "Db::DB_DIRECT" "Db::DB_NOMMAP" "Db::DB_NOSYNC" "Db::DB_RDONLY"
     "Db::DB_RECNUM" "Db::DB_THREAD" "Db::DB_VERIFY" */
  /* Offset 9 gives the best switch position.  */
  switch (name[9]) {
  case 'C':
    if (memEQ(name, "Db::DB_RECNUM", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_RECNUM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_CREATE", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_BEFORE", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_BEFORE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_CHKSUM", 13)) {
    /*                        ^          */
#ifdef DB_CHKSUM
      *iv_return = DB_CHKSUM;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_NOMMAP", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_NOMMAP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_RDONLY", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_RDONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_APPEND", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_APPEND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_DIRECT", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_DIRECT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_THREAD", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_THREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERIFY", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_VERIFY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_NOSYNC", 13)) {
    /*                        ^          */
      {
        	int tmp = DB_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_14 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CONSUME" "Db::DB_CURRENT" "Db::DB_DUPSORT" "Db::DB_ENCRYPT"
     "Db::DB_JOINENV" "Db::DB_KEYLAST" "Db::DB_LOCK_NG" "Db::DB_NOPANIC"
     "Db::DB_PRIVATE" "Db::DB_RECOVER" "Db::DB_SALVAGE" "Db::DB_UNKNOWN"
     "Db::DB_UPGRADE" "Db::TXN_NOSYNC" "Db::TXN_NOWAIT" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'C':
    if (memEQ(name, "Db::DB_CONSUME", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_CONSUME;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_CURRENT", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_CURRENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_DUPSORT", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_DUPSORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_ENCRYPT", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_ENCRYPT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "Db::DB_JOINENV", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_JOINENV;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_KEYLAST", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_KEYLAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_NG", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_LOCK_NG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NOPANIC", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_NOPANIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PRIVATE", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_PRIVATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_RECOVER", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_RECOVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_SALVAGE", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_SALVAGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_UNKNOWN", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_UNKNOWN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_UPGRADE", 14)) {
    /*                      ^             */
      {
        	int tmp = DB_UPGRADE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::TXN_NOSYNC", 14)) {
    /*                      ^             */
      {
        	int tmp = TXN_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_NOWAIT", 14)) {
    /*                      ^             */
      {
        	int tmp = TXN_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_15 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ARCH_ABS" "Db::DB_ARCH_LOG" "Db::DB_GET_BOTH" "Db::DB_INIT_CDB"
     "Db::DB_INIT_LOG" "Db::DB_INIT_TXN" "Db::DB_KEYEMPTY" "Db::DB_KEYEXIST"
     "Db::DB_KEYFIRST" "Db::DB_LOCKDOWN" "Db::DB_LOCK_GET" "Db::DB_LOCK_IWR"
     "Db::DB_LOCK_PUT" "Db::DB_MULTIPLE" "Db::DB_NEXT_DUP" "Db::DB_NOSERVER"
     "Db::DB_NOTFOUND" "Db::DB_POSITION" "Db::DB_REGISTER" "Db::DB_RENUMBER"
     "Db::DB_SNAPSHOT" "Db::DB_TRUNCATE" "Db::DB_TXN_SYNC" "Db::DB_YIELDCPU" */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'A':
    if (memEQ(name, "Db::DB_ARCH_ABS", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_ARCH_ABS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TRUNCATE", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_TRUNCATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'B':
    if (memEQ(name, "Db::DB_RENUMBER", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_RENUMBER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_INIT_CDB", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_INIT_CDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_YIELDCPU", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_YIELDCPU;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_NEXT_DUP", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_NEXT_DUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_LOCK_GET", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_LOCK_GET;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_SNAPSHOT", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_SNAPSHOT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_KEYEXIST", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_KEYEXIST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_IWR", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_LOCK_IWR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_POSITION", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_POSITION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_ARCH_LOG", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_ARCH_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_INIT_LOG", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_INIT_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_GET_BOTH", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_GET_BOTH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCKDOWN", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_LOCKDOWN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_KEYEMPTY", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_KEYEMPTY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_PUT", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_LOCK_PUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MULTIPLE", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_MULTIPLE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_KEYFIRST", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_KEYFIRST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_INIT_TXN", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_INIT_TXN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REGISTER", 15)) {
    /*                           ^         */
#ifdef DB_REGISTER
      *iv_return = DB_REGISTER;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_NOTFOUND", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_NOTFOUND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "Db::DB_NOSERVER", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_NOSERVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_TXN_SYNC", 15)) {
    /*                           ^         */
      {
        	int tmp = DB_TXN_SYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_16 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ARCH_DATA" "Db::DB_CDB_ALLDB" "Db::DB_DIRECT_DB"
     "Db::DB_FAST_STAT" "Db::DB_GET_RECNO" "Db::DB_INIT_LOCK"
     "Db::DB_JOIN_ITEM" "Db::DB_LOCK_DUMP" "Db::DB_LOCK_READ"
     "Db::DB_LOCK_WAIT" "Db::DB_MPOOL_NEW" "Db::DB_NODUPDATA"
     "Db::DB_NOLOCKING" "Db::DB_OVERWRITE" "Db::DB_PRINTABLE"
     "Db::DB_SET_RANGE" "Db::DB_SET_RECNO" "Db::DB_TXN_ABORT"
     "Db::DB_TXN_APPLY" "Db::DB_TXN_PRINT" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'A':
    if (memEQ(name, "Db::DB_ARCH_DATA", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_ARCH_DATA;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_CDB_ALLDB", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_CDB_ALLDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_DIRECT_DB", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_DIRECT_DB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_FAST_STAT", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_FAST_STAT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_GET_RECNO", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_GET_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_INIT_LOCK", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_INIT_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "Db::DB_JOIN_ITEM", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_JOIN_ITEM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_DUMP", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_LOCK_DUMP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_READ", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_LOCK_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_WAIT", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_LOCK_WAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_MPOOL_NEW", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_MPOOL_NEW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NODUPDATA", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_NODUPDATA;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOLOCKING", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_NOLOCKING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_OVERWRITE", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_OVERWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PRINTABLE", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_PRINTABLE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_SET_RANGE", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_SET_RANGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SET_RECNO", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_SET_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_TXN_ABORT", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_TXN_ABORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_APPLY", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_TXN_APPLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_PRINT", 16)) {
    /*                      ^               */
      {
        	int tmp = DB_TXN_PRINT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_17 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AGGRESSIVE" "Db::DB_DIRTY_READ" "Db::DB_DONOTINDEX"
     "Db::DB_FREE_SPACE" "Db::DB_INIT_MPOOL" "Db::DB_LOCK_IREAD"
     "Db::DB_LOCK_TRADE" "Db::DB_LOCK_WRITE" "Db::DB_MPOOL_LAST"
     "Db::DB_NEXT_NODUP" "Db::DB_NOORDERCHK" "Db::DB_PREV_NODUP"
     "Db::DB_REP_CLIENT" "Db::DB_REP_IGNORE" "Db::DB_REP_MASTER"
     "Db::DB_STAT_CLEAR" "Db::DB_SYSTEM_MEM" "Db::DB_TXN_NOSYNC"
     "Db::DB_TXN_NOWAIT" "Db::DB_VERIFY_BAD" "Db::REP_CONF_BULK" */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'A':
    if (memEQ(name, "Db::DB_REP_MASTER", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_REP_MASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_STAT_CLEAR", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_STAT_CLEAR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_NOORDERCHK", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_NOORDERCHK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_REP_IGNORE", 17)) {
    /*                           ^           */
#ifdef DB_REP_IGNORE
      *iv_return = DB_REP_IGNORE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_DONOTINDEX", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_DONOTINDEX;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_IREAD", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_LOCK_IREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_REP_CLIENT", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_REP_CLIENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_INIT_MPOOL", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_INIT_MPOOL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SYSTEM_MEM", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_SYSTEM_MEM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NEXT_NODUP", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_NEXT_NODUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PREV_NODUP", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_PREV_NODUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_TXN_NOSYNC", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_TXN_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_NOWAIT", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_TXN_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_AGGRESSIVE", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_AGGRESSIVE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_FREE_SPACE", 17)) {
    /*                           ^           */
#ifdef DB_FREE_SPACE
      *iv_return = DB_FREE_SPACE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_LOCK_TRADE", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_LOCK_TRADE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_LOCK_WRITE", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_LOCK_WRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_VERIFY_BAD", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_VERIFY_BAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_DIRTY_READ", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_DIRTY_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_LAST", 17)) {
    /*                           ^           */
      {
        	int tmp = DB_MPOOL_LAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::REP_CONF_BULK", 17)) {
    /*                           ^           */
#ifdef REP_CONF_BULK
      *iv_return = REP_CONF_BULK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_18 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AUTO_COMMIT" "Db::DB_CHKSUM_SHA1" "Db::DB_DBT_PARTIAL"
     "Db::DB_EID_INVALID" "Db::DB_ENCRYPT_AES" "Db::DB_FILE_ID_LEN"
     "Db::DB_JOIN_NOSORT" "Db::DB_LOCK_EXPIRE" "Db::DB_LOCK_IWRITE"
     "Db::DB_LOCK_NOWAIT" "Db::DB_LOCK_OLDEST" "Db::DB_LOCK_RANDOM"
     "Db::DB_LOCK_WWRITE" "Db::DB_MPOOL_DIRTY" "Db::DB_NOOVERWRITE"
     "Db::DB_NOSERVER_ID" "Db::DB_ODDFILESIZE" "Db::DB_OLD_VERSION"
     "Db::DB_REGION_INIT" "Db::DB_REP_LOCKOUT" "Db::DB_REP_NEWSITE"
     "Db::DB_REP_UNAVAIL" "Db::DB_REVSPLITOFF" "Db::DB_RUNRECOVERY"
     "Db::DB_USE_ENVIRON" "Db::DB_WRITECURSOR" */
  /* Offset 13 gives the best switch position.  */
  switch (name[13]) {
  case 'A':
    if (memEQ(name, "Db::DB_LOCK_RANDOM", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_RANDOM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_UNAVAIL", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_REP_UNAVAIL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_REP_LOCKOUT", 18)) {
    /*                            ^           */
#ifdef DB_REP_LOCKOUT
      *iv_return = DB_REP_LOCKOUT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_FILE_ID_LEN", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_FILE_ID_LEN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_DIRTY", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_MPOOL_DIRTY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_NOSERVER_ID", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_NOSERVER_ID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ODDFILESIZE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_ODDFILESIZE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_REVSPLITOFF", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_REVSPLITOFF;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_OLDEST", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_OLDEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_AUTO_COMMIT", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_AUTO_COMMIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_JOIN_NOSORT", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_JOIN_NOSORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_NOWAIT", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RUNRECOVERY", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_RUNRECOVERY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_DBT_PARTIAL", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_DBT_PARTIAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_OLD_VERSION", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_OLD_VERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_ENCRYPT_AES", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_ENCRYPT_AES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_WRITECURSOR", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_WRITECURSOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "Db::DB_EID_INVALID", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_EID_INVALID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_USE_ENVIRON", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_USE_ENVIRON;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_LOCK_IWRITE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_IWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_WWRITE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_WWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOOVERWRITE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_NOOVERWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_NEWSITE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_REP_NEWSITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'X':
    if (memEQ(name, "Db::DB_LOCK_EXPIRE", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_LOCK_EXPIRE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_CHKSUM_SHA1", 18)) {
    /*                            ^           */
#ifdef DB_CHKSUM_SHA1
      *iv_return = DB_CHKSUM_SHA1;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "Db::DB_REGION_INIT", 18)) {
    /*                            ^           */
      {
        	int tmp = DB_REGION_INIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_19 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CONSUME_WAIT" "Db::DB_LOCK_DEFAULT" "Db::DB_LOCK_INHERIT"
     "Db::DB_LOCK_PUT_ALL" "Db::DB_LOCK_PUT_OBJ" "Db::DB_LOCK_TIMEOUT"
     "Db::DB_MPOOL_CREATE" "Db::DB_MULTIPLE_KEY" "Db::DB_ORDERCHKONLY"
     "Db::DB_PRIORITY_LOW" "Db::DB_REP_ANYWHERE" "Db::REP_CONF_NOWAIT" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'E':
    if (memEQ(name, "Db::DB_MULTIPLE_KEY", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_MULTIPLE_KEY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_LOCK_DEFAULT", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_LOCK_DEFAULT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_LOCK_INHERIT", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_LOCK_INHERIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_ORDERCHKONLY", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_ORDERCHKONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_LOCK_TIMEOUT", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_LOCK_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::REP_CONF_NOWAIT", 19)) {
    /*                             ^           */
#ifdef REP_CONF_NOWAIT
      *iv_return = REP_CONF_NOWAIT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_MPOOL_CREATE", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_MPOOL_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_LOCK_PUT_ALL", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_LOCK_PUT_ALL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_PUT_OBJ", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_LOCK_PUT_OBJ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_REP_ANYWHERE", 19)) {
    /*                             ^           */
#ifdef DB_REP_ANYWHERE
      *iv_return = DB_REP_ANYWHERE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_PRIORITY_LOW", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_PRIORITY_LOW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_CONSUME_WAIT", 19)) {
    /*                             ^           */
      {
        	int tmp = DB_CONSUME_WAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_EID_BROADCAST" "Db::DB_FREELIST_ONLY" "Db::DB_IMMUTABLE_KEY"
     "Db::DB_LOCK_DEADLOCK" "Db::DB_LOCK_MAXLOCKS" "Db::DB_LOCK_MINLOCKS"
     "Db::DB_LOCK_MINWRITE" "Db::DB_LOCK_PUT_READ" "Db::DB_LOCK_YOUNGEST"
     "Db::DB_NOSERVER_HOME" "Db::DB_PAGE_NOTFOUND" "Db::DB_PRIORITY_HIGH"
     "Db::DB_RECOVER_FATAL" "Db::DB_REP_DUPMASTER" "Db::DB_REP_NEWMASTER"
     "Db::DB_REP_PERMANENT" "Db::DB_REP_REREQUEST" "Db::DB_SECONDARY_BAD"
     "Db::DB_VERB_DEADLOCK" "Db::DB_VERB_RECOVERY" "Db::DB_VERB_REGISTER"
     "Db::DB_VERB_WAITSFOR" "Db::DB_VERSION_MAJOR" "Db::DB_VERSION_MINOR"
     "Db::DB_VERSION_PATCH" "Db::TXN_WRITE_NOSYNC" */
  /* Offset 15 gives the best switch position.  */
  switch (name[15]) {
  case 'A':
    if (memEQ(name, "Db::DB_REP_DUPMASTER", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_REP_DUPMASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_NEWMASTER", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_REP_NEWMASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_PERMANENT", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_REP_PERMANENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_EID_BROADCAST", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_EID_BROADCAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_DEADLOCK", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_DEADLOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERB_DEADLOCK", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERB_DEADLOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_IMMUTABLE_KEY", 20)) {
    /*                              ^           */
#ifdef DB_IMMUTABLE_KEY
      *iv_return = DB_IMMUTABLE_KEY;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_PAGE_NOTFOUND", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_PAGE_NOTFOUND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RECOVER_FATAL", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_RECOVER_FATAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_VERB_REGISTER", 20)) {
    /*                              ^           */
#ifdef DB_VERB_REGISTER
      *iv_return = DB_VERB_REGISTER;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_MAXLOCKS", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_MAXLOCKS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_MINLOCKS", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_MINLOCKS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_VERSION_MAJOR", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERSION_MAJOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERSION_MINOR", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERSION_MINOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_LOCK_YOUNGEST", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_YOUNGEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_VERB_RECOVERY", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERB_RECOVERY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_WRITE_NOSYNC", 20)) {
    /*                              ^           */
#ifdef TXN_WRITE_NOSYNC
      *iv_return = TXN_WRITE_NOSYNC;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_VERSION_PATCH", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERSION_PATCH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Q':
    if (memEQ(name, "Db::DB_REP_REREQUEST", 20)) {
    /*                              ^           */
#ifdef DB_REP_REREQUEST
      *iv_return = DB_REP_REREQUEST;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_VERB_WAITSFOR", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_VERB_WAITSFOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_LOCK_MINWRITE", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_MINWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_SECONDARY_BAD", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_SECONDARY_BAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_FREELIST_ONLY", 20)) {
    /*                              ^           */
#ifdef DB_FREELIST_ONLY
      *iv_return = DB_FREELIST_ONLY;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "Db::DB_LOCK_PUT_READ", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_LOCK_PUT_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOSERVER_HOME", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_NOSERVER_HOME;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PRIORITY_HIGH", 20)) {
    /*                              ^           */
      {
        	int tmp = DB_PRIORITY_HIGH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return, const char **pv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_GET_BOTH_RANGE" "Db::DB_READ_COMMITTED" "Db::DB_VERSION_STRING" */
  /* Offset 20 gives the best switch position.  */
  switch (name[20]) {
  case 'D':
    if (memEQ(name, "Db::DB_READ_COMMITTE", 20)) {
    /*                                   D      */
#ifdef DB_READ_COMMITTED
      *iv_return = DB_READ_COMMITTED;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_GET_BOTH_RANG", 20)) {
    /*                                   E      */
      {
        	int tmp = DB_GET_BOTH_RANGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_VERSION_STRIN", 20)) {
    /*                                   G      */
      *pv_return = DB_VERSION_STRING;
      *iv_return = (IV)17;
      return PERL_constant_ISPVN;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_23 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_LOCK_GET_TIMEOUT" "Db::DB_MUTEX_SELF_BLOCK"
     "Db::DB_PRIORITY_DEFAULT" "Db::DB_READ_UNCOMMITTED"
     "Db::DB_REP_HOLDELECTION" "Db::DB_SET_LOCK_TIMEOUT"
     "Db::DB_TXN_FORWARD_ROLL" "Db::DB_TXN_WRITE_NOSYNC"
     "Db::DB_USE_ENVIRON_ROOT" "Db::DB_VERB_REPLICATION"
     "Db::REP_CONF_NOAUTOINIT" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'C':
    if (memEQ(name, "Db::DB_READ_UNCOMMITTED", 23)) {
    /*                             ^               */
#ifdef DB_READ_UNCOMMITTED
      *iv_return = DB_READ_UNCOMMITTED;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_REP_HOLDELECTION", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_REP_HOLDELECTION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_MUTEX_SELF_BLOCK", 23)) {
    /*                             ^               */
#ifdef DB_MUTEX_SELF_BLOCK
      *iv_return = DB_MUTEX_SELF_BLOCK;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_USE_ENVIRON_ROOT", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_USE_ENVIRON_ROOT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_SET_LOCK_TIMEOUT", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_SET_LOCK_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::REP_CONF_NOAUTOINIT", 23)) {
    /*                             ^               */
#ifdef REP_CONF_NOAUTOINIT
      *iv_return = REP_CONF_NOAUTOINIT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_VERB_REPLICATION", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_VERB_REPLICATION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_LOCK_GET_TIMEOUT", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_LOCK_GET_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_WRITE_NOSYNC", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_TXN_WRITE_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_TXN_FORWARD_ROLL", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_TXN_FORWARD_ROLL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_PRIORITY_DEFAULT", 23)) {
    /*                             ^               */
      {
        	int tmp = DB_PRIORITY_DEFAULT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_24 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CXX_NO_EXCEPTIONS" "Db::DB_PANIC_ENVIRONMENT"
     "Db::DB_PRIORITY_VERY_LOW" "Db::DB_TXN_BACKWARD_ROLL"
     "Db::REP_CONF_DELAYCLIENT" */
  /* Offset 18 gives the best switch position.  */
  switch (name[18]) {
  case 'C':
    if (memEQ(name, "Db::REP_CONF_DELAYCLIENT", 24)) {
    /*                                 ^            */
#ifdef REP_CONF_DELAYCLIENT
      *iv_return = REP_CONF_DELAYCLIENT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_TXN_BACKWARD_ROLL", 24)) {
    /*                                 ^            */
      {
        	int tmp = DB_TXN_BACKWARD_ROLL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_PANIC_ENVIRONMENT", 24)) {
    /*                                 ^            */
      {
        	int tmp = DB_PANIC_ENVIRONMENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_CXX_NO_EXCEPTIONS", 24)) {
    /*                                 ^            */
      {
        	int tmp = DB_CXX_NO_EXCEPTIONS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_PRIORITY_VERY_LOW", 24)) {
    /*                                 ^            */
      {
        	int tmp = DB_PRIORITY_VERY_LOW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return, const char **pv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/usr/bin/perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV PVN)};
my @names = (qw(),
            {name=>"Db::DB_AFTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AFTER;"},
            {name=>"Db::DB_AGGRESSIVE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AGGRESSIVE;"},
            {name=>"Db::DB_APPEND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_APPEND;"},
            {name=>"Db::DB_ARCH_ABS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_ABS;"},
            {name=>"Db::DB_ARCH_DATA", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_DATA;"},
            {name=>"Db::DB_ARCH_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_LOG;"},
            {name=>"Db::DB_AUTO_COMMIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AUTO_COMMIT;"},
            {name=>"Db::DB_BEFORE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BEFORE;"},
            {name=>"Db::DB_BTREE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BTREE;"},
            {name=>"Db::DB_CDB_ALLDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CDB_ALLDB;"},
            {name=>"Db::DB_CHKSUM", type=>"IV", macro=>["#ifdef DB_CHKSUM\n", "#endif\n"], value=>"DB_CHKSUM"},
            {name=>"Db::DB_CHKSUM_SHA1", type=>"IV", macro=>["#ifdef DB_CHKSUM_SHA1\n", "#endif\n"], value=>"DB_CHKSUM_SHA1"},
            {name=>"Db::DB_CONSUME", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CONSUME;"},
            {name=>"Db::DB_CONSUME_WAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CONSUME_WAIT;"},
            {name=>"Db::DB_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CREATE;"},
            {name=>"Db::DB_CURRENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CURRENT;"},
            {name=>"Db::DB_CXX_NO_EXCEPTIONS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CXX_NO_EXCEPTIONS;"},
            {name=>"Db::DB_DBT_PARTIAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DBT_PARTIAL;"},
            {name=>"Db::DB_DIRECT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRECT;"},
            {name=>"Db::DB_DIRECT_DB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRECT_DB;"},
            {name=>"Db::DB_DIRTY_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRTY_READ;"},
            {name=>"Db::DB_DONOTINDEX", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DONOTINDEX;"},
            {name=>"Db::DB_DUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DUP;"},
            {name=>"Db::DB_DUPSORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DUPSORT;"},
            {name=>"Db::DB_EID_BROADCAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EID_BROADCAST;"},
            {name=>"Db::DB_EID_INVALID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EID_INVALID;"},
            {name=>"Db::DB_ENCRYPT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENCRYPT;"},
            {name=>"Db::DB_ENCRYPT_AES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENCRYPT_AES;"},
            {name=>"Db::DB_EXCL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EXCL;"},
            {name=>"Db::DB_FAST_STAT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FAST_STAT;"},
            {name=>"Db::DB_FILE_ID_LEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FILE_ID_LEN;"},
            {name=>"Db::DB_FIRST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FIRST;"},
            {name=>"Db::DB_FLUSH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FLUSH;"},
            {name=>"Db::DB_FORCE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FORCE;"},
            {name=>"Db::DB_FREELIST_ONLY", type=>"IV", macro=>["#ifdef DB_FREELIST_ONLY\n", "#endif\n"], value=>"DB_FREELIST_ONLY"},
            {name=>"Db::DB_FREE_SPACE", type=>"IV", macro=>["#ifdef DB_FREE_SPACE\n", "#endif\n"], value=>"DB_FREE_SPACE"},
            {name=>"Db::DB_GET_BOTH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_BOTH;"},
            {name=>"Db::DB_GET_BOTH_RANGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_BOTH_RANGE;"},
            {name=>"Db::DB_GET_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_RECNO;"},
            {name=>"Db::DB_HASH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HASH;"},
            {name=>"Db::DB_IMMUTABLE_KEY", type=>"IV", macro=>["#ifdef DB_IMMUTABLE_KEY\n", "#endif\n"], value=>"DB_IMMUTABLE_KEY"},
            {name=>"Db::DB_INIT_CDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_CDB;"},
            {name=>"Db::DB_INIT_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_LOCK;"},
            {name=>"Db::DB_INIT_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_LOG;"},
            {name=>"Db::DB_INIT_MPOOL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_MPOOL;"},
            {name=>"Db::DB_INIT_TXN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_TXN;"},
            {name=>"Db::DB_JOINENV", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOINENV;"},
            {name=>"Db::DB_JOIN_ITEM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOIN_ITEM;"},
            {name=>"Db::DB_JOIN_NOSORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOIN_NOSORT;"},
            {name=>"Db::DB_KEYEMPTY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYEMPTY;"},
            {name=>"Db::DB_KEYEXIST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYEXIST;"},
            {name=>"Db::DB_KEYFIRST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYFIRST;"},
            {name=>"Db::DB_KEYLAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYLAST;"},
            {name=>"Db::DB_LAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LAST;"},
            {name=>"Db::DB_LOCKDOWN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCKDOWN;"},
            {name=>"Db::DB_LOCK_DEADLOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DEADLOCK;"},
            {name=>"Db::DB_LOCK_DEFAULT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DEFAULT;"},
            {name=>"Db::DB_LOCK_DUMP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DUMP;"},
            {name=>"Db::DB_LOCK_EXPIRE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_EXPIRE;"},
            {name=>"Db::DB_LOCK_GET", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_GET;"},
            {name=>"Db::DB_LOCK_GET_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_GET_TIMEOUT;"},
            {name=>"Db::DB_LOCK_INHERIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_INHERIT;"},
            {name=>"Db::DB_LOCK_IREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IREAD;"},
            {name=>"Db::DB_LOCK_IWR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IWR;"},
            {name=>"Db::DB_LOCK_IWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IWRITE;"},
            {name=>"Db::DB_LOCK_MAXLOCKS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MAXLOCKS;"},
            {name=>"Db::DB_LOCK_MINLOCKS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MINLOCKS;"},
            {name=>"Db::DB_LOCK_MINWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MINWRITE;"},
            {name=>"Db::DB_LOCK_NG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NG;"},
            {name=>"Db::DB_LOCK_NOTGRANTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NOTGRANTED;"},
            {name=>"Db::DB_LOCK_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NOWAIT;"},
            {name=>"Db::DB_LOCK_OLDEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_OLDEST;"},
            {name=>"Db::DB_LOCK_PUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT;"},
            {name=>"Db::DB_LOCK_PUT_ALL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_ALL;"},
            {name=>"Db::DB_LOCK_PUT_OBJ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_OBJ;"},
            {name=>"Db::DB_LOCK_PUT_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_READ;"},
            {name=>"Db::DB_LOCK_RANDOM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_RANDOM;"},
            {name=>"Db::DB_LOCK_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_READ;"},
            {name=>"Db::DB_LOCK_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_TIMEOUT;"},
            {name=>"Db::DB_LOCK_TRADE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_TRADE;"},
            {name=>"Db::DB_LOCK_UPGRADE_WRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_UPGRADE_WRITE;"},
            {name=>"Db::DB_LOCK_WAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WAIT;"},
            {name=>"Db::DB_LOCK_WRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WRITE;"},
            {name=>"Db::DB_LOCK_WWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WWRITE;"},
            {name=>"Db::DB_LOCK_YOUNGEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_YOUNGEST;"},
            {name=>"Db::DB_MPOOL_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_CREATE;"},
            {name=>"Db::DB_MPOOL_DIRTY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_DIRTY;"},
            {name=>"Db::DB_MPOOL_LAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_LAST;"},
            {name=>"Db::DB_MPOOL_NEW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_NEW;"},
            {name=>"Db::DB_MULTIPLE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MULTIPLE;"},
            {name=>"Db::DB_MULTIPLE_KEY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MULTIPLE_KEY;"},
            {name=>"Db::DB_MUTEX_SELF_BLOCK", type=>"IV", macro=>["#ifdef DB_MUTEX_SELF_BLOCK\n", "#endif\n"], value=>"DB_MUTEX_SELF_BLOCK"},
            {name=>"Db::DB_NEXT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT;"},
            {name=>"Db::DB_NEXT_DUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT_DUP;"},
            {name=>"Db::DB_NEXT_NODUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT_NODUP;"},
            {name=>"Db::DB_NODUPDATA", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NODUPDATA;"},
            {name=>"Db::DB_NOLOCKING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOLOCKING;"},
            {name=>"Db::DB_NOMMAP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOMMAP;"},
            {name=>"Db::DB_NOORDERCHK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOORDERCHK;"},
            {name=>"Db::DB_NOOVERWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOOVERWRITE;"},
            {name=>"Db::DB_NOPANIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOPANIC;"},
            {name=>"Db::DB_NOSERVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER;"},
            {name=>"Db::DB_NOSERVER_HOME", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER_HOME;"},
            {name=>"Db::DB_NOSERVER_ID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER_ID;"},
            {name=>"Db::DB_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSYNC;"},
            {name=>"Db::DB_NOTFOUND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOTFOUND;"},
            {name=>"Db::DB_ODDFILESIZE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ODDFILESIZE;"},
            {name=>"Db::DB_OLD_VERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OLD_VERSION;"},
            {name=>"Db::DB_ORDERCHKONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ORDERCHKONLY;"},
            {name=>"Db::DB_OVERWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OVERWRITE;"},
            {name=>"Db::DB_PAGE_NOTFOUND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PAGE_NOTFOUND;"},
            {name=>"Db::DB_PANIC_ENVIRONMENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PANIC_ENVIRONMENT;"},
            {name=>"Db::DB_POSITION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_POSITION;"},
            {name=>"Db::DB_PREV", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PREV;"},
            {name=>"Db::DB_PREV_NODUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PREV_NODUP;"},
            {name=>"Db::DB_PRINTABLE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRINTABLE;"},
            {name=>"Db::DB_PRIORITY_DEFAULT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_DEFAULT;"},
            {name=>"Db::DB_PRIORITY_HIGH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_HIGH;"},
            {name=>"Db::DB_PRIORITY_LOW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_LOW;"},
            {name=>"Db::DB_PRIORITY_VERY_HIGH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_VERY_HIGH;"},
            {name=>"Db::DB_PRIORITY_VERY_LOW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_VERY_LOW;"},
            {name=>"Db::DB_PRIVATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIVATE;"},
            {name=>"Db::DB_QUEUE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_QUEUE;"},
            {name=>"Db::DB_RDONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RDONLY;"},
            {name=>"Db::DB_READ_COMMITTED", type=>"IV", macro=>["#ifdef DB_READ_COMMITTED\n", "#endif\n"], value=>"DB_READ_COMMITTED"},
            {name=>"Db::DB_READ_UNCOMMITTED", type=>"IV", macro=>["#ifdef DB_READ_UNCOMMITTED\n", "#endif\n"], value=>"DB_READ_UNCOMMITTED"},
            {name=>"Db::DB_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECNO;"},
            {name=>"Db::DB_RECNUM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECNUM;"},
            {name=>"Db::DB_RECOVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECOVER;"},
            {name=>"Db::DB_RECOVER_FATAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECOVER_FATAL;"},
            {name=>"Db::DB_REGION_INIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REGION_INIT;"},
            {name=>"Db::DB_REGISTER", type=>"IV", macro=>["#ifdef DB_REGISTER\n", "#endif\n"], value=>"DB_REGISTER"},
            {name=>"Db::DB_RENUMBER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RENUMBER;"},
            {name=>"Db::DB_REP_ANYWHERE", type=>"IV", macro=>["#ifdef DB_REP_ANYWHERE\n", "#endif\n"], value=>"DB_REP_ANYWHERE"},
            {name=>"Db::DB_REP_CLIENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_CLIENT;"},
            {name=>"Db::DB_REP_DUPMASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_DUPMASTER;"},
            {name=>"Db::DB_REP_HOLDELECTION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_HOLDELECTION;"},
            {name=>"Db::DB_REP_IGNORE", type=>"IV", macro=>["#ifdef DB_REP_IGNORE\n", "#endif\n"], value=>"DB_REP_IGNORE"},
            {name=>"Db::DB_REP_LOCKOUT", type=>"IV", macro=>["#ifdef DB_REP_LOCKOUT\n", "#endif\n"], value=>"DB_REP_LOCKOUT"},
            {name=>"Db::DB_REP_MASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_MASTER;"},
            {name=>"Db::DB_REP_NEWMASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_NEWMASTER;"},
            {name=>"Db::DB_REP_NEWSITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_NEWSITE;"},
            {name=>"Db::DB_REP_PERMANENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_PERMANENT;"},
            {name=>"Db::DB_REP_REREQUEST", type=>"IV", macro=>["#ifdef DB_REP_REREQUEST\n", "#endif\n"], value=>"DB_REP_REREQUEST"},
            {name=>"Db::DB_REP_UNAVAIL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_UNAVAIL;"},
            {name=>"Db::DB_REVSPLITOFF", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REVSPLITOFF;"},
            {name=>"Db::DB_RMW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RMW;"},
            {name=>"Db::DB_RUNRECOVERY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RUNRECOVERY;"},
            {name=>"Db::DB_SALVAGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SALVAGE;"},
            {name=>"Db::DB_SECONDARY_BAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SECONDARY_BAD;"},
            {name=>"Db::DB_SET", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET;"},
            {name=>"Db::DB_SET_LOCK_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_LOCK_TIMEOUT;"},
            {name=>"Db::DB_SET_RANGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_RANGE;"},
            {name=>"Db::DB_SET_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_RECNO;"},
            {name=>"Db::DB_SET_TXN_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_TXN_TIMEOUT;"},
            {name=>"Db::DB_SNAPSHOT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SNAPSHOT;"},
            {name=>"Db::DB_STAT_CLEAR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_STAT_CLEAR;"},
            {name=>"Db::DB_SYSTEM_MEM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SYSTEM_MEM;"},
            {name=>"Db::DB_THREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_THREAD;"},
            {name=>"Db::DB_TRUNCATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TRUNCATE;"},
            {name=>"Db::DB_TXN_ABORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_ABORT;"},
            {name=>"Db::DB_TXN_APPLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_APPLY;"},
            {name=>"Db::DB_TXN_BACKWARD_ROLL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_BACKWARD_ROLL;"},
            {name=>"Db::DB_TXN_FORWARD_ROLL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_FORWARD_ROLL;"},
            {name=>"Db::DB_TXN_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_NOSYNC;"},
            {name=>"Db::DB_TXN_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_NOWAIT;"},
            {name=>"Db::DB_TXN_PRINT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_PRINT;"},
            {name=>"Db::DB_TXN_SYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_SYNC;"},
            {name=>"Db::DB_TXN_WRITE_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_WRITE_NOSYNC;"},
            {name=>"Db::DB_UNKNOWN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_UNKNOWN;"},
            {name=>"Db::DB_UPGRADE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_UPGRADE;"},
            {name=>"Db::DB_USE_ENVIRON", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_USE_ENVIRON;"},
            {name=>"Db::DB_USE_ENVIRON_ROOT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_USE_ENVIRON_ROOT;"},
            {name=>"Db::DB_VERB_DEADLOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_DEADLOCK;"},
            {name=>"Db::DB_VERB_RECOVERY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_RECOVERY;"},
            {name=>"Db::DB_VERB_REGISTER", type=>"IV", macro=>["#ifdef DB_VERB_REGISTER\n", "#endif\n"], value=>"DB_VERB_REGISTER"},
            {name=>"Db::DB_VERB_REPLICATION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_REPLICATION;"},
            {name=>"Db::DB_VERB_WAITSFOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_WAITSFOR;"},
            {name=>"Db::DB_VERIFY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERIFY;"},
            {name=>"Db::DB_VERIFY_BAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERIFY_BAD;"},
            {name=>"Db::DB_VERSION_MAJOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_MAJOR;"},
            {name=>"Db::DB_VERSION_MINOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_MINOR;"},
            {name=>"Db::DB_VERSION_PATCH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_PATCH;"},
            {name=>"Db::DB_VERSION_STRING", type=>"PVN", macro=>"1", value=>["DB_VERSION_STRING", "17"]},
            {name=>"Db::DB_WRITECURSOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_WRITECURSOR;"},
            {name=>"Db::DB_YIELDCPU", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_YIELDCPU;"},
            {name=>"Db::REP_CONF_BULK", type=>"IV", macro=>["#ifdef REP_CONF_BULK\n", "#endif\n"], value=>"REP_CONF_BULK"},
            {name=>"Db::REP_CONF_DELAYCLIENT", type=>"IV", macro=>["#ifdef REP_CONF_DELAYCLIENT\n", "#endif\n"], value=>"REP_CONF_DELAYCLIENT"},
            {name=>"Db::REP_CONF_NOAUTOINIT", type=>"IV", macro=>["#ifdef REP_CONF_NOAUTOINIT\n", "#endif\n"], value=>"REP_CONF_NOAUTOINIT"},
            {name=>"Db::REP_CONF_NOWAIT", type=>"IV", macro=>["#ifdef REP_CONF_NOWAIT\n", "#endif\n"], value=>"REP_CONF_NOWAIT"},
            {name=>"Db::TXN_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_NOSYNC;"},
            {name=>"Db::TXN_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_NOWAIT;"},
            {name=>"Db::TXN_SYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_SYNC;"},
            {name=>"Db::TXN_WRITE_NOSYNC", type=>"IV", macro=>["#ifdef TXN_WRITE_NOSYNC\n", "#endif\n"], value=>"TXN_WRITE_NOSYNC"});

print constant_types(); # macro defs
foreach (C_constant ("Db", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "#### XS Section:\n";
print XS_constant ("Db", $types);
__END__
   */

  switch (len) {
  case 10:
    return constant_10 (aTHX_ name, iv_return);
    break;
  case 11:
    return constant_11 (aTHX_ name, iv_return);
    break;
  case 12:
    return constant_12 (aTHX_ name, iv_return);
    break;
  case 13:
    return constant_13 (aTHX_ name, iv_return);
    break;
  case 14:
    return constant_14 (aTHX_ name, iv_return);
    break;
  case 15:
    return constant_15 (aTHX_ name, iv_return);
    break;
  case 16:
    return constant_16 (aTHX_ name, iv_return);
    break;
  case 17:
    return constant_17 (aTHX_ name, iv_return);
    break;
  case 18:
    return constant_18 (aTHX_ name, iv_return);
    break;
  case 19:
    return constant_19 (aTHX_ name, iv_return);
    break;
  case 20:
    return constant_20 (aTHX_ name, iv_return);
    break;
  case 21:
    return constant_21 (aTHX_ name, iv_return, pv_return);
    break;
  case 22:
    /* Names all of length 22.  */
    /* "Db::DB_LOCK_NOTGRANTED" "Db::DB_SET_TXN_TIMEOUT" */
    /* Offset 13 gives the best switch position.  */
    switch (name[13]) {
    case 'N':
      if (memEQ(name, "Db::DB_SET_TXN_TIMEOUT", 22)) {
      /*                            ^               */
        {
          	int tmp = DB_SET_TXN_TIMEOUT;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    case 'O':
      if (memEQ(name, "Db::DB_LOCK_NOTGRANTED", 22)) {
      /*                            ^               */
        {
          	int tmp = DB_LOCK_NOTGRANTED;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    }
    break;
  case 23:
    return constant_23 (aTHX_ name, iv_return);
    break;
  case 24:
    return constant_24 (aTHX_ name, iv_return);
    break;
  case 25:
    /* Names all of length 25.  */
    /* "Db::DB_LOCK_UPGRADE_WRITE" "Db::DB_PRIORITY_VERY_HIGH" */
    /* Offset 17 gives the best switch position.  */
    switch (name[17]) {
    case 'D':
      if (memEQ(name, "Db::DB_LOCK_UPGRADE_WRITE", 25)) {
      /*                                ^              */
        {
          	int tmp = DB_LOCK_UPGRADE_WRITE;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    case 'E':
      if (memEQ(name, "Db::DB_PRIORITY_VERY_HIGH", 25)) {
      /*                                ^              */
        {
          	int tmp = DB_PRIORITY_VERY_HIGH;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

