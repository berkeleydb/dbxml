/* -*- Implements the HelperFunctions class, which is used by the Java API to do various tasks. -*- */

class HelperFunctions;

JAVA_XMLEXCEPTION(getContentAsEventWriter)

%newobject HelperFunctions::createResults(XmlTransaction*, XmlManager*) const;
%newobject HelperFunctions::getAttributes(DBT *, XmlResults *) const;
%newobject HelperFunctions::getContentAsXmlInputStream(XmlDocument &doc) const;
%newobject HelperFunctions::getContentAsEventReader(XmlDocument &doc) const;
%newobject XmlContainer::getNode(XmlTransaction txn, String name, XmlDocumentConfig config);
%newobject XmlContainer::getNode(String name);

%javamethodmodifiers HelperFunctions::getType(const char*, const char*, const char*) const "protected";
%javamethodmodifiers HelperFunctions::getTypeURI(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getTypeName(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::asString(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::asDocument(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::asEventReader(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getNodeHandle(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getNodeName(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getNodeValue(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getNamespaceURI(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getPrefix(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getLocalName(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getParentNode(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getFirstChild(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getLastChild(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getPreviousSibling(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getNextSibling(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getAttributes(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::getOwnerElement(DBT *, XmlResults*) const "protected";
%javamethodmodifiers HelperFunctions::createDocumentValue(XmlDocument&) "protected";
%javamethodmodifiers HelperFunctions::getAtomicTypeName(int, std::string) const "protected";
%javamethodmodifiers HelperFunctions::getAtomicTypeURI(int, std::string) const "protected";
%javamethodmodifiers HelperFunctions::validateString(int, std::string) const "protected";
%javamethodmodifiers HelperFunctions::getContentAsXmlInputStream(XmlDocument &doc) const "protected";
%javamethodmodifiers HelperFunctions::getContentAsEventReader(XmlDocument &doc) const "protected";
%javamethodmodifiers HelperFunctions::getContentAsEventWriter(XmlDocument &doc, XmlEventWriter &writer) const "protected";
%javamethodmodifiers HelperFunctions::getContentAsString(XmlDocument &doc) const "protected";
%javamethodmodifiers HelperFunctions::getMetaData(XmlDocument &doc) const "protected";
%javamethodmodifiers HelperFunctions::verifyDefaultCollection(std::string defaultCollection, std::string baseURI) const "protected";
%javamethodmodifiers HelperFunctions::verifyBaseURI(std::string baseURI) const "protected";

class HelperFunctions
{
    public:
	%extend {
	//Test XmlData
	static bool testXmlData() {
	XmlData data;
	
	if(!(data.get_size() == 0)) return false;
	if(!(data.get_data() == 0)) return false;
	if(!(data.getReservedSize() == 0)) return false;
	const char *str = "abc";
	const char *str2 = "def";
	const char *str3 = "abcdef";
	data.set((void*)str, 3);
	if(!(::strncmp((char *)data.get_data(), str, 3) == 0)) return false;
	if(!(data.get_size() == 3)) return false;
	if(!(data.getReservedSize() > 0)) return false;
	data.append((void*)str2, 3);
	if(!(::strncmp((char *)data.get_data(), str3, 6) == 0)) return false;
	if(!(data.get_size() == 6)) return false;
	if(!(data.getReservedSize() > 0)) return false;
	data.set((void*)str2, 3);
	if(!(::strncmp((char *)data.get_data(), str2, 3) == 0)) return false;
	if(!(data.get_size() == 3)) return false;
	if(!(data.getReservedSize() > 0)) return false;
	XmlData data2;
	data2.append((void*)str, 3);
	if(!(::strncmp((char *)data2.get_data(), str, 3) == 0)) return false;
	if(!(data2.get_size() == 3)) return false;
	if(!(data2.getReservedSize() > 0)) return false;
	XmlData data3((void*)str, 3);
	if(!(::strncmp((char *)data3.get_data(), str, 3) == 0)) return false;
	if(!(data3.get_size() == 3)) return false;
	if(!(data3.getReservedSize() > 0)) return false;
	try {
		data3.append((void*)str2,3);
		return false;
	} catch (XmlException e) {}
	XmlData data4;
	data4.reserve(6);
	if(!(data4.getReservedSize() == 6)) return false;
	data4.set((void*)str, 3);
	if(!(::strncmp((char *)data4.get_data(), str, 3) == 0)) return false;
	if(!(data4.get_size() == 3)) return false;

	XmlData data5;
	data5.reserve(3);
	void *dat = data5.get_data();
	memcpy(dat, str, 3);
	data5.set_size(3);
	if(!(::strncmp((char *)data5.get_data(), str, 3) == 0)) return false;

	XmlData data7((void*)str, 3);
	XmlData data8;
	data8.adoptBuffer(data7);
	if (data7.get_data()) return false;
	if(!(::strncmp((char *)data8.get_data(), str, 3) == 0)) return false;
	return true;
	}
	//Tests that the Java default config is the same as the C++
	static bool testConfigDefault(XmlContainerConfig javaDefault) const {
	    XmlContainerConfig cppConfig;
		if(javaDefault.getAllowCreate() != cppConfig.getAllowCreate())
			throw XmlException(XmlException::INVALID_VALUE, "Default Allow Create does not match");
		if(javaDefault.getAllowValidation() != cppConfig.getAllowValidation())
			throw XmlException(XmlException::INVALID_VALUE, "Default Allow Validation does not match");
		if(javaDefault.getChecksum() != cppConfig.getChecksum())
			throw XmlException(XmlException::INVALID_VALUE, "Default checksum does not match");
		if(::strcmp(javaDefault.getCompressionName(),cppConfig.getCompressionName()) != 0)
			throw XmlException(XmlException::INVALID_VALUE, "Default compression name does not match");
		if(javaDefault.getContainerType() != cppConfig.getContainerType())
			throw XmlException(XmlException::INVALID_VALUE, "Default container type does not match");
		if(javaDefault.getEncrypted() != cppConfig.getEncrypted())
			throw XmlException(XmlException::INVALID_VALUE, "Default encrypted does not match");
		if(javaDefault.getExclusiveCreate() != cppConfig.getExclusiveCreate())
			throw XmlException(XmlException::INVALID_VALUE, "Default exclusive create does not match");
		if(javaDefault.getIndexNodes() != cppConfig.getIndexNodes())
			throw XmlException(XmlException::INVALID_VALUE, "Default index nodes does not match");
		if(javaDefault.getMode() != cppConfig.getMode())
			throw XmlException(XmlException::INVALID_VALUE, "Default mode does not match");
		if(javaDefault.getMultiversion() != cppConfig.getMultiversion())
			throw XmlException(XmlException::INVALID_VALUE, "Default multiversion does not match");
		if(javaDefault.getNoMMap() != cppConfig.getNoMMap())
			throw XmlException(XmlException::INVALID_VALUE, "Default no mmap does not match");
		if(javaDefault.getPageSize() != cppConfig.getPageSize())
			throw XmlException(XmlException::INVALID_VALUE, "Default container type does not match");
		if(javaDefault.getReadOnly() != cppConfig.getReadOnly())
			throw XmlException(XmlException::INVALID_VALUE, "Default read only does not match");
		if(javaDefault.getReadUncommitted() != cppConfig.getReadUncommitted())
			throw XmlException(XmlException::INVALID_VALUE, "Default read uncommitted does not match");
		if(javaDefault.getSequenceIncrement() != cppConfig.getSequenceIncrement())
			throw XmlException(XmlException::INVALID_VALUE, "Default sequence increment does not match");
		if(javaDefault.getStatistics() != cppConfig.getStatistics())
			throw XmlException(XmlException::INVALID_VALUE, "Default container type does not match");
		if(javaDefault.getThreaded() != cppConfig.getThreaded())
			throw XmlException(XmlException::INVALID_VALUE, "Default threaded does not match");
		if(javaDefault.getTransactional() != cppConfig.getTransactional())
			throw XmlException(XmlException::INVALID_VALUE, "Default transactional does not match");
		if(javaDefault.getTransactionNotDurable() != cppConfig.getTransactionNotDurable())
			throw XmlException(XmlException::INVALID_VALUE, "Default transaction not durable does not match");
		return true;
	}
	//Tests that the bit operator works for XmlContainerConfig
	static bool testConfigBitOperator(XmlManager &mgr) const {
	    XmlContainerConfig config;
  	    config = (XmlContainerConfig)XmlContainer::WholedocContainer|DBXML_TRANSACTIONAL|DB_CREATE|"compressionname";
  	    if(config.getContainerType() != XmlContainer::WholedocContainer)
		throw XmlException(XmlException::INVALID_VALUE, "Bitwise operator filed to combine container type");
  	    if(!config.getTransactional())
		throw XmlException(XmlException::INVALID_VALUE, "Bitwise operator failed for transactional flag");
  	    if(!config.getAllowCreate())
		throw XmlException(XmlException::INVALID_VALUE, "Bitwise operator failed for allow create flag");
  	    if(::strcmp(config.getCompressionName(),"compressionname") != 0)
		throw XmlException(XmlException::INVALID_VALUE, "Bitwise operator failed for compression name");
    	    XmlContainer cont = mgr.openContainer("", (XmlContainerConfig)XmlContainer::WholedocContainer|DBXML_NO_STATISTICS|DB_CREATE|XmlContainerConfig::NO_COMPRESSION);
    	    if(cont.getContainerType() != XmlContainer::WholedocContainer)
		throw XmlException(XmlException::INVALID_VALUE, "Bitwise operator did not create the correct container");
		try {
			XmlContainerConfig config2(DB_TXN_NOT_DURABLE);
			return false;
		} catch (XmlException e) {}
		try {
			XmlContainerConfig config2(DB_READ_UNCOMMITTED);
			return false;
		} catch (XmlException e) {}
	    return true;
	}
	//tests that null XmlTransactions work
	static bool testNullTxn(XmlManager &mgr) const {
	    XmlTransaction txn;
	    if(!txn.isNull())
		return false;
	    XmlContainer cont = mgr.createContainer(txn, "");
	    std::string name = "test.xml";
	    std::string content = "<a>b</a>";
	    XmlUpdateContext uc = mgr.createUpdateContext();
	    cont.putDocument(txn, name, content, uc);
	    XmlDocument doc = cont.getDocument(txn, name);
	    if(doc.getName() != name)
		return false;
	    std::string content2;
	    if(doc.getContent(content2) != content)
		return false;
	    return true;
	}
	static int getType(std::string URI, std::string name, std::string value) const{
	    AtomicTypeValue val(URI, name, value);
	    return val.getType();
	}
	static std::string getTypeURI(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getTypeURI();
	}
	static std::string getTypeName(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getTypeName();
	}
	static std::string asString(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.asString();
	}
	static XmlDocument asDocument(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.asDocument();
	}
	static XmlEventReader &asEventReader(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.asEventReader();
	}
	static std::string getNodeHandle(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getNodeHandle();
	}
	static std::string getNodeName(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getNodeName();
	}
	static std::string getNodeValue(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getNodeValue();
	}
	static std::string getNamespaceURI(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getNamespaceURI();
	}
	static std::string getPrefix(DBT *data, XmlResults *result) const{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getPrefix();
	}
	static std::string getLocalName(DBT *data, XmlResults *result) const
	{
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getLocalName();
	}
	static XmlValue getParentNode(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getParentNode();
	}
	static XmlValue getFirstChild(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getFirstChild();
	}
	static XmlValue getLastChild(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getLastChild();
	}
	static XmlValue getPreviousSibling(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getPreviousSibling();
	}
	static XmlValue getNextSibling(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getNextSibling();
	}
	static XmlResults *getAttributes(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return new XmlResults(raw.getAttributes());
	}
	static XmlValue getOwnerElement(DBT *data, XmlResults *result) const {
	    XmlValue raw(RawNodeValue::create(data->data, *result));
	    return raw.getOwnerElement();
	}
	static XmlValue createDocumentValue(XmlDocument &doc) const {
	    XmlManager mgr = (*doc).getManager();
	    XmlResults result = mgr.createResults();
	    XmlValue value(doc);
	    RawNodeValue *raw = RawNodeValue::create(value, result);
		XmlValue returnValue(raw);
	    raw->setDocument(doc);
	    return returnValue;
	}
	static XmlResults createResults(XmlTransaction *txn, XmlManager *mgr) const{
	    return new ValueResults(*mgr, *txn);
	}
	static std::string getAtomicTypeName(int type, std::string v) const
	{
	    AtomicTypeValue value((XmlValue::Type)type, v);
	    return value.getTypeName();
	}
	static std::string getAtomicTypeURI(int type, std::string v) const
	{
	    AtomicTypeValue value((XmlValue::Type)type, v);
	    return value.getTypeURI();
	}
	static void validateString(int type, std::string v) const
	{
	    XmlValue value((XmlValue::Type)type, v);
	}
	static XmlInputStream *getContentAsXmlInputStream(XmlDocument &doc) const
	{
		return doc.getContentAsXmlInputStream();
	}
	static XmlEventReader &getContentAsEventReader(XmlDocument &doc) const
	{
	    return doc.getContentAsEventReader();
	}
	static void getContentAsEventWriter(XmlDocument &doc, XmlEventWriter &writer) const
	{
	    doc.getContentAsEventWriter(writer);
	}
	static std::string getContentAsString(XmlDocument &doc) const
	{
	    std::string content;
	    doc.getContent(content);
	    return content;
	}
	static XmlValue getMetaData(std::string uri, std::string name, XmlDocument &doc) const
	{
	    XmlValue value;
	    doc.getMetaData(uri, name, value);
	    return value;
	}
	static std::string verifyDefaultCollection(XmlManager mgr, std::string defaultCollection, std::string baseURI) const
	{
	    XmlQueryContext context = mgr.createQueryContext();
	    if(baseURI.size() > 0)
		context.setBaseURI(baseURI);
	    context.setDefaultCollection(defaultCollection);
	    return context.getDefaultCollection();
	}
	static void verifyBaseURI(XmlManager mgr, std::string baseURI) const
	{
	    XmlQueryContext context = mgr.createQueryContext();
	    context.setBaseURI(baseURI);
	}
    }/*extend*/
}; /*HelperFunctions*/
