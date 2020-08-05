//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __SYNTAXMANAGER_HPP
#define	__SYNTAXMANAGER_HPP

#include <vector>
#include "Syntax.hpp"

namespace DbXml
{

class SyntaxManager
{
public:
	static SyntaxManager *getInstance();

	SyntaxManager();
	~SyntaxManager();

	void registerSyntax(const Syntax &syntax);
	const Syntax *getSyntax(const std::string &name) const;
	const Syntax *getNextSyntax(int &i) const;
	const Syntax *getSyntax(Syntax::Type type) const;
	size_t size() const;

public:
	static void initSyntaxManager(void);
	static void uninitSyntaxManager(void);
private:
	typedef std::vector<const Syntax *> SyntaxVector;
	SyntaxVector sv_;
};

}

#endif
