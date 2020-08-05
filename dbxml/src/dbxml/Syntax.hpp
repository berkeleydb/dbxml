//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SYNTAX_HPP
#define	__SYNTAX_HPP

#include "db.h"
#include "DbWrapper.hpp"
#include "KeyGenerator.hpp"

#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/Numeric.hpp>

class DatatypeFactory;

extern "C" {
int lexicographical_bt_compare(DB *db, const DBT *a, const DBT *b);
};

namespace DbXml
{

class Index;
class Buffer;

class Syntax
{
public:
	virtual ~Syntax() {}
	enum Type
	{
		NONE,
		STRING,
		DEPRECATED_ANY_URI,
		BASE_64_BINARY,
		BOOLEAN,
		DATE,
		DATE_TIME,
		DAY,
		DECIMAL,
		DOUBLE,
		DURATION,
		FLOAT,
		HEX_BINARY,
		MONTH,
		MONTH_DAY,
		DEPRECATED_NOTATION,
		DEPRECATED_QNAME,
		TIME,
		YEAR,
		YEAR_MONTH
	};
	// first two have default implementations
	virtual bool hasTypeName(const char *name) const;
	virtual const char *getName() const;
	virtual Type getType() const = 0;
	virtual bool test(const char *v, size_t len) const = 0;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const = 0;
	virtual KeyGenerator::Ptr getKeyGenerator(
		const Index &index, const char *valueBuffer,
		size_t valueLength, bool generateShortKeys) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const = 0;
};

class NoneSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual const char *getName() const;
	virtual bool test(const char *v, size_t len) const;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

// StringSyntax derives directly from Syntax due to the special case of string
// indices
class StringSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual DbWrapper::bt_compare_fn get_bt_compare() const;
	virtual KeyGenerator::Ptr getKeyGenerator(
		const Index &index, const char *valueBuffer,
		size_t valueLength, bool generateShortKeys) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

// Common functionality for all Syntaxes that are represented as
// a single arbitrary precision decimal
class MAPMSyntax : public Syntax
{
public:
	virtual DatatypeFactory *getDatatypeFactory() const = 0;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const = 0;

	virtual bool test(const char *v, size_t len) const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

class NumericSyntax : public MAPMSyntax
{
protected:
	bool testNumeric(const char* value, size_t len,
			 bool isDecimal) const;
};
	
class Base64BinarySyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

class BooleanSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	virtual bool test(const char *v, size_t len) const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

class DateSyntax : public MAPMSyntax
{
public:
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};
	
class DateTimeSyntax : public MAPMSyntax
{
public:
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class DaySyntax : public MAPMSyntax
{
public:
	virtual bool hasTypeName(const char *name) const;
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class DecimalSyntax : public NumericSyntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class DoubleSyntax : public NumericSyntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class DurationSyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

class FloatSyntax : public NumericSyntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class HexBinarySyntax : public Syntax
{
public:
	virtual Type getType() const;
	virtual bool test(const char *v, size_t len) const;
	virtual SecondaryDatabase::bt_compare_fn get_bt_compare() const;
	virtual size_t marshal(Buffer *buffer, const char *p, size_t l, int timezone) const;
};

class MonthSyntax : public MAPMSyntax
{
public:
	virtual bool hasTypeName(const char *name) const;
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class MonthDaySyntax : public MAPMSyntax
{
public:
	virtual bool hasTypeName(const char *name) const;
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class TimeSyntax : public MAPMSyntax
{
public:
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class YearSyntax : public MAPMSyntax
{
public:
	virtual bool hasTypeName(const char *name) const;
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

class YearMonthSyntax : public MAPMSyntax
{
public:
	virtual bool hasTypeName(const char *name) const;
	virtual Type getType() const;
	virtual DatatypeFactory *getDatatypeFactory() const;
	virtual MAPM createMAPM(const char *p, size_t l, int timezone, Numeric::State &state) const;
};

}

#endif
