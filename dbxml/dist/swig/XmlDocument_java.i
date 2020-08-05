/* -*- Implementation of the XmlDocument class for Java -*- */

/* XmlDocument is passed from C++ to Java, and from Java to C++ as a 
jobject.  The translation is made between the Java object and the C++
object in the functions createCPPXmlDocument and createJavaXmlDocument. */
JAVA_TYPEMAP(XmlDocument *, XmlDocument, jobject)
JAVA_TYPEMAP(XmlDocument &, XmlDocument, jobject)
JAVA_TYPEMAP(XmlDocument, XmlDocument, jobject)

/* Translate the Java XmlDocument into a C++ XmlDocument */
%typemap(in) XmlDocument & %{
	XmlDocument cdoc$argnum = createCPPXmlDocument(jenv, $input);
	if (cdoc$argnum.isNull()) return $null;
	$1 = &cdoc$argnum;
%}


//Turns the C++ XmlDocument object return by the function into a Java object
%typemap(out) const XmlDocument *,  XmlDocument *{
    if($1 == NULL)
	    $result = NULL;
    else
	    $result = createJavaXmlDocument(jenv, *$1);
}
%typemap(out) XmlDocument {
    $result = createJavaXmlDocument(jenv, $1);
}
%typemap(out) const XmlDocument &, XmlDocument &{
    $result = createJavaXmlDocument(jenv, *$1);
 }

%{

//Set the altered metadata in a Java XmlDocument from a C++ XmlDocument
void createJavaMetaData(JNIEnv *jenv, jobject jdoc, XmlDocument &cdoc, bool all = false)
{
if(!jdoc || cdoc.isNull()) return;
	
XmlMetaDataIterator itr = cdoc.getMetaDataIterator();
std::string uri, name;
XmlValue value;
std::vector<std::string> uris, names;
std::vector<jboolean> modifieds, removes;
std::vector<jobject> values;
while(itr.next(uri, name, value)){
    if(all || (*itr).previousIsModified() || (*itr).previousIsRemoved()){
		jobject jvalue = createJavaXmlValue(jenv, value);
		names.push_back(name);
		uris.push_back(uri);
		values.push_back(jvalue);
		modifieds.push_back((*itr).previousIsModified());
		removes.push_back((*itr).previousIsRemoved());
    }
}
int length = names.size();
if(length < 1) return;
jobjectArray jnames = jenv->NewObjectArray(length, jenv->FindClass("Ljava/lang/String;"), NULL); 
jobjectArray juris = jenv->NewObjectArray(length, jenv->FindClass("Ljava/lang/String;"), NULL);
jobjectArray jvalues = jenv->NewObjectArray(length, jenv->FindClass("Lcom/sleepycat/dbxml/XmlValue;"), NULL);
jbooleanArray jmodifieds = jenv->NewBooleanArray(length);
jbooleanArray jremoves = jenv->NewBooleanArray(length);

for(int i = 0; i < length; i++)
{
	jenv->SetObjectArrayElement(juris, i, jenv->NewStringUTF(uris[i].c_str()));
	jenv->SetObjectArrayElement(jnames, i, jenv->NewStringUTF(names[i].c_str()));
	jenv->SetObjectArrayElement(jvalues, i, values[i]);
	jboolean mod = modifieds[i];
	jenv->SetBooleanArrayRegion(jmodifieds, i, 1, &mod);
	mod = removes[i];
	jenv->SetBooleanArrayRegion(jremoves, i, 1, &mod);
}
jclass cls = jenv->GetObjectClass(jdoc);
jmethodID mid = jenv->GetMethodID(cls, "addMetaData", 
				  "([Ljava/lang/String;[Ljava/lang/String;[Lcom/sleepycat/dbxml/XmlValue;[Z[Z)V");
jenv->CallVoidMethod(jdoc, mid, juris, jnames, jvalues, jmodifieds, jremoves);
}

//Set all altered metadata in a C++ XmlDocument from a Java XmlDocument
void createCPPMetaData(JNIEnv *jenv, jobjectArray jdoc, XmlDocument &cdoc)
{
if(!jdoc || cdoc.isNull()) return;
	
int index = 0;
int size = jenv->GetArrayLength(jdoc);
jboolean *state = 0;
jbooleanArray jstate = 0;
jthrowable thr = NULL;
try {
	while(index < size)
	{
		jstring jname = (jstring)jenv->GetObjectArrayElement(jdoc, index);
		if(jname)
		{
			index++;
			std::string uri;
			const char *str = jenv->GetStringUTFChars(jname, (jboolean *)0);
			if (str) {
				uri = str;
				jenv->ReleaseStringUTFChars(jname, str);
			}
			std::string name;
			jname = (jstring)jenv->GetObjectArrayElement(jdoc, index++);
			if (jname) {
				str = jenv->GetStringUTFChars(jname, (jboolean *)0);
				if (str) {
					name = str;
					jenv->ReleaseStringUTFChars(jname, str);
				}
			}
			jobject jvalue = jenv->GetObjectArrayElement(jdoc, index++);
			jstate = (jbooleanArray)jenv->GetObjectArrayElement(jdoc, index++);
			state = jenv->GetBooleanArrayElements(jstate, (jboolean *)0);
			if(state[0] != JNI_FALSE) { //if modifed, add it to the doc
				XmlValue value = createCPPXmlValue(jenv, jvalue);
				cdoc.setMetaData(uri, name, value);
			}
			if(state[1] != JNI_FALSE) //if removed, remove it from the doc
				cdoc.removeMetaData(uri, name);
			jenv->ReleaseBooleanArrayElements(jstate, state, JNI_ABORT);
			state = 0;
		} else 
			index += 4;
	}
} catch (std::exception &se) {
	if (state) jenv->ReleaseBooleanArrayElements(jstate, state, JNI_ABORT);
	thr = createException(jenv, &se);
}
catch(JavaException & /* je */) {
	if (state) jenv->ReleaseBooleanArrayElements(jstate, state, JNI_ABORT);
}
catch (...) {
	if (state) jenv->ReleaseBooleanArrayElements(jstate, state, JNI_ABORT);
	thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					  XmlException::INTERNAL_ERROR,
					  jenv->NewStringUTF("Uncaught exception from C++ API Constructing XmlQueryContext"), 0, 0, 0, 0);
}
if (thr) {
	jenv->Throw(thr);
}
}

jobject createJavaXmlDocument(JNIEnv *jenv, XmlDocument &cdoc)
{
jthrowable thr = NULL;
jobject jdoc = 0;
    try {
	if(cdoc.isNull()) return jdoc;
	int cid = (*cdoc).getContainerID();
	u_int64_t docid = *(u_int64_t *)((*cdoc).getID().rawPtr());
	jbyteArray bytes = 0;
	int contentType = (*cdoc).getDefinitiveContent();
	if(contentType != Document::NONE){
		/* If the data has been modified or the document is temporary then
		   the content has to be changed to a DBT so it will not be consumed
		   from the C++ document, which is what would happen if the content
		   was an INPUTSTREAM or EVENTREADER.  */
		if((*cdoc).isContentModified() || docid == 0){
			XmlData content = cdoc.getContent();
			bytes = jenv->NewByteArray(content.get_size());
			jbyte *data = static_cast<jbyte*>(content.get_data());
			jenv->SetByteArrayRegion(bytes, 0, jenv->GetArrayLength(bytes), data);
		}
	}
	jdoc = jenv->NewObject(xml_docdecl_class, xml_doc_construct, docid, cid, bytes);
	createJavaMetaData(jenv, jdoc, cdoc);
} catch (std::exception &se) {
	thr = createException(jenv, &se);
}
catch(JavaException & /* je */) {
	// This means there's already an exception waiting in the JVM
	jdoc = 0;
}
catch (...) {
	thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					  XmlException::INTERNAL_ERROR,
					  jenv->NewStringUTF("Uncaught exception from C++ API Constructing Java XmlDocument"), 0, 0, 0, 0);
}
if (thr) {
	jenv->Throw(thr);
	jdoc = 0;
}
return jdoc;
}

//Look in XmlDocument.java at the function pack() to find the makeup of the
//jobjectArray.
XmlDocument createCPPXmlDocument(JNIEnv *jenv, jobject jdoc, bool setData)
{
    XmlDocument cdoc;
    jthrowable thr = NULL;
    jlong *state = 0;
    jlongArray jstate = 0;
    if(jdoc == 0)
	    return cdoc;
    try {
	    jclass cls = jenv->GetObjectClass(jdoc);	
	    jmethodID mid = jenv->GetMethodID(cls, "pack", "()[Ljava/lang/Object;");
	    jobjectArray jobj = (jobjectArray)jenv->CallObjectMethod(jdoc, mid);
	    jstate = (jlongArray)jenv->GetObjectArrayElement(jobj, 0);
	    state = jenv->GetLongArrayElements(jstate, (jboolean*)0);
	    jlong mgrPtr = state[3];
	    jlong resultPtr = state[2];
	    if (mgrPtr != 0){
		    XmlManager *mgr = *(XmlManager **)&mgrPtr;
		    cdoc = mgr->createDocument();
		    jlong txnPtr = state[4];
		    if(txnPtr != 0){
			    XmlTransaction *txn = *(XmlTransaction **)&txnPtr;
			    (*cdoc).getOperationContext().set(&**txn);
		    }
	    }else if(resultPtr != 0){
		    XmlResults &res = **(XmlResults **)&resultPtr;
		    XmlManager mgr = (*res).getManager();
		    cdoc = mgr.createDocument();
		    (*cdoc).getOperationContext().set((*res).getOperationContext().txn());
		    (*cdoc).setDbMinder((*res).getDbMinder());
	    }else{
		    XmlException e(XmlException::INVALID_VALUE, "Either the manager that created this document, or the results set that return it, has been deleted, cannot access the document.");
		    throw e;
	    }
	    //set doc id
	    u_int64_t docid = state[0];
	    DocID id(docid);
	    (*cdoc).setID(id);
	    
	    //set cid
	    jintArray jcidNType = (jintArray)jenv->GetObjectArrayElement(jobj, 1);
	    jint *cidNType = jenv->GetIntArrayElements(jcidNType, (jboolean*)0);
	    int cid = cidNType[0];
	    int type = cidNType[1];
	    jenv->ReleaseIntArrayElements(jcidNType, cidNType, JNI_ABORT);
	    (*cdoc).setContainerID(cid);
	    
	    if(!setData) {
	            jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	            state = 0;
		    return cdoc;
	    }
	    switch(type){
	    case Document::READER:
	    {
		    XmlEventReader *reader = *(XmlEventReader **)&(state[1]);
		    (*cdoc).setContentAsEventReader(*reader);
		    break;
	    }
	    case Document::INPUTSTREAM:
	    {
		    XmlInputStream *stream = *(XmlInputStream **)&(state[1]);
		    (*cdoc).setContentAsInputStream(&stream);
		    break;
	    }
	    case Document::DBT:
	    {
		    jbyteArray content = (jbyteArray)jenv->GetObjectArrayElement(jobj, 2);
		    jbyte *bytes = jenv->GetByteArrayElements(content, (jboolean *)0);
		    DbtOut *data;
		    if (bytes) {
				data = new DbtOut((void *)bytes, jenv->GetArrayLength(content));
				jenv->ReleaseByteArrayElements(content, bytes, JNI_ABORT);
			} else 
				data = new DbtOut(0, 0);
		    (*cdoc).setContentAsDbt(&data);
		    break;
	    }
	    default:
		    if(docid != 0 && cid != 0)
			    (*cdoc).setAsNotMaterialized();
			else if(!(*cdoc).getDbMinder().isNull() && cid == 0) {  //set query constructed documents as DOM
				(*cdoc).getDocDb();
				(*cdoc).setContentAsNsDom(docid, (*cdoc).getCacheDatabase());
			}
	    }
	    jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	    state = 0;
	    
	    jobjectArray jmetadata = (jobjectArray)jenv->GetObjectArrayElement(jobj, 3);
	    createCPPMetaData(jenv, jmetadata, cdoc);
	    
	    return cdoc;
    } catch (std::exception &se) {
		if(state)
			jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	    thr = createException(jenv, &se);
    }
    catch(JavaException & /* je */) {
		if(state)
			jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	    // This means there's already an exception waiting in the JVM
	    // Fall through
    }
    catch (...) {
		if(state)
			jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	    thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					      XmlException::INTERNAL_ERROR,
					      jenv->NewStringUTF("Uncaught exception from C++ API Constructing XmlDocument"), 0, 0, 0, 0);
    }
    if (thr)
	    jenv->Throw(thr);
    return XmlDocument();
}

/*
 * Class:     com_sleepycat_dbxml_dbxml_javaJNI
 * Method:    materializeMetaData
 * Signature: (Lcom/sleepycat/dbxml/XmlDocument;)V
 * Copies all the meta data in a stored document to the java document.  Used
 * by the Java function XmlDocument.fetchAllData()
 */
extern "C"
JNIEXPORT void JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_materializeMetaData
(JNIEnv *jenv, jclass jcls, jobject jdoc)
{
    if(!jdoc) return;
    jthrowable thr = 0;
    try{
	XmlDocument cdoc = createCPPXmlDocument(jenv, jdoc, false);
	if (cdoc.isNull()) return;
	if ((*(u_int64_t *)((*cdoc).getID().rawPtr()) == 0)) return;
	(*cdoc).setAsNotMaterialized();
	createJavaMetaData(jenv, jdoc, cdoc, true);
    }catch (std::exception &se) {
	thr = createException(jenv, &se);
    }
    catch(JavaException & /* je */) {
	// This means there's already an exception waiting in the JVM
	return;
    }
    catch (...) {
	thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					  XmlException::INTERNAL_ERROR,
					  jenv->NewStringUTF("Uncaught exception from C++ API"), 0, 0, 0, 0);
    }
    if (thr) {
	jenv->Throw(thr);
	return;
    }
}
%}
