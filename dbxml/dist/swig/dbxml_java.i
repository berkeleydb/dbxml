/* -*-java-*- */
%{
#include "../dbxml/Value.hpp"
#include "../dbxml/RawNodeValue.hpp"
#include "../dbxml/Results.hpp"
#include "../dbxml/Document.hpp"
#include "../dbxml/ScopedDbt.hpp"

using namespace DbXml;

jthrowable createException(JNIEnv *jenv, const std::exception *se);
XmlDocument createCPPXmlDocument(JNIEnv *jenv, jobject jdoc, bool setData = true);
jobject createJavaXmlDocument(JNIEnv *jenv, XmlDocument &cdoc);
%}

%include "various.i"
%include "arrays_java.i";

    %module(directors = "1") dbxml_java

 // Enable the "director" feature for allowing Java
 // implementations of virtual behavior for 3 classes,
 // XmlInputStream, XmlResolver and XmlExternalFunction.
 // NOTE: there are issues getting it to work properly
 // with XmlInputStream, because of char * -> byte[]
 // mappings.  This will need to be fixed later.  For now,
 // it's not possible to implement XmlInputStream subclasses in Java
    %feature("director") XmlResolver;
    %feature("director") XmlCompression;

%{
    class JavaException {};

    class XmlInputStream_ReadBytes_InOutByteArray {
	public:
	    XmlInputStream_ReadBytes_InOutByteArray(JNIEnv *jenv, jbyteArray &jtoFill, char *toFill, unsigned int &result)
	    : jenv_(jenv), jtoFill_(jtoFill), toFill_(toFill), result_(result)
	{
	}
	~XmlInputStream_ReadBytes_InOutByteArray() {
	    if(jenv_->ExceptionOccurred()) {
		// This is caught in the exception handling routines
		throw JavaException();
	    } else {
		jenv_->GetByteArrayRegion(jtoFill_, (jsize)0, (jsize)result_, (jbyte*)toFill_);
	    }
	}
	private:
	    JNIEnv *jenv_;
	jbyteArray &jtoFill_;
	char *toFill_;
	unsigned int &result_;
    };
    %}

// this turns the char * into byte[] for Java
%apply signed char *INOUT {char *toFill};
%typemap(freearg) char *toFill ""
    %typemap(directorin,descriptor="[B") char *toFill %{
    $input = jenv->NewByteArray((jsize)maxToRead);
    XmlInputStream_ReadBytes_InOutByteArray arg_guard$argnum(jenv, $input, $1, result);
    %}

%feature("director") XmlInputStream;
%feature("director") XmlExternalFunction;
%feature("director") XmlDebugListener;


%include "enumtypeunsafe.swg"
    %javaconst(1);

// These two typemaps for string are directly copied from the std_string.i typemap
// in the java directory that comes with SWIG.  It is modified
// to map a null input into an empty string, rather than throwing
// an exception
%typemap(in) std::string
    %{ 
    const char *$1_pstr = "";
    if($input) {
	$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
    }
    if (!$1_pstr) return $null;
    $1 =  std::string($1_pstr);
    if ($input) {
	jenv->ReleaseStringUTFChars($input, $1_pstr);
    }
    %}

%typemap(in) const std::string &
    %{ 
    const char *$1_pstr = "";
    if($input) {
	$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
    }
    if (!$1_pstr) return $null;
    std::string $1_str($1_pstr);
    $1 = &$1_str;
    if ($input) {
	jenv->ReleaseStringUTFChars($input, $1_pstr);
    }
    %}

//
// Pile of typemaps for unsigned char to be equivalent
// to signed char (Java String).  Not all may be required, but
// experimentation is necessary to eliminate any.
// NOTE: I tried using just these two, but they resulted in
// no compilation due to missing some explicit casts to JNI calls
//  %apply char {unsigned char};
//  %apply char * {unsigned char*};
//
%typemap(jni) unsigned char, const unsigned char & "jchar"
     %typemap(jtype) unsigned char, const unsigned char & "char"
     %typemap(jstype) unsigned char, const unsigned char & "char"
     %typemap(jni) unsigned char *, unsigned char[ANY], unsigned char[]    "jstring"
     %typemap(jtype) unsigned char *, unsigned char[ANY], unsigned char[]  "String"
     %typemap(jstype) unsigned char *, unsigned char[ANY], unsigned char[] "String"
     %typemap(javain) unsigned char, const unsigned char & "$javainput"
     %typemap(javain) unsigned char *, unsigned char[ANY], unsigned char[] "$javainput"
     %typemap(javaout) unsigned char, const unsigned char & { return $jnicall; }
%typemap(javaout) unsigned char *, unsigned char[ANY], unsigned char[] {
    return $jnicall;
 }

%typemap(in, noblock=1) unsigned char * {
    $1 = 0;
    if ($input) {
	$1 = ($1_ltype)JCALL2(GetStringUTFChars, jenv, $input, 0);
	if (!$1) return $null;
    }
 }
%typemap(freearg, noblock=1) unsigned char * {
    if ($1) {
	JCALL2(ReleaseStringUTFChars, jenv, $input, (const char*)$1);
    }
 }
%typemap(in, noblock=1) const unsigned char * {
    $1 = 0;
    if ($input) {
	$1 = ($1_ltype)JCALL2(GetStringUTFChars, jenv, $input, 0);
	if (!$1) return $null;
    }
 }

%typemap(out, noblock=1) const unsigned char * { if ($1) $result = JCALL1(NewStringUTF, jenv, (const char *) $1); }

%typemap(out, noblock=1) unsigned char * { if ($1) $result = JCALL1(NewStringUTF, jenv, (char *) $1); }

// end pile of typemaps for unsigned char

%exception {
    jthrowable t = NULL;
    try {
		$action
	}
    catch (std::exception &se) {
		$cleanup
		t = createException(jenv, &se);
		jenv->Throw(t);
		return $null;
    }
    catch(JavaException & /* je */) {
		$cleanup
		// This means there's already an exception waiting in the JVM
		return $null;
    }
    catch (...) {
		$cleanup
		t = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
			XmlException::INTERNAL_ERROR,
			jenv->NewStringUTF("Uncaught exception from C++ API"), 0, 0, 0, 0);
		jenv->Throw(t);
		return $null;
    }
}

%define JAVA_XMLEXCEPTION(_fname)
%javaexception("XmlException") _fname {
    jthrowable t = NULL;
    try {
        $action
    }
    catch (std::exception &se) {
        t = createException(jenv, &se);
        jenv->Throw(t);
        return $null;
    }
    catch(JavaException & /* je */) {
        // This means there's already an exception waiting in the JVM
        return $null;
    }
    catch (...) {
        t = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
			XmlException::INTERNAL_ERROR,
			jenv->NewStringUTF("Uncaught exception from C++ API"), 0, 0, 0, 0);
        jenv->Throw(t);
        return $null;
    }
}
%enddef

// Map a C++ XmlException onto a Java XmlException for XmlDebugListener.error()
%typemap(jstype) XmlException& "XmlException"
%typemap(jtype) XmlException& "XmlException"
%typemap(jni) XmlException& "jobject"
%typemap(javain) XmlException& %{ $javainput %}
%typemap(javadirectorin) XmlException& "$1"

%typemap(in) XmlException& %{
	/* Create a fake XmlException - it won't be needed */
	XmlException ex$argnum(XmlException::INTERNAL_ERROR, "Pure virtual method called");
	$1 = &ex$argnum;
%}

%typemap(directorin,descriptor="Lcom/sleepycat/dbxml/XmlException;") XmlException&
%{
	$input = createException(jenv, &$1);
%}




%typemap(javaimports) XmlTransaction %{
    import com.sleepycat.db.Transaction;
    import com.sleepycat.db.DatabaseException;
%}

%typemap(javaimports) XmlManager %{
    import com.sleepycat.db.Environment;
    import com.sleepycat.db.Transaction;
    import com.sleepycat.db.TransactionConfig;
    import com.sleepycat.db.VerifyConfig;
    import com.sleepycat.db.DatabaseException;
    import com.sleepycat.db.internal.DbEnv;
    import com.sleepycat.db.internal.DbConstants;
    import com.sleepycat.db.XmlHelper;
    import java.util.LinkedList;
%}

%typemap(javaimports) XmlResults %{
    import java.util.*;
    %}

%pragma(java) jniclassimports=%{
     import com.sleepycat.db.DatabaseException;
     %}

%pragma(java) jniclasscode=%{
     static {
	 // An alternate library name can be specified via a property.
	 //
	 String override;

	 if ((override = System.getProperty("sleepycat.dbxml.libfile")) != null) {
	     System.load(override);
	 }
	 else if ((override = System.getProperty("sleepycat.dbxml.libname")) != null) {
	     System.loadLibrary(override);
	 }
	 else {
	     String os = System.getProperty("os.name");
	     if (os != null && os.startsWith("Windows")) {
		 // library name is "libdbxml_javaXX.dll" (for example) on Windows
		 String libname = "libdbxml_java" +
		     XmlConstants.DBXML_VERSION_MAJOR + XmlConstants.DBXML_VERSION_MINOR;

		 // first try to load the release version. If that fails try debug
		 try {
		     System.loadLibrary(libname);
		 } catch (UnsatisfiedLinkError e) {
		     try {
			 libname += "d";
			 System.loadLibrary(libname);
		     } catch (UnsatisfiedLinkError e2) {
			 throw e;
		     }
		 }
	     }
	     else {
		 // library name is "libdbxml_java-Major.Minor.so" (for example) on UNIX
		 // Note: "dbxml_java" isn't good enough;
		 // some Unixes require us to use the explicit SONAME.
		 System.loadLibrary("dbxml_java-" +
				    XmlConstants.DBXML_VERSION_MAJOR + "." +
				    XmlConstants.DBXML_VERSION_MINOR);
	     }
	 }

	 initialize();
     }

     static native final void initialize();
     public final static native long new_XmlResults(long jarg1, XmlResults jarg1_);
     public final static native void materializeMetaData(XmlDocument document);
     public final static native void interruptQuery(long queryContext);
     public static final native byte[] get_data(long cptr);
     public static final native byte[] getContent(XmlDocument document);
     public static final native void set_data(long cptr, byte[] bytes);
     %}

%typemap(check) SWIGTYPE *self %{
    if (!$input) {
	$cleanup
	jenv->Throw((jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
						XmlException::INVALID_VALUE,
						jenv->NewStringUTF("null object - call after object destroyed?"),
						0, 0, 0, 0));					
	return $null;
    }
    %}

%typemap(check) SWIGTYPE *jcls %{
    UNUSED($input);	/* just in case */
    %}

%typemap(javafinalize) SWIGTYPE ""  //turn off finalizers
    
%extend XmlManager{
	XmlContainer createContainerInternal(const std::string &name, XmlContainerConfig &config)
	{
		return self->createContainer(name, config);
	}
	XmlContainer createContainerInternal(XmlTransaction &txn, const std::string &name, XmlContainerConfig &config)
	{
		return self->createContainer(txn, name, config);
	}
	XmlContainer openContainerInternal(const std::string &name, XmlContainerConfig &config)
	{
		return self->openContainer(name, config);
	}
	XmlContainer openContainerInternal(XmlTransaction &txn, const std::string &name, XmlContainerConfig &config)
	{
		return self->openContainer(txn, name, config);
	}
    XmlDocument createDocumentInternal() {
		return self->createDocument();
    }
    void upgradeContainer(const std::string &name)
    {
		XmlUpdateContext uc = self->createUpdateContext();
		self->upgradeContainer(name, uc);
    }
    void compactContainer(const std::string &name)
    {
		XmlUpdateContext uc = self->createUpdateContext();
		self->compactContainer(name, uc);
    }
	void compactContainer(XmlTransaction &txn, const std::string &name)
	{
		XmlUpdateContext uc = self->createUpdateContext();
		self->compactContainer(txn, name, uc);
	}
	void truncateContainer(const std::string &name)
	{
		XmlUpdateContext uc = self->createUpdateContext();
		self->truncateContainer(name, uc);
	}
	void truncateContainer(XmlTransaction &txn, const std::string &name)
	{
		XmlUpdateContext uc = self->createUpdateContext();
		self->truncateContainer(txn, name, uc);
	}
	void reindexContainer(const std::string &name, XmlContainerConfig flags)
	{
		XmlUpdateContext uc = self->createUpdateContext();
		flags.setThreaded(false);
		self->reindexContainer(name, uc, flags);
	}
	void reindexContainer(XmlTransaction &txn, const std::string &name,
			      XmlContainerConfig flags)
	{
		XmlUpdateContext uc = self->createUpdateContext();
		flags.setThreaded(false);
		self->reindexContainer(txn, name, uc, flags);
	}
	void loadContainer(const std::string &name, const char *filename) {
		XmlUpdateContext uc = self->createUpdateContext();
		std::ifstream in(filename);
		unsigned long lineno = 0;
		self->loadContainer(name, &in, &lineno, uc);
		in.close();
	}
	
	void registerCompressionInternal(const char *name, XmlCompression &compression)
	{ 
		self->registerCompression(name, compression);
	}
	
	void registerResolverInternal(XmlResolver *resolver)
	{ 
		self->registerResolver(*resolver);
	}
 }

/*
 * protect XmlManager methods that aren't publicly supported
 */
%javamethodmodifiers XmlManager::createTransaction(DB_TXN*) "protected";

%javamethodmodifiers XmlManager::query(const std::string &query, 
 				       XmlQueryContext &context,
 				       u_int32_t flags) "protected";
 
%javamethodmodifiers XmlManager::query(XmlTransaction &txn,
 				       const std::string &query, 
 				       XmlQueryContext &context,
 				       u_int32_t flags) "protected";
 				       
%javamethodmodifiers XmlManager::createDocumentInternal() "protected";
						
%javamethodmodifiers XmlManager::verifyContainer(const std::string &name, 
						 const char *filename,
						 u_int32_t flags) "protected";
%javamethodmodifiers XmlManager::createIndexLookupInternal(XmlContainer &cont,
							   const std::string &uri, 
							   const std::string &name,
							   const std::string &index,
							   const XmlValue &value = XmlValue(),
							   enum XmlIndexLookup::Operation op = XmlIndexLookup::NONE) "protected";
%javamethodmodifiers XmlManager::createContainerInternal(const std::string &name, 
								XmlContainerConfig &config) "protected";
%javamethodmodifiers XmlManager::createContainerInternal(XmlTransaction &txn, 
								const std::string &name, 
								XmlContainerConfig &config) "protected";
%javamethodmodifiers XmlManager::openContainerInternal(const std::string &name, 
								XmlContainerConfig &config) "protected";
%javamethodmodifiers XmlManager::openContainerInternal(XmlTransaction &txn, 
								const std::string &name, 
								XmlContainerConfig &config) "protected";	
%javamethodmodifiers XmlManager::registerResolverInternal(XmlResolver *resolver) "protected";
%javamethodmodifiers XmlManager::registerCompressionInternal(
								const char *name,
								XmlCompression &compression) "protected";					  


/*
 * The default typemap for delete makes it synchronized... change that
 */
%typemap(javadestruct, methodname="delete", methodmodifiers="public") SWIGTYPE {
    if(swigCPtr != 0 && swigCMemOwn) {
	swigCMemOwn = false;
	$jnicall;
    }
    swigCPtr = 0;
 }

/*
 * Pieces of XmlManager and XmlTransaction are implemented
 * directly in Java, for better integration with DB.
 * Also, some global constants are moved to the XmlManager class in Java
 */

/*
 * Re-map delete() for XmlManager to close the Environment if necessary,
 * using closeInternal() (protected).
 * The basic code here comes from java.swg
 */
%typemap(javadestruct, methodname="delete", methodmodifiers="public") XmlManager {
    closeInternal();
    if(swigCPtr != 0 && swigCMemOwn) {
	swigCMemOwn = false;
	$jnicall;
    }
    swigCPtr = 0;
 }
%typemap(javacode) XmlManager %{

    private Environment dbenv;
    private XmlContainerConfig defaultConfig = new XmlContainerConfig();
    private boolean threaded = true; // default on if no Environment
    private boolean adopted = false;
    private XmlManagerConfig config = null;
	private LinkedList<XmlCompression> compressionStore;
    private LinkedList<XmlResolver> resolverStore;

    public XmlManager(final Environment dbenv,
		      XmlManagerConfig config)
	throws XmlException {
	this(dbxml_javaJNI.new_XmlManager(XmlHelper.getDbEnv(dbenv),
					  ((config == null ) ? 0 :
					   config.makeFlags())),true);
	this.dbenv = dbenv;
	this.config = config;
	if ((config != null) && (config.getAdoptEnvironment()))
	    this.adopted = true;
	try {
	    threaded =
		((XmlHelper.getDbEnv(dbenv).get_open_flags() &
		  DbConstants.DB_THREAD) != 0);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
    }

    public XmlManager(XmlManagerConfig config)
	throws XmlException, java.io.FileNotFoundException {
	DbEnv newEnv = null;
	try {
	    newEnv = new DbEnv(0);
	    newEnv.set_cachesize(64 * 1024 * 1024, 1); // 64MB cache
	    newEnv.set_errpfx("BDB XML");
	    newEnv.open(null, DbConstants.DB_PRIVATE|
			DbConstants.DB_CREATE|DbConstants.DB_INIT_MPOOL|
			DbConstants.DB_THREAD, 0);
	    this.dbenv = XmlHelper.makeNewEnvironment(newEnv);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	this.adopted = true;
	this.config = config;
	// do what the SWIG-generated ctor does, in order to
	// create a C++ XmlManager object.  Add ADOPT flag,
	// since the DbEnv was internally constructed.
	int flags = (config == null) ? 0 : config.makeFlags();
	swigCPtr = dbxml_javaJNI.new_XmlManager(newEnv, flags);
	swigCMemOwn = true;
    }

    public XmlManager()
	throws XmlException, java.io.FileNotFoundException {
	this(null);
    }

    public Environment getEnvironment() {
	return dbenv;
    }

    public XmlManagerConfig getManagerConfig() {
	return config;
    }

    public synchronized void setDefaultContainerConfig(XmlContainerConfig config) {
	this.defaultConfig = config;
    }

    public XmlContainerConfig getDefaultContainerConfig() {
	return defaultConfig;
    }
    
    public XmlUpdateContext createUpdateContext() { 
	return new XmlUpdateContext(this);
    }
    
    public XmlQueryContext createQueryContext() throws XmlException{
	XmlQueryContext xqc = 
	    new XmlQueryContext(XmlQueryContext.LiveValues, XmlQueryContext.Eager);
	xqc.mgr = this;
	return xqc;
    }
    
    public XmlQueryContext createQueryContext(int rt, int et) throws XmlException{
	XmlQueryContext xqc = new XmlQueryContext(rt, et);
	xqc.mgr = this;
	return xqc;
    }
    
    public XmlQueryContext createQueryContext(int rt) throws XmlException{
	XmlQueryContext xqc = new XmlQueryContext(rt, XmlQueryContext.Eager);
	xqc.mgr = this;
	return xqc;
    }

    public XmlInputStream createInputStream(java.io.InputStream is) {
	return new NativeXmlInputStream(is);
    }

    public XmlContainer createContainer(String name)
	throws XmlException {
	return createContainer(name, defaultConfig);
    }

    public XmlContainer createContainer(XmlTransaction txn, String name)
	throws XmlException {
	return createContainer(txn, name, defaultConfig);
    }

    public XmlContainer openContainer(String name)
	throws XmlException {
	return openContainer(name, defaultConfig);
    }

    public XmlContainer openContainer(XmlTransaction txn, String name)
	throws XmlException {
	return openContainer(txn, name, defaultConfig);
    }
    
    private void setContainerConfig(XmlContainerConfig config, XmlContainer cont)
    {
	cont.setManager(this);
	int[] data = new int[8];
	String compName = cont.getContainerConfigData(data);
	config = new XmlContainerConfig(data);
	config.setCompression(compName);
	cont.setContainerConfig(config);
    }

    public XmlContainer createContainer(String name,
					XmlContainerConfig config)
	throws XmlException {
	XmlContainer cont = createContainerInternal(name, config);
	setContainerConfig(config, cont);
	return cont;
    }

    public XmlContainer createContainer(XmlTransaction txn,
					String name,
					XmlContainerConfig config)
	throws XmlException {
	XmlContainer cont = createContainerInternal(txn, name, config);
	setContainerConfig(config, cont);
	return cont;
    }

    public XmlContainer openContainer(String name,
				      XmlContainerConfig config)
	throws XmlException {
	XmlContainer cont = openContainerInternal(name, config);
	setContainerConfig(config, cont);
	return cont;
    }

    public XmlContainer openContainer(XmlTransaction txn,
				      String name,
				      XmlContainerConfig config)
	throws XmlException {
	XmlContainer cont = openContainerInternal(txn, name, config);
	setContainerConfig(config, cont);
	return cont;
    }
    
    public void setDefaultCompression(String name)
    {
	synchronized (defaultConfig) {
	    defaultConfig.setCompression(name);
	}
    }
    public String getDefaultCompression()
    {
	return defaultConfig.getCompression();
    }
	
    public void setDefaultContainerType(int type) 
    {
	synchronized (defaultConfig) {
	    defaultConfig.setContainerType(type);
	}
    }

    public int getDefaultContainerType() 
    {
	return defaultConfig.getContainerType();
    }
	
    public void setDefaultPageSize(int pageSize) throws XmlException {
	synchronized (defaultConfig) {
	    defaultConfig.setPageSize(pageSize);
	}
    }

    public int getDefaultPageSize() throws XmlException {
	return defaultConfig.getPageSize();
    }

    public void setDefaultSequenceIncrement(int incr) throws XmlException {
	synchronized (defaultConfig) {
	    defaultConfig.setSequenceIncrement(incr);
	}
    }

    public int getDefaultSequenceIncrement() throws XmlException {
	return defaultConfig.getSequenceIncrement();
    }

    public XmlTransaction createTransaction(com.sleepycat.db.Transaction toUse)
	throws XmlException {
	XmlTransaction txn = createTransaction(XmlHelper.getDbTxn(toUse));
	txn.setTransaction(toUse);
	return txn;
    }

    public XmlTransaction createTransaction(final Transaction parent,
					    TransactionConfig config)
	throws XmlException {
	Transaction newTxn = null;
	try {
	    newTxn = dbenv.beginTransaction(parent, config);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	return createTransaction(newTxn);
    }

    public XmlTransaction createTransaction()
	throws XmlException {
	return createTransaction(null, null);
    }
    
    public XmlIndexLookup createIndexLookup(XmlContainer cont, 
					    String uri, String name, String index, 
					    XmlValue value, int op) 
	throws XmlException {
	XmlIndexLookup lookup = createIndexLookupInternal(cont, uri, name, index, value, op);
	lookup.container = cont;
	lookup.lowBoundValue = value;
	return lookup;
    }
	
    public XmlIndexLookup createIndexLookup(XmlContainer cont, 
					    String uri, String name, String index, 
					    XmlValue value) 
	throws XmlException {
	XmlIndexLookup lookup = createIndexLookupInternal(cont, uri, name, index, value);
	lookup.container = cont;
	lookup.lowBoundValue = value;
	return lookup;
    }
	
    public XmlIndexLookup createIndexLookup(XmlContainer cont, 
					    String uri, String name, String index) 
	throws XmlException {
	XmlIndexLookup lookup = createIndexLookupInternal(cont, uri, name, index);
	lookup.container = cont;
	return lookup;
    }
    
    public XmlDocument createDocument()
	throws XmlException {
	XmlDocument doc = createDocumentInternal();
	doc.setManager(this);
	return doc;
    }

    public XmlResults query(String query,
			    XmlQueryContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return query(query, context, flags);
    }

    public XmlResults query(XmlTransaction txn,
			    String query,
			    XmlQueryContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return query(txn, query, context, flags);
    }
    
    public void reindexContainer(String name,
				 XmlUpdateContext context,
				 XmlContainerConfig config)
	throws XmlException {
	reindexContainer(name, config);
    }

    public void reindexContainer(XmlTransaction txn,
				 String name,
				 XmlUpdateContext context,
				 XmlContainerConfig config)
	throws XmlException {
	reindexContainer(txn, name, config);
    }
    
    public void loadContainer(String name, String filename, XmlUpdateContext uc) throws XmlException
    {
	loadContainer(name, filename);
    }

    public void verifyContainer(String name,
				String filename,
				VerifyConfig config)
	throws XmlException {
	int flags = 0;
	if (config.getAggressive())
	    flags |= DbConstants.DB_AGGRESSIVE;
	if (config.getSalvage())
	    flags |= DbConstants.DB_SALVAGE;
	if (config.getPrintable())
	    flags |= DbConstants.DB_PRINTABLE;
	if (config.getOrderCheckOnly())
	    flags |= DbConstants.DB_ORDERCHKONLY;
	if (config.getNoOrderCheck())
	    flags |= DbConstants.DB_NOORDERCHK;
	verifyContainer(name, filename, flags);
    }
    
    public void upgradeContainer(String name, XmlUpdateContext uc) throws XmlException
    {
	upgradeContainer(name);
    }
    public void compactContainer(String name, XmlUpdateContext uc) throws XmlException
    {
	compactContainer(name);
    }
    public void compactContainer(XmlTransaction txn, String name, XmlUpdateContext uc) throws XmlException
    {
	compactContainer(txn, name);
    }
    public void truncateContainer(String name, XmlUpdateContext uc) throws XmlException
    {
	truncateContainer(name);
    }
    public void truncateContainer(XmlTransaction txn, String name, XmlUpdateContext uc) throws XmlException
    {
	truncateContainer(txn, name);
    }
	
    private synchronized void setResolver(XmlResolver resolver)
				  {
				      if (resolverStore == null) resolverStore = new LinkedList<XmlResolver>();
				      resolverStore.add(resolver);  //prevents premature garbage collection
				  }
	
	public void registerResolver(XmlResolver resolver) throws XmlException
    {
    	setResolver(resolver);
    	registerResolverInternal(resolver);
    }
	
    private synchronized void setCompression(XmlCompression compression)
				  {
				      if (compressionStore == null) compressionStore = new LinkedList<XmlCompression>();
				      compressionStore.add(compression);  //prevents premature garbage collection
				  }
    
    public void registerCompression(String name, XmlCompression compression) throws XmlException
    {
    	setCompression(compression);
    	registerCompressionInternal(name, compression);
    }

    protected void closeInternal() {
	if (adopted) {
	    try {
		dbenv.close();
	    } catch (DatabaseException de) {}
	}
	adopted = false;
	this.dbenv = null;
	this.config = null;
    }

    public void close() {
	delete();
    }

    public final static int LEVEL_NONE = dbxml_java.LEVEL_NONE;
    public final static int LEVEL_DEBUG = dbxml_java.LEVEL_DEBUG;
    public final static int LEVEL_INFO = dbxml_java.LEVEL_INFO;
    public final static int LEVEL_WARNING = dbxml_java.LEVEL_WARNING;
    public final static int LEVEL_ERROR = dbxml_java.LEVEL_ERROR;
    public final static int LEVEL_ALL = dbxml_java.LEVEL_ALL;
    public final static int CATEGORY_NONE = dbxml_java.CATEGORY_NONE;
    public final static int CATEGORY_INDEXER = dbxml_java.CATEGORY_INDEXER;
    public final static int CATEGORY_QUERY = dbxml_java.CATEGORY_QUERY;
    public final static int CATEGORY_OPTIMIZER = dbxml_java.CATEGORY_OPTIMIZER;
    public final static int CATEGORY_DICTIONARY = dbxml_java.CATEGORY_DICTIONARY;
    public final static int CATEGORY_CONTAINER = dbxml_java.CATEGORY_CONTAINER;
    public final static int CATEGORY_NODESTORE = dbxml_java.CATEGORY_NODESTORE;
    public final static int CATEGORY_MANAGER = dbxml_java.CATEGORY_MANAGER;
    public final static int CATEGORY_ALL = dbxml_java.CATEGORY_ALL;
    public final static String metaDataNamespace_uri = dbxml_javaConstants.metaDataNamespace_uri;
    public final static String metaDataNamespace_prefix = dbxml_javaConstants.metaDataNamespace_prefix;
    public final static String metaDataName_name = dbxml_javaConstants.metaDataName_name;
    public final static String metaDataName_root = dbxml_javaConstants.metaDataName_root;
    %}

/*
 * XmlTransaction is mostly Java
 */
%typemap(javacode) XmlTransaction %{
    Transaction txn;

    public void setTransaction(final com.sleepycat.db.Transaction txn) {
	this.txn = txn;
    }

    public Transaction getTransaction() {
	return this.txn;
    }

    public void abort()
	throws XmlException {
	try {
	    txn.abort();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commit()
	throws XmlException {
	try {
	    txn.commit();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commitSync()
	throws XmlException {
	try {
	    txn.commitSync();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commitNoSync()
	throws XmlException {
	try {
	    txn.commitNoSync();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }
    %}

/*
 * protect XmlContainer methods that aren't publicly supported
 */
JAVA_XMLEXCEPTION(putDocument)
%javamethodmodifiers XmlContainer::putDocument(XmlDocument &,
						   u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       XmlDocument &,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name,
					       XmlInputStream *input,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name,
					       XmlInputStream *input,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name, 
					       XmlEventReader &reader,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name, 
					       XmlEventReader &reader,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name,
					       const std::string &contents,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name,
					       const std::string &contents,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocumentAsEventWriter(XmlTransaction &,
							    XmlDocument &document,
							    u_int32_t flags,
								u_int64_t *ids) "protected";

%javamethodmodifiers XmlContainer::putDocumentAsEventWriter(XmlDocument &document,
							    u_int32_t flags,
								u_int64_t *ids) "protected";

%javamethodmodifiers XmlContainer::getAllDocuments(u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getAllDocuments(XmlTransaction &,
						   u_int32_t flags) "protected";						   
%javamethodmodifiers XmlContainer::getNode(XmlTransaction&, 
					   const std::string&, u_int32_t) "protected";						
%javamethodmodifiers XmlContainer::getIndexSpecification(XmlTransaction &,
							 u_int32_t flags) const "protected";							 

%javamethodmodifiers XmlContainer::lookupIndex(
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &parent_uri,
					       const std::string &parent_name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(XmlTransaction &,
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(XmlTransaction &,
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &parent_uri,
					       const std::string &parent_name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";
%javamethodmodifiers XmlContainer::lookupIndex(XmlTransaction &, XmlQueryContext &,
					       const std::string &, const std::string &,
					       const std::string &, const XmlValue &,
					       u_int32_t)"protected";

%javamethodmodifiers XmlContainer::getDocumentInternal(const std::string &, u_int32_t) "protected";

%javamethodmodifiers XmlContainer::getDocumentInternal(XmlTransaction &, const std::string &, u_int32_t) "protected";

%javamethodmodifiers XmlContainer::putDocumentInternal(XmlDocument &,u_int32_t) "protected";
%javamethodmodifiers XmlContainer::putDocumentInternal(XmlTransaction &, XmlDocument &,u_int32_t) "protected";
%javamethodmodifiers XmlContainer::deleteDocumentInternal(XmlDocument &) "protected";
%javamethodmodifiers XmlContainer::deleteDocumentInternal(XmlTransaction &, XmlDocument &) "protected";
%javamethodmodifiers XmlContainer::updateDocumentInternal(XmlDocument &) "protected";
%javamethodmodifiers XmlContainer::updateDocumentInternal(XmlTransaction &, XmlDocument &) "protected";	
%javamethodmodifiers XmlContainer::getContainerConfigData(int []) "protected";

%extend XmlContainer{
	std::string getContainerConfigData(int data[])
	{
		XmlContainerConfig config = self->getContainerConfig();
		data[0] = config.getMode();
		data[1] = config.getContainerType();
		data[2] = (*config).getXmlFlags();
		data[3] = (*config).getDbOpenFlags();
		data[4] = config.getPageSize();
		data[5] = config.getSequenceIncrement();
		data[6] = (*config).getDbSetFlags();
		data[7] = (*config).getSeqFlags();
		std::string compName(config.getCompressionName());
		return compName;
	}

	void setAutoIndexing(bool value)
	{
	    XmlUpdateContext uc = self->getManager().createUpdateContext();
	    self->setAutoIndexing(value, uc);
	}

	void setAutoIndexing(XmlTransaction &txn, bool value)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->setAutoIndexing(txn, value, uc);
	}

	void setIndexSpecification(const XmlIndexSpecification &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->setIndexSpecification(index, uc);
	}
	
	void addIndex(const std::string &uri, const std::string &name,
		      const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addIndex(uri, name, index, uc);
	}

	void addIndex(const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type indexType,
		      enum XmlValue::Type syntaxType)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addIndex(uri, name, indexType, syntaxType, uc);
	}
	void deleteIndex(const std::string &uri, const std::string &name,
			 const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->deleteIndex(uri, name, index, uc);
	}
	void replaceIndex(const std::string &uri, const std::string &name,
			  const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->replaceIndex(uri, name, index, uc);
	}

	void addDefaultIndex(const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addDefaultIndex(index, uc);
	}
	void deleteDefaultIndex(const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->deleteDefaultIndex(index, uc);
	}
	void replaceDefaultIndex(const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->replaceDefaultIndex(index, uc);
	}


	void setIndexSpecification(XmlTransaction &txn,
				   const XmlIndexSpecification &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->setIndexSpecification(txn, index, uc);
	}

	void addIndex(XmlTransaction &txn, const std::string &uri,
		      const std::string &name, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addIndex(txn, uri, name, index, uc);
	}

	void addIndex(XmlTransaction &txn,
		      const std::string &uri, const std::string &name,
		      enum XmlIndexSpecification::Type indexType,
		      enum XmlValue::Type syntaxType)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addIndex(txn, uri, name, indexType, syntaxType, uc);
	}

	void deleteIndex(XmlTransaction &txn, const std::string &uri,
			 const std::string &name, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->replaceIndex(txn, uri, name, index, uc);
	}

	void replaceIndex(XmlTransaction &txn, const std::string &uri,
			  const std::string &name, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->replaceIndex(txn, uri, name, index, uc);
	}

	void addDefaultIndex(XmlTransaction &txn, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->addDefaultIndex(txn, index, uc);
	}

	void deleteDefaultIndex(XmlTransaction &txn, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->deleteDefaultIndex(txn, index, uc);
	}

	void replaceDefaultIndex(XmlTransaction &txn, const std::string &index)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		self->replaceDefaultIndex(txn, index, uc);
	}

	std::string putDocument(const std::string &name, const std::string &contents,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(name, contents, uc, flags);
	}

    std::string putDocument(const std::string &name, XmlEventReader &reader,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(name, reader, uc, flags);
	}

	std::string putDocument(const std::string &name,
				XmlInputStream *input,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(name, input, uc, flags);
	}

	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				XmlInputStream *input,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(txn, name, input, uc, flags);
	}

	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				const std::string &contents,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(txn, name, contents, uc, flags);
	}

	std::string putDocument(XmlTransaction &txn,
				const std::string &name,
				XmlEventReader &reader,
				u_int32_t flags)
	{
		XmlUpdateContext uc = self->getManager().createUpdateContext();
		return self->putDocument(txn, name, reader, uc, flags);
	}
				

    XmlDocument getDocumentInternal(const std::string &name, u_int32_t flags)
	{
		return self->getDocument(name, flags);
	}

    XmlDocument getDocumentInternal(XmlTransaction &txn, const std::string &name, u_int32_t flags)
	{
		return self->getDocument(txn, name, flags);
	}

    XmlDocument &putDocumentInternal(XmlDocument &document,
				     u_int32_t flags)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->putDocument(document, context, flags);
	    return document;
	}
    XmlDocument &putDocumentInternal(XmlTransaction &txn, XmlDocument &document,
				     u_int32_t flags)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->putDocument(txn, document, context, flags);
	    return document;
	}
			 
    XmlEventWriter &putDocumentAsEventWriter(XmlDocument &document,
						 u_int32_t flags, u_int64_t *ids)
    {
	XmlUpdateContext context = self->getManager().createUpdateContext();
	XmlEventWriter &writer = self->putDocumentAsEventWriter(document, context, flags);
	ids[1] = (*(document)).getContainerID();
	ids[0] = *(u_int64_t *)((*(document)).getID().rawPtr());
	return writer;
    }
    XmlEventWriter &putDocumentAsEventWriter(XmlTransaction &txn,
						 XmlDocument &document,
						 u_int32_t flags, u_int64_t *ids)
    {
	XmlUpdateContext context = self->getManager().createUpdateContext();
	XmlEventWriter &writer = self->putDocumentAsEventWriter(txn, document, context, flags);
	ids[1] = (*(document)).getContainerID();
	ids[0] = *(u_int64_t *)((*(document)).getID().rawPtr());
	return writer;
    }
    
    void deleteDocument(const std::string &name)
    {
		XmlUpdateContext uc = self->getManager().createUpdateContext();
	    self->deleteDocument(name, uc);
    }
    void deleteDocument(XmlTransaction &txn, const std::string &name)
	{
		XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->deleteDocument(txn, name, context);
	}
    XmlDocument &deleteDocumentInternal(XmlDocument &document)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->deleteDocument(document, context);
	    return document;
	}
    XmlDocument &deleteDocumentInternal(XmlTransaction &txn, XmlDocument &document)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->deleteDocument(txn, document, context);
	    return document;
	}
	
    XmlDocument &updateDocumentInternal(XmlDocument &document)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->updateDocument(document, context);
	    return document;
	}
    XmlDocument &updateDocumentInternal(XmlTransaction &txn, XmlDocument &document)
	{
	    XmlUpdateContext context = self->getManager().createUpdateContext();
	    self->updateDocument(txn, document, context);
	    return document;
	}

    XmlResults getNode(const std::string &name) {
	XmlValue value = self->getNode(name, 0);
	XmlResults result = self->getManager().createResults();
	result.add(value);
	return result;
    }
    
    XmlResults getNode(XmlTransaction &txn, const std::string &name,
		       u_int32_t flags) {
	XmlValue value = self->getNode(txn, name, flags);
	XmlResults result = new ValueResults(self->getManager(), txn);
	result.add(value);
	return result;
    }
}

%typemap(javacode) XmlContainer %{
    private XmlContainerConfig config;
    private XmlManager manager;

    public XmlContainerConfig getContainerConfig() {
	return config;
    }

    public XmlManager getManager() {
	return manager;
    }

    public void close() {
	delete(); // delete the C++ object, as well
    }
    
    // Start XmlDocument-based putDocument methods
    public void putDocument(XmlDocument document)
	throws XmlException {
	putDocument(null, document, null, null);
    }
    
    public void putDocument(XmlDocument document,
			    XmlUpdateContext context)
	throws XmlException {
	putDocument(null, document, null, null);
    }
    
    public void putDocument(XmlDocument document,
			    XmlDocumentConfig config)
	throws XmlException {
	putDocument(null, document, null, config);
    }

    public void putDocument(XmlDocument document,
			    XmlUpdateContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	putDocument(null, document, null, config);
    }
    
    public void putDocument(XmlTransaction txn, XmlDocument document)
	throws XmlException {
	putDocument(txn, document, null, null);
    }
    
    public void putDocument(XmlTransaction txn, XmlDocument document,
			    XmlUpdateContext context)
	throws XmlException {
	putDocument(txn, document, null, null);
    }
    
    public void putDocument(XmlTransaction txn, XmlDocument document,
			    XmlDocumentConfig config)
	throws XmlException {
	putDocument(txn, document, null, config);
    }

    // all putDocument calls funnel through this one
    public void putDocument(XmlTransaction txn, XmlDocument document,
			    XmlUpdateContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	XmlDocument doc;
	if (txn != null)
	    doc = putDocumentInternal(txn, document, flags);
	else
	    doc = putDocumentInternal(document, flags);
	document.copy(doc);
	document.setManager(manager);
	if (txn != null)
	    document.setTransaction(txn);
    }
    
    // Start XmlInputStream-based putDocument methods
    public String putDocument(String name,
			      XmlInputStream input)
	throws XmlException {
	return putDocument(null, name, input, null, null);
    }
    
    public String putDocument(String name,
			      XmlInputStream input,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(null, name, input, null, null);
    }
    
    public String putDocument(String name,
			      XmlInputStream input,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, input, null, config);
    }

    public String putDocument(String name,
			      XmlInputStream input,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, input, null, config);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlInputStream input)
	throws XmlException {
	return putDocument(txn, name, input, null, null);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlInputStream input,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(txn, name, input, null, null);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlInputStream input,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(txn, name, input, null, config);
    }

    // XmlInputStream putDocument methods funnel through here
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlInputStream input,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	String docName = "";
	try{
	    if (txn != null)
		docName = putDocument(txn, name, input, flags);
	    else
		docName = putDocument(name, input, flags);
	}catch(XmlException e){
	    input.release();
	    throw e;
	}
	input.release();
	return docName;
    }
    
    // Start String-based putDocument methods
    public String putDocument(String name,
			      String content)
	throws XmlException {
	return putDocument(null, name, content, null, null);
    }
    
    public String putDocument(String name,
			      String content,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(null, name, content, null, null);
    }
    
    public String putDocument(String name,
			      String content,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, content, null, config);
    }

    public String putDocument(String name,
			      String content,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, content, null, config);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      String content)
	throws XmlException {
	return putDocument(txn, name, content, null, null);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      String content,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(txn, name, content, null, null);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      String content,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(txn, name, content, null, config);
    }

    // String-based putDocument calls funnel through here
    public String putDocument(XmlTransaction txn,
			      String name,
			      String content,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	if (txn != null)
	    return putDocument(txn, name, content, flags);
	else
	    return putDocument(name, content, flags);
    }

    // Start XmlEventReader-based putDocument methods    
    public String putDocument(String name,
			      XmlEventReader reader)
	throws XmlException {
	return putDocument(null, name, reader, null, null);
    }
    
    public String putDocument(String name,
			      XmlEventReader reader,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, reader, null, config);
    }

    public String putDocument(String name,
			      XmlEventReader reader,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(null, name, reader, context, config);
    }

    public String putDocument(String name,
			      XmlEventReader reader,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(null, name, reader, context, null);
    }
    
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlEventReader reader,
			      XmlDocumentConfig config)
	throws XmlException {
	return putDocument(txn, name, reader, null, config);
    }

    public String putDocument(XmlTransaction txn, 
			      String name,
			      XmlEventReader reader)
	throws XmlException {
	return putDocument(txn, name, reader, null, null);
    }

    public String putDocument(XmlTransaction txn, 
			      String name,
			      XmlEventReader reader,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(txn, name, reader, context, null);
    }

    // all XmlEventReader-based putDocument methods funnel here
    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlEventReader reader,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	String ret = "";
	try{
	    if (txn != null)
		ret = putDocument(txn, name, reader, flags);
	    else
		ret = putDocument(name, reader, flags);
	}catch(XmlException e){
	    reader.release();
	    throw e;
	}
	reader.release(); /* no further access */
	return ret;
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlDocument document)
	throws XmlException {
	return putDocumentAsEventWriter(null, document, null, null);
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlDocument document,
						   XmlDocumentConfig config)
	throws XmlException {
	return putDocumentAsEventWriter(null, document, null, config);
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlDocument document,
						   XmlUpdateContext context)
	throws XmlException {
	return putDocumentAsEventWriter(null, document, null, null);
    }

    public XmlEventWriter putDocumentAsEventWriter(XmlDocument document,
						   XmlUpdateContext context,
						   XmlDocumentConfig config)
	throws XmlException {
	return putDocumentAsEventWriter(null, document, null, config);
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlTransaction txn,
						   XmlDocument document)
	throws XmlException {
	return putDocumentAsEventWriter(txn, document, null, null);
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlTransaction txn,
						   XmlDocument document,
						   XmlUpdateContext context)
	throws XmlException {
	return putDocumentAsEventWriter(txn, document, null, null);
    }
    
    public XmlEventWriter putDocumentAsEventWriter(XmlTransaction txn,
						   XmlDocument document,
						   XmlDocumentConfig config)
	throws XmlException {
	return putDocumentAsEventWriter(txn, document, null, config);
    }

    // All calls to putDocumentAsEventWriter funnel through this one
    public XmlEventWriter putDocumentAsEventWriter(XmlTransaction txn,
						   XmlDocument document,
						   XmlUpdateContext context,
						   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	document.clearContent(); // this call clears any existing content
	long[] ids = new long[2];
	XmlEventWriter writer;
	if (txn != null)
	    writer = putDocumentAsEventWriter(txn, document, flags, ids);
	else
	    writer = putDocumentAsEventWriter(document, flags, ids);
	XmlDocument doc = new XmlDocument(ids[0], (int)ids[1], null);
	document.copy(doc);
	document.setManager(manager);
	if (txn != null)
	    document.setTransaction(txn);;
	return writer;
    }
    
    public void deleteDocument(String name, XmlUpdateContext context)
	throws XmlException {
	deleteDocument(name);
    }
    
    public void deleteDocument(XmlDocument document)
	throws XmlException {
	deleteDocument(document, null);
    }
    
    public void deleteDocument(XmlDocument document, XmlUpdateContext context)
	throws XmlException {
	XmlDocument doc = deleteDocumentInternal(document);
	document.copy(doc);
	document.clearID();
	document.setManager(manager);
    }
    
    public void deleteDocument(XmlTransaction txn, String name, XmlUpdateContext context)
	throws XmlException {
	deleteDocument(txn, name);
    }
    
    public void deleteDocument(XmlTransaction txn, XmlDocument document)
	throws XmlException {
	deleteDocument(txn, document, null);
    }
    
    public void deleteDocument(XmlTransaction txn, XmlDocument document,
			       XmlUpdateContext context)
	throws XmlException {
	XmlDocument doc = deleteDocumentInternal(txn, document);
	document.copy(doc);
	document.clearID();
	document.setManager(manager);
	document.setTransaction(txn);
    }
    
    public void updateDocument(XmlDocument document)
	throws XmlException {
	updateDocument(document, null);
    }
	
    public void updateDocument(XmlDocument document,
			       XmlUpdateContext context)
	throws XmlException {
	XmlDocument doc = updateDocumentInternal(document);
	document.copy(doc);
	document.setManager(manager);
    }
    
    public void updateDocument(XmlTransaction txn, XmlDocument document)
	throws XmlException {
	updateDocument(txn, document, null);
    }
    
    public void updateDocument(XmlTransaction txn, XmlDocument document,
			       XmlUpdateContext context)
	throws XmlException {
	XmlDocument doc = updateDocumentInternal(txn, document);
	document.copy(doc);
	document.setManager(manager);
	document.setTransaction(txn);
    }
    
    public void setIndexSpecification(XmlIndexSpecification index, XmlUpdateContext context) throws XmlException {
	setIndexSpecification(index);
    }

    public void addIndex(String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	addIndex(uri, name, index);
    }

    public void addIndex(String uri, String name, int indexType, int syntaxType, XmlUpdateContext context) throws XmlException {
	addIndex(uri, name, indexType, syntaxType);
    }

    public void deleteIndex(String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	deleteIndex(uri, name, index);
    }

    public void replaceIndex(String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	replaceIndex(uri, name, index);
    }

    public void addDefaultIndex(String index, XmlUpdateContext context) throws XmlException {
	addDefaultIndex(index);
    }

    public void deleteDefaultIndex(String index, XmlUpdateContext context) throws XmlException {
	deleteDefaultIndex(index);
    }

    public void setIndexSpecification(XmlTransaction txn, XmlIndexSpecification index, XmlUpdateContext context) throws XmlException {
	setIndexSpecification(txn, index);
    }

    public void addIndex(XmlTransaction txn, String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	addIndex(txn, uri, name, index);
    }

    public void addIndex(XmlTransaction txn, String uri, String name, int indexType, int syntaxType, XmlUpdateContext context) throws XmlException {
	addIndex(txn, uri, name, indexType, syntaxType);
    }

    public void deleteIndex(XmlTransaction txn, String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	deleteIndex(txn, uri, name, index);
    }

    public void replaceIndex(XmlTransaction txn, String uri, String name, String index, XmlUpdateContext context) throws XmlException {
	replaceIndex(txn, uri, name, index);
    }

    public void addDefaultIndex(XmlTransaction txn, String index, XmlUpdateContext context) throws XmlException {
	addDefaultIndex(txn, index);
    }

    public void deleteDefaultIndex(XmlTransaction txn, String index, XmlUpdateContext context) throws XmlException {
	deleteDefaultIndex(txn, index);
    }
    
    public void replaceDefaultIndex(String index, XmlUpdateContext context) throws XmlException {
	replaceDefaultIndex(index);
    }

    public void replaceDefaultIndex(XmlTransaction txn, String index, XmlUpdateContext context) throws XmlException {
	replaceDefaultIndex(txn, index);
    }

    public XmlDocument getDocument(String name)
	throws XmlException {
	return getDocument(name, null);
    }

    public XmlDocument getDocument(String name,
				   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	XmlDocument doc = getDocumentInternal(name, flags);
	doc.setManager(manager);
	return doc;
    }

    public XmlDocument getDocument(XmlTransaction txn,
				   String name)
	throws XmlException {
	return getDocument(txn, name, null);
    }

    public XmlDocument getDocument(XmlTransaction txn,
				   String name,
				   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	XmlDocument doc = getDocumentInternal(txn, name, flags);
	doc.setManager(manager);
	doc.setTransaction(txn);
	return doc;
    }

    public XmlResults getAllDocuments(XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getAllDocuments(flags);
    }


    public XmlResults getAllDocuments(XmlTransaction txn,
				      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getAllDocuments(txn, flags);
    }
    
    public XmlResults getNode(XmlTransaction txn, String name, 
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getNode(txn, name, flags);
    }
	
    public XmlIndexSpecification getIndexSpecification(XmlTransaction txn,
						       XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getIndexSpecification(txn, flags);
    }

    public XmlResults lookupIndex(XmlQueryContext context,
				  String uri, String name,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(context, uri, name, index, value, flags);
    }

    public XmlResults lookupIndex(XmlTransaction txn,
				  XmlQueryContext context,
				  String uri, String name,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(txn, context, uri, name, index, value, flags);
    }

    public XmlResults lookupIndex(XmlQueryContext context,
				  String uri, String name,
				  String parentUri, String parentName,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(context, uri, name,
			   parentUri, parentName, index, value, flags);
    }

    public XmlResults lookupIndex(XmlTransaction txn,
				  XmlQueryContext context,
				  String uri, String name,
				  String parentUri, String parentName,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(txn, context, uri, name,
			   parentUri, parentName, index, value, flags);
    }

    /* package */
    void setContainerConfig(XmlContainerConfig config) {
	this.config = config;
    }

    /* package */
    void setManager(XmlManager manager) {
	this.manager = manager;
    }

    %}

/*
 * protect XmlQueryExpression methods that aren't publicly supported
 */

%javamethodmodifiers XmlQueryExpression::execute(XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(const XmlValue &, 
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(XmlTransaction &,
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(XmlTransaction &,
						 const XmlValue &, 
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%typemap(javacode) XmlQueryExpression %{
    public XmlResults execute(XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(context, flags);
    }
    public XmlResults execute(XmlValue contextItem,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(contextItem, context, flags);
    }

    public XmlResults execute(XmlTransaction txn,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, context, flags);
    }

    public XmlResults execute(XmlTransaction txn,
			      XmlValue contextItem,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, contextItem, context, flags);
    }
    %}
    
// protect non-exported XmlIndexLookup methods
%javamethodmodifiers XmlIndexLookup::setContainerInternal(XmlContainer &) "protected";
%javamethodmodifiers XmlIndexLookup::execute(XmlQueryContext &,
					     u_int32_t) const "protected";
%javamethodmodifiers XmlIndexLookup::execute(XmlTransaction &,
					     XmlQueryContext &,
					     u_int32_t) const "protected";
%javamethodmodifiers XmlIndexLookup::setLowBound(const XmlValue &,
						 enum XmlIndexLookup::Operation) "protected";
%javamethodmodifiers XmlIndexLookup::setHighBound(const XmlValue &,
						  enum XmlIndexLookup::Operation) "protected";
%javamethodmodifiers XmlIndexLookup::copy(const XmlIndexLookup&) "protected";

%newobject XmlIndexLookup::copy(const XmlIndexLookup &o);
%extend XmlIndexLookup{
    static XmlIndexLookup *copy(const XmlIndexLookup &o)
    {
	return new XmlIndexLookup(o);
    }
}

%typemap(javacode) XmlIndexLookup %{
    protected XmlContainer container;
    protected XmlValue lowBoundValue = new XmlValue();
    protected XmlValue highBoundValue = new XmlValue();
	
    public XmlIndexLookup(XmlIndexLookup o) throws XmlException {
	this(dbxml_javaJNI.XmlIndexLookup_copy(XmlIndexLookup.getCPtr(o), o), true);
	lowBoundValue = o.lowBoundValue;
	highBoundValue = o.highBoundValue;
    }
	
    public XmlContainer getContainer() 
	throws XmlException {
	return container;
    }
    public void setContainer(XmlContainer container)
	throws XmlException {
	this.container = container;
	setContainerInternal(container);
    }
    public XmlResults execute(XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(context, flags);
    }
    public XmlResults execute(XmlTransaction txn,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, context, flags);
    }
    public XmlValue getLowBoundValue()
	throws XmlException {
	return lowBoundValue;
    }
    public XmlValue getHighBoundValue()
	throws XmlException {
	return highBoundValue;
    }
    public void setHighBound(XmlValue value, int op) throws XmlException {
	setHighBoundInternal(value,op);
	highBoundValue = value;
    }
    public void setLowBound(XmlValue value, int op) throws XmlException {
	setLowBoundInternal(value,op);
	lowBoundValue = value;
    }
    %}

%typemap(javadestruct, methodname="delete", methodmodifiers="protected") XmlEventReader
{
    if(swigCPtr != 0 && swigCMemOwn) {
	swigCMemOwn = false;
	$jnicall;
    }
    swigCPtr = 0;
}

%javamethodmodifiers XmlEventReader::closeInternal() "protected";

%typemap(javacode) XmlEventReader %{
    public void close()
	/* wrapper for close that protects from double-close */
	throws XmlException {
	if (swigCPtr != 0) {
	    closeInternal();
	    swigCPtr = 0;
	}
    }
    protected void release()
    {
	swigCMemOwn = false;
	swigCPtr = 0;
    }
    %}

%typemap(javadestruct, methodname="delete", methodmodifiers="protected") XmlEventWriter
{
    if(swigCPtr != 0 && swigCMemOwn) {
	swigCMemOwn = false;
	$jnicall;
    }
    swigCPtr = 0;
}

%javamethodmodifiers XmlEventWriter::closeInternal() "protected";

%typemap(javacode) XmlEventWriter %{
    public void close()
	/* wrapper for close that protects from double-close */
	throws XmlException {
	if (swigCPtr != 0) {
	    try {
	        closeInternal();
	    } catch(XmlException e) {
	        swigCPtr = 0;
	        throw e;
	    }
	    swigCPtr = 0;
	}
    }
    protected void release()
    {
	swigCMemOwn = false;
	swigCPtr = 0;
    }
    %}
    
%javamethodmodifiers XmlInputStream::freeMemory() "protected";
%typemap(javacode) XmlInputStream %{
    protected void release()
    {
	swigCMemOwn = false;
	swigCPtr = 0;
    }
    %}
    
%javamethodmodifiers XmlResults::nextInternal() "protected";
%javamethodmodifiers XmlResults::previousInternal() "protected";
%javamethodmodifiers XmlResults::peekInternal() "protected";
%javamethodmodifiers XmlResults::nextDocumentInternal() "protected";
%javamethodmodifiers XmlResults::previousDocumentInternal() "protected";
%javamethodmodifiers XmlResults::peekDocumentInternal() "protected";
%javamethodmodifiers XmlResults::addInternal(const XmlValue&) "protected";
%javamethodmodifiers XmlResults::resetInternal() "protected";
%javamethodmodifiers XmlResults::copy(const XmlResults&) "protected";
%javamethodmodifiers XmlResults::asEventWriterInternal() "protected";

%newobject XmlResults::copy(const XmlResults &results);
%extend XmlResults{
    static XmlResults *copy(const XmlResults &results)
    {
	return new XmlResults(results);
    }
    XmlValue nextInternal() {
	XmlValue value;
	if (self->next(value)){
	    if(value.isNode()){
		if((*value).getDbXmlNodeValueObject()
			 && value.asDocument()
			 && (*value.asDocument()).getDbMinder() != 0
			 && (**self).getDbMinder() == 0){
		    (**self).getDbMinder() = (*value.asDocument()).getDbMinder();
		}
		if(!(*value).getRawNodeValueObject())
		    return RawNodeValue::create(value, *self);
	    }
	}
	return value;
    }
    XmlValue previousInternal() {
	XmlValue value;
	if (self->previous(value)){
	    if(value.isNode()){
		if ((*value).getDbXmlNodeValueObject()
			 && value.asDocument()
			 && (*value.asDocument()).getDbMinder() != 0
			 && (**self).getDbMinder() == 0){
		    (**self).getDbMinder() = (*value.asDocument()).getDbMinder();
		}
		if(!(*value).getRawNodeValueObject())
		    return RawNodeValue::create(value, *self);
	    }
	}
	return value;
    }
    XmlValue peekInternal() {
	XmlValue value;
	if (self->peek(value)){
	    if(value.isNode()){
		if((*value).getDbXmlNodeValueObject()
			 && value.asDocument()
			 && (*value.asDocument()).getDbMinder() != 0
			 && (**self).getDbMinder() == 0){
		    (**self).getDbMinder() = (*value.asDocument()).getDbMinder();
		}
		if(!(*value).getRawNodeValueObject())
		    return RawNodeValue::create(value, *self);
	    }
	}
	return value;
    }
    /* If a node is added that is protected by a transaction then it should
       be added to the Results context.  Mulitiple transactions are not allowed. */
    void addInternal(const XmlValue &value)
    {
	const RawNodeValue *raw = (*value).getRawNodeValueObject();
	if(raw){
	    Transaction *txn = raw->getTransaction();
	    if(txn){
		Transaction *resultsTxn = (**self).getOperationContext().txn();
		if(resultsTxn){
		    if(&*txn != &*resultsTxn){
			throw XmlException(XmlException::TRANSACTION_ERROR, "You should not use multiple transactions.");
		    }
		}else
		    (**self).getOperationContext().set(txn); 
	    }
	}
	self->add(value);
    }
    XmlDocument nextDocumentInternal()
    {
	XmlDocument cdoc;
	if(self->next(cdoc))
	    return cdoc;
	return 0;
    }
    XmlDocument previousDocumentInternal()
    {
	XmlDocument cdoc;
	if(self->previous(cdoc))
	    return cdoc;
	return 0;
    }
    XmlDocument peekDocumentInternal()
    {
	XmlDocument cdoc;
	if(self->peek(cdoc))
	    return cdoc;
	return 0;
    }
    XmlEventWriter &asEventWriterInternal() 
    {
        return self->asEventWriter();
    }
}

%typemap(javadestruct, methodname="delete", methodmodifiers="public") XmlResults {
    if(swigCPtr != 0 && swigCMemOwn) {
	swigCMemOwn = false;
	if(writer_ != null){
		try {
			writer_.close();
		} catch (XmlException e) {}
	}       
	$jnicall;
	if(resultsMap != null) {
		Iterator<XmlResults> itr = resultsMap.values().iterator();
		while(itr.hasNext())
			itr.next().delete();
	}
    }
    swigCPtr = 0;
 }

%typemap(javacode) XmlResults %{
    protected HashMap<Long, XmlResults> resultsMap = null;
	
    public XmlResults(XmlResults results) throws XmlException {
	this(dbxml_javaJNI.XmlResults_copy(XmlResults.getCPtr(results), results), true);
	if(results.resultsMap != null) resultsMap = new HashMap<Long, XmlResults>(results.resultsMap);
    }

    public XmlValue next() throws XmlException
    {
	XmlValue value = nextInternal();
	if(value == null || value.isNull())
	    return null;
	if(value.getReturnResultPtr() != 0)
	    value.setResult(resultsMapGet(new Long(value.getReturnResultPtr())));
	else if(value.isNode())
	    value.setResult(this);
	return value;
    }
    
    public XmlValue previous() throws XmlException
    {
	XmlValue value = previousInternal();
	if(value == null || value.isNull())
	    return null;
	if(value.getReturnResultPtr() != 0)
	    value.setResult(resultsMapGet(new Long(value.getReturnResultPtr())));
	else if(value.isNode())
	    value.setResult(this);
	return value;
    }
    
    public XmlValue peek() throws XmlException
    {
	XmlValue value = peekInternal();
	if(value == null || value.isNull())
	    return null;
	if(value.getReturnResultPtr() != 0)
	    value.setResult(resultsMapGet(new Long(value.getReturnResultPtr())));
	else if(value.isNode())
	    value.setResult(this);
	return value;
    }
    
    public void add(XmlValue value) throws XmlException
    {
	if(value == null || value.isNull())
	    throw new XmlException(XmlException.INVALID_VALUE, 
				   "A null XmlValue object cannot be added to the result set.");
	if (value.getResultPtr() != 0) {
		XmlResults res = XmlResults.copy(value.getResult());
	    resultsMapPut(new Long(value.getResultPtr()), res);
	}
	addInternal(value);
    }
    
    public void reset() throws XmlException
    {
	resetInternal();
    }
    
    public boolean next(XmlDocument document) throws XmlException
    {
	XmlDocument doc = nextDocumentInternal();
	if(doc == null)
	    return false;
	document.copy(doc);
	document.setResults(this);
	return true;
    }
    
    public boolean previous(XmlDocument document) throws XmlException
    {
	XmlDocument doc = previousDocumentInternal();
	if(doc == null)
	    return false;
	document.copy(doc);
	document.setResults(this);
	return true;
    }
    
    public boolean peek(XmlDocument document) throws XmlException
    {
	XmlDocument doc = peekDocumentInternal();
	if(doc == null)
	    return false;
	document.copy(doc);
	document.setResults(this);
	return true;
    }
    
    XmlEventWriter writer_ = null;
    public XmlEventWriter asEventWriter() throws XmlException {
        writer_ = asEventWriterInternal();
        return writer_;
    }

    protected XmlResults resultsMapGet(Long key){
	if(resultsMap == null) return null;
	return (XmlResults)resultsMap.get(key);
    }
    protected void resultsMapPut(Long key, XmlResults value){
	if(resultsMap == null) resultsMap = new HashMap<Long, XmlResults>();
	if (resultsMap.get(key) == null)
		resultsMap.put(key, value);
	else
		value.delete();
    }
    %}
    
/*
 * This is a hack to the typemap from java.swg for construction
 * that makes the writer, and optionally the reader inaccessible
 * after construction of XmlEventReaderToWriter.  It does not
 * appear possible to use %rename on constructors.
 */
%typemap(javaconstruct,directorconnect="\n $moduleJNI.$javaclassname_director_connect(this, swigCPtr, swigCMemOwn, WEAKREF);") XmlEventReaderToWriter { 
    this($imcall, true);$directorconnect
			    if(ownsWriter)
				writer.release();
    if (ownsReader)
	reader.release();
 }
 
%typemap(javacode) XmlEventReaderToWriter %{
    public XmlEventReaderToWriter(XmlEventReader reader, XmlEventWriter writer, boolean ownsReader) 
	throws XmlException { 
	this(reader, writer, ownsReader, true);
    }
    %}

%{
    /* don't use SWIG's array handling - save code space */
    #define SWIG_NOINCLUDE 1

	#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 4)
				   #define DB_EXTRA_EXC 1
				   #endif

				   // XML classes
				   static jclass xml_indexdecl_class;
    static jclass xml_valuedecl_class;
    static jclass xml_docdecl_class;
    static jclass xmlex_class;
    static jclass np_class;
    // DB classes
    static jclass dbenv_class;
    static jclass dbtxn_class;
    // DB exception classes
    static jclass dbex_class, deadex_class, lockex_class, memex_class;
    static jclass runrecex_class, rephandledeadex_class;
    #ifdef DB_EXTRA_EXC
	// DB 4.4 TBD...
	static jclass repdupmasterex_class;
    static jclass repholdelectionex_class, repunavailex_class;
    static jclass versionex_class;
    #endif


	// fields
	jfieldID dbenv_private_fid;
    jfieldID dbtxn_private_fid;

    // method IDs
    static jmethodID xml_indexdecl_construct;
    static jmethodID xml_value_construct;
    static jmethodID xml_valuenull_construct;
    static jmethodID xml_doc_construct;
    static jmethodID xmlex_construct;
    // DB exception method ids
    static jmethodID dbex_construct, deadex_construct, lockex_construct;
    static jmethodID memex_construct;
    static jmethodID rephandledeadex_construct;
    static jmethodID runrecex_construct;
    #ifdef DB_EXTRA_EXC
	// DB 4.4. TBD
	static jmethodID repdupmasterex_construct, memex_update_method;
    static jmethodID repholdelectionex_construct, repunavailex_construct;
    static jmethodID versionex_construct;
    #endif

    #define DB_PKG "com/sleepycat/db/"
    #define DB_PKG_INT "com/sleepycat/db/internal/"
    #define XML_PKG "com/sleepycat/dbxml/"

    /* Forward declarations */
    static jthrowable __dbj_get_except(JNIEnv *jenv,
				       int err, const char *msg, jobject obj, jobject jdbenv);

    // all classes
    const struct {
	jclass *cl;
	const char *name;
    } all_classes[] = {
	// XML
	{ &xml_indexdecl_class, XML_PKG "XmlIndexDeclaration" },
	{ &xml_valuedecl_class, XML_PKG "XmlValue" },
	{ &xml_docdecl_class, XML_PKG "XmlDocument" },
	{ &xmlex_class, XML_PKG "XmlException" },
	// DB
	{ &dbenv_class, DB_PKG_INT "DbEnv" },
	{ &dbtxn_class, DB_PKG_INT "DbTxn" },
	{ &dbex_class, DB_PKG "DatabaseException" },
	{ &deadex_class, DB_PKG "DeadlockException" },
	{ &lockex_class, DB_PKG "LockNotGrantedException" },
	{ &memex_class, DB_PKG "MemoryException" },
	{ &rephandledeadex_class, DB_PKG "ReplicationHandleDeadException" },
	{ &runrecex_class, DB_PKG "RunRecoveryException" },
	#ifdef DB_EXTRA_EXC
	{ &repdupmasterex_class, DB_PKG "ReplicationDuplicateMasterException" },
	{ &repholdelectionex_class, DB_PKG "ReplicationHoldElectionException" },
	{ &repunavailex_class, DB_PKG "ReplicationSiteUnavailableException" },
	{ &versionex_class, DB_PKG "VersionMismatchException" },
	#endif
	// Misc
	{ &np_class, "java/lang/NullPointerException" }
    };

    const struct {
	jmethodID *mid;
	jclass *cl;
	const char *name;
	const char *sig;
    } all_methods[] = {
	// XML methods
	{ &xml_indexdecl_construct, &xml_indexdecl_class, "<init>",
	  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"  },
	{ &xml_value_construct, &xml_valuedecl_class, "<init>",
	  "(I[Ljava/lang/Object;)V"  },
	{ &xml_valuenull_construct, &xml_valuedecl_class, "<init>",
	  "()V"  },
	{ &xml_doc_construct, &xml_docdecl_class, "<init>",
	  "(JI[B)V"  },
	{ &xmlex_construct, &xmlex_class, "<init>",
	  "(ILjava/lang/String;L" DB_PKG "DatabaseException;III)V"  },
	// DB methods
	{ &dbex_construct, &dbex_class, "<init>",
	  "(Ljava/lang/String;I)V" },
	{ &deadex_construct, &deadex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
	{ &lockex_construct, &lockex_class, "<init>",
	  "(Ljava/lang/String;IIL" DB_PKG "DatabaseEntry;L" DB_PKG "internal/DbLock;IL" DB_PKG "internal/DbEnv;)V" },
	{ &memex_construct, &memex_class, "<init>",
	  "(Ljava/lang/String;L" DB_PKG "DatabaseEntry;IL" DB_PKG "internal/DbEnv;)V" },
	{ &rephandledeadex_construct, &rephandledeadex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
	{ &runrecex_construct, &runrecex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" }
	#ifdef DB_EXTRA_EXC
	,{ &memex_update_method, &memex_class, "updateDatabaseEntry",
	   "(L" DB_PKG "DatabaseEntry;)V" },
	{ &repdupmasterex_construct, &repdupmasterex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
	{ &repholdelectionex_construct, &repholdelectionex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
	{ &repunavailex_construct, &repunavailex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
	{ &versionex_construct, &versionex_class, "<init>",
	  "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" }
	#endif
    };

    #define NELEM(x) (sizeof (x) / sizeof (x[0]))

	static bool checkDbVersion()
	{
	    int major, minor, patch, dmajor, dminor, dpatch;
	    (void) db_version(&major, &minor, &patch);
	    (void) DbXml::dbxml_db_version(&dmajor, &dminor, &dpatch);
	    // match major and minor numbers
	    // TBD: warn if patch level is different
	    if (major != dmajor || minor != dminor)
		return false;
	    return true;
	}

    extern "C"
	JNIEXPORT void JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_initialize
	(JNIEnv *jenv, jclass clazz)
	{
	    (void)clazz; /* unused */
	    jclass cl;
	    unsigned int i, j;

	    if (checkDbVersion() == false) {
		fprintf(stderr,
			"Mismatched Berkeley DB libraries");
		return;
	    }
	    // This initialization code comes from DB's java_util.i file
	    for (i = 0; i < NELEM(all_classes); i++) {
		cl = (jenv)->FindClass(all_classes[i].name);
		if (cl == NULL) {
		    fprintf(stderr,
			    "Failed to load class %s - check CLASSPATH\n",
			    all_classes[i].name);
		    return;
		}
		/*
		 * Wrap classes in GlobalRefs so we keep the reference between
		 * calls.
		 */
		*all_classes[i].cl = (jclass)(jenv)->NewGlobalRef(cl);

		if (*all_classes[i].cl == NULL) {
		    fprintf(stderr,
			    "Failed to create a global reference for %s\n",
			    all_classes[i].name);
		    return;
		}
	    }

	    // get some field ids
	    #if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
				       dbenv_private_fid = jenv->GetFieldID(dbenv_class, "private_dbobj_", "J");
	    dbtxn_private_fid = jenv->GetFieldID(dbtxn_class, "private_dbobj_", "J");
	    #else
		dbenv_private_fid = jenv->GetFieldID(dbenv_class, "swigCPtr", "J");
	    dbtxn_private_fid = jenv->GetFieldID(dbtxn_class, "swigCPtr", "J");
	    #endif
		/* Get method IDs */
		for (i = 0; i < NELEM(all_methods); i++) {
		    *all_methods[i].mid = (jenv)->
			GetMethodID(*all_methods[i].cl, all_methods[i].name,
				    all_methods[i].sig);

		    if (*all_methods[i].mid == NULL) {
			for (j = 0; j < NELEM(all_classes); j++)
			    if (all_methods[i].cl == all_classes[j].cl)
				break;
			fprintf(stderr,
				"Failed to look up method %s.%s with sig %s\n",
				all_classes[j].name, all_methods[i].name,
				all_methods[i].sig);
			return;
		    }
		}
	}
	
    /* Union to convert longs to pointers (see {get,set}_private_dbobj). */
    typedef union {
	jlong java_long;
	void *ptr;
    } long_to_ptr;

    static DB_ENV *get_DB_ENV(JNIEnv *jenv, jobject jdbenv)
    {
	if (jdbenv == NULL)
	    return NULL;
	else {
	    #if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
				       long_to_ptr lp;
	    lp.java_long = jenv->GetLongField(jdbenv, dbenv_private_fid);
	    return (DB_ENV *)lp.ptr;
	    #else
		jlong swigCPtr = jenv->GetLongField(jdbenv, dbenv_private_fid);
	    return *(DB_ENV **)&swigCPtr;
	    #endif
	}
    }

    static DB_TXN *get_DB_TXN(JNIEnv *jenv, jobject jtxn)
    {
	if (jtxn == NULL)
	    return NULL;
	else {
	    #if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
				       long_to_ptr lp;
	    lp.java_long = jenv->GetLongField(jtxn, dbtxn_private_fid);
	    return (DB_TXN *)lp.ptr;
	    #else
		jlong swigCPtr = jenv->GetLongField(jtxn, dbtxn_private_fid);
	    return *(DB_TXN **)&swigCPtr;
	    #endif
	}
    }

    struct DBT_INFO {
	DBT dbt;
	jbyte *orig_data;
    };

    %}

%{
// Deals with XmlException and std::exception 
jthrowable createException(JNIEnv *jenv, const std::exception *se)
{
    const XmlException *xe = dynamic_cast<const XmlException*>(se);
    if (xe) {
        // use 0 for dberr; otherwise, DatabaseException::toString
        // prints redundant information.  The DB error information
        // is already in the what() string.
	XmlException::ExceptionCode ecode = xe->getExceptionCode();
        if (ecode == XmlException::NULL_POINTER) {
	    // throw null pointer exception
	    jenv->ThrowNew(np_class, xe->what());
	    return NULL;
        } else {
	    jthrowable dt = 0;
	    const char *msg = xe->what();
	    int qLine = xe->getQueryLine();
	    int qCol = xe->getQueryColumn();
	    if (ecode == XmlException::DATABASE_ERROR) {
		dt = __dbj_get_except(jenv, xe->getDbErrno(),
				      msg, NULL, NULL);
	    }
	    return (jthrowable)jenv->
		NewObject(xmlex_class, xmlex_construct,
			  ecode, jenv->NewStringUTF(msg), dt, 0, qLine, qCol);
        }
    }
    std::string emsg = "std::exception thrown from C++ API: ";
    emsg += se->what();
    return (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
		XmlException::INTERNAL_ERROR,
		jenv->NewStringUTF(emsg.c_str()), 0, 0, 0, 0);
}
%}

// Typemaps
%define JAVA_TYPEMAP(_ctype, _jtype, _jnitype)
    %typemap(jstype) _ctype #_jtype
    %typemap(jtype) _ctype #_jtype
    %typemap(jni) _ctype #_jnitype
    %typemap(javain) _ctype "$javainput"
    %typemap(javaout) _ctype { return $jnicall; }
%enddef

    %apply int { u_int32_t };
    %apply int { size_t };
    JAVA_TYPEMAP(u_int32_t, int, jint)
    JAVA_TYPEMAP(size_t, int, jint)
    JAVA_TYPEMAP(u_int64_t*, long[], jlongArray)
    
    %typemap(in) u_int64_t* %{
		jlong *jids$argnum = jenv->GetLongArrayElements($input, (jboolean *)0);
		$1 = (u_int64_t*)jids$argnum;
    %}
    
    //Copy the C++ array back to the Java array and release the C++ memory
    %typemap(argout) u_int64_t* %{
		jenv->ReleaseLongArrayElements($input, jids$argnum, 0);
    %}

    %typemap(in) bool %{
    $1 = ($input != JNI_FALSE);
    %}

%typemap(out) bool %{
    $result = $1 ? JNI_TRUE : JNI_FALSE;
    %}

JAVA_TYPEMAP(DB_ENV *, com.sleepycat.db.internal.DbEnv, jobject)
    %typemap(in) DB_ENV * {
    $1 = get_DB_ENV(jenv, $input);
}

JAVA_TYPEMAP(DB_TXN *, com.sleepycat.db.internal.DbTxn, jobject)
    %typemap(in) DB_TXN * {
    $1 = get_DB_TXN(jenv, $input);
}

/*
 * Fake DBT and XmlData * translation to work around SWIG problems
 * with (void *, size_t) typemaps for Java.
 */
JAVA_TYPEMAP(DBT *, byte[], jbyteArray)

    %typemap(in) DBT *(struct DBT_INFO ldbt) %{
    if ($input != NULL) {
        ldbt.dbt.data = ldbt.orig_data = jenv->GetByteArrayElements($input, (jboolean *)0);
        ldbt.dbt.size = jenv->GetArrayLength($input);
    } else {
	ldbt.dbt.data = ldbt.orig_data = 0;
	ldbt.dbt.size = 0;
    }
    ldbt.dbt.flags = DB_DBT_MALLOC;
    $1 = &ldbt.dbt;
    %}

%typemap(freearg) DBT * %{
    if ($input != NULL)
        jenv->ReleaseByteArrayElements($input, ldbt$argnum.orig_data, JNI_ABORT);
    %}

%typemap(out) const DBT * %{
    $result = jenv->NewByteArray($1->size);
    jenv->SetByteArrayRegion($result, 0, $1->size, (jbyte *)$1->data);
    %}

JAVA_TYPEMAP(XmlIndexDeclaration *, XmlIndexDeclaration, jobject)
    %typemap(out) XmlIndexDeclaration * %{
    if ($1 == NULL)
        $result = NULL;
    else {
        $result = jenv->NewObject(xml_indexdecl_class, xml_indexdecl_construct,
				  jenv->NewStringUTF($1->uri.c_str()),
				  jenv->NewStringUTF($1->name.c_str()),
				  jenv->NewStringUTF($1->index.c_str()));
        delete $1;
    }
    %}
    
/* The XmlCompression class is passed as a pointer. */
%typemap(jstype) XmlCompression& "XmlCompression"
%typemap(jtype) XmlCompression& "long"
%typemap(jni) XmlCompression& "jlong"
%typemap(javaout) XmlCompression& { return $jnicall; }
%typemap(javain) XmlCompression & %{ XmlCompression.getCPtr($javainput) %}
%typemap(in) XmlCompression & %{ $1 = *($&1_ltype)&$input; %}
   
/* XmlContainerConfig is passed from Java to C++ by spliting it into
an int[] that contains all of its values, except the compression name, 
which is stored in the String that is also passed.  XmlContainerConfig 
is never returned from C++ to Java. */
%typemap(jstype) XmlContainerConfig& "XmlContainerConfig"
%typemap(jtype) XmlContainerConfig& "int[] data$argnum, String"
%typemap(jni) XmlContainerConfig& "jintArray data$argnum, jstring"
%typemap(javain) XmlContainerConfig& %{ $javainput.pack(), $javainput.getCompression() %}
%typemap(jstype) XmlContainerConfig "XmlContainerConfig"
%typemap(jtype) XmlContainerConfig "int[] data$argnum, String"
%typemap(jni) XmlContainerConfig "jintArray data$argnum, jstring"
%typemap(javain) XmlContainerConfig %{ $javainput.pack(), $javainput.getCompression() %}
%typemap(in) XmlContainerConfig %{
    $1 = createCPPXmlContainerConfig(jenv, data$argnum, $input);	
%}
%typemap(in) XmlContainerConfig &%{
    XmlContainerConfig config$argnum = createCPPXmlContainerConfig(jenv, data$argnum, $input);	
	$1 = &config$argnum;
%}
    
%{
    //Creates a C++ XmlContainerConfig from the information in a Java XmlContainerConfig
    XmlContainerConfig createCPPXmlContainerConfig(JNIEnv *jenv, jintArray &configArray, jstring &jname)
    {
	    XmlContainerConfig cconfig;
	    jthrowable thr = NULL;
	    jint *info = 0;
	    const char *cname = 0;
	    
	    try {
			//get the mode, container type, xml flags, db flags, page size, and sequence increment
			info = jenv->GetIntArrayElements(configArray, 0);
			cconfig.setMode(info[0]);
			cconfig.setContainerType((XmlContainer::ContainerType)info[1]);
			(*cconfig).setXmlFlags(info[2]);
			(*cconfig).setDbOpenFlags(info[3]);
			cconfig.setPageSize(info[4]);
			cconfig.setSequenceIncrement(info[5]);
			(*cconfig).setDbSetFlags(info[6]);
			(*cconfig).setSeqFlags(info[7]);
			jenv->ReleaseIntArrayElements(configArray, info, JNI_ABORT);
			info = 0;
			
			//get the compression name
			cname = jenv->GetStringUTFChars(jname, (jboolean *)0);
			cconfig.setCompressionName(cname);
			jenv->ReleaseStringUTFChars(jname, cname);
			cname = 0;
		} catch (std::exception &se) {
			if (info) jenv->ReleaseIntArrayElements(configArray, info, JNI_ABORT);
			if (cname) jenv->ReleaseStringUTFChars(jname, cname);
		    thr = createException(jenv, &se);
	    }
	    catch(JavaException & /* je */) {
			if (info) jenv->ReleaseIntArrayElements(configArray, info, JNI_ABORT);
			if (cname) jenv->ReleaseStringUTFChars(jname, cname);
		    // This means there's already an exception waiting in the JVM
		    // Fall through
	    }
	    catch (...) {
			if (info) jenv->ReleaseIntArrayElements(configArray, info, JNI_ABORT);
			if (cname) jenv->ReleaseStringUTFChars(jname, cname);
		    thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
						      XmlException::INTERNAL_ERROR,
						      jenv->NewStringUTF("Uncaught exception from C++ API Constructing XmlDocument"), 0, 0, 0, 0);
	    }
	    if (thr)
		    jenv->Throw(thr);
		return cconfig;
	}
	
    %}

// _dbj_* are directly from DB's java_except.i file

%{
    static jthrowable __dbj_get_except(
				       JNIEnv *jenv,
				       int err, const char *msg, jobject obj, jobject jdbenv) {
	jobject jmsg;

	if (msg == NULL)
	    msg = db_strerror(err);

	jmsg = jenv->NewStringUTF(msg);

	switch (err) {
	    #if 0
		// don't map these (yet?)
	case EINVAL:
	    return (jthrowable)jenv->
		NewObject(
			  illegalargex_class, illegalargex_construct, jmsg);

	case ENOENT:
	    return (jthrowable)jenv->
		NewObject(filenotfoundex_class, filenotfoundex_construct, jmsg);

	case ENOMEM:
	    return (jthrowable)jenv->
		NewObject(outofmemerr_class, outofmemerr_construct, jmsg);
	    #endif
	case DB_BUFFER_SMALL:
	    return (jthrowable)jenv->
		NewObject(memex_class, memex_construct, jmsg, obj, err, jdbenv);

	case DB_REP_HANDLE_DEAD:
	    return (jthrowable)jenv->
		NewObject(rephandledeadex_class, rephandledeadex_construct,
			  jmsg, err, jdbenv);
	case DB_RUNRECOVERY:
	    return (jthrowable)jenv->
		NewObject(runrecex_class, runrecex_construct, jmsg, err, jdbenv);

	case DB_LOCK_DEADLOCK:
	    return (jthrowable)jenv->
		NewObject(deadex_class, deadex_construct, jmsg, err, jdbenv);

	case DB_LOCK_NOTGRANTED:
	    return (jthrowable)jenv->
		NewObject(lockex_class,lockex_construct,
			  jmsg, 0, 0, NULL, NULL, 0, jdbenv);

	    #ifdef DB_EXTRA_EXC
		// These are from DB 4.4, TBD...
	case DB_REP_DUPMASTER:
	    return (jthrowable)jenv->
		NewObject(repdupmasterex_class, repdupmasterex_construct,
			  jmsg, err, jdbenv);

	case DB_REP_HOLDELECTION:
	    return (jthrowable)jenv->
		NewObject(repholdelectionex_class, repholdelectionex_construct,
			  jmsg, err, jdbenv);

	case DB_REP_UNAVAIL:
	    return (jthrowable)jenv->
		NewObject(repunavailex_class, repunavailex_construct,
			  jmsg, err, jdbenv);

	case DB_VERSION_MISMATCH:
	    return (jthrowable)jenv->
		NewObject(versionex_class,
			  versionex_construct, jmsg, 0, 0, NULL, NULL, 0, jdbenv);
	    #endif //  DB 4.4
	default:
	    return (jthrowable)jenv->
		NewObject(dbex_class,
			  dbex_construct, jmsg, err, jdbenv);
	}
    }

    %}

%rename(XmlExternalFunctionBase) XmlExternalFunction;


%{
/*
 * Class:     com_sleepycat_dbxml_dbxml_javaJNI
 * Method:    getContent
 * Signature: (Lcom/sleepycat/dbxml/XmlDocument;)[B
 */
extern "C"
JNIEXPORT jbyteArray JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_getContent
  (JNIEnv * jenv, jclass jcls, jobject jarg1){
  jbyteArray jresult = 0 ;
  XmlDocument *arg1 = 0 ;
  
  (void)jenv;
  (void)jcls;
  
  XmlDocument cdoc1 = createCPPXmlDocument(jenv, jarg1);
  if (cdoc1.isNull()) return 0;
  arg1 = &cdoc1;
  
  {
    jthrowable t = NULL;
    try {
      XmlData xd = arg1->getContent();
      u_int32_t size = xd.get_size();
      if(size >0){
        jresult = jenv->NewByteArray(size);
        if(jresult!=0)
          jenv->SetByteArrayRegion(jresult, 0, size, (jbyte*)xd.get_data());
      }else
        jresult = jenv->NewByteArray(0);
    }
    catch (std::exception &se) {
      t = createException(jenv, &se);
      jenv->Throw(t);
      return 0;
    }
    catch(JavaException & /* je */) {
      // This means there's already an exception waiting in the JVM
      return 0;
    }
    catch (...) {
      t = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
        XmlException::INTERNAL_ERROR,
        jenv->NewStringUTF("Uncaught exception from C++ API"), 0, 0, 0, 0);
      jenv->Throw(t);
      return 0;
    }
  }
  return jresult;
}


%}

%include "XmlValue_java.i"
%include "XmlDocument_java.i"
%include "XmlQueryContext_java.i"
%include "HelperFunctions_java.i"
%include "XmlData_java.i"
