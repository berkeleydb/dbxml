/* -*- Implementation of the XmlValue class for Java -*- */

/* XmlValue is passed to C++ and back to Java as a jobject.  The object 
is translated between the langagues using createCPPXmlValue and createJavaXmlValue. */
JAVA_TYPEMAP(XmlValue &, XmlValue, jobject)
JAVA_TYPEMAP(XmlValue, XmlValue, jobject)
%typemap(javadirectorin) XmlValue &reslt "reslt"


/* A very ugly hack used so that XmlResolver.resolveDocument passes the resolved
 * document stored in XmlValue to the C++ from the Java.  If the typemap
 * directorargout is ever implemented for Java, use that to implement this. 
 * IMPORTANT - whenver you change swig version or add a new director class make
 * sure the index used in Swig::director_methids below matches the one generated
 * by swig, if they do not match then this function will call the wrong Java function.
 */
%typemap(directorin,descriptor="Lcom/sleepycat/dbxml/XmlValue;") (XmlValue &reslt)
%{
$input = jenv->NewObject(xml_valuedecl_class, xml_valuenull_construct);
#if 1
jresult = (jboolean) jenv->CallStaticBooleanMethod(Swig::jclass_dbxml_javaJNI, Swig::director_methids[2], swigjobj, jtxn, jmgr, juri, jreslt);
#else
jresult = (jboolean) jenv->CallStaticBooleanMethod(Swig::jclass_dbxml_javaJNI, Swig::director_methids[2], jobj, jtxn, jmgr, juri, jreslt);
#endif
if (jenv->ExceptionOccurred()) return c_result;
c_result = jresult ? true : false; 
} else {
	SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null upcall object");
}
#if 1
if (swigjobj) jenv->DeleteLocalRef(swigjobj);
#else
if (jobj) jenv->DeleteLocalRef(jobj);
#endif
jclass cls = jenv->GetObjectClass($input);
jmethodID mid = jenv->GetMethodID(cls, "asDocument", "()Lcom/sleepycat/dbxml/XmlDocument;");
jobject jdoc = jenv->CallObjectMethod($input, mid);
if (jdoc) {
    XmlDocument cdoc = createCPPXmlDocument(jenv, jdoc);
    XmlValue val(cdoc);
    $1 = val;
}
if ($input) jenv->DeleteLocalRef($input);
return c_result;
if (true) {
%}

//Turns the Java XmlValue into a C++ XmlValue, and returns early if the translation fails
%typemap(in) XmlValue & %{
    bool valid = true;
	XmlValue val$argnum = createCPPXmlValue(jenv, $input, &valid);
	if (!valid) return $null;
	$1 = &val$argnum;
%}

//Turns the C++ XmlValue object return by the function into a Java object
%typemap(out) const XmlValue *,  XmlValue *{
    if($1 == NULL)
	    $result = NULL;
    else
	    $result = createJavaXmlValue(jenv, *$1);		    
}

%typemap(out) XmlValue &, XmlValue{
    $result = createJavaXmlValue(jenv, $1);
}

%typemap(out) const XmlValue &{
    $result = createJavaXmlValue(jenv, *$1);	
}

%{
//Translate a Java XmlValue to a C++ XmlValue when it is passed as an argument
//See XmlValue.java the function pack() to get the makeup of the object array
XmlValue createCPPXmlValue(JNIEnv *jenv, jobject jvalue, bool *valid = 0)
{
bool createdResults = false;
XmlResults xmlresult;
jlong resultPtr = 0;
jthrowable thr = NULL;
jbyte *bytes = 0;
jlong* state = 0;
jbyteArray value = 0;
jlongArray jstate = 0;
if(!jvalue) return XmlValue();
try {
	//Call pack() to get all the data of the value
	jclass cls = jenv->GetObjectClass(jvalue);
	jmethodID mid = jenv->GetMethodID(cls, "pack", "()[Ljava/lang/Object;");
	jobjectArray jdata = (jobjectArray)jenv->CallObjectMethod(jvalue, mid);
	if(!jdata) return XmlValue();
	
	//Get the type of the XmlValue
	jintArray jtypeArray = (jintArray)jenv->GetObjectArrayElement(jdata, 0);
	jint* jtype = jenv->GetIntArrayElements(jtypeArray, (jboolean *)0);
	int type = jtype[0];
	jenv->ReleaseIntArrayElements(jtypeArray, jtype, 0);
	if(type == XmlValue::NODE){
		/*If it is a node get the data held in the ID
		  and get the XmlResults object either from the
		  Java object or construct it */
		value = (jbyteArray)jenv->GetObjectArrayElement(jdata, 1);
		bytes = jenv->GetByteArrayElements(value, (jboolean *)0);
		jstate = (jlongArray)jenv->GetObjectArrayElement(jdata, 3);
		state = jenv->GetLongArrayElements(jstate, (jboolean *)0);
		resultPtr = state[0];
		if(resultPtr != 0)
			xmlresult = *(*(XmlResults **)&resultPtr);
		else{
			/* If the XmlResults object does not exist in the Java
			   then construct it from the XmlManager */
			createdResults = true;
			jlong mgrPtr = state[1];
			if (mgrPtr != 0){
				XmlManager *mgr = *(XmlManager **)&mgrPtr;
				jlong txnPtr = state[2];
				if(txnPtr != 0){
					XmlTransaction *txn = *(XmlTransaction **)&txnPtr;
					xmlresult = new ValueResults(*mgr, *txn);
				}else
					xmlresult = mgr->createResults();
			}else{
		                jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
		                state = 0;
				XmlException e(XmlException::INVALID_VALUE, "The result set for this value has been closed, cannot access value.");
				throw e;
			}
		}
		RawNodeValue *raw = RawNodeValue::create((void *)bytes, xmlresult);
		XmlValue returnValue(raw);
		jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
		state = 0;
		jenv->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
		bytes = 0;
		/*If the results where not created for this call then that
		  information should be recorded so that if the value is created
		  again in Java using the XmlResults.next/previous/peek function
		  then it will be set the its original XmlResults Java object */
		if(!createdResults) {
		    XmlResults *res = *(XmlResults **)&resultPtr;
		    raw->setReturnResults(res);
		}
		jobject jdoc = jenv->GetObjectArrayElement(jdata, 2);
		if(jdoc){
			XmlDocument cdoc = createCPPXmlDocument(jenv, jdoc);
			raw->setDocument(cdoc);
		}
		return returnValue;
	}else if(type == XmlValue::BINARY){
		jbyteArray value = (jbyteArray)jenv->GetObjectArrayElement(jdata, 1);
		if (value) {
			bytes = jenv->GetByteArrayElements(value, (jboolean *)0);
			if (bytes) {
				XmlData data((void *)bytes, jenv->GetArrayLength(value));
				XmlValue returnValue((XmlValue::Type)type, data);
				jenv->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
				bytes = 0;
				return returnValue;
			}
		}
		XmlData data;
		XmlValue returnValue((XmlValue::Type)type, data);
		return returnValue;
	}else if(type == XmlValue::NONE){
		return XmlValue();
	}else{
		std::string stringValue;
		const char *str = 0;
		jstring value = (jstring)jenv->GetObjectArrayElement(jdata, 1);
		if (value) {
			str = jenv->GetStringUTFChars(value, (jboolean *)0);
			if (str) {
				stringValue = str;
				jenv->ReleaseStringUTFChars(value, str);
			}
		}
		//type name and type value are explicitly declared
		if(jenv->GetArrayLength(jdata) > 2) {
			std::string stringName;
			std::string stringURI;
			value = (jstring)jenv->GetObjectArrayElement(jdata, 2);
			if (value) {
				str = jenv->GetStringUTFChars(value, (jboolean *)0);
				if (str) {
					stringName = str;
					jenv->ReleaseStringUTFChars(value, str);
				}
			}
			value = (jstring)jenv->GetObjectArrayElement(jdata, 3);
			if (value) {
				str = jenv->GetStringUTFChars(value, (jboolean *)0);
				if (str) {
					stringURI = str;
					jenv->ReleaseStringUTFChars(value, str);
				}
			}
			return XmlValue(new AtomicTypeValue((XmlValue::Type)type, stringURI,
				stringName, stringValue));
		} else 
			return XmlValue((XmlValue::Type)type, stringValue);
	}
}catch (std::exception &se) {
	if(bytes) jenv->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
	if(state) jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	thr = createException(jenv, &se);
}
catch(JavaException & /* je */) {
	// This means there's already an exception waiting in the JVM
	if(bytes) jenv->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
	if(state) jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	XmlQueryContext nullContext;
	if (valid)
		*valid = false;
	return XmlValue();
}
catch (...) {
	if(bytes) jenv->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
	if(state) jenv->ReleaseLongArrayElements(jstate, state, JNI_ABORT);
	thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					  XmlException::INTERNAL_ERROR,
					  jenv->NewStringUTF("Uncaught exception from C++ API Constructing XmlValue"), 0, 0, 0, 0);
}
if (thr) {
	XmlQueryContext nullContext;
	jenv->Throw(thr);
}
if (valid)
	*valid = false;
return XmlValue(); // empty return
}

//Translate a C++ XmlValue into a java XmlValue when it is returned from a function
jobject createJavaXmlValue(JNIEnv *jenv, const XmlValue &xmlvalue)
{
jthrowable thr = NULL;
jobjectArray jvalue = 0;
jint jtype = xmlvalue.getType();
try {
	if(xmlvalue.isNode()){
		/* If the XmlValue is a node then create a Java XmlValue using
		   the type, node type, and ID. */
		RawNodeValue *node = const_cast<RawNodeValue*>((*xmlvalue).getRawNodeValueObject());
		if(!node){
			return 0;
		}else{
			jvalue = jenv->NewObjectArray(4, jenv->FindClass("Ljava/lang/Object;"), NULL);
			//Set the type in jvalue[0]
			jintArray types = jenv->NewIntArray(1);
			jint nodeType = xmlvalue.getNodeType();
			jenv->SetIntArrayRegion(types, 0, 1, &nodeType);
			jenv->SetObjectArrayElement(jvalue, 0, types);
			//Set the ID in jvalue[1]
			jbyte *data = 0;
			int size = node->dumpValue((void *&)data);
			jbyteArray dataBytes = jenv->NewByteArray(size);
			jenv->SetByteArrayRegion(dataBytes, 0, jenv->GetArrayLength(dataBytes), data);
			jenv->SetObjectArrayElement(jvalue, 1, dataBytes);
			/* If the new XmlValue was created from another Java XmlValue that had
			   an XmlResult, then set the new value's results to that of the old values */
			if(node->getReturnResults())
			{
				jlong resultsPtr = 0;
				*(XmlResults **)&resultsPtr = node->getReturnResults();
				jlongArray state = jenv->NewLongArray(4);
				jenv->SetLongArrayRegion(state, 3, 1, &resultsPtr);
				jenv->SetObjectArrayElement(jvalue, 3, state);
			}
			if(node->getNodeType() == XmlValue::DOCUMENT_NODE){
				XmlDocument cdoc = node->asDocument();
				jobject jdoc = createJavaXmlDocument(jenv, cdoc);
				jenv->SetObjectArrayElement(jvalue, 2, jdoc);
			}
		}
	}else if (xmlvalue.isNull()) {
		//Nothing to do
	}else{
		//Can be a binary or a string value
		try{
			std::string value = xmlvalue.asString();
			AtomicTypeValue atomic(xmlvalue.getType(), value);
			if(atomic.getTypeName() == xmlvalue.getTypeName()) 
				jvalue = jenv->NewObjectArray(1, jenv->FindClass("Ljava/lang/Object;"), NULL);
			else {
				jvalue = jenv->NewObjectArray(3, jenv->FindClass("Ljava/lang/Object;"), NULL);
				jenv->SetObjectArrayElement(jvalue, 1, 
					jenv->NewStringUTF(xmlvalue.getTypeName().c_str()));
				jenv->SetObjectArrayElement(jvalue, 2, 
					jenv->NewStringUTF(xmlvalue.getTypeURI().c_str()));
			}
			jenv->SetObjectArrayElement(jvalue, 0, jenv->NewStringUTF(value.c_str()));
		}catch(...){}
		if(!jvalue){
			try{
				jvalue = jenv->NewObjectArray(2, jenv->FindClass("Ljava/lang/Object;"), NULL);
				//Set the types in jvalue[0]
				XmlData value = xmlvalue.asBinary();
				jbyteArray valueBytes = jenv->NewByteArray(value.get_size());
				jbyte *valueData = static_cast<jbyte*>(value.get_data());
				jenv->SetByteArrayRegion(valueBytes, 0, jenv->GetArrayLength(valueBytes), valueData);
				jenv->SetObjectArrayElement(jvalue, 0, valueBytes);
			}catch(...){
				XmlException e(XmlException::INTERNAL_ERROR, "Uncaught exception from C++ API");
				throw e;
			}
		}
	}
	return jenv->NewObject(xml_valuedecl_class, xml_value_construct,
						 jtype, jvalue);
} catch (std::exception &se) {
	thr = createException(jenv, &se);
}
catch(JavaException & /* je */) {
	// This means there's already an exception waiting in the JVM
}
catch (...) {
	thr = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
					  XmlException::INTERNAL_ERROR,
					  jenv->NewStringUTF("Uncaught exception from C++ API Constructing Java XmlValue"), 0, 0, 0, 0);
}
if (thr) {
	jenv->Throw(thr);
}
return 0;
}
%}
