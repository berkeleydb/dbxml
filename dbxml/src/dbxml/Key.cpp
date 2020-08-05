//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlValue.hpp"
#include "Key.hpp"
#include "KeyStash.hpp"
#include "Value.hpp"
#include "SyntaxManager.hpp"
#include "Name.hpp"
#include "Container.hpp"
#include "db_utils.h"
#include "ScopedDbt.hpp"
#include "Manager.hpp"

#include <string>
#include <sstream>

using namespace DbXml;
using namespace std;

Key::Key(int timezone)
	: nodeLookup_(false),
	  index_(Index::NONE),
	  id1_(0),
	  id2_(0),
	  value_(0),
	  timezone_(timezone)
{}

Key::Key(const Key&o)
	: nodeLookup_(o.nodeLookup_),
	  index_(o.index_),
	  id1_(o.id1_),
	  id2_(o.id2_),
	  value_(0),
	  timezone_(o.timezone_)
{
	setValue(o.getValue(), o.getValueSize());
}

Key &Key::operator=(const Key &o)
{
	set(o);
	return *this;
}

Key::~Key()
{
	delete value_;
}

void Key::set(const Index &i, const NameID &id1, const NameID &id2)
{
	index_ = i;
	id1_ = id1;
	id2_ = id2;
}

void Key::set(const Index &i, const NameID &id1,
	      const NameID &id2, const char *p)
{
	index_ = i;
	id1_ = id1;
	id2_ = id2;
	setValue(p, (p ? strlen(p) : 0));
}

void Key::set(const Index &i, const NameID &id1,
	      const NameID &id2, XmlValue &v)
{
	index_ = i;
	id1_ = id1;
	id2_ = id2;
	if (v.isNull()) {
		if (value_ != 0) {
			value_->reset();
		}
	} else {
		setValue(v);
	}
}

void Key::set(const Key &key)
{
	nodeLookup_ = key.nodeLookup_;
	index_ = key.index_;
	id1_ = key.id1_;
	id2_ = key.id2_;
	setValue(key.getValue(), key.getValueSize());
}

void Key::set(const Key &key, const char *p, size_t l)
{
	nodeLookup_ = key.nodeLookup_;
	index_ = key.index_;
	id1_ = key.id1_;
	id2_ = key.id2_;
	setValue(p, l);
}

void Key::set(const Key &key, const XmlValue &v)
{
	nodeLookup_ = key.nodeLookup_;
	index_ = key.index_;
	id1_ = key.id1_;
	id2_ = key.id2_;
	if (v.isNull()) {
		if (value_ != 0) {
			value_->reset();
		}
	} else {
		setValue(v);
	}
}

void Key::reset()
{
	nodeLookup_ = false;
	index_ = Index::NONE;
	id1_ = 0;
	id2_ = 0;
	if (value_ != 0) {
		value_->reset();
	}
}

const Syntax *Key::getSyntax() const
{
	return SyntaxManager::getInstance()->getSyntax(getSyntaxType());
}

size_t Key::marshal(Buffer &buffer, const char *value, size_t length) const
{
	size_t bytesWritten = 0;
	unsigned char prefix = index_.getKeyPrefix();
	bytesWritten += buffer.write(&prefix, sizeof(prefix));
	u_int32_t idlen;
	char idbuf[10];
	switch (index_.getPath()) {
	case Index::PATH_NONE:
		break;
	case Index::PATH_NODE:
		idlen = id1_.marshal(idbuf);
		bytesWritten += buffer.write(idbuf, idlen);
		break;
	case Index::PATH_EDGE:
		idlen = id1_.marshal(idbuf);
		bytesWritten += buffer.write(idbuf, idlen);
		if(!nodeLookup_) {
			idlen = id2_.marshal(idbuf);
			bytesWritten += buffer.write(idbuf, idlen);
		}
		break;
	default:
		break;
	}
	switch (index_.getKey()) {
	case Index::KEY_NONE:
		break;
	case Index::KEY_PRESENCE:
		break;
	case Index::KEY_EQUALITY:
	case Index::KEY_SUBSTRING:
 		if(value!=0) {
			bytesWritten += getSyntax()->marshal(&buffer, value, length, timezone_);
		}
		break;
	default:
		break;
	}

	return bytesWritten;
}

void Key::unmarshalStructure(Buffer &buffer)
{
	nodeLookup_ = false;
	unsigned char prefix;
	buffer.read(&prefix, sizeof(prefix));
	index_.setFromPrefix(prefix);
	u_int32_t idlen;
	switch (index_.getPath()) {
	case Index::PATH_NONE:
		break;
	case Index::PATH_NODE:
		idlen = id1_.unmarshal(buffer.getCursor());
		buffer.seek(idlen);
		break;
	case Index::PATH_EDGE:
		idlen = id1_.unmarshal(buffer.getCursor());
		buffer.seek(idlen);
		idlen = id2_.unmarshal(buffer.getCursor());
		buffer.seek(idlen);
		break;
	default:
		break;
	}
	// We don't unmarshal the value
}

void Key::setValue(const XmlValue &v)
{
	const string s(((Value*)v)->asString());
	setValue(s.c_str(), s.length());
	index_.set(((Value*)v)->getSyntaxType());
}

const char *Key::getValue() const
{
	return (value_ == 0 ? 0 : (const char *)value_->getBuffer());
}

size_t Key::getValueSize() const
{
	return (value_ == 0 ? 0 : value_->getOccupancy());
}

void Key::setValue(const char *p, size_t l)
{
	if(p && l > 0) {
		if (value_ == 0)
			value_ = new Buffer(0, 64); // Let's just assume that most keys are <64 bytes
		else
			value_->reset();
		value_->write(p, l);
	} else if(value_ != 0) {
		delete value_;
		value_ = 0;
	}
}

void Key::addValue(const char *p, size_t l)
{
	if (value_ == 0)
		value_ = new Buffer(0, 64); // Let's just assume that most keys are <64 bytes
	if (p && l > 0) {
		value_->write(p, l);
	}
}

string Key::asString() const
{
	ostringstream s;
	s << "key ";
	s << index_.asString();
	s << " ";
	switch (index_.getPath()) {
	case Index::PATH_NONE:
		break;
	case Index::PATH_NODE:
		s << "id1_=";
		s << id1_;
		s << " ";
		break;
	case Index::PATH_EDGE:
		if(!nodeLookup_) {
			s << "id2_=";
			s << id2_;
			s << " ";
		}
		s << "id1_=";
		s << id1_;
		s << " ";
		break;
	default:
		break;
	}
	switch (index_.getKey()) {
	case Index::KEY_NONE:
		break;
	case Index::KEY_PRESENCE:
		break;
	case Index::KEY_EQUALITY:
	case Index::KEY_SUBSTRING:
		if (value_ != 0) {
			s << value_->asString(false);
		}
		break;
	default:
		break;
	}
	return s.str();
}

string Key::asString_XML(OperationContext &context,
			 const Container &container) const
{
	string s;
	switch (index_.getPath()) {
	case Index::PATH_NONE:
		break;
	case Index::PATH_NODE: {
		Name n1;
		container.getDictionaryDatabase()->lookupNameFromID(context, id1_, n1);
		s += "path='";
		s += n1.asString();
		s += "' ";
	}
		break;
	case Index::PATH_EDGE: {
		s += "path='";
		if(!nodeLookup_) {
			Name n2;
			container.getDictionaryDatabase()->lookupNameFromID(context, id2_, n2);
			s += n2.asString();
			s += ".";
		}
		Name n1;
		container.getDictionaryDatabase()->lookupNameFromID(context, id1_, n1);
		s += n1.asString();
		s += "' ";
	}
		break;
	default:
		break;
	}
	switch (index_.getKey()) {
	case Index::KEY_NONE:
		break;
	case Index::KEY_PRESENCE:
		break;
	case Index::KEY_EQUALITY:
	case Index::KEY_SUBSTRING:
		if (value_ != 0 && value_->getOccupancy() != 0) {
			s += "value='";
			s += value_->asString(true);
			s += "' ";
		}
		break;
	default:
		break;
	}
	return s;
}

void Key::setDbtFromThis(DbtOut &dbt) const
{
	Buffer b(0, 64);
	marshal(b, getValue(), getValueSize());
	dbt.set(b.getBuffer(), b.getOccupancy());
}

void Key::unmarshalStructure(const DbXmlDbt &dbt)
{
	Buffer b(dbt.data, dbt.size, /*wrapper*/true); // buffer_ doesn't make a copy.
	unmarshalStructure(b);
}

void Key::setIDsFromNames(OperationContext &context, const ContainerBase &container,
			  const char *parentUriName, const char *childUriName)
{
	const_cast<ContainerBase&>(container).lookupID(
		context, childUriName, ::strlen(childUriName), id1_);
	if(parentUriName == 0 || *parentUriName == 0) {
		id2_.reset();
	} else {
		const_cast<ContainerBase&>(container).lookupID(
			context, parentUriName, ::strlen(parentUriName), id2_);
	}
}

bool Key::operator<(const Key &o) const
{
	if(index_ < o.index_) return true;
	if(o.index_ < index_) return false;
	if(id1_ < o.id1_) return true;
	if(o.id1_ < id1_) return false;
	if(nodeLookup_ < o.nodeLookup_) return true;
	if(o.nodeLookup_ < nodeLookup_) return false;

	if(nodeLookup_ && o.nodeLookup_) return false;

	if(id2_ < o.id2_) return true;
	if(o.id2_ < id2_) return false;
	if(o.value_ == 0) return false;
	if(value_ == 0) return true;
	return (*value_) < (*o.value_);
}

//static
size_t Key::structureKeyLength(const Index &index, const DbXmlDbt &dbt)
{
	if(index.getKey() == Index::KEY_SUBSTRING)
		return dbt.size;
		
	size_t n = sizeof(unsigned char); // prefix
	NameID id;
	const char *data = ((const char *)dbt.data) + n;
	switch (index.getPath()) {
	case Index::PATH_NONE:
		break;
	case Index::PATH_NODE:
		n += id.unmarshal(data);
		break;
	case Index::PATH_EDGE:
	{
		int idlen = id.unmarshal(data);
		// In case this is a prefix lookup
		if((n + idlen) < dbt.size) {
			n += id.unmarshal(data+idlen);
		}
		n += idlen;
		break;
	}
	default:
		break;
	}
	return n;
}
