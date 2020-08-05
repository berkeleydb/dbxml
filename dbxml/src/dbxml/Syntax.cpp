//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// $Id: Syntax.cpp,v 86f215ae8bc4 2008/08/20 17:42:30 john $
//

#include "DbXmlInternal.hpp"
#include <iostream>
#include "IndexSpecification.hpp"
#include "UTF8.hpp"
#include "Value.hpp"
#include "Syntax.hpp"
#include "Globals.hpp"
#include "Key.hpp"
#include "db_utils.h"
#include "nodeStore/NsFormat.hpp"
#include "nodeStore/NsUtil.hpp"

#include <xqilla/items/DatatypeLookup.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/impl/ATDateOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDateTimeOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATTimeOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATGDayOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATGMonthDayOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATGMonthOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATGYearMonthOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATGYearOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATFloatOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <xqilla/items/impl/ATDurationOrDerivedImpl.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>

using namespace DbXml;
using namespace std;

extern "C" {
int lexicographical_bt_compare(DB *db, const DBT *a, const DBT *b)
{
	UNUSED(db);

	size_t len = a->size > b->size ? b->size : a->size;
	unsigned char *p1 = (unsigned char*)a->data;
	unsigned char *p2 = (unsigned char*)b->data;
	int cmp;
	for (; len--; ++p1, ++p2) {
		cmp = (int)*p1 - (int)*p2;
		if(cmp != 0) return cmp;
	}
	return ((long)a->size - (long)b->size);
}

static int mapm_bt_compare(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	UNUSED(db);

	const xmlbyte_t *p1 = (const xmlbyte_t*)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t*)dbt2->data;

	// Calculate the end of the two keys
	const xmlbyte_t *e1 = p1 + dbt1->size;
	const xmlbyte_t *e2 = p2 + dbt2->size;

	// Compare the structure part of the key
	int res = Key::compareStructure(p1, e1, p2, e2);
	if(res != 0) return res;

	// Check to see if we've come to the end of a key
	if(p1 >= e1) {
		if(p2 >= e2) return 0;
		return -1;
	}
	if(p2 >= e2) return +1;

	return NsFormat::compareMarshaledMAPM(p1, p2);
}

static int duration_bt_compare(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	UNUSED(db);

	const xmlbyte_t *p1 = (const xmlbyte_t*)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t*)dbt2->data;

	// Calculate the end of the two keys
	const xmlbyte_t *e1 = p1 + dbt1->size;
	const xmlbyte_t *e2 = p2 + dbt2->size;

	// Compare the structure part of the key
	int res = Key::compareStructure(p1, e1, p2, e2);
	if(res != 0) return res;

	// Check to see if we've come to the end of a key
	if(p1 >= e1) {
		if(p2 >= e2) return 0;
		return -1;
	}
	if(p2 >= e2) return +1;

	// Compare months
	res = NsFormat::compareMarshaledMAPM(p1, p2);
	if(res != 0) return res;

	// Compare seconds
	return NsFormat::compareMarshaledMAPM(p1, p2);
}
};

static inline size_t marshalMAPMIntoBuffer(Buffer *buffer, const MAPM &value, Numeric::State state = Numeric::NUM)
{
	int length = NsFormat::countMAPM(value, state);
	size_t offset;
	buffer->reserve(offset, length);
	NsFormat::marshalMAPM((xmlbyte_t *)buffer->getBuffer(offset), value, state);
	return length;
}

///////////////////////////////////////////////////////////////////////////
// Syntax
const char *Syntax::getName() const
{
	
	return AtomicTypeValue::getTypeString(getType());
}

bool Syntax::hasTypeName(const char *name) const
{
	// do case-insensitive comparison
	if (NsUtil::stringEqualsIgnoreCase(name, getName()) == 0)
		return true;
	return false;
}

KeyGenerator::Ptr Syntax::getKeyGenerator(const Index &index, const char *valueBuffer, size_t valueLength, bool generateShortKeys) const
{
	if(test(valueBuffer, valueLength)) {
		return KeyGenerator::Ptr(new SingleKeyGenerator(valueBuffer, valueLength));
	}
	else if(generateShortKeys) {
		// Add a consolation key, to make presence lookups work
		return KeyGenerator::Ptr(new SingleKeyGenerator(0, 0));
	}
	else {
		return KeyGenerator::Ptr(new EmptyKeyGenerator());
	}
}

///////////////////////////////////////////////////////////////////////////
// NoneSyntax

Syntax::Type NoneSyntax::getType() const
{
	return Syntax::NONE;
}

const char * NoneSyntax::getName() const
{
	return "none";
}

bool NoneSyntax::test(const char *v, size_t len) const
{
	UNUSED(v);
	UNUSED(len);

	return true;
}

DbWrapper::bt_compare_fn NoneSyntax::get_bt_compare() const
{
	return lexicographical_bt_compare;
}

size_t NoneSyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	return buffer->write(p, l);
}

///////////////////////////////////////////////////////////////////////////
// StringSyntax

Syntax::Type StringSyntax::getType() const
{
	return Syntax::STRING;
}

bool StringSyntax::test(const char *v, size_t len) const
{
	UNUSED(v);
	UNUSED(len);

	return true;
}

DbWrapper::bt_compare_fn StringSyntax::get_bt_compare() const
{
	return lexicographical_bt_compare;
}

KeyGenerator::Ptr StringSyntax::getKeyGenerator(const Index &index, const char *valueBuffer, size_t valueLength, bool generateShortKeys) const
{
	// This is a special case. We don't need to call
	// test(), since it always returns true.

	KeyGenerator::Ptr r;
	switch (index.getKey()) {
	case Index::KEY_PRESENCE:
	case Index::KEY_EQUALITY:
		r.reset(new SingleKeyGenerator(valueBuffer, valueLength));
		break;
	case Index::KEY_SUBSTRING:
		r.reset(new SubstringKeyGenerator(valueBuffer, valueLength, generateShortKeys));
		break;
	default:
		DBXML_ASSERT(0);
		break;
	}
	return r;
}

size_t StringSyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	return buffer->write(p, l);
}

///////////////////////////////////////////////////////////////////////////
// MAPMSyntax

static const char *dbxml_nan = "NAN";
static const char *dbxml_inf = "INF";
static const char *dbxml_neginf = "-INF";

// Method to validate a numeric.
// Returns true if string is a valid double, float or decimal and
// false otherwise.   Only syntax checking is done.
// The rules for decimal are different from
// double/float so a boolean parameter distinguishes them.
//
// See XML Schema for lexical values allowed:
//    http://www.w3.org/TR/xmlschema11-2/#decimal
//    http://www.w3.org/TR/xmlschema11-2/#float
//    http://www.w3.org/TR/xmlschema11-2/#double
// Range validation is not done here.  Casts are done
// at execution time based on valid (or not) ranges
//
bool NumericSyntax::testNumeric(const char* value, size_t len,
				bool isDecimal) const
{
	NsUtil::trimWhitespace(value, len);

	if (value == NULL || len == 0)
		return false;

	size_t pos = 0;
	bool gotPoint = false;
	bool gotSign = false;
	bool gotDigit = false;
	bool stop = false;
	// gotBase is needed for float/double;
	// it will never be set true for decimal
	bool gotBase = false; 

	const char *src = value;
	char tmpChar;
	while (!stop && pos < len && *src != 0) {
		++pos;
		tmpChar = *src++;
		switch (tmpChar) {
		case L'+': {
			if (gotSign || gotDigit) {
				stop = true;
			} else {
				gotSign = true;
			}
			break;
		}
             
		case L'-': {
			if (gotSign || gotDigit) {
				stop = true;
			} else {
				gotSign = true;
			}
			break;
		}
             
		case 0x0045:
		case 0x0065: {
			if (!gotDigit || gotBase || isDecimal) {
				stop = true;
			} else {
				gotPoint = false;
				gotSign = false;        
				gotBase = true;
				gotDigit = false;
			}
			break;
		}
             
			
		case 0x002e: {  //This is '.'
			if (gotPoint || gotBase) {
				stop = true;
			} else {
				gotPoint = true;
			}
			break;
		}
             
		case 0x0030:
		case 0x0031:
		case 0x0032:
		case 0x0033:
		case 0x0034:
		case 0x0035:
		case 0x0036:
		case 0x0037:
		case 0x0038:
		case 0x0039: {
			// valid digits
			gotDigit = true;
			break;
		}
			
		default:
			stop = true;
			break;
		}
		// end switch
	} // while

	if (!gotDigit || stop) {
		if (isDecimal)
			return false;
		// check other valid strings for float/double
		// assumes white space has been trimmed by caller
		if (NsUtil::stringNEqualsIgnoreCase(value, dbxml_nan, len) == 0 ||
		    NsUtil::stringNEqualsIgnoreCase(value, dbxml_inf, len) == 0 ||
		    NsUtil::stringNEqualsIgnoreCase(value, dbxml_neginf, len) == 0)
			return true;
		return false;
	}
	if (isDecimal)
		return true;
	else {
		// Xerces-c maps overflow and underflow to
		// + or - INF respectively so don't range
		// check at this time.  If so it'd involve
		// calling strtod().
		return true;
	}
}

bool MAPMSyntax::test(const char *v, size_t len) const
{
	NsUtil::trimWhitespace(v, len);
	return getDatatypeFactory()->checkInstance(UTF8ToXMLCh(v, len).str(),
						   Globals::defaultMemoryManager);
}

SecondaryDatabase::bt_compare_fn MAPMSyntax::get_bt_compare() const
{
	return mapm_bt_compare;
}

size_t MAPMSyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	try {
		NsUtil::trimWhitespace(p, l);
		Numeric::State state = Numeric::NUM;
		MAPM value = createMAPM(p, l, timezone, state);
		return marshalMAPMIntoBuffer(buffer, value, state);
	}
	catch(XPath2TypeCastException &ex) {
		// Bad value for datatype
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////
// Base64BinarySyntax

Syntax::Type Base64BinarySyntax::getType() const
{
	return Syntax::BASE_64_BINARY;
}

bool Base64BinarySyntax::test(const char *v, size_t len) const
{
	UTF8ToXMLCh value(v, len);
	NsUtil::collapseWhitespace(value.strToModify());
	return Globals::getDatatypeLookup()->getBase64BinaryFactory()->
		checkInstance(value.str(), Globals::defaultMemoryManager);
}

SecondaryDatabase::bt_compare_fn Base64BinarySyntax::get_bt_compare() const
{
	return lexicographical_bt_compare;
}

size_t Base64BinarySyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	static const char zero = 0;

	size_t initialSize = buffer->getOccupancy();

	NsUtil::decodeBase64Binary(buffer, p, l);

	// We output a zero to distinguish a valid empty
	// value from an invalid value
	buffer->write(&zero, 1);

	return buffer->getOccupancy() - initialSize;
}

///////////////////////////////////////////////////////////////////////////
// BooleanSyntax

Syntax::Type BooleanSyntax::getType() const
{
	return Syntax::BOOLEAN;
}

DbWrapper::bt_compare_fn BooleanSyntax::get_bt_compare() const
{
	return lexicographical_bt_compare;
}

bool BooleanSyntax::test(const char *v, size_t l) const
{
	NsUtil::trimWhitespace(v, l);

	switch(*v) {
	case '0': return l == 1;
	case '1': return l == 1;
	case 't': return l == 4 && strncmp(v, "true", l) == 0;
	case 'f': return l == 5 && strncmp(v, "false", l) == 0;
	default: return false;
	}
}

size_t BooleanSyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	static const char booleanTrue = 1;
	static const char booleanFalse = 0;

	NsUtil::trimWhitespace(p, l);

	switch(*p) {
	case '1':
	case 't':
		buffer->write(&booleanTrue, 1);
		break;
	case '0':
	case 'f':
		buffer->write(&booleanFalse, 1);
		break;
	default:
		return 0;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////
// DateSyntax

Syntax::Type DateSyntax::getType() const
{
	return Syntax::DATE;
}

DatatypeFactory *DateSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getDateFactory();
}

MAPM DateSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATDateOrDerivedImpl::parseDate(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// DateTimeSyntax

Syntax::Type DateTimeSyntax::getType() const
{
	return Syntax::DATE_TIME;
}

DatatypeFactory *DateTimeSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getDateTimeFactory();
}

MAPM DateTimeSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATDateTimeOrDerivedImpl::parseDateTime(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// DaySyntax

bool DaySyntax::hasTypeName(const char *name) const
{
	if (NsUtil::stringEqualsIgnoreCase(name, "day") == 0)
		return true;
	return Syntax::hasTypeName(name);
}

Syntax::Type DaySyntax::getType() const
{
	return Syntax::DAY;
}

DatatypeFactory *DaySyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getGDayFactory();
}

MAPM DaySyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATGDayOrDerivedImpl::parseGDay(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// DecimalSyntax

Syntax::Type DecimalSyntax::getType() const
{
	return Syntax::DECIMAL;
}

bool DecimalSyntax::test(const char *v, size_t len) const
{
	return testNumeric(v, len, true);
}

DatatypeFactory *DecimalSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getDecimalFactory();
}

MAPM DecimalSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATDecimalOrDerivedImpl::parseDecimal(UTF8ToXMLCh(p, l).str());
}

///////////////////////////////////////////////////////////////////////////
// DoubleTypeSyntax

Syntax::Type DoubleSyntax::getType() const
{
	return Syntax::DOUBLE;
}

bool DoubleSyntax::test(const char *v, size_t len) const
{
	return testNumeric(v, len, false);
}

DatatypeFactory *DoubleSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getDoubleFactory();
}

MAPM DoubleSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATDoubleOrDerivedImpl::parseDouble(UTF8ToXMLCh(p, l).str(), state);
}

///////////////////////////////////////////////////////////////////////////
// DurationSyntax

Syntax::Type DurationSyntax::getType() const
{
	return Syntax::DURATION;
}

bool DurationSyntax::test(const char *v, size_t len) const
{
	NsUtil::trimWhitespace(v, len);
	return Globals::getDatatypeLookup()->getDurationFactory()->
		checkInstance(UTF8ToXMLCh(v, len).str(), Globals::defaultMemoryManager);
}

DbWrapper::bt_compare_fn DurationSyntax::get_bt_compare() const
{
	return duration_bt_compare;
}

size_t DurationSyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	NsUtil::trimWhitespace(p, l);

	MAPM months, seconds;
	ATDurationOrDerivedImpl::parseDuration(UTF8ToXMLCh(p, l).str(), months, seconds);

	size_t length = marshalMAPMIntoBuffer(buffer, months);
	length += marshalMAPMIntoBuffer(buffer, seconds);

	return length;
}

///////////////////////////////////////////////////////////////////////////
// FloatSyntax

Syntax::Type FloatSyntax::getType() const
{
	return Syntax::FLOAT;
}

bool FloatSyntax::test(const char *v, size_t len) const
{
	return testNumeric(v, len, false);
}

DatatypeFactory *FloatSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getFloatFactory();
}

MAPM FloatSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATFloatOrDerivedImpl::parseFloat(UTF8ToXMLCh(p, l).str(), state);
}

///////////////////////////////////////////////////////////////////////////
// HexBinarySyntax

Syntax::Type HexBinarySyntax::getType() const
{
	return Syntax::HEX_BINARY;
}

bool HexBinarySyntax::test(const char *v, size_t len) const
{
	NsUtil::trimWhitespace(v, len);
	return Globals::getDatatypeLookup()->getHexBinaryFactory()->
		checkInstance(UTF8ToXMLCh(v, len).str(), Globals::defaultMemoryManager);
}

SecondaryDatabase::bt_compare_fn HexBinarySyntax::get_bt_compare() const
{
	return lexicographical_bt_compare;
}

size_t HexBinarySyntax::marshal(Buffer *buffer, const char *p, size_t l, int timezone) const
{
	static const char zero = 0;

	size_t initialSize = buffer->getOccupancy();

	NsUtil::decodeHexBinary(buffer, p, l);

	// We output a zero to distinguish a valid empty
	// value from an invalid value
	buffer->write(&zero, 1);

	return buffer->getOccupancy() - initialSize;
}

///////////////////////////////////////////////////////////////////////////
// MonthSyntax

bool MonthSyntax::hasTypeName(const char *name) const
{
	if (NsUtil::stringEqualsIgnoreCase(name, "month") == 0)
		return true;
	return Syntax::hasTypeName(name);
}

Syntax::Type MonthSyntax::getType() const
{
	return Syntax::MONTH;
}

DatatypeFactory *MonthSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getGMonthFactory();
}

MAPM MonthSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATGMonthOrDerivedImpl::parseGMonth(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// MonthDaySyntax

bool MonthDaySyntax::hasTypeName(const char *name) const
{
	if (NsUtil::stringEqualsIgnoreCase(name, "monthDay") == 0)
		return true;
	return Syntax::hasTypeName(name);
}

Syntax::Type MonthDaySyntax::getType() const
{
	return Syntax::MONTH_DAY;
}

DatatypeFactory *MonthDaySyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getGMonthDayFactory();
}

MAPM MonthDaySyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATGMonthDayOrDerivedImpl::parseGMonthDay(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// TimeSyntax

Syntax::Type TimeSyntax::getType() const
{
	return Syntax::TIME;
}

DatatypeFactory *TimeSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getTimeFactory();
}

MAPM TimeSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATTimeOrDerivedImpl::parseTime(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// YearSyntax

bool YearSyntax::hasTypeName(const char *name) const
{
	if (NsUtil::stringEqualsIgnoreCase(name, "year") == 0)
		return true;
	return Syntax::hasTypeName(name);
}

Syntax::Type YearSyntax::getType() const
{
	return Syntax::YEAR;
}

DatatypeFactory *YearSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getGYearFactory();
}

MAPM YearSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATGYearOrDerivedImpl::parseGYear(UTF8ToXMLCh(p, l).str(), timezone);
}

///////////////////////////////////////////////////////////////////////////
// YearMonthSyntax

bool YearMonthSyntax::hasTypeName(const char *name) const
{
	if (NsUtil::stringEqualsIgnoreCase(name, "yearMonth") == 0)
		return true;
	return Syntax::hasTypeName(name);
}


Syntax::Type YearMonthSyntax::getType() const
{
	return Syntax::YEAR_MONTH;
}

DatatypeFactory *YearMonthSyntax::getDatatypeFactory() const
{
	return Globals::getDatatypeLookup()->getGYearMonthFactory();
}

MAPM YearMonthSyntax::createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const
{
	return ATGYearMonthOrDerivedImpl::parseGYearMonth(UTF8ToXMLCh(p, l).str(), timezone);
}

