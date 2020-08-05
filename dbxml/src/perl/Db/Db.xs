
#include <math.h>
//#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "db_cxx.h"

using namespace std;

extern "C" {

  #include "EXTERN.h"
  #include "perl.h"
  #include "XSUB.h"
  
  //#include "../ppport.h"
  
  #include "const-c.inc"

}

#include "../common.h"

#undef close
#undef open
#undef abort
#undef dup


typedef std::string	std__string;

typedef	std::exception std__exception;
//typedef	MyBaseException std__exception ;

typedef Dbt	Dbt_or_pv;


MODULE = Db		PACKAGE = Db


INCLUDE: const-xs.inc

BOOT:
    {
	SV * callers_line = perl_get_sv(CALLERS_LINENO, GV_ADD|GV_ADDMULTI) ;
	sv_setiv(callers_line, -1);
	SV * callers_filename = perl_get_sv(CALLERS_FILENAME, 
						GV_ADD|GV_ADDMULTI) ;
	sv_setpv(callers_filename, "");
	    
    }

MODULE = DbException	PACKAGE = DbException

#define DbException    MyDbException

void
DbException::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
DbException::get_errno()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = std::exception	PACKAGE = std::exception


void
MyBaseException::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

const char *
MyBaseException::what()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = DbEnv		PACKAGE = DbEnv		


DbEnv*
DbEnv::new(u_int32_t flags=0)
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
DbEnv::DESTROY()
	INIT:
        //printf("DbEnv::DESTROY()\n");
	    MY_TRY
	    if (! isClosed() )
            THIS->close(0);
	CLEANUP:
	    MY_CATCH
        //printf("end DbEnv::DESTROY()\n");

int
DbEnv::close(u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    setClosed();
	    MY_CATCH

int 
DbEnv::dbremove(DbTxn *txn, const char *name, const char *subdb, u_int32_t flags)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	    
int 
DbEnv::dbrename(DbTxn *txn, const char *name, const char *subdb, const char *newname, u_int32_t flags)	    
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


int 
DbEnv::open(SV * db_home, u_int32_t flags=0, int mode=0)	    
	INIT:
	    MY_TRY
	CODE:
	{
	    const char * ptr = NULL;
	    getPVorNULL(db_home, ptr);
	    RETVAL = THIS->open(ptr, flags, mode);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int 
DbEnv::remove(SV * db_home, u_int32_t flags=0)	    
	INIT:
	    MY_TRY
	CODE:
	{
	    const char * ptr = NULL;
	    getPVorNULL(db_home, ptr);
	    RETVAL = THIS->remove(ptr, flags);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_verbose(u_int32_t which, u_int32_t onoff)	    
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::txn_checkpoint(u_int32_t kbyte=0, u_int32_t min=0, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


char*
strerror(int error)
	NOT_IMPLEMENTED_YET

char*
version(int major, int minor, int patch)
	INIT:
	    MY_TRY
	CODE:
            RETVAL = DbEnv::version(&major, &minor, &patch);
	OUTPUT:
	    RETVAL
	    major
	    minor
	    patch
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_cachesize(u_int32_t gbyte, u_int32_t bytes, int ncache)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_data_dir(const char * dir)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_encrypt(const char * pawwsd, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_flags(u_int32_t flags, int onoff)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_lk_detect(u_int32_t detect)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_shm_key(long shm_key)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int 
DbEnv::set_tx_max(u_int32_t tx_max)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

DbTxn* 
DbEnv::txn_begin(DbTxn* parent=NULL, u_int32_t flags=0)
    SV * PARENT = ST(0) ;
	char *	CLASS = (char *)"DbTxn";
	INIT:
	    MY_TRY
	CODE:
	    (void)THIS->txn_begin(parent, &RETVAL, flags);
	    //cout << "txn_begin - txnid is " << RETVAL << endl ;
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = DbTxn		PACKAGE = DbTxn		


void
DbTxn::DESTROY()
	INIT:
	    MY_TRY
	CODE:
	    if (! isClosed() )
            THIS->abort();
	CLEANUP:
	    MY_CATCH


int
DbTxn::abort()
	INIT:
	    MY_TRY
	    setClosed();
	CLEANUP:
	    MY_CATCH

int
DbTxn::commit(u_int32_t flags=0)
	INIT:
	    MY_TRY
	    setClosed();
	CLEANUP:
	    MY_CATCH

int
DbTxn::discard(u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t
DbTxn::id()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
DbTxn::prepare()
	NOT_IMPLEMENTED_YET

int
DbTxn::set_timeout(db_timeout_t timeout, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = Dbt0	PACKAGE = Dbt0

Dbt*
Dbt0::new()
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new Dbt;
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = Dbt2	PACKAGE = Dbt2

Dbt*
Dbt2::new(SV* data, int size=-1)
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
	CODE:
	{
	    STRLEN len ;
	    void * dat = (void*)SvPV(data, len);
	    if (size < 0)
                size = len;
	    RETVAL = new Dbt(dat, (size_t)size);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = Dbt		PACKAGE = Dbt		

void
Dbt::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

SV*
Dbt::get_data()
	INIT:
	    MY_TRY
	CODE:
        {
	    u_int32_t size ;
	    char* ptr ;
	    size = THIS->get_size();
	    ptr = (char*)THIS->get_data();
	    RETVAL = newSVpvn(ptr, size);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
Dbt::set_data(SV* data)
	INIT:
	    MY_TRY
	CODE:
        {
	    STRLEN len ;
	    void * dat = (void*)SvPV(data, len);
	    THIS->set_data(dat);
	    THIS->set_size(len);
        }
	CLEANUP:
	    MY_CATCH

u_int32_t
Dbt::get_size()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
Dbt::set_size(u_int32_t size)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t
Dbt::get_ulen()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
Dbt::set_ulen(u_int32_t ulen)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

	
u_int32_t
Dbt::get_dlen()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
Dbt::set_dlen(u_int32_t dlen)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t
Dbt::get_flags()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
Dbt::set_flags(u_int32_t flags)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


MODULE = Db_		PACKAGE = Db_		

Db*
Db::new(dbenv=NULL, flags=0)
    DbEnv * dbenv
    int flags
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
    POSTCALL:
        PARENT = dbenv ? ST(1) : NULL ;
	CLEANUP:
	    MY_CATCH


MODULE = Db		PACKAGE = Db		

void
Db::DESTROY()
	INIT:
        //printf("Db::DESTROY()\n");
	    MY_TRY
	    if (! isClosed() )
            THIS->close(0);
	CLEANUP:
	    MY_CATCH

int
Db::_associate(DbTxn* txnid, Db* secondary, int callback, u_int32_t flags=0)
	NOT_IMPLEMENTED_YET

void
Db::close(u_int32_t flags=0)
	INIT:
	    MY_TRY
	    setClosed();
	CLEANUP:
	    MY_CATCH

int
Db::_del(DbTxn* txnid, SV* key, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    Dbt* dbt_key ;
	    bool del_key = false ;
	    STRLEN len ;

	    getDBT(key, dbt_key, del_key);
            RETVAL = THIS->del(txnid, dbt_key, flags);

	    if (del_key) {
	        delete dbt_key;
	    }
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::err(int error, const char* fmt,...)
	NOT_IMPLEMENTED_YET

int
Db::errx(const char* fmt,...)
	NOT_IMPLEMENTED_YET

int
Db::fd()
	INIT:
	    MY_TRY
	CODE:
        {
	    (void)THIS->fd(&RETVAL);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


#define flagSet(bitmask)	((flags & DB_OPFLAGS_MASK) == (bitmask))

#define writeToKey() (flagSet(DB_CONSUME)||flagSet(DB_CONSUME_WAIT)||flagSet(DB_GET_BOTH)||flagSet(DB_SET_RECNO))

int
Db::_get(DbTxn* txnid, SV* key, SV* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    Dbt* dbt_key ;
	    Dbt* dbt_data ;
	    bool del_key = false ;
	    bool del_data = false ;
	    STRLEN len ;

	    getDBT(key, dbt_key, del_key);
	    getDBT(data, dbt_data, del_data);

	    RETVAL = THIS->get(txnid, dbt_key, dbt_data, flags);

	    if (del_key) {
		if (writeToKey())
		    putDBT(key, dbt_key);
	        delete dbt_key;
	    }

	    if (del_data) {
		putDBT(data, dbt_data);
	        delete dbt_data;
	    }
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::_pget(DbTxn* txnid, SV* key, SV* pkey, SV* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    Dbt* dbt_key ;
	    Dbt* dbt_pkey ;
	    Dbt* dbt_data ;
	    bool del_key = false ;
	    bool del_pkey = false ;
	    bool del_data = false ;
	    STRLEN len ;

	    getDBT(key, dbt_key, del_key);
	    getDBT(pkey, dbt_pkey, del_pkey);
	    getDBT(data, dbt_data, del_data);

	    RETVAL = THIS->pget(txnid, dbt_key, dbt_pkey, dbt_data, flags);

	    if (del_key) {
		if (writeToKey())
		    putDBT(key, dbt_key);
	        delete dbt_key;
	    }

	    if (del_pkey) {
		putDBT(pkey, dbt_pkey);
	        delete dbt_pkey;
	    }

	    if (del_data) {
		putDBT(data, dbt_data);
	        delete dbt_data;
	    }
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::get_byteswapped()
	INIT:
	    MY_TRY
	CODE:
        {
	    (void)THIS->get_byteswapped(&RETVAL);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

DBTYPE
Db::get_type()
	INIT:
	    MY_TRY
	CODE:
        {
	    (void)THIS->get_type(&RETVAL);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::join()
	NOT_IMPLEMENTED_YET

int
Db::key_range()
	NOT_IMPLEMENTED_YET

int
Db::_open(DbTxn* txnid=NULL, SV* file=NULL, SV* database=NULL, int type=DB_HASH, u_int32_t flags=0, int mode=0777)
	INIT:
	    MY_TRY
	CODE:
        {
	    char * db ;
	    char * f ;

	    getPVorNULL(file, f);
	    getPVorNULL(database, db);

	    RETVAL = THIS->open(txnid, f, db, (DBTYPE)type, flags, mode);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::_put(DbTxn* txnid, SV* key, SV* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    Dbt* dbt_key ;
	    Dbt* dbt_data ;
	    bool del_key = false ;
	    bool del_data = false ;
	    STRLEN len ;

	    getDBT(key, dbt_key, del_key);
	    getDBT(data, dbt_data, del_data);

	    RETVAL = THIS->put(txnid, dbt_key, dbt_data, flags);

	    if (del_key)
	        delete dbt_key;

	    if (del_data)
	        delete dbt_data;
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::remove(const char* file, const char* database, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
Db::rename(const char* file, const char* database, const char* newname, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
Db::stat()
	NOT_IMPLEMENTED_YET

int
Db::sync(u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
Db::_truncate(DbTxn* txnid, u_int32_t count, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    RETVAL = THIS->truncate(txnid, &count, flags);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Db::upgrade(const char* file, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
Db::verify(const char* file, const char* database, const char* outfile, u_int32_t flags=0)
	NOT_IMPLEMENTED_YET


Dbc*
Db::_cursor(DbTxn* txnid=NULL, u_int32_t flags=0)
    SV * PARENT = ST(0) ;
	char *	CLASS = (char *)"Dbc";
	INIT:
	    MY_TRY
	CODE:
        {
	    (void)THIS->cursor(txnid, &RETVAL, flags);
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = Dbc		PACKAGE = Dbc		

void
Dbc::DESTROY()
	INIT:
	    MY_TRY
	CODE:
        {
            if (! isClosed() )
                THIS->close();
        }
	CLEANUP:
	    MY_CATCH

int
Dbc::close()
	INIT:
	    MY_TRY
	    setClosed();
	CLEANUP:
	    MY_CATCH

int
Dbc::count(db_recno_t count, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	   RETVAL = THIS->count(&count, flags); 
        }
	OUTPUT:
	    RETVAL
	    count
	CLEANUP:
	    MY_CATCH

int
Dbc::del(u_int32_t flags)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

Dbc*
Dbc::dup(u_int32_t flags=0)
    SV * PARENT = ST(0) ;
	char *	CLASS = (char *)"Dbc";
	INIT:
	    MY_TRY
	CODE:
        {
	   (void)THIS->dup(&RETVAL, flags); 
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Dbc::get(SV* key, SV* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CODE:
        {
	    Dbt* dbt_key ;
	    Dbt* dbt_data ;
	    bool del_key = false ;
	    bool del_data = false ;
	    STRLEN len ;

	    getDBT(key, dbt_key, del_key);
	    getDBT(data, dbt_data, del_data);

	    RETVAL = THIS->get(dbt_key, dbt_data, flags);

	    if (del_key) {
		putDBT(key, dbt_key);
	        delete dbt_key;
	    }

	    if (del_data) {
		putDBT(data, dbt_data);
	        delete dbt_data;
	    }
        }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

int
Dbc::pget(Dbt* key, Dbt* pkey, Dbt* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
Dbc::put(Dbt* key, Dbt* data, u_int32_t flags=0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = Sleepycat::Db		PACKAGE = Sleepycat::Db


BOOT:
{
  int Major, Minor, Patch;
  (void) db_version(&Major, &Minor, &Patch);
	
  /* check that the versions of db.h and libdb are compatible */
  if (Major != DB_VERSION_MAJOR || Minor != DB_VERSION_MINOR) {
    croak("\nBerkeley DB needs compatible versions of libdb and db.h\n\t you have db.h version %d.%d.%d and libdb version %d.%d.%d\n", 
	  DB_VERSION_MAJOR, DB_VERSION_MINOR, DB_VERSION_PATCH,
	  Major, Minor, Patch);
  }
}
