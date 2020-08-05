//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "SyntaxManager.hpp"

using namespace DbXml;

static SyntaxManager *syntaxManager = 0;

// IMPORTANT:  (un)initSyntaxManager MUST be called in
// a thread-safe manner

void SyntaxManager::uninitSyntaxManager()
{
	if (syntaxManager) {
		delete syntaxManager;
		syntaxManager = 0;
	}
}

void SyntaxManager::initSyntaxManager()
{
	if (syntaxManager == 0) {
		//
		// Register Default Syntaxes
		//
		syntaxManager = new SyntaxManager();
		syntaxManager->registerSyntax(*(new NoneSyntax));
		syntaxManager->registerSyntax(*(new StringSyntax));
		syntaxManager->registerSyntax(*(new Base64BinarySyntax));
		syntaxManager->registerSyntax(*(new BooleanSyntax));
		syntaxManager->registerSyntax(*(new DateSyntax));
		syntaxManager->registerSyntax(*(new DateTimeSyntax));
		syntaxManager->registerSyntax(*(new DaySyntax));
		syntaxManager->registerSyntax(*(new DecimalSyntax));
		syntaxManager->registerSyntax(*(new DoubleSyntax));
		syntaxManager->registerSyntax(*(new DurationSyntax));
		syntaxManager->registerSyntax(*(new FloatSyntax));
		syntaxManager->registerSyntax(*(new HexBinarySyntax));
		syntaxManager->registerSyntax(*(new MonthSyntax));
		syntaxManager->registerSyntax(*(new MonthDaySyntax));
		syntaxManager->registerSyntax(*(new TimeSyntax));
		syntaxManager->registerSyntax(*(new YearSyntax));
		syntaxManager->registerSyntax(*(new YearMonthSyntax));
	}
}

SyntaxManager *SyntaxManager::getInstance()
{
	if (syntaxManager == 0) {
		initSyntaxManager();
	}
	return syntaxManager;
}

SyntaxManager::SyntaxManager()
{}

SyntaxManager::~SyntaxManager()
{
	// Syntax pointers are not deleted by
	// the sv_ vector dtor. Do that by hand.
	for (unsigned int i = 0;i < sv_.size();++i) {
		delete (Syntax *) sv_[i];
	}
}

void SyntaxManager::registerSyntax(const Syntax &syntax)
{
	if ((int)sv_.size() <= syntax.getType()) {
		sv_.resize(syntax.getType() + 1);
	}
	sv_[syntax.getType()] = &syntax;
}

const Syntax *SyntaxManager::getSyntax(const std::string &name) const
{
	for(SyntaxVector::const_iterator i = sv_.begin();
	    i != sv_.end(); ++i) {
		if(*i != 0) {
			if((*i)->hasTypeName(name.c_str())) {
				return *i;
			}
		}
	}
	return 0;
}

const Syntax *SyntaxManager::getNextSyntax(int &i) const
{
	const Syntax *syntax = 0;
	if (i > -1) {
		while (i < (int)sv_.size() && syntax == 0) {
			if (sv_[i] != 0) {
				syntax = sv_[i];
			}
			++i;
		}
		if (i == (int)sv_.size()) {
			i = -1;
		}
	}
	return syntax;
}

const Syntax *SyntaxManager::getSyntax(Syntax::Type type) const
{
	return sv_[type];
}

size_t SyntaxManager::size() const
{
	return sv_.size();
}
