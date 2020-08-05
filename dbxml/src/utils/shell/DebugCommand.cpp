//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DebugCommand.hpp"

#include <xqilla/debug/InteractiveDebugger.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

string DbXml::DebugCommand::getCommandName() const
{
	return "debug";
}

string DbXml::DebugCommand::getBriefHelp() const
{
	return "Debug the given query expression, or the default pre-parsed query";
}

string DbXml::DebugCommand::getMoreHelp() const
{
	return
		string("Usage: debug [queryExpression]\n") +
		string("This command implements the XmlDebugListener API to track\n") +
		string("and intercept query execution. The user is placed into the\n") +
		string("interactive debugger mode, which accepts a different set of\n") +
		string("commands to the normal shell mode. The \"quit\" command exist\n") +
		string("the interactive debugger mode.");
}

class DbXmlInteractiveDebugger :  public XmlDebugListener,
				  private BaseInteractiveDebugger				  
{
public:
	DbXmlInteractiveDebugger()
		: stack_(0),
		  currentFrame_(0),
		  expr_(0),
		  env_(0)
	{
	}


	void debugQuery(XmlQueryExpression &expr, Environment &env)
	{
		env.context().setDebugListener(this);
		env.debugging() = true;
		expr_ = &expr;
		env_ = &env;

		try {
			readCommand();
			env.context().setDebugListener(0);
			env.debugging() = false;
		}
		catch(Quit) {
			env.context().setDebugListener(0);
			env.debugging() = false;
		}
		catch(...) {
			env.context().setDebugListener(0);
			env.debugging() = false;
			throw;
		}
	}

	static void outputLocation(const XmlStackFrame *frame, const string &query, unsigned int context = 0)
	{
		if(frame->getQueryFile() == 0 || *frame->getQueryFile() == 0) {
			BaseInteractiveDebugger::outputLocationFromString(X(query.c_str()), frame->getQueryLine(), frame->getQueryColumn(), context);
		}
		else {
			BaseInteractiveDebugger::outputLocation(X(frame->getQueryFile()), frame->getQueryLine(), frame->getQueryColumn(), context);
		}
	}

	static void outputLocation(const XmlException &ex, const string &query, unsigned int context = 0)
	{
		if(ex.getQueryFile() == 0 || *ex.getQueryFile() == 0) {
			BaseInteractiveDebugger::outputLocationFromString(X(query.c_str()), ex.getQueryLine(), ex.getQueryColumn(), context);
		}
		else {
			BaseInteractiveDebugger::outputLocation(X(ex.getQueryFile()), ex.getQueryLine(), ex.getQueryColumn(), context);
		}
	}

private:
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
	template class __declspec(dllexport) AutoReset<const XmlStackFrame *>;
#endif

	virtual void enter(const XmlStackFrame *stack)
	{
		AutoReset<const XmlStackFrame *> reset(stack_);
		AutoReset<const XmlStackFrame *> reset2(currentFrame_);
		stack_ = stack;
		currentFrame_ = stack;

		if(env_->sigBlock().isInterrupted()) {
			// Ctrl-C was pressed
			env_->sigBlock().reset();
			interrupted();
		}
		else {
			checkBreak(/*entering*/true);
		}
	}

	virtual void exit(const XmlStackFrame *stack)
	{
		AutoReset<const XmlStackFrame *> reset(stack_);
		AutoReset<const XmlStackFrame *> reset2(currentFrame_);
		stack_ = stack;
		currentFrame_ = stack;

		if(env_->sigBlock().isInterrupted()) {
			// Ctrl-C was pressed
			env_->sigBlock().reset();
			interrupted();
		}
		else {
			checkBreak(/*entering*/false);
		}
	}

	virtual void error(const XmlException &error, const XmlStackFrame *stack)
	{
		AutoReset<const XmlStackFrame *> reset(stack_);
		AutoReset<const XmlStackFrame *> reset2(currentFrame_);
		stack_ = stack;
		currentFrame_ = stack;

		breakForError(error.what());
	}

	virtual void run()
	{
		while(true) {
			next_ = 0;
			stack_ = 0;
			currentFrame_ = 0;

			try {
				queryStarted_ = true;

				u_int32_t flags = DBXML_LAZY_DOCS;
				if(env_->documentProjection())
					flags |= DBXML_DOCUMENT_PROJECTION;

				XmlTransaction txn(env_->txn() ? *env_->txn() : XmlTransaction());
				XmlResults results = expr_->execute(txn, env_->context(), flags);

				XmlValue value;
				while(results.next(value)) {
					cout << value.asString() << endl;

					if(env_->sigBlock().isInterrupted()) {
						env_->sigBlock().reset();
						break;
					}
				}
				

				queryStarted_ = false;
				cout << endl << "Query completed." << endl;
				return;
			}
			catch(Run) {
				queryStarted_ = false;
			}
			catch(Quit) {
				throw;
			}
			catch(...) {
				queryStarted_ = false;
				throw;
			}
		}
	}

	virtual bool changeFrame(unsigned int number)
	{
		const XmlStackFrame *frame = stack_;
		unsigned int count = 0;
		while(frame && count < number) {
			frame = frame->getPreviousStackFrame();
			++count;
		}

		if(frame == 0 || count < number) return false;
		currentFrame_ = frame;
		return true;
	}

	virtual unsigned int getStackSize() const
	{
		const XmlStackFrame *frame = stack_;
		unsigned int count = 0;
		while(frame) {
			frame = frame->getPreviousStackFrame();
			++count;
		}
		return count;
	}

	virtual void stackTrace() const
	{
		int count = 0;
		const XmlStackFrame *frame = stack_;
		while(frame) {
			cerr << "#" << count;
			output(frame);
			cerr << endl;

			frame = frame->getPreviousStackFrame();
			++count;
		}
	}

	virtual bool outputCurrentFrame(unsigned int context) const
	{
		if(currentFrame_ == 0) return false;

		cerr << "#" << getCurrentFrameNumber();
		output(currentFrame_);
		cerr << endl << endl;
		outputLocation(currentFrame_, expr_->getQuery(), context);

		return true;
	}

	virtual void outputCurrentFrameQueryPlan() const
	{
		if(currentFrame_ == 0) {
			cout << expr_->getQueryPlan() << endl;
		}
		else {
			cout << currentFrame_->getQueryPlan() << endl;;
		}
	}

	virtual bool queryCurrentFrame(const char *queryString) const
	{
		if(currentFrame_ == 0) return false;

		try {
			XmlResults results = currentFrame_->query(queryString);
			XmlValue value;
			while(results.next(value)) {
				cout << value.asString() << endl;

				if(env_->sigBlock().isInterrupted()) {
					env_->sigBlock().reset();
					break;
				}
			}
		}
		catch(XmlException &e) {
			cerr << e.getQueryFile() << ":" << e.getQueryLine() << ":" << e.getQueryColumn()
			     << ": error: " << e.what() << endl;
			outputLocation(e, queryString);
		}
		catch(...) {
			cerr << "Caught unknown exception" << endl;
		}

		return true;
	}

	virtual bool currentFrameLocation(string &file, unsigned int &line, unsigned int &column) const
	{
		if(currentFrame_ == 0) {
			file = "";
			line = 0;
			column = 0;
			return false;
		}

		file = currentFrame_->getQueryFile();
		line = currentFrame_->getQueryLine();
		column = currentFrame_->getQueryColumn();
		return true;
	}

	virtual void setDoLazyEvaluation(bool lazy)
	{
		env_->context().setEvaluationType(lazy ? XmlQueryContext::Lazy : XmlQueryContext::Eager);
	}

	virtual void setDoFocusOptimizationsn(bool opt)
	{
		// Not supported
	}

	virtual void setDoProjection(bool opt)
	{
		env_->documentProjection() = opt;
	}

	unsigned int getCurrentFrameNumber() const
	{
		const XmlStackFrame *frame = stack_;
		unsigned int count = 0;
		while(frame && frame != currentFrame_) {
			frame = frame->getPreviousStackFrame();
			++count;
		}
		return count;
	}

	void output(const XmlStackFrame *frame) const
	{
		string where = regexFind("(<[^>]+>)", frame->getQueryPlan());
		if(where != "") cerr << " in " << where;

		cerr << " at " << frame->getQueryFile() << ":" << frame->getQueryLine() << ":" << frame->getQueryColumn();
	}

	const XmlStackFrame *stack_;
	const XmlStackFrame *currentFrame_;

	XmlQueryExpression *expr_;
	Environment *env_;
};

string DbXml::showErrorContext(const XmlException &e, const char *query)
{
	ostringstream msg;
	msg << e.what();
	if(e.getQueryLine() != 0 && e.getQueryFile() == 0) {
		msg << endl;
		int line = 1;
		while(line != e.getQueryLine() && *query != 0) {
			if(*query == '\n' ||
				(*query == '\r' && *(query + 1) != '\n'))
				++line;
			++query;
		}
		if(line == e.getQueryLine()) {
			while(*query != 0) {
				msg << (*query);
				if(*query == '\n' ||
					(*query == '\r' && *(query + 1) != '\n'))
					break;
				++query;
			}
			if(e.getQueryColumn() != 0) {
				if(*query == 0) msg << endl;
				msg << string(e.getQueryColumn() - 1, ' ') << "^";
			}
		}
	}
	return msg.str();
}

void DbXml::DebugCommand::execute(Args &args, Environment &env)
{
	if(args.size() > 2) {
		throw CommandException("Wrong number of arguments");
	}

	string query;
	if(args.size() == 1) {
		env.testQuery();
		query = env.query()->getQuery();
	} else {
		query = args[1];
	}

	try {
		DbXmlInteractiveDebugger debugger;
		env.context().setDebugListener(&debugger);

		XmlTransaction txn(env.txn() ? *env.txn() : XmlTransaction());
		XmlQueryExpression expr = env.db().prepare(txn, query, env.context());

		debugger.debugQuery(expr, env);
	}
	catch(XmlException &e) {
		throw CommandException(showErrorContext(e, query.c_str()));
	}

	if(env.verbose())
		cout << "Finished debugging query '" << query
		     << "'" << endl << endl;
}

