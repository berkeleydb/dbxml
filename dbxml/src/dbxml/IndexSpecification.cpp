//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "IndexSpecification.hpp"
#include "SyntaxManager.hpp"
#include "tokenizer.hpp"
#include "Name.hpp"
#include "ScopedDbt.hpp"
#include "Value.hpp"
#include "Globals.hpp"
#include "Container.hpp"

#include <map>
#include <algorithm>

const char * DbXml::metaDataName_uri_name = "name:http://www.sleepycat.com/2002/dbxml";
const char * DbXml::metaDataName_uri_root = "root:http://www.sleepycat.com/2002/dbxml";

using namespace std;
using namespace DbXml;

#define NEE (XmlIndexSpecification::Type)(XmlIndexSpecification::PATH_NODE|XmlIndexSpecification::NODE_ELEMENT|XmlIndexSpecification::KEY_EQUALITY)
#define NAE (XmlIndexSpecification::Type)(XmlIndexSpecification::PATH_NODE|XmlIndexSpecification::NODE_ATTRIBUTE|XmlIndexSpecification::KEY_EQUALITY)

Index IndexSpecification::autoElementIndexString(NEE, XmlValue::STRING);
Index IndexSpecification::autoElementIndexDouble(NEE, XmlValue::DOUBLE);
Index IndexSpecification::autoAttrIndexString(NAE, XmlValue::STRING);
Index IndexSpecification::autoAttrIndexDouble(NAE, XmlValue::DOUBLE);
const char *IndexSpecification::autoIndexOn = "on";
const char *IndexSpecification::autoIndexOff = "off";

// Index

Index::Index()
	: index_(Index::NONE)
{
}

Index::Index(unsigned long index)
	: index_(index)
{
}

Index::Index(XmlIndexSpecification::Type type, XmlValue::Type syntax)
	: index_(type)
{
	set(AtomicTypeValue::convertToSyntaxType(syntax), SYNTAX_MASK);
}

Index::Index(const std::string &s)
	: index_(0)
{
	set(s);
}

bool Index::set(const std::string &s)
{
	bool valid = true;
	index_ = 0;
	std::string::size_type start = 0;
	std::string::size_type end = 0;
	while (end != string::npos) {
		end = s.find_first_of('-', start);
		string s2(s, start, (end == string::npos ? s.length() : end) - start);
		start = end + 1;

		map<string, unsigned long>::const_iterator i = (*Globals::indexMap).find(s2);
		if (i == (*Globals::indexMap).end()) {
			const Syntax *syntax = SyntaxManager::getInstance()->getSyntax(s2);
			if (syntax != 0) {
				index_ |= syntax->getType();
			} else {
				valid = false; // ERROR
			}
		} else {
			index_ |= i->second;

			// As a special case, automatically set "node"
			// when the user sets "metadata"
			if(i->second == Index::NODE_METADATA) {
				index_ |= Index::PATH_NODE;
			}
		}
	}
	if (valid) {
		// The indexing strategy didn't contain any unknown words, but
		// did we end up with an indexing strategy that makes sense?
		//
		valid = isValidIndex();
	}
	return valid;
}

bool Index::set(unsigned long index)
{
	if (index&INDEXER_MASK || index == INDEXER_ADD)
		index_ = (index_ & ~INDEXER_MASK) | (index & INDEXER_MASK);
	if (index&UNIQUE_MASK)
		index_ = (index_ & ~UNIQUE_MASK) | (index & UNIQUE_MASK);
	if (index&PATH_MASK)
		index_ = (index_ & ~PATH_MASK) | (index & PATH_MASK);
	if (index&NODE_MASK)
		index_ = (index_ & ~NODE_MASK) | (index & NODE_MASK);
	if (index&KEY_MASK)
		index_ = (index_ & ~KEY_MASK) | (index & KEY_MASK);
	if (index&SYNTAX_MASK)
		index_ = (index_ & ~SYNTAX_MASK) | (index & SYNTAX_MASK);
	return true;
}

bool Index::set(unsigned long index, unsigned long mask)
{
	index_ = (index_ & ~mask) | (index & mask);
	return true;
}

bool Index::equalsMask(Index::Type test, Index::Type mask) const
{
	return (test & mask) == (index_ & mask);
}

bool Index::indexerAdd() const
{
	return equalsMask(INDEXER_ADD, INDEXER_MASK);
}

bool Index::isNoneIndex() const
{
	return equalsMask(NONE, PNKS_MASK);
}

bool Index::isValidIndex() const
{
	// A valid index is:
	//
	// none
	//
	// x-element-presence-none
	// x-attribute-presence-none
	// node-metadata-presence-none
	//
	// x-element-equality-x
	// x-attribute-equality-x
	// node-metadata-equality-x
	//
	// x-element-substring-x
	// x-attribute-substring-x
	// node-metadata-substring-x
	//
	// unique-x-element-equality-x
	// unique-x-attribute-equality-x
	// unique-node-metadata-equality-x
	//
	return
		isNoneIndex() ||
		(((index_ & PATH_MASK) != 0) &&
		 ((index_ & NODE_MASK) != 0) &&
		 ((index_ & KEY_MASK) != 0) &&

		 ((equalsMask(KEY_PRESENCE, KEY_MASK) && equalsMask(SYNTAX_NONE, SYNTAX_MASK)) ||
		  (!equalsMask(KEY_PRESENCE, KEY_MASK) && !equalsMask(SYNTAX_NONE, SYNTAX_MASK))) &&

		 (equalsMask(UNIQUE_OFF, UNIQUE_MASK) || equalsMask(KEY_EQUALITY, KEY_MASK)) &&

		 ((equalsMask(NODE_METADATA, NODE_MASK) && equalsMask(PATH_NODE, PATH_MASK)) ||
		  !equalsMask(NODE_METADATA, NODE_MASK)));
}

void Index::setFromPrefix(unsigned char prefix)
{
	//
	// Uncompress the unsigned char to an unsigned long
	// Bit representation:
	//
	// I - index add/delete
	// U - unique
	// P - path
	// N - node
	// K - key
	// S - syntax
	//
	// unsigned char =                         PPNNNKKK
	// unsigned long = I--U--PP-----NNN-----KKKSSSSSSSS
	//

	index_ =
		((((unsigned long)prefix)<< 18) & PATH_MASK) |
		((((unsigned long)prefix) << 13) & NODE_MASK) |
		((((unsigned long)prefix) << 8) & KEY_MASK);
}

unsigned char Index::getKeyPrefix() const
{
	//
	// Compress the unsigned long to an unsigned char
	// Bit representation:
	//
	// I - index add/delete
	// U - unique
	// P - path
	// N - node
	// K - key
	// S - syntax
	//
	// unsigned long = I--U--PP-----NNN-----KKKSSSSSSSS
	// unsigned char =                         PPNNNKKK
	//
	return (unsigned char)
		(((index_&PATH_MASK) >> 18) |
		 ((index_&NODE_MASK) >> 13) |
		 ((index_&KEY_MASK) >> 8));
}

std::string Index::asString() const
{
	string s;
	if(isNoneIndex()) {
		s += "none";
	}
	else {
		if(getUnique() != UNIQUE_OFF) {
			s += axisAsName(getUnique());
			s += "-";
		}
		if(getPath() != PATH_NONE) {
			s += axisAsName(getPath());
			s += "-";
		}
		if(getNode() != NODE_NONE) {
			s += axisAsName(getNode());
			s += "-";
		}
		if(getKey() != KEY_NONE) {
			s += axisAsName(getKey());
			s += "-";
		}
		const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)(getSyntax()));
		s += syntax->getName();
	}
	return s;
}

std::string Index::axisAsName(Index::Type index) const
{
	Globals::NameToNumber::const_iterator i;
	for (i = (*Globals::indexMap).begin();i != (*Globals::indexMap).end();++i) {
		if (index == i->second) {
			return i->first;
		}
	}
	return "";
}

std::ostream& DbXml::operator<<(std::ostream& s, const Index &index)
{
	return s << index.asString();
}

// IndexVector

IndexVector::IndexVector()
{
}

IndexVector::IndexVector(const Name &name)
	: name_(name)
{}

IndexVector::IndexVector(const IndexVector &iv)
	: name_(iv.name_),
	  iv_(iv.iv_)
{}

IndexVector::~IndexVector()
{}

bool IndexVector::enableIndex(Index index)
{
	if(index.isValidIndex()) {
		// If the index is NONE, then remove all the other indexes first.
		if (index.equals(Index::NONE)) {
			iv_.clear();
		}
		// If the index is not already enabled, then add the index.
		if (!isEnabled((const Index::Type)index, Index::PNKS_MASK)) {
			iv_.push_back(index);
		}
		return true;
	}
	else {
		return false;
	}
}

bool IndexVector::enableIndex(const IndexVector &iv)
{
	const_iterator i;
	bool retval = false;
	for (i = iv.begin();i != iv.end();++i) {
		if (enableIndex(*i))
			retval = true;
	}
	return retval;
}

Index IndexVector::getIndex(const Index::Type &test, const Index::Type &mask) const
{
        const_iterator i;
        for(i = begin(); i != end(); ++i) {
                if(i->equalsMask(test, mask)) {
                        return *i;
                }
        }
        return 0;
}

bool IndexVector::isEnabled(const Index::Type &test, const Index::Type &mask) const
{
	const_iterator i;
	for(i = begin(); i != end(); ++i) {
		if(i->equalsMask(test, mask))
			return true;
	}
	return false;
}

bool IndexVector::isIndexed() const
{
	bool r = false;
	const_iterator i;
	for (i = begin();!r && i != end();++i) {
		r = !i->isNoneIndex() && i->isValidIndex();
	}
	return r;
}

void IndexVector::getNextIndex(int &i, Index::Type test, Index::Type mask, Index &index) const
{
	if (i > -1) {
		int size = (int)iv_.size(); // unsigned to signed
		while (i < size) {
			if (iv_[i].equalsMask(test, mask)) {
				index = iv_[i];
				++i;
				break;
			}
			++i;
		}
		if(i == size) {
			i = -1;
		}
	}
}

const Syntax *IndexVector::getNextSyntax(int &i, Index::Type test, Index::Type mask, Index &index) const
{
	const Syntax *syntax = 0;
	index = 0;
	if (i > -1) {
		getNextIndex(i, test, mask, index);
		if (!index.equals(Index::NONE)) {
			syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)index.getSyntax());
		}
	}
	return syntax;
}

IndexVector::const_iterator IndexVector::begin() const
{
	return iv_.begin();
}

IndexVector::const_iterator IndexVector::end() const
{
	return iv_.end();
}

std::string IndexVector::asString() const
{
	std::string s;
	bool first = true;
	const_iterator i;
	for (i = begin();i != end();++i) {
		if (first) {
			first = false;
		} else {
			s += " ";
		}
		if(!i->indexerAdd()) {
			s += "delete-";
		}
		s += i->asString();
	}
	return s;
}

bool IndexVector::disableIndex(const Index &index)
{
	if(index.isValidIndex()) {
		iv_.erase(remove(iv_.begin(), iv_.end(), index), iv_.end());
		return true;
	}
	else {
		return false;
	}
}

bool IndexVector::disableIndex(const IndexVector &iv)
{
	const_iterator i;
	bool retval = false;
	for (i = iv.begin();i != iv.end();++i) {
		if (disableIndex(*i))
			retval = true;
	}
	return retval;
}

void IndexVector::set(unsigned long index)
{
	Index::Vector::iterator i;
	for (i = iv_.begin();i != iv_.end();++i) {
		i->set(index);
	}
}

// IndexSpecification

IndexSpecification::IndexSpecification()
	: autoIndexed_(false), flags_(0)
{
	// check for global initialization (for XmlIndexSpecification
	// default ctor)
	if (!Globals::isInitialized()) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Cannot construct XmlIndexSpecification instance without an active XmlManager object to initialize BDB XML");
	}

	// Always index the document name
	enableIndex(DbXml::metaDataName_uri_name, "unique-metadata-equality-string");
}

IndexSpecification::IndexSpecification(const IndexSpecification &is)
	: defaultIndex_(is.defaultIndex_), autoIndexed_(is.autoIndexed_), flags_(0)
{
	IndexMap::const_iterator i;
	for (i = is.indexMap_.begin(); i != is.indexMap_.end(); ++i) {
		indexMap_[::strdup(i->first)] = new IndexVector(*i->second);
	}
}

IndexSpecification::~IndexSpecification()
{
	clear();
}

void IndexSpecification::addIndex(const std::string &uri, const std::string &name, const Index &index)
{
	setSpecModified();
	enableIndex(Name(uri, name).getURIName().c_str(), index);
}

void IndexSpecification::addIndex(const std::string &uri, const std::string &name, const std::string &index)
{
	setSpecModified();
	enableIndex(Name(uri, name).getURIName().c_str(), index);
}

void IndexSpecification::deleteIndex(const std::string &uri, const std::string &name, const Index &index)
{
	setSpecModified();
	disableIndex(Name(uri, name).getURIName().c_str(), index);
}

void IndexSpecification::deleteIndex(const std::string &uri, const std::string &name, const std::string &index)
{
	setSpecModified();
	disableIndex(Name(uri, name).getURIName().c_str(), index);
}

void IndexSpecification::addDefaultIndex(const Index &index)
{
	setSpecModified();
	enableIndex(0, index);
}

void IndexSpecification::addDefaultIndex(const std::string &index)
{
	setSpecModified();
	enableIndex(0, index);
}

void IndexSpecification::deleteDefaultIndex(const Index &index)
{
	setSpecModified();
	disableIndex(0, index);
}

void IndexSpecification::deleteDefaultIndex(const std::string &index)
{
	setSpecModified();
	disableIndex(0, index);
}

const IndexVector *IndexSpecification::getIndexOrDefault(const char *uriname) const
{
	IndexMap::const_iterator i = indexMap_.find(uriname);
	if (i != indexMap_.end()) {
		return i->second;
	}
	return &defaultIndex_;
}

// convenience method to see if there are content indexes (element and/
// or attribute) in the specification
bool IndexSpecification::isContentIndexed() const
{
	return (isIndexed(Index::NODE_ATTRIBUTE, Index::NODE_MASK) ||
		isIndexed(Index::NODE_ELEMENT, Index::NODE_MASK));
}

// return true if there is a default index on metdata
bool IndexSpecification::isMetaDataDefaultIndex() const
{
	return defaultIndex_.isEnabled(Index::NODE_METADATA, Index::NODE_MASK);
}

bool IndexSpecification::isIndexed(Index::Type test, Index::Type mask) const
{
	if(defaultIndex_.isEnabled(test, mask)) return true;

	IndexMap::const_iterator i;
	for (i = indexMap_.begin(); i != indexMap_.end(); ++i) {
		if(i->second->isEnabled(test, mask)) return true;
	}
	return false;
}

const IndexSpecification::NameList IndexSpecification::getIndexed(Index::Type test, Index::Type mask) const
{
	NameList result;
	IndexMap::const_iterator i;
	for(i = indexMap_.begin(); i != indexMap_.end(); ++i) {
		if(i->second->isEnabled(test, mask)) {
			result.push_back(i->second->getNameObject());
		}
	}
	return result;
}

void IndexSpecification::enableIndex(const char *uriname, const Index &index)
{
	IndexVector *iv = &defaultIndex_;
	// null uriname is default index
	// check for bad name (empty string)
	if (uriname && !*uriname)
		throw XmlException(XmlException::INVALID_VALUE, "Illegal index name (empty string)");
	if(uriname != 0) {
		IndexMap::iterator i = indexMap_.find(uriname);
		if (i != indexMap_.end()) {
			iv = i->second;
		} else {
			iv = new IndexVector(uriname);
			indexMap_[::strdup(uriname)] = iv;
		}
	}
	if(!iv->enableIndex(index)) {
		if(uriname != 0)
			throw XmlException(XmlException::UNKNOWN_INDEX, "Unknown index specification, '" + index.asString() + "', for node '" + uriname + "'.");
		else throw XmlException(XmlException::UNKNOWN_INDEX, "Unknown index specification, '" + index.asString() + "', for default index.");
	}
	buffer_.reset();
}

void IndexSpecification::enableIndex(const char *uriname, const std::string &indexString)
{
	// Enable the index strategy.
	tokenizer tok(indexString, ", ");
	std::string indexspec;
	Index index;
	while(tok.next(indexspec) == 0) {
		index.set(indexspec);
		enableIndex(uriname, index);
	}
}

void IndexSpecification::enableIndex(const char *uriname, const IndexVector &iv)
{
	// check for bad name (empty string)
	if (uriname && !*uriname)
		throw XmlException(XmlException::INVALID_VALUE, "Illegal index name (empty string)");

	IndexMap::iterator i = indexMap_.find(uriname);
	if (i != indexMap_.end()) {
		i->second->enableIndex(iv);
	}
	else {
		IndexVector *new_iv = new IndexVector(uriname);
		indexMap_[::strdup(uriname)] = new_iv;
		new_iv->enableIndex(iv);
	}
	buffer_.reset();
}

bool IndexSpecification::enableIndex(const IndexSpecification &is)
{
	IndexMap::const_iterator i;
	for (i = is.indexMap_.begin(); i != is.indexMap_.end(); ++i) {
		enableIndex(i->first, *i->second);
	}
	bool retval = defaultIndex_.enableIndex(is.defaultIndex_);
	buffer_.reset();
	return retval;
}

void IndexSpecification::disableIndex(const char *uriname, const Index &index)
{
	if(uriname == 0) {
		if(!defaultIndex_.disableIndex(index)) {
			throw XmlException(XmlException::UNKNOWN_INDEX, "Unknown index specification, '" + index.asString() + "', for default index.");
		}
		buffer_.reset();
		return;
	}

	if(::strcmp(uriname, metaDataName_uri_name) == 0 &&
	   index.equalsMask((Index::Type)(Index::NME | Syntax::STRING), Index::PNKS_MASK)) {
		throw XmlException(XmlException::INVALID_VALUE, "You cannot disable the built in index: '" + index.asString() + "', for node '" + uriname + "'.");
	}

	IndexMap::iterator i = indexMap_.find(uriname);
	if (i != indexMap_.end()) {
		if(!i->second->disableIndex(index)) {
			throw XmlException(XmlException::UNKNOWN_INDEX, "Unknown index specification, '" + index.asString() + "', for node '" + uriname + "'.");
		}
		if(!i->second->isIndexed()) {
			::free((void*)i->first);
			delete i->second;
			indexMap_.erase(i);
		}
		buffer_.reset();
	}
}

void IndexSpecification::disableIndex(const char *uriname, const std::string &indexString)
{
	// Enable the index strategy.
	tokenizer tok(indexString, ", ");
	std::string indexspec;
	Index index;
	while(tok.next(indexspec) == 0) {
		index.set(indexspec);
		disableIndex(uriname, index);
	}
}

void IndexSpecification::disableIndex(const char *uriname, const IndexVector &iv)
{
	IndexMap::iterator i = indexMap_.find(uriname);
	if (i != indexMap_.end()) {
		i->second->disableIndex(iv);
	}
	else {
		// Add a new IndexVector, with the values from the
		// default index, and then disable the values from
		// the other IndexVector
		IndexVector *new_iv = new IndexVector(uriname);
		indexMap_[::strdup(uriname)] = new_iv;
		new_iv->enableIndex(defaultIndex_);
		new_iv->disableIndex(iv);
	}
	buffer_.reset();
}

bool IndexSpecification::disableIndex(const IndexSpecification &is)
{
	IndexMap::const_iterator i;
	IndexMap::const_iterator end = is.indexMap_.end();
	for(i = is.indexMap_.begin(); i != end; ++i) {
		disableIndex(i->first, *i->second);
	}
	// Find all our urinames that don't appear in the other
	// IndexSpecification, and disable the default index from
	// them.
	IndexMap::const_iterator end2 = indexMap_.end();
	for(i = indexMap_.begin(); i != end2; ++i) {
		if(is.indexMap_.find(i->first) == end) {
			i->second->disableIndex(is.defaultIndex_);
		}
	}
	bool retval = defaultIndex_.disableIndex(is.defaultIndex_);
	buffer_.reset();
	return retval;
}

void IndexSpecification::set(unsigned long index)
{
	IndexMap::iterator i;
	for (i = indexMap_.begin(); i != indexMap_.end(); ++i) {
		i->second->set(index);
	}
	defaultIndex_.set(index);
	buffer_.reset();
}

void IndexSpecification::clear()
{
	IndexMap::iterator i;
	for (i = indexMap_.begin();i != indexMap_.end();++i) {
		::free((void*)i->first);
		delete i->second;
	}
	indexMap_.clear();
	defaultIndex_.clear();
	buffer_.reset();
	autoIndexed_ = false;
	flags_ = 0;
}

bool IndexSpecification::find(const std::string &uri, const std::string &name, std::string &index)
{
	bool r = false;
	Name n(uri, name);
	string s(n.getURIName());
	IndexMap::iterator i = indexMap_.find(s.c_str());
	if (i != indexMap_.end() && i->second->isIndexed()) {
		index = i->second->asString();
		r = true;
	}
	return r;
}

int IndexSpecification::read(const ConfigurationDatabase *config, Transaction *txn, bool lock)
{
	// We read the indexing specification into tmpBuffer_, and
	// if it's different than the cached indexing specification
	// stored in buffer_, then we parse tmpBuffer_ and copy tmpBuffer_
	// into buffer_ for that next time around.
	//
	static const char *key = "index";
	static size_t keyLength = strlen(key) + 1;
	tmpBuffer_.reset();
	int err = config->getConfigurationItem(txn, key, keyLength, tmpBuffer_, lock);
	if (err == 0) {
		if (buffer_ != tmpBuffer_) {
			clear();
			const char *p = (const char *)tmpBuffer_.getBuffer();
			if(p != 0) {
				// Default index
				const char *index = p;
				p += strlen(p) + 1;
				enableIndex(0, index);
			}
			while (p != 0 && *p != '\0') {
				const char *uriname = p;
				p += strlen(p) + 1;
				const char *index = p;
				p += strlen(p) + 1;
				enableIndex(uriname, index);
			}
			buffer_ = tmpBuffer_;
		}
	} else if (err == DB_NOTFOUND) {
		err = 0;
	}
	if (err == 0)
		autoIndexed_ = readAutoIndex(config, txn, false /*not writing auto-index*/);
	return err;
}

// auto-indexing APIs

void IndexSpecification::enableAutoElementIndexes(const char *uriname)
{
	// set auto-indexing indexes for elements for uriname
	enableIndex(uriname, autoElementIndexString);
	enableIndex(uriname, autoElementIndexDouble);
}

void IndexSpecification::enableAutoAttrIndexes(const char *uriname)
{
	// set auto-indexing indexes for attributess for uriname
	enableIndex(uriname, autoAttrIndexString);
	enableIndex(uriname, autoAttrIndexDouble);
}

// static
bool IndexSpecification::readAutoIndex(const ConfigurationDatabase *config, Transaction *txn, bool lock)
{
	Buffer buf;
	static const char *autoIndexKey = "autoindex";
	static size_t autoIndexKeyLength = strlen(autoIndexKey) + 1;
	int err = config->getConfigurationItem(txn, autoIndexKey, autoIndexKeyLength,
					       buf, lock);
	if (err == 0) {
		const char *p = (const char *)buf.getBuffer();
		if (p && (::strcmp(p, autoIndexOn) == 0))
			return true;
	}
	return false;
}

// static
int IndexSpecification::setAutoIndex(ConfigurationDatabase *config, Transaction *txn,
				     bool value)
{
	// create buffer wrapper around value
	const char *val = (value ? autoIndexOn : autoIndexOff);
	Buffer buf(val, ::strlen(val) + 1, true);
	return config->putConfigurationItem(txn, "autoindex", buf);
}


int IndexSpecification::upgrade(DbWrapper &config_db, int old_version, int current_version)
{
	int err = 0;
	if (old_version < VERSION_23) {
		// Need to remove indices of type anyURI, QName, NOTATION,
		// replacing them with equivalent string indices
		static const char *keyValue = "index";
		static size_t keyLength = strlen(keyValue) + 1;

		DbtIn key((void*)keyValue, keyLength);
		DbtOut value;
		err = config_db.get(0, &key, &value, 0);

		clear();
		if (err == 0) {
			const char *p = (const char *)value.data;
			if(p != 0) {
				// Default index
				const char *index = p;
				p += strlen(p) + 1;
				upgradeEnableIndex(0, index);
			}
			while (p != 0 && *p != '\0') {
				const char *uriname = p;
				p += strlen(p) + 1;
				const char *index = p;
				p += strlen(p) + 1;
				upgradeEnableIndex(uriname, index);
			}
		}
		else if (err == DB_NOTFOUND) {
			err = 0;
		}

		if(err == 0) {
			writeToBuffer(buffer_);
			DbtIn new_value(buffer_.getBuffer(), buffer_.getOccupancy());
			err = config_db.put(0, &key, &new_value, 0);
		}
	}
	return err;
}

void IndexSpecification::upgradeEnableIndex(const char *uriname, const std::string &indexString)
{
	// Enable the index strategy.
	tokenizer tok(indexString, ", ");
	std::string indexspec;
	Index index;
	while(tok.next(indexspec) == 0) {
		if(!index.set(indexspec) &&
			index.equalsMask(Index::KEY_EQUALITY, Index::KEY_MASK) &&
			index.equalsMask(Index::SYNTAX_NONE, Index::SYNTAX_MASK)) {
			// Need to remove indices of type anyURI, QName, NOTATION,
			// replacing them with equivalent string indices
			if(indexspec.find(AtomicTypeValue::getValueTypeString(XmlValue::ANY_URI)) != string::npos) {
				index.set(Syntax::STRING, Index::SYNTAX_MASK);
			}
			else if(indexspec.find(AtomicTypeValue::getValueTypeString(XmlValue::QNAME)) != string::npos) {
				index.set(Syntax::STRING, Index::SYNTAX_MASK);
			}
			else if(indexspec.find(AtomicTypeValue::getValueTypeString(XmlValue::NOTATION)) != string::npos) {
				index.set(Syntax::STRING, Index::SYNTAX_MASK);
			}
		}
		enableIndex(uriname, index);
	}
}


int IndexSpecification::write(ConfigurationDatabase *config, Transaction *txn,
			      Buffer *buffer) const
{
	if (!buffer) {
		writeToBuffer(buffer_);
		buffer = &buffer_;
	}
	return config->putConfigurationItem(txn, "index", *buffer);
}

void IndexSpecification::writeToBuffer(Buffer &buffer) const
{
	if (buffer.getOccupancy() == 0) {
		string index = defaultIndex_.asString();
		buffer.write(index.c_str(), index.length() + 1);

		const char *uriname;
		IndexMap::const_iterator i;
		for (i = indexMap_.begin(); i != indexMap_.end(); ++i) {
			if(i->second->isIndexed()) {
				uriname = i->first;
				index = i->second->asString();
				buffer.write(uriname, strlen(uriname) + 1);
				buffer.write(index.c_str(), index.length() + 1);
			}
		}
		char null = '\0';
		buffer.write(&null, sizeof(null));
	}
}

std::string IndexSpecification::asString() const
{
	string r;
	r += "default: ";
	r += defaultIndex_.asString();
	r += " ";
	IndexMap::const_iterator i;
	for (i = indexMap_.begin();i != indexMap_.end();++i) {
		if(i->second->isIndexed()) {
			r += i->first;
			r += "=>";
			r += i->second->asString();
			r += " ";
		}
	}
	return r;
}

IndexSpecificationIterator::IndexSpecificationIterator(const IndexSpecification &is)
	: is_(is),
	  isi_(),
	  iv_(0),
	  ivi_()
{
	reset();
}

bool IndexSpecificationIterator::next(std::string &uri, std::string &name, XmlIndexSpecification::Type &type, XmlValue::Type &syntax)
{
	bool r = false;
	while (!r && iv_) {
		if(iv_->isIndexed() && ivi_ != iv_->end()) {
			Name n(isi_->first);
			uri = n.getURI();
			name = n.getName();

			type = (XmlIndexSpecification::Type)(ivi_->get() & Index::PNK_MASK);
			syntax = AtomicTypeValue::convertToValueType((Syntax::Type)ivi_->getSyntax());

			++ivi_;

			r = true;
		}
		else {
			++isi_;
			setVectorFromSpecIterator();
		}
	}
	if (!r) {
		uri.erase();
		name.erase();
		type = (XmlIndexSpecification::Type)0;
		syntax = XmlValue::NONE;
	}
	return r;
}

void IndexSpecificationIterator::setVectorFromSpecIterator()
{
	if(isi_ == is_.end()) {
		iv_ = 0;
	}
	else {
		iv_ = isi_->second;
		ivi_ = iv_->begin();
	}
}

bool IndexSpecificationIterator::next(std::string &uri, std::string &name, std::string &index)
{
	bool r = false;
	while (!r && iv_) {
		if (iv_->isIndexed()) {
			Name n(isi_->first);
			uri = n.getURI();
			name = n.getName();
			index = iv_->asString();
			r = true;
		}
		++isi_;
		setVectorFromSpecIterator();
	}
	if (!r) {
		uri.erase();
		name.erase();
		index.erase();
	}
	return r;
}

void IndexSpecificationIterator::reset()
{
	isi_ = is_.begin();
	setVectorFromSpecIterator();
}
