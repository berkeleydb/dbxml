
#undef bool
#undef abort
#undef rename

#define ZMALLOC(to, typ) ((to = (typ *)safemalloc(sizeof(typ))), \
				Zero(to,1,typ))

#define CALLERS_FILENAME	"Db::_filename"
#define CALLERS_LINENO		"Db::_line"

#if 0
# define DEBUG_EXC(x) \
    {									\
	int line_no;							\
	string filename ;						\
	my_get_caller(line_no, filename);				\
	cout << "Caught " << x << ": " << e.what() << " in "		\
	     << filename << ", line " << line_no << endl;		\
    }

#else
# define DEBUG_EXC(x) 
#endif

#define doCatch(real, type, name)					\
    catch(real & e)							\
    {								        \
        DEBUG_EXC(name);						\
        type * e_copy = new type(e);				        \
        SV * except = sv_newmortal();				        \
        sv_setref_pv(except, name, (void*)e_copy);			\
        SV * errsv = get_sv("@", TRUE);				        \
        sv_setsv(errsv, except);					\
        croak(Nullch);						        \
    }

#define x_MY_CATCH	\

#ifdef DBXML_VERSION_MAJOR
#  define doXMLExc        doCatch(XmlException, MyXmlException, "XmlException")
#else
#  define doXMLExc        
#endif

#define MY_TRY	try {

#define MY_CATCH							\
        }								\
        doXMLExc							\
        doCatch(DbLockNotGrantedException, MyDbException, "DbLockNotGrantedException")	\
        doCatch(DbRunRecoveryException, MyDbException, "DbRunRecoveryException")	\
        doCatch(DbDeadlockException, MyDbException, "DbDeadlockException")\
        doCatch(DbException, MyDbException, "DbException")		\
        doCatch(std::exception, MyBaseException, "std::exception")	\
	catch(...)							\
	{								\
	    MyUnknownException * e_copy = new MyUnknownException();	\
            SV * except = sv_newmortal();				\
            sv_setref_pv(except, "UnknownException", (void*)e_copy);	\
            SV * errsv = get_sv("@", TRUE);				\
	    sv_setsv(errsv, except);					\
	    croak(Nullch);						\
	}								\
	{								\
	    SV* sv = perl_get_sv(CALLERS_LINENO, FALSE) ;		\
	    sv_setiv(sv, -1);						\
	}

#define String2Char(method) 						\
	    std::string tmp = THIS->method();				\
	    RETVAL = tmp.c_str() ;

#define newSVfromString(str) sv_2mortal(newSVpvn(str.c_str(), str.length()))
#define setSVfromString(sv, str) sv_setpvn(sv, str.c_str(), str.length())

#define newSVfromXmlData(data) sv_2mortal(newSVpvn((char*)data.get_data(), data.get_size()))
#define setSVfromXmlData(sv,data) sv_setpvn(sv, (char*)data.get_data(), data.get_size())

#define newSVfromXmlValue(value) { std::string str = value.asString(); sv_2mortal(newSVpvn((char*)str.c_str(), str.length())); }
#define setSVfromXmlValue(sv,value) { std::string str = value.asString(); sv_setpvn(sv, (char*)str.c_str(), str.length()); }


#define setXmlDatafromSV(xd, sv) (xd.set_data(SvPV_nolen(sv)), xd.set_size(SvCUR(sv)))
#define StringfromSV(str, sv)                               \
        {                                                   \
            STRLEN len ;                                    \
            const char * ptr = SvPV(sv, len);               \
            str.assign(ptr, len) ;                          \
        }

#define setXmlValuefromSV(str, sv)                          \
        {                                                   \
            STRLEN len ;                                    \
            const char * ptr = SvPV(sv, len);               \
            std::string str(ptr, len) ;                     \
            XmlValue x(str);                                \
            xv = x ;                                        \
        }

#define getDBT(from, to, del)						\
	    if (from == &PL_sv_undef || SvTYPE(from) == SVt_NULL){	\
	        to = new Dbt;						\
		del = true ;						\
	    }								\
            else if (sv_isobject(from) && (SvTYPE(SvRV(from)) == SVt_PVMG))\
	        to = (Dbt*)SvIV((SV*)SvRV( from ));			\
	    else {							\
		char * ptr = SvPV(from, len);				\
	        to = new Dbt(ptr, len);					\
		del = true ;						\
	    }	

#define putDBT(data, dbt_data)						\
                sv_setpvn(data, (char*)dbt_data->get_data(), 		\
				dbt_data->get_size());			

#define getPVorNULL(from, to)						\
            if (from == NULL || from == &PL_sv_undef || SvTYPE(from) == SVt_NULL)\
                to = NULL ;						\
            else {							\
	        STRLEN my_na ;						\
                to = (char*)SvPV(from,my_na) ;				\
                if (my_na == 0)						\
                    to = NULL ;						\
            }

#define Sv_ISA(i, type) 	sv_derived_from(ST(i), type)
//#define GetObjPointer(i, type) 	((type*)SvIV((SV*)SvRV(i)))
#define GetObjPointer(i, type) 	((type*)SvIV(*av_fetch((AV*)SvRV(i), 0, FALSE)))
//#define ClearObjPointer(i) 	sv_setiv((SV*)SvRV(i), 0)
#define ClearObjPointer(i) 	sv_setiv(*av_fetch((AV*)SvRV(i), 0, FALSE), 0)
#define GetStringPointer(i)	(char *)SvPV_nolen(ST(i));
#define GetUV(i, type)		(type)SvUV(ST(i));

#define SV_IS_OBJECT(x) (sv_isobject(x) && (SvTYPE(SvRV(x)) == SVt_PVMG))

#define getXmlValue(var, arg, lex)  \
    LexPointer<XmlValue> lex ;      \
    if (arg == NULL || ! SvOK(arg)) \
    {                               \
        var = new XmlValue();   \
        lex.setPtr(var) ;   \
    }   \
    else if (! sv_isobject(arg) )   \
    {   \
        string s(SvPV_nolen(arg), SvCUR(arg));  \
        var = new XmlValue(s);  \
        lex.setPtr(var) ;   \
    }   \
    else if (sv_derived_from(arg, "XmlValue")) \
    {   \
        var = GetObjPointer(arg, XmlValue); \
    }   

#define OutputObject(arg, var, Class, Parent)   \
    {   \
        HV* stash = gv_stashpv(Class, TRUE);    \
        AV * av = (AV *)sv_2mortal((SV*)newAV());  \
        av_push(av, newSViv(PTR2IV((void*)var)));   \
        av_push(av, newSViv(FALSE)); /* ! Closed */  \
        if (Parent) \
            av_push(av, newRV_inc(Parent));     \
        sv_setsv(arg, (SV*)newRV_noinc((SV*)av));  \
        (void)sv_bless(arg, stash); \
    }

#define InputObject(arg, type)  \
    (type)SvIV(*av_fetch((AV*)SvRV(arg), 0, FALSE))

#define _getClosedSV() \
    *av_fetch((AV*)SvRV(ST(0)), 1, FALSE)

#define setClosed()  \
    SvIV_set(_getClosedSV(), TRUE)

#define isClosed() \
    (bool)SvIV(_getClosedSV())


static void
ckObjectPointer(SV * arg, char* func, char* var, char * ntype)
{
	dTHX;
    char * tmp = savepv(ntype) ;
    tmp[strlen(ntype)-3] = '\0';

    if( ! sv_isobject(arg))
        croak("%s -- %s is not an object reference", func, var, tmp );
    else if (! sv_derived_from(arg, tmp)) 
        croak("%s -- %s is not an %s object reference", func, var, tmp);
    Safefree(tmp);
}

static void
my_hv_store(HV * hash, char * key, SV * value)
{
	dTHX;
    hv_store(hash, key, strlen(key), value, 0);
}

static void
my_get_caller(int& line_no, string& filename)
{
	dTHX;
    SV * sv = perl_get_sv(CALLERS_LINENO, FALSE) ;
    int i ;

    if (sv && (i = SvIV(sv)) >= 0) {
        line_no = i;
        sv = perl_get_sv(CALLERS_FILENAME, FALSE) ;
	if (sv)
	    filename = SvPV_nolen(sv);
	else
            filename =  "fred";
    }
    else {
        line_no = CopLINE(PL_curcop);
        filename =  CopFILE(PL_curcop);
    }
}


class MyBaseException
{
  public:
    MyBaseException(const char * message)
    {
        save_what(message);
    }	   

    MyBaseException(std::exception & e)
    {
        save_what(e.what());
    }	   

    void save_what(const char * what) 
    {
        message = what;
	my_get_caller(line_no, filename);
	char buff[10] ;
	sprintf(buff, "%d", line_no);
	full_message = message + " in " + filename + ", line " + buff;
    }

    ~MyBaseException()
    {
    }	   

    const char* what() const { return full_message.c_str(); }

    const char * getMessage() { return message.c_str() ; }
    int getLineNo() { return line_no; }
    const char * getFilename() { return filename.c_str() ; } 


  private:	 
    string full_message;
    string message;
    int line_no ;
    string filename ;
};

#ifdef DBXML_VERSION_MAJOR
class MyXmlException : public MyBaseException
{
  public:
	MyXmlException(XmlException & e) 
	    : MyBaseException(e)
	{
	    err = e.getDbErrno(); 
	    ex  = e.getExceptionCode();
        qLine_ = e.getQueryLine();
        qCol_ = e.getQueryColumn();
        qFile_ = e.getQueryFile();
	}

	~MyXmlException() {}
	XmlException::ExceptionCode getExceptionCode() { return ex ; }
	int getDbErrno() { return err; }

	const char *getQueryFile() const { return qFile_; }
	int getQueryLine() const { return qLine_; }
	int getQueryColumn() const { return qCol_; }

    private:
	int err;
	XmlException::ExceptionCode ex;
    const char *qFile_;
	int qLine_, qCol_;

};
#endif

class MyDbException : MyBaseException
{
  public:
	MyDbException(DbException & e) : MyBaseException(e.what())
	{
            err = e.get_errno();
            message = e.what();
	    my_get_caller(line_no, filename);
	    char buff[10] ;
	    sprintf(buff, "%d", line_no);
	    full_message = message + " in " + filename + ", line " + buff;
	}

	~MyDbException() {}

	int get_errno() { return err; }

    private:
	int err;
        string full_message;
        string message;
        int line_no ;
        string filename ;
};



class MyUnknownException : public MyBaseException
{
    public:
        MyUnknownException() : MyBaseException("Unknown Exception")
	{
	}

	~MyUnknownException()
	{
	}

};
#if 0
class LexPointer
{
    public:
    LexPointer(XmlValue * p = NULL) : ptr(p) {}
    void setPtr(XmlValue * p)
    {
        ptr = p ;
    }

    ~LexPointer() { if (ptr) delete ptr ; }

    private:
    XmlValue * ptr ;
};
#endif

template <class C> class LexPointer
{
    public:

        LexPointer(C * p = NULL) : ptr(p) {}

        void setPtr(C * p)
        {
            ptr = p ;
        }

        ~LexPointer() { if (ptr) delete ptr ; }

    private:
        C * ptr ;
};
