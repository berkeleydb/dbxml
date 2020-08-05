/* -*- Implements the XmlQueryContext class for the Java API. -*- */

/* XmlQueryContext is passed from the Java to the C++ by breaking it into 5 arguments.
The pointer to the XmlManager, the pointer to the XmlDebugListener, the evaluation type,
the number of seconds before the query times out, an Object[] containing all the namespaces
followed by all the variables, and the Java XmlQueryContext object itself.  XmlQueryContext
is never returned from the C++ to the Java. */
%typemap(jstype) XmlQueryContext& "XmlQueryContext"
%typemap(jtype) XmlQueryContext& "long mgrPtr$argnum, long dbgPtr$argnum, int eval$argnum, int querySec$argnum, Object[] data$argnum, XmlQueryContext"
%typemap(jni) XmlQueryContext& "jlong mgrPtr$argnum, jlong dbgPtr$argnum, jint eval$argnum, jint querySec$argnum, jobjectArray data$argnum, jobject"
%typemap(javain) XmlQueryContext& %{ $javainput.getManagerPtr(), $javainput.getDebugListenerCPtr(), $javainput.evaluationType, $javainput.queryInterruptSec, $javainput.pack(), $javainput %}
%typemap(in) XmlQueryContext &%{
	queryContextRef qcRef(jenv, mgrPtr$argnum, dbgPtr$argnum, eval$argnum, querySec$argnum, data$argnum, $input);
	if (qcRef.getQC()->isNull())
		return 0; // exception was thrown
	$1 = qcRef.getQC();
%}

%{
//Create a C++ XmlQueryContext object from a Java XmlQueryContext object
XmlQueryContext createCPPXmlQueryContext(JNIEnv *jenv, 
					jlong mgrPtr, jlong dbgPtr, jint et, 
					jint queryTimeout, jobjectArray objects)
{
XmlQueryContext cqc;
{
    jthrowable thr = NULL;
    try {
	if (mgrPtr == 0){
	    XmlException e(XmlException::INVALID_VALUE, "No manager exists for the XmlQueryContext, cannot preceed.");
	    throw e;
	}
	XmlManager *mgr = *(XmlManager **)&mgrPtr;
	//get the return and evaluation types
	cqc = mgr->createQueryContext(XmlQueryContext::LiveValues, (XmlQueryContext::EvaluationType)et);
	int arrayLength = jenv->GetArrayLength(objects);
	//get the base uri
	int i = 0;
	const char *curi;
	jstring juri = (jstring)jenv->GetObjectArrayElement(objects, i++);
	if(juri != 0){
	    curi = jenv->GetStringUTFChars(juri, (jboolean *)0);
	    std::string uri(curi);
	    jenv->ReleaseStringUTFChars(juri, curi);
	    cqc.setBaseURI(uri);
	}
	//get the default collection
	juri = (jstring)jenv->GetObjectArrayElement(objects, i++);
	if(juri != 0){
	    curi = jenv->GetStringUTFChars(juri, (jboolean *)0);
	    std::string uri2(curi);
	    jenv->ReleaseStringUTFChars(juri, curi);
	    cqc.setDefaultCollection(uri2);
	}
	//get the debug listener
	XmlDebugListener *listener = *(XmlDebugListener**)&dbgPtr;
	cqc.setDebugListener(listener);
	//get the Query Interrupt time
	cqc.setQueryTimeoutSeconds(queryTimeout);
	//get the namespaces
	jobject jprefix = jenv->GetObjectArrayElement(objects, i++);
	while(arrayLength > i && jprefix != 0){
	    //prefix
	    curi = jenv->GetStringUTFChars((jstring)jprefix, (jboolean *)0);
	    std::string prefix;
	    if (curi) {
			prefix = curi;
			jenv->ReleaseStringUTFChars((jstring)jprefix, curi);
		}
	    //uri
	    juri = (jstring)jenv->GetObjectArrayElement(objects, i++);
	    std::string uri;
	    if (juri) {
			curi = jenv->GetStringUTFChars(juri, (jboolean *)0);
			if (curi) {
				uri = curi;
				jenv->ReleaseStringUTFChars(juri, curi);
			}
		}
	    cqc.setNamespace(prefix, uri);
	    jprefix = jenv->GetObjectArrayElement(objects, i++);
	}
	//get variables
	while(arrayLength > i){
	    //name
	    std::string name;
	    jstring jname = (jstring)jenv->GetObjectArrayElement(objects, i++);
	    if (jname) {
			curi = jenv->GetStringUTFChars(jname, (jboolean *)0);
			if (curi) {
				name = curi;
				jenv->ReleaseStringUTFChars(jname, curi);
			}
		}
	    //value
	    jobjectArray jvalues = (jobjectArray)jenv->GetObjectArrayElement(objects, i++);
	    int valueArrayLength = jenv->GetArrayLength(jvalues);
	    XmlResults results = mgr->createResults(); 
	    for(int i = 0; i < valueArrayLength; i++){
		jobject jvalue = jenv->GetObjectArrayElement(jvalues, i);
		XmlValue value = createCPPXmlValue(jenv, jvalue);
		if (value.isNode()) { //Force materialization of nodes
			const RawNodeValue *raw = (*value).getRawNodeValueObject();
			if (raw) {
				XmlDocument doc = raw->asDocument();
				if(!doc.isNull()) {
					if (raw->getDBMinder() && !raw->getDBMinder()->isNull() && (*doc).isUninitialized()) {
						(*doc).setDbMinder(*raw->getDBMinder());
						(*doc).getDocDb();
						(*doc).setContentAsNsDom((*doc).getID(), (*doc).getCacheDatabase());
					} else
						(*doc).changeContentToNsDom(0);
				}
			}
		}
		results.add(value); 
	    }
	    results.reset();
	    //set the variable
	    cqc.setVariableValue(name, results);
	}
    } catch (std::exception &se) {
	thr = createException(jenv, &se);
    }
    catch(JavaException & /* je */) {
	// This means there's already an exception waiting in the JVM
	    return XmlQueryContext();
    }
    catch (...) {
	    thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					      XmlException::INTERNAL_ERROR,
					      jenv->NewStringUTF("Uncaught exception from C++ API Constructing XmlQueryContext"), 0, 0, 0, 0);
    }

    if (thr) {
	jenv->Throw(thr);
	return XmlQueryContext();
    }
}
return cqc;
}

/*
 * Class:     com_sleepycat_dbxml_dbxml_javaJNI
 * Method:    interruptQuery
 * Signature: (J)V
 */
extern "C"
JNIEXPORT void JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_interruptQuery
(JNIEnv *jenv, jclass cla, jlong contextPtr)
{
    XmlQueryContext context((QueryContext *)contextPtr);
    if(context.isNull()) return;
    context.interruptQuery();
}
	
struct queryContextRef {
public:
	queryContextRef(JNIEnv *jenv, jlong mgrPtr, jlong dbgPtr, jint eval, jint querySec, jobjectArray data, jobject obj) :
		env_(jenv), obj_(obj) {
		cls_ = env_->GetObjectClass(obj);
		setCPtr_ = env_->GetMethodID(cls_, "setCPtr", "(J)V");
		qc_ = createCPPXmlQueryContext(env_, mgrPtr, dbgPtr, eval, querySec, data);
		if (qc_.isNull()) return; //exception happened while creating context
		//Set the c pointer in Java so the query can be interrupted
		QueryContext *cptr = (QueryContext *)qc_;
		env_->CallVoidMethod(obj_, setCPtr_, (jlong)cptr);
	}
	~queryContextRef() {
		// clear pointer in Java object
		jthrowable ex = env_->ExceptionOccurred();
		if (ex != NULL)
		    env_->ExceptionClear();
		env_->CallVoidMethod(obj_, setCPtr_, (jlong)0);
		if(ex != NULL)
			env_->Throw(ex);
	}
	XmlQueryContext *getQC() { return &qc_; }
	XmlQueryContext qc_;
	JNIEnv *env_;
	jobject obj_;
	jclass cls_;
	jmethodID setCPtr_;
};
%}
