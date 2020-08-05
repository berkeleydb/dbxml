//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "Counters.hpp"
#include <iostream>

using namespace DbXml;
using namespace std;

// these names must match the enum in Counters.hpp
const char *Counters::names_[NCOUNTERS] = {
	"PutDocument   ",
	"GetDocument   ",
	"UpdateDocument",
	"Prepare       ",
	"Query         ",
	"Execute       ",
	"DB get        ",
	"DB put        ",
	"DB del        ",
	"DBC get       ",
	"DBC put       ",
	"DBC del       ",
	"Doc Parse     "
};

void Counters::dumpToStream(std::ostream &out) const
{
	out << "BDB XML Counter dump:\n";
	for (int i = 0; i < NCOUNTERS; i++) {
		out << names_[i] << ": " << counters_[i] << endl;
	}
}

void Counters::dump() const
{
	dumpToStream(std::cout);
}
