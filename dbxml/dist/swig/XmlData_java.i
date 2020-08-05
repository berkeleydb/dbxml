%typemap(javadestruct, methodname="delete", methodmodifiers="protected") XmlData
{}

%typemap(javacode) XmlData %{
	public byte[] get_data()
	{
		return dbxml_javaJNI.get_data(swigCPtr);
	}
%}

class XmlData
{
public:
	//Get the data and its size
	size_t get_size() const;
	
	//Sets the size of the data in the buffer
	void set_size(size_t size);

	/// Allocate a buffer of size bytes
	void reserve(size_t size);
	size_t getReservedSize() const;

	/// Take the buffer from src and give it to this XmlData
	void adoptBuffer(XmlData &src);
};
 
%extend XmlData {
	void append(DBT *data)
	{
		self->append(data->data, data->size);
	}
	
	void set(DBT *data)
	{
		self->set(data->data, data->size);
	}
}

%{
	  
	  /*
	 * Class:     com_sleepycat_dbxml_dbxml_javaJNI
	 * Method:    get_data
	 * Signature: (J)[B
	 */
	 extern "C"
	JNIEXPORT jbyteArray JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_get_1data
	  (JNIEnv *jenv, jclass jcla, jlong cptr)
	{
		if (!cptr) return jenv->NewByteArray(0);
		XmlData *self = *(XmlData **)&cptr;
		u_int32_t size = self->get_size();
		if(size >0){
			jbyteArray bytes = jenv->NewByteArray(size);
			jenv->SetByteArrayRegion(bytes, 0, size, (jbyte*)self->get_data());
			return bytes;
		}else
			return jenv->NewByteArray(0);
	}

// Not used for anything at the moment
	/*
	 * Class:    com_sleepycat_dbxml_dbxml_javaJNI
	 * Method:    set_data
	 * Signature: (J[B)V
	 */
	 extern "C"
	JNIEXPORT void JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_set_1data
	  (JNIEnv *jenv, jclass jcla, jlong cptr, jbyteArray bytes)
	{
		if (!cptr) return;
		XmlData *self = *(XmlData **)&cptr;
		if (!bytes) {
			self->set_size(0);
			return;
		}
		jbyte *jbytes = jenv->GetByteArrayElements(bytes, 0);
		if (jbytes) {
			self->set(jbytes, jenv->GetArrayLength(bytes));
			jenv->ReleaseByteArrayElements(bytes, jbytes, JNI_ABORT);
		} else
			self->set_size(0);
	}
%}
