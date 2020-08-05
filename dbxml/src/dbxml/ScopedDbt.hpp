//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SCOPEDDBT_HPP
#define	__SCOPEDDBT_HPP

#include "DbXmlInternal.hpp"
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "SharedPtr.hpp"

namespace DbXml
{
	
class DbXmlDbt : public DBT {
public:
	// Construction
	DbXmlDbt() {
		DBT *dbt = this;
		memset(dbt, 0, sizeof(DBT));
	}

	DbXmlDbt(void *data_arg, u_int32_t size_arg) {
		DBT *dbt = this;
		memset(dbt, 0, sizeof(DBT));
		data = data_arg;
		size = size_arg;
	}

	virtual ~DbXmlDbt() {}

	DbXmlDbt(const DbXmlDbt &that) {
		const DBT *from = &that;
		DBT *to = this;
		memcpy(to, from, sizeof(DBT));
	}

	DbXmlDbt &operator = (const DbXmlDbt &that) {
		if (this != &that) {
			const DBT *from = &that;
			DBT *to = this;
			memcpy(to, from, sizeof(DBT));
		}
		return (*this);
	}
	
	// this is here to allow enforcement of set_flags() for
	// DbXmlDbt and its subclasses consistently.  Direct assignment
	// of DBT.flags should never happen on these classes.
	virtual void set_flags(u_int32_t newFlags) { flags = newFlags; }
};
	
// A Dbt for passing data into a DB method.
class DbtIn : public DbXmlDbt
{
public:
	DbtIn() {
		flags = DB_DBT_USERMEM;
	}
	DbtIn(void *d, size_t sz) {
		flags = DB_DBT_USERMEM;
		set(d, sz);
	}
	DbtIn(const DbtIn &dbt)	{
		flags = DB_DBT_USERMEM;
		set(dbt.data, dbt.size);
	}
	
	void set_flags(u_int32_t newFlags) {
		DBXML_ASSERT(!(newFlags&(DB_DBT_MALLOC | DB_DBT_REALLOC | DB_DBT_USERMEM)));
		flags = (DB_DBT_USERMEM | newFlags);
	}
	void set(void *d, size_t sz) {
		data = d;
		size = (u_int32_t)sz;
		ulen = (u_int32_t)sz;
	}
	bool operator<(const DbtIn &o) const {
		return (size == o.size ? memcmp(data, o.data, size) < 0 : size < o.size);
	}
private:
	DbtIn &operator= (const DbtIn &o);
};

// A Dbt for passing into and getting data back from a DB method.
class DbtOut : public DbXmlDbt
{
public:
	typedef DbXml::SharedPtr<DbXml::DbtOut> Ptr;
	DbtOut() {
		flags = DB_DBT_REALLOC;
	}
	DbtOut(const void *d, size_t sz) {
		flags = DB_DBT_REALLOC;
		set(d, sz);
	}
	DbtOut(const DbXmlDbt &dbt) {
		flags = DB_DBT_REALLOC;
		set(dbt.data, dbt.size);
	}
	~DbtOut() {
		if (data) {
			::free(data);
		}
	}
	void set_flags(u_int32_t newFlags)
	{
		DBXML_ASSERT(!(newFlags&(DB_DBT_MALLOC | DB_DBT_REALLOC | DB_DBT_USERMEM)));
		flags = (DB_DBT_REALLOC | newFlags);
	}
	void set(const void *d, size_t sz) {
		if (sz > size) {
			data = ::realloc(data, sz);
		}
		size = (u_int32_t)sz;
		if (d != 0) {
			::memcpy(data, d, sz);
		}
	}
	void setNoCopy(void *d, size_t sz) {
		if (data) {
			::free(data);
		}
		data = d;
		size = (u_int32_t) sz;
	}
	void *adopt_data() {
		void *result = data;
		data = 0;
		size = 0;
		return result;
	}
};

}

#endif
